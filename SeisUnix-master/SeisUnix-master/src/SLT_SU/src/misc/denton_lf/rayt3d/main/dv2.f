 
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
