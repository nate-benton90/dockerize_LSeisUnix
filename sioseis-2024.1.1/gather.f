      SUBROUTINE GATHER(LTRACE,trace,itrace,lscr,scr,iscr,ISIG,NREADY)
!     GATHER PERFORMS A GATHER (OR TRACE COLLECT OR TRACE SORT) OF SEISMIC TRACES
!  ACCORDING TO A BIN NUMBER (OR RP NUMBER FOR EXAMPLE).  EACH OUTPUT GATHER
!  IS ADDITIONALLY SORTED SO THAT SUCCESSIVE TRACES WITHIN THE GATHER HAVE
!  INCREASING MAGNITUDE OF RANGE.
!     THE INPUT TO GATHER IS ONE TRACE PER CALL.  THE TRACE MUST HAVE THE BIN
!  NUMBER IN BYTES 21-24 AND THE RANGE IN BYTES 37-40 (THIS CORRESPONDS TO
!  A TRACE AND TRACE HEADER IN SEGY TAPE FORMAT).  THE TRACE AND HEADER MUST
!  BE CONTIGUOUS AND THE TOTAL LENGTH IS GATHERED.  EACH TRACE MUST
!  BE NO MORE THAN NWRDS. (NWRDS MUST NOT CHANGE WITHIN A JOB).
!     THE OUTPUT OF GATHER IS A DISC FILE CONTAINING A SET OF TRACES
!  REPRESENTING ONE OR MORE BIN NUMBERS (RP'S).  A GATHER IS OUTPUT ONLY WHEN
!  THE WHOLE GATHER IS COMPLETE.  THUS, THE CALLING PROGRAM MUST CHECK TO SEE
!  IF ANY OUTPUT IS READY OR NOT.  THE OUTPUT DISC FILE CONTAINING THE GATHERS
!  IS STARTED FROM THE BEGINNING ON EACH ENTRY TO PROGRAM GATHER (THUS, THE
!  CALLING PROGRAM MUST MOVE THE GATHERS OUT OF THE OUTPUT FILE BEFORE THE NEXT
!  CALL TO PROGRAM GATHER).  THE NUMBER OF TRACES IN THE OUTPUT FILE IS NREADY
!  AND MAY REPRESENT MORE THAN ONE GATHER.
!     GATHER SETS A -1 IN TRACE HEADER WORD IBUF(91) WHEN THE TRACE IS THE LAST
!  OF AN R.P. (THIS TELLS SUBROUTINE STACK THAT IT'S THE END OF THE R.P.)
!     AN RP OF 524272 INDICATES THAT THE TRACE DOES NOT BELONG TO A GATHER BIN
!  AND WILL BE DROPPED.
!
!  ARGUMENTS:
!
!  TRACE  - THE BUFFER OR ARRAY TO BE GATHERED.  THE TRACE MUST BE NWRDS LONG.
!  LTABLE - AN ARRAY NEEDED TO HOLD THE TABLE OF BIN NUMBER, DISC ADDRESS,
!           RANGE TRIPLETS.  THIS ARRAY MUST BE 3*MAXRPS*MAXTRS 32 BIT WORDS LONG.
!           THE CALLING PROGRAM MUST NOT MODIFY TABLE BETWEEN CALLS TO GATHER.
!  SCR    - A SCRATCH ARRAY NWRDS (32 BIT WORDS) LONG.  SCR MAY BE MODIFIED
!           BY THE CALLING PROGRAM BETWEEN SUCCESSIVE CALLS TO SUBROUTINE GATHER.
!  ISIG   - WHEN SET TO ONE, INDICATES TO SUBROUTINE GATHER THAT ARRAY TRACE DOES
!           NOT HAVE A TRACE AND THAT ALL THE GATHERS HELD BY IT ARE TO
!           BE FLUSHED OUT TO THE OUTPUT FILE.
!  NREADY - THE NUMBER OF TRACES GATHERED INTO THE OUTPUT FILE BY SUBROUTINE
!           GATHER.  NREADY IS SET BY GATHER.  NREADY MAY CONTAIN MORE THAN ONE
!           GATHER.  A ZERO INDICATES THAT NO GATHER IS READY IN THE OUTPUT
!           FILE.
!
!  COMMON NEEDED:
!     COMMON /TCOL/ LSTRP,LRPINC,NWRDS,IOUNIT,MAXRPS,MAXTRS,MINTRS
!  WHERE:
!  LSTRP  - THE STARTING BIN NUMBER (32 BIT INTEGER).  MAY BE NEGATIVE, ZERO,
!           OR POSITIVE.
!  LRPINC - THE INCREMENT OR SKIP CYCLE BETWEEN SUCCESSIVE BIN NUMBERS.  THE
!           ADDITIVE TO LSTRP TO GET THE SECOND BIN NUMBER.  32 BIT INTEGER.
!           MAY BE NEGATIVE OR POSITIVE, BUT MUST NOT BE ZERO.
!  NWRDS  - THE LARGEST NUMBER OF SAMPLES PER TRACE IN THE JOB.  THIS SHOULD BE
!           TRACE LENGTH PLUS THE TRACE HEADER LENGTH.  NWRDS MUST NOT CHANGE
!           BETWEEN CALLS (IT WILL BE IGNORED IF IT DOES!)
!  IOUNIT - THE OUTPUT UNIT NUMBER OF THE DISC FILE WHERE THE OUTPUT GATHERS
!           WILL BE PUT.  THE CALLING PROGRAM MUST OPEN THE FILE AND SET THE
!           UNIT NUMBER.
!  MAXRPS - THE MAXIMUM NUMBER OF BINS (OR RP'S) THAT NEED TO BE HELD ON
!           THE DISC AT ANY ONE TIME.  IN MARINE WORK THE NUMBER OF TRACES
!           PER SHOT WILL SUFFICE SINCE NO TWO UNGATHERED TRACES WITH THE SAME
!           RP NUMBER ARE MORE THAN A CABLE LENGTH AWAY.
!  MAXTRS - THE MAXIMUM NUMBER OF TRACES ANY ONE GATHER CAN HAVE.  IN RP GATHERS
!           THIS IS THE MAXIMUM CDP ALLOWED.
!  MINTRS - THE MINIMUM NUMBER OF TRACES EACH GATHER CAN HAVE.  IF MINTRS=0 AND
!           NO INPUT TRACES CONTRIBUTE TO A GIVEN GATHER, THAT GATHER WILL NOT BE
!           OUTPUT.
!
!
!    COPYRIGHTED BY PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY,
!                   LA JOLLA, CA. 92093
!                   NOVEMBER 1979
!    changed July 14, 1990 to add arguments trace, itrace, scr, iscr and use 'em
!   mod 12 Jan 90 - use maxtrs instead of maxrps for table search limits
!                    when looking for space in the output gather.
!   mod 28 Apr 92 - Drop dead traces (ibuf(15) = 2)
!  mod 16 Nov 92 - Move dead trace check to be first, in case the first
!                  trace is dead and has a bad trace header.
!  mod 3 May 95 - compute nsamps = nwrds - numhdr when generating mintrcs
!  mod 25 Apr 96 - Increase the max allowable traces (maxrps*maxtrs) and
!                - Add a check for exceeding max!
!  mod 2 Oct 97 - Set numdat/nsamps when flushing becuase it may not be
!            set coming in when isig is 0 (e.g. process input did that!)
!  mod May 06 - Delete all the restart stuff (frp).
!             - Redo the maximum maxtrs/maxrps messages.
!             - Check for gathers with more than 2G samples, which will
!               cause the 32 bit disk address to blow up.  podisc uses
!               a 8 byte integer to convert to bytes.
!  mod 25 Sep 08 - Use unsigned integer arithmetic for disc addresses
!  mod 9 Oct 08 - unsigned "-" was wrong for EOG. -nwrds+50 = -(nwrds-50)
!  mod 25 Aug 09 - Change from podisc to podiscun
!  mod 30 Aug 09 - Cygwin didn't like podiscun, so made it podiscun.
!  mod 7 May 10 - Argh.  unsigned disk address logic fails because a negative
!                 disk address was used to indicate the slot was empty.  
!                 Change ltable(2) to be a trace counter rather than disk addr.
!  mod 3 Jun 10 - Add warning if FRP is far away from the firts rp read.
!  mod 5 Jun 20 - Correct stupid error when elimated a DO loop that ended in 
!                 label 590.
!
!****
!****
      PARAMETER (itsize=262144)
      PARAMETER ( maxrp = 800, maxtr = 100)
      PARAMETER (max = 3 * maxtr * maxrp )                              ! 100 cdp, 800 channels max = 240,000 words
      DIMENSION LTRACE(111),LTABLE(max),LSCR(111), trace(111), scr(111)
      INTEGER*2 itrace(111), iscr(111)
      COMMON /transp/t(itsize)
      EQUIVALENCE (ltable(1),t(1))                                      ! assume no one else will use t
      CHARACTER*80 token
      LOGICAL FIRST
      COMMON /TCOL/ LSTRP,LRPINC,NWRDS,IOUNIT,MAXRPS,MAXTRS,MINTRS
      COMMON /SIOAP/ IASGND,IRELSE,IN,IOUT,NEXTAD
      COMMON /READT/ ILUN,NUMHDR,NUMDAT
      SAVE
      DATA FIRST/.TRUE./
!****
!****
      LRANGE=LTRACE(10)
      LRPNO=LTRACE(6)
      NREADY=0                                                          ! THE NUMBER OF TRACES READY IN THE OUTPUT FILE
      IF( ISIG .EQ. 1 ) THEN
          loaddr = 0
          GOTO 440                                                      ! FLUSH ALL THE RP'S?
      ENDIF
      IF( LRPNO .EQ. 524272 .OR. itrace(15) .NE. 1) RETURN              ! IS IT A TRACE TO BE GATHERED
      IF(.NOT.FIRST) GO TO 100
      FIRST=.FALSE.
      CALL GETFIL(1,IUNIT,token,ISTAT)                                  ! GET A TEMPORARY DISC FILE
      CALL GETFIL(1,IOUNIT,token,ISTAT)                                 ! GET A TEMPORARY DISC FILE
      LADDR=1
      IF( nwrds .LE. 60 ) nwrds = numhdr + numdat
!     A DISC FILE MUST BE FILLED (WRITTEN TO) BEFORE YOU CAN POSITION IN IT!
!     THUS, WRITE TO THE FILE AS MUCH AS EVER MAY BE NEEDED.  IN OTHER WORDS, I
!     CAN NOT POSTION TO WORD 1 BEFORE WORD 0 HAS BEEN WRITTEN!
!     I AM NOT USING WORD 0 BECAUSE I CAN'T TELL THE DIFFERENCE BETWEEN 0 AND -0.
!     A NEGATIVE DISC ADDRESS IN THE TABLE MEANS THE DISC SPACE IS AVAILABLE,
!     WHEREAS A POSITIVE ADDRESS MEANS A TRACE IS ALREADY THERE!.
      CALL WRDISC(IUNIT,LTRACE,LADDR)
      IF(LSTRP.EQ.32767) THEN
         LSTRP=LTRACE(6)
      ELSE
         IF( lstrp .LT. ltrace(6) - maxrps - 10 ) THEN
             PRINT *,' ****  WARNING  ****  FRP may be wrong.'
             PRINT *,' FRP is many (',ltrace(6)-lstrp,') rps from first 
     &rp of ',ltrace(6)
             PRINT *,' Process GEOM parameter   lprint 2  may help you.'
         ENDIF
      ENDIF
      LFRP=LSTRP                                                        ! SET THE FIRST BIN NO. IN THE TEMP FILE TO THE FIRST OF THE JOB
      LNXRP=LSTRP                                                       ! SET THE NEXT BIN NO. TO THE FIRST.
      K=1
      NOTRCS=MAXRPS*MAXTRS
      IF( notrcs*3 .GT. itsize ) THEN
          PRINT *,' ***  ERROR  ***  Too many traces to sort. Reduce MAX
     $TRS or MAXRPS.'
          PRINT *,' With MAXRPS ',maxrps,' the largest MAXTRS is:',
     $      itsize/3/maxrps
          PRINT *,' With MAXTRS ',maxtrs,' the largest MAXRPS is:',
     $      itsize/3/maxtrs
          PRINT *,' (MAXTRS is the maximum number of traces per gather.'
          PRINT *,' (MAXRPS is the maximum number of bins needed to acco
     $mmodate several shots.)'
          STOP
      ENDIF
      IF( REAL(notrcs) * REAL(nwrds) .GT. 2147483647.*2. ) THEN
       PRINT *,' ***  ERROR  ***  Too much data in gather (max=16GB)'
          PRINT *,' Reduce MAXTRS, MAXRPS, or NSAMPS.'
          STOP
      ENDIF
!****
!****   ltable(1) = rpno,  (2) = counter/disk address,  (3) = range
!****  Gather assumes every trace is the same length, so we can use an index
!****  or counter to compute the disk address, and thus get 64 bit addresses.
      DO 20 I=1,MAXRPS
      DO 10 J=1,MAXTRS                                                  !  TABLE IS SET UP IN TRIPLETS
          LTABLE(K)=LNXRP                                               ! WORD 1 IS THE RP NUMBER
          LTABLE(K+1)=-LADDR                                            ! WORD 2 IS THE DISC ADDRESS OF THE TRACE
!                                                                       ! WORD 3 IS THE MAGNITUDE OF THE RANGE OR OFFSET OF THE TRACE
!          CALL PODISC(IUNIT,1,LADDR)
          CALL unsigned( '*', laddr, nwrds, ltemp )
          CALL podiscun( iunit, 1, ltemp )
          CALL WRDISC(IUNIT,LTRACE,NWRDS)
!          LADDR=LADDR+NWRDS
          laddr = laddr + 1
          K=K+3
   10 CONTINUE
      LNXRP=LNXRP+LRPINC
   20 CONTINUE
      NUM1=NOTRCS*3
      NUM2=MAXTRS*3
  100 CONTINUE
      loaddr=0                                                          ! THE CURRENT ADDRESS WITHIN THE OUTPUT DISC FILE
  120 CONTINUE
      DO 400 I = 1, NUM1, NUM2                                          ! FIND THE RIGHT BIN NO. IN THE TABLE
         IF(LTABLE(I).NE.LRPNO) GO TO 400
         K = I + 1
         DO 300 J = 1, MAXTRS                                           ! FIND A FREE DISC ADDRESS
            IF( LTABLE(K) .LE. 0 ) THEN
                LTABLE(K) = -LTABLE(K)                                  ! A POSITIVE ADDRESS MEANS THAT IT IS OCCUPIED
                ITEMP = IOUT                                            !  SAVE THE VALUE OF IOUT
                IOUT = 0                                                ! GET THE DATA OUT OF THE AP IF IT IS IN IT!!
                CALL RLSEAP(LTRACE(NUMHDR+1),NUMDAT)
                IOUT = ITEMP                                            !  RESTORE THE ORIGINAL VALUE OF IOUT
                ITEMP = 1
!                CALL PODISC(IUNIT,1,LTABLE(K))
                CALL unsigned( '*', ltable(k), nwrds, ltemp )
                CALL podiscun( iunit, 1, ltemp )
                CALL WRDISC(IUNIT,LTRACE,NWRDS)
                LTABLE(K+1) = IABS(LRANGE)                              ! SAVE THE MAGNITUDE OF THE RANGE FOR FURTHER SORTING
                RETURN
            ENDIF
         k = k + 3
  300    CONTINUE
         PRINT 320, LRANGE,LRPNO
  320   FORMAT(' ***  WARNING  ***  LRANGE ',I10,' OMITTED FROM GATHER',
     *    I10,' DUE TO EXCEEDING THE MAXIMUM TRACES PER BIN.')
         RETURN
  400 CONTINUE                                                          !  LRPNO IS NOT IN THE TABLE!
      IF( LRPINC .GE. 0 ) THEN
          IF(LRPNO.LT.LSTRP) RETURN                                     ! IS THIS BEFORE THE FIRST BIN NO.?
          IF(LRPNO.GE.LNXRP) GO TO 440                                  !  HAVE WE SEEN THIS RP BEFORE?
  410     PRINT 420, LRANGE, LRPNO, MAXRPS
  420   FORMAT(' ***  WARNING  ***  LRANGE',I10,' OMITTED FROM GATHER',
     * I10,' DUE TO BEING MORE THAN',I6,' BINS AWAY FROM THE PREVIOUS.')
          RETURN
      ENDIF
      IF(LRPNO.GT.LSTRP) RETURN                                         ! IS IT BEFORE THE BEGINNING?
      IF(LRPNO.GT.LFRP) THEN                                            ! HAS THIS RP ALREADY GONE BY?
         PRINT 420, LRANGE, LRPNO, MAXRPS
         RETURN
      ENDIF
!****
!****   MOVE A WHOLE GATHER TO THE OUTPUT FILE
!****
  440 CONTINUE                                                          ! NEED TO GET RID OF A GATHER TO MAKE ROOM FOR A NEW ONE
      DO 500 I=1,NUM1,NUM2                                              ! FIND LFRP WITHIN THE TABLE
      I1=I
      IF(LFRP.EQ.LTABLE(I)) GO TO 550
  500 CONTINUE
!      PRINT 510, LFRP
!  510 FORMAT(' ***  ERROR  ***  GATHER IMPOSSIBILITY 1.  LFRP=',I10)
!     UTIG mod  - release the disk unit and return empty handed
      CALL frefil( 3, iunit, istat )
      RETURN
  550 NCDP=0                                                            ! GET RID OF A WHOLE GATHER
  560 LX=999999
      K=I1+1
      DO 590 J=1,MAXTRS                                                 ! FIND THE SHORTEST RANGE
         IF(LTABLE(K).LT.0) GO TO 580                                      ! ANY MORE TRACES IN THE TEMP FILE
         IF(LTABLE(K+1).GT.LX) GO TO 580
         KK=K
         LX=LTABLE(K+1)
  580    K=K+3
  590 continue
      K=KK
      IF(LX.LT.999999) GO TO 600
      IF(NCDP.GE.MINTRS) GO TO 700
!****
!****   WHEN THE GATHER DOESN'T HAVE MINTRS TRACES IN IT, CREATE SOME TRACES!
!****
      DO II=1,NUMHDR                                                ! USE THE TRACE HEADER OF THE TRACE IN LTRACE
  591    LSCR(II)=LTRACE(II)
      ENDDO
      LSCR(6)=LFRP
      iscr(15) = 2                                                      ! signal that the trace is dead
      nsamps = nwrds - numhdr
      numdat = nsamps
      iscr(58) = nsamps
      DO I=1, nsamps
  595    LSCR(NUMHDR+I)=0
      ENDDO
      ITEMP=MINTRS-NCDP
      DO I=1,ITEMP
         NCDP=NCDP+1
         LSCR(7)=NCDP
         LSCR(51)=0                                                     ! CLEAR THE END OF GATHER FLAG
!         CALL PODISC(IOUNIT,1,loaddr)
         CALL podiscun( iounit, 1, loaddr )
         CALL WRDISC(IOUNIT,LSCR,NWRDS)
!         loaddr = loaddr + nwrds
         CALL unsigned( '+', loaddr, nwrds, loaddr )
         NREADY=NREADY+1
      ENDDO
      GO TO 700
!****
!****  MOVE THE TRACE FROM THE TEMP DISC FILE TO THE OUTPUT DISC FILE
!****
  600 CONTINUE                                                          ! MOVE THE TRACE FROM THE TEMP FILE TO THE OUTPUT FILE
!      CALL PODISC(IUNIT,1,LTABLE(K))
      CALL unsigned( '*', ltable(k), nwrds, ltemp )
      CALL podiscun( iunit, 1, ltemp )
      CALL RDDISC(IUNIT,LSCR,NWRDS,ISTAT)
      ITEMP=2
      NCDP=NCDP+1
      LSCR(7)=NCDP                                                      ! THE TRACE NUMBER WITHIN THE GATHER
      numdat = nwrds - numhdr
      iscr(58) = numdat
      LSCR(51)=0                                                        ! CLEAR THE END OF GATHER FLAG
!      CALL PODISC(IOUNIT,1,loaddr)
      CALL podiscun( iounit, 1, loaddr )
      CALL WRDISC(IOUNIT,LSCR,NWRDS)
!      loaddr = loaddr + nwrds
      CALL unsigned( '+', loaddr, nwrds, loaddr )
      NREADY=NREADY+1
      LTABLE(K)=-LTABLE(K)
      GO TO 560
  700 CONTINUE                                                          !  FINISHED A R.P.
!      LADDR=loaddr-NWRDS+50                                            !  SET THE END OF GATHER SIGNAL IN THE HEADER
!  -nwrds+50 = -(nwrds-50)
      CALL unsigned( '-', loaddr, nwrds-50, laddr )
      LTEMP=-1
!      CALL PODISC(IOUNIT,1,LADDR)
      CALL podiscun( iounit, 1, laddr )
      CALL WRDISC(IOUNIT,LTEMP,1)
      LFRP = LFRP + LRPINC
      IF( ISIG .EQ. 1 ) THEN                                            ! ARE WE ONLY FLUSHING RP'S?
          IF( LFRP .LT. LNXRP ) GO TO 440
          CALL FREFIL(3,IUNIT,ISTAT)                                    ! RELEASE THE TEMP FILE
          RETURN
      ENDIF
!****
!****  WE MADE ROOM FOR ANOTHER GATHER, NOW SET IT UP AND USE THE ROOM!
!****
  705 CONTINUE
      KK=I1
      DO 710 J=1,MAXTRS
      LTABLE(KK)=LNXRP
      KK=KK+3
  710 continue
      LNXRP=LNXRP+LRPINC
      GO TO 120                                                         !  WE NOW HAVE ROOM FOR ANOTHER GATHER IN TEMP FILE
      END
