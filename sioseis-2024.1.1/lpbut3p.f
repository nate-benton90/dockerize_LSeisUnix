      SUBROUTINE lpbut3p( corner, sample, number, data )
!     lpbut3p is a recursive three-pole Butterworth low-pass filter.
!    Taken from Gold and Rader, "Digital Processing of Signals", pg57
!
!  ARGUMENTS:
!  corner - the corner frequency in hertz
!  sample - the sampling frequency in hertz
!  number - the number of data points in the time series
!  data   - the input and output data array
!
!  Variables:
!  delay  - unfiltered data for the previous point
!  twc    - corner / sample
!
      REAL data(111)

      twc = corner / sample
      expon = EXP(-twc)
      expon2 = EXP(-twc / 2. )
      term = twc * SQRT(3.) / 2.
      beta = 2. * expon2 * COS(term)
      eta = expon2 * (COS(term) + SIN(term) / SQRT(3.))
      delay = data(2)
      delayd = 0.
      delay1 = 0.
      delay2 = 0.

      DO 510 i = 3, number
         out1 = twc * data(i) + expon * delay
         out2 = twc * (eta*delayd-data(i)) + beta*delay1 - expon*delay2
         delayd = data(i)
         delay = out1
         delay2 = delay1
         delay1 = out2
         data(i) = -(out1 + out2)
  510 CONTINUE

      RETURN
      END
