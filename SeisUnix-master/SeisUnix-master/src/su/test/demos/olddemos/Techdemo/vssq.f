************************************************************************
* VSSQ: vector signed square and store.  Vector may overwrite.
*
*	c() = a() * abs(a())
*
* Usage:
*	call vssq(a, i, c, k, n)
*	vssq_(a, &i, c, &k, &n);
*
*	a	real vector input
*	c	real vector result; may be a
*	i	index increment in a
*	k	index increment in c
*	n	element count
*
* Credits:
*	Jack Dongarra's saxpy routine from Linpack.
*	Floating Point Systems for subroutine name and argument list.
*
* Notes:
*	Immediate silent return for vectors of non-positive length.
*	Loop unrolling done for case of all increments equal unity.
*
*---------------------------------------------------------------------------
* Permission to use this library and associated files is granted, provided
* that credit to the sources is given by retaining the headers in the files.
* Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines, 1/1/88
*---------------------------------------------------------------------------
*
* $Author: jkc $
* $Source: /src/general/Veclib/RCS/vssq.f,v $
* $Revision: 1.4 $ ; $Date: 88/01/04 15:17:54 $
************************************************************************
                  
      	subroutine vssq(a, i, c, k, n)
      	integer i, k, n
      	real a(1:n), c(1:n)

*     	...internal variables
*	tmpa, tmpa1,... storage for a(ii), a(ii+1), ...
* 	ia, ic		indices in arrays
*	ii		loop counter
*	mm		integer result of modulo operation
*	nroll		integer parameter for amount of loop unrolling
	real tmpa, tmpa1, tmpa2, tmpa3, tmpa4, tmpa5, tmpa6, tmpa7
      	integer ia, ic, ii, mm, nroll
      	parameter (nroll = 8)

*	...error return
      	if (n .le. 0) return

      	if (i .eq. 1 .and. k .eq. 1) then
*		...optimize by loop unrolling
		mm = mod(n, nroll)
      		do 10 ii = 1, mm
			tmpa = a(ii)
         		c(ii) = tmpa * abs(tmpa)
 10		continue
      		do 20 ii = mm+1, n, nroll
			tmpa  = a(ii)
			tmpa1 = a(ii + 1)
			tmpa2 = a(ii + 2)
			tmpa3 = a(ii + 3)
			tmpa4 = a(ii + 4)
			tmpa5 = a(ii + 5)
			tmpa6 = a(ii + 6)
			tmpa7 = a(ii + 7)
         		c(ii)     = tmpa  * abs(tmpa)
         		c(ii + 1) = tmpa1 * abs(tmpa1)
         		c(ii + 2) = tmpa2 * abs(tmpa2)
         		c(ii + 3) = tmpa3 * abs(tmpa3)
         		c(ii + 4) = tmpa4 * abs(tmpa4)
         		c(ii + 5) = tmpa5 * abs(tmpa5)
         		c(ii + 6) = tmpa6 * abs(tmpa6)
         		c(ii + 7) = tmpa7 * abs(tmpa7)
 20		continue
	else
*		...code for increments unequal or not equal to 1
		ia = 1
      		ic = 1
      		if (i .lt. 0) ia = (-n+1)*i + 1
      		if (k .lt. 0) ic = (-n+1)*k + 1
      		do 30 ii = 1, n
			tmpa = a(ia)
      	 		c(ic) = tmpa * abs(tmpa)
      	 		ia = ia + i
      	 		ic = ic + k
 30 		continue
	endif

      	return
      	end
