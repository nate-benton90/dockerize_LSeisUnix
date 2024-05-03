      subroutine chkbin(MrgParam,buf,lbuf,ibuf,scr,lscr,iscr)
!------------------------------------------------------------------------------
! This is a bugfix routine that DIEX calls on input to check to see whether
! we have a new style or old syle binary trace header on disk files. The
! old header was 100 4 byte words long while the new one is 50
!------------------------------------------------------------------------------
!.. Include file for SplitFK, MrgFK & related processes
!
!
!... Computer Type
      integer PRIME, VAXUNIX, APOLLO, VAXVMS, CRAY
      integer CONVEX, IEEE
      parameter (PRIME   = 1)
      parameter (VAXUNIX = 2)
      parameter (APOLLO  = 3)
      parameter (VAXVMS  = 4)
      parameter (CRAY    = 5)
      parameter (CONVEX  = 6)
      parameter (IEEE    = 7)
!
      parameter ( NOOUTP  = -997)                                       ! No Output process yet seen
!
!.. Parameters for DiskPos Subroutine
      integer   DPINIT, FIRSTHDR, TRCHDR, TRCDATA
      parameter ( DPINIT   = 1)
      parameter ( FIRSTHDR = 2)
      parameter ( TRCHDR   = 3)
      parameter ( TRCDATA  = 4)
!
!.. Entries for ReadTrce
      integer RTINIT, READDATA
      parameter(RTINIT   = 1)
      parameter(READDATA = 2)
!
      parameter ( KSPLTLN = 15)                                         ! Number of parameters in the block
!
      integer   COMPOFS, FMTOFS, DUNITOFS, IDARNOFS, DDATLOFS, CPOSOFS
      integer   LPRNTOFS, IDISKOFS, TRCCTOFS, NXOFS
      integer   SINGOFS, MULTOFS
!..                                   Define offsets to the parameters
      parameter ( COMPOFS = 1)                                          !   Computer type
      parameter (  FMTOFS = 2)                                          !   Input data format
      parameter (DUNITOFS = 3)                                          !   Input data file unit
      parameter (IDARNOFS = 4)                                          !   idarn : misalignment on the Cray
      parameter (DDATLOFS = 5)                                          !   No. of samples per output trace
      parameter ( CPOSOFS = 6)                                          !   Current Position of file pointer
      parameter (LPRNTOFS = 7)                                          !   Lprint debug value
      parameter ( NEWSOFS = 8)                                          !   A new start frequency
      parameter ( NEWEOFS = 9)                                          !   A new finish frequency
      parameter (IDISKOFS = 10)                                         !   The output process number
      parameter (TRCCTOFS = 11)                                         !   The number of transferred trace
      parameter (   NXOFS = 12)                                         !   The total number of output traces
      parameter ( SINGOFS = 13)                                         !   Pointer to header comp that is set to 1
      parameter ( MULTOFS = 14)                                         !   Pointer to header comp that records trace No
!
      integer  OHDROFS
      parameter ( OHDROFS = 15)                                         !  Temporary bugfix to read old header types

!
      integer   MrgParam(*)
      real       buf(1), scr(1)                                         ! buffer arrays
      integer   lbuf(1), lscr(1)
      integer*2 ibuf(1), iscr(1)
!
      logical oldhdr
!
      if (MrgParam(COMPOFS).ne.APOLLO) return
      MrgParam(OHDROFS) = 50
      call DskPos(DPINIT,0,MrgParam)
      call DskPos(FIRSTHDR,0,MrgParam)
      call sRdHdr(MrgParam,buf,lbuf,ibuf,iscr)
      oldhdr = .true.
      do i = 1,50
         if (lbuf(i).ne.0) oldhdr = .false.
      enddo
      if (oldhdr) MrgParam(OHDROFS) = 100                               ! Old header length in 4 byte words
      call DskPos(FIRSTHDR,0,MrgParam)
      RETURN
      end

