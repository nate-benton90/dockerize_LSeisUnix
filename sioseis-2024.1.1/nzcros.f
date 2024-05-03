!*****  NZCROS Find Specified Zero Crossing          MTHADV EXT. REL 1.0
!
!    ** COPYRIGHT 1986 QUANTITATIVE TECHNOLOGY CORPORATION **
!
!  CALL FORMAT
!
!       CALL NZCROS (A,IA,IN,IL,NF,N)
!
!       where,
!
!       A       Real input vector.
!
!       IA      Integer input stride for vector A.
!
!       IN      Integer input, zero crossing to find.
!
!       IL      Integer output, displacement of INth zero crossing.
!
!       NF      Integer output, number of zero crossings found.
!
!       N       Integer input element count.
!
!
!  DESCRIPTION
!
!       This routine scans the input vector A, with stride
!       and direction specified by IA, searching for transitions
!       in the signs of sequential elements.  (Zero is considered
!       to be a positive number.)  The scan terminates when
!       the INth zero crossing is found or N elements have
!       been scanned.  If the specified zero crossing is not
!       found, IL is set to zero.  NF is always set, even if
!       the specified zero crossing is not found.
!
!
!  EXAMPLE
!
!       CALL NZCROS (A,1,3,IL,NF,5)
!
!       Input Operands:
!
!       A =  0.000
!           -1.000
!            0.000
!            1.000
!            2.000
!
!
!       Output Operands:
!
!       IL = 0
!
!       NF = 2
!
!
!  HISTORY
!         1) Jul 86     D. Benua        Original.
!
!-----------------------------------------------------------------------
!
      SUBROUTINE NZCROS(A,IA,IN,IL,NF,N)
!
      INTEGER IA,IN,IL,NF,N,M,II,ICUR,IPRE
      REAL A(1)
!
!-----------------------------------------------------------------------
!
      IF (N.LE.0) GO TO 14
      IF (IN .LE. 0) GOTO 14
      NF = 0
      II = 1
      DO 12 M=1,N
         IF (A(II) .GE. 0.0) GOTO 6
            ICUR = -1
         GOTO 8
6           ICUR = 1
8        IF (M .EQ. 1) IPRE = ICUR
         IF (ICUR .NE. IPRE) NF = NF + 1
         IF (NF .NE. IN) GOTO 10
            IL = II - 1
            GOTO 14
10       IPRE = ICUR
         II = II + IA
12    CONTINUE
      IL = 0
14    RETURN
      END
