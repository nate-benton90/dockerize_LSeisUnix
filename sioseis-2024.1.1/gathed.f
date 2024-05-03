       SUBROUTINE GATHED(SCR,LSCR,ISCR)
!                              PROCESS GATHER
!                              ------- ------
!
!  DOCUMENT DATE: 15 March 1994
!
!    A GATHER IS A COLLECTION OR REARRANGEMENT OF TRACES ACCORDING TO SOME
!  CRITERIA.  PROCESS GATHER COLLECTS OR REARRANGES THE INPUT TRACES ACCORDING
!  TO THE REFLECTION POINT (RP) NUMBER CALCULATED BY PROCESS GEOM.  THE GEOM
!  PARAMETERS MAY BE MANIPULATED BY THE USER TO GATHER THE INPUT TRACES ACCORDING
!  TO ANY CRITERIA BY FUDGING THE GEOM PARAMETERS.  A CONSTANT OFFSET GATHER
!  OF A UNIFORM MARINE LINE MAY BE MADE BY OMITTING TRACES VIA PROCESS INPUT.
!      PROCESS GATHER SORTS EACH GATHER BY THE ABSOLUTE VALUE OF THE SHOT-
!  RECEIVER DISTANCE, SO THAT THE SHORTEST RANGE TRACE IS FIRST WITHIN THE
!  GATHER.  EACH GATHER IS IS TERMINATED BY SETTING A SPECIAL FLAG IN THE
!  TRACE HEADER.  A GATHER RECORD IS THE COLLECTION OF ALL THESE TRACES.
!     SEE PROCESS GEOM FOR THE METHOD OF CALCULATING RP NUMBERS.
!     Gather creates a disk file to store the partial gathers while
!  the data is being read.  Gather assumes that the geometry of the data
!  does not skip around very much. i.e. the geometry doesn't go backwards
!  nor does it skip more than a cable length forward.  The temporary
!  disk file can hold MAXRPS rps (preset to 5 plus the number of traces 
!  per shot from the SEGY binary header), with each each rp able to hold
!  a maximum of MAXTRS traces (also preset to the number of traces per
!  shot in the SEGY binary header), with each traces having a maximum
!  of NWRDS samples, with each sample being 4 bytes long (except on the
!  Cray).  The temporary disk file size will be: 
!      maxtrs * maxrps * (nwrds+240) * 4
!  The preset values of maxtrs, maxrps, and nwrds are designed for the
!  marine geometry of advancing .5 groups between shots (96 cdp from
!  a 96 trace streamer).
!      A NULL SET OF GATHER PARAMETERS MUST BE GIVEN EVEN IF ALL THE
!  PARAMETERS ARE THE PRESETS.  E.G.  GATHER
!                                          END
!                                     END
!
!  THE PARAMETER DICTIONARY
!  --- --------- ----------
!  FRP    - THE FIRST RP NUMBER TO GATHER.  TRACES WITH RP NUMBERS LESS THAN FRP
!           ARE NOT GATHERED.  RP NUMBERS ARE CALCULATED BY PROCESS GEOM.
!           THIS PARAMETER SHOULD BE USED WHEN RESTARTING A PARTIALLY
!           COMPLETED GATHER RUN.  THE USE OF FRP WILL CAUSE PROCESS GATHER TO
!           REWRITE THE PROCESS INPUT PARAMETERS SO THAT THE UNNEEDED INPUT
!           WILL NOT BE PROCESSED.  BOTH THE PROCESS INPUT AND PROCESS GEOM
!           PARAMETERS MUST BE GIVEN PRIOR TO THE PROCESS GATHER PARAMETERS.
!           FRP only works with PROCESS INPUT.
!           PRESET=RP NUMBER OF THE FIRST TRACE.
!  RPINC  - THE INCREMENT OF RP NUMBERS BETWEEN THE RPS TO GATHER.  THE ONLY
!           TRACES GATHERED WILL HAVE BIN NUMBERS FRP, FRP+RPINC, FRP+2*RPINC,
!           FRP+3*RPINC, . . . . ETC.
!           PRESET=1.
!  NWRDS  - THE LARGEST NUMBER OF SAMPLES PER TRACE IN THE JOB.  THIS SHOULD BE
!           TRACE LENGTH PLUS THE TRACE HEADER LENGTH.
!           PRESET=FROM FIRST INPUT TRACE.
!  MAXRPS - THE MAXIMUM NUMBER OF BINS (OR RP'S) THAT ARE NEEDED ON
!           THE DISC AT ANY ONE TIME.  IN MARINE WORK THE NUMBER OF TRACES
!           PER SHOT WILL SUFFICE SINCE NO TWO UNGATHERED TRACES WITH THE SAME
!           RP NUMBER ARE MORE THAN A CABLE LENGTH AWAY.
!           PRESET=THE NUMBER OF TRACES FROM THE SEGY BINARY HEADER PLUS 5
!  MAXTRS - THE MAXIMUM NUMBER OF TRACES ANY ONE GATHER CAN HAVE.  IN RP GATHERS
!           THIS IS THE MAXIMUM CDP ALLOWED.
!           PRESET=THE NUMBER OF TRACES FROM THE SEGY BINARY HEADER.
!  MINTRS - THE MINIMUM NUMBER OF TRACES EACH GATHER CAN HAVE.  IF MINTRS=0 AND
!           NO INPUT TRACES CONTRIBUTE TO A GIVEN GATHER, THAT GATHER WILL NOT
!           BE OUTPUT.
!           PRESET=1    e.g. mintrs 24
!
!  WRITTEN AND COPYRIGHTED BY:
!   PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, MARCH 1980
!
!
!   MODIFIED ON 3 MAY 1981 FOR THE RESTART CAPABILITY.  P.C.H.
!   modified on 24 May 1989 so the restart will work under Version 2.0
!     ( open the new input parameter file correctly and read/write
!      the correct items for the Version 2 process input parameters).
!   mod 3 Dec 1993.  Preset nwrds to 0 and let the execute determine the
!     length from the first GOOD trace it gets.
!   mod 20 Apr 96 - Skip the restart frp if geom isn't there and the cdp
!      number is is in the trace already (like geom was done in a 
!      different run)
!  mod 18 Nov 98 - Allow frp without process input, in case the first
!      trace read is not the first trace of the subsurface.
!  mod March 1999 - Change maxrps preset from intrcs + 5 to intrcs + 20
!  mod May 2006 - Change MAXTRS preset to 100
!  mod 10 July 2020 - Add ERROR if too much data.
!
!
      PARAMETER (NPARS=7)                                               ! THE NUMBER OF USER PARAMETERS
      DIMENSION SCR(111),LSCR(111),ISCR(111)
      INTEGER*2 ISCR
      INTEGER FIS,SINC,FTR,TRINC,forgat,decimf,order
      COMMON /INPUT/ IPARUN,NLISTS
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      EQUIVALENCE (VALS(1),LVALS(1))
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /READT/ILUN,NUMHDR,NUMDAT,I1,I2,INTRCS
      COMMON /GEOM/ IGUNIT
      COMMON /TCOL/ LFRP,LRPINC,MWRDS,IOUNIT,NAXRPS,NAXTRS,NINTRS
      INTEGER FRP,RPINC
!
!
      EQUIVALENCE (FRP,LVALS(1)),
     2            (RPINC,LVALS(2)),
     3            (NWRDS,LVALS(3)),
     4            (MAXRPS,LVALS(4)),
     5            (MAXTRS,LVALS(5)),
     6            (MINTRS,LVALS(6)),
     7            (LPRINT,LVALS(7))
      DATA NAMES/'FRP   ','RPINC ','NWRDS ','MAXRPS',
     *           'MAXTRS','MINTRS','LPRINT'/
      DATA LENGTH/3,5,5,4*6/
      DATA TYPES/7*'L'/
!
!
!       SET THE PRESETS
!
      FRP=32767
      RPINC=1
      NWRDS = 0
      MAXRPS=INTRCS+20
      MAXTRS = 100
      MINTRS=1
      NLISTS=0
      NS=0
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
  191 FORMAT(' ***  ERROR  *** GATHER DOES NOT HAVE A PARAMETER ',
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
      GO TO 100
  500 CONTINUE                                                          ! 32 BIT INTEGER VALUES
      LVALS(NPARAM)=AREAL
      GO TO 100
!****
!****   FINISHED A LIST, NOW DO THE ERROR AND VALIDITY CHECKS
!****
 1000 CONTINUE
      LFRP=FRP                                                          ! FORTRAN DOESN'T ALLOW EQUIVALENCING TO COMMON!!
      LRPINC=RPINC
      MWRDS=NWRDS
      NAXRPS=MAXRPS
      NAXTRS=MAXTRS
      NINTRS=MINTRS
!****
      NS=0
 2020 CALL GETOKE(TOKEN,NCHARS)                                         ! GET THE NEXT TOKEN
      CALL UPCASE(TOKEN,NCHARS)
      NTOKES=NTOKES+1
      IF(NCHARS.GT.0) GO TO 2030                                        ! WAS IT THE END OF A LINE?
      IF(NOW.EQ.1) PRINT 140
      CALL RDLINE                                                       ! GET ANOTHER LINE
      NTOKES=0
      GO TO 2020
 2030 IF(TOKEN(1:NCHARS).NE.'END'.OR.NCHARS.NE.3) GO TO 150
!****
!****  Do some editing
!****
      IF(FLOAT(maxrps)*FLOAT(maxtrs)*FLOAT(nwrds).GT. 2147483647. ) THEN
         PRINT *,' ***  ERROR  ***  Too much data to GATHER.'
         PRINT *,' Reduce MAXRPS or MAXTRS or NWRDS.  2GB max'
         ierror = ierror + 1
      ENDIF
      RETURN                                                            !  FINISHED ALL OF THE PARAMETERS!!!
      END
