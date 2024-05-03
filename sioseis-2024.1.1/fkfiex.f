      subroutine fkfiex( buf, cbuf, ibuf, scr, lscr, iscr )
!-------------------------------------------------------------------------------
!
!  FKFIEX is the execution phase of the SIOSEIS seismic process FKFILT.
!  FKFILT performs filtering in the FK domain. See subroutine FKFIED for the
!  user description.
!
!     The input traces are supplied with wavenumbers varying from k = 0 to +Nyq
!  Each trace contains frequencies from -Nyq to + Nyq. This arrangement is the
!  one supplied by the 2D FFT and is convenient for the F-K migration.
!  However it differs from the conceptual model of the user which has
!  wavenumbers running from -Nyq -> 0 -> Nyq and only positive frequencies.
!
!    This setup is convenient for velocities filtering since lines are sorted
!  by FKFIED into increasing order, but care must be taken to add appropriate
!  velocity lines near v = 0 if the user wants different velocity filters in
!  0+ & 0-. Care is also need to ensure proper continuity of filter across
!  v (inf).
!
!     Conversely filters defined in terms of dips are consistent with the
!  conceptual model, since they vary smoothly from -inf -> 0 -> +inf, but are a
!  odds with the way the traces are given to FKFIEX. Thus the input dip lines
!  are converted to velocities as part of the input process.
!
! Inputs:
!    buf/cbuf/ibuf  : Equivalenced set of arrays holding the trace (header+data)
!    scr/lscr/iscr) : Equivalence set of scratch arrays.
!
! Outputs:
!    buf/cbuf/ibuf  : Filtered version of trace
!
! Call Chain:
!    CONTRO:FKFIEX
!
! Externals:
!   ClcWpt, FrmWnd                                                      ! Local fkfiex subroutines
!   INAP, RLSEAP EXIT
!   PODISC, RDDISC                                                      ! disk i/o
!
! Mdofications
!      10/12/88 ajh.  ( SZAPMEM )
! Last Modified
!      12/20/88 ajh   Altered Logic of line addition at v = 0
!                     Changed definition of vzerop
!                     Added more Debug info
!      3/16/89 ajh  Added read of binary header info thru common /READT/
!     7 Nov 91 pch  Add reinitialization stuff for prestack.
! 8 Apr 09 - Use common numdat rather than segy header word ISAMPPTR
! 27 Sept 10 - Don't cal FrmWnd if cut/pass goes across 0


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
! fkfilt.inc
! Include file for the FK filter routines FKFIED & FKFIEX
!
      integer    NSETSP, DBLSETSP
      parameter (NSETSP = 20 )                                          ! The max. no. of FKFIED input values
      parameter (DBLSETSP  = 2*NSETSP)
!
      integer    SINGPARM                                               ! The number of single valued params
      parameter( SINGPARM = 6)
!
      integer     FILNOTST , FILTERR, FILTVEL, FILTDIP
      parameter ( FILNOTST  = -8)                                       ! Declare constants for the filter types
      parameter ( FILTERR   = -7)
      parameter ( FILTVEL   =  1)
      parameter ( FILTDIP   =  2)
!
      integer    CUT, PASS, ATINF
      parameter (CUT = 2, PASS = 1)
      parameter ( ATINF = 2)                                            ! Constant signifying lines near v = inf
!
      Integer PCdir, CPdir, PCinf, CPinf
      Parameter (PCdir =  Cut - Pass)
      Parameter (CPdir = Pass - Cut)
      Parameter (PCinf = AtInf * PCdir)
      Parameter (CPinf = AtInf * CPdir)
!
      Integer INITWDW
      Parameter (INITWDW = 32)
      Parameter (NOINIT  = 0)
!
!..                         Declare constants for the available window types
      integer     NUMWINDT
      parameter (NUMWINDT = 8)                                          ! The number of available window types
      Integer Hamming, Hanning, Gaussian, Bartlett, Rectang
      Integer Blackman, ExctBlck, BlckHarr
      Parameter (Hamming  = 1)
      Parameter (Hanning  = 2)
      Parameter (Gaussian = 3)
      Parameter (Bartlett = 4)                                          ! Triangular Window
      Parameter (RectAng  = 5)
      Parameter (BlackMan = 6)
      Parameter (ExctBlck = 7)                                          ! Exact Blackman
      Parameter (BlckHarr = 8)
!
!..                              Parameters for the window options
      Integer    NUMWOPT
      parameter (NUMWOPT = 3)                                           ! The number of windowing options
      Integer  BYA, BYK, BYW
      Parameter (ByA = 1)
      Parameter (ByK = 2)
      Parameter (ByW = 3)
! PROGRAM
!
      dimension buf(111),scr(111),lscr(111)
      integer*2 ibuf(111), iscr(111)
      complex   cbuf(111)
!
      common /FKFILT/ munit, nlists, kTrace
      common /SIOAP/ iasgnd,irelse,in,iout,nextad,lapsiz,ifree,iuseap
      common /APMEM/ ap(SZAPMEM)
      common /READT/lun,numhdr,numdat,iunhdr,ireeln,intrcs,ifmt,nskip,
     *               secs,lrenum,isrcf,idtype,
     *               nfskip, jform, itxsi, itxdel, nfktrc
!
      real    vLine(DBLSETSP + 2)
      integer vType(DBLSETSP + 2), wPt(DBLSETSP + 2)
      integer FiltTyp, wBase, Offs
      integer BasAddr0, BasAddr1
      integer Taper
      logical first, Found
!
      save
!
      data first/.TRUE./, kTrace/-1/
!
      if (first) then
          first = .FALSE.
          if ( (idtype.ne.IDFKRCT).AND.(idtype.ne.IDFKPLR).AND.
     *        (idtype.ne.IDFKPLRU) ) then
             print *,' ***  FKFILT ERROR  ***  The input must be ',
     *               ' in the FK domain.'
              STOP
          endif
!
          call podisc( munit, POSABS, DSKSTRT)                          ! Get single parameters
          call rddisc( munit, scr, SINGPARM, istat )
!
          dx      = scr(1)                                              ! trace spacing
          iwindo  = lscr(2)                                             ! type of window function
          lprint  = lscr(3)                                             ! debug print option
          iwopt   = lscr(4)                                             ! manner in which to apply window
          FiltTyp = lscr(5)                                             ! Type of filter
          nLine   = lscr(6)                                             ! Number of filter lines
!
!         call podisc( iunhdr, POSABS, NORMEBC+DSKSTRT )
!         call rddisc( iunhdr, scr, NORMBIN, istat )                    ! read Binary Header
          nk     = 2*nfktrc - 1                                         ! The no. of wavenumbers
          dt     = itxsi / 1.0E6                                        ! Sample interval in secs
          if (IAND(lprint,2).ne.0) then
            print *,'FKFIEX Initialization info'
            print *,'dx: ', dx,' nk: ', nk,' dt: ', dt
            print *,'No. of Filter Lines (nLine): ', nLine
          endif
!
!..                                   Calculate the constants
!          nsamps = ibuf(ISAMPPTR)                                       ! The no. of 4 byte samples
          nsamps = numdat
!          nw     = ibuf(ISAMPPTR) / 2                                   ! The no. of frequencies
          nw = numdat / 2
          wBase  = nw / 2 + 1                                           ! The 0 frequency position
          If (FiltTyp.eq.FILTDIP) dx = 0.001                            ! Includes allowance for mS.
          vnorm  = float(nk) * dx / dt / float(nw)                      ! Non dimensioning velocity
          pnorm  = 1. / vnorm
          vinfp  = float(nw)                                            ! Velocity  ~ Inf
          vzerop = 1./ float(nk)                                        ! Velocity  = 0+
!
          BasAddr0  = DBLSETSP + 1                                      ! Set base address in scr for type info
          BasAddr1  = BasAddr0 + 1
!
          call rddisc( munit, scr, DBLSETSP, istat )                    ! Read in filter lines
          call rddisc( munit, lscr(BasAddr1), DBLSETSP, istat )         ! & cut/pass
!
          found   = .False.                                             ! Look for filter lines stradling zero
          i       = 1
  5       continue
            i     = i + 1
            if ((scr(i) * scr(i-1) ) .le. 0.) then
              found = .true.
              izm   = i - 1                                             ! Index of  last line <  0
              izp   = i                                                 ! Index of first line >= 0
            endif
          if ( (.not.Found).AND.(i.lt.nLine) ) go to 5
!
          if (.not.Found) then                                          ! Filter Line do not straddle zero
            if(scr(1).lt.0.) then
              izm = nLine                                               ! -ve Lines only
              izp = nLine + 1
              lscr(BasAddr0+izp) = lscr(BasAddr0+1)
            else                                                        ! +ve Lines only
              izm = 0
              izp = 1
              lscr(BasAddr0+izm) = lscr(BasAddr0+nline)
            endif
          endif
!...            *** Input lines were defined as dips : Convert to velocities ***
          If (FiltTyp.eq.FILTDIP) then
            j          = izp
            do 10 i = 1, izm                                            ! Convert -ve Dips
              j        = j - 1                                          ! Reverse index to preserve ordering
              vLine(j) = pnorm / scr(i)
              vType(j) = lscr(BasAddr0 + i)
  10        continue
!
!..                 Filter lines differ across v = 0 so add lines at v = 0+ & 0-
            if ( lscr(BasAddr0+1).ne.lscr(BasAddr0+nLine) ) then
              vLine(izm+1) = -vzerop
              vLine(izm+2) =  vzerop
              vType(izm+1) = lscr(BasAddr0 + 1)
              vType(izm+2) = lscr(BasAddr0 + nLine)
              ntop         = nLine + 2
              izm          = izm   + 2
            else
              ntop         = nLine
            endif
!
            if ( Found .AND. (scr(izp).eq.0.) ) then                    ! A line at zero dip ?
              vLine(izm+1) = vInfp                                      ! Convert to inf. velocity
            else
              vLine(izm+1) = pnorm/scr(nLine)
            endif
            vType(izm+1) = lscr(BasAddr0 + nLine)
!
            j = nTop + 1                                                ! Convert +ve dips to velocity
            do 20 i = izp, nLine-1
              j        = j - 1
              vLine(j) = pnorm / scr(i)
              vType(j) = lscr(BasAddr0 + i)
  20        continue
!
            nLine        = nTop
!....                                ***  Filter lines are given as velocity ***
          else if (FiltTyp.eq.FILTVEL) then
!
            do 25 i = 1, izm                                            ! Normalize -ve velocities
              vLine(i) = pnorm * scr(i)
              vType(i) = lscr(BasAddr0 + i)
  25        continue
!..
            Offs = 0
            izpp = izp + 1
            if ( Found.and.(scr(izp).eq.0.) ) then                      ! Line at v = 0
              vLine(izp)  = -vzerop                                     ! Split into 0+ & 0- lines
              vLine(izpp) =  vzerop
              vType(izp)  = lscr(BasAddr0 + izp)
              vType(izpp) = lscr(BasAddr0 + izp)
              izp         = izpp                                        ! Adjust to first v > 0
              Offs        = 1                                           ! Added a line
!
!...                                            Line types differ across v = 0.
            else if ( lscr(BasAddr0+izm).ne.lscr(BasAddr0+izp) ) then
              vLine(izp)  = -vzerop
              vLine(izpp) =  vzerop                                     ! Add lines at v = 0+ & 0-
              vType(izp)  = lscr(BasAddr0 + izm)
              vType(izpp) = lscr(BasAddr0 + izp)
              Offs        = 2
            endif
!
            do 30 i = izp, nLine                                        ! Normalize +ve velocities
              vLine(i + Offs) = pnorm * scr(i)
              vType(i + Offs) = lscr(BasAddr0 + i)
 30         continue
!
            nLine = nLine + Offs                                        ! Allow for added lines
          endif                                                         ! of if FiltTyp =
          if (IAND(lprint,2).ne.0) then
            print '(/A)','FKFIEX : Normalized Velocities'
            print *,'Adjusted No. of Filter Lines (nLine): ', nLine
            do 40 i = 1, nLine
              if (vType(i).eq.CUT) then
                print *,' Cut Velocity : ', Vline(i)
              else if (vType(i).eq.PASS) then
                print *,'Pass Velocity : ', Vline(i)
              else
                print *,'     Velocity : ', Vline(i),' Type : ',vType(i)
              endif
  40        continue
          endif
      Endif                                                             ! of if First
!
!*************************                             *************************
!*                      Start of the Main Filtering loop
!*************************                             *************************
!
      call inap( buf(numhdr+1), nsamps )                                ! Put the data in the AP
      BasAddr0            = NextAd - 1                                  ! Base address in the AP for window
      BasAddr1            = NextAd
      ap(BasAddr0 + wbase) = 0.                                         ! Zero frequency
      kTrace              = kTrace + 1                                  ! Current wavenumber (k)
!
!***                                *** Generate a Window function for trace ***
!
      If (kTrace.eq.0) then                                             ! This is the Zero wavenumber Trace
        wPt(1)     = wbase - 1
        wPt(nLine) = wbase
      else                                                              ! Wavenumber <> 0
        call ClcWpt(0, wPt, kTrace, vLine, nLine, nw)
!..                                     Position of Line Intercepts on the trace
        iprev = wPt(1)
        i     = 1
!
  120   If ( (iprev.lt.nw).AND.(i.lt.nLine) ) then
          i = i + 1
          if (wPt(i).gt.iprev) then                                     ! Gaps between lines
            if (vType(i).eq.vType(i-1))then                             ! Simple pass/cut sector
              If (vType(i).eq.CUT ) then
                 val = 0.0
              else                                                      ! if pass then
                 val = 1.0
              endif
              do jj = iprev + 1, wPt(i)
  130           ap(BasAddr0 + jj) = val
              enddo
            else                                                        ! Taper at ends of pass/cut
              Taper = vType(i) - vType(i-1)                             ! Pass -> Cut or Cut -> Pass ?
              IF( vLine(i)*vLine(i-1) .GE. 0 ) call FrmWnd
     *         ( iwindo, iwopt, vLine(i-1), vLine(i), kTrace,
     *         iprev+1-wbase, wPt(i)-wbase, ap(BasAddr1 + iprev), Taper)
!
            endif                                                       ! of vType(i) <> vType(i-1)
          endif                                                         ! of Gaps between lines
          iprev = wpt(i)
          go to 120
        endif                                                           ! of iprev < nw & i < nLine
      endif                                                             ! of Wavenumber <> 0
!
!...                                Check the form of the sector across v = inf
      if (vType(1).eq.vType(nLine)) then                                ! Simple pass/cut sector
          if (vType(1).eq.CUT ) then
            val = 0.0
          else                                                          ! if pass
            val = 1.0
          endif
          do jj = 1, wPt(1)
 150        ap(BasAddr0 + jj) = val
          enddo
          do jj = wPt(nLine)+1, nw
 160        ap(BasAddr0 + jj) = val
          enddo
      else                                                              ! Else tapering at v(inf)
        Taper = ATINF * (vType(1) - vType(nLine))
!..                                                  Taper large -ve velocities
        call FrmWnd(iwindo, iwopt, vLine(1), vLine(nLine), kTrace,
     *                1 - wbase, wPt(1) - wbase, ap(BasAddr1), Taper)
!..                                                  Taper large +ve velocities
        call FrmWnd(iwindo, iwopt, vLine(1), vLine(nLine), kTrace,
     *                  wPt(nLine)+1-wbase, nw-wbase,
     *                  ap(BasAddr1 + wPt(nline)), Taper)
      endif
!
!*************************                             *************************
!                       Multiply Trace by its Window Fn
!*************************                             *************************
!
      If ( (idtype.eq.IDFKPLR).OR.idtype.eq.IDFKPLRU ) then
        do i = 1, nw
 200       ap(i) = ap(i) * ap(BasAddr0 + i)
        enddo
      else if (idtype.eq.IDFKRCT) then
        j = -1
        do 210 i = BasAddr0 + 1, BasAddr0 + nw
          j       = j + 2
          ap(j)   = ap(j)   * ap(i)
          ap(j+1) = ap(j+1) * ap(i)
 210    continue
      endif
!
      iout = CLEARAP                                                    ! Get the modified trace out of the Ap
      call rlseap( buf(NumHdr+1), nsamps)
      RETURN
      end
!++
!++
      Subroutine ClcWpt(type, wPt, k, v, nLine, nw)
!------------------------------------------------------------------------------
!    ClcWpt calculates the positions at which the filtering lines cross the
! current trace. These positions are returned as indices of the input trace in
! the integer array wPt. The indices are chosen to be the nearest index below
! the actual crossing line.
!
! Inputs:
!   type      This is an index denoting the form of the filtering lines.
!             Currently ignored as all lines are currently are radial lines
!             from the origin.
!   k         The input wavenumber trace k E [0,nk/2]
!   v(nLine)  The array of normalized velocities for the filter lines
!   nLine     The number of filter lines.
!   nw        The length of the input trace. Frequencies are assumed to run
!             -Nyq -> 0 -> +Nyq - dw
! Outputs:
!    wpt(nLine)  The array of returned indices
!
! Call Chain:
!    CONTRO:FKFIEX:ClcWpt
!
! Externals:
!    IBLW
!
! Last Modified:
!    10/6/88 ajh
!
!------------------------------------------------------------------------------
!
      integer type
      integer k, nw
      integer nLine
      integer wpt(nLine)
      real      v(nLine)
!
      integer wbase, iblw
!
      wbase = nw/2 + 1                                                  ! Position of the zero frequency sample
      do 10 i = 1, nLine
        wpt(i)   = iBlw(v(i) * k) + wbase                               ! Calculate crossing position
        if(wPt(i).lt.0) then                                            ! Make sure wpt in [0,nw]
          wPt(i) = 0
        else if (wPt(i).gt.nw) then
          wPt(i) = nw
        endif
  10  continue
      RETURN
      end
!**
!**
      Integer Function Iblw(x)
!-------------------------------------------------------------------------------
! Iblw returns the nearest integer below a real value x. Not the integer part.
!-------------------------------------------------------------------------------
!
      real x
!
      if(x.ge.0.) then
        Iblw = Int(x)
      else
        Iblw = Int(x) - 1
      endif
      RETURN
      end
!++
!++
      Subroutine FrmWnd(iwindo,iwopt,vst,vend,k,stPt,endPt,buf,dir)
!-------------------------------------------------------------------------------
!   FrmWnd calculates the half window function between vst & vend for the
! points stPt to endPt. vst & vend are real so the window does not necessarily
! start & finish on integer values.
!
! Inputs:
!   iwindo Type of window function
!   iwopt  Variable for windowing
!          = BYA - Window as a function of angle
!          = BYW - Window as a function of frequency
!          = BYK - Window as a function of wavenumber
!
!   vst    Start velocity of filter
!   vend   Final velocity of filter
!    stPt  First point in window
!   EndPt  Last point in window
!
!    dir  Direction of the windowing function
!         = PCDIR - The window is from pass at vst to cut vend
!         = CPDIR - The window is from cut at vst to pass vend
!         = PCINF - The velocity lines straddle v infinity +ve v is pass
!         = CPINF - "       "       "       "       "      +ve v is cut
!
! Outputs:
!   buf(stPt:EndPt)                                                     ! buffer in which to place window fn
!
! Call Chain:
!    CONTRO:FKFIEX:FrmWnd
!
! Externals:
!    Fwindw
!
! Last Modified:
!    10/6/88 ajh
!-------------------------------------------------------------------------------
!
! INCLUDE FILES
! fkfilt.inc
! Include file for the FK filter routines FKFIED & FKFIEX
!
      integer    NSETSP, DBLSETSP
      parameter (NSETSP = 20 )                                          ! The max. no. of FKFIED input values
      parameter (DBLSETSP  = 2*NSETSP)
!
      integer    SINGPARM                                               ! The number of single valued params
      parameter( SINGPARM = 6)
!
      integer     FILNOTST , FILTERR, FILTVEL, FILTDIP
      parameter ( FILNOTST  = -8)                                       ! Declare constants for the filter types
      parameter ( FILTERR   = -7)
      parameter ( FILTVEL   =  1)
      parameter ( FILTDIP   =  2)
!
      integer    CUT, PASS, ATINF
      parameter (CUT = 2, PASS = 1)
      parameter ( ATINF = 2)                                            ! Constant signifying lines near v = inf
!
      Integer PCdir, CPdir, PCinf, CPinf
      Parameter (PCdir =  Cut - Pass)
      Parameter (CPdir = Pass - Cut)
      Parameter (PCinf = AtInf * PCdir)
      Parameter (CPinf = AtInf * CPdir)
!
      Integer INITWDW
      Parameter (INITWDW = 32)
      Parameter (NOINIT  = 0)
!
!..                         Declare constants for the available window types
      integer     NUMWINDT
      parameter (NUMWINDT = 8)                                          ! The number of available window types
      Integer Hamming, Hanning, Gaussian, Bartlett, Rectang
      Integer Blackman, ExctBlck, BlckHarr
      Parameter (Hamming  = 1)
      Parameter (Hanning  = 2)
      Parameter (Gaussian = 3)
      Parameter (Bartlett = 4)                                          ! Triangular Window
      Parameter (RectAng  = 5)
      Parameter (BlackMan = 6)
      Parameter (ExctBlck = 7)                                          ! Exact Blackman
      Parameter (BlckHarr = 8)
!
!..                              Parameters for the window options
      Integer    NUMWOPT
      parameter (NUMWOPT = 3)                                           ! The number of windowing options
      Integer  BYA, BYK, BYW
      Parameter (ByA = 1)
      Parameter (ByK = 2)
      Parameter (ByW = 3)
! PROGRAM
!
      integer iwindo, iwopt
      real    vst, vend
      integer stPt, EndPt
      real    buf(stPt:EndPt)
      integer dir
!
      parameter (DUMV = 0.)
!
      integer DirLcl
      real    Fwindw
!
      if (endPt.lt.StPt) then
        print *, '*** FKFIEX (FrmWnd) ERROR *** ',
     *           ' FrmWnd stPt > Endpt', StPt,endPt
        STOP
      endif
!
      rk = float(k)                                                     ! Convert wavenumber to real
!
      if ((dir.eq.PCDIR).OR.(dir.eq.CPDIR)) then                        !.... Simple window
        if (iwopt.eq.BYA) then
            buf(StPt) = Fwindw(ATan(stPt/rk), ATan(vst), ATan(vend),
     *                          iwindo, dir, INITWDW )
!
        do i = StPt, EndPt
  10      buf(i) = Fwindw(ATan(i/rk), Dumv, Dumv, iwindo,dir, NOINIT)
        enddo
        else if (iwopt.eq.BYW) then
          buf(StPt) = Fwindw(float(StPt), rk*vst, rk*vend, iwindo,
     *                                                  dir, INITWDW )
          do i = StPt, EndPt
  20        buf(i) = Fwindw(float(i), Dumv, Dumv, iwindo, dir, NOINIT)
        enddo
        else if (iwopt.eq.BYK) then
          do i = StPt, EndPt
  30        buf(i) = Fwindw( rk, i/vst, i/vend, iwindo, dir, INITWDW )
          enddo
        endif
!
      else                                                              !........ Filter lines across vinf
        if(dir.eq.PCINF) then
          dirLcl = PCDIR
        else
          dirLcl = CPDIR
        endif
!
        if (iwopt.eq.BYW) then
           print *,'**** FKFILT(FrmWnd) Warning *** ',
     *                'Cannot window as a fn. of w across v = inf'
           print *, 'Using Angle (BYA) instead'
        endif
!
        if (iwopt.eq.BYA .OR. iwopt.eq.BYW) then
           buf(StPt) = Fwindw(ATan(rk/stPt), ATan(1./vst),
     *                         ATan(1./vend), iwindo, dirLcl,INITWDW)
             do i = StPt, EndPt
 110         buf(i) = Fwindw(ATan(rk/i), DumV, DumV, iwindo,
     *                                                dirLcl, NOINIT)
             enddo
        else if (iwopt.eq.BYK) then
          do i = StPt, EndPt
 120       buf(i) = Fwindw(rk, i/vst, i/vend, iwindo, dirLcl,INITWDW)
          enddo
        endif
      endif                                                             ! of (dir == PCDIR) | (dir == CPDIR)
      RETURN
      end
!++
!++
      Real Function Fwindw(theta, start, end, iwindo, dir, init)
!------------------------------------------------------------------------------
!   Fwindw returns the value of a window function . The windows supported
! are the standard SIOSEIS ones.
!
! Inputs:
!   theta  Value within the window. Within Fwindw it is normalized to [0,1] wrt
!          the total range of the window |end - start|
!          = 0  => complete pass
!          = 1  => cut off end of window
!
!   start  Start value of window. Used only on Initialization
!   end    End   value of window.  "    "    "    "    "    "
!   iwindo The type of window to be applied
!   dir    Direction of windowing
!          = PCDIR - The window is from pass to cut
!          = CPDIR - The window is from cut to pass
!
! Last Modified:
!   10/6/88 ajh.
!------------------------------------------------------------------------------
!
      real    theta
      real    start, end
      integer iwindo
      integer dir
!
      Parameter ( PI    = 3.1415927)
      Parameter ( TWOPI = 2. * PI)
      Parameter ( BETA  = 0.5)                                          ! Width constant for Gaussian window
!
      integer    DirLcl
!
      save thetaSt, thetaEnd, Range, b2
!
! INCLUDE FILES
! fkfilt.inc
! Include file for the FK filter routines FKFIED & FKFIEX
!
      integer    NSETSP, DBLSETSP
      parameter (NSETSP = 20 )                                          ! The max. no. of FKFIED input values
      parameter (DBLSETSP  = 2*NSETSP)
!
      integer    SINGPARM                                               ! The number of single valued params
      parameter( SINGPARM = 6)
!
      integer     FILNOTST , FILTERR, FILTVEL, FILTDIP
      parameter ( FILNOTST  = -8)                                       ! Declare constants for the filter types
      parameter ( FILTERR   = -7)
      parameter ( FILTVEL   =  1)
      parameter ( FILTDIP   =  2)
!
      integer    CUT, PASS, ATINF
      parameter (CUT = 2, PASS = 1)
      parameter ( ATINF = 2)                                            ! Constant signifying lines near v = inf
!
      Integer PCdir, CPdir, PCinf, CPinf
      Parameter (PCdir =  Cut - Pass)
      Parameter (CPdir = Pass - Cut)
      Parameter (PCinf = AtInf * PCdir)
      Parameter (CPinf = AtInf * CPdir)
!
      Integer INITWDW
      Parameter (INITWDW = 32)
      Parameter (NOINIT  = 0)
!
!..                         Declare constants for the available window types
      integer     NUMWINDT
      parameter (NUMWINDT = 8)                                          ! The number of available window types
      Integer Hamming, Hanning, Gaussian, Bartlett, Rectang
      Integer Blackman, ExctBlck, BlckHarr
      Parameter (Hamming  = 1)
      Parameter (Hanning  = 2)
      Parameter (Gaussian = 3)
      Parameter (Bartlett = 4)                                          ! Triangular Window
      Parameter (RectAng  = 5)
      Parameter (BlackMan = 6)
      Parameter (ExctBlck = 7)                                          ! Exact Blackman
      Parameter (BlckHarr = 8)
!
!..                              Parameters for the window options
      Integer    NUMWOPT
      parameter (NUMWOPT = 3)                                           ! The number of windowing options
      Integer  BYA, BYK, BYW
      Parameter (ByA = 1)
      Parameter (ByK = 2)
      Parameter (ByW = 3)
! PROGRAM
!
!...                                Initialize the window variables
      DirLcl = dir
!
      if ( init.eq.INITWDW ) then                                       ! Test for INIT bit
!****  pcdir =1, cpdir = -1
        if (DirLcl.eq.PCDIR) then
          thetaSt  = start
          thetaEnd = end
        else if (DirLcl.eq.CPDIR) then
          thetaSt  = end
          thetaEnd = start
        else
          stop '*** Error *** Fwindw incorrect direction specified'
        endif
!
        range = thetaEnd - thetaSt
        if (iwindo.eq.GAUSSIAN)
     *         b2 = 2 * BETA * BETA
      endif
!
      thetaN = (theta - thetaSt) / range                                ! Normalize theta to [0,1]
!
      if ( thetaN .lt.0.) then
        print '(A,G12.3)',
     *        '*** Fwindw Warning *** Normalized theta < 0 ', ThetaN
        thetaN = 0.
      else if (thetaN.gt.1.) then
        print '(A,G12.3)',
     *        '*** Fwindw Warning  *** Normalized theta > 1 ', ThetaN
        thetaN = 1.
      endif
!
      if (iwindo.eq.HAMMING) then
         Fwindw = 0.54 + 0.46 * Cos(PI * thetaN)
      else if (iwindo.eq.HANNING) then
         Fwindw = 0.5 + 0.5 * Cos(PI * thetaN)
      else if (iwindo.eq.GAUSSIAN) then
         Fwindw = Exp( -thetaN*b2)
      else if (iwindo.eq.BARTLETT) then
         Fwindw = 1. - ThetaN
      else if (iwindo.eq.RECTANG) then
         Fwindw = 1.
      else if (iwindo.eq.BLACKMAN) then
         Temp = PI * ThetaN
         Fwindw = 0.42 + 0.5 * Cos(temp) + 0.08 * Cos( 2 * temp)
      else if (iwindo.eq.EXCTBLCK) then
         Temp = PI * ThetaN
         Fwindw = 0.42659071 + 0.49656062 * Cos(temp) +
     *             0.07684867 * Cos( 2 * temp)
      else if (iwindo.eq.BLCKHARR) then
         Temp = PI * ThetaN
         Fwindw = 0.35875 + 0.48829 * Cos(temp) +
     *             0.14128 * Cos( 2 * temp) + 0.01168 * Cos(3 * temp)
      endif
!
      RETURN
      end
