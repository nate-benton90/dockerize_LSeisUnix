        SUBROUTINE apsID
!******************************************************************************
! This routine prints a string identifying the current APS simulator being
! used. This version is the CRAY vectorized version and uses /TRANSP/ as a
! scratch array.
!******************************************************************************
       print *,'VAPCRY : CRAY vectorized simulator version 1.0'
       return
       end

        SUBROUTINE apsVADD(a,i,b,j,c,k,n)
!******************************************************************************
! Vector Addition simulator
!******************************************************************************
        IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVADD ',a,i,b,j,c,k,n
      DO ii = 0, n-1
  100   apscr(ii) = apdata(a+ii*i) + apdata(b+ii*j)
      ENDDO
      DO ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      ENDDO
      return
      end

        SUBROUTINE apsVSADD(a,i,b,c,k,n)
!******************************************************************************
! Vector Scalar Addition simulator.
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

      real temp
!        print *,' apsVSADD ',a,i,b,j,c,k,n
      temp = apdata(b)
      do ii = 0, n-1
  100   apscr(ii) = apdata(a+ii*i) + temp
      ENDDO
      DO ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      ENDDO
      return
      end

      SUBROUTINE apsVSUB(a,i,b,j,c,k,n)
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVSUB ',a,i,b,j,c,k,n
      DO ii = 0, n-1
  100   apscr(ii) = apdata(b+ii*j) - apdata(a+ii*i)
      ENDDO
      DO ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      ENDDO
      return
      end

      SUBROUTINE apsVMUL(a,i,b,j,c,k,n)
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVMUL ',a,i,b,j,c,k,n
      DO ii = 0, n-1
  100   apscr(ii) = apdata(a+ii*i) * apdata(b+ii*j)
      ENDDO
      DO ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      ENDDO
      return
      end

      SUBROUTINE apsVSMUL(a,i,b,c,k,n)
!******************************************************************************
! Vector-Scalar Multiply simulator.
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

      real temp
!
!     print *,' apsVMUL ',a,i,b,c,k,n
      temp = apdata(b)
      do ii = 0, n-1
  100   apscr(ii) = apdata(a+ii*i) * temp
      enddo
      do ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      enddo
      return
      end

        SUBROUTINE apsVNEG(a,i,c,k,n)
!******************************************************************************
! Vector negation simulator.
!******************************************************************************
        IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVNEG ',a,i,c,k,n
      do ii = 0, n-1
  100   apscr(ii) = -apdata(a+ii*i)
      enddo
      do ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      enddo
      return
      end

      SUBROUTINE apsVMOV(a,i,c,k,n)
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVMOV ',a,i,c,k,n
      if (i.eq.1.and.k.eq.1) then
!DIR$ IVDEP
        do 50 ii = 0,n-1
          apdata(c+ii) = apdata(a+ii)
   50   continue
      else
        iinc = 0
        kinc = 0
        DO 100 ii = 0, n-1
          apdata(c+kinc) = apdata(a+iinc)
          iinc = iinc + i
          kinc = kinc + k
  100   CONTINUE
      endif
      return
      end

      SUBROUTINE apsVFLT(a,i,c,k,n)
!******************************************************************************
      IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVFLT ',a,i,c,k,n
      DO ii = 0, n-1
  100   apscr(ii) = FLOAT(iapdata(a+ii*i))
      ENDDO
      do ii = 0, n-1
  200   apdata(c+ii*k) = apscr(ii)
      ENDDO
      return
      end

        SUBROUTINE apsVFIX(a,i,c,k,n)
!******************************************************************************
        IMPLICIT INTEGER (a-z)
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

!        print *,' apsVFIX ',a,i,c,k,n
        iinc = 0
        DO 100 ii = 0, n-1
           iapscr(ii) = apdata(a+iinc)
           iinc = iinc + i
  100   CONTINUE
        kinc = 0
        DO 200 ii = 0, n-1
           iapdata(c+kinc) = iapscr(ii)
           kinc = kinc + k
  200   CONTINUE
        return
        end

      SUBROUTINE apsCONV(a,i,b,j,c,k,n,m)
!-------------------------------------------------------------------------------
! AP convolution simulator
!-------------------------------------------------------------------------------
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp

      INTEGER a, b, c
!      print *,' apsCONV ',a,i,b,j,c,k,n,m
      kk = 0
      DO 200 ii = 1, n
         answ = 0.
         iindex = ii - 1
         jindex = 0
         DO 100 jj = 1, m
            answ = apdata(a+iindex) * apdata(b+jindex) + answ
            iindex = iindex + i
            jindex = jindex + j
  100    CONTINUE
         apdata(c+kk) = answ
         kk = kk + k
  200 CONTINUE
      RETURN
      END
!
        SUBROUTINE apsVCLR(a,i,n)
!******************************************************************************
        INTEGER a
! Include file VapCry.inc
! Used to by Low Level AP simulator routines
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
      integer SZAPMEM, MAXNX, NAUX, SZAPSCR
      parameter (SZAPMEM = 65537)                                       ! Size of /APMEM/ : AP memory
      parameter (MAXNX   =  16384)                                       ! Max. no of traces as given in FDMIEX
      parameter (NAUX    =     6)                                       ! No. of time slices in APSCR
      parameter (SZAPSCR = NAUX*MAXNX)                                  ! Scratch array size

      real         apdata(0:SZAPMEM-1)
      integer     iapdata(0:SZAPMEM-1)
      common /APMEM/ apdata                                             ! Veritas called this ap120bmd
      equivalence (APDATA,IAPDATA)
!
      real     apscr(0:SZAPSCR)
      integer iapscr(0:SZAPSCR)
      common/SCRAP/apscr
      equivalence (apscr,iapscr)
!
! Define some offsets into the scratch array. These are set by SETAUX
      integer
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
      common/AUXCM/
     $   auxscr1, auxscr2, auxscr3, auxscr4, auxscr5, auxscr6
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        integer apsp(0:15)
        common/AP120BSP/apsp


!        print *,' apsVCLR ',a,i,n
        k = 0
        DO 100 ii = 0, n-1
           apdata(a+k) = 0.
           k = k+i
  100 CONTINUE
        RETURN
        END
