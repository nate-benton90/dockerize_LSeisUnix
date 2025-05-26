c author: zhiming li	8/94
c
c convert a landmark file to grid file 
c 
c        xs, ys, ts --- are tripletes of np data points from landmark
c        fx,dx,nx,fy,dy,ny --- define grid geometry
c        grids --- interpolated ts on grid
c        ib    --- boundary condition (0=use bv; 1=zero-slope)
c        bv    --- boundary value
c        interp --- interpolation mode (0=grid ts to nearest grid point; 
c                                       1=0 plus interpolation to x;
c                                       2=0, 1 plus interpolation to y
c        maxgap --- max gap (in points) to interpolate
c
c
c
	subroutine l2g(xs,ys,ts,np,fx,dx,nx,fy,dy,ny,grids,
     1			ib,bv,xi,yi,ti,ilive,a,b,c,
     2			nxi,xmin,xmax,nmax,interp,maxgap,
     3                  gmin,gmax)

	real grids(nx,ny),xs(np),ys(np),ts(np)
	real fx,dx,fy,dy,bv,gmin,gmax
	integer np,nx,ny,nmax,ib,interp,maxgap
	integer nxi(0:ny+1)
	real xmin(0:ny+1), xmax(0:ny+1)
	real xi(0:nx+1,0:ny+1)
	real yi(0:nx+1,0:ny+1)
	real ti(0:nx+1,0:ny+1)
	integer ilive(0:nx+1,0:ny+1),itmp,n
	real a(0:nmax+1),b(0:nmax+1),c(0:nmax+1)
	real x, y, tmp
	integer ifirst
	integer gap
	
	do iy=0,ny+1
	do ix=0,nx+1
		ilive(ix,iy) = 0
		ti(ix,iy) = bv
		xi(ix,iy) = fx + (ix-1)*dx
		yi(ix,iy) = fy + (iy-1)*dy
	end do 
	end do

	odx = 1./dx
	ody = 1./dy
	ifirst = 1

c place input triplets into grid 
	do ip=1,np
		y = (ys(ip)-fy)*ody + 1.5
		iy = y
		if (iy.ge.0.and.iy.le.ny+1) then
		   x = (xs(ip) - fx)*odx + 1.5
		   ix = x
		   if(ix.ge.0.and.ix.le.nx+1) then
			ti(ix,iy) = ts(ip)
			ilive(ix,iy) = 1
			if(ifirst.eq.1) then
				ifirst = 0
				gmin = ts(ip)		
				gmax = ts(ip)
			else
				if(gmin.gt.ts(ip)) gmin = ts(ip)
				if(gmax.lt.ts(ip)) gmax = ts(ip)
			end if 
		   end if
		end if
	end do


	if(interp.eq.0) then
		do iy=1,ny
		do ix=1,nx
			grids(ix,iy) = ti(ix,iy)
		end do
		end do
		return
	end if

c find lower and upper limits of x at fixed y

	do iy=0,ny+1
		xmin(iy) = fx + (nx+1) * dx
		xmax(iy) = fx - 2 * dx
		nxi(iy) = 0
		do ix=0,nx+1
		    if (ilive(ix,iy) .eq. 1) then
			nxi(iy) = nxi(iy) + 1	
			if(xmin(iy).gt.xi(ix,iy)) xmin(iy) = xi(ix,iy)
			if(xmax(iy).lt.xi(ix,iy)) xmax(iy) = xi(ix,iy)
		    end if
		end do
	end do 


c linear interpolate xmin and xmax to other y positions
	nyi = 0
	do iy=0,ny+1
		y = fy + (iy-1)*dy
		if(nxi(iy).ge.1) then
			a(nyi) =  y
			b(nyi) = xmin(iy)
			c(nyi) = xmax(iy)
			nyi = nyi + 1
		end if 
	end do
	if(nyi.eq.0) return
	do iy=0,ny+1
		y = fy + (iy-1)*dy
		if (nxi(iy).eq.0) then
		   if (y.ge.a(0).and.y.le.a(nyi-1)) then
		      call lin1d(a(0),b(0),nyi,y,xmin(iy),1,itmp)
		      call lin1d(a(0),c(0),nyi,y,xmax(iy),1,itmp)
		   end if
		end if
	end do


c linear interpolation along x
	do iy=0,ny+1
		n = 0
		do ix=0,nx+1
			if(ilive(ix,iy).eq.1) then
				a(n) = xi(ix,iy)
				b(n) = ti(ix,iy)
				n = n + 1
			end if
		end do
		if (n.ge.1) then
		      do ix=1,nx
			 x = fx + (ix-1)*dx
			 if (ilive(ix,iy).eq.0) then
			    ix0 = 0
		            ixn = 0
			    do ixx=ix-1,1,-1
				if(ilive(ix,iy).eq.1) then
					ix0 = ixx
					goto 1000
				end if  
			    end do 
1000                        continue
			    do ixx=ix+1,nx
				if(ilive(ix,iy).eq.1) then
					ixn = ixx
					goto 2000
				end if  
			    end do
2000                        continue
			    gap = ixn-ix0+1
			    if(gap.lt.0) gap = - gap
			    if(gap.gt.maxgap) goto 3000
			    if (x.lt.a(0)) then
			       if (ib.eq.1) then
				  ti(ix,iy) = b(0)
			          ilive(ix,iy) = -1
			       end if
			    else if(x.gt.a(n-1)) then
			       if (ib.eq.1) then
			          ti(ix,iy) = b(n-1)
			          ilive(ix,iy) = -1
			       end if
		        else if(x.gt.a(0).and.x.lt.a(n-1)) then
			       call lin1d(a(0),b(0),n,x,tmp,1,itmp)
			       ti(ix,iy) = tmp
			       ilive(ix,iy) = -1
			    end if
3000                        continue
			 end if
		      end do
		end if
	end do  

	do iy=1,ny
		do ix=1,nx
			if(ilive(ix,iy).eq.-1) ilive(ix,iy) = 1
		end do
	end do

	if(interp.eq.1) goto 10000
c linear interpolation along y
	do ix=1,nx
		x = fx + (ix-1)*dx
		n = 0
		do iy=0,ny+1
			if(ilive(ix,iy).eq.1) then
				a(n) = yi(ix,iy)
				b(n) = ti(ix,iy)
				n = n + 1
			end if
		end do
		if (n.ge.1) then
		      do iy=1,ny
			 y = fy + (iy-1)*dy
			 if (ilive(ix,iy).eq.0) then
			    iy0 = 0
		            iyn = 0
			    do iyy=iy-1,1,-1
				if(ilive(ix,iy).eq.1) then
					iy0 = iyy
					goto 4000
				end if  
			    end do 
4000                        continue
			    do iyy=iy+1,ny
				if(ilive(ix,iy).eq.1) then
					iyn = iyy
					goto 5000
				end if  
			    end do 
5000                        continue
			    gap = iyn-iy0+1
			    if(gap.lt.0) gap = - gap
			    if(gap.gt.maxgap) goto 6000
			    call lin1d(a(0),b(0),n,y,tmp,1,itmp)
			    if (ib.eq.0) then
                               if (x.lt.xmin(iy) .or. 
     1                     	   x.gt.xmax(iy)) tmp = bv
			    end if
			    ti(ix,iy) = tmp
6000                        continue
			 end if
		      end do 
		end if
	end do
10000   continue
	do iy=1,ny
	do ix=1,nx
		grids(ix,iy) = ti(ix,iy)
	end do
	end do

	return
	end 
