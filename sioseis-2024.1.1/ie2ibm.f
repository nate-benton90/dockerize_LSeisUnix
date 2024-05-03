      SUBROUTINE ie2ibm( inbuf, n, iobuf)
!     ie2ibm converts IEEE floating point (REAL*4) to IBM floating
! point (REAL*4).
!
!  ARGUMENTS:
!  inbuf  - The input array of DEC reals to be converted.
!  n      - The number of reals to convert.
!  iobuf  - The output array of IBM reals.  May be the same array as inbuf.
!
!  COPYRIGHT (C) The Regents of the University of California, April 1987
!  Paul Henkart, Scripps Institution of Oceanography, La Jolla, Ca. 92093
!  All rights reserved.
!
!  mod 15 June 2006 - Change RSHIFT to LRSHIFT
!  mod 30 Jan 2013 - Changed lrshift in shifts.c to zero vacated bits.
!
      DIMENSION inbuf(n), iobuf(n)
      EQUIVALENCE (temp,itemp)
!
      DO 1000 i = 1, n
      IF( inbuf(i) .EQ. 0 ) THEN
          iobuf(i) = 0
          GOTO 1000
      ENDIF
!
      jtemp = inbuf(i)   
      mant = IAND( jtemp, 8388607) 
!	print *,' in=',jtemp,' mant=',mant
! the mantissa is bits 0-22, 8388607 is hex(007FFFFF)
      mant =IOR( mant, 8388608)  
! put the hidden bit on; 8388608 = hex(00800000)
      iexp = LRSHIFT( jtemp, 23 )
!	print *,' mant=',mant,iexp
! move the exponent to the right
      iexp = IAND( iexp, 255) - 127
!	print *,' iexp=',iexp
! get rid of the sign - bias = 127
      IF( jtemp .GT. 0 ) THEN
          jsign = 0
      ELSE
          jsign = -1
      ENDIF
      IF( iexp .GE. 0 ) THEN   
! is it greater than 1?
          newexp = LRSHIFT( iexp, 2 ) + 65 
! divide by 4 (IBM is hex based)
          nshift = 3 - MOD(iexp,4)
      ELSE
          iexp = - iexp
          newexp = 64 - LRSHIFT( iexp, 2 )
          itemp = iexp + 3
          nshift = MOD( itemp, 4 )
          IF( nshift .EQ. 3 ) newexp = newexp + 1
      ENDIF
      newman = LRSHIFT( mant, nshift ) 
! IBM has the mantissa on the right
      IF( jsign .LT. 0 ) newexp =IOR( newexp, 128 ) 
! take care of the sign bit
      newexp = LLSHIFT( newexp, 24 )  
! IBM exp is on the left
      iobuf(i) =IOR( newman, newexp )
!	print *, newman,newexp,iobuf(i)
!	stop
!
 1000 CONTINUE
      RETURN
      END
