      subroutine spltfk(entry,idunit,idisko,
     $                          buf,lbuf,ibuf,scr,lscr,iscr)
!-------------------------------------------------------------------------------
!
!    spltfk is the companion routine to the Input routine MrgFK and is called
! from DOEX to reformat an output FK data file is the user wants it in
! 'User Friendly' format to look at. This format has wavenumbers running from
! -nyq -> 0 -> +nyq and frequencies running from 0 -> +nyq. These traces are
! formatted in polar coordinates with amplitudes followed by phases. This is the
! most useful format for display. User format has an IDtype = IDFKPLRU (5) set
! in the binary header.
!
! Inputs:
!  entry : The type of entry into spltfk. This is defined in terms of
!            constants in splitgbl.inc
!           = TRNSPARM - Transfer relevant input parameters to local storage
!           = CHCKBNY  - Check the binary header/input paremeter for consistency
!           = WRTETRC  - Split & Write a trace to disk.
!
! idunit : The file stream for the current output process
! idisko : The I.D number of the current output process. Up to MAXDO output
!          processes are allowed in a given job.
!
! buf/lbuf/ibuf : Equivalenced arrays holding the trace (header + data). Since
!          output can occur at any position in the processing sequence must
!          leave the trace in buf alone.
! scr/lscr/iscr : Scratch arrays used to compose output trace.
!
! Call Chain:
!     CONTRO:DOEX:spltfk
!
! Externals:
!   EXIT, CHKPROC, dskpos
!   POW2 : part of TX2FEX
!   sWrHdr, sWrTrc : local routines
!
! Last Modified:
!    3/14/89 ajh : Made all comments (I hope) start with a blank
! 8 Apr 09 - Use common numdat rather than segy header word ISAMPPTR
!-------------------------------------------------------------------------------
!
      parameter ( MAXDO   = 8 )                                         ! The max. no. of output processes.
C INCLUDE FILES
! This include file include all (?) of the parameters needed to define SIOSEIS
! I/O routines. In particular it defines positions of objects in the SEGY
! headers & constants for these values
!
! EBCDIC HEADER
! Declare Header lengths. These are the length of the EXTERNAL disk image in
! host words.
      integer     CRAYEBC, NORMEBC
      parameter ( CRAYEBC  = 400)
      parameter ( NORMEBC  = 2*CRAYEBC)
!
! BINARY HEADER
      integer     CRAYBIN, NORMBIN
      parameter ( CRAYBIN  =  50)                                       ! length of external disk image
      parameter ( NORMBIN  = 2*CRAYBIN)
!
!  The position of objects in the Binary header
      integer    NTRCPTR,IFMTPTR, IDTYPPTR, INKPTR, ITSIPTR
      integer    ITDELPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
!
!..Constants for SEGY Format (IFMTPTR)
      integer IBMFP, INT16, INT32, HOSTFP
      parameter (IBMFP  = 1)                                            ! IBM floating point
      parameter (INT32  = 2)
      parameter (INT16  = 3)
      parameter (HOSTFP = 5)                                            ! Host FP. In actuality Host FP >= 5
!
!.. Data Domains/Types (IDTYPPTR)
      integer    IDTX, IDFKRCT, IDFKPLR, IDFKPLRU
      parameter (IDTX     = 1)
      parameter (IDFKRCT  = 2)
      parameter (IDFKPLR  = 3)
      parameter (IDFKPLRU = 8)
!
! TRACE HEADER
!.. Declare header length on external disk files. Internal length is given by
! Numhdr in common block READT.
      integer  CRAYTHDR, NORMTHDR
      parameter ( CRAYTHDR =  30)                                       ! Length of external disk image
      parameter ( NORMTHDR = 2*CRAYTHDR)
!
!    The positions of elements of the trace header are defined by elements of
! the common block SEGYPTR. This block is initialized in routine SETPTR. The
! position of elements differs on the CRAY since it does not allow mixing of
! Integer*16 & Integer*32 words but only has Integer*64.
!
      integer LLSEQPTR                                                  ! Trace Sequence No. within line
      integer LRSEQPTR                                                  ! Trace Sequence No. within reel
      integer LSHOTPTR                                                  ! Shot number or Stacked Number
      integer LSHTRPTR                                                  ! Trace number within Shot
      integer LRPNPTR                                                   ! RP or CDP number
      integer LRPTRPTR                                                  ! Trace No. within CDP
      integer ITRIDPTR                                                  ! Trace ID Live(1)/Dead(2)
      integer LDISTPTR                                                  ! Source to Receiver Distance
      integer LWBDPTR                                                   ! Water Bottom depth at source
      integer LSXCOPTR                                                  ! Source X co-ordinate
      integer LRXCOPTR                                                  ! Receiver X co-ordingate
      integer IDELMPTR                                                  ! Deep water delay in ms.
      integer ISTMPTR                                                   ! Start Mute time in ms.
      integer IENDMPTR                                                  !   End Mute time in ms
      integer ISAMPPTR                                                  ! Number of data samples in trace
      integer ISIPTR                                                    ! Sample interval of trace in us
      integer IYRPTR                                                    ! Year data was recorded
      integer IDAYPTR                                                   ! Day of year
      integer IHRPTR                                                    ! Hour of day
      integer IMINPTR                                                   ! Minute of hour
      integer ISECPTR                                                   ! Second of Minute
      integer IGMTPTR                                                   ! Time zone of header Local(1)/GMT(2)
      integer LDELSPTR                                                  ! Deep water delay in secs.
      integer LSMUSPTR                                                  ! Start of mute in seconds
      integer LEMUSPTR                                                  ! End of mute in seconds
      integer LSISPTR                                                   ! Sample interval in seconds
      integer LWBTSPTR                                                  ! Water bottom time in seconds
      integer LGATPTR                                                   ! No. of traces in stacked data(>0) or End of Gather(<0)
      integer LSSMSPTR                                                  ! Start of surgical mute in secs.
      integer LESMSPTR                                                  ! End of surgical mute in secs
      integer LSBPTR                                                    ! SeaBeam slant range
!
      common /SEGYPTR/ LLSEQPTR, LRSEQPTR, LSHOTPTR, LSHTRPTR, LRPNPTR,
     *                 LRPTRPTR, ITRIDPTR, LDISTPTR, LWBDPTR,  LSXCOPTR,
     *                 LRXCOPTR, IDELMPTR, ISTMPTR,  IENDMPTR, ISAMPPTR,
     *                 ISIPTR,   IYRPTR,   IDAYPTR,  IHRPTR,   IMINPTR,
     *                 ISECPTR,  IGMTPTR,  LDELSPTR, LSMUSPTR, LEMUSPTR,
     *                 LSISPTR,  LWBTSPTR, LGATPTR,  LSSMSPTR, LESMSPTR,
     *                 LSBPTR
C
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
C
!.. Define constants for use with the DISKIO subroutines getfil, frefil,
! podisc
! GETFIL constants
      integer    CREATTMP, CREATNEW
      parameter (CREATTMP   = 1)                                        ! Create a temporary file
      parameter (CREATNEW   = 3)                                        ! Create a new named file
! FREFIL constants
      integer    RLDLCLS
      parameter (RLDLCLS = 3)
! PODISC constants
      integer    POSABS, POSREL, DSKSTRT
      parameter (POSABS     = 1)
      parameter (POSREL     = 2)
      parameter (DSKSTRT    = 0)                                        ! The starting position on the disk
!.. Include file for SplitFK, MrgFK & related processes
!
!
!... Computer Type
      integer PRIME, VAXUNIX, APOLLO, VAXVMS, CRAY
      integer CONVEX, IEEE
      parameter (PRIME   = 1)
      parameter (VAXUNIX = 2)
      parameter (APOLLO  = 3)
      parameter (VAXVMS  = 4)
      parameter (CRAY    = 5)
      parameter (CONVEX  = 6)
      parameter (IEEE    = 7)
!
      parameter ( NOOUTP  = -997)                                       ! No Output process yet seen
!
!.. Parameters for DiskPos Subroutine
      integer   DPINIT, FIRSTHDR, TRCHDR, TRCDATA
      parameter ( DPINIT   = 1)
      parameter ( FIRSTHDR = 2)
      parameter ( TRCHDR   = 3)
      parameter ( TRCDATA  = 4)
!
!.. Entries for ReadTrce
      integer RTINIT, READDATA
      parameter(RTINIT   = 1)
      parameter(READDATA = 2)
!
      parameter ( KSPLTLN = 15)                                         ! Number of parameters in the block
!
      integer   COMPOFS, FMTOFS, DUNITOFS, IDARNOFS, DDATLOFS, CPOSOFS
      integer   LPRNTOFS, IDISKOFS, TRCCTOFS, NXOFS
      integer   SINGOFS, MULTOFS
!..                                   Define offsets to the parameters
      parameter ( COMPOFS = 1)                                          !   Computer type
      parameter (  FMTOFS = 2)                                          !   Input data format
      parameter (DUNITOFS = 3)                                          !   Input data file unit
      parameter (IDARNOFS = 4)                                          !   idarn : misalignment on the Cray
      parameter (DDATLOFS = 5)                                          !   No. of samples per output trace
      parameter ( CPOSOFS = 6)                                          !   Current Position of file pointer
      parameter (LPRNTOFS = 7)                                          !   Lprint debug value
      parameter ( NEWSOFS = 8)                                          !   A new start frequency
      parameter ( NEWEOFS = 9)                                          !   A new finish frequency
      parameter (IDISKOFS = 10)                                         !   The output process number
      parameter (TRCCTOFS = 11)                                         !   The number of transferred trace
      parameter (   NXOFS = 12)                                         !   The total number of output traces
      parameter ( SINGOFS = 13)                                         !   Pointer to header comp that is set to 1
      parameter ( MULTOFS = 14)                                         !   Pointer to header comp that records trace No
!
      integer  OHDROFS
      parameter ( OHDROFS = 15)                                         !  Temporary bugfix to read old header types

!
!.. "Global" Include file for SplitFK routines that reformat an FK dataset i
!.. on output. These declarations are included in DOEX.
      integer   IPTFKU, NOFKCONV
      parameter (NOFKCONV = 0)
      parameter (IPTFKU   = 4)
!
!.. Entry constants for SplitFK
      integer TRNSPARM, CHCKBNY, WRTETRC
      parameter( TRNSPARM = 1)
      parameter( CHCKBNY  = 2)
      parameter( WRTETRC  = 3)
!
!.. Holds data types for the various output streams
      integer DataID(MAXDO)
      common /spltblk1/ DataID
      save /spltblk1/

C PROGRAM                                                               ! MAXDO is needed by splitgbl
!
      integer entry
      integer idunit, idisko
      real       buf(111), scr(111)                                         ! buffer arrays
      integer   lbuf(111), lscr(111)
      integer*2 ibuf(111), iscr(111)
!
      common /EDITS/ ierror, iwarn, irun, now, icompt
!
      common /READT/ iunit, numhdr, numdat, ihunit, ireeln, intrcs,
     *               ifmt, nskip, secs, lrenum, isrcf, idum
!
      integer SplParam(KSPLTLN, MAXDO)
      integer ldisko                                                    ! The last output process we saw
      integer baset, basei
      integer singindx, multindx
      integer TrcHdrL
      integer fno, lno, ftr, ltr
      logical first
!
      integer    pow2                                                   ! External function
      logical    chkprc                                                 ! External function
      integer    trace, outnum
!
      save ldisko
      save lno, fno, ftr, ltr
      save multindx, singindx
      save SplParam
      save TrcHdrL, first
!
      data  ldisko / NOOUTP/
      data  first / .TRUE. /
!
      lprint = SplParam(LPRNTOFS,idisko)
      if (first) then
        first = .FALSE.
        if (icompt.ne.CRAY) then                                        ! Set up the lengths for headers
          TrcHdrL = NORMTHDR
        else
          TrcHdrL = CRAYTHDR
        endif
      endif
!
!**************************                           **************************
!..                   Make a local copy of the list parameters
!..                 We will check them on the next CHCKBNY entry
!**************************                           **************************
!
      if ( entry.eq.TRNSPARM) then
        SplParam(IDISKOFS,idisko) = idisko
        SplParam(COMPOFS, idisko) = icompt
        fno = lscr(26)                                                  ! Magic Numbers are from
        lno = lscr(27)                                                  ! ordering of DIEX params
        ftr = lscr(29)
        ltr = lscr(30)
        SplParam(FMTOFS,  idisko) = lscr(33)
        SplParam(SINGOFS, idisko) = lscr(34)                            ! Store ontrcs here for now
        SplParam(LPRNTOFS,idisko) = lscr(37)
        SplParam(NEWSOFS, idisko) =  scr(38)
        SplParam(NEWEOFS, idisko) =  scr(38)
        ldisko = idisko
!
        SplParam(OHDROFS,idisko) = 0                                    ! Bug fix for old disk files
        RETURN
      endif
!
!**************************                           **************************
!       .. Check the Data ID type contained in the Binary header
!**************************                           **************************
!
      if (entry.eq.CHCKBNY) then
        idtype                    = iscr(IDTYPPTR)
        SplParam(DUNITOFS,idisko) = idunit
        SplParam(TRCCTOFS,idisko) = 0                                   ! Initialize the trace count
!
        if(IAND(lprint,2).ne.0) then
           print '(/A)',' DOEX (spltfk) debug info'
           print *,' idtype: ',idtype,' Num Traces: ',iscr(INKPTR)
        endif
!
        if (idtype.ne.IDFKPLRU)
     $     RETURN                                                       ! Dont do anything. Not our data type
!
        if (.not.chkprc() ) then                                        ! An FK process ?
          DataID(idisko) = NOFKCONV
        else                                                            ! Setup to reformat data
          DataID(idisko) =  IPTFKU
          inputNum       = iscr(INKPTR)                                 ! The no. of input wavenumber
          outNum         = 2 * inputNum - 1                             ! Index output from 0
          iscr(INKPTR)   = outnum                                       ! Adjust no. of output traces
          SplParam(NXOFS,idisko) = inputNum
!
          if (inputNum.ne.2**POW2(inputNum-1)+1) then
            print 500,
     $      'Number of wavenumbers in Binary header is not 2**N + 1'
            STOP
          endif
!
          if (fno.eq.lno) then
            if (  (ltr.ne.0).AND.                                       ! User has specified trace range
     $           ( (ftr.ne.1).OR.
     $          ( (ltr.ne.InputNum).AND.(ltr.ne.OutNum) ) ) ) then
              print 500,' Requested Output Trace Range doesn''t ',
     $                  'match FK trace range'
             STOP
            endif
          else if ( (lno.ne.0).AND.                                     ! User has specified a shot range
     $            ( (fno.ne.1).OR.
     $          ( (lno.ne.InputNum).AND.(lno.ne.OutNum) ) ) ) then
              print 500,' Requested Output Shot/RP Range doesn''t ',
     $                  'match FK Shot/RP range'
             STOP
          endif
        endif
        ldisko = idisko
        RETURN
      endif
!
!**************************                           **************************
!                         Split & write a trace to disk
!**************************                           **************************
!
      if (entry.eq.WRTETRC) then
        DO i = 1, numhdr                                            ! Move the trace header to scratch
  300      lscr(i) = lbuf(i)                                               ! so we can alter it as necessary
        ENDDO
!
        scr(LDELSPTR)   = 0.                                            ! set start to zero frequency
        iscr(IDELMPTR)  = 0                                             ! Dittio for ms. version
!        nby4            = iscr(ISAMPPTR) / 4
        nby4            = numdat / 4
!        iscr(ISAMPPTR)  = iscr(ISAMPPTR) / 2 + 2                        ! Need <= 0 - Nyq on output
        numdat = numdat / 2 + 2                        ! Need <= 0 - Nyq on output
!        SplParam(DDATLOFS,idisko) = iscr(ISAMPPTR)
        SplParam(DDATLOFS,idisko) = numdat
        ntrc           = SplParam(TRCCTOFS,idisko)
!
        if ( idisko.ne.ldisko) then
           multindx = SplParam(SINGOFS,idisko)
           singindx = SplParam(MULTOFS,idisko)
           call dskpos(DPINIT,0,SplParam(1,idisko))
        endif
!
!                                                     *** First output trace ***
        if ( ntrc.eq.0 ) then
          if(lbuf(LRPTRPTR).eq.0) then
            if(SplParam(SINGOFS, idisko).eq.1) then                     ! Indexing by shots
              SplParam(SINGOFS, idisko) = LSHTRPTR
              SplParam(MULTOFS, idisko) = LSHOTPTR
            else                                                        ! Indexing by shot tr no.
              SplParam(SINGOFS, idisko) = LSHOTPTR
              SplParam(MULTOFS, idisko) = LSHTRPTR
            endif
          else
            if(SplParam(SINGOFS, idisko).eq.1) then                     ! Indexing by RPs
              SplParam(SINGOFS, idisko) = LRPNPTR
              SplParam(MULTOFS, idisko) = LRPTRPTR
            else                                                        ! Indexing by RP tr no.
              SplParam(SINGOFS, idisko) = LRPTRPTR
              SplParam(MULTOFS, idisko) = LRPNPTR
            endif
          endif
          multindx = SplParam(SINGOFS,idisko)
          singindx = SplParam(MULTOFS,idisko)
!
          call dskpos(DPINIT  ,0,SplParam(1,idisko))
          call dskpos(FIRSTHDR,0,SplParam(1,idisko))
!
!..                         Write dummy traces to hold the -ve wavenumber traces
          do 310 i = 1, SplParam(NXOFS, idisko) - 1
            call sWrHdr(SplParam(1,idisko),scr,lscr,iscr)
            call sWrTrc(SplParam(1,idisko),scr(numhdr+1)
     $                   ,lscr(numhdr+1),iscr(2*numhdr+1))
  310     continue
        endif                                                           ! of if ntrc == 0
!
!..                                    *** Write +ve wavenumber data to disk ***
        trace = SplParam(NXOFS,idisko) + ntrc
        lscr(singindx) = 1                                              ! Fix the trace header
        lscr(multindx) = trace
!
        call dskpos(TRCHDR,trace,SplParam(1,idisko))
        call sWrHdr(SplParam(1,idisko),scr,lscr,iscr)
!...
         baset = numhdr + nby4                                          !  Transfer +ve amplitudes
         basei = numhdr
         do i = 1, nby4
  320      scr(basei + i) = buf(baset + i)
         enddo
         scr(baset + 1) = buf(numhdr + 1)                               ! Transfer Nyquist
!
         baset = numhdr + 3 * nby4                                      ! Transfer +ve phases
         basei = numhdr + nby4 + 1
         do i = 1, nby4
  330      scr(basei + i) = buf(baset + i)
         enddo
         scr(numhdr+2*nby4+ 2) = buf(numhdr + 2*nby4 + 1)               ! Transfer Nyquist
!..
         call sWrTrc(SplParam(1,idisko),scr(numhdr+1)                   ! Write +ve trace
     $                   ,lscr(numhdr+1),iscr(2*numhdr+1))
!
!                                       *** Also have a -ve wavenumber trace ***
       if (ntrc.ne.0) then
         trace = SplParam(NXOFS,idisko) - ntrc
         lscr(singindx) = 1                                             ! Fix the trace header
         lscr(multindx) = trace
         call dskpos(TRCHDR,trace,SplParam(1,idisko))
         call sWrHdr(SplParam(1,idisko),scr,lscr,iscr)
!...
         baset = numhdr + nby4 + 2                                      ! Transfer -ve amplitudes
         do i = 1, nby4+1                                           ! Start at 0. freq.
  340      scr(numhdr + i) = buf(baset - i)
         enddo
!
         baset = numhdr + 3 * nby4 + 2                                  ! Transfer -ve phases
         basei = numhdr + nby4 + 1
         do i = 1, nby4 + 1
  350      scr(basei + i) = buf(baset - i)
         enddo
!
         call sWrTrc(SplParam(1,idisko),scr(numhdr+1)                   ! Write -ve trace
     $                   ,lscr(numhdr+1),iscr(2*numhdr+1))
        endif                                                           ! of if ntrc <> 0
!
        SplParam(TRCCTOFS,idisko) = ntrc + 1                            ! Update no. of traces written
      endif                                                             ! of if entry
!
      RETURN
  500 format(' **** DOEX(spltfk) ERROR **** ',/(A))
      end
!
!
      subroutine sWrHdr(SplParam,scr,lscr,iscr)
!-------------------------------------------------------------------------------
!    sWrHdr writes an SEGY format trace header to disk. The header is format
! appropriately for the different computers supported by SIOSEIS.
!
! Inputs :
!    SplParam(1) : The relevent control parameters are passed through the
!                  SplParam block. The elements used by sWrHdr are
!    at COMPOFS  :  The computer type
!    at DUNITOFS : The file stream no.
!
!    scr/lscr/iscr : equivalenced scratch array.
!
! Call Chain:
!   CONTRO:DOEX:spltfk:sWrHdr
!
! Externals:
!   SWAP16, SWAP32 : DEC byte swap routines
!   I82I4, I82I2   : CRAY integer length conversion routines
!   WRDISC
!
! Last Modified:
!   10/7/88
!-------------------------------------------------------------------------------
!
C INCLUDE FILES
! This include file include all (?) of the parameters needed to define SIOSEIS
! I/O routines. In particular it defines positions of objects in the SEGY
! headers & constants for these values
!
! EBCDIC HEADER
! Declare Header lengths. These are the length of the EXTERNAL disk image in
! host words.
      integer     CRAYEBC, NORMEBC
      parameter ( CRAYEBC  = 400)
      parameter ( NORMEBC  = 2*CRAYEBC)
!
! BINARY HEADER
      integer     CRAYBIN, NORMBIN
      parameter ( CRAYBIN  =  50)                                       ! length of external disk image
      parameter ( NORMBIN  = 2*CRAYBIN)
!
!  The position of objects in the Binary header
      integer    NTRCPTR,IFMTPTR, IDTYPPTR, INKPTR, ITSIPTR
      integer    ITDELPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
!
!..Constants for SEGY Format (IFMTPTR)
      integer IBMFP, INT16, INT32, HOSTFP
      parameter (IBMFP  = 1)                                            ! IBM floating point
      parameter (INT32  = 2)
      parameter (INT16  = 3)
      parameter (HOSTFP = 5)                                            ! Host FP. In actuality Host FP >= 5
!
!.. Data Domains/Types (IDTYPPTR)
      integer    IDTX, IDFKRCT, IDFKPLR, IDFKPLRU
      parameter (IDTX     = 1)
      parameter (IDFKRCT  = 2)
      parameter (IDFKPLR  = 3)
      parameter (IDFKPLRU = 8)
!
! TRACE HEADER
!.. Declare header length on external disk files. Internal length is given by
! Numhdr in common block READT.
      integer  CRAYTHDR, NORMTHDR
      parameter ( CRAYTHDR =  30)                                       ! Length of external disk image
      parameter ( NORMTHDR = 2*CRAYTHDR)
!
!    The positions of elements of the trace header are defined by elements of
! the common block SEGYPTR. This block is initialized in routine SETPTR. The
! position of elements differs on the CRAY since it does not allow mixing of
! Integer*16 & Integer*32 words but only has Integer*64.
!
      integer LLSEQPTR                                                  ! Trace Sequence No. within line
      integer LRSEQPTR                                                  ! Trace Sequence No. within reel
      integer LSHOTPTR                                                  ! Shot number or Stacked Number
      integer LSHTRPTR                                                  ! Trace number within Shot
      integer LRPNPTR                                                   ! RP or CDP number
      integer LRPTRPTR                                                  ! Trace No. within CDP
      integer ITRIDPTR                                                  ! Trace ID Live(1)/Dead(2)
      integer LDISTPTR                                                  ! Source to Receiver Distance
      integer LWBDPTR                                                   ! Water Bottom depth at source
      integer LSXCOPTR                                                  ! Source X co-ordinate
      integer LRXCOPTR                                                  ! Receiver X co-ordingate
      integer IDELMPTR                                                  ! Deep water delay in ms.
      integer ISTMPTR                                                   ! Start Mute time in ms.
      integer IENDMPTR                                                  !   End Mute time in ms
      integer ISAMPPTR                                                  ! Number of data samples in trace
      integer ISIPTR                                                    ! Sample interval of trace in us
      integer IYRPTR                                                    ! Year data was recorded
      integer IDAYPTR                                                   ! Day of year
      integer IHRPTR                                                    ! Hour of day
      integer IMINPTR                                                   ! Minute of hour
      integer ISECPTR                                                   ! Second of Minute
      integer IGMTPTR                                                   ! Time zone of header Local(1)/GMT(2)
      integer LDELSPTR                                                  ! Deep water delay in secs.
      integer LSMUSPTR                                                  ! Start of mute in seconds
      integer LEMUSPTR                                                  ! End of mute in seconds
      integer LSISPTR                                                   ! Sample interval in seconds
      integer LWBTSPTR                                                  ! Water bottom time in seconds
      integer LGATPTR                                                   ! No. of traces in stacked data(>0) or End of Gather(<0)
      integer LSSMSPTR                                                  ! Start of surgical mute in secs.
      integer LESMSPTR                                                  ! End of surgical mute in secs
      integer LSBPTR                                                    ! SeaBeam slant range
!
      common /SEGYPTR/ LLSEQPTR, LRSEQPTR, LSHOTPTR, LSHTRPTR, LRPNPTR,
     *                 LRPTRPTR, ITRIDPTR, LDISTPTR, LWBDPTR,  LSXCOPTR,
     *                 LRXCOPTR, IDELMPTR, ISTMPTR,  IENDMPTR, ISAMPPTR,
     *                 ISIPTR,   IYRPTR,   IDAYPTR,  IHRPTR,   IMINPTR,
     *                 ISECPTR,  IGMTPTR,  LDELSPTR, LSMUSPTR, LEMUSPTR,
     *                 LSISPTR,  LWBTSPTR, LGATPTR,  LSSMSPTR, LESMSPTR,
     *                 LSBPTR
C
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
C
!.. Define constants for use with the DISKIO subroutines getfil, frefil,
! podisc
! GETFIL constants
      integer    CREATTMP, CREATNEW
      parameter (CREATTMP   = 1)                                        ! Create a temporary file
      parameter (CREATNEW   = 3)                                        ! Create a new named file
! FREFIL constants
      integer    RLDLCLS
      parameter (RLDLCLS = 3)
! PODISC constants
      integer    POSABS, POSREL, DSKSTRT
      parameter (POSABS     = 1)
      parameter (POSREL     = 2)
      parameter (DSKSTRT    = 0)                                        ! The starting position on the disk
!.. Include file for SplitFK, MrgFK & related processes
!
!
!... Computer Type
      integer PRIME, VAXUNIX, APOLLO, VAXVMS, CRAY
      integer CONVEX, IEEE
      parameter (PRIME   = 1)
      parameter (VAXUNIX = 2)
      parameter (APOLLO  = 3)
      parameter (VAXVMS  = 4)
      parameter (CRAY    = 5)
      parameter (CONVEX  = 6)
      parameter (IEEE    = 7)
!
      parameter ( NOOUTP  = -997)                                       ! No Output process yet seen
!
!.. Parameters for DiskPos Subroutine
      integer   DPINIT, FIRSTHDR, TRCHDR, TRCDATA
      parameter ( DPINIT   = 1)
      parameter ( FIRSTHDR = 2)
      parameter ( TRCHDR   = 3)
      parameter ( TRCDATA  = 4)
!
!.. Entries for ReadTrce
      integer RTINIT, READDATA
      parameter(RTINIT   = 1)
      parameter(READDATA = 2)
!
      parameter ( KSPLTLN = 15)                                         ! Number of parameters in the block
!
      integer   COMPOFS, FMTOFS, DUNITOFS, IDARNOFS, DDATLOFS, CPOSOFS
      integer   LPRNTOFS, IDISKOFS, TRCCTOFS, NXOFS
      integer   SINGOFS, MULTOFS
!..                                   Define offsets to the parameters
      parameter ( COMPOFS = 1)                                          !   Computer type
      parameter (  FMTOFS = 2)                                          !   Input data format
      parameter (DUNITOFS = 3)                                          !   Input data file unit
      parameter (IDARNOFS = 4)                                          !   idarn : misalignment on the Cray
      parameter (DDATLOFS = 5)                                          !   No. of samples per output trace
      parameter ( CPOSOFS = 6)                                          !   Current Position of file pointer
      parameter (LPRNTOFS = 7)                                          !   Lprint debug value
      parameter ( NEWSOFS = 8)                                          !   A new start frequency
      parameter ( NEWEOFS = 9)                                          !   A new finish frequency
      parameter (IDISKOFS = 10)                                         !   The output process number
      parameter (TRCCTOFS = 11)                                         !   The number of transferred trace
      parameter (   NXOFS = 12)                                         !   The total number of output traces
      parameter ( SINGOFS = 13)                                         !   Pointer to header comp that is set to 1
      parameter ( MULTOFS = 14)                                         !   Pointer to header comp that records trace No
!
      integer  OHDROFS
      parameter ( OHDROFS = 15)                                         !  Temporary bugfix to read old header types

C PROGRAM
!
      integer SplParam(111)
!
      common /READT/ iunit, numhdr, numdat, ihunit, ireeln, intrcs,
     *               ifmt, nskip, secs, lrenum, isrcf, idum
!
      real       scr(111)                                                 ! buffer arrays
      integer    lscr(111)
      integer*2  iscr(111)
!
      integer idunit
      integer icompt
      integer TrcHdrL
      logical first
      integer icray(200)
!
      save first, TrcHdrL
!
      data first /.TRUE./
!
      icompt = SplParam(COMPOFS)
      idunit = SplParam(DUNITOFS)
!
      if (first) then
        first = .FALSE.
        if (icompt.ne.CRAY) then                                        ! Set up the lengths of headers
          TrcHdrL = NORMTHDR
        else
          TrcHdrL = CRAYTHDR
        endif
      endif
!
!      nsamps = iscr(ISAMPPTR)                                           ! Get the number of samples per trace
      nsamps = numdat
        if( MOD(nsamps,2).ne.0 )
     $    print *, ' *** WARNING DOEX(sWrHdr) ****',
     $    'Non resampled FK data wasn''t even no of samples'
!
      SplParam(DDATLOFS) = nsamps
!
      if ( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) ) then             ! swap bytes on DEC
          call swap32( lscr(1), 7 )
          call swap16( iscr(15), 1 )
          call swap32( lscr(10), 1 )
          call swap32( lscr(16), 1 )
          call swap32( lscr(19), 1 )
          call swap32( lscr(21), 1 )
          call swap16( iscr(55), 5 )
          call swap16( iscr(79), 6 )
          call swap32( lscr(46), 10 )
      else if ( icompt.eq.CRAY ) then
          do i = 1, numhdr
  550       icray(i) = iscr(i)
         enddo
!
!..                 Move dead trace flag into 3rd integer*2 word (from the left)
          icray(8) = iscr(75) * 65536
          call i82i4( icray, iscr, 60 )                                 ! convert header to 32 bit integer
          call i82i2( icray(113), iscr(14), 8 )
          call i82i2( icray(137), iscr(20), 8 )
      endif
!
      call wrdisc( idunit, scr, TrcHdrL )                               ! write header to disk
!
! Tidy header on VAXES since the input routines might rely on a good header.
      if ( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) ) then             ! swap bytes on DEC
          call swap32( lscr(1), 7 )
          call swap16( iscr(15), 1 )
          call swap32( lscr(10), 1 )
          call swap32( lscr(16), 1 )
          call swap32( lscr(19), 1 )
          call swap32( lscr(21), 1 )
          call swap16( iscr(55), 5 )
          call swap16( iscr(79), 6 )
          call swap32( lscr(46), 10 )
       endif
      return
      end
!
!
      subroutine sWrTrc(SplParam,scr,lscr,iscr)
!-------------------------------------------------------------------------------
!    sWrTrc writes an SEGY format trace data to disk. The output is formatted
! appropriately for the different computers and output data types supported by
! SIOSEIS.
!
! Inputs :
!    SplParam(1) : The relevent control parameters are passed through the
!                  SplParam block. The elements used by sWrHdr are
!    at COMPOFS  :  The computer type
!    at DUNITOFS : The file stream no.
!    at FMTOFS   : The output format of the data
!    at DDATLOFS : The length of the output trace
!
!    scr/lscr/iscr : Equivalenced scratch array.
!
! Call Chain:
!   CONTRO:DOEX:spltfk:sWrTrc
!
! Externals:
!   SWAP16, SWAP32 : DEC byte swap routines
!   I82I4, I82I2   : CRAY integer length conversion routines
!   PR2IBM, DR2IBM, AR2IBM, CRAYIBM, IE2IBM : Convert Host FP to IBM FP
!   WRDISC
!
! Last Modified:
!   10/7/88
!-------------------------------------------------------------------------------
!
C INCLUDE FILES
! This include file include all (?) of the parameters needed to define SIOSEIS
! I/O routines. In particular it defines positions of objects in the SEGY
! headers & constants for these values
!
! EBCDIC HEADER
! Declare Header lengths. These are the length of the EXTERNAL disk image in
! host words.
      integer     CRAYEBC, NORMEBC
      parameter ( CRAYEBC  = 400)
      parameter ( NORMEBC  = 2*CRAYEBC)
!
! BINARY HEADER
      integer     CRAYBIN, NORMBIN
      parameter ( CRAYBIN  =  50)                                       ! length of external disk image
      parameter ( NORMBIN  = 2*CRAYBIN)
!
!  The position of objects in the Binary header
      integer    NTRCPTR,IFMTPTR, IDTYPPTR, INKPTR, ITSIPTR
      integer    ITDELPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
!
!..Constants for SEGY Format (IFMTPTR)
      integer IBMFP, INT16, INT32, HOSTFP
      parameter (IBMFP  = 1)                                            ! IBM floating point
      parameter (INT32  = 2)
      parameter (INT16  = 3)
      parameter (HOSTFP = 5)                                            ! Host FP. In actuality Host FP >= 5
!
!.. Data Domains/Types (IDTYPPTR)
      integer    IDTX, IDFKRCT, IDFKPLR, IDFKPLRU
      parameter (IDTX     = 1)
      parameter (IDFKRCT  = 2)
      parameter (IDFKPLR  = 3)
      parameter (IDFKPLRU = 8)
!
! TRACE HEADER
!.. Declare header length on external disk files. Internal length is given by
! Numhdr in common block READT.
      integer  CRAYTHDR, NORMTHDR
      parameter ( CRAYTHDR =  30)                                       ! Length of external disk image
      parameter ( NORMTHDR = 2*CRAYTHDR)
!
!    The positions of elements of the trace header are defined by elements of
! the common block SEGYPTR. This block is initialized in routine SETPTR. The
! position of elements differs on the CRAY since it does not allow mixing of
! Integer*16 & Integer*32 words but only has Integer*64.
!
      integer LLSEQPTR                                                  ! Trace Sequence No. within line
      integer LRSEQPTR                                                  ! Trace Sequence No. within reel
      integer LSHOTPTR                                                  ! Shot number or Stacked Number
      integer LSHTRPTR                                                  ! Trace number within Shot
      integer LRPNPTR                                                   ! RP or CDP number
      integer LRPTRPTR                                                  ! Trace No. within CDP
      integer ITRIDPTR                                                  ! Trace ID Live(1)/Dead(2)
      integer LDISTPTR                                                  ! Source to Receiver Distance
      integer LWBDPTR                                                   ! Water Bottom depth at source
      integer LSXCOPTR                                                  ! Source X co-ordinate
      integer LRXCOPTR                                                  ! Receiver X co-ordingate
      integer IDELMPTR                                                  ! Deep water delay in ms.
      integer ISTMPTR                                                   ! Start Mute time in ms.
      integer IENDMPTR                                                  !   End Mute time in ms
      integer ISAMPPTR                                                  ! Number of data samples in trace
      integer ISIPTR                                                    ! Sample interval of trace in us
      integer IYRPTR                                                    ! Year data was recorded
      integer IDAYPTR                                                   ! Day of year
      integer IHRPTR                                                    ! Hour of day
      integer IMINPTR                                                   ! Minute of hour
      integer ISECPTR                                                   ! Second of Minute
      integer IGMTPTR                                                   ! Time zone of header Local(1)/GMT(2)
      integer LDELSPTR                                                  ! Deep water delay in secs.
      integer LSMUSPTR                                                  ! Start of mute in seconds
      integer LEMUSPTR                                                  ! End of mute in seconds
      integer LSISPTR                                                   ! Sample interval in seconds
      integer LWBTSPTR                                                  ! Water bottom time in seconds
      integer LGATPTR                                                   ! No. of traces in stacked data(>0) or End of Gather(<0)
      integer LSSMSPTR                                                  ! Start of surgical mute in secs.
      integer LESMSPTR                                                  ! End of surgical mute in secs
      integer LSBPTR                                                    ! SeaBeam slant range
!
      common /SEGYPTR/ LLSEQPTR, LRSEQPTR, LSHOTPTR, LSHTRPTR, LRPNPTR,
     *                 LRPTRPTR, ITRIDPTR, LDISTPTR, LWBDPTR,  LSXCOPTR,
     *                 LRXCOPTR, IDELMPTR, ISTMPTR,  IENDMPTR, ISAMPPTR,
     *                 ISIPTR,   IYRPTR,   IDAYPTR,  IHRPTR,   IMINPTR,
     *                 ISECPTR,  IGMTPTR,  LDELSPTR, LSMUSPTR, LEMUSPTR,
     *                 LSISPTR,  LWBTSPTR, LGATPTR,  LSSMSPTR, LESMSPTR,
     *                 LSBPTR
C
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
C
!.. Define constants for use with the DISKIO subroutines getfil, frefil,
! podisc
! GETFIL constants
      integer    CREATTMP, CREATNEW
      parameter (CREATTMP   = 1)                                        ! Create a temporary file
      parameter (CREATNEW   = 3)                                        ! Create a new named file
! FREFIL constants
      integer    RLDLCLS
      parameter (RLDLCLS = 3)
! PODISC constants
      integer    POSABS, POSREL, DSKSTRT
      parameter (POSABS     = 1)
      parameter (POSREL     = 2)
      parameter (DSKSTRT    = 0)                                        ! The starting position on the disk
!.. Include file for SplitFK, MrgFK & related processes
!
!
!... Computer Type
      integer PRIME, VAXUNIX, APOLLO, VAXVMS, CRAY
      integer CONVEX, IEEE
      parameter (PRIME   = 1)
      parameter (VAXUNIX = 2)
      parameter (APOLLO  = 3)
      parameter (VAXVMS  = 4)
      parameter (CRAY    = 5)
      parameter (CONVEX  = 6)
      parameter (IEEE    = 7)
!
      parameter ( NOOUTP  = -997)                                       ! No Output process yet seen
!
!.. Parameters for DiskPos Subroutine
      integer   DPINIT, FIRSTHDR, TRCHDR, TRCDATA
      parameter ( DPINIT   = 1)
      parameter ( FIRSTHDR = 2)
      parameter ( TRCHDR   = 3)
      parameter ( TRCDATA  = 4)
!
!.. Entries for ReadTrce
      integer RTINIT, READDATA
      parameter(RTINIT   = 1)
      parameter(READDATA = 2)
!
      parameter ( KSPLTLN = 15)                                         ! Number of parameters in the block
!
      integer   COMPOFS, FMTOFS, DUNITOFS, IDARNOFS, DDATLOFS, CPOSOFS
      integer   LPRNTOFS, IDISKOFS, TRCCTOFS, NXOFS
      integer   SINGOFS, MULTOFS
!..                                   Define offsets to the parameters
      parameter ( COMPOFS = 1)                                          !   Computer type
      parameter (  FMTOFS = 2)                                          !   Input data format
      parameter (DUNITOFS = 3)                                          !   Input data file unit
      parameter (IDARNOFS = 4)                                          !   idarn : misalignment on the Cray
      parameter (DDATLOFS = 5)                                          !   No. of samples per output trace
      parameter ( CPOSOFS = 6)                                          !   Current Position of file pointer
      parameter (LPRNTOFS = 7)                                          !   Lprint debug value
      parameter ( NEWSOFS = 8)                                          !   A new start frequency
      parameter ( NEWEOFS = 9)                                          !   A new finish frequency
      parameter (IDISKOFS = 10)                                         !   The output process number
      parameter (TRCCTOFS = 11)                                         !   The number of transferred trace
      parameter (   NXOFS = 12)                                         !   The total number of output traces
      parameter ( SINGOFS = 13)                                         !   Pointer to header comp that is set to 1
      parameter ( MULTOFS = 14)                                         !   Pointer to header comp that records trace No
!
      integer  OHDROFS
      parameter ( OHDROFS = 15)                                         !  Temporary bugfix to read old header types
      parameter ( MAXDO   = 8 )                                         ! The max. no. of output processes.

C PROGRAM
!
      real       scr(1)                                                 ! buffer arrays
      integer   lscr(1)
      integer*2 iscr(1)
      integer SplParam(KSPLTLN, MAXDO)
C
      integer  ofmt
!
      icompt = SplParam(COMPOFS,idisko)
      ofmt   = SplParam(FMTOFS,idisko)
      nsamps = SplParam(DDATLOFS,idisko)
      idunit = SplParam(DUNITOFS,idisko)
!
      if ( ofmt.eq.IBMFP ) then
          if( icompt.eq.PRIME) then
             call pr2ibm( scr, nsamps, scr )
          else if( icompt.eq.VAXUNIX) then
             call dr2ibm( scr, nsamps, scr )
             call swap32(scr,nsamps)
          else if ( icompt.eq.VAXVMS) then
            call dr2ibm( scr, nsamps, scr )
            call swap32(scr,nsamps)
          else if ( icompt.eq.CRAY ) then
             CALL usscti( scr, scr, 1, nsamps, istat )
             nsamps = nsamps / 2
          else if ( icompt.eq.CONVEX) then
             call dr2ibm( scr, nsamps, scr )
          else if( icompt.eq.IEEE .OR. icompt .EQ. APOLLO ) then
             call ie2ibm( scr, nsamps, scr )
          endif
!
      else if( ofmt.eq.INT32) then
          do i = 1, nsamps
  620      lscr(i) = scr(i)
         enddo
          if( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) ) then
            call swap32(scr,nsamps)
          else if( icompt.eq.CRAY ) then
              call i82i4( iscr, iscr, nsamps )
              nsamps = nsamps / 2
          endif
!
      else if( ofmt.eq.INT16) then
          do i = 1, nsamps
  630      iscr(i) = scr(i)
         enddo
          if( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) )
     $       call swap16(scr,nsamps)
!
          if( icompt.ne.CRAY) then
              nsamps = nsamps / 2
          else
              call i82i2( iscr, iscr, nsamps )
              nsamps = nsamps / 4
          endif
      endif
!
      nwrds = nsamps
      call wrdisc( idunit, scr, nwrds )
      RETURN
      end
