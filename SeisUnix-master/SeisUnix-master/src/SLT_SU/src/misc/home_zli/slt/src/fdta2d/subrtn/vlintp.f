cccc vector linear interpolation 
c
c       t1   ---  input array (nz,nx1)
c       nz   ---  number of elements per vector
c       nx1  ---  number of input vectors
c       dx1  ---  sampling interval of input vectors
c       x01  ---  first input vector coordinate
c       t2   ---  comuted output array (nz,nx2)
c       nx2  ---  number of output vectors
c       dx1  ---  sampling interval of output vectors
c       x02  ---  first output vector coordinate
c       
	subroutine vlintp (t1,nz,nx1,dx1,x01,t2,nx2,dx2,x02)
	real t1(nz,nx1),t2(nz,nx2),dx1,x01,dx2,x02
	integer nz,nx1,nx2
	do ix=1,nx2
	   x = (x02 + (ix-1)*dx2 - x01 )/dx1 + 1.
	   ix1 = x
	   if ( ix1 .lt. 1 ) ix1 = 1
	   if ( ix1 .ge. nx1) ix1 = nx1-1
	   res = x - ix1
	      do iz=1,nz
	         t2(iz,ix) = t1(iz,ix1) + res*(t1(iz,ix1+1)-t1(iz,ix1))
	      end do
	end do
	return
	end
