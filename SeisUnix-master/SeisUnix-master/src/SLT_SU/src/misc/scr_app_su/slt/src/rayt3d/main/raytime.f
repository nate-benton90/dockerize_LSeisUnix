c*****************************************************************************
   
	subroutine raytime(na,nt,nxo,nyo,nzo,xs,ys,fa,da,amin,amax,
     1	  azhmin,azhmax,dt,fxo,fyo,fzo,dxo,dyo,dzo,fac,t,
     2	  nx,ny,nz,fx,fy,fz,dx,dy,dz,v3,
     3    vxx,vxy,vxz,vyy,vyz,
     4	  vzz,s,x,y,z,px,
     5	  py,pz,e1x,e1y,e1z,
     6	  e2x,e2y,e2z,q111,q112,
     7	  q121,q122,p211,p212,p221,
     8	  p222,q211,q212,q221,q222,
     9    v,dvdx,dvdy,dvdz,nrs,
     a	  a0,azh0,n1,n2,p2,q2,h,
     b	  gradv,d2t,i2,i3,i6,
     c    map,vs,dvdxs,dvdys,dvdzs,
     d    uxx,uxy,uxz,uyy,uyz,uzz,
     e	  tzt,xx,yy,zz,pxs,pys,
     f	  pzs,e1xs,e1ys,e1zs,e2xs,
     g	  e2ys,e2zs,p111s,p112s,p121s,
     h	  p122s,q111s,q112s,q121s,q122s,
     i    p211s,p212s,p221s,p222s,q211s,
     j	  q212s,q221s,q222s,dxs,dys,
     k	  dzs,dpxs,dpys,dpzs,de1x,
     l	  de1y,de1z,de2x,de2y,de2z,
     m	  dp111,dp112,dp121,dp122,dq111,
     n	  dq112,dq121,dq122,dp211,dp212,
     o	  dp221,dp222,dq211,dq212,dq221,
     p	  dq222,xt,yt,zt,pxt,pyt,
     q	  pzt,e1xt,e1yt,e1zt,e2xt,
     r	  e2yt,e2zt,p111t,p112t,p121t,
     s	  p122t,q111t,q112t,q121t,q122t,
     t	  p211t,p212t,p221t,p222t,q211t,
     u	  q212t,q221t,q222t,dxt,dyt,
     v    dzt,dpxt,dpyt,dpzt,de1xt,
     w	  de1yt,de1zt,de2xt,de2yt,de2zt,
     x    dp111t,dp112t,dp121t,dp122t,
     y	  dq111t,dq112t,dq121t,dq122t,
     z    dp211t,dp212t,dp221t,dp222t,
     &	  dq211t,dq212t,dq221t,dq222t)
     

c*****************************************************************************
c  calculate traveltimes by paraxial ray mehtod.  
c*****************************************************************************
c Input:
c  nxo,nyo,nzo,fxo,fyo,fzo,dxo,dyo,dzo	
c		grid parameters of traveltime
c  nx,ny,nz,fx,fy,fz,dx,dy,dz	
c		grid parameters of velocity
c  v3 		sampled velocity   ------ array(nz,ny,nx)
c  vxx,vxy,vxz,vyy,vyz,vzz         ------ array(nz,ny,nx)
c		sampled second-order derivatives of velocity
c  xs		x coordinate of source point
c  ys		y coordinate of source point
c  fa		first polar angle sample of central rays
c  da		polar angle sampling interval of central rays
c  na           number of polar angle of central rays
c  amin		lower limit of emergence polar angle
c  amax		upper limit of emergence polar angle
c  azhmin	minimum azimuth angle at takeoff point
c  azhmax	maximum azimuth angle at takeoff point
c  nt		number of time samples in ray tracing 
c  dt		time sampling interval in ray tracing
c  fac          factor to determine radius for extrapolation	 
c               ( ratio between 1/2 (d2t/dX2) *X2 and t0 )
c
c Output:	traveltime t(nzo,nyo,nxo)
c Work array	length of ray path s(nzo,nyo,nxo)
c****************************************************************************
c Note: when traveltime has multiple values, the one that has the shortest
c       ray path is chosen
c****************************************************************************
  


 	real v3(nz*ny*nx),vxx(nz*ny*nx),vxy(nz*ny*nx)
	real vxz(nz*ny*nx),vyy(nz*ny*nx),vyz(nz*ny*nx)
	real vzz(nz*ny*nx),t(nzo*nyo*nxo),s(nzo*nyo*nxo)
									

ccc	parameter (n1=128, n2=n1*1001)
ccc	parameter (i2=2,i3=3,i6=6)

	real x(n2),y(n2),z(n2),px(n2),py(n2),pz(n2),
     1	  e1x(n2),e1y(n2),e1z(n2),e2x(n2),e2y(n2),e2z(n2),
     2	  q111(n2),q112(n2),q121(n2),q122(n2),p211(n2),p212(n2),
     3    p221(n2),p222(n2),q211(n2),q212(n2),q221(n2),q222(n2),
     4    v(n2),dvdx(n2),dvdy(n2),dvdz(n2)
	integer	nrs(n1)
	real a0(n1),azh0(n1)
 
 	integer ia,nazh,iazh,ixo,iyo,izo,ir,nr 
	real   	a,azh,dazh,xo,yo,zo,exo,eyo,ezo,odxo,odyo,odzo,pi 
  
c	variables used for extrapolation
	integer	it,jr,nxf,nxe,nyf,nye,nzf,nze	 
  	real detq2,tc,xoc,yoc,zoc,xc,yc,zc,r1,v0,norm2,terr,vd1,cuv,
     1	  vd,pxd,pyd,pzd,r2,t1,t2,r1x,r2x,r1y,t1x,t1y,t2x,t2y,t2xy

	real  p2(i2,i2),q2(i2,i2),h(i3,i3),gradv(i3),d2t(i6)

	integer map(n1)
	real vs(n1),dvdxs(n1),dvdys(n1),dvdzs(n1),uxx(n1),uxy(n1),uxz(n1),
     1   uyy(n1),uyz(n1),uzz(n1),tzt(n1)

	real xx(n1),yy(n1),zz(n1),pxs(n1),pys(n1),pzs(n1),
     1    e1xs(n1),e1ys(n1),e1zs(n1),e2xs(n1),e2ys(n1),e2zs(n1),
     2    p111s(n1),p112s(n1),p121s(n1),p122s(n1),q111s(n1),q112s(n1),
     3    q121s(n1),q122s(n1),p211s(n1),p212s(n1),p221s(n1),p222s(n1),
     4    q211s(n1),q212s(n1),q221s(n1),q222s(n1),
     5    dxs(n1),dys(n1),dzs(n1),dpxs(n1),dpys(n1),dpzs(n1),
     6    de1x(n1),de1y(n1),de1z(n1),de2x(n1),de2y(n1),de2z(n1),
     7    dp111(n1),dp112(n1),dp121(n1),dp122(n1),dq111(n1),dq112(n1),
     8    dq121(n1),dq122(n1),dp211(n1),dp212(n1),dp221(n1),dp222(n1),
     9    dq211(n1),dq212(n1),dq221(n1),dq222(n1)

	real xt(n1),yt(n1),zt(n1),pxt(n1),pyt(n1),pzt(n1),
     1    e1xt(n1),e1yt(n1),e1zt(n1),e2xt(n1),e2yt(n1),e2zt(n1),
     2    p111t(n1),p112t(n1),p121t(n1),p122t(n1),q111t(n1),q112t(n1),
     3    q121t(n1),q122t(n1),p211t(n1),p212t(n1),p221t(n1),p222t(n1),
     4    q211t(n1),q212t(n1),q221t(n1),q222t(n1),dxt(n1),dyt(n1),
     5    dzt(n1),dpxt(n1),dpyt(n1),dpzt(n1),de1xt(n1),de1yt(n1),
     6    de1zt(n1),de2xt(n1),de2yt(n1),de2zt(n1),dp111t(n1),dp112t(n1),
     7    dp121t(n1),dp122t(n1),dq111t(n1),dq112t(n1),dq121t(n1),
     8    dq122t(n1),dp211t(n1),dp212t(n1),dp221t(n1),dp222t(n1),
     9    dq211t(n1),dq212t(n1),dq221t(n1),dq222t(n1) 
								 
 	
	nr = n1
	ir = 1
	pi = 3.1415926

c  maximum ranges of output points  
	exo = fxo+(nxo-1)*dxo 
	eyo = fyo+(nyo-1)*dyo 
	ezo = fzo+(nzo-1)*dzo 

c	call msgscf("fxo=",fxo)
c	call msgsci("nxo=",nxo)
c	call msgscf("dxo=",dxo)
c	call msgscf("fyo=",fyo)
c	call msgsci("nyo=",nyo)
c	call msgscf("dyo=",dyo)
c	call msgscf("fzo=",fzo)
c	call msgsci("nzo=",nzo)
c	call msgscf("dzo=",dzo)

	odxo = 1.0/dxo 
	odyo = 1.0/dyo 
	odzo = 1.0/dzo 
  	
c	initialize traveltimes	 
  	do 10 ixo=0,nxo-1 
	do 10 iyo=0,nyo-1
		i = ixo*nyo*nzo+iyo*nzo 
	    	do 20 izo=1,nzo
 			t(i+izo) = 999999.0*999999.0 
			s(i+izo) = 999999.0 
20		end do
10	end do
 
  
c	loop over emgergence angles at source point 
 	do 1000 ia=1,na
 	    a = fa+(ia-1)*da 
ccccc		call msgsci(" ia= ",ia)
c	    determine increment and number of azimuth angles  
	    if(a .eq. 0.0 .or. a. gt. PI-0.001) then
			nazh = 1 
	    else  
			nazh = 1+NINT((azhmax-azhmin)*sin(a)/da)
			if(nazh .gt. 1) then
				dazh = (azhmax-azhmin)/(nazh-1)
			else 
				dazh = 0
			end if 
								
		    if(azhmax-azhmin .gt. 2*pi-0.001 .and. nazh .gt. 1) then
				nazh = nazh-1
			end if 
	    end if

ccccc		call msgsci(" nazh= ",nazh)
 
	    do 1100 iazh=1,nazh
		azh = azhmin+(iazh-1)*dazh
ccccc		call msgsci(" iazh= ",iazh)
ccccc		call msgsci(" ir= ",ir)
 		a0(ir) = a 
		azh0(ir) = azh 
  
		if(ir .lt. nr .and. iazh+ia .lt. nazh+na) then
			ir = ir+1
			goto 1100
		end if	 
		 	
		if(ir .lt. nr)  nr = ir 

ccccc		call msgsci(" nr= ",nr)
 								 
c	 trace rays
	call makeray(nx,ny,nz,fx,fy,fz,dx,dy,dz,v3,vxx,vxy,vxz,
     1    vyy,vyz,vzz,xs,ys,0.0,a0,azh0,amin,amax,nr,nt,dt,
     2    x,y,z,px,py,pz,e1x,e1y,e1z,e2x,e2y,e2z,
     3	  q111,q112,q121,q122,p211,p212,p221,p222,
     4    q211,q212,q221,q222,v,dvdx,dvdy,dvdz,nrs,
     5    map,vs,dvdxs,dvdys,dvdzs,uxx,uxy,uxz,uyy,uyz,uzz,tzt,
     6    xx,yy,zz,pxs,pys,pzs,e1xs,e1ys,e1zs,e2xs,e2ys,e2zs,
     7    p111s,p112s,p121s,p122s,q111s,q112s,
     8	  q121s,q122s,p211s,p212s,p221s,p222s,
     9    q211s,q212s,q221s,q222s,dxs,dys,dzs,
     a	  dpxs,dpys,dpzs,de1x,de1y,de1z,de2x,
     b    de2y,de2z,dp111,dp112,dp121,
     c	  dp122,dq111,dq112,dq121,dq122,
     d    dp211,dp212,dp221,dp222,dq211,dq212,
     e	  dq221,dq222,xt,yt,zt,pxt,pyt,pzt,
     f	  e1xt,e1yt,e1zt,e2xt,e2yt,e2zt,
     g    p111t,p112t,p121t,p122t,
     h    q111t,q112t,q121t,q122t,p211t,
     i    p212t,p221t,p222t,q211t,
     j	  q212t,q221t,q222t,dxt,dyt,
     k    dzt,dpxt,dpyt,dpzt,de1xt,
     l	  de1yt,de1zt,de2xt,de2yt,de2zt,
     m    dp111t,dp112t,dp121t,dp122t,
     n	  dq111t,dq112t,dq121t,dq122t,
     o    dp211t,dp212t,dp221t,dp222t,
     p    dq211t,dq212t,dq221t,dq222t,n1)

ccc		call msgsci(" after makeray nr= ",nr)

c	    extrapolate to grids near central rays	
		do 1200 ir=1,nr 
  		    v0 = v(ir) 
 		    r2 = 0.0 
 		    xc = xs
		    yc = ys
		    zc = 0.0 
		    sd = 0.0
 		    vd1 = v0
c			call msgsci(" ir= ",ir)
c			call msgsci(" nrs= ",nrs(ir))
c		write(*,*) "ir=", ir," nrs=",nrs(ir)

			mrs = nrs(ir)
  		    do 1300 it=2,mrs
ccc  		    do 1300 it=2,nrs(ir) 
			jr = (it-1)*nr+ir
ccc			if(jr.gt.128128) call msgsci(" jr= ",jr)
 			xo = x(jr)
			yo = y(jr)
			zo = z(jr)
 			vd = v(jr)
   			sd = sd+0.5*dt*(vd+vd1)
 			vd1 = vd 
  
c	 	    if the point is within the previous sphere	 
 			if(r2 .gt. (xc-xo)*(xc-xo)+(yc-yo)*(yc-yo)
     1			  +(zc-zo)*(zc-zo) ) then
ccc				call msgsci("within it=",it)
c 				if (it .ne. nrs(ir)-1) write(*,*) "within it=",it
 				if (it .ne. nrs(ir)-1) goto 1300
c				goto 1300
			end if

c		 if the point is out of output range  
			if(xo .lt. fxo .or. xo .gt. exo .or.  
     1			   yo .lt. fyo .or. yo .gt. eyo .or.
     2			   zo .lt. fzo .or. zo .gt. ezo) then
c				if(zo.ge.fzo .and. zo.le.ezo) then
c				call msgscf("outside xo=",xo)
c				call msgscf("outside yo=",yo)
c				call msgscf("outside zo=",zo)
c				end if
c				write(*,*) "outside"
    				goto 1300
			end if
 
 			xc = xo
			yc = yo
			zc = zo 
 			q2(1,1) = q211(jr) 
			q2(2,1) = q221(jr)
			q2(1,2) = q212(jr)
			q2(2,2) = q222(jr)
			detq2 = q2(1,1)*q2(2,2)-q2(1,2)*q2(2,1) 
 
c		if caustic	 
			if(detq2 .eq. 0.0) then
				r2 = 0.0 
ccccc			call msgscf(" caustic detq2=",detq2)
c				write(*,*) "caustic"
				goto 1300
 			end if 

 			vd = v(jr)
			pxd = px(jr)
			pyd = py(jr)
			pzd = pz(jr)

			p2(1,1) = p211(jr)
			p2(2,1) = p221(jr)
			p2(1,2) = p212(jr)
			p2(2,2) = p222(jr)
 			h(1,1) = e1x(jr)
			h(2,1) = e1y(jr)
			h(3,1) = e1z(jr)
			h(1,2) = e2x(jr) 
			h(2,2) = e2y(jr)
			h(3,2) = e2z(jr)
			h(1,3) = vd*pxd
			h(2,3) = vd*pyd
			h(3,3) = vd*pzd
			gradv(1) = dvdx(jr)
			gradv(2) = dvdy(jr)
			gradv(3) = dvdz(jr)
 
c     		calculate second derivatives of traveltime	 
  			call ddt(p2,q2,h,gradv,vd,d2t,cuv,2,3,6) 
  
c		determine radius for extrapolation	 
			tc = float(it-1)*dt 
			terr = tc*fac 
  			norm2 = sqrt(d2t(1)*d2t(1)+d2t(4)*d2t(4)+
     1				  d2t(6)*d2t(6)+2.0*(d2t(2)*d2t(2)+
     2				  d2t(3)*d2t(3)+d2t(5)*d2t(5))) 
  
			r2 = terr/norm2 
 			r1 = sqrt(r2) 
 
c		keep ray-centered coordinate system regular
			if(r1*cuv .gt. 0.1) r1 = 0.1/cuv

c 		the radius cannot be too large 
			if(r1 .gt. 0.1*sd) r1 = 0.1*sd 

			r2 = r1*r1
  
 			nxf = (xc-r1-fxo)*odxo+0.9999 
			if(nxf .lt. 0) nxf = 0
			nxe = (xc+r1-fxo)*odxo+0.0001 
			if(nxe .ge. nxo) nxe = nxo-1 

cc			call msgsci(" ixo nxf=",nxf)
cc			call msgsci(" ixo nxe=",nxe)
cc			write(*,*) "nxf=",nxf," nxe=",nxe," ir=",ir
 
 			do 1400 ixo=nxf,nxe
			    xoc = fxo-xc+ixo*dxo 
 			    r2x = r2-xoc*xoc 
 			    if(r2x .lt. 0) goto 1400

			    r1x = sqrt(r2x)
			    t1x = tc+xoc*pxd 
			    t2x = tc*xoc*xoc*d2t(1)
 
			    nyf = (yc-r1x-fyo)*odyo+0.9999
			    if(nyf .lt. 0) nyf = 0
			    nye = (yc+r1x-fyo)*odyo+0.0001
			    if(nye .ge. nyo) nye = nyo-1 

cccc			    write(*,*) "nyf=",nyf," nye=",nye," ir=",ir
 			    do 1500 iyo=nyf,nye
				yoc = fyo-yc+iyo*dyo
 
c			  if output point is out of limit	 
			     	if(yoc*yoc .gt. r2x) goto 1500

 				r1y = sqrt(r2x-yoc*yoc)  
				t1y = t1x+yoc*pyd 
				t2y = t2x+tc*yoc*(yoc*d2t(4)
     1					+2.0*xoc*d2t(2)) 
				t2xy = 2.0*(xoc*d2t(3)+yoc*d2t(5)) 
									 
				nzf = (zc-r1y-fzo)*odzo+1.99 
				if(nzf .lt. 1) nzf = 1
				nze = (zc+r1y-fzo)*odzo+1.01 
				if(nze .gt. nzo) nze = nzo 

				i = ixo*nyo*nzo+iyo*nzo			
				do 1600 izo=nzf,nze
c				 if raypath shorter! 
				    if(sd .lt. s(i+izo)) then
				        zoc = fzo-zc+(izo-1)*dzo
 				        t1 = t1y+zoc*pzd 
				        t2 = t1*t1+t2y+tc*zoc
     1					   *(zoc*d2t(6)+t2xy)
 				    	s(i+izo) = sd
 				    	t(i+izo) = t2 
				    end if
1600 			end do
1500		end do
1400	   	end do
1300   	end do
1200	end do

		if(iazh+ia .lt. nazh+na) ir = 1 

1100	end do
1000	end do 

c   square root of traveltimes	*/
  	do 2000 ixo=0,nxo-1 
	do 2000 iyo=0,nyo-1
		i = ixo*nyo*nzo+iyo*nzo 
	    	do 2100 izo=1,nzo
			t(i+izo) = max(0.0,t(i+izo))
 			t(i+izo) = sqrt(t(i+izo))
2100		end do
2000	end do

c   compute traveltimes near the source
	nxf = (xs-fxo)/dxo+0.5
	nxf = max0(nxf,1)
	nxe = (xs-fxo)/dxo+2.5
	nxe = min0(nxe,nxo)
	nyf = (ys-fyo)/dyo+0.5
	nyf = max0(nyf,1)
	nye = (ys-fyo)/dyo+2.5
	nye = min0(nye,nyo)
	nzf = -fzo/dzo+0.5
	nzf = max0(nzf,1)
	nze = -fzo/dzo+2.5
	nze = min0(nze,nzo)
  	do 3000 ixo=nxf,nxe 
	do 3000 iyo=nyf,nye
		xo = fxo+(ixo-1)*dxo
		yo = fyo+(iyo-1)*dyo
		i = (ixo-1)*nyo*nzo+(iyo-1)*nzo 
	    	do 3100 izo=nzf,nze
			zo = fzo+(izo-1)*dzo
  			t(i+izo) = sqrt((xo-xs)*(xo-xs)+(yo-ys)*(yo-ys)
     1				+zo*zo)/v0
3100  end do
3000  end do
 
	    
	return
	end 


c*************************************************************************
c  Calculate second derivatives of traveltime with respect to x,y,z.
c*************************************************************************
c input:
c    p,q  --- extrapolation matrice computed from makeray
c    h    --- rotation matric between ray coordinates and (x,y,z) 
c    dv   --- dv/dx,dv/dy,dv/dz
c    v    --- velocity at this point
c output:
c    d2t  --- second derivative of t (xx,xy,xz,yy,yz,zz)
c    cuv  --- curvature of ray at this point 

 	subroutine ddt(p,q,h,dv,v,d2t,cuv,n2,n3,n6)

 	real  	odetq,o2v,qi11,qi12,qi21,qi22,
     1		m11,m12,m22,m13,m23,m33,a1,a2,a3
	real	p(n2,n2),q(n2,n2),h(n3,n3),dv(n3),d2t(n6) 
	integer n2, n3, n6

 	o2v = 1.0/(v*v) 

c	calculate inverse of determint of q	 
	odetq = 1.0/(q(1,1)*q(2,2)-q(1,2)*q(2,1)) 

c	 calculate inverse of q	 
	qi11 = q(2,2)*odetq 
	qi21 = -q(2,1)*odetq 
	qi12 = -q(1,2)*odetq 
	qi22 = q(1,1)*odetq 
	
c	multiply p by inverse of q	 
	m11 = p(1,1)*qi11+p(1,2)*qi21 
	m12 = p(1,1)*qi12+p(1,2)*qi22 
	m22 = p(2,1)*qi12+p(2,2)*qi22 

c	calculate third column of m   
 	m13 = -(dv(1)*h(1,1)+dv(2)*h(2,1)+dv(3)*h(3,1))*o2v 
 	m23 = -(dv(1)*h(1,2)+dv(2)*h(2,2)+dv(3)*h(3,2))*o2v 
 	m33 = -(dv(1)*h(1,3)+dv(2)*h(2,3)+dv(3)*h(3,3))*o2v 
  
c	compute h*m*h^T	
 	a1 = m11*h(1,1)+m12*h(1,2)+m13*h(1,3)
 	a2 = m12*h(1,1)+m22*h(1,2)+m23*h(1,3)
 	a3 = m13*h(1,1)+m23*h(1,2)+m33*h(1,3)
	d2t(1) = h(1,1)*a1+h(1,2)*a2+h(1,3)*a3
 
 	a1 = m11*h(2,1)+m12*h(2,2)+m13*h(2,3)
 	a2 = m12*h(2,1)+m22*h(2,2)+m23*h(2,3)
 	a3 = m13*h(2,1)+m23*h(2,2)+m33*h(2,3)
	d2t(2) = h(1,1)*a1+h(1,2)*a2+h(1,3)*a3 
 	d2t(4) = h(2,1)*a1+h(2,2)*a2+h(2,3)*a3 

 	a1 = m11*h(3,1)+m12*h(3,2)+m13*h(3,3)
 	a2 = m12*h(3,1)+m22*h(3,2)+m23*h(3,3)
 	a3 = m13*h(3,1)+m23*h(3,2)+m33*h(3,3)
  	d2t(3) = h(1,1)*a1+h(1,2)*a2+h(1,3)*a3 
 	d2t(5) = h(2,1)*a1+h(2,2)*a2+h(2,3)*a3 
	d2t(6) = h(3,1)*a1+h(3,2)*a2+h(3,3)*a3 

c	compute the curvature of raypath
	cuv = sqrt(m13*m13+m23*m23)*v


	return
	end 

