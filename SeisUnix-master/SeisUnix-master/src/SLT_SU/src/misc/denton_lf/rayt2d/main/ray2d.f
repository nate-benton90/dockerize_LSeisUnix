*******************************************************************************
									  
  	subroutine makeray(nx,nz,fx,fz,dxv,dzv,vel,vxx,
     1    vxz,vzz,x0,z0,a0,amin,amax,nr,nt,dt,rx,rz,rpx,rpz,
     2    rq1,rp2,rq2,rv,rdvdx,rdvdz,nrs)
******************************************************************************
c  Trace rays for uniformly sampled v(nz,nx).
******************************************************************************
c  Input:
c  nx		number of x samples of velocity 
c  nz		number of z samples of velocity
c  fx		first x sample
c  fz		first z sample
c  nz		number of z samples of velocity
c  dxv		x sampling interval
c  dzv		z sampling interval
c  vel(*) 	sampled velocity --- array(nz,nx)
c  vxx,vxz,vzz  sampled second-order derivatives of velocity --- array(nz,nx)
c  x0		x coordinate of takeoff point
c  z0		z coordinate of takeoff point
c  a0(nr)	polar angles at takeoff point
c  amin		lower limit of emergence angle measured from z-axis
c  amax		upper limit of emergence angle measured from z-axis
c  nr		number of traced rays
c  nt		number of time samples
c  dt		time sampling interval
 
c  output:	ray parameters sampled at discrete ray steps
c  rx,rz(*)  	x,z coordinates of rays ---- array(nt,nr)
c  rpx,rpz(*) 	--- ray slop (sin/v) px,pz of rays  ---- array(nt,nr)
c  rq1,rp2,rq2  arrays(nt,nr) are used in paraxial ray tracing  
c                 (extrapolation of rays near central ray)
c  rv           velocity along the ray path --- array(nt,nr) 
c  rdvdx,rdvdz  velocity derivatives  --- array(nt,nr)
c  nrs          number of points per ray --- array(nr)
******************************************************************************
c Notes:
c The ray ends when it runs out of time (after nt steps) or with the first 
c step that is out of the (x,z) bounds of the velocity function v(x,z)
c or with the first step that is out of the bounds of emergent polar angle.
*****************************************************************************/

	integer nrs(*) 
	real vel(*),vxx(*),vxz(*),vzz(*),rx(*),rz(*),rpx(*),rpz(*),
     3	  a0(*),rq1(*),rq2(*),rp2(*),rv(*),rdvdx(*),rdvdz(*)

	integer it,ir,jr,nrsave,kill 
	real txmin,txmax,tx,ov,vv,lx,lz,norm,h,h2,h6,
     1 	  v1,dvdx1,dvdz1,uxx1,uxz1,uzz1

	parameter (n1=128)
	integer map(n1)
	real v(n1),dvdx(n1),dvdz(n1),uxx(n1),uxz(n1),uzz(n1),tzt(n1) 

	real x(n1),z(n1),px(n1),pz(n1),p1(n1),p2(n1),q1(n1),q2(n1),
     1	  dx(n1),dz(n1),dpx(n1),dpz(n1),
     2	  dp1(n1),dp2(n1),dq1(n1),dq2(n1)  
									
	real xt(n1),zt(n1),pxt(n1),pzt(n1),p1t(n1),p2t(n1),q1t(n1),
     1	  q2t(n1),dxt(n1),dzt(n1),dpxt(n1),dpzt(n1),
     2	  dp1t(n1),dp2t(n1),dq1t(n1),dq2t(n1)  

   
c	save the number of total rays
	nrsave = nr
 
c	velocity and derivatives at takeoff point  
  	x(1) = x0
 	z(1) = z0
	call vel2Interp(nx,nz,fx,fz,dxv,dzv,
     1    	vel,vxx,vxz,vzz,x,z,1,v,dvdx,dvdz,uxx,uxz,uzz)

 	ov = 1.0/v(1) 
	vv = v(1)*v(1) 
  
	v1 = v(1)
	dvdx1 = dvdx(1)
 	dvdz1 = dvdz(1)
	uxx1 = uxx(1)
 	uxz1 = uxz(1)
 	uzz1 = uzz(1)

c	initialize arrays
  	do 5 ir=1,nr
		x(ir) = x0
 		z(ir) = z0
		v(ir) = v1
		dvdx(ir) = dvdx1
 		dvdz(ir) = dvdz1
		uxx(ir) = uxx1
 		uxz(ir) = uxz1
 		uzz(ir) = uzz1
		p1(ir) = 0.0
 		q1(ir) = 1.0
 		p2(ir) = 1.0
 		q2(ir) = 0.0
		map(ir) = ir
		nrs(ir) = nt
5	end do

c	compute slowness at takeoff point  
	do 10 ir=1,nr		
		px(ir) = ov*sin(a0(ir)) 
 		pz(ir) = ov*cos(a0(ir)) 
10	end do
 
 
 
c	first ray step output
	do 50 ir=1,nr
		tzt(ir) = v(ir)*pz(ir)
  		rx(ir) = x(ir)
  		rz(ir) = z(ir)
 		rpx(ir) = px(ir)
  		rpz(ir) = pz(ir)
   		rq1(ir) = q1(ir)
  		rq2(ir) = q2(ir)
   		rp2(ir) = p2(ir)
  		rv(ir) = v(ir)
 		rdvdx(ir) = dvdx(ir)
  		rdvdz(ir) = dvdz(ir)
50	    end do 
 
 
c 	compute minimum and maximum z-component of unit ray vector  
	tzmin = cos(amax)-0.01 
	tzmax = cos(amin)+0.01 

c  	determine fraction steps fpr Runge-Kuta
 	h = dt
	h2 = 0.5*dt
	h6 = dt/6.0

c 	determine the upper boundaries of velocity model
	lx = fx+(nx-1)*dxv
 	lz = fz+(nz-1)*dzv


c	loop over time steps  
	do 1000 it=1,nt-1

c	  select the rays that are not out bounds 
	    kill = 0
	    do 1100 ir=1,nr
 		if (x(ir) .lt. fx .or. x(ir) .gt. lx .or. 
     1		    z(ir) .lt. fz .or. z(ir) .gt. lz .or.
     2		    tzt(ir) .lt. tzmin .or. tzt(ir) .gt. tzmax) then
		    	kill = kill+1
			jr = map(ir)
			nrs(jr) = it
 		else 
		    if(kill .gt. 0) then
			jr = ir-kill
			map(jr) = map(ir)
			v(jr) = v(ir)
			dvdx(jr) = dvdx(ir)
 			dvdz(jr) = dvdz(ir)
			uxx(jr) = uxx(ir)
 			uxz(jr) = uxz(ir)
 			uzz(jr) = uzz(ir)
			x(jr) = x(ir) 
 			z(jr) = z(ir)
			px(jr) = px(ir)	 
 			pz(jr) = pz(ir) 	
 			p1(jr) = p1(ir) 
 			p2(jr) = p2(ir) 
			q1(jr) = q1(ir) 
 			q2(jr) = q2(ir) 
 		    end if
		end if
1100	    end do
	    nr = nr-kill 
		
c	step 1 of 4th-order Runge-Kutta
 
c	  compute k1 = f(y0)
	    call dfrungk(nr,v,dvdx,dvdz,uxx,uxz,uzz,x,z,px,pz, 
     1	  	p1,q1,p2,q2,dx,dz,dpx,dpz,dp1,dq1,dp2,dq2)
 
c	  compute y1 = y0+0.5*h*k1
	    call sum2(nr,h2,x,z,px,pz,p1,q1,p2,q2,dx,dz,dpx,dpz,
     1	  	dp1,dq1,dp2,dq2,xt,zt,pxt,pzt,p1t,q1t,p2t,q2t)
 
c	velocity and derivatives interpolation
	call vel2Interp(nx,nz,fx,fz,dxv,dzv,
     1    	vel,vxx,vxz,vzz,xt,zt,nr,v,dvdx,dvdz,uxx,uxz,uzz)
 

c	step 2 of 4th-order Runge-Kutta

c	  compute k2 = f(y1)
	    call dfrungk(nr,v,dvdx,dvdz,uxx,uxz,uzz,xt,zt,pxt,pzt, 
     1	  	p1t,q1t,p2t,q2t,dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
 
c	  compute y2 = y0+0.5*h*k2
	    call sum2(nr,h2,x,z,px,pz,p1,q1,p2,q2,dxt,dzt,dpxt,dpzt,
     1	  	dp1t,dq1t,dp2t,dq2t,xt,zt,pxt,pzt,p1t,q1t,p2t,q2t)
  
c	  compute k = k1+2.0*k2
	    call sum1(nr,2.0,dx,dz,dpx,dpz,dp1,dq1,dp2,dq2,
     1	  	dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
   
c	velocity and derivatives interpolation
	call vel2Interp(nx,nz,fx,fz,dxv,dzv,
     1    	vel,vxx,vxz,vzz,xt,zt,nr,v,dvdx,dvdz,uxx,uxz,uzz)
 
 		
c	step 3 of 4th-order Runge-Kutta

c	  compute k3 = f(y2)
	    call dfrungk(nr,v,dvdx,dvdz,uxx,uxz,uzz,xt,zt,pxt,pzt, 
     1	  	p1t,q1t,p2t,q2t,dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
 
c	  compute y3 = y0+h*k3
	    call sum2(nr,h,x,z,px,pz,p1,q1,p2,q2,dxt,dzt,dpxt,dpzt,
     1	  	dp1t,dq1t,dp2t,dq2t,xt,zt,pxt,pzt,p1t,q1t,p2t,q2t)
 
c	  compute k = k1+2.0*k2+2.0*k3
	    call sum1(nr,2.0,dx,dz,dpx,dpz,dp1,dq1,dp2,dq2,
     1	  	dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
    
	call vel2Interp(nx,nz,fx,fz,dxv,dzv,
     1    	vel,vxx,vxz,vzz,xt,zt,nr,v,dvdx,dvdz,uxx,uxz,uzz)
 
c	step 4 of 4th-order Runge-Kutta

c	  compute k4 = f(y3)
	    call dfrungk(nr,v,dvdx,dvdz,uxx,uxz,uzz,xt,zt,pxt,pzt, 
     1	  	p1t,q1t,p2t,q2t,dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
 
c	  compute k = k1+2.0*k2+2.0*k3+k4
	    call sum1(nr,1.0,dx,dz,dpx,dpz,dp1,dq1,dp2,dq2,
     1	  	dxt,dzt,dpxt,dpzt,dp1t,dq1t,dp2t,dq2t)
   
c	  compute y4 = y0+h/6*k
	    call sum1(nr,h6,x,z,px,pz,p1,q1,p2,q2,
     1	  	dx,dz,dpx,dpz,dp1,dq1,dp2,dq2) 
 
	call vel2Interp(nx,nz,fx,fz,dxv,dzv,
     1    	vel,vxx,vxz,vzz,x,z,nr,v,dvdx,dvdz,uxx,uxz,uzz)
 
	    do 480 ir=1,nr
  		tzt(ir) = v(ir)*pz(ir)
480	    end do
 
c	save ray parameters 
	    do 500 ir=1,nr
		jr = map(ir) 
 		rx(it*nrsave+jr) = x(ir)
  		rz(it*nrsave+jr) = z(ir)
 		rpx(it*nrsave+jr) = px(ir)
  		rpz(it*nrsave+jr) = pz(ir)
   		rq1(it*nrsave+jr) = q1(ir)
   		rp2(it*nrsave+jr) = p2(ir)
   		rq2(it*nrsave+jr) = q2(ir)
  		rv(it*nrsave+jr) = v(ir)
 		rdvdx(it*nrsave+jr) = dvdx(ir)
 		rdvdz(it*nrsave+jr) = dvdz(ir)
500	    end do 

1000	end do	
 
c	back to the saved number of total rays
	nr = nrsave
 
	return
	end
 

***************************************************************************
	subroutine dfrungk(n,v,vx,vz,vxx,vxz,vzz,x,z,px,pz, 
     1	  	p1,q1,p2,q2,dx,dz,dpx,dpz,dp1,dq1,dp2,dq2)
*** calculate the increment of function in Rung-Kuta ***

	integer i
	real ov,vv,c,s,v11      	
	real v(*),vx(*),vz(*),vxx(*),vxz(*),vzz(*)
 
	real x(*),z(*),px(*),pz(*),p1(*),q1(*),p2(*),q2(*),
     1	  dx(*),dz(*),dpx(*),dpz(*),dp1(*),dq1(*),dp2(*),dq2(*)  

	do 100 i=1,n 
		ov = 1.0/v(i)
		vv = v(i)*v(i)
 		dx(i) = vv*px(i)
 		dz(i) = vv*pz(i) 
		dpx(i) = -ov*vx(i) 
 		dpz(i) = -ov*vz(i) 
 									
 		c = v(i)*pz(i)
		s = v(i)*px(i)
 		v11 = vxx(i)*c*c-2.0*vxz(i)*s*c+vzz(i)*s*s
 		dp1(i) = -ov*v11*q1(i)  
	  	dq1(i) = vv*p1(i)
		dp2(i) = -ov*v11*q2(i)  
		dq2(i) = vv*p2(i)
100	end do

	return
	end

***********************  a = a+h*b  *************************************
	subroutine sum1(n,h,a1,a2,a3,a4,a5,a6,a7,a8,
     1     b1,b2,b3,b4,b5,b6,b7,b8)
 
	integer i
	real a1(*),a2(*),a3(*),a4(*),a5(*),a6(*),a7(*),a8(*),
     1	   b1(*),b2(*),b3(*),b4(*),b5(*),b6(*),b7(*),b8(*)


	do 150 i=1,n
		a1(i) = a1(i)+h*b1(i)
 		a2(i) = a2(i)+h*b2(i)
 		a3(i) = a3(i)+h*b3(i)
 		a4(i) = a4(i)+h*b4(i)
 		a5(i) = a5(i)+h*b5(i)
 		a6(i) = a6(i)+h*b6(i)
 		a7(i) = a7(i)+h*b7(i)
 		a8(i) = a8(i)+h*b8(i)
  150	    end do

	return
	end
									
***************************  c = a+h*b ************************************
	subroutine sum2(n,h,a1,a2,a3,a4,a5,a6,a7,a8,b1,b2,b3,b4,
     1	   b5,b6,b7,b8,c1,c2,c3,c4,c5,c6,c7,c8)
 
	integer i
	real a1(*),a2(*),a3(*),a4(*),a5(*),a6(*),a7(*),a8(*),
     1	  b1(*),b2(*),b3(*),b4(*),b5(*),b6(*),b7(*),b8(*),
     2	  c1(*),c2(*),c3(*),c4(*),c5(*),c6(*),c7(*),c8(*)
																	     
	    do 150 i=1,n
		c1(i) = a1(i)+h*b1(i)
 		c2(i) = a2(i)+h*b2(i)
 		c3(i) = a3(i)+h*b3(i)
 		c4(i) = a4(i)+h*b4(i)
 		c5(i) = a5(i)+h*b5(i)
 		c6(i) = a6(i)+h*b6(i)
 		c7(i) = a7(i)+h*b7(i)
 		c8(i) = a8(i)+h*b8(i)
  150	    end do

	return
	end
