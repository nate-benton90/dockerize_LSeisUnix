************************************************************************
*
* This program belongs to the Center for Wave Phenomena
* Colorado School of Mines
*
* $Author: chris $
* $Source: /src/su/src/Subs/RCS/haledmosbs.f,v $
* $Revision: 1.5 $ ; $Date: 88/09/08 10:38:04 $
*
************************************************************************

	subroutine haledmosub(rdata,nt,dt,nx,dx,h,v,
     :			      idata,cdata,wrkarea1,kx,w)

************************************************************************
*
*	HALEDMOSUB -- fortran driver for Hale's DMO
*
*	PURPOSE -- carry out preliminary fft's, initialize freq and
*		   wavenumber vectors, call sub for actually doing
*		   Hale algorithm, then do inverse ffts.
*
*
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex data
*
*   Definition of Variables - Real
*
*      	DATA(,)	=   real input data
*	DK	=   wavenumber increment
*	DMOSGN	=   flag for forward or inverse dmo
*	DT	=   time sample rate of data 
*	DW	=   frequency increment
*	DX	=   trace spacing 
*	FTEMP	=   temporary variable
*	H	=   half of source-receiver offset 
*      IDATA(,) =   imaginary part of cdata
*	KX()	=   wavenumber array
*	ONE	=   1.0
*	PI	=   pi 
*	POW	=   exponent of (i*omega) 
*	TWO	=   2.0
*	TWOPI	=   2.0*pi
*	V	=   seismic velocity (constant)
*	W()	=   frequency array
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	IKX	=   wavenumber counter
*	IW	=   frequency counter
*	IX	=   trace counter
*	NT	=   time samples on input data
*	NTNYQ	=   nyquist freq index
*	NX	=   traces on input data
*	NXNYQ	=   nyquist wavenumber index
*
************************************************************************
	integer nt,	nx,
     :		ikx,	iw,	ntnyq,	nxnyq
	complex wrkarea1(nt),	cdata(nt,nx)
	real 	rdata(nt,nx),	idata(nt,nx),	kx(nx),	w(nt),
     : 		dt,	dx,	h,	pi,	dk,	dw,
     :		one,	zero,	ftmp,	v

	parameter(pi=3.14159265)

c	.. calculate some constants

	ntnyq=nt/2+1
	nxnyq=nx/2+1
	zero = 0.0
	one = 1.0

c  	.. zero-out imaginary idata 

	call fzero(idata,nt*nx)
 
c  	.. copy rdata and idata (which is zero) into cdata
 
	call ri2c(rdata,idata,cdata,nt,nx)
 
c  	.. FT (t,x) --> (t,k)
c    	.. rowcc is an fft routine which takes a 2-D matrix of 
c    	.. data and and ft's across the rows (hence the name)
 
	call rowcc(nt,nx,cdata,-1.0,sqrt(1.0/float(nx)))
 
c 	.. Initialize kx vector
c
c						.. Positive wavenumbers
	dk= (2.0*pi)/(float(nx)*dx)
      	do 11 ikx=1,nxnyq
		ftmp = ikx - one
         	kx(ikx)= ftmp*dk
 11   	continue
c						.. Negative wavenumbers
      	do 12 ikx=nxnyq+1,nx
		ftmp = ikx - one - nx
       	  	kx(ikx)= ftmp*dk
 12   	continue
 
c 	.. Initialize w vector
c
c						.. Positive frequencies
	dw = (2.0*pi)/(float(nt)*dt)
      	do 13 iw=1,ntnyq
		ftmp = iw - one
         	w(iw)= ftmp*dw
 13   	continue
c						.. Negative frequencies
      	do 14 iw=ntnyq+1,nt
		ftmp = iw - one - nt
         	w(iw)= ftmp*dw
 14   	continue
 
c  	.. DMO = (t,k) --> (w,k)
c   	.. Hale is a slow transform (i.e., brute force) routine 
c   	.. which does Hale's dmo operation on each (t,k) trace.
 
	do 30 ikx=1,nx
		call hale(nt,dt,w,kx(ikx),h,v,cdata(1,ikx),wrkarea1)
 30	continue
 
c  	.. FT (w,k) --> (t,k)
 
	do 123 ikx=1,nx
      	   	call fft(nt,cdata(1,ikx),-1.0,sqrt(1.0/float(nt)))
 123	continue
 
c  	.. FT (t,k) --> (t,x)
 
	call rowcc(nt,nx,cdata,1.0,sqrt(1.0/float(nx)))
 
c  	.. pull rdata and idata out of cdata
 
	call c2ri(cdata,rdata,idata,nt,nx)

	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c

        subroutine hale(nt,dt,w,kx,h,v,data,wrkarea)

************************************************************************
*
*	HALE -- algorithm sub for Hale's DMO
*
*	PURPOSE -- same
*
*
*
*   Definition of Variables - Complex
*
*	DATA() 	=  complex data vector; fixed kx and all t
*	CI	=  sqrt(-1)
*	CZERO	=  cmplx(0,0)
*	KERN	=  integration kernel
*	SUM	=  running sum for integration
*    WRKAREA()  =  temporary repository for result
*
*   Definition of Variables - Real
*
*	A	=   Hale's A factor; sqrt(1+...)
*	AMP	=   amp term for inverse dmo
*	DMOSGN	=   flag for forward or inverse dmo
*	DT	=   time sample rate of data 
*	H	=   half of source-receiver offset 
*	H2	=   h*h
*	KX	=   wavenumber
*	KX2	=   kx*kx
*	ONE	=   1.0
*	TWO	=   2.0
*	T	=   time
*	T2	=   t*t
*	V	=   seismic velocity (constant)
*	W()	=   frequency array
*	W2	=   w*w
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	DMOSGN	=   flag for dmo or inv.dmo
*	IT	=   time counter
*	IW	=   frequency counter
*	NT	=   time samples on input data
*
************************************************************************

	integer iw,	it,	nt,	sgn

        complex data(nt),	ci,	czero,	kern,
     :		sum,	wrkarea(nt)

        real 	a,	amp,	dt,	h,	h2,	kx,	kx2,
     :		one,	scale,	t,	t2,	two,	v, 	w(nt),	
     :		wt,	w2,	zero

c	.. set up constants

	one = 1.0
	two = 2.0
	zero = 0.0
	czero = cmplx(zero,zero) 
	ci = cmplx(0.0,1.0)
	scale = sqrt( one / float(nt) )

	h2 = h*h
	kx2 = kx*kx

c	.. loop over output frequencies

	do 1 iw = 1,nt 
		w2 = w(iw)*w(iw)

c		.. initialize

		sum = czero
		wrkarea(iw) = czero

c		.. loop over input times

		do 2 it = 1, nt 	 
		    t = (it-1)*dt
		    wt = abs( w(iw)*t )
		    t2 = t*t

		    a =  w2*t2 +  h2*kx2 

c		    ..avoid  a <= 0

		    if ( a .le. zero ) then

			kern = czero

		    else

c			.. calc DMO kernel 
			a = sqrt(a)
		        kern = ( wt / a )*cexp( ci*a*sgn(w(iw)) )

		    endif

c		    .. limit aperture in (w,kx)-plane based on v

		    if ( abs( v*kx ) .gt. abs( two*w(iw) ) ) then
		   	kern = czero
		    endif 

c		    .. discrete sum to approximate integration over t

  		    sum = sum + kern*data(it)

 2		continue

		wrkarea(iw) = scale * sum

 1	continue

c	.. we want the result in 'data'

	do 4 iw=1,nt
		data(iw) = wrkarea(iw)
 4	continue

 5 	return
        end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine ihaledmosub(rdata,nt,dt,nx,dx,h,v,
     :			      idata,cdata,wrkarea1,kx,w)
************************************************************************
*
*	IHALEDMOSUB -- fortran driver for inverse Hale-type DMO
*
*	PURPOSE -- carry out preliminary fft's, initialize freq and
*		   wavenumber vectors, call sub for actually doing
*		   Hale algorithm, then do inverse ffts.
*
*
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex data
*
*   Definition of Variables - Real
*
*      	DATA(,)	=   real input data
*	DK	=   wavenumber increment
*	DT	=   time sample rate of data 
*	DW	=   frequency increment
*	DX	=   trace spacing 
*	FTEMP	=   temporary variable
*	H	=   half of source-receiver offset 
*      IDATA(,) =   imaginary part of cdata
*	KX()	=   wavenumber array
*	ONE	=   1.0
*	PI	=   pi 
*	POW	=   exponent of (i*omega) 
*	TWO	=   2.0
*	TWOPI	=   2.0*pi
*	V	=   seismic velocity (constant)
*	W()	=   frequency array
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	IKX	=   wavenumber counter
*	IW	=   frequency counter
*	IX	=   trace counter
*	NT	=   time samples on input data
*	NTNYQ	=   nyquist freq index
*	NX	=   traces on input data
*	NXNYQ	=   nyquist wavenumber index
*
************************************************************************
	integer ikx,	iw,	ix,
     :		nt,	ntnyq,	nx,	nxnyq
	complex wrkarea1(nt),	cdata(nt,nx)
	real 	rdata(nt,nx),	idata(nt,nx),	kx(nx),	w(nt),
     : 		dt,	dx,	h,	pi,	dk,	dw,
     :		one,	zero,	ftmp,	v

	parameter(pi=3.14159265)

c	.. calculate some constants

	ntnyq=nt/2+1
	nxnyq=nx/2+1
	zero = 0.0
	one = 1.0

c  	.. zero-out imaginary idata 

	call fzero( idata, nt*nx )
 
c  	.. copy rdata and idata (which is zero) into cdata
 
	call ri2c( rdata, idata, cdata, nt, nx )

c  	.. FT (t,x) --> (w,x)
 
	do 123 ix = 1, nx
      	   	call fft( nt, cdata(1,ix), one, sqrt( one/float(nt) ) )
 123	continue
 
c  	.. FT (w,x) --> (w,k)
 
	call rowcc( nt, nx, cdata, -one, sqrt( one/float(nx) ) )
 
c 	.. Initialize kx vector
c
c						.. Positive wavenumbers
	dk= ( 2.0*pi ) / ( float(nx)*dx )
      	do 11 ikx = 1, nxnyq
		ftmp = ikx - one
         	kx(ikx) = ftmp*dk
 11   	continue
c						.. Negative wavenumbers
      	do 12 ikx = nxnyq + 1, nx
		ftmp = ikx - one - nx
       	  	kx(ikx)= ftmp*dk
 12   	continue
 
c 	.. Initialize w vector
c
c						.. Positive frequencies
	dw = ( 2.0*pi ) / ( float(nt)*dt )
      	do 13 iw = 1, ntnyq
		ftmp = iw - one
         	w(iw)= ftmp*dw
 13   	continue
c						.. Negative frequencies
      	do 14 iw = ntnyq + 1, nt
		ftmp = iw - one - nt
         	w(iw) = ftmp*dw
 14   	continue
 
c  	.. Inverse DMO = (w,k) --> (t,k)
c   	.. ihale is a slow transform (i.e., brute force) routine 
c   	.. which does the inverse of Hale's dmo 
 
	do 30 ikx=1,nx
		call ihale(nt,dt,w,dw,kx(ikx),h,v,cdata(1,ikx),wrkarea1)
 30	continue
 
 
c  	.. FT (t,k) --> (t,x)
 
	call rowcc(nt,nx,cdata,one,sqrt(one/float(nx)))
 
c  	.. pull rdata and idata out of cdata
 
	call c2ri(cdata,rdata,idata,nt,nx)

	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c

        subroutine ihale(nt,dt,w,dw,kx,h,v,data,wrkarea)

************************************************************************
*
*	HALE -- algorithm sub for Hale-type inverse DMO
*
*	PURPOSE -- same
*
*	TECHNICAL REF -- Liner, 1988, cwp-073
*
*   Definition of Variables - Complex
*
*	DATA() 	=  complex data vector; fixed kx and all t
*	CI	=  sqrt(-1)
*	CZERO	=  cmplx(0,0)
*	KERN	=  integration kernel
*	SUM	=  running sum for integration
*    WRKAREA()  =  temporary repository for result
*
*   Definition of Variables - Real
*
*	A	=   Hale's A factor; sqrt(1+...)
*	AMP	=   amp term for inverse dmo
*	DT	=   time sample rate of data 
*	H	=   half of source-receiver offset 
*	H2	=   h*h
*	KX	=   wavenumber
*	KX2	=   kx*kx
*	ONE	=   1.0
*	PI	=   pi 
*	TWO	=   2.0
*	TWOPI	=   2.0*pi
*	T	=   time
*	T2	=   t*t
*	V	=   seismic velocity (constant)
*	W()	=   frequency array
*	W2	=   w*w
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	DMOSGN	=   flag for dmo or inv.dmo
*	IT	=   time counter
*	IW	=   frequency counter
*	NT	=   time samples on input data
*
************************************************************************

	integer iw,	it,	nt,	sgn

        complex data(nt),	ci,	czero,	kern,
     :		sum,	wrkarea(nt)

        real 	a,	amp,	dt,	dw,	h,	h2,	kx,
     :		kx2,	one,	pi,	scale,	t,	t2,	two,
     :		twopi,	v, 	w(nt),	wt,	w2,	zero

	parameter(pi=3.14159265)

c	.. set up constants

	one = 1.0
	two = 2.0
	zero = 0.0
	czero = cmplx(zero,zero) 
	ci = cmplx(0.0,1.0)
	twopi = two*pi 
	scale = sqrt( one/float(nt) )

	h2 = h*h
	kx2 = kx*kx

c	.. loop over output nmo-times
	do 1 it = 1,nt 
	        t = (it-1)*dt
	        t2 = t*t

c		.. initialize
		sum = czero
		wrkarea(it) = czero

c		.. loop over input (zero-offset) frequencies
c		.. (avoid zero freq)
		do 2 iw = 2, nt 	 
		    w2 = w(iw)*w(iw)
		    wt = abs( w(iw)*t )
		    a = sqrt( w2*t2 + h2*kx2 )

c		    .. calc integration kernel
c		    .. (avoid singularity of amp)
		    if ( a.eq.zero ) then

			kern = czero

		    else 

c			.. Shuki's amp
c			amp = wt / a

c			.. calc kernel for inverse dmo
c			.. Chris' amp
			amp = ( a*a + h2*kx2 ) / ( a*a )
	                kern = amp*cexp( - ci*a*sgn( w(iw) ) )

		    endif

c		    .. limit aperture in (w,kx)-plane based on v
		    if (abs(v*kx).gt.abs(two*w(iw))) then
		   	kern = czero
		    endif 

c		    .. discrete sum to approximate integration over t
  		    sum = sum + kern*data(iw)

 2		continue

c		.. temporarily put result in wrkarea()
		wrkarea(it) = scale*sum
 1	continue

c	.. we want the result back in data()
	do 4 it=1,nt
		data(it) = wrkarea(it)
 4	continue

 5 	return
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
	subroutine fzero(x,n)
	real x(n)
	do 10 i=1,n
 10	x(i) = 0.0
	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	integer function sgn(x)
	real x
	if (x.lt.0.0) then
		sgn = -1.0
	else if (x.gt.0.0) then
		sgn = 1.0
	else
		sgn = 0.0
	endif
	return
	end
