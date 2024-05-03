      SUBROUTINE CONVO(ITYPE,BUFIN,NSAMPS,FILTER,NPTS,OBUF,NOUT)
!     CONVO PERFORMS TIME DOMAIN CONVOLUTION AND CORRELATION BETWEEN ARRAYS
! BUFIN AND FILTER.  THE OUTPUT IS RETURNED IN OBUF AND IS  VARIABLE IN LENGTH
! DEPENDING ON THE TYPE OF OPERATION REQUESTED.
!
! ARGUMENTS:
!        ITYPE  - TYPE OF CONVOLUTION OR CORRRELATION DESIGNATOR.
!               =-1,  FULL CONVOLUTION.  THE TIME REFERENCE OF THE FIRST OUTPUT
!                     POINT IS T1+T2, WHERE T1 IS THE TIME OF THE FIRST SAMPLE
!                     IN BUFIN AND T2 IS THE TIME OF THE FIRST SAMPLE IN ARRAY
!                     FILTER.  E.G.  IF A ZERO PHASE FILTER HAS TIME POINTS -19
!                     TO +19 AND THE FIRST SAMPLE IN BUFIN IS AT TIME 0, THEN
!                     THE TIME OF THE FIRST OUTPUT SAMPLE IS -19.
!               =+1,  FULL CORRELATION.  BOTH POSITIVE AND NEGATIVE LAGS ARE
!                     COMPUTED.  CORRELATIONS ARE EVEN FUNCTIONS, THEREFORE HALF
!                     CORRELATIONS GIVE THE SAME INFORMATION AND SHOULD BE USED
!                     MOST OF THE TIME.
!               =+2,  HALF CORRELATION.  ONLY THE RIGHT HALF OF THE CORRELATION
!                     IS PERFORMED..
!        BUFIN  - INPUT ARRAY TO BE FILTERED (CONVOLVED) OR CORRELATED.
!        NSAMPS - THE NUMBER OF SAMPLES IN BUFIN TO FILTER.
!        FILTER - AN ARRAY OF REAL FILTER WEIGHTS.  AN ARRAY OF VALUES TO BE
!                 CORRELATED.
!        MPTS   - THE NUMBER OF POINTS IN THE FILTER ARRAY.
!        OBUF   - THE OUTPUT ARRAY.
!        NOUT   - THE NUMBER OF OUTPUT SAMPLES TO GENERATE.  FOR CONVOLUTION
!                 THIS SHOULD NORMALLY BE MSAMPS+MPTS.  FOR CORRELATION THIS IS
!                 THE NUMBER OF LAGS TO OUTPUT, IT MIGHT BE  MIN0(MSAMPS,MPTS).
!
!   NOTE:  LEADING AND TRAILING ZEROES DO NOT HAVE TO BE GIVEN.
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, FEBRUARY 1979
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
c
c   mod 28 Feb 2011 - correlation was (itype +2) was wrong.
c
      DIMENSION BUFIN(nsamps),FILTER(npts),OBUF(nout)
      NEXTM=1                                                           ! THE INDEX TO THE FIRST FILTER POINT
      I=0
      IF( IABS(ITYPE) .EQ. 1 ) THEN
!****
!****    DO THE FRONT TAPER IF A FULL CONVOLUTION/CORRELATION
!****
          M = NPTS - 1
          IF( ITYPE .LE. 0 ) THEN
              DO 50 N1 = 1, M                                               ! convolution
                 I = I + 1
                 TEMP = 0.
                 DO 40 N2 = 1, N1
                    TEMP = TEMP + BUFIN(N2) * FILTER(N1+1-N2)           !  CONVOLUTION
   40            CONTINUE
                 obuf(i) = temp
   50         CONTINUE
          ELSE
              DO 70 N1 = 1, M                                               ! correlation
                 I = I + 1
                 TEMP = 0.
                 DO 60 N2 = 1, N1
                    TEMP = TEMP + BUFIN(N2) * FILTER(npts-n1-N2)
   60            CONTINUE
                 obuf(i) = temp
   70         CONTINUE
          ENDIF
      ENDIF
!****
!****  DO THE CONVOLUTION OR CORRELATION
!****
      I = I + 1
      JJ = 0
      IF( ITYPE .GE. 0 )THEN
          K = NPTS -1
          IF( ITYPE .EQ. 2 ) JJ = NPTS - 1
          INC = -1
      ELSE
          K = 0
          INC = 1
      ENDIF
      IF( inc .GT. 0 ) THEN
          DO 100 ii = i, nout 
             J = K
             TEMP = 0.
             M = NEXTM
             jjj = ii + jj
             DO 90 j = k, npts - 1
                TEMP = TEMP + BUFIN(jjj-J) * FILTER(M)
                M = M + 1
   90        CONTINUE
             obuf(ii) = temp
             IF( ii .GE. nsamps) THEN
                 k = k + inc
                 nextm = nextm + 1
             ENDIF
  100     CONTINUE
      ELSE
          DO 150 ii = i, nout 
             J = K
             TEMP = 0.
             M = NEXTM
             jjj = ii + jj
             DO 140 j = k, 0, inc
                IF( m .LE. npts .AND. jjj-j .LE. nsamps ) THEN
                    TEMP = TEMP + BUFIN(jjj-J) * FILTER(M)
                    M = M + 1
                ENDIF
  140        CONTINUE
             obuf(ii) = temp
             IF( ii .GE. nsamps) THEN
                 k = k + inc
                 nextm = nextm + 1
             ENDIF
  150     CONTINUE
      ENDIF
      RETURN
      END
