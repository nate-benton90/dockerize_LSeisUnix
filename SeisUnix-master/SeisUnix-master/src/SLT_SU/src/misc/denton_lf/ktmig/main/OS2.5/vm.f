

	subroutine vr22vi2(vr2,vi2,ntau)
	real vr2(ntau),vi2(ntau)
	integer ntau,i
	vi2(1) = vr2(1)
	do i=2,ntau
		vi2(i) = i*vr2(i)-(i-1)*vr2(i-1)
	end do
	return
	end

	subroutine vi22vq4(vi2,vq4,ntau)
	real vi2(ntau),vq4(ntau)
	integer ntau,i
	vq4(1) = vi2(1)*vi2(1)
	do i=2,ntau
		vq4(i) = vq4(i-1) + vi2(i)*vi2(i)
	end do
	do i=2,ntau
		vq4(i) = vq4(i)/i
	end do
	return
	end

	subroutine vt2s2(vr2,vq4,s2,tau,ntau) 
	real vr2(ntau),vq4(ntau),s2(ntau),tau(ntau)
	real vr4
	integer i, ntau
	do i=2,ntau
		vr4 = vr2(i)*vr2(i)
		s2(i) = (vr4-vq4(i))/(4.*tau(i)*tau(i)*vr4*vr2(i))
	end do
	if(ntau.gt.1) then
		s2(1) = s2(2)
	else 
		s2(1) = 0.
	end if
	return
	end

	subroutine sx2sc(s2,x2,sc,ntau)
	real s2(ntau),x2,sc(ntau)
	integer i, ntau
	do i=2,ntau
		sc(i) = 1. - s2(i)*x2
	end do
	if(ntau.gt.1) then
		sc(1) = sc(2)
	else 
		sc(1) = 0.
	end if
	return
	end 

