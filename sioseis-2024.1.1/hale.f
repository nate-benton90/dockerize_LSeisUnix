      subroutine hale(vskso4,cpw,cqw,nw,ninterp,cscl,rcscl,iw,dw,t0)
!-------------------------------------------------------------------------------
!
!    Hale is the heart of FK migration when it is performed in host memory
! (Non-AP version). The migration is an implementation of Stolt migration
! It accepts an input trace at a single wavenumber (cpw) and
! produces the output migrated trace (cqw) by the following steps.
!
!   (i) A phase shift to remove the propagation delay. This allows for a non
!       zero start-time t0.
!
!  (ii) Includes the directivity factor for the propagation (wv/w)
!
! (iii) Resampes the input data onto a uniform output array using sinc function
!       interpolation. It is this resampling that effectively accomplishes the
!       migration (c.f. Stolt)
!
! The input trace is arranged so as to run from
!
!
! Inputs:
!    vsks04 - The value (V*K/2*DW)**2
!
!    cpw    - The input trace at a given wavenumber k. The trace is arranged
!             with frequencies running as
!                             -Nyq -> 0 -> +Nyq - delta w
!             The trace has been normalized wrt frequency. Thus locally delta w
!             is 1 & Nyq = nw/2
!
!    nw     - The number of frequencies to migrate.
!    ninterp- The number of adjacent frequencies used in interpolation.
!
!    cscl   - A scratch array used by HALE to hold nw+1 complexes
!             After interpolation each element of the output trace (cqw) is
!             multiplied by a factor which includes directivity, phase shift &
!             an interpolation constant.
!    rcscl  - A real array equivalent to cscl.
!
!    iw     - A scratch integer array nw+1 long.
!    dw     - A scratch real array nw+1 long.
!
!             The index of iw & dw corresponds to the output frequencies from
!             -Nyq to Nyq (nw+1 values). iw & dw give the integer & fractional
!             part of the corresponding input frequency. The values are used
!             in the sinc interpolation. dw is made to satisfy
!                  DELTA <= dw >= 1. - DELTA
!             dw is bounded away from 0 & 1 to ensure numerical stability
!
!    t0     - The time delay of the first sample before FFT.
!
! Outputs:
!   cqw    - The migrated output frequency data (complex).
!            *** CPW  must not be the same as CQW.  ***
!            **** The input array must not be the same as the output array
!
! Call route:
!     contro:fkmiex:hale
!
! Externals:
!     calls no other routines
!
! Last Modified:
!     10/4/88  ajh.
!              Added use of symmetries to calculate -ve freq. from +ve
!              & updated the comments.
!              ** N.B. The size of iw & dw was incresed by 1 to nw+1 **
!
!-------------------------------------------------------------------------------
!
      parameter (DELTA   = 0.0001)                                      ! The tolerance in interpolation
      parameter (ONEMDEL = 1 - DELTA)
      parameter (PI      = 3.141592654)
      parameter (OPI     = 1./PI)
!
      complex cpw(nw+2),cqw(nw+1)
      complex cscl(nw+1)
      real    rcscl(nw+nw+2)                                            ! The real equivalent to cscl
      complex czero
      integer iw(nw+1)
      integer rindxl, rindxh
      real    dw(nw+1)
c
!..                                 Initialize the constants
      czero  = cmplx(0.0,0.0)
      nwo2   = nw / 2                                                   ! The nyquist frequency
      wv     = -nwo2                                                    ! -Nyquist
      j0     = -(ninterp/2 - 1)                                         ! Half width of the interpolation range
!
      j1     = nw + 1
      j2     = nw + 2
      do 50 i = 1,nw                                                    ! Shift the input data by 1 complex sample
        cpw(j2-i) = cpw(j1-i)
   50 continue
      cpw(1)    = czero                                                 ! & pad either end with zeros
      cpw(nw+2) = czero

      middle =  nwo2 + 2                                                ! Position of 0 frequency sample after shift.
      indxlo = -nwo2 - 1                                                ! Index of 1st sample relative to middle
      indxhi =  nwo2                                                    ! Index of last sample relative to middle
!
!..                  Calculate scaling factors & frequency indices for +ve freqs
      i      = nwo2
      indx   = nw - 1                                                   ! The index to the array rcscl
      do 100 iwv = 0, nwo2
        i    =  i + 1
        indx = indx + 2
        wv   = float(iwv)                                               ! The output frequency
        w    = sqrt( float(iwv * iwv) + vskso4 )                        ! & corresponding input freq
        iwh  = int (w)                                                  ! Express as an integer part
        dwh  = w - float( iwh )                                         ! & a fractional part
        if ( dwh.lt.DELTA) then                                         ! Bound away from 0. & 1
          dwh = DELTA
        else if ( dwh.gt.ONEMDEL ) then
          dwh = ONEMDEL
        endif
        iw(i)         = iwh
        dw(i)         = dwh
!
        pidw = pi * dwh                                                 ! Interpolation phase shift
        if ( t0.ne.0. ) then
          phase  = pidw + (w - wv) * t0                                 ! Add shift if non-zero start time
        else
          phase  = pidw
        endif
        if ( abs(w).lt.DELTA ) w = sign(DELTA, w)                       ! watch for divide by zero!
        temp          = sin(pidw) * opi * wv/w                          ! Interpolation + directivty
        rcscl(indx)   =  temp * cos(phase)                              ! Real part of scaling
        rcscl(indx+1) = -temp * sin(phase)                              ! & the imaginary part
  100 continue
!
!..                       Generate the -ve frequency elements from the +ve ones
      nwo2p  = nwo2 + 1
      rindxl = nw + 1
      rindxh = rindxl
      do 110 iwv = 1, nwo2
         indxl          = nwo2p - iwv
         indxh          = nwo2p + iwv
        rindxl          = rindxl - 2
        rindxh          = rindxh + 2
        iw(indxl)       = -iw(indxh) - 1                                ! Negate & subtract 1.
        dw(indxl)       =  1. - dw(indxh)                               ! to ensure dw > 0
        rcscl(rindxl)   = -rcscl(rindxh)                                ! Negate real part of scaling
        rcscl(rindxl+1) =  rcscl(rindxh+1)                              ! & transfer imaginary part
  110 continue
c
c      iwv = -nwo2-1
c      do 120 i = 1, nw
c      iwv = iwv + 1
c      print *,'i: ',i,' iwv: ', iwv,' iw: ',iw(i),' dw: ',dw(i),
c     $        ' cscl: ',cscl(i)
c  120 continue
c
c..                           Do the sinc function interpolation
c
      rj0 = float(j0)                                                   ! Do it only once. N.B j0 < 0
      do 200 i = 1,nw
        iw(i)  = iw(i) + j0                                             ! Adjust to base of interpolation range
        dw(i)  = dw(i) - rj0
        cqw(i) = czero                                                  ! Clear the output array
  200 continue
!
      intc = 1
  300 continue
      do 400 i = 1,nw
        iwpjc = iw(i)
        if ( iwpjc.lt.indxlo ) then
           iwpjc = indxlo
        else if ( iwpjc.gt.indxhi ) then
           iwpjc = indxhi
        endif
        cqw(i) = cqw(i) + cpw(middle+iwpjc) / dw(i)
  400 continue
c
      if ( intc.lt.ninterp ) then                                       ! Increment interpolation indexes
        do 500 i = 1,nw
          dw(i) = dw(i) - 1.
          iw(i) = iw(i) + 1
  500   continue
        intc = intc + 1
        GO TO 300
      endif
!..
      do 600 i = 1, nw
        cqw(i) = cqw(i) * cscl(i)
  600 continue
c
c      cqw(nwo2)   = czero                                              ! ????
      cqw(nwo2+1) = czero                                               ! Remove D.C. frequency component
      RETURN
      end
