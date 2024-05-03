      SUBROUTINE logsed( n )
!
!                       PROCESS LOGSTX, where X = { 1, 2 }
!                       ------- ------
!
!  Document date: 31 October 1991
!
!      Process LOGSTX applies a log function to "stretch" each trace in
!  time.  The most common use of process LOGSTX is prior to, and 
!  subsequent to, PROCESS DMO, exact log Dip Move OUT.  Log stretching 
!  allows DMO to work in FK space while preserving the DMO ellipse.
!
!      Prior to dip move out in quasi-FK space [Fourier Transform of log stretched
!  trace does not give F, but a log frequency (OMEGA) - Liner, Geophysics May 1990]
!  use log stetch type 1(stretch), after DMO use log stretch type 2(compress).
! 
!      To preserve dips in OMEGA-K space the data which is resampled via a cubic
!  spline must be resampled at an adequate rate to prevent (temporal and spatial 
!  aliasing of frequency content and seismic dips, respectively.
!
!
!  PARAMETER DICTIONARY
!  --------- ----------
!  TYPE  - The type of log stretch to be applied 1 or 2.
!        =1, "Stretch", tau = ln(t/tc) 
!          where tau is the new log time, t is time of trace, and tc
!          is the cutoff time which prevents the log of zero to be taken.
!        =2, "Compress" = 2, t = tc*exp(tau)
!          where the variables (t,tc and Tau) are defined as above.
!          Preset = 1 "compress"
!
!  Additional Parameters:
!  ---------- -----------
!  TSAMP1 - The new sample rate (*1000) for the log stretched trace.
!           May be different than time sample rate.
!           Used in Process LOGST1
!           Preset = 0.004  (250 samples/sec)
!  TSAMP2 - The new sample rate (*1000) subsquent to log compression.
!           May be different than intial sampling or log sampling. 
!           Used in Process LOGST2
!           Preset = 0.004  (250 samples/sec)
!  TCUT   - Time cut for log stretch/destretch.
!           Must be same for Processes LOGST1 and LOGST2 
!           Times prior to TCUT will be nulled.
!           Preset = 0.1 seconds
!  SLTIME - Start time of trace subsequent to DMO and "unstretch".
!           Preset = 0.0
!  ELTIME - End time of trace subsequent to DMO and "unstretch".
!           Preset = 6.0
!  LOGHZ  - Highest frequency(prestretch) used.
!
!  Copyright (C) 1991 The Regents of the University of California
!  ALL RIGHTS RESERVED.
!  Written By Graham Kent, September 1991
!
      PARAMETER ( npars = 8 )                                           ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      DIMENSION scr(npars), lscr(npars)
      EQUIVALENCE (scr(1),lscr(1))
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /logcom/ ilogunit1, ilogunit2, nloglists, nlogwrds
      DATA names / 'TYPE  ','TSAMP1','TSAMP2','TCUT  ','SLTIME',
     &             'ELTIME', 'LOGHZ ', 'LPRINT' /
!**** 
!****    Set the parameter presets and various variable presets
!****
      itype = 1
      rtsamp1 = 0.004
      rtsamp2 = 0.004
      rtcut   = 0.10
      rsltime = 0.0
      reltime = 6.0
      rloghz  = 40.0 
      nloglists = 0  
      lprint = 0     
      if ( n.eq.1 ) then
        CALL getfil( 1, ilogunit1, token, istat )                          ! get a file for the LOGS parameters
      else
        CALL getfil( 1, ilogunit2, token, istat )                          ! get a file for the LOGS parameters
      endif
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
             ns = ns + 1
             CALL upcase( token, nchars )     

!*GMK        NO TOKENS THAT ARE CHARATERS - CONVERT TO ALPHANUMERIC

             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
             IF( names(nparam) .EQ. 'TYPE' ) itype = NINT(areal)
             IF( names(nparam) .EQ. 'TSAMP1' ) rtsamp1 = areal
             IF( names(nparam) .EQ. 'TSAMP2' ) rtsamp2 = areal
             IF( names(nparam) .EQ. 'TCUT' ) rtcut = areal
             IF( names(nparam) .EQ. 'SLTIME' ) rsltime = areal
             IF( names(nparam) .EQ. 'ELTIME' ) reltime = areal
             IF( names(nparam) .EQ. 'LOGHZ' ) rloghz = areal
             IF( names(nparam) .EQ. 'LPRINT' ) lprint = NINT(areal)
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
      IF( itype .LT. 1 .OR. itype .GT. 2 ) THEN
          PRINT *,' ***  ERROR  ***  Illegal TYPE value of ',itype
          ierror = ierror + 1
      ENDIF
      IF( rtsamp1 .LT. 0 .OR. rtsamp2 .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  Illegal TSAMP Value (1) or (2)
     *of',rtsamp1,rtsamp2
          ierror = ierror + 1
      ENDIF
      IF( RTCUT .EQ. 0 .OR. RTCUT .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  Illegal TCUT value of ',rtcut
          ierror = ierror + 1
      ENDIF
      IF( RSLTIME .LT. 0 .OR. RSLTIME .GT. RELTIME ) THEN
          PRINT *,' ***  ERROR  ***  Illegal SL or EL TIME value
     * of ',rsltime, reltime
          ierror = ierror + 1
      ENDIF
      IF( RLOGHZ .LT. 0) THEN
          PRINT *,' ***  ERROR  ***  Illegal LOGHZ value of ',rloghz
          ierror = ierror + 1
      ENDIF
      IF( nloglists .GT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  Only one LOGS list is permitted.'
          ierror = ierror + 1
      ENDIF
!****
!****   Write the parameter list to disk
!****
      lscr(1) = itype
      scr(2) = rtsamp1
      scr(3) = rtsamp2
      scr(4) = rtcut
      scr(5) = rsltime
      scr(6) = reltime
      scr(7)= rloghz 
      lscr(8)= lprint
      nlogwrds = npars
      nloglists = nloglists + 1
      if ( n.eq.1 ) then
        CALL wrdisc( ilogunit1, scr, nlogwrds )
      else
        CALL wrdisc( ilogunit2, scr, nlogwrds )
      endif                            
      IF( IAND(lprint,1) .NE. 0 ) THEN
       PRINT *,'  type=',itype, ' tsamp1=',rtsamp1, ' tsamp2=',rtsamp2,
     &         ' tcut=',rtcut , ' sltime=',rsltime, ' eltime=',reltime,
     &         ' loghz=', rloghz
      ENDIF
!****
!****    finish up the parameter reading
!****
 2000 CONTINUE
      CALL getoke( token, nchars )                                       ! get the next token
      IF( nchars .LE. 0 ) THEN
          IF( now .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'
          CALL rdline                                                          ! get a new line of parameters
          ntokes = 0
          GOTO 2000
      ENDIF
      CALL upcase( token, nchars )
      ntokes = ntokes + 1
      IF( token(1:nchars) .NE. 'END' .OR. nchars .NE. 3 ) GOTO 100
      RETURN
      END
