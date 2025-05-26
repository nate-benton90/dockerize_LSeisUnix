************************************************************************
*
* This program belongs to the Center for Wave Phenomena
* Colorado School of Mines
*
* $Author: chris $
* $Source: /src/su/src/Subs/RCS/2dfftsbs.f,v $
* $Revision: 1.1 $ ; $Date: 88/04/14 11:19:39 $
*
************************************************************************

      subroutine twodfftsub(rdata,idata,cdata,ctemp,nt,nx)
************************************************************************
*
*	TWODFFTSUB  -- Fast Fourier transform input (t,x)-domain data
*		       to (omega,wavenumber)-domain
*
*	PURPOSE -- same
*
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex input data
*
*   Definition of Variables - Real
*
*      RDATA(,)	=   real part of input data
*      IDATA(,) =   imaginary part of input data (zero)
*	ONE	=   1.0
*
*   Definition of Variables - Integer 
*
*	IKX	=   wavenumber counter
*	NT	=   time samples on input data
*	NX	=   traces on input data
*	IW	=   frequency counter
*
************************************************************************

      	integer	ikx,	nx, 	nt,	iw
      	real 	idata(nt,nx),	one,	rdata(nt,nx)
      	complex	cdata(nt,nx),	ctemp(nt,nx)

c 	.. constants

      	one = 1.0

c  	.. zero-out idata 

	call fzero(idata,nt*nx)

c  	.. copy rdata and idata (which is zero) into cdata

	call ri2c(rdata,idata,cdata,nt,nx)

c  	.. Fourier transform:  (t,x) --> (t,k)
c  	.. rowcc is a Rocca's  fft routine which takes a 2-D matrix 
c  	.. of data and and FT's across the rows (hence the name)

	call rowcc(nt,nx,cdata,-one,sqrt(one/float(nx)))

c  	.. Fourier transform:  (t,k) --> (w,k)
c  	.. for each k there is a time vector, which fft
c  	.. transforms into an omega, w, vector

	do 123 ikx=1,nx
      	   	call fft(nt,cdata(1,ikx),one,sqrt(one/float(nt)))
 123	continue

c	.. wavenumbers and frequencies are in 'machine order'
c	.. i.e., 0 -> nyq , - nyq -> 0 (nearly)
c	.. now flip around for human viewing:  -nyq -> +nyq
	do 20 iw=1,nt
	   do 10 ikx=1,nx
c		.. flip first quadrant (kx>0 and w>0) 
		if ( ikx.le.nx/2 + 1 .and. iw.le.nt/2 + 1 ) then
			ctemp(iw+nt/2-1, ikx+nx/2-1) = cdata (iw,ikx)	
		endif
c		.. flip second quadrant (kx<0 and w>0) 
		if ( ikx.ge.nx/2 + 2 .and. iw.le.nt/2 + 1 ) then
			ctemp(iw+nt/2-1, ikx-nx/2-1) = cdata (iw,ikx)	
		endif
c		.. flip third quadrant (kx>0 and w<0) 
		if ( ikx.le.nx/2 + 1 .and. iw.ge.nt/2 + 2 ) then
			ctemp(iw-nt/2-1, ikx+nx/2-1) = cdata (iw,ikx)	
		endif
c		.. flip fourth quadrant (kx<0 and w<0) 
		if ( ikx.ge.nx/2 + 2 .and. iw.ge.nt/2 + 2 ) then
			ctemp(iw-nt/2-1, ikx-nx/2-1) = cdata (iw,ikx)	
		endif
 10	   continue
 20	continue
		
c	.. now copy ctemp back into cdata
	do 50 iw=1,nt
		do 60 ikx=1,nx
			cdata(iw,ikx) = ctemp(iw,ikx)
 60		continue
 50	continue

c  	.. separate real and imaginary parts of complex result 

	call c2ri(cdata,rdata,idata,nt,nx)

	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine cfzero(x,n)
	integer i,n
	complex x(n)
	do 10 i=1,n
 10		x(i) = cmplx(0.0,0.0)
	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine fzero(x,n)
	integer i,n
	real x(n)
	do 10 i=1,n
 10		x(i) = 0.0
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
