*    $Author: jkc $
*    $Source: /src/su/czfilters/RCS/cz3fil.f,v $
*    $Revision: 1.8 $
*    $Date: 88/05/21 18:26:15 $

**************************************************************************
*                                                                        *
*  CZ3FIL - strip info from CZ traces and output bare traces             *
*                                                                        *
*  Purpose:                                                              *
*     Prepare CZ traces for SU line by stripping off information         *
*     records and putting essential data in a file for suaddhead         *
*     and sushw (but fortran blanks need to be removed, say by sed).     *
*                                                                        *
*     Put zero traces at left to replace those not inverted.             *
*     Similarly zero top for shallow region not inverted.  There is      *
*     no similar zero fill on the right or bottom because CZ does not    *
*     transmit the necessary information.                                *
*                                                                        *
*  Credits:                                                              *
*       TEXACO: Joe Higginbotham                                         *
*       CWP: Brian, Jack                                                 *
*                                                                        *
**************************************************************************

      program cz3fil

**************************************************************************
*
* Globals:
*    CWPID - CWP identifier string
*    DDX - double precision DX (from CZ2)
*    DX - trace spacing
*    DZ - nominal interval for equally spaced depth values
*    FINAL - name of final output file
*    FRF - final result file unit number
*    I, J - counters
*    INC() - holds velocity profile (here ignored)
*    INTRA() - holds a trace from CZ2
*    INZ() - holds z values for traces
*    INZJ, INZJM1 - temporaries for INZ() values
*    JI() - locations for linear interpolation
*    JM1 - J minus one
*    NPTS - number of computed points on trace
*    NPTSM1 - NPTS minus 1
*    NPTSM2 - NPTS minus 2
*    NTRM1 - NTRACE minus one
*    NTROUT - NTRACE minus one
*    NZERFL - number of zero fill points at top of trace
*    NZOTM1 - NZOUT minus one
*    NZOUT - total number of points on trace including top zero fill
*    OUTTRA() - holds a trace to be sent to SU plotting
*    OZMAX - last z on traces
*    OZMIN - first z on traces
*    PARFIL - SU parameter file for suaddhead and sushw
*    PF - bare trace file unit number
*    PTSMAX - max points per trace
*    RCSID - revision control identifier string
*    STDIN - unit number for standard input
*    STDOUT - unit number for standard output
*    SUF - unit number for SU parameter file
*    TN - trace number
*    TRNBAS - base for trace numbers
*    TRNBM1 - TRNBAS minus one
*    W0, W1 - weights for linear interpolation
*    Z - depth value
*
**************************************************************************


      integer stdin  , stdout , frf    , suf    , pf    , ptsmax

      parameter (stdin = 5,
     :           stdout = 6,
     :           frf = 1,
     :           suf = 7,
     :           pf = 8,
     :           ptsmax = 2047)

      real
     :        ozmin  , ozmax  , inzj   , inzjm1 , dz     , z      ,
     :        dx


      double precision ddx


      real
     :        intra(0:ptsmax) , outtra(0:ptsmax) ,
     :        inc(0:ptsmax)   , inz(0:ptsmax)    ,
     :        w0(0:ptsmax)    , w1(0:ptsmax)

      integer
     :        i      , j      , jm1    , trnbas , trnbm1 ,
     :        ntrace , ntrout , ntrm1  , npts   , nptsm1 , nptsm2 ,
     :        nzout  , nzotm1 , tn     , nzerfl , nt

      integer ji(0:ptsmax)


      character final*15


*     ...Revision control system identifier string.
      character cwpid*80, progid*80, rcsid*80
      parameter (cwpid = ' Center for Wave Phenomena: ',
     :    progid = ' $Source: /src/su/czfilters/RCS/cz3fil.f,v $',
     :    rcsid = ' $Revision: 1.8 $ ; $Date: 88/05/21 18:26:15 $')


*     ...Code begins. Open I/O devices.
      open(unit = stdout)
      write(stdout, '(a/a/a//)') cwpid, progid, rcsid


*     ...Create and open SU parameter file.  Since this is a temporary
*     ...file, hard wire name (as "parfil").
      open(unit = suf, file = 'parfil', access = 'sequential',
     :     form = 'formatted', status = 'new', err = 10)
      go to 20
  10     write(stdout, '(a)') ' ***Can not open SU parameter file.'
         stop


*     ...Get final result file name.
  20  read(stdin, '(a15)') final
      write(stdout, '(a,a15)') ' Final result file name: ',final

*     ...Open final result file containing CZ's inverted traces.
      open(unit = frf, file = final, access = 'sequential',
     :     form = 'unformatted', status = 'old', err = 30)
      go to 40
  30     write(stdout, '(a)') ' ***Can not open final result file.'
         stop

*     ...Open output bare trace file.  Since this is a temporary file,
*     ...hard wire name (as "BARE").
  40  open(unit = pf, file = 'BARE', access = 'sequential',
     :     form = 'unformatted', status = 'new', err = 50)
      go to 60
  50     write(stdout, '(a)') ' ***Can not open bare trace file.'
         stop


*     ...Read trace and depth value information.
  60  rewind frf
      read (frf) trnbas, ntrace, ddx, npts


*     ...Compute auxiliary variables and echo trace information.
      trnbm1 = trnbas - 1
      ntrm1 = ntrace - 1
      ntrout = ntrm1 + trnbas
      dx = real(ddx)
      nptsm1 = npts - 1
      nptsm2 = nptsm1 - 1

      write(stdout, '(a/a,i4,a,i4/)') ' File contents:',
     :      ' Trace range: ', trnbas, ' through ',trnbas+ntrm1


*     ...Get z values.
      read (frf) (inz(i), i = 0, nptsm1)


*     ...Compute auxiliary variables and echo depth information.
      ozmin = inz(0)
      ozmax = inz(nptsm1)

      write(stdout, '(a, i4/,a,f9.2,a,f9.2/)')
     :      ' Computed points from CZ: ', npts,
     :      ' Z range: ', ozmin, ' through ', ozmax


*     ...Compute nominal trace spacing and shift for top zero fill.
      if ((ozmax .le. ozmin) .or. nptsm1 .le. 0) then
         write(stdout, '(a)') ' ***Error in z values.'
       stop
      endif
      dz = (ozmax - ozmin)/nptsm1
      nzerfl = int(ozmin/dz)
      nzout = nzerfl + npts

      write(stdout, '(a,f9.2/, a,i4/)')
     :      ' DZ: ', dz,
     :      ' Total points on trace: ', nzout
      if (nzout .gt. ptsmax) then
	 write(stdout, '(a, 2i8)')
     :	       ' ***nzout too large, increase ptsmax.', nzout, ptsmax
       stop
      endif
      nzotm1 = nzout - 1


*     ...Not using velocity profile but still must read it out of way.
      read (frf) (inc(i), i = 0, nptsm1)
      

*     ...Write information in SU parameter file.
      write(suf, '(a/, a, i5/, 2(a, f9.2/))')
     :      ' ftn=1',
     :      ' ns=', nzout,
     :      ' key=dz a=', dz,
     :      ' key=zmin a=', ozmin


*     ...Overall strategy:
*     ...Interpolate to an equally spaced mesh to accommodate the plot
*     ...routine.  Since this is just for plotting and not for later
*     ...computations, just use linear interpolation.
*     ...Since the spacing is the same on every trace, table the
*     ...interpolation locations and weights.
*     ...At the endpoints just match values instead of interpolating.
*     ...The JI array stores the mesh point immediately to the right of z.
*     ...The W0 array stores the weight at the left end and
*     ...the W1 array stores the weight at right end.
*     ...Later zero fill the z's at top which CZ did not compute.


*     ...Compute j such that inz(j) is the first input value to
*     ...the right of z.
      j = 0
      do 80 i = 1, nptsm2
         z = i * dz + ozmin

  70     if (inz(j) .lt. z) then
	    j = j + 1
	  go to 70
	 endif

*        ...Store the location.
	 ji(i) = j

*        ...Compute and store weights for linear interpolation.
	 jm1 = j - 1
	 inzj = inz(j)
	 inzjm1 = inz(jm1)
	 w0(i) = (inzj - z) / (inzj - inzjm1)
	 w1(i) = (z - inzjm1) / (inzj - inzjm1)

  80  continue   


*     ...Output zero traces for those not inverted.  Compute a null
*     ...trace and write it repeatedly.
      do 85 i = 0, nzotm1
         outtra(i) = 0.0
  85  continue

      do 90 tn = 1, trnbm1
         write(pf) (outtra(i), i = 0, nzotm1) 
  90  continue


*     ...Interpolate each computed trace to equally spaced output trace.
*     ...First and last inverted points are just matched instead of
*     ...being interpolated.  Note that zero top fill is already
*     ...in place, since the whole outtra has been zeroed.
      do 120 tn = trnbas, ntrout
      
         read (frf) (intra(i), i = 0, nptsm1)

*        ...Fill shifted output traces by interpolation where have
*        ...stored locations and weights (1 <= i <= nptsm2) and
*        ...match at i = 0 and nptsm1.
	 outtra(nzerfl) = intra(0)
	 do 110 i = 1, nptsm2
	    j = ji(i)
	    outtra(i + nzerfl) = intra(j-1) * w0(i) +
     :                           intra(j)   * w1(i)

 110     continue
	 outtra(nptsm1 + nzerfl) = intra(nzotm1)

         write(pf) (outtra(i), i = 0, nzotm1) 

 120  continue


      write(stdout, '(a)') ' Files closed. Processing completed.'


*     ...Declare this war a victory and bring the boys home.
      stop 'SUCCEED'

      end
