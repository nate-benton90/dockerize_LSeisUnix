      SUBROUTINE lendeg( alat, amlat, amlong)
!     lendeg returns the length, in meters, of a degree of latitude
! and longitude, given the latitude.  The formula is taken from pg. 5
! of Nathaniel Bowditch's "American Practical Navigator", Vol II., 1981
! It is based on the WGS72 ellipsoid.  Note that it's Volume TWO.
! Read the preface of the book, it's worth the time.
! This routine gets different answers from Bowditch on different
! computers, try your computer and insert the results here!
! Use a latitude of 45.0 degrees.
! Bowditch gets lat meters = 111132, long meters = 78847
! Casio fx-450 calculator  = 111132.92           = 78847.306
! Masscomp 5400 (68020)    = 111132              = 78840.3
!
! Paul Henkart, March 1991
! mod 14 Aug 07 - g95 ca't declare and set in same statement,
!
      REAL alat, amlat, amlong
      REAL*8 pi
      DATA pi/3.141592654/
      REAL*8 alatrads
!
      alatrads = ABS(alat) * pi / 180.D0
      amlat = 111132.92D0 - 559.82D0 * DCOS(2.D0*alatrads)
     &      + 1.175D0 * DCOS(4.D0*alatrads) 
     &      - 0.0023D0 * DCOS(6.D0*alatrads)
      amlong = 111412.84D0 * DCOS(alatrads) 
     &       - 93.5D0 * DCOS(3.D0*alatrads)
     &       + 0.118D0 * DCOS(5.D0*alatrads)
      RETURN
      END
!
!
!
      SUBROUTINE dlendeg( dlat, dmlat, dmlong)
! REAL*8 arguments
      REAL*8 dlat, dmlat, dmlong
      REAL*8 pi
      DATA pi/3.141592654/
      REAL*8 dlatrads
!
      dlatrads = DABS(dlat) * pi / 180.D0
      dmlat = 111132.92D0 - 559.82D0 * DCOS(2.D0*dlatrads)
     &      + 1.175D0 * DCOS(4.D0*dlatrads)
     &      - 0.0023D0 * DCOS(6.D0*dlatrads)
      dmlong = 111412.84D0 * DCOS(dlatrads)
     &       - 93.5D0 * DCOS(3.D0*dlatrads)
     &       + 0.118D0 * DCOS(5.D0*dlatrads)
      RETURN
      END

