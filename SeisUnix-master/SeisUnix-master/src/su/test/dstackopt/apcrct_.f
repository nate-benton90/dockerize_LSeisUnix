	subroutine apcrct(anew0,aold0,dd,omdd,nt,len,ntn,n)
	integer new0,old0,dd,omdd,nt,len,ntn,n!
	integer j,old0p1,anew0,aold0!
	old0=aold0
	new0=anew0
	j = 0!
!
!	/* clear the stack */
	call vclr(0,1,ntn)!
!
!	/* loop over traces in data */
10	old0p1 = old0 + 1!
	call vsmul(old0p1,1,dd,new0,1,len)!       /* new[j] = dd*old[j+id+1] */
	call vsma(old0,1,omdd,new0,1,new0,1,len)! /* new[j] += omdd*old[j+id] */
	old0 = old0 + nt!
	new0 = new0 + nt!
	j = j + 1!
	if (j .lt. n) goto 10!
	return
	end
