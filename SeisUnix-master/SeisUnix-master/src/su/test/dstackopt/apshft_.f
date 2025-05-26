	subroutine apshft(nt,ppd,ph,apq)
	integer nt,appd,ppd,aph,ph,apq
	appd=ppd
	aph=ph
	call vsmul(appd,1,aph,apq,1,nt)
	appd = appd - 1
	aph  = aph + 1
	call vsma(appd,1,aph,apq,1,apq,1,nt)
	return
	end
