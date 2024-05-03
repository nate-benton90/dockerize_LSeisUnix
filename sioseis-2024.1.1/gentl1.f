      SUBROUTINE GENTL2(IBUF,TLINES,VSCALE,TIMEL,XOFF,idir)
!     GENTL GENERATES AN ARRAY OF TIME LINE RASTERS.  THE OUTPUT IS AN ARRAY
! IN ZEROES AND ONES WITH ONES BEING IN THE APPROPRIATE BITS FOR THE TIME
! LINE MARKINGS.  THE ARRAY SHOULD BE OR'ED WITH THE DATA ARRAY BEFORE SENDING
! TO THE RASTER PLOTTER.
!     THE TIME LINE FREQUENCY WILL BE GENERATED ACCORDING TO THE VALUES IN THE
! ARRAY TLINES , WITH THE VERTICAL SCALE OF VSCALE.  THE FIRST ENTRY OF TLINES
! WILL BE ONE DOT WIDE, THE SECOND WILL BE TWO DOTS WIDE, THE THIRD WILL BE
! THREE DOTS WIDE, ETC.  ODD ENTRIES WILL HAVE THE LINE CENTRED WHILE THE
! EVEN ENTRIES WILL HAVE THE EXTRA LINE PRIOR TO THE CORRECT PLACE.
!
! ARGUMENTS:
!        IBUF   - AN INTEGER*2 ARRAY THAT WILL CONTAIN THE RASTERS.
!                 THE LENGTH OF IBUF MUST BE AT LEAST 180 16 WORDS LONG.
!
!        TLINES - TIME LINE ARRAY.  TIMES ARE FLOATING POINT SECONDS.
!                 UP TO 4 TIME LINES MAY BE GIVEN.
!
!        VSCALE - VERTICAL SCALE IN INCHES PER SECOND OF THE TIME LINES
!                 TO BE GENERATED.
!
!        TIMEL  - TIME LENGTH ( THE MAXIMUM TIME) TO PLOT.
!                 FLOATING POINT SECONDS.
!
!        XOFF   - OFFSET OF THE FIRST POINT IN INCHES.
!                 XOFF "must" be big enough to go past the annotation!
!
!    COMMON REQUIREMENTS:
!    COMMON /VERSAT/NIBS,RNIBS
!      NIBS  - THE NUMBER OF NIBS (DOTS OR RASTERS PER INCH) OF THE PLOTTER.
!              32 BIT INTEGER.
!      RNIBS - THE REAL OF NIBS!
!
!
!   COPYRIGHTED BY:
!         PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, NOVEMBER 1978
! mod 8 Nov 1994 - add color, with rgb and cym.
! mod 25 July 95 - Change dtable for the Alpha
!  20 Apr 98 - Change common defs and colors from 6 to 8
!  13 Jan 04 - Change common defs and colors from 8 to 9
!  29 Nov 01 - Use ibit = NINT(bit) rather than truncation.
!              It makes a difference on the Linux PC!
!  22 Jan 03 - The PC version still screws up.  Added    SAVE
!  15 Aug 06 - heavy timing lines started at the back on ltr plots
!  10 Dec 07 - Honor TRIM
!
!
      DIMENSION IBUF(1),TLINES(1)
      INTEGER*2 IBUF
      COMMON /SEISPL/OFFSET,DEF,PCTFIL,dummy,TRPIN,ITYPE,dummies(4),
     *  STIMEL,dumm1,IUNPLT,WIGGLE,BIAS,idummy,ICDOTS,scalar,clip,itlan,
     &  irectify, ndptr, chart(2), size, itrim
      COMMON /colors/ defs(9), colors(9), ncolors, bcolor, rgb
      INTEGER colors, bcolor, rgb
      COMMON /VERSAT/ NIBS,RNIBS
      COMMON /EDITS/ IERROR,IWARN,IRUN,NOW,ICOMPT
!*** ICOMPT=1 MEANS PRIME, ICOMPT=2 MEANS DEC
      INTEGER*2 ITABLE(16),DTABLE(16)
!****  PRIME GOES 2**15,2**14,2**13,...,2**0
!****  DEC GOES 2**0,2**1,2**2,....,2**15    , THERFORE USE DTABLE
!      DATA ITABLE/:100000,:040000,:020000,:010000,
!     1                    :004000,:002000,:001000,
!     2                    :000400,:000200,:000100,
!     3                    :000040,:000020,:000010,
!     4                    :000004,:000002,:000001/
      DATA ITABLE/-32768,16384,8192,4096,2048,1024,512,
     *          256,128,64,32,16,8,4,2,1/
!      DATA DTABLE /1,2,4,8,16,32,64,128,256,512,1024,
!     *   2048,4096,8192,16384,-32768/
      DATA DTABLE /128,64,32,16,8,4,2,1,
     *      -32768,16384,8192,4096,2048,1024,512,256/
      SAVE
!
      NBITS=IFIX(TIMEL*VSCALE*RNIBS)+1
      IPOINT=0
      NDONE=0
      joff = xoff*rnibs
      OFF=XOFF*RNIBS
      IF( itrim .GT. 0 ) THEN
          IF( idir .GT. 0 .AND. ( itrim .EQ. 1 .OR. itrim .EQ. 2 )) THEN
             off = 17
             joff = 17
          ENDIF
      ENDIF
      IF( idir .LT. 0 ) OFF=XOFF*RNIBS + nbits
      IOFF=OFF
  100 NDONE=NDONE+1
      BITi=OFF
!      MBIT=IFIX(TLINES(NDONE)*VSCALE*RNIBS+.5)                          ! THE INC BETWEEN TLINES
      BIT=TLINES(NDONE)*VSCALE*RNIBS
      IF(TLINES(NDONE).EQ.0.OR.NDONE.GT.4) RETURN
  200 CONTINUE
      ibit = NINT(biti)
! 	 print *,' tllines=',tlines(ndone),' vs=',vscale,' rn=',rnibs,
!     *  ' timel=',timel
!      print *,' ibit=',ibit,' bit=',bit,' ioff=',ioff,' joff=',joff
      IF( idir .LT. 0 .AND. IBIT.LT.JOFF ) GO TO 300
      IF( idir .GT. 0 .AND. ibit .GT. nbits+ioff ) GOTO 300
      IWORD=IBIT/16
      JBIT=IBIT-IWORD*16                                                 ! THE BIT WITHIN IWORD
      IF(JBIT.NE.0) GO TO 220
      IWORD=IWORD-1
      JBIT=16
  220 CONTINUE
      IF( ICOMPT .EQ. 2 .OR. icompt .EQ. 4 ) THEN
          IF( ncolors .EQ. 0 .OR. rgb .EQ. 0 ) THEN
              IBUF(IWORD)=IOR(IBUF(IWORD),DTABLE(JBIT))                  !IT'S A DEC
          ELSE
              IBUF(IWORD)=IAND(IBUF(IWORD),NOT(DTABLE(JBIT)))
          ENDIF
      ELSE
          IF( ncolors .EQ. 0 .OR. rgb .EQ. 0 ) THEN
              IBUF(IWORD)=IOR(IBUF(IWORD),ITABLE(JBIT))                  ! IT'S not DEC
          ELSE
             IBUF(IWORD)=IAND(IBUF(IWORD),NOT(ITABLE(JBIT)))
          ENDIF
      ENDIF
  240 IF( idir .GT. 0 ) biti = biti + bit
      IF( idir .LT. 0 ) BITi=BITi-BIT
      GO TO 200
!
!    MAKE SURE THE NEXT SET OF TIME LINES IS DARKER
  300 CONTINUE
      IF(NDONE.NE.1) GO TO 310
      OFF=OFF+1.                                                        ! CHANGE THE OFFSET BY 1 DOT
      GO TO 100
  310 IF(NDONE.NE.2) GO TO 320
      OFF=OFF-2.                                                        ! 1 DOT BEFORE THE ORIGINAL OFFSET
      GO TO 100
  320 IF(NDONE.NE.3) GO TO 100
      OFF=OFF+3.                                                        ! 2 DOTS AFTER THE ORIGINAL OFFSET
      GO TO 100
      END
