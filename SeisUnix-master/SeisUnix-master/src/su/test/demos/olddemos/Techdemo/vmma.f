************************************************************************
* VMMA: multiplies first pair of vectors, multiplies second pair of
*      vectors, adds results and stores in a fifth vector.  Vectors may
*      overwrite.
*
*	e() = (a() * b()) + (c() * d())
*
* Usage:
*	call vmma(a, incra, b, incrb, c, incrc, d, incrd, e, incre, n)
*	vmma_(a, &incra, b, &incrb, c, &incrc, d, &incrd, e, &incre, &n);
*
*	a	real vector factor
*	b	real vector factor; may be a
*	c	real vector factor; may be a or b
*	d	real vector factor; may be a, b or c
*	e	real vector result; may be a, b, c or d
*	i	index increment in a
*	j	index increment in b
*	k	index increment in c
*	l	index increment in d
*	m	index increment in e
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
* $Source: /src/general/Veclib/RCS/vmma.f,v $
* $Revision: 1.4 $ ; $Date: 88/01/04 15:17:17 $
************************************************************************
                  
      	subroutine vmma(a, i, b, j, c, k, d, l, e, m, n)
      	integer i, j, k, l, m, n
      	real a(1:n), b(1:n), c(1:n), d(1:n), e(1:n)

*     	...internal variables
* 	ia, ib, ic, id, ie	indices in arrays
*	ii			loop counter
*	mm			integer result of modulo operation
*	nroll			integer parameter for loop unrolling
      	integer ia, ib, ic, id, ie, ii, mm, nroll
      	parameter (nroll = 8)

*	...error return
      	if (n .le. 0) return

      	if (i .eq. 1 .and. j .eq. 1 .and. k .eq. 1
     :               .and. l .eq. 1 .and. m .eq. 1) then
*		...optimize by loop unrolling
    		mm = mod(n, nroll)
      		do 10 ii = 1, mm
         		e(ii) = (d(ii) * c(ii)) + (b(ii) * a(ii))
 10 		continue
      		do 20 ii = mm+1, n, nroll
			e(ii)     = (d(ii)     * c(ii))     +
     :				    (b(ii)     * a(ii))
			e(ii + 1) = (d(ii + 1) * c(ii + 1)) +
     :				    (b(ii + 1) * a(ii + 1))
			e(ii + 2) = (d(ii + 2) * c(ii + 2)) +
     :				    (b(ii + 2) * a(ii + 2))
			e(ii + 3) = (d(ii + 3) * c(ii + 3)) +
     :				    (b(ii + 3) * a(ii + 3))
			e(ii + 4) = (d(ii + 4) * c(ii + 4)) +
     :				    (b(ii + 4) * a(ii + 4))
			e(ii + 5) = (d(ii + 5) * c(ii + 5)) +
     :				    (b(ii + 5) * a(ii + 5))
			e(ii + 6) = (d(ii + 6) * c(ii + 6)) +
     :				    (b(ii + 6) * a(ii + 6))
			e(ii + 7) = (d(ii + 7) * c(ii + 7)) +
     :				    (b(ii + 7) * a(ii + 7))
 20 		continue
	else
*		...code for increments unequal or not equal to 1
		ia = 1
		ib = 1
      		ic = 1
	        id = 1
	        ie = 1
	        if (i .lt. 0) ia = (-n+1)*i + 1
	        if (j .lt. 0) ib = (-n+1)*j + 1
	        if (k .lt. 0) ic = (-n+1)*k + 1
          	if (l .lt. 0) ic = (-n+1)*l + 1
      		if (m .lt. 0) ic = (-n+1)*m + 1
      		do 30 ii = 1, n
      	 		e(ie) = (d(id) * c(ic)) + (b(ib) * a(ia))
      	 		ia = ia + i
      	 		ib = ib + j
      	 		ic = ic + k
      	 		id = id + l
      	 		ie = ie + m
 30 		continue
	endif

      	return
	end
