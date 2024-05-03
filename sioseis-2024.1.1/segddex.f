      SUBROUTINEsegddex( buf, lbuf, ibuf, scr, lscr, iscr, istop )
! SEG-D disk input - similar to segdex
!
! (C) The Regents of the University of California
! Written by Paul Henkart, July 2002
!
! mod ?? Aug 02 - Increase lat/long field size in log file.
! mod 19 Sep 02 - Allow the files in a stack not have / as the first
!                 character to allow symbolic links.
! mod 21 Sep 02 - Write a message in the log file when newfile is set.
! mod 29 Sep 02 - Don't save/use bad water depths.
! mod 4 Oct 02 - Get the line name from the LDEO header (don't know
!                what to do with it though!)
! mod 8 Oct 02 - ftr/ltr and ipath didn't terminate correctly
! mod 19 Mar 03 - Always write the general header to luntr0 and
!                 write the ldeo external header to lunldeo if present.
! mod 20 Mar 03 - Move ns_done count to be after stack wait.
! mod 21 Mar 03 - Do the log file differently if ldeo_shotno = 0
! mod 8 Jul 03 - Seisnet failed because the seisnet header was a different
!                size than expected.  Find the first trace differently!
! mod 6 Mar 04 - Allow format 8036 - 24 bit integers
! mod 29 Apr 04 - Use the record length from GH#2 if rlen = FFF in GH#1.
!               - Decode the number of General Header blocks
!               - Partially set up the SIO-RT nav and depth files
! mod 4 May 04 - nsamps calculated from GH#2 on PC-Linux need explicit
!                NINT(FLOAT(length)/si*1000.)
! mod 5 May 04 - Get the nav from the extended header if its starts 
!                with $GPGGA
! mod 6 June 04 - Honor SEG-D Rev. 1 Expanded file number
!               - Start looking at the MP Factor - descaling factor.
! mod 27 June 04 - Apply the MP descaling factor.
! mod 14 Sep 04 - Extended file number was wrong.
! mod 19 Sep 04 - The MP factor is sign AND magnitude!
! mod 27 Sep 04 - Make up a shot number if the LDEO shot number = 0.
! mod 13 Jun 05 - Set newfile = 0 on every call/trace.
!               - lprint 32 means print the External header in ASCII.
!               - remove the sio realtime stuff since it won't happen!
!               - Add a call to Lee Ellett's External header.
! mod 28 Jul 05 - Add lprint of the K constants.
! mod 31 Jan 06 - Put the SIO lat/long in the SEG-Y header regardless of
!                 it's time of fix.  i.e. ignore time of fix.
! mod 12 Oct 06 - Set ship_lat and ship_long to 0 on every shot before
!                 doing the external header in case it's not there.
!               - Get rid of Healy05 and Healy06 calls.
! mod 16 Oct 06 - Generalize $GPGGA to be NMEA strings in general.
!               - Use $DBT for NMEA water depth.
!               - Get rid of Leeshdr.
! mod 10 Jan 06 - stack didn't honor extended shot numbers (> 9999)
! mod 18 July 07 - Allow $SDDBT in addition to $DBT
! mod 14 Aug 07 - g95 IAND requires arguments to be same type and kind.
!               - g95 can't declare type and set value on same statement
! mod 17 Apr 08 - The LDEO external header was broken.
! mod 21 Apr 08 - Change iformat to 6 (LDEO) if LDEO external header ($1)
!               - Add nav to non-ldeo log file.
! mod 19 May 08 - Write the water depth to real word 54 (why was it commented out?)
! mod 20 May 08 - The extended file number was screwed up
!               - FLUSH the log so it can been seen immediately. NOT ON LANGSETH
! mod 7 July 08 - Reset ftr on each channel set
! mod 17 Jul 08 - Add shot_inc (odd/even) to STACK
! mod 24 Jul 08 - Use lrshift to shift a long integer (filen in additional block)
! mod 28 Jul 08 - Allow shooting the line backwards with decreasing shot numbers
!               - but catch a ldgo_shot of 0
! mod 24 May 10 - Use lbuf(16) rather than buf(54) for water depth
! mod 22 Jul 11 - Mess with ***  WARNING  *** Water depth has not changed in 50 shots.
!               - FLUSH (fortran function) doesn't work on cygwin
! mod 18 Nov 11 - Redo reading NMEA strings from the external header
! mod 21 Nov 11 - lrshift (and c) don't zero the vacated bits on right shifts
!  mod 19 Dec 11 - Honor file "in" when STACK is used.
! mod 13 Feb 15 - Very large external header caused buffer overflow.
!               - print warning if format HTI is given and data are segd rev 1
! mod 6 Jun 15 - Honor extended number of channel sets in general header #2
! mod Feb 16 - really the change is in gpgga for bad Geo Eel gps strings
! mod 29 Mar 16 - Big modify to gpgga & added arguments mils & istat
!               - put the mils in SEGY (84) if mils is available (it's 9999 if not)
! mod 26 Nov 19 - use integer*2 itohex rather than tohex
!               - get rid of Hydroscience's ncsets 165 - problem with fortran99 and equivalence
! mod 21 Jan 20 - convert comment lines from c to !
!               - Add format 8024 as per Alistair
!               - Add skipping 4 bytes for format ldeo for 2019 Langseth cruise
!                 which breaks older format ldeo code, but that probably doesn't matter
!               - Do the descalar for SEG-D Rev 2
!               - Change DO CONTINUE to DO ENDDO
! mod 14 Apr 20 - Add storage_hdr (set by segdded) to skip the SEGD 128 byte "Storage Unit Header"
!                 Ignore that on seisnet and other real-time data.
! mod 16 Apr 20 - Allow FORMAT ifmt 7 (special one off for Tahir Helvaci)
!               - Every trace has one too many samples
!               - Multiple shots in the file
!               - Some shots don't have all the traces indicated in the channel set header
! mod 26 Apr 22 - Change the logic of huge extended headers and external headers
!               - Redo 21 Jan 20 mod for the 2019 Lanseth cruise so that older Lanseth
!                 data works and passes testing!
!               - Read only MIN(1000,extended_header_size) and skip the rest.
! mod 24 Jan 24 - work on format 8015 - call segd20 rather than abort
!
      PARAMETER ( maxndeps = 50 )
!     remember that scr, iscr, lscr are equivalenced
      DIMENSION buf(1000), scr(1000)
      DIMENSION delay(99), idelay(99), H2O_depths(maxndeps),descalar(99)
      INTEGER*2 ibuf(1000), iscr(1000), rshift, lshift
!   we need to define the length of constants for IAND to work right
      INTEGER*2 i15, i255, i127, i128
      CHARACTER*1 harray(1000)
      DATA i15/15/, i255/255/, i127/127/, i128/128/
      DATA i16777215/16777215/   ! FFFFFF or lower 24 bits on

! Additional declarations for 8024 format data ajh
      INTEGER*2 i14, i7, i4095, i32767
      PARAMETER ( i14=14, i7=7, i4095=4095, i32767=32767)  !32767=0x7FFF or lower 15 bits
      INTEGER*2 expo, fraction

      INTEGER lbuf(1000), lscr(1000)
      REAL*8 ship_lat, ship_long, tail_lat, tail_long
      COMMON /sioap/ iasgnd, irelse, in, iout,nextad,lapsiz,ifree,iuseap
c**** segdin and segddin are mutually exclusive, so segddin uses segdin's common
      COMMON /segdin/ junit, nlists, nwrds, luntr0, luntr0a, lunldeo,
     &                shot_inc, storage_hdr
      COMMON /segddin/ cpath
      CHARACTER*80 cpath
      COMMON /readt/ iunit, numhdr, numdat, ihunit, ireeln, intrcs
c**** writet is needed so we can signal output to write a new segy file
      COMMON /WRITET/ounit,NSAMPSO,OREEL,POSAFT,OFMT,NTRCS,LFOR,ONUMTR,
     &       nfskipo, rewindo, newfile, itrace0, ounit2
      INTEGER*4 OFMT,OUNIT,POSAFT,ONUMTR,OREEL, rewindo
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      CHARACTER*200 cbufin
      COMMON /sioln1/cbufin
      COMMON /sioln2/ jchar, ncbuf
      COMMON /sioln4/ ICHAR4, NCHARS4, iprint4, lunpo4          ! used by rline1
      COMMON /inputdev/inputdev
      CHARACTER*80 inputdev, token, top1, top2, filename
      PARAMETER ( ldeo_size = 1000 )
      CHARACTER*1000 ldeo_ascii
      INTEGER*2 ldeo_dumb(500)
      EQUIVALENCE (ldeo_ascii,ldeo_dumb(1))
      CHARACTER*10 line_name
      CHARACTER*3 dgps_id
      INTEGER*2 idumb(2)
      EQUIVALENCE (idumb(1),ldumb)
      INTEGER ffilen, filinc, ftr, trinc, decimf, fgmt, fday, renum
      INTEGER naddblocks, manuf, serial, fftr, fcset, shot_inc,
     &        gmtinc, stack, badtrace, retrac, descale, trcount,
     &        storage_hdr
      LOGICAL first, getlist, iexist, ldeoopen
      DATA lastshot/0/, lastfilen/0/, ldeoopen/.TRUE./, badtrace/0/
      DATA fmt_ldeo/8/
      SAVE
      DATA first/.TRUE./, mtrgat/0/, ldgo_shotno/0/, nchanges/0/
      DATA wdepth /0./, owdepth/-1./, getnewshot/0/, istop_stack/0/
      DATA ns_done/-1/                                                  ! the segdded read the first segd general header
      DATA lunin/0/, idifference/0/, lastcsn/0/
      DATA ship_lat/0./, ship_long/0./, rev/0./
      DATA milsg/9999/
!****
!****  format = 1, 
!****  format = 2, Seisnet/LDEOLIST
!****  format = 3, Hydroscience with extra 32 byte header
!****  format = 4, SIO-RT ---  This never came to fruition - where
!****              there was an external file with all the metadata.
!****  format = 5, Geometrics - ASCII (nonNMEA) info in the external header.
!****  format = 6, LDEO external header - same as lunldeo non-zero.
!****  format = 7, ARAM which writes 1 more sample than it says
!****  format = 8, 2019 Lanseth with an extra 4 bytes
!****
!****  get the parameters from disc on the first entry
!****
!****
      newfile = 0
      IF( first ) THEN
          first = .FALSE.
          CALL podisc( junit, 1, 0 )                                    ! rewind the parameter file
          mlists = 0
          getlist = .TRUE.
          DO i = 1, maxndeps
             H2O_depths(i) = -1.
          ENDDO
          nh2o = 0
      ENDIF
      IF( getlist ) THEN
          mlists = mlists + 1
          getlist = .FALSE.
          CALL rddisc( junit, lscr, nwrds, istat )
          iunit = lscr(1)
          ffilen = lscr(2)
          lfilen = lscr(3)
          filinc = lscr(4)
          ftr = lscr(5)
          ltr = lscr(6)
          trinc = lscr(7)
          secs = scr(8)
          decimf = lscr(9)
          lprint = lscr(10)
          fday = lscr(11)
          lday = lscr(12)
          fgmt = lscr(13)
          lgmt = lscr(14)
          ntrgat = lscr(15)
          stime = scr(16)
          renum = lscr(17)
          fcset = lscr(18)
          lcset = lscr(19)
          gmtinc = lscr(20)
          iformat = lscr(21)
          retrac = lscr(22)
          stack = lscr(23)
          list = lscr(24)
          lunlog = lscr(25)
          nspfile = lscr(26)
          lunotape = lscr(27)
          ldeolist = lscr(29)
          descale = lscr(30)
!****
!****  ibuf, the SEGY header, was created by the edit (segded.f).
          igmt = ibuf(81)*100 + ibuf(82)                                ! the GMT of the shot from the general header
          fftr = ftr                                                    ! the next trace wanted
          idelay(1) = ibuf(55)
          micros = ibuf(59) / decimf
          iyear = ibuf(79)
          iday = ibuf(80)
          ihour = ibuf(81)
          imin = ibuf(82)
          isec = ibuf(83)
          si = buf(49) / decimf
          IF( IAND(lprint,2) .NE. 0 ) THEN
              PRINT *,iunit,ffilen,lfilen,filinc,ftr,ltr,trinc,secs,
     *                decimf
              PRINT *,fday,lday,fgmt,lgmt,ntrgat,stime,renum,igmt,
     *             idelay(1),nsamps,micros,iyear,iday,ihour,imin
              PRINT *, isec, si, fcset, lcset, gmtinc, ifmt
              PRINT *, iformat,retrac,stack,list,lunlog,nspfile,
     &                 lunotape,ldeolist,descale, storage_hdr
          ENDIF
          itrcno = 0
          IF( stack + ldeolist .NE. 0 .AND. lunin .EQ. 0 ) THEN
              CALL getfil( 2, lunin, token, istat )                     ! get a lun for the IN file
              OPEN(UNIT=lunin,FILE='in',STATUS='OLD',IOSTAT=istat)
              CLOSE(UNIT=lunin,STATUS='DELETE')
              OPEN(UNIT=lunin,FILE='IN',STATUS='OLD',IOSTAT=istat)
              CLOSE(UNIT=lunin,STATUS='DELETE')
          ENDIF
      ENDIF
      IF( getnewshot .GT. 0 ) GOTO 1000
      IF( itrcno .GT. 0 ) GOTO 200
      CALL podisc( iunit, 1, 0 )
!**** 
!****    get the general headers
!****
  100 CONTINUE
!**** Seisnet has a header before the SEG-D General header and it's in
!****  little endian (pc byte order).
!   word 2 = number of channels
!        3 = number of bytes in the Seisnet header
!        4 = number of bytes in the General header
!        5 = 3+4 = address of 
      IF( iformat .EQ. 2 ) THEN
          CALL rddisc( iunit, lscr, 3, istat )
          IF( icompt .NE. 2 .AND. icompt .NE. 4 ) CALL swap32(lscr(3),1)
          nbytes = lscr(3)
          CALL podiscb( iunit, 1, nbytes )
      ENDIF          
      IF( storage_hdr .NE. 0 ) THEN
          CALL rddiscb( iunit, iscr(1), 128, istat )
      ENDIF
  130 CALL rddiscb( iunit, iscr(1), 32, istat )
      IF( istat .NE. 32 ) GOTO 980
      indx = 17
      IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     *        CALL swap16( iscr(1), 32 )         ! swap bytes on DEC computers
      IF( IAND(lprint,16) .NE. 0 ) THEN
          CALL itohex( iscr(1), 32, token )
          PRINT *,' General Header #1, ',token(1:64)
      ENDIF
      ifilen = IAND( rshift(iscr(1),12), i15) * 1000 +                    ! file number
     *         IAND( rshift(iscr(1),8), i15) * 100 +
     *         IAND( rshift(iscr(1),4), i15) * 10 +
     *         IAND(iscr(1),i15)  
      ns_done = ns_done + 1
      newfile = 0
      ifmt = IAND( rshift(iscr(2),12), i15) * 1000 +
     *       IAND( rshift(iscr(2),8), i15) * 100 +
     *       IAND( rshift(iscr(2),4), i15) * 10 +
     *       IAND( iscr(2),i15 )
!**** HydroScience has an extra block with a different trace length
!      IF( ifmt .EQ. 3336 .AND. iformat .EQ. 3 ) THEN
      IF( ifmt .EQ. 8036 .AND. iformat .EQ. 3 ) THEN
          IF( icompt .NE. 2 .AND. icompt .NE. 4 ) THEN
              CALL swap32( lscr(1), 8 )
          ELSE
              CALL swap16( iscr(1), 32 )                                
          ENDIF
          itrsize = lscr(4)
          GOTO 130
      ENDIF
      IF( IAND(lprint,16) .NE. 0 ) THEN
          CALL itohex( iscr(indx+2), 6, token )
          PRINT *,' General Constants: ',token(1:6)
      ENDIF
      iyear = IAND( rshift(iscr(6),12), i15) * 10 +
     *        IAND( rshift(iscr(6),8), i15)
      IF( iyear .LT. 80 ) THEN
          iyear = iyear + 2000
      ELSE
          iyear = iyear + 1900
      ENDIF
      naddblocks = IAND( rshift(iscr(6),4), i15)
      IF( naddblocks .GT. 0 ) THEN
          nbytes = naddblocks * 32
          CALL rddiscb( iunit, iscr(indx), nbytes, istat )
          IF( istat .NE. nbytes ) GOTO 980
!****     seisnet doesn't swap the general header
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     &        CALL swap16( iscr(indx), nbytes/2 )
          IF( IAND(lprint,16) .NE. 0 ) THEN
!****         watch out - token is character*80
              CALL itohex( iscr(indx), 40, token )
              PRINT *,' additional General Headers: ',token(1:80)
          ENDIF
          rev = IAND(rshift(iscr(22),8),i255) + 
     &          REAL(IAND(iscr(22),i255))/10.
          ntrailer = iscr(23) * 32
!****     16665 = 15000 + 1500 + 150 + 15 = FFFF
          IF( ifilen .EQ. 16665 ) THEN
!              CALL itohex(iscr(indx),4,token)
!              print *,' token=',token(1:8)
              IF( icompt .NE. 2 .AND. icompt .NE. 4 ) THEN
                  idumb(1) = iscr(indx)
                  idumb(2) = iscr(indx+1)
              ELSE
!****             it was 16 bit swapped earlier, so swap the short word order
                  idumb(1) = iscr(indx+1)
                  idumb(2) = iscr(indx)
              ENDIF
              ldumb = lrshift(ldumb,8)
              ldumb = IAND(ldumb,i16777215)
              ifilen = ldumb
!               - Add skipping 4 bytes for format ldeo for 2019 Langseth cruise
!                 which breaks older format ldeo code, but that probably doesn't matter
          ENDIF
          indx = indx + nbytes / 2
      ENDIF
      IF( stack .GT. 0 .AND. ifilen .EQ. lastfilen ) THEN
          CALL sleep(3)
          GOTO 1000
      ENDIF
      IF( ns_done .EQ. nspfile ) THEN
          newfile = 1
          ns_done = 0
      ENDIF
      ishotno = ifilen
      iday = IAND( iscr(6),i15) * 100 +
     *       IAND( rshift(iscr(7),12), i15) * 10 +
     *       IAND( rshift(iscr(7),8), i15)
      ihour = IAND( rshift(iscr(7),4), i15) * 10 +
     *        IAND(iscr(7),i15)
      imin = IAND( rshift(iscr(8),12), i15) * 10 +
     *       IAND( rshift(iscr(8), 8), i15)
      igmt = ihour*100 + imin
      IF( IAND(lprint,2) .NE. 0 ) PRINT *,' file',ifilen,
     *    ' day',iday,' hour',ihour,' min',imin,' mils',milsg
      IF( (fgmt .NE. 0 .OR. lgmt .NE. 2400) .AND. lday .EQ. iday
     *    .AND. igmt .GT. lgmt ) THEN                                   ! If the user gave LGMT and this shot is bigger, STOP
          PRINT *,' User stop with GMT parameters.'
          istop = -1
          RETURN
      ENDIF
      IF( iday .GT. lday .AND. fday. LT. lday ) THEN                    ! fday set equal to lday if not using GMT
          PRINT *,' User stop with DAY parameters.'
          istop = -1                                                    ! thus will not exit if reading by shot and
          RETURN                                                        ! day change occurs. GMK
      ENDIF
      isec = IAND( rshift(iscr(8),4), i15) * 10 +
     *       IAND(iscr(8), i15) 
      manuf = IAND( rshift(iscr(9),12), i15) * 10 +
     *        IAND( rshift(iscr(9),8), i15)
!     serial will be wrong on little endian computers
      serial = IAND( rshift(iscr(9),4), i15) * 1000 +
     *         IAND(iscr(9), i15) * 100 +
     *         IAND( rshift(iscr(10),12), i15) * 10 +
     *         IAND( rshift(iscr(10),8), i15 )
      itemp = IAND( rshift(iscr(12),8), i255 )                            ! the sample interval in base 1/16 mils
      si = FLOAT(itemp) / 16. / 1000.                                   ! the REAL sample interval
      micros = FLOAT(itemp) / 16. * 1000.                               ! the sample interval in microseconds
      length = IAND(iscr(13),i15) * 10 +
     *         IAND( rshift(iscr(14),12), i15)
      temp = IAND( rshift(iscr(14),8), i15)
      rlen = (FLOAT(length) + temp/10. ) * 1.024
      nsamps = rlen / si
      IF( iformat .EQ. 3 ) nsamps = (itrsize-20)/3
!     Number of channels set is nibble 1 & 2 of byte 29
      ncsets = IAND(rshift(iscr(15),12),i15) * 10 +
     *         IAND( rshift(iscr(15),8), i15)
      IF( IAND(lprint,16) .NE. 0 ) THEN
          CALL itohex( iscr(1), 32, token )
          PRINT *,' General Header #1, ',token(1:64)
      ENDIF
!     If ncsets = hex FF dec 255, then get it from the next general header
!     the 165 is a bug in the Hydroscience acquisition system. dec 165 = hex A5 = bin 1010 0101
      IF( ncsets .EQ. 165 ) THEN
          PRINT *,' *** ABORTED ***'
          PRINT *,' ncsets of 165 no longer supported.'
          PRINT *,' Contact phenkart@gmail.com'
!         it's an integer in bytes 4 & 5 in general header #2
!          ncsets = IAND( iscr(18),i15) * 10 +
!     &             IAND( rshift(iscr(19),12), i15)
!          IF( icompt .EQ. 2 .or. icompt .EQ. 4 ) THEN
!****  between fortran and endiannes this sucks.  We did a 16 bit byte swap before
!              idumb(1) = iscr(17)
!              idumb(2) = iscr(18)
!              CALL swap16( idumb, 2 )
!              CALL swap32( ldumb, 1 )
!              ncsets = IAND( ldumb, i15 ) * 10
!              idumb(1) = iscr(19)
!              idumb(2) = iscr(20)
!              CALL swap16( idumb, 2 )
!              ncsets = ncsets + idumb(1)
!          ENDIF
      ENDIF
      IF( ncsets .GT. 0 ) THEN
          nbytes = 32
          DO i = 1, ncsets
             CALL rddiscb( iunit, iscr(indx), nbytes, istat )
             IF( istat .NE. nbytes ) GOTO 980
             IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     &           CALL swap16( iscr(indx), nbytes/2 )
             IF( IAND(lprint,16) .NE. 0 ) THEN
                 CALL itohex( iscr(indx), 40, token )
                 PRINT *,' Channel Set header:',token(1:80)
             ENDIF
             indx = indx + nbytes / 2
          ENDDO
      ENDIF
      nskew = IAND( rshift(iscr(15),4), i15) * 10 +
     *        IAND(iscr(15),i15)
      IF( nskew .GT. 0 ) THEN
          nbytes = nskew * 32
          CALL rddiscb( iunit, iscr(indx), nbytes, istat )
          IF( istat .NE. nbytes ) GOTO 980
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     &        CALL swap16( iscr(indx), nbytes/2 )
          IF( IAND(lprint,16) .NE. 0 ) THEN
		    CALL itohex( iscr(indx), 40, token )
		    PRINT *,' at nskew, ',token(1:80)
          ENDIF
          indx = indx + nbytes / 2
      ENDIF
      nextend = IAND(rshift(iscr(16),12),i15) * 10 +
     *          IAND( rshift(iscr(16),8), i15)
      IF( nextend .EQ. 165 ) nextend = lshift(IAND(iscr(19),i255),8) +
     &             IAND( rshift(iscr(20),8),i255)
      IF( nextend .GT. 0 ) THEN
          nbytes = nextend * 32
          CALL adrdisc( iunit, istart_addr )
!    don't read too many 
          nto_read = MIN0(nbytes,ldeo_size)
          CALL rddiscb( iunit, iscr(indx), nto_read, istat )
          IF( istat .NE. nto_read ) GOTO 980
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     &        CALL swap16( iscr(indx), nto_read/2 )
          IF( IAND(lprint,16) .NE. 0 ) THEN
              PRINT *,' Extended header length = ',nbytes,' bytes.'
              CALL itohex( iscr(indx), 40, token )
              PRINT *,' Extended header (partial):'
              PRINT *,token(1:MIN0(nbytes*2,80))
          ENDIF
          indx = indx + nto_read / 2
!   now get to the end of te extended headers
          CALL podiscb( iunit, 0, istart_addr + nbytes )
          CALL adrdisc( iunit, iend_addr )
      ENDIF
      ship_lat = 0.
      ship_long = 0.
      nexternal = IAND( rshift(iscr(16),4), i15) * 10 +
     *            IAND(iscr(16),i15)
      IF( nexternal .EQ. 165 ) nexternal=lshift(IAND(iscr(20),i255),8) +
     &             IAND( rshift(iscr(21),8),i255)
      IF( nexternal .GT. 0 ) THEN
          CALL adrdisc( iunit, istart_addr )
          nbytes = nexternal * 32
          nto_read = MIN0(nbytes,ldeo_size)
          CALL rddiscb( iunit, iscr(indx), nto_read, istat )
          IF( istat .NE. nto_read ) GOTO 980
          IF( IAND(lprint,16) .NE. 0 ) THEN
              PRINT *,' External header length = ',nbytes,' bytes.'
              CALL itohex( iscr(indx), 40, token )
              PRINT *,' External header (partial): ',token(1:80)
          ENDIF
!****     watch out for ldeo_ascii size
          DO i = 1, MIN0(ldeo_size/2,400)
             ldeo_dumb(i) = iscr(indx+i-1)
          ENDDO
          IF( IAND(lprint,16) .NE. 0 ) THEN
              PRINT *,' 1:80=',ldeo_ascii(1:80)
              PRINT *,' 81:160=',ldeo_ascii(81:160)
              PRINT *,' 161:240=',ldeo_ascii(161:240)
              PRINT *,' 241:320=',ldeo_ascii(241:320)
              PRINT *,' 321:400=',ldeo_ascii(321:400)
          ENDIF
          IF( ldeo_ascii(1:2) .EQ. '$1' .AND. iformat .NE. 6 ) THEN
              PRINT *,' ***  WARNING  ***  LDEO external header detected
     & - changing to segddin FORMAT LDEO.'
              iwarn = iwarn + 1
              iformat = 6
          ENDIF
!****
!****  Search the External header for NMEA strings
!****  Put the ASCII string in the common buffer so getoke works
!****
          ncbuf = 82  ! NMEA string maximum
          CALL upcase( ldeo_ascii, nto_read )
          DO jjchar = 1, nto_read
             IF( ldeo_ascii(jjchar:jjchar) .EQ. '$' .AND.
     $           ldeo_ascii(jjchar+3:jjchar+5) .EQ. 'GGA' .AND.
     &           jjchar+40 .LT. nbytes ) THEN
                 jchar = 1
                 cbufin(1:82) = ldeo_ascii(jjchar:jjchar+81)
                 IF( IAND(lprint,16) .NE. 0 ) PRINT *,cbufin(1:81)
                 CALL gpggaa( ihourg, iming, isecg, milsg,
     &                       ship_lat, ship_long, istat )
             ENDIF
          ENDDO
          DO jjchar = 1, nto_read
             IF( ldeo_ascii(jjchar:jjchar) .EQ. '$' .AND.
     &           ( ldeo_ascii(jjchar+1:jjchar+3) .EQ. 'DBT' .OR.
     &           ldeo_ascii(jjchar+3:jjchar+5) .EQ. 'DBT' )) THEN
                 jchar = 1
                 cbufin(1:82) = ldeo_ascii(jjchar:jjchar+81)
                 IF( IAND(lprint,16) .NE. 0 ) PRINT *,cbufin(1:81)
                 CALL dbt( wdepth )
             ENDIF
          ENDDO
          IF( IAND(lprint,8) .NE. 0 .AND. iformat .NE. 6 ) THEN
             PRINT *,' ihourg, iming, isecg, milsg, ship_lat, ship_long'
              PRINT *, ihourg, iming, isecg, milsg, ship_lat, ship_long
          ENDIF
          IF( ldeo_ascii(3:3) .EQ. '-' .AND.ldeo_ascii(6:6).EQ.'-') THEN
!****         It's Lee's 2005 external header
              cbufin = ldeo_ascii(1:ntodo)
              jchar = 1
              ncbuf = nto_read
!****         itemp, itemp1, temp are the time of shot 
!****         ihourg, iming, secg are the $GPGGA (time of the fix)
              CALL leeshdr( itemp, itemp1, temp, 
     &            ihourg, iming, secg, ship_lat, ship_long, 
     &            wdepth, rmaggie)
              IF( IAND(lprint,8) .NE. 0 ) THEN
                  PRINT *,  ' SIO nav times: ',
     &                  itemp, itemp1, temp, ihourg, iming, secg
                  PRINT *,' SIO nav: ',ship_lat, ship_long
                  PRINT *,' water depth=', wdepth, ' maggie=',rmaggie
              ENDIF
!****         The time of the fix may not be the time of the shot.
!****         What to do?  Let's use it anyway.  Is the nearest second
!****         good enough?  5 seconds?  10 secs?
          ENDIF
          indx = indx + nto_read / 2
!   now get to the end of the extended headers
          CALL podiscb( iunit, 0, istart_addr + nbytes )   ! address from the start!
          CALL adrdisc( iunit, iend_addr )
      ENDIF
      ntotal = 1 + naddblocks + ncsets + nskew + nextend + nexternal
!**** Get the record length from GH#2 if it's FFF.
!**** This one is in mils
      IF( length .EQ. 165 ) THEN
          length = iscr(24) * 256 + IAND(rshift(iscr(25),8),i255)
          nsamps = NINT(FLOAT(length) / (si * 1000.))
          IF( iformat .EQ. 3 ) nsamps = (itrsize-20)/3
      ENDIF
      IF( iformat .EQ. 7 ) nsamps = nsamps + 1
      IF( IAND(lprint,2) .NE. 0 ) THEN
          PRINT *,' ncsets=',ncsets,' nskew=',nskew,' nextend=',nextend,
     &      ' nexternal=',nexternal,' naddblks=',naddblks
          PRINT *,' total general header blocks ',ntotal 
          PRINT *,' SEG-D rev ',rev
          PRINT *,' ntrailer=',ntrailer
      ENDIF
      IF( iformat .EQ. 3 .AND. rev .NE. 0 ) THEN
          PRINT *,' ***  WARNING  ***  Remove parameter format HTI'
      ENDIF
      intrcs = 0
!      DO 150 i = fcset, lcset
      DO i = 1, lcset          !  ajh
         index = (naddblocks+i)*16
!****  Bytes 3 & 4 are the channel set start time
         idelay(i) = iscr(index+2) * 2                                  ! a binary number - in 2 mil increments
!****  Bytes 5 & 6 are the channel set end time
!****  Rev 0 byte 8 was the MP factor, byte 7 was unused
!****    Bytes 7 & 8 are the MP factor, which changed in REV. 1 by
!****    adding byte 7 as a "precision extension".  Byte 8 is the
!****    high order byte and 7 is the low order byte.  It used to be
!****    a count of .25 increments.  In Rev 2 it's .000976525 (.25 / 256.).
!****  Rev 3 MP is in a totally different spot!
!****    It's a sign and a magnitude.
!****    A byte swap does this for us!
         IF( iscr(index+4) .NE. 0 ) THEN
             isign = IAND(iscr(index+4),i128)
!  bad              iscr(index+4) = IAND(iscr(index+4),i127) <- Not good: zeros byte 7
! Change mask after swap to avoid zeroing byte 7
             CALL swap16(iscr(index+4),1)

!ajh changes to do descalar correctly
             iscr(index+4) = IAND(iscr(index+4),i32767)   ! mask out the sign bit
             temp = REAL(iscr(index+4))
             IF( isign .NE. 0 ) temp = -temp
             descalar(i) = 2. ** (temp /1024. )

                     ! ECHOS from Paradigm seems to use descalar^2 why????
            ! descalar(i) = descalar(i)*descalar(i)

             IF( isign .NE. 0 ) temp = -temp
             descalar(i) = 2. ** (temp * .25 / 256. )
             IF( IAND(lprint,8) .NE. 0 ) THEN
                if (i==fcset) print *
                   PRINT '(A,I2,A,G10.4,A,F12.3)', 
     &                'Channel set ',i, 
     &                ' has a descalar of ',descalar(i),' MP=',temp
             endif
         ENDIF
         n = IAND( rshift(iscr(index+5),12), i15) * 1000 +
     *         IAND( rshift(iscr(index+5),8), i15) * 100 +
     *         IAND( rshift(iscr(index+5),4), i15) * 10 +
     *         IAND(iscr(index+5),i15)
         intrcs = intrcs + n
         delay(i) = REAL(idelay(i))/1000.
      ENDDO
!  150 CONTINUE
      IF( ltr .GT. intrcs .OR. ltr .EQ. 0 ) ltr = intrcs
      ltr1 = ltr
      IF( iday .lt. fday ) GOTO 1000
      IF( iday .eq. fday .and. igmt .LT. fgmt ) GOTO 1000
      IF( ftr .NE. 99999 ) fftr = ftr
      IF( rlen .GT. 100. ) THEN                                         ! extended record length
          itemp = IAND( rshift(iscr(25),8), i255 )
          rlen = REAL(iscr(24)) * 256. + REAL(itemp)
          rlen = rlen / 1000.                                           ! convert segd mils to secs
      ENDIF
!**** Always write ALL of the SEG-D headers to luntr0
!**** the only thing not in the SEGY trace header is the gun info.
!     CALL podiscb( luntr0, 0, 0 )
      nbytes = ntotal * 32
      CALL wrdisc( luntr0, nbytes, 1 )
      CALL wrdiscb( luntr0, scr, nbytes )
!****  LDEO's Syntron system has a "LDEO block" in the external header
!****  added fmt_ldeo in 2022 while testing!
      IF( iformat .EQ. 2 .OR. iformat .EQ. 6 .OR. 
     &    iformat .EQ. fmt_ldeo ) THEN
!****     The LDEO/Syntrak/Spectra block start with $1
          IF( ldeo_ascii(1:2) .EQ. '$1' ) THEN
              IF( ldeo_ascii(19:19) .EQ. '.' ) THEN
!****   2A=$1, I4=length of header, 4A=program revision, I2=line status,
                  READ( ldeo_ascii,'(2x,I4,4x,2x,3I2,1x,I6,I4,2I2,3x,
     &                  I6,A16,2F11.6,F6.1,2F11.6,2F5.1,F4.1)' )
     &            len_hdr,ldgo_hr, ldgo_min, ldgo_sec, ldgo_mil,
     &            ldgo_yr, month, iday, ldgo_shotno, line_name,
     &            ship_lat, ship_long, wdepth, tail_lat, tail_long,
     &            gyro, cmg, speed
              ELSE
                  ldgo_mil = 0
                  READ( ldeo_ascii,'(2x,I4,4x,2x,3I2,I4,2I2,3x,
     &                  I6,16x,2F11.6,F6.1,2F11.6,2F5.1,F4.1)' )
     &            len_hdr,ldgo_hr, ldgo_min, ldgo_sec,
     &            ldgo_yr, month, iday, ldgo_shotno,
     &            ship_lat, ship_long, wdepth, tail_lat, tail_long,
     &            gyro, cmg, speed
              ENDIF
!              CALL tohex(ldgo_shotno,4,token)
!              print *,' token=',token(1:8)
              ldgo_mil = FLOAT(ldgo_mil) / 1000.
              CALL caljul( month, iday, ldgo_yr, ldgo_day )
              IF( lunldeo .NE. 0 ) THEN
                  CALL podiscb( lunldeo, 0, 0 )
                  CALL wrdiscb( lunldeo, ldeo_ascii, len_hdr )
              ENDIF
          ELSE
!****         The old Digicon Trace 0
              CALL ldgo_tr0( ldeo_ascii, ldgo_shotno, ldgo_yr,
     &             ldgo_day, ldgo_hr, ldgo_min, ldgo_sec, ldgo_mil,
     &             ship_lat, ship_long, wdepth,
     &             tail_lat, tail_long, tail_dist, tail_bear )
              IF( IAND(lprint,256) .NE. 0 ) THEN
                  PRINT *,' ',ldeo_ascii(52:77),' ',ldeo_ascii(8:28),
     &              ' ',ldeo_ascii(158:162),
     &              ' ',ldeo_ascii(1:6),' ',ldeo_ascii(229:314)
                  line_name = ldeo_ascii(140:149)
              ENDIF
!****     make it look like the old Digicon trace 0, which starts with 
!****     24 bytes of the SEG-D record header and followed by a series of
!****     block or sections.   INT*2 word 1 of the section is the section
!****     number and word 2 is the number of bytes in the section (including
!****     the 4 byte section header).
!****     Section 11 was an LDEO ASCII header with 214 bytes 
!****     Section 13 with the Digicourse ASCII birds
!          IF( lunldeo .NE. 0 ) THEN
!              CALL podiscb( lunldeo, 0, 0 )
!              DO i = 1, 6
!                 scr(i) = 0
!              ENDDO
!              CALL wrdiscb( lunldeo, iscr, 24 )
!              iscr(1) = 11
!              iscr(2) = 214
!              CALL wrdiscb( lunldeo, iscr, 4 )
!              CALL wrdiscb( lunldeo, ldeo_dumb, 210 )
!          ENDIF
          ENDIF
!****     Check for consecutive file numbers
          IF( ifilen .NE. lastfilen + 1 .AND. lastfilen .NE. 0 ) THEN
              PRINT 151
              IF( lunlog .NE. 0 ) WRITE (lunlog, 151)
  151   FORMAT(' ***  WARNING  ***   File numbers indicate lost files.')
          ENDIF
!****     Spectra can shoot the line backwards and the shot number decreases, so stop this check
!          IF( ldgo_shotno .NE. lastshot + 1 .AND. lastshot .NE. 0 ) THEN
!              PRINT 152
!              IF( lunlog .NE. 0 ) WRITE (lunlog, 152)
!  152   FORMAT(' ***  WARNING  ***   Shot numbers indicate lost shots.')
!          ENDIF
!          IF( ldgo_shotno .NE. ifilen + idifference ) THEN
!              idifference = ldgo_shotno - ifilen
!              IF( lastshot.NE.0) THEN
!                  PRINT 153
!                  IF( lunlog .NE. 0 ) WRITE (lunlog, 153)
!  153   FORMAT(' ***  WARNING  ***   Shot and file numbers diverge.')
!              ENDIF
!          ENDIF
!****     The LDEO header gets dropped sometimes (on ew0210 it was always
!****     at the start of line), and Joyce wants a non zero number.
!****     Make it consistent with SEGDIN
          IF( ldgo_shotno .LE. lastshot .AND. ldgo_shotno .EQ. 0 .AND.
     &        lastfilen .NE. ifilen ) THEN
              PRINT *,' ***  WARNING  ***  Bad LDGO shot number of ',
     &             ldgo_shotno,' Using ', lastshot + 1
              ldgo_shotno = lastshot + 1
          ENDIF
!****     Check that the multibeam is working and passing depths
          DO i = 1, maxndeps 
             IF( wdepth .NE. H2O_depths(i) ) GOTO 160
          ENDDO
          IF( wdepth .EQ. -1 ) THEN
          PRINT *,' ***  WARNING  ***  Water depth not in SEG-D header.'
              PRINT *,' Is the multi-beam working?'
              PRINT *,' Is the multi-beam passing the water depth?'
              GOTO 160
          ELSE
              PRINT *,
     &' ***  WARNING  *** Water depth has not changed (check multibeam)'
          ENDIF
  160     DO i = 1, maxndeps - 1
             H2O_depths(i) = H2O_depths(i+1)
          ENDDO
          H2O_depths(maxndeps) = wdepth
!**** 
          IF( IAND(lprint,4) .NE. 0 ) THEN
              PRINT *,' Shot number: ', ldgo_shotno,
     &            ' byte index=',index*2
              PRINT *,' Joe date: ',ldgo_yr, ldgo_day,
     &                 ldgo_hr, ldgo_min, ldgo_sec, ldgo_mil
              PRINT *,' ship lat: ',ship_lat, ' ship long: ',ship_long
              PRINT *,' water depth: ',wdepth, 
     &               ' tail pos: ',tail_lat, tail_long
              PRINT *,' tail dist and bearing: ',tail_dist, tail_bear
          ENDIF
          IF( ldgo_yr + ldgo_day .LE. 0 ) PRINT *,
     &             ' ***  WARNING  ***  Bad LDGO header on file ',ifilen
!****     luntr0 is circular.  geom and output need the current tr0
          ldeo_ascii(1:484) = ldeo_ascii(212:695)
!         READ( ldeo_ascii(1:4), '(I4)' ) iscr(2)
!****  ah f!%^&,  READ fails/bombs sioseis if the Digicourse stuff isn't there
          CALL dcode( ldeo_ascii, 4, areal, istat )
          IF( istat .EQ. 2 ) THEN
              iscr(2) = NINT(areal) + 4
          ELSE
              PRINT *,' LDEO "nav" block does not have Digicourse.'
          ENDIF
          IF( lunldeo .NE. 0 ) THEN
              CALL wrdiscb( lunldeo, scr, 4 )
              CALL wrdiscb( lunldeo, ldeo_ascii(5:5), 2028 )
          ENDIF
      ENDIF
!**** seisnet seems to have an extra 136 blocks (4352 bytes) in the gen header
!****   On 2 channel sets there's 139 blocks (4448 bytes)
!      IF( iformat .EQ. 2 ) THEN
!****     4352 is also the value of index - the 16 bit word index or
!     index = (1+naddblocks+ncsets+nskew+nextend) * 32 / 2
!          IF( ncsets .EQ. 1 ) CALL podiscb( iunit, 2, 4352 )
!          IF( ncsets .EQ. 2 ) CALL podiscb( iunit, 2, 4448 )
!**** Seisnet has a bunch of stuff now - like a list of disk
!**** addresses and trace data length and other stuff.  So,
!**** skip to trace 1, BUT add 512 for some reason - it's like
!**** every write they do has an extra 512 bytes.  curious.
      IF( iformat .EQ. 2 ) THEN
          CALL rddiscb( iunit, scr, 4, istat )
          CALL swap32( lscr, 1 )
          CALL podiscb( iunit, 0, lscr(1) + 512 )
      ENDIF
      getnewshot = 0
!****
!****   Find the proper demultiplexed trace - Do the DEMUX header
!****
  200 CONTINUE
      IF( getnewshot .EQ. 1 ) GOTO 100
      CALL rddiscb( iunit, scr, 20, istat )
      IF( istat .NE. 20 ) GOTO 1000
      IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) CALL swap16( iscr, 10 )
      IF( IAND(lprint,16) .NE. 0 ) THEN
          CALL itohex( iscr, 20, token )
          PRINT *,' SEG-D trace header =',token(1:40)
      ENDIF
!*****
!*****  another bug in the "ARAM" data - not all traces are written.
      IF( iformat .EQ. 7 ) THEN
          CALL itohex( iscr, 20, token )
          IF( token(5:8) .EQ. '8058' ) THEN
!             oops, it's general header #1
              CALL podiscb( iunit, 2, -20 ) !  backup over the trace header
              GOTO 130
          ENDIF
      ENDIF
!
      nmore = IAND(iscr(5),i255) * 32    !  number of extra trace header extension blocks
      IF( nmore .NE. 0 .AND. IAND(lprint,16) .NE. 0 ) 
     &    PRINT *,' Number of trace header extensions =',nmore
      IF( nmore .GT. 0 ) CALL rddiscb( iunit, scr(11), nmore, istat )
      nbytes = nsamps * 4
      IF( ifmt .EQ. 8036 ) nbytes = nsamps * 3
      if (ifmt .EQ. 8024) nbytes = nsamps * 2                 ! ajh
      IF( ifmt .EQ. 8015 ) THEN     ! PCH Jan 2024
!         it's 2 I*2 (4 bytes) of exponent followed by 4 I*2 data words
          nbytes = nsamps * 2 + (nsamps/4) *2
      ENDIF
      CALL rddiscb( iunit, buf(numhdr+1), nbytes, istat )   !   This is the binary trace!
      IF( istat .NE. nbytes ) GOTO 990
      IF( iformat .EQ. 2 ) CALL podiscb(iunit, 2, 512 )   ! seisnet has 2 extra bytes per trace
!****
!**** AJH changes for 2019 Langseth cruise
!**** PCH had to change fmt_ldeo because AJH change made all previous LDEO work fail
!****
      if (iformat .EQ. fmt_ldeo) then
        if ( iand(lprint,16) /= 0) then
          call rddiscb(iunit,ldumb,4,istat)
          call tohex(ldumb,4,token)
          print '(2(A,I5),A)','Read ',nsamps,' samples with ',
     &       nbytes,' bytes'
          print '(A,A)',' Additional 4 bytes at LDEO trace end Hex:',
     &       token(1:8)
        else
          call podiscb(iunit,2,4)
        endif
      endif

!**** The file number in the trace header might be bogus because it's only 4 BCD
!**** long and the real file number might be in the extended general header
      icsn = IAND( rshift(iscr(2),4), i15) * 10 + IAND(iscr(2),i15)         ! channel set number
!****  Remember this is packed BCD, not HEX. FF in BCD is 165 (150 + 15)
      IF( icsn .EQ. 165 ) THEN                       
          itemp = IAND( rshift(iscr(9),8), i255)
          icsn = IAND( iscr(8),i255 ) * 256 + itemp
      ENDIF
      IF( icsn .NE. lastcsn .AND. ftr .NE. 99999 ) fftr = ftr
      lastcsn = icsn
      itrcno = IAND( rshift(iscr(3),12), i15) * 1000 +                    ! trace number
     *         IAND( rshift(iscr(3),8), i15) * 100 +
     *         IAND( rshift(iscr(3),4), i15) * 10 +
     *         IAND(iscr(3),i15)  
      IF( icsn .EQ. 1 .AND. itrcno .EQ. 1 ) trcount = 0
      trcount = trcount + 1
      IF( IAND(lprint,2) .NE. 0 ) 
     *     PRINT *,' ifilen=',ifilen,' trace=',itrcno,' fftr=',fftr,
     *       ' ltr=',ltr,' cn=',icsn,' igmt=',igmt,' ffilen=',ffilen,
     *       ' trcount=',trcount
!ccccc      IF( itrcno .GE. ltr ) istop = 1
      IF( ffilen .NE. 99999 .AND. ifilen .LT. ffilen ) GOTO 1000        ! find the first shot
      IF( icsn .GT. lcset ) GOTO 1000                                   ! get a new shot
      IF( icsn .LT. fcset ) GOTO 200                                    ! get another trace
      IF( filinc .EQ. 99999 ) ffilen = 99999                            ! got ffilen, so change it to accept all files
!ccccc      IF( itrcno .NE. fftr .AND. ltr .NE. 0 ) GOTO 200                   ! get the right trace
!**** If ftr is not given, then take all traces in all channel sets.
      IF( ftr .NE. 99999 ) THEN
!****     This statement allows all traces numbered between ftr and ltr
          IF( itrcno .LT. fftr .OR. (itrcno .GT. ltr .AND. ltr .NE. 0 ))
     &        GOTO 200
      ENDIF
      itword = iscr(4)                                                  ! time word
      itemp = IAND( rshift(iscr(5),8), i255)
      tword = itword + FLOAT(itemp)/256.
      ithext = IAND(iscr(5),i255)
      itemp = IAND( rshift(iscr(6),8), i255 )                             ! sample skew 
      skew = FLOAT(itemp) / 256.
      itimeb = iscr(7)                                                  ! time break
      itemp = IAND( rshift(iscr(8),8), i255)
      tbreak = itimeb + FLOAT(itemp)/256.
!****
!****    GOT A TRACE TO KEEP!   unpack the data and create an SEGY trace header
!**** seisnet (iformat 2) is swapped
      IF( (icompt .EQ. 2 .OR. icompt .EQ. 4) .AND. iformat .NE. 2 ) THEN
!****     8024 and 8015 are 16 bit data words
          IF (ifmt == 8024 .OR. ifmt == 8015 ) THEN
             n16wrds = nsamps
             IF( ifmt == 8015 ) n16wrds = nsamps + nsamps/4  ! 5 words for 4 data
             CALL swap16(ibuf(2*numhdr+1),n16wrds)
          else
            CALL swap32( buf(numhdr+1), nsamps )
          endif
      ENDIF
!  8015 = 20 bit SEGD floating point (4 bit hex exponent)
!  8022 = 8 bit integer
!  8024 = 16 bit IEEE floating point (not the same as UTIG F.P.)
!  8036 = 24 bit integer (new in rev 2)
!  8038 = 32 bit integer
!  8048 = 32 bit IBM FP (not in rev 2)
!  8058 = 32 bit IEEE FP
      IF( ifmt .EQ. 8015 ) THEN
          CALL segd20( ibuf(2*numhdr+1), scr, nsamps )
          DO i = 1, nsamps
             buf(numhdr+i) = scr(i)
          ENDDO
      ENDIF
      if (ifmt .EQ. 8024) then
         DO i = 1, nsamps
!         iscr(1:nsamps) = ibuf(2*numhdr+1:2*numhdr+nsamps)
           iscr(i) = ibuf(2*numhdr+i)
           expo = iand(rshift(iscr(i),11),i14) - 12
           fraction = iand(iscr(i),i4095)
           if (iscr(i) <  0) fraction = fraction - 4095
           buf(numhdr+i) = real(fraction) * 2.0 ** expo
         end do
      endif

      IF( ifmt .EQ. 8036 ) THEN
          CALL i24i32( buf(numhdr+1), lscr, nsamps )
          DO i = 1, nsamps
             buf(numhdr+i) = FLOAT( lscr(i) )
          ENDDO
      ENDIF
      IF( ifmt .EQ. 8038 ) THEN
          DO i = 1, nsamps
             buf(numhdr+i) = FLOAT(lbuf(numhdr+i))
          ENDDO
      ENDIF
      IF( ifmt .EQ. 8048 ) 
     &    CALL ibm2fp( buf(numhdr+1),nsamps,buf(numhdr+1))
      numdat = nsamps
      IF( stime .GT. 0. .AND. stime .GT. delay(icsn) ) THEN             ! should we get rid of the front of the data trace?
          n = NINT( (stime - delay(icsn)) / si )
          numdat = numdat - n
          DO 400 i = 1, numdat
             buf(numhdr+i) = buf(numhdr+n+i)                            ! move the data so stime is the first data sample
  400     CONTINUE
!          delay(icsn) = stime
!          idelay(icsn) = stime * 1000.
      ENDIF
      IF( decimf .GT. 1 ) THEN
          j = 1
          DO i = 1, numdat, decimf
             buf(numhdr+j) = buf(numhdr+i)
             j = j + 1
          ENDDO
          numdat = numdat / decimf 
!****     Don't change si for decimf because it's set only once per shot
      ENDIF 
      IF( secs .NE. 0 ) THEN
!****     numdat has been reduced by stime and decimf
          itemp = NINT(secs/(si*decimf))
          numdat = MIN0(numdat,itemp)
      ENDIF
!**** Descale by the MP factor if the user asks for it.  icsn is the 
!**** current trace's channel set number (I think).
      IF( descale .EQ. 1 .AND. descalar(icsn) .NE. 0 ) THEN
          temp = descalar(icsn)
          DO i = 1, numdat
             buf(numhdr+i) = buf(numhdr+i) * temp
          ENDDO
      ENDIF
      DO i = 1, 60 
  500    lbuf(i) = 0
      ENDDO
      lbuf(3) = ishotno
      IF( ldgo_shotno .NE. 0 ) lbuf(3) = ldgo_shotno
      IF( stack .NE. 0 .AND. shot_inc .NE. 0 ) THEN
          itemp = MOD(lbuf(3),2)
!****     1 means odd, 2 means even
          IF( (shot_inc .EQ. 1 .AND. itemp .EQ. 0) .OR.
     &        (shot_inc .EQ. 2 .AND. itemp .EQ. 1) ) THEN
              getnewshot = 1
              lastfilen = ifilen
              lastshot = lbuf(3)
!****         don't do the log or renum or retrac - we're not using this shot
              RETURN
          ENDIF
      ENDIF
      IF( renum .GT. 0 ) lbuf(3) = renum
      IF( retrac .LT. 0 ) lbuf(4) = trcount
      IF( retrac .EQ. 0 ) lbuf(4) = itrcno
      IF( retrac .GT. 0 ) THEN
!****     renumber the traces from RETRAC if it's a new shot or if
!****     the trace count is the number of input traces per shot
!****     (needed in case the shot numbers don't increment
          IF( lbuf(3) .NE. lastshot )
     &        otrcno = retrac
          lbuf(4) = otrcno
          otrcno = otrcno + 1
      ENDIF
      lbuf(5) = ifilen
      lastfilen = lbuf(5)
      ibuf(15) = 1
      buf(46) = AMAX1(delay(icsn),stime)
      ibuf(55) = NINT(buf(46) * 1000.)
      ibuf(58) = numdat
      ibuf(59) = micros * decimf
      ibuf(79) = iyear
      ibuf(80) = iday
      ibuf(81) = ihour
      ibuf(82) = imin
      ibuf(83) = isec
      ibuf(84) = 1
      IF( milsg .NE. 9999 ) ibuf(84) = milsg
      buf(49) = si * decimf
!****  Put the ship position in the group position because
!****  process geom uses 19 & 20 for the x/y coordinate and geom is
!****  needed to get the steamer depth.   Remember that x is longitude
!****  and y is latitude.
      IF( ship_lat + ship_long .NE. 0 ) THEN
          ibuf(36) = -100
!****     arcsec = 60sec/min * 60min/deg = 3600. sec/deg
          lbuf(19) = NINT(ship_long*3600.*100.)
          lbuf(20) = NINT(ship_lat*3600.*100.)
          lbuf(21) = NINT(ship_long*3600.*100.)
          lbuf(22) = NINT(ship_lat*3600.*100.)
          ibuf(45) = 2
      ENDIF
      lbuf(16) = wdepth
!      buf(50) = wdepth / 750.
!      buf(54) = wdepth
      IF( ntrgat .GT. 0 ) THEN
          mtrgat = mtrgat + 1
          IF( mtrgat .EQ. ntrgat ) THEN
              lbuf(51) = -1
              mtrgat = 0
          ENDIF
          lbuf(6) = lbuf(3)
      ENDIF
      IF( ldgo_shotno .NE. 0 ) THEN
          IF( wdepth .EQ. 0. .AND. owdepth .NE. -1. ) THEN
          PRINT *,' ***  WARNING  ***  Bad water depth, using previous.'
              wdepth = owdepth
          ENDIF
!****     Don't save/use bad water depths
          IF( wdepth .GT. 6 ) THEN
              owdepth = wdepth
          ELSE
              wdepth = 0.
          ENDIF
          ibuf(79) = ldgo_yr
          ibuf(80) = ldgo_day
          ibuf(81) = ldgo_hr
          ibuf(82) = ldgo_min
          ibuf(83) = ldgo_sec
!          ibuf(84) = NINT( (ibuf(83) - sec_ldgo ) * 1000.)
          ibuf(84) = ldgo_mil
      ENDIF
      IF( lunlog .NE. 0 .AND. lbuf(4) .EQ. 1 ) THEN
          lat = ship_lat
          long = ship_long
          WRITE(lunlog,170) lbuf(5),lbuf(3),ibuf(80),ibuf(81),ibuf(82),
     &          ibuf(83), ibuf(84),
     &          lat, (ABS(ship_lat)-IABS(lat)) * 60.,
     &          long, (ABS(ship_long)-IABS(long)) * 60., wdepth
  170           FORMAT (' file ',I6,' shot ',I6,' day ',I3,1X,2I2,'z ',
     &      I3,'.',I3,' lat/long ',I3,1x,F7.4,1X,I4,1X,F7.4,' WD ',F7.1)
           FLUSH( lunlog )
      ENDIF
      lastshot = lbuf(3)
      IF( IAND(lprint,4) .NE. 0 ) THEN
          PRINT *,' SEGY header:'
          PRINT *,lbuf(3),lbuf(4),ibuf(15),ibuf(55),ibuf(58),ibuf(59)
          PRINT *,lbuf(16),ibuf(36),lbuf(19),lbuf(20)
          PRINT *,(ibuf(i),i=79,84),buf(46),buf(49),lbuf(51)
      ENDIF
      IF( renum .GT. 0 ) lbuf(3) = renum
!**** renum won't work if multiple channel sets. (intrcs is set to the sum
!**** of all the traces whereas itrcno is the trace number within each channel set.
      IF( renum .GT. 0 .AND. itrcno .GE. intrcs ) renum = renum + 1
      in = 0                                                            ! signal that the data is not in the ap
!****
!****    finished a trace, now set up ftr for the next shot
!****
      IF( ftr .NE. 99999 ) THEN
          fftr = fftr + trinc
!     ltr is set to intrcs if it wasn't given
!****   Check itrcno too since fftr is just a count, whereas itrcno 
!****   comes from the data!
          IF( fftr .GT. ltr .OR. itrcno .GE. ltr ) THEN                     ! past the last trace requested?
              fftr = ftr
              IF( ffilen .NE. 99999 .AND. filinc .NE. 99999 ) 
     &            ffilen = ffilen + filinc
              IF( ifilen .GE. lfilen ) THEN
                  IF( mlists .EQ. nlists ) THEN
                      istop = 1
                  ELSE
                      getlist = .TRUE.
                  ENDIF
              ENDIF
          ENDIF
      ENDIF
!****  Ah shoot, when doing multiple channel sets, itrcno is the
!****  trace number in the channel set, but ltr is counting the
!****  total number of traces in the file.  Let's assume that
!****  when that happens, retrac was used and the segy trace number is
!****  the count of the output traces (assumes ftr was not given).
!	print *,' ftr=',ftr,' ltr=',ltr,' intrcs=',intrcs,' lb=',lbuf(4)
!	print *,' ifilen=',ifilen,' lfilen=',lfilen,' list=',list
!	print *,' istop=',istop,' stack=',stack
      IF( ftr .EQ. 99999 .AND. ltr .EQ. intrcs .AND. 
     &    lbuf(4) .EQ. ltr ) THEN
          IF( ifilen .EQ. lfilen ) THEN
              istop = 1
              RETURN
          ENDIF
          IF( iformat .EQ. 2 .AND. list+stack+ldeolist .EQ. 0 ) THEN    ! reading the file directly
!****         read a bunch because seisnet has headers and trailers
              CALL rddiscb( iunit, scr, 5000, istat )
              CALL podiscb( iunit, 2, -5000 )
              IF( istat .NE. 100 ) THEN
                  IF( mlists .EQ. nlists ) THEN
                      istop = 1
                  ELSE
                      getlist = .TRUE.
                  ENDIF
              ENDIF
              RETURN
          ENDIF
          getnewshot = 1
      ENDIF
!****  make it work for now - sleepie
      IF( ftr .NE. 99999 .AND. ltr .EQ. intrcs .AND.
     &    lbuf(4) .EQ. ltr ) THEN
          IF( list+stack .NE. 0 ) getnewshot = 1
      ENDIF
      RETURN
!****
!****    Come here when the shot is truncated.
!****    Don't abort on truncated shots, move on to the next shot.
!****
  980 CONTINUE
      IF( istat .EQ. -1 ) THEN
          PRINT *,' End-Of-File detected.'
          istop = -1
          RETURN
      ENDIF
      PRINT *,' ***  WARNING  ***   Bad SEG-D general header.'
      PRINT *,'       Skipping to next shot.'
      GOTO 1000
  990 CONTINUE
!**** seisnet has a trailer, so a short trace is really just the trailer.
      IF( iformat .EQ. 2 ) GOTO 1000
      PRINT *,
     &' ***  WARNING  ***   Bad (short) trace - skipping to next shot .'
      PRINT *,'   Check file ',ifilen,' shot ',lbuf(3),' trace ',lbuf(4)
      PRINT *,'   Wanted ',nbytes,' read ',istat,' bytes.'
      GOTO 1000
!****
!****    No trace ready, get another shot
!****
 1000 CONTINUE
      IF( list .GT. 0 ) THEN
          CALL frefil( 2, iunit )
          CALL rline1(list)
          CALL getoke1( token, nchars )
          IF( nchars .EQ. 0 .AND. list .NE. 0 ) THEN
              istop = -1
              RETURN
          ENDIF
          IF( nchars .EQ. 0 ) THEN
              istop = 1
              RETURN
          ENDIF
          CALL getfil( 4, iunit, token, istat )
          IF( istat .NE. 0 ) THEN
              PRINT *,' ***  ERROR  ***  Could not open file ', token
              istop = -1
              RETURN
          ENDIF
          GOTO 100
      ENDIF
!****  Some compilers (Fedora and intel) don't allow GOTO into the middle
!****  of an if/then block, so set a flag and make 1010 a block of it's own.
      iflag = 0
!**** There all sorts of timing issues with the "current" shot, so
!**** let's always use the second from current
!**** I'm tempted to assume the inode/address of the file holding the
!**** pathname of the latest shot remains the same, it might not, so
!**** close the file after reading it.
      IF( stack+ldeolist .GT. 0 ) THEN
!****     Ethan wants stdout flushed
          CALL fdsync( 1 )
          CALL frefil( 2, iunit )
          iflag = 1
      ENDIF
      IF( iflag .EQ. 0 ) GOTO 1040
 1010     CONTINUE
!****     Use the SIOSEIS negative number in file IN/in convention of tapes.
          OPEN(UNIT=lunin,FILE='in',STATUS='UNKNOWN',IOSTAT=istat)
          READ(lunin,'(I4)',END=1020,ERR=1020)i
          CLOSE(UNIT=lunin,STATUS='DELETE')
 1015     IF( i .GE. 0 ) GOTO 1030
          IF( ldeolist .GT. 0 ) CLOSE(UNIT=ldeolist,STATUS='DELETE')
          IF( stack .GT. 0 ) THEN
              istop_stack = 1
          ELSE
              PRINT *,' User stop with file "in".'
              istop = -1
              RETURN
          ENDIF
 1020     CLOSE(UNIT=lunin,STATUS='DELETE')
          OPEN(UNIT=lunin,FILE='IN',STATUS='UNKNOWN',IOSTAT=istat)
          READ(lunin,'(I4)',END=1030,ERR=1030)i
          CLOSE(UNIT=lunin,STATUS='DELETE')
          IF( i .LT. 0 ) GOTO 1015
 1030     CONTINUE
!****  Need the next DELETE incase an empty or bad file IN exists
          CLOSE(UNIT=lunin,STATUS='DELETE')
!      ENDIF
 1040 CONTINUE
      IF( stack .GT. 0 ) THEN
!****     the stack loop never times out because shooting may be temporarily suspended
!****     because of mammals.  We don't want to have to restart sioseis everytime that happens.
!****     A bit of Marx brothers - Who is on first and What is on second.
          OPEN( UNIT=stack, FILE=cpath,
     &          FORM='FORMATTED', STATUS='OLD')
          CALL rline1(stack)   ! read first line in file stack
          CALL getoke1( top1, n1chars )
!****     damn, the inode may exist, but the name may not - slow down!
          IF( top1(1:1) .EQ. ' ' ) THEN
              CLOSE (UNIT=stack)
              PRINT *,' waiting for a pathname in stack file ',cpath
              CALL sleep(1)
              GOTO 1010
          ENDIF
          CALL rline1(stack)   ! read second line in file stack
          CLOSE (UNIT=stack)   ! we're finished with file stack
!****     Another gotcha.  If we're always taking the second to last,
!****     we never process the last shot!
          CALL getoke1( top2, n2chars )
          IF( top2(1:1) .EQ. ' ' ) THEN
              CALL sleep(1)
              GOTO 1010
          ENDIF
          IF( nchars4 .LT. 0 ) THEN    ! nchars4 is the number of characters rline1 read
              PRINT *,' Stopping due to nothing in the segddin stack.'
              istop = -1
              RETURN
          ENDIF
          IF( istop_stack .GT. 0 ) THEN    ! finish up cleanly
              IF( top1 .EQ. filename ) THEN
                  PRINT *,' Normal stop of segddin stack.'
                  istop = -1
                  RETURN
              ENDIF
!****     it's unclear if the second from top has been used
!****     filename is the name of the last file processed
              IF( top2 .EQ. filename ) THEN
                  filename = top1
                  nchars = n1chars
              ELSE
                  filename = top2
                  nchars = n2chars
              ENDIF
          ELSE
              filename = top2
              nchars = n2chars
          ENDIF
!****     crap.  There might be other crap in the seisnet directory
          IF( filename(nchars-3:nchars) .NE. '.sgd' .AND.
     &        filename(nchars-4:nchars) .NE. '.SEIS' ) THEN
              CALL sleep(1)
              GOTO 1010
          ENDIF
          CALL getfil( 4, iunit, filename, istat )
          IF( istat .NE. 0 ) THEN
              PRINT *,' ***  ERROR  ***  Could not open file ',filename
              CALL sleep(1)
              GOTO 1010
          ENDIF
          IF( IAND(lprint,8) .NE. 0 ) 
     &        PRINT *,' opened file: ',filename(1:nchars)
          GOTO 100
      ENDIF
      IF( ldeolist .GT. 0 ) THEN
          IF( ldeoopen ) THEN
              CALL rline1(ldeolist)
              IF( nchars4 .GT. 0 ) GOTO 1802
              PRINT *,' Read end of ldeolist, deleting the list.'
              CLOSE(UNIT=ldeolist,STATUS='DELETE')
              ldeoopen = .FALSE.
          ENDIF
 1800     PRINT *,' Waiting for LDEOLIST.'
          CALL SLEEP(10)
          INQUIRE( FILE=cpath, EXIST=iexist )
!****    IF( NOT(iexist) ) never turns false in Sun Solaris!
          IF( iexist ) GOTO 1801
          GOTO 1010
 1801     OPEN( UNIT=ldeolist, FILE=cpath,
     &          FORM='FORMATTED', STATUS='OLD')
          CALL rline1(ldeolist)
 1802     CALL getoke1( token, nchars )
          CALL getfil( 4, iunit, token, istat )
          IF( istat .LT. 0 ) THEN
              PRINT *,' ***  ERROR  ***  Check LDEOLIST.'
          ELSE
              ldeoopen = .TRUE.
!              PRINT *,' Getting file: ',token
          ENDIF
          GOTO 100
      ENDIF
      IF( iformat .EQ. 4 .OR. iformat .EQ. 5 ) THEN
          CALL frefil( 2, iunit )
          IF( ifilen .EQ. lfilen ) GOTO 2000
          itemp = ifilen + 1
          CALL bldgname( cpath, itemp, token )
          CALL getfil( 4, iunit, token, istat )
          IF( istat .LT. 0 ) THEN
              istop = -1
              RETURN
          ENDIF
          GOTO 100
      ENDIF
!****
!****   No traces left, no trace in buf,  STOP now!
!****
 2000 CONTINUE
      IF( iformat .EQ. 7 ) THEN
          CALL adrdisc( iunit, laddr )
          GOTO 130   ! no storage label this time
      ENDIF
      istop = -1
      RETURN
      END
