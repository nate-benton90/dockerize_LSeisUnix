      SUBROUTINE tx2tex( buf, lbuf, ibuf, scr, istop, nready )
!     tx2tex is the execution phase of SIOSEIS proces TX2TP or the
!  transformation from time-space domain to the tau-p space using the
!  slant stack technique developed by Mary Kappus = HOP.
!            HOP is the application of the paper "A new
!            method for slant stacking refraction data" by Henry,
!            Orcutt, and Parker (GRL, Dec 1980).  The problem of
!            slant-stacking seismic records at a number of ranges to
!            synthesize a tau-p curve is posed as a linear inverse 
!            problem for fixed frequency.  Using an inner product
!            weighted by (k^2 + b^2)*k (where k is wavenumber and b
!            some real positive number), then the representers are
!            bessel functions of k*range, scaled by 1/(k^2 + b^2),
!            and the model is U(k,w) (vert comp only).  The inverse
!            of the Gram matrix can be found analytically as sums
!            and differences of products of modified bessel functions
!            of b*range. If desired, the tau-p representers can be
!            used to compute the T-X predictions and the misfit of
!            these predictions to the original data.
!                        M.E. KAPPUS 10/86
!                        latest revision 7/88
!
! 
!            The program is dimensioned for a max of 200 input
!            seismograms (set in parameter statement mdist in routines
!            HOP,BESFL,GBINV,GGINV,MODEL,SPACE.
!     The following has changed and is no longer true  *************
!            Parameter nmax is (2*200*1026) for data and
!            coefficients + (3*nx) for Gram matrix + 1026 for model
!            + 513 for spectrum.  Allows 200 seismograms at 1024 points
!            each producing 200 tau-p grams of the same length.
!            Parameter ndist limits number of tau-p grams to 300.
!                                               
!                                               
!                             VARIABLES
!
!            tmin,tmax - start and end times to read in from input 
!                    traces
!            digit - digitization rate (# samples/sec) of input file -
!            tdpt - time delay per trace - difference in data start 
!                   times for adjacent traces in input data.  If input
!                   data all have same data start time tdpt = 0.
!                   The delay represents a constant reduction velocity,
!                   i.e. start time for each trace must be delayed
!                   by same amount relative to previous trace
!            pmin,pmax,np - first,last and number of p's at which to
!                        calculate models (locations of output traces)
!            b - scaling factor for representers
!            fc - cutoff frequency for calculating models
!            pcnti - % taper applied to input data before FFT
!            pcnto - % taper applied to models before inverse FFT
!            irev -  input t-x traces (1=are, 0=are not) in reverse 
!                    order (decreasing slowness)
!
!            CALCULATED -
!
!            ipntr(n),igpnt,iapnt,iwmod - pointers for starting loca-
!                      tions of transformed seismograms(ss), gram
!                      matrix(gi), coefficients(a), and model(wmod)
!                      in array s (in COMMON ssgia)
!            kf - point number for end freq at which to do calcs
!            limi,limo - number of points on which to apply taper,
!                        based on the % taper chosen by the user
!            lens - the amount of storage required for array s
!            lub2 - closest power of 2 greater than nt - used for FFT
!            mm - the power associated with lub2
!            nt - number of time points to be read in from each trace
!            nx - number of input traces
!            tapi,tapo - the tapers applied to time(freq) series
!
!          **calls subroutines SPACE,PREFORM,
!          **FFT2,REALTR,GGINV,BALPH,MODEL
!
!            NOTE 1.  times actually start at selected tmin (or taumin)
!                     + 1/digit - ie length of time does not include 
!                     start time and does include end time
!            NOTE 2.  values of range (or slowness) may be + or - , but 
!                     must all be of the same sign, and must be of
!                     increasing(irev=0) or decreasing(irev=1) order    
!              The SEGY trace header pointers:
!
!  mod 17 sep 94 - make percents percents by dividing by 100.
!  mod 3  Oct 94 - Change the memory allocation.  Store the data in the
!              transpose array (COMMON/transp/ss(nmax)) until all data
!              have been read and transformed.  Then use the larger ap
!              memory (COMMON /apmem/ s(1)).
!  mod 7 Oct 94 - change mdist = 200 to mdist = 300 to allow 300 input
!              traces. Should allow 300 output traces as well. Bumped
!              up nmax to reflect larger input size from 262144 to 393216
!              Updated tx2ted.f to reflect larger sizes, ***GMK     
!  mod 26 Dec 96 - Allow prestack
!  mod 11 May 07 - Do np preset (to intrcs when np = 0)
!                - make the headers file a segy file.
!  mod 29 May 07 - Use ABS(range) so increasing negative ranges works.
!                - Reset oldrange = 0 after all are done so prestack works
!
      COMMON /segyptr/ llsegptr, lrseqptr, lshotptr, lshtrptr, lrpnptr,
     *                 lrptrptr, itridptr, ldisptr,  lwbdptr,  lsxcoptr,
     *                 lrxcoptr, idelmptr, istmptr,  iendmptr, nsampptr,
     *                 isiptr,   iyrptr,   idayptr,  ihrptr,   iminptr,
     *                 isecptr,  igmtptr,  ldelsptr,  lsmusptr,lemusptr,
     *                 lsisptr,  lwbtsptr, lgatptr,  lssmsptr, lesmsptr,
     *                 lsbptr
!
!             set the TX2TP COMMON blocks
!
      parameter (mdist = 300, ndist = 300, nmax = 262144)
      COMMON/bxs/b,ifx,ilx,nx,x(mdist)
      COMMON/ts/tmin,tmax,nt,tdpt 
      COMMON/ps/pmin,pmax,np,dp,p(ndist) 
      COMMON/digs/digit,fc,lub2,mm,df,kf,dw,ishift,limo
      COMMON/arbs/icomp,irev,ispec,imft
      COMMON/points/ipntr(mdist),igpnt,iapnt,imod(2*mdist)
      COMMON/transp/ss(nmax)
      COMMON /tx2tp/ sshift, sep(2), nnp, setau(2), bb, ffc, ppcnti, 
     *               ppcnto, iirev, fon, dummy, iimft, set(2), lprint,
     *               lunhdr, txprestk, ntx2tp
      INTEGER txprestk
      INTEGER fon
      COMMON /readt/ itunit, numhdr, numdat, ihunit, ireeln, intrcs,
     *               ifmt, nskip, secs, lrenum, isrcf, idtype,
     *               nfskip, jform, itxsi, itxdel
      COMMON /sioap/ iasgnd, irelse, in, iout, nextad, lapsiz, ifree,
     *               iuseap, idecim, mdsize
!****  apmem is 5,000,000
      COMMON /apmem/ s(333333)
      DIMENSION buf(111), lbuf(111), ibuf(111), scr(111)
      INTEGER*2 ibuf
      SAVE
      DATA oldrange/0./, eps/.0001/, nrps/0/
!            set variable types
!
      data da1,da2,pi/3.75,80.0,3.14159265/
      DATA nx/0/
!
!    assume that we only get those traces meant for us, so we don't need
!  a first trace - last trace
!    also assume that process plot does the plotting
!    calculate pointers for array s in COMMON data which holds the data,
!   its spectrum, Gram matrix, the coefficients, and models (SPACE)
!
      IF( istop .LT. 0 ) GOTO 1000                                      ! istop = -1 means there isn't a trace in buf!
      IF( ibuf(itridptr) .GT. 2 ) RETURN
      nx = nx + 1                                                       ! count the traces as they come in
      range = FLOAT(lbuf(ldisptr)) / 1000.                              ! the range (distance) in kilometers
      IF( range .LT. 0. .AND. range .LT. oldrange ) range = ABS(range)
      IF( range .EQ. 0. ) THEN
          range = .00001
          PRINT *,' range of 0. changed to .0001'
      ENDIF
      IF( range .EQ. oldrange ) THEN
          range = range + .00001
          PRINT *,' ***  WARNING  ***  duplicate range ',oldrange,
     *            ' becoming ',range
      ENDIF
      IF( range .LT. oldrange ) THEN                                    ! subroutine gginv requires increasing ranges
          PRINT *,' ***  ERROR  ***  tx2tp requires increasing ranges.'
          STOP
      ENDIF
      oldrange = range
      x(nx) = range
      nsamps = ibuf(nsampptr)
      delay = buf(ldelsptr)                                              ! the time of the first sample in seconds
      si = buf(lsisptr)                                                 ! the sample interval in seconds
      IF( tmin .LT. 0. ) tmin = delay
      IF( ntx2tp .EQ. 0 ) THEN
          itrno = 0
          tmin = set(1) 
          IF( tmin .LT. 0. ) tmin = delay                               ! if user didn't give stime, use the delay of the first trace 
          tmax = set(2)
          IF( tmax .LT. 0. ) tmax = delay + FLOAT(nsamps-1) * si
          digit = 1. / si                                               ! the sample rate = 1./(sample interval)
! OSU Begin July, 1990 Daniel Sattel
! OSU Deleted the next line.  tdpt gets reset later again.
! OLD        tdpt = sshift
! OSU End
          pmin = sep(1)                                                 ! first p value
          pmax = sep(2)                                                 ! last p value
          np = nnp                                                      ! the number of p's to do
          IF( np .EQ. 0 ) np = intrcs
          b = bb
          fc = ffc
          IF( fc .LE. 0 ) fc = digit / 2.
          pcnti = ppcnti / 100.
          pcnto = ppcnto / 100.
          irev = iirev
          imft = iimft
!      compute least power of 2 over seismogram length for the FFT program
          lub2 = 1
          itemp = (nsamps + 1) / 2
          DO j = 1, 12
             mm = j
             lub2 = lub2 + lub2
             IF( lub2 .GE. itemp ) GOTO 120
          ENDDO
          PRINT *,'trace too long - must be less than 2**12'
          STOP
  120     CONTINUE
          taumin = setau(1)
          taumax = setau(2)
          IF( taumin .GT. 99990. ) THEN
              taumin = tmin
              taumax = tmin + (lub2+lub2)*si
          ENDIF
          df = digit/FLOAT(lub2 + lub2)
          nt = lub2 + lub2
          kf = int(fc/df)
          dw = pi * digit/FLOAT(lub2)
          limi = NINT(pcnti * float(nsamps))
          limo = NINT(pcnto * float(kf))
          ishift = MOD(NINT((taumin-tmin)*(digit+eps)),2*lub2)
          IF (ishift .LT. 0) ishift = ishift + 2*lub2
          tdpt = 0.
          IF( IAND(lprint,2) .NE. 0 ) THEN
              PRINT *,'min t  ',tmin,' max t ',tmax,'  nt ',nt
              PRINT *,'1st p  ',pmin,' last p ',pmax,'  np ',np
              PRINT *,'min tau ',taumin,' max tau ',taumax,' dig ',digit
              PRINT *,'beta ',b,' %input taper',pcnti,' % output ',pcnto
              PRINT *,'seis len ',nsamps,' padded to ',2*lub2
              PRINT *,'cutoff freq',fc,' cutoff pnt',kf,' df=',df,
     &         ' ishift=', ishift
          ENDIF
          CALL podisc( ihunit, 1, 0 )
          CALL podisc( lunhdr, 1, 0 )
          CALL rddiscb( ihunit, scr, 3200, istat )
          CALL wrdiscb( lunhdr, scr, 3200 )
          CALL rddiscb( ihunit, scr, 400, istat )
          CALL wrdiscb( lunhdr, scr, 400 )
      ENDIF
      ndone = 0
      ntx2tp = ntx2tp + 1
!****
!****  make sure the first sample is at tmin and the last is at tmax
!****
      IF( tmin .LT. delay ) THEN
          n = (tmin-delay) / si
          DO i = 1, n
             scr(i) = 0.             ! zero fill from tmin to delay
          ENDDO
          ndone = n
      ENDIF
      itemp = ibuf(58)
      ibuf(58) = 0
      CALL wrdisc( lunhdr, lbuf, numhdr )                               ! save the trace header on disk
      ibuf(58) = itemp
      iout = 0                                                          ! tell rlseap to move the data
      CALL rlseap( buf(numhdr+1), nsamps )                              ! get the data out of the ap or ap simulator
!
!     in loop through all traces read the data, taper and
!     pad (PREFORM), Fourier transform (FFT2), and unscramble
!     the real and imaginary parts (REALTR), account for phase 
!     shift if there is varying data start time (FAZE)
!     note:  nt data points are read in and padded
!     out to a power of 2 in PREFORM, but next trace is 
!     written over starting at 2*(kf+1) +1, which is how the
!     pointers were set up in SPACE
!        
      IF( tmin .GT. delay ) THEN
          istart = (delay - tmin) / si
      ELSE
          istart = 1
      ENDIF
      CALL preform( buf(numhdr+istart), limi, nsamps-istart+1, lub2 )
      DO i = 1, nsamps
         scr(ndone+i) = buf(numhdr+istart+i-1)
      ENDDO
      ndone = ndone + nsamps
      IF( ndone .LT. nt+2 ) THEN
          DO i = 1, nt-ndone+2
             scr(ndone+i) = 0.
          ENDDO
      ENDIF
      CALL FFT2( scr, mm, 0 )
      ipntr(nx) = (nx-1) * (kf+1) * 2 + 1
      iptr = ipntr(nx) - 1
      DO i = 1, nt+2
         ss(iptr + i) = scr(i)
      ENDDO
      CALL REALTR( ss(ipntr(nx)), ss(ipntr(nx)+1), lub2, 2 )
      IF(IAND(lprint,2) .NE. 0) PRINT *,' CALL REALTR. ipntr=',ipntr(nx)
! OSU Begin July, 1990 Daniel Sattel
! OSU Added the following line...
      if (nx.gt.1) tdpt=sshift
! OSU End
      IF ( TDPT .NE. 0. ) THEN
           CALL FAZE( nx, ss(ipntr(nx)))
           IF( IAND(lprint,2) .NE. 0 ) PRINT *,' CALL FAZE. tdpt=',tdpt,
     &           ' ipntr=',ipntr(nx)
      ENDIF
      nready = 0                                                        ! no output traces ready yet!
      IF( txprestk .LE. 0 ) THEN
          IF( istop .EQ. 0 ) RETURN
      ELSE
          IF( lbuf(51) .LT. 0 ) nrps = nrps + 1
          IF( nrps .NE. txprestk ) RETURN
          nrps = 0
      ENDIF
!
!            test for enough room in arrays s,x, and p
!
 1000 CONTINUE
      IF( np .EQ. 0 ) np = nx
      dp = (pmax-pmin)/(np-1)
      DO i = 1, np
         p(i) = pmin + (i-1)*dp
      ENDDO
!

      CALL SPACE(nx,np,lens)
      IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL SPACE( ',nx,np,lens
      CALL inap(s,10000)                                                   ! get the ap array allocated
!  new compilers don't like the following for some reason
      PRINT *,' **** WARNING  ****  tx2tp compiler issue.'
      PRINT *,' Please contact the SIOSEIS authors.'
      DO i = 1, nmax
         s(i) = ss(i)
      ENDDO
      IF (NX.GT.MDIST.OR.NP.GT.NDIST.OR.LENS.GT.lapsiz) THEN
          PRINT *,'max storage exceeded - decrease nx or np.'
          PRINT *,' nx=',nx,' np=',np,' lens=',lens
          STOP
      ENDIF
!
!            test for size of arguments of Bessel function to 
!            determine which subroutine to use or if arguments are
!            too large to do at all
!
      CALL BESFL(ymin,ymax,ydif)
      IF( IAND(lprint,2) .NE. 0 ) PRINT *,
     &   ' CALL BESFL, b=',b,' ymin=',ymin,' ymax=',ymax,' ydif=',ydif
!
!            use appropriate subroutine to compute Gram matrix inverse
!            and print it out
!
      IF (ymax.LT.da2) THEN
          ibr = 0
          CALL GGINV(s(igpnt))
          IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL GGINV( ',igpnt
      ELSE IF (ymax.GE.da2.AND.ymin.GT.da1.AND.ydif.LT.da2) THEN
          ibr = 1
          CALL GBINV(s(igpnt))
          IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL GBINV( ',igpnt
      ELSE 
          PRINT *,' ******  TX2TP error   ******'
          PRINT '(a,/,a,/,a)','incompatible range of Bessel fx arguments 
     &        b*x','if b*xmax>80, b*xmin must be >3.75 and b*(2*dx)',
     &        ' must be<80 - adjust ranges of x or b to conform'     
          stop
      ENDIF
!
!            compute the alpha vector of coefficients, putting the 
!            result in s, starting at pointer iapnt
!
      IF( IAND(lprint,2) .NE. 0 ) PRINT *,' CALL BALPH(nx,kf',nx,kf,
     &      ipntr(1), igpnt, iapnt
      CALL BALPH(nx,kf,s(ipntr(1)),s(igpnt),s(iapnt))
!
      nready = np
      itrno = 0
      IF( IAND(lprint,2) .NE. 0 ) PRINT *,' p=',(p(i),i=1,5)
      IF( IAND(lprint,2) .NE. 0 ) PRINT *,' x=',(x(i),i=1,5)
      CALL podiscb( lunhdr, 1, 3600 )
      RETURN
!
      ENTRY getntp( buf, lbuf, ibuf )
!****
!****   return a trace, which is really a tau-p gram
!****
!
!             For each output p, build the model for each 
!             frequency, putting these (for fixed p) in s vector
!             starting at pointer imod (MODEL),
!             taper and pad the models (PREFORM),
!             unscramble the real and imaginary parts (REALTR), 
!             inverse Fourier transform (FFT2).
!
      itrno = itrno + 1                                                 ! increment the trace number
      CALL MODEL(itrno,s(iapnt),s(imod(1)))
      IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL MODEL( ',
     &    itrno,iapnt,imod(1)
      CALL PREFORM(s(imod(1)),limo,2*(kf+1),lub2)  
      IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL PERFORM( ',
     &    imod(1), limo,2*(kf+1),lub2
      CALL REALTR(s(imod(1)),s(imod(1)+1),lub2,-2)
      IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL REALTR( ',
     &     imod(1),imod(1)+1,lub2
      CALL FFT2(s(imod(1)),mm,1)
      IF( IAND(lprint,2) .NE. 0) PRINT *,' CALL FFT2( ',imod(1),mm
!****
!****   create the output trace headers - make it up as we go
!****
      CALL rddisc( lunhdr, buf, numhdr, istat )
      IF( istat .NE. numhdr ) THEN
          CALL podisc( lunhdr, 2, -numhdr )
          CALL rddisc( lunhdr, buf, numhdr, istat )
          IF( istat .NE. numhdr ) PRINT *,
     * ' ***  WARNING  ***  tx2tp had a problem with the trace headers.'
      ENDIF
      IF( lbuf(7) .NE. 0 ) THEN
          lbuf(7) = itrno
      ELSE
          lbuf(4) = itrno
      ENDIF
      ibuf(itridptr) = 1                                                ! it's a live trace
      lbuf(ldisptr) = NINT( p(itrno)*1000.)                             ! put the p value in the range, but make it * 1000.
      itemp = NINT((taumax-taumin)/si)
      numdat = MIN( lub2+lub2, itemp )
      ibuf(nsampptr) = numdat
      ibuf(isiptr) = NINT( si*1000000. )                                ! the sample intervat in milliseconds
      buf(lsisptr) = si                                                 ! sample interval in seconds
      buf(ldelsptr) = taumin                                            ! the tau of the first sample
      ibuf(idelmptr) = NINT(taumin*1000.)
!****
!****   move the trace to buf.  The trick here is that the tau data might
!****  not have the same origin as the time data.  e.g.  If the data is
!****  4 mil, from 9 - 13 secs; after forward and inverse fft, the data
!****  goes from 9 to 13.096 (9+1024*.004). Because of "wraparound", the
!****  same data is for times 0.808 to 4.900, 4.904 to 8.996, 9.000 to 13.092
!****  13.096 to 17.192.
!****  variable ishift was calculated earlier - 
!****          ishift = MOD(NINT((taumin-tmin)*(dig+eps)),2*lub2)
!****
      index = numhdr
      jndex = imod(1) + ishift -1
      n = lub2 + lub2 - ishift
      DO i = 1, n
         buf(index+i) = s(jndex+i)
      ENDDO
      index = numhdr + n
      jndex = imod(1) - 1
      n = numdat - n
      IF( n .GT. 0 ) THEN
          DO i = 1, n
             buf(index+i) = s(jndex+i)
          ENDDO
      ENDIF
      lbuf(51) = 0
      IF( itrno .EQ. np ) THEN                                          ! if no more, reset the trace header
          itrno = 0
          CALL podisc( lunhdr, 1, 0 )
          nx = 0
          ntx2tp = 0
          oldrange = 0.
          IF( txprestk .NE. 0 ) lbuf(51) = -1
      ENDIF
!****
!****   set stuff for the SEGY binary tape header
!****
      in = 0                                                            ! the trace is not in the ap!
      intrcs = np                                                       ! each output record (shot) will contain np traces
      idtype = 7                                                        ! the data type is tau-p
      itxsi = NINT( 1./digit*1000000.)                                  ! the tx domain sample interval in mics
      itxdel = NINT( tmin*1000. )                                       ! the time of start of data in tx in mils
      oldrange = 0
      RETURN
      end
!*********************************************************************
      subroutine balph(nx,kf,ss,gi,a)
!*********************************************************************
!            builds the complex matrix of coefficients, alpha,stored 
!            in array s starting at iapnt.  here it is stored in 3-d
!            rep - 2(real+complex) by nx by kf+1 (number of freqs)
!            called a for alpha.  alpha = sum over x of gram matrix
!            element times fourier-transformed data 
!
!          **CALLS NO OTHER SUBROUTINES**
!
      dimension a(2,nx,kf+1), gi(3*nx-2), ss(2,kf+1,nx)
      do 630 j = 1,kf+1
!
!            compute first element (real+complex) outside of loop
!
      a(1,1,j) = gi(1)*ss(1,j,1) + gi(2)*ss(1,j,2)
      a(2,1,j) = gi(1)*ss(2,j,1) + gi(2)*ss(2,j,2)
      ig = 2
!
!            compute rest (except last) element in loop
!
         do 620 n = 2,nx-1
         a(1,n,j) = 0.0
         a(2,n,j) = 0.0
             do 610 m = 1,3
             im = n + m - 2
             ig = ig + 1
             a(1,n,j) = a(1,n,j) + gi(ig)*ss(1,j,im)
             a(2,n,j) = a(2,n,j) + gi(ig)*ss(2,j,im)
  610        continue
  620    continue
!
!            compute last element (real+complex) outside of loop
!
      a(1,nx,j) = gi(3*nx-3)*ss(1,j,nx-1) + gi(3*nx-2)*ss(1,j,nx)
      a(2,nx,j) = gi(3*nx-3)*ss(2,j,nx-1) + gi(3*nx-2)*ss(2,j,nx)
  630 continue
      return
      end
!*********************************************************************
      subroutine besfl(ymin,ymax,ydif)
!*********************************************************************
!            This subroutine tests for arguments (b*x) of the Bessel
!            functions which will be too large for the inverse Gram 
!            matrix subroutines to handle. Actually the difference
!            between two of these is required, and the failure 
!            conditions depend on this.  Subroutine GGINV can handle
!            computations when both arguments are <80.  Subroutine
!            GBINV can handle the computations if one argument is >80
!            as long as the other is >3.75, and the difference between
!            the two is <80.  This subroutine determines which 
!            Gram matrix subroutine to call.
!
!         ** CALLS NO OTHER SUBROUTINES**
!
      parameter (mdist = 300)
      COMMON/bxs/b,ifx,ilx,nx,x(mdist)
      ymin = b * x(1)
      ymax = b * x(nx)
      ydif = b * 2.0 * (x(nx) - x(1))/float(nx-1)
      return
      end      
!**********************************************************************
      subroutine faze(n,ss)
!**********************************************************************
!            This subroutine corrects for a phase shift if the input
!            data does not have a constant start time - tdpt is the 
!            delay in start time between adjacent traces
!
!          **CALLS NO OTHER SUBROUTINES**
!
      COMMON/ts/tmin,tmax,nt,tdpt
      COMMON/digs/digit,fc,lub2,mm,df,kf,dw,ishift,limo
      dimension ss(2,lub2+1)
      data pi/3.14159265/
      do 400 i = 1,lub2+1
! OSU Begin July, 1990 Daniel Sattel
! OSU Added the "2x"
         arg = (2*dw * (i-1)) * (tdpt * (n-1))
! OLD         arg = (dw * (i-1)) * (tdpt * (n-1))
! OSU End
         tre = cos(arg) * ss(1,i) - sin(arg) * ss(2,i)
         tim = sin(arg) * ss(1,i) + cos(arg) * ss(2,i)
         ss(1,i) = tre
         ss(2,i) = tim
  400 continue
      return
      end
!**********************************************************************
      subroutine gbinv(gi)                                        
!**********************************************************************
!            for the case of large arguments b*x
!            computes the inverse of the gram matrix gi directly.
!            as this is tridiagonal, it is stored as a 3*nx - 2 vector
!          **CALLS FUNCTION ABO**
!
      parameter (mdist = 300)
      COMMON/bxs/b,ifx,ilx,nx,x(mdist)
      dimension gi(3*nx-2),c(2),d(2),b1(mdist),b2(mdist),b3(mdist),
     &          b4(mdist)
!
!            compute products of modified Bessel functions
!
      do 500 n = 2,nx-1
      b1(n) = ABO(b*x(n),b*x(n+1))
      b2(n) = ABO(b*x(n+1),b*x(n))
      b3(n) = ABO(b*x(n+1),b*x(n-1))
      b4(n) = ABO(b*x(n-1),b*x(n+1))
  500 continue
!
!            use AIO type routine to compute factors for term g1
!            and AKO type routine to compute factors for term gn
!
      do 510 i = 1,2
      t = 3.75/(b*x(i))
      d(i)=.39894228+t*(.01328592+t*(.00225319-t*(.00157565-t*(.00916281
     &     -t*(.02057706-t*(.02635537-t*(.01647633-t*.00392377)))))))
      s = 2.0/(b*x(nx-2+i))
      c(i) = 1.25331414-s*(.07832358-s*(.02189568-s*(.01062446-
     &       s*(.00587872-s*(.00251540-s*.00053208))))) 
  510 continue
!
!            compute ratios of 1st two bessel fxs of first kind and
!            last two of 2nd kind.  used to scale first and last
!            matrix elements.
!
      g1 = -(d(2)/d(1)) * exp(b*(x(2)-x(1))) * sqrt(x(1)/x(2))
      gn = -(c(1)/c(2)) * exp(b*(x(nx)-x(nx-1))) * sqrt(x(nx)/x(nx-1))
!
!            compute elements of inverse matrix, using tridiagonality
!
      gi(2) = 1.0/(ABO(b*x(2),b*x(1)) - ABO(b*x(1),b*x(2)))
      gi(1) = g1 * gi(2)
!
      do 520 i = 2,nx-1
      kp = (3*i) - 3
      kd = (3*i) - 2
      km = (3*i) - 1
      gi(kp) = gi(km-3)
      gi(km) = 1.0/(b2(i) - b1(i))
      gid = b4(i) - b3(i)
      gi(kd) = gi(km) * gi(kp) * gid
  520 continue
!
      gi(3*nx-3) = gi(3*nx-4)
      gi(3*nx-2) = gn * gi(3*nx-3)
      return
      end
!**********************************************************************
      subroutine gginv(gi)                                        
!**********************************************************************
!            computes the inverse of the gram matrix gi directly.
!            as this is tridiagonal, it is stored as a 3*nx - 2 vector
!
!          **CALLS FUNCTIONS AIO AND AKO**
!
      parameter (mdist = 300)
      COMMON/bxs/b,ifx,ilx,nx,x(mdist)
      dimension bi(mdist),bk(mdist),gi(3*nx-2)
!
!            compute modified bessel fxs(beta*x)
!
      do 500 n = 1,nx
      bi(n) = AIO(b*x(n))
      bk(n) = AKO(b*x(n))
  500 continue
!
!            compute ratios of 1st two bessel fxs of first kind and
!            last two of 2nd kind.  used to scale first and last
!            matrix elements.
!
      g1 = (-bi(2)/bi(1))
      gn = (-bk(nx-1)/bk(nx))
!
!            compute elements of inverse matrix, using tridiagonality
!
      gi(2) = 1.0/((bi(1)*bk(2)) - (bi(2)*bk(1)))
      gi(1) = g1 * gi(2)
!
      do 520 i = 2,nx-1
      kp = (3*i) - 3
      kd = (3*i) - 2
      km = (3*i) - 1
      gi(kp) = gi(km-3)
      gi(km) = 1.0/((bi(i)*bk(i+1)) - (bi(i+1)*bk(i)))
      gid = ((bi(i+1)*bk(i-1)) - (bi(i-1)*bk(i+1)))
      gi(kd) = gi(km) * gi(kp) * gid
  520 continue
!
      gi(3*nx-3) = gi(3*nx-4)
      gi(3*nx-2) = gn * gi(3*nx-3)
      return
      end
!*****************************************************************
      subroutine model(i,a,wmod)                     
!*****************************************************************
!            forms the representer = Jo(kx)/(k**2 + b**2) and the
!            model = sum of representer times coefficient (alpha)
!            for each freq separately for fixed p.  result is a vector
!            of models, one for each freq.
!            the fourier transform of the models gives tau-p.
!
!          **CALLS FUNCTION AJO**
!                      
      parameter (mdist = 300, ndist = 300)    
      COMMON/bxs/b,ifx,ilx,nx,x(mdist)
      COMMON/ps/pmin,pmax,np,dp,p(ndist)
      COMMON/digs/digit,fc,lub2,mm,df,kf,dw,ishift,limo
      dimension wmod(2,lub2+1),a(2,nx,kf+1)
!             
      do 860 j = 1,kf+1      
      wmod(1,j) = 0.0
      wmod(2,j) = 0.0
      w = float(j-1) * dw
      wp2b2 = (w*p(i))**2 + b**2
!
!            form the nth representer, rep.  multiply by alpha and sum
!            to form the model (for fixed freq), wmod.
!
          do 820 n = 1,nx
          bf = AJO(x(n)*w*p(i))      
          rep = bf/wp2b2
          wmod(1,j) = wmod(1,j) + a(1,n,j)*rep
          wmod(2,j) = wmod(2,j) + a(2,n,j)*rep
  820     continue                        
  860 continue
      return
      end
!*********************************************************************
      subroutine preform(ss,lim,nn,lub2)
!*********************************************************************
!
!           tapers and pads data before Fourier transform
!
!         **CALLS NO OTHER SUBROUTINES**
!          
      dimension ss(2*(lub2+1))                         
      data pi/3.14159265/
!
!           taper with cos**2 taper, amount specified by user
!
      IF (LIM.GT.0) THEN
          do 310 j = 1,lim
             tap = 0.5 * (1.0 - cos((j-1.)*pi/float(lim)))
             ss(j) = ss(j) * tap
             ss(nn-j+1) = ss(nn-j+1) * tap
  310     continue
      ENDIF
!
!           pad end with z1eroes to power of two
!
      IF (NN.LT.2*LUB2) THEN
          do 320 j = nn+1,2*(lub2+1)
             ss(j) = 0.0
  320     continue
      ENDIF
      return
      end
!******************************************************************
      subroutine space(nx,np,lens)
!******************************************************************
!           computes the values of the pointers for array s in COMMON
!           data.  IPNTR locates the beginning of each data trace - 
!           one for each p.  only 2*(kf+1) points are allowed for each
!           trace - accounting for the real and imaginary part of the
!           Fourier-transformed data up to some cutoff point kf assoc-
!           iated with user-selected cutoff frequency fc. one extra
!           'trace' is allowed in order to store the spectrum of the
!           data (it is later overwritten by the spectrum of the model)
!           IGPNT locates the beginning of the inverse Gram matrix
!           IAPNT locates the beginning of the vector of coefficients.
!           IMOD locates the beginning of each model
!           trace - one for each p. 2*(lub2+1) points are allowed.
!
!      **CALLS NO OTHER SUBROUTINES**
!
!            set COMMONs and variable types
!                                                       
      parameter (mdist = 300)
      COMMON/digs/digit,fc,lub2,mm,df,kf,dw,ishift,limo
      COMMON/points/ipntr(mdist),igpnt,iapnt,imod(2*mdist)
!
!            compute pointers
!
      igpnt = ipntr(nx) + lub2+1
      iapnt = igpnt + 3*nx-2
      imod(1) = iapnt + ((kf+1)*2*nx)
!
!             compute overall length
!
      lens = imod(1) + 2*(lub2+1)
      return
      end
!******************************************************************
      FUNCTION AJO(X)                                              
!*********************************************************************
!$$$$ CALLS NO OTHER ROUTINES
!  BESSEL FUNCTION OF THE 1ST KIND, 0TH ORDER, REAL ARGUMENT.
!  POLYNOMIAL APPROXIMATIONS FROM ABRAMOWITZ+STEGUN PP369-370.
      Y=ABS(X)
      IF (Y.GT. 3.0) GO TO 2000
      T=(X/3.0)**2
      AJO=1.0-T*(2.2499997-T*(1.2656208-T*(.3163866-T*(.0444479-T*
     +  (.0039444-T*.00021)))))
      RETURN
 2000 T=3.0/Y
      F=.79788456+T*(-.00000077+T*(-.0055274+T*(-.00009512+T*
     +  (.00137237+ T*(-.00072805+T*.00014476)))))
      A=.78539816-T*(-.04166397+T*(-.00003954+T*(.00262573+T*
     +  (-.00054125 +T*(-.00029333+T*.00013558)))))
      AJO=F*COS(Y-A)/SQRT(Y)
      RETURN
      END                                                               AJO
!*****************************************************************
      FUNCTION AIO(X)                                             
!*****************************************************************
!$$$$$ CALLS NO OTHER ROUTINES
!  MODIFIED BESSEL FUNCTION OF THE 1ST KIND, 0TH ORDER, REAL ARGUMENT
!  POLYNOMIAL APPROXIMATIONS FROM ABRAMOWITZ+STEGUN, P378.
      Y=ABS(X)
      IF (Y.GT. 3.75) GO TO 2000
      T=(X/3.75)**2
      AIO=1.0+T*(3.5156229+T*(3.0899424+T*(1.2067492
     +  +T*(.2659732+T*(.0360768+T*.0045813)))))
      RETURN
 2000 T=3.75/Y
      A=.39894228+T*(.01328592+T*(.00225319-T*(.00157565-T*(.00916281
     +  -T*(.02057706-T*(.02635537-T*(.01647633-T*.00392377)))))))
      AIO=A*EXP(Y)/SQRT(Y)
      RETURN
      END                                                               AIO
!                                                                 
!*****************************************************************
      FUNCTION AKO(X)                                             
!*****************************************************************
!$$$$ CALLS NO OTHER ROUTINES
! COMPUTES MODIFIED BESSESL FUNCTIONS OF THE 2ND KIND (K), REAL ARGUMENT
!  WITH TRUNCATED EXPRESSIONS OF ABRAMOWITZ + STEGUN PP378- 9.
      IF (X.GT. 2.0) GO TO 2000
      T=X/3.75
      T=T*T
      AIO=1.0+T*(3.5156229+T*(3.0899424+T*(1.2067492+T*(.2659732+
     +  T*(.0360768+T*.0045813)))))
      Y=.25*X*X
      AKO=-AIO*ALOG(.5*X)-.57721566+Y*(.42278420+Y*(.23069756+
     +  Y*(.03488590+Y*(.00262698+Y*(.00010750+Y*.00000740)))))
      RETURN
 2000 Y=2.0/X
      T=1.25331414-Y*(.07832358-Y*(.02189568-Y*(.01062446-
     +  Y*(.00587872-Y*(.00251540-Y*.00053208)))))
      AKO=T*EXP(-X)/SQRT(X)
      RETURN
      END                                                               AKO
!*******************************************************************
      FUNCTION ABO(x,y)
!*******************************************************************
!        ***CALLS NO OTHER ROUTINES***
!        COMPUTES PRODUCT OF MODIFIED BESSEL FUNCTION OF 1ST KIND (AIO) 
!        ARGUMENT Y WITH MODIFIED BESSEL FUNCTION OF 2ND KIND (AKO)
!        ARGUMENT X WHEN BOTH ARGUMENTS ARE GREATER THAN 3.75
!        IS JUST COMBINATION OF AIO AND AKO ABOVE
!
!        preliminary part of AKO(x)
      s = 2.0/x
      c = 1.25331414-s*(.07832358-s*(.02189568-s*(.01062446-
     +  s*(.00587872-s*(.00251540-s*.00053208))))) 
!        preliminary part of AIO(y)
      t = 3.75/y
      d = .39894228+t*(.01328592+t*(.00225319-t*(.00157565-t*(.00916281
     +  -t*(.02057706-t*(.02635537-t*(.01647633-t*.00392377)))))))
!        combine fintal expression
      ABO = c*d*exp(y-x)/sqrt(x*y)
      return
      end

