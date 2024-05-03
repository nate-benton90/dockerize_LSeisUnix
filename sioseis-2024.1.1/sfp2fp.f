      SUBROUTINE sfp2fp( ibuf, n, obuf )
!     UTIG short FP, NOT IEEE
!     sfp2fp converts "short floating point" data to internal floating
!  point (REAL).  Short floating point is a 16 bit word, the sign bit,
!  followed by 11 bits of 2's complement mantissa, followed by 4 bits
!  of exponent expressed as 2**(15-exp).
!     This format was first brought to my attention by UTIG.
!
!  ARGUMENTS:
!  ibuf - the input array of 16 bit "floating point" words.
!  n    - The number of data samples to convert.
!  obuf - The output REAL array.
!
! COPYRIGHT (C) Paul Henkart, Seismic Reflection Processors, 22 Mar 1990
!  ALL RIGHTS RESERVED.
!  mod 14 Aug 07 - g95 IAND args must be same size.
!
      INTEGER n, rshift
      INTEGER*2 ibuf(n), ib, i15
      REAL obuf(n)
      INTEGER itemp
      REAL gain(0:15)
      DATA i15/15/
      DATA gain/ 32768, 16384, 8192, 4096, 2048, 1024, 512, 256,
     &           128, 64, 32, 16, 8, 4, 2, 1 /
!
      DO 100 i = 1, n
         ib = ibuf(i)
         jsign = 0
         IF( ib .LT. 0 ) jsign = 1                                      ! the sign bit
!         mant = IAND( ib, 32752 )   ! the 11 bit mantissa - 32752 = 7FF0
         jexp = IAND( ib, i15 )      ! the 4 bit exponent
	 mant = ib
         itemp1 = RSHIFT(mant,4)
         itemp = IAND( itemp1, 2047 )  ! 2047 = 7FF
         IF( jsign .EQ. 1 ) THEN       ! 1 means a negative
	   itemp = IOR( itemp, NOT(2047) )
         ENDIF
         obuf(i) = FLOAT(itemp) * gain(jexp)
  100 CONTINUE
      RETURN
      END
