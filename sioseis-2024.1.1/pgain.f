      SUBROUTINE pgain( t, b, dt, nsamps, stime, etime, timmax, alpha )
!    Computes and applies a unique gain correction designed by M.W. Lee
!  of the U.S.G.S. Branch of Petroleum Geology, Denver (303)-236-5753
!     The gain = (time*1000.)**alpha
!   modified by pch to use stime in the calculation of the gain.
!   (it previously assumed both the gain function and the first sample
!   started at time zero, but then only applied the gain from stime
!   to etime)
!
      DIMENSION t(1), b(1)
      LOGICAL virgin 
      SAVE virgin
      DATA virgin/.TRUE./
!  t : trace buffer nsamps long
!  b : scratch buffer
!  dt : sample interval in msec
!  nsamps : number of samples
!  stime : start time of trace in msec
!  etime : endding time to apply gain function
!  timmax : maximum time expected in msec
!  alpha : exponent to the time
!
      IF( virgin ) THEN
          virgin = .FALSE.
          jsamp = IFIX(timmax / dt) + 1
          isamp = IFIX(etime / dt) + 1
          DO 100 k = 1, isamp
             time = stime + (k-1) * dt
             b(k) = time ** alpha
  100     CONTINUE
          IF( isamp .LT. jsamp ) THEN
              DO k = isamp + 1, jsamp
  110            b(k) = b(isamp)
              ENDDO
          ENDIF
      ENDIF
!
! Apply gain function to the trace
!
      DO k = 1, nsamps
  200    t(k) = t(k) * b( k)
      ENDDO
      RETURN
      END

