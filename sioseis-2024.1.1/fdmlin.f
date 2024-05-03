      subroutine fdmlin (gamma, rho, theta, dt, ntr, pads, pade, ntau,
     +    taustp, tausiz, nsamp, lunvel, nx, lunt, nrows,
     +    ncols, nrho, fcrho, big)
!  changes:
!  mod 24 Mar 2015 - taun was bad when sample interval was < 1 mil
!
!-------------------------------------------------------------------------------
!     VERITAS SOFTWARE LTD.                  CALGARY, ALBERTA, CANADA
!
! AUTHOR:      N.M.M.                        DATE: April, 1985
!
! ENTRY NAMES: FDMLIN
!
! FUNCTION:    Apply 45-degree finite-difference migration algorithm to one
!              line. This routine provides the looping through all Tau steps,
!              migrating seismic data from and back to array buf in the
!              process. This routine also handles data transfers to/from memory
!              and AP. The maximum number of time slices that can be held
!              in the AP will be packed in a large array and transfered to the
!              AP to minimise costly AP transfers.
!
! PARAMETERS:  GAMMA (R4) = Constant in migration calculation.
!              RHO   (R4) = Constant in migration calculation.
!              THETA (R4) = Constant in migration calculation.
!              DT    (R4) = Delta t - sample rate in seconds.
!              NTR   (I4) = Number of traces to migrate in line.(no pads)
!              PADS  (I4) = Trace pad for start of line (always >= 1).
!              PADE  (I4) = Trace pad for end of line (always >= 1).
!              NTAU  (I4) = Number of Tau steps.
!              TAUSTP(NTAU) (I4) = Times of velocity functions for all Tau
!                                  steps.
!              TAUSIZ(NTAU) (I4) = Delta Tau for all Tau steps (ms.). (The time
!                                  between adjacent velocity functions.)
!              NSAMP (I4) = Number of samples.
!              VRMS(NTR,NTAU) (R4) = Velocity functions for all Tau steps.
!              PFDMO  = (unused)
!              BLOCKO = (unused)
!              TRC_INFO(NROWS,NCOLS) = (R4) Multiplexed data buffer.
!              NROWS  (R4) = Spatial dimension of line = PADS + NTR + PADE.
!              NCOLS  (R4) = Temporal dimension of line = NSAMP.
!              BLOCKX (R4) (unused)
!              SLICES (R4) (unused)
!              NRHO (R4) = Constant for tapering Rho.
!              FCRHO (R4) = Constant for tapering Rho.
!
! INPUTS:      Array buf - This virtual memory array is defined in program
!                            FDM. This array contains the multiplexed traces
!                            of the line that is to be migrated. Each pass
!                            through the line will cause buf to be updated
!                            with the sample values at the next Tau step. (Ie.
!                            on entry the array is considered to contain sample
!                            values of Tau step 0.)
!
! OUTPUTS:     Array buf - On exit from this routine array buf will
!                            contain multiplexed, migrated traces. The array is
!                            built from the top down - samples from Tau step 0
!                            are added to the top of the section; samples from
!                            Tau step NTAU are added to the bottom of the
!                            section. (See the references for a complete
!                            description.)
!
! CALLED BY:   FDM
!
! EXTERNALS:   ABORT, DBUG3, FDMCAP, FDMCAR, MAPAPRA, MOVE, SET, VCLR
!
! COMMON BLOCKS: /APLOCS/  - Contains the AP-addressing for the current line
!                            being processed. (This is used for debugging only
!                            and should not be used elsewhere.)
!
! ERRORS & LIMITATIONS: (See FDM)
!
! REFERENCES:  -H.Brysk, GEOPHYSICS, Vol.48, No.5 (May 1983); P.532-542.
!              -Design notes for FDM, etc.
!===============================================================================
!      REVISIONS:
! AUTHOR:      N.M.M.                        DATE: August-September, 1985
! DESCRIPTION: 1 - Remove the Tau step interpolation of adjacent samples (into
!              file 'FDMO') and replace it with a taper upon the current Tau
!              step velocity function.
!              2 - Apply a Butterworth taper to Rho over the current Tau step:
!
!                               (              1              )
!                               ( --------------------------- )
!                  RHO1 = RHO * (     (   f   ) ** (2 * NRHO) )
!                               ( 1 + ( ----- )               )
!                               (     ( FCRHO )               )
!                  Where,
!                  f = (J - TAUP) / (TAUC - TAUP)
!                  J = sample being processed in current Tau step.
!                  TAUP = previous Tau step sample
!                  TAUC = current Tau step sample (velocity information  for Tau
!                         steps are accurate for Tau step samples, TAUC)
!                  Note: TAUC >= J >= TAUP
!
! AUTHOR:                                    DATE: August, 1986
! DESCRIPTION: Optimizations involving mostly changing file storage to virtual
!              memory storage.
!===============================================================================
!
      integer    DEBUG, PRSTEP
      parameter (DEBUG  = 1)
      parameter (PRSTEP = 1)
!
      integer  ntr, pads, pade, ntau, taustp(ntau), tausiz(ntau), nsamp,
     +         nrows, ncols
      real     gamma, rho, theta, dt, nrho, fcrho
!
!.... AP-locations (All AP MD-locations start with "A". Except some locations
!     are referred to starting with the letter "P" - these refer to the next
!     location of the same mnemonic starting with "A". Ie. PC=AC+1.)
      INTEGER  at, agamma, arho, arhosq, a2rho, atheta, adtau, adt,
     +         adt4, ac, apn, apnj1, apnj2, apn1, apn1j1, apn1j2,
     +         as3, as2, av, ar, as1, au, ag, apnj0,
     +         pc, ppnj1, ppnj2, ppn1, ppn1j1, ppn1j2, pr, pg, pu, pv,
     +         ps1, ps3
      COMMON   /APLOCS/
     +         at, agamma, arho, arhosq, a2rho, atheta, adtau, adt,
     +         adt4, ac, apn, apnj1, apnj2, apn1, apn1j1, apn1j2,
     +         as3, as2, av, ar, as1, au, ag, apnj0
!
      INTEGER  lslice, mxslic, sr, taup, tauc, taun, jstart, jend,
     +         esamp, tau, j, lslic2, i
      REAL     buffap(8011), scale, rho1, f
      LOGICAL  big
!
! Define a common block which is used to simulates the AP120-B data memory
! which has the size of 32K 32-bit floating point words.
!
        REAL    APDATA(0:5000000)
        INTEGER IAPDATA(0:5000000)
        COMMON /apmem/ apdata                                           ! Verita
        EQUIVALENCE (APDATA,IAPDATA)
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        INTEGER APSP(0:15)
        COMMON/AP120BSP/APSP
!
      PARAMETER (maxnx = 16384)                                         ! the ma
      DIMENSION vrms(maxnx)                                             ! room f
!   transp holds the transpose matrix t
      PARAMETER (isize = 262144 )                                        ! 512x5
      COMMON /transp/ t(isize)
      COMMON /sioap/ iasgnd, irelse, in, iout, nextad, lapsiz, ifree,
     *               iuseap, idecimf, mdsize                            ! see in
!
!.... Define AP-arrays and constant locations. (Note that the conventions using
!.... MAPCLR, MAP and MAPRES have been discarded so that the entire AP main data
!.... memory may be used.)
!
!.... Constants go in the 1st 11 locations.
      at     = 0                                                        ! 3-poin

      arho   = 4                                                        ! Rho
      arhosq = 5                                                        ! Rho**2
      a2rho  = 6                                                        ! 2*Rho
      atheta = 7                                                        ! Theta
      adtau  = 8                                                        ! Delta
      adt    = 9                                                        ! Delta
      adt4  = 10                                                        ! Delta
!
!.... Define the time slice length and the number that may be held at one time
!.... in the AP (in array APN). (Note that 7 additional vector locations are
!.... required.)
      lslice = ntr + pads + pade
      mxslic = (mdsize-11) / lslice - 7
      print *, mxslic,' Time slices of length ', lslice,
     *         ' traces can be held in the AP'
      if ( mxslic .le. 1 ) then
          print *,' Time slices + pads are too long for processing!'
          STOP
      endif
!      print *,' dt=',dt,' ntr=',ntr,' pads=',pads,' pade=',pade,
!     *        ' ntau=',ntau,' nsamp=',nsamp,' nx=',nx
!      print *,' nrows=',nrows,' ncols=',ncols,' big=',big
!
!.... Now define the 8 vector locations.
      ac    = 11                                                        ! Veloci
      apn   = ac + lslice                                               ! Sample
      apnj1 = apn + lslice*mxslic                                       ! Sample
      apnj2 = apnj1 + lslice                                            ! Sample
      apn1  = apnj2 + lslice                                            ! Sample
      apn1j1= apn1  + lslice                                            ! Sample
      apn1j2= apn1j1 + lslice                                           ! Sample
      as3   = apn1j2 + lslice                                           ! Scratc
!
!.... For clarity define some arrays that overwrite the defined arrays above.
      as2   = apnj2                                                     ! Scratc
      av    = apnj2                                                     ! v. (Se
      ar    = apn1                                                      ! R. (Se
      as1   = apn1j2                                                    ! Scratc
      au    = apn1j2                                                    ! u. (Se
      ag    = as3                                                       ! g. (Se
!
!.... Set up "next locations" of some AP-arrays. This is done here to take this
!.... code out of the "200" loop below.
      pc = ac+1
      ppnj1  = apnj1+1
      ppnj2  = apnj2+1
      ppn1   = apn1+1
      ppn1j1 = apn1j1+1
      ppn1j2 = apn1j2+1
!
      ps3 = as3+1
      lslic2 = lslice-2
!
!      sr   = nint( dt * 1000.)  !  sr is integer mils, which sucks on < 1mil
      tauc = 1
      taun = nint(taustp(1)/(dt*1000.)) + 1   ! Change ajh
!
!.... Initialize the Tau step loop - set up the constants. (Delta Tau, the 9th
!.... item, will be filled inside the loop.)
      buffap(1) = -1.
      buffap(2) =  2.
      buffap(3) = -1.
      buffap(4) = gamma
      buffap(5) = rho
      buffap(6) = rho**2
      buffap(7) = 2. * rho
      buffap(8) = theta
      buffap(10)= dt
      buffap(11)= dt * .25
!
      if (DEBUG.ne.0) then
        print '(/A)',' About to Start Tau Step Loop'
        call timer
      endif
!
!.... Now loop on all Tau steps.
      do 100 tau = 1, ntau
!
!....     Temporary: Recalculate Rho each time through the Tau loop (because
!....     Rho is scaled at the end of each step).
          buffap(5) = rho
          buffap(6) = rho**2
          buffap(7) = 2. * rho
!
!         dbgtau = tau                                                  ! Used by AP debugging
!         slclen = lslice                                               ! Used by AP debugging
          taup = tauc
          tauc = taun
          if (tau .ge. ntau) then
            taun = nsamp+1
          else
!            taun = taustp(tau+1)/sr + 1
            taun = nint(taustp(tau+1)/(1000.*dt)) 
          end if
          esamp = taup + 1
          jend = nsamp + 1                                              ! force
!
!
!....     Load the AP with constants and C' (velocity information). Get Delta
!....     Tau here.
          buffap(9) = tausiz(tau) / 1000.
!
          ipos = (tau-1)*nx+1
          call podisc( lunvel, 1, ipos )
          call rddisc( lunvel, vrms(pads+1 ), ntr, istat )
          if( istat .ne. ntr ) then
              print *,' rddisc error at 160 in fdmlin, ipos=',ipos,
     *                ' ntr=',ntr
              stop
          endif
          if( pads .GT. 1 ) then
              do i = 1, pads-1
  160            vrms(1+i) = vrms(pads+1)
              enddo
          endif
!
          IF( pade .GT. 1 ) THEN
              DO i = 1, pade-1
  165            vrms(pads+ntr+i) = vrms(pads+ntr)
              ENDDO
          ENDIF
!
          vrms(1)             = 0.
          vrms(pads+ntr+pade) = 0.
!
          DO i = 1, 11
  170        apdata(at+i-1) = buffap(i)
          ENDDO
!
          DO i = 1, nrows                                           ! Transfer velocity slice
  171        apdata(at+10+i) = vrms(i)
          ENDDO
!
          DO i = 1, lslice*6                                        ! Zero time slices at j+1, j+2
  172        apdata(apnj1+i-1) = 0.
          ENDDO
!
          call setaux(lslice)                                           ! Zero auxillary slices
!
!         call fdmChck3(3,esamp,tauc)
!
!....     Process all samples for this Tau step.
          DO 200 j = nsamp, esamp, -1
!         dbgtim = j                                                    ! used by ap debugging
!
!....          Get the next sample. If we don't have the sample we need then
!....          load up the AP with as many samples as will fit.
               if (jend .gt. j) then
                 jstart = j
                 jend   = max (j-mxslic+1, esamp)
                 IF( tau .EQ. 1 .OR. mxslic .LE. nsamp ) THEN           ! Data i
                   nslice = jstart - jend + 1
                   call getslice(jstart,nslice,nsamp,nx,apn,lunt,big)
                 ENDIF
                 apnj0 = apn                                            ! Reset
               ELSE
                 apnj0 = apnj0 + lslice
               ENDIF
!
!....          Once the current Tau step samples are reached then the velocity
!....          function for this step must be scaled so as to correctly merge
!....          this step with the previous.
!
               IF (j.lt.tauc) THEN
                   scale = float(j-taup) / float(tauc-taup)
!
                    DO i = 1, lslice
  194                  apdata(ac+i-1) = vrms(i) * scale
                    ENDDO
!
!....               Butterworth taper Rho over the same interval.
                    f         = float(tauc-j) / float(tauc-taup)
                    rho1      = rho / (1. + (f/fcrho) ** (2.*nrho))
                    buffap(5) = rho1
                    buffap(6) = rho1 ** 2
                    buffap(7) = 2. * rho1
!
                    DO i = 1, 3
  195                  apdata(arho+i-1) = buffap(4+i)
                    ENDDO
               END IF
!
!....          Do the migration.
!....          Note that each AP-array is padded with a 0 at either end to avoid
!....          problems with the 3-point convolution in FDMCAR. Most of the
!....          calculations should not include the 0's, and the array addresses
!....          reflect this.
!....          FDMCAL does the following:
!....          1- Call FDMCAR to calculate the R terms for this Tau sample.
!....          2- Call FDMCAP to recursively calculate U, V and finally the Tau
!....             sample, P(n+1).
!....          3- Shuffle AP-buffers (move up in time for the next sample) and
!....             save the new P(n+1,j) time slice.
!
               CALL vsfdmcal (pc, apnj0+1, ppnj1, ppnj2, ppn1, ppn1j1,
     +              ppn1j2, ps3, lslic2)
!
!
!....          If all time slices in the AP are done then rewrite them to file
!....          FDMX.
             IF (j .EQ. jend) THEN
               IF ( tau.eq.ntau .OR. mxslic.le.nsamp ) THEN             ! Transfer to disk
                 call putslice(jstart,nslice,nsamp, nx, apn,lunt,big)
               ENDIF
             ENDIF
  200     CONTINUE
!
        if ( (DEBUG.ne.0) .and. (mod(tau,PRSTEP).eq.0) ) then
          print '(A,I3,2(A,F8.3),A,I4,A)',
     $     'Step: ',Tau,' Tau: ',taup*dt,' - ',(tauc-1)*dt,'s ',
     $               nsamp - esamp + 1,' Samples'
          call timer
        endif
  100 CONTINUE
      RETURN
      END
