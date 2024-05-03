      SUBROUTINE ACOREX(BUF,LBUF,IBUF,SCR,LSCR)
!     ACOREX IS THE EXECUTION PHASE OF THE SEISMIC REFLECTION PROCESS ACORR
!  (AUTOCORRELATION).  THE USER'S PARAMETERS MUST BE IN
!  DISC FILE MUNIT (IN COMMON /ACOR/) AND THE TRACE WITH TRACE HEADER
!  MUST BE IN MEMORY ARRAY BUF.  THE OUTPUT WILL ALSO BE IN ARRAY BUF AND
!  WILL CONSIST OF THE POSITIVE LAGS OF THE AUTOCORRELATION.  MULTIPLE WINDOW
!  AUTOCORRELATION OUTPUT IS STILL A SINGLE TRACE, WITH THE WINDOWS CONCATENATED.
!  AUTOCORRELATION WINDOWS TIMES FOR TRACES BETWEEN
!  THOSE SHOTS OR RPS DESCRIBED BY THE USER ARE CALCULATED BY LINEAR
!  INTERPOLATION.
!
!  ARGUMENTS:
!  BUF    - THE TRACE TO BE PROCESSED, INCLUDING THE TRACE HEADER.  THE FIRST
!           DATA SAMPLE MUST BE AT TIME DELAY.  THIS IS THE FLOATING
!           POINT (REAL) TRACE ARRAY.
!  LBUF   - THE LONG INTEGER TRACE ARRAY.  THIS IS REALLY THE SAME AS BUF, BUT
!           PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING ANYTHING TO AN ARGUMENT.
!  IBUF   - THE SHORT INTEGER TRACE ARRAY.  NEEDED FOR 16 BIT TRACE HEADER
!           ADDRESSES.
!  SCR    - A SCRATCH ARRAY FOR READING THE PARAMETERS.  THEREFORE, SCR MUST
!           BE AT LEAST 56 32BIT WORDS BIG.  SCR MAY BE DESTROYED BY THE CALLING
!           ROUTINE.
!  LSCR   - THE SAME SCRATCH ARRAY BECAUSE OF THE EQUIVALENCING PROBLEM.
!
!  COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, JULY 1980
!  mod 17 May 10 - comment out zeroing of real header words 47 & 48 (mute times)
!
!
      PARAMETER (MAX=10)                                               ! THE MAXIMUM NUMBER OF ELEMENTS OF THE USER ARRAY SETS
      PARAMETER (NPARS=19)                                             ! THE LENGTH OF EACH PARAMETER LIST
      DIMENSION BUF(111),LBUF(111),IBUF(111),SCR(111),LSCR(111)
      INTEGER*2 IBUF
      DIMENSION OLDSET(MAX),SETS(MAX),ISETS(MAX)
      DIMENSION OLEN(5),NLAGS(5)
      COMMON /ACOR/ MUNIT,NLISTS
      COMMON /SIOAP/ IASGND,IRELSE,IN,IOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP
      COMMON /APMEM/ A(32766)
      COMMON /READT/ ILUN,NUMHDR,NUMDAT
      COMMON /WRITET/ JUNIT,NSAMPS
      INTEGER FNO
      LOGICAL FIRST
      SAVE
      DATA FIRST /.TRUE./
!****
!****     FIND THE PARAMETER LIST (ON DISC) FOR THIS SHOT (RP)
!****
      ISIG=0
      IF(.NOT.FIRST) GO TO 50
      FIRST=.FALSE.
   10 CONTINUE                                                         !GET THE FIRST PARAMETER LIST INT0 MEMORY ARRAY SCR
      CALL PODISC(MUNIT,1,0)                                           ! REWIND THE PARAMETER FILE
      CALL RDDISC(MUNIT,SCR,NPARS,ISTAT)
      ISIG=1                                                           ! SET SIGNAL INDICATING THAT PARAM LIST IS IN SCR
      FNO=LSCR(1)
      LNO=LSCR(2)
      MLISTS=1
   50 CONTINUE
      LNUM=LBUF(3)                                                     !  IS THE DATA ON TAPE SORTED BY SHOT
   60 IF(LBUF(7).NE.0) LNUM=LBUF(6)                                    ! OR BY RP
      IF(LNUM.EQ.LLNUM) GO TO 1000                                     ! IS IT THE SAME AS THE LAST SHOT (RP)
      LLNUM=LNUM                                                       !NO, IT'S NOT THE SAME - DO WE NEED NEW PARAMS
   70 IF(LNUM.GE.FNO) GO TO 100                                        ! IS THIS SHOT BEFORE THIS PARAMTER LIST
      IF(MLISTS.EQ.1) GO TO 500                                        !IS IT BEFORE THE FIRST LIST
      IF(LNUM.LE.LNO) GO TO 10                                         ! IS IT IN OR BEFORE THE LAST LIST
      GO TO 500                                                        !IT MUST BE BETWEEN THE 2 LISTS
  100 CONTINUE                                                         !  THE CURRENT SHOT (RP) IS >= LNO
      IF(LNUM.LE.LNO) GO TO 500                                        !USE THE PARAMETERS OF THIS LIST
      IF(MLISTS.LT.NLISTS) GO TO 110                                   ! ANY MORE USER PARAM LISTS ON DISC
      IF(ISIG.EQ.0) GO TO 1000
      GO TO 500
!****
!****   GET ANOTHER USER PARAMETER LIST FROM DISC
!****
  110 CONTINUE                                                         ! SET THE PRESENT LIST INTO OLD SO WE CAN GET A NEW ONE IN SCR
      IF(ISIG.EQ.1) GO TO 118
      DO I=1,MAX
         OLDSET(I)=SETS(I)
      ENDDO
      GO TO 130
  118 CONTINUE
      DO I=1,MAX                                                   ! SAVE THE CURRENT PARAMETER SET
         OLDSET(I)=SCR(I+9)
      ENDDO
      DO I=1,5
         OLEN(I)=SCR(I+4)
      ENDDO
  130 CALL RDDISC(MUNIT,SCR,NPARS,ISTAT)
      ISIG=1
      FNO=LSCR(1)
      LNO=LSCR(2)
      MLISTS=MLISTS+1
      GO TO 70
!****
!****     SAVE THE CURRENT LIST IN CUR AND LEVS
!****
  500 CONTINUE
      IF(ISIG.EQ.0) GO TO 1000
      IADDWB=LSCR(3)
      LPRINT=LSCR(4)
      IF(LNUM.LT.FNO) GO TO 600                                        !DON'T BOTHER IF SPATIALLY VARYING
      DO I=1,10
         SETS(I)=SCR(I+9)
      ENDDO
      DO I=1,5
         OLEN(I)=SCR(I+4)
      ENDDO
      GO TO 1000
!****
!****      SPATIALLY VARY THE DESIGN AND APPLICATION WINDOW TIMES
!****
  600 CONTINUE
      RATIO=(LNUM-LNO)/(FNO-LNO)
      DO I=1,MAX
         SETS(I)=RATIO*(SCR(I+9)-OLDSET(I))+OLDSET(I)
      ENDDO
!****
!****       SETUP THE INDEXES AND THE AP ARRAYS
!****
 1000 CONTINUE
      NSAMPS=IBUF(58)                                                  !THE NUMBER OF DATA SAMPLES IN THE TRACE
      DELAY=BUF(46)                                                    !THE FLOATING POINT DEEP WATER DELAY IN SECONDS
      SI=BUF(49)                                                       !THE FLOATING POINT SAMPLE INTERVAL IN SECONDS
      MAXLAG=0
      DO I=1,5
         NLAGS(I)=OLEN(I)/SI+1
         MAXLAG=MAXLAG+NLAGS(I)
      ENDDO
      IF( iaddwb .EQ. 0 ) THEN                                         !should we add the water bottom time in?
          wbtime = 0.                                                  ! no
      ELSE
          wbtime = buf(50)                                             !  get the water bottom time
      ENDIF
      ndows = 0                                                        ! count the number of data windows
      DO 1020 i = 1, max
          isets(i) = 0
          IF( sets(i) .EQ. 0. .AND. i .NE. 1 ) GOTO 1020               ! a 0 time means no more
          isets(i) = ( sets(i) + wbtime - delay ) / si + 1
          IF( isets(i) .LT. 1 ) isets(i) = 1                           !always start after the beginning
          IF( isets(i) .GT. nsamps ) isets(i) = nsamps                 !but don't go too far
          IF( MOD(i,2) .EQ. 0 ) THEN                                   !is it the end of a window?
              ndows = ndows + 1
              IF( isets(i) .LE. isets(i-1) ) THEN                      !is the end before the start?
                  isets(i-1) = 0                                       !drop the whole window
                  isets(i) = 0
                  ndows = ndows -1
              ENDIF
          ENDIF
 1020 CONTINUE
      IF( ndows .EQ. 0 ) THEN                                          !preset to do the whole trace
          isets(1) = 1
          isets(2) = nsamps
          ndows = 1
      ENDIF
!****
!****    FIGURE OUT THE AP MEMORY ALLOCATION
!****
 1060 IOUT=1                                                           ! LEAVE THE DATA IN THE AP
      CALL INAP(BUF(NUMHDR+1),NSAMPS)                                  !PUT THE DATA IN THE AP IF NOT THERE
      ITEMP=NSAMPS/2                                                   ! ZERO FILL THE END OF THE TRACE (ASSUME NOTHING IS THERE!)
      IF(IUSEAP.EQ.0) GO TO 1070                                       ! ARE TO USE THE AP OR THHE HOST?
      CALL VCLR(IN+NSAMPS,1,ITEMP)                                     ! ZERO THE REST OF THE INPUT - THERE SEEMS
!                    TO BE SOME CRAP ON SETS THAT GO PAST END OF DATA
!****  SCALE THE DATA DOWN SO THAT THE CONVOLUTION DOESN'T OVERFLOW ON
!****  LONG WINDOWS
      CALL MAXMGV(IN,1,NEXTAD,NSAMPS)
      CALL APWR
      CALL APGET(AMAXV,NEXTAD,1,2)
      CALL APWD
      AMULT=1./AMAXV
      CALL APPUT(AMULT,NEXTAD,1,2)
      CALL APWD
      CALL VSMUL(IN,1,NEXTAD,IN,1,NSAMPS)
      GO TO 1100
 1070 CONTINUE                                                         ! THIS IS FOR HOST ACCOR
      J=IN-1                                                           ! AS IN THE AP CODE, NORMALIZE THE INPUT DATA
      AMAXV=0.                                                         !SO THAT ALL NUMBERS ARE LESS THAN 1. IN THE INPUT TRACE.
      DO 1080 II=1,NSAMPS                                              !THIS AVOIDS ARITHMETIC OVERFLOW DURING THE CORRELATION
      TEMP=ABS(A(J+II))
      IF(TEMP.GT.AMAXV) AMAXV=TEMP
 1080 CONTINUE
      AMULT=1./AMAXV
      DO II=1,NSAMPS
         A(J+II)=A(J+II)*AMULT
      ENDDO
 1100 NSAMPS=0                                                         !THE OUTPUT LENGTH
      IFOUT=NEXTAD                                                     ! THE AP ADDRESS OF THE OUTPUT
      ISCRAP=NEXTAD+MAXLAG                                             ! FIND A SCRATCH AP LOCATION AWAY FROM THE OUTPUT!
      DO I=1,NDOWS
         IF(IBUF(15).EQ.2) GO TO 1200                                     ! IS IT A DEAD TRACE
         J=I*2
         J1=J-1
         ILEN=ISETS(J)-ISETS(J1)
         ISETS(J1)=ISETS(J1)+IN-1                                         ! THE AP ADDRESS OF THE WINDOW
         IF(IUSEAP.EQ.0) GO TO 1110
         IF(IAND(LPRINT,2).NE.0) PRINT 1180,ISETS(J1),NLAGS(I),ILEN
 1180    FORMAT(' ABOUT TO AUTO FROM ',I5,' NLAGS=',I5,' LENGTH=',I8)
         CALL CONV(ISETS(J1),1,ISETS(J1),1,IFOUT,1,NLAGS(I),ILEN)
         GO TO 1120
 1110    INDX=ISETS(J1)
         IF(IAND(LPRINT,2).NE.0) PRINT 1115, INDX,ILEN,IFOUT,NLAGS(I),
     *   NEXTAD,ISCRAP
 1115    FORMAT(' CONVO ARGS:',7(1X,I5))
         CALL CONVO(+2,A(INDX),ILEN,A(INDX),ILEN,A(IFOUT),NLAGS(I))
 1120    IFOUT=IFOUT+NLAGS(I)                                             ! AP ADDRESS OF THE NEXT OUTPUT
 1200    NSAMPS=NSAMPS+NLAGS(I)                                           ! CHANGE THE RECORD LENGTH
      ENDDO
!****  SCALE THE AUTOCORRELATION SO THAT ZERO LAG IS +1.  THIS STOPS PEOPLE FROM
!**** ASKING TOO MANY DAMN QUESTIONS!!
      IF(IUSEAP.EQ.0) GO TO 1300
!****
!****  DO IT IN THE AP
      CALL APWR
      CALL APGET(AMAXV,NEXTAD,1,2)
      CALL APWD
      AMULT=1./AMAXV
      CALL APPUT(AMULT,ISCRAP,1,2)
      CALL APWD
      CALL VSMUL(NEXTAD,1,ISCRAP,IN,1,NSAMPS)
      GO TO 1400
!****
!****  DO THE AUTOCORRELATION SCALING IN THE HOST
 1300 J=NEXTAD-1
      K=IN-1
      AMULT=1./A(NEXTAD)                                               !ASSUME THAT TH ZERO LAG IS THE LARGEST CORRELATION
      DO II=1,NSAMPS
         A(K+II)=A(J+II)*AMULT
      ENDDO
!****
!****       SET THE TRACE HEADER AND COMMON VARIABLES FOR A RECORD LENGTH CHANGE
!****
 1400 IBUF(55)=0
      IBUF(56)=0
      IBUF(57)=0
      IBUF(58)=NSAMPS
      BUF(46)=0.
!      BUF(47)=0.
!      BUF(48)=0.
      BUF(50)=0.
      NUMDAT=NSAMPS
      RETURN
      END
