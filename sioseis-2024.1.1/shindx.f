      subroutine shindx(n,arr,iarr,iopt)
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
! Shindx sorts an index array IARR of an ARR of length N to indicate
! either ascending or descending numerical order
! by the Shell - Mezgar algorithm . (Numerical Recipes p 229)
!
! Inputs:
!   arr  - The array to be indexed
!   n    - length of sort
!   iopt - >0 sort into ascending order
!          <0 "      "  descending order
!
! Outputs:
!   iarr - The index array
!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!
      real     arr(n)
      integer iarr(n)
!
      parameter ( ALN2I = 1./0.69314718, TINY = 1.E-5)
!
      lognb2 = INT( ALOG( FLOAT(n) ) * ALN2I + TINY)
!
      do i = 1,n
   1     iarr(i) = i
      enddo
!
      if (iopt.lt.0) GO TO 100
      m = n
      do 12 nn = 1, lognb2
        m = m/2
        k = n - m
        do 11 j = 1, k
          i = j
   3      CONTINUE
          l = i + m
          if ( arr( iarr(l) ).lt.arr( iarr(i) ) ) THEN
                 it = iarr(i)
            iarr(i) = iarr(l)
            iarr(l) = it
            i = i - m
            if (i.ge.1) GO TO 3
          endif
   11   continue
   12 continue
      RETURN
!
  100 CONTINUE
      m = n
      do 112 nn = 1, lognb2
        m = m/2
        k = n - m
        do 111 j = 1, k
          i = j
 103      CONTINUE
          l = i + m
          if ( arr( iarr(l) ).gt.arr( iarr(i) ) ) then
                 it = iarr(i)
            iarr(i) = iarr(l)
            iarr(l) = it
            i = i - m
            if (i.ge.1) GO TO 103
          endif
  111   continue
  112 continue
      RETURN
      end
