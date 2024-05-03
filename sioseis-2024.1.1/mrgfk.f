      subroutine mrgFK(entry,buf,lbuf,ibuf,scr,lscr,iscr,istop,
     $                 idunit,iptype)
!------------------------------------------------------------------------------
!    MRGFK is responsible for reformatting an input FK file if necessary
! The circumstance in which this is necessary are
!
!   (i) The file is in 'User format' i.e. a series of traces that have
!       wavenumbers running from -Nyq -> 0 -> +Nyq and frequencies running
!       from 0 -> +Nyq. These traces are formatted in polar coordinates with
!       amplitudes followed by phases. This is the most useful format for
!       display. User format has an IDtype = idFKplrU (5) set in the
!       binary header.
!
!  (ii) The user is going to do fk filtering or fk migration on the data
!       These processes require the data be in internal format with
!       traces having wavenumbers in 0 -> +nyq and frequencies between
!       -nyq -> 0 -> +nyq.
!
!   To maintain maximal independance from diex ( the usual disk input) MRGFK is
! called at appropriate points from DIEX and handles the majority of the disk
! I/O functions in this case
!
! Inputs :
!    entry : The type of entry. Constants are declared in mrggbl
!          = CHCKBIN : This is a temporary bug fix that checks the binary header
!                      of the input file to see whether it is new format, 50 4
!                      words or the old format 100 words & adjusts reading
!                      accordingly
!          = TRNSPARM : Transfers a copy of the relevant DIEX parameters to
!                       local storage. Consistency checks are performed at
!                       MRGINIT
!           = MRGINIT : Initialize routine. It copies parts of the binary header
!                       & checks input parameters for consistency.
!           = MRGTRC  : Merges two split traces into one &  returns it to DIEX
!
!   istop  : The stop flag.
!          = NOSTOP  Usual condition. A trace has been returned.
!          = STOPTRC The last trace of the dataset has been read/merged.
!
!   idunit : The file stream for the input data
!   scr/lscr/iscr : Equivalenced scratch arrays.
!
! Outputs :
!    iptype : This is set to our input type (IPTFKU) if the data is to be
!             converted. This is the signal to DIEX to call mrgFK for data.
!
! buf/lbuf/ibuf : The equivalenced arrays containing the data trace
!
! Call Chain:
!   CONTRO:DIEX:MRGFK
!
! Externals
!   sRdHdr, sRdTrc : Local routines
!   EXIT
!   ChkBin, DskPos,
!   ChkPrc : Check for an FK process in procs list
!
! Modifications:
!   12/21/88 ajh.  Added presetting of lno if not given by user
!    3/14/89 ajh.  Changed external name lengths to be 6 letters
!                  Add blank as first character of printed output
! Last Modified
!    3/23/89 ajh Changed wrong ibase1 assignment
!------------------------------------------------------------------------------
!
! INCLUDE FILES
!.. Include file defining constants of use globally throughout SIOSEIS
!
! Modified 11/11/88     added DATINAP
! 8 Apr 09 - Use common numdat rather than segy header word ISAMPPTR
!
! Define the sizes of some Large common blocks

      integer SZAPMEM, SZTRANP
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (SZTRANP = 262144)                                      ! Size of /TRANP/: Transpose array at 512x512
! parameter definitions for the stop signal istop
      integer STOPNOTR, STOPTR, NOSTOP
      parameter (STOPNOTR  = -1)                                        ! last call but no trace passed
      parameter (STOPTR    =  1)                                        ! last call and a trace passed
      parameter (NOSTOP    =  0)                                        ! normal call to the routine
!
! parameter for AP
      integer CLEARAP, USEAP, NOAP
      integer DATINAP
      parameter (USEAP    = 1)                                          ! There is an AP available
      parameter (NOAP     = 0)                                          ! No AP available
      parameter (CLEARAP  = 0)                                          ! Get data out of AP simulator
      parameter (DATINAP   = 1)                                         ! Data is in the AP
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
      integer    ITDELPTR, IVERSPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
      parameter (IVERSPTR = 35)                                         ! The SIOSEIS version number
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
!.. Version Number
      integer  CURVERS
      parameter( CURVERS = 210)
!
! TRACE HEADER
!.. Declare header length on external disk files. Internal length is given by
! Numhdr in common block READT.
      integer  CRAYTHDR, NORMTHDR
      parameter ( CRAYTHDR =  30)                                       ! Length of external disk image
      parameter ( NORMTHDR = 2*CRAYTHDR)
!
      common /SEGYPTR/ LLSEQPTR, LRSEQPTR, LSHOTPTR, LSHTRPTR, LRPNPTR,
     *                 LRPTRPTR, ITRIDPTR, LDISTPTR, LWBDPTR,  LSXCOPTR,
     *                 LRXCOPTR, IDELMPTR, ISTMPTR,  IENDMPTR, ISAMPPTR,
     *                 ISIPTR,   IYRPTR,   IDAYPTR,  IHRPTR,   IMINPTR,
     *                 ISECPTR,  IGMTPTR,  LDELSPTR, LSMUSPTR, LEMUSPTR,
     *                 LSISPTR,  LWBTSPTR, LGATPTR,  LSSMSPTR, LESMSPTR,
     *                 LSBPTR
!
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
!
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
! global include file for mrgFK to be added to diex.ftn
      integer TRNSPARM,MRGINIT,MRGTRC
      parameter (TRNSPARM = 1)
      parameter (MRGINIT  = 2)
      parameter (MRGTRC   = 3)
      integer IPTFKU                                                    ! Special data type for FK User type
      parameter (IPTFKU = 4)
!.. Bug fix for old disk file
      integer CHCKBIN
      parameter(CHCKBIN = 5)
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

! PROGRAM
!
      integer   entry
      real       buf(1111), scr(1111)                                         ! buffer arrays
      integer   lbuf(1111), lscr(1111)
      integer*2 ibuf(1111), iscr(1111)
      integer   idunit, iptype
!
      common /EDITS/ ierror, iwarn, irun, now, icompt
      common /READT/ itunit, numhdr, TrcDatl, ihunit, ireeln, intrcs,
     *               ifmtx
!
      integer MrgParam(15)                                              ! The parameter control block for subroutines
      integer outnum                                                    ! Total number of traces that will be output
      integer otrcs                                                     ! Index of the output traces
!
      integer fno, lno, ftr, ltr
      integer trace1, trace2                                            ! The traces no.s to be merged
      integer base1, base2, base3, base4                                ! Standard offsets in data trace
      integer base2p1, base3p1, base4p1
!
      integer top
      real    NegNyqf
      integer pow2                                                      ! External function
      logical chkprc                                                    ! External function
!
      logical binchck                                                   ! Bug fix for old binary header length
!
      save
!
      data otrcs / 0 /
      data binchck /.false./
!
      if (entry.eq.CHCKBIN) then
          binchck = .true.
          MrgParam(COMPOFS) = icompt
          MrgParam(FMTOFS)  = iscr(IFMTPTR)
          call ChkBin(MrgParam,buf,lbuf,ibuf,scr,lscr,iscr)
          RETURN
      endif
!
!**************************                           **************************
!          Copy the relevant elements of the Input Parameter list
!**************************                           **************************
!
      if ( entry.eq.TRNSPARM) then
          lprint             = lscr(26)
          fno                = lscr(27)
          lno                = lscr(28)
          ftr                = lscr(30)
          ltr                = lscr(31)
          MrgParam(LPRNTOFS) = lprint
          MrgParam(DUNITOFS) = idunit
          RETURN
      endif
!
!**************************                           **************************
!        Make a copy of the binary Header variables & Check consistency
!**************************                           **************************
!
      if ( entry.eq.MRGINIT) then
        if(iscr(IDTYPPTR).ne.IDFKPLRU)
     *    RETURN                                                        ! Only interested in user polar cordinate data
!
        if (chkprc()) then                                              ! FK process. Setup to reformat data
          iptype = IPTFKU                                               ! Change data type to ensure call back
!
          intrcs = iscr(NTRCPTR)                                        ! Copy binary header variables
          ifmt   = iscr(IFMTPTR)
          inpnum = iscr(INKPTR)                                         ! Number of traces in the input file
          NegNyqf = -0.5E4/iscr(ITSIPTR)                                ! Find -Nyq freq from delt (dHz)
!
          if(inpnum.ne.2**pow2(inpnum-1)+1) then
            print '(2A)',' **** DIEX(mrgfk) ERROR ****',
     $             ' FK data file doesnt have 2**n + 1 traces'
            STOP
          endif
!
          nx     = inpnum/2
          outnum = nx + 1                                               ! Number of output traces
          iscr(INKPTR) = outnum
!
!                 Check no. of traces requested by the user matches that in file
          if(intrcs.eq.1) then                                          ! traces are indexed by shot/rp no.
!
            if ( lno .EQ. 0 ) lno = inpnum                              ! preset lno to the last trace
            if ( (lno.ne.inpnum).AND.(lno.ne.outnum)) then
              print '(2A)',' **** DIEX(mrgfk) ERROR ****'
     $        ,' Requested Shot/RP range doesn''t match size of FK file'
              STOP
            endif
          else                                                          ! Indexed by traces
            if (ltr.eq.0) then
              ftr = 1
              ltr = inpnum
            endif
            if( (ftr.ne.1).OR.
     $         ((ltr.ne.inpnum).AND.(ltr.ne.outnum))) then
              print '(2A)',' **** DIEX(mrgfk) ERROR ****',
     $        'Requested Trace range doesn''t match size of FK file'
              STOP
            endif
          endif                                                         ! of if intrcs.eq.1
!                                                SetUp parameters in MrgParam
          MrgParam(COMPOFS)  = icompt
          MrgParam(FMTOFS)   = ifmt
          MrgParam(DUNITOFS) = idunit
          MrgParam(LPRNTOFS) = lprint
          if (.not.BINCHCK) MrgParam(OHDROFS) = 0                       ! Binary check on Apollo only
!
          call DskPos(DPINIT,0,MrgParam)                                ! Read the first trace header
          call DskPos(FIRSTHDR,1,MrgParam)
          call sRdHdr(MrgParam,buf,lbuf,ibuf,iscr)
!          MrgParam(DDATLOFS) = ibuf(ISAMPPTR)                           ! No. of samples in input trace
          MrgParam(DDATLOFS) = numdat                           ! No. of samples in input trace
!          nt      = ibuf(ISAMPPTR) - 2                                  ! No. of samples in merged trace
          nt = numdat -2
          TrcDatl  = 2 * nt
          nby2    = nt/2
!
          if(nt.ne.2**pow2(nt)) then
            print '(2A)',' **** DIEX(mrgfk) ERROR ****',
     $            ' FK data trace length doesnt have 2**n + 2 samples'
            STOP
          endif
!
          call DskPos(DPINIT,1,MrgParam)                                ! Initialize with trace length
          call sRdTrc(RTINIT,MrgParam,buf,lbuf,ibuf,scr,iscr)
!
!               Set up base indexes for use when merging & rearranging the data
!               These are basically at nby2 = [0,Nyq] intervals.
!            But adjustments are needed because the input traces are 2*nby2 + 2
!
          base1   = numhdr + nby2
          base2   = base1  + nby2
          base3   = base2  + nby2
          base4   = base3  + nby2
          base2p1 = base2 + 1
          base3p1 = base3 + 1
          base4p1 = base4 + 1
        endif                                                           ! of if found
        RETURN                                                          ! of entry = MRGINIT
!
!**************************                           **************************
!                        Returned a Merged trace
!**************************                           **************************
!
      else if (entry.eq.MRGTRC) then
        if(otrcs.eq.0) then                                             ! *** Initialization ( 1st trace ) ***
          trace1 = nx + 1
          istop  = NOSTOP
        else if (otrcs.eq.nx) then
          trace1 = 2*nx + 1
          istop  = STOPTR
        else
          trace1 =  nx + 1 - otrcs
          trace2 =  nx + 1 + otrcs
          istop  = NOSTOP
        endif
!
        call DskPos(TRCHDR,trace1,MrgParam)
        call sRdHdr(MrgParam,buf,lbuf,ibuf,iscr)
!
!..                                              Reformat header of merged trace
        ibuf(ISAMPPTR) = TrcDatl                                        ! New trace length
        numdat = TrcDatl
        ibuf(IDELMPTR) = nint(NegNyqf * 1000.)                          ! Record New Start Time
         buf(LDELSPTR) = NegNyqf
        if (lbuf(LRPTRPTR).eq.0) then                                   ! Indexing by shot no.
           if (intrcs.eq.1) then
             lbuf(LSHOTPTR) = otrcs + 1                                 ! Adjust shot no.
             lbuf(LSHTRPTR) = 1
           else
             lbuf(LSHOTPTR) = 1
             lbuf(LSHTRPTR) = otrcs + 1                                 ! Adjust shot trace no.
           endif
         else                                                           ! Indexing by RP no.
           if (intrcs.eq.1) then
             lbuf(LRPNPTR) = otrcs + 1                                  ! Adjust RP no.
             lbuf(LRPTRPTR) = 1
           else
             lbuf(LRPNPTR) = 1
             lbuf(LRPTRPTR) = otrcs + 1                                 ! Adjust RP trace no.
           endif
         endif
!
!..                                         *** Read data and reformat trace ***
!
        if ( (otrcs.eq.0).OR.(otrcs.eq.nx) ) then
          ibase1 = 2* base1
          call DskPos(TRCDATA,trace1,MrgParam)
          call sRdTrc(READDATA,MrgParam, buf(base1+1), lbuf(base1+1),
     $                 ibuf(ibase1+1), scr, iscr)
          top = base2 + 2
          do i = 1, nby2                                             ! Derive -ve amps from +ve ones
   30       buf(numhdr+i) = buf(top - i)
          enddo
!
          buf(base2p1) = buf(base3 + 2)                                 !  Move +Nyq to -Nyq
          do i = 1, nby2                                             ! Move +ve phases into position
   40       buf(base4p1 - i) = buf(base3 + 2 - i)
          enddo
!
          do i = 1, nby2-1                                           ! Derive -ve phases from +ve ones
   50       buf(base2p1+i) = -buf(base4p1 - i)                          ! Remember to negate !
          enddo
!
!
!                                                 *** Reformat General Trace ***
       else
         ibase2 = 2 * base2
         call DskPos(TRCDATA,trace1,MrgParam)                           ! Read -ve frequencies
         call sRdTrc(READDATA,MrgParam,buf(base2+1), lbuf(base2+1),
     $                ibuf(ibase2+1), scr, iscr)
!
!..                                 Reverse & shift the -ve amps  ( -Nyq -> -1 )
         do i = 1, nby2
  100      buf(numhdr+i) = buf(base3 + 2 - i)
         enddo
!                                                                       ! Read +ve frequencies
         ibase1 = 2*base1
         call DskPos( TRCDATA, trace2, MrgParam)
         call sRdTrc( READDATA,MrgParam, buf(base1+1), lbuf(base1+1),
     $                ibuf(ibase1+1), scr, iscr)
!
!..                              +ve amplitude are in position but the -ve phase
!..                                components are reversed & lie above +ve ones
         do i = 1, nby2
  110       scr(i) = buf(base2p1 + i)
         enddo
!                                                                       ! Reverse & shift -ve components
         top = base4 + 3
         do i = 1, nby2
  120      buf(base2+i) = buf(top - i)
         enddo
!
         do i = 1, nby2
  130      buf(base3+i) = scr(i)
         enddo
        endif                                                           ! of if otrcs == 0 | otrcs == nx
!
        otrcs = otrcs + 1
      endif                                                             ! of if entry
      RETURN
      end
!
!
      subroutine sRdHdr(MrgParam,buf,lbuf,ibuf,iscr)
!------------------------------------------------------------------------------
!   sRdHdr reads a SEGY trace header from disk & take care of format conversion
! for the machines supported by SIOSEIS.
!
! Inputs :
!   MrgParam  : The majority of inputs are passed through the MrgParam array.
!               The elements used by sRdHdr are
! at FMTOFS   : The input format of the data
! at LPRNTOFS : The value of the debug print option
! at IDARNOFS : The misalignment parameter. This indicates how many 16 bit words
!               away we are from a 64 bit word alignment on the CRAY
! at DUNITOFS : The file stream of the input data.
!
!        iscr : Scratch Array
!
! Outputs
!   buf/lbuf/ibuf. Equivalenced array containing header starting at position 1
!
! Call Chain
!   CONTRO:DIEX:MRGFK:sRdHdr
!
! Externals:
!   SWAP16, SWAP32 : DEC byte swap routines
!  I22I8, I42I8
!   RDDISC, EXIT
!
! Last Modified
!   10/7/88 ajh.
!------------------------------------------------------------------------------
!
! INCLUDE FILES
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
      integer    ITDELPTR, IVERSPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
      parameter (IVERSPTR = 35)                                         ! The SIOSEIS version number
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
!.. Version Number
      integer  CURVERS
      parameter( CURVERS = 210)
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
!
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
!
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
! PROGRAM
!
      integer MrgParam(15)
      real       buf(1111)                                                 ! buffer arrays
      integer   lbuf(1111)
      integer*2 ibuf(1111),iscr(1111)
!
      integer   icompt
      integer   idarn
      integer    idunit
!
      logical first
      integer EBChdrL, BinHdrL, TrcHdrL
!
      save first
      save icompt, idarn, idunit, lprint, ifmt
      save EBChdrL, BinHdrL, TrcHdrL
!
      data first / .TRUE./
!
      if ( first ) then                                                 ! First call to routine ?
        first = .not.first
        icompt = MrgParam(COMPOFS)
!
        if (icompt.ne.CRAY) then                                        ! Set up the lengths for headers
          EBChdrL = NORMEBC                                             ! Length in 4 byte words
          BinHdrL = NORMBIN
          TrcHdrL = NORMTHDR
        else
          EBChdrL = CRAYEBC                                             ! Length in 8 byte CRAY words
          BinHdrL = CRAYBIN
          TrcHdrL = CRAYTHDR
        endif
      endif                                                             ! of if First
!
      ifmt   = MrgParam(FMTOFS)
      lprint = MrgParam(LPRNTOFS)
      idarn  = MrgParam(IDARNOFS)
      idunit = MrgParam(DUNITOFS)
!
      if ( (icompt .eq. CRAY).AND.(idarn.ne.0)) then
        n = TrcHdrL + 1
      else
        n = TrcHdrL
      endif
!
      call rddisc( idunit, buf, n, istat )                              ! read the SEGY trace header
      if ( istat .ne. n) then
         print 500
         print '(2(A,I5))','RDDISC returned ',istat,' not requested ', n
         STOP
      endif
!
      if( icompt .eq. VAXUNIX .OR. icompt .eq. VAXVMS ) then
          call swap32( lbuf(1), 7 )
          call swap16( ibuf(15), 1 )
          call swap32( lbuf(10), 1 )
          call swap32( lbuf(16), 1 )
          call swap32( lbuf(19), 1 )
          call swap32( lbuf(21), 1 )
          call swap16( ibuf(55), 5 )
          call swap16( ibuf(79), 6 )
          call swap16( ibuf(110), 1 )
!
      else if ( icompt .eq. CRAY ) then
          call i22i8( ibuf, iscr, 100-idarn)
          if ( idarn.ne.0 ) then
              do i = 100, 3, -1
  250            iscr(i) = iscr(i-idarn)
              enddo
          endif
!
          do i = 55, 59                                             ! Move 16 bit integer header stuff
  260        ibuf(NORMTHDR + i) = iscr(i)
          enddo
          ibuf(75) = iscr(15)
          do i = 79, 84
  270        ibuf(NORMTHDR + i) = iscr(i)
          enddo
!
          call i42i8( ibuf, iscr, NORMTHDR - idarn )
          if ( idarn.ne.0 ) then
              do i = NORMTHDR, 2, -1
  280           iscr(i) = iscr(i-idarn/2)                               ! we're off by 1 32 bit word
              enddo
          endif
!
          do i = 1, 7
  290       ibuf(i) = iscr(i)
          enddo
!
          ibuf(10) = iscr(10)
          ibuf(16) = iscr(16)
          ibuf(19) = iscr(19)
          ibuf(21) = iscr(21)
          ibuf(51) = iscr(51)
      endif
      RETURN
 500  format (' **** ERROR DIEX(sRdHdr) ****')
      end
!
!
      subroutine sRdTrc(entry,MrgParam,buf,lbuf,ibuf,scr,iscr)
!------------------------------------------------------------------------------
!   sRdTrc reads the data part of an SEGY trace & reformats it appropriatley
! depending on input type and computer.
!
! Inputs :
!   entry    : The type of entry into sRdTrc
!            = RTINIT : Initialization. This just makes local copies of
!                     relevant Parameters & works out trace length
!            = READDATA : Read the trace data & reformat. The disk is positioned
!                      Correctly prior to entry. ( Except for possible alignment
!                      Problems on the CRAY
!.
!  MrgParam   : The majority of inputs are passed through the MrgParam array.
!               The elements used by sRdTrc are
! at COMPOFS  : The computer type
! at FMTOFS   : The input format of the data
! at LPRNTOFS : The value of the debug print option
! at DDATLOFS : The trace data length
! at IDARNOFS : The misalignment parameter. This indicates how many 16 bit words
!               away we are from a 64 bit word alignment on the CRAY
! at DUNITOFS : The file stream of the input data.
!
!   scr/iscr  : Equivalenced Scratch arrays.
!
! Outputs:
!    buf/lbuf/ibuf : The equivalenced arrays through which the data is returned
!                    Data is returned starting at element 1. Thus the routine
!                    should be called with an appropriate offset into these
!                    arrays
!
! Call Chain:
!   CONTRO:DIEX:MRGFK:sRdTrc
!
! Externals:
!   SWAP16, SWAP32       : DEC byte swap routines
!  I22I8, I42I8, IBMCRAY : Convertion routines for CRAY
!  IBMFP                 : Convertio routine for Host FP
!   RDDISC, EXIT
!
! Last Modified
!   3/23/89: VAX and IBM handling was incorrect because of numhdr offsets
!            VAX byte swapping is incorrect because didn't handle 16 & 32
!            bit words indepedently. Changed references to numdat to use
!            TrcDatl.
!------------------------------------------------------------------------------
! INCLUDE FILES
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
      integer    ITDELPTR, IVERSPTR
      parameter (NTRCPTR  =  7)                                         ! No of traces / gather
      parameter (IFMTPTR  = 13)                                         ! The SEGY Format of the data
      parameter (IDTYPPTR = 31)                                         ! The Domain/ID of the data e.q. T-X data
      parameter (INKPTR   = 32)                                         ! The No. of wavenumbers in an f-k dataset
      parameter (ITSIPTR  = 33)                                         ! Record of tx sample interval in us
      parameter (ITDELPTR = 34)                                         ! Record of tx time delay in ms.
      parameter (IVERSPTR = 35)                                         ! The SIOSEIS version number
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
!.. Version Number
      integer  CURVERS
      parameter( CURVERS = 210)
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
!
!.. Define Constants for Trace Header
!
!.. Constants for Trace ID (ITRIDPTR)
      integer    LIVETR, DEADTR                                         ! Parameters for trace Type
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
!
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
! PROGRAM
!
      integer   entry
      integer   MrgParam(15)
      real       buf(111), scr(111)                                         ! buffer arrays
      integer   lbuf(111)
      integer*2 ibuf(111), iscr(111)
!
      common /READT/ itunit, numhdr, TrcDatl, ihunit, ireeln, intrcs,
     *               ifmtx
      common /SIOAP/ iasgnd,irelse,in,iout,nextad,lapsiz,ifree,iuseap
!
      integer TrcDatL                                                   ! Length of trace data
!      integer TrRem                                                     ! The remainder of the trace modulo 8 byte CRAY words
!                                                                       ! It is either 16 bit or 32 bit words as appropriate
      integer nwrds                                                     ! Length of trace in local word length
!
      save nwrds
      save lprint, icompt, ifmt
!
      if (entry.eq.RTINIT) then                                         ! <<<< Initialization >>>>
        icompt  = MrgParam(COMPOFS)
        ifmt    = MrgParam(FMTOFS)
        lprint  = MrgParam(LPRNTOFS)
        TrcDatL = MrgParam(DDATLOFS)
        nwrds   = TrcDatL
        if (ifmt.eq.INT16)
     $    nwrds = TrcDatL / 2
        if (icompt.eq.CRAY.AND.ifmt.ne.HOSTFP)
     $      nwrds = TrcDatL / 2                                         ! Length in 8 byte words
        RETURN
!
      else if (entry.eq.READDATA) then                                  ! <<<<   Read Data    >>>>
         idarn  = MrgParam(IDARNOFS)
         idunit = MrgParam(DUNITOFS)
!
        if ( (icompt.eq.CRAY).AND.(idarn.ne.0) ) then
          nread = nwrds + 1
        else
          nread = nwrds
        endif
!
        call rddisc( idunit, lbuf(1), nread, istat )
        if (istat .ne. nread ) then
          print *,' ***  ERROR DIEX(sRdTrc) ***'
          print *,' Return Status ', istat,' Requested data length ',
     $             nread
          STOP
        endif
!
!..                                                                     ! Reformat the data
        if ( icompt.eq.CRAY ) then
!
          if (ifmt.eq.HOSTFP) then
            continue
          else if ( ifmt.eq.IBMFP ) then
            ioffset = idarn / 2
            CALL ussctc( lbuf(numhdr+1), 1, scr, trcdatl+idarn )
            do i = 1, TrcDatl
  350         buf(i) = scr(i + ioffset)
            enddo
          else
            if ( ifmt.eq.INT32 ) then
              ioffset = idarn / 2
              call i42i8( lbuf(1), iscr, TrcDatl )
            else if ( ifmt.eq.INT16 ) then
              ioffset = idarn
              call i22i8( lbuf(1), iscr, TrcDatl )
            endif
            do i = 1, TrcDatl
  360         buf(i) = iscr(i + ioffset)                                ! Float the integer
            enddo
         endif
!
       else                                                             ! Non Cray
!
         if ( ifmt.eq.IBMFP) then
           if ( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) )
     $        call swap32( buf, TrcDatl )
              IF( iuseap .EQ. 0 ) THEN
                  in = 0
                  call ibm2fp( buf(1), TrcDatl, buf(1) )
               ELSE
                   in = 1                                               ! what happens here ?
                   call inap( buf, TrcDatl )                            ! assign the ap
                   call apput( buf(1), in, TrcDatl, 3 )
              endif
         else if ( ifmt.eq.INT32) then
           if ( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) )
     $        call swap32( buf, TrcDatl )
              in = 0
              do i = 1, TrcDatl
  500           buf(i) = lbuf(i)
              enddo
         else if ( ifmt.eq.INT16 ) then
           if ( (icompt.eq.VAXUNIX).OR.(icompt.eq.VAXVMS) )
     $        call swap16( buf, TrcDatl )
              in = 0
              do i = 1, TrcDatl
  600           scr(i) = ibuf(i)
              enddo
              do i = 1, TrcDatl
  610            buf(i) = scr(i)
              enddo
         endif
!
       endif                                                            ! if Cray
      endif                                                             ! if entry
      return
      end

