* Copyright (c) Colorado School of Mines, 1990.
* All rights reserved.

c                  CSHOT1 GRAPHICS ROUTINES

c---------------------------------------------------------------------------

      subroutine ploti()
c     Initialize the plot

      return
      end

c---------------------------------------------------------------------------

      subroutine plote()
c     Close the plot

      return
      end

c---------------------------------------------------------------------------

      SUBROUTINE PLOTIN(ipen,
     1                  MAXINT,MAXSPL,MXSPM1,MAXN,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  VINT,ivch)

c     Plots the interfaces
      integer  ipen

      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN


      REAL        XINT(0:MAXINT,MAXSPL),     ZINT(0:MAXINT,MAXSPL)
      REAL        VINT(0:MAXINT,MAXSPL)
      REAL*8      A0(0:MAXINT,MXSPM1),       A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),       A3(0:MAXINT,MXSPM1)
      REAL        SIGN(0:MAXN)

      INTEGER     NPTS(0:MAXINT),   NINT,      NORDER(MAXN)
      INTEGER     ivch

cc    Local variables:
c     DX      X spacing between spline points
c     I       loop variable
c     IERR    error flag required by plot routines
c     J,K     loop variables
c     NXPTS   number of points between spline points, through which
c             to draw a line representing the interface
c     X()     array of interface x coordinates to plot
c     Z()     array of interface z coordinates to plot



      INTEGER     I,      J,     K,    NXPTS
      PARAMETER ( NXPTS = 10)
      REAL	DX,   X(0:NXPTS),  Z(0:NXPTS)
      REAL*8	T	

      REAL va, vb
      INTEGER l

      DO 40 I = 0,  NINT

         DO 30  J = 1, NPTS(I) - 1

            DX = XINT(I,J+1) - XINT(I,J)

	    if(ivch.eq.1 .and. i.gt.0) then
		vb = vint(i,j)
		l = max0(i-1,0)
		k = 1
10		if(xint(i,j).gt.xint(l,k)) then
			k = k + 1
			goto 10
		end if
		k = max0(k - 1,1)
		va = vint(l,k)
	    else
		va = 1.0
		vb = 2.0
	    end if 

	    if(va.ne.vb) then
            	DO 20   K = 0,  NXPTS
               		T = XINT(I,J) + K * DX / NXPTS
               		X(K) = T
               		Z(K) = A0(I,J) + A1(I,J)*T + A2(I,J)*T**2
     :                                       + A3(I,J)*T**3
20              CONTINUE
            	call line(x,z,nxpts+1,ipen)
	    end if

30          CONTINUE

40       CONTINUE


      RETURN
      end

c-------------------------------------------------------------------

      subroutine plotwl(zwell,nwell,w0,w1,w2,w3,ipen)

c     Plots the well
      integer  nwell,          ipen
      real     zwell(0:nwell)
      real*8   w0(nwell),      w1(nwell),   w2(nwell),  w3(nwell)
      real*8   t


cc    Local variables
c     DZ      z-distance between spline points
c     J,K     counters
c     NXPTS   number of points between spline points in line plot
c     X()     array of well x coordinates to plot
c     Z()     array of well z coordinates to plot

      integer   nxpts,         j,               k
      parameter(nxpts = 10 )
      real      x(0:nxpts),  z(0:nxpts),    dz


      DO 30  J = 1, nwell - 1

         DZ = zwell(J) - zwell(J-1)

         DO 20   K = 0,  NXPTS
            t = zwell(J-1) + K * DZ / NXPTS
            Z(K) = t
            X(K) = w0(J) + w1(J)*t + w2(J)*t**2
     :                                + w3(j)*t**3
20          CONTINUE

         call line(x,z,nxpts+1,ipen)

30       CONTINUE

      RETURN
      END

c---------------------------------------------------------------------------

      SUBROUTINE rayplt(X,Z,N,ipen)

c     Plots the raypaths.

      INTEGER      N,          ipen
      REAL         X(0:N+3),   Z(0:N+3)

      call line(x,z,n+2,ipen)

      RETURN
      END

c------------------------------------------------------------------------

      subroutine pltsym(xsym,zsym,nsym,trugeo,ipen)

c     Plots a symbol (a square) at the each of the locations specified

      integer   nsym,        ipen
      real      xsym(nsym),  zsym(nsym),   trugeo

cc    Local variables:
c     I     counter
c     SIDE  length of side of square in model units
c     X()   x-coordinates of square
c     Z()   z-coordinates of square

      integer   i,     npix
      real      x(5),  z(5),  side,  smag

ccc   Increase symmag for bigger symbol
      parameter( symmag= .1 )


      side = symmag * trugeo 
      do 10 i = 1,  nsym
         x(1) = xsym(i) - side / 2.
         z(1) = zsym(i) 
         x(2) = x(1)
         z(2) = z(1) + side 
         x(3) = x(2) + side
         z(3) = z(2)
         x(4) = x(3)
         z(4) = z(1) 
         x(5) = x(1)
         z(5) = z(1)
         call line(x,z,5,ipen)
10       continue

      return
      end

c---------------------------------------------------------------------------

      subroutine line(x,z,n,ipen)

c     Draws straight line segments through coordinates in
c     x and z arrays.

      integer n,    ipen,     stdout
      real x(n),  z(n)
      parameter ( stdout = 6)

c     Local variables:
c     I     Counter

      write(stdout,*)n,ipen
      do 10 i = 1, n
	 write(stdout,*)x(i),z(i)
10       continue

      return
      end

c---------------------------------------------------------------------------
