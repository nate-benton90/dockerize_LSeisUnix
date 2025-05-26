	real dz, dzo
	real fz, fzo
	real zi
	integer nz, nzo

	fz = 0.
	fzo = 0.
	dz = 50.
	dzo = 5.
	nz = 50
	nzo = 590

	do i=1,nzo
		zi = (fzo+(i-1)*dzo-fz)/dz+1.0
		iz = aint(zi)
		write(*,*) 'i=',i,'iz=',iz,'zi=',zi
	end do

	stop
	end

