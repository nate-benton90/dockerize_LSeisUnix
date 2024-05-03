      SUBROUTINE mag_dummies
c     Entry points to satisfy the loader for magtap and magmagosx removal
c
      ENTRY astape
            PRINT *, ' CALLED astape'
            RETURN
      ENTRY magtap
            PRINT *, ' CALLED magtap'
            RETURN
      ENTRY freetp
            PRINT *, ' CALLED freetp'
            RETURN
      ENTRY offlmt
            PRINT *, ' CALLED offlmt'
            RETURN
      ENTRY untape
            PRINT *, ' CALLED untape'
            RETURN
      END
