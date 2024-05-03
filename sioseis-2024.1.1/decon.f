      SUBROUTINE DECON(IDESIG,NDPTS,ITRACE,NSAMPS,PREWHI,NFPTS,IAUTO,
     *   IPREDI,IFILT,IERR,IRESUL,ISCR,double,lprint, igap)
!
!     DECON CALCULATES AND APPLIES A DECON FILTER TO A DATA SET.  THIS
!  ROUTINE OPERATES IN HOST MEMORY WHEREAS SUBROUTINE SDECON IS THE
!  COMPANION ROUTINE FOR THE AP.
!
!  ARGUMENTS:
!  IDESIG - THE INDEX OF APMEM POINTING TO THE START OF THE DESIGN WINDOW.
!  NDPTS  - THE NUMBER OF DATA POINTS IN THE DESIGN WINDOW
!  ITRACE - THE INDEX OF APMEM POINTING TO THE START OF THE APPLICATION WINDOW.
!  NSAMPS - THE NUMBER OF POINTS IN THE APPLICATION WINDOW
!  PREWHI - THE PREWHITENER, AS A PERCENT OF THE ZERO LAG OF THE AUTOCORRELATION.
!           THIS IS A DECIMAL FRACTION!!! AND IS ADDED TO THE ZERO LAG.
!  NFPTS  - THE NUMBER OF FILTER POINTS.
!  IAUTO  - THE INDEX OF AN ARRAY IN APMEM WHERE THE AUTOCORRELATION CAN BE PUT.
!           (A SCRATCH ARRAY AT LEAST NFPTS+IPREDI LONG).
!  IPREDI - THE NUMBER OF SAMPLES OF THE PREDICTION DISTANCE ( 1 OR GREATER!!)
!  IFILT  - THE INDEX OF AN ARRAY IN APMEM OF WHERE THE FILTER CAN BE STORED.
!           (A SCRATCH ARRAY AT LEAST NFPTS+IPREDI LONG).
!  IERR   - THE INDEX OF AN ARRAY IN APMEM WHERE THE ERROR OPERATOR MAY BE STORED.
!           (A SCRATCH ARRAY AT LEAST NFPTS+IPREDI LONG).
!  IRESUL - THE INDEX OF AN ARRAY IN APMEM WHERE THE OUTPUT (RESULTS) CAN BE PLACED.
!           MUST BE NSAMPS LONG.
!  ISCR   - THE INDEX OF AN ARRAY IN APMEM FOR A SCRATCH ARRAY AT LEAST NFPTS+
!           IPREDI LONG.
!
!  COPYRIGHTED:
!  PAUL HENKART, SEISMIC REFLECTION PROCESSORS, SAN DIEGO, CA. 18 JAN 1983
!  ALL RIGHTS RESERVED.
!
!  modifications:
!  17 apr 89 - add double
!  28 Nov 89 - check for the autocorrelation being zero
!  16 Jul 10 - Add lprint to print the filter points.
!  2 Feb 11 - Add igap 
!             This was always setting the gap to the prediction distance
!
      COMMON /APMEM/A(32766)
      N=NFPTS+IPREDI                                                    ! THE LENGTH WE'LL ACTUALLY FILTER WITH!
      IF( double .NE. 1 ) THEN
          CALL CONVO(+2,A(IDESIG),NDPTS,A(IDESIG),NDPTS,A(IAUTO),N+1)   ! THE AUTOCORRELATION
      ELSE
          CALL DCONVO(+2,A(IDESIG),NDPTS,A(IDESIG),NDPTS,A(IAUTO),N+1)  ! THE AUTOCORRELATION
      ENDIF
      IF( a(iauto) .EQ. 0 ) THEN
          PRINT *,' ***  WARNING  *** Autocorrelation is zero.'
          PRINT *,' Deconvolution not done.'
          RETURN
      ENDIF
      CALL DCONVO(+2,A(IDESIG),NDPTS,A(IDESIG+ipredi),NDPTS,
     *            a(icross),N+1)  ! THE AUTOCORRELATION
      FACTOR=1./A(IAUTO)                                                ! SCALE THE AUTOCORRELATION SO THAT LAG(0)=1.
      ITEMP=IAUTO-1
      DO I=1,N
         A(ITEMP+I)=A(ITEMP+I)*FACTOR
      ENDDO
      A(IAUTO)=A(IAUTO)+A(IAUTO)*PREWHI                                  ! ADD IN THE PREWHITENER TO THE ZERO LAG
      ICROSS=IAUTO+IPREDI                                                ! THE ADDRESS OF WHERE THE CROSS-CORRELATION GOES
!	print *,' ipredi=',ipredi,' nfpts=',nfpts,' n=',n
      CALL EUREKA(NFPTS,A(IAUTO),A(ICROSS),A(ISCR),A(IERR))             ! WEINER-LEVINSON
      IF( IAND(lprint,8) .NE. 0 ) THEN
          PRINT *,' The auto-correlation has ',nfpts,' points:'
          DO i = 0, n-1
             PRINT *,a(iauto+i)
          ENDDO
          PRINT *,' The cross-correlation has ',nfpts,' points:'
          DO i = 0, n-1
             PRINT *,a(icross+i)
          ENDDO
      ENDIF
!**** the filter points are in a(iscr)
!*****  gapped decon, 
!      J=IFILT+IPREDI-1
!      IF(IPREDI.LE.1) GO TO 115
!      ITEMP=IPREDI-1
!      DO 110 I=1,ITEMP                                                   !  PUT ZEROES IN FRONT OF THE FILTER
!  110 A(IFILT+I)=0.
!  115 A(IFILT)=1.                                                         ! PUT A 1. IN FRONT OF THE ZEROES AND THE FILTER  (A GAPPED FILTER!)
      J=IFILT+igap-1
!	print *,' igap = ',igap,' ifilt=',ifilt,' j=',j
      DO I=1,NFPTS
         A(J+I)=-A(ISCR-1+I)                                                ! NEGATE THE FILTER COEFFICIENTS AND MOVE TO THE FILTER ARRAY
      ENDDO
      IF( igap .NE. 0 ) THEN
          itemp = igap - 1
          DO i = 1, itemp
             a(ifilt+i) = 0.
          ENDDO
          a(ifilt) = 1.
      ENDIF
      n = nfpts + igap
      IF( IAND(lprint,8) .NE. 0 ) THEN
          PRINT *,' The Weiner filter has ',n,' points:'
          DO i = 0, n-1
             PRINT *,a(ifilt+i)
          ENDDO
      ENDIF
      CALL DCONVO(-1,A(ITRACE),NSAMPS,A(IFILT),N,A(ISCR),NSAMPS+N)        ! FILTER
      J = IRESUL - 1
      K = ISCR - 1
      DO I = 1, NSAMPS
         A(J+I) = A(K+I)
      ENDDO
      RETURN
      END
