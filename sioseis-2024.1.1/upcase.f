      subroutine upcase(cbuf,n)
!      upcase converts n characters of cbuf to uppercase.  Upcase only converts
! lowercase alphabetic ASCII characters.
!
!  ARGUMENTS:
!    cbuf - The type character string to be converted.
!    n    - The number of characters in cbuf to convert. INTEGER
!
! mod 3 Sept 91 - VMS craps out during execution if n = 0 with an
!                 error about adjustible dimensions if character*1(n)
      character*1 cbuf(1)
!
      IF( n .LE. 0 ) RETURN
      do 100 i=1,n
      if(ichar(cbuf(i)).ge.96.and.ichar(cbuf(i)).le.122)
     *     cbuf(i)=char(ichar(cbuf(i))-32)
  100 continue
      return
      end

