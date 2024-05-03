      SUBROUTINE resaed
!
!                       PROCESS RESAMP
!                       ------- ------
!
!  Document date: 16 November 1993
!
!      Process RESAMP resamples the seismic trace from one sample
!  interval to another by: 1) transforming the trace into the frequency
!  domain then back to the time domain; or 2) doing a polynomial
!  interpolation in the time domain.
!      This process is useful for converting data that is recorded at 
!  "not nice" sample rates.  E.g. OBS data is recorded with 128 samples
!  per second which is .0078125 seconds per sample.  The SIOSEIS process
!  PLOT has problems with that, but not with .008 or 125 samples per 
!  second.
!
!  PARAMETER DICTIONARY
!  --------- ----------
!  NEWSI  - The output sample interval, in seconds.
!           REQUIRED.            e.g.   newsi  .004
!  TYPE   - The method of resampling.
!         = 1, Performed in the frequency domain by using the IMSL 
!              FFT routines FFTRC and FFTCC.
!             *****    Available ONLY if IMSL is available.   ******
!         = 2, Performed in the time domain using polynomial 
!              interpolation of order n, as discussed in "Numerical
!              Recipes", section 3.1
!         Preset = 1    limits   0 < type < 2         e.g.   type 2
!  ORDER  - The order of the interpolation when using type 2 
!           interpolation.  According to "Numerical Recipes", "We
!           enthusiastically endorse interpolations with 3 or 4 points,
!           we are perhaps tolerant of 5 or 6; but we rarely go higher".
!         Preset = 4   limits   1 < order < 7        e.g.   order 3
!
!
!  Copyright (C) 1991 The Regents of the University of California
!  ALL RIGHTS RESERVED.
!
!  mod 17 July 2008 - Change type preset to time domain (type 2)
!
      PARAMETER ( npars = 4 )                                           ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      DIMENSION scr(npars), lscr(npars)
      EQUIVALENCE (scr(1),lscr(1))
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /resamp/ si, lprint, type, order
      INTEGER type, order
      DATA names / 'NEWSI ', 'LPRINT', 'TYPE  ', 'ORDER ' /
!**** 
!****    Set the parameter presets and various variable presets
!****
      si = -99999.
      lprint = 0
      type = 2
      order = 4
!****
!****     get the user's parameters
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
             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
             IF( names(nparam) .EQ. 'NEWSI' ) si = areal
             IF( names(nparam) .EQ. 'LPRINT' ) lprint = NINT(areal)
             IF( names(nparam) .EQ. 'TYPE' ) type = NINT(areal)
             IF( names(nparam) .EQ. 'ORDER' ) order = NINT(areal)
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
      IF( si .EQ. -99999. ) THEN
          PRINT *,' ***  ERROR  ***  NEWSI must be given.'
          ierror = ierror + 1
      ENDIF
      IF( si .LE. 0. .AND. si .NE. -99999. ) THEN
          PRINT *,' ***  ERROR  ***  NEWSI must be positive.'
          ierror = ierror + 1
      ENDIF
      IF( type .LT. 1 .OR. type .GT. 2 ) THEN
          PRINT *,' ***  ERROR  ***  type must be 1 or 2.'
          ierror = ierror + 1
      ENDIF
      IF( order .LT. 2 .OR. order .GT. 6 ) THEN
          PRINT *,' ***  ERROR  ***  ORDER must be between 2 and 6.'
          ierror = ierror + 1
      ENDIF
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,' newsi=', si,' type=',type,' order=',order
      ENDIF
!****
!****    finish up the parameter reading
!****
 2000 CONTINUE
      CALL getoke( token, nchars )                                      ! get the next token
      IF( nchars .LE. 0 ) THEN
          IF( now .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'
          CALL rdline                                                   ! get a new line of parameters
          ntokes = 0
          GOTO 2000
      ENDIF
      CALL upcase( token, nchars )
      ntokes = ntokes + 1
      IF( token(1:nchars) .NE. 'END' .OR. nchars .NE. 3 ) GOTO 100
      RETURN
      END
