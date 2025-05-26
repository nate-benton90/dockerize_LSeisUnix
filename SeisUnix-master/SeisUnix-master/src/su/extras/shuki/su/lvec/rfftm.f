       SUBROUTINE RFFTM(A,C,TRIG,IFAX,INC1,INC2,NFFT,NUM,ISIGN)
************************************************************
C       MIXED RADIX REAL FFT ROUTINE BASED ON TEMPERTON.
C
C  (R*4)  A      - first real element in data array
C                  (input and output)
C  (R*4)  C      - work array (nfft)
C  (R*4)  TRIG   - array containing trigonometric functions (2*nfft)
C  (I*4)  IFAX   - array containing FFT factors
C  (I*4)  INC1   - stride within each transform 
C  (I*4)  INC2   - stride between transforms 
C  (I*4)  NFFT   - fft length
C  (I*4)  NUM    - number of fft's to be performed
C  (I*4)  ISIGN  - FFT sign  (-1 = real to complex, 
C                           +1 = complex to real )
C
C         * results are notmalized when isign=+1
C
C         * in-place algorithm (imaginary DC and imaginary NYQUIST
C           are ignored).
************************************************************
       DIMENSION A(1),C(1),TRIG(1),IFAX(1)
       DO 100 NN=1,NUM
       IP=(NN-1)*INC2+1
       CALL RFFT(A(IP),C,NFFT,TRIG,IFAX(2),IFAX(1),INC1,ISIGN)
100    CONTINUE
C----- for 'standard' ordering ....
C      IP=(NN-1)*INC2+1
C      IE2=N*INC1+1
C      IE1=IP+2*INC1
C      CALL RFFT(VIN(IP),WORK,N,TRIGS,IFF(2),IFF(1),INC1,ISIGN)
C      DO 110 I=IE2,IE1,-INC1
C10    VIN(I)=VIN(I-INC1)        ! REARRANGE DATA TO FFT991 ORDER
C      VIN(IE2+INC1)=0.          ! IMAGINARY NYQUIST
C      VIN(IE1-INC1)=0.          ! IMAGINARY DC
       RETURN
       END
C---------------------------------------------------------------------
        SUBROUTINE RFFT(A,C,N,TRIGS,IFAX,NFAC,ISKIP,ISIGN)
        DIMENSION TRIGS(1)
        INTEGER IFAX(1)
        DIMENSION A(1),C(1)
        IF(ISIGN.GT.0) GO TO 100
CCCC
C    ------ REAL TO COMPLEX -------
CCCC
        MA=N
        LA=1
         II=1
        NF=NFAC+1
        DO 102 I=1,NFAC
         IFAC=IFAX(NF-I)
         MA=MA/IFAC
         MASKIP=MA*ISKIP
         IF(II.GT.0) CALL RPASS(A,A(1+MASKIP*IFAC),C,C(1+MA),
     .       TRIGS,TRIGS(2),IFAC,MA,LA,ISKIP,1,N)
         IF(II.LT.0) CALL RPASS(C,C(1+MA*IFAC),A,A(1+MASKIP),
     .       TRIGS,TRIGS(2),IFAC,MA,LA,1,ISKIP,N)
         LA=LA*IFAC
         II=-II
102     CONTINUE
        IF(II.LT.0) THEN
         JJ=0
         DO 101 J=1,N*ISKIP,ISKIP
          JJ=JJ+1
          A(J)=C(JJ)
101      CONTINUE
        END IF
        RETURN
100        CONTINUE
CCCC
C       ------ COMPLEX TO REAL ---------
CCCC
        LA=1
         II=1
C        .. MULTIPLY DC (AND NYQUIST) BY 0.5 ACCORDING TO TEMPERTON. ..
        A(1)=0.5*A(1)
        N1=(N-1)*ISKIP+1
        IF(MOD(N,2).EQ.0)A(N1)=0.5*A(N1)
        CON=2./FLOAT(N)
        MA=N
        DO 103 I=1,NFAC
         IFAC=IFAX(I)
         MA=MA/IFAC
         LASKIP=LA*ISKIP
         IF(II.GT.0) CALL RPASSM(A,A(1+LASKIP),C,C(1+LA*IFAC),
     .        TRIGS,TRIGS(2),IFAC,LA,MA,ISKIP,1,N)
         IF(II.LT.0) CALL RPASSM(C,C(1+LA),A,A(1+LASKIP*IFAC),
     .        TRIGS,TRIGS(2),IFAC,LA,MA,1,ISKIP,N)
         LA=LA*IFAC
         II=-II
103     CONTINUE
        IF(II.LT.0) THEN
         JJ=0
         DO 104 J=1,N*ISKIP,ISKIP
          JJ=JJ+1
          A(J)=C(JJ)*CON
104      CONTINUE
        ELSE
        DO 105 J=1,N*ISKIP,ISKIP
         A(J)=A(J)*CON
105     CONTINUE
        END IF
        RETURN
        END
C-----------------------------------------------------------------
C
C       PERFORM COMPLEX TO REAL RFFT PASS BASED ON DECIMATION IN
C        FREQUENCY
C
C-----------------------------------------------------------------
        SUBROUTINE RPASSM(A,B,C,D,TRIGS,TRIGSI,IFAC,LA,MA,
     .      ISKIP,ISKIP1,N)
        DIMENSION A(N),C(N),TRIGS(1),TRIGSI(1),B(1),D(1)
        SAVE SIN45,SIN60,SIN72,SIN36,SQ54,SIN3672
        DATA IFLAG /0/
        IF(IFLAG.EQ.0) THEN
         IFLAG=1
         SIN45=0.5*SQRT(2.)
         SIN60=SIN(3.14159265/3.)
         SIN72=SIN(3.14159265*72./180.)
         SIN36=SIN(3.14159265*36./180.)
         SQ54=0.25*SQRT(5.)
         SIN3672=SIN36/SIN72
        END IF
        M=N/IFAC
        M2=M+M
        MSKIP=M*ISKIP
        M2SKIP=MSKIP+MSKIP
        MHLF=M/2
        LASKIP=LA*ISKIP
        LASKIP1=LA*ISKIP1
        IB=LASKIP1
        IB2=IB+IB
        IB3=IB2+IB
        JUMP=(IFAC-1)*LASKIP1
        JUMP3=JUMP*3
        IJ=(N-LA)*ISKIP
        GO TO (100,200,300,400,500,600),IFAC
*100        PRINT *,'**** RADIX ONE ENCOUNTERED ****'
*       STOP 999
100     continue
200        CONTINUE
C       ------- FACTOR TWO ------------
C         CALCULATE DC
         L1=1
         DO 10 L=1,LASKIP,ISKIP
          Y0=A(L)
          Y1=A(L+IJ)
          C(L1)=Y0+Y1
          C(L1+IB)=Y0-Y1
          L1=L1+ISKIP1
10       CONTINUE
         IF(LA.EQ.M) RETURN
         I=LASKIP+1
         J=LASKIP1+1+JUMP
         MM=MHLF
         IF(MOD(MA,2).EQ.0) MM=MM-LA
        DO 11 K=LA,MM,LA
         KK=K+K
         JN=IJ-KK*ISKIP
         KK=KK+1
         TREAL=TRIGS(KK)
         TIMAG=TRIGSI(KK)
         DO 12 L=1,LASKIP,ISKIP
          A0REAL=A(I)
          LJN=L+JN
          ALJNREAL=A(LJN)
          A0IMAG=B(I)
          ALJNIMAG=B(LJN)
          C(J)=A0REAL+ALJNREAL
          D(J)=A0IMAG-ALJNIMAG
          JIB=J+IB
          C(JIB)=TREAL*(A0REAL-ALJNREAL)-TIMAG*(A0IMAG+
     .        ALJNIMAG)
          D(JIB)=TIMAG*(A0REAL-ALJNREAL)+
     .       TREAL*(A0IMAG+ ALJNIMAG)
          I=I+ISKIP
          J=J+ISKIP1
12       CONTINUE
         J=J+JUMP3
         I=I+LASKIP
11      CONTINUE
        DO 13 L=1,LASKIP,ISKIP
         C(J)=A(I)
         C(J+IB)=-B(I)
         I=I+ISKIP
         J=J+ISKIP1
13      CONTINUE
        RETURN
C        --------- FACTOR THREE ------------
300        CONTINUE
        LA3SKIP=LASKIP1*3
        KEND=(N-LA)/2+LA
        IF(MOD(MA,2).EQ.0) KEND=(N-2*LA)/2+LA+LA
        KDIF=KEND+KEND-M2-LA
        IF(MOD(MA,2).EQ.0) KDIF=KDIF-LA
        JUMPLA3=JUMP+LA3SKIP
        IJ2=M2SKIP
        IJ1=IJ2-LASKIP
        IC=IB+LASKIP1
C         CALCULATE DC
        L1=1
        DO 14 L=1,LASKIP,ISKIP
         A0=A(L)
         A1=A(L+IJ1)
         B1=A(L+IJ2)
         T2=A0-0.5*A1
         T3=SIN60*B1
         C(L1)=A0+A1
         C(L1+IB)=T2-T3
         C(L1+IC)=T2+T3
         L1=L1+ISKIP1
14      CONTINUE
        IF(LA.EQ.M) RETURN
        I=LASKIP+1
        J=LASKIP1+1+JUMP
         MM=MHLF
         IF(MOD(MA,2).EQ.0) MM=MM-LA
        DO 15 K=LA,MM,LA
          K2=K+K
          KJ=KDIF-K2
          KJ=(KJ+KJ)*ISKIP
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=TRIGSI(KK)
          DO 16 L=1,LASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IM2SKIP=I+M2SKIP
           A1REAL=A(IM2SKIP)
           A1IMAG=B(IM2SKIP)
           JNL=KJ+I
           A2REAL=A(JNL)
           A2IMAG=-B(JNL)
           T1REAL=A1REAL+A2REAL
           T1IMAG=A1IMAG+A2IMAG
           T2REAL=A0REAL-0.5*T1REAL
           T2IMAG=A0IMAG-0.5*T1IMAG
           T3REAL=SIN60*(A1REAL-A2REAL)
           T3IMAG=SIN60*(A1IMAG-A2IMAG)
           C(J)=A0REAL+T1REAL
           D(J)=A0IMAG+T1IMAG
           X1REAL=T2REAL-T3IMAG
           X1IMAG=T2IMAG+T3REAL
           X2REAL=T2REAL+T3IMAG
           X2IMAG=T2IMAG-T3REAL
           JIB=J+IB
           C(JIB)=TR1REAL*X1REAL-TR1IMAG*X1IMAG
           D(JIB)=TR1IMAG*X1REAL+TR1REAL*X1IMAG
           JIC=J+IC
           C(JIC)=TR2REAL*X2REAL-TR2IMAG*X2IMAG
           D(JIC)=TR2IMAG*X2REAL+TR2REAL*X2IMAG
           I=I+ISKIP
           J=J+ISKIP1
16        CONTINUE
          J=J+JUMPLA3
          I=I+LASKIP
15       CONTINUE
         IF(MOD(MA,2).NE.0)RETURN
         DO 17 L=1,LASKIP,ISKIP
          A0=A(I)
          B0=A(I+LASKIP)
          IM2SKIP=I+M2SKIP
          A1=A(IM2SKIP)
          T1=0.5*A0-A1
          C(J)=A0+A1
          C(J+IB)=T1-SIN60*B0
          C(J+IC)=-T1-SIN60*B0
          I=I+ISKIP
          J=J+ISKIP1
17       CONTINUE
        RETURN
C       ------ FACTOR 4 -----------
400        CONTINUE
        LA4SKIP=LASKIP1*4
        JUMPLA4=JUMP+LA4SKIP
        IJ2=M2SKIP
        IJ1=IJ2-LASKIP
        IC=IB+LASKIP1
        ID=IC+LASKIP1
        L1=1
        DO 20 L=1,LASKIP,ISKIP
         Y0=A(L)
         Y1=A(L+IJ1)
         Y3=A(L+IJ2)
         Y2=A(L+IJ)
         T1=Y0+Y2
         T3=Y0-Y2
         C(L1)=T1+Y1
         C(L1+IB)=T3-Y3
         C(L1+IC)=T1-Y1
         C(L1+ID)=T3+Y3
         L1=L1+ISKIP1
20      CONTINUE
        IF(LA.EQ.M) RETURN
        I=LASKIP+1
        J=LASKIP1+1+JUMP
         MM=MHLF
         IF(MOD(MA,2).EQ.0) MM=MM-LA
        DO 21 K=LA,MM,LA
          K2=K+K
         JN=IJ-K2*ISKIP
         JM1=JN-M2SKIP
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=TRIGSI(KK)
          DO 22 L=1,LASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IM2SKIP=I+M2SKIP
           A1REAL=A(IM2SKIP)
           A1IMAG=B(IM2SKIP)
           JNL=JN+L
           A2REAL=A(JNL)
           A2IMAG=-B(JNL)
           JM1L=JM1+L
           A3REAL=A(JM1L)
           A3IMAG=-B(JM1L)
           T1REAL=A0REAL+A2REAL
           T1IMAG=A0IMAG+A2IMAG
           T2REAL=A1REAL+A3REAL
           T2IMAG=A1IMAG+A3IMAG
           T3REAL=A0REAL-A2REAL
           T3IMAG=A0IMAG-A2IMAG
           T4REAL=A1REAL-A3REAL
           T4IMAG=A1IMAG-A3IMAG
           C(J)=T1REAL+T2REAL
           D(J  )=T1IMAG+T2IMAG
           X1REAL=T3REAL-T4IMAG
           X1IMAG=T3IMAG+T4REAL
           X2REAL=T1REAL-T2REAL
           X2IMAG=T1IMAG-T2IMAG
           X3REAL=T3REAL+T4IMAG
           X3IMAG=T3IMAG-T4REAL
           JIB=J+IB
           C(JIB)=TR1REAL*X1REAL-TR1IMAG*X1IMAG
           D(JIB)=TR1IMAG*X1REAL+TR1REAL*X1IMAG
           JIC=J+IC
           C(JIC)=TR2REAL*X2REAL-TR2IMAG*X2IMAG
           D(JIC)=TR2IMAG*X2REAL+TR2REAL*X2IMAG
           JID=J+ID
           C(JID)=TR3REAL*X3REAL-TR3IMAG*X3IMAG
           D(JID)=TR3IMAG*X3REAL+TR3REAL*X3IMAG
           I=I+ISKIP
           J=J+ISKIP1
22        CONTINUE
          J=J+JUMPLA4
          I=I+LASKIP
21       CONTINUE
         DO 23 L=1,LASKIP,ISKIP
          A0=A(I)
          B0=B(I)
          IM2SKIP=I+M2SKIP
          A1=A(IM2SKIP)
          B1=B(IM2SKIP)
          T1=SIN45*(B0+B1)
          T2=SIN45*(A0-A1)
          C(J)=A0+A1
          C(J+IB)=T2-T1
          C(J+IC)=B1-B0
          C(J+ID)=-(T2+T1)
          I=I+ISKIP
          J=J+ISKIP1
23       CONTINUE
        RETURN
C        ------- FACTOR 5 ---------
500        CONTINUE
        LA5SKIP=LASKIP1*5
        KEND=(N-LA)/2+LA
        IF(MOD(MA,2).EQ.0) KEND=(N-2*LA)/2+LA+LA
        KDIF=KEND+KEND-M2-LA-M
        IF(MOD(MA,2).EQ.0) KDIF=KDIF-LA
        JUMPLA5=JUMP+LA5SKIP
        IJ2=M2SKIP
        IJ1=IJ2-LASKIP
        IJ4=M2SKIP+M2SKIP
        IJ3=IJ4-LASKIP
        IC=IB+LASKIP1
        ID=IC+LASKIP1
        IE=ID+LASKIP1
C         CALCULATE DC
        L1=1
        DO 30 L=1,LASKIP,ISKIP
         A0=A(L)
         A1=A(L+IJ1)
         B1=A(L+IJ2)
         A2=A(L+IJ3)
         B2=A(L+IJ4)
         T3=SIN72*B1
         T4=SIN72*B2
         T5=A1+A2
         T6=SQ54*(A1-A2)
         T7=A0-0.25*T5
         T8=T7+T6
         T9=T7-T6
         T10=T3+SIN3672*T4
         T11=SIN3672*T3-T4
         C(L1)=A0+T5
         C(L1+IB)=T8-T10
         C(L1+IC)=T9-T11
         C(L1+ID)=T9+T11
         C(L1+IE)=T8+T10
         L1=L1+ISKIP1
30      CONTINUE
        IF(LA.EQ.M) RETURN
        I=LASKIP+1
        J=LASKIP1+1+JUMP
         MM=MHLF
         IF(MOD(MA,2).EQ.0) MM=MM-LA
        DO 31 K=LA,MM,LA
          K2=K+K
          KJ=KDIF-K2
          KJ=(KJ+KJ)*ISKIP
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=TRIGSI(KK)
          KK=KK+K2
          TR4REAL=TRIGS(KK)
          TR4IMAG=TRIGSI(KK)
          DO 32 L=1,LASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IM2SKIP=I+M2SKIP
           A1REAL=A(IM2SKIP)
           A1IMAG=B(IM2SKIP)
           IM4=IM2SKIP+M2SKIP
           A2REAL=A(IM4)
           A2IMAG=B(IM4)
           JNL=KJ+I
           A3REAL=A(JNL)
           A3IMAG=-B(JNL)
           JNLM2SKIP=JNL-M2SKIP
           A4REAL=A(JNLM2SKIP)
           A4IMAG=-B(JNLM2SKIP)
           T1REAL=A1REAL+A4REAL
           T1IMAG=A1IMAG+A4IMAG
           T2REAL=A2REAL+A3REAL
           T2IMAG=A2IMAG+A3IMAG
           T3REAL=SIN72*(A1REAL-A4REAL)
           T3IMAG=SIN72*(A1IMAG-A4IMAG)
           T4REAL=SIN72*(A2REAL-A3REAL)
           T4IMAG=SIN72*(A2IMAG-A3IMAG)
           T5REAL=T1REAL+T2REAL
           T5IMAG=T1IMAG+T2IMAG
           T6REAL=SQ54*(T1REAL-T2REAL)
           T6IMAG=SQ54*(T1IMAG-T2IMAG)
           T7REAL=A0REAL-T5REAL*0.25
           T7IMAG=A0IMAG-T5IMAG*0.25
           T8REAL=T7REAL+T6REAL
           T8IMAG=T7IMAG+T6IMAG
           T9REAL=T7REAL-T6REAL
           T9IMAG=T7IMAG-T6IMAG
           T10REAL=T3REAL+SIN3672*T4REAL
           T10IMAG=T3IMAG+SIN3672*T4IMAG
           T11REAL=SIN3672*T3REAL-T4REAL
           T11IMAG=SIN3672*T3IMAG-T4IMAG
           C(J)=A0REAL+T5REAL
           D(J)=A0IMAG+T5IMAG
           X1REAL=T8REAL-T10IMAG
           X1IMAG=T8IMAG+T10REAL
           X2REAL=T9REAL-T11IMAG
           X2IMAG=T9IMAG+T11REAL
           X3REAL=T9REAL+T11IMAG
           X3IMAG=T9IMAG-T11REAL
           X4REAL=T8REAL+T10IMAG
           X4IMAG=T8IMAG-T10REAL
           JIB=J+IB
           C(JIB)=TR1REAL*X1REAL-TR1IMAG*X1IMAG
           D(JIB)=TR1IMAG*X1REAL+TR1REAL*X1IMAG
           JIC=J+IC
           C(JIC)=TR2REAL*X2REAL-TR2IMAG*X2IMAG
           D(JIC)=TR2IMAG*X2REAL+TR2REAL*X2IMAG
           JID=J+ID
           C(JID)=TR3REAL*X3REAL-TR3IMAG*X3IMAG
           D(JID)=TR3IMAG*X3REAL+TR3REAL*X3IMAG
           JIE=J+IE
           C(JIE)=TR4REAL*X4REAL-TR4IMAG*X4IMAG
           D(JIE)=TR4IMAG*X4REAL+TR4REAL*X4IMAG
           I=I+ISKIP
           J=J+ISKIP1
32        CONTINUE
          J=J+JUMPLA5
          I=I+LASKIP
31       CONTINUE
         IF(MOD(MA,2).NE.0)RETURN
         DO 33 L=1,LASKIP,ISKIP
          A0=A(I)
          B0=B(I)
          IM2SKIP=I+M2SKIP
          A1=A(IM2SKIP)
          B1=B(IM2SKIP)
          IM4=IM2SKIP+M2SKIP
          A2=A(IM4)
          T1=A0+A1
          T2=0.25*T1-A2
          T3=SQ54*(A0-A1)
          T4=SIN36*B0+SIN72*B1
          T5=SIN72*B0-SIN36*B1
          T6=T3+T2
          T7=T3-T2
          C(J)=T1+A2
          C(J+IB)=T6-T4
          C(J+IC)=T7-T5
          C(J+ID)=-T7-T5
          C(J+IE)=-T6-T4
          I=I+ISKIP
          J=J+ISKIP1
33       CONTINUE
        RETURN
C        ----- FACTOR 6 -----------------
600        CONTINUE
        LA6SKIP=LASKIP1*6
        JUMPLA6=JUMP+LA6SKIP
        IJ2=M2SKIP
        IJ1=IJ2-LASKIP
        IC=IB+LASKIP1
        ID=IC+LASKIP1
        IE=ID+LASKIP1
        IG=IE+LASKIP1
        L1=1
         DO 41 L=1,LASKIP,ISKIP
          A0=A(L)
          LIJ1=L+IJ1
          A1=A(LIJ1)
          B1=B(LIJ1)
          LIJ3=LIJ1+M2SKIP
          A2=A(LIJ3)
          B2=B(LIJ3)
          A3=A(L+IJ)
           T2REAL=A0-A2*0.5
           T3IMAG=SIN60*B2
           Y0REAL=A0+A2
           Y4REAL=T2REAL-T3IMAG
           Y2REAL=T2REAL+T3IMAG
           T2REAL=A3-A1*0.5
           T3IMAG=-SIN60*B1
           Y3REAL=A3+A1
           Y1REAL=T2REAL-T3IMAG
           Y5REAL=T2REAL+T3IMAG
           X0REAL=Y0REAL+Y3REAL
           X1REAL=Y4REAL-Y1REAL
           X2REAL=Y2REAL+Y5REAL
           X3REAL=Y0REAL-Y3REAL
           X4REAL=Y4REAL+Y1REAL
           X5REAL=Y2REAL-Y5REAL
           C(L1   )=X0REAL
           C(L1+IB)=X1REAL
           C(L1+IC)=X2REAL
           C(L1+ID)=X3REAL
           C(L1+IE)=X4REAL
           C(L1+IG)=X5REAL
           L1=L1+ISKIP1
41       CONTINUE
        IF(LA.EQ.M) RETURN
        I=LASKIP+1
        J=LASKIP1+1+JUMP
         MM=MHLF
         IF(MOD(MA,2).EQ.0) MM=MM-LA
        DO 42 K=LA,MM,LA
          K2=K+K
         JN=IJ-K2*ISKIP
         JM1=JN-M2SKIP
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=TRIGSI(KK)
          KK=KK+K2
          TR4REAL=TRIGS(KK)
          TR4IMAG=TRIGSI(KK)
          KK=KK+K2
          TR5REAL=TRIGS(KK)
          TR5IMAG=TRIGSI(KK)
          DO 43 L=1,LASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IM2SKIP=I+M2SKIP
           A1REAL=A(IM2SKIP)
           A1IMAG=B(IM2SKIP)
           IM4=IM2SKIP+M2SKIP
           A2REAL=A(IM4)
           A2IMAG=B(IM4)
           JNL=JN+L
           A3REAL=A(JNL)
           A3IMAG=-B(JNL)
           JNLM2SKIP=JNL-M2SKIP
           A4REAL=A(JNLM2SKIP)
           A4IMAG=-B(JNLM2SKIP)
           JNLM4=JNLM2SKIP-M2SKIP
           A5REAL=A(JNLM4)
           A5IMAG=-B(JNLM4)
           T1REAL=A2REAL+A4REAL
           T1IMAG=A2IMAG+A4IMAG
           T2REAL=A0REAL-0.5*T1REAL
           T2IMAG=A0IMAG-0.5*T1IMAG
           T3REAL=SIN60*(A2REAL-A4REAL)
           T3IMAG=SIN60*(A2IMAG-A4IMAG)
           Y0REAL=A0REAL+T1REAL
           Y0IMAG=A0IMAG+T1IMAG
           Y4REAL=T2REAL-T3IMAG
           Y4IMAG=T2IMAG+T3REAL
           Y2REAL=T2REAL+T3IMAG
           Y2IMAG=T2IMAG-T3REAL
           T1REAL=A5REAL+A1REAL
           T1IMAG=A5IMAG+A1IMAG
           T2REAL=A3REAL-0.5*T1REAL
           T2IMAG=A3IMAG-0.5*T1IMAG
           T3REAL=SIN60*(A5REAL-A1REAL)
           T3IMAG=SIN60*(A5IMAG-A1IMAG)
           Y3REAL=A3REAL+T1REAL
           Y3IMAG=A3IMAG+T1IMAG
           Y1REAL=T2REAL-T3IMAG
           Y1IMAG=T2IMAG+T3REAL
           Y5REAL=T2REAL+T3IMAG
           Y5IMAG=T2IMAG-T3REAL
           X0REAL=Y0REAL+Y3REAL
           X0IMAG=Y0IMAG+Y3IMAG
           X4REAL=Y4REAL+Y1REAL
           X4IMAG=Y4IMAG+Y1IMAG
           X2REAL=Y2REAL+Y5REAL
           X2IMAG=Y2IMAG+Y5IMAG
           X3REAL=Y0REAL-Y3REAL
           X3IMAG=Y0IMAG-Y3IMAG
           X1REAL=Y4REAL-Y1REAL
           X1IMAG=Y4IMAG-Y1IMAG
           X5REAL=Y2REAL-Y5REAL
           X5IMAG=Y2IMAG-Y5IMAG
           C(J    )=X0REAL
           D(J      )=X0IMAG
           JIB=J+IB
           C(JIB)=TR1REAL*X1REAL-TR1IMAG*X1IMAG
           D(JIB)=TR1IMAG*X1REAL+TR1REAL*X1IMAG
           JIC=J+IC
           C(JIC)=TR2REAL*X2REAL-TR2IMAG*X2IMAG
           D(JIC)=TR2IMAG*X2REAL+TR2REAL*X2IMAG
           JID=J+ID
           C(JID)=TR3REAL*X3REAL-TR3IMAG*X3IMAG
           D(JID)=TR3IMAG*X3REAL+TR3REAL*X3IMAG
           JIE=J+IE
           C(JIE)=TR4REAL*X4REAL-TR4IMAG*X4IMAG
           D(JIE)=TR4IMAG*X4REAL+TR4REAL*X4IMAG
           JIG=J+IG
           C(JIG)=TR5REAL*X5REAL-TR5IMAG*X5IMAG
           D(JIG)=TR5IMAG*X5REAL+TR5REAL*X5IMAG
           I=I+ISKIP
           J=J+ISKIP1
43        CONTINUE
          J=J+JUMPLA6
          I=I+LASKIP
42       CONTINUE
         IF(MOD(MA,2).NE.0) RETURN
         DO 44 L=1,LASKIP,ISKIP
          A0=A(I)
          B0=B(I)
          IM2SKIP=I+M2SKIP
          A1=A(IM2SKIP)
          B1=B(IM2SKIP)
          IM4=IM2SKIP+M2SKIP
          A2=A(IM4)
          B2=B(IM4)
          T1=A0+A2
          T2=B0+B2
          T3=SIN60*(A0-A2)
          T4=SIN60*(B0-B2)
          T5=0.5*T1-A1
          T6=0.5*T2+B1
          C(J)=A1+T1
          C(J+IB)=T3-T6
          C(J+IC)=T5-T4
          C(J+ID)=B1-T2
          C(J+IE)=-(T4+T5)
          C(J+IG)=-(T3+T6)
          I=I+ISKIP
          J=J+ISKIP1
44       CONTINUE
         RETURN
        END
C----------------------------------------------------------
C
C       PERFORM REAL TO COMPLEX RFFT PASS
C
C----------------------------------------------------------
        SUBROUTINE RPASS(A,B,C,D,TRIGS,TRIGSI,IFAC,MA,LA,
     .    ISKIP,ISKIP1,N)
        DIMENSION A(N),C(N),TRIGS(1),TRIGSI(1),B(1),D(1)
        SAVE SIN45,SIN60,SIN72,SIN36,SQ54,SIN3672
        DATA IFLAG /0/
        IF(IFLAG.EQ.0) THEN
         IFLAG=1
         SIN45=0.5*SQRT(2.)
         SIN60=SIN(3.14159265/3.)
         SIN72=SIN(3.14159265*72./180.)
         SIN36=SIN(3.14159265*36./180.)
         SQ54=0.25*SQRT(5.)
         SIN3672=SIN36/SIN72
        END IF
        M=N/IFAC
        M2=M+M
        MSKIP=M*ISKIP
        M2SKIP=MSKIP+MSKIP
        MSKIP1=M*ISKIP1
        M2SKIP1=MSKIP1+MSKIP1
        MHLF=M/2
        MASKIP=MA*ISKIP
        MASKIP1=MA*ISKIP1
        IB=MASKIP
        JUMP=(IFAC-1)*MASKIP
        JUMP3=JUMP*3
        IJ=(N-MA)*ISKIP1
        GO TO (100,200,300,400,500 ,600),IFAC
*100        PRINT *,'**** RADIX ONE ENCOUNTERED ****'
*       STOP 999
100     continue
200        CONTINUE
C       -------- FACTOR TWO ----------
         L1=1
         DO 10 L=1,MASKIP,ISKIP
          A0REAL=A(L)
          A1REAL=A(L+IB)
          C(L1)=A0REAL+A1REAL
          C(L1+IJ)=A0REAL-A1REAL
          L1=L1+ISKIP1
10       CONTINUE
         J=MASKIP1+1
         I=MASKIP+1+JUMP
         IF(MA.EQ.M) RETURN
         MM=MHLF
         IF(MOD(LA,2).EQ.0) MM=MM-MA
        DO 11 K=MA,MM,MA
         KK=K+K
         JN=IJ-KK*ISKIP1
         KK=KK+1
         TREAL=TRIGS(KK)
         TIMAG=TRIGSI(KK)
         L1=1
         DO 12 L=1,MASKIP,ISKIP
          A0REAL=A(I)
          A0IMAG=B(I)
          IIB=I+IB
          A1REAL=A(IIB)
          A1IMAG=B(IIB)
          C(J)=A0REAL+A1REAL*TREAL+A1IMAG*TIMAG
          D(J)=A0IMAG-A1REAL*TIMAG+A1IMAG*TREAL
          JNL=JN+L1
          C(JNL)=A0REAL-A1REAL*TREAL-A1IMAG*TIMAG
          D(JNL)=-A0IMAG-A1REAL*TIMAG+
     .      A1IMAG*TREAL
          L1=L1+ISKIP1
          I=I+ISKIP
          J=J+ISKIP1
12       CONTINUE
         I=I+JUMP3
         J=J+MASKIP1
11      CONTINUE
        IF(MOD(LA,2).NE.0) RETURN
        DO 13 L=1,MASKIP,ISKIP
         C(J)=A(I)
         D(J)=-A(I+IB)
         I=I+ISKIP
         J=J+ISKIP1
13      CONTINUE
        RETURN
C       ------ FACTOR 3 -----------
300        CONTINUE
        MA3SKIP=MASKIP*3
        KEND=(N-MA)/2+MA
        IF(MOD(LA,2).EQ.0) KEND=(N-2*MA)/2+MA*2
        KDIF=KEND+KEND-M2-MA
        IF(MOD(LA,2).EQ.0) KDIF=KDIF-MA
        JUMPMA3=JUMP+MA3SKIP
        IJ2=M2SKIP1
        IJ1=IJ2-MASKIP1
        IC=IB+MASKIP
        L1=1
         DO 20 L=1,MASKIP,ISKIP
          X0=A(L)
          X1=A(L+IB)
          X2=A(L+IC)
          T1=X1+X2
          T2=X0-0.5*T1
          T3=SIN60*(X1-X2)
          C(L1)=X0+T1
          C(L1+IJ1)=T2
          C(L1+IJ2)=-T3
          L1=L1+ISKIP1
20       CONTINUE
         IF(MA.EQ.M) RETURN
         J=MASKIP1+1
         I=MASKIP+1+JUMP
         MM=MHLF
         IF(MOD(LA,2).EQ.0) MM=MM-MA
        DO 21 K=MA,MM,MA
          K2=K+K
          KJ=KDIF-K2
          KJ=(KJ+KJ)*ISKIP1
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=-TRIGSI(KK)
          DO 22 L=1,MASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IIB=I+IB
           A1REAL=A(IIB)*TR1REAL-B(IIB)*TR1IMAG
           A1IMAG=B(IIB)*TR1REAL+A(IIB)*TR1IMAG
           IIC=I+IC
           A2REAL=A(IIC)*TR2REAL-B(IIC)*TR2IMAG
           A2IMAG=B(IIC)*TR2REAL+A(IIC)*TR2IMAG
           T1REAL=A1REAL+A2REAL
           T1IMAG=A1IMAG+A2IMAG
           T2REAL=A0REAL-T1REAL*0.5
           T2IMAG=A0IMAG-T1IMAG*0.5
           T3REAL=SIN60*(A1REAL-A2REAL)
           T3IMAG=SIN60*(A1IMAG-A2IMAG)
           C(J)=A0REAL+T1REAL
           D(J  )=A0IMAG+T1IMAG
           JM2SKIP=J+M2SKIP1
           C(JM2SKIP)=T2REAL+T3IMAG
           D(JM2SKIP)=T2IMAG-T3REAL
           JNL=KJ+J
           C(JNL   )=T2REAL-T3IMAG
           D(JNL)=-(T2IMAG+T3REAL)
           I=I+ISKIP
           J=J+ISKIP1
22        CONTINUE
          I=I+JUMPMA3
          J=J+MASKIP1
21       CONTINUE
         IF(MOD(LA,2).NE.0) RETURN
         DO 23 L=1,MASKIP,ISKIP
          X0=A(I)
          X1=A(I+IB)
          X2=A(I+IC)
          T1=X1-X2
          C(J)=X0+T1*0.5
          C(J+MASKIP1)=-SIN60*(X1+X2)
          JM2SKIP=J+M2SKIP
          C(JM2SKIP)=X0-T1
          I=I+ISKIP
          J=J+ISKIP1
23       CONTINUE
         RETURN
C       ------ FACTOR 4 -----------
400        CONTINUE
        MA4SKIP=MASKIP*4
        JUMPMA4=JUMP+MA4SKIP
        IJ2=M2SKIP1
        IJ1=IJ2-MASKIP1
        IC=IB+MASKIP
        ID=IC+MASKIP
        L1=1
         DO 30 L=1,MASKIP,ISKIP
          A0REAL=A(L)
          A1REAL=A(L+IB)
          A2REAL=A(L+IC)
          A3REAL=A(L+ID)
          T1=A0REAL+A2REAL
          T2=A1REAL+A3REAL
          T3=A0REAL-A2REAL
          T4=A3REAL-A1REAL
          C(L1)=T1+T2
          C(L1+IJ1)=T3
          C(L1+IJ2)=T4
          C(L1+IJ)=T1-T2
          L1=L1+ISKIP1
30       CONTINUE
         IF(MA.EQ.M) RETURN
         J=MASKIP1+1
         I=MASKIP+1+JUMP
         MM=MHLF
         IF(MOD(LA,2).EQ.0) MM=MM-MA
        DO 31 K=MA,MM,MA
          K2=K+K
         JN=IJ-K2 *ISKIP1
         JM1=JN-M2SKIP1
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=-TRIGSI(KK)
          L1=1
          DO 32 L=1,MASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IIB=I+IB
           A1REAL=A(IIB)*TR1REAL-B(IIB)*TR1IMAG
           A1IMAG=B(IIB)*TR1REAL+A(IIB)*TR1IMAG
           IIC=I+IC
           A2REAL=A(IIC)*TR2REAL-B(IIC)*TR2IMAG
           A2IMAG=B(IIC)*TR2REAL+A(IIC)*TR2IMAG
           IID=I+ID
           A3REAL=A(IID)*TR3REAL-B(IID)*TR3IMAG
           A3IMAG=B(IID)*TR3REAL+A(IID)*TR3IMAG
           T1REAL=A0REAL+A2REAL
           T1IMAG=A0IMAG+A2IMAG
           T2REAL=A1REAL+A3REAL
           T2IMAG=A1IMAG+A3IMAG
           T3REAL=A0REAL-A2REAL
           T3IMAG=A0IMAG-A2IMAG
           T4REAL=A1REAL-A3REAL
           T4IMAG=A1IMAG-A3IMAG
           C(J)=T1REAL+T2REAL
           D(J  )=T1IMAG+T2IMAG
           JM2SKIP=J+M2SKIP1
           C(JM2SKIP)=T3REAL+T4IMAG
           D(JM2SKIP)=T3IMAG-T4REAL
           JNL=JN+L1
           C(JNL   )=T1REAL-T2REAL
           D(JNL)=T2IMAG-T1IMAG
           JM1L=JM1+L1
           C(JM1L   )=T3REAL-T4IMAG
           D(JM1L)=-T4REAL-T3IMAG
           I=I+ISKIP
           J=J+ISKIP1
           L1=L1+ISKIP1
32        CONTINUE
          I=I+JUMPMA4
          J=J+MASKIP1
31       CONTINUE
         IF(MOD(LA,2).NE.0) RETURN
         DO 33 L=1,MASKIP,ISKIP
          X0=A(I)
          X1=A(I+IB)
          X2=A(I+IC)
          X3=A(I+ID)
          T1=SIN45*(X1-X3)
          T2=SIN45*(X1+X3)
          C(J)=X0+T1
          D(J)=-T2-X2
          JM2SKIP=J+M2SKIP1
          C(JM2SKIP)=X0-T1
          D(JM2SKIP)=X2-T2
          I=I+ISKIP
          J=J+ISKIP1
33       CONTINUE
         RETURN
C        ------- FACTOR 5 ------------
500        CONTINUE
        MA5SKIP=MASKIP*5
        KEND=(N-MA)/2+MA
        IF(MOD(LA,2).EQ.0) KEND=(N-2*MA)/2+MA*2
        KDIF=KEND+KEND-M2-MA-M
        IF(MOD(LA,2).EQ.0) KDIF=KDIF-MA
        JUMPMA5=JUMP+MA5SKIP
        IJ2=M2SKIP1
        IJ1=IJ2-MASKIP1
        IJ4=IJ2+M2SKIP1
        IJ3=IJ4-MASKIP1
        IC=IB+MASKIP
        ID=IC+MASKIP
        IE=ID+MASKIP
        L1=1
         DO 40 L=1,MASKIP,ISKIP
          X0=A(L)
          X1=A(L+IB)
          X2=A(L+IC)
          X3=A(L+ID)
          X4=A(L+IE)
          T1=X1+X4
          T2=X2+X3
          T3=SIN72*(X1-X4)
          T4=SIN72*(X2-X3)
          T5=T1+T2
          T6=SQ54*(T1-T2)
          T7=X0-0.25*T5
          C(L1)=X0+T5
          C(L1+IJ1)=T7+T6
          C(L1+IJ2)=-(T3+SIN3672*T4)
          C(L1+IJ3)=T7-T6
          C(L1+IJ4)=-(SIN3672*T3-T4)
          L1=L1+ISKIP1
40       CONTINUE
         IF(MA.EQ.M) RETURN
         J=MASKIP1+1
         I=MASKIP+1+JUMP
         MM=MHLF
         IF(MOD(LA,2).EQ.0) MM=MM-MA
        DO 41 K=MA,MM,MA
          K2=K+K
          KJ=KDIF-K2
          KJ=(KJ+KJ)*ISKIP1
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR4REAL=TRIGS(KK)
          TR4IMAG=-TRIGSI(KK)
          DO 42 L=1,MASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IIB=I+IB
           A1REAL=A(IIB)*TR1REAL-B(IIB)*TR1IMAG
           A1IMAG=B(IIB)*TR1REAL+A(IIB)*TR1IMAG
           IIC=I+IC
           A2REAL=A(IIC)*TR2REAL-B(IIC)*TR2IMAG
           A2IMAG=B(IIC)*TR2REAL+A(IIC)*TR2IMAG
           IID=I+ID
           A3REAL=A(IID)*TR3REAL-B(IID)*TR3IMAG
           A3IMAG=B(IID)*TR3REAL+A(IID)*TR3IMAG
           IIE=I+IE
           A4REAL=A(IIE)*TR4REAL-B(IIE)*TR4IMAG
           A4IMAG=B(IIE)*TR4REAL+A(IIE)*TR4IMAG
           T1REAL=A1REAL+A4REAL
           T1IMAG=A1IMAG+A4IMAG
           T2REAL=A2REAL+A3REAL
           T2IMAG=A2IMAG+A3IMAG
           T3REAL=SIN72*(A1REAL-A4REAL)
           T3IMAG=SIN72*(A1IMAG-A4IMAG)
           T4REAL=SIN72*(A2REAL-A3REAL)
           T4IMAG=SIN72*(A2IMAG-A3IMAG)
           T5REAL=T1REAL+T2REAL
           T5IMAG=T1IMAG+T2IMAG
           T6REAL=SQ54*(T1REAL-T2REAL)
           T6IMAG=SQ54*(T1IMAG-T2IMAG)
           T7REAL=A0REAL-T5REAL*0.25
           T7IMAG=A0IMAG-T5IMAG*0.25
           T8REAL=T7REAL+T6REAL
           T8IMAG=T7IMAG+T6IMAG
           T9REAL=T7REAL-T6REAL
           T9IMAG=T7IMAG-T6IMAG
           T10REAL=T3REAL+SIN3672*T4REAL
           T10IMAG=T3IMAG+SIN3672*T4IMAG
           T11REAL=SIN3672*T3REAL-T4REAL
           T11IMAG=SIN3672*T3IMAG-T4IMAG
           C(J)=A0REAL+T5REAL
           D(J)=A0IMAG+T5IMAG
           JM2SKIP=J+M2SKIP1
           C(JM2SKIP)=T8REAL+T10IMAG
           D(JM2SKIP)=T8IMAG-T10REAL
           JM4=JM2SKIP+M2SKIP1
           C(JM4)=T9REAL+T11IMAG
           D(JM4)=T9IMAG-T11REAL
           JNL=KJ+J
           C(JNL   )=T9REAL-T11IMAG
           D(JNL)=-(T9IMAG+T11REAL)
           JNLM2SKIP=JNL-M2SKIP1
           C(JNLM2SKIP)=T8REAL-T10IMAG
           D(JNLM2SKIP)=-(T8IMAG+T10REAL)
           I=I+ISKIP
           J=J+ISKIP1
42        CONTINUE
          I=I+JUMPMA5
          J=J+MASKIP1
41       CONTINUE
         IF(MOD(LA,2).NE.0) RETURN
         DO 43 L=1,MASKIP,ISKIP
          X0=A(I)
          X1=A(I+IB)
          X2=A(I+IC)
          X3=A(I+ID)
          X4=A(I+IE)
          T1=X1-X4
          T2=X1+X4
          T3=X2-X3
          T4=X2+X3
          T5=T1-T3
          T6=X0+0.25*T5
          T7=SQ54*(T1+T3)
          C(J)=T6+T7
          D(J)=-SIN36*T2-SIN72*T4
          JM2SKIP=J+M2SKIP1
          C(JM2SKIP)=T6-T7
          D(JM2SKIP)=-SIN72*T2+SIN36*T4
          C(JM2SKIP+M2SKIP1)=X0-T5
          I=I+ISKIP
          J=J+ISKIP1
43       CONTINUE
         RETURN
C        ---- FACTOR 6 -----------
600        CONTINUE
        MA6SKIP=MASKIP*6
        JUMPMA6=JUMP+MA6SKIP
        IJ2=M2SKIP1
        IJ1=IJ2-MASKIP1
        IC=IB+MASKIP
        ID=IC+MASKIP
        IE=ID+MASKIP
        IG=IE+MASKIP
        L1=1
         DO 50 L=1,MASKIP,ISKIP
          X0=A(L)
          X1=A(L+IB)
          X2=A(L+IC)
          X3=A(L+ID)
          X4=A(L+IE)
          X5=A(L+IG)
           T1REAL=X2+X4
           T2REAL=X0-0.5*T1REAL
           T3REAL=SIN60*(X2-X4)
           Y0REAL=X0+T1REAL
           Y4REAL=T2REAL
           Y4IMAG=-T3REAL
           Y2REAL=T2REAL
           Y2IMAG=T3REAL
           T1REAL=X5+X1
           T2REAL=X3-0.5*T1REAL
           T3REAL=SIN60*(X5-X1)
           Y3REAL=X3+T1REAL
           Y1REAL=T2REAL
           Y1IMAG=-T3REAL
           Y5REAL=T2REAL
           Y5IMAG=T3REAL
           C(L1    )=Y0REAL+Y3REAL
           LIJ1=L1+IJ1
           C(LIJ1)=Y4REAL-Y1REAL
           D(LIJ1)=Y4IMAG-Y1IMAG
           LIJ3=LIJ1+M2SKIP1
           C(LIJ3)=Y2REAL+Y5REAL
           D(LIJ3)=Y2IMAG+Y5IMAG
           C(L1+IJ)=Y0REAL-Y3REAL
           L1=L1+ISKIP1
50       CONTINUE
         IF(MA.EQ.M) RETURN
         J=MASKIP1+1
         I=MASKIP+1+JUMP
         MM=MHLF
         IF(MOD(LA,2).EQ.0) MM=MM-MA
        DO 51 K=MA,MM,MA
          K2=K+K
         JN=IJ-K2*ISKIP1
         JM1=JN-M2SKIP1
          KK=K2+1
          TR1REAL=TRIGS(KK)
          TR1IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR2REAL=TRIGS(KK)
          TR2IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR3REAL=TRIGS(KK)
          TR3IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR4REAL=TRIGS(KK)
          TR4IMAG=-TRIGSI(KK)
          KK=KK+K2
          TR5REAL=TRIGS(KK)
          TR5IMAG=-TRIGSI(KK)
          DO 52 L=1,MASKIP,ISKIP
           A0REAL=A(I)
           A0IMAG=B(I)
           IIB=I+IB
           A1REAL=A(IIB)*TR1REAL-B(IIB)*TR1IMAG
           A1IMAG=B(IIB)*TR1REAL+A(IIB)*TR1IMAG
           IIC=I+IC
           A2REAL=A(IIC)*TR2REAL-B(IIC)*TR2IMAG
           A2IMAG=B(IIC)*TR2REAL+A(IIC)*TR2IMAG
           IID=I+ID
           A3REAL=A(IID)*TR3REAL-B(IID)*TR3IMAG
           A3IMAG=B(IID)*TR3REAL+A(IID)*TR3IMAG
           IIE=I+IE
           A4REAL=A(IIE)*TR4REAL-B(IIE)*TR4IMAG
           A4IMAG=B(IIE)*TR4REAL+A(IIE)*TR4IMAG
           IIG=I+IG
           A5REAL=A(IIG)*TR5REAL-B(IIG)*TR5IMAG
           A5IMAG=B(IIG)*TR5REAL+A(IIG)*TR5IMAG
           T1REAL=A2REAL+A4REAL
           T1IMAG=A2IMAG+A4IMAG
           T2REAL=A0REAL-0.5*T1REAL
           T2IMAG=A0IMAG-0.5*T1IMAG
           T3REAL=SIN60*(A2REAL-A4REAL)
           T3IMAG=SIN60*(A2IMAG-A4IMAG)
           Y0REAL=A0REAL+T1REAL
           Y0IMAG=A0IMAG+T1IMAG
           Y4REAL=T2REAL+T3IMAG
           Y4IMAG=T2IMAG-T3REAL
           Y2REAL=T2REAL-T3IMAG
           Y2IMAG=T2IMAG+T3REAL
           T1REAL=A5REAL+A1REAL
           T1IMAG=A5IMAG+A1IMAG
           T2REAL=A3REAL-0.5*T1REAL
           T2IMAG=A3IMAG-0.5*T1IMAG
           T3REAL=SIN60*(A5REAL-A1REAL)
           T3IMAG=SIN60*(A5IMAG-A1IMAG)
           Y3REAL=A3REAL+T1REAL
           Y3IMAG=A3IMAG+T1IMAG
           Y1REAL=T2REAL+T3IMAG
           Y1IMAG=T2IMAG-T3REAL
           Y5REAL=T2REAL-T3IMAG
           Y5IMAG=T2IMAG+T3REAL
           C(J    )=Y0REAL+Y3REAL
           D(J    )=Y0IMAG+Y3IMAG
           JM2SKIP=J+M2SKIP1
           C(JM2SKIP    )=Y4REAL-Y1REAL
           D(JM2SKIP    )=Y4IMAG-Y1IMAG
           JM4=JM2SKIP+M2SKIP1
           C(JM4    )=Y2REAL+Y5REAL
           D(JM4    )=Y2IMAG+Y5IMAG
           JNL=JN+L
           C(JNL    )=Y0REAL-Y3REAL
           D(JNL    )=-(Y0IMAG-Y3IMAG)
           JNLM2SKIP=JNL-M2SKIP1
           C(JNLM2SKIP)=Y4REAL+Y1REAL
           D(JNLM2SKIP)=-(Y4IMAG+Y1IMAG)
           JNLM4=JNLM2SKIP-M2SKIP1
           C(JNLM4)=Y2REAL-Y5REAL
           D(JNLM4)=-(Y2IMAG-Y5IMAG)
           I=I+ISKIP
           J=J+ISKIP1
52        CONTINUE
          I=I+JUMPMA6
          J=J+MASKIP1
51       CONTINUE
         IF(MOD(LA,2).NE.0) RETURN
         DO 53 L=1,MASKIP,ISKIP
          X0=A(I)
          X1=A(I+IB)
          X2=A(I+IC)
          X3=A(I+ID)
          X4=A(I+IE)
          X5=A(I+IG)
          T1=SIN60*(X5-X1)
          T2=SIN60*(X2+X4)
          T3=X2-X4
          T4=X1+X5
          T5=X0+0.5*T3
          T6=-X3-0.5*T4
          C(J)=T5-T1
          D(J)=T6-T2
          JM2SKIP=J+M2SKIP1
          C(JM2SKIP)=X0-T3
          D(JM2SKIP)=X3-T4
          JM4=JM2SKIP+M2SKIP1
          C(JM4)=T5+T1
          D(JM4)=T6+T2
          I=I+ISKIP
          J=J+ISKIP1
53       CONTINUE
         RETURN
         END
