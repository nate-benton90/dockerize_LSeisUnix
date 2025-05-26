************************************************************************
* $Author: chris $
* $Source: /src/su/src/Subs/RCS/iomegasubs.f,v $
* $Revision: 1.2 $ ; $Date: 88/03/14 13:43:23 $
************************************************************************

      	subroutine iomegasub(data,nt,dt,nx,pow,idata,cdata,w)

************************************************************************
*
*   Definition of Variables - Complex
*
*	CDATA(,) =  complex data
*	CI	=   sqrt(-1) 
*	PHASE	=   complex phase of (i*w)**pow 
*
*   Definition of Variables - Real
*
*	AMP	=   real amp of (i*w)**pow 
*	DT	=   time sample rate of data 
*	DW	=   frequency increment
*	IDATA(,) =  imaginary part of cdata
*      	DATA(,)	=   real input data
*	ONE	=   1.0
*	PI	=   pi 
*	POW	=   exponent of (i*omega) 
*	TWO	=   2.0
*	TWOPI	=   2.0*pi
*	W()	=   frequency array
*	ZERO	=   0.0
*
*   Definition of Variables - Integer 
*
*	IW	=   frequency counter
*	IX	=   trace counter
*	NT	=   time samples on input data
*	NTNYQ	=   nyquist freq index
*	NX	=   traces on input data
*
************************************************************************

      	integer nt,nx,ntnyq,iw,ix
	complex ci,cdata(nt,nx),phase
      	real data(nt,nx),idata(nt,nx),amp
      	real w(nt),pow,dt,dw,zero,one,two,twopi,pi
      	parameter (pi=3.14159265)

c
c Constants
c
      	zero = 0.0
      	one = 1.0
      	two = 2.0
      	twopi = 2*pi
	ci = cmplx(zero,one)
c
c Zero-out idata
c
	call fzero(idata,nt*nx)
c
c
c Initialize w vector
c
c					Positive frequencies
	ntnyq=nt/2+1
	dw = (twopi)/(float(nt)*dt)
      	do 13 iw=1,ntnyq
         	w(iw)= float(iw-one)*dw
 13   	continue
c					Negative frequencies
      	do 14 iw=ntnyq+1,nt
         	w(iw)= float(iw-one-nt)*dw
 14   	continue
c
c  copy data and idata (which is zero) into cdata
c
	call ri2c(data,idata,cdata,nt,nx)
c
c  If pow=0 then we can save a LOT of time
c
	if (pow.eq.zero) goto 125
c
c  Fourier Transform: cdata(t,x) --> cdata(w,x)
c  for each trace
c
	do 123 ix=1,nx
      	   call fft(nt,cdata(1,ix),-1.0,sqrt(1.0/float(nt)))
c
c  	   do the iomega mult
c
	   do 124 iw=1,nt
	  	if (w(iw).eq.0) then
			cdata(iw,ix) = cmplx(zero,zero)
			goto 124
		endif
			if (w(iw).gt.zero) then
			   phase = cexp(ci*pi*pow/2.0)
			else
			   phase = cexp(-ci*pi*pow/2.0)
			endif
			amp = (abs(w(iw)))**pow
	 	   	cdata(iw,ix)=cdata(iw,ix)*amp*phase
 124	   continue
c
c  	   Fourier Transform: cdata(w,x) --> cdata(t,x)
c
      	   call fft(nt,cdata(1,ix),1.0,sqrt(1.0/float(nt)))
 123	continue
c
c  pull data and idata out of cdata
c
 125	call c2ri(cdata,data,idata,nt,nx)

	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
	subroutine fzero(x,n)
	real x(n)
	do 10 i=1,n
 10		x(i) = 0.0
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
