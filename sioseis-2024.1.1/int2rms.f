      SUBROUTINE int2rms( vtpint, vtp, n )
!   int2rms converts interval velocity two-way-travel time pairs to 
! rms velocity two-way-trace time pairs using Dix's formula.
!
!   Vrms(n) = SQRT( (IV(n)**2(T(n)-T(n-1) + Vrms(n-1)**2*T(n-1))  / T(n) )
!
!  ARGUMENTS:
!  vtpint - The input interval velocity (two way) time pairs.
!  vtp    - The array to receive the output average velocity
!           (two way) time pairs.
!  n      - the number of elements in the vtpint and vtp arrays.
!
      DIMENSION vtpint(n), vtp(n)
!
      vtp(1) = vtpint(1)
      vtp(2) = vtpint(2)
      IF( n .EQ. 2 ) RETURN
      DO j = 3, n, 2
         temp = vtp(j-2) * vtp(j-2) * vtp(j-1)
     &        + vtpint(j) * vtpint(j) * (vtpint(j+1)-vtpint(j-1))
         vtp(j) = SQRT( temp / vtpint(j+1) )
         vtp(j+1) = vtpint(j+1)
      ENDDO
      RETURN
      END

