
cccc fortran subroutine to do 3d ray tracing in parallel

	subroutine rt3dp(np,nzyx,nzyxo,nz,ny,nx,nzo,nyo,nxo,ek,nt,
     1   na,fa,da,amin,amax,azhmin,azhmax,dt,tmax,fxo,fyo,fzo,
     2	 dxo,dyo,dzo,fac,fx,fy,fz,dx,dy,dz,aperx,apery,v,vxx,vxy,vxz,
     3	 vyy,vyz,vzz,ov2,vt,vxxt,vxyt,vxzt,vyyt,vyzt,vzzt,tt1,tt2,t,s,
     4	 xsp,ysp,azhnp,azhxp,fxtp,fytp,nxtp,nytp,nzyxt,
     5   xp,yp,zp,pxp,pyp,pzp,e1xp,e1yp,e1zp,e2xp,e2yp,e2zp,
     6	 q111p,q112p,q121p,q122p,p211p,p212p,p221p,p222p,
     7	 q211p,q212p,q221p,q222p,vp,dvdxp,dvdyp,dvdzp,nrsp,
     8	 a0p,azh0p,n1,n2,p2p,q2p,hp,gradvp,d2tp,i2,i3,i6,
     9   map,vs,dvdxs,dvdys,dvdzs,uxx,uxy,uxz,uyy,uyz,uzz,tzt,
     a   xx,yy,zz,pxs,pys,pzs,e1xs,e1ys,e1zs,e2xs,e2ys,e2zs,
     b   p111s,p112s,p121s,p122s,q111s,q112s,q121s,q122s,
     c   p211s,p212s,p221s,p222s,q211s,q212s,q221s,q222s,
     d   dxs,dys,dzs,dpxs,dpys,dpzs,de1x,de1y,de1z,de2x,
     e   de2y,de2z,dp111,dp112,dp121,dp122,dq111,dq112,dq121,dq122,
     f   dp211,dp212,dp221,dp222,dq211,dq212,dq221,dq222,
     g   xt,yt,zt,pxt,pyt,pzt,e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     h   p111t,p112t,p121t,p122t,q111t,q112t,q121t,q122t,p211t,
     i   p212t,p221t,p222t,q211t,q212t,q221t,q222t,dxt,dyt,
     j   dzt,dpxt,dpyt,dpzt,de1xt,de1yt,de1zt,de2xt,de2yt,de2zt,
     k   dp111t,dp112t,dp121t,dp122t,dq111t,dq112t,dq121t,dq122t,
     l   dp211t,dp212t,dp221t,dp222t,dq211t,dq212t,dq221t,dq222t)

	integer np,nzyx,nzyxo,nz,ny,nx,nzo,nyo,nxo,ek,na,nt,nzyxt
	real fa,da,amin,amax,azhmin,azhmax,dt,tmax
	real fxo,fyo,fzo,dxo,dyo,dzo,fac,fx,fy,fz
	real dx,dy,dz,aperx,apery
	real v(nzyx)
	real vxx(nzyx),vxy(nzyx),vxz(nzyx)
	real vyy(nzyx),vyz(nzyx),vzz(nzyx)
	real ov2(nzyxo)
	real vt(nzyxt,np)
	real vxxt(nzyxt,np),vxyt(nzyxt,np),vxzt(nzyxt,np)
	real vyyt(nzyxt,np),vyzt(nzyxt,np),vzzt(nzyxt,np)
	real tt1(nyo,nxo,np),tt2(nyo,nxo,np)
	real t(nzyxo,np),s(nzyxo,np)
	real ysp(np),xsp(np)
	real azhnp(np),azhxp(np),fxtp(np),fytp(np)
	integer nxtp(np),nytp(np)

	integer n1, n2, i2, i3, i6
	real xp(n2,np),yp(n2,np),zp(n2,np),
     a    pxp(n2,np),pyp(n2,np),pzp(n2,np),
     1    e1xp(n2,np),e1yp(n2,np),e1zp(n2,np),
     b    e2xp(n2,np),e2yp(n2,np),e2zp(n2,np),
     2    q111p(n2,np),q112p(n2,np),q121p(n2,np),
     c    q122p(n2,np),p211p(n2,np),p212p(n2,np),
     3    p221p(n2,np),p222p(n2,np),q211p(n2,np),
     d    q212p(n2,np),q221p(n2,np),q222p(n2,np),
     4    vp(n2,np),dvdxp(n2,np),dvdyp(n2,np),dvdzp(n2,np)
	integer nrsp(n1,np)
	real a0p(n1,np),azh0p(n1,np)
	real p2p(i2*i2,np),q2p(i2*i2,np),hp(i3*i3,np)
	real gradvp(i3,np),d2tp(i6,np)


ccc temporary variables
	real xs,ys,temp,tmp,fxt,fyt,eyt,ext,PI
	real azhx,azhn
	integer nx0,ny0,ip


	integer map(n1,np)
	real vs(n1,np),dvdxs(n1,np),dvdys(n1,np),dvdzs(n1,np)
	real uxx(n1,np),uxy(n1,np),uxz(n1,np),
     1   uyy(n1,np),uyz(n1,np),uzz(n1,np),tzt(n1,np)
	real xx(n1,np),yy(n1,np),zz(n1,np)
	real pxs(n1,np),pys(n1,np),pzs(n1,np),
     1   e1xs(n1,np),e1ys(n1,np),e1zs(n1,np),e2xs(n1,np),
     2	 e2ys(n1,np),e2zs(n1,np),p111s(n1,np),p112s(n1,np),
     3   p121s(n1,np),p122s(n1,np),q111s(n1,np),q112s(n1,np),
     4   q121s(n1,np),q122s(n1,np),p211s(n1,np),p212s(n1,np),
     5	 p221s(n1,np),p222s(n1,np),q211s(n1,np),q212s(n1,np),
     6	 q221s(n1,np),q222s(n1,np),dxs(n1,np),dys(n1,np),dzs(n1,np),
     7	 dpxs(n1,np),dpys(n1,np),dpzs(n1,np),de1x(n1,np),de1y(n1,np),
     8	 de1z(n1,np),de2x(n1,np),de2y(n1,np),de2z(n1,np),
     9   dp111(n1,np),dp112(n1,np),dp121(n1,np),dp122(n1,np)
	real dq111(n1,np),dq112(n1,np),dq121(n1,np),dq122(n1,np),
     1	 dp211(n1,np),dp212(n1,np),dp221(n1,np),dp222(n1,np),
     2   dq211(n1,np),dq212(n1,np),dq221(n1,np),dq222(n1,np)

	real xt(n1,np),yt(n1,np),zt(n1,np),pxt(n1,np),pyt(n1,np),
     1	 pzt(n1,np),e1xt(n1,np),e1yt(n1,np),e1zt(n1,np),e2xt(n1,np),
     2	 e2yt(n1,np),e2zt(n1,np),p111t(n1,np),p112t(n1,np),
     3	 p121t(n1,np),p122t(n1,np),q111t(n1,np),q112t(n1,np),
     4   q121t(n1,np),q122t(n1,np),p211t(n1,np),p212t(n1,np),
     5	 p221t(n1,np),p222t(n1,np),q211t(n1,np),q212t(n1,np),
     6	 q221t(n1,np),q222t(n1,np),dxt(n1,np),dyt(n1,np),
     7   dzt(n1,np),dpxt(n1,np),dpyt(n1,np),dpzt(n1,np),
     8	 de1xt(n1,np),de1yt(n1,np),de1zt(n1,np),de2xt(n1,np),
     9	 de2yt(n1,np),de2zt(n1,np),dp111t(n1,np),dp112t(n1,np)
	real dp121t(n1,np),dp122t(n1,np),dq111t(n1,np),dq112t(n1,np),
     1	 dq121t(n1,np),dq122t(n1,np),dp211t(n1,np),dp212t(n1,np),
     2	 dp221t(n1,np),dp222t(n1,np),dq211t(n1,np),dq212t(n1,np),
     3	 dq221t(n1,np),dq222t(n1,np)

	PI = 3.14159265

	do ip=1,np
		xs = xsp(ip)
		ys = ysp(ip)
c 	reduce the velocity model according to source and output
	    	temp = fxo
	    	if(xs.lt.temp) temp = xs
	    	if(xs-aperx.gt.temp) temp = xs-aperx
	    	nx0 = (temp-fx)/dx
	    	fxt = fx+nx0*dx
	    	temp = fxo+(nxo-1)*dxo
	    	if(xs.gt.temp) temp = xs
	    	if(xs+aperx.lt.temp) temp = xs+aperx
	    	tmp = 1+((temp-fx)/dx+0.99)-nx0
	    	nxt = tmp
	    	ext = fxt+(nxt-1)*dx
	    	temp = fyo
	    	if(ys.lt.temp) temp = ys
	    	if(ys-apery.gt.temp) temp = ys-apery
	    	ny0 = (temp-fy)/dy
	    	fyt = fy+ny0*dy
	    	temp = fyo+(nyo-1)*dyo
	    	if(ys.gt.temp) temp = ys
	    	if(ys+apery.lt.temp) temp = ys+apery
	    	tmp = 1+((temp-fy)/dy+0.99)-ny0
		nyt = tmp
	    	eyt = fyt+(nyt-1)*dy
c	determine range of azimuth angle	
		azhn = azhmin
	    	azhx = azhmax
	    	if(xs.eq.fxt) then
			if(azhx.gt.PI)
     1				azhx = PI
			if(ys.eq.fyt .and. 
     1				azhx.gt.0.5*PI) 
     2 					azhx = 0.5*PI
			if(ys.eq.eyt .and. 
     1				azhn.lt.0.5*PI) 
     2					azhn = 0.5*PI
	    	else if(xs.eq.ext) then
			if(azhn.lt.PI) azhn = PI
			if(ys.eq.fyt .and. 
     1				azhn.lt.1.5*PI) 
     2					azhn=1.5*PI
			if(ys.eq.eyt .and.
     1				azhx.lt.1.5*PI) 
     2					azhx = 1.5*PI
	    	else if(ys.eq.eyt) then
		 	if(azhn.lt.0.5*PI) 
     1				azhn = 0.5*PI
		 	if(azhx.gt.1.5*PI) 
     1				azhx = 1.5*PI
   	    	end if
		azhnp(ip) = azhn
		azhxp(ip) = azhx
		nxtp(ip) = nxt
		nytp(ip) = nyt
		fxtp(ip) = fxt
		fytp(ip) = fyt

		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,v,vt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vxx,vxxt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vxy,vxyt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vyy,vyyt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vxz,vxzt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vyz,vyzt(1,ip))
		call trans(ny,nx,nz,nyt,nxt,ny0,nx0,vzz,vzzt(1,ip))

	end do
cccc	  compute travel time by paraxial ray tracing 
cccc parallel loop
ccc--convex---$dir force_parallel
ccc--sgi--ccc$	call mp_set_numthreads(np)
ccc--sgi--ccc$doacross 
ccc$par doall private(ip)
ccc$par doall shared(xsp,ysp,azhnp,azhxp,t)
ccc$par doall shared(nxtp,nytp,fxtp,fytp,vt,vxxt)
ccc$par doall shared(vxyt,vxzt,vyyt,vyzt,vzzt,s)
ccc$par doall shared(na,nt,nxo,nyo,nzo,fa,da,amin,amax,dt,fxo,fyo,fzo)
ccc$par doall shared(dxo,dyo,dzo,fac,nz,fz,dx,dy,dz)
ccc$par doall readonly(na,nt,nxo,nyo,nzo,fa,da,amin,amax,dt,fxo,fyo,fzo)
ccc$par doall readonly(dxo,dyo,dzo,fac,nz,fz,dx,dy,dz)
c$par doall
	do ip=1,np
 	   call raytime(na,nt,nxo,nyo,nzo,xsp(ip),ysp(ip),fa,da,amin,amax,
     1   azhnp(ip),azhxp(ip),dt,fxo,fyo,fzo,dxo,dyo,dzo,fac,t(1,ip),    
     2 	 nxtp(ip),nytp(ip),nz,fxtp(ip),fytp(ip),fz,dx,dy,dz,vt(1,ip),
     3	 vxxt(1,ip),vxyt(1,ip),vxzt(1,ip),vyyt(1,ip),vyzt(1,ip),
     4   vzzt(1,ip),s(1,ip),xp(1,ip),yp(1,ip),zp(1,ip),pxp(1,ip),
     5	 pyp(1,ip),pzp(1,ip),e1xp(1,ip),e1yp(1,ip),e1zp(1,ip),
     6   e2xp(1,ip),e2yp(1,ip),e2zp(1,ip),q111p(1,ip),q112p(1,ip),
     7	 q121p(1,ip),q122p(1,ip),p211p(1,ip),p212p(1,ip),p221p(1,ip),
     8	 p222p(1,ip),q211p(1,ip),q212p(1,ip),q221p(1,ip),q222p(1,ip),
     9   vp(1,ip),dvdxp(1,ip),dvdyp(1,ip),dvdzp(1,ip),nrsp(1,ip),
     a	 a0p(1,ip),azh0p(1,ip),n1,n2,p2p(1,ip),q2p(1,ip),hp(1,ip),
     b	 gradvp(1,ip),d2tp(1,ip),i2,i3,i6,
     c   map(1,ip),vs(1,ip),dvdxs(1,ip),dvdys(1,ip),dvdzs(1,ip),
     d   uxx(1,ip),uxy(1,ip),uxz(1,ip),uyy(1,ip),uyz(1,ip),uzz(1,ip),
     e   tzt(1,ip),xx(1,ip),yy(1,ip),zz(1,ip),pxs(1,ip),pys(1,ip),
     f	 pzs(1,ip),e1xs(1,ip),e1ys(1,ip),e1zs(1,ip),e2xs(1,ip),
     g	 e2ys(1,ip),e2zs(1,ip),p111s(1,ip),p112s(1,ip),p121s(1,ip),
     h	 p122s(1,ip),q111s(1,ip),q112s(1,ip),q121s(1,ip),q122s(1,ip),
     i	 p211s(1,ip),p212s(1,ip),p221s(1,ip),p222s(1,ip),q211s(1,ip),
     j	 q212s(1,ip),q221s(1,ip),q222s(1,ip),dxs(1,ip),dys(1,ip),
     k	 dzs(1,ip),dpxs(1,ip),dpys(1,ip),dpzs(1,ip),de1x(1,ip),
     l	 de1y(1,ip),de1z(1,ip),de2x(1,ip),de2y(1,ip),de2z(1,ip),
     m   dp111(1,ip),dp112(1,ip),dp121(1,ip),dp122(1,ip),dq111(1,ip),
     n	 dq112(1,ip),dq121(1,ip),dq122(1,ip),dp211(1,ip),dp212(1,ip),
     o	 dp221(1,ip),dp222(1,ip),dq211(1,ip),dq212(1,ip),dq221(1,ip),
     p	 dq222(1,ip),xt(1,ip),yt(1,ip),zt(1,ip),pxt(1,ip),pyt(1,ip),
     q	 pzt(1,ip),e1xt(1,ip),e1yt(1,ip),e1zt(1,ip),e2xt(1,ip),
     r	 e2yt(1,ip),e2zt(1,ip),p111t(1,ip),p112t(1,ip),p121t(1,ip),
     s	 p122t(1,ip),q111t(1,ip),q112t(1,ip),q121t(1,ip),q122t(1,ip),
     t   p211t(1,ip),p212t(1,ip),p221t(1,ip),p222t(1,ip),q211t(1,ip),
     u	 q212t(1,ip),q221t(1,ip),q222t(1,ip),dxt(1,ip),dyt(1,ip),
     v   dzt(1,ip),dpxt(1,ip),dpyt(1,ip),dpzt(1,ip),de1xt(1,ip),
     w	 de1yt(1,ip),de1zt(1,ip),de2xt(1,ip),de2yt(1,ip),de2zt(1,ip),
     x   dp111t(1,ip),dp112t(1,ip),dp121t(1,ip),dp122t(1,ip),
     y	 dq111t(1,ip),dq112t(1,ip),dq121t(1,ip),dq122t(1,ip),
     z   dp211t(1,ip),dp212t(1,ip),dp221t(1,ip),dp222t(1,ip),
     &   dq211t(1,ip),dq212t(1,ip),dq221t(1,ip),dq222t(1,ip))

	end do
cccc     make up in shadow zone by eikonal equation
 	if (ek.eq.1) then 
cccc$dir force_parallel
cccc$	call mp_set_numthreads(np)
cccc$doacross 
c$par doall
		do ip=1,np
			call eiknl(t(1,ip),ov2,tt1(1,1,ip),
     1			   tt2(1,1,ip),nxo,nyo,nzo,
     2		           dxo,dyo,dzo,tmax)
		end do
	end if
	return
	end
