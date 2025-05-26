  
   	
 
	subroutine dv2(nx,nz,dx,dz,v,vxx,vxz,vzz)
c  calculate second DeriVatives from a 2D Velocity grid by finite difference	 c  input is velocity v 
c  outputs are vxx,vxz,vxz,vyy,vyz and vzz  	 
c


 	integer ix,  iz 
 	real   odx, odz, odxx, odzz, odxz 
	real v(nz,nx),vxx(nz,nx),vxz(nz,nx),vzz(nz,nx)
  
   	
	odx = 1.0/dx 
 	odz = 1.0/dz 
	odxx = odx*odx 
 	odzz = odz*odz 
 	odxz = 0.25*odx*odz 
 
c	initialize
	do 10 ix=1,nx   
 		do 20 iz=1,nz
 			vxx(iz,ix) = 0. 
 			vxz(iz,ix) = 0. 
 			vzz(iz,ix) = 0. 
20		end do
10	end do

 	
c	 finite difference 
	do 30 ix=2,nx-2   
 	    do 40 iz=2,nz-1
 		vxx(iz,ix) = odxx*(v(iz,ix+1)-2.0*v(iz,ix)+v(iz,ix-1)) 
 		vxz(iz,ix) = odxz*(v(iz+1,ix+1)-v(iz-1,ix+1) 
     1				 -v(iz+1,ix-1)+v(iz-1,ix-1)) 
     		vzz(iz,ix) = odzz*(v(iz+1,ix)-2.0*v(iz,ix)+v(iz-1,ix))
40	    end do
30	end do
 
	return 
	end    
 

***	translate  ***************************
 	subroutine trans(nx,nz,nxt,nx0,v,vt)  

	integer i,i0,ix,iz 
 	real v(nz,nx),vt(nz,nx)  	

 	do 10 ix=1,nxt
  		do 20 iz=1,nz
     		    vt(iz,ix) = v(iz,ix+nx0)
20		end do
10	end do
 
	return
	end

 
