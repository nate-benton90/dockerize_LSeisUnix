      SUBROUTINE fdmvel (strzon, ntrzon, dt, dx, dtpool, rtpool, ntau,
     +  taustp, nx, lunvel, lenint, vpadss, vpadse, vpadgs, vpadge,
     +     line3d, ncrbln, nline, insamp, crbdum)
!
! changes:
! mod 24 Mar 2015 - handle dt < 1 mil
!
!
!-------------------------------------------------------------------------------
!$R   FDM___: Calculate RMS velocities over each Tau interval for the interval.
!     This returns a velocity function for EVERY trace (ntrzon traces).
!
!     VERITAS SOFTWARE LTD.                  CALGARY, ALBERTA, CANADA
! Author:      N.M.M.                        Date:      February, 1985
! Inputs:      RMS velocities for each trace at control points. (See array
!              DTPOOL.) Note that this RMS velocity is from time = 0 to length
!              of data, and this is NOT the same as the RMS velocity to be
!              calculated over the Tau intervals.
!
! Function:    1- Calculate the interval velocity V(n) at all samples of each
!                 CRB:
!                            -----------------------------------------
!                           / Vin(n)**2 * t(n+1) - Vin(n-1)**2 * t(n)
!                 V(n) =   /  ---------------------------------------
!                       /\/               t(n+1) - t(n)                ,
!                     n = 0 .. length of data (in steps of sample rate),
!                     Vin = input RMS velocity for the CRB.
!
!              2- For each Tau interval calculate 1/2 the RMS velocity over the
!                 interval times a factor for use in the migration calculation:
!                             --------------------------------
!                            /        sum (V(n)**2)
!                 C(n) =    /   -----------------------------
!                        /\/    4 * (t(i+1)/DT - t(i)/DT + 1) ,
!                     i = 1 .. the number of TAU intervals,
!                     n = t(i) .. t(i+1),
!                     DT = Sample rate (in seconds).
!
!                 Note that this is equivalent to:
!                            ------------------------------------------------
!                           /      1             ( V(n)**2                 )
!                          / ------------- * sum ( ------- * (t(n+1)-t(n)) )
!                       /\/  t(i+1) - t(i)       (    4                    )
!
!              3- Partially calculate g. G is used in the migration calculation
!                 and requires: (Refer to FDM programmers' notes.)
!                 a = delta-tau * C'
!                 b = DT/4 * C'
!                         C(n)**2 * DT
!                 C'(n) = ------------
!                           4 * DX**2 ,
!                     DX = Trace separation.
!
!              4- Determine the velocity at the zone of interest at the section
!                 ends. (This is for the trace pad calculation.)
!
!              *****************************************************************
!              * NOTE THE FOLLOWING:                                           *
!              *   A) The square root in point 1 above is NOT taken because of *
!              *      "V(n)**2" in point 2.                                    *
!              *   B) The square root in point 2 above is NOT taken because of *
!              *      "C(n)**2" in point 3.                                    *
!              *****************************************************************
!
! Outputs:     Array VRMS (2D calculation) or file 'FDMV' (3D).
!
! Calling Sequence:
!  STRZON = Start CRB of current zone (line).
!  NTRZON = # of CRB's in the zone.
!  DT     = Sample rate (seconds).
!  DX     = Trace separation (for 3D this is in the "in-line" direction).
!  DTPOOL = Array in which the velocity parameters will be contained. DTPOOL is
!           defined (230,200) to hold 198 control points (the first 2 rows have
!           a special use). The format of DTPOOL is now described:
!    (1,1)= 3 (Always. This is the row index of the first control point.
!    (2,1)= Index of the last control point. (The number of control points =
!           DTPOOL(2,1)-2.
!    (3..230,1) = Unused.
!    (1..230,2) = Interpolation buffer (used in FDMVEL).
!    (1,ix) = Type of input function. (1=TRMS, 2=TVI, 3=TNMO, 4=TDEP)
!    (2,ix) = Control point interpolation code (1="S",2="C" or 3="E")
!    (3,ix) = CRB number of control point.
!    (4,ix) = Distance at which NMO function picked (TNMO only).
!    (5,ix) = Number of time, velocity pairs read at this control point.
!    (6....117,ix) = Control times of function.
!    (118..230,ix) = Velocities at control times.
!  NTAU   = # of Tau steps.
!  TAUSTP = Time of each Tau step.
!  TAUSIZ = Delta-tau for each Tau step.
!  VRMS   = Output velocity (2D only).
!           (NOTE: This is referred to as " C' " in the notes.)
!  LENINT = Greatest time in zone of interest.
!  VPADSS = Interval velocity (V(n)) for 1st CRB at zone of interest.
!  VPADSE = Interval velocity (V(n)) for last CRB at zone of interest.
!           (For 3D VPADSS & VPADSE are the maximum V(n) in the zone of interest
!           at all CRB's that are the start or end of a section.)
!  VPADGS = Same as VPADSS but for 3D cross-lines.
!  VPADGE = Same as VPADSE but for 3D cross-lines.
!  LINE3D = Flag for 3D.
!  NCRBLN = # of CRB's in a line (3D).
!  NLINE  = # of lines (cross-lines) in the 3D survey.
!  INSAMP = # of samples input.
!  CRB    = Current CRB #.
!
! EXTERNALS:
!     AVBUFIN
!
! REVISIONS:
!  Author:   N.M.M.                           Date: August, 1985
! Description: Use interval velocity at the bottom of each Tau step (rather than
!             the middle) so that velocities may be tapered over the Tau step
!             samples to the previous Tau step samples to properly merge steps.
!
! Revised by:   N.M.M.                          Date:   May, 1987
! Reason:       Add 8192 sample limit with no sample rate or length restriction.
!
!  17 June 1988, by pch to make f77 for non-vms, non-ap,  and non-veritas!
!  August - pch.  the vrms array is a virual array, not in Cray!  So,
!                vrms is now stored on disc.  vrms contains a velocity
!                for every tau step of every trace.  It must be
!                transposed too, all of each time steps next to each
!                  other
!           Also think in seconds rather than milleseconds.
!  Sept - pch   CRB is an argument, but it is used as a do loop
!               counter, so I changed the argument to crbdum
!  June 2000 - g77 didn't like float((dt*1000.) because it was a real
!  9 June 2004 - Increase apdata to 5000000
! 14 Aug 2007 - g95 requires array indices must be integer
!
! Non-standard Fortran-66 items:
!    1- IMPLICIT NONE statement.
!    2- Common block names and/or external routines with names > 6 characters.
!    3- Use of %REF or %VAL functions.
!-------------------------------------------------------------------------------
!
      INTEGER crbdum
!
      COMMON /sioap/ iasgnd, irelse, in, iout, nextad, lapsiz, ifree,
     *               iuseap, idecimf, mdsize
      COMMON /apmem/ apdata(0:5000000)
!      IMPLICIT NONE
!
      PARAMETER ( MAXSMP = 8192 )
      INTEGER  strzon,   ntrzon,   ntau,     lenint,   ncrbln,   nline,
     +         insamp,   crb,
     +         dtpool(230,200),    taustp(ntau)
      REAL     rtpool(230,200)
!     equivalence(dtpool,rtpool)

      REAL     dt,       dx,       vpadss,   vpadse,   vpadgs,   vpadge
      LOGICAL  line3d
!
      REAL     vin(112)
!      integer   VIN(112)
      INTEGER  crbst,    crben,    list(5),  time(112),
     +         nv,       i,        lsamp,    n,        esamp,
     +         sr,       tau,      sp,       sc,       s,        c
      REAL     gbase,    vn,       v,        vl,       tl,
     +         tc,       tp,       velint,   veltau,   veloc(MAXSMP)
!
      DATA     list      /118, 6, 5, 4, 0/
      DATA nsofar/1/, icol/1/
!
!
!.... Calculate some loop indeces and partially calculate G. (see notes above)
!      print *,' strzon=',strzon,' ntrzon=',ntrzon,' dt=',dt,' dx=',
!     * dx,' lunvel=',lunvel,' lenint=',lenint
!      print *,vpadss,vpadse,vpadge,vpadge
!      print *,' line3d=',line3d,' ncrbln=',ncrnln,' nline=',nline,
!     *   ' insamp=',insamp,' crbdum=',crbdum
!
      IF( insamp .LE. 0 ) THEN
          PRINT *,' fdmvel error, insamps=',insamps
          STOP
      ENDIF
      maxcol = mdsize / insamp
      maxco  = maxcol
      sr     = nint(dt * 1000.0)
      crbst  = strzon
      crben  = strzon + ntrzon - 1
      vpadss = 0.0
      vpadse = 0.0
      vpadgs = 0.0
      vpadge = 0.0
      gbase  = dt / (4.0 * dx**2)
!
!.... Get the velocity for each CRB in the zone.
      DO 100 crb = crbst, crben
!
!....     Interpolate the input (possibly bulked) RMS velocity functions to get
!....     the velocity for this CRB.
!****     crb can be sent in dtpool(2,2) or as an argument - I chose argument
          CALL avbufin (dtpool, rtpool, 230, 2, -3, list, crb)
!
!....     Convert RMS to interval velocity and fill a velocity array with the
!....     interval velocity at every sample. (Refer to note 1 above.)
          nv = dtpool(5,2)
!
          DO 10 i = 1, nv
             time(i) = dtpool(5+i,2)
              vin(i) = rtpool(117+i,2)
   10     CONTINUE
!
          v     = vin(1)
          tim   = time(1)
          lsamp = 1
          DO 110 n = 2, nv
!
!....          Calculate the interval velocity.
               vl  = v
               tl  = tim
               v   = vin(n)
               tim = time(n)
               vn  = (v**2 * tim - vl**2 * tl) / (tim-tl)
!              call fdmChck1(crb, n-1, tl, tim, vn)
!
!....          Now fill the velocity array with the interval velocity at all
!....          samples in the interval.
!               esamp = nint(tim / sr) + 1
               esamp = nint(tim / (dt*1000.)) + 1
               if (esamp .gt. insamp) esamp = insamp
               if (esamp .ge. lsamp) then
                   DO 80 i = 1, esamp-lsamp+1
                      veloc(lsamp+i-1) = vn
   80              CONTINUE
                   lsamp = esamp + 1
               END IF
  110     CONTINUE
!
!          IF (LSAMP .LE. INSAMP)
!     +         CALL SET (VELOC(LSAMP), INSAMP-LSAMP+1, VN)
!
          IF ( lsamp .le. insamp) THEN                                  ! Pad to end of trace with vint
              DO 120 i = 1, insamp-lsamp+1
                 veloc(lsamp+i-1) = vn
  120         CONTINUE
          ENDIF
!
!....     Get V(n) at the zone of interest.
!          velint = sqrt(veloc(lenint/sr))
!          velint = sqrt(veloc(lenint/(dt*1000.)))
!          itemp = dt*1000.
!          velint = sqrt(veloc(lenint/itemp))
          itemp = floor(lenint/(dt*1000.))     ! Change AJH to handle dt < 1 ms
          velint = sqrt(veloc(itemp))
!
!....     Start the Tau step loop to calculate C' (1/2 the RMS velocity over the
!....     Tau interval). Refer to the note above and note that the square root
!....     is never actually taken. (Refer to notes 2 and 3 above.)
          sc = 0
          DO 200 tau = 1, ntau
               sp = MIN (sc+1, insamp)
!
               tc = taustp(tau)                                         ! Update End of tau interval
!               sc = tc / float(sr) + 1.0                                ! Sample number
               sc = tc / (dt*1000.) + 1.0                        ! Sample number
               sc = min (sc, insamp)
!
               veltau = 0.0                                             ! Average velocity over tau step
               DO 300 s = sp, sc
                  veltau = veltau + veloc(s)
  300          CONTINUE
               veltau     = veltau / (4.0 * (sc-sp+1))
               veloc(tau) = veltau * gbase                              ! Store average back in veloc
!              call fdmChck2(crb,tau,sp,sc,veltau,dt,dx)
  200     CONTINUE
!
!....     If the line is a 2D line then move C' into the array VRMS for use by
!....     routine FDMLIN. Don't forget to find the velocity of the zone of
!....     interest at the end CRB's.
          IF (.not.line3d) THEN
!****                  Multiplex the velocity function and save it on disk
               jndex = crb - crbst + 1
               index = jndex-((jndex-1)/maxcol)*maxcol
               DO 400 tau = 1, ntau
!                    VRMS(CRB-CRBST+1,TAU) = VELOC(TAU)
                  apdata(index) = veloc(tau)
                  index         = index + maxco
  400          CONTINUE
               icol = icol + 1
               IF( jndex .EQ. ntrzon )
     $            maxcol = icol - 1                                     ! Force O/P: Last CDP done.
               IF( icol .GT. maxcol ) THEN
                   index = 1
                   n = icol - 1
                   DO 420 i = 1, ntau
                      ipos = (i-1)*nx+nsofar
                      CALL podisc( lunvel, 1, ipos)
                      CALL wrdisc( lunvel, apdata(index), n )
                      index = index + maxco
  420              CONTINUE
                   nsofar = nsofar + maxcol
                   icol   = 1
               ENDIF

               IF (crb .eq. crbst) vpadss = velint
               IF (crb .eq. crben) vpadse = velint
!
!....     But if this is a 3D line then write C' to file 'FDMV' and check for
!....     the maximum velocity in the zone of interest at the boundaries of the
!....     grid.
          ELSE
!               CALL FILEWRIT ('FDMV', CRB, VELOC)
!
!....          Check for the maximum velocity in the zone of interest in the
!....          first and last lines of the survey.
               IF (CRB .LE. NCRBLN)       VPADGS = MAX (VPADGS, VELINT)
               IF (CRB .GT. CRBEN-NCRBLN) VPADGE = MAX (VPADGE, VELINT)
!
!....          Now check the start and end sections (first and last
!....          "cross-lines").
               DO 501 c = crbst, crben, ncrbln
                  IF (crb .eq. c) vpadss = max (vpadss, velint)
  501          CONTINUE
               DO 502 c = crbst+ncrbln-1, crben, ncrbln
                  IF (crb .eq. c) vpadse = max (vpadse, velint)
  502          CONTINUE
          END IF
  100 CONTINUE
      RETURN
      END
