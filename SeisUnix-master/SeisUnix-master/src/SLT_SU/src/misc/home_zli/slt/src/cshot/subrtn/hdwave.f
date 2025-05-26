* Copyright (c) Colorado School of Mines, 1990.
* All rights reserved.

      subroutine hdwave(raylst,shtout,stderr,list,shtrec,tinfo,pltray,
     1                  x,z,xs,zs,xstart,xend,xrec,zrec,nrec,recdpt,
     2                  trugeo,slayer,vel,vref,betai,betaf,deltab,
     3                  irefl,ievent,irecd,pi,
     4                  ntr1b,ntrnb,ntr1f,ngap,head,maxevt,maxref,mu,
     5                  ipen,SUBDJ,DJ,SUPDJ,PHI,
     6                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     7                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     8                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,
     9 			xold, COSINC, COSTRA, VINT)


      integer  raylst,    shtout,   stderr,  irecd,
     :         slayer,    ievent,   ntr1b,   ntrnb,    ntr1f,
     :         maxevt,    maxref,   mu,      ipen
      integer  irefl(maxevt,0:maxref)


      real     xrec(nrec),  zrec(nrec),
     :         xs,  zs,  xstart,  xend,  recdpt,  trugeo, 
     :         pi
      real     x(0:*),   z(0:*),  vel(0:*),        vref(*)  
      logical  list,    shtrec,  tinfo,  pltray,   head


      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1

      REAL        XINT(0:MAXINT,MAXSPL),     ZINT(0:MAXINT,MAXSPL)
      REAL*8      A0(0:MAXINT,MXSPM1),       A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),       A3(0:MAXINT,MXSPM1)
      REAL        SIGN(0:MAXN)

      INTEGER     NPTS(0:MAXINT),   NINT,      NORDER(MAXN)

      REAL*8     DZ(MAXN),       DDZ(MAXN),
     :           DELTAX(MAXNP1), DELTAZ(MAXNP1)
      REAL       D(MAXNP1),      V(MAXNP1)

      INTEGER    N

      REAL*8    SUBDJ(MAXN),     DJ(MAXN),
     :          SUPDJ(MAXN),     PHI(MAXN)

      REAL   	xold(maxn/2),   COSINC(MAXN),   COSTRA(MAXN)


c  Local Variables:

c  AMP    Amplitude of the wavelet
c  AMP1   Required by sub RAYDAT, not used here
c  BETA   Takeoff angle in shooting search for critical ray
c  BETA1  Initial takeoff angle in search
c  BETAC  Takeoff angle of critical ray
c  CAUSTC FALSE in this subroutine - head waves that pass through a
c         caustic are not taken into account in this version of the program
c  DANOLD Difference between incident angle and critical angle at the
c         refracting interface.  When this difference changes sign then
c         we are close to critical.
c  DONE   TRUE when receiver rays have been found for this source ray
c         segment
c  DR     Distance travelled along the refractor
c  DX     x-distance between end point of ray and next receiver
c  EXIT   TRUE when near critical angle for this source segment
c  HDAMP  Constant part of head wave amplitude
c  I      Counter
c  INC    + or - 1, depending on relative x-location of next receiver
c  INGAP  TRUE if receiver is located in the gap
c  IREFRA The refracting interface
c  ISEG   Source segment counter
c  ISRSEG Number of source segments 
c  ITRACE Trace number corresponding to this receiver
c  KREFRA The intersection number of the refractor
c  L      Counter
c  LAST   Number of the last receiver for this source segment
c  NOCONV TRUE if can' find the ray or ray not near critical (shooting)
c  NOHDWV TRUE if critical ray emerges betond limits of spread
c  NOLD   Temporary storage of N 
c  NTRABS Station number of this receiver
c  NXTREC Next receiver at which to find ray
c  PHASE,1 Zero here. (No phase shifts on head wave wavelet.)
c  SGNOFF 1. if receivers are to right of source; -1. if to left
c  SINTHC Sine of critical angle
c  TCOEFF Transmission effects along the ray
c  TIME1  Traveltime along the ray
c  TR     Traveltime along receiver segment of the ray
c  TREF   Traveltime along the refractor segment of the ray
c  TS     Traveltime along source segment of the ray
c  VALID  TRUE if this is a valid event
c  VREF() Not used here (required by sub ORDER)
c  X1MX0  x(1) minus x(0)
c  XOLD() Temporary storage of receiver segment
c  XTMP   x-coordinate where previous ray segment hits interface
c  Z1MZ0  z(1) minus z(0)
c  ZTMP   z-coordinate where previous ray segment hits interface  


      real   amp,    amp1,   beta,   beta1,   betac,   danold,
     :       dr,     dx,     hdamp,  phase,   phase1,  sgnoff,
     :       sinthc, tcoeff, time1,  tr,      tref,    ts,
     :       x1mx0,  xtmp,   z1mz0,  ztmp


      integer  i,      inc,    irefra,   iseg,   isrseg,  itrace,
     :         krefra, l,      last,     nold,   ntrabs,  nxtrec

      logical  noconv,  caustc,  nohdwv,  ingap,   valid,  exit,
     :         done

      

c     This is the refracting interface
      irefra = irefl(ievent,1)

      if(irefra.lt.slayer) then
         if(list) write(raylst,'(2x,a,1x,i2)')
     :     'Invalid refraction - event number', ievent
         return     
      end if

c     calculate and check the sine of the critical angle
      sinthc = vel(irefra) / vel(irefra+1)
      if(sinthc.ge.1.) then
c        no valid refraction
         if(list) write(raylst,'(a)')
     :     'Invalid refraction - see layer velocities.'
         return    
      else
c        calculate constant used in amplitude factor
         hdamp = vel(irefra) * sinthc / ( 2. * pi * (1.-sinthc**2) )
      end if

c     isrseg is number of required source segments
      isrseg = 0
c     first assume all receivers are to right of source
      sgnoff = 1.
      if(xrec(1).lt.xs) then
c        at least some of the receivers are to left of shot
c        consider these first
         isrseg = isrseg + 1
         sgnoff = -1.
      end if
      if(xrec(nrec).gt.xs) then
c        some receivers are to right of shot
         isrseg = isrseg + 1
      end if

      phase = 0.
      caustc = .false.
      danold = 0.
      beta1 = betai
c     this is the intersection number of the refractor
      krefra = irefra - slayer + 1

      do 1000 iseg = 1,  isrseg
c        isrseg is either 1 or 2 depending on locations of
c        receivers relative to source

c        first set necessary arrays, depending on order of intersections
         call order(irefl,1,slayer,ievent,vel,norder,v,sign,vref,n,
     :              valid,recdpt,list,raylst,stderr,maxevt,maxref)
         if(.not.valid) then
c           this should not occur at this point
            if(list) write(raylst,'(2x,a)')'Invalid refraction.'
            return     
         end if


c        beta = betai
         beta = beta1
         exit = .false.

500      if(.not.exit.and.beta.le.betaf) then

c           shoot rays until one hits refracting interface near
c           the critical angle (SHOOT returns noconv=true if not
c           near critical)


            CALL SHOOT(X,z,NOCONV,BETA,PI,TRUGEO,xstart,xend,
     1                 recdpt,.true.,krefra,sinthc,sgnoff,danold,
     2                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)

            
            if(noconv) then
               beta = beta + deltab
            else
c              don't shoot any more for this segment
               exit = .true.
c              near critical, converge on solution
               nold = n
               n = irefra - slayer + 1
c              looking for precise ray that hits at critical
               CALL REFRACT(x,z,n,NOCONV,sgnoff,sinthc,
     1                      MAXN,SUBDJ,DJ,SUPDJ,PHI)
               if(noconv) then
c                 do nothing - can't find source segment
                  beta1 = beta - deltab
               else
c                 found this source segment
                  IF(list) THEN
                     write(raylst,'(2x,a)')'Source segment:'
                     call xzout(x,z,n,raylst)
                     write(raylst,'(/2x,a)')'Receiver segments:'
                  END IF
                  if(pltray) CALL RAYplt(X,Z,N-1,ipen)
                  if(tinfo) call ttime(n-1,d,v,ts)
c                 calculate takeoff angle at source of the ray that
c                 intersects refractor at critical angle
                  x1mx0 = x(1) - x(0)
                  z1mz0 = z(1) - z(0)
                  xtmp = x(n)
                  ztmp = z(n)
                  betac = 180. * atan2(x1mx0,z1mz0) / pi
                  beta1 = betac
c                 now shoot a ray at this takeoff angle so we can
c                 see where it emerges on the line
                  n = nold
                  CALL SHOOT(X,z,NOCONV,betac,PI,TRUGEO,xstart,
     1                 xend,recdpt,.false.,0,0.,0.,0.,
     2                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)

                  if(noconv) then
c                    ray probably emerges outside model - so no
c                    refractions for this source ray
                     if(list) write(raylst,'(2x,a/)')
     :               'No head waves found'
                  else
                     deltax(n+1) = x(n+1) - x(n)
                     deltaz(n+1) = z(n+1) - z(n)
                     d(n+1) = sqrt(deltax(n+1)**2 + deltaz(n+1)**2)
c                    calculate some amplitude information
                     if(shtrec) CALL RAYDAT(x(n+1),VREF,irefl,
     1               ievent,AMP1,PHASE1,tcoeff,maxevt,maxref,
     2		     COSINC,COSTRA,
     3               XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     4               DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     5               MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)


c                    now go on and find receiver segments
c                    first need to bracket emergence point of critical ray
                     call brackt(x(n+1),xrec,nrec,sgnoff,nxtrec,nohdwv)
                     if(nohdwv) then
c                       ray emerges beyond farthest receiver
c                       no refractions recorded 
                        if(list) write(raylst,'(2x,a/)')
     :                  'No head waves found'
                     else
c                       temporary storage of receiver portion of ray
                        do 550 i = 1, irefra
                           xold(i) = x(n-i+1)
550                        continue
c                       now set x to receiver portion of ray
c                       also need to reset a few things (ray starts at 
c                       receiver in the following)
                        dx = xrec(nxtrec) - x(n+1)
                        do 600 i = 1, irefra
                           x(i) = xold(i) + dx
                           v(i) = vel(i)
                           sign(i) = -1.
                           norder(i) = i
600                        continue
c                       now find rays to remaining receivers
                        if(sgnoff.lt.0.) then
                           last = 1
                           inc = -1
                        else
                           last = nrec
                           inc = 1
                        end if
                        n = irefra
c                       initialize distance traveled along refractor
                        dr = 0.
                        i = nxtrec
                        done  = .false.
700                     if(.not.done) then
                           if(i.eq.last) done = .true.
                           x(0) = xrec(i)
                           z(0) = zrec(i)
                           CALL REFRACT(x,z,n,NOCONV,-sgnoff,sinthc,
     1                   		MAXN,SUBDJ,DJ,SUPDJ,PHI)
c                          watch for turning ponts...
                           if(sgnoff.lt.0..and.x(n).gt.xtmp) then
                              noconv=.true.
                           end if
                           if(sgnoff.gt.0..and.x(n).lt.xtmp) then
                              noconv=.true.
                           end if
                           if(noconv) then
c                             one more attempt at solution using
c                             a different first guess
                              do 725 l = 1, n
                                 x(l) = xrec(i)
725                              continue
                              CALL REFRACT(x,z,n,NOCONV,-sgnoff,sinthc,
     1                   		   MAXN,SUBDJ,DJ,SUPDJ,PHI)
c                             watch for turning ponts...
                              if(sgnoff.lt.0..and.x(n).gt.xtmp) then
                                 noconv=.true.
                              end if
                              if(sgnoff.gt.0..and.x(n).lt.xtmp) then
                                 noconv=.true.
                              end if
                           end if
                           if(noconv) then
                              do 750 l = 1, n
                                 x(l) = xrec(i)
750                              continue
                           else
c                             calculating distance travelled along refractor
                              dr = dr + sqrt((xtmp-x(n))**2 
     :                        + (ztmp-z(n))**2)
                              call gap(i,ntr1b,ntrnb,ntr1f,ingap,
     :                        ntrabs)
                              if(.not.ingap) then
                                 if(pltray) then
                                    CALL RAYplt(X,Z,N-1,ipen)
                                 end if
                                 if(tinfo) then
                                    call ttime(n-1,d,v,tr)
                                    tref = dr / vel(irefra+1)
                                    time1 = ts + tr + tref
                                 end if
                                 if(shtrec) then
c                                   ntrabs = i + ntr1b - 1
                                    if(ntrabs.le.ntrnb) then
                                       itrace = i 
                                    else
                                       itrace = i - ngap
                                    end if
                                    amp = hdamp * tcoeff / 
     :                              (xs-xrec(i))**2
                                    write(shtout)irecd,itrace,
     :                              ntrabs+mu,time1,
     :                              xrec(i)-xs,ievent,zs,
     :                              amp,phase,caustc,head
                                 end if
                                 if(list) then
                                    call xzout(x,z,n,raylst)
                                    WRITE(RAYLST,'(3X,A,F10.6/)')
     :                              't = ',TIME1
                                 end if
                              else
c                                in the gap
                              end if
                              xtmp = x(n)
                              ztmp = z(n)
                           end if
c                          first guess for next receiver segment
                           dx = trugeo * sgnoff
                           do 775 l = 1, n
                              x(l) = x(l) + dx
775                           continue
c                          end if
                           i = i + inc
                           go to 700
                        end if
                     end if
                  end if
               end if
            end if
            go to 500
         end if
         sgnoff = 1.
         n = nold
         x(0) = xs
         z(0) = zs
         danold = 0.

1000     continue



c     reset source location (was set at receiver location)
      x(0) = xs
      z(0) = zs
      return     
      end

c----------------------------------------------------------------------

      SUBROUTINE REFRACT(X,z,N,NOCONV,sgnoff,sinthc,
     1                   MAXN,SUBDJ,DJ,SUPDJ,PHI)


c     This subroutine uses Newton's method to find a refracted ray
c     segment.  It must be given a first guess.
c     No continuation is used here.

      INTEGER N
      REAL     X(0:N+1),   z(0:n+1),  sgnoff,  sinthc
      LOGICAL  NOCONV

      INTEGER   MAXN
      REAL*8    SUBDJ(MAXN),     DJ(MAXN),
     :          SUPDJ(MAXN),     PHI(MAXN)

cc    Local Variables:
c     I     Counter
c     FAIL  TRUE if failure in Newton iteration
c     MAXIT Maximum number of Newton iterations
c     SOLN  residual must fall below this for a solution
c     RESID residual of system of equations to be solved

      REAL       SOLN,      RESID
      INTEGER    I,         MAXIT
      LOGICAL    FAIL

c     PARAMETER (MAXIT = 10)
      PARAMETER (MAXIT = 50)

      soln = float(n)
      NOCONV = .FALSE.

c     Start the Newton iterations.
c     After each iteration we check to see if the
c     iteration has been carried out successfully, and
c     if so, check to see if it has found a solution.
      I = 0
      CALL NEWTON(X,z,RESID,FAIL,.TRUE.,sgnoff,sinthc,
     1          XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2          DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     3          MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,
     4          SUBDJ,DJ,SUPDJ,PHI)

100   IF(fail.or.RESID.GT.SOLN) THEN
         I = I + 1
         IF(.not.fail.and.I.LT.MAXIT) THEN
            CALL NEWTON(X,z,RESID,FAIL,.TRUE.,sgnoff,sinthc,
     1          XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2          DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     3          MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,
     4          SUBDJ,DJ,SUPDJ,PHI)

         ELSE
c           Newton has failed.
c           Quit
            NOCONV = .TRUE.
            return
         END IF
         GO TO 100
      else
          return
      END IF

      RETURN
      END

c---------------------------------------------------------------------
