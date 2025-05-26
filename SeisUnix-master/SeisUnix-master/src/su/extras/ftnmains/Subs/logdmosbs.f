************************************************************************
*
* This program belongs to the Center for Wave Phenomena
* Colorado School of Mines
*
* $Author: chris $
* $Source: /src/su/src/Subs/RCS/logdmosbs.f,v $
* $Revision: 1.3 $ ; $Date: 88/09/08 20:51:50 $
*
************************************************************************

	subroutine lsdmosub(rdata,nt,dt,ny,dy,h,dmosgn,idata,cdata,ky,w)

************************************************************************
*
*	LSDMOSUB -- fortran subroutine for doing 'Full Log' DMO
*
*	INPUT DATA/PARAMETERS -- rdata(nt,ny),nt,dt,ny,dy,h,dmosgn
*
*	INPUT EMPTY ARRAYS FOR MEMORY ALLOCATION 
*			      -- idata(nt,ny),cdata(nt,ny),ky(ny),w(ny)
*
*	OUTPUT DMO'D DATA     -- rdata(nt,ny)	
*			         .. still needs inverse log stretch ..
*
*	SUMMARY -- carry out preliminary fft's, initialize freq and
*		   wavenumber vectors, make DMO phase operator,
*		   mult data by operator for all freqs and wavenumbers, 
*		   then do inverse ffts.
*
*   Technical Reference: Liner, C. L., 1988, Colo. School of Mines,
*			 Center for Wave Phenomena, Res. Report cwp-073
*		      &	 Bale and Jakubowicz, 1987, SEG Expanded Abstr.,
*			 New Orleans Meeting, p.714 
*
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex data
*	CI	 =  sqrt(-1)
*	CZERO	 =  cmplx(0,0) 
*	PHASE(,) =  phase function of complex operator
*	OPER(,)  =  DMO operator in (freq,wavenumber) space
*     WRKAREA(,) =  ***** NOT USED ******* 
*
*   Definition of Variables - Real
*
*	AMP	=   amplitude of DMO operator 
*	ARG	=   argument for the phase log function
*	BETA	=   ys / h
*      RDATA(,)	=   real common offset input data
*	DKY	=   wavenumber increment
*	DMOSGN	=   flag for forward or inverse dmo
*		    +1 = DMO .... -1 = inv.DMO
*	DT	=   time sample rate of data 
*	DW	=   frequency increment
*	DY	=   trace spacing (midpoint increment) 
*	H	=   half of source-receiver offset 
*	H2	=   h*h
*      IDATA(,) =   imaginary part of cdata
*	KY()	=   wavenumber array
*	KY2	=   ky()*ky()
*	ONE	=   1.0
*	PI	=   pi 
*	TWO	=   2.0
*	W()	=   frequency array
*	W2	=   w()*w()
*	WOKY	=   w()/ky()
*	YS	=   y stationary point (from stationary phase)
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	DMOSGN	=   +1 for DMO; -1 for inverse DMO
*	IKY	=   wavenumber counter
*	IW	=   frequency counter
*	IY	=   trace counter
*	NT	=   time samples on input data
*	NTNYQ	=   nyquist freq index
*	NY	=   traces on input data
*	NYNYQ	=   nyquist wavenumber index
*
************************************************************************
	integer dmosgn,	iky,	iw,	iy,
     :		nt,	ntnyq,	ny,	nynyq
	real 	amp,	arg,	beta,	dk,	dt,	dw,	dy,
     : 		h,	h2,	idata(nt,ny),	ky(ny),	ky2,
     :		one,	pi,	rdata(nt,ny),
     :		two,	w(nt),	woky,	w2,	ys,	zero
	complex ci,	cdata(nt,ny),	czero,	phase,	oper
	parameter(pi=3.14159265)

c	.. set constants
	zero = 0.0
	one = 1.0
	two = 2.0
	czero = cmplx(zero,zero)
	ci = cmplx(zero,one)

c	.. calculate some constants
	ntnyq = nt/2 + 1
	nynyq = ny/2 + 1
	h2 = h*h
 
c  	.. zero-out idata 
	call fzero(idata,nt*ny)
 
c  	..copy rdata and idata (which is zero) into cdata
	call ri2c( rdata, idata, cdata, nt, ny )
 
c  	.. FT (t,x) --> (w,x)
c	.. Fourier transform each trace
	do 123 iy = 1, ny
      	   	call fft( nt, cdata(1,iy), one, sqrt(one/float(nt)) )
 123	continue
 
c  	.. FT (w,x) --> (w,k)
c    	.. rowcc is Rocca's fft routine which takes a 2-D matrix of 
c    	.. data and and FT's across the rows (hence the name)
	call rowcc( nt, ny, cdata, - one, sqrt(one/float(ny)) )
 
c 	.. Initialize ky vector
c
c					.. Positive wavenumbers
	dk= ( two*pi ) / ( float(ny)*dy )
      	do 11 iky = 1, nynyq
         	ky(iky) = ( iky - one )*dk
 11   	continue
c					.. Negative wavenumbers
      	do 12 iky = nynyq + 1, ny
       	  	ky(iky) = ( iky - one - ny )*dk
 12   	continue
 
c 	.. Initialize w vector
c
c					.. Positive frequencies
	dw = ( two*pi ) / ( float(nt)*dt )
      	do 13 iw = 1, ntnyq
         	w(iw) = ( iw - one )*dw
 13   	continue
c					.. Negative frequencies
      	do 14 iw = ntnyq + 1, nt
         	w(iw) = ( iw - one - nt )*dw
 14   	continue
 
c	.. MAIN LOOPS ..
 
c	.. loop over wavenumbers
      	do 111 iky = 1, ny
	        ky2 = ky(iky)*ky(iky)
 
c		.. loop over frequencies
      		do 113 iw = 1, nt
		   	w2 = w(iw)*w(iw)
 
c			.. calc DMO operator avoiding zero freq
c			.. and zero argument for the log function
			if (w(iw).eq.zero) then
			    oper = czero
			else
			    arg = one - h2*ky2/w2
			    if ( arg.gt.zero ) then
			        arg = - .5*log(arg)
			        phase = dmosgn*ci*w(iw)*arg
			        oper = cexp(phase)
			    else
			     	oper = czero
			    endif
			endif
 
c  			.. mult. (w,k)-data by DMO operator
			cdata(iw,iky) = cdata(iw,iky)*oper
 113		continue
 111	continue
 
c  	.. FT (w,k) --> (t,k)
c	.. inverse Fourier transform columns 
	do 124 iky=1,ny
      	   	call fft(nt,cdata(1,iky),-one,sqrt(one/float(nt)))
 124	continue
 
c  	.. FT (t,k) --> (t,x)
c	.. inverse Fourier transform rows 
	call rowcc(nt,ny,cdata,one,sqrt(one/float(ny)))
 
c  	.. pull rdata and idata out of cdata
	call c2ri(cdata,rdata,idata,nt,ny)

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
	subroutine fzero(x,n)
	real x(n)
	do 10 i=1,n
 10	x(i) = 0.0
	return
	end
