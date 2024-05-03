      SUBROUTINE FKMIED
!                                 PROCESS FKMIGR
!                                 ------- ------
!
!  DOCUMENT DATE: 11 MARCH 1986                                           35
!
!       PROCESS FKMIGR PERFORMS FK MIGRATION ON DATA THAT IS IN THE FREQUENCY-
!  WAVENUMBER DOMAIN.  THE DATA MUST HAVE BEEN THROUGH PROCESS TX2FK PRIOR TO
!  FKMIGR.  THE OUTPUT FROM FKMIGR IS ALSO IN THE FK DOMAIN, THUS, THE DATA
!  MAY BE TRANSFORMED TO THE TIME-SPACE DOMAIN VIA PROCESS FK2TX AFTER PROCESS
!  FKMIGR.
!        FK MIGRATION ASSUMES A CONSTANT VELOCITY FOR THE ENTIRE SECTION.  THIS
!  TYPE OF MIGRATION ALSO ASSUMES THAT THE DATA IS "ZERO-OFFSET" DATA.  SINGLE
!  CHANNEL DATA WITH FAIRLY SMALL SHOT-RECEIVER DISTANCE ARE ZERO-OFFSET. MOVED
!  OUT DATA ARE ZERO-OFFSET.  THE ZERO-OFFSET DIFFRACTION HYPERBOLA THAT ARE
! COLLAPSED BY FK MIGRATION  HAVE THE FORMULA TX=2*SQRT(T0**2/4+X**2/V**2).
!       AT LEAST 60 TRACES SHOULD ADDED TO THE BEGINING AND THE END OF THE
!  SECTION TO BE MIGRATED IN PROCESS TX2FK (PARAMETER NXPAD).  THIS PADDING
!  SHOULD BE SUFFICIENT TO PREVENT "EDGE" OR BOUNDARY EFFECTS.
!      FKMIGR USES STOLT'S ALGORITHM TO PERFORM MIGRATION IN THE FK DOMAIN AND
!  MAY BE FOUND IN "IMAGING THE EARTH'S INTERIOR", BY JON CLAERBOUT.
!  VEL    - THE CONSTANT VELOCITY TO USE TO MIGRATE THE DATA.
!           PRESET=0.  E.G. VEL 1.
!  NFINT  - THE NUMBER OF ADJACENT FREQUENCIES TO USE IN INTERPOLATION.
!           PRESET =10   E.G. NFINT 2
!  DELTAX - THE DISTANCE BETWEEN TRACES.
!           PRESET 1.   E.G. DELTAX 100.
!  DELTAT - THE TIME SAMPLE INTERVAL., IN SECONDS.
!           PRESET = TRACE HEADER.   E.G.  DELTAT 1.
!  END    - TERMINATES EACH PARAMETER LIST.
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, DECEMBER 1983
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod 16 July 2007 - Print warning if DELTAX is not given.
!                   - ERROR if velocity is no given.
!
      PARAMETER (NPARS=5)                                               ! THE NUMBER OF USER PARAMETERS
      CHARACTER*6 NAMES(NPARS)
      CHARACTER*1 TYPES(NPARS)
      DIMENSION LENGTH(NPARS)
      CHARACTER*80 TOKEN
      DIMENSION VALS(NPARS),LVALS(NPARS)
      EQUIVALENCE (VALS(1),LVALS(1))
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
      COMMON /FKMIGR/V,NINT,LPR,DX,DT
!
!
      EQUIVALENCE (VEL,VALS(1)),
     2            (LPRINT,LVALS(2)),
     3            (NFINT,LVALS(3)),
     4            (DELTAX,VALS(4)),
     5            (DELTAT,VALS(5))
      DATA NAMES/'VEL   ',
     2           'LPRINT',
     3           'NFINT ',
     4           'DELTAX',
     5           'DELTAT'/
      DATA LENGTH/3,6,5,6,6/
      DATA TYPES /'F','L','L','F','F'/
!****
!****      SET THE PRESETS
!****
      VEL = 0.
      LPRINT=0
      NFINT=10
      DELTAX=1.
      DELTAT=0.
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
  191 FORMAT(' ***  ERROR  *** FKMIGR DOES NOT HAVE A PARAMETER ',
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
      GO TO 100
  240 CONTINUE
      CALL DCODE(TOKEN,NCHARS,AREAL,ISTAT)                              ! TRY AND DECODE IT
      IF(ISTAT.EQ.2) GO TO 420                                          ! =2 MEANS IT IS A NUMERIC
      IERROR=IERROR+1                                                   ! DCODE PRINTED AN ERROR
      GO TO 100
  420 IF(TYPES(NPARAM).EQ.'L') GO TO 500
      VALS(NPARAM)=AREAL                                                !  FLOATING POINT VALUES
      GO TO 100
  500 CONTINUE                                                          !  32 BIT INTEGER VALUES
      LVALS(NPARAM)=AREAL
      GO TO 100
!****
!****   FINISHED A LIST, NOW DO THE ERROR AND VALIDITY CHECKS
!****
 1000 CONTINUE
      IF( deltax .EQ. 1 ) THEN
          PRINT *,' ***  WARNING  ***  Using DELTAX 1.'
          iwarn = iwarn + 1
      ENDIF
      IF( vel .EQ. 0 ) THEN
          PRINT *,' ***  ERROR  ***  VEL must be given.'
          ierror = ierror + 1
      ENDIF
      V=VEL
      LPR=LPRINT
      NINT=NFINT
      DX=DELTAX
      DT=DELTAT
      IF(IAND(LPRINT,1).EQ.1) PRINT 1100,V,NINT,DX,DT
 1100 FORMAT(' FKMIGR PARAMETERS: VEL=',F10.5,' NFINT=',I5,
     *   ' DX=',F10.5,' DT=',F10.5)
      NLISTS=NLISTS+1
      LNO=32768                                                         ! DEFAULT THE DEFAULTS
      NS=0                                                              ! SET THE NUMBER OF MULTI-VALUED PARAMETER ENTRIES BACK TO ZER0
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
