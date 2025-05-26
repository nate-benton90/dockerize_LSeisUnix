cccccc
c event-mapping migration
c
c author:	zhiming li	      	1/28/93
cccccc 
	subroutine emig(nx,nz,x0,dx,mig,nt,t0,dt,tmute,trace,scale,
     1		ne,xe,ze,te,zmig,tm,fold,index,mwd,sx,gx,ixlive,
     2		mapmax,aper)

	real x0, dx, t0 , dt, tm, odx, odt
	real mig(nz,nx), trace(nt)
	real fold(nz,nx), scale(mwd)
	real xe(ne), ze(ne), te(ne),zmig(nz),sx,gx
	integer nx, nz, nt, ne, index(nz), mwd, ixlive(nx)
	real h2, h, z02, xx2, xx, zz, z
	real odz, zmax, z0, dz, zlarge,aper
	integer mh,mapmax

cccc
	if(ne.le.0) return
	z0 = zmig(1)
	dz = zmig(2) - zmig(1)
	odz = 1./dz
	odx = 1./dx
	odt = 1./dt
	tmax = t0 + (nt-1)*dt
	xmax = x0 + (nx-1)*dx
	xmh = mwd/2 * dx
	zmax = z0 + (nz-1)*dz
	zlarge = zmax + dz
	mh = (mwd+1)/2 
	if(mwd.gt.1) then
	   sum = 0.
	   do i=1,mwd
              scale(i) = mh - abs(i-mh)
              sum = sum + scale(i)
           end do
           sum = 1./sum
           do i=1,mwd
              scale(i)= scale(i)*sum
           end do
	end if
	xm = 0.5 * (sx + gx)
	h = 0.5 * (sx-gx)
	h2 = h * h 
ccc
	iend = ne + 1
	if(mapmax.eq.0) iend = ne

	do ie=1,iend
	   z1 = 0.
	   z2 = 0.
	   if(ie.eq.1 .and. ze(1).gt.z0) then
	      x1 = xm
	      z1 = 0.
	      t1 = tm 
	      x2 = xe(1)
	      z2 = ze(1)
	      t2 = te(1)
	   else if(ie.eq.ne+1 .and. ze(ne).lt.zmax) then
	      x1 = xe(ne)
	      z1 = ze(ne)
	      t1 = te(ne)
	      if (ne.eq.1) then
	         x2 = xm + (zmax)*(x1-xm)/(z1)
		 z2 = zmax
		 t2 = tm + (zmax)*(t1-tm)/(z1)
	      else 
		 x2 = xe(ne-1) + (zmax-ze(ne-1))*
     1			(x1-xe(ne-1))/(z1-ze(ne-1))
		 z2 = zmax
		 t2 = te(ne-1) + (zmax-te(ne-1))*
     1			(t1-te(ne-1))/(z1-ze(ne-1))
	      end if
	   else if(ie.gt.1 .and. ie.le.ne) then
	      x1 = xe(ie-1)
	      z1 = ze(ie-1)
	      t1 = te(ie-1)
	      x2 = xe(ie)
	      z2 = ze(ie)
	      t2 = te(ie)
	   end if
	   zdis = z2 - z1
	   tmp = zdis * odz
	   np = tmp
	   if (tmp.gt.0.0) then
	      idp = 1
	   else
	      idp = -1
	   end if
           if(zdis.ne.0.) then 	
	      slopex = (x2-x1)/zdis 
	      slopet = (t2-t1)/zdis
	      p0 = (z1-z0)*odz + 1.0
	      ip0 = p0
	      ipn = ip0+np-1
	      ip0 = max0(ip0,1)
	      ip0 = min0(ip0,nz)
	      ipn = min0(ipn,nz)
	      ipn = max0(ipn,1)
	      do ip=ip0,ipn,idp
	         z = z0 + (ip-1)*dz
		 x = x1 + (z-z1)*slopex 
		 t = t1 + (z-z1)*slopet 
		 if (t.ge.tmute .and. t.lt.tmax .and. x.ge.x0 .and.
     1		     x.le.xmax .and. abs(x-xm).le.aper) then 

		    t = (t - t0)*odt + 1.
		    it = t
		    res = t - it
		    amp = trace(it)+res*(trace(it+1)-trace(it))
		    if (mwd.gt.1) then
		       z02 = z*z
		       do ix=1,mwd
		          xx = x-xmh+(ix-1)*dx 
			  xx2 = (xx - xm) 
			  xx2 = xx2 * xx2
			  zz = z02-xx2/(z02+h2) 
			  if (zz.ge.0.) then
			     zz = sqrt(zz)
			  else 
			     zz = zlarge 
			  end if
			  xx = (xx-x0)*odx+1.
			  ixx = xx
			  zz=(zz-z0)*odz+1.
			  izz=zz
			  if (izz.ge.1 .and. izz.le.nz .and.
     1                        ixx.ge.1 .and. ixx.le.nx ) then 

			     mig(izz,ixx) = mig(izz,ixx) + 
     1						amp*scale(ix)
			     fold(izz,ixx) = fold(izz,ixx) + 1.
			     ixlive(ixx) = 1
			  end if
		       end do
		    else 
		       x = (x - x0)*odx + 1.
		       ix = x
	               mig(ip,ix) = mig(ip,ix) + amp 
		       fold(ip,ix)= fold(ip,ix) + 1.
		       ixlive(ix) = 1
		    end if
		 end if
	      end do
	   end if
	end do
	return
	end
