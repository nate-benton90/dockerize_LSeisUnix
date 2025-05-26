      subroutine gazdcsub(rdata,nt,dt,nx,dx,v,z,idata,cdata,kx,w)
************************************************************************
*
*	GAZdcSUB -- Gazdag phase shift downward continuation (const. vel)
*
*	PURPOSE -- same
*
*	TECHNICAL REF -- Jon Claerbout 
*			 Imaging the Earth's Interior, pp.30-33
*
*	NOTE: Const. vel for now, but can be extended to c(z)
*
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex data
*	CI	 =  sqrt(-1)
*
*   Definition of Variables - Real
*
*      RDATA(,)	=   real part of input/output data
*	DK	=   wavenumber increment
*	DT	=   time sample rate of input data 
*	DW	=   frequency increment
*	DX	=   trace spacing 
*      IDATA(,) =   imaginary part of input/output data
*	KZ	=   kz wavenumber
*	KX()	=   wavenumber array
*	PI	=   pi 
*	TWOPI	=   2.0*pi
*	V	=   seismic velocity (constant)
*	VKX	=   v*kx
*	VKXOW	=   v*kx/w         [w=freq]
*	VKXOW2	=   vkxow*vkxow
*	W()	=   frequency array
*	Z	=   downward continuation depth
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

      	integer	ikx,	it,	itau,	iw,	ix,	nx,
     : 		nxnyq,	nt,	ntnyq, i, j

      	real 	dk,	dt,	dw,	dx,	idata(nt,nx),
     :		kz,	kx(nx),
     :		pi,	rdata(nt,nx),	twopi,
     :		v,	vkx,	vkow,	vkowsq,	w(nt),	z

      	complex	cdata(nt,nx),	ci,	czero
      	parameter (pi=3.14159265)

c 	.. constants
      	twopi = 2.0*pi
      	ci = cmplx(0.0, 1.0)
	czero = cmplx(0.0, 0.0)
      	ntnyq = nt/2 + 1
      	nxnyq = nx/2 + 1

c  	.. algorithm uses half-speed 
	v = v/2.0

c  	.. zero-out idata 
c	call vfill(0.0, idata, 1, nt*nx)
	do 991 j = 1, nx
		do 990 i = 1, nt
			idata(i,j) = 0.0
 990		continue
 991	continue

c  	.. copy rdata and idata (which is zero) into cdata
	call ri2c(rdata, idata, cdata, nt, nx)

c  	.. Fourier transform:  (t,x) --> (t,k)
c  	.. rowcc is a Rocca's  fft routine which takes a 2-D matrix 
c  	.. of data and and FT's across the rows (hence the name)
	call rowcc(nt, nx, cdata, -1.0, sqrt(1.0/float(nx)))

c  	.. Fourier transform:  (t,k) --> (w,k)
c  	.. for each k there is a time vector, which fft
c  	.. transforms into an omega, w, vector
	do 10 ikx = 1, nx
      	   	call fft(nt, cdata(1, ikx), 1.0, sqrt(1.0/float(nt)))
 10	continue


c 	.. Initialize wavenumber, kx, array
c					.. Positive wavenumbers
      dk= (twopi)/(float(nx)*dx)
      do 20 ikx = 1, nxnyq
         kx(ikx) = (ikx-1)*dk
 20   continue
c					.. Negative wavenumbers
      do 30 ikx = nxnyq+1, nx
         kx(ikx) = (ikx-1-nx)*dk
 30   continue


c 	.. Initialize frequency, w, array
c					.. Positive frequencies
      dw = twopi/(float(nt)*dt)
      do 40 iw = 1, ntnyq
         w(iw) = (iw-1)*dw
 40   continue
c					.. Negative frequencies
      do 50 iw = ntnyq+1, nt
         w(iw) = (iw-1-nt)*dw
 50   continue


c					.. Loop over wavenumbers
         do 70 ikx = 1, nx
            vkx = v*kx(ikx)
c					.. Loop over frequencies
            do 60 iw = 2, nt
               	vkow = vkx/w(iw)
               	vkowsq = vkow*vkow

c		.. test criteria for rejecting evanescent energy
               	if (vkowsq .gt. 1.0) then
                   cdata(iw, ikx) = 0.0
               	else
c		   .. construct vertical, ktau, wavenumber
                   kz = w(iw)*sqrt(1.0 - vkowsq) / v

c		   .. downward continue data via mult by propagator
                   cdata(iw, ikx) = cexp( - ci*kz*z )*cdata(iw, ikx)
               	endif
 60          continue
 70       continue

c  	.. Fourier transform:  (w,k) --> (w,x)
	call rowcc(nt, nx, cdata, 1.0, sqrt(1.0/float(nx)))

c  	.. Fourier transform:  (w,x) --> (t,x)
	do 80 ikx = 1, nx
      	   	call fft(nt, cdata(1, ikx), -1.0, sqrt(1.0/float(nt)))
 80	continue

c  	.. get real part of downward continued data
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
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	.. Combine two real arrays into a complex array
c	.. Probably should add to Veclib
	subroutine ri2c(re, im, c, nt, nx)
	integer nt, nx
	real re(nt, nx), im(nt, nx)
	complex c(nt, nx)

c	.. local variables
	integer ix, it

	do 10 ix = 1, nx
		do 20 it = 1, nt
			c(it, ix) = cmplx(re(it, ix), im(it, ix))
 20		continue
 10	continue
	return
	end
