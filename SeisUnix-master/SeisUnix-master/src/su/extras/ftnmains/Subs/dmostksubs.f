************************************************************************
*
* $Author: chris $
* $Source: /src/su/src/Subs/RCS/dmostksubs.f,v $
* $Revision: 1.1 $ ; $Date: 88/10/21 14:20:50 $
*
************************************************************************

      	subroutine spred1(trace,stkdata,ntn,dtn,nt0,dt0,
     :			  xmin,nx,dx,h,mid,v)

************************************************************************
*
*	SPRED1 -- fortran subroutine for doing Common 
*		  Offset DMO on a single input trace using a 
*		  Kirchhoff (x-t) domain algorithm.
*
*	INPUT DATA/PARAMETERS -- trace(ntn),ntn,dtn,nt0,dt0,
*						xmin,nx,dx,h,mid,v
*			         .. input data has been NMO'd ..
*
*	OUTPUT DMO'D DATA     -- stkdata(nt0,nx)	
*			         .. still needs sqrt(iw) post processing ..
*
*	SUMMARY --   The algorithm is 'input trace sequential', each
*		     input trace is 'spread' between source and receiver
*		     along its impulse response.  By repeated call of
*		     this sub a stack is built by superpostion. 
*		     Memory requirements are low because only the
*		     zero offset data volume (nt,nx) is kept in memory.
*
*	CAVEATS --   Theory predicts scale=1/sqrt(2*pi), but
*		     scale=pi is best match to Hale amplitude (??)
*
*   Technical Reference: Liner, C. L., 1988, Colo. School of Mines,
*			 Center for Wave Phenomena, Res. Report cwp-073R
*
*
*
*   Definition of Variables - Real
*
*	AMP	=   operator amplitude
*	B	=   abs( x - y )
*	B2	=   b * b
*	DTN	=   time sample rate on input data
*	DT0	=   time sample rate on output data
*	DX	=   trace spacing on output data
*	H	=   one-half of source-receiver offset of trace
*     	ONE 	=   1.0
*     	MID 	=   midpoint of input trace
*     	PI 	=   pi
*	SCALE	=   constant scale factor
*    STKDATA(,) =   output DMO stacked data
*       TRACE() =   input data trace
*	TDIP	=   (x,t0)-dip of operator limb at some point
*	TN	=   time on input data ("nmo time")
*	TN2	=   tn * tn
*	TNMAX	=   maximum time on input data
*	TMP	=   temporary variable
*	T0	=   time on output data ("zero-offset time")
*	T02	=   t0 * t0
*	TWO	=   2.0
*	TWOPI	=   2.0*pi
*	V	=   constant seismic velocity for operator truncation
*	W	=   interpolation weight 
*	W1	=   interpolation weight 
*	W2	=   interpolation weight
*	X	=   midpoint coordinate on output data
*	XMIN	=   minimum midpoint coordinate on output data
*	ZERO	=   0.0
*
*   Definition of Variables - Integer
*
*	IMID	=   index of midpoint coord 'mid'
*	INDEX	=   general purpose index 
*	ITDIP	=   (x,t0)-dip of operator limb in samples per trace 
*	ITN	=   input time sample counter 
*	IT0	=   output time sample counter 
*	ITT	=   local t0 index
*	IX	=   output trace counter
*	IXX	=   local x index
*	IXMIN	=   index of midpoint coord 'xmin'
*	K	=   counter
*	MAXX	=   max output trace for spreading 
*	MINX	=   min output trace for spreading
*	NTN	=   time samples on input data
*	NT0	=   time samples on output data
*	NX	=   traces on output data
*
************************************************************************

      	integer k,	itdip,	itn,	itt,	it0
	integer	ix,	ixx,	imid,	index,	ixmin 
      	integer ntn,	nt0,	nx

      	real 	trace(ntn),	stkdata(nt0,nx)
	real 	amp,	b,	b2,	dtn,	dt0,	dx,	h
	real 	h2,	mid,	minx,	maxx,	one,	pi,	scale
	real	tdip,	tmp,	tn,	tn2,	two,	t0,	t02
	real 	v,	vtno2h,	w,	w1,	w2,	x,	xmin	
	real	y,	zero

      	parameter (pi=3.14159265)

c 	.. constants
      	zero = 0.0
      	one = 1.0
      	two = 2.0
	scale = pi

c 	.. calc some indices and a handy constant
	imid = mid/dx +1
	ixmin = xmin/dx + 1
	h2 = h*h

c .. single trace Kirchoff algorithm for Hale's dmo
 
c   	.. handle special case of zero offset trace (no DMO needed)
	if ( h .eq. zero ) then
	   do 1 itn = 1, ntn
		it0 = itn
		stkdata(it0,imid) = stkdata(it0,imid) + trace(itn)
 1	   continue
	   return
	endif
 
c	.. Loop over input times (nmo times, t sub n) 
      	do 22 itn = 1, ntn
	    tn = ( itn - 1 ) * dtn
	    tn2 = tn*tn

	    vtno2h = v*tn / (two*h)

c   	    .. if this sample is zero, there is nothing to spread
c	    .. (a tolerance criteria could be inserted here)
	    if ( trace(itn) .eq. zero ) goto 22

c   	    .. only output traces between source and receiver
c	    .. can possibly be involved
	    minx = ( mid - h ) / dx + one 
	    maxx = ( mid + h ) / dx + two	

c   	    .. don't spread IR past edges of stkdata
	    if ( minx. lt. ixmin )  minx = ixmin 
	    if ( maxx .gt. nx ) maxx = ixmin + nx

c	    .. Loop over output traces between source and receiver
	    do 23 ix = minx, maxx
		x = ( ix - 1 )*dx

c		... find output midpoint index for 
c		... this trace
		ixx = ix - ixmin + 1

		b = abs ( x - mid )
		b2 = b*b
 
c		.. operator limb truncation is
c		.. based on dt0/dx < 2/v
		if ( b .lt. h / sqrt(vtno2h*vtno2h + one) ) then
 
c			.. avoid negative "tmp"
c		   	.. (which would imply complex t0) 
		   	tmp = one - b2 / h2
		   	if ( tmp .lt. zero ) goto 23
		   	tmp = sqrt( tmp )

c		   	.. t0 is impulse response time-intercept 
c		   	.. on this trace
		   	t0 = tn*tmp
	    		t02 = t0*t0

			if ( t0 . eq. 0.0 .or. b .ge. h ) goto 23

c			.. find dip (time samples per trace) of operator limb
			tdip = tn*b*dx / (h*sqrt( h*h - b*b ))
			itdip = int ( tdip / dt0 ) 

c		   	.. amplitude from asymptotics 
		   	amp = scale*tn*tn / (h*t0*sqrt(t0))

c		   	.. taper amp to zero where asymptotics 
c			.. break down near b = h
			if ( b .gt. .5*h ) then 
		   		amp = amp * 2.0 * ( one - b / h )
			endif

c		   	.. getwts calcs interpolation weights
c		   	.. since t0 will (usually) be between samples
c			   .. w1 is distance to sample it0
c			   .. w2 is distance to sample it0 + 1
		   	call getwts(t0,dt0,nt0,it0,w1,w2)

c		   	.. when limb dip is more than one time sample per trace
c		   	.. spread the spike to a length sufficient 
c			.. for avoiding spatial aliasing of operator


     			tmp =  trace(itn)*amp
			do 111 k = -itdip, itdip 

c			   ... find the nearest sample point
c			   ... then calc linear interp weight at t0=itt
			   if ( w1 .ge. w2 ) then
			      w = float(itdip + 1 - abs(k)) /
     :				      float(itdip + 1)
			      itt = it0 - k
			   else
			      w = float(itdip + 1 - abs(k)) /
     :				      float(itdip + 1)
			      itt = it0 + 1 - k
			   endif

c			   ... place the interpolated amp 
c			   ... on the output trace
			   if ( itt .ge. 1 .and. itt .le. nt0 ) then
		   	      stkdata(itt,ixx) = stkdata(itt,ixx)+tmp*w
			   endif

 111			continue 


		endif

 23          continue

 22	continue

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
	subroutine getwts(t,dt,nt,it,w1,w2)
	real delta,t,dt,w1,w2
	integer it,ix,nt

c
c	    .. t is on sample 'it' or between 'it' and 'it+1'
	    it = ifix(t/dt + 1.0) 
c
c	    .. check if point is out of range
	    if(it.gt.nt .or. it+1.lt.1 ) then
		w1 = 0.0
		w2 = 0.0
	    else
c
c	    	.. delta is the fraction of dt from 'it' to t
		delta = t/dt - (float(it)-1.0)
c
c	    	.. when delta=0 then t is ON 'it' 
		if (delta.eq.0.0) then
			w1 = 1.0
			w2 = 0.0
c
c	    	.. when delta.ne.0 then then assign weights
c		.. to the two sample points 
		else
			w1 = 1 - delta
			w2 = delta
		endif
	    endif
	return
	end
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      	subroutine iomegasub(data,nt,dt,nx,pow,idata,cdata,w)

************************************************************************
*
*   IOMEGASUB -- fortran subroutine for applying a general (fractional)
*	     	 time derivative or integral as multiplication by
*	     	 power of i*omega in the frequency domain
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

 
c 	.. Constants
      	zero = 0.0
      	one = 1.0
      	two = 2.0
      	twopi = 2*pi
	ci = cmplx(zero,one)
 
c 	.. Zero-out idata
	call fzero(idata,nt*nx)
 
c 	.. Initialize w vector
c
c					Positive frequencies
	ntnyq=nt/2+1
	dw = (twopi)/(float(nt)*dt)
      	do 13 iw=1,ntnyq
         	w(iw)= float(iw-1)*dw
 13   	continue
c					Negative frequencies
      	do 14 iw=ntnyq+1,nt
         	w(iw)= float(iw-1-nt)*dw
 14   	continue
 
c  	.. copy data and idata (which is zero) into cdata
	call ri2c(data,idata,cdata,nt,nx)
 
c  	.. If pow=0 then we can save a LOT of time
	if (pow.eq.zero) goto 125
 
c  	.. Fourier Transform: cdata(t,x) --> cdata(w,x)
c  	.. for each trace
	do 123 ix=1,nx
      	   call fft(nt,cdata(1,ix),1.0,sqrt(1.0/float(nt)))
 
c  	   .. do the iomega mult
	   do 124 iw=1,nt
	  	if (w(iw).eq.0) then
			cdata(iw,ix) = cmplx(zero,zero)
		else
			if (w(iw).gt.zero) then
			   phase = cexp( ci*pi*pow/2.0 )
			else
			   phase = cexp( - ci*pi*pow/2.0 )
			endif
			amp = (abs(w(iw)))**pow
	 	   	cdata(iw,ix)=cdata(iw,ix)*amp*phase
		endif
 124	   continue
 
c  	   	.. Fourier Transform: cdata(w,x) --> cdata(t,x)
      	   call fft(nt,cdata(1,ix),-1.0,sqrt(1.0/float(nt)))
 123	continue
 
c  	.. pull data and idata out of cdata
 125	call c2ri(cdata,data,idata,nt,nx)

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
