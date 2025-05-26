******************************************************************************
*  Functions
*  vel3Interp	interpolate velocity and its derivatives, vectorized for
*  multiple rays.

******************************************************************************
*  Notes:
*  Linear interpolation is performed respectively for velocity, first 
*  derivatives, and second derivarives.
*****************************************************************************/
 
  	subroutine vel3Interp(nx,ny,nz,fx,fy,fz,dx,dy,dz,
     1    v,vxx,vxy,vxz,vyy,vyz,vzz,x,y,z,nr,
     2	  u,ux,uy,uz,uxx,uxy,uxz,uyy,uyz,uzz)
*  Input:
*  x,y,z	3D coordinate at which to interpolate v(x,y,z) and derivatives
*                ---- array(nr)
*  v	velocity array with dimension (nz,ny,nx) on uniform grids
*  vxx,vxy,vxz,vyy,vyz,vzz	second derivatives on uniform grids
*  nr	number of rays
 
*  Output:  (all are array(nr))
*  u		v(x,y,z)
*  ux		dv/dx
*  uz		dv/dz
*  uxx		ddv/dxdx 
*  uxy		ddv/dxdy
*  uxz		ddv/dxdz
*  uyy		ddv/dydy 
*  uyz		ddv/dydz
*  uzz		ddv/dzdz
*****************************************************************************/
 
  	integer	k0,k1,k2,k3,k4,k5,k6,k7 
	real v(*),vxx(*),vxy(*),vxz(*),vyy(*),vyz(*),vzz(*)
	real x(*),y(*),z(*),u(*),ux(*),uy(*),uz(*)
	real uxx(*),uxy(*),uxz(*),uyy(*),uyz(*),uzz(*)
 	real ax,ay,az,sx,sy,sz,sxx,syy,szz,a0,a1,a2,a3,a4,a5,a6,a7
 	real gx0,gx1,gx2,gx3,gx4,gx5,gx6,gx7,g0,g1,g2,g3,g4,g5,g6,g7,
     1 	 gy0,gy1,gy2,gy3,gy4,gy5,gy6,gy7,gz0,gz1,gz2,gz3,gz4,gz5,gz6,gz7									
 
	odx = 1.0/dx
	ody = 1.0/dy
	odz = 1.0/dz

	do 100 ir=1,nr
c	  determine interpolation coefficients  	
	    ax = (x(ir)-fx)*odx 
	    jx = ax 
	    jx = max0(jx,0)
	    jx = min0(jx,nx-2)
 	    sx = ax-jx 
	    ay = (y(ir)-fy)*ody 
	    jy = ay 
	    jy = max0(jy,0) 
	    jy = min0(jy,ny-2)
 	    sy = ay-jy 
	    az = (z(ir)-fz)*odz 
	    jz = az 
	    jz = max0(jz,0)
	    jz = min0(jz,nz-2)
 	    sz = az-jz 

	    sxx = 0.5*sx*(1.0-sx)*dx*dx 
	    syy = 0.5*sy*(1.0-sy)*dy*dy 
	    szz = 0.5*sz*(1.0-sz)*dz*dz 

	    a0 = (1.0-sx)*(1.0-sy)*(1.0-sz) 
	    a1 = (1.0-sx)*(1.0-sy)*sz 
	    a2 = (1.0-sx)*sy*(1.0-sz) 
	    a3 = (1.0-sx)*sy*sz 
	    a4 = sx*(1.0-sy)*(1.0-sz) 
	    a5 = sx*(1.0-sy)*sz 
	    a6 = sx*sy*(1.0-sz) 
	    a7 = sx*sy*sz 

c	 set the table of indices for interpolator  
	    k0 = ny*nz*jx+nz*jy+jz+1 
	    k1 = k0+1 
	    k2 = k0+nz 
	    k3 = k2+1 
	    k4 = k0+nz*ny 
	    k5 = k4+1 
	    k6 = k4+nz 
	    k7 = k6+1 
 
 	    g0 = v(k0) 
 	    g1 = v(k1) 
 	    g2 = v(k2) 
 	    g3 = v(k3) 
 	    g4 = v(k4) 
 	    g5 = v(k5) 
 	    g6 = v(k6) 
 	    g7 = v(k7) 
	    gx0 = vxx(k0) 
	    gx1 = vxx(k1) 
	    gx2 = vxx(k2) 
	    gx3 = vxx(k3) 
	    gx4 = vxx(k4) 
	    gx5 = vxx(k5) 
	    gx6 = vxx(k6) 
	    gx7 = vxx(k7) 
	    gy0 = vyy(k0) 
	    gy1 = vyy(k1) 
	    gy2 = vyy(k2) 
	    gy3 = vyy(k3) 
	    gy4 = vyy(k4) 
	    gy5 = vyy(k5) 
	    gy6 = vyy(k6) 
	    gy7 = vyy(k7) 
	    gz0 = vzz(k0)
	    gz1 = vzz(k1)
	    gz2 = vzz(k2)
	    gz3 = vzz(k3)
	    gz4 = vzz(k4)
	    gz5 = vzz(k5)
	    gz6 = vzz(k6)
	    gz7 = vzz(k7)
 
c	  linear interpolation  
    	    uxx(ir) = a0*gx0+a1*gx1+a2*gx2+a3*gx3+
     1	 	a4*gx4+a5*gx5+a6*gx6+a7*gx7
  	    uxy(ir) = a0*vxy(k0)+a1*vxy(k1)+a2*vxy(k2)+a3*vxy(k3)+
     1	 	a4*vxy(k4)+a5*vxy(k5)+a6*vxy(k6)+a7*vxy(k7) 
  	    uxz(ir) = a0*vxz(k0)+a1*vxz(k1)+a2*vxz(k2)+a3*vxz(k3)+
     1	 	a4*vxz(k4)+a5*vxz(k5)+a6*vxz(k6)+a7*vxz(k7)
   	    uyy(ir) = a0*gy0+a1*gy1+a2*gy2+a3*gy3+
     1	 	a4*gy4+a5*gy5+a6*gy6+a7*gy7
  	    uyz(ir) = a0*vyz(k0)+a1*vyz(k1)+a2*vyz(k2)+a3*vyz(k3)+
     1	 	a4*vyz(k4)+a5*vyz(k5)+a6*vyz(k6)+a7*vyz(k7)
   	    uzz(ir) = a0*gz0+a1*gz1+a2*gz2+a3*gz3+
     1	 	a4*gz4+a5*gz5+a6*gz6+a7*gz7

	    u(ir) = a0*g0+a1*g1+a2*g2+a3*g3+a4*g4+a5*g5+a6*g6+a7*g7
     1		-(sxx*uxx(ir)+syy*uyy(ir)+szz*uzz(ir)) 

	    ux(ir) = ((1.0-sy)*((1.0-sz)*(g4-g0-sxx*(gx4-gx0)
     1			-syy*(gy4-gy0)-szz*(gz4-gz0))
     2		    +sz*(g5-g1-sxx*(gx5-gx1)
     3			-syy*(gy5-gy1)-szz*(gz5-gz1)))
     4		+sy*((1.0-sz)*(g6-g2-sxx*(gx6-gx2)
     5			-syy*(gy6-gy2)-szz*(gz6-gz2))
     6		    +sz*(g7-g3-sxx*(gx7-gx3)
     7 			-syy*(gy7-gy3)-szz*(gz7-gz3))))*odx
     8		+(sx-0.5)*dx*uxx(ir) 		

	    uy(ir) = ((1.0-sx)*((1.0-sz)*(g2-g0-sxx*(gx2-gx0)
     1			-syy*(gy2-gy0)-szz*(gz2-gz0))
     2		    +sz*(g3-g1-sxx*(gx3-gx1)
     3			-syy*(gy3-gy1)-szz*(gz3-gz1)))
     4		+sx*((1.0-sz)*(g6-g4-sxx*(gx6-gx4)
     5			-syy*(gy6-gy4)-szz*(gz6-gz4))
     6		    +sz*(g7-g5-sxx*(gx7-gx5)
     7			-syy*(gy7-gy5)-szz*(gz7-gz5))))*ody
     8		+(sy-0.5)*dy*uyy(ir) 
		
	    uz(ir) = ((1.0-sx)*((1.0-sy)*(g1-g0-sxx*(gx1-gx0)
     1			-syy*(gy1-gy0)-szz*(gz1-gz0))
     2		    +sy*(g3-g2-sxx*(gx3-gx2)
     3			-syy*(gy3-gy2)-szz*(gz3-gz2)))
     4		+sx*((1.0-sy)*(g5-g4-sxx*(gx5-gx4)
     5			-syy*(gy5-gy4)-szz*(gz5-gz4))
     6		    +sy*(g7-g6-sxx*(gx7-gx6)
     7			-syy*(gy7-gy6)-szz*(gz7-gz6))))*odz
     8		+(sz-0.5)*dz*uzz(ir)  
100	end do

	return
	end
		
   
