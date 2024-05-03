      SUBROUTINE filters( nt, rvector, srate, f1, f2, idb, ifiltyp,
     &                    minphase, iii )
!     FILTERS produces a frequency domain filter which is multiplied
!  with the transformed time series.  The time series is then inverse
!  transformed back to the time domain.
!
!  ARGUMENTS:
!  nt      - The number of time points in rvector.
!  rvector - The time series to be filtered.  The filtered output is
!            returned in rvector.  10000 max
!  srate   - The sample rate in samples per second.
!  f1      - The low corner frequency.
!  f2      - The high corner frequency.
!  idb     - The INTEGER attenuation in dB at first octave (6dB for each
!            pole)
!  ifiltyp - filter type
!          =0, a combination of low and high pass
!          =1, low pass Butterworth, where f1 is the corner frequency
!          =2, high pass Butterworth, where f1 is the corner frequency
!          =3, notch filter, where f1 is the lower frequency and
!               f2 is the upper one.
!  minphase =1 means minimum phase filter, otherwise it is zero phase.
!  iii     =1, generate a new filter, otherwise no filter is generated
!
!  Author: John Shay, Oregon State Univbersity, April 8, 1988
!  mod 31 jan 92 - change array sizes to 5000 from 10000 so that it
!         compiles on the Apollo!
!                - Also commented out an unused factor = statement.
!                - Correct high pass (type 2)
!  mod 1 July 93 - zero from nt+1 to nfft rather with data!
!  mod 20 Jun 95 by mwh - check trace length and make array lengths a
!                  parameter and increase it to a power of 2!
! mod 16 July 95 by Gail Christeson
!       - reduce output amplitudes by nfft
!       - Fix notch filters
! mod 18 Nov 96 - Alistair - Redo minimum phase so that it works in all cases
!  mod 15 Apr. 97 - Make some of filters.f arrays COMMON.
!  mod 16 Oct 03 - Alistair - Add factor of 2 to minimum phase dropoff
!  ????  change MAXLEN to 16384
!  6 June 20 - Change MAXLEN to 32767
!
      INTEGER MAXLEN
!   IF you change MAXLEN, change contro also!
      PARAMETER( MAXLEN = 32767 * 2 )
      REAL maxabsval, rvector(1)
      LOGICAL firsttime
      COMMON /filtersc/ filt1(MAXLEN), refw(MAXLEN),
     &    filtl(MAXLEN), filth(MAXLEN)
      COMPLEX filt1, refw, filtl, filth
      COMPLEX filt(MAXLEN), filtt(1)
      EQUIVALENCE (filt,filtt)
      SAVE filt, firsttime, nfft, nw
      PARAMETER( pi = 3.14159265 )
      DATA nfftold/0/, f1old/0./, f2old/0./, idbold/0/
!
!  mod. June 95 - Change MAXLEN to 8192
!  mod. 23 Oct 95 - Change wording of error message

!      print *,' filters( ',nt, srate,f1,f2,idb,ifiltyp, minphase,iii
      nfft = 0
!     find the next power of 2 larger than nt
!     find the power of 2 of nfft
      npower2 = 1
      nfft = 2
   10 CONTINUE
      IF( nfft .LT. nt ) THEN
          nfft = nfft + nfft
          npower2 = npower2 + 1
          GOTO 10
      ENDIF
      rnfft = REAL(nfft)
!              force a new filter if this filter length is different from the last
      IF( nfft .NE. nfftold ) firsttime = .TRUE.
      nfftold = nfft

!     force a new filter if the corners are different from the last
      IF( f1 .NE. f1old .OR. f2 .NE. f2old ) firsttime = .TRUE.
      f1old = f1
      f2old = f2
      IF( idb .NE. idbold) firsttime = .TRUE.
      idbold = idb

!      IF( iii .EQ. 1 ) firsttime = .TRUE.

      IF( firsttime ) THEN
          deltafreq = 0.
          tsec = nfft / srate
          nw = nfft / 2

          IF( nfft .GT. MAXLEN .OR. nt .GT. MAXLEN ) THEN
              PRINT *,' ***  ERROR  ***  Too much data for freq filt.'
              PRINT *,' Use fewer samples per trace or change filters.f'
              PRINT *,' nt = ',nt,' nw = ',nw,' nfft=',nfft
              STOP
          ENDIF

          DO 500 i = 1, nw+1
             filt(i)  = CMPLX(1.0, 0.0)
             filtl(i) = CMPLX(1.0, 0.0)
             filth(i) = CMPLX(1.0, 0.0)
  500     CONTINUE

          IF( minphase .EQ. 1 ) deltafreq = 2. * pi / tsec

          IF( ifiltyp .EQ. 0 ) THEN
              poles = FLOAT(idb) / 6. + 1.
              tr = 1. / ((f2-deltafreq) * tsec)

              DO j = 1, nw + 1
  501            filtl(j) = filtl(j) / SQRT(1.+(REAL(j-1)*tr)**poles)
              ENDDO

              tr = 1. / ((f1 + deltafreq) * tsec)

              DO 502 j = 1, nw+1
                 factor = (REAL(j-1) * tr) ** poles
                 filth(j) = filth(j) * SQRT(factor / (1. + factor))
  502         CONTINUE

              DO j = 1, nw+1
  503            filt(j) = filtl(j) * filth(j)
              ENDDO

          ELSEIF (ifiltyp .EQ. 1 ) THEN
              poles = FLOAT(idb) / 6. + 1.
              tr = 1. / ((f1-deltafreq) * tsec)
              DO j = 1, nw + 1
  504            filt(j) = filt(j) / SQRT(1.+(REAL(j-1)*tr)**poles)
              ENDDO

          ELSEIF (ifiltyp .EQ. 2 ) THEN
              poles = FLOAT(idb) / 6. + 1.
              tr = 1. / ((f1+deltafreq) * tsec)
              DO j = 1, nw + 1
                 factor = (REAL(j-1) * tr) ** poles
  505            filt(j) = filt(j) * SQRT(factor / (1. + factor))
              ENDDO

          ELSEIF (ifiltyp .EQ. 3 ) THEN
              index1 = IFIX((f1-deltafreq) * tsec) + 1
              index2 = IFIX((f2-deltafreq) * tsec) + 2
              IF( index2 - index1 .LT. 16 ) THEN
                  PRINT *, ' ***  ERROR  ***  Notch too narrow'
                  STOP
              ENDIF
              tr = 1. - 10. ** (-FLOAT(idb) / 20.)
              DO j = index1, index2
  506            filt(j) = filt(j) *
     &       (1.+tr*(COS(2.*pi*REAL((j-index1)/(index2-index1)))-1.)/2.)
              ENDDO
          ENDIF

          IF( minphase .EQ. 1 ) THEN
!   This is the "efficient" version that reduces the number
! of operations - I have checked it on a simple test case, and believe if it
! doesn't work then the original will not work. However if you want to play
! safe the only real change need is for the rnfft scaling
!  Alistair

              DO 507 i = 1, nw+1
                 IF( REAL(filt(i)) .EQ. 0. ) THEN
                     filt(i) = CMPLX(-30.0,0.0)
                 ELSE
                     filt(i) = CMPLX(ALOG(REAL(filt(i))),0.0)
                 ENDIF
  507         CONTINUE

              DO i = nw+2, nw*2
  508            filt(i) = CONJG(filt(nw*2-i+2))
              ENDDO

              CALL fftinv( filt, npower2 )

              DO 509 i = 2, nw+1
                 r = (1. + COS(pi * REAL(i-1) / nw)) / 2.
                 filt(i) = r * filt(i)
  509         continue
!  509            filt(nw*2+2-i) = r * filt(nw*2+2-i)

!              DO 510 i = 1, nw
!  510            filt1(i) = filt(i)

!              DO 511 i = nw+1, nw*2
!  511            filt1(i) = -CONJG(filt(i))
              DO 511 i = nw+1, nfft
                  filt(i) = cmplx(0., 0.)
  511         continue

              CALL fftfwd( filt, npower2 )
!              CALL fftfwd( filt1 , npower2 )

              maxabsval = 0.

!   straight implementation of the analytic Kramers-Kronig 
!   relationship - ajh
              DO 512 i = 1, nw+1
!                 filt(i) = CEXP((filt(i) + filt1(i))/2./rnfft)
!                 filt(i) = CEXP(filt(i)/rnfft)
!..                                     Add factor of 2 here ajh.
                 filt(i) = CEXP(2.*filt(i)/rnfft)
                 maxabsval = AMAX1(CABS(filt(i)),maxabsval)
  512         CONTINUE

              IF( maxabsval .EQ. 0. ) THEN
                  PRINT *,' Filter is zero, data not filtered.'
                  RETURN
              ENDIF

              DO i = 1, nw+1
  513            filt(i) = filt(i) / maxabsval
              ENDDO
         ENDIF
         firsttime = .FALSE.
      ENDIF

!     the actual filtering follows
      DO i = 1, nt
  519    refw(i) = CMPLX( rvector(i),0. )
      ENDDO
!     zero fill the backend of the data!
      IF( nt+1 .LT. nfft ) THEN
          DO i = nt+1, nfft
!  520    refw(i) = CMPLX( rvector(i),0. )
  520        refw(i) = CMPLX( 0.,0. )
          ENDDO
      ENDIF
      CALL fftfwd( refw, npower2 )

      DO i = 1, nw+1
!       if( filt(i) .ne. 0. ) print *,' i=',i,' filt=',filt(i)
  521    refw(i) = refw(i) * filt(i)
      ENDDO
      DO i = nw+2, nw*2
  522    refw(i) = CONJG(refw(nw*2-i+2))
      ENDDO

      CALL fftinv( refw, npower2 )

      DO i = 1, nt
  523    rvector(i) = REAL(refw(i)) / nfft
      ENDDO

      RETURN
      END
