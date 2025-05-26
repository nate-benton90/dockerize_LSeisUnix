 	subroutine ov2int(v,nx,ny,nz,fx,fy,fz,dx,dy,dz,ov2,
     1	 	nxo,nyo,nzo,fxo,fyo,fzo,dxo,dyo,dzo)
c  interpolate a 3D velocity and calculate its slowness squares  
c
c  v	   input velocity
c  ov2	   output slowness squares
c  input grid parameters are nx,ny,nz,fx,fy,fz,dx,dy,dz
c  output grid parameters are nxo,nyo,nzo,fxo,fyo,fzo,dxo,dyo,dzo

  	integer jx, jy, jz, ixo, iyo, izo 
 	real   odx, ody, odz, x, y, z, ax, ay, az, sx, sy, sz, temp
	real v(nz,ny,nx),ov2(nzo,nyo,nxo)
  
   	
	odx = 1.0/dx 
	ody = 1.0/dy 
	odz = 1.0/dz 
 
 	do 10 ixo=1,nxo
		x = fxo+(ixo-1)*dxo
		ax = 1.0+(x-fx)*odx 
		jx = ax 
 		jx = max0(jx,1) 
 		jx = min0(jx,nx-1)
		sx = ax-jx 

 	    do 20 iyo=1,nyo
		y = fyo+(iyo-1)*dyo
 		ay = 1.0+(y-fy)*ody 
		jy = ay
 		jy = max0(jy,1) 
		jy = min0(jy,ny-1)
 		sy = ay-jy
 
 		do 30 izo=1,nzo
		    z = fzo+(izo-1)*dzo
  		    az = 1.0+(z-fz)*odz 
	 	    jz = az 
 		    jz = max0(jz,1)
		    jz = min0(jz,nz-1) 
 		    sz = az-jz 
 
  		    temp = (1.0-sx)*((1.0-sy)*((1.0-sz)*v(jz,jy,jx)
     1						+sz*v(jz+1,jy,jx))
     2					+sy*((1.0-sz)*v(jz,jy+1,jx) 
     3						+sz*v(jz+1,jy+1,jx)))
     4   	      +sx*((1.0-sy)*((1.0-sz)*v(jz,jy,jx+1)
     5					+sz*v(jz+1,jy,jx+1))
     6				+sy*((1.0-sz)*v(jz,jy+1,jx+1)
     7					+sz*v(jz+1,jy+1,jx+1))) 
									
		    ov2(izo,iyo,ixo) = 1.0/(temp*temp) 
30		end do
20	    end do
10	end do

	return
	end


*************************************************************************
 	subroutine eiknl(t, ov2, tt1, tt2, nx, ny, nz, dx, dy, dz, tmax)
c  compute the traveltime at shadow zone by solving eikonal equation
c input:
c  t	 traveltime from ray tracing 
c  ov2	 input slowness squares
c  tt1 & tt2	two work arrays
c  tmax  upper limit of normal traveltime 
c output:
c  t	 traveltime (unchanged if t<=tmax; computed if t>tmax) 
c

	integer ix,iy,iz  
	real tx2, ty2, tz2, t0, tl, tr, dtl2, dtr2, odx2, ody2
	real t(nz,ny,nx), ov2(nz,ny,nx), tt1(ny,nx), tt2(ny,nx)

	odx2 = 1.0/(dx*dx)
	ody2 = 1.0/(dy*dy)
 		
	do 5 ix = 1,nx		
	do 5 iy = 1,ny
 		tt1(iy,ix) = t(1,iy,ix)
5		continue
	do 10 iz = 2,nz 
		do 15 ix = 1,nx		
		do 15 iy = 1,ny
 			tt2(iy,ix) = t(iz,iy,ix)
15		continue
 
  	    do 20 ix = 1,nx 
   	    	do 30 iy = 1,ny
c 	if traveltime is abnormal the and its upper is normal
		    t0 = tt1(iy,ix)  
    		    if (tt2(iy,ix) .gt. tmax .and. t0 .le. tmax) then

			tl = 0.0
			tr = 0.0						  
	 		if(ix .gt. 1) tl =  tt1(iy,ix-1) 
			if(ix .lt. nx) tr =  tt1(iy,ix+1) 
			dtl2 = (tl-t0)*(tl-t0) 
			dtr2 = (tr-t0)*(tr-t0) 	
  			tx2 = min(dtl2,dtr2)*odx2  
		
			tl = 0.0
			tr = 0.0						  
	 		if(iy .gt. 1) tl = tt1(iy-1,ix) 
			if(iy .lt. ny) tr = tt1(iy+1,ix) 
			dtl2 = (tl-t0)*(tl-t0) 
			dtr2 = (tr-t0)*(tr-t0) 	
  			ty2 = min(dtl2,dtr2)*ody2	 

 			tz2 = ov2(iz-1,iy,ix)-tx2-ty2
  			if(tz2 .gt. 0.0) then
  				tt2(iy,ix) = t0+dz*sqrt(tz2)
 			end if  
     		  end if
30 		continue
20	    continue
	    do 25 ix = 1,nx		
	    do 25 iy = 1,ny
 		t(iz,iy,ix) = tt2(iy,ix) 
		tt1(iy,ix) = tt2(iy,ix)
25	    continue 
10 	continue

	return
	end
 

