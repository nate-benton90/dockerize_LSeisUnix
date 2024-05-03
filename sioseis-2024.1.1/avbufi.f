      subroutine avbufin(buf, rbuf, rowsz,intrp,ctrlin,list,sh)
      integer rowsz, buf(rowsz,5000), intrp, ctrlin, list(50), sh
      real          rbuf(rowsz,5000)
!-------------------------------------------------------------------------------
!  aurora-vax  interpolate control point parameters from pool buffer
!
!  veritas software ltd.                   calgary, alberta, canada
!
! parameters:
!    buf:       buffer containing parameters to be interpolated by control
!               points.
!               buf(1,1) =      column index of 1st control point
!               buf(2,1) =      column index of last control point
!               buf(1,2)..buf(rowsz,2)          = interpolated control point
!               buf(1,buf(1,1))..buf(rowsz,buf(1,1))    =  1st control point
!               buf(1,buf(2,1))..buf(rowsz,buf(2,1))    = last control point
!    rowsz:     size of control point (ie. row size)
!    intrp      row index of interpolation option (s,c,e) for control points.
!    ctrl       row index of control point # (ie. shot# or crb#)
!               if -ve then flag for special linear interpolation (see avintrp)
!    list       list of indexes to each control point (row elements). if
!               positive then do floating point interpolation, if negative,
!               avoid interpolation. end of list =0.
!    sh         the shot/crb number of the current velocity function.
!
!    To clear up any ambiguities this is how the shot-point interpolation
! option is to be interpreted:
!
!    given shot control points n1 < n2 < n3, and interpolation options:
!         's', 'c', and 'e'. ( 's' = 1, 'c' = 2, 'e' = 3 )
!
! case 1    's' n1, 'c' n2, 'e' n3.
!      a  interpolate from n1 to n2
!      b  interpolate from n2+1 to n3
!      c  skip from n3+1 to 9999
!
! case 2    's'-n1, 'c'-n2, 's'-n3
!      a  interpolate from n1 to n2
!      b  straight-line from n2+1 to n3-1
!      c  straight-line from n3 to 9999
!
! case 3    's'-n1, 'e'-n2, 's'-n3
!      a  interpolate from n1 to n2
!      b  skip from n2+1 to n3-1
!      c  straight-line from n3 to 9999
!
! CALL CHAIN : fdmiex:fdmvel:avbufin
!
! EXTERNALS : avintrp
!             mymove  (at end of source file)
!
! NOTES: In FD migration we should never reach 3a or 3b since the control pts.
!        are defined only by 's','c','c'...,'e' and so there is no straight
!        lining between controls unless to Controls are equal.
!
! revised by:   n.m.m.                          date:   may, 1987
! reason:       add 8192 sample limit with no sample rate or length restriction.
!  17 june 1988 by pch to make f77, non-vms, non-ap, non-veritas
!  17 june 1988 by pch to make f77 - changing 's','c','e' to 1,2,3 respectively.
!  17 june 1988 by pch to make sh an argument rather that who knows - like
!     how is sh set overwise?
!
!  4/25/89 a.j.h. - Added rbuf to the subroutine arguments. This array is a
!                   real array equivalent to buf. This allows velocities to be
!                   interpolated as reals and not integers.
!                 - Added routine mymove to force conversion of control pt
!                   velocities to real.
!-------------------------------------------------------------------------------
!
      integer  inlen,    outlen,   xoutle,   youtle,   zoutle,
     +         insr,     outsr,    xoutsr,   youtsr,   zoutsr,
     +         inntr,    outntr,   xoutnt,   youtnt,   zoutnt,
     +         insamp,   outsam,   xoutsa,   youtsa,   zoutsa,
     +         cursam,   cursr,    crb,      tr,       endsh,    taper
!
      logical  gather,   stacke,   dead
      real     dist
      common   /avdata/
     +         inlen,    outlen,   xoutle,   youtle,   zoutle,
     +         insr,     outsr,    xoutsr,   youtsr,   zoutsr,
     +         inntr,    outntr,   xoutnt,   youtnt,   zoutnt,
     +         insamp,   outsam,   xoutsa,   youtsa,   zoutsa,
     +         cursam,   cursr,    crb,      tr,       endsh,
     +         taper,    gather,   dist,     stacke,   dead
!
      integer
     +       adtrin,   adtrot,   adwin1,   adwin2,   adwin3,   adwin4,
     +       adnliv,   adstak,   adscra
      common /avap/
     +       adtrin,   adtrot,   adwin1,   adwin2,   adwin3,   adwin4,
     +       adnliv,   adstak,   adscra
!
      integer sz, ctrl, ixa, ixb, ixend, ctrla, ctrlb, intrpa,
     +        intrpb, savsh, savix
!
      integer rwvbas, rwnpair                                           ! corresponding row index
!
      integer IXVBAS, IXNPAIR                                           ! Index into list for row pointer
      parameter (IXVBAS  = 1)                                           ! Index to row base of velocity
      parameter (IXNPAIR = 3)                                           ! Index to row containing num. times/velocities
!
      rwvbas  = list(IXVBAS)
      rwnpair = list(IXNPAIR)
!
!....   find out at which control point to start.
    1   continue
        sz    = iabs (rowsz)
        ctrl  = iabs(ctrlin)
        ixa   = buf(1,1)
        ixend = buf(2,1)
        if (ixa .ge. ixend) then
                buf(1,1) = ixend
                ixa = ixend
                ixb = ixend
        else
                ixb = ixa+1
        end if
        ctrla   = buf(ctrl,ixa)
        intrpa  = buf(intrp,ixa)
        ctrlb   = buf(ctrl,ixb)
        intrpb  = buf(intrp,ixb)
!
!....   check all cases:
!
!....   1. current control point >= [s|c|e] ctrla  (sh = current control point)
        if (ctrla .ge. sh) then
          nv = buf(rwnpair,ixa)
          call mymove (buf(1,ixa), buf(1,2),rbuf(1,2), sz, nv, rwvbas)
          buf(ctrl, 2) = sh                                             ! ajh Added update of cdp no.
!
!....     2. current control point = [s|c|e] ctrlb
        else if (ctrlb .eq. sh) then
          nv = buf(rwnpair,ixb)
          call mymove (buf(1,ixb), buf(1,2),rbuf(1,2), sz, nv, rwvbas)
          buf(1,1) = ixb                                                ! Increment start control point
!
!....     3. [s|c|e] ctrla  < current control point <  [s|c|e] ctrlb
        else if (ctrla.lt.sh .and. sh.lt.ctrlb) then
!
!....          3.a intrpa = e, intrpb = s|c|e
          if (intrpa .eq. 3 ) then                                      ! Only one control for job
            nv = buf(rwnpair,ixb)
            call mymove (buf(1,ixb), buf(1,2),rbuf(1,2), sz, nv, rwvbas)
            buf(1,1) = ixb
!
!....           3.b intrpa = s|c, intrpb = s
          else if (intrpb .eq. 1) then
            nv = buf(rwnpair,ixa)
            call mymove (buf(1,ixa), buf(1,2),rbuf(1,2), sz, nv, rwvbas)
            buf(ctrl,2) = sh
!
!....           3.c intrpa = s|c, intrpb = c|e
          else
            call avintrp (buf(1,2), buf(1,ixa), buf(1,ixb),
     +                    rowsz, intrp, ctrlin, list, inlen, sh,
     +                    adwin1, adwin3, cursr)
            buf(ctrl,2) = sh
          end if
!
!....   4. [s|c|e] ctrla <= [s|c|e] ctrlb < current control point
        else
!
!....             4.a get next parameter control point.
         if (ixb .lt. ixend) then
           buf(1,1) = buf(1,1) + 1
           go to 1
!
!....             4.b no more parameter control points. use the last one.
!....       (if possible)
        else
          buf(1,1) = ixend
          nv = buf(rwnpair,ixb)
          call mymove (buf(1,ixb), buf(1,2),rbuf(1,2), sz, nv, rwvbas)
!
          if (intrpb .ne. 3) buf(ctrl,2) = sh
        end if
      end if
!
      return
      end
!
      subroutine mymove(bufa, bufi,rbufi, sz, nv, rwvbas)
      integer sz, nv, rwvbas
      integer bufa(sz), bufi(sz)
      real    rbufi(sz)
!-------------------------------------------------------------------------------
!   mymove transfers the velocity-time function for the control point from the
! input buffer bufa to the output buffer bufi/rbufi while at the same time
! forcing conversion of velocities to real values.
!  bufa       - The buffer to copy from
!  bufi/rbufi - The buffer to copy to
!  sz         - Length of buffers
!  nv         - The number of velocities in the buffer
!  rwvbas     - The base index for velocities
!-------------------------------------------------------------------------------
!
      do 10 i = 1, rwvbas-1
        bufi(i) = bufa(i)
  10  continue
!
      do 20 i = rwvbas, rwvbas+nv-1
        rbufi(i) = bufa(i)
  20  continue
      return
      end
