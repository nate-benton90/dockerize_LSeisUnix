c     tshoot.f is for transmitted waves
c     modified from shoot.f (cwp) by z. li 	2-23-93 
c---------------------------------------------------------------------

      SUBROUTINE TSHOOT(X,z,icross,BETA,PI,TRUGEO,XSTART,XEND,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)
cc    This subroutine shoots a ray from a given
c     source location at a given takeoff angle.  The
c     end point of the ray is not constrained to be at a
c     receiver location.
c     Snell's law is solved at each interface in turn.
c     The direction of the ray with reference to the x-z
c     coordinate system is found after each intersection
c     with an interface.  This direction is used by another
c     subroutine to find the coordinates of the next
c     intersection point.
c
c    
c  X()      	x-coordinates of ray intersections with interfaces
c  z()        	z-coordinates of ray intersections with interfaces
c  icross    	number of cross points of the ray with interfaces 
c  BETA     	Takeoff angle at the source
c  PI       	3.1415927
c  TRUGEO   	Station spacing
c  XSTART   	Minimum x-coordinate in model (taken from upper surface)
c  XEND     	Maximum x-coordinate in model (taken from upper surface)
c  recdpt   	Depth of (all) receivers below the upper surface
c  XINT(,)  	x-coordinates of points defining each interface
c  ZINT(,)  	z-coordinates of points defining each interface
c  A0(,),A1(,),A2(,),A3(,)  
c         	Cubic spline coefficients of the interfaces.  First index
c         	defines the interface, second index the portion of the
c  SIGN()   	Indicates the direction of the ray leaving each intersection
c           	point. 1. if in the direction of the upward normal to the
c           	interface. Otherwise, -1.
c  NPTS()   	Number of points defining each interface.
c  NINT     	Number of interfaces in the model (not counting the upper 
c		surface)
c  NORDER() 	Order of intersections in current event.
c  DZ()     	First derivative of each interface at intersection point
c  DDZ()    	Second derivative of each interface at intersection point
c  DELTAX() 	x-distance travelled within a layer
c  DELTAZ() 	z-distance travelled within a layer
c  D()      	Length of ray segment
c  V()      	Interval velocity on each ray segment
c  N        	Number of intersection points between source and receiver
c  MAXINT   	Maximum number of interfaces in the model
c  MAXSPL   	Maximum number of points defining an interface
c  MAXN     	Maximum value of N
c  MAXNP1   	MAXN plus 1
c  MXSPM1   	MAXSPL minus 1 
c  VINT(,)  	velocity at points defining each interface
c


      REAL       X(0:*),       z(0:*),       BETA,        TRUGEO,
     :           XSTART,       XEND,         PI,          recdpt

      integer    icross

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1

      REAL         XINT(0:MAXINT,MAXSPL),    ZINT(0:MAXINT,MAXSPL)
      REAL         VINT(0:MAXINT,MAXSPL)
      REAL*8       A0(0:MAXINT,MXSPM1),      A1(0:MAXINT,MXSPM1),
     :             A2(0:MAXINT,MXSPM1),      A3(0:MAXINT,MXSPM1)
      REAL         SIGN(0:MAXN)

      INTEGER      NPTS(0:MAXINT),  NINT,      NORDER(MAXN)

      REAL*8     DZ(MAXN),        DDZ(MAXN),
     :           DELTAX(MAXNP1),   DELTAZ(MAXNP1)
      REAL       D(MAXNP1),       V(MAXNP1)

      INTEGER    N

cc    Local   variables
c     DANGLE  difference between critical angle and angle of incidence
c             of ray
c     DELX    step in search for next intersection point
c     DENOM   denominator in expressions for next ray segment
c     HUGE    A large number
c     J       section of splined interface
c     K       loop index
c     L       identifies splined interface
c     SGNCHK  there are usually 2 ray segments leaving the source which
c             intersect the refracting interface at critical.  The one
c             we need depends on the relative offset of the receivers from
c             the source.  SGNCHK makes sure the source segment is the
c             correct one for the current receivers.
c     SGNTAN  determines the quadrants in which we search for
c             solutions (ie, whether takeoff angle is measured
c             from the upward or downward pointing vertical)
c     SINTH   sine of angle between normal and ray leaving interface
c     TANTH   tangent of angle ray makes with downward vertical
c     TRANSX  x-component of ray leaving interface
c     TRANSZ  z-component of ray leaving interface
c     XFAIL   true if can't find next intersection point

      REAL       DELX,     DENOM,     SINTH,      TANTH,
     :           TRANSX,   TRANSZ,    SGNTAN,     huge

      INTEGER    J,        K,         L

      LOGICAL    XFAIL

      real tanth0

      parameter( huge = 10000000.)


      icross = 1
c     Find first intersection point.

      if(abs(beta).eq.180.) then
c        invalid angle
         return
      end if
      if(abs(beta).eq.90.) Then
c        tanth = beta * huge / beta
         tanth = abs(beta) * huge / beta
      else
         TANTH = TAN( PI * BETA / 180. )
      end if
c     measure from upward or downward vertical?
      sgntan = - sign(0)

c     use geophone spacing as step size in search
c     direction of search is direction of beta
      if(tanth.eq.0.) then
      else
         DELX = ABS( BETA * TRUGEO ) / BETA
      end if

      CALL FINDX(X(0),Z(0),TANTH,SGNTAN,DELX,1,N,
     1           X(1),z(1),XFAIL,recdpt,
     2 		XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3          MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)


      IF(XFAIL) THEN
c        can't find first intersection point
	 if(beta.lt.0.) then 
		x(1) = xstart
	        z(1) = (x(1)-x(0))/tanth
	 else
		x(1) = xend
	        z(1) = (x(1)-x(0))/tanth
	 end if
         icross = 2
         RETURN
      END IF


c     Find remaining intersections.


      DO 100 K = 1,  N

cccccc  z li added variable velocity within layer
         l = norder(k-1)
	 if(l.lt.0) l = 0
         j = 1
5050     if(x(k-1).gt.xint(l,j).and.j.le.npts(l)) then
                j = j+1
                goto 5050
         end if
         j = j - 1
	 if(j.lt.1) j = 1
         vk = vint(l,j)
c        number of interface for next intersection
         L = NORDER(K)
c        find section of interface on which previous
c        intersection point lies
         J = 1
50       IF(X(K).GT.XINT(L,J).and.j.le.npts(l)) THEN
            J = J + 1
            GO TO 50
         END IF
         J = J - 1
	 if(j.lt.1) j = 1
	 vkp1 = vint(l,j)

         tanth0 = tanth

c        find slope at previous intersection
         DZ(K) = A1(L,J) + 2. * A2(L,J) * X(K)
     :         + 3. * A3(L,J) * X(K)**2


         DELTAX(K) = X(K) - X(K-1)
         DELTAZ(K) = Z(K) - Z(K-1)
         D(K) = SQRT( DELTAX(K)**2 + DELTAZ(K)**2 )

c        calculate sine of angle between normal and ray
c        leaving the intersection point
         DENOM = SQRT( 1. + DZ(K)**2 )

	 IF(D(K).EQ.0.) THEN 
	    icross = k + 1
            RETURN
	 END IF


         SINTH = vkp1 * ( DELTAX(K) + DELTAZ(K) * DZ(K) ) /
     :           ( vk * D(K) * DENOM )

	 V(k+1) = vkp1
	 V(k) = vk

c         SINTH = V(K+1) * ( DELTAX(K) + DELTAZ(K) * DZ(K) ) /
c     :           ( V(K) * D(K) * DENOM )


         IF(ABS(SINTH).GE.1.) THEN
c           no transmitted ray beyond critical
	    icross = k + 1
            RETURN
         END IF

c        find component of transmitted ray in x-direction
         TRANSX = ( SINTH + SIGN(K) * SQRT( 1. - SINTH**2 )
     :            * DZ(K) ) / DENOM

c        find z-component of transmitted ray
         TRANSZ = ( SINTH * DZ(K) - SIGN(K) *
     :             SQRT( 1. - SINTH**2 ) ) / DENOM

         if(abs(transz).gt.0.) then
            TANTH = TRANSX / TRANSZ
         else
c           ray propagating horizontally (a problem for taking tangent)
            tanth = huge
         end if

c        search for next intersection in the direction
c        of the x-component
         if(tanth.eq.0.) then
         else
            DELX = ABS( TRANSX * TRUGEO ) / TRANSX
         end if
         CALL FINDX(X(K),Z(K),TANTH,1.,DELX,K+1,N,
     1   	X(K+1),z(k+1),XFAIL,recdpt,
     2 		XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3          MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)


ccc a check by z li
	 if(xfail .eq. .false.) then
		if(x(k+1).gt.x(k) .and. beta.lt.0.) then
			xfail = .true.
		else if(x(k+1).lt.x(k) .and. beta.gt.0.) then
			xfail = .true.
		else if(z(k+1).lt.z(k)) then
			xfail = .true.
		end if
	 end if 

         IF(XFAIL) THEN
c           failed to find next intersection
	    if(k.lt.n .and. tanth*tanth0.ge.0.) then
	    	if(tanth.lt.0. .and. beta.lt.0.) then
	        	x(k+1) = xstart
			z(k+1) = z(k) + (x(k+1)-x(k))/tanth
	    		icross = k + 2
	    	else if(tanth.gt.0. .and. beta.gt.0.) then
	        	x(k+1) = xend
			z(k+1) = z(k) + (x(k+1)-x(k))/tanth
	    		icross = k + 2
	    	end if
		call zcheck(x(k),z(k),x(k+1),z(k+1),l, 
     1 		       		xint,zint,npts,nint,
     2                 		maxint, maxspl, ierr)
		if(ierr.eq.1) icross = k + 1
	    else
	    	icross = k + 1
	    end if
            RETURN
         END IF

100      CONTINUE

      icross = N  + 1

      RETURN
      END

	subroutine zcheck(xk,zk,xkp1,zkp1,intf, 
     1 		       xint,zint,npts,nint,
     2                 maxint, maxspl, ierr)

      	real xint(0:maxint,maxspl), zint(0:maxint,maxspl)
        integer npts(0:maxint),maxint,maxspl,ierr,intf
	real xk,zk,xkp1,zkp1

	l = intf
        j = 1
50      if(xk.gt.xint(l,j).and.j.le.npts(l)) then
        	j = j + 1
            	go to 50
        end if

	j = j - 1
	ierr = 0

	if(xkp1.gt.xk) then
		dzdx = (zkp1-zk)/(xkp1-xk)
		do i=j+1,npts(l)
			ztest = zk + (xint(l,i)-xk)*dzdx
			if(ztest.le.zint(l,i)) then
				ierr = 1
				return
			end if
		end do
	else if (xkp1.lt.xk) then
		dzdx = (zkp1-zk)/(xkp1-xk)
		do i=j-1,1,-1
			ztest = zk + (xint(l,i)-xk)*dzdx
			if(ztest.le.zint(l,i)) then
				ierr = 1
				return
			end if
		end do
	else 
		ierr = 1
		return
	end if


	if(l.eq.nint) return

	l = l + 1
        j = 1
60      if(xk.gt.xint(l,j).and.j.le.npts(l)) then
        	j = j + 1
            	go to 60
        end if

	j = j - 1
	ierr = 0

	if(xkp1.gt.xk) then
		dzdx = (zkp1-zk)/(xkp1-xk)
		do i=j+1,npts(l)
			ztest = zk + (xint(l,i)-xk)*dzdx
			if(ztest.ge.zint(l,i)) then
				ierr = 1
				return
			end if
		end do
	else if (xkp1.lt.xk) then
		dzdx = (zkp1-zk)/(xkp1-xk)
		do i=j-1,1,-1
			ztest = zk + (xint(l,i)-xk)*dzdx
			if(ztest.ge.zint(l,i)) then
				ierr = 1
				return
			end if
		end do
	else 
		ierr = 1
	end if

	return
	end
	
