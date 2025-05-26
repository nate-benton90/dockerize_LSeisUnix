* $Revision: 1.23 $ ; $Date: 87/05/08 17:14:46 $
* $Source: /mnt2/h/jkcohen/Su/RCS/nmosubs.f,v $

************************************************************************
************************************************************************
**                                                                    **
** Subroutine package for SYCNMO                                      **
**                                                                    **
**                                                                    **
**     VARNMO - NMO for vertically varying velocity                   **
**                                                                    **
**     CONNMO - NMO for constant velocity                             **
**                                                                    **
**     SNCTRP - Sinc interpolation                                    **
**                                                                    **
**     LINTRP - Linear interpolation                                  **
**                                                                    **
**     STRTCH - Compute nmo-stretched trace                           **
**                                                                    **
** Credits:                                                           **
**     Shuki, Jack                                                    **
**                                                                    **
** This program belongs to the Center for Wave Phenomena              **
** Colorado School of Mines                                           **
**                                                                    **
** Copyright 1987 by Center for Wave Phenomena                        **
**                                                                    **
************************************************************************
************************************************************************

************************************************************************
*
* VARNMO - NMO for vertically varying velocity
*
* Parameters:
*    TNMO() - nmo stretched times (output)
*    NT - number of data points on time trace
*    V(), TV() - velocity profile, velocity and time picks
*    NV - number of V, TV pairs
*    X - offset
*
* Local variables:
*    IT - counter over the equally spaced (integer) trace times
*    J - counter over v-tv picks
*    VMOD - velocities on the it mesh
*    ARG - temporary
*
* Notes:
*	The times on the trace have been normalized to the
*	integers, 0, 1, ..., nt-1.
*
*	It might be worthwhile making a separate module VELMOD
*	called in the C main.
*
* C call:    varnmo_(tnmo, nt, ovv, x);
*	     int *nt; float *tnmo,*ovv,*x;
* F77 use:   varnmo (tnmo,  nt, ovv, x)
*
************************************************************************

 	subroutine varnmo(tnmo, nt, ovv, x)
	integer nt
 	real tnmo(0:nt-1), ovv(0:nt-1), x

	integer it
 	real arg

	do 10 it = 0,nt-1
 		arg = it*it + x*x*ovv(it)
		tnmo(it) = sqrt(arg)
 10	continue

	return
	end


************************************************************************
*
* CONNMO - NMO for constant velocity
*
* Parameters:
*    TNMO() - nmo stretched times (output)
*    NT - number of data points on time trace
*    V - the constant velocity
*    X - offset
*
* Local variables
*    IT - counter over trace times
*    ARG - temporary
*
* Notes:
*	The times on the trace have been normalized to the
*	integers, 0, 1, ..., nt-1.
*
* C call:    connmo_(tnmo, nt, ovv, x);
*	     int *nt; float *tnmo,*ovv,*x;
* F77 use:   connmo (tnmo,  nt, ovv, x)
*
************************************************************************

	subroutine connmo(tnmo, nt, ovv, x)
	integer nt
	real tnmo(0:nt-1), ovv, x

	integer it
	real arg,xxovv

	xxovv = x*x*ovv
	do 10 it = 0, nt-1
		arg = it*it + xxovv
		tnmo(it) = sqrt(arg)
 10	continue
	return
	end


************************************************************************
*
* SNCTRP - Sinc interpolation
*
*    IT() - base point for the interpolation (output)
*    W(,) - the interpolation weights (output)
*    T0 - smallest row with some non-zero weight (output)
*    TE - largest row with some non-zero weight (output)
*    T() - the exact nmo times
*    NT - number of data points on time trace
*    NW - number of weights used
*
* Local variables
*
* Notes:
*	The times have been scaled to 0, 1, ..., nt-1.
*	Thus in STRTCH we must not access t(-1) or t(nt).
*	However, in STRTCH we access t(it(i)+j), 1 <= j <= nw,
*	so we require it(i) + 1 >= 0 and  it(i) + nw <= nt-1, that is,
*	it(i) >= -1 and it(i) <= nt-nw-1 or equivalently for integers:
*	it(i) > -2 and it(i) < nt-nw.
*
* C call:    snctrp_(itnmo, w, &t0, &te, tnmo, &nt, &nw);
* F77 use:   snctrp (it,    w,  t0,  te,    t,  nt,  nw)
*
************************************************************************

	subroutine snctrp(it, w, t, nt, nw, gainfu)
	integer nt, it(0:nt-1), nw
	real t(0:nt-1), w(0:nt-1, 1:nw), gainfu(0:nt-1)

***	STUB
***    	call wrtstr('Entered SNCTRP-NOT READY')
***    	call die()
***   	return

	integer j, i, nwby2
	real cx, x, p, blacksinc, pi
	parameter (pi=3.14159265)

C** SAFETY FIRST TEMPS IN CASE WANT TO STAY CLOSER TO PRESTR
	integer lp, lq
	integer first
	save first
	data first/1/

C**	Translation notes:
C**		strfun -> t
C**		nj -> nw
C**		delta -> p

C   CONSTANTS
	cx = 2.0*pi/nw
	nwby2 = nw/2

C** FAKE IN VALUES FOR lp,lq IN CASE WE WANT TO RETAIN THEM AS PARAMETERS. 
C** IF WE ELIMINATE THEM, REPLACE LATER REFS IN CODE BY 0,nt-1.
	lp = 0
	lq = nt - 1

C   SET THE INTEGER PART OF THE STRETCH FUNCTION
	do 10 i = 0, lq
		it(i) = int(t(i)) - nwby2
 10	continue

C***...Guess
	if(first.eq.1) then
		call mktbl(nw)
		first = 0
	endif

C   SET THE INTERPOLATION COEFFICIENTS
*	...Seemed to be paging wrong in the original.
	do 30 j = 1, nw
		do 20 i = 0, nt-1
			p = t(i) - int(t(i))
			x = j - p - nwby2
			w(i, j) = blacksinc(x) * gainfu(i)
 20		continue
 30	continue

	return
	end

c-------------------------------------------------------------

C  END EFFECTS 
	subroutine trpends(it, w, t0, te, nt, nw)
	integer nt, it(0:nt-1), nw,t0,te
	integer iti, hitflag, dj,lp,lq
	real w(0:nt-1, 1:nw)

C TEMPORARY PATCH SO IT WILL WORK
	do 1 i=0,nt-1
		if(it(i).lt.0.or.it(i).ge.nt-nw) then
			it(i) = 0
			do 2 j=1,nw
				w(i,j) = 0.0
 2			continue
		endif
 1	continue
	return
C END OF PATCH

	lp = nt
	lq = nt

	te = -1
	t0 = nt
C**     ...Backshifted index by one--hope that's right.
	do 100 i = 0, lq
		iti = it(i)
		hitflag = 1
		if ( iti .le. -nw ) then
			do 40 j = 1, nw
				w(i, j) = 0.0
 40			continue
			hitflag = 0
			it(i) = 0
C**		...Still should have 0?
		else if ( iti .lt. 0 ) then
			do 50 j = 1, nw+iti
				w(i, j) = w(i, j-iti)
 50			continue
			do 60 j = nw+iti+1, nw
				w(i, j) = 0.0
 60			continue
			it(i) = 0
		else if ( iti .ge. lp ) then
			do 70 j = 1, nw
				w(i, j) = 0.0
 70			continue
			hitflag = 0
			it(i) = lp - nw
		else if ( iti .gt. lp-nw ) then
			it(i) = lp - nw
			dj = iti - it(i)
			do 80 j = nw, 1+dj, -1
				w(i, j) = w(i, j-dj)
 80			continue
			do 90 j = dj, 1, -1
				w(i, j) = 0.0
 90			continue
		endif

C  FIND T0 AND TE
		if ( hitflag .ne. 0 ) then
			if ( te .lt. i ) te = i
			if ( t0 .gt. i ) t0 = i
			hitflag = 1
		endif
 100	continue

	return
	end

c-------------------------------------------------------------

	subroutine mktbl(nj)
	integer nj

C**	include 'sinctbl.cmn'
	integer lsnctb
	parameter (lsnctb=1000)
	real snctbl(lsnctb), sncodx
	common /sincXcb/sintbl, snctbl, sncodx

	real pi
	parameter (pi=3.14159265)

	integer ix
	real x,  dx,  cx,  blackw,  sincp

	cx = 2.0*pi/nj
	dx = real(nj/2)/lsnctb
	sncodx = 1.0/dx
	do 10 ix = 1, lsnctb
		x = dx*(ix-1)
		snctbl(ix) = blackw(cx*x)*sincp(pi*x)
 10	continue
	return
	end

c-------------------------------------------------------------

	real function blacksinc(x)
	real x

	integer ix
C**	include 'sinctbl.cmn'
	integer lsnctb
	parameter (lsnctb=1000)
	real snctbl(lsnctb), sncodx
	common /sincXcb/sintbl, snctbl, sncodx

	ix = sncodx*abs(x)+1.49999
	if ( ix .le. lsnctb ) then
		blacksinc = snctbl(ix)
	else
		blacksinc = 0.0
	endif
	return
	end

c-------------------------------------------------------------

	real function blackw(x)
	real x

C  BLACKMAN WINDOW FROM BLACKMAN & TUKEY P. 98

	blackw = 0.42+0.50*cos(x)+0.08*cos(2.0*x)
	return
	end

c----------------------------------------------------------

	real function sincp(x)
	real x

C  SINCP(X) = SINC(X/PI) = SIN(X)/(X)

*	...Note: small**2 = 2**(24-1)
	parameter (small=3.45E-4)

	if (x.gt.small.or.x.lt.-small) then
		sincp = sin(x)/x
	else
		sincp = 1.0 - 0.1666667*x*x
	endif
	return
	end
   	end



************************************************************************
*
* LINTRP - Linear interpolation
*
* Parameters:
*    IT() - base point for the interpolation (output)
*    W(,) - the interpolation weights (output)
*    T0,TE - NOT USED in LINTRP (needed for compatibility with snctrp)
*    TE - largest row with some non-zero weight (output)
*    T() - the exact nmo times
*    NT - number of data points on time trace
*    NW - NOT USED in LINTRP (needed for compatibility with snctrp)
*
* Local variables
*    I - counter over trace times
*    P - proportional distance for interpolation
*
* Notes:
*	The times have been scaled to 0, 1, ..., nt-1.
*	Thus in STRTCH we must not access t(-1) or t(nt).
*	However, in STRTCH we access t(it(i)+j), 1 <= j <= nw=2,
*	so we require it(i) + 1 >= 0 and  it(i) + 2 <= nt-1, that is,
*	it(i) >= -1 and it(i) <= nt-3 or equivalently for integers:
*	it(i) > -2 and it(i) < nt-2.
*
* C call:    lintrp_(itnmo, w, &t0, &te, tnmo, &nt, &nw);
* F77 use:   lintrp (it,    w,  t0,  te     t,  nt,  nw)
*
************************************************************************

	subroutine lintrp(it, w, t, nt, nw, gain)
	integer nt, it(0:nt-1), nw
	real t(0:nt-1), w(0:nt-1, 2), gain(0:nt-1)

	integer i
	real p

 	do 10 i = 0, nt-1
 		it(i) = int(t(i)) - 1
 		if ( it(i).gt.-2 .and. it(i).lt.nt-2 ) then
 			p = t(i) - int(t(i))
 			w(i, 1) = (1.0 - p) * gain(i)
 			w(i, 2) = p	    * gain(i)
 		else
 			it(i) = -1
 			w(i, 1) = 0.0
 			w(i, 2) = 0.0
 		endif
 10	continue
	return
	end


***Try at t0,te routine - not right, but leave in place for now.
*	t0 = 0
*	te = nt - 1
*	do 10 i = 0, nt-1
*		it(i) = int(t(i)) - 1
*		if ( it(i) .le. -2 ) then
*			t0 = t0 + 1
*		elseif ( it(i) .ge. nt-2 ) then
*			te = te - 1
*		else
*			p = t(i) - int(t(i))
*			w(i, 1) = 1.0 - p
*			w(i, 2) = p
*		endif
* 10	continue
*	return
*	end
***


************************************************************************
*
* STRTCH - Compute nmo-stretched trace
*
* Parameters:
*    Y() - nmo stretched data (output)
*    X() - time trace
*    NT - number of data points on time trace
*    T0 - smallest row with some non-zero weight (output)
*    TE - largest row with some non-zero weight (output)
*    IT() - base point for the interpolation
*    W(,) - the interpolation weights
*    NW - number of weights used in each interpolation
*
* Local variables:
*    I - counter over trace times
*    J - counter over weights
*
* C call:    strtch_(nmoed, tr.data, &nt, &t0, &te, itnmo, w, &nw);
* F77 use:   strtch (    y,       x,  nt,  t0,  te,    it, w,  nw)
*
************************************************************************

	subroutine strtch(y, x, nt, t0, te, it, w, nw)
	integer it(0:nt-1), t0, te
	real x(0:nt-1), y(0:nt-1), w(0:nt-1, 1:nw) 

	integer i, j

	do 10 i = 0, nt-1
		y(i) = 0.0
 10	continue

*       ...Kludge for now
	t0 =0
	te = nt - 1
	do 30 j = 1, nw
		do 20 i = t0, te
			y(i) = y(i) + w(i, j) * x(it(i) + j)
 20		continue
 30	continue

**** Old routine - leave in place for now. Indices switched and
*		   now page the wrong way if we go back to this.
*	do 20 i = 0, nt-1
*		y(i) = 0.0
*		do 10 j = 1, nw
*			y(i) = y(i) + w(i, j) * x(it(i) + j)
*10		continue
*20	continue
 	return
	end
c -------------------------------------------------------------
	subroutine setgain(t,g,n)
	real t(n),g(n)
	g(1) = t(2)-t(1)
	do 1 i=2,n-1
 1	   g(i) = 0.5*(t(i+1)-t(i-1))
	g(n) = t(n)-t(n-1)
	return
	end
c -------------------------------------------------------------
