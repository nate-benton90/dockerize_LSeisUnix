ccc    amplitude computation in 2-d media with (dx=dz)
ccc   author: z. li           8/6/1991
ccc
ccc   tt(nz,nx)          ---      travel time
c     nz                 ---      number of depths
c     nx                 ---      number of horizontal grids
c     aa(nz,nx)          ---      computed amplitude (v variation ignored)
c     ii(nz,nx)          ---      computed propagation angle from vertical 
c     angmin             ---      minimum angle to compute amplitude table
c     angmax             ---      maximum angle to compute amplitude table
c				  (angle is meassured from vertical,
c				   negative if at left side,  
c				   positive if at right side.
c				   all angles are in radian)  
c     amptyp             ---      amplitude computation type:
c				   0=simplified method (z/(x**2+z**2)**3/2;
c                                    do not compute ii();
c				   1=Bleistein method
c				   2=Vidale-Houston method)
c				   3=same as 0; also compute ii()
c     work(nz,nx)        ---      work buffer (needed only when amptyp=2)
c     sx		 ---	  source x location 
c     sz		 ---	  source z location 
c     ox  		 ---	  starting x location of receiver
c     oz  		 ---	  receiver depth
c     dx  		 ---	  receiver spacing
c     dz  		 ---	  depth spacing
c     
c    referecen: Vidale and Houston, 1990, Rapid calculation of seismic 
c	          amplitudes, Geophysics, 55, 11.
c               Bleistein, Cohen and Hagin, 1987, Two and one-half dimensional
c		  Born inversion with an arbitrary reference, 
c	          Geophysics, 52, 1.
cccccc
	subroutine amp2d (tt,aa,ii,nz,nx,angmin,angmax,work,
     1 			  amptyp,sx,sz,ox,oz,dx,dz)
	real tt(nz,nx), aa(nz,nx), ii(nz,nx),angmin,angmax
	real work(nz,nx)
	integer nx,nz,amptyp
	real sx,sz,ox,oz,dx,dz
	real pi
	real xx,xx2,zz,ang
	real dd
	pi = 3.1415926543
	
ccccccc simplified method

	if (zsmall.eq.sz) zsmall = 0.001 * sz
	if (amptyp .eq. 0) then
		do ix=1,nx
			xx = ox+(ix-1)*dx - sx
			xx2 = xx * xx
			do iz=1,nz
				zz = oz + (iz-1)*dz - sz
				if(zz.eq.0.) then
					aa(iz,ix) = 0. 
				else 
					dd = xx/zz
					ang = atan(dd)
				        if(ang.ge.angmin .and. 
     1 			 		   ang.le.angmax) then

			 		  zz2=zz*zz
					  tmp = sqrt(xx2+zz2)
				  	  aa(iz,ix)=zz/(tmp*sqrt(tmp))
				        else 
					  aa(iz,ix) = 0.
				        end if
				end if
			end do
		end do
		return
	end if
ccccc compute propagation angle
c inside 
	do ix=2,nx-1
	   do iz=2,nz-1
	       ii(iz,ix) = atan2(tt(iz,ix+1)-tt(iz,ix-1),
     &			         tt(iz+1,ix)-tt(iz-1,ix))
	   end do
	end do
c top edge
        do ix=2,nx-1
	   ii(1,ix) = atan2(tt(1,ix+1)-tt(1,ix-1),
     &			    2.*(tt(2,ix)-tt(1,ix)))
	end do
	
c bottom edge
        do ix=2,nx-1
	   ii(nz,ix) = atan2(tt(nz,ix+1)-tt(nz,ix-1),
     &			    2.*(tt(nz,ix)-tt(nz-1,ix)))
	end do
c left edge
        do iz=2,nz-1
	   ii(iz,1) = atan2(2*(tt(iz,2)-tt(iz,1)),
     &			    tt(iz+1,1)-tt(iz-1,1))
	end do
c right edge
        do iz=2,nz-1
	   ii(iz,nx) = atan2(2*(tt(iz,nx)-tt(iz,nx-1)),
     &			    tt(iz+1,nx)-tt(iz-1,nx))
	end do
c top-left corner
	ii(1,1) = atan2(tt(1,2)-tt(1,1),tt(2,1)-tt(1,1))
c top-right corner
	ii(1,nx) = atan2(tt(1,nx)-tt(1,nx-1),tt(2,nx)-tt(1,nx))
c bottom-left corner
	ii(nz,1) = atan2(tt(nz,2)-tt(nz,1),tt(nz,1)-tt(nz-1,1))
c bottom-right corner
	ii(nz,nx) = atan2(tt(nz,nx)-tt(nz,nx-1),tt(nz,nx)-tt(nz-1,nx))

cccccccc compute amplitute   
	if (amptyp .eq. 3) then
                do ix=1,nx
                        xx = ox+(ix-1)*dx - sx
                        xx2 = xx * xx
                        do iz=1,nz
                                zz = oz + (iz-1)*dz - sz 
                                if(zz.eq.0.) then
                                        aa(iz,ix) = 0. 
                                else
                                        dd = xx/zz
                                        ang = atan(dd)
                                        if(ang.ge.angmin .and.
     1                                     ang.le.angmax) then

                                          zz2=zz*zz
                                          tmp = sqrt(xx2+zz2)
                                          aa(iz,ix)=zz/(tmp*sqrt(tmp))        
                                        else
                                          aa(iz,ix) = 0.
                                        end if
                                end if
                        end do
                end do
		return
	else if (amptyp .eq. 1) then
	   do ix=1,nx
	      do iz=1,nz
		 if(tt(iz,ix).gt.0. .and. ii(iz,ix).ge.angmin .and.
     &		    ii(iz,ix).le.angmax ) then
	            cosa = cos(ii(iz,ix))
	            aa(iz,ix) = cosa*cosa/sqrt(tt(iz,ix))
	         else
		    aa(iz,ix) = 0.
	         end if
	      end do
	   end do
	   return
	end if 
ccccc compute amplitude for amptyp equal 2 
c inside 
	do ix=2,nx-1
	   do iz=2,nz-1
	       ax = amods( abs( ii(iz,ix+1) - ii(iz,ix-1) ) )
	       az = amods( abs( ii(iz+1,ix) - ii(iz-1,ix) ) )
	       aa(iz,ix) = sqrt( ax*ax + az*az ) 
	   end do
	end do
c top edge
        do ix=2,nx-1
	   ax = amods( abs( ii(1,ix+1) - ii(1,ix-1) ) )
	   az = amods( 2. * abs( ii(2,ix) - ii(1,ix) ) )
	   aa(1,ix) = sqrt( ax*ax + az*az ) 
	end do
c bottom edge
        do ix=2,nx-1
	   ax = amods( abs( ii(nz,ix+1) - ii(nz,ix-1) ) )
	   az = amods( 2. * abs( ii(nz,ix) - ii(nz-1,ix) ) )
	   aa(nz,ix) = sqrt( ax*ax + az*az ) 
	end do
c left edge
        do iz=2,nz-1
	   ax = amods( 2. * abs( ii(iz,2) - ii(iz,1) ) )
	   az = amods( abs( ii(iz+1,1) - ii(iz-1,1) ) )
	   aa(iz,1) = sqrt( ax*ax + az*az ) 
	end do
c right edge
        do iz=2,nz-1
	   ax = amods( 2. * abs( ii(iz,nx) - ii(iz,nx-1) ) )
	   az = amods( abs( ii(iz+1,nx) - ii(iz-1,nx) ) )
	   aa(iz,nx) = sqrt( ax*ax + az*az ) 
	end do
c top-left corner
	ax = amods( 2. * abs( ii(1,2) - ii(1,1) ) )
	az = amods( 2. * abs( ii(2,1) - ii(1,1) ) )
	aa(1,1) = sqrt( ax*ax + az*az ) 
c top-right corner
	ax = amods( 2. * abs( ii(1,nx) - ii(1,nx-1) ) )
	az = amods( 2. * abs( ii(2,nx) - ii(1,nx) ) )
	aa(1,nx) = sqrt( ax*ax + az*az ) 
c bottom-left corner
	ax = amods( 2. * abs( ii(nz,2) - ii(nz,1) ) )
	az = amods( 2. * abs( ii(nz,1) - ii(nz-1,1) ) )
	aa(nz,1) = sqrt( ax*ax + az*az ) 
c bottom-right corner
	ax = amods( 2. * abs( ii(nz,nx) - ii(nz,nx-1) ) )
	az = amods( 2. * abs( ii(nz,nx) - ii(nz-1,nx) ) )
	aa(nz,nx) = sqrt( ax*ax + az*az ) 
	do ix=1,nx
	   do iz=1,nz
	      if (ii(iz,ix).ge.angmin .and. ii(iz,ix).le.angmax ) then
	         aa(iz,ix) = sqrt(amods(aa(iz,ix)))
	      else
	         aa(iz,ix) = 0.
	      end if
	   end do
	end do
c   suppress sudden changes (due to f.d.) using smoothing
	lsm = 7
	call vsmth(aa,nz,nx,lsm,work)
	return
	end

	function amods(x)
	real x, amods
	real tmp
	tmp = x
	if ( x .gt. 3.1415926543 )  tmp  = 6.283185307 - x
	amods = tmp
	return
	end

