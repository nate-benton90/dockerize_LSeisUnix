ccccc  tau-p transform and interpolation in frequency domain
cccc   with least-squre inverse t-p filter applied to reduce
c      artifacts due to irregular, limited and coarsely sampled
c      input data
c         
cc 
c
c   auhtor: Zhiming Li              6/7/91
c
c
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c
ccccc initpf computes lengths of some work buffers for tau-p inverse filtering 
c
c   auhtor: Zhiming Li              6/7/91
c
c       input: 
c             ntfft  --  nt padded with zero's to be fft length
c             dt     --  time sampling interval
c             fmin   --  minimum frequency to process
c             fmax   --  maximum frequency to process
c             nxi    --  number of input trace positions
c        output:
c             ifmin  --  minimum frequency index (fmin/df+1)
c             df     --  frequency sampling interval 
c             nf     --  number of frequencies to process
c             lftwk  --  length of fft buffer needed
c             np     --  number of p values needed
c			 (if np=0 in input, it will be computed)
c             nph    --  (np-1)/2
cccccccccc 
	subroutine initpf(ntfft,dt,fmin,fmax,nxi,
     *			  ifmin,df,nf,lftwk,np,nph)
	integer nxi,nph,nf,lftwk,ntfft,np
	integer ifmin
	real df
	df = 1./(ntfft*dt) 
	nfmax = ntfft/2+1
	tmp = fmin/df + 1.
	ifmin = tmp
	ifmin =max0(2,ifmin)
	ifmin = min0(ifmin,nfmax)
	tmp = fmax/df + 1.
	ifmax = tmp
	ifmax = max0(ifmax,2)
	ifmax = min0(ifmax,nfmax)
	nf = ifmax - ifmin + 1
        lftwk = 2*ntfft + 15 
	if (np .eq. 0) np = nxi
c  make np odd number
	np = np/2*2+1
	nph = (np-1)/2
	return
	end 
ccccc
cccc filter compute the inverse filter (frequency independent)
c 
c  author:    z. li   	        6/7/91
c  
c  input:
c    xi(nxi)              ---     real*4     input trace positions
c					     (must be monotonically increasing)
c    nxi                  ---     integer    number of input
c    nph                  ---     integer    (number of p's - 1)/2
c    b(-nph:nph)          ---     complex    work array
c    r(-nph:nph,-nph:nph) ---     complex    work array
c    wk1(-nph:nph)        ---     complex    work array
c    wk2(-nph:nph)        ---     complex    work array
c    wk3(-nph:nph)        ---     complex    work array
c    wk4(-nph:nph)        ---     complex    work array
c    wk5(-nph:nph)        ---     complex    work array
c    sig(-nph:nph)        ---     complex    sigma filter computed 
c    tol		  ---     real*4     tolerance of accuracy
c    niter 		  ---     integer    maximum number of iterations 
c					     allowed in inverse filter solver
c					     (conjugate gradiant method)
c  output:
c    dxi(nxi)             ---     real*4     input trace spacings 
c    dxmin	          ---     real*4     minimum trace spacing 
c    d(-nph:nph)          ---     complex    inverse filter computed 
c
c
	subroutine filter(xi,nxi,nph,dxi,dxmin, 
     *          	  d,sig,r,b,wk1,wk2,wk3,wk4,wk5,tol,niter)
	real xi(nxi),dxi(nxi),dxmin
	complex sig(-nph:nph),d(-nph:nph)
	complex r(-nph:nph,-nph:nph),b(-nph:nph)
	complex wk1(-nph:nph), wk2(-nph:nph), wk3(-nph:nph)
	complex wk4(-nph:nph), wk5(-nph:nph)
	integer nxi,nph,niter
	real tol
c
cccccccccccc compute sampling interval along x axis
c
	do ix=2,nxi-1
	   dxi(ix) = ( xi(ix+1) - xi(ix-1) ) /2
	end do 
	dxi(1) = xi(2) - xi(1)
	dxi(nxi) = xi(nxi)-xi(nxi-1)
cccc find minimum x interval 
	dxx = dxi(1)
	do ix=2,nxi
	   if ( dxx .gt. dxi(ix) ) dxx = dxi(ix) 
	end do
	dxmin = dxx
cccc  determin the maximum p value
c    we use a reference frequency of 20 Hz for computing dp, since
c    dp=0.5/(dxx*f*nph) and the term used in inverse filter design 
c    w*dp=2*pi*f*dp=pi/(dxx*nph) is independent of f.
cccc
	fref = 20.
	pmax = 0.5/(dxx*fref)
	dpp = pmax/nph
	w = 2.*3.141592654*fref
	p = - pmax
cccc computer the filter
        do ip=-nph,nph
	   call sigma(xi,dxi,nxi,w,p,sig(ip))
	   p = p + dpp
	end do
cccc compute the inverse filter
c
cccc set up matrix
	do j=-nph,nph
	   do i=-nph,nph
	      if( (i-j).ge.-nph .and. (i-j).le.nph ) then
		 r(i,j) = sig(i-j)
	      else
		 r(i,j) = (0.,0.) 
	      end if
	   end do
	end do
cccc compute right-hand side 
	do ip=-nph,nph
	   b(ip) = (0.,0.)
	end do
	b(0) = (1.,0)
cccc now use conjugate gradient method to solve the r d = b
c to find the inverse filter
c
c initialize d = 0
	do ip=-nph,nph
	   d(ip) = (0.,0.)
	end do
	d(0) = (1.,0.)
	m = nph*2+1
	call ccgg(r,d,b,wk1,m,m,wk2,wk3,wk4,wk5,tol,niter,kiter)
	return
	end 
cccccccc
cccc   fwdtp does forward tau-p transform 
ccc  input:
c      fyi(nf,nxi)    ---    complex     input traces fft'ed
c      wp(-nph:nph,nf)---    complex     2 * pi * f * p 
c					 (to be computed if iniwp=0)
c      dp(nf)         ---    real*4	 p sampling interval at f
c					 (to be computed if iniwp=0)
c      xi(nxi)        ---    real*4      input trace positions
c      dxi(nxi)       ---    real*4      input trace spacings
c      dxmin          ---    real*4      minimum trace spacing
c      nph            ---    integer     (np-1)/2 
c      					 (where np is number of p values used)
c      nf             ---    integer     number of frequencies used
c      ifmin          ---    integer     minimum frequency index (fmin/df+1)
c      df 	      ---    real*4      frequency sampling interval 
c      nxi            ---    integer     number of input traces
c      iniwp          ---    integer     initialization of wp & dp computations
c				 	 (=0 need compute; 1= use only)
c      icexp          ---    integer     initialization of ccexp
c                                        (=-1 will compute internally; 
c					      no need for ccexp;
c                                         =0 will compute ccexp again;
c                                         =1 will not recompute ccexp;
c   ouput:
c      si(-nph:nph,nf)---    complex     forwad taup-p transformed data in
c					 frequency-p domain
c      wp(-nph:nph,nf)---    real*4      2 * pi * f * p 
c					 (to be computed if iniwp=0)
c      dp(nf)         ---    real*4      p sampling interval at f
c					 (to be computed if iniwp=0)
c  ccexp(-nph:nph,nf,nxi) --- complex    computed complex coefficients
c                                        dependent of xi and dxi 
c
c      
	subroutine fwdtp(fyi,si,wp,dp,nf,ifmin,df,
     *			 nph,xi,dxi,nxi,dxmin,iniwp,ccexp,icexp)
	complex fyi(nf,nxi),si(-nph:nph,nf)
	complex ccexp(-nph:nph,nf,nxi)
	real wp(-nph:nph,nf)
	integer nxi,nf,ifmin,iniwp,nph,icexp
	real dp(nf),dxi(nxi),dxmin,xi(nxi)
	parameter (pi2=3.141592654*2.)
	complex ctemp,cf
	real arg
c
cccc compute wp(p,f);  p sampling is frequency dependent
c    
c
	if ( iniwp .eq. 0 ) then
	   do iff=1,nf 
	      f = (iff+ifmin-2)*df
	      w = pi2 * f
	      pmax = 0.5/(dxmin*f)
	      dp(iff) = pmax/nph
 	      do ip=-nph,nph 
	         wp(ip,iff) = ip * dp(iff) * w
	      end do
	   end do
	end if 
	call zeroal(si,(2*nph+1)*nf,8)
	if (icexp .eq. -1 ) then
		do ix=1,nxi
	   	xxi = xi(ix)
	   	dxxi = dxi(ix)
		do iff=1,nf
	      	cf = fyi(iff,ix)*dxxi
		do ip=-nph,nph
			arg = wp(ip,iff)*xxi
		 	ctemp=cf*cmplx(cos(arg),sin(arg))
			si(ip,iff) = si(ip,iff)+ctemp
		end do
		end do
		end do
	else 
	    if ( icexp .eq. 0 ) then
		do ix=1,nxi
	   	xxi = xi(ix)
	   	dxxi = dxi(ix)
		do iff=1,nf
		do ip=-nph,nph
			arg = wp(ip,iff)*xxi
		 	ccexp(ip,iff,ix)=dxxi*cmplx(cos(arg),sin(arg))
		end do
		end do
		end do
	    end if
	    do ix=1,nxi
	    do iff=1,nf 
	      cf = fyi(iff,ix)
 	      do ip=-nph,nph
		 si(ip,iff) = si(ip,iff)+ccexp(ip,iff,ix)*cf
	      end do
	    end do
	    end do
	end if
	return
	end
c
cccc   filtp applies inverse filter to tau-p transformed data to recover
c      correct tau-p spectrum
c
c  author:    zhiming li	           6-7-91
c
c   input:
c      si(-nph:nph,nf)---    complex     p-f spectrum from fwdtp
c      d(-nph:nph)    ---    complex     inverse filter
c      nf             ---    integer     number of frequencies used
c      nph            ---    integer     (np-1)/2 
c      					 (where np is number of p values used)
c   output:
c      so(-nph:nph,nf)---    complex     p-f spectrum after inverse filtering
c      
c   
	subroutine filtp(si,so,d,nf,nph)
	complex si(-nph:nph,nf),so(-nph:nph,nf)
	complex d(-nph:nph) 
	integer nph,nf
c   and applies inverse filter
c1111 via convolution
  	call vcconv(si,d,so,nph,nph,nph,nph,nf)
c2222 via fft
c	nr = nph+nph+1
c	km = nr - nph
c	kn = nr + nph
c 	call vccvft(si(-nph,1),d(-nph),so(-nph,1),nr,km,kn,nf)
	return
	end
ccc inverse tau-p transform to do interpolation
c
c  author:    zhiming li	           6-7-91
c   input:
c      so(-nph:nph,nf)---    complex     p-f spectrum from filtp
c      wp(-nph:nph,nf)---    real*4      2 * pi * f * p 
c      dp(nf)         ---    real*4      p sampling interval at f
c      xo(nxo)        ---    real*4      output trace positions
c      swk(nf)        ---    complex     work buffer
c      wk(ntfft)      ---    real*4      work buffer
c      ftwk(lftwk)    ---    real*4      trace fft coefficients precomputed 
c      nf             ---    integer     number of frequencies used
c      nt             ---    integer     number of time samples per trace
c      ntfft          ---    integer     trace fft length 
c      nxo            ---    integer     number of output trace positions  
c      nph            ---    integer     (np-1)/2 
c      					 (where np is number of p values used)
c      ifmin          ---    integer     minimum frequency index (fmin/df+1)
c      lftwk          ---    integer     length of trace fft coefficients 
c					 (ntfft*2+15)
c      icexpo         ---    integer     precomputed ccexpo
c                                        =-1   no; will compute internally
c                      			 =0    no; will compute and store
c                                        =1    yes; will apply
c      
c  output:
c      yo(nt,nxo)     ---    real*4      output data traces	
c ccexpo(-nph:nph,nf,nxo) --- complex*8  computed coefficients 
c
        subroutine invtp(so,yo,dp,swk,wp,xo,wk,ftwk,
     *			 nf,nt,ntfft,nxo,nph,ifmin,lftwk,
     *                   ccexpo,icexpo)
	complex swk(nf), so(-nph:nph,nf)
	complex ccexpo(-nph:nph,nf,nxo)
	real yo(nt,nxo),xo(nxo)
	real dp(nf)
	real wp(-nph:nph,nf)
	real ftwk(lftwk),wk(ntfft)
	integer nt,nxo,nph,nf,lftwk,ntfft,ifmin
	real arg
	integer icexpo
cccc
	scale = 1./ntfft
	ifmax = ifmin + nf - 1
	nfmax = ntfft/2+1
	call zeroal(yo,nt*nxo,4)
	if(icexpo .eq. 0) then
		do ix=1,nxo
	   	xxo = xo(ix)
		do iff=1,nf
		do ip=-nph,nph
			arg = wp(ip,iff) * xxo
		 	ccexpo(ip,iff,ix)=cmplx(cos(arg),-sin(arg))
		end do
		end do
		end do
	end if
	do ix=1,nxo
	   xxo = xo(ix)
	   call zeroal(wk,ntfft,4)
 	   call zeroal(swk,nf,8)
	   if ( icexpo .eq. -1 ) then
	   	do iff=1,nf 
 	   	do ip=-nph,nph
		 arg = wp(ip,iff) * xxo
		 swk(iff)=swk(iff)+so(ip,iff)*cmplx(cos(arg),-sin(arg))
	        end do
	   	end do
	   else 
	   	do iff=1,nf 
 	   	do ip=-nph,nph
		 swk(iff)=swk(iff)+so(ip,iff)*ccexpo(ip,iff,ix)
	        end do
	   	end do
	   end if
	   if ( ifmin .eq. 1 ) then
	      wk(1) = real(swk(1))
	   else
	      ir=2*ifmin-2
	      ii=2*ifmin-1
	      wk(ir) = real(swk(1))
	      wk(ii) = aimag(swk(1))
	   end if
	   if ( ifmax .eq. nfmax ) then
	      wk(ntfft) = real(swk(nf))
	   else
	      ir=2*ifmax-2
	      ii=2*ifmax-1
	      wk(ir) = real(swk(nf))
	      wk(ii) = aimag(swk(nf))
	   end if
           do iff=2,nf-1
	      k = iff + ifmin  - 1
	      ir=2*k-2
	      ii=2*k-1
	      wk(ir) = real(swk(iff))
	      wk(ii) = aimag(swk(iff))
	   end do
	   call rfftb(ntfft,wk,ftwk)
	   do it=1,nt
	      yo(it,ix) = wk(it)*scale
	   end do
	end do
	return
	end
c
cccccccc subroutine vcconv computes the convolution of  
c	 two given complex functions
c
c			   n
c          vcon(k,ic) = SUM d(i) s(k-i,ic)
c		          i=-m
c	         (-km <= k <= kn; 1<= ic <nc ) 
c	         (where km <=2m; kn <= 2n) 
c
c  author:    zhiming li	           6-7-91
c
	subroutine vcconv(s,d,vcon,m,n,km,kn,nc)
	integer m,n,km,kn,nc
	complex s(-m:n,nc), d(-m:n)
	complex vcon(-km:kn,nc),ctemp
	do k=-km,kn
	   il = - n + k
	   il = max0(il,-m)
	   ih = m + k
	   ih = min0(ih,n)
	   do ic=1,nc
	      ctemp = cmplx(0.,0.)
	      do i=il,ih
	         ctemp = ctemp+d(i)*s(k-i,ic)
	      end do
	      vcon(k,ic) = ctemp
	   end do
	end do
	return
	end
cccc
cccc subroutine ccgg(a,x,y,res,m,n,g,s,gg,ss,tol,niter,kiter)
cccc
cccc   subroutine ccgg() minimize squared error of the complex system:
cccc       a(m,n)*x(n) = 0  
cccc
cccc   Author: Z. Li 
cccc
cccc    input:
cccc       a(m,n)     ---       m by n matrix 
cccc         x(n)     ---       initial guess of unknowns  
cccc         y(m)     ---       right hand side vector
cccc            m     ---       number of equations
cccc            n     ---       number of unknowns  
cccc          tol     ---       residul tolerance
cccc        niter     ---       maximum number of iterations allowed
cccc         g(n)     ---       working buffer
cccc         s(n)     ---       working buffer
cccc        gg(m)     ---       working buffer
cccc        ss(m)     ---       working buffer
cccc
cccc     output:
cccc         x(n)     ---       least-squared-error solution to: 
cccc					a(m,n)*x(n) = y(m)
cccc       res(m)     ---       residuals: res(m)=y(m)-a(m,n)*x(n) 
cccc        kiter     ---       actual number of iterations used
cccc                             
	subroutine ccgg(a,x,y,res,m,n,g,s,gg,ss,tol,niter,kiter)
	integer m,n,iter,niter,kiter
	complex x(n),y(m),res(m),a(m,n)
	complex g(n),s(n),gg(m),ss(m)
	real tol
	complex cdot,csds,cgdg,cgds,csdg,cgdr,csdr,crdg,crds
	real sds,gdg,gds,gdr,sdr
	complex ctmp
	real determ,alfa,beta
ccccc
c       do i=1,n
c	   x(i) = (0.,0.)
ccccc	end do
	do i=1,m
	   ctmp = (0.,0.)
	   do j=1,n
	      ctmp = ctmp + x(j)*a(i,j)
	   end do
	   res(i) = y(i) - ctmp
	end do
	do iter=0,niter
	   kiter = iter
	   do j=1,n
	      ctmp = (0.,0.)
	      do i=1,m
		 ctmp = ctmp + conjg(a(i,j))*res(i)
	      end do
	      g(j) = ctmp
	   end do
	   do i=1,m
	      ctmp = (0.,0.) 
	      do j=1,n
		 ctmp = ctmp + a(i,j)*g(j)
	      end do
	      gg(i) = ctmp
	   end do
	   if (iter .eq. 0 ) then 
	      ctmp = cdot(m,res,gg)/(cdot(m,gg,gg)+1.e-30)
	      alfa = real(ctmp)
	      beta = (0.,0.)
	   else
	      cgdg = cdot(m,gg,gg)
	      csds = cdot(m,ss,ss)
	      cgds = cdot(m,gg,ss)
	      csdg = cdot(m,ss,gg)
	      gdg = real(cgdg)*2.
	      sds = real(csds)*2.
	      gds = real(cgds) + real(csdg)
	      determ = gdg*sds - gds*gds + 1.e-30
	      cgdr = cdot(m,gg,res)
	      crdg = cdot(m,res,gg)
	      csdr = cdot(m,ss,res)
	      crds = cdot(m,res,ss)
	      gdr = real(cgdr) + real(crdg)
	      sdr = real(csdr) + real(crds)
	      alfa = (sds*gdr-gds*sdr)/determ
	      beta = (-gds*gdr+gdg*sdr)/determ
	   end if
	   do i=1,n
	      s(i) = alfa*g(i) + beta*s(i)
	      x(i) = x(i) + s(i)
	   end do
	   do i=1,m
	      ss(i) = alfa*gg(i) + beta*ss(i)
	      res(i) = res(i) - ss(i)
	   end do
	   do i=1,m
	      if ( cabs(res(i)) .gt. tol ) goto 100  
	   end do
	   return   
  100	   continue	
	end do
	return
	end
ccccc   complex dot product: cdot = conjg(x(n))*y(n)  
	complex function cdot(n,x,y)
	complex x(n),y(n)
	integer n
	complex ctmp
	ctmp = (0.,0.)
	do i=1,n
	   ctmp = ctmp + conjg(x(i))*y(i)
	end do
	cdot = ctmp
	return
	end
c
cccccccc subroutine sigma computes the sigma filter for 
c	 given x(nx), dx(nx), frequency w, and slope p
c
c 			  nx
c 	      sigma(p) = SUM  exp(i*w*x(ix)*p) * dx(ix)
c		  	 ix=1
c
	subroutine sigma(x,dx,nx,w,p,sig)
	real x(nx),dx(nx)
	complex sig
	integer nx
	real w, p
	real sr, si
	sr = 0.
	si = 0.
	do ix=1,nx
	   arg = w*x(ix)*p
	   sr = sr+cos(arg)*dx(ix)
	   si = si+sin(arg)*dx(ix)
	end do
	sig = cmplx(sr,si)
	return
	end 
c
cccccccc function cauto computes the autocorrelation of  
c	 given complex function
c
c			   n
c              cauto(k) = SUM s(i) conjg(s(i+k))
c			  i=-m
c	         (-n-m <= k <= n+m ) 
c
	complex function cauto(s,m,n,k)
	integer m,n,k
	complex s(-m:n), ctemp
	ctemp = cmplx(0.,0.)
	il = - m - k
	il = max0(il,-m)
	ih = n - k
	ih = min0(ih,n)
	do i=il,ih
	   ctemp = ctemp + s(i)*conjg(s(i+k))
	end do
	cauto = ctemp
	return
	end
c
c
cccccccc function ccross computes the cross correlation of  
c	 two given complex functions
c
c			   n
c             ccross(k) = SUM d(i) conjg(s(i+k))
c			  i=-m
c	         (-n-m <= k <= n+m ) 
c
	complex function ccross(d,s,m,n,k)
	integer m,n,k
	complex s(-m:n), d(-m:n), ctemp
	ctemp = cmplx(0.,0.)
	il = - m - k
	il = max0(il,-m)
	ih = n - k
	ih = min0(ih,n)
	do i=il,ih
	   ctemp = ctemp + d(i)*conjg(s(i+k))
	end do
	ccross = ctemp
	return
	end
c
c
cccccccc function cconvl computes the convolution of  
c	 two given complex functions
c
c			   n
c             cconvl(k) = SUM d(i) s(k-i)
c			  i=-m
c	         (-2m <= k <= 2n ) 
c
	complex function cconvl(d,s,m,n,k)
	integer m,n,k
	complex s(-m:n), d(-m:n), ctemp
	ctemp = cmplx(0.,0.)
	il = - n + k
	il = max0(il,-m)
	ih = m + k
	ih = min0(ih,n)
	do i=il,ih
	   ctemp = ctemp + d(i)*s(k-i)
	end do
	cconvl = ctemp
	return
	end
ccccccccccccccccccccccccccccc
c
cccc trafft computes input trace fft
c
c   input:
c         yi(nt,nxi)      ---     real*4    input 2-D data  
c         wk(ntfft)       ---     real*4    work array 
c         ftwk(lftwk)     ---     real*4    fft coefficients computed 
c					    (when inifft = 1 ) 
c         nt              ---     integer   number of time samples per trace
c         ntfft           ---     integer   trace fft length
c         nxi             ---     integer   number of input traces
c         ifmin           ---     integer   minimum frequency index
c					    (fmin/df+1)
c         nf              ---     integer   number of frequencies to process
c         lftwk           ---     integer   length of array ftwk (2*ntfft+15)
c         inifft          ---     integer   initialization of fft
c					    (=0 need compute ftwk; 1= use ftwk)
c				
c  output:
c         fyi(nf,nxi)     ---     complex   fft'ed traces
c 
	subroutine trafft(yi,fyi,wk,ftwk,nt,ntfft,nxi,
     *		  	  ifmin,nf,lftwk,inifft)
	real yi(nt,nxi),wk(ntfft),ftwk(lftwk)
	complex fyi(nf,nxi)
	integer nxi,nt,ntfft,ifmin,nf,lftwk,inifft
	if (inifft .eq. 0 ) then
	   call rffti(ntfft,ftwk)
	end if
	ifmax = ifmin + nf - 1
	nfmax = ntfft/2 + 1
	do ix=1,nxi
	   call zeroal(wk,ntfft,4)
	   do it=1,nt
	      wk(it) = yi(it,ix)
	   end do   	 
	   call rfftf(ntfft,wk,ftwk)
	   if ( ifmin .eq. 1 ) then
	      fyi(1,ix) = cmplx(wk(1),0.)
	   else
	      fyi(1,ix) = cmplx(wk(2*ifmin-2),wk(2*ifmin-1))
	   end if
	   if ( ifmax .eq. nfmax ) then
	      fyi(nf,ix) = cmplx(wk(ntfft),0.)
	   else
	      fyi(nf,ix) = cmplx(wk(2*ifmax-2),wk(2*ifmax-1))
	   end if
	   do iff=2,nf-1
	      k = ifmin + iff - 1 
	      fyi(iff,ix) = cmplx(wk(2*k-2),wk(2*k-1))
	   end do
	end do
	return
	end
