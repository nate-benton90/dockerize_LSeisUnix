  
 
**  residual traveltime calculation based on reference time **
	subroutine resit(t,nx,ny,nz,nr,dx,dy,dr,fx,fy,x0,y0,tb)
c
c Input:
c   t(nz,nx,ny) traveltime before implementation of residual
c   fx,fy,fz,nx,ny,nz,dx,dy,dz define t grid 
c   tb(nz,nr) reference traveltime
c   nr,dr define tb as function of lateral distance r (fr=0.)
c Output:
c   t(nz,nx,ny) residual traveltime  

	integer ix,iy,iz,jr  
	real xi,yi,ar,sr,sr1 
	real t(nz,nx,ny),tb(nz,nr)  		
  
   
 	do 10 iy = 1,ny
  	    do 20 ix = 1,nx 
		yi = fy+(iy-1)*dy-y0
		xi = fx+(ix-1)*dx-x0
     		ar = sqrt(xi*xi+yi*yi)/dr 
		jr = 1+int(ar) 
 		jr = min0(nr-1,jr)
		sr = 1.0+ar-float(jr) 
 		sr1 = 1.0-sr 
c$DIR FORCE_VECTOR
    	    	do 30 iz = 1,nz
30    		  t(iz,ix,iy) = t(iz,ix,iy)-sr1*tb(iz,jr)-sr*tb(iz,jr+1)  
20	    continue
10 	continue
 
	return
	end 	
  
	
	subroutine recot(t,nout,nz,nr,dr,x,y,x0,y0,tb)
c
c  compute travel time from residual travel time
c 
c input:
c   t ---  residual time     array(nz,nout)
c   x0 --- source position inline
c   y0 --- source position crossline 
c   nout --- number of lateral position of t array
c   x(nout),y(nout)  --- x and y coordinates where t is specified
c   tb   ---  reference time defined at 0,dr,...,(nr-1)*dr
c              array(nz,nr) 
c output:
c   t ---  travel time recovered 
c 

	integer iz,jr,iout,nz1,nz2 
	real xi,yi,ar,sr,sr1 
	real t(*),tb(*),x(nout),y(nout)  		
  
   
 	do 10 iout = 0,nout-1
 		yi = y(iout+1)-y0
		xi = x(iout+1)-x0
     		ar = sqrt(xi*xi+yi*yi)/dr 
		jr = int(ar) 
 		if(jr .ge. nr-1) jr = nr-2
		sr = ar-float(jr) 
 		sr1 = 1.0-sr 
 		nz1 = jr*nz 
		nz2 = nz1+nz 
  	    	do 30 iz = 1,nz
30    		    t(iout*nz+iz) = t(iout*nz+iz)
     1					+sr1*tb(nz1+iz)+sr*tb(nz2+iz)  
10 	continue															
	return
	end 	
   		
c***	linear interpolation  in lateral directions ***********************
c
c input:
c      t(nz,nx,ny)        ----  travel time table
c      fx,fy,dx,dy,nx,ny  ----  define (x,y) positions of table 
c      nz                 ----  number of depth samples
c      x(nout),y(nout)    ----  x, y locations of output tracel time
c      nout               ----  number of lateral locations of tout
c output:
c      tout(nz,nout)      ----  interpolated travel time at x,y 
c
c

	subroutine interp(nx,ny,nz,nout,fx,fy,dx,dy,x,y,t,tout)
									
	integer iz,jx,jy,i0,i1,i2,i3,iout 
	real odx,ody,ay,sy,ax,sx,w0,w1,w2,w3 
	real t(*),tout(*),x(nout),y(nout) 		
  
	ody = 1.0/dy
	odx = 1.0/dx
   
 	do 10 iout = 0,nout-1
 		ay = (y(iout+1)-fy)*ody
 		jy = int(ay)
		if(jy .ge. ny-1) jy = ny-2
		sy = ay-jy 
		if(sy .le. 0.01) sy = 0.
		if(sy .ge. 0.99) sy = 1.0
		ax = (x(iout+1)-fx)*odx
 		jx = int(ax)
		if(jx .ge. nx-1) jx = nx-2
		sx = ax-jx 
		if(sx .le. 0.01) sx = 0.
		if(sx .ge. 0.99) sx = 1.0
		w0 = (1.0-sx)*(1.0-sy)
		w1 = sx*(1.0-sy)
		w2 = (1.0-sx)*sy
		w3 = sx*sy
  		i0 = jy*nx*nz+jx*nz
		i1 = i0+nz 
		i2 = i0+nz*nx
		i3 = i2+nz 
  	    	do 30 iz = 1,nz
     		    tout(iout*nz+iz) = w0*t(i0+iz)+w1*t(i1+iz)+w2*t(i2+iz)
     1				+w3*t(i3+iz)
30		end do  
10 	continue
 
	return
	end 	


c***	tranlate ----- x, y shift 
c
c       vt(iz,ix,iy) = v(iz,nx0+ix,ny0+iy)
c
c      v(nz,nx,ny)  
c      vt(nz,nxt,nyt)
c      nxt+nx0 <= nx
c      nyt+ny0 <= ny
c
 	subroutine trans(nx,ny,nz,nxt,nyt,nx0,ny0,v,vt)  

	integer i,i0,ix,iy,iz 
 	real v(*),vt(*)  	

  	do 10 iy=0,nyt-1  
	do 10 ix=0,nxt-1
		i = iy*nxt*nz+ix*nz
		i0 = (iy+ny0)*nx*nz+(ix+nx0)*nz 
 		do 20 iz=1,nz
     		    vt(i+iz) = v(i0+iz)
20		end do
10	end do
 
	return
	end


c***** calculate reference traveltime and horizontal slowness	******
c
c    input:
c           v0 --- velocity at z=0
c           a  --- dv/dz
c           dr --- lateral distance increment
c           nr --- number of lateral positions
c           fz,dz,nz   define  the depth of travel time 
c    output:
c           t(nz,nr) --- travel time at (z,r)       
c
	subroutine timeb(nr,nz,dr,dz,fz,a,v0,t)
 
 	integer i, ir, iz 
 	real   r,z,zc,v,rc,oa,ov0,rou,t(*)  
   
   	ov0 = 1.0/v0

	if(a .eq. 0.0) then
c       when r=0
	    do 10 iz=1,nz
	    	z = (iz-1)*dz+fz
	    	t(iz) = z*ov0
 10	    end do

c   	when r>0
	    do 30 ir=1,nr-1
		r = ir*dr
		i = ir*nz    
  		do 40 iz=1,nz
			z = (iz-1)*dz+fz  
 			rou = sqrt(r*r+z*z) 
			t(i+iz) = rou*ov0 
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
	    	t(iz) = log(v*ov0)*oa
 110	    end do
 
c   when r>0
	do 130 ir=1,nr-1
		r = ir*dr
		i = ir*nz    
  		do 140 iz=1,nz
			z = zc+(iz-1)*dz+fz  
  			v = v0+a*(z-zc) 
  			rc = (r*r+z*z-zc*zc)/(2.0*r) 
 			rou = sqrt(zc*zc+rc*rc) 
 			t(i+iz) = log((v*(rou+rc))/(v0*(rou+rc-r)))*oa 
 140		end do
130	end do
 
	return
	end 

 
 
 
 
  	
