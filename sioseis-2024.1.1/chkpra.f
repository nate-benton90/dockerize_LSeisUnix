      logical function chkpra(inproc)
!-------------------------------------------------------------------------------
! chkproc checks to see if there if the user has specified an FK process in the
! procs list.
!   Outputs:
!      .TRUE.  if an FK process is specified.
!      .FALSE. if no FK process specified
!
!   Last Modified:
!      10/20/88
!-------------------------------------------------------------------------------
!
      character*(*) inproc
!
!.. The following are needed to check the user processes (set in GETPRO)
      parameter ( NAMESS  = 47)
      integer pnum                                                      ! The number of proceeses called
      character*7 pnames(NAMESS)
      common /porder/ pnum, iorder(NAMESS)
      common /pnames/ pnames                                            ! The names of the processes called
!
      parameter ( NSETPROC = 1)                                         ! The number of sets of procs
      parameter ( MAXSETL = 4)                                          ! The max. length of a set
      parameter ( NFKPROC = 4)
!      character*7 setproc(NSETPROC)
      character*7 holdproc(MAXSETL)
      character*7 fkproc(NFKPROC)                                       ! List of known FK processes
!
      character*7 cpproc
      integer     holdnum
      logical     found
!
      save  setproc, fkproc
!
!      data  setproc/'FK'/
      data  fkproc / 'FK2TX ','FKMIGR','FKFILT','TX2FK'/
!
      cpproc = ' '
      cpproc = inproc
      nchars = lenstr(cpproc)
      call upcase(cpproc,nchars)
!
!.. Check to see if it is a process set
      if (cpproc.eq.'FK') then
        do i = 1,NFKPROC
           holdproc(i) = fkproc(i)
        enddo
        holdnum = NFKPROC
      else
        holdproc(1) = cpproc
        holdnum     = 1
      endif
!
! Check the procs list for the given process or set of processes
      i = 1
      found = .FALSE.
 10   if ( (i.le.pnum) .AND. .not.found) then
        j = 1
 15     if ( (j.le.holdnum) .AND. .not.found) then
          if (pnames(i).eq.holdproc(j) ) found = .TRUE.
          j = j + 1
          GO TO 15
        endif
        i = i + 1
        GO TO 10
      endif
!
      chkpra = found
      if (chkpra) then
!        print *,'CHKPRA found  ', inproc
      else
!        print *,'CHKPRA did not find ',inproc
      endif
      return
      end
