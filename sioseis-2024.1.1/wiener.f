!*****  WIENER  Wiener-Levinson Algorithm         MATH ADVANTAGE REL 3.0
!
!    ** COPYRIGHT 1984-1985 QUANTITATIVE TECHNOLOGY CORPORATION **
!
!  CALL FORMAT
!
!       CALL WIENER (LR,R,G,F,A,IFLG,IERR)
!
!       where,
!
!       LR      Integer input filter length.
!
!       R       Real input vector,
!               auto-correlation coefficients.
!
!       G       Real input vector, right-hand side
!               coefficients.  Not used if IFLG=0.
!
!       F       Real output vector, filter coefficients if IFLG=1;
!                                   stability indicators if IFLG=0.
!
!       A       Real output vector, prediction error operator.
!
!       IFLG    Integer input processing option flag:
!                   =0 for spike deconvolution.
!                   =1 for general deconvolution.
!
!       IERR    Integer output completion code:
!                  =0 if the routine terminated normally.
!                  >0 if a failure occurred. The value of IERR
!                     is the pass number where failure occurred.
!
!
!  DESCRIPTION
!
!       This routine solves a system of single-channel normal
!       equations which arise in least-squares filtering and
!       prediction problems for single-channel time series.
!       It uses a recursive method of solving simultaneous
!       equations.
!
!       It solves:
!
!            1.  The following set of equations for F:
!                      SUM[F(s)*R(1+ABS(t-s))] = G(t)
!                      for s=1,LR  and  t=1,LR
!
!            2.  The following set of equations for A:
!                      SUM[A(s)*R(1+ABS(t-s))] = V*D
!                      for s=1,LR  and  t=1,LR
!
!                where,
!
!                      A(1) = 1.0,
!
!                      D = 1.0 when t=1,
!
!                      (D = 0.0 otherwise),
!
!                      V = A(1)*R(1)+....+A(LR)*R(LR),
!
!       All arrays are LR long.
!
!
!  REFERENCE
!
!       E. A. Robinson.  Multichannel Time Series Analysis with
!       Digital Computer Programming.
!
!       N. Levinson.  1947.  The Wiener RMS (root-mean-square)
!       Error Criterion in Filter Design and Prediction.
!       J. Math. Phys., Vol. 25,  pp. 261-278.
!
!       A. Jurkevics & R. Wiggins.  Dec 1984.  A Critique of
!       Siesmic Deconvolution Methods.  Geophysics, Vol. 49, No. 12.
!       pp. 2109-2116.
!
!
!  EXAMPLE
!
!       For General Deconvolution:
!
!       CALL WIENER (2,R,G,F,A,1,IERR)
!
!       Input Operands:
!
!       R =  10.000
!             4.000
!
!       G =   2.000
!             0.000
!
!
!       Output Operands:
!
!       F =  0.238
!           -0.095
!
!       A =  1.000
!           -0.400
!
!       IERR = 0
!
!
!       For Spike Deconvolution:
!
!       CALL WIENER (2,R,G,F,A,0,IERR)
!
!       Input Operands:
!
!       R =  10.000
!             4.000
!
!       G =   2.000
!             0.000
!
!
!       Output Operands:
!
!       F =  0.200
!           10.000
!
!       A =  1.000
!           -0.400
!
!       IERR = 0
!
!  HISTORY
!         1) May 85     D. Cooper       Original.
!
      SUBROUTINE WIENER(LR,R,G,F,A,IFLG,IERR)
!
      INTEGER LR,IFLG,IERR,ID,L2,IB,IC,LH
      REAL R(*),G(*),F(*),A(*),V,D,Q,AL,AJOLD,FL
      IF (LR.LE.0 .OR. IFLG.LT.0 .OR. IFLG.GT.1) GOTO 9000
      IERR = 0
      V = R(1)
      D = R(2)
      A(1) = 1.0
      F(1) = G(1) / V
      Q = F(1) * R(2)
      IF (LR.EQ.1) GOTO 9000
!
      DO 999 ID = 2, LR
        AL   = - D / V
        A(ID) = AL
        IF (V.GT.0.0) GOTO 100
          IERR = ID
          GOTO 9000
100     CONTINUE
        IF (IFLG.EQ.0) F(ID) = V
        V = V + AL * D
        IF (ID.EQ.2) GOTO 410
          L2 = ID/2
          IF (L2 .LT. 2) GOTO 350
            DO 299 IB=2, L2
! .......     Do pairs of elements of A:
              IC = ID - IB + 1
              AJOLD = A(IB)
              A(IB) = AJOLD + AL * A(IC)
              A(IC) = A(IC)  + AL * AJOLD
299         CONTINUE
350       CONTINUE
          IF (L2+L2.EQ.ID) GOTO 400
! .......   IF ID is Odd, cover the unpaired element:
            LH = L2 + 1
            A(LH) = A(LH) + AL * A(LH)
400       CONTINUE
410     CONTINUE
        IF (IFLG.EQ.0) GOTO 750
! ....... IF General Deconvolution:
          FL   = (G(ID) - Q) / V
          F(ID) = FL
          DO 699 IB = 1, ID-1
            IC = ID - IB + 1
            F(IB) = F(IB) + FL * A(IC)
699       CONTINUE
750     CONTINUE
        IF (ID.EQ.LR) GOTO 9000
! ....... Compute D and Q for next loop iteration.
! ....... Q is used for General Deconvolution, D for both.
        D = 0.0
        Q = 0.0
        DO 899 IB = 1, ID
          IC = ID - IB + 2
          D = D + A(IB)*R(IC)
          Q = Q + F(IB) * R(IC)
899     CONTINUE
999   CONTINUE
!
9000  RETURN
      END
