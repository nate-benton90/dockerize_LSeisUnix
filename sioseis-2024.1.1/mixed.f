      SUBROUTINE MIXED(BUF,LBUF)
!                              PROCESS MIX
!                              ------- ---
!
!
!    PROCESS MIX PERFORMS A RUNNING, WEIGHTED, DIP MIX.  MIX IS DEFINED TO BE
!  THE SUM OR ADDITION OF ADJACENT (IN A NUMERICAL SENSE). 
!     There are three types of mix: 
! 1) A running roll-along mix - The mix crosses record boundaries.  If the
!    data are shots, the last trace of a shot is mixed with the first trace
!    of the next shot.  e.g. A three trace equally weighted zero dip
!    unweighted running mix will output the following traces:
!          TRACE 1 = INPUT TRACES 1
!          TRACE 2 = INPUT TRACES 1+2
!          TRACE 3 = INPUT TRACES 1+2+3
!          TRACE 4 = INPUT TRACES 2+3+4
!          TRACE 5 = INPUT TRACES 3+4+5
!          ETC.
! 2) A running record mix - The mix stops and the end of a shot and starts
!    over on the next shot.  A three trace equally weighted zero dip running
!    record mix of 24 trace shots will output the following traces:
!          TRACE 1 = INPUT TRACES 1
!          TRACE 2 = INPUT TRACES 1+2
!          TRACE 3 = INPUT TRACES 1+2+3
!          TRACE 4 = INPUT TRACES 2+3+4
!                .     .
!                .     .
!          TRACE 24 = INPUT TRACES 22+23+24
! 3) A record mix - This mix starts over after every mix set and does not
!    output the tappered traces.  e.g. A three trace record mix of 24 trace
!    shots will output the following;
!          TRACE 1 = INPUT TRACES 1+2+3
!          TRACE 2 = INPUT TRACES 4+5+6
!          TRACE 3 = INPUT TRACES 7+8+9
!            .        .
!            .        .
!          TRACE 8 = INPUT TRACES 22+23+24
!
!     A WEIGHTED OR TAPERED MIX ALLOWS EACH TRACE TO BE INDEPENDANTLY SCALED
!  PRIOR TO THE MIX.  REFERING TO THE EARLIER EXAMPLE, A 1 2 1 WEIGHTED MIX
!  WILL HAVE OUTPUT TRACE 3 CONTAINING (TRACE1)*1+(TRACE2)*2+(TRACE3)*1, OUTPUT
!  TRACE 4=(TRACE2)*1.+(TRACE3)*2.+(TRACE4)*1.
!     A DIP MIX IS A MIX WITH EACH TRACE SHIFTED IN TIME PRIOR TO THE MIX.  THE
!  TIME SHIFT IS RELATIVE TO THE FIRST TRACE WITHIN THE MIX SO THAT THE FIRST
!  INPUT TRACE IS NOT SHIFTED, THE SECOND IS SHIFTED BY DIP SECONDS, THE THIRD
!  BY DIP*2 SECONDS.
!     EACH PARAMETER LIST MUST BE TERMINATED WITH THE WORD END.  THE ENTIRE SET
!  OF MIX PARAMETERS MUST BE TERMINATED BY THE WORD END.
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  FNO    - THE FIRST SHOT (OR RP) TO APPLY THE MIX TO.  SHOT (RP) NUMBERS
!           MUST INCREASE MONOTONICALLY.
!           PRESET=1
!  LNO    - THE LAST SHOT (RP) NUMBER TO APPLY THE MIX TO.  LNO MUST BE
!           LARGER THAN FNO IN EACH LIST AND MUST INCREASE LIST TO LIST.
!           DEFAULT = 999999
!  TYPE   - The type of mix to be performed.
!         =1, Running mix.
!         =2, Running record mix.
!         =3, Record mix.
!           Preset = 1                e.g. type 3
!  WEIGHT - A LIST OF WEIGHTS TO USE IN THE MIX.  THE TOTAL NUMBER OF WEIGHTS
!           GIVEN INDICATES THE NUMBER OF TRACES IN THE MIX. E.G. A 3 TRACE
!           EQUALLY WEIGHTED MIX WILL BE DONE BY GIVING WEIGHT 1 1 1.
!           AT LEAST 2 WEIGHTS MUST BE GIVEN AND NO MORE THAN 10 MAY BE GIVEN.
!           REQUIRED.
!  DIP    - THE AMOUNT OF SHIFT, IN SECONDS, TO APPLY TO SUCCESSIVE INPUT TRACES
!           WITHIN EACH MIXED TRACE.
!           PRESET=0.
!  MAXLEN - THE MAXIMUM LENGTH, IN SECONDS, OF THE LARGEST INPUT TRACE
!           (NEEDED FOR ALLOCATING AP MEMORY).
!           PRESET=THE LENGTH OF THE FIRST INPUT TRACE.
!  END    - TERMINATES EACH PARAMETER LIST.
!
!  NOTE *****
!    1) AT LEAST 2, BUT NOT MORE THAN 10, WEIGHTS MUST BE GIVEN.
!    2) NEITHER WEIGHT NOR DIP MAY BE CHANGED WITHIN A JOB.
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 11 MARCH 1981
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!  mods:
!   22 March 1991 by pch to add the mix type parameter
!
!   THE PARAMETER LIST PASSED TO MIXEX ON THE DISC LOOKS LIKE:
!    WORD 1)  FNO (INTEGER*4)
!         2)  LNO (INTEGER*4)
!         3)  DIP (FLOATING POINT)
!         4)  NWEIGS (INTEGER*4) - THE NUMBER OF TRACES IN THE MIX
!         5)  LPRINT (INTEGER*4)
!         6)  MAXLEN (REAL)
!         7)  type (INTEGER)
!         8)  hdr
!         9)  lhdr
!        10)  ihdr
!        11) - MAXMIX+NPARS) - THE MIX WEIGHTS. 
!
!  ARGUMENTS:
!  BUF    - A SCRATCH ARRAY AT LEAST 60 32 BIT WORDS LONG.
!  LBUF   - THE SAME ARRAY BUT THE INTEGER*4 EQUIVALENT.  NEEDED
!           BECAUSE PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING OF ARGUMENTS.
!
!
!
      PARAMETER (NPARS=10)                                              ! THE NUMBER OF USER PARAMETERS
      PARAMETER (MULTIV=10)                                             ! THE FIRST MULTI-VALUED PARAMETER
      PARAMETER (MAXMIX=10)                                             ! THE MAXIMUM NUMBER OF TRACES THAT MAY BE MIXED
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      EQUIVALENCE (VALS(1),LVALS(1))
      COMMON /MIXR/ MUNIT,NLISTS, nwrds
      DIMENSION BUF(111),LBUF(111)
      INTEGER FNO, type, hdr, lhdr, ihdr
      REAL MAXLEN
!
!
      EQUIVALENCE (FNO,LVALS(1)),
     2            (LNO,LVALS(2)),
     3            (DIP,VALS(3)),
     4            (LPRINT,LVALS(4)),
     5            (MAXLEN,VALS(5)),
     6            (type,lvals(6)),
     7            (hdr,lvals(7)),
     8            (lhdr,lvals(8)),
     9            (ihdr,lvals(9)),
     *            (WEIGHT,VALS(10))
      DATA NAMES/ 'FNO   ', 'LNO   ', 'DIP   ', 'LPRINT', 'MAXLEN',
     *            'TYPE  ', 'HDR   ', 'LHDR  ', 'IHDR  ', 'WEIGHT'/
      DATA LENGTH/3,3,3,6,6,4,3,4,4,6/
      DATA TYPES/'L','L','F','L','F','L',3*'L','F'/
!****
!****      SET THE PRESETS
!****
      FNO=1
      LNO = 999999
      MAXLEN=0.
      DIP=0.
      WEIGHT=0.
      LPRINT=0
      LLNO = 0
      NLISTS=0
      type = 1
      hdr = 0
      ihdr = 0
      lhdr = 0
      NS=0
!****
      CALL GETFIL(1,MUNIT,token,ISTAT)
!****   THE CURRENT COMMAND LINE IN THE SYSTEM BUFFER MAY HAVE THE PARAMETERS.
!****   GET A PARAMETER LIST FROM THE USER.
!****
      NTOKES=1
  100 CONTINUE
      CALL GETOKE(TOKEN,NCHARS)                                         ! GET A TOKEN FROM THE USER PARAMETER LINE
      CALL UPCASE(TOKEN,NCHARS)                                         ! CONVERT THE TOKEN TO UPPERCASE
      IF( NCHARS .LE. 0 ) THEN
          IF(NOW.EQ.1) PRINT *,' <  ENTER PARAMETERS  >'
          CALL RDLINE                                                   ! GET ANOTHER USER PARAMETER LINE
          NTOKES=0
          GOTO 100
      ENDIF
  150 NTOKES=NTOKES+1
      DO 190 I = 1, NPARS                                               ! SEE IF IT IS A PARAMETER NAME
         LEN = LENGTH(I)                                                ! GET THE LEGAL PARAMETER NAME LENGTH
         IPARAM = I                                                     ! SAVE THE INDEX
         IF( TOKEN(1:NCHARS) .EQ. NAMES(I)(1:LEN) .AND.
     &      NCHARS.EQ.LEN) GO TO 200
  190 CONTINUE                                                          ! STILL LOOKING FOR THE NAME
      IF(TOKEN(1:NCHARS).EQ.'END'.AND.NCHARS.EQ.3) GO TO 1000           ! END OF PARAM LIST?
      IF(NS.NE.0) GO TO 230
      PRINT *,' ***  ERROR  *** MIX DOES NOT HAVE A PARAMETER NAMED ',
     &      TOKEN(1:NCHARS)
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
      IF( NCHARS .LE. 0 ) THEN                                          ! END OF LINE?
          IF(NOW.EQ.1) PRINT *,' <  ENTER PARAMETERS  >'
          CALL RDLINE                                                   ! GET ANOTHER LINE
          NTOKES=0
          GOTO 210
      ENDIF
  230 CONTINUE
      IF(TYPES(NPARAM).NE.'A') GO TO 240
      GO TO 100
  240 CONTINUE
      CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                              ! TRY AND DECODE IT
      IF(ISTAT.EQ.2) GO TO 420                                          ! =2 MEANS IT IS A NUMERIC
      IERROR=IERROR+1                                                   ! DCODE PRINTED AN ERROR
      GO TO 100
  420 IF(TYPES(NPARAM).EQ.'L') GO TO 500
      IF(NPARAM.LT.MULTIV) GO TO 490                                    !  IS IT A MULTIVALUED PARAMETER
      NS=NS+1                                                           !  THE TOKEN WAS A MULTI-VALUED PARAMETER
      BUF(NS+NPARS)=AREAL
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
      IF(LNO.EQ.32768) LNO=FNO                                          ! DEFAULT LNO TO FNO
      IF( FNO .LE. LLNO .OR. lno .LT. fno ) THEN
          PRINT *,' ***  ERROR  ***  SHOT AND RP NUMBERS MUST INCREASE.'
          IERROR=IERROR+1
      ENDIF
      LLNO=LNO
      IF( NS .LE. 1 ) THEN
          PRINT *,
     *    ' ***  ERROR  ***  AT LEAST 2 TRACE WEIGHTS MUST BE GIVEN.'
          IERROR=IERROR+1
      ENDIF
      IF( ABS(DIP) .GT. 5. ) THEN
          PRINT *,' ***  ERROR  ***  A DIP OF ',dip,' SECONDS EXCEEDS ',
     *     '5.'
          IERROR=IERROR+1
      ENDIF
      IF( type .LT. 1 .OR. type .GT. 4 ) THEN
          PRINT *,' ***  ERROR  ***  TYPE must be 1-4.'
          ierror = ierror + 1
      ENDIF
!****
!****      WRITE THE PARAMETER LIST TO DISC
!****
      IF( NS .GT. MAXMIX .AND. type .NE. 4 ) THEN
          ITEMP=MAXMIX
          PRINT *,
     *    ' ***  ERROR  ***  MIX CAN HANDLE ONLY ',itemp,' WEIGHTS.'
          IERROR=IERROR+1
      ENDIF
      IF( hdr+lhdr+ihdr .NE. 0 ) type = 4
      LBUF(1)=FNO
      LBUF(2)=LNO
      BUF(3)=DIP
      LBUF(4)=NS
      LBUF(5)=LPRINT
      BUF(6)=MAXLEN
      lbuf(7) = type
      lbuf(8) = hdr
      lbuf(9) = lhdr
      lbuf(10) = ihdr
      ITEMP=NPARS+1
      nwrds = NPARS + NS
      IF( IAND(LPRINT,1) .EQ. 1 )  THEN
          PRINT *,(LBUF(I),I=1,2),
     *  BUF(3),LBUF(4),LBUF(5),BUF(6),(lbuf(j),j=7,10)
          PRINT *,(BUF(J),J=ITEMP,nwrds)
      ENDIF
      CALL WRDISC(MUNIT,BUF,NWRDS)
      NLISTS=NLISTS+1
      NS=0
      LLNO=LNO
      LNO=32768                                                         ! DEFAULT THE DEFAULTS
 2020 CALL GETOKE(TOKEN,NCHARS)                                         ! GET THE NEXT TOKEN
      CALL UPCASE(TOKEN,NCHARS)
      NTOKES=NTOKES+1
      IF(NCHARS.GT.0) GO TO 2030                                        ! WAS IT THE END OF A LINE?
      IF(NOW.EQ.1) PRINT *,' <  ENTER PARAMETERS  >'
      CALL RDLINE                                                       ! GET ANOTHER LINE
      NTOKES=0
      GO TO 2020
 2030 IF(TOKEN(1:NCHARS).NE.'END'.OR.NCHARS.NE.3) GO TO 150
      RETURN                                                            !  FINISHED ALL OF THE PARAMETERS!!!
      END
