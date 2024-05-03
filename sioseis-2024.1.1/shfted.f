      SUBROUTINE SHFTED(BUF,LBUF)
!                              PROCESS SHIFT
!                              ------- -----
!
!  DOCUMENT DATE:  23 June 1993
!
!    PROCESS SHIFT APPLIES A TIME SHIFT TO THE SEISMIC TRACES BY THE ENTIRE
!  LINE, OR BY INDIVIDUAL SHOT OR RP, OR BY THE TRACE WITHIN THE SHOT OR RP.
!  ANY COMBINATION MAY BE APPLIED, WITH THE RESULTING TRACE SHIFT BEING THE SUM
!  OF ALL THE SHIFTS FOR THE TRACE.
!    SHIFTS ARE GIVEN IN UNITS OF SECONDS.  A NEGATIVE TIME SHIFT WILL
!  SHIFT THE TRACE TO THE LEFT, OR THE RESULTING TRACE WILL APPEAR EARLIER. E.G.
!  FOR A SAMPLE INTERVAL OF .004, A SHIFT OF  -.1 SECONDS, DATA BETWEEN INPUT
!  TIMES 0.000 AND .096 WILL BE DROPPED, TIME 1.00 BECOMES TIME 0.000, TIME .104
!  WILL BECOME TIME .004, TIME .108 WILL BECOME .008, ETC, AND THE LAST .100 WILL
!  BE ZERO FILLED.
!     LIKEWISE, THE POSITIVE SHIFTED TRACES WILL HAVE THE FRONT OF THE TRACE
!  ZEROED.
!
!     Parameters that are defaulted are reset after being used, whereas 
!  parameters that are preset remain the same until changed by giving the
!  parameter again.
!
!     PROCESS SHIFT will shift only the shots/rps and traces specifically
!  specified, thus the parameters rshift, TSP, XSP, and GSP are defaulted to 0.
!  e.g.     fno 1 rshift .008 tsp 10 .004 end
!           fno 2 tsp 2 -.04 end
!  results in shot 1 trace 10 to be shifted .008+.004 and all other traces in
!  shot 1 to be shifted .008.  The only trace shifted in shot 2 is trace 2,
!  which will be shifted -.04.
!
!     XSP, TSP, and GSP allow a maximum of 1300 pairs to be given.
!
!     EACH PARAMETER LIST MUST BE TERMINATED WITH THE WORD END.  THE ENTIRE SET
!  OF SHIFT PARAMETERS MUST BE TERMINATED BY THE WORD END.
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  LSHIFT - LINE SHIFT. THE AMOUNT OF TIME TO SHIFT EVERY TRACE IN THE SEISMIC
!           LINE OR COMPUTER RUN.
!           PRESET = 0.
!  RSHIFT - RECORD SHIFT.  THE AMOUNT OF TIME TO SHIFT THE SHOT (RP) DEFINED BY
!           FNO - LNO.  EVERY TRACE WITHIN THE SHOT (RP) IS SHIFTED BY RSHIFT.
!           RSHIFT IS RESET TO 0. AFTER LNO IS REACHED.
!           DEFAULT = 0.
!  FNO    - THE FIRST SHOT (OR RP) TO APPLY RSHIFT AND/OR XSP/TSP TO.  SHOT (RP)
!           NUMBERS MUST INCREASE MONOTONICALLY.
!           PRESET = NONE
!  LNO    - THE LAST SHOT (RP) NUMBER TO APPLY AND/OR XSP/TSP TO.  LNO MUST BE
!           LARGER THAN FNO IN EACH LIST AND MUST INCREASE LIST TO LIST.
!           DEFAULT = FNO
!  XSP    - RANGE-SHIFT-PAIRS.  A LIST OF RANGE AND SHIFT PAIRS.
!           XSP MUST BE GIVEN WITH INCREASING RANGES.  THE PROGRAM
!           COMPUTES THE ABSOLUTE VALUE OF BOTH USER RANGES AND DATA RANGES.
!           E.G. XSP 1000 3.0 2000 -1.1  - TRACES WITH RANGES EXACTLY EQUAL TO
!           1000. WILL BE SHIFTED BY 3. SECONDS, AND TRACES WITH A RANGE OF
!           EXACTLY 2000. WILL BE SHIFTED BY -1.1 SECONDS.  ALL OTHER TRACES
!           WILL NOT BE SHIFTED UNLESS LSHIFT OR RSHIFT WAS GIVEN.
!           DEFAULT = ALL 0.
!  TSP    - TRACE NUMBER-SHIFT-PAIRS.  A LIST OF TRACE NUMBERS (OF A SHOT OR RP)
!           AND SHIFTS (LISTED IN PAIRS).  ONLY THOSE TRACES SPECIFIED WILL BE
!           SHIFTED.  TRACE NUMBERS MUST INCREASE WITHIN EACH LIST.
!           E.G. TSP 4 -1. 20 .5 INDICATES THAT TRACE 4 WILL BE SHIFTED BY -1.000
!           SECONDS AND TRACE TWENTY WILL BE SHIFTED BY .5 SECONDS.
!           DEFAULT = ALL 0.
!  GSP    - GROUP-SHIFT-PAIRS.  A LIST OF GROUP NUMBERS AND TIMES SHIFTS.  GROUP
!           NUMBERS ARE THE SAME AS TRACE NUMBERS.  GROUPS NOT SPECIFIED RECEIVE
!           NUMBERS MUST BE STRICTLY INCREASING.
!           DEFAULT = ALL 0
!  APPVEL - Apparent velocity.  Used to calculate the angle by which shots 
!           are projected onto the sea floor (i.e. sin(angle) = VELH2O /appvel).
!           Time shifts are calculated using:
!              TIMECORR = WATERDEPTH / (VELH2O*COS(ANGLE))
!           A shift for range is also applied using:
!              RANGECORR = WATERDEPTH * TAN(ANGLE)
!           THE RANGE IN THE SEGY TRACE HEADER IS ADJUSTED!
!           Shifts from APPVEL are performed prior to the reduction velocity
!           time shift determined by parameter REDVEL.
!           PRESET = 0. (No correction)    e.g.   appvel 9000
!  VELH2O - Velocity of water.  Used to calculate the time shift for 
!           projecting onto the sea floor (parameter APPVEL).
!           PRESET = 0.  (No correction) 
!  REDVEL - Reduction velocity.  Shifts every trace in time according to:
!             SHIFT =  RANGE / REDVEL
!           PRESET = 0. (No shift)    e.g. redvel 8000
!  REDUCE - Reduce the amount of data in the trace by changing the SEGY
!           header values of the delay and the number of samples as well
!           as shifting the data.  Data reduction will take place when 
!           REDUCE is give a value of YES.
!           PRESET = no     e.g.   reduce yes
!  LAGA   - When set to "YES", each trace will be shifted by the negative
!           of the amount contained in the SEGY trace header bytes 105-106,
!           the lag time A in milliseconds.  The lag time is defined to be:
!           "the time in ms. between (the) end of (the) 240-byte trace
!           identification header and (the) time break.  Positive if (the)
!           time break occurs after (the) end of (the) header, negative
!           if (the) time break occurs before (the) end of (the) header.
!           (The) time break is defined as the initiation pulse ......"
!           PRESET = 0              e.g.   laga yes
!  LAGB   - When set to "YES", each trace will be shifted by the negative
!           of the amount contained in the SEGY trace header bytes 107-108,
!           the lag time B in milliseconds. 
!           PRESET = 0              e.g.   lagb yes
!  END    - TERMINATES EACH PARAMETER LIST.
!
!    XSP, TSP AND GSP ARE MUTUALLY EXCLUSIVE, ONLY ONE MAY BE GIVEN IN A
!  PARAMETER LIST.
!
!  COPYRIGHT (C) 1980, The Regents of the University of California
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, DECEMBER 1980
!  ALL RIGHTS ARE RESERVED.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  modified by pch Nov. 1990 for the OSU 1988 additions of redvel, appvel,
!    and appvel
!   modified by pch 18 Aug 1991 for REDUCE
!  mod 6 June 93 - allow fno of 0
!  mod 23 June 93 - add lag
!  mod 23 Oct 96 - Add parameters DATUME and DATUMV
!  mod 28 Jan. 97 - Add parameter INTERP
!  mod 4 Aug. 97 - Add parameter INDICES
!  mod 26 June 01 - Check for spatial variation of "tsp" lists of different length.
!                 - interp was preset on, doc says off
!  mod 13 Jul 01 - OOPS, set interp to be on EXCEPT when "tsp" or rshift
!                  are given.
!  mod 6 Nov. 01 - parameter indices had to have another parameter given, else error.
!  mod 1 Jul 03 - parameter indices was always being reset to 0.
!  mod 14 Jun 06 - gfortran bug on PC Linux - swap order of equivalence of vals
!  mod 22 Sep 10 - interp preset didn't work for rshift - check should be shiftr
!  mod 14 Jun 11 - indices without I/L/R didn't give error.
!
!   THE PARAMETER LIST PASSED TO SHFTEX ON THE DISC LOOKS LIKE:
!    WORD 1)  LSHIFT (FLOATING POINT)
!         2)  RSHIFT (FLOATING POINT)
!         3)  FNO (32 BIT INTEGER)
!         4)  LNO (32 BIT INTEGER)
!         5)  NXSPS (32 BIT INTEGER)  - THE NUMBER OF WORDS IN XSP OR TSP
!         6)  LTYPE - XSP=1, TSP=2, GSP=3
!         7)  LPRINT (32 BIT INTEGER)
!         8)  redvel
!         9)  appvel
!        10)  velh2o
!        11)  reduce
!        12)  laga
!        13)  lagb
!        14)  datume
!        15)  datumv
!        16)  interp
!        18) - MAXXSP+NPARS - XSP OR GSP OR TSP ARRAY
!
!  ARGUMENTS:
!  BUF    - A SCRATCH ARRAY AT LEAST 60 32 BIT WORDS LONG.
!  LBUF   - THE SAME ARRAY BUT THE 32 BIT INTEGER EQUIVALENT.  NEEDED
!           BECAUSE PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING OF ARGUMENTS.
!
!
      PARAMETER (NPARS = 19)                                            ! THE NUMBER OF USER PARAMETERS
      PARAMETER (MAXXSP=2600)                                           ! THE MAXIMUM NUMBER OF TSPS OR XSPS THAT SHFTEX CAN HANDLE
      PARAMETER (MULTIV = 16 )                                          ! THE FIRST MULTI-VALUED PARAMETER
      PARAMETER (NWRDS=MAXXSP+NPARS)
      CHARACTER*7 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      EQUIVALENCE (LVALS(1),VALS(1))
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /SHIFT/ MUNIT,NLISTS
      DIMENSION BUF(11111),LBUF(1111)
      REAL LSHIT
      INTEGER FNO
      INTEGER indices(20)
!
!
      EQUIVALENCE (FNO,LVALS(1)),
     2            (LNO,LVALS(2)),
     3            (LSHIT,VALS(3)),
     4            (SHIFTR,VALS(4)),
     5            (LPRINT,LVALS(5)),
     6            (redvel,vals(6)),
     7            (appvel,vals(7)),
     8            (velh2o,vals(8)),
     9            (reduce,vals(9)),
     *            (laga,lvals(10)),
     1            (lagb,lvals(11)),
     2            (datume,vals(12)),
     3            (datumv,vals(13)),
     4            (interp,lvals(14)),
     5            (smult,vals(15)),
     6            (XSP,VALS(16)),
     7            (TSP,VALS(16)),
     8            (GSP,VALS(16))
      DATA NAMES/ 'FNO   ', 'LNO   ', 'LSHIFT', 'RSHIFT', 'LPRINT',
     *            'REDVEL', 'APPVEL', 'VELH2O', 'REDUCE', 'LAGA   ',
     *            'LAGB  ', 'DATUME', 'DATUMV', 'INTERP', 'SMULT ',
     *            'XSP   ', 'TSP   ', 'GSP   ', 'INDICES'/
      DATA LENGTH /3,3,6,6,6,6,6,6,6,4,4,6,6,6,5,3,3,3,7/
      DATA TYPES /'L','L','F','F','L',3*'F',3*'A',2*'F','A',4*'F','L'/
!****
!****      SET THE PRESETS
!****
      LSHIT=0.
      SHIFTR=0.
      FNO = 1
      LNO = 9999999
      XSP=-1.
      LPRINT=0
      DO I=1,NWRDS
   10    BUF(I)=0.
      ENDDO
      LLNO = 0
      LTYPE=0
      NS=0
      nshfts = 0
      last_nshfts = 0
      redvel = 0.
      appvel = 0.
      velh2o = 0.
      ireduce = 0
      laga = 0
      lagb = 0
      datume = -99999.
      datumv = -99999.
      interp = -1
      smult = 1.
      NLISTS=0
      DO i = 1, 20
         indices(i) = 0
      ENDDO
      nindices = 0
!****
!*****    GET A PARAMETER FILE
!****
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
      IF( NS .NE. 0 .OR. nindices .GT. 0 ) GO TO 230
      PRINT 191, TOKEN(1:NCHARS)
  191 FORMAT(' ***  ERROR  *** SHIFT DOES NOT HAVE A PARAMETER ',
     *  'NAMED ',A10)
      IERROR=IERROR+1
      GO TO 100
!****
!****    FOUND THE PARAMETER NAME, NOW FIND THE VALUE
!****
  200 NS=0
!      nindices = 0
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
      IF( TYPES(NPARAM) .EQ. 'A' ) THEN
          IF(NAMES(NPARAM).EQ.'ADDWB'.AND.TOKEN(1:NCHARS).EQ.'YES')
     *        IADDWB=1
          IF(NAMES(NPARAM).EQ.'REDUCE'.AND.TOKEN(1:NCHARS).EQ.'YES')
     *        ireduce = 1
          IF(NAMES(NPARAM).EQ.'LAGA'.AND.TOKEN(1:NCHARS).EQ.'YES')laga=1
          IF(NAMES(NPARAM).EQ.'LAGB'.AND.TOKEN(1:NCHARS).EQ.'YES')lagb=1
          IF( names(nparam) .EQ. 'INTERP' ) THEN
              IF( token(1:nchars) .EQ. 'ON' ) interp = 1
              IF( token(1:nchars) .EQ. 'OFF' ) interp = 0
              IF( token(1:nchars) .EQ. 'YES' ) interp = 1
              IF( token(1:nchars) .EQ. 'NO' ) interp = 0
              IF( token(1:nchars) .EQ. '1' ) interp = 1
              IF( token(1:nchars) .EQ. '0' ) interp = 0
          ENDIF
          GOTO 100
      ENDIF
      IF( names(nparam) .NE. 'INDICES' ) THEN
          CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                          ! TRY AND DECODE IT
          IF(ISTAT .NE. 2) THEN                                         ! =2 MEANS IT IS A NUMERIC
             IERROR=IERROR+1                                            ! DCODE PRINTED AN ERROR
             GO TO 100
          ENDIF
      ENDIF
      IF(TYPES(NPARAM).EQ.'L') GO TO 500
      IF(NPARAM.LT.MULTIV) GO TO 490                                    !  IS IT A MULTIVALUED PARAMETER
      NS=NS+1                                                           !  THE TOKEN WAS A MULTI-VALUED PARAMETER
      nshfts = ns
      BUF(NS+NPARS)=AREAL
      IF(NPARAM.EQ.MULTIV) LTYPE=1                                      ! =1 MEANS XSP
      IF(NPARAM.EQ.MULTIV+1)  LTYPE=2                                   !  =2 MEANS TSP
      IF( nparam .EQ. multiv+2 ) ltype = 3                              !  =3 means GSP
      GO TO 100
  490 VALS(NPARAM)=AREAL                                                !  FLOATING POINT VALUES
      IF( nparam .EQ. 6 ) ltype = 4                                     ! =4 means redvel
      GO TO 100
  500 CONTINUE                                                          !  32 BIT INTEGER VALUES
      IF( names(nparam) .NE. 'INDICES' ) THEN
          LVALS(NPARAM)=AREAL
          GOTO 100
      ENDIF
      IF( token(1:1) .NE. 'I' .AND. token(1:1) .NE. 'L' .AND.
     &    token(1:1) .NE. 'R' ) THEN
          PRINT *,' ***  ERROR  ***  Indices must start with I, L, or R'
          ierror = ierror + 1
      ENDIF
      nindices = nindices + 1
      IF( token(1:1) .EQ. 'I' ) indices(nindices) = 1
      IF( token(1:1) .EQ. 'L' ) indices(nindices) = 2
      IF( token(1:1) .EQ. 'R' ) indices(nindices) = 3
      nindices = nindices + 1
      token(1:nchars-1) = token(2:nchars)
      token(nchars:nchars) = ' '
      nchars = nchars - 1
      CALL dcode( token, nchars, areal, istat )
      indices(nindices) = NINT(areal)
      GOTO 100
!****
!****   FINISHED A LIST, NOW DO THE ERROR AND VALIDITY CHECKS
!****
 1000 CONTINUE                                                          ! MAKE SURE ALL SHOT & RP NUMBERS INCREASE
       IF( LNO .EQ. 9999999 ) LNO = FNO                                 ! DEFAULT LNO TO FNO
      IF( FNO .LT. LLNO ) THEN                                          !  IS FNO LARGER THAN THE LAST LNO
          PRINT *,' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.'
          IERROR = IERROR + 1
      ENDIF
      IF( LNO .LT. FNO ) THEN                                           ! DO THEY INCREASE IN THIS LIST
          PRINT *,' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.'
          IERROR = IERROR + 1
      ENDIF
      LLNO = LNO
      IF( ltype .EQ. 4 ) THEN
          IF( redvel .LE. 0. ) THEN
              PRINT *,' ***  ERROR  ***  REDVEL must be > 0.'
              ierror = ierror + 1
          ENDIF
          IF( appvel .LT. velh2o ) THEN
              PRINT *,' ***  ERROR  *** APPVEL must be greater than or',
     &                ' equal to VELH2O'
              ierror = ierror + 1
          ENDIF
      ENDIF
      CONTINUE                                                          !  MAKE SURE THE RANGES OF XSP INCREASE
      IF(LTYPE.EQ.2 .OR. ltype .EQ. 3 ) GO TO 1200                      !  1=XSP, 2=TSP, 3=GSP
      IF( last_nshfts .NE. 0 .AND. last_nshfts .NE. nshfts ) THEN
          PRINT *,' ***  ERROR  ***  TSP, XSP, GSP must have the same ',
     &            'number of shifts in each list.'
          ierror = ierror + 1
      ENDIF
      DO I=1,nshfts,2
 1130    BUF(NPARS+I)=ABS(BUF(NPARS+I))                                    ! USE THE ABS VALUE OF THE RANGES
      ENDDO
      IF( nshfts .LE. 2) GO TO 1300
      DO 1150 I=3,nshfts,2
      IF(BUF(NPARS+I).GT.BUF(NPARS+I-2)) GO TO 1150
      PRINT 1140
 1140 FORMAT(' ***  ERROR  ***  THE RANGES OF XSP MUST INCREASE.')
      IERROR=IERROR+1
 1150 CONTINUE
      GO TO 1300
 1200 CONTINUE                                                          !  CHECK THE TSP PARAMETER
      DO 1220 I=1,nshfts,2
      IF(BUF(NPARS+I).GE.0.) GO TO 1220                                  ! ALLOW A TRACE NUMBER OF ZERO
      PRINT 1210
 1210 FORMAT(' ***  ERROR  ***  ILLEGAL TSP or GSP.')
      IERROR=IERROR+1
 1220 CONTINUE
      IF(nshfts.LE.2) GO TO 1300
      DO 1230 I=3,nshfts,2
      IF(BUF(NPARS+I).GT.BUF(NPARS+I-2)) GO TO 1230
      PRINT 1210
      IERROR=IERROR+1
 1230 CONTINUE
 1300 CONTINUE                                                           !  MAKE CHECKS COMMON TO XSP, TSP, and GSP
      IF(MOD(nshfts,2).EQ.0) GO TO 1320
      PRINT 1310
 1310 FORMAT(' ***  ERROR  ***  TSP, XSP, and GSP MUST BE IN PAIRS.')
      IERROR=IERROR+1
 1320 CONTINUE                                                           !  MAKE SURE THE TIMES ARE OK
!**** preset interp to on or yes, EXCEPt when tsp, gsp, gxp, or rshift
!**** are given.
      IF( interp .EQ. -1 ) THEN
          interp = 1
          IF( nshfts .NE. 0 .OR. shiftr .NE. 0 ) interp = 0
      ENDIF
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      IF(nshfts.LE.MAXXSP) GO TO 1360
      ITEMP=MAXXSP / 2
      PRINT 1350,ITEMP
 1350 FORMAT(' ***  ERROR  ***  SHIFT HAS A MAXIMUM OF ',I4,
     *    ' TSP, XSP, or GSP PAIRS.')
      IERROR=IERROR+1
 1360 CONTINUE
      BUF(1)=LSHIT
      BUF(2)=SHIFTR
      LBUF(3)=FNO
      LBUF(4)=LNO
      LBUF(5)=nshfts
      last_nshfts = nshfts
      LBUF(6)=LTYPE
      LBUF(7)=LPRINT
      buf(8) = redvel
      buf(9) = appvel
      buf(10) = velh2o
      lbuf(11) = ireduce
      lbuf(12) = laga
      lbuf(13) = lagb
      buf(14) = datume
      buf(15) = datumv
      lbuf(16) = interp
      lbuf(17) = nindices
      buf(18) = smult
      ITEMP=NPARS+1
      ITEMP1=NPARS+nshfts
      IF( IAND(LPRINT,1) .EQ. 1 ) THEN
          PRINT *,' Shift parameters:'
          PRINT *,BUF(1),BUF(2), (LBUF(I),I=3,7),(BUF(J),J = 8, 10)
          PRINT *, (lbuf(i),i=11,13), (buf(j), j=14,15), lbuf(16)
          PRINT *, (buf(j), j=itemp,itemp1)
      ENDIF
      CALL WRDISC(MUNIT,BUF,itemp1)
      IF( nindices .GT. 0 ) CALL wrdisc( munit, indices, 20 )
      NLISTS=NLISTS+1
      LLNO=LNO
      LNO = 9999999                                                      ! DEFAULT THE DEFAULTS
      nshfts = 0
      shiftr =0.
      nindices = 0
 2020 CALL GETOKE(TOKEN,NCHARS)                                          ! GET THE NEXT TOKEN
      CALL UPCASE(TOKEN,NCHARS)
      NTOKES=NTOKES+1
      IF(NCHARS.GT.0) GO TO 2030                                         ! WAS IT THE END OF A LINE?
      IF(NOW.EQ.1) PRINT 140
      CALL RDLINE                                                        ! GET ANOTHER LINE
      NTOKES=0
      GO TO 2020
 2030 IF(TOKEN(1:NCHARS).NE.'END'.OR.NCHARS.NE.3) GO TO 150
      RETURN                                                             !  FINISHED ALL OF THE PARAMETERS!!!
      END
