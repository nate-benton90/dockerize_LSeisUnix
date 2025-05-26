  
   
  
 
c**  3D residual traveltime calculation based on reference time **
	subroutine resit(t,nx,ny,nz,nz0,nr,dx,dy,dr,fx,fy,x0,y0,tb)
c
c Input:
c   t(nz,nx,ny) traveltime before implementation of residual
c   tb(nz,nr) reference traveltime
c Output:
c   t(nz,nx,ny) residual traveltime  

	integer ix,iy,iz,jr  
	real xi,yi,ar,sr,sr1 
	real t(nz,nx,ny),tb(nz,nr)  		
	real odr
  
   
	odr = 1./dr
 	do 10 iy = 1,ny
  	    do 20 ix = 1,nx 
		yi = fy+(iy-1)*dy-y0
		xi = fx+(ix-1)*dx-x0
     		ar = sqrt(xi*xi+yi*yi)*odr 
		jr = 1+int(ar) 
 		jr = min0(nr-1,jr)
		sr = 1.0+ar-float(jr) 
 		sr1 = 1.0-sr 
ccccccc$DIR FORCE_VECTOR
    	    	do 30 iz = 1,nz
30    		  t(iz,ix,iy)=t(iz,ix,iy)-sr1*tb(iz,jr)-sr*tb(iz,jr+1)          
20	    continue
10 	continue

	do iy=1,ny
	do ix=1,nx
	do iz=1,nz0
		if(t(iz,ix,iy).gt.100.) t(iz,ix,iy) = 0.
	end do
	end do
	end do 
 
	return
	end 	

**  2D residual traveltime calculation based on reference time **
	subroutine resit0(t,nout,nz,nz0,nr,dr,x,y,x0,y0,tb)
c
c Input:
c   t(nz,nout) traveltime before implementation of residual
c   tb(nz,nr) reference traveltime
c Output:
c   t(nz,nout) residual traveltime  

	integer iz,jr,iout,nz1,nz2 
	real xi,yi,ar,sr,sr1 
	real t(nz,nout),tb(nz,nr),x(nout),y(nout)  		
	real odr
  
   
	odr = 1/dr
 	do 10 iout = 1,nout
 		yi = y(iout)-y0
		xi = x(iout)-x0
     		ar = sqrt(xi*xi+yi*yi)*odr 
		jr = 1+int(ar) 
 		jr = min0(nr-1,jr)
		sr = 1.0+ar-float(jr) 
 		sr1 = 1.0-sr
  	    	do 30 iz = 1,nz
30    		    t(iz,iout) = t(iz,iout)
     1				-sr1*tb(iz,jr)-sr*tb(iz,jr)  
10 	continue
			
	do iout=1,nout
		do iz=1,nz0
			if(t(iz,iout).gt.100.) t(iz,iout) = 0.
		end do
	end do
 
	return
	end
  		
   		
c**  lateral interpolation and vertical shift ***********************
c
c   interpolation within a constant source travel time table
c     interpolated from coarse grid to output fine grid
c
	subroutine latint(nx,ny,nz,x,y,t,n1,n2,nzt,n0,f1,f2,d1,d2,tt)
c
c Input:
c   tt(nzt,n1,n2) traveltime tabel for interpolation  
c   f1,f2,d1,d2,n1,n2,nzt    define tt grid  
c   n0  vertical shift (iz=izt+n0)               
c   x,y   migration output lateral position 
c Output:
c   t(nz,nx,ny) interpolated traveltime  
c
									
	integer ix,iy,iz,jx,jy 
	real od1,od2,ay,sy,ax,sx,w0,w1,w2,w3 
	real t(nz,nx,ny),tt(nzt,n1,n2),x(nx,ny),y(nx,ny) 		
	real ratio,xt,res
	integer ixt
  
	if(d2 .gt. 999990.) then
	    if(nx.eq.n1) then
c	    no lateral interpolation  
   	    	do ix = 1,nx 
   	    		do iz = 1,nz
     		    		t(iz,ix,1) = tt(iz+n0,ix,1)
  			end do
	    	end do
	    else 
		ratio = (n1-1.)/(nx-1.) 
   	    	do ix = 1,nx 
			xt = (ix-1)*ratio + 1.
			ixt = xt
			res = xt - ixt 
			if(ixt.ge.n1) then
				ixt = n1 - 1 
				res = 1.
			end if
   	    		do iz = 1,nz
     		    		t(iz,ix,1) = tt(iz+n0,ixt,1)*(1.-res)
     1						+res*tt(iz+n0,ixt+1,1)
  			end do
	    	end do
	    end if
	    return
	end if  

	od1 = 1.0/d1
	od2 = 1.0/d2

	if(n2 .eq. 1) then 
c	    no y-derection interpolation  
   	    do 10 ix = 1,nx 
 		ax = (x(ix,1)-f1)*od1
 		jx = 1+int(ax)
		jx = min0(jx,n1-1)
		sx = 1.0+ax-jx 
		if(sx .le. 0.01) sx = 0.
		if(sx .ge. 0.99) sx = 1.0
		w0 = (1.0-sx) 
		w1 = sx 
c$DIR FORCE_VECTOR
    	    	do  iz = 1,nz
     		  t(iz,ix,1) = w0*tt(iz+n0,jx,1)+w1*tt(iz+n0,jx+1,1)
  		end do  
10	    continue
 
	else if (n1. eq. 1) then
c	    no x-derection interpolation  
 	    do 20 iy = 1,ny
 		ay = (y(1,iy)-f2)*od2
 		jy = 1+int(ay)
		jy = min0(jy,n2-1)
		sy = 1.0+ay-jy 
		if(sy .le. 0.01) sy = 0.
		if(sy .ge. 0.99) sy = 1.0
 		w0 = (1.0-sy)
 		w2 = sy
c$DIR FORCE_VECTOR
    	    	do iz = 1,nz
     		  t(iz,1,iy) = w0*tt(iz+n0,1,jy)+w2*tt(iz+n0,1,jy+1)
  		end do  
20	    continue
	
	else
    	    do 30 iy = 1,ny
  	    do 30 ix = 1,nx 
		ay = (y(ix,iy)-f2)*od2
 		jy = 1+int(ay)
		jy = min0(jy,n2-1)
		sy = 1.0+ay-jy 
		if(sy .le. 0.01) sy = 0.
		if(sy .ge. 0.99) sy = 1.0
		ax = (x(ix,iy)-f1)*od1
 		jx = 1+int(ax)
		jx = min0(jx,n1-1)
		sx = 1.0+ax-jx 
		if(sx .le. 0.01) sx = 0.
		if(sx .ge. 0.99) sx = 1.0
		w0 = (1.0-sx)*(1.0-sy)
		w1 = sx*(1.0-sy)
		w2 = (1.0-sx)*sy
		w3 = sx*sy
c$DIR FORCE_VECTOR
   	    	do  iz = 1,nz
     		  t(iz,ix,iy) = w0*tt(iz+n0,jx,jy)+w1*tt(iz+n0,jx+1,jy)
     1     		+w2*tt(iz+n0,jx,jy+1)+w3*tt(iz+n0,jx+1,jy+1)
  		end do  
30	    continue
	end if
 
	return
	end 	


***	bilinear interpolation  ***************************
c
c  for interpolation of tables, i.e., given 4 tables, it interpolates
c  to output a new table for the new source location 
c
 	subroutine blinint(nx,ny,nz,sx,sy,t0,t1,t2,t3,t)  
c
c Input:
c   t0,t1,t2,t3(nz,nx,ny)
c	 	traveltime tabels at 4 source locations 
c   sx,sy       relative position of new source to the source of t0
c               sx=(x-x0)/(x1-x0)
c               sy=(y-y0)/(y2-y0)
c               where (xi,yi) are x,y of table ti
c               (x,y) is the new source location
c               t0 is at min x and min y location
c               t1 is at max x and min y location
c               t2 is at min x and max y location
c               t3 is at max x and max y location
c Output:
c   t(nz,nx,ny) interpolated traveltime  

	integer ix,iy,iz 
 	real t0(nz,nx,ny),t1(nz,nx,ny),t2(nz,nx,ny),t3(nz,nx,ny)
	real t(nz,nx,ny),w0,w1,w2,w3
  	

	w0 = (1.0-sx)*(1.0-sy)
	w1 = sx*(1.0-sy)
	w2 = (1.0-sx)*sy
	w3 = sx*sy
 
 	do 10 iy=1,ny  
	do 10 ix=1,nx
  		do 20 iz=1,nz
     		    t(iz,ix,iy) = w0*t0(iz,ix,iy)+w1*t1(iz,ix,iy)
     1				+w2*t2(iz,ix,iy)+w3*t3(iz,ix,iy)
20		end do
10	end do
 
	return
	end


***** calculate reference traveltime and horizontal slowness	******
	subroutine timeb(nr,nz,dr,dz,fz,a,v0,t,p)
c
c Input:
c   v0		velocity at the surface
c   a		velocity vertical-gradient  
c Output:
c   t(nz,nr)  	traveltime based on velocity, v+az 
c   p(nz,nr)  	lateral slowness based on velocity, v+az 
 
 	integer ir, iz 
 	real   r,z,zc,v,rc,oa,ov0,rou,t(nz,nr),p(nz,nr) 
	real or2
   
   	ov0 = 1.0/v0

	if(a .eq. 0.0) then
c       when r=0
	    do 10 iz=1,nz
	    	z = (iz-1)*dz+fz
	    	t(iz,1) = z*ov0
		p(iz,1) = 0.0
10	    end do

c   	when r>0
	    do 30 ir=2,nr
		r = (ir-1)*dr
   		do 40 iz=1,nz
			z = (iz-1)*dz+fz  
 			rou = sqrt(r*r+z*z) 
			t(iz,ir) = rou*ov0 
			p(iz,ir) = r/(rou*v0)
40	 	end do
30	    end do
	    return
	end if

c   a is not 0
	oa = 1.0/a
	zc = v0*oa
c       when r=0
	do 110 iz=1,nz
	    	z = (iz-1)*dz+fz
		v = v0+a*z
	    	t(iz,1) = log(v*ov0)*oa
		p(iz,1) = 0.0
110	    end do
 
c   when r>0
	do 130 ir=2,nr 
		r = (ir-1)*dr
		or2 = 0.5/r
   		do 140 iz=1,nz
			z = zc+(iz-1)*dz+fz  
  			v = v0+a*(z-zc) 
  			rc = (r*r+z*z-zc*zc)*or2 
 			rou = sqrt(zc*zc+rc*rc) 
 			t(iz,ir) = log((v*(rou+rc))/(v0*(rou+rc-r)))*oa 
			p(iz,ir) = sqrt(rou*rou-rc*rc)/(rou*v0) 
140		end do
130	end do
 
	return
	end 

 
 
 
 
  	
