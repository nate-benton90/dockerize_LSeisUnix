c--------------------------------------------------------------------

cc List of variables:

c  A0(,),A1(,)  Cubic spline coefficients of the interfaces.  First index
c  A2(,),A3(,)  defines the interface, second index the portion of the 
c               interface.
c  AMP      Amplitude of the wave (wavelet is scaled by this amount)
c  AMP1,2   Part of the amplitude that can be calculated from knowledge
c           of the raypath alone
c  BEGPLT   TRUE to begin plotting
c  BELAST   Takeoff angle of last good ray found by continuation (this
c           ray does not necessarily end at a receiver location)
c  BETA     Takeoff angle at the source
c  BETA1,2,3  Values of three adjacent rays making up the ray tube 
c           in the amplitude calculatiuon
c  BETAF    Final takeoff angle in shooting search
c  BETAI    Initial takeoff angle in shooting search
c  BETNEW   Takeoff angle of new ray found by continuation procedure
c  CARD     Character record read from geometry file (depending on the
c           record, either land or marine shooting is specified)
c  CAUSTC   True if the ray has passed through a caustic
c  COLORS   Name of file containing plot colors
c  COLSIN   Unit number of plot colors file
c  CONST    Constant used in the amplitude calculation
c  D()      Length of ray segment
c  DBETA    Change in BETA for raya making up the ray tube
c  DBMAX    Max allowed change in BETA. If DBMAX and DX1MAX exceeded
c           then suspect a missing branch of ray solutions
c  DDZ()    Second derivative of each interface at intersection point
c  DELTAB   Increment in takeoff angle in shooting search
c  DELTAX() x-distance travelled within a layer
c  DELTAZ() z-distance travelled within a layer
c  DETJ     Value of the determinant of the jacobian
c  DOWNHL   TRUE if shooting in downhole mode
c  DSRC     Spacing (arc length) between sources down the well
c  DUMML    Logical dummy variable (value not used)  
c  DUMMY    Real dummy variable (value not used)
c  DX1      Change in X(1) from one ray to next.  Used, along with the 
c           change in takeoff angle, to check for a missing branch - both
c           will be large when a branch has been skipped.
c           (When a source is located near an interface the change in
c           takeoff angle may be large even though no branches are
c           missing.  DX1, on the other hand, will not be large.)
c  DX1FAC   DX1FAC times TRUGEO is the maximum allowed change in DX1
c  DX1MAX   Maximum allowed change in DX1 (when DX1MAX and DBMAX are
c           exceeded then program suspects a missing branch)
c  DZ()     First derivative of each interface at intersection point
c  EOF      TRUE if end of file has been reached (cards file)
c  EVENT    Character variable.  May be the list of refracting interfaces
c           or a list of reflectors making up an extra event (e.g. a multiple)
c  EVTYPE() Character array describing type of event (direct, head wave,
c           or reflection
c  FIRST    TRUE if this is the first ray in a branch of solutions
c  FSHOT    Shot location, referenced to receiver stations.  FSHOT is a float,
c           thus allowing the shots to be located between receiver stations.
c  GEOINC   x-distance between end point of ray and next receiver
c  GEOMFL   Name of file containing shot cards
c  GEOMS    Unit number of shot cards file
c  GEOZ     z-distance between end point of ray and next receiver
c  HEAD     TRUE if this is a headwave
c  I        Loop variable
c  ICOLOR() Integer array containing colors for the plot
c  IDUMMY   Dummy integer variable (value not used)
c  IEVENT   Event number
c  IHEAD    Number of head waves counter
c  INGAP    TRUE if current station lies within the gap
c  INSIDE   TRUE if station lies inside limits of model
c  INTERF   Unit number of model file
c  IPEN     Pen color for plotting
c  IREC     Receiver number or counter
c  IRECD    Shot (record) counter
c  IRECP    Receiver number of previous ray
c  IREFL(,) First dimension is the event number.  Second dimension 
c           is a list of the reflecting interfaces met by the ray.
c  ITRACE   Trace number
c  J        Loop variable
c  JOBTYP   Job descriptor. May be r or R for ray plot, l or L for 
c           listing,  or T for time section
c  JUMP     TRUE if a missing branch of solution is suspected (ie if
c           DBMAX and DX1MAX have both been exceeded)
c  K        Loop variable
c  LAND     TRUE if land-type shooting (depends on how geometry is described)
c  LIST     TRUE if listings are to be generated
c  MARINE   TRUE if marine-type shooting (depends on how geometry is described)
c  MAXEVT   Maximum number of events / shot
c  MAXINT   Maximum number of interfaces in the model
c  MAXN     MAximum value of N
c  MAXNP1   MAXN plus 1
c  MAXNP3   MAXN plus 3
c  MAXREC   Maximum number of receivers / shot
c  MAXREF   Maximum number of reflections in any event
c  MAXSPL   Maximum number of points defining an interface
c  MAXSRC   Maximum number of shots in the job
c  MODE     Shooting mode: surface (s or S) or downhole (d or D)
c  MODEL    Name of the file containing the interface coordinates
c  MU       Move-up in number of stations from first shot (marine shooting)
c  MXSPM1   MAXSPL minus one
c  N        Number of intersection points between source and receiver
c  NCHARC   Number of characters in OUTNAM (up to first blank)
c  NDUMMY   Dummy integer (value not used)
c  NEVENT   Number of events in the shot (some may be invalid)
c  NGAP     Number of stations in the gap
c  NHEADS   Number of head wave events / shot
c  NINT     Number of interfaces in the model (not counting the upper surface)
c  NNUM     Number of integers and floats in the character variable CARD
c  NOCONV   TRUE if can't find ray or ray exits line
c  NORDER() Order of intersections in current event.
c  NPTS()   Number of points defining each interface.
c  NREC     Number of receivers for this shot
c  NREFER   Reference station number
c  NREFLS() Number of reflections that occur within each event
c  NSRC     Number of shots
c  NTR1B    Station number of firts receiver for this shot
c  NTR1F    Station number of first receiver after the gap
c  NTRABS   Station number of this receiver or trace
c  NTRMAX   Maximum receiver station number
c  NTRMIN   Minimum receiver station number
c  NTRNB    Station number of last receiver before the gap
c  NTRNF    Station number of last receiver for this shot
c  NTRREC   Number of traces/shot (must be the same for all shots
c           when generating shot data)
c  NWELL    Number of points defining the well
c  OUTFIL   Full output file name
c  OUTNAM   First part of name given to all output files
c  PARIN    Unit number of file PARAM
c  PHASE1,2 Phase due to postcritical reflections and caustics
c  PI       3.1415927
c  PIXPI    Number of pixels/inch on the screen
c  PLTGEO   If TRUE then the receiver locations will be plotted    
c  PLTMOD   TRUE if the model is to be plotted
c  PLTRAY   TRUE if rays are o be plotted
c  PLTSRC   TRUE if source locations are to be plotted
c  PLTWEL   TRUE if well is to be plotted
c  PLTYPE   PLot descriptor
c  QTPLOT   Quit plotting after first plot descriptor
c  QTPLT2   Quit plotting after second plot descriptor
c  R        Length of straight raypath 
c  RAYLST   Unit number of listing file
c  RAYOUT   Unit number of ray data file
c  RAYTRC   TRUE if ray tracing will be necessary
c  RDGEOM   TRUE if shot cards are to be read
c  RECDPT   Depth of (all) receivers below the upper surface
c  S1       Depth (arc length) down the well to the first source location
c  SHTDPT   Shot depth below upper surface
c  SHTOUT   Unit number of shot data file (this data used by CSHOT2)
c  SHTREC   TRUE to generate shot data (used by cshot2 to build time section)
c  SIGN()   Indicates the direction of the ray leaving each intersection
c           point. 1. if in the direction of the upward normal to the
c           interface. Otherwise, -1.
c  SLAYER() Integer array giving layer number of each source location
c  SPACIN   Sets the x-width of the ray tube as 1 or 2 receiver spacings
c  SPFAIL   TRUE if cannot fit spline through interfaces or well
c  STDERR   Unit number of all error messages
c  SURFAC   TRUE for surface shooting; FALSE for downhole shooting
c  TCOEFF   Transmission effects along a ray (used in headwave calculation)
c  TIME1,2  Traveltime along a ray
c  TINFO    True if traveltimes are to be calculated
c  TOTLMU   Total move-up in ft or m from first shot in marine shooting
c  TRUGEO   Station spacing 
c  V()      Interval velocity on each ray segment
c  VALID    TRUE if this is a valid event
c  VEL      Interval velocities of the layers. VEL(1) is the velocity
c           of the shallowest layer.
c  VINT(,)  velocities of the layers at points defining each interface 
c  VREF()   Velocities of the layers from which the ray reflects
c  W0,W1,W2,W3  Cubic spline coefficients of the well
c  WELL     Unit number of file describing the well and downhole sources
c  WELLFL   Name of file containing well coordinates and source locations
c  WELOPN   TRUE if the well file is to be opened
c  X()      x-coordinates of ray intersections with interfaces
c  X1LAST   x(1) of last good ray
c  XDIM     x dimension of plot in inches
c  XEND     Maximum x-coordinate in model (taken from upper surface)
c  XINT(,)  x-coordinates of points defining each interface
c  XLAST    x-coordinate of last good ray found by continuation
c           method.  This ray has takeoff angle BELAST.
c  XR       x-coordinate of receiver (for plotting)
c  XREC()   x-coordinates of the receivers for this shot
c  XREF0    x-coordinate of station number zero
c  XREFER   x-coordinate of station reference station NREFER 
c  XRMAX    Maximum receiver x-coordinate
c  XRMIN    Minimum receiver x-coordinate
c  XS()     x-coordinates of sources
c  XSCRN    x-dimension of screen in inches
c  XSTART   Minimum x-coordinate in model (taken from upper surface)
c  XWELL()  Array of x-coordinates defining the well
c  YORN     Character - y or n
c  Z        z-coordinates of ray intersections with interfaces
c  ZDIM     x dimension of plot in inches
c  ZINT(,)  z-coordinates of points defining each interface 
c  ZR       z-coordinate of receiver (for plotting)
c  ZREC()   z-coordinates of the receivers for this shot
c  ZS()     z-coordinates of sources
c  ZSCRN    z-dimension of screen in inches
c  ZWELL()  Array of z-coordinates defining the well

c-----------------------------------------------------------------------


      INTEGER     PARIN,   SHTOUT,  INTERF,  RAYOUT,  RAYLST,
     :            colsin,  well,    geoms,   stderr

      PARAMETER ( stderr = 0,
     :            PARIN  = 10,
     :            INTERF = 12,
     :            SHTOUT = 13,
     :            RAYOUT = 15,
     :            RAYLST = 16,
     :            colsin = 17,
     :            well   = 18,
     :            geoms  = 21)


      INTEGER    MAXINT,       MAXSPL,         MXSPM1,
     :           MAXN,         MAXNP1,         MAXNP3,
     :           MAXREF,       MAXEVT,         maxrec,     maxsrc

      PARAMETER ( MAXINT = 50,
     :            MAXSPL = 101,
     :            MAXN   = 100,
     :            MAXREF = 50,
     :            MAXEVT = 50,
     :            maxrec = 1024,
     :            maxsrc = 2000)

      PARAMETER ( MAXNP1 = MAXN + 1,
     :            MAXNP3 = MAXN + 3,
     :            MXSPM1 = MAXSPL - 1)

      REAL        XINT(0:MAXINT,MAXSPL),     ZINT(0:MAXINT,MAXSPL)
      REAL        VINT(0:MAXINT,MAXSPL)
      REAL*8      A0(0:MAXINT,MXSPM1),       A1(0:MAXINT,MXSPM1),
     :            A2(0:MAXINT,MXSPM1),       A3(0:MAXINT,MXSPM1)
      REAL        SIGN(0:MAXN)

      INTEGER     NPTS(0:MAXINT),   NINT,      NORDER(MAXN)


      REAL*8     DZ(MAXN),	DDZ(MAXN),
     :           DELTAX(MAXNP1),DELTAZ(MAXNP1)
      REAL       D(MAXNP1),	V(MAXNP1) 

      INTEGER    N

      REAL       X(0:MAXNP3),       z(0:maxnp3),
     :           VEL(0:MAXINT+1),   VREF(MAXREF),
     :           xrec(maxrec),      zrec(maxrec),
     :           xwell(0:maxspl),   zwell(0:maxspl),
     :           xs(maxsrc),        zs(maxsrc)
      REAL*8     w0(mxspm1),        w1(mxspm1),
     :           w2(mxspm1),        w3(mxspm1)

      REAL*8    C(MXSPM1),               D2(MXSPM1),
     :          E(MXSPM1),               B(MXSPM1),
     :          CV(0:MXSPM1)
      REAL      SAVEX(MAXNP1),  DXDL(MAXN)

      REAL*8    SUBDJ(MAXN),     DJ(MAXN),
     :          SUPDJ(MAXN),     PHI(MAXN),
     :          DPHIDR(MAXN),	 DUMMY(MAXN)
      REAL      xold(maxn/2),   COSINC(MAXN),   COSTRA(MAXN)


      REAL       AMP,       AMP1,       AMP2,        r,
     :           BELAST,    BETA,       BETAI,       BETAF,
     :           BETA1,     BETA2,      BETA3,       BETNEW,
     :           CONST,     DBETA,      DBMAX,       dx1fac,
     :           DELTAB,    PI,          dx1max,
     :           GEOINC,    PHASE1,     PHASE2,      geoz,
     :           SPACIN,    TIME1,      TIME2,       tcoeff,
     :           TRUGEO,    XEND,       XLAST,       XSTART,
     :           x1last,    xdim,       zdim,        s1,
     :           dsrc,      xrefer,     recdpt,      xref0,
     :           shtdpt,    xrmin,      xrmax,      
     :           xr,        zr,
     :           dx1,       rdummy,      fshot,       fmove,
     :           totlmu

      REAL*8     DETJ

      integer    irefl(maxevt,0:maxref),  nrefls(maxevt),
     :           icolor(7),        slayer(maxsrc)

      INTEGER    I,       K,       J,        IEVENT,   ipen,
     :           NCHARC,  NDUMMY,  NEVENT,   NREC,     nwell,
     :           nsrc,    nrefer,  ntrmin,   ntrmax,   
     :           ntr1b,   ntrnb,   ntr1f,    ntrnf,    
     :           ntrrec,  nheads,  ihead,    idummy,
     :           irecd,   ngap,    itrace,   irec,     ntrabs,
     :           irecp,   mu,      nnum


      CHARACTER   MODEL*40,    OUTNAM*40,   OUTFIL*40,
     :            geomfl*40,   event*30,
     :            YORN*1,      colors*40,   wellfl*40,   pltype*3,
     :            jobtyp*3,    mode*3,      card*81

      character evtype(maxevt)*1
     

      LOGICAL     NOCONV,     CAUSTC,       
     :            FIRST,      raytrc,       jump,
     :            pltmod,     pltwel,       pltgeo,        pltsrc,
     :            pltray,     begplt,       qtplot,        list,
     :            shtrec,     qtplt2,       welopn,
     :            downhl,     surfac,       rdgeom,
     :            dumml,      inside,       eof,           ingap,
     :            valid,      tinfo,        head,          spfail,
     :            land,       marine

      INTEGER ivch, ret, getparstring
      CHARACTER * 80 PARAM1

      PARAMETER ( PI = 3.141592653589)
      PARAMETER ( DBMAX  = 10.,
     :            DX1FAC = 2.)


ccc   Begin...
c
      ret = getparstring('param1',PARAM1) 
      IF( ret .eq. 0 ) THEN 
        WRITE(stderr,'(1x,A)') 'Can''t get parameter param1.'
        STOP
      ENDIF

c     Reading from the file PARAM1.
      OPEN(UNIT=PARIN,FILE=PARAM1,STATUS ='old',ERR=5)
      GO TO 10
5     WRITE(stderr,'(1x,A,1x,A)') 'Can''t open file param1.',PARAM1
      STOP

10    REWIND PARIN

      READ(PARIN,'(A)') MODEL
      OPEN(UNIT=INTERF,FILE=MODEL,STATUS='old',ERR=15)
      GO TO 20
15    WRITE(stderr,'(1x,A)') 'Can''t open file :',MODEL
      STOP

20    REWIND INTERF

      READ(PARIN,*) NINT
      IF(NINT.GT.MAXINT) THEN
         WRITE(stderr,'(1x,A)') 'MAIN : too many interfaces'
         STOP
      END IF

c     Reading the points defining each interface.
c     A large and negative z value defines the end of an interface.

      DO 30 I = 0,  NINT

         J = 1
c  added variable velocities within interfaces by z. li 
25       READ(INTERF,*) XINT(I,J),ZINT(I,J),VINT(I,J)
         IF(ZINT(I,J).LT.-9999.) THEN
            NPTS(I) = J - 1
            IF(NPTS(I).LT.2) THEN
               WRITE(stderr,'(1x,A,I2)')
     :        'MAIN : not enough points defining interface ',I
               STOP
            END IF
         ELSE
            J = J + 1
            IF(J.GT.MAXSPL) THEN
               WRITE(stderr,'(1x,A,I2)')
     :        'MAIN : too many enough points defining interface ',I
               WRITE(stderr,'(1x,A,I3)')
     :        'Maximum points/interface = ',MAXSPL
               STOP
            END IF
            GO TO 25
         END IF

30       CONTINUE
      READ(INTERF,*) ivch

c     edges of the model (taken as edges of upper surface)
      xstart = xint(0,1)
      xend   = xint(0,npts(0))     

c-----------------------------------------------------------
      READ(PARIN,'(A)') colors

      OPEN(UNIT=colsin,FILE=colors,STATUS='old',ERR=32)
      GO TO 35
32    WRITE(stderr,'(1x,A)') 'MAIN: Can''t open file :',colors
      STOP

35    REWIND colsin
      do 40 i = 1,  6
         read(colsin,*,end=48) icolor(i)
40       continue
      go to 50

48    WRITE(stderr,'(1x,a,1x,a)') 
     : 'MAIN: not enough colors defined in file',colors
      stop

50    continue

c------------------------------------------------------------
c     Calculating the cubic spline coefficients of each interface.
      CALL CUSPLN(NINT,XINT,ZINT,NPTS,A0,A1,A2,A3,spfail,
     1            MAXINT,MXSPM1,C,D2,E,B,CV)
      if(spfail) then
         write(stderr,'(1x,a)') 
     :   'MAIN: Failed to fit spline to interfaces.'
         stop
      end if

c     Read plot descriptor 
      READ(PARIN,'(A)') pltype
      begplt = .false.
      call setvar(pltype,pltmod,pltwel,qtplot,begplt,
     :'m','M','w','W','q','Q')


c     Read filename containing well description
      read(parin,'(a)',end=100) wellfl

      if(pltwel) welopn = .true.
      if(qtplot) go to 100

c     Read shooting mode  
      READ(PARIN,'(A)',end=100) mode
      call setvar(mode,downhl,surfac,dumml,dumml,
     :'d','D','s','S',' ',' ')
      if(downhl) then
         welopn = .true.
      else if(surfac) then
      else
         write(stderr,'(1x,a)')
     :   'MAIN: Invalid shooting mode'
         stop
      end if


c     Read receiver geometries file
      READ(PARIN,'(A)',end=100) geomfl

c     Read second plot descriptor 
      READ(PARIN,'(A)',end=100) pltype
      call setvar(pltype,pltsrc,pltgeo,qtplt2,begplt,
     :'s','S','g','G','q','Q')
      if(pltgeo) rdgeom = .true.
      if(qtplt2) go to 100

c     Read job descriptor 
      READ(PARIN,'(A)',end=100) jobtyp
      call setvar(jobtyp,list,pltray,shtrec,dumml,
     :'l','L','r','R','t','T')
      if(list)   rdgeom = .true.
      if(shtrec) rdgeom = .true.
      if(pltray) rdgeom = .true.
      if(pltray) begplt = .true.
      if(list)   tinfo = .true.
      if(shtrec) tinfo = .true.


100   continue

      if(welopn) then
        
         open(unit=well,file=wellfl,status='old',ERR=150)
         GO TO 155
150      WRITE(stderr,'(1x,A,a)') 'MAIN: Can''t open well file :',wellfl
         stop

155      rewind well
c        Read the x,z coordinates of the well 
c        track the number of points defining the well
         nwell = 0
         zwell(0) = 0.
         read(well,*,end=180) xwell(0)
170      if(zwell(nwell).gt.-9999.) then
            nwell = nwell + 1
            if(nwell.gt.maxspl) then
               write(stderr,'(1x,a,i2)') 
     :         'MAIN: Too many points defining the well - max is:',
     :          maxspl
               stop
            end if
            read(well,*,end=180) xwell(nwell),zwell(nwell)
            go to 170
         end if

180      if(nwell.lt.2) then
            write(stderr,'(1x,a,a)') 
     :      'MAIN: Not enough points defining the well in file :',
     :       wellfl
            stop
         end if

c        calculate z-coordinate of well at surface
         if(xwell(0).le.xstart.or.xwell(0).gt.xend) then
c          well outside model
           write(stderr,'(1x,a)') 'MAIN: well outside model'
           stop
         end if
         j = 1
185      if(xwell(0).gt.xint(0,j)) then
            j = j + 1
            go to 185
         end if
         j = j - 1
         zwell(0) = a0(0,j) + a1(0,j) * xwell(0)
     :                      + a2(0,j) * xwell(0)**2
     :                      + a3(0,j) * xwell(0)**3

c        fit cubic spline to well
         call cusplw(zwell,xwell,nwell,w0,w1,w2,w3,spfail,
     1                  MXSPM1,C,D2,E,B,CV)
         if(spfail) then
            write(stderr,'(1x,a)') 
     :      'MAIN: Failed to fit spline to well.'
            stop
         end if

         if(downhl) then
c           downhole mode
c           source locations specified in well file
            read(well,*) s1
            read(well,*) nsrc, dsrc
            if(nsrc.gt.maxsrc) then
               write(stderr,'(1x,a,i3)') 
     :         'MAIN: too many sources in the well - max is: ',
     :         maxsrc
               stop
            end if
           
c           Calculate x-z coordinates of sources in well
            call xzsrc(s1,nsrc,dsrc,zwell,nwell,w0,w1,w2,w3,
     :                 xs,zs,stderr)
c           Find which layer each source is in
            do 190 j = 1,  nsrc
               call layer(xs(j),zs(j),slayer(j),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
               if(.not.inside) then
                  write(stderr,'(1x,a,i4)')
     :            'MAIN: Can''t find source layer - source #',j
                   stop
               end if
190            continue
            
         end if

      end if
        

      if(rdgeom) then
c        read shooting gemetry

         open(unit=geoms,file=geomfl,STATUS='old',err=200)
         rewind geoms
         go to 205
200      write(stderr,'(1x,a)')'MAIN: Can''t open geometry file'
         stop
205      continue

c        read location of a reference group
         read(geoms,*) nrefer, xrefer
c        read receiver spacing and receiver depth
         READ(geoms,*) TRUGEO,recdpt
c        Next is the maximum change in x(1) from one ray
c        to the next - if a branch of solutions is missed
c        then the change in x(1) should be large and we can
c        detect the jump
         dx1max = dx1fac * trugeo

         xref0 = xrefer - trugeo * nrefer

c        count number of cards in file
c        if surface shooting then calculate source z-coord. and source layer
         if(surfac) then
            read(geoms,*,end=215) ntr1b,ntrnb,ntr1f,ntrnf,fshot,shtdpt
            nsrc = 1
            xs(1) = xref0 + fshot * trugeo
            call elevs(xs(1),1,shtdpt,zs(1),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
            if(.not.inside) then
               write(stderr,'(1x,a,f6.2)')
     :         'MAIN: Shot located outside model - at station ',
     :          fshot
                stop
            end if
            call layer(xs(1),zs(1),slayer(1),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
            if(.not.inside) then
c              this only happens if source is deep in model
c              and interfaces are not fully defined - can't tell
c              which layer source is in
               write(stderr,'(1x,a,f6.2)')
     :         'MAIN: Can''t find source layer - source at station ',
     :          fshot
                stop
            end if
         else
c           Downhole shooting mode - read receiver layout only (sources
c           are decribed in well file)
            read(geoms,*,end=215) ntr1b,ntrnb,ntr1f,ntrnf
         end if

c        track min and max station numbers
         ntrmin = ntr1b
         ntrmax = ntrnf

         go to 220

215      write(stderr,'(1x,a,1x,a)')'No shot cards defined in file',
     :   geomfl
         stop

c        number of traces per record
220      ntrrec = ntrnb - ntr1b  +  ntrnf - ntr1f  +  2
         if(ntrrec.gt.maxrec) then
            write(stderr,'(1x,a,i4,a)') 
     :      'MAIN: too many receivers/shot - max is: ',
     :      maxrec,' - check first geometry card.'
            stop
         end if


c        For surface shooting there are 3 possibilities at this point:
c        (1) Only 1 shot (ie, 1 card) is defined in the file
c        (2) There are many shots, each described by a card
c        (3) The number of shots and shot moveup is defined by the 
c            next (which is the last) record of the file.
c        Cases (1) and (2) are considered land shooting.
c        We can think of Case (3) as marine shooting (or a quick way
c        to specify a land survey where the recording geometry is fixed).
c        The next piece of code is a patch to allow for Case 3.

c        first assume land shooting
c        check to see what comes next in geometry file 
         land = .true.
         marine = .false.
         if(surfac) then
c           read in next record as a character variable
            read(geoms,'(a)',end=235) card
c           find out how many numbers have been specified
            call chkcrd(card,nnum)
c           if less than 2 numbers are in the record then treat as eof
            if(nnum.lt.2) go to 235
            backspace geoms
            if(nnum.ge.6) then
c              assume land shooting
            else
c              assume marine shooting
c              try to read #shots and moveup
               read(geoms,*,err=235,end=235)nsrc,fmove
c              successfull read, this is marine shooting
               marine = .true.
               land = .false.
            end if
         end if

224      continue


         if(surfac.and.marine) then
c           marine shooting
c           set coordinates of sources, check that receivers stay in model
            if(nsrc.gt.maxsrc) then
               write(stderr,'(1x,a,i3)') 
     :         'MAIN: too many sources - max is: ',
     :         maxsrc
               stop
            end if
            do 226 i = 2,  nsrc 
               xs(i) = xs(i-1) + fmove
               call elevs(xs(i),1,shtdpt,zs(i),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
               if(.not.inside) then
                  write(stderr,'(1x,a,i4)')
     :            'MAIN: Shot located outside model - source #',i
                   stop
               end if
               call layer(xs(i),zs(i),slayer(i),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
               if(.not.inside) then
                  write(stderr,'(1x,a,i4)')
     :            'MAIN: Can''t find source layer - source #',i
                   stop
               end if
226            continue
         end if


         if(surfac.and.land) then
230         read(geoms,*,end=231)ntr1b,ntrnb,ntr1f,ntrnf,fshot,shtdpt
            nsrc = nsrc + 1
            if(nsrc.gt.maxsrc) then
               write(stderr,'(1x,a,i3)') 
     :         'MAIN: too many sources - max is: ',
     :         maxsrc
               stop
            end if
            ntrrec = ntrnb - ntr1b  +  ntrnf - ntr1f  +  2
            if(ntrrec.gt.maxrec) then
               write(stderr,'(1x,a,i4,a,i4)') 
     :         'MAIN: too many receivers/shot - max is: ',
     :         maxrec,' - check shot ',nsrc
               stop
            end if
            xs(nsrc) = xref0 + fshot * trugeo
            call elevs(xs(nsrc),1,shtdpt,zs(nsrc),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
            if(.not.inside) then
               write(stderr,'(1x,a,f6.2)')
     :         'MAIN: Shot located outside model - at station ',
     :          fshot
                stop
            end if
            call layer(xs(nsrc),zs(nsrc),slayer(nsrc),inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
            if(.not.inside) then
               write(stderr,'(1x,a,f6.2)')
     :         'MAIN: Can''t find source layer - source at station ',
     :          fshot
                stop
            end if

            if(ntr1b.lt.ntrmin) ntrmin = ntr1b
            if(ntrnf.gt.ntrmax) ntrmax = ntrnf
            go to 230

231         continue    
         end if


         if(downhl) then
232         read(geoms,*,end=233) ntr1b,ntrnb,ntr1f,ntrnf
            if(ntr1b.lt.ntrmin) ntrmin = ntr1b
            if(ntrnf.gt.ntrmax) ntrmax = ntrnf
            go to 232
233         continue
         end if


235      rewind geoms

c        check that all receivers lie inside limits of model
         if(marine) then
            if(fmove.ge.0.)then
               xrmin = xref0 + ntrmin * trugeo
               xrmax = xref0 + ntrmax * trugeo + (nsrc-1) * fmove
            else
               xrmin = xref0 + ntrmin * trugeo - (nsrc-1) * fmove
               xrmax = xref0 + ntrmax * trugeo 
            end if
         else
c           land or downhole
            xrmin = xref0 + ntrmin * trugeo
            xrmax = xref0 + ntrmax * trugeo
         end if

         if(xrmin.le.xstart.or.xrmax.ge.xend) then
            write(stderr,'(1x,a)')
     :     'MAIN: Receiver stations outside limits of model.'
            stop
         end if


      end if


      if(begplt) then

c        Plotting
c        initialize plot
         call ploti

         if(pltmod) then
c           plot the model
            ipen = icolor(6)
            CALL PLOTIN(ipen,
     1                  MAXINT,MAXSPL,MXSPM1,MAXN,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  VINT,ivch)
         end if

         if(pltwel) then
c           plot the well
            ipen = icolor(3)
            call plotwl(zwell,nwell,w0,w1,w2,w3,ipen)
         end if

         if(qtplot) then
c           quit after plotting the model and/or the well
            call plote
            stop
         end if

         if(pltgeo) then
c           plot the receiver locations
            ipen = icolor(1)
            if(land) then
               do 250 k = ntrmin,  ntrmax
c                 first calculate x and z coords. of receiver stations
                  xr = xref0 + trugeo * k
                  call elevs(xr,1,recdpt,zr,inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
                  call pltsym(xr,zr,1,trugeo,ipen)
250               continue
            else
               do 260 i = 1,  nsrc
                  xr = xref0 + ntrmin * trugeo + (i-1) * fmove
                  do 255 k = ntrmin,  ntrmax
c                    first calculate x and z coords. of receiver stations
                     call elevs(xr,1,recdpt,zr,inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
                     call pltsym(xr,zr,1,trugeo,ipen)
                     xr = xr + trugeo
255                  continue
260               continue
            end if

         end if

         if(pltsrc) then
c           plot the source locations
            ipen = icolor(2)
            call pltsym(xs,zs,nsrc,trugeo,ipen)
         end if

         if(qtplt2) then
c           quit after above plot options
            call plote
            stop
         end if

         if(.not.pltray) then
c           no more plotting
            call plote
         end if
      
      end if


      if(pltray.or.list) then
c        proceed
      else if(shtrec) then
c        proceed
      else
c        nothing else to do
         stop
      end if


c     Read remainder of param

c     Read the name to be given the output files, if there are to be any.
      READ(PARIN,'(A)',end=300) OUTNAM
      go to 305
300   write(stderr,'(1x,a)')
     :'PARAM1 not filled out - stopped at output file name.'
      if(begplt) call plote
      stop

305   if(list.or.shtrec) then
c        Count the number of characters in the name, up to first blank.
         J = 1
310      IF(OUTNAM(J:J).EQ.' ') THEN
         ELSE
            J = J + 1
            GO TO 310
         END IF
         NCHARC = J - 1
      end if

      IF(list) THEN
         OUTFIL = OUTNAM(1:NCHARC)//'data'
         OPEN(UNIT=RAYOUT,FILE=OUTFIL,ERR=350)
         GO TO 355
350      write(stderr,'(1x,a)')'Can''t open ray data file.'
         if(begplt) call plote
         STOP
355      REWIND RAYOUT

         OUTFIL = OUTNAM(1:NCHARC)//'listing'
         OPEN(UNIT=RAYLST,FILE=OUTFIL,ERR=360)
         GO TO 365
360      write(stderr,'(1x,a)')'Can''t open ray listing file.'
         if(begplt) call plote
         STOP
365      REWIND RAYLST
      end if


c     Want to generate a shot record ?
      IF(shtrec) THEN
         OUTFIL = OUTNAM(1:NCHARC)//'shot'
c        OPEN(UNIT=SHTOUT,FILE=OUTFIL,
         OPEN(UNIT=SHTOUT,FORM='UNFORMATTED',FILE=OUTFIL,
     :   ERR=400)
         GO TO 405
400      write(stderr,'(1x,a)')'Error creating output trace file.'
         if(begplt) call plote
         STOP
405      REWIND SHTOUT
      END IF


c     continue reading from PARAM file
      READ(PARIN,*,end=420) BETAI,BETAF
      go to 425
420   write(stderr,'(1x,a)')
     :'PARAM1 not filled out - stopped at range of takeoff angles.'
      if(begplt) call plote
      stop

425   IF(BETAI.GT.BETAF) THEN
         WRITE(stderr,'(1x,A)') 'MAIN : must have betaf > betai'
         STOP
      END IF

      READ(PARIN,*,end=430) DELTAB
      go to 435
430   write(stderr,'(1x,a)')
     :'PARAM1 not filled out - stopped at change in takeoff angle.'
      if(begplt) call plote
      stop
435   IF(DELTAB.LE.0.) THEN
         WRITE(stderr,'(1x,A)') 'MAIN : deltab must be positive'
         STOP
      END IF


c     read layer velocities
      READ(PARIN,*,end=450) (VEL(I),I=1,NINT+1)
      go to 455
450   write(stderr,'(1x,a)')
     :'Not enough velocities - need one more than #interfaces.'
      if(begplt) call plote
      stop
455   continue

c     now read in events
c     initializing
      nevent = 0

c     want direct wave?
      read(parin,'(a)',end=465) yorn 
      go to 470
465   write(stderr,'(1x,a)')
     :'MAIN: PARAM1 not filled out - no events specified.'
      if(begplt) call plote
      stop
470   continue
      if(yorn.eq.'y') then
         nevent = 1
         evtype(nevent) = 'd'
      end if

c     want headwaves?
      read(parin,'(a)',end=475) event
      go to 480
475   write(stderr,'(1x,a)')
     :'MAIN: PARAM1 not filled out - need head wave and primary specs.'
      if(begplt) call plote
      stop
480   continue
      nevent = nevent + 1
      call setref(event,irefl,nrefls(nevent),nevent,valid,
     :            maxevt,maxref,nint)
c     setref was originally designed to deal with extra event
c     specification - this patch is for the head wave specification
      if(valid) then
c        setref places interface numbers contained in "event" 
c        into irefl(nevent,here)
c        the number of interfaces specified is given by nrefls(nevent)
c        for headwaves each interface# specifies a refractor

c        #headwaves
         nheads = nrefls(nevent)
c        now consider each head wave as separate event
         ihead = 1
         do 500 k = nevent,  nevent + nheads - 1
            nrefls(k) = 1
            irefl(k,1) = irefl(nevent,ihead)
            evtype(k) = 'h'
            ihead = ihead + 1
500         continue

c        this is the true number of events
         nevent = nevent + nheads - 1
      else
c        error in specifying refractors - noninteger characters used or more
c        than 3 digits for an interface (max interface is 99 in sub setref -
c        might be less, depending on dimensions of program)
         nevent = nevent - 1
         nheads = 0
         write(stderr,'(/,2x,a)') 
     :   'Invalid headwave description.'
      end if
         

c     want all primaries?
      read(parin,'(a)',end=505) yorn
      go to 510
505   write(stderr,'(1x,a)')
     :'MAIN: PARAM1 not filled out - specify y or n for primaries.'
      if(begplt) call plote
      stop
510   continue
      if(yorn.eq.'y') then
c        calculate primary reflections
         do 520 k = 1, nint
            nevent = nevent + 1
            nrefls(nevent) = 1
            irefl(nevent,1) = k
            evtype(nevent) = 'r'
520         continue
      end if


c     now come the extra events (specified by the reflecting interfaces)
c     there are as many extra events as there are records left in param
550   read(parin,'(a)',end=600) event
      nevent = nevent + 1
      call setref(event,irefl,nrefls(nevent),nevent,valid,
     :            maxevt,maxref,nint)
      if(nrefls(nevent).eq.0) then
c        not an event - probably a blank line
         nevent = nevent - 1
      else if(.not.valid) then
c        not a valid event - noninteger characters used or more
c        than 3 digits for an interface (max interface is 99)
         nevent = nevent - 1
         write(stderr,'(/,2x,a)') 
     :   'Invalid extra event specification.'
      else
         evtype(nevent) = 'r'
      end if
      go to 550
600   continue

      if(list) then

         write(raylst,'(/,20x,a/)')
     :   'CSHOT1 Listing File'
         write(raylst,'(/2x,a)') 'Velocities:'
         do 605 i = 1,  nint + 1
            write(raylst,'(2x,a,i2,1x,f8.1)')
     :      'layer ',i,vel(i)
605         continue

         WRITE(RAYLST,'(/,2X,A,I4)') 'Number of shots = ',nsrc
         WRITE(RAYLST,'(2X,A,I4)') 'Number of events per shot = ',
     :   NEVENT

         if(downhl) then
            write(raylst,'(1x,a,f8.2,a,f8.2/)')
     :      'Top of well is at coordinates ',xwell(0),',',zwell(0)
            write(raylst,'(3x,a,11x,a,8x,a)')'shot',
     :      'x-z coordinates',
     :      'layer number'
            do 610 k = 1,  nsrc
               write(raylst,'(2x,i3,10x,f8.2,5x,f8.2,9x,i2)')
     :         k,xs(k),zs(k),slayer(k)
610            continue
         end if

      end if


      IF(shtrec) THEN
c        Need this constant for amplitude calculations.
c        CONST = ( 1. / ( 4. * PI ) ) * SQRT( VEL(1) / TRUGEO )
         CONST = 1. / ( 4. * PI * SQRT(TRUGEO) )
c        VEL(0) identifies reflections from the surface of the
c        earth ( reflection coefficient is then set to -1 ).
         VEL(0) = 0.
      END IF

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
ccccccccccccc   Main loop over shots   ccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     read shot/receiver geometry cards
c     skip over first two records
      read(geoms,*) idummy
      read(geoms,*) rdummy
c     set pen color for rays
      if(pltray) then
         ipen = icolor(5)
      end if

      eof = .false.
      if(marine) then
         read(geoms,*) ntr1b,ntrnb,ntr1f,ntrnf
         xref0 = xref0 - fmove
         totlmu = - fmove
      else
         mu = 0
      end if

      do 2000 irecd = 1,  nsrc

      if(land) then
c        Check for eof is necessary here because when downhole shooting there
c        may be more shots than geometry cards.  If so, keep the geometry
c        of the last card until all shots are done.
         if(.not.eof) read(geoms,*,end=650) ntr1b,ntrnb,ntr1f,ntrnf
         go to 655
650      eof = .true.
655      continue
      else
         xref0 = xref0 + fmove
         totlmu = totlmu + fmove
         mu = ( totlmu + trugeo/10. ) / trugeo
      end if

c     ray begins at source location
      x(0) = xs(irecd)
      z(0) = zs(irecd)

c     number of receivers
      nrec = ntrnf - ntr1b + 1
c     number of receivers in the gap
      ngap = ntr1f - ntrnb  - 1

c     set x-coordinates of receivers for this shot
      xrec(1) = xref0 + trugeo * ntr1b
      do 670 i = 1,  nrec - 1
         xrec(i+1) = xrec(i) + trugeo
670      continue

c     calculate z-coords. of receivers
      call elevs(xrec,nrec,recdpt,zrec,inside,
     1                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     2                 MAXINT,MAXSPL,MAXN,MXSPM1)
      if(inside) then
      else
         write(stderr,'(1x,a)')
     :   'MAIN: warning - receivers outside model.'
      end if


      IF(list) THEN
         WRITE(RAYLST,'(1(/),2X,A,I4)') 'x and z coordinates of shot',
     :   IRECD
         WRITE(RAYLST,5005) X(0),Z(0)
         WRITE(RAYLST,'(/,2X,A,I4)') 'Number of receivers = ',NREC
         WRITE(RAYLST,'(2X,A)') 'x and z coordinates of receivers :'
         DO 680 I = 1,  NREC
            if(i.gt.ntrnb.and.i.lt.ntr1f) then
c              receiver in the gap
            else
               WRITE(RAYLST,5005) xrec(i),zrec(i)
            end if
680         CONTINUE
      END IF


ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
ccccccccccccccc   Do for each event   ccccccccccccccccccccccccccccccc
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      DO 1000 IEVENT = 1,  NEVENT


c     Initializing
      BETA = BETAI
      NOCONV = .TRUE.

      IF(list) THEN
         WRITE(RAYLST,'(2(/),2X,A,I4,6X,A,I4)')
     :   'Shot ',irecd,'event ',IEVENT
      END IF


c     An event if of type d (direct wave), h (headwave), or
c     r (reflected event).

      if(evtype(ievent).eq.'d') then
c        DIRECT WAVE

         head = .false.
         if(list) write(raylst,'(2x,a/)')'This is a direct wave.'
         if(slayer(irecd).eq.1) then

c           source in same layer as receivers (layer 1)
c           use free space green's function and straight rays
            itrace = 1
            x(0) = xs(irecd)
            z(0) = zs(irecd)
            do 700 irec = 1,  nrec
               x(1) = xrec(irec)
               z(1) = zrec(irec)
               call gap(irec,ntr1b,ntrnb,ntr1f,ingap,ntrabs)
               if(.not.ingap) then
                  if(tinfo) then
c                    length of raypath
                     r = sqrt( (x(1)-x(0))**2 + (z(1)-z(0))**2 )
c                    put wavelet on trace
                     time1 = r / vel(1)
                     if(shtrec) then
                        if(r.eq.0.) then
c                          amplitude infinite - set it to 0!
                           amp = 0.
                        else
                           amp = 1. / (4. * pi * r)
                        end if
                        phase1 = 0.
                        caustc = .false.
                        write(shtout)irecd,itrace,ntrabs+mu,time1,
     :                  x(1)-x(0),ievent,zs(irecd),
     :                  amp,phase1,caustc,head
                        itrace = itrace + 1
                     end if
                     if(list) then
                        write(rayout,*)2,time1
                        call xzout(x,z,1,rayout)
                        call xzout(x,z,1,raylst)
                        WRITE(raylst,'(3X,A,F10.6/)') 
     :                  't = ',TIME1
                     end if
                  end if
                  if(pltray) CALL RAYplt(X,Z,0,ipen)
               end if

700            continue

c           done with this event
            raytrc = .false.

         else

c           source is deeper than layer 1
c           direct ray must go up
            sign(0) = 1.
c           set number of intersections
            n = slayer(irecd) - 1
c           set order, velocities, etc.
            do 750 i = 1,  n
               k = slayer(irecd) - i
               norder(i) = k
               sign(i) = 1.
               v(i) = vel(k+1)
750            continue
            v(n+1) = vel(1)
c           set this to an invalid reflector so that sub RAYDAT will
c           not compute a reflection coefficient
            irefl(ievent,1) = -1
c           now ready to enter ray tracing routines
            raytrc = .true.

         end if


      else if(evtype(ievent).eq.'h') then
c        HEADWAVE EVENT

         head = .true.
         if(list) write(raylst,'(2x,a/)')'This is a head wave event.'
         call hdwave(raylst,shtout,stderr,
     :            list,shtrec,tinfo,pltray,x,z,xs(irecd),zs(irecd),
     :            xstart,xend,xrec,zrec,nrec,recdpt,trugeo,
     :            slayer(irecd),vel,vref,betai,betaf,deltab,
     :            irefl,ievent,irecd,pi,
     :            ntr1b,ntrnb,ntr1f,ngap,head,maxevt,maxref,mu,ipen,
     5            SUBDJ,DJ,SUPDJ,PHI,
     6            XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     7            DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     8            MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,
     9            xold, COSINC, COSTRA, VINT)


c        do not need any more ray tracing for this event
         raytrc = .false.



      else if(evtype(ievent).eq.'r') then
c        REFLECTED EVENT

         head = .false.
         if(list) write(raylst,'(2x,a/)')'This is a reflection event.'
         call order(irefl,nrefls(ievent),slayer(irecd),ievent,
     :   vel,norder,v,sign,vref,n,valid,recdpt,list,raylst,stderr,
     :   maxevt,maxref)
         if(valid) then
c           enter ray tracing routines
            raytrc = .true.
         else
            raytrc = .false.
         end if
        

      end if

      if(.not.raytrc) go to 999 

c     Begin Ray Tracing 

c     Begin search for the first ray.
c     Shoot rays until one emerges within the line of receivers.
c     Continue that ray onto the first receiver location.

800   IF(NOCONV) THEN

         CALL SHOOT(X,z,NOCONV,BETA,PI,TRUGEO,
     1   	    xrec(1),xrec(nrec),recdpt,.false.,0,0.,0.,0.,
     2              XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3              DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4              MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)


c		do i=0,n
c	 		write(stderr,*) x(i),z(i),i
c		end do

         IF(NOCONV) THEN
c           shooting procedure failed or ray exited the model
c           increment the takeoff angle and try again
            BETA = BETA + DELTAB
            IF(BETA.GT.BETAF) THEN
               WRITE(stderr,'(1x,A)')
     :        'Reached max takeoff angle without finding a ray:'
               WRITE(stderr,'(1x,A,1X,I4,4X,A,1X,I2)')
     :        'Shot number',irecd,'Event number',IEVENT
c              try next event
               GO TO 999 
            END IF
         ELSE
c           find sign of jacobian
            CALL DETJAC(X,z,DETJ,
     1                  SUBDJ,DJ,SUPDJ,DUMMY,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            IF(DETJ.LT.0.) THEN
c              ray has gone through a caustic
c              assume this branch starts at the end of the line
               irec = nrec
               GEOINC = xrec(irec) - X(N+1)
               geoz = zrec(irec) - z(n+1)
            ELSE
c              go to beginning of line
               irec = 1
               GEOINC = xrec(irec) - X(N+1)
               geoz = zrec(irec) - z(n+1)
            END IF

            CALL RECCON(x,z,geoinc,GEOZ,PI,
     :      		BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            IF(NOCONV) THEN
c              continuation didn't make it all the way
c              proceed to nearest receiver
               IF(DETJ.LT.0.) THEN
c                 go to the left
                  irec = ( X(N+1) - xrec(1) ) / TRUGEO + 1
                  geoinc = xrec(irec) - x(n+1)
                  geoz = zrec(irec) - z(n+1)
               ELSE
c                 go to the right
                  irec = ( X(N+1) - xrec(1) ) / TRUGEO + 2
                  geoinc = xrec(irec) - x(n+1)
                  geoz = zrec(irec) - z(n+1)
               END IF
               CALL RECCON(x,z,geoinc,GEOZ,PI,
     :         		   BETNEW,NOCONV,sign(0),
     1                     SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                     XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                     DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                     MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            END IF
            IF(NOCONV) THEN
c              will have to shoot another ray
               BETA = BETA + DELTAB
            ELSE
               IF(DETJ.LT.0.) THEN
c                 future rays will emerge at decreasing x coordinates
c                 prepare to continue to the left
c                 set pen color for caustic ray
                  GEOINC = - TRUGEO
                  CAUSTC = .TRUE.
                  ipen = icolor(4)
               ELSE
c                 prepare to continue to the right
c                 set pen for ordinary ray
                  GEOINC = TRUGEO
                  CAUSTC = .FALSE.
                  ipen = icolor(5)
               END IF
            END IF
         END IF

         GO TO 800

      END IF

ccccccccccccccccccccc   Found the first ray   ccccccccccccccccccc

      BETA = BETNEW
      BELAST = BETA
      x1last = x(1)

      IF(PLTray) THEN
c        plot first ray
         CALL RAYplt(X,Z,N,ipen)
      END IF

      if(tinfo) then
c        calculate traveltime for this ray
         CALL TTIME(N,D,V,TIME1)
      end if

      IF(shtrec) THEN
c        get amplitude data for first ray
         CALL RAYDAT(x(n+1),VREF,irefl,ievent,
     1   	     AMP1,PHASE1,tcoeff,maxevt,maxref,COSINC,COSTRA,
     2               XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3               DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4               MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

c        identify this ray as the first in a branch
         FIRST = .TRUE.
         BETA2 = BETA
      END IF

      IF(list) THEN
c        output data for first ray
         write(rayout,*)n+2,time1
         call xzout(x,z,n+1,rayout)
         call xzout(x,z,n+1,raylst)
         WRITE(RAYLST,'(3X,A,F10.6/)') 't = ',TIME1
      END IF


c     Entering main ray tracing loop.  We use continuation
c     methods until they break down or we reach the end
c     of the line.  In these situations we switch to a
c     shooting procedure, either to get away from the trouble
c     spot for the continuation methods or to search for more
c     ray solutions occurring at larger takeoff angles.


900   IF(BETA.LE.BETAF) THEN
c        while takeoff angle is within range

         IF(NOCONV) THEN
c           enter shooting scheme

c	    write(*,*) "before shoot"

            CALL SHOOT(X,z,NOCONV,BETA,PI,TRUGEO,
     1                 xrec(1),xrec(nrec),recdpt,.false.,0,0.,0.,0.,
     2                 XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                 DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                 MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)

c	    write(*,*) "after shoot"

            IF(NOCONV) THEN
c              shoot again
               BETA = BETA + DELTAB
            ELSE
c              find sign of jacobian
               CALL DETJAC(X,z,DETJ,
     1                     SUBDJ,DJ,SUPDJ,DUMMY,
     2                     XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                     DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                     MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
               IF(DETJ.LT.0.) THEN
c                 caustic rays

                  IF(X(N+1).GE.XLAST) THEN
c                    an unusual situation
c                    might have lost a branch
c                    try to continue to nearest receiver to left
                     irec = ( X(N+1) - xrec(1) ) / TRUGEO + 1
                     geoinc = xrec(irec) - x(n+1)
                     geoz = zrec(irec) - z(n+1)
                     CALL RECCON(x,z,geoinc,GEOZ,PI,
     :               	BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                  ELSE
c                    x(n+1) < xlast
                     IF(xrec(irec).EQ.xrec(nrec).OR.GEOINC.GT.0.) THEN
                        GEOINC = xrec(irec) - X(N+1)
                        geoz = zrec(irec) - z(n+1)
                     ELSE
                        irec = irec - 1
                        GEOINC = xrec(irec) - X(N+1)
                        geoz = zrec(irec) - z(n+1)
                     END IF
                     CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     IF(NOCONV) THEN
c                       try to get to nearest receiver to left
                        irec = ( X(N+1) - xrec(1) ) / TRUGEO + 1
                        geoinc = xrec(irec) - x(n+1)
                        geoz = zrec(irec) - z(n+1)
                        CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     END IF
                     IF(BETNEW.LE.BELAST) THEN
c                       trouble - takeoff angle should always
c                       increase; go back to shooting
                        CALL SHOOT(X,z,NOCONV,BETA,PI,
     :                  TRUGEO,xrec(1),xrec(nrec),recdpt,
     1                  .false.,0,0.,0.,0.,
     2              	XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3              	DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4             	MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)
c                       continue to nearest receiver to left
                        irec= ( X(N+1) - xrec(1) ) / TRUGEO + 1
                        geoinc = xrec(irec) - x(n+1)
                        geoz = zrec(irec) - z(n+1)
                        CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     END IF

                  END IF

                  dx1 = abs(x1last-x(1))
                  IF((BETNEW-BETA).GE.DBMAX.and.dx1.ge.dx1max) THEN
                     jump = .TRUE.
                  else
                     jump = .false.
                  END IF
                  IF(BETNEW.LE.BELAST.or.jump) THEN
                     NOCONV = .TRUE.
                  END IF
                  IF(NOCONV) THEN
c                    all efforts failed - shoot again
                     BETA = BETA + DELTAB
                  ELSE
c                    prepare to go back to continuation
                     BETA = BETNEW
                     GEOINC = - TRUGEO
                     CAUSTC = .TRUE.
                     ipen = icolor(4)
                  END IF

               ELSE
c                 detj > 0.

                  IF(X(N+1).LT.XLAST) THEN
c                    unusual situation
c                    continue to nearest receiver to right
                     irec = ( X(N+1) - xrec(1) ) / TRUGEO + 2
                     geoinc = xrec(irec) - x(n+1)
                     geoz = zrec(irec) - z(n+1)
                     CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                  ELSE
c                    x(n+1) > xlast
                     IF(xrec(irec).EQ.xrec(1).OR.GEOINC.LT.0.) THEN
                        GEOINC = xrec(irec) - X(N+1)
                        geoz = zrec(irec) - z(n+1)
                     ELSE
                        irec = irec + 1
                        GEOINC = xrec(irec) - X(N+1)
                        geoz = zrec(irec) - z(n+1)
                     END IF
                     CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     IF(NOCONV) THEN
c                       try to get to nearest receiver to right
                        irec = ( X(N+1) - xrec(1) ) / TRUGEO + 2
                        geoinc = xrec(irec) - x(n+1)
                        geoz = zrec(irec) - z(n+1)
                        CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     END IF
                     IF(BETNEW.LE.BELAST) THEN
c                       trouble - takeoff angle should always
c                       increase; go back to shooting
                        CALL SHOOT(X,z,NOCONV,BETA,PI,
     :                  TRUGEO,xrec(1),xrec(nrec),recdpt,
     1                  .false.,0,0.,0.,0.,
     2              	XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3              	DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4             	MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1,VINT)
c                       try to get to nearest receiver to right
                        irec = ( X(N+1) - xrec(1) ) / TRUGEO + 2
                        geoinc = xrec(irec) - x(n+1)
                        geoz = zrec(irec) - z(n+1)
                        CALL RECCON(x,z,geoinc,GEOZ,PI,
     :                  BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     END IF

                  END IF

                  dx1 = abs(x1last-x(1))
                  IF((BETNEW-BETA).GE.DBMAX.and.dx1.ge.dx1max) THEN
                     jump = .TRUE.
                  else
                     jump = .false.
                  END IF
                  IF(BETNEW.LE.BELAST.or.jump) THEN
                     NOCONV = .TRUE.
                  END IF
                  IF(NOCONV) THEN
c                    all efforts failed
c                    go back to shooting
                     BETA = BETA + DELTAB
                  ELSE
c                    prepare to return to continuation
                     BETA = BETNEW
                     GEOINC = TRUGEO
                     CAUSTC = .FALSE.
                     ipen = icolor(5)
                  END IF

               END IF

               IF(NOCONV) THEN
               ELSE
                  call gap(irec,ntr1b,ntrnb,ntr1f,ingap,ntrabs)
c                 plot or output this ray
                  IF(.not.ingap.and.PLTray) THEN
                     CALL RAYplt(X,Z,N,ipen)
                  ENDIF
                  IF(.not.ingap.and.tinfo) THEN
                     CALL TTIME(N,D,V,TIME1)
                  end if
                  IF(shtrec) THEN
                     BETA2 = BETA
                     FIRST = .TRUE.
                     if(.not.ingap) then
                     CALL RAYDAT(x(n+1),VREF,irefl,ievent,
     :               AMP1,PHASE1,tcoeff,maxevt,maxref,
     1   	     COSINC,COSTRA,
     2               XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3               DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4               MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                     end if
                  END IF
                  IF(.not.ingap.and.list) THEN
                     write(rayout,*)n+2,time1
                     call xzout(x,z,n+1,rayout)
                     call xzout(x,z,n+1,raylst)
                     WRITE(RAYLST,'(3X,A,F10.6/)') 't = ',TIME1
                  END IF
               END IF

            END IF

         ELSE

c           Use continuation methods.
c           update the receiver location
            irecp = irec
            x1last = x(1)
            jump = .false.

            IF(xrec(irec).GT.xrec(1).AND.xrec(irec).LT.xrec(nrec)) THEN
c              receiver inside end points of line
               if(geoinc.gt.0.) then
                  geoz = zrec(irec+1) - z(n+1)
               else
                  geoz = zrec(irec-1) - z(n+1)
               end if
               CALL RECCON(x,z,geoinc,GEOZ,PI,
     :         		BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            ELSE IF(xrec(irec).EQ.xrec(nrec).AND.GEOINC.LT.0.) THEN
c              at the end of the line and moving to the left
               geoz = zrec(irec-1) - z(n+1)
               CALL RECCON(x,z,geoinc,GEOZ,PI,
     :         		BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            ELSE IF(xrec(irec).EQ.xrec(1).AND.GEOINC.GT.0.) THEN
c              at the start of the line and moving to the right
               geoz = zrec(irec+1) - z(n+1)
               CALL RECCON(x,z,geoinc,GEOZ,PI,
     :         		BETNEW,NOCONV,sign(0),
     1                  SAVEX,DXDL,SUBDJ,DJ,SUPDJ,PHI,DPHIDR,
     2                  XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3                  DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4                  MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)

            ELSE
c              reached the end of the line
c              search for more solutions by shooting
               NOCONV = .TRUE.

            END IF

c           make sure the takeoff angle of the new ray found
c           by the continuation procedure is greater than the
c           takeoff angle for the previous ray
c           also check to see that the change in takeoff angle
c           is not too big - if it is, suspect some missing solutions

            dx1 = abs(x1last-x(1))
            IF((BETNEW-BETA).GE.DBMAX.and.dx1.ge.dx1max) THEN
               jump = .true.
            end if
            IF(BETA.GE.BETNEW.OR.jump) THEN
               NOCONV = .TRUE.
               XLAST = xrec(irec)
               BELAST = BETA
               BETA = BETA + DELTAB
            ELSE
c              update beta
               BETA = BETNEW
               IF(NOCONV) THEN
c                 set values from last good ray
c                 ray might not end at a receiver location
                  XLAST = X(N+1)
                  BELAST = BETA
                  BETA = BETA + DELTAB
                  x1last = x(1)
               else
                  if(geoinc.gt.0.) then
                     irec = irec + 1
                  else
                     irec = irec - 1
                  end if
               END IF
            END IF

            IF(.NOT.NOCONV) THEN
               call gap(irec,ntr1b,ntrnb,ntr1f,ingap,ntrabs)
               if(.not.ingap.and.pltray) then
                  CALL RAYplt(X,Z,N,ipen)
               END IF
               IF(.NOT.ingap.AND.tinfo) THEN
                  CALL TTIME(N,D,V,TIME2)
               end if
               IF(.NOT.ingap.AND.list) THEN
                  write(rayout,*) n+2,time2
                  call xzout(x,z,n+1,rayout)
                  call xzout(x,z,n+1,raylst)
                  WRITE(RAYLST,'(3X,A,F10.6/)') 't = ',TIME2
               END IF
            END IF

            IF(shtrec) THEN
c              Do the amplitude calculation.
c              Ray tube consists of three rays, except at
c              start and end of branch.
c              Add the wavelet into the trace at the correct traveltime.

               IF(NOCONV) THEN
                  IF(FIRST) THEN
c                    only one ray in tube
                     DBETA = 0.
                  ELSE
c                    last ray in branch
c                    two rays in tube
                     DBETA = BETA2 - BETA1
                     SPACIN = 1.
                  END IF
               ELSE
c                 do some amplitude calculations for this ray
c                 these will be used next time
                  if(.not.ingap) then
                     CALL RAYDAT(x(n+1),VREF,irefl,ievent,
     :               AMP2,PHASE2,tcoeff,maxevt,maxref,
     1   	     COSINC,COSTRA,
     2               XINT,ZINT,A0,A1,A2,A3,SIGN,NPTS,NINT,NORDER,
     3               DZ,DDZ,DELTAX,DELTAZ,D,V,N,
     4               MAXINT,MAXSPL,MAXN,MAXNP1,MXSPM1)
                  end if

                  BETA3 = BETA

                  IF(FIRST) THEN
c                    first ray of branch
c                    two rays in tube
                     DBETA = BETA3 - BETA2
                     SPACIN = 1.
                  ELSE
c                    three rays in tube
                     DBETA = BETA3 - BETA1
                     SPACIN = 2.
                  END IF
               END IF

               IF(DBETA.EQ.0.) THEN
c                 can't find amplitude ( spreading )
               ELSE
c                 finish the amplitude calculation for the previous ray
c                 if it is not in the gap
                  call gap(irecp,ntr1b,ntrnb,ntr1f,ingap,ntrabs)
                  if(.not.ingap) then
                     AMP = CONST * AMP1 *
     :               SQRT( ( V(1) * DBETA * PI / 180. ) / SPACIN )
                     if(ntrabs.le.ntrnb) then
                        itrace = irecp
                     else
                        itrace = irecp - ngap
                     end if
                     write(shtout)irecd,itrace,ntrabs+mu,time1,
     :               xrec(irecp)-x(0),ievent,zs(irecd),
     :               amp,phase1,caustc,head
                  end if
               END IF

               IF(NOCONV) THEN
c                 will have to start again ( shooting )
               ELSE
c                 drop first ray, prepare to pick up new ray
                  TIME1 = TIME2
                  AMP1 = AMP2
                  PHASE1 = PHASE2
                  BETA1 = BETA2
                  BETA2 = BETA3
                  FIRST = .FALSE.
               END IF

            END IF

         END IF

         GO TO 900

      END IF
      
c      write(*,*) "999 reached "

999   continue
c     Identify end of event.
      IF(list) THEN
         WRITE(RAYLST,'(2X,A)') 'End of event'
      END IF


1000  CONTINUE


      IF(list) THEN
         WRITE(RAYLST,'(/2X,A)') 'End of Shot'
      END IF


2000  CONTINUE

      IF(list) THEN
         WRITE(RAYLST,'(2(/),2X,A)') 'End of listing'
      END IF


c     close files


      IF(begplt) THEN
         call plote
      END IF

      IF(shtrec) THEN
         CLOSE(UNIT=SHTOUT,STATUS='keep')
      END IF

      IF(list) THEN
         CLOSE(UNIT=RAYOUT,STATUS='keep')
         CLOSE(UNIT=RAYLST,STATUS='keep')
      END IF

5005  FORMAT(2F10.2)

      STOP
      END

*--------------- end of main program -------------------------------
