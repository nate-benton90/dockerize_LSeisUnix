h26930
s 00000/00000/00072
d D 1.6 88/11/15 14:04:20 shuki 6 5
c 
e
s 00003/00008/00069
d D 1.5 88/05/17 10:26:59 moshe 5 4
c 
e
s 00002/00002/00075
d D 1.4 88/05/17 09:15:34 shuki 4 3
c 
e
s 00007/00001/00070
d D 1.3 88/05/17 09:14:19 shuki 3 2
c 
e
s 00001/00001/00070
d D 1.2 88/05/15 12:05:53 moshe 2 1
c 
e
s 00071/00000/00000
d D 1.1 88/05/15 11:58:53 moshe 1 0
c date and time created 88/05/15 11:58:53 by moshe
e
u
U
f e 0
t
T
I 1
D 5
      SUBROUTINE AGC(TRACE,NSAMP,WIN,DT,IND)
E 5
I 5
      SUBROUTINE AGC(TRACE,AAA,NSAMP,WIN,DT,IND)
E 5
C     ***************************************************
C      AUTOMATIC GAIN CONTROL
C     ****************************************************
C      TRACE - INPUT TRACE
I 5
C      AAA   - SCRETCH ARRAY (= TRACE)
E 5
C      NSAMP - NUMBER OF SAMPLES
C      DT    - TIME INTERVAL (M.SEC)
C      WIN    - WINDOW LENGTH (M.SEC)
C      IND - SUMMING OPTION :
C                            0 - ABSOLUTE VALUE SUM
C                            1 - R.M.S
C      REMARKS:
C               1. DATA LEVEL IS SET TO BE 50.
C     ****************************************************
D 2
      DIMENSION TRACE(1),AAA(7000)
E 2
I 2
D 3
      DIMENSION TRACE(1),AAA(10000)
E 3
I 3
D 5
      DIMENSION TRACE(1),AAA(15000)
E 5
I 5
      DIMENSION TRACE(1),AAA(1)
E 5
E 3
E 2
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
I 3
D 5
      if(nwin.ge.nsamp)then
      print *,'** AGC window is greater than the trace !!'
D 4
      print *,'** window length is set to be equal to 
      print *,'** the trace length.
E 4
I 4
      print *,'** window length is set to be equal to'
      print *,'** the trace length.'
E 4
      nwin=nsamp-1
      endif
E 5
E 3
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
E 1
