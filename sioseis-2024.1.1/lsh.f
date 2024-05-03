      PROGRAM LSH 
!  Purpose:  List stuff from the first and last headers of an SEG-Y disk file.
!  Usage: lsh (list SEG-Y disk file) usage: lsh filename
!  Example:
!  71>lsh 0010_2307_LF.sgy
!  0010_2307_LF.sgy          Starts: day271 2307z, Ends: day272 0634z, data times:  0.000 to  7.000 secs.
!
! mod 1 Feb 2022 - move CHARACTER to be after DIMENSION due to unknown
!                  problem id getfil (diskio mknam) on Mac with gcc 9.0
      INTEGER*2 ibuf
      DIMENSION BUF(70000),IBUF(222),LBUF(222)
      CHARACTER*200 name, token
      EQUIVALENCE (BUF(1),IBUF(1)),(BUF(1),LBUF(1))
      DATA icompt/7/, ndone/0/, long/0/

      nargs = iargc()
      IF( nargs .LT. 1 .OR. nargs .GT. 4 ) THEN
          PRINT *,' lsh (list SEG-Y disk file) usage:',
     &      ' lsh filename '
          STOP
      ENDIF
      CALL getarg( 1, name )
      CALL getfil( 4, lun, name, istat )
      IF( istat .NE. 0 ) STOP
      token = name
      DO i = 80, 1, -1
         ichar = i
         IF( token(i:i) .EQ. '/' ) GOTO 10
      ENDDO
   10 name = token(i+1:80)
!
      IF( is_big_endian() .LT. 0 ) icompt = 4
      CALL rddisc( lun, buf, 800, istat )
      IF( istat .NE. 800 ) THEN
          PRINT *,' ***  ERROR  ***  disk file incorrect.',
     *        ' wanted 800 words, read ',istat,' words.'
          STOP
      ENDIF
      CALL rddisc( lun, buf, 100, istat )
      IF( istat .NE. 100 ) THEN
          PRINT *,' ***  ERROR  ***  disk file incorrect.',
     *            ' wanted 100 words, read ',istat,' words.'
          STOP
      ENDIF
      IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) CALL swap16( ibuf, 200 )
      idtype = ibuf(13)
!****  segy rev 1
      ltemp = ibuf(153) * 3200
      IF( ibuf(151) .GT. 256 .AND. ibuf(151) .LT. 512 .AND.
     &     ibuf(153) .GT. 0 ) CALL podiscb( lun, 2, ltemp )
!****  This will fail if the number of headers is unknown (ibuf(153) = -1)
  110 CONTINUE
  120 CALL rddisc( lun, buf, 60, istat)
	 itemp = 60
      IF( istat .EQ. -1 ) THEN
          IF( long1 .NE. 0 .OR. lat1 .NE. 0 ) THEN
              temp = lat1
              IF( scalar .GT. 0 ) temp = temp * scalar
              IF( scalar .LT. 0 ) temp = ABS(temp / scalar)
              CALL secsdms( 1, temp, latdeg1, latmin1, seclat1 )
              temp = long1
              IF( scalar .GT. 0 ) temp = temp * scalar
              IF( scalar .LT. 0 ) temp = ABS(temp / scalar)
              CALL secsdms( 1, temp, longdeg1, longmin1, seclong1 )
!   571663. 158 47  42.9820786
              temp = lat2
              IF( scalar .GT. 0 ) temp = temp * scalar
              IF( scalar .LT. 0 ) temp = ABS(temp / scalar)
              CALL secsdms( 1, temp, latdeg2, latmin2, seclat2 )
              temp = long2
              IF( scalar .GT. 0 ) temp = temp * scalar
              IF( scalar .LT. 0 ) temp = ABS(temp / scalar)
              CALL secsdms( 1, temp, longdeg2, longmin2, seclong2 )
          ENDIF
          IF( min .EQ. min1 .AND. isec .EQ. isec1 .AND.
     &        ihour .EQ. ihour1 .AND. jday .EQ. jday1 ) isec = isec + 1
          IF( latdeg1 .NE. 0 .AND. longdeg1 .NE. 0 .AND.
     &        IABS(latdeg1) .LT. 91 .AND. IABS(longdeg1) .LT. 181 ) THEN
              PRINT 900,name, jday1, ihour1, min1, isec1,
     &           latdeg1, latmin1, seclat1, longdeg1, longmin1, seclong1 
  900            FORMAT(A25,' Begins: day',I3,1x,I2.2,1H:,I2.2,1H:,I2.2,
     &           ', lat: ', I4,1x,I2,1x,F6.3,' long: ',I4,1x,I2,1x,F6.3)
              PRINT 910,name,jday, ihour, min, isec,
     &          latdeg2, latmin2, seclat2, longdeg2, longmin2, seclong2,
c23456789012345678901234567890123456789012345678901234567890123456789012
     &           stime, etime
  910         FORMAT(A25,'   Ends: day',I3,1x,I2.2,1H:,I2.2,1H:,I2.2,
     &           ', lat: ', I4,1x,I2,1x,F6.3,' long: ',I4,1x,I2,1x,F6.3,
     &           ' data times: ',F5.3,' to ',F6.3,' secs.')
          ELSE
              PRINT 950, name, jday1, ihour1, min1, isec1, 
     &                   jday, ihour, min, isec, stime, etime
  950            FORMAT(A25,' Begins: day',I3,1x,I2.2,1H:,I2.2,1H:,I2.2,
     &                      ' Ends: day',I3,1x,I2.2,1H:,I2.2,1H:,I2.2,
     &           ' data times: ',F5.3,' to ',F6.3,' secs.')
          ENDIF
          STOP
      ENDIF
      ndone = ndone + 1
      IF( istat .NE. 60 ) THEN
          PRINT *,' ***  ERROR  ***  disk file incorrect.',
     *            ' wanted 60 words, read ',istat,' words.'
          STOP
      ENDIF
      IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) THEN
          CALL swap32( lbuf(1), 7 )
          CALL swap16( ibuf(15), 1 )
          CALL swap32( lbuf(10), 1 )
          CALL swap32( lbuf(16), 1 )
          CALL swap32( lbuf(19), 4 )
          CALL swap16( ibuf(36), 1 )
          CALL swap16( ibuf(55), 5 )
          CALL swap16( ibuf(79), 6 )
      ENDIF
!      nsamps = ibuf(58)
!      IF( IAND(ibuf(58),32768) .NE. 0 ) 
!     &    nsamps = IAND(ibuf(58),32767) + 32768
!      IF( ibuf(58) .EQ. 32767 ) nsamps = lbuf(58)
      CALL ushort2long( ibuf(58), nsamps )
      si = ibuf(59) / 1000000.
      idelay = ibuf(55)
      IF( idelay .LT. 0 ) idelay = 65536 + ibuf(55)
      delay = FLOAT(idelay) / 1000.
      IF( ndone .EQ. 1 ) THEN
          stime = delay 
          etime = delay + si * FLOAT(nsamps)
          jday1 = ibuf(80)
          ihour1 = ibuf(81)
          min1 = ibuf(82)
          isec1 = ibuf(83)
          lat1 = lbuf(20)
          long1 = lbuf(19)
      ENDIF
!**** watch out for bad date/time, usually at the end of the file
      IF( ibuf(80) .GT. jday1-5 .AND. ibuf(80) .LT. jday1+4) THEN
          jday = ibuf(80)
          ihour = ibuf(81)
          min = ibuf(82)
          isec = ibuf(83)
      ENDIF
      stime = AMIN1(stime,delay)
      temp = delay + si * FLOAT(nsamps)
      etime = AMAX1(etime,temp)
      lat2 = lbuf(20)
      long2 = lbuf(19)
!**  Mac G95 doesn't like the following:
!      scalar = FLOAT(ibuf(36))
      scalar = ibuf(36)
      IF( idtype .EQ. 3 .OR. idtype .EQ. 4 ) THEN
          nbytes = nsamps * 2
      ELSE
          nbytes = nsamps * 4
      ENDIF
      IF( nsamps .GT. 0 ) THEN
          CALL rddiscb( lun, buf, nbytes, istat )
          IF( istat .NE. nbytes ) THEN
              PRINT *,' ***  ERROR  ***  disk file incorrect.',
     *            ' wanted',nbytes,' bytes, read ',istat,' bytes.'
              STOP
          ENDIF
      ENDIF
      GO TO 110
      END
