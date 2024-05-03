      subroutine psfk(vskso,cpw,cqw,nw,cscl,rcscl,tadj,td)
!-------------------------------------------------------------------------------
!
! This is the corresponding Phase shifting routine for FKSHFT. It performs the
! appropriate phase shifting to lower the apparent recording level to tlev.
!
!
! Inputs:
!    vsks0 - The value (V*K/DW)**2
!
!    cpw    - The input trace at a given wavenumber k. The trace is arranged
!             with frequencies running as
!                             -Nyq -> 0 -> +Nyq - delta w
!             The trace has been normalized wrt frequency. Thus locally delta w 
!             is 1 & Nyq = nw/2
!
!    nw     - The number of frequencies to migrate.
!
!    cscl   - A scratch array used by HALE to hold nw+1 complexes
!             After interpolation each element of the output trace (cqw) is
!             multiplied by a factor which includes directivity, phase shift &
!             an interpolation constant.
!    rcscl  - A real array equivalent to cscl.
!
!             The index of iw & dw corresponds to the output frequencies from
!             -Nyq to Nyq (nw+1 values). iw & dw give the integer & fractional
!             part of the corresponding input frequency. The values are used
!             in the sinc interpolation. dw is made to satisfy
!                  DELTA <= dw >= 1. - DELTA
!             dw is bounded away from 0 & 1 to ensure numerical stability
!
!
! Outputs:
!   cqw    - The migrated output frequency data (complex).
!            *** CPW  must not be the same as CQW.  ***
!            **** The input array must not be the same as the output array
!
! Call route:
!     contro:fkshft:psfk
!
! Externals:
!     calls no other routines
!
! Last Modified:
!     11/10/88  ajh.
!
!-------------------------------------------------------------------------------
!
      parameter (PI      = 3.141592654)
      parameter (OPI     = 1./PI)
      parameter (ZERO    = 0.0)
!
      complex cpw(nw+2),cqw(nw+1)
      complex cscl(nw+1)
      real    rcscl(nw+nw+2)          ! The real equivalent to cscl
      complex czero
      integer rindxl, rindxh
!
      czero  = cmplx(0.0,0.0)
!
!..                  Calculate scaling factors & frequency indices for +ve freqs
      nwo2   = nw / 2              ! The nyquist frequency
      indx   = nw - 1              ! The index to the array rcscl
      do 100 iwv = 0, nwo2
        indx = indx + 2
        qsq   = iwv * iwv - vskso
        if (qsq.lt.0.) then
          rcscl(indx)   = ZERO
          rcscl(indx+1) = ZERO
        else
          phase  =   sqrt(qsq) * td + float(iwv) *  tadj
          rcscl(indx)   =  cos(phase)          ! Real part of scaling
          rcscl(indx+1) =  sin(phase)          ! & the imaginary part
        endif
  100 continue
!
!..                       Generate the -ve frequency elements from the +ve ones
      rindxl = nw + 1
      rindxh = rindxl
      do 110 iwv = 1, nwo2
        rindxl          = rindxl - 2
        rindxh          = rindxh + 2
        rcscl(rindxl)   =   rcscl(rindxh)           ! real part of scaling
        rcscl(rindxl+1) =  -rcscl(rindxh+1)         ! & imaginary part
  110 continue
!
!..
      do 600 i = 1, nw
        cqw(i) = cpw(i) * cscl(i)
  600 continue
!
      cqw(nwo2+1) = czero                     ! Remove D.C. frequency component 
      RETURN
      end
