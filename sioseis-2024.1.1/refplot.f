      SUBROUTINE refplot( hscale, scr, buf, lbuf, frange, lrange, istop,
     &            prange, ishotskip, si, nsamps, magnitude )
!      Refraction plotting (non-equal trace spacing) is accomplished by
! making trplot think that there is a trace on every nib by having set
! trpin to nibs.  We figure out how many nibs there need to be between
! the last trace plotted and the current, then call TRPLOT that many times
! to create thet many "blank traces" (spaces).  This is neat because the
! 1 inch buffer gets flushed automatically and we can have very large gaps
!  between traces!  The only disadvantage is that we can' back up!
!
!   Watch out for truncation errors.  We must always measure the distance 
!  on the plot from the beginning of the plot rather than from the 
!  previous trace plotted.  Keep track of where we are on the plot.  
!  Return the range of the plot range rather than the trace range.
!
!  mod 29 Jul 06 - ranges < previous were NOT skipped.
!
      DIMENSION buf(111), lbuf(111), scr(111)
      COMMON /lanno/ lanno
      CHARACTER*8 LANNO
      COMMON /VERSAT/ NIBS,RNIBS
      INTEGER frange
      LOGICAL first
      SAVE
      DATA first/.TRUE./, ishit/0/, ntotal/0/, last/0/
!
!     variable dist is the range of the last trace plotted.
!     frange starts out as the user given range, but becomes the range 
!         of the first trace on the plot (the origin trace range).
!     ffrange is the original user given frange.
!     variable ishit is needed so that we can establish the direction
!          of the plot when frange/lrange is given.
!     ntotal is the total number of n, the number of rasters that have 
!          plotted so far.
!     prange is the plot range, or the range to the trace on the plot 
!         (which might be different from the trace range because of the 
!          truncation errors)
!
!****
!****   we don't plot the trace itself (plotex does), so create the
!****   blank traces at the end of the plot only if the trace has 
!****   already been plotted, which can happen only when istop = -1
!****
      IF( istop .LT. 0 ) THEN
          IF( lrange .EQ. 99999 ) RETURN
          n = ( lrange - frange ) / hscale * rnibs
          IF( n .LT. 0 ) n = -n
          n = n - ntotal - 1
          IF( n .GT. 0 ) THEN
              DO 500 i = 1, n
                 CALL trplot( scr, si, idummy, 0, 1, 2 )
  500         CONTINUE
         ENDIF
         frange = lrange
         RETURN
      ENDIF
!****
!****  create the spaces and return without doing the trace itself.
!****  We can't determine the direction when frange = 99999 until we
!****  get the second trace - thus ishit.
!****
      range = lbuf(10)
      ishotskip = 0
      IF( ishit .EQ. 0 ) THEN
          IF( ffrange .EQ. 99999 .AND. range .LT. dist ) idir = -1
          ishit = 1
      ENDIF
!****
!****   set range direction
!****
      IF( first ) THEN
          IF( frange .LE. lrange ) THEN
              idir = +1
          ELSE
              idir = -1
          ENDIF
          n = 0
          IF( frange .NE. 99999 ) THEN
              IF( idir .GT. 0 .AND. range .LT. frange ) THEN
                  ishotskip = 1
                  RETURN
              ENDIF
              IF( idir .LT. 0 .AND. range .GT. frange ) THEN
                  ishotskip = 1
                  RETURN
              ENDIF
              IF( idir .GT. 0 .AND. frange .LT. range )
     &                n = ( range - FLOAT(frange) ) / hscale * rnibs
              IF( idir .LT. 0 .AND. range .LT. frange )
     &                n = ( FLOAT(frange) - range ) / hscale * rnibs
              IF( n .NE. 0 ) THEN
                 DO 100 i = 1, n
                     CALL trplot( scr, si, nsamps, 0, 1, 2 )
  100             CONTINUE
              ENDIF
          ENDIF
          ntotal = n + 1
          ffrange = frange
          IF( frange .EQ. 99999 ) frange = range
          dist = range
          prange = frange + FLOAT(idir*(ntotal-1)) / rnibs * hscale
          first = .FALSE.
          ishit = 0
          RETURN
      ENDIF
!****
!****  A special case exists whereby the ranges are really the magnitude
!****  or absolute value of the range.  When this condition exists, 
!****  assume that the ranges are monotonically decreasing so that any
!****  range that increases means that it is negative.
!****
      IF( magnitude .NE. 0 .AND. range .GT. dist ) range = -range
!****
!****  We can't backup, so if this range has already gone by, skip it
!****
      IF( idir .GT. 0 .AND. range .LE. dist ) THEN
          ishotskip = 1
          RETURN
      ENDIF
      IF( idir .LT. 0 .AND. range .GE. dist ) THEN
          ishotskip = 1
          RETURN
      ENDIF
      IF( lrange .NE. 99999 ) THEN
          IF( idir .GT. 0 .AND. range .GT. lrange ) THEN
              ishotskip = 1
              RETURN
          ENDIF
          IF( idir .LT. 0 .AND. range .LT. lrange ) THEN
              ishotskip = 1
              RETURN
          ENDIF
      ENDIF
      n = NINT(( range - frange ) / hscale * rnibs)
!	print *,' n=',n,' range=',range,' frange=',frange,' ntotal=',
!     & ntotal
      IF( n .LT. 0 ) n = -n
      n = n - ntotal
      IF( n .GT. 0 ) THEN
          DO 300 i = 1, n
             CALL trplot( scr, si, idummy, 0, 1, 2 )
  300     CONTINUE
      ENDIF
      ntotal = ntotal + n + 1                                           ! don't forget the trace itself!
      IF( ntotal .EQ. last ) THEN
          ishotskip = 1
          RETURN
      ENDIF
      prange = frange + FLOAT(idir*(ntotal-1)) / rnibs * hscale
!	 print *,' range ',range,prange, ' plots at ',ntotal
      last = ntotal
      dist = range
      RETURN
      END
