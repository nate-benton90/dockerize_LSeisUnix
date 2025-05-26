c******************************************************************************
									  
  	subroutine makeray(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,vel,vxx,vxy,vxz,
     1    vyy,vyz,vzz,x0,y0,z0,a0,azh,amin,amax,nr,nt,dt,
     2	  rx,ry,rz,rpx,rpy,rpz,re1x,re1y,re1z,re2x,re2y,re2z,
     3    rq111,rq112,rq121,rq122,rp211,rp212,rp221,rp222,
     4	  rq211,rq212,rq221,rq222,rv,rdvdx,rdvdy,rdvdz,nrs,
     5    map,v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz,tzt,
     6	  x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     7	  p111,p112,p121,p122,q111,q112,
     8	  q121,q122,p211,p212,p221,p222,
     9    q211,q212,q221,q222,dx,dy,dz,
     a    dpx,dpy,dpz,de1x,de1y,de1z,de2x,
     b	  de2y,de2z,dp111,dp112,dp121,
     c    dp122,dq111,dq112,dq121,dq122,
     d	  dp211,dp212,dp221,dp222,dq211,dq212,
     e    dq221,dq222,xt,yt,zt,pxt,pyt,pzt,
     f    e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     g	  p111t,p112t,p121t,p122t,
     h    q111t,q112t,q121t,q122t,p211t,
     i	  p212t,p221t,p222t,q211t,
     j	  q212t,q221t,q222t,dxt,dyt,
     k	  dzt,dpxt,dpyt,dpzt,de1xt,
     l	  de1yt,de1zt,de2xt,de2yt,de2zt,
     m	  dp111t,dp112t,dp121t,dp122t,
     n	  dq111t,dq112t,dq121t,dq122t,
     o    dp211t,dp212t,dp221t,dp222t,
     p	  dq211t,dq212t,dq221t,dq222t,n1)

c
c   
c
c                     / x (crossline --- line number)
c                    /
c                   /
c                  ----------- y (inline ---- trace number)
c                  |
c                  |
c                  |
c                  | z (depth)
c
c
c******************************************************************************
c*  Trace rays for uniformly sampled vel(nz,ny,nx).
c******************************************************************************
c  Input:
c  nx		number of x samples of velocity 
c  ny		number of y samples of velocity
c  nz		number of z samples of velocity
c  fx		first x sample
c  fy		first y sample
c  fz		first z sample
c  dxv		x sampling interval
c  dyv		y sampling interval
c  dzv		z sampling interval
c  vel(*)	sampled velocity  ----- array(nz,ny,nx)
c  vxx,vxy,vxz,vyy,vyz,vzz(*)     ------- array(nz,ny,nx)
c		sampled second-order derivatives of velocity
c  x0		x coordinate of takeoff point
c  y0		y coordinate of takeoff point
c  z0		z coordinate of takeoff point
c  a0(nr)	polar angles at takeoff point (0 <= a0 <= pi) measured 
c               vertical
c  azh(nr)	azimuth angles at takeoff point (0<= azh <= 2 pi) 
c               measured from y axis 
c  amin		lower limit of emergence polar angle 
c  amax		upper limit of emergence polar angle  
c  nr		number of traced rays
c  nt		number of time samples
c  dt		time sampling interval
c 
c****************************************************************************
c  output:	ray parameters sampled at discrete ray steps
c  rx,ry,rz(*)  x,y,z coordinates of rays ---- array(nt,nr)
c  rpx,rpy,rpz(*) --- ray slop (sin/v) px,py,pz of rays  ---- array(nt,nr)
c  re1x,re1y,re1z,re2x,re2y,re2z   --- normal to rays  ----  array(nt,nr)
c               p(x,y,z), re1(x,y,z), re2(x,y,z) are orthogonal
c  rq111,rq112,rq121,rq122   
c  rp211,rp212,rp221,rp222      these 12 arrays(nt,nr) are used 
c  rq211,rq212,rq221,rq222      in paraxial ray tracing (extrapolation 
c                               of rays near central ray)
c  rv                           velocity along the ray path --- array(nt,nr) 
c  rdvdx,rdvdy,rdvdz            velocity derivatives  --- array(nt,nr)
c  nrs                          number of points per ray --- array(nr)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c Notes:
c The ray ends when it runs out of time (after nt steps) or with the first 
c step that is out of the (x,y,z) bounds of the velocity function v(x,y,z)
c or with the first step that is out of the bounds of emergent polar angle.
c****************************************************************************

	integer nrs(nr) 
	real vel(nz*ny*nx),vxx(nz*ny*nx),vxy(nz*ny*nx),vxz(nz*ny*nx),
     1    vyy(nz*ny*nx),vyz(nz*ny*nx),vzz(nz*ny*nx),
     2	  a0(nr),azh(nr),rx(nt*nr),ry(nt*nr),rz(nt*nr),rpx(nt*nr),
     3	  rpy(nt*nr),rpz(nt*nr),re1x(nt*nr),re1y(nt*nr),
     4	  re1z(nt*nr),re2x(nt*nr),re2y(nt*nr),re2z(nt*nr),
     5	  rq111(nt*nr),rq112(nt*nr),rq121(nt*nr),rq122(nt*nr),
     6    rp211(nt*nr),rp212(nt*nr),rp221(nt*nr),rp222(nt*nr),
     7	  rq211(nt*nr),rq212(nt*nr),rq221(nt*nr),rq222(nt*nr),
     8	  rv(nt*nr),rdvdx(nt*nr),rdvdy(nt*nr),rdvdz(nt*nr)

	integer it,ir,jr,nrsave,kill 
	real tzmin,tzmax,tx,ty,tz,ov,vv,lx,ly,lz,norm,h,h2,h6,
     1 	  v1,dvdx1,dvdy1,dvdz1,uxx1,uxy1,uxz1,uyy1,uyz1,uzz1
c   
c      nr <= 128 
c  
cccc	parameter (n1=128)
	integer n1
	integer map(n1)
	real v(n1),dvdx(n1),dvdy(n1),dvdz(n1),uxx(n1),uxy(n1),uxz(n1),
     1	  uyy(n1),uyz(n1),uzz(n1),tzt(n1) 

	real x(n1),y(n1),z(n1),px(n1),py(n1),pz(n1),
     1	  e1x(n1),e1y(n1),e1z(n1),e2x(n1),e2y(n1),e2z(n1),
     2	  p111(n1),p112(n1),p121(n1),p122(n1),q111(n1),q112(n1),
     3    q121(n1),q122(n1),p211(n1),p212(n1),p221(n1),p222(n1),
     4    q211(n1),q212(n1),q221(n1),q222(n1),
     5	  dx(n1),dy(n1),dz(n1),dpx(n1),dpy(n1),dpz(n1),
     6	  de1x(n1),de1y(n1),de1z(n1),de2x(n1),de2y(n1),de2z(n1),
     7	  dp111(n1),dp112(n1),dp121(n1),dp122(n1),dq111(n1),dq112(n1),
     8    dq121(n1),dq122(n1),dp211(n1),dp212(n1),dp221(n1),dp222(n1),
     9    dq211(n1),dq212(n1),dq221(n1),dq222(n1) 
									
	real xt(n1),yt(n1),zt(n1),pxt(n1),pyt(n1),pzt(n1),
     1	  e1xt(n1),e1yt(n1),e1zt(n1),e2xt(n1),e2yt(n1),e2zt(n1),
     2	  p111t(n1),p112t(n1),p121t(n1),p122t(n1),q111t(n1),q112t(n1),
     3    q121t(n1),q122t(n1),p211t(n1),p212t(n1),p221t(n1),p222t(n1),
     4    q211t(n1),q212t(n1),q221t(n1),q222t(n1),dxt(n1),dyt(n1),
     5	  dzt(n1),dpxt(n1),dpyt(n1),dpzt(n1),de1xt(n1),de1yt(n1),
     6	  de1zt(n1),de2xt(n1),de2yt(n1),de2zt(n1),dp111t(n1),dp112t(n1),
     7	  dp121t(n1),dp122t(n1),dq111t(n1),dq112t(n1),dq121t(n1),
     8    dq122t(n1),dp211t(n1),dp212t(n1),dp221t(n1),dp222t(n1),
     9    dq211t(n1),dq212t(n1),dq221t(n1),dq222t(n1) 

	real fxr,lxt,fyr,lyr

   
cc	call msgsci(" makeray n1= ",n1)
cc	call msgsci(" makeray nr= ",nr)
c	save the number of total rays
	nrsave = nr
 
c	 velocity and derivatives at takeoff point  
  	x(1) = x0
	y(1) = y0
	z(1) = z0
	call vel3Interp(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,
     1    	vel,vxx,vxy,vxz,vyy,vyz,vzz,x,y,z,1,
     2	  	v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz)

 	ov = 1.0/v(1) 
	vv = v(1)*v(1) 
  
	v1 = v(1)
	dvdx1 = dvdx(1)
	dvdy1 = dvdy(1)
	dvdz1 = dvdz(1)
	uxx1 = uxx(1)
	uxy1 = uxy(1)
	uxz1 = uxz(1)
	uyy1 = uyy(1)
	uyz1 = uyz(1)
	uzz1 = uzz(1)

c	initialize arrays
ccc	call msgsci(" loop 5 nr= ",nr)
  	do 5 ir=1,nr
		x(ir) = x0
		y(ir) = y0
		z(ir) = z0
		v(ir) = v1
		dvdx(ir) = dvdx1
		dvdy(ir) = dvdy1
		dvdz(ir) = dvdz1
		uxx(ir) = uxx1
		uxy(ir) = uxy1
		uxz(ir) = uxz1
		uyy(ir) = uyy1
		uyz(ir) = uyz1
		uzz(ir) = uzz1
		p111(ir) = 0.0
		p112(ir) = 0.0
		p121(ir) = 0.0
		p122(ir) = 0.0
		q111(ir) = 1.0
		q112(ir) = 0.0
		q121(ir) = 0.0
		q122(ir) = 1.0
		p211(ir) = 1.0
		p212(ir) = 0.0
		p221(ir) = 0.0
		p222(ir) = 1.0
		q211(ir) = 0.0
		q212(ir) = 0.0
		q221(ir) = 0.0
		q222(ir) = 0.0
		map(ir) = ir
		nrs(ir) = nt
5	end do

c	compute slowness at takeoff point  
ccc	call msgsci(" loop 10 nr= ",nr)
	do 10 ir=1,nr		
		px(ir) = ov*sin(azh(ir))*sin(a0(ir)) 
		py(ir) = ov*cos(azh(ir))*sin(a0(ir)) 
		pz(ir) = ov*cos(a0(ir)) 
10	end do
 
 
c	dertermine ray-centered coordinates at takeoff point
ccc	call msgsci(" loop 20 nr= ",nr)
	do 20 ir=1,nr	
c  		unit tangent vector  
		tx = v1*px(ir) 
		ty = v1*py(ir) 
		tz = v1*pz(ir)

c	     first unit normal vector 
		if (ABS(tx) .gt. 0.5 .or. ABS(tz) .gt. 0.5) then
			norm = 1.0/sqrt(tx*tx+tz*tz) 
			e1x(ir) = norm*tz 
			e1y(ir) = 0.0 
			e1z(ir) = -norm*tx 
		else  
			norm = 1.0/sqrt(ty*ty+tz*tz) 
			e1x(ir) = 0.0 
			e1y(ir) = norm*tz 
			e1z(ir) = -norm*ty 
		end if	

c	    second normal unit vector is cross product of t and e1  
		e2x(ir) = ty*e1z(ir)-tz*e1y(ir)
		e2y(ir) = tz*e1x(ir)-tx*e1z(ir)
		e2z(ir) = tx*e1y(ir)-ty*e1x(ir)
20	end do
 
c	first ray step output
ccc	call msgsci(" loop 50 nr= ",nr)
	do 50 ir=1,nr
		tzt(ir) = v(ir)*pz(ir)
  		rx(ir) = x(ir)
 		ry(ir) = y(ir)
 		rz(ir) = z(ir)
 		rpx(ir) = px(ir)
 		rpy(ir) = py(ir)
 		rpz(ir) = pz(ir)
 		re1x(ir) = e1x(ir)
 		re1y(ir) = e1y(ir)
 		re1z(ir) = e1z(ir)
 		re2x(ir) = e2x(ir)
 		re2y(ir) = e2y(ir)
 		re2z(ir) = e2z(ir)
  		rq111(ir) = q111(ir)
  		rq112(ir) = q112(ir)
  		rq121(ir) = q121(ir)
  		rq122(ir) = q122(ir)
  		rp211(ir) = p211(ir)
  		rp212(ir) = p212(ir)
  		rp221(ir) = p221(ir)
  		rp222(ir) = p222(ir)
  		rq211(ir) = q211(ir)
  		rq212(ir) = q212(ir)
  		rq221(ir) = q221(ir)
  		rq222(ir) = q222(ir)
 		rv(ir) = v(ir)
 		rdvdx(ir) = dvdx(ir)
 		rdvdy(ir) = dvdy(ir)
 		rdvdz(ir) = dvdz(ir)
50	    end do 

ccc	call msgsci(" after loop 50 nr= ",nr)
 
 
c 	compute minimum and maximum z-component of unit ray vector  
	tzmin = cos(amax)-0.01 
	tzmax = cos(amin)+0.01 

c  	determine fraction steps fpr Runge-Kuta
 	h = dt
	h2 = 0.5*dt
	h6 = dt/6.0

c 	determine the upper boundaries of velocity model
	lx = fx+(nx-1)*dxv
	ly = fy+(ny-1)*dyv
	lz = fz+(nz-1)*dzv

c       boundary tolerance
	fxr = fx - 0.001*dxv
	lxr = lx + 0.001*dxv
	fyr = fy - 0.001*dyv
	lyr = ly + 0.001*dyv

c	call msgscf("tzmin=",tzmin)
c	call msgscf("tzmax=",tzmax)
c	call msgscf("fx=",fx)
c	call msgscf("lx=",lx)
c	call msgscf("fy=",fy)
c	call msgscf("ly=",ly)
c	call msgscf("fz=",fz)
c	call msgscf("lz=",lz)

c	loop over time steps  
	do 1000 it=1,nt-1

c	  select the rays that are not out bounds 
	    kill = 0
	    do 1100 ir=1,nr

cccc round up to the boudaries if x,y,z are very close 
		if(x(ir).lt.fx .and. x(ir).gt.fxr) x(ir) = fx
		if(x(ir).gt.lx .and. x(ir).lt.lxr) x(ir) = lx
		if(y(ir).lt.fy .and. y(ir).gt.fxr) y(ir) = fy
		if(y(ir).gt.ly .and. y(ir).lt.lyr) y(ir) = ly

 		if (x(ir) .lt. fx .or. x(ir) .gt. lx .or. 
     1		    y(ir) .lt. fy .or. y(ir) .gt. ly .or.
     2		    z(ir) .lt. fz .or. z(ir) .gt. lz .or.
     3		    tzt(ir) .lt. tzmin .or. tzt(ir) .gt. tzmax) then

c		if(x(ir).lt.fx) call msgscf("x=",x(ir))
c		if(x(ir).gt.lx) call msgscf("x=",x(ir))
c		if(y(ir).lt.fy) call msgscf("y=",y(ir))
c		if(y(ir).gt.ly) call msgscf("y=",y(ir))
c		if(z(ir).lt.fz) call msgscf("z=",z(ir))
c		if(z(ir).gt.lz) call msgscf("z=",z(ir))
c		if(tzt(ir).lt.tzmin) call msgscf("tzt=",tzt(ir))
c		if(tzt(ir).gt.tzmax) call msgscf("tzt=",tzt(ir))

		    	kill = kill+1
			jr = map(ir)
			nrs(jr) = it
 		else 
		    if(kill .gt. 0) then
			jr = ir-kill
			map(jr) = map(ir)
			v(jr) = v(ir)
			dvdx(jr) = dvdx(ir)
			dvdy(jr) = dvdy(ir)
			dvdz(jr) = dvdz(ir)
			uxx(jr) = uxx(ir)
			uxy(jr) = uxy(ir)
			uxz(jr) = uxz(ir)
			uyy(jr) = uyy(ir)
			uyz(jr) = uyz(ir)
			uzz(jr) = uzz(ir)
			x(jr) = x(ir) 
			y(jr) = y(ir) 
			z(jr) = z(ir)
			px(jr) = px(ir)	 
			py(jr) = py(ir)	 
			pz(jr) = pz(ir) 	
			e1x(jr) = e1x(ir)
			e1y(jr) = e1y(ir)
			e1z(jr) = e1z(ir)
			e2x(jr) = e2x(ir)
			e2y(jr) = e2y(ir)
			e2z(jr) = e2z(ir)
			p111(jr) = p111(ir) 
			q111(jr) = q111(ir) 
			p112(jr) = p112(ir) 
			q112(jr) = q112(ir) 
			p121(jr) = p121(ir) 
			q121(jr) = q121(ir) 
			p122(jr) = p122(ir) 
			q122(jr) = q122(ir) 
			p211(jr) = p211(ir) 
			q211(jr) = q211(ir) 
			p212(jr) = p212(ir) 
			q212(jr) = q212(ir) 
			p221(jr) = p221(ir) 
			q221(jr) = q221(ir) 
			p222(jr) = p222(ir) 
			q222(jr) = q222(ir) 
		    end if
		end if
1100	    end do
	    nr = nr-kill 
		
c	step 1 of 4th-order Runge-Kutta
 
c	  compute k1 = f(y0)
	    call dfrungk(nr,v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz,
     1	  	x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     2	  	p111,p112,p121,p122,q111,q112,q121,q122,
     3	  	p211,p212,p221,p222,q211,q212,q221,q222,
     4	  	dx,dy,dz,dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     5	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     6	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222)
 
c	  compute y1 = y0+0.5*h*k1
	    call sum2(nr,h2,x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     1	  	p111,p112,p121,p122,q111,q112,q121,q122,
     2	  	p211,p212,p221,p222,q211,q212,q221,q222,
     3	  	dx,dy,dz,dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     4	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     5	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222,
     6	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     7	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     8	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t)

c	velocity and derivatives interpolation
	    call vel3Interp(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,
     1    	vel,vxx,vxy,vxz,vyy,vyz,vzz,xt,yt,zt,nr,
     2	  	v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz)


c	step 2 of 4th-order Runge-Kutta

c	  compute k2 = f(y1)
	    call dfrungk(nr,v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz,
     1	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     2	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     3	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)

c	  compute y2 = y0+0.5*h*k2
	    call sum2(nr,h2,x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     1	  	p111,p112,p121,p122,q111,q112,q121,q122,
     2	  	p211,p212,p221,p222,q211,q212,q221,q222,
     3	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     4	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     5	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t,
     6	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     7	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     8	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t)
 
c	  compute k = k1+2.0*k2
            call sum1(nr,2.0,dx,dy,dz,
     1		dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     2	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     3	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)
  
c	velocity and derivatives interpolation
 	    call vel3Interp(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,
     1    	vel,vxx,vxy,vxz,vyy,vyz,vzz,xt,yt,zt,nr,
     2	  	v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz)
 
 		
c	step 3 of 4th-order Runge-Kutta

c	  compute k3 = f(y2)
	    call dfrungk(nr,v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz,
     1	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     2	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     3	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)

c	  compute y3 = y0+h*k3
	    call sum2(nr,h,x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     1	  	p111,p112,p121,p122,q111,q112,q121,q122,
     2	  	p211,p212,p221,p222,q211,q212,q221,q222,
     3	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     4	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     5	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t,
     6	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     7	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     8	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t)

c	  compute k = k1+2.0*k2+2.0*k3
            call sum1(nr,2.0,dx,dy,dz,
     1		dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     2	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     3	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)
   
	    call vel3Interp(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,
     1    	vel,vxx,vxy,vxz,vyy,vyz,vzz,xt,yt,zt,nr,
     2	  	v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz)
 
c	step 4 of 4th-order Runge-Kutta

c	  compute k4 = f(y3)
	    call dfrungk(nr,v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz,
     1	  	xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     2	  	p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,
     3	  	p211t,p212t,p221t,p222t,q211t,q212t,q221t,q222t,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)

c	  compute k = k1+2.0*k2+2.0*k3+k4
            call sum1(nr,1.0,dx,dy,dz,
     1		dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     2	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     3	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222,
     4	dxt,dyt,dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     5	  dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     6	  dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)
   
c	  compute y4 = y0+h/6*k
	    call sum1(nr,h6,x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     1	  	p111,p112,p121,p122,q111,q112,q121,q122,
     2	  	p211,p212,p221,p222,q211,q212,q221,q222,
     3	  	dx,dy,dz,dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     4	  	dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     5	  	dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222)
 
 
 	    call vel3Interp(nx,ny,nz,fx,fy,fz,dxv,dyv,dzv,
     1    	vel,vxx,vxy,vxz,vyy,vyz,vzz,x,y,z,nr,
     2	  	v,dvdx,dvdy,dvdz,uxx,uxy,uxz,uyy,uyz,uzz)

	    do 480 ir=1,nr
  		tzt(ir) = v(ir)*pz(ir)
480	    end do
 
c	save ray parameters 
	    do 500 ir=1,nr
		jr = map(ir) 
 		rx(it*nrsave+jr) = x(ir)
 		ry(it*nrsave+jr) = y(ir)
 		rz(it*nrsave+jr) = z(ir)
 		rpx(it*nrsave+jr) = px(ir)
 		rpy(it*nrsave+jr) = py(ir)
 		rpz(it*nrsave+jr) = pz(ir)
 		re1x(it*nrsave+jr) = e1x(ir)
 		re1y(it*nrsave+jr) = e1y(ir)
 		re1z(it*nrsave+jr) = e1z(ir)
 		re2x(it*nrsave+jr) = e2x(ir)
 		re2y(it*nrsave+jr) = e2y(ir)
 		re2z(it*nrsave+jr) = e2z(ir)
  		rq111(it*nrsave+jr) = q111(ir)
  		rq112(it*nrsave+jr) = q112(ir)
  		rq121(it*nrsave+jr) = q121(ir)
  		rq122(it*nrsave+jr) = q122(ir)
  		rp211(it*nrsave+jr) = p211(ir)
  		rp212(it*nrsave+jr) = p212(ir)
  		rp221(it*nrsave+jr) = p221(ir)
  		rp222(it*nrsave+jr) = p222(ir)
  		rq211(it*nrsave+jr) = q211(ir)
  		rq212(it*nrsave+jr) = q212(ir)
  		rq221(it*nrsave+jr) = q221(ir)
  		rq222(it*nrsave+jr) = q222(ir)
 		rv(it*nrsave+jr) = v(ir)
 		rdvdx(it*nrsave+jr) = dvdx(ir)
 		rdvdy(it*nrsave+jr) = dvdy(ir)
 		rdvdz(it*nrsave+jr) = dvdz(ir)
500	    end do 

1000	end do	
 
c	back to the saved number of total rays
	nr = nrsave
 
	return
	end
 

***************************************************************************
	subroutine dfrungk(n,v,vx,vy,vz,vxx,vxy,vxz,vyy,vyz,vzz,
     1	  x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     2	  p111,p112,p121,p122,q111,q112,q121,q122,
     3	  p211,p212,p221,p222,q211,q212,q221,q222,
     4	  dx,dy,dz,dpx,dpy,dpz,de1x,de1y,de1z,de2x,de2y,de2z,
     5	  dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     6	  dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222)
*** calculate the increment of function in Rung-Kuta ***

	integer i
	real ov,vv,e1v,e2v,tm11,tm12,tm21,tm22,tm31,tm32,v11,v12,v22      	
	real v(*),vx(*),vy(*),vz(*),vxx(*),vxy(*),vxz(*),
     1	  vyy(*),vyz(*),vzz(*)
 
	real x(*),y(*),z(*),px(*),py(*),pz(*),
     1	  e1x(*),e1y(*),e1z(*),e2x(*),e2y(*),e2z(*),
     2	  p111(*),p112(*),p121(*),p122(*),q111(*),q112(*),
     3    q121(*),q122(*),p211(*),p212(*),p221(*),p222(*),
     4    q211(*),q212(*),q221(*),q222(*),
     5	  dx(*),dy(*),dz(*),dpx(*),dpy(*),dpz(*),
     6	  de1x(*),de1y(*),de1z(*),de2x(*),de2y(*),de2z(*),
     7	  dp111(*),dp112(*),dp121(*),dp122(*),dq111(*),dq112(*),
     8    dq121(*),dq122(*),dp211(*),dp212(*),dp221(*),dp222(*),
     9    dq211(*),dq212(*),dq221(*),dq222(*) 

	do 100 i=1,n 
		ov = 1.0/v(i)
		vv = v(i)*v(i)
 		dx(i) = vv*px(i)
		dy(i) = vv*py(i) 
		dz(i) = vv*pz(i) 
		dpx(i) = -ov*vx(i) 
		dpy(i) = -ov*vy(i) 
		dpz(i) = -ov*vz(i) 
		e1v = v(i)*(e1x(i)*vx(i)+e1y(i)*vy(i)+e1z(i)*vz(i)) 
		de1x(i) = e1v*px(i)  
		de1y(i) = e1v*py(i)  
		de1z(i) = e1v*pz(i)  
		e2v = v(i)*(e2x(i)*vx(i)+e2y(i)*vy(i)+e2z(i)*vz(i))
 		de2x(i) = e2v*px(i) 
		de2y(i) = e2v*py(i) 
		de2z(i) = e2v*pz(i) 
									
 		tm11 = e1x(i)*vxx(i)+e1y(i)*vxy(i)+e1z(i)*vxz(i)
		tm12 = e2x(i)*vxx(i)+e2y(i)*vxy(i)+e2z(i)*vxz(i)
		tm21 = e1x(i)*vxy(i)+e1y(i)*vyy(i)+e1z(i)*vyz(i)
		tm22 = e2x(i)*vxy(i)+e2y(i)*vyy(i)+e2z(i)*vyz(i)
		tm31 = e1x(i)*vxz(i)+e1y(i)*vyz(i)+e1z(i)*vzz(i)
		tm32 = e2x(i)*vxz(i)+e2y(i)*vyz(i)+e2z(i)*vzz(i)
		v11 = -ov*(tm11*e1x(i)+tm21*e1y(i)+tm31*e1z(i))
		v12 = -ov*(tm12*e1x(i)+tm22*e1y(i)+tm32*e1z(i))
		v22 = -ov*(tm12*e2x(i)+tm22*e2y(i)+tm32*e2z(i))
		dp111(i) = v11*q111(i)+v12*q121(i)  
	  	dq111(i) = vv*p111(i)
		dp112(i) = v11*q112(i)+v12*q122(i) 
		dq112(i) = vv*p112(i)
		dp121(i) = v12*q111(i)+v22*q121(i) 
		dq121(i) = vv*p121(i) 
		dp122(i) = v12*q112(i)+v22*q122(i) 
		dq122(i) = vv*p122(i)
		dp211(i) = v11*q211(i)+v12*q221(i)
		dq211(i) = vv*p211(i)
		dp212(i) = v11*q212(i)+v12*q222(i) 
		dq212(i) = vv*p212(i) 
		dp221(i) = v12*q211(i)+v22*q221(i)
		dq221(i) = vv*p221(i)
		dp222(i) = v12*q212(i)+v22*q222(i) 
		dq222(i) = vv*p222(i)
100	end do

	return
	end

***********************  a = a+h*b  *************************************
	subroutine sum1(n,h,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13, 
     1     a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,
     2     b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14, 
     3     b15,b16,b17,b18,b19,b20,b21,b22,b23,b24,b25,b26,b27,b28)
 
	integer i
	real a1(*),a2(*),a3(*),a4(*),a5(*),a6(*),a7(*),a8(*),a9(*),
     1	  a10(*),a11(*),a12(*),a13(*),a14(*),a15(*),a16(*),a17(*),
     2	  a18(*),a19(*),a20(*),a21(*),a22(*),a23(*),a24(*),a25(*),
     3	  a26(*),a27(*),a28(*),b1(*),b2(*),b3(*),b4(*),b5(*),b6(*),
     4	  b7(*),b8(*),b9(*),b10(*),b11(*),b12(*),b13(*),b14(*),b15(*),
     5 	  b16(*),b17(*),b18(*),b19(*),b20(*),b21(*),b22(*),b23(*),
     6	  b24(*),b25(*),b26(*),b27(*),b28(*)																		 
	do 150 i=1,n
		a1(i) = a1(i)+h*b1(i)
 		a2(i) = a2(i)+h*b2(i)
 		a3(i) = a3(i)+h*b3(i)
 		a4(i) = a4(i)+h*b4(i)
 		a5(i) = a5(i)+h*b5(i)
 		a6(i) = a6(i)+h*b6(i)
 		a7(i) = a7(i)+h*b7(i)
 		a8(i) = a8(i)+h*b8(i)
 		a9(i) = a9(i)+h*b9(i)
 		a10(i) = a10(i)+h*b10(i)
		a11(i) = a11(i)+h*b11(i)
 		a12(i) = a12(i)+h*b12(i)
 		a13(i) = a13(i)+h*b13(i)
 		a14(i) = a14(i)+h*b14(i)
 		a15(i) = a15(i)+h*b15(i)
 		a16(i) = a16(i)+h*b16(i)
 		a17(i) = a17(i)+h*b17(i)
 		a18(i) = a18(i)+h*b18(i)
 		a19(i) = a19(i)+h*b19(i)
 		a20(i) = a20(i)+h*b20(i)
		a21(i) = a21(i)+h*b21(i)
 		a22(i) = a22(i)+h*b22(i)
 		a23(i) = a23(i)+h*b23(i)
 		a24(i) = a24(i)+h*b24(i)
 		a25(i) = a25(i)+h*b25(i)
 		a26(i) = a26(i)+h*b26(i)
 		a27(i) = a27(i)+h*b27(i)
 		a28(i) = a28(i)+h*b28(i)
 150	    end do

	return
	end
									
***************************  c = a+h*b ************************************
	subroutine sum2(n,h,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13, 
     1     a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,
     2     b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14, 
     3     b15,b16,b17,b18,b19,b20,b21,b22,b23,b24,b25,b26,b27,b28,
     4     c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14, 
     5     c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28)
 
	integer i
	real a1(*),a2(*),a3(*),a4(*),a5(*),a6(*),a7(*),a8(*),a9(*),
     1	  a10(*),a11(*),a12(*),a13(*),a14(*),a15(*),a16(*),a17(*),
     2	  a18(*),a19(*),a20(*),a21(*),a22(*),a23(*),a24(*),a25(*),
     3	  a26(*),a27(*),a28(*),b1(*),b2(*),b3(*),b4(*),b5(*),b6(*),
     4	  b7(*),b8(*),b9(*),b10(*),b11(*),b12(*),b13(*),b14(*),b15(*),
     5 	  b16(*),b17(*),b18(*),b19(*),b20(*),b21(*),b22(*),b23(*),
     6	  b24(*),b25(*),b26(*),b27(*),b28(*),c1(*),c2(*),c3(*),c4(*),
     7	  c5(*),c6(*),c7(*),c8(*),c9(*),c10(*),c11(*),c12(*),c13(*),
     8	  c14(*),c15(*),c16(*),c17(*),c18(*),c19(*),c20(*),c21(*),
     9	  c22(*),c23(*),c24(*),c25(*),c26(*),c27(*),c28(*)																	     
	    do 150 i=1,n
		c1(i) = a1(i)+h*b1(i)
 		c2(i) = a2(i)+h*b2(i)
 		c3(i) = a3(i)+h*b3(i)
 		c4(i) = a4(i)+h*b4(i)
 		c5(i) = a5(i)+h*b5(i)
 		c6(i) = a6(i)+h*b6(i)
 		c7(i) = a7(i)+h*b7(i)
 		c8(i) = a8(i)+h*b8(i)
 		c9(i) = a9(i)+h*b9(i)
 		c10(i) = a10(i)+h*b10(i)
		c11(i) = a11(i)+h*b11(i)
 		c12(i) = a12(i)+h*b12(i)
 		c13(i) = a13(i)+h*b13(i)
 		c14(i) = a14(i)+h*b14(i)
 		c15(i) = a15(i)+h*b15(i)
 		c16(i) = a16(i)+h*b16(i)
 		c17(i) = a17(i)+h*b17(i)
 		c18(i) = a18(i)+h*b18(i)
 		c19(i) = a19(i)+h*b19(i)
 		c20(i) = a20(i)+h*b20(i)
		c21(i) = a21(i)+h*b21(i)
 		c22(i) = a22(i)+h*b22(i)
 		c23(i) = a23(i)+h*b23(i)
 		c24(i) = a24(i)+h*b24(i)
 		c25(i) = a25(i)+h*b25(i)
 		c26(i) = a26(i)+h*b26(i)
 		c27(i) = a27(i)+h*b27(i)
 		c28(i) = a28(i)+h*b28(i)
 150	    end do

	return
	end
