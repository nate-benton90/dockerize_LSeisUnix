      SUBROUTINE doex( ndisko, ibuf, buf, lbuf, iscr, scr, lscr, istop )
!     doex is the execution phase of the SIOSEIS process DISKOX, where X
! can be any of 4 different processes.  i.e. there can be 4 processes,
! DISKOA, DISKOB, DISKOC, and DISKOD, all using this one subroutine.
! Therefore, this routine must keep track of up to 4 sets of variables.
!     This guy permits traces to be dropped from the output file, will
! set/change shot and trace numbers and reformat the data all without 
! altering the data in buf (the input data and the data passed to the next
! process.
!    This thing only works if the shot/rp numbers are monotonically increasing.
!    This renumbers the output shot/rp numbers and trace numbers so there are
!  monotonically increasing.
!    The disk writes must be done in two steps, the header and the data because
!  in order to read the correct amount of data you need to know how much to
!  read, which is in the header.  This is particularily important in VMS because
!  the header and data are in different "records".
!      NOTE:  The SEGY headers in the temporary file are not really SEGY
!  since the binary header is in host oriented integers rather than SEGY
!  (In other words DEC machine don't have to swap the bytes on the SEGY
!  "tape header" from this temporary disk file"
!
!  ARGUMENTS:
!  ndisko - The number of the disko being called.  Must be between 1 and 4.
!  ibuf, buf, lbuf - The input data.
!  iscr, scr, lscr - A scratch array.
!
!  Copyright (C), The Regents of the University of California, 1988
!  Written by Paul Henkart, Scripps Institution of Oceanography,
!      La Jolla, Ca. 92093   January 1988
!  ALL RIGHTS RESERVED.
!
!  mods:  
!  20 Mar 90 by pch to put the sample interval (in mics) and the number of
!      samples of the first trace into the SEGY binary header because
!      some stupid packages like GEOQUEST and Galbrath's Vista need them!
!  30 July 91 by pch to add the desort stuff
!   4 Mar 91 by pch to add osecs and decimf
!  12 May 92 by pch - Change icompt = 2 from DEC BSD to DecStation Ultrix
!  7 July 92 by pch - If ontrcs isn't given, use notrcs from common
!      /writet/ if it's non zero, else use intrcs from common /readt/
!      (stack sets notrcs)
!  13 Mar 95 - Set the header for the delay to be the "real" time of
!      the first sample when using SET (some non even sample intervals
!      can lead to roundoff errors).
!  8 May 95 - Do the above correctly!
!  17 May 95 - Add the SU format option.
!  20 June 95 by mwh - Updated binary header values plus floating point
!      version of sampling interval to reflect new sampling interval
!      when decimf > 1
!  5 Aug 95 - Write "IRIS" header if nsamps > 32767
!           - Remove Cray CTSS Mass Storage junk.
!  29 Jan. 96 - Change lrange preset from 999999 to NONE
!  21 Aug. 96 - Add the rewind parameter to start a new file on every trace 1
!  25 Sep. 96 - posaft didn't work because binary header read was wrong!
!             - posaft > 0 didn't work when posaft was last in file
!  17 Mar. 97 - Add FLINC
!  5 Apr 99 - Use lastno (last shot/rp number) for checking new shot/rp
!            rather than trace 1 (might not have a trace 1)!
!  7 June 99 - Make BIG use getfil64
!  13 July 99 - Increase MAXDO from 4 to 10.  Now up to diskoj.
!  18 Dec 99 - Error out "gracefully" if OPATH is not writable.
!  29 Feb 01 - Add a bunch of swaps for little endian (dec and pc)
!  24 July 02 - Count the lists read only if they are read. (Prevent
!               counting and overflow when traces > lno).
!  2 Aug 02 - SAVE mlists  (oops)
!  27 Aug 02 - PC needed a byte swap on trace header when posaft -1
!  15 Nov 02 - PC needs data byte swapped on IEEE fp
!  7 Jan 03 - Make the delay into a 32bit integer, violating the SEG-Y
!             standard and wiping out the SEG-Y "lag B" entry.  Only
!             used when parameter set is given.
!  21 Jan 03 - Increase the bug check for max diskoxes from 5 to 10.
!  18 Mar 03 - Add parameter TRACE0. Write trace 0 to output SEGY file.
!  1 May 03 - Add wriing of the Rev 1 Extended Textual Headers
!           - Add parameter EXTHDR to control writing of those headers!
!  14 May 03 - Implement the REV 1 fixed trace length flag.
!              SIOSEIS will modify the formal definition which says
!  the fixed trace flag mean constant trace length and sample interval.
!  I think the intent of this flag was to allow randon access to the
!  traces, so the sample interval changing doesn't matter.  More
!  important is that no traces or ensembles (shots or rps) are missing,
!  as well as the constant number of samples per trace.
!  27 May 03 - Always rewrite si and nsamps in the binary header.
!  mod 4 Jun 03 - Add FORMAT BINARY, no headers at all
!  24 Jun 03 - Do checks for SEG-Y Rev 1 differently so bad headers
!              (e.g. Edgetech) don't cause Extended headers.
!  9 July 03 - Trace 0 out didn't honor noinc
!  18 July 03 - Keep track of how many traces are being written and
!               write it to trace header long word 2.
!  25 Mar 05 - OFMT 0 means same as IFMT.
!  10 June 05 - Honor newfile, which is really segddin parameter & flag
!  13 June 05 - Create filename if DATE or SHOTNO.
!  6 Sept 05 - Take care of large (>32767) samples by using unsigned
!              16 bit integer (max of 65535) rather than IRIS scheme.
!  mod 27 Apr 06 - gfortran chokes on internal reads that were necessary for VMS
!                - enddox had a bad index which caused a sementation fault
!  mod 25 Jul 06 - Allow nsamps (segy short word 58) to be an unsigned int.
!  mod 4 Oct 06 - Don't do newfile if not OPATH DATE or OPATH SHOTNO
!  mod 23 Apr 08 - trace0 didn't work on Intel
!  mod 30 Jun 08 - nsamps > 16384 didn't work on Intel
!                - Get rid of all the cray crap
!                - Get rid of the AP code
!  mod 1 July 08 - g95 doesn't like FLOAT() of an integer*2
!                - didn't decimate enough samps (probably due to 30 Jun)
!  mod 10 Jul 08  - Do decimation before sets.
!  mod 4 Aug 08 - The initial delay wasn't set when set = delay
!  mod 7 Aug 08 - Check for SET causing too big a trace.
!               - Also watch out for the end SET being after the delay.
!  mod 24 Sep 08 - Change desort addresses from bytes to unsigned words.
!                - Desort didn't work - the sort file "format" changed long ago.
!                - Get rid of some Cray (icompt 5) oriented code.
!  mod 26 Sep 08 - Do the trace header number of samples as unsigned short in c
!  mod 12 Jan 09 - Rev 1 fixed length flag wasn't being set.
!  17 Mar 09 - Change segy bin hdr ntraces only if the user gave it.
!  mod 17 Jun 09 - Add RETRAC (RENUM was done in DOED as the same as FNO)
!  mod 26 Jun 09 - Set the fixed flag properly when writing gathers.
!  mod 16 Jul 09 - Use the binary header from ihunit so process header changes take place
!                - Setting ontrcs seemed (and was) wrong.
!  mod 4 Nov 09 - fon, retrac, and ontrcs doesn't work "right".  Need to
!                  set intrcs!
!  26 June 09 - Make SEG-Y Rev 1 Fixed mean shot/rp have the same number
!  of traces and that shot/rp numbers and trace numbers increase monotonically.
!  11 June 11 - Set the binary header sort entry (15) based on rp trace number
!  12 June 12 - POSAFT was using the number of samples from the input rather than output. (ajh)
!  mod 2 Jul 12 - posaft -1 didn't work correctly if number of samples was different
!               - set fixed to 0 if number of samples is different.
!  5 Jul 12 - Binary header sort entry of 11 June 11 was wrong
!  31 Jul 12 - delay in ms set incorrectly when SET is given
!  5 Feb 14 - Don't set the output segy rev to 1 if the input
!            segy rev was 0 and the delay scalar (trace header word 108) is non-zero
!  6 Feb 14 - Byte swapping of trace header bytes 181-240 is ugly for rev 1
!             Sioseis only uses the delay scalar, so that's all we worry about for now.
!  7 Oct 14 - Use swp_trhdr for trace header byte swap so diskin and disko are the same
!  7 Feb 15 - common /readt/ was wrong
!  15 Nov 15 - zero the rev 1 delay scalar when SET is used
!  25 Jul 18 - SET was off by 1 sample
!
      PARAMETER ( maxdo = 10 )                                           ! the maximum number of process diskos
      DIMENSION buf(10000), scr(10000), set(2,maxdo)
      INTEGER*2 iscr(10000), ibuf(10000)
      INTEGER lbuf(10000), lscr(10000)
      CHARACTER*200 patold(maxdo), opath(maxdo), pathname
      CHARACTER*80 token
      INTEGER ofmt(maxdo), fno(maxdo), lno(maxdo), noinc(maxdo),
     *        ftr(maxdo), ltr(maxdo), trinc(maxdo), fon(maxdo),
     *        ontrcs(maxdo), posaft(maxdo), lprint(maxdo), iform(maxdo),
     *        mass(maxdo), decimf(maxdo), lunsort(maxdo),mtraces(maxdo),
     *        rewind(maxdo), flinc(maxdo), lastno(maxdo),ibig(maxdo),
     *        itrace0(maxdo), iexthdr(maxdo), lasttr(maxdo), 
     *        fixed(maxdo), numsamps(maxdo), ntrdone(maxdo),
     *        micros(maxdo), iappend(maxdo), retrac(maxdo), 
     *        last_ncdp(maxdo), numsave(maxdo)
      REAL frange(maxdo), lrange(maxdo), osecs(maxdo)
      INTEGER mlists(maxdo), idunit(maxdo), itrno(maxdo)
      LOGICAL first(maxdo)
      COMMON /apmem/ a(65536)
      COMMON /diskox/ ipunit(maxdo), nlists(maxdo), npwrds
      COMMON /edits/ ierror, iwarn, irun, now, icompt, isite, maxsamps
      COMMON /readt/ itunit, numhdr, numdat, ihunit, ireeln, intrcs,
     *               ifmt, nskip, secs, lrenum, isrcf, idtype,
     *               nfskip, jform, itxsi, itxdel, nfktrc, norigtr,
     &               nrskip, nfiles, irewind, delay, rev_in, si
      COMMON /segdin/ idummy1(3), luntr0, luntr0a, lunldeo
      COMMON /WRITET/ idummy2(5), notrcs, idummy3(4), newfile
      COMMON /sioap/ iasgnd, irelse, in, jout, nextad, lapsiz, ifree,
     *               iuseap, idecim
      COMMON /binhdr/ ibinhdr(200)
      INTEGER*2 ibinhdr
      COMMON /segyptr/ llsegptr, lrseqptr, lshotptr, lshtrptr, lrpnptr,
     *                 lrptrptr, itridptr, ldisptr,  lwbdptr,  lsxcoptr,
     *                 lrxcoptr, idelmptr, istmptr,  iendmptr, isampptr,
     *                 isiptr,   iyrptr,   idayptr,  ihrptr,   iminptr,
     *                 isecptr,  igmtptr,  ldelsptr, lsmusptr, lemusptr,
     *                 lsisptr,  lwbtsptr, lgatptr,  lssmsptr, lesmsptr,
     *                 lsbptr,   ifoldptr, icvleptr, lespnptr
      COMMON /sort1/ ldummy, lunin, lunout, lkey1, ikey1, limit1(2),
     &            lkey2, ikey2, limit2(2), iflag51, reverse1, reverse2
!.. Constants for file format types
      PARAMETER( SIO = 1, ARAB = 2, SU = 3, BINARY = 4 )
!
!**** do Alistair's fk include  -  include 'splitgbl.inc'
!
!.. "Global" Include file for spltfk routines that reformat an FK dataset i
!.. on output. These declarations are included in DOEX.
      integer   IPTFKU, NOFKCONV
      parameter (NOFKCONV = 0)
      parameter (IPTFKU   = 4)
!
!.. Entry constants for spltfk
      integer TRNSPARM, CHCKBNY, WRTETRC
      parameter( TRNSPARM = 1)
      parameter( CHCKBNY  = 2)
      parameter( WRTETRC  = 3)
!
!.. Holds data types for the various output streams
      integer DataID(MAXDO)
      common /spltblk1/ DataID
      save /spltblk1/

      SAVE patold, ofmt, fno, lno, noinc, ftr, ltr, trinc, fon, set,
     *     first, ontrcs, idunit, lprint, iform, itrno, frange, lrange,
     *     mass, osecs, decimf, lunsort, mtraces, ntraces, rewind,
     *     flinc, lastno, lasttr, fixed, numsamps, itrace0, ntrdone,
     *     opath, posaft, micros, iappend, retrac, last_ncdp, numsave,
     *     rev_out
      SAVE mlists, iprtwarn
      DATA first/maxdo*.TRUE./, patold/maxdo*'old'/, itrno/maxdo*1/,
     *     idarn/0/, idunit/maxdo*0/, lastno/maxdo*-1/
      DATA iprtwarn/0/, iappend/maxdo*0/
!****
!****
!****
      idisko = ndisko                                                   ! watch out for indirect addressing (slower)
!****  argh.  Something in OSX is writing over us.
      IF( idisko .LT. 0 .OR. idisko .GT. 10 )THEN
          PRINT *,' Bad doex, idisko=',idisko
          STOP
      ENDIF
      IF( newfile .EQ. 1 ) GOTO 104
      IF( .NOT. first(idisko) ) GOTO 200                                ! first time for this disko?
      first(idisko) = .FALSE.
      CALL podisc( ipunit(idisko), 1, 0 )                               ! rewind the parameter file
      mlists(idisko) = 0
      mtraces(idisko) = 0
      lastno(idisko) = -1
      lasttr(idisko) = -1                                               ! look out for trace 0
      fixed(idisko) = 1
      numsamps(idisko) = 0
      ntrdone(idisko) = 0
      micros(idisko) = 0
      numsave(idisko) = -1
      rev_out = 1
  100 CONTINUE                                                          ! get a parameter list
      IF( mlists(idisko) .lt. 0 .or. mlists(idisko) .GT. 10) THEN
          PRINT *,' Bad doex1, idisko=',idisko,' mlists=',mlists(idisko)
          STOP
      ENDIF
      IF( mlists(idisko) .GE. nlists(idisko) ) GOTO 9000                ! the data must be past the last list of disko parameters
      mlists(idisko) = mlists(idisko) + 1                               ! count the parameters list
!**** doed write 200 characters of opath, followed by npwrds-25 
!**** this is some old hang over
      CALL rddisc( ipunit(idisko), opath(idisko), 50, istat )
      CALL rddisc( ipunit(idisko), scr(26), npwrds-25, istat )                 ! read a parameter list
      IF( istat .NE. npwrds-25 ) THEN
          PRINT *,' ***  SIOSEIS ABORT in doex ***'
          PRINT *,' npwrds=',npwrds,' istat=',istat,' idisko=',idisko,
     &   ' lists=', mlists(idisko),nlists(idisko)
          STOP
      ENDIF
!      WRITE( opath(idisko), '(25A4)' ) (lscr(i),i=1,25)
      fno(idisko) = lscr(26)
      lno(idisko) = lscr(27)
      noinc(idisko) = lscr(28)
      ftr(idisko) = lscr(29)
      ltr(idisko) = lscr(30)
      trinc(idisko) = lscr(31)
      fon(idisko) = lscr(32)
      ofmt(idisko) = lscr(33)
      IF( ofmt(idisko) .EQ. 0 ) ofmt(idisko) = ifmt
      ontrcs(idisko) = lscr(34)
!      IF( ontrcs(idisko) .LE. 0 .AND. ftr(idisko) .GE. 0 .AND.
!     &   ltr(idisko) .GE. 0 ) ontrcs(idisko) = ltr(idisko) - ftr(idisko)
      posaft(idisko) = lscr(35)
      iform(idisko) = lscr(36)
      lprint(idisko) = lscr(37)
      set(1,idisko) = scr(38)
      set(2,idisko) = scr(39)
      frange(idisko) = scr(40)
      lrange(idisko) = scr(41)
      mass(idisko) = lscr(42)
      osecs(idisko) = scr(43)
      decimf(idisko) = lscr(44)
      lunsort(idisko) = lscr(45)
      rewind(idisko) = lscr(46)
      flinc(idisko) = lscr(47)
      ibig(idisko) = lscr(48)
      itrace0(idisko) = lscr(49)
      iexthdr(idisko) = lscr(50)
      retrac(idisko) = lscr(51)
      CALL spltfk( TRNSPARM, idunit(idisko), idisko,                    ! FK domain ain't done by doex!
     *              buf, lbuf, ibuf, scr, lscr, iscr )
      IF( IAND(lprint(idisko),2) .NE. 0 ) THEN
          PRINT *,opath(idisko)
          PRINT *, (lscr(i),i=26,37)
          PRINT *, (scr(i),i=38,41),lscr(42)
          PRINT *, scr(43), (lscr(i),i=44,51)
      ENDIF
!****  if appending, see if it's the same file as used before
      IF( posaft(idisko) .LT. 0 .AND. idisko .GT. 1 ) THEN
          DO i = 1, idisko-1
             IF( opath(i) .EQ. opath(idisko) ) 
     &           iappend(idisko) = i
          ENDDO
      ENDIF
!****  the following destroys my segy rev 1 fixed logic, so set fixed to NOT
!      IF( ontrcs(idisko) .EQ. 0 .AND. lbuf(7) .EQ. 0 ) THEN
!          ontrcs(idisko) = intrcs
!          fixed(idisko) = 0
!      ENDIF
  104 IF( opath(idisko) .NE. patold(idisko) .OR. newfile .EQ. 1) THEN           ! have we seen this file before?
          IF( newfile .EQ. 1 .AND. opath(idisko)(1:4) .NE. 'DATE' .AND.
     &        opath(idisko)(1:4) .NE. 'date' .AND.opath(idisko)(1:6).NE.
     &        'SHOTNO' .AND. opath(idisko)(1:6) .NE. 'shotno' ) THEN
              PRINT *,' ***  WARNING  ***  New output file not opened.',
     &        ' Use OPATH DATE or OPATH SHOTNO.'
              GOTO 200
          ENDIF
          IF( idunit(idisko) .NE. 0) CALL frefil(2,idunit(idisko),istat)! close the old file
          pathname = opath(idisko)
          IF( opath(idisko)(1:4) .EQ. 'DATE' .OR. 
     &        opath(idisko)(1:4) .EQ. 'date' ) THEN
              WRITE( pathname, 105 ) ibuf(80), ibuf(81), ibuf(82)
  105         FORMAT('day',I3.3,'-',2I2.2,'z.segy')
          ENDIF
          IF( opath(idisko)(1:6) .EQ. 'SHOTNO' .OR. 
     &        opath(idisko)(1:6).EQ.'shotno') THEN
              WRITE( pathname, 106 ) lbuf(3)
  106         FORMAT('shot',I6.6,'.segy')
          ENDIF
          IF( posaft(idisko) .EQ. 0 ) THEN                              ! do we start at the beginning of the file?
              IF( ibig(idisko) .EQ. 0 ) THEN
                  CALL getfil(3, idunit(idisko), pathname, istat )      ! create a new file
              ELSE
                  CALL getfil64(3, idunit(idisko), pathname, istat )
              ENDIF
              IF( istat .LT. 0 ) THEN
                  PRINT *,' ***  ERROR  ***  Can not write to output',
     &                ' file ',pathname
                  STOP
              ENDIF
              CALL podisc( ihunit, 1, 0 )                               ! rewind the SEGY header file
!****  The EBCDIC header is in EBCDIC on disk
              CALL rddisc( ihunit, lscr, 800, istat )                   ! get the EBCDIC header
              IF( iform(idisko) .NE. SU .AND. 
     &            iform(idisko) .NE. BINARY) 
     &            CALL wrdisc( idunit(idisko), lscr, 800 )
              CALL rddisc( ihunit, iscr, 100, istat )                   ! get the binary header
              IF( ontrcs(idisko) .GT. 0 ) iscr(7) = ontrcs(idisko)
              iscr(9) = ibuf(59) * decimf(idisko)                       ! the sample interval in mics of the first trace
!              itemp = ibuf(58)
!              IF( itemp .EQ. 32768 ) itemp = lbuf(58)
!              iscr(11) = itemp / decimf(idisko)                         ! the number of samples of the first trace
              iscr(11) = numdat / decimf(idisko)                        ! the number of samples of the first trace
!****         g95 doesn't do  FLOAT(integer*2)
              temp = iscr(9)
              IF( set(idisko,2) .NE. 0 ) 
     &            iscr(9) = (set(idisko,2)-set(idisko,1)) / temp + 1
              iscr(13) = ofmt(idisko)
              iscr(31) = idtype
              iscr(32) = nfktrc                                         ! the number of trace in the fk domain
              iscr(33) = itxsi                                          ! the tx domain sample interval in microsecond
              iscr(34) = itxdel                                         ! the tx domain time delay
              iscr(36) = norigtr                                        ! the number of original traces (before tx2fk)
              CALL spltfk( CHCKBNY, idunit(idisko), idisko,             ! FK domain ain't done by doex!
     *              buf, lbuf, ibuf, scr, lscr, iscr )
              nextra = 0
              segyrev = REAL(iscr(151)) / 256.
              IF( segyrev .GE. 1.0 .AND. segyrev .LT. 2.0 .AND.
     &            iscr(153) .GT. 0 ) THEN
                  IF( iexthdr(idisko) .EQ. 0 ) THEN
                      iscr(153) = 0
                      PRINT *,
     &    ' ***  WARNING  ***  SEG-Y Rev 1 records will not be written.'
                      iwarn = iwarn + 1
                  ELSE
                      nextra = iscr(153)
                  ENDIF
              ENDIF
              IF(icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap16(iscr,200)
              IF( iform(idisko) .NE. SU .AND. iform(idisko) .NE. BINARY) 
     &            CALL wrdisc( idunit(idisko), iscr, 100 )
              IF( nextra .GT. 0 ) THEN
                  DO i = 1, nextra
                     CALL rddiscb( ihunit, iscr, 3200, istat )
                     CALL wrdiscb( idunit(idisko), iscr, 3200 )
                  ENDDO
              ENDIF
!             END IF( posaft(idisko) .EQ. 0 ) THEN 
          ELSE
!             START IF( posaft(idisko) .NE. 0 ) THEN 
!****         getfil(4 should handle a BIG file without being told
              CALL getfil( 4, idunit(idisko), pathname, istat )         ! open the existing file opath
              CALL rddisc( idunit(idisko), scr, 800, istat )            ! get the EBCDIC header
              CALL rddisc( idunit(idisko), scr, 100, istat )            ! get the binary header
              IF( icompt .EQ. 2 .OR. icompt .EQ.4) CALL swap16(iscr,200)
              IF( ontrcs(idisko) .GT. 0 ) iscr(7) = ontrcs(idisko)
              iscr(13) = ofmt(idisko)
  110         CONTINUE
              n = numhdr
              CALL rddisc( idunit(idisko), scr, n, istat )              ! get the trace header
              IF( posaft(idisko) .LT. 0 .AND. istat .LT. 0 ) GOTO 120   ! did we find the end of file?
              IF( istat .NE. n ) THEN
                  IF( istat .EQ. -1 .AND. posaft(idisko).EQ.no) GOTO 120
                  PRINT *,' ***  ERROR  ***  Trouble positioning file ',
     *             pathname
                  STOP
              ENDIF
              IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) 
     &            CALL swp_trhdr( iscr, lscr )
              IF( iscr(isampptr) .NE. nsamps ) THEN
!****   number of samples is an unsigned short
                  CALL ushort2long( iscr(isampptr), nsamps )
                  fixed(idisko) = 0
              ENDIF
              IF( lscr(7) .EQ. 0 ) THEN                                 ! determine the shot/rp and trace number of this one
                  no = lscr(3)                                          ! shot
                  itr = lscr(4)
              ELSE
                  no = lscr(6)                                          ! rp
                  itr = lscr(7)
              ENDIF
              n = nsamps
              IF( ofmt(idisko) .EQ. 3 .OR.ofmt(idisko) .EQ. 4) n = n / 2! 16 bit samples?
              CALL rddisc( idunit(idisko), scr, n, istat )
              IF( posaft(idisko) .LT. 0 ) GOTO 110                      ! get another trace
              IF( no .NE. posaft(idisko) .OR. itr .LT. intrcs ) GOTO 110! must be posaft > 0
          ENDIF
  120     CONTINUE
          IF( fon(idisko) .LT. 0 ) fon(idisko) = no + 1
!****     set ontrcs to 1 if it's a stacked trace
!****  this doesn't seem right - pch 16 jul 09
!          IF( ontrcs(idisko) .LE. 0 .AND. notrcs .EQ. 0 .AND. 
!     &       lbuf(7) .EQ. 0 ) THEN
!                  ontrcs(idisko) = intrcs
!          ELSE
!              IF( notrcs .NE. 0 ) ontrcs(idisko) = notrcs
!          ENDIF
          patold(idisko) = opath(idisko)
      ENDIF
  200 CONTINUE
!****
!****   Square the sort file away if this is the first time.  Make a 
!****   copy of it because someone else might use it and then our
!****   positioning within it will be wrong.
!****     Fill up the output file because the sorted traces will not
!****     be placed sequentially in the file.
!****
      IF( lunsort(idisko) .NE. 0 .AND. mtraces(idisko) .EQ. 0 ) THEN
          itemp = lunsort(idisko)
          CALL getfil( 1, lunsort(idisko), token, istat )               ! open a scratch file
          CALL podisc( itemp, 1, 0 )                                    ! rewind
          CALL rddisc( itemp, token, 20, istat )
          CALL wrdisc( lunsort(idisko), token, 20 )
          CALL rddisc( itemp, ntraces, 1, istat )
          CALL wrdisc( lunsort(idisko), ntraces, 1, istat )
          DO i = 1, 4
             CALL rddisc( itemp, scr, ntraces, istat )
             CALL wrdisc( lunsort(idisko), scr, ntraces )
          ENDDO
          CALL frefil( 2, itemp, istat )
          CALL podisc( lunsort(idisko), 1, 0 )                          ! rewind
          CALL rddisc( lunsort(idisko), token, 20, istat )
          CALL rddisc( lunsort(idisko), ntraces, 1, istat )
!          CALL rddisc( lunsort(idisko), lscr, ntraces*3, istat )        ! get past the disk addreses
          CALL podisc( lunsort(idisko), 2, ntraces*3 )
          DO 205 i = 1, ntraces
             CALL rddisc( lunsort(idisko), itemp, 1, istat )            ! get the number of samples
             CALL wrdisc( idunit(idisko), scr, itemp+numhdr )           ! write a gabage trace
  205     CONTINUE
!****     now position the sort file to the list of input disk addresses
          CALL podisc( lunsort(idisko), 1, 0 )                          ! rewind   
          CALL rddisc( lunsort(idisko), token, 20, istat )
          CALL rddisc( lunsort(idisko), ntraces, 1, istat )
      ENDIF
!      nsamps = ibuf(isampptr)
!      IF( IAND(ibuf(58),32768) .NE. 0 ) nsamps = IAND(lbuf(29),65535)
!      IF( nsamps .EQ. 32767 ) nsamps = lbuf(isampptr)
      nsamps = numdat
!****
!****  Write a trace 0 if requested and the trace in buf is trace 1.
!****  Shot.  Trace 0 may break the SEG-Y Rev 1 logic.  arghhh.
!****
      IF( lbuf(4) .EQ. 1 .AND. luntr0 .NE. 0 .AND. 
     &    itrace0(idisko) .EQ. 1 ) THEN
          IF( noinc(idisko) .NE. 1 ) THEN
              DO i = fno(idisko), lno(idisko), noinc(idisko)
                 IF( lbuf(3) .EQ. i ) GOTO 206
              ENDDO
              GOTO 209
          ENDIF
  206     CONTINUE
          DO i = 1, numhdr
             lscr(i) = lbuf(i)
          ENDDO
          lscr(4) = 0                                                   ! trace 0
          iscr(15) = 28                                                 ! trace id = 28
!*****    some other processing systems choke if all traces are not the
!*****    the same length, so write nsamps
          iscr(58) = nsamps                                             ! do the same length as the data trace for other systems!
!****   The following was to allow unsigned integer in iscr(58)
!****   Don't forget to byte swap it on intel!
!          IF( iscr(57) .EQ. 0 ) lscr(29) = nsamps
          CALL podiscb( luntr0, 0, 0 )
          CALL rddisc( luntr0, nbytes, 1, istat )
          CALL rddiscb( luntr0, lscr(numhdr+1), nbytes, istat )
          IF( nbytes .GT. nsamps*4 ) THEN
              IF( iprtwarn .LT. 10 ) THEN
                  iprtwarn = iprtwarn + 1
                  PRINT *,' ***  WARNING  ***  Trace 0 is truncated.',
     &               ' general header nbytes=',nbytes,
     &               ' data trace bytes =',nsamps*4
              ENDIF
          ENDIF
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) 
     &        CALL swp_trhdr( iscr, lscr )
          IF( iform(idisko) .NE. BINARY)
     &        CALL wrdisc( idunit(idisko), scr(1), numhdr )
          CALL wrdisc( idunit(idisko), lscr(numhdr+1), nsamps )
      ENDIF   !  end trace 0 stuff
  209 CONTINUE
!****
!****
!****
      IF( dataid(idisko) .EQ. IPTFKU ) THEN
          jout = 0                                                      ! tell rlseap to get the data out of the ap
          CALL rlseap( buf(numhdr+1), nsamps )                          ! get the data out of the ap if it is in the ap
          CALL spltfk( WRTETRC, idunit(idisko), idisko,                 ! FK domain ain't done by doex!
     *              buf, lbuf, ibuf, scr, lscr, iscr )
          RETURN
      ENDIF
      IF( lbuf(7) .EQ. 0 ) THEN                                         ! get the current shot/rp number
          no = lbuf(3)
          itr = lbuf(4)
      ELSE
          no = lbuf(6)
          itr = lbuf(7)
      ENDIF
!      print *,' no=',no,' itr=',itr,' fno=',fno(idisko),' ftr=',
!     &   ftr(idisko),' lno=',lno(idisko),' ltr=',ltr(idisko),
!     &   ' flinc=',flinc(idisko)
      IF( lno(idisko) .GT. 0 .AND. no .GT. lno(idisko) .AND. 
     &    flinc(idisko) .GT. 0 ) THEN
          fno(idisko) = fno(idisko) + flinc(idisko)
          lno(idisko) = lno(idisko) + flinc(idisko)
      ENDIF
!****
!****   If appending a file used by another diskox previous in the procs list,
!****    sync up the disk addresses  since they have different address pointers
!****
      IF( iappend(idisko) .NE. 0 ) THEN
          CALL filsiz( opath(iappend(idisko)), laddress )
          CALL podiscb( idunit(idisko), 1, laddress )
      ENDIF
!****
!****
      IF( no .LT. fno(idisko) )  GOTO 9000                              ! is this before the current fno?
      IF( itr .LT. ftr(idisko) ) THEN
!****    my random disk input assumes the first trace number is 1
          fixed(idisko) = 0
          GOTO 9000
      ENDIF
      IF( no .GT. lno(idisko) .AND. lno(idisko) .GT. 0 ) GOTO 100       ! get another parameter list
      IF( itr .GT. ltr(idisko) .AND. ltr(idisko) .GT. 0 ) GOTO 9000
      IF( noinc(idisko) .NE. 1 ) THEN
          fixed(idisko) = 0
          DO 210 i = fno(idisko), lno(idisko), noinc(idisko)
             IF( no .EQ. i ) GOTO 220
  210     CONTINUE
          GOTO 9000
      ENDIF
  220 IF( trinc(idisko) .NE. 1 ) THEN
          fixed(idisko) = 0
          DO 230 i = ftr(idisko), ltr(idisko), trinc(idisko)
             IF( itr .EQ. i ) GOTO 240
  230     CONTINUE
          GOTO 9000
      ENDIF
  240 CONTINUE
      temp = ABS(lbuf(10))
      IF( temp .LT. frange(idisko) .OR. (lrange(idisko) .NE. 999999 
     &    .AND. temp .GT. lrange(idisko) ) ) GOTO 9000
!****
!****  If the file has fixed trace length so far, see if this is too.
!****  Rev 1 says same sample interval and same number of samples.  Nothing
!**** about monotonically increasing shot numbers or missing traces,
!**** but DISKIN does make that assumption, so require it.
!****
      IF( fixed(idisko) .NE. 0 .AND. lastno(idisko) .GE. 0 .AND.
     &    lasttr(idisko) .GE. 0 ) THEN
          IF( no .EQ. lastno(idisko) ) THEN                             ! different shot/rp?
              IF( itr .NE. lasttr(idisko) + 1 ) fixed(idisko) = 0
          ELSE
              IF( itr .GT. 1 ) fixed(idisko) = 0
              IF( no .NE. lastno(idisko) + 1 ) fixed(idisko) = 0
              IF( ontrcs(idisko) .EQ. 0 ) ontrcs(idisko) =lasttr(idisko)
              IF( ontrcs(idisko) .NE. lasttr(idisko) ) fixed(idisko) = 0
          ENDIF
      ENDIF
!**** if sorted by rp, see if this rp has the same number of traces at the last
      IF( lbuf(7) .NE. 0 .AND. fixed(idisko) .NE. 0 ) THEN
          IF( last_ncdp(idisko) .GT. 0 ) THEN
              IF( lbuf(51) .EQ. -1 .AND. last_ncdp(idisko) .NE. lbuf(7))
     &            fixed(idisko) = 0
          ELSE
              IF( lbuf(51) .EQ. -1 ) last_ncdp(idisko) = lbuf(7)
          ENDIF
      ENDIF
!**** disko sets the output segy rev to 1, but if the input rev was zero and the
!**** rev 1 delay scalar is set, then the next read of the file will honor the delay scalar
      IF( rev_in .EQ. 0 .AND. ibuf(108) .NE. 0 ) rev_out = 0
!****
!****   We now have a trace to be output!
!****   Do all work in the scratch array because the data in buf needs
!****  to be passed to the next process, which wants it in host format!
!****
!**** rewind means "start a new SEGY file" for EVERY shot/rp in an
!**** effort to create a "circular" file with only one shot/rp in it.
!**** Weird happenings when I did this just using podiscb - the file
!**** wouldn't get positioned correctly, I guess because someone was
!**** using over NFS perhaps.  So open and close it instead.
!**** It doesn't rewind if trace 1 is omitted by segdin!
      IF( rewind(idisko) .EQ. 1 .AND. no .NE. lastno(idisko) ) THEN
          CALL frefil( -2, idunit(idisko), istat )                      ! free the file but not the unit number
          CALL getfil( -4, idunit(idisko), patold(idisko), istat )      ! open the file on the unit
          CALL podiscb( idunit(idisko), 0, 3600 )
      ENDIF
      ntrdone(idisko) = ntrdone(idisko) + 1
      lbuf(2) = ntrdone(idisko)
      DO i = 1, numhdr                                              ! move the trace header
  500    lscr(i) = lbuf(i)
      ENDDO
!**** 
      IF( fon(idisko) .NE. 0 .AND. retrac(idisko) .GE. 0 .AND.
     &    ontrcs(idisko) .EQ. 0 ) THEN
          IF( numsave(idisko) .EQ. -1 ) THEN
              numsave(idisko) = no
              itrno(idisko) = retrac(idisko)
          ENDIF
          IF( numsave(idisko) .NE. no ) THEN
              fon(idisko) = fon(idisko) + 1
              itrno(idisko) = retrac(idisko)
          ENDIF
          numsave(idisko) = no
          IF( lbuf(7) .EQ. 0 ) THEN
              lscr(3) = fon(idisko)
              lscr(4) = itrno(idisko)
          ELSE
              lscr(6) = fon(idisko) 
              lscr(7) = itrno(idisko)
          ENDIF
          itrno(idisko) = itrno(idisko) + 1
      ENDIF
      IF( fon(idisko) .NE. 0 .AND. retrac(idisko) .GE. 0 .AND.
     &    ontrcs(idisko) .NE. 0 ) THEN
          IF( numsave(idisko) .LT. 0 ) THEN
              itrno(idisko) = retrac(idisko)
              numsave(idisko) = 0
          ENDIF
          IF( lbuf(7) .EQ. 0 ) THEN
              lscr(3) = fon(idisko)                                     ! set the shot number
              lscr(4) = itrno(idisko)
          ELSE
              lscr(6) = fon(idisko)
              lscr(7) = itrno(idisko)
          ENDIF
          itrno(idisko) = itrno(idisko) + 1
          IF( itrno(idisko)-retrac(idisko)+1 .GT.ontrcs(idisko))THEN
              fon(idisko) = fon(idisko) + 1
              itrno(idisko) = retrac(idisko)
          ENDIF
      ENDIF
      IF( fon(idisko) .EQ. 0 .AND. retrac(idisko) .GE. 0 ) THEN
          IF( no .NE. lastno(idisko) ) THEN
              itr = retrac(idisko)
          ELSE
              itr = lasttr(idisko) + 1
          ENDIF
          IF( lscr(7) .NE. 0 ) THEN
              lscr(7) = itr
          ELSE
              lscr(4) = itr
          ENDIF
          itrno(idisko) = itrno(idisko) + 1
      ENDIF
      IF( fon(idisko) .NE. 0 .AND. retrac(idisko) .EQ. -1 ) THEN
          IF( no .NE. lastno(idisko) ) THEN
              IF( lastno(idisko) .NE. -1 ) fon(idisko) = fon(idisko) + 1
          ENDIF
          IF( lscr(7) .NE. 0 ) THEN
              lscr(6) = fon(idisko)
          ELSE
              lscr(3) = fon(idisko)
          ENDIF
      ENDIF
!****   last needs to be the unmodified number
      lastno(idisko) = no
      lasttr(idisko) = itr
!      nsamps = ibuf(isampptr)                                           ! get the number of samples per trace
!      numsamps(idisko) = ibuf(isampptr)
!      IF( nsamps .EQ. 32767 ) nsamps = lbuf(isampptr)
!      IF( IAND(nsamps,32768) .NE. 0 ) nsamps = lbuf(29)
      istart = 0                                                        ! this is an additive to an index to the start of data
      si = buf(lsisptr)
      IF( osecs(idisko) .GT. 0. ) 
     &    nsamps = NINT( osecs(idisko)/si + 1. )
!****
!****  take care of the trace header first   (don't know why)
C****
      IF( decimf(idisko) .GT. 1 ) THEN
	  nsamps = nsamps / decimf(idisko)
	  iscr(isampptr) = nsamps
	  iscr(isiptr) = iscr(isiptr) * decimf(idisko)
	  scr(lsisptr) = scr(lsisptr) * decimf(idisko)
      ENDIF
      IF( set(2,idisko) .NE. 0 ) THEN
          iscr(108) = 0     ! set the delay scalar to 0
          delay = buf(46)
          iscr(idelmptr) = NINT(set(1,idisko) * 1000.)
          scr(ldelsptr) = set(1,idisko)
          nsamps = NINT( (set(2,idisko)-set(1,idisko)) / 
     &            (si*decimf(idisko))+1.)
          IF( nsamps .GT. maxsamps ) THEN
      PRINT *,' ***  ERROR  ***  SET causes too many samples per trace.'
      PRINT *,' Asking for ',nsamps,' samples - max is ',maxsamps
      PRINT *,' Change SET or decimate (resample) the data.'
      PRINT *,' SET = ',set(1,idisko),set(2,idisko),' sample interval=',
     &        (si*decimf(idisko))
              CALL EXIT
          ENDIF
      ENDIF
!****
!**** If nsamps is bigger than a signed 16 bit integer (16384) clobber
!**** the short integer before it (end mute time) by writing a long integer
!****
!      IF( nsamps .LT. 16384 ) THEN
!          iscr(isampptr) = nsamps
!      ELSE
!          lscr(29) = nsamps
!          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) THEN
!              itemp = iscr(57)
!              iscr(57) = iscr(58)
!              iscr(58) = itemp
!          ENDIF
!      ENDIF
!**** make the header for number of samples an unsigned short
      CALL long2ushort( nsamps, iscr(isampptr) )
      IF( numsamps(idisko) .NE. 0 .AND. numsamps(idisko) .NE. nsamps )
     &    fixed(i) = 0
      numsamps(idisko) = nsamps
      IF( micros(idisko) .NE. 0 .AND. micros(idisko) .NE. nsamps )
     &    fixed(i) = 0
      micros(idisko) = iscr(isiptr)
      IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) CALL swp_trhdr( iscr, lscr)
      IF( lunsort(idisko) .NE. 0 ) THEN
          CALL rddisc( lunsort(idisko), lscr, 2, istat )
          laddress = lscr(1)
!*****    undo the end of gather flag if the user used it.
          IF( iflag51 .NE. 0 ) lbuf(51) = 0
          CALL podisc( idunit(idisko), 1, laddress )
          mtraces(idisko) = mtraces(idisko) + 1
      ENDIF
      IF( iform(idisko) .NE. BINARY) 
     &    CALL wrdisc( idunit(idisko), scr, numhdr )                    ! write the trace header to disk
!****
!****   we did the header, now the data
!****   Do the decimation while moving it to the scratch buffer
!****
      j = 0
!****  nsamps is the number of output samples, numdat is what is in the input
      IF( iasgnd .EQ. 0 .OR. in .EQ. 0 ) THEN                           ! Is the data in the ap simulator?
          DO i = 1, numdat, decimf(idisko)                          ! move the data to scr
             j = j+1
  600        scr(j) = buf(numhdr+i)
          ENDDO
      ELSE
          DO i = 1, numdat, decimf(idisko)
             j = j+1
  610        scr(j) = a(in+i-1)
          ENDDO
      ENDIF
      ndone = numdat / decimf(idisko)
      si = si * decimf(idisko)
!****
!****  Now apply SET
      IF( set(2,idisko) .NE. 0 ) THEN
          delay = buf(46)                                               ! start with set(1) or delay
          istart = NINT( (set(1,idisko)-delay) / si)
          IF( delay .GT. set(2,idisko) ) THEN
              PRINT *,' ***  WARNING  ***  The delay of ',delay,
     &          ' is after the end SET time of ',set(2,idisko)
              ndone = 0
              istart = -nsamps
          ENDIF
          IF( delay .GT. set(1,idisko) .OR. istart .LE. 0 ) THEN
!****         shift the data down and insert zeros
              n = -istart
              DO i = ndone, 1, -1
                 scr(i+n) = scr(i)
              ENDDO
              DO i = 1, n
                 scr(i) = 0.
              ENDDO
              istart = 0
              ndone = ndone + n
              delay = set(1,idisko)
          ELSE
!****         recompute the delay because of possible roundoff errors
!****         newdelay = (number of samples of fill) + olddelay
              delay = (FLOAT(istart) * si) + delay
              IF( delay .GT. 16.384 ) THEN
                  PRINT *,' ***  ERROR  ***  Delay of ',delay*1000.
                  PRINT *,' exceeds max SEG-Y delay in mils of 16384.'
                  STOP
              ENDIF
              j = 1
              DO i = istart+1, ndone    !  the +1 thanks to ajh
                 scr(j) = scr(i)
                 j = j + 1
              ENDDO
              ndone = ndone - istart
          ENDIF
!****     Now do the back end zero pad
          IF( nsamps .GT. ndone ) THEN
              n = nsamps - ndone
              DO i = 1, n
                 scr(ndone+i) = 0.
              ENDDO
              ndone = ndone + n
          ENDIF
      ENDIF
      nbytes = nsamps * 4
      IF( ofmt(idisko) .GE. 6 ) GOTO 900                                ! reformat the data?
      IF( ofmt(idisko) .EQ. 1 ) THEN                                    ! is it IBM?
          IF( icompt .EQ. 1 ) CALL pr2ibm( scr, nsamps, scr )           ! PRIME?
          IF( icompt .EQ. 2 ) CALL ie2ibm( scr, nsamps, scr )           ! DecStation
          IF( icompt .EQ. 3 ) CALL ie2ibm( scr, nsamps, scr )           ! Apollo?
          IF( icompt .EQ. 4 ) CALL dr2ibm( scr, nsamps, scr )           ! Vax?
          IF( icompt .EQ. 5 ) CALL usscti( scr, scr, 1, nsamps, istat )
          IF( icompt .EQ. 6 ) CALL dr2ibm( scr, nsamps, scr )           ! Convex?
          IF( icompt .EQ. 7 ) CALL ie2ibm( scr, nsamps, scr )           ! IEEE (SUN, MASSCOMP)
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap32(scr,nsamps)
      ENDIF
      IF( ofmt(idisko) .EQ. 2 ) THEN                                    ! 32 bit integer?
          DO i = 1, nsamps
  620        lscr(i) = scr(i)
          ENDDO
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap32(scr,nsamps)
          IF( icompt .EQ. 5 ) CALL i82i4( iscr, iscr, nsamps )
      ENDIF
      IF( ofmt(idisko) .EQ. 3 ) THEN                                    ! 16 bit integer?
          nbytes = nsamps * 2
          DO i = 1, nsamps
  630        iscr(i) = scr(i)
          ENDDO
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap16(scr,nsamps)
          IF( icompt .EQ. 5 ) CALL i82i2( iscr, iscr, nsamps )
      ENDIF
      IF( ofmt(idisko) .EQ. 4 ) THEN                                    ! 16 bit fp
          nbytes = nsamps * 2
          CALL fp2sfp( scr, nsamps, scr )
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap16(scr,nsamps)
          IF( icompt .NE. 5 ) CALL i82i2( iscr, iscr, nsamps )
      ENDIF
      IF( ofmt(idisko) .EQ. 5 ) THEN                                    ! IEEE floating point
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4) CALL swap32(scr,nsamps)
      ENDIF
!****
!****     Write the damn thing to disk
!****
  900 CALL wrdiscb( idunit(idisko), scr, nbytes )
!****
!****    Always come here before returning. 
!****
 9000 CONTINUE
!****
!****   If appending a file used by another diskox previous in the procs list,
!****    sync up the disk addresses since they have different address pointers
!****
      IF( iappend(idisko) .NE. 0 ) THEN
          CALL filsiz( opath(idisko), laddress )
          CALL podiscb( idunit(iappend(idisko)), 1, laddress )
      ENDIF
      RETURN
!****
!****   ENTRY ENDDOX is needed so that all output files are closed
!****  properly and deleted or saved.  Part of the problem is that we
!****  may be stopped without a trace in the buffer yet we need to
!****  clean up!
!****
      ENTRY ENDDOX
!**** rewrite the binary header for SEG-Y REV 1 - Some things may have changed
!**** during processing
!**** There's only one copy of the binary header yet there may be
!**** several diskox.
      DO 9100 i = 1, maxdo
         IF( idunit(i) .GT. 0 .AND. iform(i) .NE. BINARY ) THEN
!****    use the binary header on ihunit rather that the original data
!             CALL podiscb( idunit(i), 0, 3200 )
!             CALL rddiscb( idunit(i), ibinhdr(1), 400, istat )
!             IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
!     *           CALL swap16( ibinhdr, 200 )
!**** the binary header on ihunit is in "native" byte order - not segy byte order
             CALL podiscb( ihunit, 0, 3200 )
             CALL rddiscb( ihunit, ibinhdr(1), 400, istat )
             IF( luntr0 .GT. 0 .AND. itrace0(i) .NE. 0 )
     &           ibinhdr(7) = ibinhdr(7) + 1
!****     (7) = number of data traces per ensemble.
!****     (8) = Number of aux channels
!****     (9) = sample interval in microseconds
!****     (11) = number of samples per trace
!****     (13) = format code
!****     (14) = ensemble fold
!****     (15) = trace sorting code (0=unknown, 1=shot, 2=cdp, 3=single fold continuous, 4=hor stacked, 5=)
!****     (31) = domain (e.g. 6 = depth domain)
!****     (151) = SEG-Y revision number.
!****     (152) = Fixed Length Trace Flag
!****     (153) = Number of 3200 byte Textual Header Extension Records
             IF( ontrcs(i) .NE. 0 ) ibinhdr(7) = ontrcs(i)
             ibinhdr(9) = micros(i)
             ibinhdr(11) = numsamps(i)
             ibinhdr(13) = ofmt(i)
C****   ***  bitch  *** setting the sort is problematic when multiple diskox and
!**** a process changes the sort (prestack and post stack disko).  random uses the sort.
!**** Read the trace header and set sort based on the rp trace number set or not
             CALL podiscb( idunit(i), 0, 3200+400+6*4)
             CALL rddisc( idunit(i), lrptrno, 1, istat )
             ibinhdr(15) = 1
             IF( lrptrno .GT. 0 ) ibinhdr(15) = 2
             ibinhdr(151) = 256
             IF( rev_out .EQ. 0 ) ibinhdr(151) = 0
             ibinhdr(152) = fixed(i)
             IF( IAND(lprint(i),4) .NE. 0) THEN
                 PRINT *, ' SEG-Y binary header:'
                 PRINT *, '      7-15: ', (ibinhdr(j),j=7,15)
                 PRINT *, '      151-153: ',(ibinhdr(j),j=151,153)
             ENDIF
             IF( icompt .EQ. 2 .OR. icompt .EQ. 4 )
     *           CALL swap16( ibinhdr, 200 )
             CALL podiscb( idunit(i), 1, 3200 )
             CALL wrdiscb( idunit(i), ibinhdr, 400 )
             CALL frefil( 2, idunit(i), istat )
             idunit(i) = 0
         ENDIF
 9100 CONTINUE
      RETURN
      END
