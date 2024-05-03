      SUBROUTINE tp2ted
!                                                                      
!                       PROCESS TP2TX
!                       ------- -----
!
!  Document date: 25 August 1989
!
!          Process TP2TX transform data in the tau-p space domain to
!  the time-distance space using the "HOP" method for slant stack (see
!  process TX2TP).
!         Since the tx to tau-p transformation changes the meaning of 
!  shots and rps, TX2TP will output the data as if it is a shot 
!  containing np traces and ALL THE TX DOMAIN TRACE INFORMATION IS LOST.
!  The SEGY trace headers will contain the following information:
!  record ("shot") number   (SEGY word 3)
!  trace number with the record  (SEGY word 4)
!  trace id flag (live trace flag)  (SEGY word 15)
!  number of data samples (SEGY word 58)
!  
!  Limitations :  300 input time traces
!                1024 samples within a time trace
!                 200 output p traces
!                 400 taus within a p trace
!                                               
!                                               
!  PARAMETER DICTIONARY
!  --------- ----------
!  SETAU  - The start and end taus to input.
!           Preset = calculated from the data.
!  SET    - The start and end times of the output t-x data. 
!           Preset = 
!  SEX    - Start and end ranges (x) to calculate and output.
!           Preset = 
!  NX     - The number of ranges to calculate.  NX is the increment
!           between the start and end ranges (SEX).
!           Preset = the number of input time traces
!  IHPATH - Input header pathname.  TP2TX will ouput the data with the 
!           SEGY trace headers from disk file IPATH rather than from the
!           input tau-p traces.
!           Preset =  none
!  PCNTO  - Percent taper applied to models before the inverse FFT
!  SHIFT  - The time shift to apply to successive traces.  The shift
!           is accumulative, thus each trace is shifted by SHIFT 
!           relative to the previous trace.  The shift represents a
!           constant reduction velocity, i.e. start time for each trace
!           must be delayed by same amount relative to previous trace
!           Required.              e.g.  shift .05
!
!
!  Copyright (C) by The Regents of The University of California, 1988
!  ALL RIGHTS RESERVED.
!
      PARAMETER ( npars = 15 )                                          ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      CHARACTER*1 types(npars)                                          ! the type of parameter
      REAL vals(npars)                                                  ! holds the REAL parameter values
      DIMENSION lvals(npars)                                            ! holds the INTEGER parameter values
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /tp2tx/ sshift, sex(2), nnx, setau(2), beta, ffc, ppcnti, 
     *               ppcnto, iirev, ffon, dummy, iimft, set(2), lprt,
     &               tpprestk
!****  we need tx2tp common because of lunhdr - see tx2ted for some notes
      COMMON /tx2tp/ sshift1, sep1(2), nnp1, setau1(2), beta1, ffc1, 
     *               ppcnti1, ppcnto1, iirev1, ffon1, dummy1, iimft1, 
     *               set1(2), lprt1, lunhdr, txprestk
      INTEGER fon, ffon, txprestk
      EQUIVALENCE ( shift, vals(1) ),
     3            ( nx, lvals(3) ),
     5            ( b, vals(5) ),
     6            ( fc, vals(6) ),
     7            ( pcnti, vals(7) ),
     8            ( pcnto, vals(8) ),
     9            ( irev, lvals(9) ),
     *            ( fon, lvals(10) ),
     1            ( prestk, lvals(11) ),
     2            ( imft, lvals(12) ),
     4            ( lprint, lvals(14) )
      DATA names /'SHIFT ','SEX   ','NX    ','SETAU ','B     ',
     *            'FC    ','PCNTI ','PCNTO ','IREV  ','FON   ',
     *            'PRESTK','IMFT  ','SET   ','LPRINT','IHPATH' /
      DATA types /2*'F','L',5*'F',2*'L','A','L','F','L','A' /
!**** 
!****    Set the parameter presets and various variable presets
!****
      shift = 0.
      b = 2.                                                            ! hidden parameter - scaling factor for representers
      sex(1) = -1.
      sex(2) = -1.
      nx = 0
      fc = 0
      pcnti = 0.
      pcnto = 0.
      irev = 0
      fon = 1
      set(1) = -1.
      set(2) = -1.
      lprint = 0
      setau(1) = 99999.
      tpprestk = 0
!****
!****     get the user's parameters -  there must be something, at least an "end"
!****
      ntokes = 0                                                        ! count the tokens
  100 CONTINUE
      ns = 0
      CALL getoke( token, nchars )                                      ! get a token and it's length
      CALL upcase( token, nchars )                                      ! convert parameter names to upper case
      IF( nchars .EQ. 0 ) THEN                                          ! anything there?
          CALL rdline                                                   ! nope, get another line
          ntokes = 0
          GOTO 100
      ENDIF
  110 ntokes = ntokes + 1
      DO 200 nparam = 1, npars
         IF( token(1:nchars) .EQ. names(nparam) ) THEN                  ! find the parameter name in our list
  120        CALL getoke( token, nchars )                               ! get the value
             IF( nchars .EQ. 0 ) THEN
                 CALL rdline
                 ntokes = 0
                 GOTO 120
             ENDIF
             ntokes = ntokes + 1
             IF( names(nparam) .EQ. 'IHPATH' ) THEN
                 IF( lunhdr .EQ. 0 ) 
     &               CALL getfil( 4, lunhdr, token, istat )             ! open the existing file
                 GOTO 100
             ENDIF
             IF( names(nparam) .EQ. 'PRESTK' ) THEN
                 CALL upcase( token, nchars )
                 IF( token(1:1) .EQ. '1' ) tpprestk = 1
                 IF( token(1:1) .EQ. 'Y' ) tpprestk = 1
                 GOTO 100
             ENDIF
             ns = ns + 1
             CALL upcase( token, nchars )
             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
             IF( names(nparam) .EQ. 'SET' ) THEN                        ! set is an array
                 set(ns) = areal
                 IF( ns .EQ. 1 ) GOTO 120
                 GOTO 100
             ENDIF
             IF( names(nparam) .EQ. 'SEX' ) THEN                        ! sex is an array
                 sex(ns) = areal / 1000.
                 IF( ns .EQ. 1 ) GOTO 120
                 GOTO 100
             ENDIF
             IF( names(nparam) .EQ. 'SETAU' ) THEN                        ! setau is an array
                 setau(ns) = areal
                 IF( ns .EQ. 1 ) GOTO 120
                 GOTO 100
             ENDIF
             IF( types(nparam) .EQ. 'L' ) THEN
                 lvals(nparam) = areal                                  ! convert the real to INTEGER*4
             ELSE
                 vals(nparam) = areal                                   ! move the real to the parameter
             ENDIF
             GOTO 100
         ENDIF
  200 CONTINUE
      IF( token(1:nchars) .NE. 'END') THEN
          PRINT *,' ***  ERROR  ***  No such parameter as ',
     *      token(1:nchars)
          ierror = ierror + 1
          GOTO 100
      ENDIF
!****
!****    Do some ERROR checking
!****
      IF( nx .LT. 0 .OR. (nx .GT. 0 .AND. nx .LT. 2 ) )THEN
          PRINT *,' ***  ERROR  ***  NX must be greater than 2.'
          ierror = ierror + 1
      ENDIF
      IF( nx .GT. 0 .AND. sex(1)+sex(2) .EQ. -2. ) THEN
         PRINT *,' ***  ERROR  ***  SEX must be given when NX is given.'
          ierror = ierror + 1
      ENDIF
      IF( sex(1)+sex(2) .NE. -2. .AND. nx .EQ. 0 ) THEN
         PRINT *,' ***  ERROR  ***  NX must be given when SEX is given.'
          ierror = ierror + 1
      ENDIF
!****
!****    finish up the parameter reading
!****
 2000 CONTINUE
      CALL getoke( token, nchars )                                       ! get the next token
      CALL upcase( token, nchars )
      ntokes = ntokes + 1
      IF( nchars .LE. 0 ) THEN
          IF( now .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'
          CALL rdline                                                          ! get a new line of parameters
          ntokes = 0
          GOTO 2000
      ENDIF
      IF( token(1:nchars) .NE. 'END' .OR. nchars .NE. 3 ) THEN
          PRINT *,' ***  ERROR  ***  TX2TP can have only one list .'
          ierror = ierror + 1
      ENDIF
      sshift = shift
      nnx = nx
      beta = b
      ffc = fc
      ppcnti = pcnti
      ppcnto = pcnto
      iirev = irev
      iimft = imft
      lprt = lprint
      ffon = fon
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,' sshifts=',sshifts,' sex=',sex,' nnx=',nnx,
     *      ' setau=',setau,' beta=',beta,' ffc=',ffc
          PRINT *,' ppcnti=',ppcnti,' ppcnto=',ppcnto,' iirev=',
     *      iirev,' fon=',ffon,' set=',set(1),set(2),' lunhdr=',lunhdr
      ENDIF
      RETURN
      END
