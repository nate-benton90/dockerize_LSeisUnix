      PROGRAM contro
!     ***  BEWARE ***  Unix can not tolerate undefined subroutines, thus some 
!  processes and subroutines are commented out so that they are not undefined.
!  In order to implement a new process, uncomment the edit and execution calls
!  and add the object files to the list in make.sioseis. 
!     THIS PROGRAM IS THE CONTROLLER FOR THE PRODUCTION MULTICHANNEL SEISMIC
!  REFLECTION PROCESSING SYSTEM.  THIS PROGRAM READS THE USER'S PARAMETERS AND
!  SEARCHES FOR A PROCESS NAME.  ONCE IT FINDS A PROCESS NAME, IT CALLS THE
!  APPROPRIATE PROCESS EDIT.  EACH EDIT RETURNS TO THIS PROGRAM AFTER IT READS
!  IT'S PARAMETERS WHICH ARE TERMINATED BY THE USER PARAMETER 'END'.
!  THE USER MUST TERMINATE HIS ENTIRE PARAMETER SET WITH 'END' ALSO.  THE USER
!  CONTROLS THE PROCESSING ORDER BY A PROCESS CALLED PROCS.  THE EDIT FOR PROCS
!  IS CALLED GETPRO, WHICH THEN BUILDS A TABLE OF PROCESS NUMBERS IN THE ORDER
!  THEY SHOULD BE CALLED .
!     THIS IS NICE AND SIMPLE AS LONG AS THE ORDER IS INPUT PROC1 PROC2 OUTPUT
!  WHERE PROC1 AND PROC2 ARE NOT MULTI-INPUT PROCESSES SUCH AS STACK OR GATHER.
!  GATHER IS ALSO A MULTI-OUT PROCESS.  MULTI-INPUT MEANS THAT MORE THAN ONE
!  TRACE MUST GO INTO IT BEFORE ONE COMES OUT AND ADDITIONAL PROCESSING CAN BE
!  DONE ON THAT ONE OUTPUT.  MULTI-OUTPUT MEANS THAT 1 TRACE WENT INTO THE
!  PROCESS, BUT SEVERAL COME OUT.
!     EACH PROCESS IS DEFINED IN A TABLE CONTAINING THE PROCESS NAME, THE NUMBER
!  OF CHARACTERS IN THE NAME, AND THE TYPE OF PROCESS. THERE ARE TWO TYPES
!  OF PROCESSES AT PRESENT, INPUT AND NON-INPUT PROCESSES.  AN INPUT PROCESS
!  INSERTS DATA INTO THE PROCESSING STREAM AND FILLS A CORE ARRAY WITH A TRACE
!  FROM SOME MASS STORAGE DEVICE.  A NON-INPUT PROCESS NEEDS A TRACE ALREADY IN
!  MEMORY.  INPUT PROCESSES ARE 1 IN THE TABLE 'NAMES' AND NON-INPUT ARE 0.
!     AFTER ALL PARAMETERS ARE READ AND EDITED, THIS PROGRAM WILL THEN EXECUTE
!  THE JOB BY CALLING THE APPROPRIATE EXECUTE SUBROUTINES IN THE ORDER NEEDED.
!  ONE HANG UP ALLUDED TO EARLIER IS THAT THE ORDER MAY NOT BE IDENTICAL BECAUSE
!  OF THE MULTI-INPUT AND MULTI-OUTPUT PROBLEM. EACH EXECUTE MUST SET THE NUMBER
!  OF TRACES READY IN ARRAY IOUT.  IN THIS MANNER, WHEN DATA IS NEEDED THIS
!  PROGRAM WILL BACKUP (IN THE ORDER OF PROCESSES) UNTIL IT SEES A PROCESS THAT
!  IS EITHER AN INPUT PROCESS OR THAT HAS DATA READY (SIGNALED THROUGH IOUT).
!
!  COPYRIGHTED BY:
!   PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, FEBRUARY 1980
!  mod 20 nov 90 by pch - add hidden paremeters noecho and echo
!  mod 12 Jan 91 by pch - fix weird gather bug when end occured before any
!           traces were output,
!  mod 21 Jan 91 by pch - add process pseudo
!  mod 11 April 91 by pch to add an argument to mixex
!  mod 3 July 92 by pch to add processes maxin and maxout
!  mod 12 Aug 92 by pch to add SADD and DESPIKE - increase name length to 7
!  mod 5 Nov 92 by pch to prevent infinite loop on HPUX when no processes.
!  mod 9 May 94 - more problems with gather and end of job.
!  mod 16 May 94 - problems with cat and end of job.
!  mod 3 May 95 - initialize ncdp to 0 (dropped first trace on SGI)
!  mod 5 July 95 - Add process ssmigr
!  mod 11 Jul 95 - reset idisko to 0 in case params given but not a proc
!  mod ? jul 95 - Dec Alpha needs Versatec plot header byte swapped
!  mod 5 Apr 96 - change txprestk from logical to integer counter
!  mod 15 May 96 - Add processes UADD and UMULT
!  mod 9 Sep 96 - Add tau-p prestack option
!  mod 19 Sept 96 - Need to set segdin/geom variable luntr0 to 0
!                 - Add arguments to geomex.
!  mod 10 Feb. 97 - Add process HISTORY
!  mod 11 Feb. 97 - open a file for rdline to write ALL parameter lines to.
!  mod 15 Apr. 97 - Make some of filters.f arrays COMMON.
!  mod 9 July 97 - Make TP2TX work as a multi-input/output
!  mod 22 Jul 97 - Add process NMO3
!  mod 27 sep 97 - Add stack common and preset it's lheader variable
!  mod 2 Apr 98 - despike is now multi-input
!  mod 24 Aug 98 - Add SEG2IN
!  mod 9 Oct 98 - Add XCORR
!  mod 1 Jul 99 - Add process STK and change stked to stacked
!  mod 12 Jul 99 - Allow 10 DISKOX (diskoe-diskoj)
!  mod Dec 99 - close Fortran SCRATCH files
!  mod Jul 00 - Changed number of args to gained
!  mod 11 Sept 00 - Add process TREDIT 
!  mod 28 Sept 00 - Add warning when a processes parameters are fiven
!                   more than once.
!  mod 5 Oct 00 - Add STATUS = 'DELETE' to try to get Solaris to delete
!                 Fortran's temporary file
!  mod 9 Jan 01 - Add process grdout
!  mod 1 June 01 - Call grdout one more time if istop = -1
!  mod 20 June 01 - Add processes WBT2 and WBT3
!  mod 20 June 01 - Add processes HEADER2 and HEADER3
!  mod 11 July 01 - Add process XSTAR
!  mod 14 Jan 02 - Add       COMMON /SEGY/ header(60)
!  mod 22 Jan 02 - Don't DELETE velan's output file!
!  mod 3 May 02 - Add DATA nxstar/0/
!  mod 15 Jul 02 - Add process SEGDDIN
!  mod 1 May 03 - Add COMMON /binhdr/ibinhdr(200)
!  mod 8 Aug 05 - Add REALTIME to the procs
!  mod 21 Sep 05 - Add istop to poutex
!                - Call poutex when istop = -1
!  mod 17 Apr 06 - Add is_big_endian and determine byte order on the fly
!  mod 25 Jul 06 - Change maxsam from 33000 to 50000
!  mod 7 Aug 06 - Add istop to geomex
!  mod 11 Oct 06 - Add istop to t2dex
!                - Call t2dex when istop = -1
!  mod 21 Dec 06 - Add nsevere and OVERRIDE
!  mod 8 Jan 07 - Add GAINS2 & GAINS3
!  mod 14 May 07 - Remove common xs - used by tp2tx, but not used here
!  mod 15 May 07 - Add args to tp2tex
!  mod 21 May 07 - Clean up prestack TX2TP 
!  mod 16 Jul 07 - Increase MAXSAM to 64536 from 50000
!                - Use all 3 trace buffers as scratch buffers for FKMIGR
!  mod 5 Dec 07 - Flush the print buffer before "END OF SIOSEIS RUN"
!  mod 19 Dec 07 - That fortran print comes out before the c bufffer is
!                  flushed, so just stop printing "END OF SIOSEIS RUN"
!  mod 2 Aug 08 - txed and fked must be initialized.
!  mod 19 Sep 08 - increase apmem from 5mw to 10 mw (megawords) for sort
!  mod 13 Apr 09 - redo some CAT logic
!  mod 5 Oct 09 - Add scr argument to wbtex.
!  mod ???? - Use -1 rather than istop when t2d_flag set.
!  mod 19 Jan 10 - nswell needs to be set to 0
!                - swell wasn't terminating correctly
!  mod 10 Apr 12 - comment out pseudo
!  mod 31 Mar 14 - add , delay, segyrev, si to common /readt/
!  mod 14 Apr 14 - Add dbuf cbuf to CALL HEADEX
!  mod 19 Jul 18 - nmoed needs ibuf to set binary header
!  mod 20 Mar 20 - remove tape input/output (processes input, output, segdin)
!                - change comments in column 1 from c to !
!                - change DO CONTINUE to DO ENDDO
!  mod 9 Apr 20  - remove GRDOUT and HIST and IRIS
!  mod 6 Jun 20 - Change MAXLEN fir filters to be 32K
!  mod 20 Jun 20 - COMMON /pltcom/ sizes were bad bad bad - see trplot.f
!                - removed COMMON /pltcom/ since trplot.f was the only users
!  mod ??   - remove xstar
!
      PARAMETER (NAMESS=100)                                            ! THE NUMBER OF LEGAL PROCESS NAMES
      PARAMETER (MAXSAM=65536)                                          ! THE MAXIMUM TRACE LENGTH INCLUDING HEADER
      PARAMETER (MAXS3=MAXSAM*3)                                        ! THE AMOUNT OF MEMORY NEEDED
!  Process INPUT needs 2 buffers (for double buffering) and we need 1
!  for a scratch array.
      DIMENSION BUF(maxs3)
      PARAMETER (MAXWBTS=3)
!
      COMMON /PORDER/ NUM,IORDER(NAMESS)                                ! BUILT BY GETPRO - holds the processing order indices
      COMMON /pnames/ pnames(namess)                                    ! holds the process names in the order of PROCS
      CHARACTER*7 pnames
      COMMON /REQUIR/ IOK(NAMESS)                                       ! 1 MEANS THE PARAMS FOR THAT PROC ARE REQUIRED
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT,isite, maxsamps,
     & nbperw, ireal, nsevere, override
! ierror - counts the errors in the edit phase
! iwarn counts the warnings in the edit phase
! irun = 0 for edit only jobs
!      = 1 for edit and execute
! now = 0 for batch or backgrount jobs
!     = 1 for interactive jobs
! icompt = 1 for PRIME
!        = 2 for Ultrix DecStation (IEEE words in Dec byte order)
!        = 2 for PC
!        = 3 for APOLLO
!        = 4 for VAX VMS
!        = 5 for Cray CTSS
!        = 6 for Convex Unix
!        = 7 for Sun and Masscomp (IEEE little endian)
! nbperw - number of bytes per word
! ireal = 1 means REAL TIME (diskin waits rather than stops &
!         plot closes it's files after every trace).
! nsevere = The number of severe warnings, which are like errors, but the
!         can be overridden with override.
      COMMON /sunras/ lunras,iwrdcount,ilincount, irashead(8), iptype
      COMMON /imaget/ maxx, maxy, lunimg
      COMMON /SEISPL/ OFFSET,DEFLEC,PCTFI,VSCAL,TRCPIN,JTYPE,TLINE(4),
     *       STIMEL,TIMEIL,IUNPLT,WIGGL,BIASS,LANN,ICDOT,iplt_dummy(9)
      COMMON /TCOL/ LSTRP,LRPINC,NGWRDS,IGATUN,MAXRPS,MAXTRS,MINTRS
      COMMON /READT/ILUN,NUMTRHDR,NUMDAT,IUNHDR,IREELM,INTRCS,IFMT,
     *   NSKIP,SECS,LRENUM,ISRCF,IDTYPE, nfskip, jform, itxsi,itxdel,
     &   nfktrc, norigtr, nrskip, nfiles, irewind, delay, segyrev, si
      COMMON /WRITET/ounit,NSAMPS,OREEL,POSAFT,OFMT,NTRCS,LFOR,ONUMTR,
     &       nfskipo, rewindo, newfile, itrace0, ounit2
      INTEGER OUNIT,POSAFT,OFMT
      COMMON /VELAN/ IVUNIT,NLISTS,IVELU1,IVELU2, ivelu3
      COMMON /GEOM/ IGUNIT, nglists
      COMMON /INPUT/ IPARUN, input_dummy
      COMMON /PLOT/ MUNIT, plot_dummy(8), lunhead, nraster
      COMMON /SIOAP/ IASGND,IRELSE,IN,JOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP,
     *     IDECIM, mdsize
      CHARACTER*200 cbuf
      COMMON /sioln1/ cbuf
      COMMON /sioln2/ ichar, mchars, iprint, lunpo
      COMMON /TAPES/ NMILS,NTRIES,NPAR,NWREAD
      COMMON /tx2tp/ sshift, sep(2), nnp, setau(2), beta, ffc, ppcnti, 
     *               ppcnto, iirev, ffon, dummy, iimft, set(2), lprt, 
     *               lunhdr, tpprestk, ntx2tp
      INTEGER tpprestk
!      COMMON /xs/xmin,xmax,ntp2tx,dx,xdumb(700)                         ! used by tp2tx
      logical txinit, fkinit, txed, fked, tmptxhdr
      integer ltxunt1, ltxunt2, lfkunt1, lfkunt2, txunit, ohdrtxfk, 
     &        txprestk
      common /TXFKE/ txinit, fkinit, txed, fked, txunit, ltxunt1,
     &               ltxunt2, lfkunt1, lfkunt2, ohdrtxfk, tmptxhdr,
     &               txprestk, ntx2fk, nfk2tx, range, idummyfk
      COMMON /fkfilt/ munitfk, nlistsfk, ktrace
      COMMON /fkmigr/ fktemp,ifktemp(2),fktemp1(2), realk
      COMMON /fkshift/ fktemp2,ifktemp1(2),fktemp3(2), realk1,fktemp4(2)
      COMMON /segdin/ junit, nlists1, nwrds, luntr0, luntr0a, lunldeo
      COMMON /stack/ lheader
      PARAMETER( MAXLEN = 32767*2 )                                     ! they are complex in subroutine filters!
      COMMON /filtersc/ filt1(MAXLEN), refw(MAXLEN),
     &    filtl(MAXLEN), filth(MAXLEN)
      COMPLEX filt1, refw, filtl, filth
      COMMON /f2t/ osi, lprintfk, iofmt
      COMMON /SEGY/ header(60)
      COMMON /binhdr/ ibinhdr(200)
      INTEGER*2 ibinhdr, ipbuf1, ipbuf2, ipbuf3
!****  not sure why the following is common.  Only use in trplot.f  
!      COMMON /pltcom/ ipbuf1(1650,600), ipbuf2(1650,600), 
!     &       ipbuf3(1650,600)
!
      DIMENSION IBUF(111),LBUF(111)
      DIMENSION LENGTH(NAMESS),ITYPE(NAMESS)
      CHARACTER*7 NAMES(NAMESS)
      EQUIVALENCE (BUF(1),IBUF(1)),(BUF(1),LBUF(1))
      REAL temp
      INTEGER ltemp
      EQUIVALENCE( temp,ltemp )
      DIMENSION IOUT(NAMESS)
      CHARACTER*100 CTOKEN                                              ! ALLOW A TOKEN OF 50 CHARACTERS
      LOGICAL FIRST1, t2d_flag
      INTEGER idowbt(MAXWBTS), iwbtyp(MAXWBTS)
!******************************************************************************
!
! Define a common block which is used to simulates the AP120-B data memory 
! which has the size of 32K 32-bit floating point words.
!  Systems that have an ap should set iapsiz to the amount of ap
!  main data memory actually on the ap.
      PARAMETER (iapsiz = 10000000)
!
        REAL    APDATA(0:iapsiz)
        INTEGER IAPDATA(0:iapsiz)
        COMMON /apmem/ apdata                                           ! Veritas called this ap120bmd
        EQUIVALENCE (APDATA,IAPDATA)
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        INTEGER APSP(0:15)
        COMMON/AP120BSP/APSP
!
!******************************************************************************
      PARAMETER (itsize = 262144 )                                      ! 512x512
      COMMON /transp/ t(itsize)
!   FDMIGR and TX2TP now use transp, so look out it you change the size.
!    check tx2tex.f
!
!
      DATA NAMES/'PROCS ','INPUT ','OUTPUT','GEOM  ','GATHER',
     2           'WBT   ','MUTE  ','NMO   ','STACK ','AVENOR',
     3           'FILTER','DECON ','SYN   ','ACORR ','PROUT ',
     4           'WEIGHT','SHIFT ','TREDIT','AGC   ','MIX   ',
     5           'VELAN ','FLATTEN','DISKIN','DEBIAS','SMUTE ',
     6           'COMPEN','SPECTR','UFILTR','T2D   ','PLOT  ',
     7           'TX2FK ','FK2TX ','FKMIGR','T2F   ','F2T   ',
     8           'REALTM','UDECON','FLATEN','FFILTR','FKFILT',
     9           'SEGDIN','HEADER','DISKOA','DISKOB','DISKOC',
     &           'DISKOD','FDMIGR','TX2TP ','TP2TX ','IRIS  ',
     1           'GAINS ','FDFMOD','PSEUDO','SORT  ','DMO   ',
     2           'LOGST1','LOGST2','RESAMP','MAXIN ','MAXOUT',
     3           'DESPIKE','SADD ','CAT   ','FKSHIFT','NMO2 ',
     4           'PSMIGR','UADD  ','UMULT ','HISTORY','NMO3  ',
     5           'COFILT','SEG2IN','XCORR ','STK   ','DISKOE',
     6           'DISKOF','DISKOG','DISKOH','DISKOI','DISKOJ',
     7           'GRDOUT','WBT2  ','WBT3  ','HEADER2','HEADER3',
     8           'XSTAR ','SEGDDIN','GAINS2','GAINS3','SWELL ',
     8     10*' '/
      DATA LENGTH/5,5,6,4,6,3,4,3,5,6,6,5,3,5,5,6,5,6,3,3,5,5,6,6,
     *  5,6,6,6,3,4,5,5,6,3,3,12*6,2*5,4,5,6,6,4,3,6,6,6,5,6,7,4,3,7,4,
     *  6,4,5,7,4,6,6,5,3,7*6,2*4,2*7,5,7,2*6,5,10*0/
      DATA FIRST1/.TRUE./, nready/0/, nrdy/0/, ibackup/0/, num/0/
      DATA istop/0/, jstop/0/, kstop/0/, idisko/0/, t2d_flag/.FALSE./
      DATA iuseap/0/, iasgnd/0/, in/0/, nextad/0/
      DATA lunhdr/0/, ncdp/0/, ntx2fk/0/, nfk2tx/0/, igunit/0/,iofmt/1/,
     &     ngathers/0/, tpprestk/0/, txprestk/0/, iparun/0/
     &     ivelu4/0/, nxstar/0/, ntx2tp/0/, ntp2tx/0/, nswell/0/
      DATA txed/.FALSE./, fked/.FALSE./
!****
!****    Set the site parameters (set the computer and ap switches). Ploted and
!**** inap may also have to be changed from installation to installation.
!****
      CALL version
      numtrhdr = 60                                                     ! set the SEGY header length
      isite = 9
      mdsize = iapsiz
!****  no array processors any more, so set it to none
      iuseap = 0
!****  check for endianness.  Little endian (Intel byte order) is 2 or 4
      icompt = 7
      IF( is_big_endian() .LT. 0 ) icompt = 2
      nbperw = 4
      IF( icompt .EQ. 5 ) nbperw = 8
      CALL setptr                                                       ! set the SEGY trace header pointers
!****
!****    INITIALIZE ALL ON/OFF SWITCHES
!****
      ISPLOT=0
      IPLOT=0
      OUNIT=0
      IINRW=0
      IOUTRW=0
      iwarn=0
      ierror=0
      nsevere = 0
      override = 0.
      lprint=0
      iprint=1                                                          ! tell rdline to print
      now=0
      in=0
      idecim=1
      ntrcs=0
      luntr0 = 0
      luntr0a = 0
      lunldeo = 0
      lheader = 1
      lunhead = 0
!**** iok = 1 means the process is in the procs list and parameters
!****         are required.
      DO i=1,namess
         iok(i)=0
         iout(i)=0
         itype(i) = 0
      ENDDO
      CALL txfkinit                                                     ! call only once for any fk proc in list
!****
!****  Always get a temporary file so the parameter file may be saved for process history.
!****
      CALL getfil( 2, lunpo, ctoken, istat )
      OPEN( UNIT=lunpo, STATUS='SCRATCH', FORM='FORMATTED' )
!****
!****  INPUT PROCESSES MUST HAVE A SIGNAL SET TO 1 IN THE ARRAY ITYPE
!****
      ITYPE(2)=1                                                        ! PROCESS INPUT
      ITYPE(5)=1                                                        ! GATHER
      ITYPE(13)=1                                                       ! PROCESS SYN
      ITYPE(21)=1                                                       ! PROCESS VELAN
      ITYPE(23)=1                                                       ! PROCESS DISKIN
      ITYPE(31)=1                                                       ! PROCESS TX2FK
      ITYPE(32)=1                                                       ! PROCESS FK2TX
      itype(36)=1                                                       ! process REALTM
      itype(41) = 1                                                     ! SEG-D input
      itype(47) = 1                                                     ! FDMIGR
      itype(48) = 1                                                     ! tx2tp
      itype(49) = 1                                                     ! tp2tx
      itype(52) = 1                                                     ! fddiff
      itype(59) = 1                                                     ! ProMax input
      itype(63) = 1                                                     ! CAT
      itype(66) = 1                                                     ! ssmigr
      itype(71) = 1                                                     ! cofilt
      itype(72) = 1                                                     ! seg2in
      itype(86) = 1                                                     ! XSTAR
      itype(87) = 1                                                     ! SEGDDIN
      itype(90) = 1                                                     ! swell
!****
!****  SET THE JOB TO AUTOMATICALLY RUN (UNLESS ERRORS).  USE 'PROCESS' EDIT
!****  TO EDIT THE JOB WITHOUT EXECUTING IT (LIKE HANGING TAPES!)
!****
      IRUN=1
!****
!****     SET UP THE INDEXES FOR THE ARRAY BUF
!****
      IOK(1)=1                                                          !  REQUIRE PROCESS PROCS
      maxsamps = maxsam                                                 ! put the maximum trace length into common
      INDEX1=1
      INDEX2=INDEX1+MAXSAM                                              ! MAXSAM IS THE LARGEST TRACE THAT CAN BE PROCESSED
      INDEX3=INDEX2+MAXSAM                                              !  THIS STARTS A SCRATCH ARRAY MAXSAM LONG
      IRELSE=1                                                          ! DON'T RELEASE THE AP - BE AN AP HOG!!!!
      JOUT=1
      IF( NOW .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'
      CALL RDLINE                                                       ! READ A LINE INTO THE HIDDEN COMMON BUFFER Q$LINE
!****
!****       GET A PARAMETER FROM THE USER.  IT MUST BE A PROCESS NAME
!****
    5 NTOKES=0                                                          !  A TOKEN IS AN ITEM ON THE CARD IMAGE THAT IS SEPARATED BY BLANKS
   10 CONTINUE
      CALL GETOKE(CTOKEN,NCHARS)                                        ! GET A TOKEN INTO CTOKEN
      IF( NCHARS .EQ. 0 ) THEN                                          ! WAS IT THE END OF A LINE?
          IF( NOW .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'  
          CALL RDLINE                                                   ! READ A LINE INTO THE HIDDEN COMMON BUFFER Q$LINE
          GOTO 5                                                        ! START THE LINE AT THE BEGINNING 
      ENDIF
!****
!****     GOT A TOKEN  -  IS IT A PROCESS NAME
!****
   30 CONTINUE
      NTOKES=NTOKES+1
      CALL UPCASE(CTOKEN,NCHARS)                                        ! CONVERT TO UPPERCASE
      DO I=1,NAMESS
         NAMNUM=I                                                          ! THE NUMBER OF THE PROCESS IN THE NAMES TABLE
         ITEMP=LENGTH(I)                                                   ! THE LEGAL PROCESS NAME IS LENGTH(I) CHARACTERS LONG
         IF( itemp .EQ. 0 ) GOTO 60
         IF(NAMES(I) (1:ITEMP) .EQ.CTOKEN(1:NCHARS)) GO TO 100             ! SEARCH THE NAMES TABLE FOR THE PROCESS NAME
      ENDDO
   60 CONTINUE
      IF( CTOKEN(1:3) .EQ. 'END' ) THEN                                 ! CHECK FOR END OF JOB
          IF( num .EQ. 0 ) THEN
              PRINT *,' ***  ERROR  ***  Nothing to do.'
              ierror = ierror + 1
              num = 1
          ENDIF
          GOTO 1000
      ENDIF
      IF( CTOKEN(1:5) .EQ. 'DEBUG' ) THEN                               ! CHECK FOR 'DEBUG'
          LPRINT = 1
          GOTO 10
      ENDIF
      IF( ctoken(1:nchars) .EQ. 'NOECHO' ) THEN
          iprint = 0
          GOTO 10
      ENDIF
      IF( ctoken(1:nchars) .EQ. 'ECHO' ) THEN
          iprint = 1
          GOTO 10
      ENDIF
      IF( CTOKEN(1:4) .EQ. 'APON' ) THEN                                ! CHECK FOR 'APON'
        IUSEAP = 1                                                      ! USE THE AP
        GOTO 10
      ENDIF
      IF( CTOKEN(1:5) .EQ. 'APOFF' ) THEN                               ! WAS IT 'APOFF'?
          IUSEAP = 0                                                    ! DON'T USE THE AP
          GOTO 10
      ENDIF
      IF( CTOKEN(1:4) .EQ. 'EDIT' ) THEN
          IRUN = 0                                                      ! THIS IS AN EDIT ONLY JOB
          GOTO 10
      ENDIF
      IF( CTOKEN(1:4) .EQ. 'EXEC' ) THEN
          IRUN = 1                                                      ! SET THE JOB TO EXECUTE
          GOTO 10
      ENDIF
      IF( ctoken(1:nchars) .EQ. 'REALTIME' ) THEN
          ireal = 1
          GOTO 10
      ENDIF
      IF( ctoken(1:nchars) .EQ. 'OVERRIDE' ) THEN
          override = 1.
          GOTO 10
      ENDIF
!****
!****     GOT A PROCESS NAME  -  NOW CALL THAT PROCESS' EDIT
!****
  100 CONTINUE
      IF( lprint .EQ. 1 ) PRINT *,' Debug info:  Computer type =',
     &    icompt
!****  MAKE SURE THAT THE NAME IS THE RIGHT LENGTH.  THIS MAKES SURE THAT
!****  THE IS A DELIMITER BETWEEN THE PROCESSES.  WITHOUT THIS CHECK, A
!****  PROCESS COULD BE SKIPPED IF THE DELIMITED WAS OMMITTED. (I.E.
!****  PROCS INPUT FILTER OUTPUT END WOULD RUN WITHOUT DOING THE FILTER!
      IF( NCHARS .NE. LENGTH(NAMNUM) ) THEN
          PRINT *, ' ***  ERROR  ***  Process name ',CTOKEN(1:NCHARS),
     *         ' is incorrect.'
          IERROR=IERROR+1
          GOTO 10
      ENDIF
      IF( iok(namnum) .EQ. -1 ) THEN
          PRINT *,' ***  WARNING  *** These process ',names(namnum),
     &             ' parameters supercede the previous ones.'
          nsevere = nsevere + 1
      ENDIF
      IOK(NAMNUM) = -1                                                  !  THE PROCESS' PARAMETERS ARE GIVEN!
      IF( lprint .NE. 0 ) THEN
          PRINT *,' about to enter edit of ',names(namnum)
      ENDIF
      GOTO (110,120,130,140,150,160,170,180,190,200,210,220,230,240,
     *   250,260,270,280,290,300,310,480,330,10,350,360,370,380,390,
     *  400,410,420,430,440,450,460,470,480,490,500,510,520,531,532,533,
     *   534, 570, 580, 590, 10, 610, 620, 630, 640, 650, 660, 670, 680,
     *   690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800,
     *   810, 820, 830, 840, 535, 536, 537, 538, 539, 540, 850, 162,
     *   163, 522, 523, 860, 870, 612, 613, 880 ), NAMNUM
  110 CALL GETPRO(NAMES,LENGTH,ITYPE)                                   ! GET THE PROCESSING ORDER 
      GO TO 10
  120 CONTINUE
!      CALL INEDIT(BUF,BUF,BUF,IBUF(INDEX3))                             ! THIS ALSO READS THE FIRST TRACE INTO BUF
      GO TO 10
  130 CONTINUE
!      CALL TOUTED                                                       ! TAPE OUTPUT
      GO TO 10
  140 CONTINUE
      CALL GEOMED(BUF(INDEX3),BUF(INDEX3))
      GO TO 10
  150 CONTINUE
      CALL GATHED(BUF(INDEX3),BUF(INDEX3),BUF(INDEX3))                  ! GATHER EDIT
      GO TO 10
  160 numwbt = 1
      GOTO 169
  162 numwbt = 2
      GOTO 169
  163 numwbt = 3
  169 CALL wbted( numwbt )
      GO TO 10
  170 CONTINUE
      CALL MUTEED(BUF(INDEX3),BUF(INDEX3))                              !  MUTE EDIT
      GO TO 10
  180 CONTINUE
      nmonum = 1
  185 CALL NMOED( BUF(INDEX3), BUF(INDEX3), BUF(INDEX3), nmonum )       ! NMO EDIT
      GO TO 10
  190 CONTINUE
      CALL stacked
      GOTO 10
  200 CONTINUE
      CALL AVENED(BUF(INDEX3),BUF(INDEX3))                              !  AVENOR EDIT
      GO TO 10
  210 CONTINUE
      CALL FILTED(BUF(INDEX3),BUF(INDEX3),BUF)                          ! FILTER EDIT
      GO TO 10
  220 CONTINUE
      CALL SDECED(BUF(INDEX3),BUF(INDEX3))                              !  SDECON EDIT
      GO TO 10
  230 CONTINUE
      CALL SYNED(BUF,BUF(INDEX3),BUF(INDEX3),BUF(INDEX3))               ! SYN EDIT
      GO TO 10
  240 CONTINUE
      CALL ACORED(BUF(INDEX3),BUF(INDEX3))                              ! ACORR EDIT
      GO TO 10
  250 CONTINUE
      CALL POUTED(BUF(INDEX3),BUF(INDEX3))                              ! PRINTER OUTPUT EDIT
      GO TO 10
  260 CONTINUE
      CALL WEIGED(BUF(INDEX3),BUF(INDEX3))                              !  WEIGHT EDIT
      GO TO 10
  270 CONTINUE
      CALL SHFTED(BUF(INDEX3),BUF(INDEX3))                              !  SHIFT EDIT
      GO TO 10
  280 CALL tredited( BUF(index1), BUF(index1), BUF(index1) )
      GOTO 10
  290 CONTINUE
      CALL AGCED(BUF(INDEX3),BUF(INDEX3))                             
      GO TO 10
  300 CONTINUE
      CALL MIXED(BUF(INDEX3),BUF(INDEX3))                               ! MIX EDIT
      GO TO 10
  310 CONTINUE
      CALL VELAED(BUF(INDEX3),BUF(INDEX3),BUF(INDEX3))                  !  VELAN EDIT
      ifin = 0
      GO TO 10
  330 CONTINUE
      CALL DIED(BUF(index1),BUF(index1),BUF(index1),
     *     BUF(INDEX3),BUF(INDEX3),buf(index3))    
      GO TO 10
  350 CONTINUE
      CALL SMUTED(BUF(INDEX3),BUF(INDEX3))                              !  SMUT EDIT
      GO TO 10
  360 CONTINUE
!       CALL comped( buf(index3), buf(index3) )
      GOTO 10
  370 CONTINUE
!  370 CALL SPECED(BUF(INDEX3),BUF(INDEX3))                             !  SPECTR EDIT
      GO TO 10
  380 CONTINUE
      CALL UFILED(BUF(INDEX3),BUF(INDEX3))                              !  UFILTR EDIT
      GO TO 10
  390 CALL T2DED(BUF(INDEX3),BUF(INDEX3))
      GO TO 10
  400 CONTINUE
      CALL PLOTED(BUF,BUF,BUF,BUF(INDEX3),BUF(INDEX3),BUF(INDEX3))      !  PLOT EDIT
      GO TO 10
  410 CONTINUE
      CALL TX2FED
      GO TO 10
  420 CONTINUE
      CALL fk2ted( buf(index3), buf(index3) )
      GO TO 10
  430 CONTINUE
      CALL FKMIED
      GO TO 10
  440 CONTINUE
      CALL T2FED                                                        ! T2F EDIT
      GO TO 10
  450 CONTINUE
      CALL f2ted
      GOTO 10
  460 CONTINUE
!      CALL realed(buf,buf,buf,buf(index3),buf(index3),buf(index3))
      GOTO 10
  470 CONTINUE
      CALL udeced( buf(index3), buf(index3) )
      GOTO 10
  480 CONTINUE
      CALL flated( buf(index3), buf(index3) )
      GOTO 10
  490 CONTINUE
!      CALL ffiled
      GOTO 10
  500 CONTINUE
      CALL fkfied( buf(index3),buf(index3))
      GOTO 10
  510 CONTINUE
!      CALL segded( buf, buf, buf, buf(index3) )                         ! leaves a trace in buf!
      GOTO 10
  520 numhdrp = 1
      GOTO 525
  522 numhdrp = 2
      GOTO 525
  523 numhdrp = 3
  525 CALL headed(numhdrp)
      GOTO 10
  531 idisko = 1
      GOTO 565
  532 idisko = 2
      GOTO 565
  533 idisko = 3
      GOTO 565
  534 idisko = 4
      GOTO 565
  535 idisko = 5
      GOTO 565
  536 idisko = 6
      GOTO 565
  537 idisko = 7
      GOTO 565
  538 idisko = 8
      GOTO 565
  539 idisko = 9
      GOTO 565
  540 idisko = 10
  565 CALL doed(idisko,buf(index1),buf(index1),buf(index1),
     *    buf(index3),buf(index3),buf(index3))
      GOTO 10
  570 CALL fdmied
      GOTO 10
  580 CALL tx2ted
      GOTO 10
  590 CALL tp2ted
      GOTO 10
  610 num_gains = 1
  611 CALL gained( buf(index3), num_gains )
      GOTO 10
  612 num_gains = 2
      GOTO 611
  613 num_gains = 3
      GOTO 611
  620 CALL fdfmed
      GOTO 10
  630 CONTINUE
!      CALL pseued
      GOTO 10
  640 CALL sorted
      GOTO 10
  650 CALL dmoed
      GOTO 10
  660 lognum = 1
  661 CALL logsed( lognum )
      GOTO 10
  670 lognum = 2
      GOTO 661
  680 CALL resaed
      GOTO 10
  690 continue
!      CALL maxied( BUF(index1), BUF(index1), BUF(index1) )
      GO TO 10
  700 continue
!      CALL maxoed( BUF(index1), BUF(index1), BUF(index1) )
      GO TO 10
  710 CALL despiked( BUF(index1), BUF(index1), BUF(index1) )
      GOTO 10
  720 CALL sadded
      GOTO 10
  730 CALL cated
      GOTO 10
  740 CALL fkshed
      GOTO 10
  750 nmonum = 2
      GOTO 185
  760 CALL ssmied
      GOTO 10
  770 CALL uadded(BUF(INDEX3),BUF(INDEX3))
      GO TO 10
  780 CALL umulted(BUF(INDEX3),BUF(INDEX3))
      GO TO 10
  790 CONTINUE
!  790 CALL histed
      GO TO 10
  800 nmonum = 3
      GOTO 185
  810 CALL cfiled( BUF(index3), BUF(index3) )
      GO TO 10
  820 CALL seg2ed( buf(index1), buf(index1), buf(index1) )
      GO TO 10
  830 CALL xcored( buf(index1), buf(index1), buf(index1),
     &             buf(index3), buf(index3), buf(index3) )
      GO TO 10
  840 CALL stked( buf(index3), buf(index3) )
      GO TO 10
  850 CONTINUE
!  850 CALL grdouted
      GO TO 10
  860 CONTINUE
!  860 CALL xstared
      GO TO 10
  870 CONTINUE
      CALL segdded( buf, buf, buf, buf(index3) )
      GOTO 10
  880 CONTINUE
      CALL swelled( buf(index3), buf(index3) )
      GOTO 10

!****
!****
!****  EXECUTE THE JOB - PROCESS SOME DATA        ********************
!****
!****
 1000 CONTINUE
      idisko = 0
      DO I=1,NUM
         ITEMP=IORDER(I)                                                ! POINT TO THE PROCESS
         IF( IOK(ITEMP) .GT. 0 ) THEN
             PRINT *,' ***  ERROR  ***  ',names(itemp),
     &               ' parameters are required.'
             IERROR=IERROR+1
         ENDIF
      ENDDO
      PRINT 1005,IERROR
 1005 FORMAT(' ****  ',I3,' ERRORS IN THIS JOB   ****')
      IF( iwarn .NE. 0 ) 
     &    PRINT *,' **** ',iwarn,' WARNINGS IN THIS JOB  ****'
      IF( nsevere .NE. 0 .AND. override .EQ. 0. ) THEN
          PRINT *,' **** ',nsevere,' SEVERE WARNINGS.  ****'
          PRINT *,' Use parameter OVERRIDE or correct the problem causin
     &g the warning.'
          ierror = ierror + 1
      ENDIF
      IF(IRUN.EQ.0 .OR. (ierror .NE. 0 .AND. now .EQ. 0) ) GOTO 1100
      I=0                                                               ! INDEX TO THE IORDER ARRAY
 1010 CONTINUE
      I=I+1
      numpro = IORDER(I)                                                ! FIND THE PROCESS CORRESPONDING TO THE I TH IN ORDER
      IF(I.GT.NUM.AND.ISTOP.NE.0) GO TO 1030                            ! LAST PROCESS AFTER THE LAST INPUT?
      IF(I.LE.NUM) GO TO 1020
!****
!****     BACKUP IN THE PROCESSING ORDER UNTIL AN INPUT TYPE OF PROCESS IS FOUND
!****
 1015 I=I-1
      numpro = IORDER(I)
      IF( ITYPE(numpro) .EQ. 1 ) THEN                                   ! IS IT AN INPUT PROCESS?
          IF( i .LE. 2 .AND. istop .NE. 0 ) GOTO 1030                   ! sort and diskin have trouble stopping
          GOTO 1020
      ENDIF
      IF(I.EQ.1)  GO TO 1020                                            ! THIS SHOULDN'T REALLY HAPPEN
      GO TO 1015                                                        ! KEEP LOOKING
 1020 CONTINUE
      IF( LPRINT .EQ. 1 ) PRINT *,' ABOUT TO ENTER ', NAMES(numpro),
     &    ' istop=',istop
      GO TO (1010,1200,1300,1400,1500,1600,1700,1800,1900,2000,2100,
     *   2200,2300,2400,2500,2600,2700,2800,2900,3000,3100,4800,3300,
     *  3400,3500,3600,3700,3800,3900,4000,4100,4200,4300,4400,4500,
     *  4600,4700,4800,4900,5000,1200,5200,5301,5302,5303,5304,5700,
     *  5800,5900,6000,6100,6200,6300,6400,6500,6600,6700,6800,6900,
     *  7000, 7100, 7200, 7300, 7400, 7500, 7600, 7700, 7800, 7900,8000,
     *  8100, 8200, 8300, 8400, 5305, 5306,5307,5308,5309,5310,
     &  8500, 1602, 1603, 5202, 5203, 8600, 8700, 6120, 6130, 8800
     &   ), numpro
 1030 CONTINUE                                                          !  WE'RE GOING TO FINISH FINISHING
!      IF(IASGND.NE.0.AND.IUSEAP.NE.0) CALL APRLSE                      !  RELEASE THE AP IF IT IS ASSIGNED
!      IF( IINRW.EQ.1.AND.ILUN.GE.0) CALL reltap( ilun, 1 )
!      IF( IOUTRW .EQ. 1 .AND. ounit .GE. 0 ) CALL reltap( ounit, 2 )
! THE OPERATOR CAN TERMINATE WITH -1 UNIT NUMBER WHICH ALSO MEANS THAT THE DRIVE IS UNASSIGNED!
!      IF( isplot .NE. 0 ) THEN
!           CALL PLOTEX( BUF(INDEX1), BUF(INDEX1), BUF(INDEX1),
!     *             BUF(INDEX3), BUF(INDEX3), BUF(INDEX3), -1 )
!      ENDIF
      IF( iunplt+lunras .NE. 0 .AND. isplot .NE. 0 ) 
     &    CALL TRPLOT(buf(index3),.001,0,1,1,1)                         ! FLUSH THE SEISMIC PLOT FILE
      IF( lunras .NE. 0 ) THEN
          CALL podisc( lunras, 1, 0 )
          irashead(1)=1504078485                                        ! in hex = 59a66a95
          irashead(2)=iwrdcount*32                                      ! width
          irashead(3)=ilincount                                         ! height
          irashead(4)=1                                                 ! depth
          irashead(5)=irashead(2)*irashead(3)*irashead(4)/8             ! length of image in bytes
          irashead(6)=1                   
          irashead(7)=0
          irashead(8)=0
          PRINT *,'Image size is ', irashead(2), ' by ', irashead(3)
          IF( icompt .EQ. 2 .OR. icompt .EQ. 4 ) CALL swap32(irashead,8)
          CALL wrdisc( lunras, irashead, 8 )
          CALL frefil( 2, lunras, istat )
      ENDIF
      IF( lunhead .NE. 0 ) CALL frefil( 2, lunhead, istat )
      IF( lunimg .NE. 0 .AND. isplot .NE. 0 ) THEN
         PRINT *, ' image is ', maxx, ' (traces) by ', maxy,' (samples)'
         CALL frefil( 2, lunimg, istat )                                ! close the output file
      ENDIF
      IF( idisko .NE. 0 ) CALL enddox                                   ! take care of diskox files
      IF( t2d_flag ) CALL T2DEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),
     &   BUF(INDEX3), BUF(INDEX3),BUF(INDEX3), -1 )
!**** now close Fortran files that were opened SCRATCH
 1100 IF( igunit .NE. 0 ) CLOSE( igunit, STATUS = 'DELETE' )
      IF( iparun .NE. 0 ) CLOSE( iparun, STATUS = 'DELETE' )
      IF( lunpo .NE. 0 ) CLOSE( lunpo, STATUS = 'DELETE' )
      IF( munit .NE. 0 ) CLOSE( munit, STATUS = 'DELETE' )
      IF( ivelu3 .NE. 0 ) CLOSE( ivelu3, STATUS = 'KEEP' )
      CALL FREFIL(4,IDUM,IDUM)                                          ! RELEASE AND DELETE ALL DISC FILES
      CALL fdsync( 1 )
!      PRINT *,' END OF SIOSEIS RUN'
      CALL EXIT
!****
!****         INPUT EXECUTE
!****
 1200 CONTINUE
      JOUT=1                                                            ! LEAVE THE DATA IN THE AP WHENEVER POSSIBLE
      IF(FIRST1) GO TO 1210
      ITEMP=INDEX1                                                      ! SWITCH INPUT BUFFERS (INPUT IS DOUBLE BUFFERED)
      INDEX1=INDEX2
      INDEX2=ITEMP
 1210 CONTINUE
      FIRST1=.FALSE.
      IF(ISTOP.EQ.0) GO TO 1240
!****
!*****   THIS NEXT SECTION IS FOR ANY INPUT TYPE PROCESS THAT DETECTS THE
!*****  END OF JOB AND DOES NOT HAVE A TRACE TO PASS ON.  SOME PROCESSES
!*****  SUCH AS GATHER, NEED TO KNOW WHEN THE LAST TRACE IS DETECTED IN ORDER
!*****  TO DO ADDITIONAL WORK,  THEREFORE CARRY ON IF THERE IS SUCH A PROCESS.
 1220 DO J=1,NUM                                                   ! DON'T DO ANYMORE PREGATHER PROCESSES
         IF( pnames(j) .EQ. 'GATHER' ) GOTO 1010
         IF( pnames(j) .EQ. 'WBT' .AND. iwbtyp(numwbt) .EQ. 2) GOTO 1010 
         IF( pnames(j) .EQ. 'TX2FK' ) GOTO 1010 
         IF( pnames(j) .EQ. 'FK2TX' ) GOTO 1010
         IF( pnames(j) .EQ. 'FDMIGR' ) GOTO 1010
         IF( pnames(j) .EQ. 'TX2TP' ) GOTO 1010
         IF( pnames(j) .EQ. 'TP2TX' ) GOTO 1010
         IF( pnames(j) .EQ. 'PSMIGR' ) GOTO 1010
         IF( pnames(j) .EQ. 'COFILT' ) GOTO 1010
         IF( pnames(j) .EQ. 'GRDOUT' ) GOTO 1010
         IF( pnames(j) .EQ. 'SWELL' ) GOTO 1010
         IF( pnames(j) .EQ. 'PROUT' ) 
     &     CALL POUTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     &           BUF(INDEX3),BUF(INDEX3), istop )
         IF( pnames(j) .EQ. 'GEOM' ) lbuf(index1+50) = -1
         IORDER(J)=1
         IOUT(J)=0                                                      ! SET THE PROCESS TO DUMMY
      ENDDO
      GOTO 1010                                                         ! GO TO THE NEXT PROCESS
 1240 CONTINUE
      IF( names(numpro) .EQ. 'INPUT' ) THEN
!          CALL INPUTX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX2),
!     *      istop)
!      ELSEIF( names(numpro) .EQ. 'SEGDIN' ) THEN
!          CALL segdex( buf(index1), buf(index1), buf(index1),
!     *       buf(index3), buf(index3), buf(index3), istop )
      ENDIF
      IINRW=1                                                           ! A SIGNAL INDICATING THAT INPUT WAS DONE & THE UNIT MAY BE REWOUND
      IF(ISTOP.GE.0) GO TO 1250                                         ! ISTOP=-1 IF OPERATOR IS STOPPING THE JOB
!****                           ISTOP=1 IF THE USER IS STOPPING THRU PARAMETERS
      IF( istop .EQ. -1 ) GOTO 1220
!      ISTOP=1
!      GO TO 1210
 1250 IOUT(I)=1                                                         ! A TRACE IS READY IF ISTOP=1
      GO TO 1010
!****
!****          OUTPUT EXECUTE
!****
 1300 CONTINUE
      IOUTRW=1
!      CALL WRTTRC(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
!     *  BUF(INDEX3),BUF(INDEX3))
      GO TO 1010                                                        ! GO TO THE NEXT PROCESS
!****
!****            GEOM EXECUTE
!****
 1400 CONTINUE
      CALL geomex( buf(index1), buf(index1), buf(index1),
     &             buf(index3), buf(index3), buf(index3), istop )
      GO TO 1010
!****
!****           GATHER EXECUTE
!****
 1500 CONTINUE
      IF( IOUT(I) .NE. 0 ) THEN                                         ! ARE THERE STILL TRACES GATHERED BUT NOT PROCESSED
!****  GET THE NEXT TRACE FROM GATHER'S DISC FILE
          CALL RDDISC(IGATUN,BUF(INDEX1),NGWRDS,ISTAT)
          ncdp = ncdp + 1
          IOUT(I)=IOUT(I)-1                                             ! ONE LESS TRACE READY!
          IN=0                                                          ! TELL THE NEXT PROCESS THAT THE DATA IS NOT IN THE AP
!****     the following kludge fixes a problem when the last trace occurred
!****     before anything was output.
          IF( iout(i) .NE. 0 .AND. istop .NE. 0 ) THEN
              istop = 0
              jstop = 1
          ENDIF
          IF( iout(i) .EQ. 0 .AND. jstop .EQ. 1 ) istop = 1
          GOTO 1010                                                     ! NOW PROCESS THAT TRACE
      ENDIF
      IF( ncdp .NE. 0 ) THEN
!         this means that the last time thru was to get rid of a trace and we
!         now need a new input trace
          ncdp = 0
          IF( kstop .EQ. 1 .AND. istop .EQ. 0 ) THEN
              jstop = 1
              GOTO 1550
          ENDIF
          GOTO 1015
      ENDIF
      IF( istop .NE. 0 ) kstop = 1                                      ! no trace input and end of job
 1550 CALL GATHER(BUF(INDEX1),buf(index1),buf(index1),
     *    buf(index3),buf(index3),BUF(INDEX3),KSTOP,NREADY)
      IF( nready .NE. 0 ) THEN                                          ! any ready to pass on?
          ncdp = 1                                                      ! VMS record number
          CALL PODISC(IGATUN,1,0)                                       ! rewind except on VMS
          IOUT(I) = NREADY                                              ! THE NUMBER OF TRACES LEFT IN THE GATHERED DISC FILE
      ELSE
          ncdp = 0
          IF( kstop .EQ. 1 ) THEN                                       ! gather didn't have anything to flush?
              istop = -1
              GOTO 1010
          ENDIF
      ENDIF
!**** Oh boy.  istop = 1 means the current input trace is the last of the job
!**** istop = 0 means there is no end of data in sight.  istop = -1 means the 
!**** last trace already happened and there is not a current trace
      IF( istop .EQ. 1 .AND. kstop .EQ. 0 ) THEN                        ! is this the last trace to be input?
          kstop = 1                                                     ! set to flush the gather disk buffer
          istop = 0
          IF( nready .EQ. 0 ) GOTO 1550
      ENDIF
      IF( nready .NE. 0 ) GOTO 1500
      IF( kstop .EQ. 1 ) GOTO 1030
      GOTO 1015
!****
!****      WBT (WATER BOTTOM TIMES) EXECUTE  -  The auto picker,
!****  iwbtyp has been deactivated by removing WBT as an input process
!****
 1600 numwbt = 1
      GOTO 1650
 1602 numwbt = 2
      GOTO 1650
 1603 numwbt = 3
 1650 CONTINUE
      IF( idowbt(numwbt) .EQ. 1 .AND. iout(i) .LT. 2 .AND. istop .EQ. 0 
     *     .AND. iwbtyp(numwbt) .EQ. 2 ) THEN
          idowbt(numwbt) = 0
          GOTO 1015
      ENDIF
      CALL WBTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1), buf(index3),
     &           iwbtyp(numwbt), nready, istop, numwbt )
      iout(i) = nready
      IF( nready .EQ. 0 ) GOTO 1015
      idowbt(numwbt) = 1
      GO TO 1010
!****
!****                MUTE EXECUTE
!****
 1700 CONTINUE
      CALL MUTEEX( BUF(INDEX1), BUF(INDEX1), BUF(INDEX1),
     *             buf(index3), BUF(INDEX3))
      GO TO 1010
!****
!****                NMO EXECUTE
!****
 1800 CONTINUE
      nmonum = 1
      CALL NMOEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3),BUF(INDEX3), nmonum)
      GO TO 1010
!****
!****                STACK EXECUTE
!****
 1900 CONTINUE
      CALL stackex(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),NREADY)
      IOUT(I)=NREADY
      IF(IOUT(I).NE.0) GO TO 1010                                       ! ANY STACKED TRACES? 
      GO TO 1015                                                        !  NO TRACES FOR THE NEXT PROCESS - FIND AN INPUT PROCESS
!****
!****                 AVENOR EXECUTE
!****
 2000 CONTINUE
      CALL AVENEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****                        FILTER EXECUTE
!****
 2100 CONTINUE
      CALL FILTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****                        SPIKING DECON EXECUTE
!****
 2200 CONTINUE
      CALL SDECEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****                        SYNTHETIC INPUT EXECUTE
!****
 2300 CONTINUE
      JOUT=1                                                            ! LEAVE THE DATA IN THE AP WHENEVER POSSIBLE
      CALL SYNEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3),ISTOP)
      IOUT(I)=1
      GO TO 1010
!****
!****                        AUTOCORRELATION EXECUTE
!****
 2400 CONTINUE
      CALL ACOREX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****              PRINTER OUTPUT EXECUTE
!****
 2500 CONTINUE
      CALL POUTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3),BUF(INDEX3), istop )
      GO TO 1010
!****
!****       WEIGHT (SCALAR MULTIPLY) EXECUTE
!****
 2600 CONTINUE
      CALL WEIGEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****     SHIFT EXECUTE
!****
 2700 CONTINUE
      CALL SHFTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****    TREDIT
!****
 2800 CALL treditex( buf(index1), buf(index1), buf(index1),
     &               buf(index3), buf(index3), buf(index3), iout(i) )
      IF( iout(i) .NE. 0 ) GOTO 1010
      GOTO 1015
!****
!****      AGC EXECUTE
!****
 2900 CONTINUE
      CALL AGCEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3))
      GO TO 1010
!****
!****      MIX EXECUTE
!****
 3000 CONTINUE
      CALL MIXEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3), nrdymix )
      IF( nrdymix .EQ. 0 ) GOTO 1015
      GO TO 1010
!****
!****           VELOCITY ANALYSIS EXECUTE
!****
 3100 CONTINUE
      IF(IOUT(I).NE.0.OR.IFIN.NE.2) GO TO 3105
!     WHEN IOUT(I)=0 AND IFIN=2, IT MEANS THAT
!         ONE ANALYSIS HAS BEEN DONE AND ALL OF IT'S OUTPUT HAS BEEN TAKEN
!         CARE OF.  THEREFORE, GO GET SOME MORE INPUT DATA.
      GO TO 1015                                                        ! GO GET SOME INPUT
 3105 CONTINUE
      IF(IOUT(I).EQ.0) GO TO 3160                                       ! ARE THERE STILL TRACES NOT PROCESSED 
!****
!****  GET THE NEXT TRACE FROM VELAN'S OUTPUT DISC FILE
 3110 CONTINUE
      CALL rddisc(ivelu2,n,1,istat)                                     ! read the number of words to read
      NUMDAT=N-NUMTRHDR
      CALL rddisc(ivelu2,buf(index1),n,istat)                           ! read the trace and header
      IOUT(I)=IOUT(I)-1                                                 ! ONE LESS TRACE READY!
      IF(IOUT(I).EQ.0.AND.IFIN.EQ.2) ISTOP=JSTOP                        ! RESET THE STOP SIGNAL
      IN=0                                                              ! TELL SUBROUTINE INAP THAT THE DATA IS NOT IN THE AP
      GO TO 1010                                                        ! NOW PROCESS THAT TRACE
 3160 CONTINUE                                                          !  THERE'S AN INPUT TRACE TO VELAN IN BUF(INDEX1)
      CALL VELAEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3),BUF(INDEX3),NREADY,ISTOP,IINRW,IFIN)
      IF(NREADY.EQ.0) GO TO 1015                                        ! ANY OUTPUT FROM VELAN
      IF(ISTOP.NE.0) JSTOP=1
      ISTOP=0                                                           ! SET THE STOP TO NO STOP
      IOUT(I)=NREADY
      CALL podisc(ivelu2,1,0)
      GO TO 3110
!****
!****      DISKIN EXECUTE
!****
 3300 CONTINUE
      JOUT=1
      CALL diex(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3),BUF(INDEX3),ISTOP)
      IF(ISTOP.EQ.-1) GO TO 1220                                        !  -1 MEANS THERE IS NO TRACE TO BE PROCESSED
      IOUT(I)=1
      GO TO 1010
!****
!****         DEBIAS EXECUTE
!****
 3400 CONTINUE
      CALL DEBIEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1))
      GO TO 1010
!****
!****                SMUT EXECUTE
!****
 3500 CONTINUE
      CALL SMUTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3))
      GO TO 1010
!****
!****                COMPEN EXECUTE
!****
 3600 CONTINUE
!      CALL COMPEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
!     *   BUF(INDEX3))
      GO TO 1010
!****
!****                SPECTR EXECUTE
!****
 3700 CONTINUE
!      CALL SPECEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
!     *   BUF(INDEX3))
      IPLOT=1                                                           ! SET A FLAG SAYING THAT A PLOT FILE HAS BEEN OPENED
      GO TO 1010
!****
!****            UFILTR EXECUTE
!****
 3800 CONTINUE
      CALL UFILEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3))
      GO TO 1010
!****
!****                T2D EXECUTE
!****
 3900 CONTINUE
      CALL T2DEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3),BUF(INDEX3), istop )
      t2d_flag = .TRUE.
      GO TO 1010
!****
!****     PLOT (SEISMIC) EXECUTE
!****
 4000 CONTINUE
      CALL PLOTEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *  BUF(INDEX3),BUF(INDEX3), istop )
      ISPLOT=1                                                          ! INDICATE THAT A SEISMIC PLOT WAS MADE AND THE FILE MUST BE CLOSED
      GO TO 1010
!****
!****                TX2FK EXECUTE
!****
 4100 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN                                         ! TX2FK NEEDS ALL THE TRACES BEFORE IT OUTPUTS ANY
          IF( istop .EQ. -1 .AND. ntx2fk .EQ. 0 ) GOTO 1030             ! A kludge
          istop1 = istop
          IF( txprestk .NE. 0 ) THEN                                    ! Is this prestack tx2fk?
              temp = buf(index1+50)                                     ! get the end-of-sort flag
              IF( ltemp .EQ. -1 ) THEN
                  ngathers = ngathers + 1
                  IF( ngathers .EQ. txprestk ) THEN
                      istop = 1
                      ngathers = 0
                  ENDIF
              ENDIF
          ENDIF
          CALL TX2FEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *         buf(index3),buf(index3),ISTOP,NREADY)
          iout(i) = nready
          IF( nready .EQ. 0 ) GOTO 1015                                 ! GO GET ANOTHER INPUT TRACE
          istop = 0                                                     ! FAKE OUT THE OTHER PROCESSES BY THINKING THERE IS MORE DATA TO COME
      ENDIF
      CALL getnfk(buf(index1),buf(index1),buf(index1))
      iout(i) = iout(i) - 1                                             ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      itype(numpro) = 1                                                 ! make sure tx2fk is set to be an input process now
      IF( iout(i) .EQ. 0 ) THEN                                         ! ARE THERE ANY MORE TRACES TO COME FROM HERE?
          istop = istop1                                                ! use the istop from before
          IF( istop .EQ. -1 ) istop = 1                                 ! but there is a trace!
          ntx2fk = 0                                                    ! force tx2fex to reinitialize next time
          itype(numpro) = 0                                             ! INDICATE THAT TX2FK IS NO LONGER AN INPUT PROCESS!!!!
      ENDIF
      GO TO 1010
!****
!****                FK2TX EXECUTE
!****
 4200 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN
          IF( istop .EQ. -1 .AND. nfk2tx .EQ. 0 ) GOTO 1030             ! A kludge
          istop2 = istop
          CALL FK2TEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),buf(index3),
     *          buf(index3), buf(index3), ISTOP,NREADY)
          iout(i) = nready
          IF( nready .EQ. 0 ) GOTO 1015
          istop = 0
      ENDIF
      CALL getnxx(buf(index1),buf(index1),buf(index1))
      iout(i) = iout(i) - 1
      itype(numpro) = 1
      IF( iout(i) .EQ. 0 ) THEN
          istop = istop2
          IF( istop .EQ. -1 ) istop = 1                                 ! but there is a trace!
          nfk2tx = 0
          itype(numpro) = 0
      ENDIF
      GO TO 1010
!****
!****                FKMIGR EXECUTE
!****
 4300 CONTINUE
!****   Use all 3 trace buffers as scratch buffers.   This assumes the
!**** input data is saved and used from the "ap" (index1) and
!**** process input is not used for double buffering (index2) 
      CALL FKMIEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),
!     &     BUF(INDEX3), BUF(INDEX3),BUF(INDEX3))
     &     BUF(INDEX1), BUF(INDEX2),BUF(INDEX3))
      IF( ntx2fk .EQ. 0 ) realk = 0.
      GO TO 1010
!****
!****                T2F EXECUTE
!****
 4400 CONTINUE
      CALL T2FEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),
     *   buf(index3),BUF(INDEX3))
      GO TO 1010
!****
!****                F2T EXECUTE
!****
 4500 CONTINUE
      CALL F2TEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3))
      GO TO 1010
!****
!****         REALTM execute     REAL TIME INPUT
!****
 4600 CONTINUE
!     CALL realex(buf(index1),buf(index1),buf(index1),buf(index3),
!    *     buf(index3),buf(index3),istop)
!     iout(i)=1
      GOTO 1010
!****
!****           UDECON execute
!****
 4700 CONTINUE
      CALL udecex( buf(index3), buf(index3), istop )
      GOTO 1010
!****  
!****      FLATEN
!****
 4800 CONTINUE
      CALL flatex(buf(index1),buf(index1),buf(index1),
     *     buf(index3),buf(index3) )
      GOTO 1010
!****
!****    FFILTR  (frequency domain filter)
!****
 4900 CONTINUE
!     CALL ffilex( buf(index1), buf(index1), buf(index1),
!    *     buf(index3) )
      GOTO 1010
!****
!****    FKFILT  (FK domain filter)
!****
 5000 CONTINUE
      CALL fkfiex( buf(index1), buf(index1), buf(index1),
     *     buf(index3), buf(index3), buf(index3) )
      IF( ntx2fk .EQ. 0 ) ktrace = -1                                   ! force reinitialization
      GOTO 1010
!****
!****    PROCESS HEADER
!****
 5200 numhdrp = 1
      GOTO 5205
 5202 numhdrp = 2
      GOTO 5205
 5203 numhdrp = 3
 5205 CALL headex( buf(index1), buf(index1), buf(index1), buf(index1),
     &             buf(index1), numhdrp )
      GOTO 1010
!****
!****    PROCESSes DISKOA, DISKOB, DISKOC, and DISKOD
!****
 5301 idisko = 1
      GOTO 5610
 5302 idisko = 2
      GOTO 5610
 5303 idisko = 3
      GOTO 5610
 5304 idisko = 4
      GOTO 5610
 5305 idisko = 5
      GOTO 5610
 5306 idisko = 6
      GOTO 5610
 5307 idisko = 7
      GOTO 5610
 5308 idisko = 8
      GOTO 5610
 5309 idisko = 9
      GOTO 5610
 5310 idisko = 10
 5610  CALL doex( idisko, buf(index1), buf(index1), buf(index1), 
     *    buf(index3), buf(index3), buf(index3), istop )
      GOTO 1010
!****
!****     Finite Difference Time Migration
!****
 5700 CONTINUE
      IF(IOUT(I).NE.0) GO TO 5720                                       ! FDMIGR NEEDS ALL THE TRACES BEFORE IT OUTPUTS ANY
      CALL fdmiex ( buf(index1), buf(index1), buf(index1),
     *    buf(index3), buf(index3), buf(index3), istop, nready )
      IOUT(I)=NREADY
      IF(NREADY.EQ.0) GO TO 1015                                        ! GO GET ANOTHER INPUT TRACE
      ISTOP=0                                                           ! FAKE OUT THE OTHER PROCESSES BY THINKING THERE IS MORE DATA TO COME
 5720 CALL getntx(buf(index1),buf(index1),buf(index1),buf(index3))
      IOUT(I)=IOUT(I)-1                                                 ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      IF(IOUT(I).NE.0) GO TO 1010                                       ! ARE THERE ANY MORE TRACES TO COME FROM HERE?
      ISTOP=1                                                           ! INDICATE THAT THIS IS THE LAST TRACE
      ITYPE(I)=0                                                        ! INDICATE THAT FDMIGR IS NO LONGER AN INPUT PROCESS!!!!
      GOTO 1010
!****
!****                TX2TP EXECUTE
!****
 5800 CONTINUE
      IF( IOUT(I) .EQ. 0 ) THEN                                         ! TX2TP NEEDS ALL THE TRACES BEFORE IT OUTPUTS ANY
          IF( istop .EQ. -1 .AND. ntx2tp .EQ. 0 ) GOTO 1030
          istop1 = istop
          IF( tpprestk .NE. 0 ) THEN                                    ! Is this prestack tx2tp?
              temp = buf(index1+50)                                     ! get the end-of-sort flag
              IF( ltemp .EQ. -1 ) THEN
                  ngathers = ngathers + 1
                  IF( ngathers .EQ. tpprestk ) THEN
                      istop = 1
                      ngathers = 0
                  ENDIF
              ENDIF
          ENDIF
          CALL TX2TEX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),
     $       buf(index3), ISTOP, NREADY )
          iout(i) = nready
          IF( nready .EQ. 0 ) GOTO 1015                                 ! GO GET ANOTHER INPUT TRACE
          istop = 0                                                     ! FAKE OUT THE OTHER PROCESSES BY THINKING THERE IS MORE DATA TO COME
      ENDIF
      CALL getntp(buf(index1),buf(index1),buf(index1))
      iout(i) = iout(i) - 1                                             ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      itype(numpro) = 1                                                 ! make sure tx2fk is set to be an input process now
      IF( iout(i) .EQ. 0 ) THEN                                         ! ARE THERE ANY MORE TRACES TO COME FROM HERE?
          istop = istop1                                                ! use the istop from before
          IF( istop .EQ. -1 ) istop = 1                                 ! but there is a trace!
          ntx2tp = 0                                                    ! force tx2tex to reinitialize next time
          itype(numpro) = 0                                             ! INDICATE THAT TX2TP IS NO LONGER AN INPUT PROCESS!!!!
      ENDIF
      GOTO 1010
!****
!****                TP2TX EXECUTE
!****
 5900 CONTINUE
      IF( IOUT(I) .EQ. 0 ) THEN
          IF( istop .EQ. -1 ) GOTO 1030
          istoptp = istop
          CALL tp2tex( buf(index1), buf(index1), buf(index1),
     &     buf(index3), buf(index3), buf(index3),istoptp, ntxready )
          iout(i) = ntxready
          IF( ntxready .EQ. 0 ) GOTO 1015
          istop = 0
      ENDIF
      CALL gettptx( buf(index1), buf(index1), buf(index1) )
      iout(i) = iout(i) - 1
      itype(numpro) = 1
      IF( iout(i) .EQ. 0 ) THEN
          istop = istoptp
          IF( istop .EQ. -1 ) istop = 1
          itype(numpro) = 0
      ENDIF
      GO TO 1010
!****
!****    IRIS execute
!****
 6000 CONTINUE
!      CALL irisex( buf(index1), buf(index1), buf(index1), buf(index3) )
      GOTO 1010
!****
!****    GAINS execute
!****
 6100 CONTINUE
      num_gains = 1
 6110 CALL gainex( buf(index1), buf(index1), buf(index1), buf(index3), 
     &     num_gains )
      GOTO 1010
 6120 CONTINUE
      num_gains = 2
      GOTO 6110
 6130 CONTINUE
      num_gains = 3
      GOTO 6110
!****
!****     Finite Difference Diffractions
!****
 6200 CONTINUE
      IF(IOUT(I).NE.0) GO TO 6220                                       ! FDDIFF NEEDS ALL THE TRACES BEFORE IT OUTPUTS ANY
      CALL fdfmex ( buf(index1), buf(index1), buf(index1),
     *    buf(index3), buf(index3), buf(index3), istop, nready )
      IOUT(I)=NREADY
      IF(NREADY.EQ.0) GO TO 1015                                        ! GO GET ANOTHER INPUT TRACE
      ISTOP=0                                                           ! FAKE OUT THE OTHER PROCESSES BY THINKING THERE IS MORE DATA TO COME
 6220 CALL getntd(buf(index1),buf(index1),buf(index1),buf(index3))
      IOUT(I)=IOUT(I)-1                                                 ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      IF(IOUT(I).NE.0) GO TO 1010                                       ! ARE THERE ANY MORE TRACES TO COME FROM HERE?
      ISTOP=1                                                           ! INDICATE THAT THIS IS THE LAST TRACE
      ITYPE(I)=0                                                        ! INDICATE THAT FDDIFF IS NO LONGER AN INPUT PROCESS!!!!
      GOTO 1010
!****
!****     USGS PSEUDO REFLECTION COEFFICIENT
!****
 6300 CONTINUE
!      CALL pseuex( buf(index1), buf(index1), buf(index1), 
!     &             buf(index3) )
      GOTO 1010
!****
!****   SORT
!****
 6400 CALL sortex( buf(index1), buf(index1), buf(index1) )
!**** SORT might be the only process.  If so, stop all processing.
      IF( num .EQ. 1 ) istop = -1
      GOTO 1010
!****
!****    DMO execute
!****
 6500 CONTINUE
      IF( istop .EQ. -1 ) GOTO 1010
      CALL dmoex( buf(index1), buf(index1), buf(index1), 
     &            buf(index3), buf(index3), buf(index3) )
      GOTO 1010
!****
!****    LOGST1 execute
!****
 6600 lognum = 1
 6610 CALL logsex( lognum, buf(index1), buf(index1), buf(index1), 
     &     buf(index3) )
      GOTO 1010
!****
!****    LOGST2 execute
!****
 6700 lognum = 2
      GOTO 6610
!****
 6800 CALL resaex( buf(index1), buf(index1), buf(index1), 
     &             buf(index3), buf(index3) )
      GOTO 1010
!****
!****    ProMax disk input (not SEGY)
!****
 6900 CONTINUE
!      JOUT=1
!      CALL maxiex( BUF(INDEX1), BUF(INDEX1), BUF(INDEX1), istop )
!      IF(ISTOP.EQ.-1) GO TO 1220                                        !  -1 MEANS THERE IS NO TRACE TO BE PROCESSED
!      IOUT(I)=1
      GO TO 1010
!****
!****    ProMax disk output (not SEGY)
!****
 7000 CONTINUE
!      CALL maxoex( buf(index1), buf(index1), buf(index1),
!     &             buf(index3), buf(index3), buf(index3), istop )
      GOTO 1010
!****
!****    Despike
!****
 7100 CALL despikex( buf(index1), buf(index1), buf(index1),
     &               buf(index3), buf(index3), buf(index3), iout(i) )
      IF( iout(i) .NE. 0 ) GOTO 1010
      GOTO 1015
!****
!****    SADD
!****
 7200 CALL saddex( buf(index1), buf(index1), buf(index1) )
      GOTO 1010
!****
!****    CAT
!****
 7300 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN
          IF( ibackup .EQ. 1 ) THEN                                     ! iout(i) will be 0 after all the getcat(s) are done.
              ibackup = 0
              istop = istopcat
              GOTO 1015
          ENDIF
          CALL catex( buf(index1), buf(index1), buf(index1), 
     &               buf(index3), buf(index3), buf(index3), nreadycat )
          IF( nreadycat .EQ. 0 ) THEN
              ibackup = 1
              GOTO 1010
          ENDIF
          IF( nreadycat .LT. 0 ) GOTO 1015
          iout(i) = nreadycat
          istopcat = istop
          istop = 0
      ENDIF
      IF( iout(i) .LT. 0 ) GOTO 1010
      CALL getcat( buf(index1), buf(index1), buf(index1),
     &               buf(index3), buf(index3), buf(index3) )
      iout(i)=iout(i)-1                                                 ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      IF( iout(i) .EQ. 0 ) ibackup = 1
      GOTO 1010
!****
!****                FKSHIFT EXECUTE
!****
 7400 CONTINUE
      CALL fkshex( buf(index1), buf(index1), buf(index1),
     &             buf(index3), buf(index3), buf(index3) )
      IF( ntx2fk .EQ. 0 ) realk1 = 0.
      GO TO 1010
!****
!****                NMO2 EXECUTE
!****
 7500 CONTINUE
      nmonum = 2
      CALL NMO2EX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3),BUF(INDEX3), nmonum)
      GO TO 1010
!****
!****     Split Step Migration
!****
 7600 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN
          CALL ssmiex ( buf(index1), buf(index1), buf(index1),
     *         buf(index3), buf(index3), buf(index3), istop, nready )
          iout(i) = nready
          IF( nready .EQ. 0 ) GOTO 1015
          istop = 0
      ENDIF
      CALL getnext(buf(index1),buf(index1),buf(index1),buf(index3))
      IOUT(I)=IOUT(I)-1                                                 ! SIGNAL THAT THERE IS NOW ONE LESS TRACE READY
      IF(IOUT(I).NE.0) GO TO 1010                                       ! ARE THERE ANY MORE TRACES TO COME FROM HERE?
      ISTOP=1                                                           ! INDICATE THAT THIS IS THE LAST TRACE
      ITYPE(I)=0                                                        ! INDICATE THAT IT IS NO LONGER AN INPUT PROCESS!!!!
      GOTO 1010
!****
!****            UADD EXECUTE
!****
 7700 CONTINUE
      CALL uaddex(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3))
      GO TO 1010
!****
!****            UMULT EXECUTE
!****
 7800 CONTINUE
      CALL umultex(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3))
      GO TO 1010
!****
!****           HISTORY
!****
 7900 CONTINUE
!      CALL histex( buf(index3), istop )
      GOTO 1010
!****
!****                NMO3 EXECUTE
!****
 8000 CONTINUE
      nmonum = 3
      CALL NMO3EX(BUF(INDEX1),BUF(INDEX1),BUF(INDEX1),BUF(INDEX3),
     *   BUF(INDEX3),BUF(INDEX3), nmonum)
      GO TO 1010
!****
!****                COFILT
!****
 8100 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN
!          IF( istop .EQ. -1 .AND. ncofilt .EQ. 0 ) GOTO 1030
          istop2 = istop
          CALL cfilex( buf(index1), buf(index1), buf(index1), 
     &        buf(index3),buf(index3),buf(index3),istop, nready )
          iout(i) = nready
          IF( nready .EQ. 0 ) GOTO 1015
          istop = 0
      ENDIF
      CALL getcfil( buf(index1) )
      iout(i)=iout(i)-1
      itype(numpro) = 1
      IF( iout(i) .EQ. 0 ) THEN
          istop = istop2
          IF( istop .EQ. -1 ) istop = 1                                 ! but there is a trace!
          ncofilt = 0
          itype(numpro) = 0
      ENDIF
      GOTO 1010
!****
!****   SEG2 input
!****
 8200 CONTINUE
      iout(i) = 0
      CALL seg2ex( buf(index1), buf(index1), buf(index1), 
     &        buf(index3),buf(index3),buf(index3), istop )
      IF( istop .EQ. -1 ) GOTO 1220                                     ! means no end of job and no trace in buf
      iout(i) = 1
      GOTO 1010
!****
!****            CORR EXECUTE
!****
 8300 CONTINUE
      CALL xcorex( buf(index1), buf(index1), buf(index1),
     &             buf(index3), buf(index3), buf(index3) )
      GO TO 1010
!****
!****                TRIM
!****
 8400 CONTINUE
      CALL stkex( buf(index1), buf(index1), buf(index1), 
     &        buf(index3),buf(index3),buf(index3),istop, nready )
      iout(i) = nready
      IF( nready .EQ. 0 ) GOTO 1015
      GOTO 1010
!****
!****           GRDOUT
!****
 8500 CONTINUE
!      CALL grdoutex( buf(index1), buf(index1), buf(index1),
!     &             buf(index3), buf(index3), buf(index3), istop )
      GO TO 1010
!****
!****           XSTAR
!****  This is somewhat messy becuase sometimes there's no output
!****  and sometimes there's more than one trace to output.
 8600 CONTINUE
      IF( nxstar .EQ. 1 ) THEN
!****     we got rid of the last ping on disk, keep backing up
          nxstar = 0
          GOTO 1015
      ENDIF
      IF( nxstar .GT. 0 ) THEN
!          CALL getxstartr(buf(index1),buf(index1),buf(index1),nrdyxstar)
          nxstar = nxstar -1
          iout(i) = iout(i) -1
          GOTO 1010
      ELSE
!          CALL xstarex( buf(index1), buf(index1), buf(index1),
!     &         nrdyxstar)
          iout(i) = nrdyxstar + 1
          nxstar = nrdyxstar
          IF( nrdyxstar .EQ. 0 ) GOTO 1015
      ENDIF
      GO TO 1010
!****
!****    SEG-D disk input
!****
 8700 CONTINUE
      CALL segddex( buf(index1), buf(index1), buf(index1),
     *       buf(index3), buf(index3), buf(index3), istop )
      IF( istop .EQ. -1 ) GOTO 1220
      GOTO 1010
!****
!****      SWELL EXECUTE
!****
 8800 CONTINUE
      IF( iout(i) .EQ. 0 ) THEN
          IF( istop .LT. 0 .AND. isw_stop .LT. 0 ) GOTO 1220
          IF( nswell .NE. 0 ) THEN
              nswell = 0
              istop = isw_stop
              GOTO 1015
          ENDIF
          isw_stop = istop
          CALL swellex( buf(index1), buf(index1), buf(index1),
     *         buf(index3), buf(index3), buf(index3), istop, nrdyswell )
          IF( nrdyswell .EQ. 0 ) GOTO 1015    ! no output
          nswell = nrdyswell
          iout(i) = nrdyswell - 1
          IF( iout(i) .GT. 0 ) istop = 0
          IF( isw_stop .GE. 0 ) GOTO 1010   ! pass the trace to the next process
      ENDIF
      CALL get_swell( buf(index1), buf(index1), buf(index1) )
      iout(i) = iout(i) - 1
      IF( iout(i) .LT. 0 ) THEN
          istop = isw_stop
          GOTO 1010
      ENDIF
      GOTO 1010
!****
      END
