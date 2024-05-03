      SUBROUTINE tohex( inbuf, nbytes, obuf )
!     tohex converts nbytes of the input array into hex characters in
!  the output array.  REMEMBER THAT THERE ARE 2 OUTPUT CHARACTERS for
!  EACH BYTE CONVERTED.
!
!  ARGUMENTS:
!  inbuf  - The input array to be converted. ANY TYPE!
!  nbytes - the number of bytes to convert.
!  obuf   - The output CHARACTER*1 array. MUST BE AT LEAST NBYTES*2 LONG
!
!  mod 15 June 2006 - Use lright rather than rshift because of int/long 
!                     problem in shifts.c
!
      INTEGER*4 inbuf(1), lrshift
      CHARACTER*1 obuf(1)
      CHARACTER*1 hex(16)
      DATA hex/'0','1','2','3','4','5','6','7',
     *         '8','9','A','B','C','D','E','F'/

          j = 1
          n = nbytes + 4
          DO 200 i = 1, n/4
             index = IAND(lrshift(inbuf(i),28),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),24),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),20),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),16),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),12),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),8),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(lrshift(inbuf(i),4),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
             index = IAND(inbuf(i),15) + 1
             IF( j .GT. nbytes+nbytes ) GOTO 200
             obuf(j) = hex(index)
             j = j + 1
  200     CONTINUE
      RETURN
      END
