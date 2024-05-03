          subroutine avintrp (bufi, bufa, bufb, size, intrp, ctrlin,
     +                     list, inlen, sh, adscr1, adscr2, cursr)
!-------------------------------------------------------------------------------
!  av_____: perform control point (shot or crb) interpolation.
!
!       veritas software ltd.                   calgary, alberta, canada
!
!    bufi       interpolated parameter control points
!    bufa       1st parameter control point buffer
!    bufb       2nd parameter control point buffer
!    size       the size of bufi, bufa, bufb.
!    intrp      index to interpolation option (s|c|e)
!    ctrlin     index to parameter control point #
!                  if ctrlin < 0 then array list contains special parameters
!                  (detailed below), and bufi will be greater than size
!    list       list of indexes for interpolation action. all values are
!               assumed to require integer interpolation, except:
!               1. if an index in list > 0 then the value at index is
!                  assumed to be floating point.
!               2. if an index in list < 0  then no interpolation is done.
!               3. index = 0 is the end of the list. (the list must always
!                  contain at least 1 element = 0.)
!               special case when ctrlin < 0:
!    list(1)    = index of start of function to be interpolated
!    list(2)    = index of times corresponding to the function values
!    list(3)    = index of element counts (# of elements in bufa might not
!                 (bufa and bufb might not be same length)
!    list(4)    = start of list of indices of elements
!                 to move directly from bufa to bufi
!    inlen      length of data (necessary for special interpolation)
!    sh         shot (crb) now being processed.
!    adscr1, adscr2     ap scratch arrays used for spatial interpolation
!    cursr      = current sample rate (ms.)
!
! CALL CHAIN  fdmiex:fdmvel:avbufin:avintrp
!
! EXTERNALS   myspintr
!
! NOTE : This is essentially a do-nothing routine under FDM since ctrlin < 0
!        always and control is passed to myspintr. The case of sh = ctrla or
!        sh = ctrlb is handled by avbufin.
!
! REVISIONS:
!  17 june 1988 by pch to convert to f77, non-ap, non-vms, non-veritas.
!  17 june 1988 by pch to remove the call to avflint
!  29 April 1989 ajh : A revised and smaller version that calls myspintr
!
!-------------------------------------------------------------------------------
!
      integer SZBUF
      parameter (SZBUF = 500)
      integer       bufi(SZBUF), bufa(SZBUF), bufb(SZBUF)
      integer       size, intrp,
     +              ctrlin, list(50), inlen, sh, adscr1, adscr2,
     +              cursr
!
      real          a, shfac
      integer       ctrl, ctrla, ctrlb, li, sz, intrpa, fn,
     +              tm, cnt, scix, inx, i, skip
!
!....     check whether the current shot is one of the control points
      ctrl  = iabs(ctrlin)
      ctrla = bufa(ctrl)
      ctrlb = bufb(ctrl)
!
      if( (ctrla.eq.sh).or.(ctrla.eq.ctrlb) ) then
        do 5 i = 1, iabs(size)
           bufi(i) = bufa(i)
    5    continue
         go to 40
      endif
!
      li     = 1
      sz     = size
      intrpa = bufa(intrp)
      shfac  = float(sh-ctrla)/float(ctrlb-ctrla)
!
!....     shot point interpolation
      if (ctrlin.gt.0) then
         do 10 i = 1, sz
            skip = list(li)
!
!....                            interpolate a floating point value
            if (skip.eq.i) then
              bufi(i) = bufa(i) + (bufb(i)-bufa(i)) * shfac
              li = li + 1
!....                                     Don't interpolate
            else if (skip.eq.-i) then
              bufi(i) = bufa(i)
              li      = li + 1
!
!....                                  interpolate an integer value
            else
              a       = bufa(i)
              bufi(i) = a+(float(bufb(i))-a)*shfac
            end if
   10     continue
!
          bufi(intrp) = intrpa
!
!....                             special interpolation
      else
         call myspintr(bufi, bufi, bufa, bufb, size, intrp, ctrlin,
     $                    list, inlen, sh, cursr)
      endif
   40 bufi(ctrl) = sh
      return
      end
