      SUBROUTINE segyxy( itype, hdr, lhdr, ihdr, lprint )
!     Calculate the range and rp number given the source and 
!  receiver coordinates in the SEG-Y header.
!
!  Written by Paul Henkart, Scripps Oceanography, August 2003
!  Copyright (c) 2003, The Regents of the University of California
!
!  mod 24 July 2006 - Add type 17 to insert the distance from the first
!        shot to the current shot as the SEG-Y range (word 10).
!                   - Set the RP number and end of gather flag
!  mod 3 Aug 06 - Do type as sum of distances between shots, killing
!                 traces where the distance from trace 1 decreases.
!  mod 14 Aug 97 - g95 can't dividew by a negative (a/-b)
!                - g05 can't declare and set in the same statement
!
      PARAMETER (NPTS=10)
      DIMENSION hdr(60), lhdr(60), ihdr(120)
      INTEGER*2 ihdr
      REAL*8 x_origin, y_origin, range, delta_x, delta_y,
     &       x_source, y_source, x_receiver, y_receiver, dtemp,
     &       ddlat, ddlong, dbpts, dlast, xlast, ylast, dtotal,
     &       pi, rad, deg, dangle, danglelast
      REAL*8 x(NPTS), y(NPTS)
      COMMON /rpcalc/ dfls, dbrps, smear
      LOGICAL first
      DATA first/.TRUE./
      DATA dlast/0/, danglelast/999./, num_out/1/
     &  ifirst/0/, dist_max/0./, last_rp/1/
      DATA pi/3.141592653589/, rad/57.2957795131/, deg/.0174532925199/
      SAVE
!
! ihdr(45) = coordinate units.
!            1 = length (meters or feet)
!            2 = seconds of arc
!            3 = decimal degrees
!            4 = DDDMMSS
! ihdr(36) = coordinate scalar.  < 0 means divisor, >0 means multiply
! lhdr(19) = source x or longitude
! lhdr(20) = source y or latitude
! lhdr(21) = receiver x or longitude
! lhdr(22) = receiver y or latitude
!
      dtemp = DFLOAT(ihdr(36))
      IF( ihdr(45) .EQ. 1 ) THEN
          IF( first ) THEN
              first = .FALSE.
              IF( itype .EQ. 17 ) THEN
                  x_origin = lhdr(19)
                  y_origin = lhdr(20)
              ELSE
                  x_origin = lhdr(21)
                  y_origin = lhdr(22)
              ENDIF
              IF( ihdr(36) .LT. 0 ) THEN
                  x_origin = -x_origin / dtemp
                  y_origin = -y_origin / dtemp
              ENDIF
              IF( ihdr(36) .GT. 0 ) THEN
                  x_origin = x_origin * dtemp
                  y_origin = y_origin * dtemp
              ENDIF
              dbrps2 = ABS(dbrps) / 2.
          ENDIF
          x_source = lhdr(19)
          y_source = lhdr(20)
          x_receiver = lhdr(21)
          y_receiver = lhdr(22)
          IF( ihdr(36) .LT. 0 ) THEN
              x_source = -x_source / dtemp
              y_source = -y_source / dtemp
              x_receiver = -x_receiver / dtemp
              y_receiver = -y_receiver / dtemp
          ENDIF
          IF( ihdr(36) .GT. 0 ) THEN
              x_source = x_source * dtemp
              y_source = y_source * dtemp
              x_receiver = x_receiver * dtemp
              y_receiver = y_receiver * dtemp
          ENDIF
          IF( itype .EQ. 11 ) THEN
              delta_x = x_source - x_receiver
              delta_y = y_source - y_receiver
              range = DSQRT( delta_x * delta_x + delta_y * delta_y )
              delta_x = (x_source + x_receiver) / 2. - x_origin
              delta_y = (y_source + y_receiver) / 2. - y_origin
              distance = DSQRT( delta_x * delta_x + delta_y * delta_y )
              rpno = (distance + dbrps2) / dbrps
              lhdr(6) = NINT(rpno)
              lhdr(10) = NINT(range)
          ENDIF
          RETURN
      ENDIF
!****
!****    In seconds of arc
!****
      IF( ihdr(45) .EQ. 2 ) THEN
          IF( first ) THEN
              first = .FALSE.
              IF( itype .EQ. 17 ) THEN
                  x_origin = (DFLOAT(lhdr(19)) / (60.D0 * 60.D0 ))
                  y_origin = (DFLOAT(lhdr(20)) / (60.D0 * 60.D0 ))
              ELSE
                  x_origin = (DFLOAT(lhdr(21)) / (60.D0 * 60.D0 ))
                  y_origin = (DFLOAT(lhdr(22)) / (60.D0 * 60.D0 ))
              ENDIF
              IF( ihdr(36) .GE. 0 ) THEN
                  y_origin = y_origin * dtemp
                  x_origin = x_origin * dtemp
              ELSE
                  y_origin = -y_origin / dtemp
                  x_origin = -x_origin / dtemp
              ENDIF
              DO i = 1, NPTS
                 x(i) = x_origin
                 y(i) = y_origin
              ENDDO
          ENDIF
          x_source = (DFLOAT(lhdr(19)) / (60.D0 * 60.D0 ))
          y_source = (DFLOAT(lhdr(20)) / (60.D0 * 60.D0 ))
          x_receiver = (DFLOAT(lhdr(21)) / (60.D0 * 60.D0 ))
          y_receiver = (DFLOAT(lhdr(22)) / (60.D0 * 60.D0 ))
          IF( ihdr(36) .GE. 0 ) THEN
              y_source = y_source * dtemp
              x_source = x_source * dtemp
              y_receiver = y_receiver * dtemp
              x_receiver = x_receiver * dtemp
          ELSE
              y_source = -y_source / dtemp
              x_source = -x_source / dtemp
              y_receiver = y_receiver / dtemp
              x_receiver = x_receiver / dtemp
          ENDIF
!****   x = long,   y = lat
          CALL dlendeg( y_source,ddlat,ddlong)
          IF( itype .EQ. 11 ) THEN
              delta_x = (x_source - x_receiver) * ddlong
              delta_y = (y_source - y_receiver) * ddlat
              range = DSQRT( delta_x * delta_x + delta_y * delta_y )
              delta_x = (x_source + x_receiver) / 2. - x_origin
              delta_y = (y_source + y_receiver) / 2. - y_origin
              distance = DSQRT( delta_x * delta_x + delta_y * delta_y )
              rpno = (distance + dbrps2) / dbrps
              lhdr(6) = NINT(rpno)
              lhdr(10) = NINT(range)
          ENDIF
          IF( itype .EQ. 17 ) THEN
              delta_x = (x_source - x_origin) * ddlong
              delta_y = (y_source - y_origin) * ddlat
              distance = DSQRT( delta_x * delta_x + delta_y * delta_y )
              IF( ifirst .EQ. 0. ) THEN
                  ifirst = 1
                  dbpts = distance
                  dtotal = distance
                  danglelast = DATAN2(delta_y,delta_x) * rad
              ELSE
                  delta_x = (x_source - xlast) * ddlong
                  delta_y = (y_source - ylast) * ddlat
                  dbpts = DSQRT( delta_x * delta_x + delta_y * delta_y )
              ENDIF
              DO i = NPTS, 2, -1
                 x(i) = x(i-1)
                 y(i) = y(i-1)
              ENDDO
              x(1) = x_source
              y(1) = y_source
              delta_x = x(NPTS) - x(1)
              delta_y = y(NPTS) - y(1)
              dangle = DATAN2(delta_y,delta_x) * rad
!              IF( dangle .LT. 0 ) dangle = dangle + 360.D0
!              IF( dangle .LT. -90 .OR. dangle .GT. 90 ) dbpts = -dbpts
!      print *,' dx=',delta_x,' dy=',delta_y,' dangle=',dangle,danglelast
              IF( danglelast .EQ. 999. ) danglelast = dangle
              IF( DABS(dangle-danglelast) .GT. 90. ) dbpts = -dbpts
              danglelast = dangle
!****         dtotal is the distance traveled along the ship track, including
!****               backing up.
!****         dist_max is like dtotal, but excludes backing up.
              dtotal = dtotal + dbpts
              lhdr(7) = 0
              lhdr(51) = 0
              lhdr(10) = NINT(dtotal)
!****         throw out (set the kill flag) if this rp is behind.
!	print *,' dtotal=',dtotal,' dist_max=',dist_max
              IF( dtotal .GT. dist_max ) THEN
                  temp = (dtotal + dbrps2) / dbrps
                  lhdr(6) = NINT(temp) + 1
              ELSE
                  ihdr(15) = 2
                  lhdr(6) = last_rp
              ENDIF
!	print *,' last_rp=',last_rp,' lhdr(6)=',lhdr(6)
              IF( lhdr(6) .GT. last_rp ) THEN
!****     The end-of-gather flag is one trace too late
                  lhdr(51) = -1
                  lhdr(5) = lhdr(3)
                  lhdr(6) = last_rp
                  last_rp = NINT(temp) + 1
!	print *,' set -1, last_rp=',last_rp
              ENDIF
              IF( dtotal .GT. dist_max ) dist_max = dtotal
!****         What do we do with gaps?  What if this cdp is more than dbrps
!****         from the previous one?  i.e. the rp number will not be monotonic.
!              IF( last_rp .EQ. 0 .OR. last_rp .NE. lhdr(6) ) THEN
!                  lhdr(6) = num_out
!                  num_out = num_out + 1
!              ENDIF
              xlast = x_source
              ylast = y_source
              dlast = dbpts
              IF( IAND(lprint,512) .NE. 0 ) THEN
                  PRINT *, ' shot=',lhdr(3),' dbpts=', dbpts,
     &    ' dangle=',dangle 
              ENDIF
          ENDIF
          RETURN
      ENDIF
!****
!****   Decimal degrees
!****
      IF( ihdr(45) .EQ. 3 ) THEN
          IF( first ) THEN
              first = .FALSE.
              IF( itype .EQ. 17 ) THEN
                  x_origin = hdr(19)
                  y_origin = hdr(20)
              ELSE
                  x_origin = hdr(21)
                  y_origin = hdr(22)
              ENDIF
              IF( ihdr(36) .LT. 0 ) THEN
                  x_origin = -x_origin / dtemp
                  y_origin = -y_origin / dtemp
              ENDIF
              IF( ihdr(36) .GT. 0 ) THEN
                  x_origin = x_origin * dtemp
                  y_origin = y_origin * dtemp
              ENDIF
              CALL dlendeg( y_origin,ddlat,ddlong)
          ENDIF
          x_source = hdr(19)
          y_source = hdr(20)
          x_receiver = hdr(21)
          y_receiver = hdr(22)
          IF( ihdr(36) .LT. 0 ) THEN
              x_source = -x_source / dtemp
              y_source = -y_source / dtemp
              x_receiver = -x_receiver / dtemp
              y_receiver = -y_receiver / dtemp
          ENDIF
          IF( ihdr(36) .GT. 0 ) THEN
              x_source = x_source * dtemp
              y_source = y_source * dtemp
              x_receiver = x_receiver * dtemp
              y_receiver = y_receiver * dtemp
          ENDIF
          delta_x = (x_source - x_receiver) * ddlong
          delta_y = (y_source - y_receiver) * ddlat
          range = DSQRT( delta_x * delta_x + delta_y * delta_y )
          delta_x = (x_source + x_receiver) / 2. - x_origin
          delta_y = (y_source + y_receiver) / 2. - y_origin
          distance = DSQRT( delta_x * delta_x + delta_y * delta_y )
          IF( itype .EQ. 17 ) THEN
              lhdr(10) = NINT(distance)
              IF( dbrps .GT. 0 ) THEN
                  lhdr(7) = 0
                  lhdr(51) = 0
                  temp = distance / dbrps
                  lhdr(6) = NINT( temp ) + 1
                  IF( distance .NE. dist_max .AND. dist_max .NE. 0 )
     &                lhdr(51) = -1
                  dist_max = lhdr(6)
              ENDIF
          ELSE
              rpno = (distance + dbrps2) / dbrps
              lhdr(6) = NINT(rpno)
              lhdr(10) = NINT(range)
          ENDIF
          RETURN
      ENDIF
      END
