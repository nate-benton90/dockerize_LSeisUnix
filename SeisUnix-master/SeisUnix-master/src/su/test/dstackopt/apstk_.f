	subroutine apstk(anhm1,ntsh)
	integer nhm1,ntsh,anhm1
	integer j!
	j = ntsh!
	nhm1=anhm1!
!
!	/* loop over traces */
10	call vadd(0,1,j,1,0,1,ntsh)!
	j = j + ntsh!
	nhm1 = nhm1 - 1!
	if (nhm1.gt.0) goto 10! /* (nhm1) the first trace was already there */
!
!	/* power */
	call svesq(0,1,ntsh,ntsh)!
	return
	end
