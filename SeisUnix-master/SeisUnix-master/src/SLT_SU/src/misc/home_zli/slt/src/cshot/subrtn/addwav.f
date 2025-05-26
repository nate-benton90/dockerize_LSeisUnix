      subroutine addwav(phase,caustc,nptwav,nptby2,t,ntpts,trace,
     :           amp,wimag,wreal,whead,pi,ntrace,dt,deltat,head,
     :           resamp,maxtr,maxtpt)

c     Adds the wavelet to trace ntrace.  Uses a resampled wavelet
c     to avoid jumps in dipping events (ie, arrival time is not just
c     rounded to nearest sample time).

      integer  nptwav,    nptby2,    ntrace,     resamp,
     :         ntpts,     maxtr,    maxtpt

      real     wreal(*),  wimag(*),  whead(*),   deltat,
     :         phase,     amp,       dt,         t,
     :         pi,        TRACE(MAXTPT,maxtr)

      logical  caustc,    head


c  Local variables:
c  COSPHS  Cosine of PHASE
c  J,K,L   Counters
c  MAXTPT  Maximum samples/trace
c  MAXTR   Maximum number of traces per output panel
c  NPTS    Number of output samples in wavelet
c  NRSAMP  Nearest sample time at rate DELTAT to the arrival time
c  NSAMP   Nearest lesser output sample to arrival time
c  NSHIFT  Number of DELTAT samples between arrival time and output sample
c  NSTART  First output sample number to start adding wavelet to trace
c  SINPHS  Sine of PHASE
c  TNEW    Closest DELTAT sample time to actual arrival time.  TNEW is
c          then treated as the arrival time of the event.
c  TTMP    Output sample time corresponding to sample NSAMP

      real     cosphs,   sinphs,   tnew,   ttmp

      integer j, k, l,   npts,     nrsamp, nsamp,
     :        nshift,    nstart



c     nearest sample to time t (at sample rate deltat)
      nrsamp = nint(t/deltat + 1)

c     tnew is sample time closest to t at sample interval deltat
      tnew = ( nrsamp - 1 ) * deltat

c     this is the output sample <= tnew
      nsamp = ( tnew + deltat/10. ) / dt + 1

c     calculate time difference between out sample time and tnew...
      ttmp = ( nsamp - 1 ) * dt

c     nshift is number of deltat samples between output sample and tnew
      nshift = nint( (tnew-ttmp)/deltat )


      if(nshift.eq.0) then
c        tnew falls right on output sample
         nstart = nsamp - nptby2
         npts   = nptwav
      else
c        tnew is between 2 output samples
c        nshift now becomes # deltat samples that tnew is later
c        than output sample time
         nshift = resamp - nshift
         nstart = nsamp - nptby2 + 1
c        we get one less point in output wavelet for this case
         npts   = nptwav - 1
      end if
         

      if(head) then
c        headwave event
c        the extra shift by resamp/2 here is necessary because
c        resampled wavelet is longer than output wavelet (by one
c        output sample interval)

         l = nshift + 1 + resamp / 2
         DO 100 K = 0,  NPTS - 1
            J = nstart + K 
            IF(J.LT.1.OR.J.GT.NTPTS) THEN
            ELSE
               TRACE(j,NTRACE) = TRACE(j,NTRACE) + AMP * whead(l)
            END IF
            l = l + resamp
100         CONTINUE

      else

         IF(PHASE.EQ.0.) THEN
c           no phase changes due to post critical reflections
   
            IF(CAUSTC) THEN
c              phase shift is - pi / 2
               l = nshift + 1 + resamp / 2
               DO 200 K = 0,  NPTS - 1
                  J = nstart + K 
                  IF(J.LT.1.OR.J.GT.NTPTS) THEN
                  ELSE
                     TRACE(j,NTRACE) = TRACE(j,NTRACE)
     :               + AMP * WIMAG(l)
                  END IF
                  l = l + resamp
200               CONTINUE
            ELSE
c              wavelet has no phase shifts
               l = nshift + 1 + resamp / 2
               DO 300 K = 0,  NPTS - 1
                  J = nstart + K
                  IF(J.LT.1.OR.J.GT.NTPTS) THEN
                  ELSE
                     TRACE(j,NTRACE) = TRACE(j,NTRACE)
     :               + AMP * WREAL(l)
                  END IF
                  l = l + resamp
300               CONTINUE
            END IF

         ELSE

c           some post critical reflections
            IF(CAUSTC) THEN
c              also gone through a caustic
               PHASE = PHASE - PI / 2.
            END IF
            COSPHS = COS(PHASE)
            SINPHS = SIN(PHASE)
            l = nshift + 1 + resamp / 2
            DO 400 K = 0,  NPTS - 1
               J = nstart + K
               IF(J.lt.1.or.j.GT.NTPTS) THEN
c                 beyond end of trace
               ELSE
                  TRACE(j,NTRACE) = TRACE(j,NTRACE) + AMP *
     :            ( COSPHS * WREAL(l) - SINPHS * WIMAG(l) )
               END IF
               l = l + resamp
400            CONTINUE

         END IF

      end if

      return
      end
c---------------------------------------------------------------------
