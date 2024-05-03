      subroutine cmpslice(nslice,nx,nrows,big,lunt)
      integer nslice, nx, nrows, lunt
      logical big
!-------------------------------------------------------------------------------
!    This routine compresses a set of nslice time slices which have been
!  multiplexed using a width of nrows down to a true length of nx.
! The dataset may either be contained on disk or in the array T.
!
!   nslice - The number of time slices to compress
!   nx     - The number of ranges used to multiplex a time slice
!   nrow   - The number of traces in the actual dataset
!   lunt   - The file stream for the back up data
!   big    - Is data on disk or in array T ?
!-------------------------------------------------------------------------------
!
      integer SZAPMEM, SZTRANP
      parameter (SZAPMEM = 65537)                                       ! Size of the AP memory simulator
      parameter (SZTRANP = 262144)                                      ! Transpose array is 512*512
!
      real    apdata(0:SZAPMEM-1)
      real    t(SZTRANP)
      common /APMEM/ apdata
      common /TRANSP/ t
!
      integer POSABS
      parameter (POSABS = 1)
!
      integer ipos                                                      ! The current position in the disk file
      integer maxslic                                                   ! The maximum number of slices that can be held in T
      integer nread, nwrite
      integer nrem, nproc, numslic
      integer istat
!
      if (nx.eq.nrows) return
!
      if (big) then
        maxslic = SZTRANP/nx
        nrem    = nslice                                                ! number of slices left to process
        nproc   = 0                                                     ! number of slices currently processed
  5     continue
        if (nrem.gt.0) then
          numslic = min(nrem,maxslic)
          ipos    = nproc * nx + 1
          nread   = numslic * nx
          call podisc(lunt, POSABS, ipos)
          call rddisc(lunt, t, nread,istat)
          IF( istat .NE. nread ) THEN
            PRINT *,' rddisc error in CMPSLICE',
     *                ' ipos =',ipos,' nread =',nread,
     *                ' istat =',istat,' nx =',nx
            STOP
          ENDIF
!
          call pressary(numslic,nx,nrows)
!
          ipos   = nproc * nrows + 1
          nwrite = numslic * nrows
          call podisc(lunt, POSABS, ipos)
          call wrdisc(lunt, t, nwrite)
!
          nproc = nproc + numslic
          nrem  = nrem - numslic
          go to 5
        endif
      else
        call pressary(nslice,nx,nrows)
      endif
      nx = nrows                                                        ! update nx
      return
      end
!
      subroutine pressary(nslice,nx,nrows)
      integer nslice, nx, nrows
!-------------------------------------------------------------------------------
! This routine compresses a set of nslice time slices which have been
! multiplexed into the transpose array T. They are compressed from a length
! nx down to slices of length nrows, the true length c including pads. The
! first time slice is assumed to start at array position 0
!
!   nslice - The number of time slices to compress
!   nx     - The number of ranges in a time slice
!   nrow   - The length used to multiplex the time slices
!-------------------------------------------------------------------------------
      integer SZTRANP
      parameter (SZTRANP = 262144)                                      ! Transpose array is 512*512
!
      real    t(SZTRANP)
      common /TRANSP/ t
!
      integer indx1, indx2
      integer j, ii
!
      if (nx.eq.nrows) return
!
      do 10 ii = 1, nslice-1
        indx1 = nrows * ii + 1                                          ! base index for compressed slice
        indx2 = nx * ii + 1
        do 5 j = 0, nrows-1
          t(indx1+j) = t(indx2+j)
  5     continue
 10   continue
      return
      end
!
      subroutine getslice(jstart, nslice, nsamp, nx, apn, lunt, big)
      integer jstart, nslice
      integer nsamp, nx, apn, lunt
      logical big
!-------------------------------------------------------------------------------
! getslice gets time slices from back up storage either disk file or the large
! array T  and puts them into the AP array APDATA. The stored data is in time
! slices in reversed time order.
!
!  jstart - The first time slice to transfer to AP mem
!  nslice - The number of slices to transfer
!  nsamp  - The length of the time series to migrate.
!  nx     - The length of an individual time slice including pads.
!  apn    -  The starting index for data in APMEM
!  lunt   - The stream containing the back up data
!  big    - Is data on disk or in array T ?
!-------------------------------------------------------------------------------
! External Variables
      integer SZAPMEM, SZTRANP
      parameter (SZAPMEM = 65537)                                       ! Size of the AP memory simulator
      parameter (SZTRANP = 262144)                                      ! Transpose array is 512*512
!
      real    apdata(0:SZAPMEM-1)
      real    t(SZTRANP)
      common /APMEM/ apdata
      common /TRANSP/ t
!
      integer index                                                     ! The index into apdata
      integer ipos                                                      ! The current position in the disk file
      integer ipoint                                                    ! The position in the transpose array.
      integer ntrans
      integer istat, jj
!
      ntrans = nx * nslice
      index  = apn
      IF( big ) THEN
         ipos  = (nsamp-jstart) * nx + 1
         CALL podisc( lunt, 1, ipos )
         CALL rddisc( lunt, apdata(index), ntrans,istat)
         IF( istat .NE. ntrans ) THEN
            PRINT *,' rddisc error in GETSLICE',
     *                'ipos=',ipos,' ntrans=',ntrans,
     *               ' istat=',istat,' nsamp',
     *                 nsamp,' jstart=',jstart,' nx=',nx
            STOP
        ENDIF
      ELSE
!
        ipoint = (nsamp-jstart)*nx+1                                    ! pointer to transpose array
           DO 179 jj = 0, ntrans-1
              apdata(index+jj) = t(ipoint+jj)
 179       CONTINUE
      ENDIF
      return
      end


      subroutine putslice(jstart, nslice, nsamp, nx, apn, lunt, big)
      integer jstart, nslice
      integer nsamp, nx, apn, lunt
      logical big
!-------------------------------------------------------------------------------
! putslice puts time slices back into storage either disk file or the large
! array T after they have been migrated in AP memory. The stored data is in time
! slices in reversed time order.
!
!  jstart - The first time slice to transfer to AP mem
!  nslice - The number of slices to transfer to AP mem (Cannot use jend as this
!           may change before slices are written out
!  nsamp  - The length of the time series to migrate.
!  nx     - The length of an individual time slice including pads.
!  apn    -  The starting index for data in APMEM
!  lunt   - The stream containing the back up data
!  big    - Is data on disk or in array T ?
!-------------------------------------------------------------------------------
! External Variables
      integer SZAPMEM, SZTRANP
      parameter (SZAPMEM = 65537)                                       ! Size of the AP memory simulator
      parameter (SZTRANP = 262144)                                      ! Transpose array is 512*512
!
      real    apdata(0:SZAPMEM-1)
      real    t(SZTRANP)
      common /APMEM/ apdata
      common /TRANSP/ t
!
      integer index, ipos, ipoint
      integer ntrans, jj
!
      index  = apn
      ntrans = nslice*nx
      IF ( big ) THEN
        ipos  = (nsamp-jstart) * nx + 1
        CALL podisc( lunt, 1, ipos )
        CALL wrdisc( lunt, apdata(index), ntrans )
      ELSE
        ipoint = (nsamp-jstart)*nx + 1                                  ! pointer to transpose array
        DO 198 jj = 0, ntrans-1
          t(ipoint+jj) = apdata(index+jj)
 198    CONTINUE
      ENDIF
      return
      end
