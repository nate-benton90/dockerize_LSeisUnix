      SUBROUTINE stacked
!                         PROCESS STACK
!                         ------- -----
!
!
!    DOCUMENT DATE: 22 December 1992
!
!         Process STACK adds consecutive traces, completing the sum when
!   the special SIOSEIS end-of-gather flag (header word 51 set to -1) is
!   detected.  The end-of-gather flag may be set by process GATHER,
!   process INPUT or DISKIN parameter NTRGAT, or process HEADER.
!         The summed trace is scaled or averaged by the number of
!   live trace samples contributing to each stacked trace.  I.E. mute
!   times are accounted for, as well as the number of live traces
!   in the summation.
!         STACK honors trace length changes within the gather as well as
!   changes of the deep water delay with the gather.
!         Some of the SEGY trace headers are modified by process STACK.
!   The rp number is always set to the rp number of the first trace of
!   the gather.
!   The rp trace number is always 1.
!   The X and Y shot coordinates are set to zero.
!   The number of stacked traces (cdp or fold) is set.
!   
!   
!  PARAMETER DICTIONARY
!  --------- ----------
!  header - The type of header replacement.  The SEGY trace header words
!           for the shot number, shot trace number, range and GMT are
!           the only header words affected.
!         = FIRST, The above header values are taken from the first
!                  trace of each gather.
!         = NORMAL, The shot number, the shot trace number, and the
!                  trace range are set to 0.  The GMT is of the first
!                  trace of each gather.
!         = LAST, The above header values are taken from the last
!                  trace of each gather.
!           Preset = NORMAL
!
!  END    - Terminates the parameter list.
!
!  Copyright (C) 1992 The Regents of the University of California
!  ALL RIGHTS RESERVED.
!
!  mod 28 aug 95 - allow parameter lprint
!  mod 25 Sep 05 - Add parameter NEW
!  mod 25 May 06 - Change NEW preset from 0 to 1
!
      PARAMETER ( npars = 3 )                                           ! the number of user parameters
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      DIMENSION scr(npars), lscr(npars)
      EQUIVALENCE (scr(1),lscr(1))
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /stack/ lheader, lprint, new
      DATA names / 'HEADER', 'LPRINT', 'NEW' /
!****
!****    Set the parameter presets and various variable presets
!****
      lheader = 2
      new = 1
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
             IF( names(nparam) .EQ. 'HEADER' ) THEN
                 IF( token(1:nchars) .EQ. 'FIRST' ) THEN
                     lheader = 1
                 ELSEIF( token(1:nchars) .EQ. 'NORMAL' ) THEN
                     lheader = 2
                 ELSEIF( token(1:nchars) .EQ. 'LAST' ) THEN
                     lheader = 3
                 ELSE
                     PRINT *,' ***  ERROR  ***  Illegal HEADER type.'
                     ierror = ierror
                 ENDIF
                 GOTO 100
             ENDIF
             IF( names(nparam) .EQ. 'NEW' ) THEN
                 IF( token(1:2) .EQ. 'NO' ) new = 0
                 IF( token(1:3) .EQ. 'OFF' ) new = 0
                 IF( token(1:1) .EQ. '0' ) new = 0
                 GOTO 100
             ENDIF
             CALL dcode( token, nchars, areal, istat )                  ! convert the alpha number to an internal machine number
             IF( istat .NE. 2 ) ierror = ierror + 1                     ! was the an error decoding it?
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
