ccc vector smoothing 	
	subroutine vsmth(data,n1,n2,lsm,work)
	real data(n1,n2),work(n1,n2)
	integer n1,n2
	real f1(4096), f2(4096)
	nf1 = lsm 
	nf2 = lsm 
	if(nf1.gt.4096) call cancel("nf1 too large >4096",nf1)
	if(nf2.gt.4096) call cancel("nf2 too large >4096",nf2)
	call smth2d(data,work,f1,f2,n1,n2,nf1,nf2)
	return
	end 
