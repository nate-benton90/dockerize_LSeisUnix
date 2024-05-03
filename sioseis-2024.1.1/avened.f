      SUBROUTINE AVENED(BUF,LBUF)
!                  PROCESS AVENOR  (AVERAGE AMPLITUDE NORMALIZE)
!                  ------- ------
!
!  DOCUMENT DATE: 27 JANUARY 1983                                            56
!
!     PROCESS AVENOR NORMALIZES EVERY TRACE WINDOW TO THE USER DESCRIBED WINDOW
!  LEVEL.  THE AVERAGE AMPLITUDE WINDOW NORMALIZE CALCULATES AND APPLIES A
!  MULTIPLIER SO THAT THE AVERAGE AMPLITUDE WITHIN THE WINDOW IS AT A CERTAIN
!  LEVEL.  THE RESULTING TRACES WILL BE MORE UNIFORM IN AMPLITUDE.
!     AVENOR FINDS A WINDOW MULTIPLIER BY DIVIDING THE USER'S WINDOW LEVEL BY
!  THE AVERAGE ABSOLUTE VALUE OF THE WINDOW.  THE MULTIPLIER IS HELD CONSTANT
!  FOR ALL DATA BEFORE THE CENTER OF THE FIRST WINDOW, IS LINEARLY INTERPOLATED
!  BETWEEN WINDOW CENTERS AND HELD CONSTANT FOR ALL DATA AFTER THE CENTER OF THE
!  LAST WINDOW.  THUS, DEFINING ONLY ONE WINDOW RESULTS IN A CONSTANT MULTIPLIER
!  FOR EACH TRACE.
!     UP TO 4 WINDOWS MAY BE GIVEN, EACH WITH A DIFFERENT WINDOW LEVEL, AND MAY BE
!  SPATIALLY VARIED BY SHOT OR RP OR BY HANGING THE WINDOWS ON THE WATER BOTTOM.
!     ALL PARAMETERS THAT REMAIN CONSTANT FOR A SET OF SHOTS (RPS) MAY BE
!  DESCRIBED IN A PARAMETER SET FNO TO LNO.  WINDOWS BETWEEN TWO PARAMETER
!  SETS ARE CALCULATED BY LINEARLY INTERPOLATING BETWEEN LNO OF ONE SET AND FNO
!  OF THE NEXT SET.
!     EACH PARAMETER LIST MUST BE TERMINATED WITH THE WORD END.  THE ENTIRE SET
!  OF NORMALIZE PARAMETERS MUST BE TERMINATED BY THE WORD END.
!      A NULL SET OF AVENOR PARAMETERS MUST BE GIVE IF ALL PARAMETERS TO BE
!  USED ARE THE PRESETS.  E.G.    AVENOR
!                                    END
!                                 END
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  FNO    - THE FIRST SHOT (OR RP) TO APPLY THE NORMALIZE TO.  SHOT (RP) NUMBERS
!           MUST INCREASE MONOTONICALLY.
!           PRESET=1
!  LNO    - THE LAST SHOT (RP) NUMBER TO APPLY THE NORMALIZE TO.  LNO MUST BE
!           LARGER THAN FNO IN EACH LIST AND MUST INCREASE LIST TO LIST.
!           DEFAULT=FNO
!  SETS   - START-END TIME PAIRS DEFINING THE WINDOWS.  TIMES ARE IN SECONDS
!           AND MAY BE NEGATIVE WHEN HANGING THE WINDOWS FROM THE WATER BOTTOM.
!           A MAXIMUM OF 4 WINDOWS MAY BE GIVEN.
!           PRESET= DELAY TO LAST TIME.
!  LEVS   - THE AMPLITUDE LEVEL OF EACH WINDOW DESCRIBED BY SETS.  EACH WINDOW
!           MAY HAVE A DIFFERENT LEVEL.  A NEGATIVE LEVEL REVERSES THE POLAARITY.
!           UP TO 4 LEVELS MAY BE GIVEN.
!           PRESET= 10000. 10000. 10000. 10000.
!  END    - TERMINATES EACH PARAMETER LIST.
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, MAY 1980
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  modified 10 nov 89 to remove addwb because it doesn't work right!
!  mod July 1997 - add vel
!  mod 10 Oct 02 - Add parameter hold
!  mod 11 June 09 - LEVS wasn't working
!
!  THE PARAMETER LIST ON DISC IS:
!  WORD 1)  FNO
!       2   LNO
!       3)  ADDWB
!       4)  LPRINT
!       5)  vel
!       6)  hold
!    7-14) SETS
!   15-18) LEVS
!
!
!  ARGUMENTS:
!  BUF    - A SCRATCH ARRAY AT LEAST 60 32 BIT WORDS LONG.
!  LBUF   - THE SAME ARRAY BUT THE 32 BIT INTEGER EQUIVALENT.  NEEDED
!           BECAUSE PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING OF ARGUMENTS.
!
!
      PARAMETER (NPARS=8)                                               ! THE NUMBER OF USER PARAMETERS
      PARAMETER (MULTIV=7)
      PARAMETER (MAX=8)                                                 ! THE MAXIMUM NUMBER OF SETS AVENOR CAN HANDLE
      PARAMETER (NWRDS=18)                                              ! THE NUMBER OF WORDS IN EACH PARAMETER LIST
      EQUIVALENCE (VALS(1),LVALS(1))
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /AVEN/ MUNIT,NLISTS
      DIMENSION BUF(111),LBUF(111)
      INTEGER FNO, hold
      REAL LEVS
!
!
      EQUIVALENCE (FNO,LVALS(1)),
     2            (LNO,LVALS(2)),
     3            (ADDWB,LVALS(3)),
     4            (LPRINT,LVALS(4)),
     5            (vel,vals(5)),
     6            (hold,lvals(6)),
     7            (SETS,VALS(7)),
     8            (LEVS,VALS(8))
      DATA NAMES/'FNO   ','LNO   ','ADDWB ','LPRINT','VEL   ',
     *           'HOLD  ','SETS  ','LEVS  '/
      DATA LENGTH/3,3,5,6,3,4,4,4/
      DATA TYPES/'L','L','A','L','F','L',2*'F'/
!****
!****      SET THE PRESETS
!****
      FNO=1
      LNO=32768
      LPRINT=0
      vel = 0.
      DO I=7,14                                                      ! PRESET SETS BY PUTTING 0. INTO THE DISC FILE VALUES
         BUF(I)=0.                                                   !  A 0. START AND END TIME TO THE EXECUTE MEANS ONLY 1 WINDOW
      ENDDO
      DO I=15,18                                                     ! PRESET LEVS
         BUF(I)=10000.
      ENDDO
      IADDWB=0
      LLNO = 0
      NLISTS=0
      NS=0
      nlevs = 0
      hold = 0
      CALL GETFIL(1,MUNIT,token,ISTAT)
!****
!****   THE CURRENT COMMAND LINE IN THE SYSTEM BUFFER MAY HAVE THE PARAMETERS.
!****   GET A PARAMETER LIST FROM THE USER.
!****
      NTOKES=1
  100 CONTINUE
      CALL GETOKE(TOKEN,NCHARS)                                         ! GET A TOKEN FROM THE USER PARAMETER LINE
      CALL UPCASE(TOKEN,NCHARS)                                         ! CONVERT THE TOKEN TO UPPERCASE
      IF(NCHARS.GT.0) GO TO 150
      IF(NOW.EQ.1) PRINT 140
  140 FORMAT(' <  ENTER PARAMETERS  >')
      CALL RDLINE                                                       ! GET ANOTHER USER PARAMETER LINE
      NTOKES=0
      GO TO 100
  150 CONTINUE
      NTOKES=NTOKES+1
      DO 190 I=1,NPARS                                                  ! SEE IF IT IS A PARAMETER NAME
      LEN=LENGTH(I)                                                     ! GET THE LEGAL PARAMETER NAME LENGTH
      IPARAM=I                                                          ! SAVE THE INDEX
      IF(TOKEN(1:NCHARS).EQ.NAMES(I)(1:LEN).AND.NCHARS.EQ.LEN) GO TO 200
  190 CONTINUE                                                          ! STILL LOOKING FOR THE NAME
      IF(TOKEN(1:NCHARS).EQ.'END'.AND.NCHARS.EQ.3) GO TO 1000           ! END OF PARAM LIST?
      IF(NS+nlevs.NE.0) GO TO 230
      PRINT 191, TOKEN(1:NCHARS)
  191 FORMAT(' ***  ERROR  *** AVENOR DOES NOT HAVE A PARAMETER ',
     *  'NAMED ',A10)
      IERROR=IERROR+1
      GO TO 100
!****
!****    FOUND THE PARAMETER NAME, NOW FIND THE VALUE
!****
  200 CONTINUE
      NPARAM=IPARAM
  210 CONTINUE                                                          !  NOW FIND THE VALUE
      CALL GETOKE(TOKEN,NCHARS)
      CALL UPCASE(TOKEN,NCHARS)
      NTOKES=NTOKES+1
      IF(NCHARS.GT.0) GO TO 230                                         ! END OF LINE?
      IF(NOW.EQ.1) PRINT 140                                            ! THIS ALLOWS A PARAMETER TO BE ON A DIFFERENT LINE FROM THE NAME
      CALL RDLINE                                                       ! GET ANOTHER LINE
      NTOKES=0
      GO TO 210
  230 CONTINUE
      IF(TYPES(NPARAM).NE.'A') GO TO 240
      IF(NAMES(NPARAM).EQ.'ADDWB'.AND.TOKEN(1:NCHARS).EQ.'YES')
     *    IADDWB=1
      GO TO 100
  240 CONTINUE
      CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                              ! TRY AND DECODE IT
      IF(ISTAT.EQ.2) GO TO 420                                          ! =2 MEANS IT IS A NUMERIC
      IERROR=IERROR+1                                                   ! DCODE PRINTED AN ERROR
      GO TO 100
  420 IF(TYPES(NPARAM).EQ.'L') GO TO 500
      IF(NPARAM.LT.MULTIV) GO TO 490                                    !  IS IT A MULTIVALUED PARAMETER
      IF( names(nparam) .EQ. 'SETS' ) THEN
          BUF(ns+7)=AREAL
          ns = ns + 1
          GO TO 100
      ENDIF
      IF( names(nparam) .EQ. 'LEVS' ) THEN
          BUF(nlevs+15)=AREAL
          nlevs = nlevs + 1
          GO TO 100
      ENDIF
  490 VALS(NPARAM)=AREAL                                                !  FLOATING POINT VALUES
      GO TO 100
  500 CONTINUE                                                          !  32 BIT INTEGER VALUES
      LVALS(NPARAM)=AREAL
      GO TO 100
!****
 1000 CONTINUE                                                          ! MAKE SURE ALL SHOT & RP NUMBERS INCREASE
      IF(LNO.EQ.32768) LNO=FNO                                          ! DEFAULT LNO TO FNO
      IF(FNO.GT.LLNO) GO TO 1020                                        !  IS FNO LARGER THAN THE LAST LNO
      PRINT 1010
 1010 FORMAT(' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.')
      IERROR=IERROR+1
 1020 IF(LNO.GE.FNO) GO TO 1030                                         ! DO THEY INCREASE IN THIS LIST
      PRINT 1010
      IERROR=IERROR+1
 1030 LLNO=LNO
! 1100 CONTINUE                                                         ! CHECK THE SETS FOR VALIDITY
      DO 1120 I=7,14                                                    ! ALLOW NEGATIVE TIMES
      IF(ABS(BUF(I)).LT.20.) GO TO 1120
      PRINT 1110,BUF(I)
 1110 FORMAT(' ***  WARNING  *** WINDOW TIME OF ',F10.4,
     *   ' LOOKS WRONG. (NOT FATAL)')
      IWARN=IWARN+1
 1120 CONTINUE                                                          !  PRESET THE LEVS TO 10000.
      DO 1130 I=15,18                                                   ! ALLOW THE USER TO GIVE NEGATIVE LEVS
      IF(BUF(I).EQ.0.) BUF(I)=10000.
 1130 CONTINUE
      IF( hold .LT. 0 .OR. hold .GT. 10000 ) THEN
          PRINT *,' ***  ERROR  ***  HOLD must be > 0 and < 10000.'
          ierror = ierror + 1
      ENDIF
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      IF(NS.LE.MAX) GO TO 1360
      ITEMP=MAX
      PRINT 1350,ITEMP
 1350 FORMAT(' ***  ERROR  ***  AVENOR CAN HANDLE ONLY ',I3,' SETS.')
      IERROR=IERROR+1
 1360 CONTINUE
      LBUF(1)=FNO
      LBUF(2)=LNO
      LBUF(3)=IADDWB
      LBUF(4)=LPRINT
      buf(5) = vel
      lbuf(6) = hold
      IF(IAND(LPRINT,1).EQ.1)  THEN
         PRINT *, ' nwrds=',nwrds,(LBUF(I),I=1,4),buf(5),lbuf(6)
         PRINT *,  (BUF(J),J=7,18)
      ENDIF
      CALL WRDISC(MUNIT,BUF,NWRDS)
      NLISTS=NLISTS+1
      NS=0
      nlevs = 0
      LLNO=LNO
      LNO=32768                                                         ! DEFAULT THE DEFAULTS
 2020 CALL GETOKE(TOKEN,NCHARS)                                         ! GET THE NEXT TOKEN
      CALL UPCASE(TOKEN,NCHARS)
      NTOKES=NTOKES+1
      IF(NCHARS.GT.0) GO TO 2030                                        ! WAS IT THE END OF A LINE?
      IF(NOW.EQ.1) PRINT 140
      CALL RDLINE                                                       ! GET ANOTHER LINE
      NTOKES=0
      GO TO 2020
 2030 IF(TOKEN(1:NCHARS).NE.'END'.OR.NCHARS.NE.3) GO TO 150
      RETURN                                                            !  FINISHED ALL OF THE PARAMETERS!!!
      END
