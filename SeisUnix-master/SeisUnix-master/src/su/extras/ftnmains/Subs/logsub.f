	subroutine lsdmosub(rdata,nt,dt,nx,dx,h,dmosgn,idata,cdata,wrkarea,kx,w)
	integer nt,nx,dmosgn
	real dt,dx,h
	real rdata(nt,nx)
	real kx(nx),w(nt)
	real idata(nt,nx)
	complex wrkarea(nt,nx),cdata(nt,nx)
c
c  dmo impulse response using phase-shift method 
c  on log-stretched common offset data 
c 
	real pi
	parameter(pi=3.14159265)
c
c  rdata = real data ; idata = imaginary data
c  cdata = complex data
c  input is read in as rdata
c
	integer ikx,iw,ntnyq,nxnyq
	real dk,dw,one,zero,ftmp
	real h2,kx2,w2,fnt,fnx,arg
	complex ci,a2,phase,oper

c	dx   = 25.
c	dt   = .04
c	h = 200.0

	fnt = nt
	fnx = nx
	ntnyq=nt/2+1
	nxnyq=nx/2+1
	h2 = h*h
	one = 1.0
	zero = 0.0
	ci = csqrt(cmplx(-1.0,zero))
c
c  zero-out idata 
c
	call fzero(idata,nt*nx)
c
c  copy rdata and idata (which is zero) into cdata
c
	call ri2c(rdata,idata,cdata,nt,nx)
c
c  FT (t,x) --> (w,k)
c    ft2d is an fft routine which takes a 2-D matrix of 
c    complex data and does 2-D fft (hence the name) 
c
      	call ft2d(nt,nx,cdata,1.,-1.,sqrt(1./fnt),sqrt(1./fnx),wrkarea)
c
c Initialize kx vector
c
c					Positive wavenumbers
	dk= (2.0*pi)/(fnx*dx)
      	do 11 ikx=1,nxnyq
		ftmp = ikx - one
         	kx(ikx)= ftmp*dk
 11   	continue
c					Negative wavenumbers
      	do 12 ikx=nxnyq+1,nx
		ftmp = ikx - one - nx
       	  	kx(ikx)= ftmp*dk
 12   	continue
c
c Initialize w vector
c
c					Positive frequencies
	dw = (2.0*pi)/(fnt*dt)
      	do 13 iw=1,ntnyq
		ftmp = iw - one
         	w(iw)= ftmp*dw
 13   	continue
c					Negative frequencies
      	do 14 iw=ntnyq+1,nt
		ftmp = iw - one - nt
         	w(iw)= ftmp*dw
 14   	continue
c
c  mult. (w,k)-data by DMO operator
c  [avoid zero frequency, and neg
c   or zero argument for log function]
c
      	do 111 ikx=1,nx
      		do 113 iw=1,nt
			if (w(iw).eq.zero) then
				cdata(iw,ikx) = cmplx(zero,zero)
				goto 113
			else
			    kx2 = kx(ikx)*kx(ikx)
			    w2 = w(iw)*w(iw)
			    arg = one - h2*kx2/w2
			    if (arg.gt.zero) then
				a2 = .5*clog(cmplx(arg,zero)) 
				phase = dmosgn*ci*w(iw)*a2 
				oper = cexp(phase)
				cdata(iw,ikx) = cdata(iw,ikx)*oper
			    else
				cdata(iw,ikx) = cmplx(zero,zero)
			    endif
			endif
 113		continue
 111	continue
c
c  FT (w,k) --> (t,x)
c    ft2d is an fft routine which takes a 2-D matrix of 
c    data and does 2-D fft (hence the name) 
c
     	call ft2d(nt,nx,cdata,-1.,1.,sqrt(1./fnt),sqrt(1./fnx),wrkarea)
c
c  pull rdata and idata out of cdata
c
	call c2ri(cdata,rdata,idata,nt,nx)

	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine c2ri(c,re,im,nt,nx)
	real re(nt,nx),im(nt,nx)
	complex c(nt,nx)
	do 10 ix=1,nx
		do 20 it=1,nt
			re(it,ix) = real (c(it,ix))
			im(it,ix) = aimag(c(it,ix))
 20		continue
 10	continue
	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine ri2c(re,im,c,nt,nx)
	real re(nt,nx),im(nt,nx)
	complex c(nt,nx)
	do 10 ix=1,nx
		do 20 it=1,nt
			c(it,ix) = cmplx( re(it,ix), im(it,ix))
 20		continue
 10	continue
	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      subroutine ft2d (n1,n2,cp,sign1,sign2,scale1,scale2,cwork)
      integer n1,n2,i2
      real sign1,sign2
      real scale1,scale2
      complex cp(n1,n2),cwork(n2)
      do 23032 i2 = 1,n2
         call fft (n1,cp(1,i2),sign1)
23032 continue
      call ft1d(n1,n2,cp,sign2,scale2,cwork)

      return
      end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      subroutine ft1d(n1,n2,cp,sign2,scale2,cbf)
      integer n1,n2,i1,i2
      real sign2
      real scale2
      complex cp(n1,n2),cbf(n2)
      do 23034 i1 = 1,n1 
         do 23036 i2 = 1,n2
            cbf(i2) = cp(i1,i2)
23036    continue
         call fft(n2,cbf,sign2)
         do 23038 i2 = 1,n2
            cp(i1,i2) = cbf(i2)
23038    continue
23034 continue
      return
      end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      subroutine fft(lx,cx,signi)
      integer lx
      complex cx(lx),carg,cexp,cw,ct
      real sc, signi, sqrt
      integer i,j,m,k,istep
      j = 1
      k = 1
      sc = sqrt(1./lx)
      do 23000 i = 1,lx 
      if(.not.(i.le.j))goto 23002
      ct=cx(j)*sc
      cx(j)=cx(i)*sc
      cx(i)=ct 
23002 continue
      m = lx/2
23004 if(.not.(j.gt.m))goto 23005
      j=j-m
      m=m/2
      if(.not.(m.lt.1))goto 23006
      goto 23005
23006 continue
      goto 23004
23005 continue
      j = j+m
23000 continue
23008 continue
      istep = 2*k
      do 23011 m = 1,k 
      carg = (0.,1.)*(3.14159265*signi*(m-1))/k
      cw = cexp(carg)
      do 23013 i = m,lx,istep
      ct=cw*cx(i+k)
      cx(i+k)=cx(i)-ct
      cx(i)=cx(i)+ct 
23013 continue
23011 continue
      k = istep
23009 if(.not.(k.ge.lx))goto 23008
      return
      end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine fzero(x,n)
	real x(n)
	do 10 i=1,n
 10	x(i) = 0.0
	return
	end
