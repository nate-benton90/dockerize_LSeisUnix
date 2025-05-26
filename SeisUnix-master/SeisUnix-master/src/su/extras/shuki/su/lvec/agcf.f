      SUBROUTINE AGC(TRACE,AAA,NSAMP,WIN,DT,IND)
C     ***************************************************
C      AUTOMATIC GAIN CONTROL
C     ****************************************************
C      TRACE - INPUT TRACE
C      AAA   - SCRETCH ARRAY (= TRACE)
C      NSAMP - NUMBER OF SAMPLES
C      DT    - TIME INTERVAL (M.SEC)
C      WIN    - WINDOW LENGTH (M.SEC)
C      IND - SUMMING OPTION :
C                            0 - ABSOLUTE VALUE SUM
C                            1 - R.M.S
C      REMARKS:
C               1. DATA LEVEL IS SET TO BE 50.
C     ****************************************************
      DIMENSION TRACE(1),AAA(1)
      IF(IND.EQ.1)THEN
      DO 21 I=1,NSAMP
21    AAA(I)=TRACE(I)*TRACE(I)
      ELSE
      DO 22 I=1,NSAMP
22    AAA(I)=ABS(TRACE(I))
      ENDIF
      DL=50.
      IP=0
      N1=NSAMP-1
      NWIN=WIN/DT
      IF(NWIN/2*2.EQ.NWIN)NWIN=NWIN+1
      SUM=AAA(1)
      IN=1
      DO 100 I=2,N1
      IF(I.LE.NWIN/2+1)THEN
      IN=IN+1
      SUM=SUM+AAA(IN)
      IN=IN+1
      SUM=SUM+AAA(IN)
      DL1=IN*DL/NWIN
      IF(SUM.LT.0.1E-30)GOTO 100
      IF(IND.EQ.1)DEV=SQRT(SUM)
      IF(IND.EQ.0)DEV=SUM
      TRACE(I)=TRACE(I)*DL1/DEV
      GOTO 100
      ENDIF
      IF(IN.GE.NWIN.AND.IN.LT.NSAMP)THEN
      IN=IN+1
      SUM=SUM+AAA(IN)
      IW=IN-NWIN
      SUM=SUM-AAA(IW)
      IF(SUM.LT.0.1E-30)GOTO 100
      IF(IND.EQ.1)DEV=SQRT(SUM)
      IF(IND.EQ.0)DEV=SUM
      TRACE(I)=TRACE(I)*DL/DEV
      GOTO 100
      ENDIF
      IF(IP.NE.0)IN1=IN1+1
      IF(IP.EQ.0)THEN
      IP=1
      IN1=NSAMP-NWIN+1
      ENDIF
      SUM=SUM-AAA(IN1)
      IN1=IN1+1
      SUM=SUM-AAA(IN1)
      DL1=(NSAMP-IN1)*DL/NWIN
      IF(SUM.LT.0.1E-30)GOTO 100
      IF(IND.EQ.1)DEV=SQRT(SUM)
      IF(IND.EQ.0)DEV=SUM
      TRACE(I)=TRACE(I)*DL1/DEV
100   CONTINUE
      TRACE(1)=0.
      TRACE(NSAMP)=0.
      RETURN
      END
