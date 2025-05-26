      subroutine gazmigsub(rdata,outrdata,nt,dt,nx,dx,v,ntau,
     :		            idata,cdata,result,kx,w)
************************************************************************
*
*	GAZMIGSUB -- Gazdag phase shift migration (const. vel)
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
*	CDATA(,) =  complex input data
*      RESULT(,) =  complex result
*	CI	 =  sqrt(-1)
*
*   Definition of Variables - Real
*
*      RDATA(,)	=   real part of input data
*	DK	=   wavenumber increment
*	DT	=   time sample rate of input data 
*	DTAU	=   'time' sample rate of output data 
*	DW	=   frequency increment
*	DX	=   trace spacing 
*      IDATA(,) =   imaginary part of input data (zero)
*	KTAU	=   modified kz wavenumber
*	KX()	=   wavenumber array
*   OUTRDATA(,)	=   real part of output data
*	PI	=   pi 
*	SCALE	=   scale factor for amp preservation
*	TEST	=   factor for determining evanescent cutoff
*	TWOPI	=   2.0*pi
*	V	=   seismic velocity (constant)
*	VKX	=   v*kx
*	VKXOW	=   v*kx/w         [w=freq]
*	VKXOW2	=   vkxow*vkxow
*	W()	=   frequency array
*
*   Definition of Variables - Integer 
*
*	IKX	=   wavenumber counter
*	ITAU	=   tau counter 
*	IW	=   frequency counter
*	IX	=   trace counter
*	NT	=   time samples on input data
*	NTAU	=   'time' samples on output data
*	NTNYQ	=   nyquist freq index
*	NX	=   traces on input data
*	NXNYQ	=   nyquist wavenumber index
*
************************************************************************

      	integer	ikx,	it,	itau,	iw,	ix,	nx,
     : 		nxnyq,	nt,	ntau,	ntnyq, i, j

      	real 	dk,	dt,	dtau,	dw,	dx,	idata(nt,nx),
     :		ktau,	kx(nx), outrdata(ntau,nx),
     :		pi,	rdata(nt,nx),	scale,	test,	twopi,
     :		v,	vkx,	vkow,	vkowsq,	w(nt)

      	complex	cdata(nt,nx),	ci,	czero,  result(ntau,nx)
      	parameter (pi=3.14159265)

c 	.. constants
      	twopi = 2.0*pi
      	ci = cmplx(0.0, 1.0)
	czero = cmplx(0.0, 0.0)
      	ntnyq = nt/2 + 1
      	nxnyq = nx/2 + 1
      	dtau = (float(nt)/float(ntau))*dt

c  	.. scale for amp consistency; 
c  	.. same as for inverse FT over omega 
      	scale = 1.0/sqrt(float(nt))

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

c 	.. zero-out complex result
c	call cvfill(czero, result, 1, ntau*nx)
	do 993 j = 1, nx
		do 992 i = 1, ntau
			result(i,j) = 0.0
 992		continue
 993	continue

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


c  	.. Transformation:  (w,k) --> (tau,k)
c  	.. Gazdag phase shift algorithm is
c  	.. effectively a 'slow' transform to tau,
c	.. a depth variable (in time units)
c					.. Loop over tau
      do 80 itau = 1, ntau
c					.. Loop over wavenumbers
         do 70 ikx = 1, nx
            vkx = v*kx(ikx)
c					.. Loop over frequencies
            do 60 iw = 2, nt
               	vkow = vkx/w(iw)
               	vkowsq = vkow*vkow

c		.. test criteria for rejecting evanescent energy
          	test = 1.0 - float(itau)/float(ntau)
		test = test*test
               	if (vkowsq .gt. test) then
                   cdata(iw, ikx) = 0.0
               	else
c		   .. construct vertical, ktau, wavenumber
                   ktau = w(iw)*sqrt(1.0 - vkowsq)

c		   .. downward continue data via mult by propagator
                   cdata(iw, ikx) = cexp( - ci*ktau*dtau)*cdata(iw, ikx)

c		   .. sum over all freqs to get result at itau
c		   .. (imaging condition: inv.FT with t=0)
                   result(itau,ikx) = result(itau,ikx) + cdata(iw,ikx)
               	endif
 60          continue
 70       continue
 80    continue

c  	.. Fourier tranform:  (tau,k) --> (tau,x)

	call rowcc(ntau, nx, result, 1.0, sqrt(1.0/float(nx)))

c  	.. migrated data is real part of complex result 
c  	.. (don't forget scale)
	do 100 ix = 1, nx
		do 90 it = 1, ntau
			outrdata(it, ix) = scale*real(result(it, ix))
 90		continue
 100	continue
	return
	end
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
