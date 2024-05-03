      SUBROUTINE cated
!
!                       PROCESS CAT
!                       ------- ---
!
!  Document date: 29 September 1992
!
!      Process CAT concatenates consecutive traces or shots.  The
!  concatenation is done without regard for the deep water delay.
!  i.e. The last sample of the trace being appended is always 
!  adjacent to the first sample the next trace.
!      The output SEGY header is the header of the first trace of
!  the series being concatenated.
!      Shot/rp concatenation means that like traces are concatenated.
!  e.g.  shot 2 trace 1 is appended to shot 1 trace 1 and
!        shot 2 trace 2 is appended to shot 1 trace 2.
!
!      There is no spatial interpolation.
!      Each parameter list must be terminated with the word end.
!      A null set of parameters must be given if all the parameters are
!  presets.  e.g.    cat end end
!
!  PARAMETER DICTIONARY
!  --------- ----------
!  TYPE   - The type of concatenation.
!         = TRACE,
!         = SHOT,
!           Preset = shot         e.g. type trace
!  N      - The number of consecutive traces or shots to concatenate
!           in each output record.  A value of 0 or 1 means that no
!           concatenation should take place.
!           Preset = 2            e.g. n 3
!  FNO    - The first shot/rp number the parameter list applies to.
!           Preset = the first shot/rp received.    e.g.   fno 101
!  LNO    - The last shot/rp number the parameter list applies to.
!           Preset = the last shot/rp received.    e.g.   lno 101
!  FTR    - The first trace number the parameter list applies to.
!           Preset = the first trace of each shot/rp.    e.g.   ftr 10
!  LTR    - The last trace number the parameter list applies to.
!           Preset = the last trace of each shot/rp.    e.g.   ltr 10
!  END    - Terminates the parameter list.
!
!
!  Copyright (C) 1992 The Regents of the University of California
!  ALL RIGHTS RESERVED.
!
      PARAMETER ( npars = 7 )                                           ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      CHARACTER*5 type
      DIMENSION scr(npars), lscr(npars)
      EQUIVALENCE (scr(1),lscr(1))
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /cat/ lun, nlists, nwrds
      INTEGER fno, lno, ftr, ltr, n, lprint
      DATA names / 'TYPE  ','N     ', 'LPRINT', 'FNO   ', 'LNO   ', 
     &             'FTR   ','LTR   ' /
!**** 
!****    Set the parameter presets and various variable presets
!****
      type = 'SHOT'
      n = 2
      lprint = 0
      fno = 0
      lno = 9999999
      ftr = 0
      ltr = 999999
      nwrds = npars
      nlists = 0
      CALL getfil( 1, lun, token, istat )                               ! get a file for the parameters
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
  120        CALL getoke( token, nchars )                               ! get the value
             IF( nchars .EQ. 0 ) THEN
                 CALL rdline
                 ntokes = 0
                 GOTO 120
             ENDIF
             ntokes = ntokes + 1
             CALL upcase( token, nchars )
             IF( names(nparam) .EQ. 'TYPE' ) THEN
                 type = token(1:nchars)
                 GOTO 100
             ENDIF
             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
             IF( names(nparam) .EQ. 'N' ) n = areal
             IF( names(nparam) .EQ. 'LPRINT' ) lprint = NINT(areal)
             IF( names(nparam) .EQ. 'FNO' ) fno = NINT(areal)
             IF( names(nparam) .EQ. 'LNO' ) lno = NINT(areal)
             IF( names(nparam) .EQ. 'FTR' ) ftr = NINT(areal)
             IF( names(nparam) .EQ. 'LTR' ) ltr = NINT(areal)
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
      IF( type .EQ. 'SHOT' ) THEN
          itype = 1
      ELSEIF( type .EQ. 'TRACE' ) THEN
          itype = 2
      ELSE
          PRINT *,' ***  ERROR  *** TYPE must be SHOT or TRACE.'
          ierror = ierror + 1
      ENDIF
      IF( n .LT. 0 ) THEN
          PRINT *,' ***  ERROR  *** N must be positive.'
          ierror = ierror + 1
      ENDIF
!****
!****   Write the parameter list to disk
!****
      scr(1) = itype
      lscr(2) = fno
      lscr(3) = lno
      lscr(4) = ftr
      lscr(5) = ltr
      lscr(6) = lprint
      lscr(7) = n
      nlists = nlists + 1
      CALL wrdisc( lun, scr, nwrds )
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,' itype=',itype,' fno=',fno,' lno=',lno,' ftr=',ftr,
     &            ' ltr=',ltr,' n=',n
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
