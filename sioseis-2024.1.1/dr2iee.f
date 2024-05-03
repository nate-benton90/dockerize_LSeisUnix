      SUBROUTINE dr2iee( inbuf, n, iobuf)
!     dr2iee converts DEC floating point (REAL*4) to IEEE floating point
!  (REAL*4).
!
!  ARGUMENTS:
!  inbuf  - The input array of DEC reals to be converted.
!  n      - The number of reals to convert.
!  iobuf - The output array of IEEE reals.  May be the same array as inbuf.
!
!  Copyright (C) The Regents of the University of California, August 1988
!  Paul Henkart, Scripps Institution of Oceanography, La Jolla, Ca. 92093
!  All rights reserved.
!
      DIMENSION inbuf(n), iobuf(n)
      INTEGER rshift
!
      DO 1000 i = 1, n
!   count bits from right to left, starting with 0
!         mant = IAND( inbuf(i), 16#007FFFFF )  { the mantissa is bits 0-22
         IF( inbuf(i) .EQ. 0 ) GOTO 1000
         mant = IAND( inbuf(i), 8388607 )
         isign = RSHIFT( inbuf(i),31 )
         isign = IAND(isign,1)
         isign = LSHIFT(isign,31)
         iexp = RSHIFT( inbuf(i),23 )
         iexp = IAND(iexp,255)
         iexp = iexp - 2
         iexp = LSHIFT(iexp,23)
         iobuf(i) = IOR( iexp, mant )
         iobuf(i) = IOR( iobuf(i),isign )
!
 1000 CONTINUE
      RETURN
      END
