      SUBROUTINE ssmied
!                                                                       
!                          PROCESS SSMIGR
!                          ------- ------
!
!  Document date:  
!
!                        Split-Step Migration
!
!    Reference: Stoffa et al., Split-step Fourier Migration,
!                              Geophysics,55,p.410-421,1990.
!
!  This process performs a "split-step" Fourier migration on
!  stacked seismic data.  The method is designed to provide a
!  fast f-k approach to migration in laterally varying velocity
!  media.  The approach is straightforward.  The data are migrated
!  in small depth increments of dz.  For each dz a loop over a
!  frequency range fmin to fmax is performed in which the data are:
!
!  1) trasformed from f-x to f-k;
!  2) phase shifted by exp(i*dz*Kz) [Kz=csqrt(w^2*u0^2-Kx^2)]
!     using a reference slowness u0;
!  3) transformed from f-k to f-x;
!  4) phase shifted by exp(i*w*(u(x,z)-u0))
!
!  Imaging is then done at depth Zj by a sum over frequencies
!  from fmin to fmax.
!
!  Notes:
!
!  The complex square root calculation of Kz results in an exponentially
!  damped response to inhomogeneous interface waves, a difficulty in
!  other implementations of this type of phase-shift-plus-correction
!  method.
!
!  Extreme lateral velocity variations, such as seen at some
!  continental margins, may be handled by breaking the migration
!  into several overlapping panels which are later spliced together.
!
!  Big step, ZBIG:
!    You may migrate in one big step through an upper constant
!    slowness region such as the water column.  No imaging is done
!    for this region.  If there is a deep water delay in your data,
!    you may want to remove delay to negate temporal wrap-around.
!    This is most easily done through start-end-time pair in process
!    DISKIN (i.e. set 0.0 8.0)
!
!  Migration bandwidth, FMIN, FMAX:
!    Runtime increases in direct proportion to migration bandwidth.
!    It is worthwhile doing a couple of small tests to see what
!    frequencies are actually useful and adjusting the parameters
!    FMIN and FMAX accordingly.
!
!  Depth step, DELTAZ:
!    The migration depth step dz need not be tiny, but in general small
!    enough to provide unaliased sampling of the smallest vertical
!    wavenumber of interest.
!
!  Data tapers, BPAD, EPAD:
!    The user is given the option of padding either side of the data
!    panel being migrated. The padded region is filled with copies of
!    the end traces which are tapered down within the pad region. This
!    is highly recommended as it reduces Gibbs phenomena and Nyquist
!    noise.  It should be kept in mind, however, that a power of 2 FFT
!    is used and that the length of the FFT is determined as the next
!    power of 2 abover (data panel lenght)+BPAD+EPAD.  So if you are
!    migrating 1900 traces and you pad with 100 on each side, you will
!    be using a 4096 FFT for each depth step. If you had padded with 50
!    on each you would be using a 2048 FFT, much faster.
!
!  Migration taper, MTAP:
!    In addition to the tapered padding of the first and last trace, an
!    exponential taper can be applied to the ends of the spatial window
!    at each migration step.  This inhibits "wraparound" of migration
!    smiles from the data panel ends sides.  An exponental taper on the
!    order of MTAP=25 traces is recommended.  The example below should
!    clarify the issue.  It is important to note that the FFT length is
!    not based on the length of the exponential taper, only on the
!    (data panel lenght)+BPAD+EPAD, so that if care is not taken data
!    may be affected by the exponental taper.
!
!    MTAP=5
!    |<5>|                                      |<5>|
!    eeeee11111111111111111111111111111111111111eeeee  <-*1
!    tttttttdddddddddddddddddddddddddddttttttt0000000
!    |< 7 >||                         ||< 7 >|      |
!    |BPAD=7|                         |EPAD=7       |
!    |      |                         |             |
!    |      |<- data panel = 27 ----->|             |
!    |                                              |
!    |<-------- 2^n based on 27+(2*7) = 48 -------->|
!
!
!    *1) this vector is applied to each frequency at each depth
!        step.  In f-k jargon this type of taper is called a sponge.
!
!  EXAMPLE MIGRATION:
!  Shallow Sediments near ODP HOLE 504B
!  diskin
!      ipath 504B.dmo.stack.224.624 set 0.0 8.0 end
!  end 
!  ssmigr
!      deltax 12.5 deltaz 10 vskip 1500 zskip 3000 ez 10000
!      bpad 50 epad 50 mtap 25 twinlen 0.25
!      ref 0 nvsmth 3 vpath v.scratch
!      fmin 5 fmax 40 path ps.scratch sgypath vmodel.segy 
!      fno  224 lno  224 vdp 1500.0 3480.0 1850.0 3490.0 1850.0 3721.3 end
!      fno  424 lno  424 vdp 1500.0 3442.5 1850.0 3452.5 1850.0 3785.5 end
!      fno  524 lno  524 vdp 1500.0 3397.5 1850.0 3407.5 1850.0 3694.3 end
!      fno  624 lno  624 vdp 1500.0 3390.0 1850.0 3400.0 1850.0 3686.8 end 
!  end
!  
!  PARAMETER DICTIONARY
!  --------- ----------
!  EZ     - End Depth, in meters.  The number of samples output will be 
!           EDEPTH  / DELTAZ + 1.  The first output sample is ALWAYS 0.
!         - Required.  e.g.  ez 6000
!  DELTAX - The distance between traces, in meters.
!           Required.
!  DELTAZ - The output sample interval in meters per sample.  DELTAZ may
!           not exceed 32 since it is carried in the SEGY header in
!           millimeters and as a 16 bit integer (thus 32767 millimeters
!           is the maximum).  This is analogous to the sample interval
!           in time which the SEG-Y format carries as nanoseconds
!           (milliseconds / 1000).
!           Required.
C TWINLEN - Time WINdow LENgth of temporal taper at end of trace. This
C           taper is exponential in nature and is given in seconds. The
C           time taper should reduce energy migrating from truncation
C           of time trace.
C           Preset = 0.25
!  MTAP   - Migration TAPer.  The exponential spatial taper discussed above.
C           preset = 25
!  FMIN   - Minimum frequency of interest.
!           Preset = 0
!  FMAX   - Maximum frequency of interest
!         - preset = nyquist / 2  or   (4/si)
!  VDP    - The rms velocity to use in migration, given as a list of
!           Velocity Depth Pairs.  The velocity should be in units of
!           meters per second and the depth should be in units of
!           meters.
!           Required.    velocity range 350 to 32000
!  NVSMTH - This parameter specifies the number of Velocity SMooTHing
!           operations desired before constructing velocity file.
!           The VSMTH parameter is useful for smoothing across velocity
!           contrasts which otherwise can cause distortion in depth
!           migration. This is a 3 point convolutional operator.
!           Preset = 3
!  FNO    - The first shot/rp number the parameter list applies to.
!           Preset = the first shot/rp received.    e.g.   fno 101
!  LNO    - The last shot/rp number the parameter list applies to.
!           Preset = the last shot/rp received.    e.g.   lno 101
!  BPAD   - The number of zero amplitude traces to insert prior to the
!           first trace.
!           Preset = 1   range 1 to 500      e.g. bpad 10
!  EPAD   - The number of zero amplitude traces to append after the last
!           trace.
!           Preset = 1   range 1 to 500      e.g. epad 10
!  ZSKIP  - The first depth to compute.  The first output sample is
!           ALWAYS 0.  This saves computer time!  The first velocity of
!           VDP of the first FNO/LNO list is used as the velocity for
!           time to depth conversion unless parameter VSKIP is given.
!           Preset = 0.
!  VSKIP  - Velocity used in extrapolation for parameter ZSKIP, usually
!           the water velocity in marine work. If using ZSKIP, then VSKIP
!           is required.
!  REF    - Reference slowness, u0, from a given depth.  The minimum
!           slowness may provide a better reference than the average
!           slowness for imaging features such as a rough basement 
!           surface beneath low velocity sediments.
!         =0, minimum slowness.
!         =1, average slowness.
!         =2, maximum slowness.
!           Preset = 1
!  PATH   - The pathname (filename) of a scratch file SSMIGR should use
!           for the intermediate transposed data.  The purpose of this
!           parameter is to allow the user to specify the exact disk
!           partition to use in case the "current" partition does not 
!           have enough space.
!           preset = a scratch file in the current directory
!           e.g.    path /user/scratch/moreroom
!  VPATH  - The pathname (filename) of a scratch file SSMIGR should use
!           for the transposed velocity slices.  The purpose of this
!           parameter is to allow the user to specify the exact disk
!           partition to use in case the "current" partition does not 
!           have enough space.
!           preset = a scratch file in the current directory
!           e.g.    vpath /user/scratch/vmoreroomc
!  SGYPATH - The pathname (filename) of an additional SEGY compatible
!           velocity file to be output for external purposes. Includes
!           the smoothing operators.
!           preset = none
!           e.g.    sgypath /user/scratch/vmoreroomc.segy
!   
!  Copyright (C) 1995 by:
!  Woods Hole Oceanographic Institution and
!  The Regents of The University of California, 1995
!
!  Written by Dan Lizarralde, WHOI, and adapted by Paul Henkart, SIO
!  Modifications by Graham Kent & Dan Lizarralde, WHOI 
!  ALL RIGHTS RESERVED.
!
!  Present limitations include 8192 traces, 5000 depth steps & 800 frequencies.
!  If the number of depths * number of traces exceed 500,000 use sioseis.BIG
!  which allows a product of nz*nx = 3,500,000, also remember padding when
!  estimating your nx.
!  
      PARAMETER ( npars = 21 )                                          ! the number of user parameters
      PARAMETER ( maxvel = 55 )                                         ! the maximum number of velocity-depth pairs
      CHARACTER*80 token
      CHARACTER*7 names(npars)
      CHARACTER*1 types(npars)                                          ! the type of parameter
      REAL vals(npars)                                                  ! holds the REAL parameter values
      DIMENSION lvals(npars)                                            ! holds the INTEGER parameter values
      EQUIVALENCE (vals(1),lvals(1))                                    ! must be the same so that wrdisc scr works!
      DIMENSION vels(maxvel*2)
!
      COMMON /edits/ ierror, iwarn, irun, now, icompt
      COMMON /ssmigr/ junit, nlists, nwrds
!
      INTEGER fno, bpad, epad, ref, vdp, vintpl
      INTEGER nvsmth, mtap
!
      EQUIVALENCE ( lprint, lvals(1) ),
     2            ( fno, lvals(2) ),
     3            ( lno, lvals(3) ),
     4            ( nvels, lvals(4) ),
     5            ( bpad, lvals(5) ),
     6            ( epad, lvals(6) ),
     7            ( zskip, vals(7) ),
     8            ( fmin, vals(8) ),
     9            ( fmax, vals(9) ),
     *            ( deltax, vals(10) ),
     1            ( deltaz, vals(11) ),
     2            ( ez, vals(12) ),                                     ! I couldn't resist
     3            ( lunscr, lvals(13) ),
     4            ( lunvel, lvals(14) ),
     5            ( vskip, vals(15) ),
     6            ( ref, lvals(16) ),
     7            ( vintpl, lvals(17) ),
     8            ( nvsmth, lvals(18) ),
     9            ( twinlen, vals(19) )
      EQUIVALENCE ( mtap, lvals(20) ),
     1            ( lunsegy, lvals(21) )
     
      DATA names /'LPRINT ', 'FNO    ', 'LNO    ', 'VDP    ', 'BPAD   ',
     1            'EPAD   ', 'ZSKIP  ', 'FMIN   ', 'FMAX   ', 'DELTAX ',
     2            'DELTAZ ', 'EZ     ', 'PATH   ', 'VPATH  ', 'VSKIP  ',
     3            'REF    ', 'VINTPL ', 'NVSMTH ', 'TWINLEN', 'MTAP   ',
     4            'SGYPATH'/
      DATA types / 6*'L',6*'F',2*'A','F',2*'L','L','F','L','A'/    
!**** 
!****    Set the parameter presets and various variable presets
!****
      lprint = 0
      fno  = 0
      lno = 0
      nvels = 0
      bpad = 0
      epad = 0
      zskip = 0.
      fmin = 0.
      fmax = 0.
      deltax = 0.
      deltaz = 0.
      ez = 0.
      lunscr = 0
      lunvel = 0
      lunsegy = 0
      nz = 0
      vskip = 0.
      ref = 1
      vdp = 0
      vintpl = 1
      vsmth = 3
      twinlen = .25
      mtap = 25
      CALL getfil( 1, junit, token, istat )                             ! get a file for the DISK parameters
!****
!****     get the user's parameters
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
      IF( vdp .EQ. 1 ) GOTO 160
      PRINT *,' ***  ERROR  ***  No such parameter as ',token(1:nchars)
      ierror = ierror + 1
      GOTO 100
  150 CONTINUE
      vdp = 0
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
          IF( names(nparam) .EQ. 'PATH' ) THEN
              CALL getfil( 3, lunscr, token(1:nchars), istat )
              IF( istat .NE. 0 ) THEN
                  PRINT *,' ***  ERROR  ***   Can not open file ',
     &                  token(1:nchars)
                  ierror = ierror + 1
              ENDIF
          ENDIF
          IF( names(nparam) .EQ. 'VPATH' ) THEN
              CALL getfil( 3, lunvel,  token(1:nchars), istat )         ! create the velocity file
                  IF( istat .NE. 0 ) THEN
                    PRINT *,' ***  ERROR  ***   Can not open file ',
     &                  token(1:nchars)
                    ierror = ierror + 1
                  ENDIF
          ENDIF
           IF( names(nparam) .EQ. 'SGYPATH' ) THEN
               CALL getfil( 3, lunsegy,  token(1:nchars), istat )        ! create segy velocity file
                  IF( istat .NE. 0 ) THEN
                    PRINT *,' ***  ERROR  ***   Can not open file ',
     &                  token(1:nchars)
                    ierror = ierror + 1
                  ENDIF
          ENDIF         
          GOTO 100
      ENDIF
  160 CALL dcode( token, nchars, areal, istat )                         ! convert the alpha number to an internal machine number
      IF( istat .NE. 2 ) ierror = ierror + 1                            ! was the an error decoding it?
      IF( names(lparam) .EQ. 'VDP' ) THEN
          nvels = nvels + 1
          vels(nvels) = areal
          vdp = 1
          GOTO 100
      ENDIF
      IF( types(lparam) .EQ. 'L' ) THEN
          lvals(lparam) = areal                                         ! convert the real to INTEGER*4
      ELSE
          vals(lparam) = areal                                          ! move the real to the parameter
      ENDIF
      GOTO 100
!****
!****    Do the parameter validity checks
!****
  200 CONTINUE
      IF( fno .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  FNO must be positive.'
          ierror = ierror + 1
      ENDIF
      IF( lno .LT. 0 ) THEN
          PRINT *,' ***  ERROR  ***  LNO must be positive.'
          ierror = ierror + 1
      ENDIF
      IF( deltax .LT. 1. .OR. deltax .GT. 500. ) THEN
          PRINT *,' ***  ERROR  ***  DELTAX must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( nvels .EQ. 0 ) THEN
          PRINT *,' ***  ERROR  ***  A velocity function must be given.'
          ierror = ierror + 1
      ENDIF
      IF( ref .LT. 0 .OR. ref .GT. 2 ) THEN
          PRINT *,' ***  ERROR  ***  REF must be 0, 1, or 2'
          ierror = ierror + 1
      ENDIF
      IF( nvels/2*2 .NE. nvels ) THEN
          PRINT *,' ***  ERROR  ***  Velocities must be in pairs.'
          ierror = ierror + 1
      ENDIF
      DO 1234 i = 1, nvels, 2
         IF( vels(i) .LT. 350. .OR. vels(i) .GT. 32000. ) THEN
             PRINT *,' ***  ERROR  ***  Velocities must be between',
     *          ' 350 and 32000.'
             ierror = ierror + 1
         ENDIF
         IF( i .NE. 1 .AND. vels(i+1) .LE. vels(i-1) ) THEN
             PRINT *,' ***  ERROR  ***  The depths within the velocity',
     *          ' depth pair must increase.'
             ierror = ierror + 1
         ENDIF
 1234 CONTINUE
      IF( bpad .LT. 0 .OR. bpad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  BPAD must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( epad .LT. 0 .OR. epad .GT. 500 ) THEN
          PRINT *,' ***  ERROR  ***  EPAD must be between 1 and 500.'
          ierror = ierror + 1
      ENDIF
      IF( deltaz .LT. 1. .OR. deltaz .GT. 32. ) THEN
          PRINT *,' ***  WARNING  *** DELTAZ must be between 1. and 32.'
          iwarn = iwarn + 1
      ENDIF
      IF( ez .LT. 100 .OR. ez .GT. 30000 ) THEN
          PRINT *,' ***  ERROR  *** EZ must be between 100 & 30000'
          ierror = ierror + 1
      ENDIF
!****
!****    Write the parameters to a disc file  and get another list!
!****
      IF( lno .EQ. 0 ) lno = fno
      nwrds = npars 
      CALL wrdisc( junit, lvals, nwrds )
      IF( nvels .GT. 0 ) CALL wrdisc( junit, vels, nvels )
      nlists = nlists + 1
!
      IF( IAND(lprint,1) .NE. 0 ) THEN
          PRINT *,(lvals(i),i=1,6)
          PRINT *,(vals(i),i=7,12)
          PRINT *,(lvals(i), i=13,npars)
          IF( nvels .NE. 0 ) PRINT *,' vels:',(vels(i),i=1,nvels)
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
      IF( token(1:nchars) .NE. 'END' .OR. nchars .NE. 3 ) GOTO 110

      RETURN
      END
