      SUBROUTINE flated( scr, lscr )
!                            PROCESS FLATEN
!                            ------- ------
!
!  DOCUMENT DATE: 20 February 1987
!
!      Process FLATEN flattens the seismic line to user given time.  Each
!  trace is shifted from the water bottom (depth or time) to the user given
!  output time.  Any SEGY trace header word may be used as the water bottom
!  time.  Water bottom depths may be converted to time by giving a velocity.
!      The original concept of flatten started with the SIO Sea Beam center
!  depth being used as the depth.  That depth is the depth directly under the
!  center of the ship (SEGY header word ihdr(16), not at all what seismic
!  really sees.  Next, the Sea Beam closest beam depth was put into SEGY header
!  word ihdr(107).  Finally, PROCESS WBT was modified to not only use the
!  closest Sea Beam depth, but to look forward and aft for the shallowest depth.
!       The SIO single channel system started recording the SeaBeam depths
!  in spring 1987 (Crossgrain 1).
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  OTIME  - The time, in seconds, of the of the water bottom after process
!           FLATEN. 
!           REQUIRED.    e.g. otime 5.6 
!  VEL    - The velocity of the water column used to convert the water depth
!           to the water bottom time.  A zero velocity indicates that the header
!           word is a time.  time = header / vel.  SIO Sea Beam uses 1500 m/s
!           preset = 0   e.g. vel 1500
!  HDR    - The index of the water bottom depth/time within the REAL SEGY
!           header.  PROCESS WBT puts the water bottom time in hdr(50).
!           preset = 50
!  IHDR   - The index of the water bottom within the 16 bit SEGY trace header.
!           Use only if the water bottom depth/time is not in word 16.
!           preset = 0     e.g.   ihdr 66
!  LHDR   - The index of the water bottom within the 32 bit SEGY trace header.
!           Use only if the water bottom depth/time is not in word 16.
!           preset = 0    e.g.   lhdr 63
!  NAVE   - The number of trace depths to average across.  The depth for a given
!           trace will be the average the current trace and the previous NAVE-1
!           traces.
!           preset = 1    e.g.   nave 5
!  FNO    - The first shot (or RP) to FLATEN.  Shot (RP) numbers must increaseC c           monotonically.
!  LNO    - The last shot (RP) to FLATTEN.  LNO must be larger than FNO in each
!           successive parameter list. 
!  END    - TERMINATES EACH PARAMETER LIST.
!
!  COPYRIGHTED (C) AND WRITTEN BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, February 1987
!  ALL RIGHTS RESERVED.
!
!  THE PARAMETER LIST ON DISC IS:
!  WORD 1)  otime
!       2)  vel
!       3)  ihdr
!       4)  lhdr
!       5)  lprint
!       6)  FNO
!       7)  lno
!       8)  hdr
!
!
!  ARGUMENTS:
!  SCR    - A SCRATCH ARRAY AT LEAST 60 32 BIT WORDS LONG.
!  LSCR   - THE SAME ARRAY BUT THE 32 BIT INTEGER EQUIVALENT.  NEEDED
!           BECAUSE PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING OF ARGUMENTS.
!
!
      PARAMETER (NPARS = 9)                                              ! THE NUMBER OF USER PARAMETERS
      PARAMETER ( nwrds = npars )                                        ! THE NUMBER OF WORDS IN EACH PARAMETER LIST
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPE(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /readt/ lun,numhdr
      EQUIVALENCE (VALS(1),LVALS(1))
      COMMON /flaten/ MUNIT,NLISTS
      DIMENSION SCR(111),LSCR(111)
      INTEGER FNO,hdr
!
!
      EQUIVALENCE (otime,VALS(1)),
     2            (vel,VALS(2)),
     3            (ihdr,LVALS(3)),
     4            (lhdr,lvals(4)),
     5            (fno,lvals(5)),
     6            (lno,lvals(6)),
     7            (LPRINT,LVALS(7)),
     8            (nave,lvals(8)),
     9            (hdr,lvals(9))
      DATA NAMES/'OTIME ','VEL   ','IHDR  ','LHDR  ','FNO   ',
     *           'LNO   ','LPRINT','NAVE  ','HDR   '/
      DATA LENGTH/5,3,4,4,3,3,6,4,3/
      DATA TYPE/'F','F','L','L','L','L','L','L','F'/
!****
!****      SET THE PRESETS
!****
      otime = -.1
      vel = 0.
      ihdr = 0
      lhdr = 0
      FNO=1
      LNO=32768
      LPRINT=0
      nave = 1
      hdr = 50
!****
!****    GET A PARAMETER FILE
!****
      CALL GETFIL(1,MUNIT,TOKEN,ISTAT)
!****
!****   THE CURRENT COMMAND LINE IN THE SYSTEM SCRFER MAY HAVE THE PARAMETERS.
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
  191 FORMAT(' ***  ERROR  *** FLATEN DOES NOT HAVE A PARAMETER ',
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
 1100 CONTINUE                                                          ! CHECK SET FOR VALIDITY
      IF( otime .EQ. -.1 ) THEN
          PRINT *,' ***  ERROR  ***  OTIME must be given.'
          ierror = ierror + 1
          otime = 0.
      ENDIF
      IF( otime .LT. 0. .OR. otime .GT. 50. ) THEN
          PRINT *,' ***  ERROR  ***  Illegal otime of',otime
          ierror = ierror + 1
      ENDIF
      IF( ihdr .LT. 0. .OR. ihdr .GT. numhdr*2 ) THEN
          PRINT *,' ***  ERROR  ***  Illegal IHDR of ',ihdr
          ierror = ierror + 1
      ENDIF
      IF( lhdr .LT. 0 .OR. lhdr .GT. numhdr ) THEN
          PRINT *,' ***  ERROR  ***  Illegal LHDR of ',lhdr
          ierror = ierror + 1
      ENDIF
      IF( vel .LT. 0. .OR. vel .GT. 20000. ) THEN
          PRINT *,' ***  ERROR  ***  Illegal VEL of',vel
          ierror = ierror + 1
      ENDIF
      IF( nave .LE. 0 .OR. nave .GT. 100 ) THEN
          PRINT *,' ***  ERROR  ***  NAVE must be between 1 and 99.'
          ierror = ierror + 1
      ENDIF
      IF( hdr .LT. 0 .OR. hdr .GT. numhdr ) THEN
          PRINT *,' ***  ERROR  ***  Illegal HDR value of ',hdr
          ierror = ierror + 1
      ENDIF
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      scr(1) = otime
      scr(2) = vel
      lscr(3) = ihdr
      lscr(4) = lhdr
      lscr(5) = lprint
      lscr(6) = fno
      lscr(7) = lno
      lscr(8) = nave
      IF( ihdr+lhdr .NE. 0 .AND. hdr .EQ. 50 ) hdr = 0
      lscr(9) = hdr
      IF( IAND(LPRINT,1) .EQ.1 )  THEN
          PRINT *,' FLATEN parameters:'
          PRINT *,otime,vel,ihdr,lhdr,fno,lno,lprint,nave,hdr
      ENDIF
      CALL WRDISC(MUNIT,scr,NWRDS)
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
      RETURN
      END
