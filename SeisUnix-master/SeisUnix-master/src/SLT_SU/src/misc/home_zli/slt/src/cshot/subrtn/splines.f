* Copyright (c) Colorado School of Mines, 1990.
* All rights reserved.

      SUBROUTINE CUSPLN(NINT,X,Z,NPTS,A0,A1,A2,A3,fail,
     1                  MAXINT,MXSPM1,C,D,E,B,CV)

c     This subroutine fits a cubic spline through the points
c     defining each interface.  The curvatures at each end are
c     taken to be zero.  The remaining curvatures are found by
c     inverting a tridiagonal matrix.  Knowing the curvatures we
c     evaluate the spline coefficients.
c     Reference: Numerical Computing and Mathematical Analysis
c                by Stephen M. Pizer.  Science Research Associates, Inc.
c                Pages 307-311.



      INTEGER     MAXINT,                  MXSPM1

c     MXSPM1 should equal MAXSPL in main, less one

      REAL        X(0:MAXINT,0:MXSPM1),    Z(0:MAXINT,0:MXSPM1)
      REAL*8      A0(0:MAXINT,MXSPM1),     A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),     A3(0:MAXINT,MXSPM1)

      INTEGER     NINT,                    NPTS(0:NINT)

      logical     fail

      REAL*8    C(MXSPM1),               D(MXSPM1),
     :          E(MXSPM1),               B(MXSPM1),
     :          CV(0:MXSPM1)



cc    local   variables
c     B()     right hand side of matrix equation for curvatures
c     C()     subdiagonal of matrix to be solved for curvatures
c     CV()    curvatures at spline points
c     D()     diagonal of matrix to be solve for curvatues
c     DX      difference in x-coordinates
c     DX2     DX sqaured
c     E()     superdiagonal of matrix to be solved for curvatures
c     I       loop variable
c     INFO    indicates if matrix is singular
c     J       loop variable over interfaces
c     M       loop variable
c     MXSPM1  maximum number of points in interface, less one
c     N       number of unknown curvatures
c     SGNDET  required by sub. tridi, not used here
c     STDOUT  unit number of standard output for error messages




      REAL      SGNDET
      REAL*8    dx,   dx2, X1, X2, Z1, Z2, Z0, X0

      INTEGER   I,     INFO,     J,      N,     M


      fail = .false.

      DO 200 J = 0,  NINT


         IF(NPTS(J).EQ.2) THEN

c           Interface is a straight line.
	    X0 = X(J,0)
	    X1 = X(J,1)
	    Z0 = Z(J,0)
	    Z1 = Z(J,1)

            A0(J,1) = ( Z0*X1 - Z1*X0 ) / ( X1 - X0 )
            A1(J,1) = ( Z1 - Z0 ) / ( X1 - X0 )
            A2(J,1) = 0.
            A3(J,1) = 0.

            GO TO 200

         END IF

c        evaluating bands of tridiagonal matrix, to be inverted
c        for the curvatures ( see reference, eqn. (142) )

c        N is the number of unknown curvatures
         N = NPTS(J) - 2

c        diagonal
         DO 50  I = 1,  N
            D(I) = 2. * ( X(J,I+1) - X(J,I-1) )
50          CONTINUE

c        superdiagonal
         DO 60  I = 1,  N-1
            E(I) = X(J,I+1) - X(J,I)
60          CONTINUE

c        subdiagonal
         DO 70  I = 2,  N
            C(I) = X(J,I) - X(J,I-1)
70          CONTINUE

c        right hand side
         DO 80  I = 1,  N
	    X0 = X(J,I-1)
	    X1 = X(J,I)
	    X2 = X(J,I+1)
	    Z0 = Z(J,I-1)
	    Z1 = Z(J,I)
	    Z2 = Z(J,I+1)
            B(I) = 6. * ( (Z2-Z1) / (X2-X1)
     :                - (Z1-Z0) / (X1-X0) )
80          CONTINUE



c        invert the matrix
         CALL TRIDI(N,C,D,E,B,SGNDET,INFO)
         IF(INFO.NE.0) THEN
c           failed to fit spline
            fail = .true.
            return
         END IF


c        curvature at end points is set to zero
         CV(0) = 0.
         CV(N+1) = 0.

c        set the remaining curvatures found by tridi.
         DO 90  I = 1, N
            CV(I) = B(I)
90          CONTINUE




c        using the curvatures, solve for the spline coefficients
c        here we have expanded the product terms in reference eqn. (132)

         DO 100  M = 0,  N

            dx = x(j,m+1) - x(j,m)
            dx2 = dx * dx
	    X1 = X(J,M) 
	    Z1 = Z(J,M) 
	    X2 = X(J,M+1) 
	    Z2 = Z(J,M+1) 
            a0(j,m+1) = ( CV(M)*X2**3 - CV(M+1)*X1**3
     :                + 6.*Z1*X2
     :                - CV(M)*X2*dx2
     :                - 6.*Z2*X1 + CV(M+1)*X1
     :                * dx2 ) / ( 6.* dx )


            a1(j,m+1) = (-.5*CV(M)*X2**2 + .5*CV(M+1)*X1**2
     :          -Z1 + (CV(M) * dx2 ) / 6. + Z2
     :          - CV(M+1) * dx2 / 6. ) / dx


            a2(j,m+1) = (CV(M)*X2-CV(M+1)*X1)
     :                           /(2.* dx)


            a3(j,m+1) = ( CV(M+1) - CV(M) ) / (6.*dx)


100         CONTINUE



200      CONTINUE


      RETURN

      END



*------------------------------------------------------------------------


      SUBROUTINE CUSPLW(X,Z,NPTS,A0,A1,A2,A3,fail,
     1                  MXSPM1,C,D,E,B,CV)

c     This subroutine fits a cubic spline through the points
c     defining the well.  The code is the same as in CUSPLN
c     except that the spline coeffs are 1-D arrays and that x and z
c     are reversed in the calling sequence


      INTEGER MXSPM1
c     MXSPM1 should equal MAXSPL in main, less one

      REAL        X(0:MXSPM1),    Z(0:MXSPM1)
      REAL*8      A0(MXSPM1),     A1(MXSPM1),
     :            A2(MXSPM1),     A3(MXSPM1)

      INTEGER     NPTS

      logical   fail

      REAL*8    C(MXSPM1),               D(MXSPM1),
     :          E(MXSPM1),               B(MXSPM1),
     :          CV(0:MXSPM1)


cc    local   variables
c     B()     right hand side of matrix equation for curvatures
c     C()     subdiagonal of matrix to be solved for curvatures
c     CV()    curvatures at spline points
c     D()     diagonal of matrix to be solve for curvatues
c     E()     superdiagonal of matrix to be solved for curvatures
c     I       loop variable
c     INFO    indicates if matrix is singular
c     J       loop variable over interfaces
c     M       loop variable
c     MXSPM1  maximum number of points in well, less one
c     N       number of unknown curvatures
c     SGNDET  required by sub. tridi, not used here
c     STDOUT  unit number of standard output for error messages




      REAL	SGNDET
      REAL*8    dx,   dx2
      REAL*8 	X0,X1,X2,Z0,Z1,Z2

      INTEGER   I,     INFO,     J,      N,     M



      fail = .false.

      IF(NPTS.EQ.2) THEN

c        well is a straight line.

	 Z1 = Z(1)
	 Z0 = Z(0)
	 X1 = X(1)
	 X0 = X(0) 
         A0(1) = ( Z0*X1 - Z1*X0 ) / ( X1 - X0 )
         A1(1) = ( Z1 - Z0 ) / ( X1 - X0 )
         A2(1) = 0.
         A3(1) = 0.

         GO TO 200

      END IF

c     evaluating bands of tridiagonal matrix, to be inverted
c     for the curvatures ( see reference, eqn. (142) )

c     n is the number of unknown curvatures
      N = NPTS - 2

c     diagonal
      DO 50  I = 1,  N
         D(I) = 2. * ( X(I+1) - X(I-1) )
50       CONTINUE

c     superdiagonal
      DO 60  I = 1,  N-1
         E(I) = X(I+1) - X(I)
60       CONTINUE

c     subdiagonal
      DO 70  I = 2,  N
         C(I) = X(I) - X(I-1)
70       CONTINUE

c     right hand side
      DO 80  I = 1,  N
	 Z2 = Z(I+1)
	 Z1 = Z(I)
	 Z0 = Z(I-1)
	 X2 = X(I+1)
	 X1 = X(I)
	 X0 = X(I-1)
         B(I) = 6. * ( (Z2-Z1) / (X2-X1)
     :             - (Z1-Z0) / (X1-X0) )
80       CONTINUE



c     invert the matrix
      CALL TRIDI(N,C,D,E,B,SGNDET,INFO)
      IF(INFO.NE.0) THEN
c        failed to fit spline.
         fail = .true.
         return
      END IF


c     curvature at end points is set to zero
      CV(0) = 0.
      CV(N+1) = 0.

c     set the remaining curvatures found by tridi.
      DO 90  I = 1, N
         CV(I) = B(I)
90       CONTINUE




c     using the curvatures, solve for the spline coefficients
c     here we have expanded the product terms in reference eqn. (132)

      DO 100  M = 0,  N

         dx = x(m+1)-x(m)
         dx2 = dx * dx

	 X1 = X(M)
	 Z1 = Z(M)
	 X2 = X(M+1)
	 Z2 = Z(M+1)
	 

         a0(m+1) = ( CV(M)*X2**3 - CV(M+1)*X1**3
     :             + 6.*Z1*X2
     :             - CV(M)*X2*dx2
     :             - 6.*Z2*X1 + CV(M+1)*X1
     :             * dx2 ) / ( 6.*dx )


         a1(m+1) = ( -.5*CV(M)*X2*X2 + .5*CV(M+1)*X1*X1
     :       -Z1 + ( CV(M)*dx2 ) / 6. + Z2
     :       - CV(M+1)*dx2 / 6. ) / dx


         a2(m+1) = (CV(M)*X2-CV(M+1)*X1)
     :                        /(2.*dx)


         a3(m+1) = ( CV(M+1) - CV(M) ) / (6.*dx)


100      CONTINUE



200   CONTINUE


      RETURN

      END

*-------------------------------------------------------------------


