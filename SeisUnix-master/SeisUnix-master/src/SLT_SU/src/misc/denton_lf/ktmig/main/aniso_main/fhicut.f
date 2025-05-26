	subroutine fhigint(f0,df,nf,ftaper,dt,ifcut,ltaper,
     2			wsave,nsave,nfft)
	real wsave(nsave)
	integer ifcut(nf),ltaper(nf),nf
	real ftaper,f0,df,dt
	real sfft
	integer nsave

	if(2*nfft+15.gt.nsave) then
		call msgsc(" rffti error; check nfft ")
		stop 
	end if
	call rffti(nfft,wsave)
	sfft = 1./nfft
	delf = 1./(nfft*dt)

	tmp = ftaper/delf + 1.
	itaper = tmp
	if(itaper.gt.nfft/2) itaper=nfft/2

	do iw=1,nf
	f = f0 + (iw-1)*df
		tmp = f/delf + 1.	
		ii = tmp
		if(ii.gt.nfft/2) ii=nfft/2
		ifcut(iw) = ii
		ltaper(iw) = itaper
	end do
	return
	end


	subroutine fhigcut(trace,nt,f0,df,nf,ftaper,dt,
     1		ifcut,ltaper,tras,
     2		wsave,nsave,work1,work2,nfft)
	real trace(nt),tras(nt,nf),wsave(nsave),work1(nfft)
	real work2(nfft)
	integer ifcut(nf),ltaper(nf),nf,nt
	real ftaper,f0,df,dt
	real sfft

	sfft = 1./nfft

	do it=1,nfft
		work1(it) = 0.
	end do
	do it=1,nt
		work1(it) = trace(it)
	end do
	call rfftf(nfft,work1,wsave)

	do iw=1,nf
		l = 2*ifcut(iw)
		m = (nfft+1)/2
		n = ltaper(iw)
		do k=1,l-1
			work2(k) = work1(k)*sfft
		end do
		do k=l,nfft
			work2(k) = 0.
		end do	
		if(n.gt.1) st = sfft/n
		ifc = ifcut(iw)
		do k=1,n-1
			ir = 2*(ifc+k) - 2
			ii = ir + 1
			taper = (n-k) * st
			work2(ir) = work1(ir)*taper
			work2(ii) = work1(ii)*taper
		end do
		call rfftb(nfft,work2,wsave)
		do it=1,nt
			tras(it,iw) = work2(it)
		end do
	end do
	return
	end
	

