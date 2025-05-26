c     This set of subroutines is used in the shooting method.
c---------------------------------------------------------------------

      SUBROUTINE SHOOT(X,z,NOCONV,BETA,PI,TRUGEO,XSTART,XEND,recdpt,
     1                 head,krefra,sinthc,sgnoff,danold,
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
c  NOCONV   	TRUE if can't find ray or ray exits line
c  BETA     	Takeoff angle at the source
c  PI       	3.1415927
c  TRUGEO   	Station spacing
c  XSTART   	Minimum x-coordinate in model (taken from upper surface)
c  XEND     	Maximum x-coordinate in model (taken from upper surface)
c  recdpt   	Depth of (all) receivers below the upper surface
c  head     	TRUE if this is a headwave
c  krefra       refracting interface number 
c  sinthc       sine of critical angle 	
c  sgnoff       1. if receivers are to right of source; -1. if to left
c  danold	Difference between incident angle and critical angle at the
c         	refracting interface.  When this difference changes sign then
c         	we are close to critical.
c  XINT(,)  	x-coordinates of points defining each interface
c  ZINT(,)  	z-coordinates of points defining each interface
c  VINT(,)  	velocities of layers at points defining each interface
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
c


      REAL       X(0:*),       z(0:*),       BETA,        TRUGEO,
     :           XSTART,       XEND,         PI,          recdpt,
     :           sinthc,       sgnoff,       danold

      LOGICAL    NOCONV,   head

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1,         krefra

      REAL         XINT(0:MAXINT,MAXSPL),    ZINT(0:MAXINT,MAXSPL),
     :             VINT(0:MAXINT,MAXSPL)
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
     :           TRANSX,   TRANSZ,    SGNTAN,     huge,
     :           dangle,   sgnchk

      real vk, vkp1, vop

      INTEGER    J,        K,         L

      LOGICAL    XFAIL

      parameter( huge = 10000000.)


      NOCONV = .FALSE.
c     Find first intersection point.

      if(abs(beta).eq.180.) then
c        invalid angle
         noconv = .true.
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
         NOCONV = .TRUE.
         RETURN
      END IF

c     Find remaining intersections.

      DO 100 K = 1,  N

cccccc  z li added variable velocity within layer 
	 if(k.eq.1) then 
		l = 0
	 else
	 	l = norder(k-1)
	 end if
	 if ( l .lt. 0 ) l = 0
	 if(sign(k-1).gt.0.) l = max0(l-1,0) 
	 j = 1
5050     if(x(k-1).gt.xint(l,j) .and. j.le.npts(l)) then
	 	j = j+1
		goto 5050
	 end if
	 j = j - 1
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
	 if(sign(k).lt.0.) then
		vkp1 = vint(l,j)
	 else 
		l = max0(l-1,0) 
         	J = 1
5060      	IF(X(K).GT.XINT(L,J).and.j.le.npts(l)) THEN
            		J = J + 1
           		GO TO 5060
         	END IF
         	J = J - 1
		vkp1 = vint(l,j)
	 end if

c        find slope at previous intersection
         DZ(K) = A1(L,J) + 2. * A2(L,J) * X(K)
     :         + 3. * A3(L,J) * X(K)**2

c	 write(*,*) "dz=",dz(k)," xk=",x(k), "xkm1=",x(k-1)
c	 write(*,*) "zk=",z(k), "zkm1=",z(k-1)

         DELTAX(K) = X(K) - X(K-1)
         DELTAZ(K) = Z(K) - Z(K-1)
         D(K) = SQRT( DELTAX(K)**2 + DELTAZ(K)**2 )
c	 write(*,*) "d=",d(k)


c        calculate sine of angle between normal and ray
c        leaving the intersection point
         DENOM = SQRT( 1. + DZ(K)**2 )

	 IF(D(K).EQ.0.) THEN 
            NOCONV = .TRUE.
            RETURN
	 END IF

c	 write(*,*) "vk=",vk," v(k)=",v(k)," vkp1=",vkp1,
c     :          	" v(k+1)=",v(k+1), " k=",k

         SINTH = vkp1 * ( DELTAX(K) + DELTAZ(K) * DZ(K) ) /
     :           ( vk * D(K) * DENOM )

c         SINTH = V(K+1) * ( DELTAX(K) + DELTAZ(K) * DZ(K) ) /
c     :           ( V(K) * D(K) * DENOM )


c	 write(*,*) "sinth=",sinth

ccc------The following is a patch for headwaves
         if(head.and.k.eq.krefra) then
c           this is the intersection at the refracting interface
c           check for correct source segment for this set of receivers
c           sinth must have same sign as offset
            sgnchk = sinth / sgnoff
            if(sgnchk.lt.0.) then
               noconv = .true.
            else
c              now check to see how close to critical 
c              look for ray to pass through the critical angle
               dangle = (asin(abs(sinth)) - asin(sinthc))
               dangle = dangle * 180. / pi
               if(dangle.eq.0.)then
c                 sign change - actually at critical
               else
                  if(danold/dangle.lt.0.) then
c                    sign change - passed through critical
                  else
c                    haven't passed through critical yet
c                    shoot another ray
                     danold = dangle
                     noconv = .true.
                  end if
               end if
            end if
            return
         end if
ccc------End of head wave patch
               
         IF(ABS(SINTH).GE.1.) THEN
c           no transmitted ray beyond critical
            NOCONV = .TRUE.
            RETURN
         END IF

c   added by z. li to handle no reflection when velocity does not change
	 if(sign(k)*sign(k-1).lt.0.) then
         	l = norder(k)
		if(sign(k).lt.0.) l = max0(l-1,0)
         	j = 1
5070       	if(x(k).gt.xint(l,j).and.j.le.npts(l)) then
            		j = j + 1
            		go to 5070 
         	end if
         	j = j - 1
		vop = vint(l,j)
		if(vk.eq.vop) then
			noconv = .true.
	        	return
		end if
	 end if
	

c        find component of transmitted ray in x-direction
         TRANSX = ( SINTH + SIGN(K) * SQRT( 1. - SINTH**2 )
     :            * DZ(K) ) / DENOM

c        find z-component of transmitted ray
         TRANSZ = ( SINTH * DZ(K) - SIGN(K) *
     :             SQRT( 1. - SINTH**2 ) ) / DENOM

c	 write(*,*) "transx=",transx," transz=",transz

c        find tangent with downward vertical
         if(abs(transz).gt.0.) then
            TANTH = TRANSX / TRANSZ
         else
c           ray propagating horizontally (a problem for taking tangent)
            tanth = huge
         end if

c	 write(*,*) "tanth=",tanth

c        search for next intersection in the direction
c        of the x-component
cccc         if(tanth.eq.0.) then
ccc         else
ccc            DELX = ABS( TRANSX * TRUGEO ) / TRANSX
cccc         end if
ccccccccc a fix by z li
	 if(TRANSX.ne.0.) then
               DELX = ABS( TRANSX * TRUGEO ) / TRANSX
	 else 
               DELX = TRUGEO
	 end if
	
c	 write(*,*) "before findx k=",k
         CALL FINDX(X(K),Z(K),TANTH,1.,DELX,K+1,N,
     1   	X(K+1),z(k+1),XFAIL,recdpt,
     2 		XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3          MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

c	 write(*,*) "after findx k=",k," n=",n, " xfail=",xfail
c	 write(*,*) "sign(k)=",sign(k)
c	 write(*,*) "after findx k=",k," zk=",z(k), " zkp1=",z(k+1)


         IF(XFAIL) THEN
c           failed to find next intersection
            NOCONV = .TRUE.
            RETURN
         END IF

c   added by z. li to make sure ray go to the right direction
	 if(sign(k).lt.0.) then
		if(z(k+1).le.z(k)) then
			noconv = .true.
			return
		end if
	 else 
		if(z(k+1).ge.z(k)) then
			noconv = .true.
			return
		end if
	 end if

	 v(k) = vk
	 v(k+1) = vkp1

100      CONTINUE


c     if(headwv) return

      IF(X(N+1).LT.XSTART.OR.X(N+1).GT.XEND) THEN
c        end point of ray lies outside line of receivers
         NOCONV = .TRUE.
      END IF

      RETURN
      END

*---------------------------------------------------------------------

      SUBROUTINE FINDX(XK,ZK,TANTH,SGNTAN,DELX,KP1,N,XKP1,zkp1,XFAIL,
     1                 recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

c     Given a known intersection point and a ray direction,
c     this subroutine finds the intersection of the ray with
c     the next interface.  It tries for a quick solution first, using
c     Newton's method. If this fails it resorts to a simple bisection procedure
c     to solve for the intersection of a straight line (ray)
c     and a curve (interface).

      REAL       XK,      ZK,     TANTH,     DELX,     XKP1,   zkp1,
     :           SGNTAN,  recdpt

      INTEGER    KP1,   N

      LOGICAL    XFAIL

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1

      REAL         XINT(0:MAXINT,MAXSPL),    ZINT(0:MAXINT,MAXSPL)
      REAL*8       A0(0:MAXINT,MXSPM1),      A1(0:MAXINT,MXSPM1),
     :             A2(0:MAXINT,MXSPM1),      A3(0:MAXINT,MXSPM1)
      REAL         SIGN(0:MAXN)

      INTEGER      NPTS(0:MAXINT),  NINT,      NORDER(MAXN)


cc    local   variables
c     CLOSE   a small number
c     G       the equation we're trying to solve
c     G0      the value of g at the beginning of the search
c     ITER1   first iteration counter
c     ITER2   second iteration counter
c     MAXIT   max value for the iteration counters
c     X1,X2   bisection points
c     XKP1A   temporary storage of XKP1

      REAL      G,       G0,       X1,         X2,
     :          CLOSE,   xkp1a

      INTEGER   ITER1,   ITER2,    MAXIT

      PARAMETER ( MAXIT = 1000,
     :            CLOSE = 1. )

      XFAIL = .FALSE.
c     initialise the iteration counters
      ITER1 = 0
      ITER2 = 0
c     begin at the known intersection point
      XKP1 = XK
c     evaluate the function we're trying to make zero
      CALL FUNCG(XK,ZK,TANTH,SGNTAN,KP1,N,xkp1,zkp1,G0,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
      IF(XFAIL.OR.G0.EQ.0.) RETURN

      G = G0
c     first, look for a zero crossing
5     IF(G/G0.GT.0.) THEN
         ITER1 = ITER1 + 1
         IF(ITER1.GT.MAXIT) THEN
            XFAIL = .TRUE.
            RETURN
         END IF
         XKP1 = XKP1 + DELX
         CALL FUNCG(XK,ZK,TANTH,SGNTAN,KP1,N,xkp1,zkp1,G,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
         IF(XFAIL.OR.G.EQ.0.) RETURN
         GO TO 5
      END IF


c     try for a quick solution using newton's method
      xkp1a = xkp1
      xkp1 = xkp1 - delx / 2.
      CALL quick(XK,ZK,TANTH,SGNTAN,KP1,N,xkp1,zkp1,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
      if(.not.xfail) return

      XFAIL = .FALSE.
      xkp1 = xkp1a
c     now try bisection to approach solution
      X2 = XKP1
      X1 = XKP1 - DELX

10    IF(ABS(X1-X2).GT.CLOSE) THEN
         ITER2 = ITER2 + 1
         IF(ITER2.GT.MAXIT) THEN
            XFAIL = .TRUE.
            RETURN
         END IF
         XKP1  = ( X1 + X2 ) / 2.
         CALL FUNCG(XK,ZK,TANTH,SGNTAN,KP1,N,xkp1,zkp1,G,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
         IF(XFAIL.OR.G.EQ.0.) RETURN
         IF(G/G0.GT.0.) THEN
            X1 = XKP1
         ELSE
            X2 = XKP1
         END IF
         GO TO 10
      END IF

      RETURN
      END

*-----------------------------------------------------------------

      SUBROUTINE FUNCG(XK,ZK,TANTH,SGNTAN,KP1,N,
     1                 xkp1,zkp1,G,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

c     Evaluates the function g ( the equation for the
c     intersection point of the ray and the next interface )
c     for subroutine findx.

      REAL       XK,        ZK,       recdpt,   SGNTAN,
     :           XKP1,      zkp1,     TANTH,         G

      INTEGER    KP1,       N

      LOGICAL    XFAIL

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1

      REAL        XINT(0:MAXINT,MAXSPL),     ZINT(0:MAXINT,MAXSPL)
      REAL*8      A0(0:MAXINT,MXSPM1),       A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),       A3(0:MAXINT,MXSPM1)
      REAL        SIGN(0:MAXN)

      INTEGER     NPTS(0:MAXINT),   NINT,      NORDER(MAXN)

cc    local   variables
c     BURIED  depth of the receivers below the surface when we are
c             looking for the final point on the ray. Otherwise zero.
c     J       identifies section of interface
c     L       identifies interface

      real      buried
      INTEGER   J,           L

      if(kp1.eq.n+1) then
         l = 0
         buried = recdpt
      else
c        look up the number of the interface
         L = NORDER(KP1)
         buried = 0.
      end if

c     Check to see if the given value of x lies within range
c     of the model.
      IF(XKP1.LE.XINT(L,1).OR.XKP1.GT.XINT(L,NPTS(L))) THEN
         XFAIL = .TRUE.
         RETURN
      END IF

c     calculate the depth of the interface for this x
      J = 1
5     IF(XKP1.GT.XINT(L,J)) THEN
         J = J + 1
         GO TO 5
      END IF
      J = J - 1

      ZKP1 = A0(L,J) + A1(L,J) * XKP1 +
     :       A2(L,J) * XKP1**2 + A3(L,J) * XKP1**3 + buried

      if(tanth.eq.0.) then
         g = 0.
         return
      end if

      G = ZKP1 - ZK - SGNTAN * (XKP1 - XK) / TANTH
      RETURN
      END

c-----------------------------------------------------------------------

      SUBROUTINE quick(XK,ZK,TANTH,SGNTAN,KP1,N,
     1                 xkp1,zkp1,XFAIL,recdpt,
     2 		       XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

c     This subroutine solves for the intersection of the ray
c     with the next interface, when shooting.  It uses Newton's method.

      REAL       XK,           ZK,       
     :           XKP1,         zkp1,     TANTH,
     :           SGNTAN,       recdpt

      INTEGER    KP1,          N

      LOGICAL    XFAIL

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,	       MAXNP1

      REAL        XINT(0:MAXINT,MAXSPL),     ZINT(0:MAXINT,MAXSPL)
      REAL*8      A0(0:MAXINT,MXSPM1),       A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),       A3(0:MAXINT,MXSPM1)
      REAL        SIGN(0:MAXN)

      INTEGER     NPTS(0:MAXINT),   NINT,      NORDER(MAXN)

cc    local   variables
c     BURIED  depth of the receivers below the surface when we are
c             looking for the final point on the ray. Otherwise zero.
c     DZKP1   derivative of ZKP1 w.r.t. XKP1
c     G       equation we're solving 
c     GPRIME  derivative of G w.r.t. XKP1
c     ITER    iteration counter
c     J       identifies section of interface
c     L       identifies interface
c     MAXIT   max number os Newton iterations
c     SOLN    residual of equation to be solved must be < SOLN for
c             a solution

      INTEGER   iter, J,       L,      maxit
      real      soln, buried,  dzkp1,  g,  gprime

      parameter(maxit = 10)
      parameter(soln  = 1.)


      if(tanth.eq.0) then
         XFAIL = .TRUE.
         RETURN
      end if	
      if(kp1.eq.n+1) then
c        looking for end point of ray at receivers
         l = 0
         buried = recdpt
      else
c        look up the number of the interface
         L = NORDER(KP1)
         buried = 0.
      end if

      g = 2.*soln
      iter = 0
100   if(abs(g).gt.soln) then
         iter = iter + 1

         if(iter.le.maxit) then
c           Check to see if the given value of x lies within range
c           of the model.
            IF(XKP1.LE.XINT(L,1).OR.XKP1.GT.XINT(L,NPTS(L))) THEN
               XFAIL = .TRUE.
               RETURN
            END IF
c           calculate the depth of the interface for this x
            J = 1
5           IF(XKP1.GT.XINT(L,J)) THEN
               J = J + 1
               GO TO 5
            END IF
            J = J - 1
            ZKP1 = A0(L,J) + A1(L,J) * XKP1 +
     :             A2(L,J) * XKP1**2 + A3(L,J) * XKP1**3 + buried
            dZKP1 = A1(L,J) + 2. * A2(L,J) * XKP1 +
     :              3. * A3(L,J) * XKP1**2

            G = ZKP1 - ZK - SGNTAN * (XKP1 - XK) / TANTH
            gprime = dzkp1 - sgntan / tanth
      	    if(gprime.eq.0) then
         	XFAIL = .TRUE.
         	RETURN
      	    end if	
            xkp1 = xkp1 - g / gprime
            go to 100
         else
            xfail = .true.
         end if
      end if

      
      IF(XKP1.LE.XINT(L,1).OR.XKP1.GT.XINT(L,NPTS(L))) THEN
c        beyond limits of interface
         XFAIL = .TRUE.
         RETURN
      END IF

      RETURN
      end

c     End of shooting method routines.
c-----------------------------------------------------------------------
