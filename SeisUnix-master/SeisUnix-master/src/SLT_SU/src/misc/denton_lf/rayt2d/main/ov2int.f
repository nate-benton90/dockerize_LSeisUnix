 
* interpolate a 2D velocity and calculate its slowness squares  *
	subroutine ov2int(v,nx,nz,fx,fz,dx,dz,ov2,
     1	 	nxo,nzo,fxo,fzo,dxo,dzo)
c  v	   input velocity
c  ov2	   output slowness squares
c  input grid parameters are nx,nz,fx,fz,dx,dz
c  output grid parameters are nxo,nzo,fxo,fzo,dxo,dzo

  	integer jx, jz, ixo, izo 
 	real   odx, odz, x, z, ax, az, sx, sz, temp
	real v(nz,nx),ov2(nzo,nxo)
  
   	
	odx = 1.0/dx 
 	odz = 1.0/dz 
 
 	do 10 ixo=1,nxo
		x = fxo+ixo*dxo
		ax = 1.0+(x-fx)*odx 
		jx = ax 
 		jx = max0(jx,1) 
 		jx = min0(jx,nx-1)
		sx = ax-jx 

  		do 30 izo=1,nzo
		    z = fzo+(izo-1)*dzo
  		    az = 1.0+(z-fz)*odz 
	 	    jz = az 
 		    jz = max0(jz,1)
		    jz = min0(jz,nz-1) 
 		    sz = az-jz 
 
  		    temp = (1.0-sx)*((1.0-sz)*v(jz,jx)+sz*v(jz+1,jx))
     1			+sx*((1.0-sz)*v(jz,jx+1)+sz*v(jz+1,jx+1))
									
		    ov2(izo,ixo) = 1.0/(temp*temp) 
30		end do
10	end do

	return
	end

*************************************************************************
 	subroutine eiknl(t, ov2, tt1, tt2, nx, nz, dx, dz, tmax)
c  compute the traveltime at shadow zone by solving eikonal equation
c input:
c  t	 traveltime from ray tracing 
c  ov2	 input slowness squares
c  tt1 & tt2	two work arrays
c  tmax  upper limit of normal traveltime 
c output:
c  t	 traveltime (unchanged if t<=tmax; computed if t>tmax) 
c
 
	integer ix, iz  
	real tx2, tz2, t0, tl, tr, dtl2, dtr2, odx2
	real t(nz,nx), ov2(nz,nx), tt1(nx), tt2(nx)

	odx2 = 1.0/(dx*dx)
  		
	do 5 ix = 1,nx		
 		tt1(ix) = t(1,ix)
5		continue
	do 10 iz = 2,nz 
		do 15 ix = 1,nx		
 			tt2(ix) = t(iz,ix)
15		continue
 
  	    do 20 ix = 1,nx 
c 	if traveltime is abnormal the and upper is normal
		    t0 = tt1(ix)  
    		    if (tt2(ix) .gt. tmax .and. t0 .le. tmax) then

			tl = 0.0
			tr = 0.0 
	 		if(ix .gt. 1) tl = tt1(ix-1) 
			if(ix .lt. nx) tr = tt1(ix+1) 
			dtl2 = (tl-t0)*(tl-t0) 
			dtr2 = (tr-t0)*(tr-t0) 	
  			tx2 = min(dtl2,dtr2)*odx2  
		
  			tz2 = ov2(iz-1,ix)-tx2
  			if(tz2 .ge. 0.0) then
  				tt2(ix) = t0+dz*sqrt(tz2)
 			end if  
     		  end if
20	    continue
		do 25 ix = 1,nx		
 			t(iz,ix) = tt2(ix) 
			tt1(ix) = tt2(ix)
25		continue 
10 	continue

	return
	end
 

