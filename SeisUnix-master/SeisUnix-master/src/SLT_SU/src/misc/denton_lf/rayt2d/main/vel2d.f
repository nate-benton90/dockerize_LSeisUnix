* Functions to support interpolation of velocity and its derivatives.
******************************************************************************
*  Functions:
*  vel2Interp	interpolate v(x,z) and its derivatives
******************************************************************************
*  Notes:
*  Linear interpolation is performed respectively for velocity, first 
*  derivatives, and second derivarives.
*****************************************************************************/
 
  	subroutine vel2Interp(nx,nz,fx,fz,dx,dz,
     1    v,vxx,vxz,vzz,x,z,nr,u,ux,uz,uxx,uxz,uzz)
*  Input:
*  x,z		2D coordinate at which to interpolate v(x,y,z) and derivatives
*                ---- array(nr)
*  v		velocity array with dimension (nz,ny,nx) on uniform grids
*  vxx,vxz,vzz	second derivatives on uniform grids
*  nr		number of rays
 
*  Output:  (all are array(nr))
*  u		v(x,z)
*  ux		dv/dx
*  uz		dv/dz
*  uxx		ddv/dxdx 
*  uxz		ddv/dxdz
*  uzz		ddv/dzdz
*****************************************************************************/
 
  	integer	k0,k1,k2,k3  
	real v(*),vxx(*),vxz(*),vzz(*)
	real x(*),z(*),u(*),ux(*),uz(*),uxx(*),uxz(*),uzz(*)
 	real ax,az,sx,sz,sxx,szz,a0,a1,a2,a3 
 	real gx0,gx1,gx2,gx3,g0,g1,g2,g3,gz0,gz1,gz2,gz3									
 
	do 100 ir=1,nr
c	  determine interpolation coefficients  	
	    ax = (x(ir)-fx)/dx 
	    jx = ax 
	    jx = max0(jx,0)
	    jx = min0(jx,nx-2)
 	    sx = ax-jx 
 	    az = (z(ir)-fz)/dz 
	    jz = az 
	    jz = max0(jz,0)
	    jz = min0(jz,nz-2)
 	    sz = az-jz 

	    sxx = 0.5*sx*(1.0-sx)*dx*dx 
 	    szz = 0.5*sz*(1.0-sz)*dz*dz 

	    a0 = (1.0-sx)*(1.0-sz) 
	    a1 = (1.0-sx)*sz 
	    a2 = sx*(1.0-sz) 
	    a3 = sx*sz 
 
c	 set the table of indices for interpolator  
	    k0 = nz*jx+jz+1 
	    k1 = k0+1 
	    k2 = k0+nz 
	    k3 = k2+1 
  
 	    g0 = v(k0) 
 	    g1 = v(k1) 
 	    g2 = v(k2) 
 	    g3 = v(k3) 
 	    gx0 = vxx(k0) 
	    gx1 = vxx(k1) 
	    gx2 = vxx(k2) 
	    gx3 = vxx(k3) 
  	    gz0 = vzz(k0)
	    gz1 = vzz(k1)
	    gz2 = vzz(k2)
	    gz3 = vzz(k3)
  
c	  linear interpolation  
    	    uxx(ir) = a0*gx0+a1*gx1+a2*gx2+a3*gx3 
   	    uxz(ir) = a0*vxz(k0)+a1*vxz(k1)+a2*vxz(k2)+a3*vxz(k3)
    	    uzz(ir) = a0*gz0+a1*gz1+a2*gz2+a3*gz3

	    u(ir) = a0*g0+a1*g1+a2*g2+a3*g3-(sxx*uxx(ir)+szz*uzz(ir)) 

 	    ux(ir) = ((1.0-sz)*(g2-g0-sxx*(gx2-gx0)-szz*(gz2-gz0))
     1		    +sz*(g3-g1-sxx*(gx3-gx1)-szz*(gz3-gz1)))/dx
     2 		+(sx-0.5)*dx*uxx(ir) 
		
	    uz(ir) = ((1.0-sx)*(g1-g0-sxx*(gx1-gx0)-szz*(gz1-gz0))
     1		    +sx*(g3-g2-sxx*(gx3-gx2)-szz*(gz3-gz2)))/dz
     2		+(sz-0.5)*dz*uzz(ir)  
100	end do

	return
	end
		
   
