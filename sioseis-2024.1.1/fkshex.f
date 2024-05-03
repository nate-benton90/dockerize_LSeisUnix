      Subroutine FKSHEX(ibuf,buf,lbuf,iscr,scr,lscr)
!-------------------------------------------------------------------------------
!
!    This is a test version of FKSHFT, a routine to extrapolate FK
! transformed input data. It uses a Phase shift to lower or raise the apparent 
! recording depth of the input data 
!
! Inputs:
!    BUF - The complex frequency trace to be migrated
!   IBUF - The integer*2 equivalent of buf. Used to pass trace header info
!   LBUF - The integer*4 equivalent of buf. Used to pass trace header info
!   SCR/ISCR/LSCR/ - Scratch array and its integer equivalents.
!
! Outputs:
!    BUF - contains the migrated trace.
!
! Called from : CONTRO
!
! Externals :
!  PODISC, RDDISC    : Disk i/o routines
!  INAP
! Non-AP version
!  RECTC, POLARC     : Coordinate conversion routines
!-------------------------------------------------------------------------------
! mod 8 Apr 09 - Use common numdat rather than segy header word ISAMPPTR
! mod 5 Jun 09 - Forgot to divide by 2 on the above

!
      integer SZAPMEM, SZTRANP
      parameter (SZAPMEM = 65537)
      parameter (SZTRANP = 262144)                                      ! Size of /TRANP/: Transpose array at 512 x512
! parameter definitions for the stop signal istop
      integer STOPNOTR, STOPTR, NOSTOP
      parameter (STOPNOTR  = -1)                                        ! last call but no trace passed     
      parameter (STOPTR    =  1)                                        ! last call and a trace passed
      parameter (NOSTOP    =  0)                                        ! normal call to the routine
! parameter for AP 
      integer CLEARAP, USEAP, NOAP 
      integer DATINAP
      parameter (USEAP    = 1)                                          ! There is an AP available
      parameter (NOAP     = 0)                                          ! No AP available
      parameter (CLEARAP  = 0)                                          ! Get data out of AP simulator
      parameter (DATINAP   = 1)

      integer     CRAYEBC, NORMEBC  
      parameter ( CRAYEBC  = 400)
      parameter ( NORMEBC  = 2*CRAYEBC)
      integer     CRAYBIN, NORMBIN
      parameter ( CRAYBIN  =  50)
      parameter ( NORMBIN  = 2*CRAYBIN)
      integer    NTRCPTR, NAUXPTR, IFMTPTR, IDTYPPTR, INKPTR, ITSIPTR
      integer    ITDELPTR, IVERSPTR
      parameter (NTRCPTR  =  7)
      parameter (NAUXPTR  =  8)
      parameter (IFMTPTR  = 13)
      parameter (IDTYPPTR = 31)
      parameter (INKPTR   = 32)
      parameter (ITSIPTR  = 33)
      parameter (ITDELPTR = 34)
      parameter (IVERSPTR = 35)    
      integer IBMFP, INT16, INT32, HOSTFP
      parameter (IBMFP  = 1)
      parameter (INT32  = 2)
      parameter (INT16  = 3)
      parameter (HOSTFP = 5)
      integer    IDTX, IDFKRCT, IDFKPLR, IDTP, IDFKPLRU
      parameter (IDTX     = 1)
      parameter (IDFKRCT  = 2)
      parameter (IDFKPLR  = 3)       
      parameter (IDTP     = 7)
      parameter (IDFKPLRU = 8)
      integer  CURVERS
      parameter( CURVERS = 210)
      integer  CRAYTHDR, NORMTHDR  
      parameter ( CRAYTHDR =  30)
      parameter ( NORMTHDR = 2*CRAYTHDR) 
!
      common /SEGYPTR/ LLSEQPTR, LRSEQPTR, LSHOTPTR, LSHTRPTR, LRPNPTR,
     *                 LRPTRPTR, ITRIDPTR, LDISTPTR, LWBDPTR,  LSXCOPTR,
     *                 LRXCOPTR, IDELMPTR, ISTMPTR,  IENDMPTR, ISAMPPTR,
     *                 ISIPTR,   IYRPTR,   IDAYPTR,  IHRPTR,   IMINPTR,
     *                 ISECPTR,  IGMTPTR,  LDELSPTR, LSMUSPTR, LEMUSPTR,
     *                 LSISPTR,  LWBTSPTR, LGATPTR,  LSSMSPTR, LESMSPTR,
     *                 LSBPTR
      integer    LIVETR, DEADTR    
      parameter (LIVETR  = 1)
      parameter (DEADTR  = 2)
      integer    CREATTMP, RESERVEU, CREATNEW, OPENOLD
      parameter (CREATTMP   = 1)    
      parameter (RESERVEU   = 2)    
      parameter (CREATNEW   = 3)    
      parameter (OPENOLD    = 4)
      integer    RLUNIT, RLDLCLS, RLCLS
      parameter (RLUNIT  = 1)
      parameter (RLCLS   = 2)
      parameter (RLDLCLS = 3)
      integer    POSABS, POSREL, DSKSTRT
      parameter (POSABS     = 1)
      parameter (POSREL     = 2)   
      parameter (DSKSTRT    = 0)
      integer   EOF
      parameter  (EOF = -1)
      integer CDRECT, CDPOLAR, CDPOLARU
      parameter ( CDRECT   = 1)
      parameter ( CDPOLAR  = 2)
      parameter ( CDPOLARU = 3)
      integer BoxCar
      parameter (BoxCar = 5)
!
      Parameter (NSCLRS = 7)
      Parameter (PI = 3.141592654)
!
      common /SIOAP/ iasgnd,irelse,in,iout,nextad,lapsiz,ifree,iuseap,
     *               idecim
      common /READT/ilun,numhdr,numdat,iunhdr,ireelm,intrcs,ifmt,nskip,
     *               secs,lrenum,isrcf,idtype,
     *               nfskip, jform, itxsi, itxdel, nfktrc
      common /FKSHIFT/ v,nfint,lprint,dx,dt, realk, zextrap, to
      common /APMEM/ ap(SZAPMEM)
!
      dimension lbuf(1111),buf(1111), scr(1111),lscr(1111)
      integer*2 ibuf(1111),iscr(1111)
      integer   txsius
      integer   LocCoord
      logical   first
      save
!
      data first /.true./, nxdone/0/
!
      if (first) then                                   
        first = .false.
!
!..                       Read the time delay/sample interval from READT common
        txsius = itxsi                                                  ! t-x sample interval in us
        delay  = itxdel/1000.                                           ! & time delay in secs.
!
        if (idtype.eq.IDFKRCT) then
           LocCoord = CDRECT
        else if ((idtype.eq.IDFKPLR).OR.(idtype.eq.IDFKPLRU)) then
           LocCoord = CDPOLAR
        else
          print *, ' *** FKSHFT Error *** ',
     *           ' The input data is not in the FK domain.'
          STOP
        endif
        nv    = 1                                                       ! Only a single velocity allowed
        nx    = 2* (nfktrc-1)                                           ! Calcalate no. of output traces
        if(dt.le.0.)
     *    dt  = FLOAT(txsius)/1.0E6                                     ! Delta t
        dk    = 2.*pi/(float(nx)*dx)                                    ! Delta k (distance between wavenumbers)
!        nw    = ibuf(ISAMPPTR)/2                                        ! The number of frequencies
        nw = numdat / 2
        dw    = 2.*pi/(float(nw)*dt)                                    ! Delta w (distance between frequencies)
        vnorm = v*v / (dw*dw)                                           ! (v** 2  normalized wrt sampling rate
!        nwrds = ibuf(ISAMPPTR)                                          ! No. of 32 bit words in the complex trace
        nwrds = numdat
        nt    = nwrds / 2                                               ! No. of time samples
        realk = 0.                                                      ! Wavenumber of the initial input trace
!
        td   = zextrap/v * dw
        tadj = (to - delay) * dw
!
        delay = to
        itxdel =  nint(delay * 1000.)                                   ! update delay in common block      
      endif                                                             ! of FKSHFT initialization
!
!***********************                                ***********************
!*                          Start
!***********************                                ***********************
!
      call inap(buf(numhdr+1),nwrds)                                    ! Assume inap puts it at ap location 1
      vskso = vnorm * realk * realk                                     ! ( (v * k)/(dw) ) ** 2
!
!..                                  Host memory (Non-AP) version
      if ( iuseap.eq.NOAP ) then
        if ( LocCoord.eq.CDPOLAR ) then
          call rectc( nw, lscr, ap(in), ap(in+nw) )                     ! Convert to rect coords
          do 1010 j = 1, nwrds, 2
            ap(in+j-1) = scr(j)
            ap(in+j)   = scr(j+1)
 1010     continue
        endif
!
        if ( IAND(lprint,2).eq.2) then
          print '(/A)','Debug Info FKSHFT: Host memory version '
          print *,'Trace No: ', nxdone,' Wavenumber: ', realk
          print *,'No of w (nw): ', nw,' No of ranges (nx): ', nx,
     *            ' (v*k)/(dw)**2 (vkso): ',vskso
          print *,'AP address In: ', in, ' Nextad: ', nextad
          print *,'Delay: ', delay,' Adjust: ', tadj/dw,
     *                                 ' T extrap: ', td/dw
        endif
!
        call PSFK(vskso, ap(in), ap(nextad), nw, scr, scr, tadj, td)
!
        if ( LocCoord.eq.CDRECT) then
          do i  = 1, nwrds                                         ! Shift address of migrated trace
 1040       ap(in+i-1) = ap(nextad+i-1)        
          enddo
        else if ( LocCoord.eq.CDPOLAR ) then
          call polarc(nw,ap(nextad),scr(1),scr(nw+1))
          do 1050 i=1,nw
            ap(in+i-1)    = scr(i)
            ap(in+nw+i-1) = scr(nw+i)
 1050     continue
        endif
      endif                                                             ! of Non-AP version 
!
      realk  = realk  + dk                                              ! Increment the wavenumber
      nxdone = nxdone + 1                                               ! & the number of traces completed
      RETURN
      end
