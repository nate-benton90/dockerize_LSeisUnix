      SUBROUTINE T2DED(SCR,LSCR)
      
!                             PROCESS T2D  (TIME TO DEPTH CONVERSION)
!                             ------- ---
!
!  DOCUMENT DATE: 19 November 1990
!
!     T2D TRANSFORMS TIME DOMAIN TRACES TO DEPTH DOMAIN TRACES USING USER
!  DEFINED VELOCITY FUNCTIONS.  THE VELOCITIES MAY BE VARIED TEMPORALLY AND
!  SPATIALLY.  THE VELOCITIES MAY BE AVERAGE VELOCITIES OR INTERVAL VELOCITIES.
!  INTERVAL VELOCITIES ARE CONVERTED TO AVERAGE VELOCITIES.  DEPTH 
!  CONVERSION UTILIZES THE FORMULA     D=T/2*V
!  INTERVAL VELOCITY DEPTH CONVERSION UTILIZES THE FORMULA
!  D(N+1)=(T(N+1)-T(N))/2*V(N+1).
!     THE VELOCITY FUNCTION MAY BE VARIED SPATIALLY BETWEEN VELOCITY CONTROL
!  POINTS.  VELOCITY CONTROL POINTS ARE DEFINED AS EITHER SHOT POINT NUMBERS
!  OR RP NUMBERS. EACH PARAMETER LIST HAS A START AND END CONTROL POINT WITH THE
!  PARAMETERS BEING CONSTANT FOR ALL POINTS BETWEEN THE FIRST AND LAST CONTROL
!  POINT OF THE LIST.  E.G.  IF THE FIRST CONTROL POINT=100 AND THE LAST CONTROL
!  POINT=110, THE POINTS 101 TO 109 WILL ALSO HAVE THE SAME PARAMETERS APPLIED.
!                 E.G. FNO 1 VTP 2100 1.1 3100 2.1 END
!                      FNO 3 VTP 2300 1.3 3300 2.3 END
!                      FNO 4 VTP 2200 1.0 2500 1.5 3000 2.0B END
!      RESULTS IN FNO 2 VTP 2200 1.2 3200 2.2
!      THE AVERAGE VELOCITY FUNCTION IS EVALUATED FOR EVERY TIME SAMPLE FOR THE
!  CONTROL POINTS.  TRACES BETWEEN VELOCITY CONTROL POINTS RECEIVE A VELOCITY
!  FUNCTION THAT IS THE LINEAR INTERPOLATION OF THE VELOCITIES OF THE
!  CORRESPONDING TIMES ON THE ADJACENT CONTROL POINTS.
!     THE OUTPUT SAMPLE INTERVAL IS CHANGED BY PROCESS T2D TO 1 MIL (.001 SECOND).
!  THUS, IN ORDER TO PLOT DEPTH DATA, USE PLOT PARAMETERS FOR 1 MIL TIME DATA.
!     SPATIAL VARIATION IS BY SHOT IF THE DATA IS SORTED BY SHOT AND IS
!  VARIED BY RP IF THE DATA IS SORTED BY RP.
!     The output units of T2D are meters.  Whenever the SEGY header
!  uses milliseconds, T2D converts it to meters.  Whenever the SEGY header
!  uses seconds, T2D converts it to kilometers.
!     EACH PARAMETER LIST MUST BE TERMINATED WITH THE WORD END.  THE ENTIRE SET
!  OF T2D PARAMETERS MUST BE TERMINATED BY THE WORD END.
!
!    EXAMPLE:
!       T2D
!            SDEPTH 0 EDEPTH 20000 OSI 10 VTYPE AVE
!           FNO 1 VTP 1500 2.2 2200 4.5 3500 6.7 4500 7.0 END
!           FNO 223 VTP 1500 2.9 2100 4.2 3500 6.6 4500 7.0 END
!           FNO 253 VTP 1500 3.0 2100 4.9 3500 6.5 4500 7.0 END
!       END
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  FNO    - THE FIRST SHOT (OR RP) TO APPLY THE VELOCITIES TO.  SHOT (RP) NUMBERS
!           MUST INCREASE MONOTONICALLY.
!           PRESET=1
!  LNO    - THE LAST SHOT (RP) NUMBER TO APPLY THE VELOCITIES TO.  LNO MUST BE
!           LARGER THAN FNO IN EACH LIST AND MUST INCREASE LIST TO LIST.
!           DEFAULT=FNO
!  VTYPE  - THE TYPE OF VELOCITY FUNCTION GIVEN IN THE VTP PARAMETER.
!         = AVE,  AVERAGE VELOCITIES.
!         = INT,  INTERVAL VELOCITIES.
!           PRESET = INT.  E.G. VTYPE AVE
!  VTP    - VELOCITY-TIME-PAIRS.  A LIST OF VELOCITY AND TWO-WAY TRAVE TIME
!           (IN SECONDS) PAIRS.  VTP MUST BE GIVEN IN EACH T2D PARAMETER
!           LIST.  A MAXIMUM OF 25 PAIRS MAY BE GIVEN.  DATA TIMES BEFORE
!           THE FIRST GIVEN TIME IN VTP WILL RECEIVE THE FIRST GIVEN VELOCITY.
!           LIKEWISE, DATA TIMES EXCEEDING THE LAST GIVEN TIME IN VTP WILL
!           RECEIVE THE LAST VELOCITY GIVEN IN VTP.
!           DEFAULT=NONE. E.G. VTP 1490 1.0 2000 2.0
!  SDEPTH - THE START DEPTH OF THE OUTPUT DEPTH SECTION.  When this is,
!           the SEGY header is changed so that the deep water delay = SDEPTH
!           PRESET = 0.  E.G. SDEPTH 1000.
!  EDEPTH - THE END DEPTH OF THE OUTPUT DEPTH SECTION.  ALL OUTPUT TRACES WILL
!           HAVE THE SAME END DEPTH.
!           PRESET = 10000.  E.G. EDEPTH 8000
!  OSI    - THE OUTPUT SAMPLE INTERVAL EXPRESSED AS THE DISTANCE BETWEEN SAMPLES.
!           The output sample interval should be similar to time sample 
!           intervals.  e.g.  osi 4 gets written in the SEGY as 4 meters
!           whereas in time it would be 4 mils.
!           PRESET = 5. (METERS).  E.G. OSI 10.
!  ADDWB  - WHEN GIVEN A VALUE OF YES, THE WATER BOTTOM TIME IS ADDED TO THE
!           VELOCITY FUNCTION AFTER SPATIAL VARIATION HAS BEEN DONE.
!           PRESET=NO
!  END    - TERMINATES EACH PARAMETER LIST.
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, JULY 1983
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!  mod 12 oct 89 to convert interval vtps to average vtps.
!  mod 10 mar 90 by pch to to the above on interval velocities ONLY!
!  mod 6 Aug 97 - Add check for OSI > 32 because OSI*1000 must fit in a
!       SEG-Y 16 bit header word.
!  mod 6 Oct 06 - Change sdepth & edepth preset to -1, so each they can change
!                 on every trace like delay and nsamps do
!  mod 6 Oct 06 - Change OSI preset to 1 (like as in 1m, or 1mil)
!
!  ARGUMENTS:
!  SCR    - A SCRATCH ARRAY AT LEAST 60 32BIT WORDS LONG.
!  LSCR   - THE SAME ARRAY BUT USED FOR LONG INTEGERS.  PRIME DOESN'T ALLOW
!           EQUIVALENCING OF ARGUMENTS.
!
!  DISC PARAMETER LIST ORDER:
!  WORD 1)  FNO      (ALL ENTRIES ARE 32 BITS LONG)
!       2   LNO
!       3)  OSI
!       4)  SDEPTH
!       5)  EDEPTH
!       6)  ADDWB
!       7)  LPRINT
!       8)  NVTPS  - THE NUMBER OF ENTRIES IN THE VTP GIVEN BY THE USER
!       9)  ODELAY - THE DELAY OF THE OUTPUT
!       10)  VTYPE
!       11-60) VTP  - ALWAYS 50 LONG
!
!
!
      PARAMETER (NPARS=10)                                              !* THE NUMBER OF USER PARAMETERS
      PARAMETER (MAXVTP=50)                                             ! THE MAXIMUM NUMBER OF VTPS THAT T2DEX CAN HANDLE
      PARAMETER (NWRDS=MAXVTP+NPARS)                                    ! THE NUMBER OF WORDS IN EACH DISK OPARAMETER LIST
      PARAMETER (MULTIV=10)                                             ! THE INDEX OF THE FIRST MULTIVALUED PARAMETER
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /T2DCOM/ MUNIT,NLISTS
      COMMON /READT/ILUN,NUMHDR,NUMDAT,IUNHDR,IREELM,INTRCS,IFMT,NSKIP,
     *   SECS,LRENUM,ISRCF,IDTYPE
      DIMENSION SCR(111),LSCR(111)
      INTEGER FNO,SDEPTH,EDEPTH,VTYPE
!
!
      EQUIVALENCE (FNO,LVALS(1)),
     2            (LNO,LVALS(2)),
     3            (ADDWB,LVALS(3)),
     4            (LPRINT,LVALS(4)),
     5            (OSI,VALS(5)),
     6            (SDEPTH,LVALS(6)),
     7            (EDEPTH,LVALS(7)),
     9            (VTYPE,LVALS(9)),
     *            (VTP,VALS(10))
      DATA NAMES/'FNO   ',
     2           'LNO   ',
     3           'ADDWB ',
     4           'LPRINT',
     5           'OSI   ',
     6           'SDEPTH',
     7           'EDEPTH',
     8           'ODELAY',
     9           'VTYPE ',
     *           'VTP   '/
      DATA LENGTH/3,3,5,6,3,6,6,6,5,3/
      DATA TYPES/'L','L','A','L','F','L','L','F','A','F'/
      DATA NVTPS/0/, NVTPS1/-1/
!****
!****      SET THE PRESETS
!****
      FNO=1
      LNO=9999999
      VTP=-1.
      OSI=1.                                                            ! 5. METERS BETWEEN SAMPLES
      SDEPTH=-1.
      EDEPTH=-1
      VTYPE=2                                                           ! 1= INTERVAL, 2= AVERAGE
      LPRINT=0
      IADDWB=0
      LLNO = 0
      NLISTS=0
      NS=0
!****
!****   GET A PARAMETER FILE
!****
      CALL GETFIL(1,MUNIT,TOKEN,ISTAT)
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
      IF(NS.NE.0) GO TO 230
      PRINT 191, TOKEN(1:NCHARS)
  191 FORMAT(' ***  ERROR  *** T2D DOES NOT HAVE A PARAMETER ',
     *  'NAMED ',A10)
      IERROR=IERROR+1
      GO TO 100
!****
!****    FOUND THE PARAMETER NAME, NOW FIND THE VALUE
!****
  200 CONTINUE
      NS=0
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
      IF(NAMES(NPARAM).EQ.'VTYPE'.AND.TOKEN(1:NCHARS).EQ.'AVE') VTYPE=2
      GOTO 100
  240 CONTINUE
      CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                              ! TRY AND DECODE IT
      IF(ISTAT.EQ.2) GO TO 420                                          ! =2 MEANS IT IS A NUMERIC
      IERROR=IERROR+1                                                   ! DCODE PRINTED AN ERROR
      GO TO 100
  420 IF(TYPES(NPARAM).EQ.'L') GO TO 500
      IF(NPARAM.LT.MULTIV) GO TO 490                                    !  IS IT A MULTIVALUED PARAMETER
      NS=NS+1                                                           !  THE TOKEN WAS A MULTI-VALUED PARAMETER
      nvtps = ns
      ITEMP=MULTIV
      SCR(NS+ITEMP)=AREAL
      GO TO 100
  490 VALS(NPARAM)=AREAL                                                !  FLOATING POINT VALUES
      GO TO 100
  500 CONTINUE                                                          !  32 BIT INTEGER VALUES
      LVALS(NPARAM)=AREAL
      GO TO 100
!****
!****   FINISHED A LIST, NOW DO THE ERROR AND VALIDITY CHECKS
!****
 1000 CONTINUE                                                          ! MAKE SURE ALL SHOT & RP NUMBERS INCREASE
      IF(LNO.EQ.9999999) LNO=FNO                                        ! DEFAULT LNO TO FNO
      IF(FNO.GT.LLNO) GO TO 1020                                        !  IS FNO LARGER THAN THE LAST LNO
      PRINT 1010
 1010 FORMAT(' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.')
      IERROR=IERROR+1
 1020 IF(LNO.GE.FNO) GO TO 1030                                         ! DO THEY INCREASE IN THIS LIST
      PRINT 1010
      IERROR=IERROR+1
 1030 IF( nvtps .EQ. 0 ) THEN
         PRINT *,' ***  ERROR  ***  VTP NOT GIVEN.'
         IERROR=IERROR+1
      ENDIF
! 1070 IF( SDEPTH .GE.-1.) GO TO 1080
!      PRINT 1075
! 1075 FORMAT(' ***  ERROR  ***  SDEPTH MUST BE POSITIVE.')
!      IERROR=IERROR+1
! 1080 IF(EDEPTH.GT.0.) GO TO 1090
!      PRINT 1085
! 1085 FORMAT(' ***  ERROR  ***  EDEPTH IS REQUIRED.')
!      IERROR=IERROR+1
! 1090 IF((EDEPTH-SDEPTH)/OSI.GT.100.) GO TO 1100
!      PRINT 1095
! 1095 FORMAT(' ***  WARNING  ***  UNUSUALLY SHORT OUTPUT REQUESTED.')
!      IWARN=IWARN+1
 1100 CONTINUE
      IF(OSI.GT.0.) GO TO 1110
      PRINT 1105,OSI
 1105 FORMAT(' ***  ERROR  ***  INVALID VALUE OF ',F10.5,' FOR OSI.')
      IERROR=IERROR+1
 1110 CONTINUE
      IF( osi .GT. 32 ) THEN
          PRINT *,' ***  ERROR  ***  OSI must be less than 32.'
          ierror = ierror + 1
      ENDIF
      DO 1200 II=1,NVTPS,2                                              !  CHECK THE VTP FOR ERRORS
      I=II+NPARS
      SCR(I)=SCR(I)/2.                                                  ! ****  CONVERT TWO WAY TRAVE TIMES TO ONE WAY!!!!!
      IF(SCR(I).GT.0) GO TO 1160
      PRINT 1150,SCR(I)
 1150 FORMAT(' ***  ERROR  ***  ILLEGAL VTP VELOCITY OF ',F10.4)
      IERROR=IERROR+1
 1160 J=I+1
      IF(SCR(J).GE.0.AND.SCR(J).LT.20.) GO TO 1180
      PRINT 1170,SCR(J)
 1170 FORMAT(' ***  ERROR  ***  ILLEGAL VTP TIME OF ',F10.4)
      IERROR=IERROR+1
 1180 IF(II.EQ.1) GO TO 1200
      IF(SCR(J).GT.SCR(J-2)) GO TO 1200
      PRINT 1190,SCR(J)
 1190 FORMAT(' ***  ERROR  ***  THE VTP TIME OF ',F10.4,' DECREASED.')
      IERROR=IERROR+1
 1200 CONTINUE
      NVTPS1=NVTPS
      LLNO=LNO
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      IF(NVTPS.LE.MAXVTP) GO TO 1360
      ITEMP=MAXVTP/2
      PRINT 1350,ITEMP
 1350 FORMAT(' ***  ERROR  ***  T2D CAN HANDLE ONLY ',I3,' VTPS.')
      IERROR=IERROR+1
 1360 CONTINUE
      LSCR(1)=FNO
      LSCR(2)=LNO
      SCR(3)=OSI
      LSCR(4)=SDEPTH
      LSCR(5)=EDEPTH
      LSCR(6)=IADDWB
      LSCR(7)=LPRINT
      LSCR(8)=NVTPS
      LSCR(10)=VTYPE
      ITEMP=NPARS+1
      ITEMP1=NPARS+NVTPS
      IF(IAND(LPRINT,1).EQ.1)  PRINT *,(LSCR(I),I=1,2),
     *   SCR(3),(LSCR(J),J=4,8),LSCR(10),(SCR(J),J=ITEMP,ITEMP1)
      CALL WRDISC(MUNIT,SCR,NWRDS)
      NLISTS=NLISTS+1
      LLNO=LNO
      LNO=9999999                                                       ! DEFAULT THE DEFAULTS
      NS=0                                                              ! SET THE NUMBER OF MULTI-VALUED PARAMETER ENTRIES BACK TO ZER0
      VTP=-1.
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
