      SUBROUTINE AGCED(BUF,LBUF)
!                            PROCESS AGC (AUTOMATIC GAIN CONTROL)
!                            ------- ---
!
!  DOCUMENT DATE: 13 April 1991
!
!     PROCESS AGC APPLIES AGC (AUTOMATIC GAIN CONTROL) TO EVERY TRACE.  AGC
!  IS A TYPE OF AMPLITUDE NORMALIZATION (MODIFICATION) THAT RESULTS IN THE
!  AMPLITUDES BEING MORE UNIFORM, ESPECIALLY WHEN THE WINDOW LENGTH DECREASES.
!  AGC STARTS BY FINDING THE FIRST NONZERO SAMPLE AND THEN CALCULATES THE
!  AVERAGE ABSOLUTE VALUE OF THE WINDOW.  SUCCESSIVE WINDOWS ARE CALCULATED
!  BY SHIFTING THE WINDOW DOWN ONE SAMPLE.  EACH AVERAGE ABSOLUTE VALUE IS
!  THEN TURNED INTO A MULTIPLIER BY DIVIDING THE AVERAGE BY AN OUTPUT LEVEL.
!     ALL PARAMETERS THAT REMAIN CONSTANT FOR A SET OF SHOTS (RPS) MAY BE
!  DESCRIBED IN A PARAMETER SET FNO TO LNO.  WINDOWS BETWEEN TWO PARAMETER
!  SETS ARE CALCULATED BY LINEARLY INTERPOLATING BETWEEN LNO OF ONE SET AND FNO
!  OF THE NEXT SET.
!     EACH PARAMETER LIST MUST BE TERMINATED WITH THE WORD END.  THE ENTIRE SET
!  OF AGC PARAMETERS MUST BE TERMINATED BY THE WORD END.
!      A NULL SET OF AGC PARAMETERS MUST BE GIVEN EVEN IF ALL THE PARAMETERS
!  ARE THE PRESETS.  E.G.  AGC
!                               END
!                          END
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  FNO    - THE FIRST SHOT (OR RP) TO APPLY THE AGC TO.  SHOT (RP) NUMBERS
!           MUST INCREASE MONOTONICALLY.
!           PRESET=1
!  LNO    - THE LAST SHOT (RP) NUMBER TO APPLY THE AGC TO.  LNO MUST BE
!           LARGER THAN FNO IN EACH LIST AND MUST INCREASE LIST TO LIST.
!           DEFAULT=FNO
!  WINLEN - THE AGC WINDOW LENGTH IN SECONDS.
!           PRESET=.100
!  PCTAGC - Percent AGC.  The percentage of the computed multiplier to 
!           use in each AGC window.  A PCTAGC < 100."softens" the
!           effect the AGC.
!           PRESET = 100.        e.g.    pctagc 50
!  CENTER - The center point, in seconds,  of the AGC window that 
!           receives the multiplier of the window.  
!           PRESET = winlen / 2
!  END    - TERMINATES EACH PARAMETER LIST.
!
!  COPYRIGHTED AND WRITTEN BY:
!   PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, JANUARY 1981
!  ALL RIGHTS RESERVED BY THE AUTHOR.
!
!   mods:
!  13 April 1991 by pch - add PCTAGC and CENTER
!
!  THE PARAMETER LIST ON DISC IS:
!  WORD 1)  FNO
!       2   LNO
!       3)  WINLEN
!       4)  LPRINT
!
!
!  ARGUMENTS:
!  BUF    - A SCRATCH ARRAY AT LEAST 60 32 BIT WORDS LONG.
!  LBUF   - THE SAME ARRAY BUT THE 32 BIT INTEGER EQUIVALENT.  NEEDED
!           BECAUSE PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING OF ARGUMENTS.
!
!
      PARAMETER (NPARS=6)                                               ! THE NUMBER OF USER PARAMETERS
      PARAMETER (NWRDS = npars)                                         ! THE NUMBER OF WORDS IN EACH PARAMETER LIST
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPE(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      EQUIVALENCE (VALS(1),LVALS(1))
      COMMON /AGCC/ MUNIT,NLISTS
      DIMENSION BUF(111),LBUF(111)
      INTEGER FNO
!
!
      EQUIVALENCE (FNO,LVALS(1)),
     2            (LNO,LVALS(2)),
     3            (WINLEN,VALS(3)),
     4            (LPRINT,LVALS(4)),
     5            ( pctagc, vals(5) ),
     6            ( center, vals(6) )
      DATA NAMES/ 'FNO   ', 'LNO   ', 'WINLEN', 'LPRINT', 'PCTAGC',
     &            'CENTER' /
      DATA LENGTH/3,3,4*6/
      DATA TYPE/'L','L','F','L',2*'F'/
!****
!****      SET THE PRESETS
!****
      FNO=1
      LNO=32768
      WINLEN=.1
      LPRINT=0
      pctagc = 100.
      center = -1.
!****
!*****    GET A PARAMETER FILE
!****
      CALL GETFIL(1,MUNIT,TOKEN,ISTAT)
!****
!****   THE CURRENT COMMAND LINE IN THE SYSTEM BUFFER MAY HAVE THE PARAMETERS.
!****   GET A PARAMETER LIST FROM THE USER.
!****
      NLISTS=0
      ns=0
      IADDWB=0
      LLNO=0
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
      IF(NS.NE.0) GO TO 230
      PRINT 191, TOKEN(1:NCHARS)
  191 FORMAT(' ***  ERROR  *** AGC DOES NOT HAVE A PARAMETER ',
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
      IF(TYPE(NPARAM).NE.'A') GO TO 240
      IF(NAMES(NPARAM).EQ.'ADDWB'.AND.TOKEN(1:NCHARS).EQ.'YES')
     *    IADDWB=1
      GO TO 100
  240 CONTINUE
      CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                              ! TRY AND DECODE IT
      IF(ISTAT.EQ.2) GO TO 420                                          ! =2 MEANS IT IS A NUMERIC
      IERROR=IERROR+1                                                   ! DCODE PRINTED AN ERROR
      GO TO 100
  420 IF(TYPE(NPARAM).EQ.'L') GO TO 500
  490 VALS(NPARAM)=AREAL                                                !  FLOATING POINT VALUES
      GO TO 100
  500 CONTINUE                                                          !  32 BIT INTEGER VALUES
      LVALS(NPARAM)=AREAL
      GO TO 100
!****
!****  FINISHED A LIST, NOW DO THE ERROR AND VALIDITY CHECKS
!****
 1000 CONTINUE
      
      IF(LNO.EQ.32768) LNO=FNO                                          ! DEFAULT LNO TO FNO
      IF(FNO.GT.LLNO) GO TO 1020                                        !  IS FNO LARGER THAN THE LAST LNO
      PRINT 1010
 1010 FORMAT(' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.')
      IERROR=IERROR+1
 1020 IF(LNO.GE.FNO) GO TO 1030                                         ! DO THEY INCREASE IN THIS LIST
      PRINT 1010
      IERROR=IERROR+1
 1030 LLNO=LNO
 1100 CONTINUE                                                          ! CHECK SET FOR VALIDITY
      IF(WINLEN.GT.0.) GO TO 1210
      PRINT 1205,WINLEN
 1205 FORMAT(' ***  ERROR  ***  ILLEGAL WINDOW LENGTH OF ',F10.3)
      IERROR=IERROR+1
 1210 CONTINUE
      IF(WINLEN.LT.1.100) GO TO 1220
      PRINT 1215,WINLEN
 1215 FORMAT(' ***  WARNING  ***  A WINDOW LENGTH OF ',F10.3,
     *   ' IS UNUSUALLY LARGE.')
      IWARN=IWARN+1
 1220 CONTINUE
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      LBUF(1)=FNO
      LBUF(2)=LNO
      BUF(3)=WINLEN
      LBUF(4)=LPRINT
      buf(5) = pctagc / 100.
      IF( center .GE. 0 ) THEN
          buf(6) = center
      ELSE
          buf(6) = winlen / 2.
      ENDIF
      IF(IAND(LPRINT,1).EQ.1)  PRINT *,(LBUF(I),I=1,2),
     *   BUF(3),LBUF(4), buf(5), buf(6)
      CALL WRDISC(MUNIT,BUF,NWRDS)
      NLISTS=NLISTS+1
      NS=0
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
