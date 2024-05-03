      SUBROUTINE dd2ieee( inbuf, n, iobuf)
!     dd2ieee converts DEC DOUBLE PRECISION (REAL*8) to IEEE 
!  DOUBLE PRECISION (REAL*8).  Use swap64 to swap bytes if necessary.
!
!  ARGUMENTS:
!  inbuf  - The input array of DEC doubles (8 byte words) to be converted.
!  n      - The number of double words (8 byte words) to convert.
!  iobuf - The output array of IEEE doubles.  May be the same array as inbuf.
!
!  Copyright (C) The Regents of the University of California, August 1994
!  Paul Henkart, Scripps Institution of Oceanography, La Jolla, Ca. 92093
!  All rights reserved.
!
      DIMENSION inbuf(n), iobuf(n)
      INTEGER inbuf, n, iobuf, mant, iexp, isign, i
      INTEGER RSHIFT
!
      DO 1000 i = 1, n*2, 2
         isign = RSHIFT( inbuf(i), 31 )
         mant = IAND( inbuf(i), 7 )
         iexp = RSHIFT( inbuf(i), 3 )
         iexp = IAND(iexp,4095)
         iexp = iexp - 1
         iobuf(i) = LSHIFT(isign,31)
         iexp = LSHIFT(iexp,3)
         iobuf(i) = IOR( iexp, iobuf(i) )
         iobuf(i) = IOR( iobuf(i), mant )
 1000 CONTINUE
      RETURN
      END
