* $Revision: 1.29 $ ; $Date: 88/04/16 19:56:29 $
* $Source: /src/su/src/Subs/RCS/nmosubs.f,v $

************************************************************************
************************************************************************
**                                                                    **
** Subroutine package for SUNMO                                       **
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
* LINTRP - Linear interpolation
*
* Parameters:
*    IT() - base point for the interpolation (output)
*    W(,) - the interpolation weights (output)
*    T() - the exact nmo times
*    NT - number of data points on time trace
*    GAIN() - stretch array
*
* Local variables
*    I - counter over trace times
*    P - proportional distance for interpolation
*
* Notes:
*	The times have been scaled to 0, 1, ..., nt-1.
*	Thus in STRTCH we must not access t(-1) or t(nt).
*	However, in STRTCH we access t(it(i)+j), 1 <= j <= 2,
*	so we require it(i) + 1 >= 0 and  it(i) + 2 <= nt-1, that is,
*	it(i) >= -1 and it(i) <= nt-3 or equivalently for integers:
*	it(i) > -2 and it(i) < nt-2.
*
* C call:    lintrp_(itnmo, w, tnmo, &nt, gain);
* F77 use:   lintrp (it,    w, t,     nt, gain)
*
************************************************************************

	subroutine lintrp(it, w, t, nt, gain)
	integer nt, it(0:nt-1)
	real t(0:nt-1), w(0:nt-1, 1:2), gain(0:nt-1)

	integer i
	real p

 	do 10 i = 0, nt-1
 		it(i) = int(t(i)) - 1
 		if (it(i) .gt. -2 .and. it(i) .lt. nt-2) then
 			p = t(i) - int(t(i))
 			w(i, 1) = (1.0 - p) * gain(i)
 			w(i, 2) =      p    * gain(i)
 		else
 			it(i) = -1
 			w(i, 1) = 0.0
 			w(i, 2) = 0.0
 		endif
 10	continue
	return
	end


************************************************************************
*
* STRTCH - Compute nmo-stretched trace
*
* Parameters:
*    Y() - nmo stretched data (output)
*    X() - time trace
*    NT - number of data points on time trace
*    IT() - base point for the interpolation
*    W(,) - the interpolation weights
*
* Local variables:
*    I - counter over trace times
*    J - counter over weights
*
* C call:    strtch_(nmoed, tr.data, &nt, itnmo, w);
* F77 use:   strtch (    y,       x,  nt, it,    w)
*
************************************************************************

	subroutine strtch(y, x, nt, it, w)
	integer nt, it(0:nt-1)
	real x(0:nt-1), y(0:nt-1), w(0:nt-1, 1:2) 

	integer i

	do 10 i = 0, nt-1
		y(i) = w(i, 1) * x(it(i) + 1) + w(i, 2) * x(it(i) + 2)
 10	continue
 	return
	end
