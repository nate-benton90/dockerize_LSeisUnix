      SUBROUTINE TVFILT(BUFIN,BUFOUT,SCRBUF,FILPTS,LISTL,WLIST,INDEXS,
     *    NDOWS,NSAMPS)
!     TVFILT PERFORMS TIME VARYING FILTERING (TIME DOMAIN CONVOLUTION) FOR ZERO
!  PHASE FILTERS.  NO PROVISION IS MADE FOR FILTERS OF DIFFERENT PHASE LAGS.
!     TIME VARYING FILTER IS PERFORMED BY APPLYING DIFFERENT FILTERS TO DIFFERENT
!  PARTS OF THE TRACE.  THE DIFFERENT PARTS OF THE TRACE ARE CALLED WINDOWS.
!  THE PORTION OF THE TRACE BETWEEN WINDOWS ARE MERGED BY RAMPING (LINEAR).
!  THE MERGE ZONE THUS CONTAINS DATA THAT HAS BEEN FILTERED BY DIFFERENT FILTERS
!  AND THEN ADDED TOGETHER AFTER BEING RAMPED.
!  THE WEIGHTS OF THE WINDOWS CAN BE DIFFERENT, HOWEVER THE MERGE ZONE WILL THEN
!  CONTAIN MORE OF ONE TYPE OF FILTER THAN THE OTHER.
!     E.G.
!               F1            F2            F3
!                        ..........     ..........
!                       .          .   .
!          ..........  .            . .
!                    ..              .
!                   .  .            . .
!                  .    .          .   .
!
!     ARGUMENTS:
!     BUFIN  - THE INPUT ARRAY.  MUST BE NSAMPS LONG.
!     BUFOUT - THE OUTPUT ARRAY.  MUST BE NSAMPS LONG.
!              BUFOUT MUST BE DIFFERENT FROM BUFIN OTHERWISE THE MERGE ZONE WILL
!              WILL BE WRONG.
!     SCRBUF - A SCRATCH ARRAY AT LEAST AS LONG AS THE LONGEST WINDOW PLUS THE
!              FILTER LENGTH.
!     FILPTS - AN ARRAY CONTAINING ALL THE FILTER POINTS.  SUCCESSIVE FILTERS
!              MUST BE CONCATENATED TO THE PREVIOUS.  IN OTHER WORDS, FILPTS
!              STARTS WITH ALL THE FILTER POINTS FOR FILTER 1, THEN FOLLOWED BY
!              THE FILTER POINTS FOR FILTER 2, ETC.
!     LISTL  - AN INTEGER LIST OF FILTER LENGTHS (POINTS).  SINCE EACH FILTER
!              MAY BE A DIFFERENT LENGTH, EACH LENGTH MUST BE SUPPLIED.  THIS
!              ALSO POINTS TO THE FILTERS IN FILPTS.
!     WLIST  - AN ARRAY OR LIST OF WINDOW WEIGHTS.  ALL AMPLITUDES WITHIN THE
!              WINDOW ARE MULTIPLIED BY THE APPROPRIATE WEIGHT AFTER FILTERING.
!              EACH WINDOW MAY HAVE A DIFFERENT WEIGHT.
!     INDEXS - AN ARRAY OF START AND END INDICES FOR EACH WINDOW.  THESE ARE
!              INDICES, NOT TIMES.
!     NDOWS  - THE NUMBER OF WINDOWS OR FILTERS TO PROCESS.
!     NSAMPS - THE NUMBER OF SAMPLES TO PROCESS ON THE WHOLE TRACE.
!              FILTERING IS STOPPED WHEN NSAMPS HAVE BEEN DONE, REGARDLESS OF
!              WINDOW LENGTHS OR INDEXS.  THUS INDEXS MAY EXCEED NSAMPS WITHOUT
!              ILL EFFECTS, HOWEVER IF THERE ARE NOT ENOUGH INDEXS OR
!              FILTERS OR WEIGHTS OR FILTER LENGTHS THEN TVFILT WILL PROBABLY
!              MAKE UTTER GARBAGE.
!
!     COPYRIGHTED BY:
!     PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, MARCH 1979
!
      DIMENSION BUFIN(1),BUFOUT(1),SCRBUF(1),FILPTS(1),LISTL(1),
     *     WLIST(1),INDEXS(1)
      INDEXF=1                                                          ! THE INDEX OF THE CURRENT FILTER
      DO I=1,NSAMPS                                                  ! ZERO THE OUTPUT SINCE WE MAY ADD INTO IT
   50    BUFOUT(I)=0.
      ENDDO
      NUMFIL=1                                                          ! THE CURRENT FILTER NUMBER
      IST=1                                                             !  THE START INDEX
  100 CONTINUE
      IS=INDEXS(2*NUMFIL-1)                                             ! THE START INDEX OF THE CURRENT WINDOW
      IE=INDEXS(2*NUMFIL)                                               !  THE END INDEX OF THE CURRENT WINDOW
      IE1=INDEXS(2*NUMFIL+1)                                            !  THE START INDEX OF THE NEXT WINDOW OR THE
!          END OF THE MERGE ZONE AFTER THE CURRENT WINDOW.
      IF(NUMFIL.EQ.NDOWS) IE1=NSAMPS
      NPTS=LISTL(NUMFIL)                                                !  THE NUMBER OF FILTER POINTS
      N=IE1-IST+NPTS+1                                                  !  THE NUMBER OF POINTS TO FILTER
      NOUT=N+NPTS
!****
!****     FILTER THE WINDOW PLUS THE 2 MERGE ZONES WITH THE CURRENT FILTER
!****
      CALL CONVO(-1,BUFIN(IST),N,FILPTS(INDEXF),NPTS,SCRBUF(1),NOUT)
!****
!****   DO THE UP RAMP OR THE MERGE ZONE PRIOR TO THE WINDOW
!****
      N=IS-IST                                                          !  THE NUMBER OF POINTS IN THE MERGE ZONE
      X=WLIST(NUMFIL)                                                   ! THE WEIGHT OF THE CURRENT WINDOW
      K=IST                                                             !  MAKES ADDING AN INDEX ONE ADD LESS
      J=NPTS/2                                                          ! THE FILTER TAPER - GET TIME ZERO
      IF(N.LE.1) GO TO 120                                              ! DON'T DO THE UP RAMP IF IT DOESN'T EXIST
      RAMP=X/FLOAT(N+1)                                                 !  THE RAMP STEP FOR THE FIRST MERGE
      DO I=1,N
!     ADD THE UP RAMP TO WHAT IS ALREADY IN THE OUTPUT SINCE A DOWN RAMP MIGHT
!            BE THERE ALREADY.
  110    BUFOUT(K+I)=SCRBUF(J+I)*RAMP*FLOAT(I)+BUFOUT(K+I)
      ENDDO
!****
!****       DO THE WINDOW
!****
      J=J+N                                                             ! THE POINTER TO IS IN THE FILTERED DATA (TIME REFERENCE=0)
  120 N=IE-IS+1                                                         !  THE NUMBER OF POINTS IN THE WINDOW
      K=IS-1                                                            ! THE START INDEX-1
      DO I=1,N
  130    BUFOUT(K+I)=SCRBUF(J+I)*X
      ENDDO
      IF(IE.GE.NSAMPS) RETURN
!****
!****          DO THE DOWN RAMP OR THE POST WINDOW MERGE
!****
      J=J+N                                                             !  THE INDEX OF THE START OF THE REAR MERGE ZONE
      N=IE1-IE                                                          !  THE NUMBER OF POINTS IN THE MERGE
      K=IE
      RAMP=X/FLOAT(N)                                                   !  THE RAMP STEP BETWEEN SAMPLES
      DO I=1,N
  200    BUFOUT(K+I)=SCRBUF(J+I)*(X-RAMP*FLOAT(I))
      ENDDO
      INDEXF=INDEXF+LISTL(NUMFIL)
      NUMFIL=NUMFIL+1
      IST=IE
      GO TO 100
      END
