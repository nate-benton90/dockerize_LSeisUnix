      SUBROUTINE fdmied
!                                                                       
!                          PROCESS FDMIGR
!                          ------- ------
!
!  Document date:  5 October 1988
!
!                     Finite-difference Migration using the
!                              45-degree algorithm
!
!
!      The finite-difference migration technique is an effective way to
!   handle many types of migration problems.  It was developed and made
!   popular by J. F. Claerbout at Stanford University.  For most stack
!   sections, finite-difference migration gives results comparable to
!   other schemes; however there are assumptions and stability
!   limitations which must be considered.  For certain conditions,
!   frequency domain (process FKMIGR) migration is more effective in 
!   resolving typical imaging and positioning problems.
!
!     Limitations
!     ___________
!
!     Steep Dips
!       It is possible to add more terms to the finite-difference 
!  equation to obtain successively more accurate equations to deal with
!  the steep dip problem.  However, these schemes quickly become
!  impractical to implement due to their cost.  Further limitations on
!  dip angle are imposed since the finite-difference method itself
!  introduces errors.  The equation used in FDMIGR is known as the
!  45-degree equation, and is capable of handling dips up to angles
!  of 45 degrees with sufficient accuracy. 
!       A certain confusion exists regarding the meaning the meaning of
!  the dips referred to in the 45 degree equation.  This is not simply
!  the dip of continuous reflectors.  These are the dips included in all
!  events of interest as seen in the F-K domain.  A sharp fault, for
!  example, contains dips up to 90 degrees, and the 45 degree algorithm
!  will only properly migrate certain components, with increasing
!  distortion at higher dips.  The parameters in the algorithm are set
!  to suppress those dips which are poorly imaged.
!
!     Velocity
!       Within the finite-difference equation there is no term to 
!  describe differences in velocity. Hence, a major assumption of the 
!  scheme is that velocity is constant throughout the section.  In 
!  practice, it is sufficient for the velocity to vary slowly enough
!  that it looks roughly constant within the effective "aperture" of the
!  algorithm.  This aperture can be thought of as a box whose time
!  length equals one Tau-step size and whose spatial length equals the
!  effective width of a point diffraction pattern.
!
!     Boundary Effects
!       Ideally, we would like to perform migration on all of space.  But
!  in the real situation, we can only migrate a finite section of the
!  earth, so we must consider the effects of the imposed boundaries.
!  The main consideration is for the sides of the section, where we
!  normally think of the earth as simply ceasing to exist, and the 
!  events stopping. This view induces the mathematical equivalent of a
!  vertical reflection coefficient, and events which are migrated
!  towards it will be partly reflected back into the section.  In order
!  to suppress, or at least attenuate these undesirable events, a buffer
!  zone, or pad, consisting of a number of traces, is inserted at both
!  sides of the section.  The traces are set to zero before migration,
!  and the velocity is the same as the attached traces in the section.
!  Studying the padded traces after migration can sometimes yield
!  valuable information about events close to the edge of the section,
!  especially if other data in the area is available.
!
!
!     Comparison with FKMIGR
!     ______________________
!     a) Run Time
!          One of the most practical considerations when deciding which
!  migration scheme to use is the difference in cost.  Depending on the 
!  values of certain parameters used, FDMIGR can run 3 to 4 times as
!  long as FKMIGR.  Clearly, if there is no advantage in data quality to
!  be obtained, FDMIGR should not be used.
!          Both FKMIGR and FDMIGR must migrate from time zero, so both
!  processes replace the deep water delay with sufficienient zeroes.  The 
!  inserted zeroes are removed after migration so that the output traces
!  will have the same delay as the input traces.  Deep water delays do not
!  affect the migration run time.
!     b) Steep Dips
!          Use of FDMIGR will produce inaccuracies if events are dipping
!  by more than 45 degrees. FKMIGR, the frequency domain approach,
!  migrates all dips with equal accuracy.
!     c) Velocity
!          In general, FDMIGR will perform better in the presence of 
!  velocity variations, although both methods assume that velocity is
!  slowly varying.
!     d) Stability
!           While FKMIGR is very stable in almost all conditions,
!  FDMIGR uses parameters which, if mis-used, can cause the migration 
!  equation to become unstable. It is also possible to set values for
!  particular data sets in order to control noise on the output section,
!  but you should have a good understanding of finite-difference 
!  migration first. In general, the default values will produce stable
!  results.
!     e) Noise Suppression 
!          All migration algorithms tend to suppress random noise and
!  enhance coherent events.  The result is that the output section will
!  look more "mixed" than the input.  The effect will be more prominent
!  as the accuracy of the algorithm increases.  For this reason, FKMIGR
!  FKMIGR will generally look more mixed than FDMIGR, which will, in
!  turn, look more mixed than a 15 degree algorithm.
!
!     Some Important Parameters
!     _________________________
!
!       The parameter Rho is inserted into the expression for the 
!  discretization of the time derivative. This serves to counteract any
!  potential growing waves from the expression for migration, as an
!  explicit damping with time. It can be thought of as a "numerical
!  viscosity".  A value of Rho less than 1 reinforces stability.
!  However, any deviation of Rho away from 1, by at most 1 percent,
!  results in some loss of signal as well as noise.
!        In the discretization of depth, the parameter Theta is
!  introduced, with the most natural value being .500.  If Theta = 0 is
!  used, there is a tendency to overshoot on variations, whereas 
!  Theta = 1 will produce an overdamping of change.
!        To discretize the horizontal distance component, an
!  approximation to the second derivative is found by an iterative
!  method.  When the iteration is truncated, the parameter Gamma is
!  introduced, which is allowed to vary between .08 and .17, based
!  primarily on the look of migrated sections.  If Gamma is allowed to
!  increase too much more, spurious noise results.
!         In the ideal case, Tau would equal the sample rate of the
!  data, meaning that the entire section would be migrated exactly one
!  sample rate step at each pass through the section. While this scheme
!  reduces the errors, it is impractical due to the huge run-time
!  needed. In practise, Tau should be chosen in the range of 20 to 200 
!  ms. (.02 to .2 secs), with the smaller Tau values producing greater
!  accuracy.  It is possible to vary Tau vertically, and this should be
!  done in order to save run-time. Generally, the value of Tau should 
!  decrease from shallow to deep data times. This is because greater
!  accuracy is needed in the migration of the deeper events where the
!  greatest movement is taking place.
!         More detailed explanation of the origin of these parameters,
!  and some results of allowing them to vary, may be found in the paper
!  published by H. Brysk (Geophysics: May 1983).
!
!
!  PARAMETER DICTIONARY
!  --------- ----------
!  DX     - Trace separation distance.  This is the distance between reflection
!           points.  DX is a constant for the entire seismic line.
!           REQUIRED.  range 1.0 to 500.0      e.g.  dx 25
!  FNO    - The first shot/rp number the parameter list applies to.
!           Preset = the first shot/rp received.    e.g.   fno 101
!  LNO    - The last shot/rp number the parameter list applies to.
!           Preset = the last shot/rp received.    e.g.   lno 101
!  VTP    - The rms velocity to use in migration.  The rms velocity function is
!           the same as the velocity function used to moveout the data.  Given
!           as velocity-time pairs.  Velocities not specified are calculated
!           through interpolation and "straight-lining" from the ends.
!           Times must be given in seconds.
!           Preset = none    velocity range 0 to 32000
!  BPAD   - The number of zero amplitude traces to insert prior to the first
!           trace.
!           Preset = 1   range 1 to 500      e.g. bpad 10
!  EPAD   - The number of zero amplitude traces to append after the last trace.
!           Preset = 1   range 1 to 500      e.g. epad 10
!  OPAD   - A switch indicating that the pad traces (both bpad and epad) should
!           be output in addition to the migrated input.
!           Preset = no   range yes/no       e.g.   opad yes
!  NRHO   - A parameter used to control the Tau step interpolation.
!           Preset = 2.0   range 0. to 10000  
!  FCRHO  - A parameter used to control the Tau step interpolation.
!           Preset = .99   range .0001 to 1.
!  RHO    - A "hidden" migration parameter discussed above.
!           Preset = .9990   range  0 to .9999  
!  THETA  - A "hidden" migration parameter discussed above.
!           Preset = .501  range  0 to 1.0
!  GAMMA  - A "hidden" migration parameter discussed above.  Claerbout's
!           "Imaging the Earth's Interior", page 264 shows examples of
!           various gamma values.
!           Preset = .125   range  .08 to .17
!  TSTEPS - A set of time-delta-tau pairs governing the tau step size (delta-
!           tau) in the time interval terminating with the time given.
!           Up to 7 pairs of time and delta-tau may be given.  The delta-tau
!           values will be interpolated between the specified times and will be
!           "straight-lined" at the trace ends.  The units of time and 
!           delta-tau are seconds.
!           Preset = REQUIRED        e.g. tsteps .5 .2 1.0 .1
!  NX     - The total number of traces, including pads, to migrate.  The entire
!           seismic line must be transformed from TX (time-space) to XT
!           (space-time).  FDMIGR requires much extra disk I/O if the entire
!           seismic line (nx*maxsam) is larger than the computer memory
!           allocated for the transformation (the Cray does not have a virtual
!           memory).  NX does not have to be a power of 2.
!           Preset = 16384       e.g.   nx 500
!  MAXSAM - The maximum number of samples per trace, including the deep
!           water delay, to migrate.  A trace exceeding MAXSAM will be
!           truncated.
!           Preset = the number of samples plus delay of the first trace.
!  PATH   - The pathname (filename) of a scratch file FDMIGR should use
!           for the intermediate transposed data.  The purpose of this
!           parameter is to allow the user to specify the exact diskpartition
!           to use in case the "current" partition does not have enough
!           space.
!           preset = a scratch file in the current directory
!           e.g.    path /user/scratch/moreroom
!
!   
!  Copyright (C) by The Regents of The University of California, 1988
!  Written by Paul Henkart, Scripps Institution of Oceanography, La Jolla, Ca.
!  and by Veritas Seismic Processors, Ltd., Calgary, Alberta.
!  ALL RIGHTS RESERVED.
!
!  mod 25 Mar 2001 - Allow velocities between 0 & 32000 (before was 350 & 32000)
!  mod 1 Sep 2004 - Error when nx > 16384 since vfsdmc.f has that.
!  mod 27 Oct 10 - tsteps not given was set incorrectly (tsteps1 not set to 0)
!  mod 11 Jun 12 - TSTEPS warning was wrong.
!  mod 24 Mar 15 - TSTEPS warning was wrong when max tsteps were given.
!
      PARAMETER ( npars = 25 )                                          ! the number of user parameters
      PARAMETER ( maxvel = 55 )                                         ! the maximum number of velocity-time pairs
      PARAMETER ( maxdts = 10 )                                         ! the maximum number of time- delta tau pairs
      CHARACTER*80 token
      CHARACTER*6 names(npars)
      CHARACTER*1 types(npars)                                          ! the type of parameter
      REAL vals(npars)                                                  ! holds the REAL parameter values
      DIMENSION lvals(npars)                                            ! holds the INTEGER parameter values
      EQUIVALENCE (vals(1),lvals(1))                                    ! must be the same so that wrdisc scr works!
!
      DIMENSION vels(maxvel*2), dtaus(maxdts*2)
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /fdmigr/ junit, nlists, nwrds
!
      INTEGER fno, bpad, epad, opad, bclpad, eclpad
      REAL maxdip, nrho, ldx
!
      EQUIVALENCE ( lprint, lvals(1) ),
     2            ( fno, lvals(2) ),
     3            ( lno, lvals(3) ),
     4            ( dx, vals(4) ),
     5            ( maxdip, vals(5) ),
     6            ( vtp, vals(6) ),
     7            ( vdix, vals(7) ),
     8            ( bpad, lvals(8) ),
     9            ( epad, lvals(9) ),
     *            ( opad, lvals(10) ),
     1            ( velmul, vals(11) ),
     2            ( nrho, vals(12) ),
     3            ( fcrho, vals(13) ),
     4            ( rho, vals(14) ),
     5            ( theta, vals(15) ),
     6            ( gamma, vals(16) ),
     7            ( tsteps, vals(17) ),
     8            ( ntrpl, lvals(18) )
      EQUIVALENCE ( nlines, lvals(19) ),
     *            ( ldx, vals(20) ),
     1            ( bclpad, lvals(21) ),
     2            ( eclpad, lvals(22) ),
     3            ( nx, lvals(23) ),
     4            ( maxsam, lvals(24) ),
     5            ( lunt, lvals(25) )
      DATA names /'LPRINT', 'FNO   ', 'LNO   ', 'DX    ', 'MAXDIP',
     *            'VTP   ', 'VDIX  ', 'BPAD  ', 'EPAD  ', 'OPAD  ',
     *            'VELMUL', 'NRHO  ', 'FCRHO ', 'RHO   ', 'THETA ',
     *            'GAMMA ', 'TSTEPS', 'NTRPL ', 'NLINES', 'LDX   ',
     *            'BCLPAD', 'ECLPAD', 'NX    ', 'MAXSAM', 'PATH  ' /
      DATA types / 3*'L', 4*'F', 2*'L', 'A', 7*'F', 2*'L', 'F', 4*'L',
     *             'A' /
!**** 
!****    Set the parameter presets and various variable presets
!****
      lprint = 0
      fno  = 0
      lno = 0
      dx = 0.
      maxdip = 0.
      nvels = 0
      vtp = 0.
      vdix = 0.
      ivtype = 0
      bpad = 1
      epad = 1
      opad = 0.
      velmul = 1.0
      nrho = 2.
      fcrho = .99
      rho = .999
      theta = .501
      gamma = .125
      ndtaus = 0
      tsteps1 = 0.
      tsteps = 0.
      ntrpl = 0
      nlines = 1.
      ldx = 0.
      bclpad = 0
      eclpad = 0
      nx = 16384
      maxsam = 0
      lunt = 0
      CALL getfil( 1, junit, token, istat )                             ! get a file for the DISK parameters
!****
!****     get the user's parameters -  there must be something, at least an "end"
!****
      ntokes = 0                                                        ! count the tokens
  100 CONTINUE
      CALL getoke( token, nchars )                                      ! get a token and it's length
      CALL upcase( token, nchars )                                      ! convert parameter names to upper case
      IF( nchars .EQ. 0 ) THEN                                          ! anything there?
          CALL rdline                                                   ! nope, get another line
          ntokes = 0
          GOTO 100
      ENDIF
      ntokes = ntokes + 1
  110 DO 120 i = 1, npars
         nparam = i
         IF( token(1:nchars) .EQ. names(nparam) ) GOTO 150
  120 CONTINUE
      IF( token(1:nchars) .EQ. 'END' ) GOTO 200
      IF( names(lparam) .EQ. 'VTP' .OR. names(lparam) .EQ. 'VDIX'
     *  .OR. names(lparam) .EQ. 'TSTEPS' ) GOTO 160
      IF( token(1:nchars) .EQ. 'END') GOTO 200
      PRINT *,' ***  ERROR  ***  No such parameter as ',token(1:nchars)
      ierror = ierror + 1
      GOTO 100
  150 CONTINUE
      lparam = nparam
!****  
!****   Got the parameter name, now get the value
!****
      CALL getoke( token, nchars )                                      ! get the value
      ntokes = ntokes + 1
      IF( nchars .EQ. 0 ) THEN
          CALL rdline
          ntokes = 0
          GOTO 150
      ENDIF
      IF( types(nparam) .EQ. 'A' ) THEN
          IF( names(nparam) .EQ. 'OPAD' ) THEN
              CALL upcase( token, 1 )
              IF( token(1:1) .EQ. 'Y' ) opad = 1
          ENDIF
          IF( names(nparam) .EQ. 'PATH' ) THEN
              CALL getfil( 3, lunt, token(1:nchars), istat )
              IF( istat .NE. 0 ) THEN
                  PRINT *,' ***  ERROR  ***   Can not open file ',path
                  ierror = ierror + 1
              ENDIF
          ENDIF
          GOTO 100
      ENDIF
  160 CALL dcode( token, nchars, areal, istat )                         ! convert the alpha number to an internal machine number
      IF( istat .NE. 2 ) ierror = ierror + 1                            ! was the an error decoding it?
      IF( types(lparam) .EQ. 'L' ) THEN
          lvals(lparam) = areal                                         ! convert the real to INTEGER*4
      ELSE
          vals(lparam) = areal                                          ! move the real to the parameter
          IF( names(lparam) .EQ. 'VTP' ) THEN
              ivtype = 1
              nvels = nvels + 1
              vels(nvels) = areal
          ENDIF
          IF( names(nparam) .EQ. 'VDIX' ) THEN
              ivtype = 2
              nvels = nvels + 1
              vels(nvels) = areal
          ENDIF
          IF( names(lparam) .EQ. 'TSTEPS' ) THEN
              tsteps1 = 1                                               ! set it so we know it was given at least once!
              ndtaus = ndtaus + 1
              dtaus(ndtaus) = areal
          ENDIF
      ENDIF
      GOTO 100
!****
!****    Do the parameter validity checks
!****
  200 CONTINUE
      IF( nx .GT. 16384 ) THEN
          PRINT *,' ***  ERROR  ***  NX may not exceed 16384.'
          ierror = ierror + 1
      ENDIF
      IF( fno .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  FNO must be positive.'
          ierror = ierror + 1
      ENDIF
      IF( lno .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  LNO must be positive.'
          ierror = ierror + 1
      ENDIF
      IF( dx .LT. 1. .OR. dx .GT. 500. ) THEN
          PRINT *,' ***  ERROR  ***  DX must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( nvels .EQ. 0 ) THEN
          PRINT *,' ***  ERROR  ***  A velocity must be given.'
          ierror = ierror + 1
      ENDIF
      IF( ivtype .NE. lvtype .AND. nlists .GT. 1 ) THEN
          PRINT *,' ***  ERROR  ***  Only one velocity type may be ',
     *      'given.'
          ierror = ierror + 1
      ENDIF
      IF( nvels/2*2 .NE. nvels ) THEN
          PRINT *,' ***  ERROR  ***  Velocities must be in pairs.'
          ierror = ierror + 1
      ENDIF
      DO 1234 i = 1, nvels, 2
         IF( vels(i) .LT. 0. .OR. vels(i) .GT. 32000. ) THEN
             PRINT *,' ***  ERROR  ***  Velocities must be between',
     *          ' 0 and 32000.'
             ierror = ierror + 1
         ENDIF
         IF( i .NE. 1 .AND. vels(i+1) .LE. vels(i-1) ) THEN
             PRINT *,' ***  ERROR  ***  The times within the velocity ',
     *          ' time pair must increase.'
             ierror = ierror + 1
         ENDIF
         IF( i .NE. 1 .AND. vels(i) .LT. vels(i-2) ) THEN
             PRINT *,' ***  WARNING  ***  Velocity inversion.'
             iwarn = iwarn + 1
         ENDIF
 1234 CONTINUE
      IF( bpad .LT. 1 .OR. bpad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  BPAD must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( epad .LT. 1 .OR. epad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  EPAD must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( velmul .LT. .5 .OR. velmul .GT. 2.0 ) THEN
          PRINT *,' ***  ERROR  ***  VELMUL must be between .5 and 2.0'
          ierror = ierror + 1
      ENDIF
      IF( nrho .LT. 0. .OR. nrho .GT. 10000. ) THEN
          PRINT *,' ***  ERROR  ***  NRHO must be between 0 and 10000.'
          ierror = ierror + 1
      ENDIF
      IF( fcrho .LT. .0001 .OR. fcrho .GT. 1. ) THEN
          PRINT *,' ***  ERROR  ***  FCRHO must be between .0001 and ',
     *            '1.0'
          ierror = ierror + 1
      ENDIF
      IF( rho .LT. 0. .OR. rho .GT. .9999 ) THEN
          PRINT *,' ***  ERROR  ***  RHO must be between 0 and .9999'
          ierror = ierror + 1
      ENDIF
      IF( theta .LT. 0. .OR. theta .GT. 1.0 ) THEN
          PRINT *,' ***  ERROR  ***  THETA must be between 0. and 1.0'
          ierror = ierror + 1
      ENDIF
      IF( gamma .LT. .08 .OR. gamma .GT. .17 ) THEN
          PRINT *,' ***  ERROR  ***  GAMMA must be between .08 and .17'
          ierror = ierror + 1
      ENDIF
      IF( tsteps1 .EQ. 0 ) THEN
          PRINT *,' ***  ERROR  ***  TSTEPS must be given.'
          ierror = ierror + 1
      ENDIF
      IF( ndtaus .EQ. 0 ) GOTO 2346
      IF( dtaus(1) .EQ. 0 ) THEN
          PRINT *,' ***  WARNING  ***  The first TSTEPS should not be 0'
          iwarn = iwarn + 1
      ENDIF
      IF( ndtaus .LE. 3 ) THEN
        PRINT *,' ***  WARNING  ***  At least 4 TSTEPS should be given.'
          iwarn = iwarn + 1
      ENDIF
      DO 2345 i = 1, ndtaus, 2
         IF( dtaus(i) .LT. 0. .OR. dtaus(i) .GT. 20. ) THEN
             PRINT *,' ***  ERROR  ***  TSTEPS time ',dtaus(i),
     *          'is not between 0 and 20.'
             ierror = ierror + 1
         ENDIF
         IF( dtaus(i+1) .LT. .00001 .OR. dtaus(i+1) .GT. .5 ) THEN
             PRINT *,' ***  ERROR  ***  TSTEPS delta-tau ',dtaus(i+1),
     *          'is not between 0 .AND. .5'
             ierror = ierror + 1
         ENDIF
         IF( i .NE. 1 .AND. dtaus(i) .LE. dtaus(i-2) ) THEN
             PRINT *,' ***  ERROR  ***  TSTEPS times must increase.'
             ierror = ierror + 1
         ENDIF
         IF( i .LE.  ndtaus-3 .AND. dtaus(i+1) .GT. dtaus(i+3) ) THEN     
             PRINT *,' ***  WARNING  ***  ',
     *           ' tsteps tau normally increases with time.'
             iwarn = iwarn + 1
         ENDIF
         IF( dtaus(i+1) .GT. .25 ) THEN
             PRINT *,' ***  WARNING  ***  ',
     *               ' tsteps tau is normally smaller!'
             iwarn = iwarn + 1
         ENDIF
 2345 CONTINUE
 2346 CONTINUE
      IF( ntrpl .NE. 0 .AND. ntrpl .LT. 3 .OR. ntrpl .GT. 1000 ) THEN
          PRINT *,' ***  ERROR  ***  NTRPL must be between 3 and 1000.'
          ierror = ierror + 1
      ENDIF
      IF( nlines .NE. 1 .AND. nlines .LT. 3 .OR. nlines .GT. 1000 ) THEN
          PRINT *,' ***  ERROR  ***  NLINES must be between 3 and 1000.'
          ierror = ierror + 1
      ENDIF
      IF( bclpad .LT. 0 .OR. bclpad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  BCLPAD must be between 0 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( eclpad .LT. 0 .OR. eclpad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  ECLPAD must be between 0 and 500.'
          ierror = ierror + 1
      ENDIF
!****
!****    Do some more presetting
!****
      lvals(6) = nvels                                                  ! clobber vtp
      lvals(7) = ivtype                                                 ! clobber vdix
!****
!****    Write the FDMIGR parameters to a disc file  and get another list!
!****
      nwrds = npars 
      lvals(17) = ndtaus 
      CALL wrdisc( junit, lvals, nwrds )
      IF( nvels .GT. 0 ) CALL wrdisc( junit, vels, nvels )
      IF( ndtaus .GT. 0 ) CALL wrdisc( junit, dtaus, ndtaus )
      nlists = nlists + 1
      IF( nlists .GT. 198 ) THEN
         PRINT *,' ***  ERROR  ***  FDMIGR can only handle 198 control',
     *       ' points.'
          ierror = ierror + 1
      ENDIF
!
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,(lvals(i),i=1,3), (vals(i),i=4,5),lvals(6),lvals(7)
          PRINT *,(lvals(i),i=8,10), (vals(i),i=11,16)
          PRINT *,(lvals(i),i=17,19), vals(20), (lvals(i),i=21,nwrds)
          IF( nvels .NE. 0 ) PRINT *,' vels:',(vels(i),i=1,nvels)
          IF( ndtaus .NE. 0 ) PRINT *,' dtaus:',(dtaus(i),i=1,ndtaus)
      ENDIF
!****
!****    finish up the parameter reading
!****
 2000 CONTINUE
      CALL getoke( token, nchars )                                       ! get the next token
      CALL upcase( token, nchars )
      ntokes = ntokes + 1
      IF( nchars .LE. 0 ) THEN
          IF( now .EQ. 1 ) PRINT *,' <  ENTER PARAMETERS  >'
          CALL rdline                                                   ! get a new line of parameters
          ntokes = 0
          GOTO 2000
      ENDIF
      lno = 0
      nvels = 0
      ndtaus = 0
      lvtype = ivtype                                                   ! save the velocity type for the next list
      IF( token(1:nchars) .NE. 'END' .OR. nchars .NE. 3 ) GOTO 110

      RETURN
      END
