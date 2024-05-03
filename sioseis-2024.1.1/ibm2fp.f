      SUBROUTINE IBM2FP( idatin, n, datout)
!    IBM2FP converts an array of IBM floating point words to native floating
!  point words.
!    The IBM floating point word is (bit 0 on the left!):
!  bit 0 = the sign bit
!  bits 1-7 is the hex based exponent
!  bits 8-31 is the mantissa
!  The beauty of this algorithm is that a) it should be fairly machine
!  independent (at the loss of execution speed)  and b) a table look up is
!  used instead of doing an exponentiation ( this was suggested by Chris
!  Garrod - thanks Chris).
!
!  ARGUMEWNTS:
!  idatin - the array of IBM floating point values to be converted.
!  n      - the number of values in idatin to convert
!  datout - the array to receive the output floating point.  This may be the
!           same array as idatin.
!
C
! decimal       IBM (as a base 10 number)              IBM F.P. as a hex number
! 0             0                                      00000000
! 1.0           1091567616                             41100000
! .1            1075419545                             40199999
! 1.1           1091672473                             41119999
! -.1           -1072064103                            C0199999
!
C  WRITTEN AND COPYRIGHTED (C) BY:
C  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 13 November 1985
C  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
C  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod 14 June 2006 - Use LRSHIFT rather than RSHIFT because int vs long in shifts.c
!  mad 18 July 2011 - AND out the vacated bit when doing a right shift
!****    rshift (  >> in c ) may fill vacated bit with garbage
!****    rshift_sio and lrshift (see shifts.c) zero the vacated bits.
!
      DIMENSION idatin(n),datout(n)
      DIMENSION expo(127)                                                ! the table of IBM exponent values
      SAVE
      LOGICAL first
      DATA first/.TRUE./
!
      IF( first ) THEN
          first = .FALSE.
          shift = 1. / 1048576.                                          ! 1/1000000 (HEX)
          expo(64) = 1./16.                                              ! set the center value
!  16**32 is too big for most computers - lets hope we never see it!
          DO 10 i = 1, 32                                                ! build the lookup table only once
              expo(64+i) = expo(64+i-1) * 16.
              expo(64-i) = expo(64 -i+1) / 16.
   10     CONTINUE
      ENDIF
      DO 100 i = 1,n
         jsign = 1
         IF( idatin(i) .LT. 0 ) jsign = -1
         iexp = IAND( idatin(i), 2130706432)                              ! get the exponent by AND 7F000000
         iexp = LRSHIFT( iexp,24 )                                        ! right shift 24 bits
         iexp = IAND( iexp,255 )
         IF( iexp .EQ. 0 ) THEN
             datout(i) = 0.
             GOTO 100
         ENDIF
         IF( iexp .LT. 32 .OR. iexp .GT. 96 ) THEN
             IF( iexp .GT. 96 )
     *       PRINT *,' ***  ERROR  ***  The',i,'th IBM value is out of',
     *       ' range for this computer.'
!             PRINT 11, idatin(i)
   11        FORMAT(1x,Z8)
             datout(i) = 0.
             GOTO 100
         ENDIF
         mantis = IAND(idatin(i), 16777215)                               ! get the mantissa by AND with FFFFFF
         mantis = jsign * mantis                                         ! put the sign back on
         temp = FLOAT( mantis ) * shift                                  ! normalize the mantissa
         datout(i) = temp * expo(iexp)                                   ! apply the converted exponent
  100 CONTINUE
      RETURN
      END
