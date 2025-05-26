   
	subroutine raytime(na,nt,nxo,nzo,xs,fa,da,amin,amax,dt,fxo,fzo,
     1	  dxo,dzo,fac,t,nx,nz,fx,fz,dx,dz,v2,vxx,vxz,vzz,s)
  
******************************************************************************
*  calculate traveltimes by paraxial ray mehtod.  
******************************************************************************
c Input:
c  nxo,nzo,fxo,fzo,dxo,dzo 	grid parameters of traveltime
c  nx,nz,fx,fz,dx,dz		grid parameters of velocity
c  v3 		sampled velocity   ------ array(nz,nx)
c  vxx,vxz,vzz  ------ array(nz,ny,nx)
c		sampled second-order derivatives of velocity
c  xs		x coordinate of source point
c  ys		y coordinate of source point
c  fa		first take-off angle sample of central rays
c  da		take-off angle sampling interval of central rays
c  na           number of take-off angles of central rays
c  amin		lower limit of emergence angle
c  amax		upper limit of emergence angle
c  nt		number of time samples in ray tracing 
c  dt		time sampling interval in ray tracing
c  fac          factor to determine radius for extrapolation	 
c               ( ratio between 1/2 (d2t/dX2) *X2 and t0 )
c
c Output:	traveltime t(nzo,nxo)
c Work array	length of ray path s(nzo,nxo)
******************************************************************************
* Note: when traveltime has multiple values, the one that has the shortest
*       ray path is chosen
******************************************************************************

 	real v2(*),vxx(*),vxz(*),vzz(*),t(*),s(*)
									
	parameter (n1=128, n2=n1*1001)
	real x(n2),z(n2),px(n2),pz(n2),q1(n2),q2(n2),p2(n2),
     1    v(n2),dvdx(n2),dvdz(n2)
	
	integer	nrs(n1)
	real a0(n1) 
 
 	integer ia,ixo,izo,ir,nr 
	real   	a,xo,zo,exo,ezo,odxo,odzo,pi 
  
c	variables used for extrapolation
	integer	it,jr,nxf,nxe,nzf,nze	 
  	real tc,xoc,zoc,xc,zc,r1,v0,norm2,terr,vd1,cuv,
     1	  vd,pxd,pzd,r2,t1,t2,r1x,r2x,t1x,t2x,t2xz,
     2	  p2d,q2d,gradv(2),d2t(3)
								 
	nr = n1
	ir = 1
	pi = 3.1415926

c  maximum ranges of output points  
	exo = fxo+(nxo-1)*dxo 
  	ezo = fzo+(nzo-1)*dzo 
 
	odxo = 1.0/dxo 
 	odzo = 1.0/dzo 
  	
c	initialize traveltimes	 
  	do 10 ixo=0,nxo-1 
 	    	do 20 izo=1,nzo
 			t(ixo*nzo+izo) = 999999.0*999999.0 
			s(ixo*nzo+izo) = 999999.0 
20		end do
10	end do
 
  
c	loop over emgergence angles at source point 
 	do 1000 ia=1,na
 	    	a = fa+(ia-1)*da 
   		a0(ir) = a 
   
		if(ir .lt. nr .and. ia .lt. na) then
			ir = ir+1
			goto 1000
		end if	 
		 	
		if(ir .lt. nr)  nr = ir 
 								 
c	 trace rays
		call makeray(nx,nz,fx,fz,dx,dz,v2,vxx,vxz,vzz,
     1  	    xs,0.0,a0,amin,amax,nr,nt,dt,x,z,px,pz,
     2	    	    q1,p2,q2,v,dvdx,dvdz,nrs)  

c	    extrapolate to grids near central rays	
 		do 1200 ir=1,nr 
   		    v0 = v(ir) 
 		    r2 = 0.0 
 		    xc = xs
 		    zc = 0.0 
		    sd = 0.0
 		    vd1 = v0
  		    do 1300 it=2, nrs(ir)
			jr = (it-1)*nr+ir
 			xo = x(jr)
 			zo = z(jr)
 			vd = v(jr)
   			sd = sd+0.5*dt*(vd+vd1)
  			vd1 = vd 
  
c	 	    if the point is within the previous circle	 
 			if(r2 .gt. (xc-xo)*(xc-xo) 
     1			  +(zc-zo)*(zc-zo)) then
				if (it .ne. nrs(ir)-1) goto 1300
			end if

c		 if the point is out of output range  
			if(xo .lt. fxo .or. xo .gt. exo .or.  
     1			   zo .lt. fzo .or. zo .gt. ezo) then
    				goto 1300
			end if
 
 			xc = xo
 			zc = zo 
 			q2d  = q2(jr) 
  
c		if caustic	 
			if(q2d .eq. 0.0) then
				r2 = 0.0 
				goto 1300
 			end if 
 
 			vd = v(jr)
			pxd = px(jr)
 			pzd = pz(jr)

			p2d = p2(jr)
 			sind = vd*pxd
 			cosd = vd*pzd
			gradv(1) = dvdx(jr)
 			gradv(2) = dvdz(jr)
 
c     		calculate second derivatives of traveltime	 
  			call ddt(p2d,q2d,cosd,sind,gradv,vd,d2t,cuv) 
   
c		determine radius for extrapolation	 
			tc = float(it-1)*dt 
			terr = tc*fac 
  			norm2 = sqrt(d2t(1)*d2t(1)+d2t(3)*d2t(3)+
     1      			2.0*d2t(2)*d2t(2)) 
  
			r2 = terr/norm2 
 			r1 = sqrt(r2) 
 
c		keep ray-centered coordinate system regular
			if(r1*cuv .gt. 0.1) r1 = 0.1/cuv

c 		the radius cannot be too large 
 			if(r1 .gt. 0.1*sd) r1 = 0.1*sd 
 
c 		the radius should not be too small near source 
c 			if(it .le. 10 .and. r1 .lt. vd*dt) r1 = vd*dt
								 
			r2 = r1*r1
  
 			nxf = (xc-r1-fxo)*odxo+0.9999 
			if(nxf .lt. 0) nxf = 0
			nxe = (xc+r1-fxo)*odxo+0.0001 
			if(nxe .ge. nxo) nxe = nxo-1 
 
 			do 1400 ixo=nxf,nxe
			    	xoc = fxo-xc+ixo*dxo 
 			    	r2x = r2-xoc*xoc 
 			    	if(r2x .lt. 0) goto 1400

			    	r1x = sqrt(r2x)
			    	t1x = tc+xoc*pxd 
			    	t2x = tc*xoc*xoc*d2t(1)
				t2xz = 2.0*xoc*d2t(2)
  								 
				nzf = (zc-r1x-fzo)*odzo+1.99 
				if(nzf .lt. 1) nzf = 1
				nze = (zc+r1x-fzo)*odzo+1.01 
				if(nze .gt. nzo) nze = nzo 

				i = ixo*nzo			
				do 1600 izo=nzf,nze
c				 if raypath shorter! 
				    if(sd .lt. s(i+izo)) then
				    	zoc = fzo-zc+(izo-1)*dzo
 				    	t1 = t1x+zoc*pzd 
				    	t2 = t1*t1+t2x+tc*zoc
     1					   *(zoc*d2t(3)+t2xz)
 			    		s(i+izo) = sd
 				    	t(i+izo) = t2 
				    end if
1600 				end do
1400		    	end do
1300		    end do
1200		end do

		if(ia .lt. na) ir = 1 

1000	end do 


c   square root of traveltimes	*/
  	do 2000 ixo=0,nxo-1 
 		i = ixo*nzo 
	    	do 2100 izo=1,nzo
			t(i+izo) = max(0.0,t(i+izo))
 			t(i+izo) = sqrt(t(i+izo))
2100		end do
2000	end do

c   compute traveltimes near the source
	nxf = (xs-fxo)/dxo-0.5
	nxf = max0(nxf,1)
	nxe = (xs-fxo)/dxo+3.5
	nxe = min0(nxe,nxo)
 	nzf = -fzo/dzo+0.5
	nzf = max0(nzf,1)
	nze = -fzo/dzo+2.5
   	do 3000 ixo=nxf,nxe 
 		xo = fxo+(ixo-1)*dxo
 		i = (ixo-1)*nzo 
	    	do 3100 izo=nzf,nze
			zo = fzo+(izo-1)*dzo
  			t(i+izo) = sqrt((xo-xs)*(xo-xs)+zo*zo)/v0
3100		end do
3000	end do
 
	    
	return
	end 


**************************************************************************
*  Calculate second derivatives of traveltime with respect to x,y,z.
**************************************************************************
 	subroutine ddt(p,q,c,s,dv,v,d2t,cuv)

 	real  	o2v,m11,m12,m22 
	real	dv(2),d2t(3) 

 	o2v = 1.0/(v*v) 

 	m11 = p/q  
 
c	calculate 2nd column of m   
 	m12 = -(dv(1)*c-dv(2)*s)*o2v 
  	m22 = -(dv(1)*s+dv(2)*c)*o2v 
  
c	compute h*m*h^T	
  	d2t(1) = m11*c*c+2.0*m12*c*s+m22*s*s 
   	d2t(2) = (m22-m11)*s*c+m12*(c*c-s*s) 
 	d2t(3) = m11*s*s-2.0*m12*c*s+m22*c*c 
 
c	compute the curvature of raypath
	cuv = abs(m12)*v

	return
	end 

 

  
	
