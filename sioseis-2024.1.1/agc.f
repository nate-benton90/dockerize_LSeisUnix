      SUBROUTINE AGC(BUFIN,BUFOUT,SCRBUF,SPEC)
!     AGC CALCULATES AND APPLIES AGC (AUTOMATIC GAIN CONTROL) TO AN ARRAY OF REAL
! VALUES (A SEISMIC TRACE).  AGC IS A TYPE OF SCALING SUCH THAT THE AVERAGE
! ABSOLUTE VALUE OF THE AMPLITUDES WITHIN A WINDOW WILL BE A CERTAIN LEVEL
! AFTER SCALING.
!     THIS AGC PROGRAM DIFFERS FROM OTHER AGCS IN THE WAY IT HANDLES ZERO (OR
! NEAR ZERO) INPUT AMPLITUDES.  IF THE WINDOW CONTAINS TOO MANY ZEROES, THE
! CALCULATED MULTIPLIER FOR THAT WINDOW IS IGNORED AND THE LAST ONE IS USED.
! E.G. IN THE CASE OF MUTES PRIOR TO P-BREAKS, THE FIRST MULTIPLIER WILL BE
! CALULATED FROM SAY 75 PERCENT LIVE VALUES, THUS PREVENTING THE P-BREAKS FROM
! BEING OVERDRIVEN.
!     ANY TRACE WITH ALL AMPLITIDES BELOW A CERTAIN LEVEL (DEAD) WILL BE SET TO
! ZERO.
!
!   AGC VARIABLES:
!   ARGUMENTS:
!   BUFIN  - THE INPUT ARRAY (REAL)
!   BUFOUT -  THE OUTPUT ARRAY (REAL) - THE INPUT AND OUTPUT ARRAYS MAY BE THE
!             SAME.
!   SCRBUF  - A SCRATCH ARRAY (REAL) - MUST BE INDXET LONG
!    SPEC   - SPECIAL CASE SWITCH
!           <0, PRINT THE AVERAGE ABSOLUTE VALUE FOR EACH WINDOW
!           =0, DO NOTHING SPECIAL
! COMMON VARIABLE:
!   INDXST  - THE INDX OF THE FIRST SAMPLE IN BUFIN TO PROCESS.  BUFOUT WILL
!             HAVE UNTOUCHED DATA VALUES PRIOR TO INDXST.  THIS PARAMETER CAN BE
!             NON-ZERO IN ORDER TO AVOID SCALING THE WATER LAYER (SAY CPU TIME).
!             THIS SHOULD BE 1 IN MOST CASES.
!   INDXET - THE INDEX OF BUFIN OF THE LAST SAMPLE TO PROCESS.  THE NUMBER OF
!            SAMPLES IN THE ARRAY (WHEN INDXET IS 1).
!   OLEVEL - THE OUTPUT AMPLITUDE LEVEL.  THE AVERAGE ABSOLUTE VALUE.
!   CLIP   - THE CLIP LEVEL.  ANY AMPLITUDE (ABS VALUE) EXCEEDING THE CLIP
!            VALUE WILL BE SET TO CLIP.
!   NPTS   - THE NUMBER OF SAMPLES IN EACH WINDOW( THE WINDOW LENGTH IN SAMPLES)
!            REMEMBER THAT THE WINDOW IS SLIDE (ADVANCED) BY ONE SAMPLE
!   NDEAD  - THE NUMBER OF DEAD VALUES TO ALLOW IN A WINDOW IN ORDER TO GET A
!            NEW WINDOW MULTIPLIER.
!   DEAD   - A LEVEL BELOW WHICH AN AMPLITUDE IS CONSIDERED DEAD.
!   MIDPT  - THE MID POINT OF THE WINDOW TO RECEIVE THE MULTIPLIER FOR THE WINDOW
!            A FORWARD LOOKING WINDOW WOULD HAVE MIDPT=1
!   agcpct - The percent AGC, expressed as percent/100.  agcpct
!            multiplies the multiplier.  An agcpct < 1 has the 
!            effect of lessening the AGC (makes it "softer").
!
!
!    PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, JANUARY 1979
!  mod 13 April 1991 by pch - add agcpct
!  mod June 96 by s.s - change the way agcpct works
!
      COMMON /AGCCOM/ INDXST,INDXET,OLEVEL,CLIP,NPTS,NDEAD,DEAD,MIDPT,
     &     agcpct
      DIMENSION BUFIN(1),BUFOUT(1),SCRBUF(1)
      LOGICAL FIRST
      double precision agctot, agcmean
      FIRST=.TRUE.
      NZEROS=0
      SUM=0.
      NDONE=0
!****
!****       PUT THE ABSOLUTE VALUES INTO SCRBUF
!****
      DO 100 I = 1, INDXET
      SCRBUF(I)=ABS(BUFIN(I))
  100 CONTINUE
!  S.S  percent agc change.  First, find the average absolute value of nonzero
!    samples for the entire trace
      if( agcpct .ne. 1.) then
        agctot = 0
        nnonzero = 0
        do 110 i=1,indxet
          if(scrbuf(i) .ne. 0.) then
            agctot = agctot + abs(scrbuf(i))
            nnonzero = nnonzero + 1
          endif
110     continue
        agcmean = agctot / nnonzero
!        write (*,*) 'agcmean ',agcmean,' agcpct',agcpct
      endif
!****
!****            FIND THE FIRST WINDOW WITH A GOOD MULTIPLIER
!****
      INDX=INDXST-1
      DO 190 I = 1, NPTS                                                !  DO THE FIRST WINDOW
         J = INDX + I
         IF( SCRBUF(J) .GT. DEAD ) THEN
             SUM = SUM + SCRBUF(J)
         ELSE
             NZEROS = NZEROS + 1
         ENDIF
  190 CONTINUE
  200 CONTINUE
      NDONE = NDONE + 1
      IF(NZEROS.LT.NDEAD) GO TO 300
      J=INDXST+NDONE-1
      K = J + NPTS
      IF(K.GT.INDXET) GO TO 250
      IF( SCRBUF(J).LE.DEAD) THEN
          NZEROS = NZEROS - 1
          IF( nzeros .LT. 0 ) nzeros = 0
      ELSE
          SUM = SUM - SCRBUF(J)
      ENDIF
      IF( SCRBUF(K) .LE. DEAD ) THEN
          NZEROS = NZEROS + 1
          IF( nzeros .GT. npts ) nzeros = npts
      ELSE
          SUM = SUM + SCRBUF(K)
      ENDIF
      GO TO 200
!****
!****           THE TRACE IS DEAD HERE
!****
  250 CONTINUE
      DO I = 1, INDXET                                              ! MOVE THE INPUT WITHOUT CHANGING THE AMPLITUDES
         BUFOUT(I)=BUFIN(I)
      ENDDO
      PRINT 270
  270 FORMAT(' ***  WARNING  ***  NOT ENOUGH NON-ZERO VALUES IN ORDER',
     *   ' TO AGC, NO AGC APPLIED.')
      RETURN
!****
!****
  300 CONTINUE
      TEMP=FLOAT(NPTS-NZEROS)                                           ! CHECK FOR NUMBER SIZE
      IF( TEMP .LE. 0. ) THEN
          AVE = CLIP  
      ELSE
          AVE = SUM / TEMP
      ENDIF
      IF(AVE.EQ.0.) AVE=.00000001
      IF( SPEC .LT. 0 ) PRINT *,' ave=',ave,' ndone=',ndone
!****
!****         MULTIPLY THE CENTRE POINT OF THE WINDOW AND ADVANCE THE WINDOW
!****         ONE SAMPLE AND REPEAT
!****           USE THE FIRST MULTIPLIER FOR ALL POINTS LESS THAN THE FIRST MID
!****      POINT
!****
! S.S.  changing way pctagc works
!      xmult = olevel / ave * agcpct
!      xmult = olevel / ave / (2.-agcpct)
      IF( agcpct .EQ. 1. ) THEN
          xmult = olevel / ave
      ELSE
          xmult = olevel / (ave + ((1.-agcpct) * agcmean))
      ENDIF
      IF( FIRST ) THEN
          FIRST = .FALSE.
          N = INDXST + NDONE + MIDPT - 3
          DO 390 I = 1, N
             BUFOUT(I) = BUFIN(I) * XMULT
             IF( CLIP .NE. 0. ) THEN
                 IF( ABS(BUFOUT(I)) .GT. CLIP ) 
     *               BUFOUT(I) = SIGN(CLIP,BUFIN(I))
             ENDIF
  390     CONTINUE
      ENDIF
  400 CONTINUE
      I = INDXST + NDONE + MIDPT - 2
      BUFOUT(I) = BUFIN(I) * XMULT
      IF( CLIP .NE. 0.) THEN
          IF( ABS(BUFOUT(I)) .GT. CLIP ) 
     *        BUFOUT(I) = SIGN(CLIP,BUFIN(I))
      ENDIF
      IF( INDXST+NPTS+NDONE-1 .GT. INDXET ) GOTO 1000
      J=INDXST+NDONE-1
      K=J+NPTS
      IF( SCRBUF(J) .LE. DEAD ) THEN
          NZEROS = NZEROS - 1
          IF( nzeros .LT. 0 ) nzeros = 0
      ELSE
          SUM = SUM - SCRBUF(J)
      ENDIF
      IF( SCRBUF(K) .LE. DEAD ) THEN
          NZEROS = NZEROS + 1
          IF( nzeros .GT. npts ) nzeros = npts
      ELSE
          SUM = SUM + SCRBUF(K)
      ENDIF
      NDONE = NDONE + 1
      IF( NZEROS .LT. NDEAD ) GOTO 300
      GOTO 400
!****
!****            FINISHED FINDING XMULTIPLIERS, NOW FINISH THE REMAINDER OF THE
!****       LAST WINDOW
!****
 1000 CONTINUE
      J=INDXST+NDONE+MIDPT-1
      DO 1010 I=J,INDXET
      BUFOUT(I)=BUFIN(I)*XMULT
      IF(CLIP.EQ.0.) GO TO 1010
      IF(ABS(BUFOUT(I)).GT.CLIP) BUFOUT(I)=SIGN(CLIP,BUFIN(I))
 1010 CONTINUE
      RETURN
      END
