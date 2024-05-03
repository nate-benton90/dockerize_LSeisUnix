      SUBROUTINE navgeom( navfil, ihead, lhead, dfls )
!  Return dfls and insert long/lat in SEG-Y header words 19, 20.
!     Compute and return DLFS (distance from last shot) from the SEG-Y
!  header and a navigation file whose format is:
!      yr day hr min sec deglat minlat deglong minlong
!  yr can be 2 or 4 digits (e,g, 1997 or 97)
!  day is day of year
!  hr is the 24 hour clock
!  min
!  sec is the floating point second of the fix.
!  deglat is the latitude in degrees.  Minus is South.
!  minlat is the decimal minutes of latitude
!  deglong is the longitude in degrees.  Minus is West.
!  longmin is the decimal minutes of longitide.
!
!  The entries are free field and may be blank or tab delimited.
!
!   Think SEG-Y and think GPS.
!  Assume a flat earth.
!  Assume a constant distance per degree latitude/longitude after
!      the first call.
!
!  mod Oct 00 - Add the illegal SEG-Y milliseconds
!             - make a bunch more variables REAL*8
!             - check for the shot being at the fix, esp with mils!
!  mod 17 Oct 02 - D0 negative lat/long on first one correctly!
!  mod 20 May 04 - Write the long/lat into the SEG-Y header
!                - Bad dfls when shot time = fix time
!  mod 24 May 04 - The scalar in word 36 had the wrong sign
!  mod 3 Aug 04 - Didn't handle longitude over 180/-180
!  mod 25 Aug 05 - Changed scalar from 10 to 100.
!  mod 29 Apr 10 - Check for mil changing (might be shooting fast!)
!  mod 5 May 10 - Change warning of shot-times to < 1 second.
!

      INTEGER*2 ihead(120)
      DIMENSION lhead(60)
      CHARACTER*10 token
      COMMON /sioln2/ ICHAR, NCHARS1, iprint, lunpo
      INTEGER ichar, nchars1, iprint, lunpo
      REAL*8 dlat, dlong, dlatlast, dlonglast, dtemp, dxm, dym, distm,
     &       timefix, timelast, timeshot, sec, speed, secs, scalar
      REAL*8 seclog, deglat, rminlat, deglong, rminlong, dpdlat, dpdlong
      LOGICAL first
      SAVE 
      DATA lastmin/-1/, timelast, timefix/2*400.D0/, scalar/100./
      DATA first/.TRUE./

!**** if same shot time
      IF( ihead(83) .EQ. lastsec .AND. ihead(82) .EQ. lastmin .AND.
     &    ihead(81) .EQ. ihr .AND. ihead(80) .EQ. lastday .AND.
     *    ihead(84) .EQ. lastmil ) THEN
          dtemp = speed * secs
          dfls = dtemp
          lhead(19) = NINT(dlong * 60.D0 * 60.D0 * scalar)
          lhead(20) = NINT(dlat * 60.D0 * 60.D0 * scalar)
          ihead(36) = -NINT(scalar)
          ihead(45) = 2
          RETURN
      ENDIF
      iday = ihead(80)
      ihr = ihead(81)
      min = ihead(82)
      isec = ihead(83)
      mil = 0
      IF( ihead(84) .GT. 4 ) mil = ihead(84)
      ishotno = lhead(3)
      IF( lastmin .GE. 0 .AND. ishotno .NE. lastshotno + 1 ) THEN
          PRINT *,' Non consecutive shot numbers ',lastshotno,ishotno
      ENDIF
      sec = DBLE(isec) + DBLE(mil)/1000.D0
      timeshot = DBLE(iday) + DBLE(ihr)/24.D0 +
     &           DBLE(min)/1440.D0 + sec/86400.D0
!	 print *,' segy ',ishotno,iday,ihr,min,isec,mil,timeshot
!      print *,lastshotno,lastday,lasthr,lastmin,lastsec,timelast,timefix
!**** First time through is really just setting up.
      IF( first ) THEN
          dfls = 0
          lastyr = ihead(79)
          lastday = ihead(80)
          lasthr = ihead(81)
          lastmin = ihead(82)
          lastsec = ihead(83)
!****     SEG-Y standard says word 84 is the time base (local vs GMT),
!****     SIOSEIS likes to put the millisecond in there
          lastmil = 0
          IF( ihead(84) .gt. 4 ) lastmil = ihead(84)
          lastshotno = lhead(3)
   10     CONTINUE
!****     get the fix of the first shot
!	 print *,' timeshot=',timeshot,' timefix=',timefix
          IF( timeshot .LT. timefix ) THEN
              dlatlast = dlat
              dlonglast = dlong
              timelast = timefix
              CALL rline( navfil )
              IF( nchars1 .LT. 1 ) THEN
                  PRINT *,' ***  ERROR  ***  SEGY time ', lastday,
     &              lasthr, lastmin, lastsec,' not in NAVFIL.'
                  RETURN
              ENDIF
              CALL getoke( token, nchars )
              CALL dcode( token, nchars, areal, istat )
              logyr = NINT(areal)
              CALL getoke( token, nchars )
              CALL dcode( token, nchars, areal, istat )
              logday = NINT(areal)
              CALL getoke( token, nchars )
              CALL dcode( token, nchars, areal, istat )
              loghr = NINT(areal)
              CALL getoke( token, nchars )
              CALL dcode( token, nchars, areal, istat )
              logmin = NINT(areal)
              CALL getoke( token, nchars )
              CALL ddcode( token, nchars, seclog, istat )
              CALL getoke( token, nchars )
              CALL ddcode( token, nchars, deglat, istat )
              CALL getoke( token, nchars )
              CALL ddcode( token, nchars, rminlat, istat )
              CALL getoke( token, nchars )
              CALL ddcode( token, nchars, deglong, istat )
              CALL getoke( token, nchars )
              CALL ddcode( token, nchars, rminlong, istat )
              timefix = DBLE(logday) + DBLE(loghr)/24.D0 +
     &                  DBLE(logmin)/1440.D0 + seclog/86400.D0
!	 print *,' read log1 ',logday, loghr, logmin, deglat,rminlat,
!     &                    deglong, rminlong, timefix
              GOTO 10
          ENDIF
          dlat = DABS(deglat) + rminlat/60.D0
          IF( deglat .LT. 0 ) dlat = -dlat
          dlong = DABS(deglong) + rminlong/60.D0
          IF( deglong .LT. 0 ) dlong = -dlong
!         GET DISTANCE (KM) PER DEGREE
          CALL dlendeg( dlat, dpdlat, dpdlong )
!	print *,' dlat=',dlat,' dpdlat=',dpdlat,' dpdlong=',dpdlong
          IF( timeshot .LE. timefix ) THEN
              first = .FALSE.
              RETURN
          ENDIF
      ENDIF
  100 CONTINUE
!	 print *,' at 100, shot ',timeshot,' fix ',timefix, first
      IF( timeshot .GT. timefix ) THEN
          dlatlast = dlat
          dlonglast = dlong
          timelast = timefix
          CALL rline( navfil )
          IF( nchars1 .LT. 1 ) THEN
              PRINT *,' ***  ERROR  ***  SEGY time ', lastday,
     &              lasthr, lastmin, lastsec,' not in NAVFIL.'
              lastshotno = ishotno
              RETURN
          ENDIF
          CALL getoke( token, nchars )
          CALL dcode( token, nchars, areal, istat )
          logyr = NINT(areal)
          CALL getoke( token, nchars )
          CALL dcode( token, nchars, areal, istat )
          logday = NINT(areal)
          CALL getoke( token, nchars )
          CALL dcode( token, nchars, areal, istat )
          loghr = NINT(areal)
          CALL getoke( token, nchars )
          CALL dcode( token, nchars, areal, istat )
          logmin = NINT(areal)
          CALL getoke( token, nchars )
          CALL ddcode( token, nchars, seclog, istat )
          CALL getoke( token, nchars )
          CALL ddcode( token, nchars, deglat, istat )
          CALL getoke( token, nchars )
          CALL ddcode( token, nchars, rminlat, istat )
          CALL getoke( token, nchars )
          CALL ddcode( token, nchars, deglong, istat )
          CALL getoke( token, nchars )
          CALL ddcode( token, nchars, rminlong, istat )
          dlat = DABS(deglat) + rminlat/60.D0
          IF( deglat .LT. 0 ) dlat = -dlat
          dlong = DABS(deglong) + rminlong/60.D0
          IF( deglong .LT. 0 ) dlong = -dlong
          timefix = DBLE(logday) + DBLE(loghr)/24.D0 +
     &              DBLE(logmin)/1440.D0 + seclog/86400.D0
!          print *,' read log2 ',logday, loghr, logmin, seclog,
!     &           deglat,rminlat, deglong, rminlong, timefix
!         CALCULATE THE LATITUDE RANGE (Y) IN METERS
          dym=(dlat-dlatlast)*dpdlat
!         CALCULATE THE LONGITUDE RANGE (X) IN METERS
          dxm=(dlong-dlonglast)*dpdlong
!        print *,' dlong ',dlong,dlonglast,dxm,dpdlong
          IF( DABS(dxm) .GT. 100000. ) THEN
!	 print *, DABS(dlong - dlonglast)
              IF( DABS(dlong - dlonglast) .GT. 180. ) THEN
                  IF( dlong .LT. 0. ) THEN
                      dtemp = 180. + (180.+dlong)
                      dxm=(dtemp-dlonglast)*dpdlong
                  ELSE
                      dtemp = 180. + (180.+dlonglast)
                      dxm=(dlong-dtemp)*dpdlong
                  ENDIF
              ENDIF
          ENDIF
!         CALCULATE THE RANGE (ASSUMING A FLAT EARTH)
          distm = DSQRT(dxm*dxm+dym*dym)
!	 print *,' dlat ',dlat,dlatlast,dym,dpdlat
!	 print *,' dlong ',dlong,dlonglast,dxm,dpdlong
!	print *,' distm=',distm
          GOTO 100
      ENDIF
!      IF( timeshot .EQ. timefix ) THEN
!          dtemp = distm
!      ELSE
          secs = DFLOAT(iday-lastday) * 24.D0 * 60.D0 * 60.D0 +
     &       DFLOAT(ihr-lasthr) * 60.D0 * 60.D0 +
     &       DFLOAT(min-lastmin) * 60.D0 +
     &       DFLOAT(isec-lastsec) +
     &       DFLOAT(mil-lastmil)/1000.D0
          IF( secs .LT. 1 .OR. secs .GT. 60 )THEN
              IF( .NOT. first ) PRINT *,
     &          ' Odd SEG-Y shot time on shot ',ishotno,' time ',
     &          iday, ihr, min, isec, secs
          ENDIF
!	 print *,' distm=',distm,' time ',(timefix-timelast)*86400.D0
          speed = distm / ((timefix-timelast)*86400.D0)
!	print *,' speed=',speed,' secs=',secs
          dtemp = speed * secs
!      ENDIF
      dfls = SNGL(dtemp)
      lastday = iday
      lasthr = ihr
      lastmin = min
      lastsec = isec
      lastmil = mil
      lastshotno = ishotno
      rlastlat = deglat
      rlastlong = deglong
!**** ibuf(36) is the "scalar to be applied to the coordinates in
!**** bytes 73-88 (lbuf(19), lbuf(20), lbuf(21), lbuf(22))
!**** lbuf(19) is "longitude in seconds of arc" of the shot
!**** lbuf(20) is "latitude in seconds of arc" of the shot
!**** ibuf(45) is the type of units - 2 = seconds of arc
      lhead(19) = NINT(dlong * 60.D0 * 60.D0 * scalar)
      lhead(20) = NINT(dlat * 60.D0 * 60.D0 * scalar)
      ihead(36) = -NINT(scalar)
      ihead(45) = 2
      first = .FALSE.
      RETURN
      END
