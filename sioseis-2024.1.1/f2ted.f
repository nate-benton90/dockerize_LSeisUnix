      SUBROUTINE f2ted
!                                     PROCESS F2T
!                                     _______ ___
!
!  DOCUMENTATION DATE:  13 October 1993
!
!       Process F2T transforms frequency domain data to the time domain.
!  Process T2F must have been used to create the frequency domain.  The
!  frequency domain data may be in rectangular or polar coordinates.
!  F2T permits the user to select any time domain sample interval, thus
!  acheiving a resampling of time domain data.
!       Resampling is performed as follows:
!  1)  Convert the frequency domain to polar coordinates if it isn't.
!  2)  Interpolate (spline) both the frequency and phase spectra.
!
!      At least one parameter list must be given, even if no parameters
!  are specified, in order that the parameter presets be set.  e.g.
!      f2t
!              end
!      end
!
!  PARAMETER DICTIONARY
!  --------- ----------
!  OSI - Output Sample Interval, in seconds.  No filtering is performed
!        before resampling.
!        Preset = the time domain sample interval prior to T2F
!                 (The original or input time domain interval)
!  TYPE -
!
!  WRITTEN AND COPYRIGHTED (C) BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 21 JANUARY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod 25 Aug 1993 - make an edit, so END may be given.
!  mod 13 Oct 93 - Add parameter OSI
!  mod 13 Apr 00 - Add parameter OFMT
!  mod 28 Jan 04 - LPRINT needed to have default.
!
      PARAMETER ( npars = 3 )                                           ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      DIMENSION scr(npars), lscr(npars)
      EQUIVALENCE (scr(1),lscr(1))
      COMMON /f2t/ osi, lprint, iofmt
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      DATA names / 'OSI   ', 'LPRINT','TYPE  ' /
!****
!****    Set the parameter presets and various variable presets
!****
      osi = 0.
      iofmt = 1
      lprint = 0
!****
!****     get the user's parameters
!****
      ntokes = 0                                                        ! count the tokens
  100 CONTINUE
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
  120        CALL getoke( token, nchars )
      ! get the value
             IF( nchars .EQ. 0 ) THEN
                 CALL rdline
                 ntokes = 0
                 GOTO 120
             ENDIF
             ntokes = ntokes + 1
             ns = ns + 1
             CALL upcase( token, nchars )
             IF( names(nparam) .EQ. 'TYPE  ' ) THEN
                 IF( token(1:4) .EQ. 'TIME' ) iofmt = 1
                 IF( token(1:7) .EQ. 'COMPLEX' ) iofmt = 2
                 IF( token(1:7) .EQ. 'HILBERT' ) iofmt = 3
                 IF( token(1:8) .EQ. 'ANALYTIC' ) iofmt = 4
                 GOTO 100
             ENDIF
             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
             IF( names(nparam) .EQ. 'OSI   ' ) osi = areal
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
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,' osi=',osi,' iofmt=',iofmt
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
