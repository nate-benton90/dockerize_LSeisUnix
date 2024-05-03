      SUBROUTINE i24i32( in, iout, nsamps )
!     I24I32 converts 24 bit integers (packed) into 32 bit integers.
!
!  ARGUMENTS:
!  in  - The array of 24 bit integers, packed so that successive integers are
!        in adjecent bits.  There must be nsamps 24 bit words in the array.
!  iout - The output array.  IOUT must be nsamps.   INTEGER*4 words long.
!  nsamps - The number of samples.
!
!  Copyright (C) Paul Henkart, Seismic Reflection Processors, San Diego, Ca.
!  ALL RIGHTS RESERVED.
!
!   00800000 = HEX(8388608) = sign bit of a 24 bit integer
!   00080000 = HEX(524288) = sign bit of a 20 bit integer
!   FF000000 = HEX(-16777216) = sign extension of a negative 24 bit number
!   FFF00000 = HEX(-1048576) = sign extension of a negative 20 bit number
!   00FFFFFF = HEX(16777215)
!   0000FFFF = HEX(65535)
!
!
      INTEGER*4 in(nsamps), llshift, lrshift
      INTEGER iout(nsamps)
!      CHARACTER* 20 c,d,e
!
      i = 1
      j = 1
   10 CONTINUE
!
      iout(j) = lrshift( in(i), 8 )
      IF( IAND(iout(j),8388608) .NE. 0 ) iout(j)=IOR(iout(j),-16777216)
      IF( j .EQ. nsamps ) RETURN
      j = j+1

      itemp = IAND( in(i),255 )
      itemp = llshift( itemp, 16 )
      i = i + 1
      jtemp = lrshift( in(i), 16 )
      jtemp = IAND( jtemp, 65535 )
      iout(j) = IOR(itemp,jtemp)
      IF( IAND(iout(j),8388608) .NE. 0 ) iout(j)=IOR(iout(j),-16777216)
      IF( j .EQ. nsamps ) RETURN
      j = j+1

      itemp = IAND( in(i),65535 )
      itemp = llshift( itemp, 8 )
      jtemp = lrshift( in(i), 24 )
      jtemp = IAND(jtemp,15)
      iout(j) = IOR(itemp,jtemp)
      IF( IAND(iout(j),8388608) .NE. 0 ) iout(j)=IOR(iout(j),-16777216)
      i = i + 1
      IF( j .EQ. nsamps ) RETURN
      j = j+1

      itemp = llshift( in(i), 8 )
      jtemp = lrshift( itemp, 8 )
      iout(j) = jtemp
      IF( IAND(iout(j),8388608) .NE. 0 ) iout(j)=IOR(iout(j),-16777216)
      i = i + 1
      IF( j .EQ. nsamps ) RETURN
      j = j+1
!
      GOTO 10
!
!
      END

