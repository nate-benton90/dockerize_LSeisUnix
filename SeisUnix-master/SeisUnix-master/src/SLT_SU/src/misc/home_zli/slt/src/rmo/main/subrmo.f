C  DATE:      MARCH 10,  1987                                           19490500
C  TYPE:      SUBROUTINE                                                19490600
C  LANGUAGE:  FORTRAN 77                                                19490700
C                                                                       19490900
C  NAME: ANALY                                                          19491000
C                                                                       19491200
C  PURPOSE: INVERT DATA (FREQUENCY SLICE) FOR MODEL PARAMETERS          19491300
C                                                                       19491600
C  LINKAGE EDIT:                                                        19491700
C                                                                       19491900
C     CALL ANALY( ARAYLN   ,ARAYLL                                      19492000
C    *            ,FSLICE   ,WRKBUF   ,FPSPAC                           19492100
C    *            ,OFFSET   ,PVSTRT   ,PVINCS                           19492200
C    *            ,NPVSMP   ,NCHANS   ,SFREQ                            19492300
C    *            ,PRWHIT                                               19492400
C    *           )                                                      19492500
C                                                                       19492700
C  NOTE: VARIABLE NAMES FOLLOW FORTRAN CONVENTION (I.E. VARIABLE NAMES  19492800
C        BEGINNING WITH I-NX ARE INTEGERS). AN EXCEPTION TO THIS RULE   19492900
C        IS THE EXPLICIT DECLARATION OF CHARACTER VARIABLES.            19493000
C                                                                       19493100
C                                                                       19493200
C  ARGUMENT  USE   TYPE  DESCRIPTION                                    19493300
C  --------  ---   ----  -----------                                    19493400
C  ARAYLN           C*8  FOWARD MODELING MATRIX                         19493500
C  ARAYLL           C*8  LEAST SQAURES INVERSION MATRIX                 19493700
C  FSLICE     IN    C*8  ARRAY CONTAINING INPUT FREQUENCY SLICE (DATA)  19493800
C  WRKBUF           R*4  WORK BUFFER NEEDED BY LEQT1C                   19493900
C  FPSPAC           C*8  ARRAY CONTAINING MODELING COEFFICIENTS         19494000
C  OFFSET     IN    R*4  ARRAY CONTAINING OFFSETS (SQUARED AND          19494100
C                         NORMALIZED)                                   19494200
C  PVSTRT     IN    R*4  STARTING P VALUE                               19494300
C  PVINCS     IN    R*4  P VALUE INCREMENT                              19494400
C  NPVSMP     IN    R*4  NUMBER OF P VALUES TO MODEL                    19494500
C  NCHANS     IN    R*4  NUMBER OF INPUT TRACES (DIMENSION OF FSLICE)   19494600
C  SFREQ      IN    R*4  FREQUENCY VALUE                                19494700
C  PRWHIT     IN    R*4  PREWHITENING VALUE                             19494800
C                                                                       19494900
                                                                        19495000
      SUBROUTINE ANALY ( ARAYLN   ,ARAYLL                               19495200
     *                   ,FSLICE   ,WRKBUF   ,FPSPAC                    19495300
     *                   ,OFFSET   ,PVSTRT   ,PVINCS                    19495400
     *                   ,NPVSMP   ,NCHANS   ,SFREQ                     19495500
     *                   ,PRWHIT                                        19495600
     *                  )                                               19495700
                                                                        19495800
      COMPLEX    ARAYLN(NCHANS,NPVSMP)                                  19495900
      COMPLEX    ARAYLL(NPVSMP,NPVSMP) ,FSLICE(NCHANS)                  19496000
      COMPLEX    FPSPAC(NPVSMP)                                         19496100
      DIMENSION  OFFSET(NCHANS)        ,WRKBUF(*)                       19496200
      LOGICAL    FIRST/.TRUE./                                          19496300
                                                                        19496400
      DATA       MR,IJOB/1,0/                                           19496500
                                                                        19496600
      PVAL = PVSTRT                                                     19496700
      PVNC = PVINCS                                                     19496800
                                                                        19496900
C BUILD MODELING MATRIX AND ITS HERMITIAN                               19497000
                                                                        19497100
      DO 10 IPVL=1,NPVSMP                                               19497200
                                                                        19497300
         S =  SFREQ * ( PVAL + (IPVL-1)*PVNC )                          19497400
                                                                        19497500
         DO 20 ICHN=1,NCHANS                                            19497600
                                                                        19497700
         T = S * OFFSET(ICHN)                                           19498200
         TI = SIN(T)                                                    19498300
         TR = COS(T)                                                    19498400
         ARAYLN(ICHN,IPVL) = CMPLX( TR, TI )                            19498500
                                                                        19498600
20       CONTINUE                                                       19498700
                                                                        19498800
10    CONTINUE                                                          19499100
                                                                        19499200
C PREMULTIPLY THE MODELING MATRIX BY ITS HERMITIAN                      19499300
                                                                        19501100
      CALL CGEMUL(ARAYLN, ARAYLN, ARAYLL, NCHANS, NPVSMP, NPVSMP)       19501300
                                                                        19501500
C APPLY PREWHITENING IF REQUESTED                                       19501600
                                                                        19501700
      IF ( PRWHIT .GT. .0 ) THEN                                        19501800
                                                                        19501900
        DO 75 IROW=1,NPVSMP                                             19502000
                                                                        19502100
           ARAYLL(IROW,IROW)   = CMPLX(PRWHIT + REAL(ARAYLL(IROW,IROW)),19502300
     *                                          IMAG(ARAYLL(IROW,IROW)))19502400
                                                                        19502500
75        CONTINUE                                                      19502600
                                                                        19502700
      ENDIF                                                             19502800
                                                                        19502900
C BUILD NEW ROTATED DATA VECTOR: PREMULTIPLY DATA MATRIX BY HERMITIAN   19503000
                                                                        19503100
      CALL CGEMUL(ARAYLN, FSLICE, FPSPAC, NCHANS, NPVSMP, 1)            19504600
                                                                        19504800
                                                                        19504900
C INVERT DATA FOR MODEL PARAMETERS (IMSL ROUTINE)                       19505000
                                                                        19505100
      CALL LEQT1C( ARAYLL ,NPVSMP  ,NPVSMP ,FPSPAC                      19505200
     *            ,MR     ,NPVSMP  ,IJOB   ,WRKBUF                      19505300
     *            ,IERR                                                 19505400
     *           )                                                      19505500
                                                                        19505600
      RETURN                                                            19505700
      END                                                               19505800
                                                                        19505900
                                                                        19506200
C   IMSL ROUTINE NAME   - LEQT1C                                        19506300
C                                                                       19506400
C-----------------------------------------------------------------------19506500
C                                                                       19506600
C   COMPUTER            - IBM77/SINGLE                                  19506700
C                                                                       19506800
C   LATEST REVISION     - JANUARY 1, 1978                               19506900
C                                                                       19507000
C   PURPOSE             - MATRIX DECOMPOSITION, LINEAR EQUATION         19507100
C                           SOLUTION - SPACE ECONOMIZER SOLUTION -      19507200
C                           COMPLEX MATRICES                            19507300
C                                                                       19507400
C   USAGE               - CALL LEQT1C (A,N,IA,B,M,IB,IJOB,WA,IER)       19507500
C                                                                       19507600
C   ARGUMENTS    A      - INPUT COMPLEX N BY N MATRIX CONTAINING THE    19507700
C                           COMPLEX COEFFICIENTS OF THE EQUATION AX = B.19507800
C                         ON OUTPUT, A CONTAINS THE L-U DECOMPOSITION OF19507900
C                           A ROWWISE PERMUTATION OF THE INPUT MATRIX A.19508000
C                N      - ORDER OF MATRIX A. (INPUT)                    19508100
C                IA     - ROW DIMENSION OF MATRIX A EXACTLY AS          19508200
C                           SPECIFIED IN THE DIMENSION STATEMENT IN THE 19508300
C                           CALLING PROGRAM. (INPUT)                    19508400
C                B      - INPUT COMPLEX N BY M MATRIX CONTAINING THE    19508500
C                           M COMPLEX VALUED RIGHT HAND SIDES OF THE    19508600
C                           EQUATION AX = B.                            19508700
C                         ON OUTPUT, THE SOLUTION MATRIX X REPLACES B.  19508800
C                           IF IJOB=1, B IS NOT USED.                   19508900
C                M      - NUMBER OF RIGHT HAND SIDES (COLUMNS IN B).    19509000
C                           (INPUT)                                     19509100
C                IB     - ROW DIMENSION OF MATRIX B EXACTLY AS SPECIFIED19509200
C                           IN THE DIMENSION STATEMENT IN THE CALLING   19509300
C                           PROGRAM. (INPUT)                            19509400
C                IJOB   - INPUT OPTION PARAMETER.  IJOB=I IMPLIES WHEN  19509500
C                           I=0, FACTOR THE MATRIX AND SOLVE THE        19509600
C                             EQUATION AX=B.                            19509700
C                           I=1, FACTOR THE MATRIX A.                   19509800
C                           I=2, SOLVE THE EQUATION AX=B.  THIS         19509900
C                             OPTION IMPLIES THAT LEQT1C HAS ALREADY    19510000
C                             BEEN CALLED USING IJOB=0 OR 1 SO THAT     19510100
C                             THE MATRIX HAS ALREADY BEEN FACTORED.  IN 19510200
C                             THIS CASE OUTPUT MATRIX A MUST HAVE BEEN  19510300
C                             SAVED FOR REUSE IN THE CALL TO LEQT1C.    19510400
C                WA     - WORK AREA OF LENGTH N CONTAINING THE PIVOT    19510500
C                           INDICES.                                    19510600
C                IER    - ERROR PARAMETER. (OUTPUT)                     19510700
C                         TERMINAL ERROR                                19510800
C                           IER=129 INDICATES THAT MATRIX A IS          19510900
C                             ALGORITHMICALLY SINGULAR.  (SEE THE       19511000
C                             CHAPTER L PRELUDE.)                       19511100
C                                                                       19511200
C   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32                         19511300
C                       - SINGLE/H36,H48,H60                            19511400
C                                                                       19511500
C   REQD. IMSL ROUTINES - UERTST,UGETIO                                 19511600
C                                                                       19511700
C   NOTATION            - INFORMATION ON SPECIAL NOTATION AND           19511800
C                           CONVENTIONS IS AVAILABLE IN THE MANUAL      19511900
C                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP  19512000
C                                                                       19512100
C   REMARKS  1.  WHEN IJOB=1, ARGUMENTS B, M AND IB ARE NOT USED BY     19512200
C                LEQT1C.                                                19512300
C            2.  INPUT MATRIX A IS DESTROYED WHEN IJOB=0 OR 1. WHEN     19512400
C                IJOB=0 OR 2, B IS REPLACED WITH THE SOLUTION X.        19512500
C            3.  LEQT1C CAN BE USED TO COMPUTE THE INVERSE OF A COMPLEX 19512600
C                MATRIX. THIS IS DONE BY CALLING LEQT1C WITH M=N,       19512700
C                B=THE N BY N IDENTITY MATRIX AND IJOB=0. WHEN N IS     19512800
C                LARGE, IT MAY BE MORE PRACTICAL TO COMPUTE THE INVERSE 19512900
C                A COLUMN AT A TIME. TO DO THIS, FIRST CALL LEQT1C WITH 19513000
C                IJOB=1 TO FACTOR A. MAKE SUCCEEDING CALLS WITH M=1, B  19513100
C                =A COLUMN OF THE IDENTITY MATRIX AND IJOB=2. B WILL BE 19513200
C                REPLACED BY THE CORRESPONDING COLUMN OF A INVERSE.     19513300
C            4.  THE DETERMINANT OF A CAN BE COMPUTED AFTER LEQT1C HAS  19513400
C                BEEN CALLED AS FOLLOWS                                 19513500
C                                                                       19513600
C                  DET = (1.0,0.0)                                      19513700
C                  DO 5 I = 1,N                                         19513800
C                     IPVT = WA(I)                                      19513900
C                     IF (IPVT .NE. I) DET = -DET                       19514000
C                     DET = DET*A(I,I)                                  19514100
C                5 CONTINUE                                             19514200
C                                                                       19514300
C   COPYRIGHT           - 1978 BY IMSL, INC. ALL RIGHTS RESERVED.       19514400
C                                                                       19514500
C   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN 19514600
C                           APPLIED TO THIS CODE. NO OTHER WARRANTY,    19514700
C                           EXPRESSED OR IMPLIED, IS APPLICABLE.        19514800
C                                                                       19514900
C-----------------------------------------------------------------------19515000
C                                                                       19515100
      SUBROUTINE LEQT1C (A,N,IA,B,M,IB,IJOB,WA,IER)                     19515200
C                                  SPECIFICATIONS FOR ARGUMENTS         19515300
                                                                        19515400
      PARAMETER (KUT=12)                                                19515500
      COMPLEX   TEMPI                                                   19515600
                                                                        19515700
      INTEGER            N,IA,M,IB,IJOB,IER                             19515800
      COMPLEX            A(IA,N),B(IB,M)                                19515900
      REAL               WA(N)                                          19516000
C                                  SPECIFICATIONS FOR LOCAL VARIABLES   19516100
      REAL               P,Q,ZERO,ONE,T(2),RN,BIG                       19516200
      COMPLEX            SUM,TEMP                                       19516300
      INTEGER            I,J,JM1,IM1,K,IMAX,JP1,IW,N1                   19516400
      EQUIVALENCE        (SUM,T(1))                                     19516500
      DATA               ZERO/0.0/,ONE/1.E0/                            19516600
C                                  INITIALIZATION                       19516700
C                                  FIRST EXECUTABLE STATEMENT           19516800
      IER = 0                                                           19516900
      IF (IJOB .EQ. 2) GO TO 75                                         19517000
      RN = N                                                            19517100
C                                  FIND EQUILIBRATION FACTORS           19517200
      DO 10 I=1,N                                                       19517300
         BIG = ZERO                                                     19517400
         DO 5 J=1,N                                                     19517500
            TEMP = A(I,J)                                               19517600
            P = CABS(TEMP)                                              19517700
            IF (P .GT. BIG) BIG = P                                     19517800
    5    CONTINUE                                                       19517900
         IF (BIG .EQ. ZERO) GO TO 105                                   19518000
         WA(I) = ONE/BIG                                                19518100
   10 CONTINUE                                                          19518200
C                                  L-U DECOMPOSITION                    19518300
      DO 70 J = 1,N                                                     19518400
         JM1 = J-1                                                      19518500
         IF (JM1 .LT. 1) GO TO 25                                       19518600
C                                  COMPUTE U(I,J), I=1,...,J-1          19518700
         DO 20 I=1,JM1                                                  19518800
            SUM = A(I,J)                                                19518900
            IM1 = I-1                                                   19519000
            IF (IM1 .LT. 1) GO TO 20                                    19519100
                                                                        19519600
C USE VECTORIZED FUNCTIONS FOR COMPLEX ARRAY MULTIPLY                           
                                                                                
      IF (IM1 .LT. KUT) THEN                                            19519700
CIBMD PREFER SCALAR                                                     19519800
             DO 15 K=1,IM1                                              19519900
                TAR = REAL(A(I,K))                                      19520100
                TAI = IMAG(A(I,K))                                      19520200
                TR  = TAR * REAL(A(K,J)) - TAI * IMAG(A(K,J))           19520300
                TI  = TAR * IMAG(A(K,J)) + TAI * REAL(A(K,J))           19520400
                SUM = SUM - CMPLX( TR, TI)                              19520500
   15           CONTINUE                                                19520600
                        ELSE                                            19520700
CIBMD PREFER VECTOR                                                     19520800
             DO 16 K=1,IM1                                              19520900
                TAR = REAL(A(I,K))                                      19521100
                TAI = IMAG(A(I,K))                                      19521200
                TR  = TAR * REAL(A(K,J)) - TAI * IMAG(A(K,J))           19521300
                TI  = TAR * IMAG(A(K,J)) + TAI * REAL(A(K,J))           19521400
                SUM = SUM - CMPLX( TR, TI)                              19521500
   16           CONTINUE                                                19521600
                        ENDIF                                           19521700
                                                                        19521800
            A(I,J) = SUM                                                19521900
   20    CONTINUE                                                       19522000
   25    P = ZERO                                                       19522100
C                                  COMPUTE U(J,J) AND L(I,J), I=J+1,...,19522200
         DO 45 I=J,N                                                    19522300
            SUM = A(I,J)                                                19522400
            IF (JM1 .LT. 1) GO TO 40                                    19522500
                                                                        19523000
C USE VECTORIZED FUNCTIONS FOR COMPLEX ARRAY MULTIPLY                           
                                                                                
      IF (JM1 .LT. KUT) THEN                                            19523100
CIBMD PREFER SCALAR                                                     19523200
             DO 35 K=1,JM1                                              19523300
C..             SUM = SUM-A(I,K)*A(K,J)                                 19523400
                TAR = REAL(A(I,K))                                      19523500
                TAI = IMAG(A(I,K))                                      19523600
                TR  = TAR * REAL(A(K,J)) - TAI * IMAG(A(K,J))           19523700
                TI  = TAR * IMAG(A(K,J)) + TAI * REAL(A(K,J))           19523800
                SUM = SUM - CMPLX( TR, TI)                              19523900
   35           CONTINUE                                                19524000
                        ELSE                                            19524100
CIBMD PREFER VECTOR                                                     19524200
             DO 36 K=1,JM1                                              19524300
C..             SUM = SUM-A(I,K)*A(K,J)                                 19524400
                TAR = REAL(A(I,K))                                      19524500
                TAI = IMAG(A(I,K))                                      19524600
                TR  = TAR * REAL(A(K,J)) - TAI * IMAG(A(K,J))           19524700
                TI  = TAR * IMAG(A(K,J)) + TAI * REAL(A(K,J))           19524800
                SUM = SUM - CMPLX( TR, TI)                              19524900
   36           CONTINUE                                                19525000
                        ENDIF                                           19525100
                                                                        19525200
            A(I,J) = SUM                                                19525300
   40       Q = WA(I)*CABS(SUM)                                         19525400
            IF (P .GE. Q) GO TO 45                                      19525500
            P = Q                                                       19525600
            IMAX = I                                                    19525700
   45    CONTINUE                                                       19525800
C                                  TEST FOR ALGORITHMIC SINGULARITY     19525900
         Q = RN+P                                                       19526000
         IF (Q .EQ. RN) GO TO 105                                       19526100
         IF (J .EQ. IMAX) GO TO 60                                      19526200
C                                  INTERCHANGE ROWS J AND IMAX          19526300
         DO 50 K=1,N                                                    19526400
            TEMP = A(IMAX,K)                                            19526500
            A(IMAX,K) = A(J,K)                                          19526600
            A(J,K) = TEMP                                               19526700
   50    CONTINUE                                                       19526800
         WA(IMAX) = WA(J)                                               19526900
   60    WA(J) = IMAX                                                   19527000
         JP1 = J+1                                                      19527100
         IF (JP1 .GT. N) GO TO 70                                       19527200
                                                                        19527300
                                                                        19527400
C                                  DIVIDE BY PIVOT ELEMENT U(J,J)       19527500
                                                                        19527600
         TEMPI = ONE / A(J,J)                                           19527800
                                                                        19527900
         IF (N-JP1+1 .LT. KUT) THEN                                     19528400
CIBMD PREFER SCALAR                                                     19528500
             DO 65 I = JP1,N                                            19528600
   65           A(I,J) = A(I,J) * TEMPI                                 19528700
                          ELSE                                          19528900
CIBMD PREFER VECTOR                                                     19529000
             DO 66 I = JP1,N                                            19529100
   66           A(I,J) = A(I,J) * TEMPI                                 19529200
                          ENDIF                                         19529400
                                                                        19529500
   70 CONTINUE                                                          19529600
   75 IF (IJOB .EQ. 1) GO TO 9005                                       19529700
      DO 103 K = 1,M                                                    19529800
C                                  SOLVE UX = Y FOR X                   19529900
         IW = 0                                                         19530000
         DO 90 I = 1,N                                                  19530100
            IMAX = WA(I)                                                19530200
            SUM = B(IMAX,K)                                             19530300
            B(IMAX,K) = B(I,K)                                          19530400
            IF (IW .EQ. 0) GO TO 85                                     19530500
            IM1 = I-1                                                   19530600
                                                                        19531100
         IF (I  -IW   .LT. KUT) THEN                                    19531300
CIBMD PREFER SCALAR                                                     19531400
             DO 80 J = IW,IM1                                           19531500
                TAR = REAL(A(I,J))                                      19531700
                TAI = IMAG(A(I,J))                                      19531800
                TR  = TAR * REAL(B(J,K)) - TAI * IMAG(B(J,K))           19531900
                TI  = TAR * IMAG(B(J,K)) + TAI * REAL(B(J,K))           19532000
                SUM = SUM - CMPLX( TR, TI)                              19532100
   80           CONTINUE                                                19532200
                                ELSE                                    19532300
CIBMD PREFER VECTOR                                                     19532400
             DO 81 J = IW,IM1                                           19532500
                TAR = REAL(A(I,J))                                      19532700
                TAI = IMAG(A(I,J))                                      19532800
                TR  = TAR * REAL(B(J,K)) - TAI * IMAG(B(J,K))           19532900
                TI  = TAR * IMAG(B(J,K)) + TAI * REAL(B(J,K))           19533000
                SUM = SUM - CMPLX( TR, TI)                              19533100
   81           CONTINUE                                                19533200
                                ENDIF                                   19533300
                                                                        19533400
            GO TO 88                                                    19533500
   85       IF (T(1) .NE. ZERO .OR. T(2) .NE. ZERO) IW = I              19533600
   88       B(I,K) = SUM                                                19533700
   90    CONTINUE                                                       19533800
C                                  SOLVE LY = B FOR Y                   19533900
         N1 = N+1                                                       19534000
         DO 100 IW = 1,N                                                19534100
            I = N1-IW                                                   19534200
            JP1 = I+1                                                   19534300
            SUM = B(I,K)                                                19534400
            IF (JP1 .GT. N) GO TO 98                                    19534500
                                                                        19535000
         IF (N-JP1+1 .LT. KUT) THEN                                     19535100
CIBMD PREFER SCALAR                                                     19535200
             DO 95 J = JP1,N                                            19535300
                TAR = REAL(A(I,J))                                      19535500
                TAI = IMAG(A(I,J))                                      19535600
                TR  = TAR * REAL(B(J,K)) - TAI * IMAG(B(J,K))           19535700
                TI  = TAR * IMAG(B(J,K)) + TAI * REAL(B(J,K))           19535800
                SUM = SUM - CMPLX( TR, TI)                              19535900
   95           CONTINUE                                                19536000
                          ELSE                                          19536100
CIBMD PREFER VECTOR                                                     19536200
             DO 96 J = JP1,N                                            19536300
                TAR = REAL(A(I,J))                                      19536500
                TAI = IMAG(A(I,J))                                      19536600
                TR  = TAR * REAL(B(J,K)) - TAI * IMAG(B(J,K))           19536700
                TI  = TAR * IMAG(B(J,K)) + TAI * REAL(B(J,K))           19536800
                SUM = SUM - CMPLX( TR, TI)                              19536900
   96           CONTINUE                                                19537000
                          ENDIF                                         19537100
                                                                        19537200
   98       B(I,K) = SUM/A(I,I)                                         19537300
  100    CONTINUE                                                       19537400
  103 CONTINUE                                                          19537500
      GO TO 9005                                                        19537600
C                                  ALGORITHMIC SINGULARITY              19537700
  105 IER = 129                                                         19537800

 9005 RETURN                                                            19538200
      END                                                               19538300
                                                                        19538400
                                                                        19538700
C  DATE:      MARCH 10,  1987                                           19538800
C  TYPE:      SUBROUTINE                                                19538900
C  LANGUAGE:  FORTRAN 77                                                19539000
C                                                                       19550000
C  NAME: BLDMD                                                          19560000
C                                                                       19580000
C  PURPOSE: BUILD MODEL OF MULTIPLES (OR PRIMARIES) ON FREQUENCY        19590000
C           SLICE AT A TIME. A FREQUENCY SLICE OF THE MODELING          19600000
C           PARAMETERS ARE INPUT AND A FREQUENCY SLICE OF EITHER        19610000
C           THE MULTIPLE MODEL OR PRIMARY MODEL IS OUTPUT.              19620000
C                                                                       19630000
C  LINKAGE EDIT:                                                        19650000
C                                                                       19670000
C     CALL BLDMD( ARAYLN   ,FSLICE   ,FPSPAC                            19680000
C    *            ,NPVCUT   ,NPVSMP   ,NCHANS                           19690000
C    *           )                                                      19700000
C                                                                       19720000
C  NOTE: VARIABLE NAMES FOLLOW FORTRAN CONVENTION (I.E. VARIABLE NAMES  19730000
C        BEGINNING WITH I-NX ARE INTEGERS). AN EXCEPTION TO THIS RULE   19740000
C        IS THE EXPLICIT DECLARATION OF CHARACTER VARIABLES.            19750000
C                                                                       19760000
C  ARGUMENT  USE   TYPE  DESCRIPTION                                    19780000
C  --------  ---   ----  -----------                                    19790000
C  ARAYLN           C*8  FOWARD MODELING MATRIX                         19800000
C  FSLICE     IN    C*8  ARRAY CONTAINING INPUT FREQUENCY SLICE (DATA)  19810000
C  FPSPAC           C*8  ARRAY CONTAINING MODELING COEFFICIENTS         19820000
C  NPVCUT     IN    R*4  P VALUE CUTOFF                                 19830000
C  NPVSMP     IN    R*4  NUMBER OF P VALUES TO MODEL                    19840000
C  NCHANS     IN    R*4  NUMBER OF INPUT TRACES (DIMENSION OF FSLICE)   19850000
C                                                                       19860000
                                                                        19880000
      SUBROUTINE BLDMD ( ARAYLN   ,FSLICE   ,FPSPAC                     19890000
     *                   ,NPVCUT   ,NPVSMP   ,NCHANS                    19900000
     *                  )                                               19910000
                                                                        19920000
      COMPLEX    ARAYLN(NCHANS,NPVSMP)  ,FPSPAC(NPVSMP)                 19930000
      COMPLEX    FSLICE(NCHANS)                                         19940000
      COMPLEX    ARG                                                    19950000
                                                                        19960000
C MODEL THE MULTIPLES ONE FREQUENCY AT A TIME                           19970000
                                                                        19980000
      DO 30 ICHN=1,NCHANS                                               19990000
         ARG  = 0.                                                      20010000
         DO 20 IPVL=NPVCUT+1,NPVSMP                                     20030000
            ARG = ARG + ARAYLN(ICHN,IPVL) * FPSPAC(IPVL)                20050000
20       CONTINUE                                                       20070000
         FSLICE(ICHN) = ARG                                             20090000
30    CONTINUE                                                          20110000
                                                                        20120000
      RETURN                                                            20130000
      END                                                               20140000
                                                                        20150000
C DATE:      AUGUST 10,  1993 
C TYPE SUBROUTINE
C LANGUAGE VS FORTRAN
CC
C NAME CGEMUL
cc
C PURPOSE: one matrix multiplied by the Hermitian of another matrix

C CALL CGEMUL(A,B,C,M,N,L)
CC
C ARGUMENT	USE	TYPE	DESCRIPTION
C A		IN	C*8	MATRIX 1
C B		IN	C*8	MATRIX 2
c C		OUT	C*8	MATRIX 3
C M		IN 	R*4	ROW NUMBER OF A and B
C N		IN	R*4	COLUMN NUMBER OF A
C L		IN	R*4	COLUMN NUMBER OF B
cc

	subroutine cgemul(a,b,c,m,n,l)

	complex a(m,n), b(m,l), c(n,l)
	integer i, j, k
	real ar, ai, br, bi, cr, ci, zero
	data zero/0.0/

	do 10 i=1,n
	do 10 j=1,l
		c(i,j) = cmplx(zero,zero)
		do 20 k=1,m
			ar = real(a(k,i))
			ai = imag(a(k,i))
			br = real(b(k,j))
			bi = imag(b(k,j))
			cr = ar*br+ai*bi
			ci = ar*bi-ai*br
			c(i,j) = c(i,j)+cmplx(cr,ci)
20		continue
10	continue

	return
	end
