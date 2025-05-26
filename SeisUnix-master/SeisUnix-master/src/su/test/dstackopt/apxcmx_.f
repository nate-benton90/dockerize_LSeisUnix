	subroutine apxcmx(apstack,approf,apc,apmx,apdd,aprtr,nt,ntsh,ntn,nc,n)
	integer apstack,approf,apc,apmx,apdd,prtr,nt,ntsh,ntn,nc,n!
	integer  sttr,j,d0,aprtr!
!
	sttr = apstack!
	prtr = aprtr!
	j = 0!
!
!	/* subtract the profile */
10	call vsub(prtr,1,sttr,1,sttr,1,ntsh)!
	sttr  = sttr + nt!		/* advance pointer to stack trace  */
	prtr = prtr + nt!		/* advance pointer to profile trace */
	j = j + 1!
	if(j .lt. n) goto 10!
!
!	/* cross correlate */
	call ccort(approf,apstack,apc,nc,ntn)!
	return
	end
