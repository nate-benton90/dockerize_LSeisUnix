      SUBROUTINE SMUTEX(BUF,LBUF,IBUF,SCR,LSCR)
!     SMUTEX IS THE EXECUTION PHASE OF THE SEISMIC PROCESS MUTE, WHICH MUTES OR
!  ZEROS A PORTION OF A TRACE.  THE MUTE PARAMETERS MUST BE ON DISC FILE MUNIT
!  AND THE TRACE (WITH HEADER) MUST BE IN MEMORY LOCATION BUF.  THE MUTE TIMES
!  FOR TRACES BETWEEN THOSE SHOTS OR RPS DESCRIBED BY THE USER ARE CALCULATED
!  BY LINEAR INTERPOLATION.  LIKEWISE, TIMES FOR TRACES NOT SPECIFIED BY THE
!  USER OBTAINED THROUGH EXTRAPOLATION OR INTERPOLATION.  DEEP WATER DELAYS ARE
!  HONORED - I.E. MUTE TIMES ARE RELATIVE TO TIME ZERO REGARDLESS OF THE TIME
!  OF THE FIRST DATA SAMPLE.
!     SUBROUTINE MUTEED CONTAINS THE EXPLAINATION OF THE USER PARAMETERS AND THE
!  ORDER OF THE USER PARAMETERS ON DISC.
!
!  ARGUMENTS:
!  BUF    - THE TRACE TO BE MUTED, INCLUDING THE TRACE HEADER.  THE FIRST
!           DATA SAMPLE MUST BE AT TIME DELAY.  THIS IS THE FLOATING
!           POINT (REAL) TRACE ARRAY.
!  LBUF   - THE LONG INTEGER TRACE ARRAY.  THIS IS REALLY THE SAME AS BUF, BUT
!           PRIME FORTRAN DOESN'T ALLOW EQUIVALENCING ANYTHING TO AN ARGUMENT.
!  IBUF   - THE SHORT INTEGER TRACE ARRAY.  NEEDED FOR 16 BIT TRACE HEADER
!           ADDRESSES.
!  SCR    - A SCRATCH ARRAY FOR READING THE PARAMETERS.  THEREFORE, SCR MUST
!           BE AT LEAST 56 32BIT WORDS BIG.  SCR MAY BE DESTROYED BY THE CALLING
!           ROUTINE.
!  LSCR   - THE SAME SCRATCH ARRAY BECAUSE OF THE EQUIVALENCING PROBLEM.
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 3 MAY 1982
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!
!  10 Dec 98 - Bizarre situation where default time of 99999 and a VERY
!              small sample interval converted to a negative index. geez
!  23 Aug 91 - make it work (smuted was changed sometime or other and it broke smutex)
!                   - make maxset = 300
!                   - make interp work
!  27 Aug 95 - set stime to delay if delay > stime
!  March 1999 - set the SEG-Y trace header start/end times in mils 
!               (short words 56 & 57)
!  21 March 00 - Spatial interpolation was incorrect.
!  4 Oct 02 - Allow fno 0
!  20 Apr 07 - Make iaddwb a multiplier 
!  12 Apr 10 - Eliminate writing the mute times to the segy header.
!
      PARAMETER (MAXSET = 300)                                          ! THE MAXIMUM NUMBER OF ELEMENTS THE USER ARRAY CAN BE
      PARAMETER (NWRDS=MAXSET+9)                                        ! THE NUMBER OF WORDS IN EACH DISC PAR LIST
      DIMENSION BUF(111),LBUF(111),IBUF(111),SCR(1111),LSCR(1111)
      INTEGER*2 IBUF
      DIMENSION CURSET(MAXSET)
      COMMON /SMUTER/ MUNIT,NLISTS
      COMMON /SIOAP/ IASGND,IRELSE,IN,IOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP
      COMMON /APMEM/A(32766)
      COMMON /READT/ ILUN,NUMHDR
      INTEGER FNO, fno2
      REAL newset(maxset), oldset(maxset)
      LOGICAL FIRST
      SAVE
      DATA FIRST /.TRUE./, fno2/0/
!****
!****     FIND THE PARAMETER LIST (ON DISC) FOR THIS SHOT (RP)
!****
      IF(IBUF(15).EQ.2) RETURN                                          ! IS IT A DEAD TRACE
      ISIG=0
   10 IF( first ) THEN
          first = .FALSE.
          mlists = 1
          CALL podisc( munit, 1, 0 )                                    ! rewind
          CALL rddisc( munit, lscr, nwrds, istat )                      ! read the first parameter list
          fno = lscr(1)
          lno = lscr(2)
          iaddwb = lscr(3)
          nsets = lscr(4)
          ltype = lscr(5)
          lprint = lscr(6)
          interp = lscr(7)
          DO 15 i = 1, nsets
             newset(i) = scr(i+9)
             curset(i) = scr(i+9)
   15     CONTINUE
      ENDIF
      IF( lbuf(7) .EQ. 0 ) THEN
          lnum = lbuf(3)                                                ! shot sorted
          trno = lbuf(4)
      ELSE
          lnum = lbuf(6)                                                ! rp sorted
          trno = lbuf(7)
      ENDIF
   20 IF( lnum .LT. fno ) THEN                                          ! assume shot numbers always increase
          IF( mlists .EQ. 1 ) THEN
              IF( interp .EQ. 0 ) RETURN
              GOTO 500
          ENDIF
      ENDIF
      IF( lnum .LE. lno ) GOTO 500                                      ! is the shot in this list
      IF( mlists .GT. nlists ) THEN
          IF( interp .EQ. 0 ) RETURN
          GOTO 500
      ENDIF
      IF( lnum .GE. fno2 ) THEN
          IF( mlists .GT. 1 ) THEN
              fno = fno2
              lno = lno2
              nsets = nsets2
          ENDIF
          DO 90 i = 1, nsets
             curset(i) = newset(i)
             oldset(i) = newset(i)
   90     CONTINUE
          IF( mlists .LT. nlists ) THEN
              CALL rddisc( munit, lscr, nwrds, istat )                          ! read the next parameter list
              fno2 = lscr(1)
              lno2 = lscr(2)
              nsets2 = lscr(4)
              ltype = lscr(5)
              DO 100 i = 1, maxset
                 oldset(i) = newset(i)
                 newset(i) = scr(i+9)
  100         CONTINUE
          ENDIF
          mlists = mlists + 1
          GOTO 20
      ENDIF
      IF( interp .EQ. 0 ) RETURN
!****
!****   Do the spatial interpolation
!****
      IF( nsets .NE. nsets2 ) THEN
          PRINT *,' ***  ERROR  ***  The number of mutes must be the ',
     &      'same in order to perform spatial variation.'
          STOP
      ENDIF
      DO 300 i = 1, nsets, 3
!         IF( oldset(i) .NE. newset(i) ) THEN
!             PRINT *,' ***  ERROR  ***  The trace numbers or ranges ',
!     &         ' must be the same in each list when spatially varying.'
!             STOP
!         ENDIF
!         curset(i) = oldset(i)
         temp =  (FLOAT(lnum) -FLOAT(lno)) / (FLOAT(fno2)-FLOAT(lno))
         curset(i) = temp * (newset(i) - oldset(i)) + oldset(i)
         curset(i+1) = temp * (newset(i+1) - oldset(i+1)) + oldset(i+1)
         curset(i+2) = temp * (newset(i+2) - oldset(i+2)) + oldset(i+2)
  300 CONTINUE
!****
!****    NOW FIND THE MUTE TIME FOR THE  TRACE NUMBER (OR RANGE)
!****
  500 I=1
      IF( LTYPE .EQ. 1 ) THEN
          TEMP=LBUF(10)                                                 ! TRNO=ABS(LBUF(10) DOESN'T WORK!!!
          TRNO=ABS(TEMP)                                                ! SET THE TRACE NO TO THE RANGE IF XTP WAS GIVEN
      ENDIF
  510 IF( trno .LT. curset(i) ) THEN
          IF( interp .EQ. 0 ) RETURN
          time1 = curset(i+1)
          time2 = curset(i+2)
          GOTO 2000
      ENDIF
      IF( trno .EQ. curset(i) ) THEN
          time1 = curset(i+1)
          time2 = curset(i+2)
          GOTO 2000
      ENDIF
      IF( i+3 .GE. nsets ) THEN                                         ! any more smuts?
          IF( interp .EQ. 0 ) RETURN                                    ! no more smut
          time1 = curset(i+1)
          time2 = curset(i+2)
          GOTO 2000
      ENDIF
      IF( trno .GE. curset(i+3) ) THEN
          i = i + 3
          GOTO 510
      ENDIF
      IF( interp .EQ. 0 ) RETURN                                        ! it's bigger than the last given and no extrapolation
      temp = (trno - curset(i)) / (curset(i+3) - curset(i) )
      time1 = temp * (curset(i+4) - curset(i+1)) + curset(i+1)
      time2 = temp * (curset(i+5) - curset(i+2)) + curset(i+2)
!****
!****   Apply the mute !
!****
 2000 CONTINUE
      SI=BUF(49)                                                        ! THE SAMPLE INTERVAL IN SECONDS
      DELAY=BUF(46)                                                     ! THE DEEP WATER DELAY IN SECONDS
      IF( IADDWB .GT. 0 ) THEN                                          ! ADD IN THE WATER BOTTOM TIME
          time1 = time1 + buf(50) * iaddwb
          time2 = time2 + buf(50) * iaddwb
      ENDIF
      IS = NINT((TIME1-DELAY)/SI) + 1
      IF( is .LT. 1 ) is = 1                                            ! watch out for delay > time1
      IE = NINT((TIME2-DELAY)/SI) + 1                                   ! THE END INDEX OF THE MUTE
      NSAMPS=IBUF(58)                                                   ! 16 BIT NUMBER OF SAMPLES PER TRACE
      IF( TIME2 .EQ. 99999. ) IE = NSAMPS
      IF(IE.LT.IS) IE=IS
      IF(IS.GE.IE) RETURN                                               ! DON'T DO ANY MUTING IF NONE TO DO!
      IF(IE.GT.NSAMPS) IE=NSAMPS
!      IF(IN.NE.0.AND.IUSEAP.NE.0) GO TO 2100                            ! DATA IS IN THE AP IF IN<>0
      IF(IASGND.NE.0.AND.IUSEAP.NE.0) GO TO 2100
      IF(IUSEAP.EQ.0.AND.IN.NE.0) GO TO 2020
      CALL MUTE(BUF(numhdr+1),IS,IE,SI,NSAMPS)                             ! THE IN MEMORY MUTE
      GO TO 2200
 2020 CALL MUTE(A(IN),IS,IE,SI,NSAMPS)
      GO TO 2200
 2100 CONTINUE                                                          ! THE TRACE BELONGS IN THE AP
      CALL INAP(BUF(NUMHDR+1),NSAMPS)                                   ! PUT THE DATA IN THE AP
      IF(IS.NE.1.OR.IE.LT.NSAMPS) GO TO 2150                            ! ARE WE KILLING THE WHOLE TRACE?
      CALL VCLR(IN,1,NSAMPS)                                            ! ZERO THE WHOLE TRACE
      IBUF(15)=2                                                        ! SET THE DEAD TRACE FLAG
      GO TO 2200
 2150 N=5                                                               !  THE LENGTH OF THE TAPER
      CALL APWR                                                         ! MAKE SURE WE DON'T CLOBBER ANYTHING IN PROGRESS IN NEXTAD
      TEMP=.2                                                           ! THE INCREMENT OF THE TAPER BETWEEN POINTS OF THE TAPER
      IF(IE.LE.N) N=0
      IF(IE-IS.LT.N+N) N=0                                              ! WATCH OUT FOR VERY SHORT MUTES
      CALL APPUT(TEMP,NEXTAD,1,2)                                       ! PUT THE INCREMENT IN AP LOCATION NEXTAD
      IIS=IS+IN-1                                                       ! ADD IN THE AP ADDRESS OF THE BEGINNING OF THE TRACE
      IIE=IE+IN-1
      CALL APWD                                                         ! WAIT FOR THE AP DATA TRANSFER TO COMPLETE
!      CALL MUTEAP(IIS,IIE,N,NEXTAD,NSAMPS)
 2200 CONTINUE                                                          !  NOW PUT THE MUTE TIMES IN THE TRACE HEADER
      IF(IS.EQ.1.AND.IE.GE.NSAMPS) IBUF(15)=2                           ! SET THE DEAD TRACE FLAG
!      BUF(52)=(IS-1)*SI+DELAY
!      ibuf(56) = NINT(buf(52)*1000.)                                    ! mute start time in mils
!      BUF(53)=(IE-1)*SI+DELAY
!      ibuf(57) = NINT(buf(53)*1000.)                                    ! mute end time in mils
      IF(IAND(LPRINT,2).EQ.2) PRINT 2210,LNUM,TRNO,(IS-1)*SI+DELAY,
     *   (IE-1)*SI+DELAY,IS,IE
 2210 FORMAT(' SHOT/RP ',I6,' TRACE ',F6.1,' IS MUTED FROM ',F10.4,
     &   ' TO ',F10.4,' (',2I8,')')
      RETURN
      end
