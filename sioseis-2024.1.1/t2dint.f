      SUBROUTINE T2DINT(BUFIN,NSAMPS,ST,SIT,BUFOUT,MSAMPS,SD,SID,SCRBUF,
     *  vtp, nvtp )
!     T2DINT CONVERTS A TIME TRACE (BUFIN) INTO A DEPTH TRACE (BUFOUT) USING
!  INTERVAL VELOCITIES.  The output trace is uniformly sampled by sid.
!
!  ARGUMENTS:
!   BUFIN  - THE ARRAY OF TIME DOMAIN DATA TO BE TRANSFORMED.
!   NSAMPS - THE INTEGER NUMBER OF SAMPLES IN ARRAY BUFIN.  THE LENGTH OF THE
!            THE INPUT DATA.
!   ST     - THE START TIME OF BUFIN.  THE TIME IN SECONDS ASSOCIATED WITH THE
!            FIRST INPUT SAMPLE, BUFIN(1).
!   SIT    - SAMPLE INTERVAL IN TIME.  THE TIME UNITS, IN SECONDS, BETWEEN
!            SUCCESSIVE SAMPLES IN BUFIN.
!   BUFOUT - AN ARRAY TO RECEIVED THE TRANSFORMED TRACE.  THE DEPTH DOMAIN TRACE
!            THE OUTPUT ARRAY.  MUST BE DIFFERENT FROM BUFIN.
!   MSAMPS - THE NUMBER OF OUTPUT SAMPLES.  T2D WILL ONLY STORE MSAMPS SAMPLES
!            IN THE OUTPUT ARRAY.
!   SD     - START DEPTH.  THE DEPTH OF THE FIRST OUTPUT SAMPLE (BUFOUT(1)).
!   SID    - SAMPLE INTERVAL IN THE DEPTH DOMAIN.  THE  DISTANCE BETWEEN
!            SUCCESSIVE OUTPUT SAMPLES.
!   SCRBUF - A SCRATCH ARRAY OF LENGTH NSAMPS.
!   vtp    - The array of interval velocity-time pairs to use in conversion.
!            The first interval contains all times up to the first vtp time.
!   nvtp   - The number of words in the vtp array.
!
!   (C) ???????
!      PAUL HENKART, October 1990
!
      DIMENSION BUFIN(1),BUFOUT(1),SCRBUF(1),vtp(1)
!
!    METHOD:
!    1)  CALCULATE A DEPTH FOR EVERY TIME SAMPLE.
!    2)  Work through the output depths one-by-one and look for the
!        first input depth greater or equal to it.
!
!****
!****  fill scrbuf with the depth of every output sample
!****
      j = 1
      DO 100 I = 1, NSAMPS
         T = ST+SIT*FLOAT(I-1)
   10    IF( t .LE. vtp(j+1) ) THEN
             IF( j .EQ. 1 ) THEN
                 scrbuf(i) = vtp(1) * t
                 depth = scrbuf(i)
                 time = t
             ELSE
                 scrbuf(i) = depth + (t - time) * vtp(j)
             ENDIF
         ELSE
             depth = scrbuf(i-1)
             time = st + sit * FLOAT(i-2)
             j = j + 2
             GOTO 10
         ENDIF
!          print *,' i=',i,' depth=',scrbuf(i),' j=',j,' vtp=',vtp(j),
!     *       ' time=',time,' t=',t
  100 CONTINUE
!
!    FIND THE CLOSEST DEPTH IN THE CONVERTED DEPTH ARRAY FOR EVERY
!    OUTPUT DEPTH SAMPLE.  THE OUTPUT DEPTH ARRAY IS A UNIFORMLY SAMPLED
!    ARRAY BUT THE CONVERTED (TIME) ARRAY IS NOT.  USE WHATEVER CONVERTED
!    ARRAY POINT THAT IS CLOSEST, THUS ALLOWING THE SAME POINT TO APPEAR
!    IN THE OUTPUT ARRAY OR SKIPPING SOME.
!
      INDXT=1                                                           ! THE INDEX TO THE CONVERTED TIME ARRAY
      INDXD=0                                                           ! THE INDEX TO THE OUTPUT DEPTH ARRAY
!
  110 INDXD=INDXD+1
      IF(INDXD.GT.MSAMPS) RETURN
      DEPTH=SD+(INDXD-1)*SID                                            ! CALCULATE THE NEXT DEPTH VALUE
  120 IF( DEPTH .LT. SCRBUF(INDXT) ) THEN
          IF( DEPTH .LT. SCRBUF(INDXT-1) ) THEN
!****         the depth wanted is before the one we're at, so back up
              INDXT=INDXT-1
              IF(INDXT.GT.0) GO TO 120
              BUFOUT(INDXD)=0.                                                  ! THE OUTPUT IS BEFORE THE INPUT!
              INDXT=1
          ELSE
!****         the depth is between this one and the prior one, use the prior one
              BUFOUT(INDXD)=BUFIN(INDXT-1)
          ENDIF
!         print *,' 130 dep=',depth,' indxd=',indxd,' indxt=',indxt,
!     &        ' scr=',scrbuf(indxt)
          GO TO 110
       ENDIF
!****
!**** if the depth is after this one, go to the next one
      IF( DEPTH .EQ. SCRBUF(INDXT) ) THEN
!***      the depth is the same as this one, use it!
          BUFOUT(INDXD)=BUFIN(INDXT)
!          print *,' 200 dep=',depth,' indxd=',indxd,' indxt=',indxt,
!     &        ' scr=',scrbuf(indxt),' buf=',bufout(indxd)
          GO TO 110
      ENDIF
!****
!****  set up to try the next time sample
  300 INDXT=INDXT+1
      IF(INDXT.LE.NSAMPS) GO TO 120
      DO I=INDXD,MSAMPS
  310    BUFOUT(I)=0.
      ENDDO
      RETURN
      END
