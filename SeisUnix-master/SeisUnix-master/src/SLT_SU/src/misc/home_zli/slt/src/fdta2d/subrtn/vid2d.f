c   vidale's 2-d finite-difference ray tracing
c
c        ss= 		 SLOWNESS FUNCTION MULTIPLED BY GRID SPACING 
c	 nx=		 X-DIMENSION OF MESH 
c        nz=             Z-DIMENSION OF MESH
c        xs=             SHOT POSITION IN X (in grid points 0<=xs<nx)
c        zs=             SHOT POSITION IN Z (in grid points 0<=zs<nz)
c        tt=		 TRAVEL TIME 
c        mss=		 MODE OF SMOOTHING SLOWNESS SS (0=4-point smoothing;
c			  otherwise=no smoothing; If ss is the same for 
c			  different source location, mss must be set to 0
c			  the first time time2d is called, mss must be greater
c			  than 0 afterwards)
c   note: both ss and tt are array(0:nx-1,0:nz-1)
c after call, ss will be updated with smoothed version of input ss, if mss=0
c
	SUBROUTINE time2d (ss,tt,nx,nz,xs,zs,mss)
	INTEGER nx, nz, xs, zs, mss
	REAL ss(0:*), tt(0:*)
	INTEGER zup, zdn, xlt, xrt, nbox, k, ix, iz, j, x, z
	REAL smm, smp, spm, spp, guess, corn, edge
C find slowness averaged over four points
	if ( mss .eq. 0 ) then
		do j=0,nx*nz-1
	   	   	tt(j) = ss(j)
		end do
		DO iz=0,nz-1
           		iz0 = iz*nx
	   		iz1 = (iz+1)*nx
	   		if (iz .eq. nz-1 ) iz1 = iz0
	   		DO ix=0,nx-1
              			ix0 = ix
	      			ix1 = ix+1
	      			if ( ix .eq. nx-1 ) ix1 = ix0	
	      			ss(ix+iz0) = 0.25*(
     &				     tt(ix0+iz0) + 
     &				     tt(ix1+iz0) + 
     &				     tt(ix0+iz1) +
     &				     tt(ix1+iz1) )
	   		ENDDO
		ENDDO
	end if
c
	DO j=0,nx*nz-1
	    tt(j) = -1.0
	ENDDO
C  initialize source point	
	if (zs-1 .ge. 0	.and. xs-1 .ge. 0) then
	   smm = ss((xs-1)+(zs-1)*nx)
	else 
	   smm = ss((xs)+(zs)*nx)
	end if
	if (zs+1 .le. nz-1 .and. xs-1 .ge. 0) then
	   smp = ss((xs-1)+(zs+1)*nx)
	else 
	   smp = ss((xs)+(zs)*nx)
	end if
	if (zs-1 .ge. 0	.and. xs+1 .le. nx-1) then
	   spm = ss((xs+1)+(zs-1)*nx)
	else 
	   spm = ss((xs)+(zs)*nx)
	end if
	if (zs+1 .le. nz-1 .and. xs+1 .le. nx-1) then
	   spp = ss((xs+1)+(zs+1)*nx)
	else 
	   spp = ss((xs)+(zs)*nx)
	end if
cccc 
	tt((xs)+(zs)*nx)=0.
	IF (zs-1 .GE. 0) THEN
	    tt((xs)+(zs-1)*nx)=0.5*(smm + spm)
	ENDIF
	IF (zs+1 .LE. nz-1) THEN
	    tt((xs)+(zs+1)*nx)=0.5*(smp + spp)
	ENDIF
	IF (xs-1 .GE. 0) THEN
	    tt((xs-1)+(zs)*nx)=0.5*(smm + smp)
	ENDIF
	IF (xs+1 .LE. nx-1) THEN
	    tt((xs+1)+(zs)*nx)=0.5*(spm + spp)
	ENDIF
	IF (xs-1 .GE. 0 .AND. zs-1 .GE. 0) THEN
	    tt((xs-1)+(zs-1)*nx) = smm * sqrt(2.)
	ENDIF
	IF (xs-1 .GE. 0 .AND. zs+1 .LE. nz-1) THEN
	    tt((xs-1)+(zs+1)*nx) = smp * sqrt(2.)
	ENDIF
	IF (xs+1 .LE. nx-1 .AND. zs-1 .GE. 0) THEN
	    tt((xs+1)+(zs-1)*nx) = spm * sqrt(2.)
	ENDIF
	IF (xs+1 .LE. nx-1 .AND. zs+1 .LE. nz-1) THEN
	    tt((xs+1)+(zs+1)*nx) = spp * sqrt(2.)
	ENDIF

C	
	IF (zs-2 .GE. 0) THEN
	    zup = zs -2
	ELSE
	    zup = -1
	ENDIF
	IF (zs+2 .LE. nz-1) THEN
	    zdn = zs + 2
	ELSE
	    zdn = nz
	ENDIF
	IF (xs-2 .GE. 0) THEN
	    xlt = xs - 2
	ELSE
	    xlt = -1
	ENDIF
	IF (xs+2 .LE. nx-1) THEN
	    xrt = xs + 2
	ELSE
	    xrt = nx
	ENDIF
	nbox= 0
	IF (xs-2 .GT. nbox) nbox= xs-2
	IF (zs-2 .GT. nbox) nbox= zs-2
	IF (nx-xs-2 .GT. nbox) nbox= nx-xs-2
	IF (nz-zs-2 .GT. nbox) nbox= nz-zs-2

C loop over boxes	
	DO k=0,nbox
C         top edge	    
	   IF (zup .GE. 0) THEN
C	      
	      IF(tt((xlt+1)+(zup+1)*nx).LE.tt((xlt+2)+(zup+1)*nx)) THEN    
	         tt((xlt+1)+(zup)*nx)= tt((xlt+1)+(zup+1)*nx) +
     &				       ss((xlt+1)+(zup)*nx)
	      ENDIF
	      IF(tt((xrt-1)+(zup+1)*nx).LE.tt((xrt-2)+(zup+1)*nx)) THEN
	         tt((xrt-1)+(zup)*nx)= tt((xrt-1)+(zup+1)*nx) +
     &				       ss((xrt-1)+(zup-1)*nx)
	      ENDIF
	      DO x=xlt+2,xrt-2
C	        
	         IF(tt((x)+(zup+1)*nx).LE.tt((x-1)+(zup+1)*nx) .AND.
     &	            tt((x)+(zup+1)*nx) .LE. tt((x+1)+(zup+1)*nx)) THEN
	            tt((x)+(zup)*nx)= edge(tt((x)+(zup+1)*nx),
     &					   tt((x-1)+(zup+1)*nx),
     &	              			   tt((x+1)+(zup+1)*nx),
     &					   0.5*(ss((x-1)+(zup)*nx)+
     &				                ss((x)+(zup)*nx)))
C	      
	         ELSE IF(tt(x+(zup+1)*nx).GT.tt((x-1)+(zup+1)*nx)) THEN     
	            tt((x)+(zup)*nx)= corn(tt((x-1)+(zup+1)*nx),
     &					   tt((x-1)+(zup)*nx),
     &	                                   tt((x)+(zup+1)*nx),
     &					   ss((x-1)+(zup)*nx))
	         ENDIF
	      ENDDO
C  wavefronts traveling up and left	      
	      DO x=xrt-2,xlt+2,-1
	         IF(tt((x)+(zup+1)*nx).GT.tt((x+1)+(zup+1)*nx)) THEN
	            guess = corn(tt((x+1)+(zup+1)*nx),
     &				 tt((x+1)+(zup)*nx),
     &	                         tt((x)+(zup+1)*nx),
     &			         ss((x)+(zup)*nx))
C	          
C	          
	            IF (guess.LT.tt((x)+(zup)*nx) .OR.
     &			tt((x)+(zup)*nx).LT.0.0) THEN
	                tt((x)+(zup)*nx) = guess
	            ENDIF
	         ENDIF
	      ENDDO
C	      
	      IF (tt((xrt-1)+(zup)*nx).LT.0.0) THEN
	         tt((xrt-1)+(zup)*nx)= corn(tt((xrt-2)+(zup+1)*nx),
     &					    tt((xrt-2)+(zup)*nx),
     &	                                    tt((xrt-1)+(zup+1)*nx),
     &					    ss((xrt-2)+(zup)*nx))
	      ENDIF
	      IF (tt((xlt+1)+(zup)*nx).LT.0.0) THEN
	         tt((xlt+1)+(zup)*nx)= corn(tt((xlt+2)+(zup+1)*nx),
     &					    tt((xlt+2)+(zup)*nx),
     &	        			    tt((xlt+1)+(zup+1)*nx),
     &					    ss((xlt+1)+(zup)*nx))
	      ENDIF
	   ENDIF
C bottom edge	    
	   IF (zdn .LT. nz) THEN
	      IF(tt((xlt+1)+(zdn-1)*nx).LE.tt((xlt+2)+(zdn-1)*nx)) THEN
	         tt((xlt+1)+(zdn)*nx)= tt((xlt+1)+(zdn-1)*nx) + 
     &				       ss((xlt+1)+(zdn-1)*nx)
	      ENDIF
	      IF(tt((xrt-1)+(zdn-1)*nx).LE.tt((xrt-2)+(zdn-1)*nx)) THEN 
	         tt((xrt-1)+(zdn)*nx)= tt((xrt-1)+(zdn-1)*nx) +
     &				       ss((xrt-2)+(zdn-1)*nx)
	      ENDIF
	      DO x=xlt+2, xrt-2
	         IF(tt((x)+(zdn-1)*nx).LE.tt((x-1)+(zdn-1)*nx) .AND.
     &	            tt((x)+(zdn-1)*nx).LE.tt((x+1)+(zdn-1)*nx)) THEN
	            tt((x)+(zdn)*nx)= edge(tt((x)+(zdn-1)*nx),
     &		 			   tt((x-1)+(zdn-1)*nx),
     &	                                   tt((x+1)+(zdn-1)*nx),
     &					   0.5*(ss((x-1)+(zdn-1)*nx)+
     &				                ss((x)+(zdn-1)*nx)))
	         ELSE IF(tt(x+(zdn-1)*nx).GT.tt((x-1)+(zdn-1)*nx)) THEN
	            tt((x)+(zdn)*nx)= corn(tt((x-1)+(zdn-1)*nx),
     &					   tt((x-1)+(zdn)*nx),
     &	           			   tt((x)+(zdn-1)*nx),
     &					   ss((x-1)+(zdn-1)*nx))
	         ENDIF
	      ENDDO
	      DO x=xrt-2, xlt+2,-1
	         IF (tt((x)+(zdn-1)*nx).GT.tt((x+1)+(zdn-1)*nx)) THEN
	            guess = corn(tt((x+1)+(zdn-1)*nx),
     &				 tt((x+1)+(zdn)*nx),
     &	                         tt((x)+(zdn-1)*nx),
     &				 ss((x)+(zdn-1)*nx))
	            IF (guess.LT.tt((x)+(zdn)*nx) .OR. 
     &			tt((x)+(zdn)*nx).LT.0.0) THEN
	               tt((x)+(zdn)*nx) = guess
	            ENDIF
	         ENDIF
	      ENDDO
	      IF (tt((xrt-1)+(zdn)*nx) .LT. 0.0) THEN
	         tt((xrt-1)+(zdn)*nx)= corn(tt((xrt-2)+(zdn-1)*nx),
     &					    tt((xrt-2)+(zdn)*nx),
     &	        			    tt((xrt-1)+(zdn-1)*nx),
     &					    ss((xrt-2)+(zdn-1)*nx))
	      ENDIF
	      IF (tt((xlt+1)+(zdn)*nx) .LT. 0.0) THEN
	         tt((xlt+1)+(zdn)*nx)= corn(tt((xlt+2)+(zdn-1)*nx),
     &					    tt((xlt+2)+(zdn)*nx),
     &	        			    tt((xlt+1)+(zdn-1)*nx),
     &					    ss((xlt+1)+(zdn-1)*nx))
	      ENDIF
	   ENDIF
C left edge	    
	   IF (xlt .GE. 0) THEN
	      IF(tt((xlt+1)+(zup+1)*nx).LE.tt((xlt+1)+(zup+2)*nx)) THEN
	         tt((xlt)+(zup+1)*nx)= tt((xlt+1)+(zup+1)*nx) + 
     &				       ss((xlt)+(zup+1)*nx)
	      ENDIF
	      IF(tt((xlt+1)+(zdn-1)*nx).LE.tt((xlt+1)+(zdn-2)*nx)) THEN
	         tt((xlt)+(zdn-1)*nx)= tt((xlt+1)+(zdn-1)*nx) +
     &				       ss((xlt-1)+(zdn-1)*nx)
	      ENDIF
	      DO z=zup+2, zdn-2
	         IF(tt((xlt+1)+(z)*nx).LE.tt((xlt+1)+(z-1)*nx) .AND.
     &	            tt((xlt+1)+(z)*nx).LE.tt((xlt+1)+(z+1)*nx)) THEN
	            tt((xlt)+(z)*nx)= edge(tt((xlt+1)+(z)*nx),
     &					   tt((xlt+1)+(z-1)*nx),
     &	               			   tt((xlt+1)+(z+1)*nx),
     &					   0.5*(ss((xlt)+(z-1)*nx)+
     &						ss((xlt)+(z)*nx)))
	         ELSE IF(tt(xlt+1+(z)*nx).GT.tt(xlt+1+(z-1)*nx)) THEN
	            tt((xlt)+(z)*nx)= corn(tt((xlt+1)+(z-1)*nx),
     &					   tt((xlt)+(z-1)*nx),
     &	                                   tt((xlt+1)+(z)*nx),
     &					   ss((xlt)+(z-1)*nx))
	         ENDIF
	      ENDDO
	      DO z=zdn-2, zup+1,-1
	         IF(tt((xlt+1)+(z)*nx).GT.tt((xlt+1)+(z+1)*nx)) THEN
	            guess = corn(tt((xlt+1)+(z+1)*nx),
     &				 tt((xlt)+(z+1)*nx),
     &	                         tt((xlt+1)+(z)*nx),
     &				 ss((xlt)+(z)*nx))
	            IF(guess.LT.tt((xlt)+(z)*nx) .OR.
     &		       tt((xlt)+(z)*nx).LT.0.0) THEN
	               tt((xlt)+(z)*nx) = guess
	            ENDIF
	         ENDIF
	      ENDDO
	      IF(tt((xlt)+(zdn-1)*nx) .LT. 0.0) THEN
	         tt((xlt)+(zdn-1)*nx)= corn(tt((xlt+1)+(zdn-2)*nx),
     &					    tt((xlt)+(zdn-2)*nx),
     &	       				    tt((xlt+1)+(zdn-1)*nx),
     &					    ss((xlt)+(zdn-2)*nx))
	      ENDIF
	      IF(tt((xlt)+(zup+1)*nx) .LT. 0.0) THEN
	         tt((xlt)+(zup+1)*nx)= corn(tt((xlt+1)+(zup+2)*nx),
     &					    tt((xlt)+(zup+2)*nx),
     &	        			    tt((xlt+1)+(zup+1)*nx),
     &					    ss((xlt)+(zup+1)*nx))
	      ENDIF
	   ENDIF
C   right edge	    
	   IF (xrt .LT. nx) THEN
	      IF(tt((xrt-1)+(zup+1)*nx).LE.tt((xrt-1)+(zup+2)*nx)) THEN
	         tt((xrt)+(zup+1)*nx)= tt((xrt-1)+(zup+1)*nx) +
     &				       ss((xrt-1)+(zup+1)*nx)
	      ENDIF
	      IF(tt((xrt-1)+(zdn-1)*nx).LE.tt((xrt-1)+(zdn-2)*nx)) THEN
	         tt((xrt)+(zdn-1)*nx)= tt((xrt-1)+(zdn-1)*nx) +
     &				       ss((xrt-1)+(zdn-2)*nx)
	      ENDIF
	      DO z=zup+2, zdn-2
	         IF(tt((xrt-1)+(z)*nx).LE.tt((xrt-1)+(z-1)*nx) .AND.
     &	            tt((xrt-1)+(z)*nx).LE.tt((xrt-1)+(z+1)*nx)) THEN
	            tt((xrt)+(z)*nx)= edge(tt((xrt-1)+(z)*nx),
     &					   tt((xrt-1)+(z-1)*nx),
     &	               			   tt((xrt-1)+(z+1)*nx),
     &				           0.5*(ss((xrt-1)+(z-1)*nx)+
     &						ss((xrt-1)+(z)*nx)))
	         ELSE IF(tt(xrt-1+(z)*nx).GT.tt(xrt-1+(z-1)*nx)) THEN
	            tt((xrt)+(z)*nx)= corn(tt((xrt-1)+(z-1)*nx),
     &					   tt((xrt)+(z-1)*nx),
     &	                                   tt((xrt-1)+(z)*nx),
     &					   ss((xrt-1)+(z-1)*nx))
	         ENDIF
	      ENDDO
	      DO z=zdn-2, zup+2,-1
	         IF(tt((xrt-1)+(z)*nx).GT.tt((xrt-1)+(z+1)*nx)) THEN
	            guess = corn(tt((xrt-1)+(z+1)*nx),
     &				 tt((xrt)+(z+1)*nx),
     &	                         tt((xrt-1)+(z)*nx),
     &				 ss((xrt-1)+(z)*nx))
	            IF(guess.LT.tt((xrt)+(z)*nx) .OR. 
     &		       tt((xrt)+(z)*nx).LT.0.0) THEN
	               tt((xrt)+(z)*nx) = guess
	            ENDIF
	         ENDIF
	      ENDDO
	      IF(tt((xrt)+(zdn-1)*nx) .LT. 0.0) THEN
	         tt((xrt)+(zdn-1)*nx)= corn(tt((xrt-1)+(zdn-2)*nx),
     &					    tt((xrt)+(zdn-2)*nx),
     &	        			    tt((xrt-1)+(zdn-1)*nx),
     &					    ss((xrt-1)+(zdn-2)*nx))
	      ENDIF
	      IF(tt((xrt)+(zup+1)*nx) .LT. 0.0) THEN
	         tt((xrt)+(zup+1)*nx)= corn(tt((xrt-1)+(zup+2)*nx),
     &					    tt((xrt)+(zup+2)*nx),
     &	        			    tt((xrt-1)+(zup+1)*nx),
     &					    ss((xrt-1)+(zup+1)*nx))
	      ENDIF
	   ENDIF
C   corners	    
	   IF (xlt .GE. 0 .AND. zup .GE. 0) THEN
	      tt((xlt)+(zup)*nx)= corn(tt((xlt+1)+(zup+1)*nx),
     &				       tt((xlt)+(zup+1)*nx),
     &	        		       tt((xlt+1)+(zup)*nx),
     &				       ss((xlt)+(zup)*nx))
	   ENDIF
	   IF (xlt .GE. 0 .AND. zdn .LT. nz) THEN
	      tt((xlt)+(zdn)*nx)= corn(tt((xlt+1)+(zdn-1)*nx),
     &				       tt((xlt)+(zdn-1)*nx),
     &	        		       tt((xlt+1)+(zdn)*nx),
     &				       ss((xlt)+(zdn-1)*nx))
	   ENDIF
	   IF (xrt .LT. nx .AND. zup .GE. 0) THEN
	      tt((xrt)+(zup)*nx)= corn(tt((xrt-1)+(zup+1)*nx),
     &				       tt((xrt)+(zup+1)*nx),
     &	        		       tt((xrt-1)+(zup)*nx),
     &				       ss((xrt-1)+(zup)*nx))
	   ENDIF
	   IF (xrt .LT. nx .AND. zdn .LT. nz) THEN
	      tt((xrt)+(zdn)*nx)= corn(tt((xrt-1)+(zdn-1)*nx),
     &				       tt((xrt)+(zdn-1)*nx),
     &	                               tt((xrt-1)+(zdn)*nx),
     &				       ss((xrt-1)+(zdn-1)*nx))
	   ENDIF

C	   expand the box 
	   IF (xlt .GE. 0) xlt = xlt - 1
	   IF (zup .GE. 0) zup = zup - 1
	   IF (xrt .LT. nx) xrt = xrt + 1
	   IF (zdn .LT. nz) zdn = zdn + 1
	ENDDO
	RETURN
	END

	FUNCTION corn(ta,tb,tc,hs)
	REAL corn, ta, tb, tc, hs
	REAL diff, arg

	diff= tc - tb
	arg= 2.0*hs*hs - diff*diff
C	
 	IF (arg .LT. 0.0) THEN
	    corn = ta
	    IF (corn .LT. tb) THEN
	      corn = tb
	    ENDIF
	    IF (corn .LT. tc) THEN
	      corn = tc
	    ENDIF
	    RETURN
	ENDIF
	corn = ta + sqrt(arg)
	IF (corn .LT. tb) THEN
	    corn = tb
	ENDIF
	IF (corn .LT. tc) THEN
	    corn = tc
	ENDIF
	RETURN
	END

	FUNCTION edge(ta,tb,tc,hs)
	REAL edge, ta, tb, tc, hs
	REAL diff, arg
	diff= 0.5*(tc- tb)
	arg= hs*hs - diff*diff
C	
	IF (arg .LT. 0.0) THEN
	    edge = ta + hs
	    RETURN
	ENDIF
	edge = ta + sqrt(arg)
	RETURN
	END
