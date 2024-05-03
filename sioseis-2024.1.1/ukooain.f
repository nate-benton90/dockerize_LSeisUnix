      SUBROUTINE ukooain( navfil, ihdr, lhdr, hdr, istat )
!     Read an UKOOA P190 nav file already opened with a Fortran read
!  Arguments:
!    navfil - Fortran unit number of the opened UKOOA file.
!    ihdr - integer*2 SEGY header
!    lhdr - integer*4 SEGY header
!    hdr - real*4 SEGY header
!    istat - status flag.   =0, ok.      = 1, failed
!
!   ukooain puts the ukooa shot & receiver coordinates and streamer
! depth in the segy header for the trace in lhdr(19-22)
!   Also computes the shot-receiver range.
!   Also computes the rp number
!   SEGY header changed:
!   lhdr(6) = rpno
!   lhdr(7) = 0
!   lhdr(10) = range
!   lhdr(11) = receiver depth
!   lhdr(16) = water depth
!   ihdr(45) = type of coordinates (1=distance/length from origin (itype 8)
!                                  (1=whatever the P190 is (itype 10)
!                                  (3=source in decimal degrees (itype 19)
!                                  
!   ihdr(36) = -10  (scalar used for the following coordinates)
!   lhdr(19) = shot easting
!   lhdr(20) = shot northing
!   lhdr(21) = receiver easting
!   lhdr(22) = receiver northing
!
!   Call ukooain for every trace.
!   Don't play with the ukooa file betweeen calls to ukooain.
!
!   S line name, shot number, easting & northing for shot, time
!   R channel number, easting & northing (meters), receiver depth (meters)
!
! Copyright (c) the Regents of the University of California
! Written by Paul Henkart, Scripps Institution of Oceanography March 2000
!
!  mod Jul 02 - Put the water depth and receiver depth in the SEG-Y header
!             - Also Use NINT when stuffing into the SEG-Y trace header
!  mod 4 Jun 03 - Use UKOOA coordinates in SEG-Y trace header when TYPE 10
!  mod 19 Jun 03 - Non-metric grid reads were wrong.
!  mod 9 Aug 03 - Typo in dbrps made bad rp calculation.
!  mod 16 Oct 03 - Use rline1 rather than rline to prevent walking over rdline.
!  mod 28 Jan 04 - Add argument istat
!  mod 15 Jun 05 - Use the SEG-Y scalar in short word 36 to preserve the
!                  UKOOA precision of tenths of meters.
!                - sh_northing and sh_easting need to be real*8!
!  mod 14 Aug 07 - can't declare and set in same statement.
!  mod 14 Sep 10 - Get angular_units from H2002 and read lat and long.
!                - Use REAL degrees
!  mod 23 Sep 10 - Make H2000, H2001, H2002 less finicky
!  mod 2 Dec 10 - Read the first shot regardless of shot number when the segy
!                 shot number in lbuf(3) is less than 0.
!  mod 3 Mar 11 - Print warning if first line doesn't start with 'H'
!  mod 18 Apr 11 - Increase MAXTR to 2000 (4 * 468 = 1872) from 500
!  mod 21 Apr 11 - angular_units was not preset
!                - find the origin using just this streamer
!  mod 23 May 12 - Keep a copy of cbuf because others (e.g. segddin) also use rline1
!  mod 24 May 12 - rpno was bad
!  mod 27 Jun 12 - Make source_lat & source_long computations DOUBLE
!                - The UKOOA channel numbers may decrease instead of increase.
!                - The origin was not set due to 21 Apr 11 mod.
!  mod 11 Jul 12 - Print a warning if the shot-to-shot distance of the tailbuoy and shot differ.
!  mod 8 Aug 12  - Mac didn't compile     DFLOAT(sec)/(60.D0*60.D0)
!  mod 17 Nov 12  - Mac didn't compile     DFLOAT(sec)/(60.D0*60.D0) - tail buoy
!
      PARAMETER (MAXTR = 2000)
      COMMON /SIOLN3/ CBUF1
      COMMON /sioln4/ ICHAR1, NCHARS, iprint, lunpo
      INTEGER ichar1, nchars, iprint, i, lunpo
      CHARACTER*200 CBUF1, cbuf
      CHARACTER*1 alpha
      REAL hdr(111)
      REAL*8  source_lat, source_long
      INTEGER angular_units/1/
      INTEGER*4 lhdr(111), rpno
      INTEGER*2 ihdr(111)
      COMMON /segyptr/ llsegptr, lrseqptr, lshotptr, lshtrptr, lrpnptr,
     *                 lrptrptr, itridptr, ldisptr,  lwbdptr,  lsxcoptr,
     *                 lrxcoptr, idelmptr, istmptr,  iendmptr, isampptr,
     *                 isiptr,   iyrptr,   idayptr,  ihrptr,   iminptr,
     *                 isecptr,  igmtptr,  ldelsptr,  lsmusptr,lemusptr,
     *                 lsisptr,  lwbtsptr, lgatptr,  lssmsptr, lesmsptr,
     *                 lsbptr,   ifoldptr, icvleptr, lespnptr, ilagaptr,
     *                 ilagbptr
      COMMON /RPCALC/ dfls, dbrps, smear, itype
      COMMON /readt/ ilun, numhdr, numdat, iunhdr, ireel, intrcs
      CHARACTER*40 line_name
      CHARACTER*4 blank4
      DATA blank4/'    '/
      REAL ddfls, ddbrps, range
      REAL*8 r_eastings(MAXTR), r_northings(MAXTR)
      REAL*8 east_origin, north_origin, dtemp, dtemp1, dtemp2
      REAL*8 sh_easting, sh_northing, xr_to_origin, xs_to_origin
      REAL*8 sh_east_old/0./, sh_north_old/0./
      REAL*8 sh_east_new/0./, sh_north_new/0./
      REAL*8 tb_east_old/0./, tb_north_old/0./
      REAL*8 tb_east_new/0./, tb_north_new/0./
      REAL r_depths(MAXTR)
      DATA lastshot/-1/, metric_grid/1/, metric_height/1/, ireceiver/0/
      DATA east_origin/-99999./, north_origin/-99999./, isign/0/
      SAVE
!
      istat = 0
      IF( lastshot .LT. 0 ) THEN
          ddfls = dfls
          ddbrps = dbrps
          REWIND navfil
!         read to the first non H record.
          CALL rline1( navfil )
          cbuf = cbuf1
          IF( cbuf(1:1) .NE. 'H' .AND. lastshot .NE. -2 ) PRINT *,
     &        ' ***  WARNING  ***   NAVFIL does not look like UKOOA.'
          REWIND navfil
          lastshot = -2
  100     CONTINUE
          CALL rline1( navfil )
          cbuf = cbuf1
          IF( cbuf(1:1) .EQ. 'H' ) THEN
              IF( cbuf(2:5) .EQ. '2000' .AND.
     &            cbuf(33:33) .EQ. '2' ) metric_grid = 2
!****  Only possibilities are 1 or 2, so preset to 1 and set only if 2
!****  I encountered tabs rather than blanks, so READ bombed
!     &            READ( cbuf(33:33), '(I1)' ) metric_grid
              IF( cbuf(2:5) .EQ. '2001' .AND.
     &             cbuf(33:33) .EQ. '2' ) metric_height = 2
!     &            READ( cbuf(33:33), '(I1)' ) metric_height
             IF( cbuf(2:5) .EQ. '2002' .AND.
     &            cbuf(33:33) .EQ. '2' ) angular_units = 2
!    &            READ( cbuf(33:33), '(I1)' ) angular_units
              GOTO 100
          ENDIF
      ENDIF
!****
!****   Read until we find the right shot number.  Assume the UKOOA shot
!**** numbers increase and the SEG-Y shot numbers increase.
!****   We might have the shot read, so check that before reading!
!****
!****   Then read all the UKOOA receiver information.
!****   SIOSEIS needs the RP numbers to be increasing, so we need to
!**** know if the eastings/northings increase or decrease on
!**** successive shots.  So, read the second shot and find out!
!****
  200 CONTINUE
      IF( cbuf(1:1) .EQ. 'S' ) THEN
          IF( lhdr(3) .EQ. long_shotno .AND. ireceiver .EQ. 1) GOTO 1000
          IF( lhdr(3) .LT. 0 .AND. ireceiver .EQ. 1) GOTO 1000
          line_name = cbuf(2:13)
          READ( cbuf(20:25), '(I6)' ) long_shotno
          lastshot = long_shotno
          IF( lhdr(3) .EQ. long_shotno .OR. lhdr(3) .LT. 0 ) THEN
              IF( angular_units .EQ. 1 ) THEN
                  READ( cbuf(26:35), '(2(I2), F5.2, A1)') 
     &                  ideg,min,sec,alpha
                  dtemp = sec      ! Mac insists on DFLOAT(integer)
                  source_lat = DFLOAT(ideg) + DFLOAT(min)/60.D0 + 
     &                 dtemp/(60.D0*60.D0)
!     &                DFLOAT(sec)/(60.D0*60.D0)
                  IF( alpha .EQ. 'S' ) source_lat = -source_lat
                  READ( cbuf(36:46), '(I3, I2, F5.2, A1)') 
     &                  ideg,min,sec,alpha
                  dtemp = sec      ! Mac insists on DFLOAT(integer)
                  source_long= DFLOAT(ideg) + DFLOAT(min)/60.D0 +
     &                 dtemp/(60.D0*60.D0)
!     &                 DFLOAT(sec)/(60.D0*60.D0)
                  IF( alpha .EQ. 'W' ) source_long = -source_long
              ELSE 
                  READ( cbuf(26:35), '(F9.6,A1)') source_lat, alpha
                  IF( alpha .EQ. 'S' ) source_lat = -source_lat
                  READ( cbuf(36:46), '(F10.6,A1)') source_long, alpha
                  IF( alpha .EQ. 'S' ) source_long = -source_long
              ENDIF
              IF( metric_grid .EQ. 1 ) THEN
                  READ( cbuf(47:55), '(F9.1)' ) sh_easting
                  READ( cbuf(56:64), '(F9.1)' ) sh_northing
                  IF( cbuf(65:70) .NE. ' ' )
     &                READ( cbuf(65:70), '(F6.1)' ) wdepth
              ELSE
                  READ( cbuf(47:55), '(I9)' ) sh_easting
                  READ( cbuf(56:64), '(I9)' ) sh_northing
                  IF( cbuf(65:70) .NE. ' ' )
     &                READ( cbuf(65:70), '(I6)' ) wdepth
              ENDIF
              READ( cbuf(71:73), '(I3)' ) jday
              DO i = 1, MAXTR
                 r_eastings(i) = 0.
                 r_northings(i) = 0.
                 r_depths(i) = 0.
              ENDDO
              ireceiver = 0
              sh_east_old = sh_east_new
              sh_north_old = sh_north_new
              sh_east_new = sh_easting
              sh_north_new = sh_northing
          ENDIF
      ENDIF
      IF( lhdr(3) .NE. long_shotno .AND. lhdr(3) .GE. 0 ) THEN
          CALL rline1( navfil )
          cbuf = cbuf1
          IF( nchars .LT. 0 ) THEN
              PRINT *,' ***  ERROR  ***  NO UKOOA entry for shot ',
     &           lhdr(3)
              istat = 1
              REWIND navfil
              RETURN
          ENDIF
          GOTO 200
      ENDIF
      IF( cbuf(1:1) .EQ. 'T' ) THEN
          IF( angular_units .EQ. 1 ) THEN
              READ( cbuf(26:35), '(2(I2), F5.2, A1)')
     &              ideg,min,sec,alpha
              dtemp = sec      ! Mac insists on DFLOAT(integer)
              tail2_lat = DFLOAT(ideg) + DFLOAT(min)/60.D0 +
     &                 dtemp/(60.D0*60.D0)
!     &            DFLOAT(sec)/(60.D0*60.D0)
              IF( alpha .EQ. 'S' ) tail2_lat = -tail2_lat
              READ( cbuf(36:46), '(I3, I2, F5.2, A1)')
     &              ideg,min,sec,alpha
              dtemp = sec      ! Mac insists on DFLOAT(integer)
              tail2_long= DFLOAT(ideg) + DFLOAT(min)/60.D0 +
     &                 dtemp/(60.D0*60.D0)
!     &             DFLOAT(sec)/(60.D0*60.D0)
              IF( alpha .EQ. 'W' ) tail2_long = -tail2_long
          ELSE
              READ( cbuf(26:35), '(F9.6,A1)') tail2_lat, alpha
              IF( alpha .EQ. 'S' ) tail2_lat = -tail2_lat
              READ( cbuf(36:46), '(F10.6,A1)') tail2_long, alpha
              IF( alpha .EQ. 'S' ) tail2_long = -tail2_long
          ENDIF
          tb_east_old = tb_east_new
          tb_north_old = tb_north_new
          IF( metric_grid .EQ. 1 ) THEN
              READ( cbuf(47:55), '(F9.1)' ) tb_east_new
              READ( cbuf(56:64), '(F9.1)' ) tb_north_new
          ELSE
              READ( cbuf(47:55), '(I9)' ) tb_east_new
              READ( cbuf(56:64), '(I9)' ) tb_north_new
          ENDIF
!****  Print a warning if the shot-to-shot distance between two tailbuoy
!****  positions is "significantly different" from the shots distance.
!****  The problem is ukooain determines a bad origin (and range and rp number)
!****  When the streamer is in a turn.  The user may not realize this!
          IF( tb_east_old + tb_north_old .NE. 0. ) THEN
              tb_dist = DSQRT( (tb_east_old - tb_east_new) **2 
     &                       + (tb_north_old - tb_north_new) **2 )
              sh_dist = DSQRT( (sh_east_old - sh_east_new) **2 
     &                       + (sh_north_old - sh_north_new) **2 )
              diff = tb_dist - sh_dist
              IF( diff .GT. 5. ) THEN
                  PRINT *,
     &' ***  WARNING  ***  Tailbuoy and shot moving at different speeds.
     &  In a Turn?'
                  PRINT *,' Shot ',lhdr(3),'Diff=',diff,' is > 5.',
     &                 ' Tailbuoy diff=',tb_dist, ' Shot diff=',sh_dist
              ENDIF
          ENDIF
      ENDIF
      IF( cbuf(1:1) .EQ. 'R' ) THEN
          ireceiver = 1
!   R channel number, easting & northing (meters), receiver depth (meters)
!2345678901234567890123456789012345678901234567890123456789012345678901234567890
!R   1  23046.0   8320.010.0   2  23029.0   8339.010.0   3  23013.0   8357.010.0
!*****  WATCH OUT for null (\n) terminating the line.  It's not a blank!
          IF( cbuf(5:5) .NE. ' ' .AND. ICHAR(cbuf(2:2)) .NE. 0 ) THEN
              READ( cbuf(2:5), '(I4)' ) ichan
              IF( metric_grid .EQ. 1 ) THEN
                  READ( cbuf(6:14), '(F9.1)' ) r_eastings(ichan)
                  READ( cbuf(15:23), '(F9.1)' ) r_northings(ichan)
              ELSE
                  READ( cbuf(6:14), '(I9)' ) r_eastings(ichan)
                  READ( cbuf(15:23), '(I9)' ) r_northings(ichan)
              ENDIF
              IF( metric_height .EQ. 1 ) THEN
                  READ( cbuf(24:27), '(F4.1)') r_depths(ichan)
              ELSE
                  READ( cbuf(24:27), '(I4)') r_depths(ichan)
              ENDIF
!	print *,r_eastings(ichan),r_northings(ichan), r_depths(ichan)
          ENDIF
          IF( cbuf(31:31) .NE. ' ' .AND.ICHAR(cbuf(28:28)).NE.0) THEN
              READ( cbuf(28:31), '(I4)' ) ichan
              IF( metric_grid .EQ. 1 ) THEN
                  READ( cbuf(32:40), '(F9.1)' ) r_eastings(ichan)
                  READ( cbuf(41:49), '(F9.1)' ) r_northings(ichan)
              ELSE
                  READ( cbuf(32:40), '(I9)' ) r_eastings(ichan)
                  READ( cbuf(41:49), '(I9)' ) r_northings(ichan)
              ENDIF
              IF( metric_height .EQ. 1 ) THEN
                  READ( cbuf(50:53), '(F4.1)') r_depths(ichan)
              ELSE
                  READ( cbuf(50:53), '(I4)') r_depths(ichan)
              ENDIF
!	print *,r_eastings(ichan),r_northings(ichan), r_depths(ichan)
          ENDIF
          IF( cbuf(57:57) .NE. ' ' .AND.ICHAR(cbuf(54:54)).NE.0) THEN
              READ( cbuf(54:57), '(I4)' ) ichan
              IF( metric_grid .EQ. 1 ) THEN
                  READ( cbuf(58:66), '(F9.1)' ) r_eastings(ichan)
                  READ( cbuf(67:75), '(F9.1)' ) r_northings(ichan)
              ELSE
                  READ( cbuf(58:66), '(I9)' ) r_eastings(ichan)
                  READ( cbuf(67:75), '(I9)' ) r_northings(ichan)
              ENDIF
              IF( metric_height .EQ. 1 ) THEN
                  READ( cbuf(76:79), '(F4.1)') r_depths(ichan)
              ELSE
                  READ( cbuf(76:79), '(I4)') r_depths(ichan)
              ENDIF
!	print *,r_eastings(ichan),r_northings(ichan), r_depths(ichan)
          ENDIF
      ENDIF
      CALL rline1( navfil )
      cbuf = cbuf1
      IF( nchars .GT. 0 ) GOTO 200
      IF( lhdr(3) .EQ. long_shotno .AND. ireceiver .EQ. 1) GOTO 1000
      istat = lastshot
      RETURN
!****
 1000 CONTINUE
!****
!****    Now calculate the RP number and RANGE.
!****
!**** Use the smallest shot/receiver easting/northing of the first
!**** SEG-Y shot as the origin.
      IF( east_origin .EQ. -99999. ) THEN
!****    If the first shot, assume the second shot is in cbuf.  See if
!****    the second shot is further or closer to the origin
         IF( metric_grid .EQ. 1 ) THEN
             READ( cbuf(47:55), '(F9.1)' ) dtemp
             READ( cbuf(56:64), '(F9.1)' ) dtemp1
         ELSE
             READ( cbuf(47:55), '(I9)' ) dtemp
             READ( cbuf(56:64), '(I9)' ) dtemp1
         ENDIF
         dtemp2 = dtemp*dtemp + dtemp1*dtemp1
         dtemp1 = sh_easting*sh_easting + sh_northing*sh_northing
         isign = 1
         IF( dtemp1 .GT. dtemp2 ) isign = -1
         east_origin = sh_easting
         north_origin = sh_northing
!  The origin is needed to calculate the rp number
!  Make the origin the receiver with the smallest range of that streamer
!         DO i = 1, MAXTR
!         DO i =  lhdr(4), lhdr(4) + intrcs -1
!*****  the ukooa trace numbers may increase or decrease
         DO i = 1, intrcs
            IF( r_eastings(i) .NE. 0 ) THEN
                dtemp2 = r_eastings(i)*r_eastings(i)
     &                  + r_northings(i)*r_northings(i)
                IF( isign .GT. 0 ) THEN
                    IF( dtemp2 .LT. dtemp1 ) THEN                       ! dtemp1 is the first shot the first time
                        east_origin = r_eastings(i)
                        north_origin = r_northings(i)
                        dtemp1 = dtemp2
                    ENDIF
                ELSE
                    IF( dtemp2 .GT. dtemp1 ) THEN
                        east_origin = r_eastings(i)
                        north_origin = r_northings(i)
                        dtemp1 = dtemp2
                    ENDIF
               ENDIF
            ENDIF
         ENDDO
      ENDIF
      ltrcno = lhdr(4)
!   xs_ is the distance of the source from the origin
!   xr_ is the distance of the receiver from the origin
      xs_to_origin = DSQRT( (sh_easting - east_origin) *
     &                     (sh_easting - east_origin)
     &                  +  (sh_northing - north_origin ) *
     &                     (sh_northing - north_origin ) )
      xr_to_origin = DSQRT( (r_eastings(ltrcno) - east_origin) *
     &                     (r_eastings(ltrcno) - east_origin)
     &                  +  (r_northings(ltrcno) - north_origin ) *
     &                     (r_northings(ltrcno) - north_origin ) )
!   The following is valid, don't know why it was changed
      range = DSQRT( (r_eastings(ltrcno) - sh_easting) *
     &               (r_eastings(ltrcno) - sh_easting)
     &             + (r_northings(ltrcno) - sh_northing) *
     &               (r_northings(ltrcno) - sh_northing) )
!     assume dbrps is half the group interval.  Do some rounding!
      rpno = NINT((xs_to_origin + xr_to_origin + ddbrps) /2./ddbrps)+1.
!      range = xr_to_origin - xs_to_origin
      lhdr(6) = rpno
      lhdr(7) = 0
      lhdr(10) = NINT(range)
!****  SEGY says the receiver elevation, not depth.  i.e. depth is a negative number
      lhdr(11) = -NINT(r_depths(ltrcno))
!      lhdr(13) = source depth which is not in UKOOA P1/90
      lhdr(16) = NINT(wdepth)
!****  ukooain called with itype 8, 10, 13
      IF( itype .EQ. 8 ) THEN
! type 8 = UKOOA as distance from from origin
          ihdr(45) = 1                                                  ! units are "length"
!     The SEGY scalar is negative if the coordinate should be divided
!     by the scalar.  We multiply by 10 here, so the next guy divides.
          ihdr(36) = -10.
          lhdr(19) = NINT((sh_easting - east_origin) * 10. )
          lhdr(20) = NINT((sh_northing - north_origin) * 10. )
          lhdr(21) = NINT((r_eastings(ltrcno) - east_origin) * 10. )
          lhdr(22) = NINT((r_northings(ltrcno) - north_origin) * 10. )
      ENDIF
      IF( itype .EQ. 10 ) THEN
! type 10 = write UKOOA coordinates in SEG-Y header
          ihdr(45) = 1                                                  ! units are "length"
          ihdr(36) = -10.
          lhdr(19) = NINT((sh_easting) * 10. )
          lhdr(20) = NINT((sh_northing) * 10. )
          lhdr(21) = NINT((r_eastings(ltrcno)) * 10. )
          lhdr(22) = NINT((r_northings(ltrcno)) * 10. )
      ENDIF
      IF( itype .EQ. 19 ) THEN
! type 19 = put the source lat/long into SEG-Y as real degrees
          ihdr(45) = 3
          ihdr(36) = 0
          hdr(19) = source_long
          hdr(20) = source_lat
      ENDIF
      RETURN
      END
