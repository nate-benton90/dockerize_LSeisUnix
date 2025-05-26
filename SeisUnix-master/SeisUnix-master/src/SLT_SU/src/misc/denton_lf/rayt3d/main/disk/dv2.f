 
	subroutine dv2(nx,ny,nz,dx,dy,dz,v,vxx,vxy,vxz,vyy,vyz,vzz)
c  calculate second DeriVatives from a 3D VELocity grid
c  by finite diffrence
c  
c  input is velocity v 
c  outputs are vxx,vxz,vxz,vyy,vyz and vzz  	 
c
 	integer ix, iy, iz 
 	real   odx, ody, odz, odxx, odyy, odzz, odxy, odxz, odyz 
	real v(nz,ny,nx),vxx(nz,ny,nx),vxy(nz,ny,nx),vxz(nz,ny,nx),
     1		vyy(nz,ny,nx),vyz(nz,ny,nx),vzz(nz,ny,nx)
  
   	
	odx = 1.0/dx 
	ody = 1.0/dy 
	odz = 1.0/dz 
	odxx = odx*odx 
	odyy = ody*ody 
	odzz = odz*odz 
	odxy = 0.25*odx*ody 
	odxz = 0.25*odx*odz 
	odyz = 0.25*ody*odz 

c	initialize
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vxx(iz,iy,ix) = 0. 
		vxy(iz,iy,ix) = 0. 
		vxz(iz,iy,ix) = 0. 
		vyy(iz,iy,ix) = 0. 
		vyz(iz,iy,ix) = 0. 
		vzz(iz,iy,ix) = 0. 
 10	end do

 	
c	finite difference 
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
 		vxx(iz,iy,ix) = odxx*(v(iz,iy,ix+1)-2.0*v(iz,iy,ix)
     1			+v(iz,iy,ix-1)) 
		vxy(iz,iy,ix) = odxy*(v(iz,iy+1,ix+1)-v(iz,iy-1,ix+1)
     1				 -v(iz,iy+1,ix-1)+v(iz,iy-1,ix-1)) 
		vxz(iz,iy,ix) = odxz*(v(iz+1,iy,ix+1)-v(iz-1,iy,ix+1) 
     1				 -v(iz+1,iy,ix-1)+v(iz-1,iy,ix-1)) 
		vyy(iz,iy,ix) = odyy*(v(iz,iy+1,ix)-2.0*v(iz,iy,ix)
     1			+v(iz,iy-1,ix)) 
		vyz(iz,iy,ix) = odyz*(v(iz+1,iy+1,ix)-v(iz-1,iy+1,ix)
     1				 -v(iz+1,iy-1,ix)+v(iz-1,iy-1,ix)) 
     		vzz(iz,iy,ix) = odzz*(v(iz+1,iy,ix)-2.0*v(iz,iy,ix)
     1			+v(iz-1,iy,ix))
30	end do
	return 
	end

	subroutine dv2xx(nx,ny,nz,dx,dy,dz,v,vxx)
 	integer ix, iy, iz 
 	real odx, odxx
	real v(nz,ny,nx),vxx(nz,ny,nx)

	odx = 1.0/dx 
	odxx = odx*odx 
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vxx(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
 		vxx(iz,iy,ix) = odxx*(v(iz,iy,ix+1)-2.0*v(iz,iy,ix)
     1			+v(iz,iy,ix-1)) 
30	end do
	return
	end

	subroutine dv2xy(nx,ny,nz,dx,dy,dz,v,vxy)
 	integer ix, iy, iz 
 	real odx, ody, odxy
	real v(nz,ny,nx),vxy(nz,ny,nx)
   	
	odx = 1.0/dx 
	ody = 1.0/dy 
	odxy = 0.25*odx*ody 
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vxy(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
		vxy(iz,iy,ix) = odxy*(v(iz,iy+1,ix+1)-v(iz,iy-1,ix+1)
     1				 -v(iz,iy+1,ix-1)+v(iz,iy-1,ix-1)) 
30	end do
	return
	end

	subroutine dv2xz(nx,ny,nz,dx,dy,dz,v,vxz)
 	integer ix, iy, iz 
 	real odx, odz, odxz 
	real v(nz,ny,nx),vxz(nz,ny,nx)
  
	odx = 1.0/dx 
	odz = 1.0/dz 
	odxz = 0.25*odx*odz
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vxz(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
		vxz(iz,iy,ix) = odxz*(v(iz+1,iy,ix+1)-v(iz-1,iy,ix+1) 
     1				 -v(iz+1,iy,ix-1)+v(iz-1,iy,ix-1)) 
30	end do
	return
	end

	subroutine dv2yy(nx,ny,nz,dx,dy,dz,v,vyy)
 	integer ix, iy, iz 
 	real   ody, odyy 
	real v(nz,ny,nx),vyy(nz,ny,nx)
  
	ody = 1.0/dy 
	odyy = ody*ody
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vyy(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
		vyy(iz,iy,ix) = odyy*(v(iz,iy+1,ix)-2.0*v(iz,iy,ix)
     1			+v(iz,iy-1,ix)) 
30	end do
	return
	end

	subroutine dv2yz(nx,ny,nz,dx,dy,dz,v,vyz)
 	integer ix, iy, iz 
 	real ody, odz, odyz 
	real v(nz,ny,nx),vyz(nz,ny,nx)
  
	ody = 1.0/dy 
	odz = 1.0/dz 
	odyz = 0.25*ody*odz
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vyz(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
		vyz(iz,iy,ix) = odyz*(v(iz+1,iy+1,ix)-v(iz-1,iy+1,ix)
     1				 -v(iz+1,iy-1,ix)+v(iz-1,iy-1,ix)) 
30	end do
	return
	end

	subroutine dv2zz(nx,ny,nz,dx,dy,dz,v,vzz)
 	integer ix, iy, iz 
 	real odz, odzz 
	real v(nz,ny,nx),vzz(nz,ny,nx)
  
	odz = 1.0/dz 
	odzz = odz*odz
	do 10 ix=1,nx   
	do 10 iy=1,ny     
 	do 10 iz=1,nz
 		vzz(iz,iy,ix) = 0. 
 10	end do
	do 30 ix=2,nx-1   
	do 30 iy=2,ny-1     
 	do 30 iz=2,nz-1
     		vzz(iz,iy,ix) = odzz*(v(iz+1,iy,ix)-2.0*v(iz,iy,ix)
     1			+v(iz-1,iy,ix))
30	end do
	return
	end
