c  For the following average velocity model,          
c
c               -------------------> va(z)
c               |   + 
c               |
c               |      +
c               |
c               |
c           z(1)|-----   +
c               |
c               |         * 
c               |          *
c               |           *
c               |            *
c               |
c           z(n)|--------     +
c               |
c               |
c            z  v                +
c
c
c estimate the interval velocity model vi(z) = v0 + a (z-z(1)) within 
c [z(1),z(n)], from n data points of [va(i),z(i)], i=1,2,...,n. 
c
c Note: z(1) and va(1) are depth and average velocity at top of the layer.
c       z(n) and va(n) are depth and average velocity at bottom of the layer.
c		
c
c 1. Exact fit at end points z(1) and z(n) are required, when ibfit=1.
c
c Method:
c
c let
c  	 z va(z) - z(1) va(1)         z - z(1)
c    y = -------------------- and x = --------
c              z - z(1)		         2
c
c find v0 and a such that
c
c	y = v0 + a x	will fit exactly at z=z(n) and yet the slope a is the
c average slope determined by n-2 data points.
c
c 2. Least-squared-error fit is performed , when ibfit=0.
c
c Author: 	Zhiming Li		       		10/25/91
c
c example:
c	parameter (n=11,z0=1000.,va0=2000.,zb=2000.,vab=3000.)
c	real z(n),va(n),v0,a
c
c	dz = (zb-z0)/(n-1)
c	dv = (vab-va0)/(n-1)
c	do i=1,n
c		z(i) = z0 + (i-1) * dz
c		va(i) = va0 + (i-1) * dv 
c	end do
c
c	ibfit = 1
c	call estvi (z,va,n,ibfit,v0,a)
c	write(*,*) "CS estimated v0 = ",v0," a = ", a
c
c	ibfit = 0
c	call estvi (z,va,n,ibfit,v0,a)
c	write(*,*) "LS estimated v0 = ",v0," a = ", a
c
c	stop
c	end
ccccccccccccccccc
	subroutine estvi (z,va,n,ibfit,v0,a)
	real z(n),va(n),v0,a
	integer n,ibfit
	real sum
	integer i
	real xi,yi,xb,yb
	real sx,sy,sxx,sxy,del
	if (n.eq.1) then
cccc constant interval velocity
		v0 = va(1)
		a = 0.
	else if (n.eq.2) then
cccc constant interval velocity
		v0 = (z(2)*va(2)-z(1)*va(1))/(z(2)-z(1))
		a = 0.
	else if(ibfit .eq. 1) then
cccc constrained fit at bottom
cccccccccc		write(*,*) "constrained fit"
		xb = 0.5 * ( z(n) - z(1) ) 
		yb = (z(n)*va(n)-z(1)*va(1))/(z(n)-z(1))
		sum = 0.
		do i=2,n-1
			xi = 0.5 * ( z(i) - z(1) ) 
			yi = (z(i)*va(i)-z(1)*va(1))/(z(i)-z(1))
			sum = sum + (yi-yb)/(xi-xb)
		end do
		a = sum/(n-2)
		v0 = yb - a * xb 
cccc least-squared-error fit of n data points
	else
cccccccccc		write(*,*) "least-square fit"
		sx = 0.
		sy = 0.
		sxy = 0.
		sxx = 0.
		do i=2,n
			xi = 0.5 * ( z(i) - z(1) ) 
			yi = (z(i)*va(i)-z(1)*va(1))/(z(i)-z(1))
			sx = sx + xi 
			sy = sy + yi 
			sxy = sxy + xi*yi 
			sxx = sxx + xi*xi 
		end do
		del = (n-1) * sxx - sx * sx
		v0 = (sxx * sy - sx * sxy ) / del 
		a = ((n-1) * sxy - sx * sy) / del
	end if
	return
	end
