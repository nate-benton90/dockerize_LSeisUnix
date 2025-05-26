ccccccccccccccccccccccccccccccccccc
c
c   vector code of omega-x migration subroutine                 
c                                                                       
c   	vectorization is performed along                               
c             	line direction (3-d) --- vmode=line (migrating n1 lines) 
c         or   	cdp direction (3-d)  --- vmode=xline (migrating n1 cdps) 
c         or 	frequency axis (2-d) --- vmode=freq (migrating n1 frequecies)
c
cccccccccccccccccccccccccccccccccc 
c                                                                       
c                                                                       
c     	subroutine vwmig(n1,n2,om,v,dt,dstep,d2,para,trick,alpha,beta,
c     1  		 timig,wave,a,aa,r,a1,ar,asave,aasave,isave)
c	integer n1,n2
c	real om(n1),v(n1,n2),dt,dstep,d2,para,trick,alpha,beta
c       logical timig
c	real wave(n1,n2,2)
c	real a(n1,n2,2),aa(n1,n2,2),r(n1,n2,2),al(n1,2),ar(n1,2)
c	real asave(n2,n1,2),aasave(n2,n1,2)
c       integer isave
cccccccccccccc
c    input:                                                             
c        n1 ---  number of parallel migrations:             
c              	 number of lines, when vmode=line                       
c              	 number of cdps, when vmode=xline                       
c              	 number of frequencies, when vmode=freq                 
c        n2 ---  number of cdp's (or lines) to migrate 
c    om(n1) ---  circular frequency(s) to migrate:               
c              	 single frequency: om(1)=om(2)=...=om(n1); vmode=line (xline)  
c                different frequencies: om(i),i=1,n1; vmode=freq             
c  v(n1,n2) ---  velocities:
c                v(i1,i2)=v(line_i1,cdp_i2), when vmode=line          
c                v(i1,i2)=v(cdp_i1,line_i2), when vmode=xline          
c                v(i1,i2)=v(cdp_i2)   when vmode=freq 
c        dt ---  time sampling interval in seconds 
c     dstep ---  downward extrapolation interval 
c                in meters or feet, if timig=.false.
c                in seconds, if timig=.true.
c        d2 ---  migration trace spacing
c                cdp spacing, when vmode=line  
c                line spacing, when vmode=xline  
c                cdp spacing, when vmode=freq  
c      para ---  dipfiltering constant (between 0. and 1):
c                (0.=no dipfiltering to suppress evanacent noise) 
c     trick ---  1/6 trick value to approximate 2nd derivative 
c     alpha ---  coefficient computed from subroutine alpbe  
c      beta ---  coefficient computed from subroutine alpbe  
c     timig ---  time migration or depth migration mode
c                .true.   time migration
c                .false.  depth migration
c wave(n1,n2,2)--- wave fields  (1 real part, 2 imaginary part )
c                  wave(line,cdp)   when vmode=line 
c                  wave(cdp,line)   when vmode=xline 
c                  wave(frequency,cdp)   when vmode=freq 
c  a(n1,n2,2) ---  working array  (when isave=-1 a is precomputed from the
c                                  previous call, i.e., it is the
c                                  inline migration's asave array) 
c  aa(n1,n2,2) ---  working array  (when isave=-1 aa is precomputed from the
c                                  previous call, i.e., it is the
c                                  inline migration's aasave array) 
c  r(n1,n2,2) ---  working array                                  
c    al(n1,2) ---  working array                                  
c    ar(n1,2) ---  working array                                  
c  asave(n2,n1,2) ---  working array (when isave=1 asave is used to store
c                                     a transpose for the crossline migration)
c  aasave(n2,n1,2) ---  working array (when isave=1 aasave is used to store
c                                     aa transpose for the crossline migration)
c       isave ---  save a and aa array transpose for cross-line migration  
c                  0=no  (2d migration or when asave and aasave arrays are
c                        not allocated)
c                  1=yes (inline migration 3D, asave and aasave allocated)
c                 -1=no  a and aa arrays are precomputed 
c                        (crossline migration 3D, a and aa precomputed)
c   output:                                                             
c wave(n1,n2,2) --- wave fields extrapolated (1=real 2=imaginary)
c om(n1)        ---  -1./om(i),i=1,2,...n1
c  asave(n2,n1,2) ---  working array (when isave=1 asave is used to store
c                                     a transpose for the crossline migration)
c  aasave(n2,n1,2) ---  working array (when isave=1 aasave is used to store
c                                     aa transpose for the crossline migration)
c                                                                       
c   author:   zhiming li                             5/92             
c                                                                       
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc   
c
     	subroutine vwmig(n1,n2,om,v,dt,dstep,d2,para,trick,alpha,beta,
     1  		 timig,wave,a,aa,r,al,ar,asave,aasave,isave)
ccc input/output arrays and variables 
	integer n1,n2
	real om(n1),v(n1,n2),dt,dstep,d2,para,trick,alpha,beta
	logical timig
	real wave(n1,n2,2)
	real a(n1,n2,2),aa(n1,n2,2),r(n1,n2,2),al(n1,2),ar(n1,2)
	real asave(n2,n1,2),aasave(n2,n1,2)
	integer isave
ccc auxiliary variables
	real ddi,ccci,ccc1i,cai,b1,b2
	real ccc2r,cbr,a1r,a2r
	real ccc2i,cbi,a1i,a2i
	real cp2r,cp3r,cpnm1r,cpnm2r
	real cp2i,cp3i,cpnm1i,cpnm2i
	real bb,pi,aaa,rw,rw2
	real tmpr,tmpi,tmplr,tmpli,tmprr,tmpri,tlr,tli,trr,tri
ccc  initialization 
	pi = 3.141592654
      	aaa = (8.*para*dt)/pi
      	ccci = aaa/(2.*d2*d2)  
	bb = beta/(d2*d2)
	ddi = dstep*alpha/(2.*d2*d2)
cccc compute tridiagonal matrix coefficients 
	do i1=1,n1
		if(om(i1).ne.0.) then
			om(i1) = -1./om(i1)
		else 
			om(i1) = -1./0.001
		end if
	end do
	if(isave.ne.-1) then
		do i2=1,n2
			do i1=1,n1
            			rw = om(i1)
            			rw2 = rw * rw
            			ccc1i = rw*ddi
				if(timig) ccc1i = ccc1i * v(i1,i2)
            			ccc2r = rw2*bb
            			ccc2i = rw*ccci
               			cai = v(i1,i2)*ccc1i
               			cbr = v(i1,i2)*v(i1,i2)*ccc2r + trick
               			cbi = v(i1,i2)*v(i1,i2)*ccc2i
               			a(i1,i2,1) = cbr 
               			a(i1,i2,2) = cbi - cai
               			aa(i1,i2,1) = cbr  
               			aa(i1,i2,2) = cbi + cai  
			end do
		end do 
	end if
	if(isave.eq.1) then
		do i2=1,n2
			do i1=1,n1
				asave(i2,i1,1) = a(i1,i2,1)
				asave(i2,i1,2) = a(i1,i2,2)
				aasave(i2,i1,1) = aa(i1,i2,1)
				aasave(i2,i1,2) = aa(i1,i2,2)
			end do
		end do
	end if
cccc absorbing boundary conditions
	do i1=1,n1
            	cp2r = wave(i1,2,1)    
            	cp2i = wave(i1,2,2)    
            	cp3r = wave(i1,3,1)     
            	cp3i = wave(i1,3,2)     
            	cpnm1r = wave(i1,n2-1,1)  
            	cpnm1i = wave(i1,n2-1,2)  
            	cpnm2r = wave(i1,n2-2,1)   
            	cpnm2i = wave(i1,n2-2,2)   

            	a1r = 2.*(cp2r*cp3r+cp2i*cp3i)
            	a1i = 2.*(-cp2r*cp3i+cp2i*cp3r)
            	b1 = cp2r*cp2r+cp2i*cp2i+cp3r*cp3r+cp3i*cp3i 
            	if(b1.eq.0.) then
               		a1r = 1.
			ali = 0.
            	else
               		a1r = a1r / b1 
               		a1i = a1i / b1 
            	end if
       		if(a1i.lt.0.) a1i = -a1i
		al(i1,1) = a1r
		al(i1,2) = a1i

            	a2r = 2.*(cpnm1r*cpnm2r+cpnm1i*cpnm2i)
            	a2i = 2.*(-cpnm1r*cpnm2i+cpnm1i*cpnm2r)
            	b2 = cpnm1r*cpnm1r+cpnm1i*cpnm1i
     1 			+cpnm2r*cpnm2r+cpnm2i*cpnm2i 
            	if(b2.eq.0.) then
               		a2r = 1.
               		a2i = 0.
            	else
               		a2r = a2r / b2 
               		a2i = a2i / b2 
            	end if
       		if(a2i.lt.0.) a2i = -a2i
		ar(i1,1) = a2r
		ar(i1,2) = a2i


ccccccccccccccccc zero value bc 
c		al(i1,1) = (0.,0.)
c		al(i1,2) = (0.,0.)
c		ar(i1,1) = (0.,0.)
c		ar(i1,2) = (0.,0.)
cccccccccccccccc
	end do
cccc compute the right-hand side
	do i2=2,n2-1 
		do i1=1,n1
			tmpr = 1.-2*a(i1,i2,1)
			tmpi = -2*a(i1,i2,2)
			r(i1,i2,1) = a(i1,i2+1,1)*wave(i1,i2+1,1)
     1			           - a(i1,i2+1,2)*wave(i1,i2+1,2)
     2                         	   + tmpr*wave(i1,i2,1)
     3                         	   - tmpi*wave(i1,i2,2)
     4                             + a(i1,i2-1,1)*wave(i1,i2-1,1)
     5                             - a(i1,i2-1,2)*wave(i1,i2-1,2)
			r(i1,i2,2) = a(i1,i2+1,1)*wave(i1,i2+1,2)
     1			           + a(i1,i2+1,2)*wave(i1,i2+1,1)
     2                         	   + tmpr*wave(i1,i2,2)
     3                         	   + tmpi*wave(i1,i2,1)
     4                             + a(i1,i2-1,1)*wave(i1,i2-1,2)
     5                             + a(i1,i2-1,2)*wave(i1,i2-1,1)
		end do
	end do
	do i1=1,n1
		tlr = al(i1,1)-2.
		tli = al(i1,2)
		tmplr = 1. + tlr*a(i1,1,1) 
     1		           - tli*a(i1,1,2) 
		tmpli = tlr*a(i1,1,2) 
     1                + tli*a(i1,1,1)
            	r(i1,1,1) = a(i1,2,1)*wave(i1,2,1)
     1       	          - a(i1,2,2)*wave(i1,2,2)
     2                    + tmplr*wave(i1,1,1)
     3                    - tmpli*wave(i1,1,2)
            	r(i1,1,2) = a(i1,2,1)*wave(i1,2,2)
     1       	          + a(i1,2,2)*wave(i1,2,1)
     2                    + tmplr*wave(i1,1,2)
     3                    + tmpli*wave(i1,1,1)
		trr = ar(i1,1)-2.
		tri = ar(i1,2)
		tmprr = 1. + trr*a(i1,n2,1) 
     1                     - tri*a(i1,n2,2) 
		tmpri = trr*a(i1,n2,2) 
     1                + tri*a(i1,n2,1)
            	r(i1,n2,1) = a(i1,n2-1,1)*wave(i1,n2-1,1)
     1       	           - a(i1,n2-1,2)*wave(i1,n2-1,2)
     2                     + tmprr*wave(i1,n2,1)
     3                     - tmpri*wave(i1,n2,2)
            	r(i1,n2,2) = a(i1,n2-1,1)*wave(i1,n2-1,2)
     1       	           + a(i1,n2-1,2)*wave(i1,n2-1,1)
     2                     + tmprr*wave(i1,n2,2)
     3                     + tmpri*wave(i1,n2,1)
	end do
	do i2=1,n2
		do i1=1,n1
			wave(i1,i2,1) = r(i1,i2,1)
			wave(i1,i2,2) = r(i1,i2,2)
		end do
	end do
c compute the tridiagonal coefficients (stable extrapolation)
	do i2=2,n2-1
            	do i1=1,n1                                                  
               		a(i1,i2,1) = aa(i1,i2+1,1)
               		a(i1,i2,2) = aa(i1,i2+1,2)
               		r(i1,i2,1) = aa(i1,i2-1,1)
               		r(i1,i2,2) = aa(i1,i2-1,2)
		end do
	end do
	do i1=1,n1
            a(i1,1,1) = aa(i1,2,1)
            a(i1,1,2) = aa(i1,2,2)
            r(i1,n2,1) = aa(i1,n2-1,1)
            r(i1,n2,2) = aa(i1,n2-1,2)
	end do
	do i2=2,n2-1
		do i1=1,n1
			aa(i1,i2,1) = 1. - 2.*aa(i1,i2,1)	
			aa(i1,i2,2) = - 2.*aa(i1,i2,2)	
		end do
	end do

	do i1=1,n1
		tmplr = al(i1,1)-2.
		tmpli = al(i1,2)
		tlr = aa(i1,1,1)
		tli = aa(i1,1,2)
            	aa(i1,1,1) = tlr*tmplr   
     1            	   - tli*tmpli + 1. 
            	aa(i1,1,2) = tlr*tmpli   
     1            	   + tli*tmplr  
		tmprr = ar(i1,1)-2.
		tmpri = ar(i1,2)
		trr = aa(i1,n2,1)
		tri = aa(i1,n2,2)
            	aa(i1,n2,1) = trr*tmprr   
     1            	    - tri*tmpri + 1. 
            	aa(i1,n2,2) = trr*tmpri   
     1            	    + tri*tmprr  
	end do
c
ccccc a upper, aa diagonal, r lower, wave right-hand side 
c
	call v2tris(n1,n2,a,aa,r,wave) 
      	return 
      	end
ccc real array scaled by a real scale
	subroutine rrscale(a,scale,n)
	integer n
	real a(n),scale
	do i=1,n
		a(i) = a(i) * scale
	end do
	return
	end 
ccc complex array scaled by a complex scale
	subroutine ccscale(a,scale,n)
	integer n
	complex a(n),scale
	do i=1,n
		a(i) = a(i) * scale
	end do
	return
	end 
ccc shift term for depth migration
	subroutine cshift(cp,om,v,n,dz)
	integer n
	real dz,om,v(n)
	complex cp(n)
	real tmp,omdz
	omdz = om*dz
	do i=1,n
		tmp = omdz/v(i)
		cp(i) = cp(i) *cmplx(cos(tmp),sin(tmp))
	end do
	return
	end

ccc shift term for depth migration
	subroutine shiftc(cp,om,dzov,n,cph,ncph,odpp,opi2)
	integer n
	real om,dzov(n),opi2,odpp
	complex cp(n),cph(ncph)
	real phase, tmp
	integer iphase
	tmp = om * opi2
	do i=1,n
		phase = tmp * dzov(i)
		iphase = phase
		phase = (phase-iphase)*odpp+1.5
		iphase = phase
        	if (iphase.lt.ncph) then
			cp(i) = cp(i) *cph(iphase)
        	end if
	end do
	return
	end

ccc combine real and imaginary parts into complex
	subroutine ri2c(cr,ci,c,n)
	integer n
	complex c(n)
	real cr(n),ci(n)
	do i=1,n
		c(i) = cmplx(cr(i),ci(i))
	end do
	return
	end
	subroutine ri2ct(cr,ci,c,n1,n2)
	integer n1,n2
	complex c(n2,n1)
	real cr(n1,n2),ci(n1,n2)
	do i2=1,n2
		do i1=1,n1
			c(i2,i1) = cmplx(cr(i1,i2),ci(i1,i2))
		end do
	end do
	return
	end
ccc split complex into real and imaginary parts 
	subroutine c2ri(c,cr,ci,n)
	integer n
	complex c(n)
	real cr(n),ci(n)
	do i=1,n
		cr(i) = real(c(i))
		ci(i) = aimag(c(i))
	end do
	return
	end
	subroutine c2rit(c,cr,ci,n1,n2)
	integer n1,n2
	complex c(n1,n2)
	real cr(n2,n1),ci(n2,n1)
	do i2=1,n2
		do i1=1,n1
			cr(i2,i1) = real(c(i1,i2))
			ci(i2,i1) = aimag(c(i1,i2))
		end do
	end do
	return
	end
ccc real array transpose
	subroutine rtransp(p,pt,n1,n2)
	integer n1,n2
	real p(n1,n2),pt(n2,n1)
	do i2=1,n2 
		do i1=1,n1 
			pt(i2,i1) = p(i1,i2)
		end do
	end do
	return
	end 
ccc find an average of a real array
	subroutine ravg(a,n,aavg)
	integer n
	real a(n),aavg
	aavg = a(1)
	do i=2,n
		aavg = aavg + a(i)
	end do
	if(n.gt.0) aavg = aavg / n
	return
	end
ccc find a minimum of a real array
	subroutine rmin(a,n,amin)
	integer n
	real a(n),amin
	amin = a(1)
	do i=2,n
		if(amin.gt.a(i)) amin=a(i)
	end do
	return
	end
ccc real to real array copy
	subroutine rrcopy(a,b,n)
	integer n
	real a(n),b(n)
	do i=1,n
		b(i) = a(i)
	end do
	return
	end
cccc complex array copy
	subroutine cccopy(a,b,n)
	complex a(n),b(n)
	integer n
	do i=1,n
		b(i) = a(i)
	end do
	return
	end
