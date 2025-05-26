	subroutine aprstk(pstack,dd,aprtr,nt,ntsh,n)
	integer pstack,apstack,dd,prtr,nt,ntsh,n!
!
	integer omdd,prtrp1,j,aprtr!
!
	apstack=pstack
	prtr=aprtr
	omdd = dd + 1!		/* 1. - dd */
!
!	/* add the profile with linear interpolation */
	j = 0!
10	prtrp1 = prtr + 1!		/* trace[j-id-1] of profile   */
!	/*-=(1-dd)*profile[j]  */
	call vsma(prtr,1,omdd,apstack,1,apstack,1,ntsh)
!	/*-=dd*profile[j+1]    */
	call vsma(prtrp1,1,dd,apstack,1,apstack,1,ntsh)
	apstack  = apstack + nt!	/* advance pointer to stack trace  */
	prtr = prtr + nt!		/* advance pointer to profile trace */
	j = j + 1!
	if(j .lt. n) goto 10!
	return
	end
