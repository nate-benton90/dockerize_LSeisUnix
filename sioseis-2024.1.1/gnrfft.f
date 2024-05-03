      subroutine gnrfft(ar,nlen,direct,norm)
!
!-------------------------------------------------------------------------------
! This is the routine that is called by SIOSEIS to perform the fft of a real tim
! series. This is a glue routine that defines a common call interface between
! SIOSEIS and FFT routine that have been optimized for different machines.
!
! This version is the general version and uses the simple fft pair & scram
! to perform transform of a real time series.
!
!     integer nlen      The length of the input real time series. A power of two
      real    ar(nlen)
!                       The array containing the time series/DFT trace. In
!                       frequency domain the (real) Nyquist frequency component
!                       is expect to be packed at ar(2) after the zero frequency
!                       component at ar(1). All other frequency components are
!                       as expected i.e. real at ar(2i), imag at ar(2i+1)
      logical direct
!                       Direction of FFT.
!                       = TRUE  forward  FFT time -> frequency.
!                       = FALSE inverse FFT frequency -> time.
!                         sign convention is exp(+iwt) for forward transform
!                                            exp(-iwt) for inverse transform
      logical    norm
!                      = TRUE  normalize the output FFT by 1/nlen
!                      = FALSE no normalization.
!-------------------------------------------------------------------------------
!
!
      integer pow2
!
      np = pow2(nlen) - 1
      if(direct) then                                                   ! forward FFT
        call fftfwd(ar,np)                                              ! A +ve exponent
!        call fftinv(ar,np)
        call scram(ar,nlen/2,+1,.FALSE.)
      else
        call scram(ar,nlen/2,-1,.TRUE.)
        call fftinv(ar,np)
!        call fftfwd(ar,np)
      endif
      if (norm) then
        rlen = float(nlen/2)
        do i = 1, nlen
  10    ar(i) = ar(i)/rlen
        enddo
      endif
      return
      end
!
!
      subroutine scram(a,n,isgn,inv)
!---------------------------------------------------------------------------
!    This routine enables the DFT of a real time series of length 2n to
! be recoverd from the DFT of the sames series when it is treated as a complex
! time series of length n.
!
!    On the forward transform the elements real series of 2n are packed
! sequentially into the real & imaginary parts of the complex array. i.e.
! the even elements (starting at 0) are packed into the real parts & the
! odd elements into the imaginary parts. The FFT of this complex series
! is not the same as the FFT of the real series; the output must be
! processed further to recover the correct positive frequency values. This
! is the job of scram.
!
!   On inverse transform the positive frequency components must be repacked
! prior to being passed to a complex inverse FFT if the resulting output is
! be a real time series.
!
!   For the forward transform, this routine returns the Nyquist frequency
! component packed into the imaginary part of the zero frequency component.
! It expects the Nyquist frequency component packed here for the inverse
! transfrom.
!
      complex a(0:n-1)
!                  The array to be transformed
!     Integer n    The length of the complex arrray
      integer isgn
!                   The sign of the current transform
      logical inv
!                 = TRUE  this is an inverse transform. The frequency
!                         components are being packed prior to DFT
!                 = FALSE this is a forward transform. The positive
!                         frequency components are being upacked after
!                         a DFT.
! Modifications:
!     10/20/88 ( Added conjg to atem a(n-i)= )
!
! Last Modified
!     14/3/89 Changed to single precision for CRAY compatibility
!--------------------------------------------------------------------------
!
      parameter (pi = 3.1415926535)
      integer jsgn
      real    rexp, iexp
      real    rphas, iphas, rtem
      real    rtems, items
      complex atem, btem
      complex yi
!
      nby2 = n/2
      jsgn = sign(1,isgn)
      rtem = pi / float(n)
      rexp =  cos(rtem)
      iexp =  sin(jsgn*rtem)
      rphas = 1.0
      iphas = 0.0
      if (inv) then
        yi = cmplx(0.,-0.5)
      else
!      yi   = jsgn * cmplx(0., 0.5)
        yi   = cmplx(0., 0.5)
      endif
!
      do 10 i = 1, nby2
      rtem   = rexp * rphas - iexp * iphas
      iphas  = rexp * iphas + iexp * rphas
      atem   = 0.5 * (a(i) + conjg(a(n-i)))
      btem   = yi * cmplx(rtem, iphas) * ( a(i) - conjg( a(n-i) ) )
      a(i)   = atem - btem
      a(n-i) = atem + conjg(btem)
      a(n-i) = conjg(atem) + conjg(btem)
      rphas  = rtem
   10 continue
!
!.. Assume that the Nyquist frequency is packed/wanted in the imaginary
!    part of the zero frequency component
      rtems = real(a(0))
      items = aimag(a(0))
      a(0)  = cmplx(rtems + items, rtems - items)
      if (inv) a(0) = 0.5 * a(0)
      return
      end
