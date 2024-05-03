      subroutine myspintr(bufi, rbufi, bufa, bufb, size, intrp, ctrlin,
     $                    list, inlen, sh, sr)
      integer size, intrp, ctrlin, list(50), inlen, sh, sr
      integer bufi(size), bufa(size), bufb(size)
      real    rbufi(size)
!-------------------------------------------------------------------------------
!      MYSPINTR is a substitute routine for the FDM spatial interpolation. It
!  replaces routines AVSPINTR/VSPINTR/VSINTPOL which currently do not work
!  correctly with FDMVEL. These routines use APDATA as scratch space but
!  FDMVEL accumulates interpolated velocities in APDATA at the same time !.
!
! Integer bufi  - Interpolation buffer   (column 2 of dtpool)
! Integer bufa  - Start /Left control point buffer
! integer bufb  - Last  /Right control point buffer
! integer size  - Length of the buffers
! integer intrp - row index of interpolation parameter
! integer ctrlin - row index of CDP no.
! integer list  - Array containing row indices into buf
!    list(1)    = row index of start of (vel.) fn. to be interpolated
!    list(2)    = row index of times corresponding to the function values
!    list(3)    = row index of element counts
!                 (bufa and bufb might not be same length)
!    inlen      -  length of data - not used
!    sh         -  shot (crb) now being processed.
!    sr      - current sample rate (ms.) (not used)
!
! CALL CHAIN: fdmiex:fdmvel:avbufin:avintrp:myspintr
!
! EXTERNALS: myintpol (contained in source file)
!
! NOTES: The common time base control (velocity) fns are saved between calls to
!        myspintr and are only updated when they bounding control points change.
!
! A. Harding
! mod Apr 1997 by ADH and GMK to correct some bad interpolation
!
!-------------------------------------------------------------------------------
!
      integer DEBUG, PRINTV
      parameter (DEBUG = 0)                                             ! Non Debug turns on debugging info.
      parameter (PRINTV = 100)                                          ! Interval between printing interpolated fn.
!
      integer IXVBAS, IXTBAS, IXNPAIR                                   ! Index into list for row pointer
      parameter (IXVBAS  = 1)                                           ! Index to row base of velocity
      parameter (IXTBAS  = 2)                                           ! Index to row base of times
      parameter (IXNPAIR = 3)                                           ! Index to row containing num. times/velocities
!
      integer    SZTRASC
      parameter (SZTRASC = 1000)
!
      integer rwvbas, rwtbas, rwnpair                                   ! corresponding row index
      integer rwcdpno                                                   ! row index of cdp no.
      integer precdpa, precdpb                                          ! Indices of previous bounding CDPs
      integer cdpa, cdpb                                                ! Current CDP no.
      integer basinta, basintb                                          ! basis for the interpolated vel. fn.
      integer szintp                                                    ! Length of the interpolated fn.
      integer itemp, sza, szb
      integer ncall
!
      real shfac, onemfac
!
      real     trasc(0:SZTRASC)
      integer itrasc(0:SZTRASC)
      equivalence (trasc,itrasc)
!
      save precdpa, precdpb, szintp, basinta, basintb, trasc
      save ncall
!
      data precdpa, predcdpb/-9999,-9999/
      data ncall /0/
!
!.. Interval velocity function
      intv(vu,vl,itu,itl) = sqrt((vl*vl*itl - vu*vu*itu)/(itl-itu))
!
!...  Set up pointer into buffers
      rwcdpno = iabs(ctrlin)
      rwvbas  = list(IXVBAS)
      rwtbas  = list(IXTBAS)
      rwnpair = list(IXNPAIR)
!
      cdpa = bufa(rwcdpno)
      cdpb = bufb(rwcdpno)
!
!...   When the start and finish control points change must interpolate new
!...   control points onto a common time base.
!
      if ( (cdpa.ne.precdpa).or.(cdpb.ne.precdpb) ) then
        sza = bufa(rwnpair)
        szb = bufb(rwnpair)
!
        do 10 i = 0, sza-1
          trasc(i) = bufa(rwvbas+i)                                     ! Convert velocities to real
   10   continue
        do 20 i = 0, szb-1
           trasc(sza+i) = bufb(rwvbas+i)
   20   continue
!
        basinta = sza + szb
        basintb = sza + szb + size
!
!..        Interpolate velocity fns onto a common set of time pts
!..         Put interpolated time pts directly into bufi
        call myintpol(trasc(0),bufa(rwtbas), sza, bufb(rwtbas), szb,
     $                trasc(basinta), bufi(rwtbas), size, itemp )
!
        call myintpol(trasc(sza),bufb(rwtbas), szb, bufa(rwtbas), sza,
     $                trasc(basintb), bufi(rwtbas), size, szintp )
!
        if (szintp.ne.itemp) then
          print *,' Error in MYSPINTR. Length of interpolated functions'
     $           ,' disagree'
          STOP
        endif
!
        if (DEBUG.ne.0) then
          print '(3(A,I5))',' Time  CDP: ',cdpa,' CDP: ',cdpb,
     $        '   Interval Velocity'
          print '(I5,2F10.0,2X,2I7)',
     $       (bufi(rwtbas+i), trasc(basinta+i),trasc(basintb+i),
     $        intv( trasc(basinta+i-1), trasc(basinta+i),
     $               bufi(rwtbas+i-1),    bufi(rwtbas+i) ),
     $        intv( trasc(basintb+i-1), trasc(basintb+i),
     $               bufi(rwtbas+i-1), bufi(rwtbas+i) ),
     $         i=1, szintp-1)
        endif
!
        precdpa = cdpa                                                  ! Save current control point values
        precdpb = cdpb
      endif
!
!.. Do linear interpolation between time points
      shfac   = float (sh - cdpa) / float(cdpb -cdpa)
      onemfac = 1. - shfac
!
      do 30 i = 0, szintp-1                                             ! truncate to integer during interpolation
        rbufi(rwvbas+i) = trasc(basinta+i) * onemfac
     $                 + trasc(basintb+i) * shfac
   30 continue
      bufi(rwnpair) = szintp                                            ! Transfer length of interpolated function
!
      if (DEBUG.ne.0) then
        ncall   = ncall + 1
        if (mod(ncall, PRINTV) .eq.0) then
          print *,' Interpolated function: '
          print '(2I8)',(bufi(rwvbas+i),bufi(rwtbas+i),i=0,szintp-1)
        endif
      endif
      return
      end
!
!
      subroutine myintpol(vela,tima, sza, timb, szb, res, tres, size,
     $                    szintp)
      integer sza, szb, size, szintp
      real    vela(sza)
      integer tima(sza), timb(szb)
      real    res(size)
      integer tres(size)
!------------------------------------------------------------------------------
!  Myintpol interpolates a velocity function, vela, defined at a set of time
! points, tima, into a function res, defined at
!              tres = ordered union of ( tima, timb).
! The interpolation assumes that the original velocity fn. consists of
! r.m.s. velocity - time pairs and defines a set of interval velocities
! between these time points. These interval velocities are then used to
! generate the
!
! vela() - velocitiy function to interpolate
! tima() - time pts defining vela
! sza    - Length of vela, tima
! timb() - Time pts to interpolate into the velocity function
! szb    - Length of timb
! res()  - Output interpolated function
! tres() - Set of combined time points
! size   - dimension of res/tres
! szintp - length of interpolated function
!------------------------------------------------------------------------------
!
      integer indxl, indxu                                              ! Indexes into A arrays vela, tima
      integer tl, tu                                                    ! Corresponding times
      real    vl, vu                                                    ! and velocities
      integer indxb                                                     ! Index   into B array timb
      integer tb                                                        !  and corresponding time
      integer indxres                                                   ! Index   into result arrays : res, tres
      real    vint
      real    calcv
      logical flag
!
! Setup :
      if ( (tima(1).ne.0).or.(timb(1).ne.0) ) then
        print *,' Error MYINTPOL: Non-zero start times'
        STOP
      endif
!
      indxl = 1
      indxu = 1
      tl    = tima(indxl)
      tu    = tima(indxu)
      vl    = vela(indxl)
      vu    = vela(indxu)
      vint  = vl
!
      indxb   = 1
      indxres = 1
      flag    = .FALSE.
!
   10 continue                                                          ! repeat
        tb    = timb(indxb)
   15   continue
        if ((tb.ge.tu).and.(indxu.lt.sza) ) then                        ! Want tl <= tb < tu
           res(indxres) = vela(indxu)                                   ! Add current velocity to o/p
          tres(indxres) = tima(indxu)                                   ! Add current time
                indxres = indxres + 1
!
          tl    = tu
          vl    = vu
!
          indxu = indxu + 1
          tu    = tima(indxu)
          vu    = vela(indxu)
!
          vint  = (vu**2 * tu - vl**2 * tl)/(tu - tl)                   ! Interval velocity
          go to 15
        else if ((tb.ge.tu).and.(indxu.eq.sza) ) then
           if(.not.flag) then                                           ! Add the last velocity from A
             flag          = .TRUE.                                     ! Indicate last A time in list
              res(indxres) = vu
             tres(indxres) = tu
             indxres       = indxres + 1
           endif
!
           if (tb.gt.tu) then                                           ! Add time tb to end of list
              res(indxres) = calcv(vl,tl,vint,tb)
             tres(indxres) = tb
             indxres       = indxres + 1
           endif
!
        else if ( (tb.gt.tl).and.(tb.lt.tu) ) then                      !  tl < tb < tu
            res(indxres) = calcv(vl,tl,vint,tb)
           tres(indxres) = tb
           indxres       = indxres + 1
        endif
        indxb = indxb + 1
      if (indxb.le.szb) go to 10                                        ! until finished B time pts
!
      if ((indxu.le.sza).and.(.not.flag)) then
        do 20 i = indxu, sza                                            ! Add last A time pts to list
           res(indxres) = vela(i)
          tres(indxres) = tima(i)
          indxres       = indxres + 1
  20    continue
      endif
      szintp = indxres - 1                                              ! Return length of interpolated fn
      return
      end
!
!
      real function calcv(vl,tl,vint,tc)
      real    vl, vint
      integer tl, tc
!-------------------------------------------------------------------------------
! calcv returns the rms. velocity at time tc given the interval velocity of a
! layer, vint, and the rms velocity, vl, and time, tl, at the top of the layer.
!-------------------------------------------------------------------------------
      real tratio
      if (tc.eq.tl) then
       calcv = vl
      else
       tratio = float(tl)/float(tc)
       calcv  = sqrt(vl**2 * tratio + vint * (1. - tratio))
      endif
      return
      end
