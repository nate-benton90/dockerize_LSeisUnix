      SUBROUTINE INAP(BUFIN,NSAMPS)
!     INAP RELIEVES THE USER OF ALL THE BULLSHIT IN ASSIGNING AND TRANSFERRING
!   THE INITIAL DATA TO THE AP IN A SUBOUTINE.
!     THE SIO CONVENTION IS TO ALWAYS RELEASE THE AP BACK TO THE SYSTEM AS SOON
!   AS POSSIBLE SO OTHER USERS CAN USE IT, BUT THE PROBLEM WITH THAT IS THAT
!   APINIT HAS AN UNGODLY AMOUNT OF OVERHEAD.
!     IF THERE ISN'T AN AP ON THE SYSTEM, THE DATA IS IN COMMON /APMEM/.
!  THIS MEMORY JUST LOOKS LIKE A REAL AP DATA MEMORY SO THAT ALL THE INDEXES
!  AND DATA ADDRESSES CAN REMAIN THE SAME, REGARDLESS OF AP EXISTANCE.
!
!  ARGUMENTS:
!    BUF    - THE ARRAY IN THE HOST THAT MAY OR MAY NOT BE IN THE AP.
!    NSAMPS - THE NUMBER OF SAMPLES IN THE ARRAY BUF.
!
!   PAUL HENKART, SCRIPPS OCEANOGRAPHY, NOVEMBER 1979
!
!  mdsize is the size of the ap main data memory.  If there isn't really
!  an ap on the system and we are going to use an ap simulator, which
!  uses an internal array specified in contro, make it very large!
!  Change the ap memory size in contro.f - set mdsize to the size of md!
!
      PARAMETER (IFMT=2)
!      COMMON /APMEM/ A(32768)                                           ! contro has this allocated!
      COMMON /APMEM/ A(5000000)                                          ! contro has this allocated!
      COMMON /SIOAP/ IASGND,IRELSE,IN,IOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP,
     *          IDECIM, mdsize
!          IASGND - AP ASSIGNMENT SWITCH
!                 =0,  AP IS NOT ASSIGNED - CALL APINIT
!                 =1,  AP IS ASSIGNED - DO NOT CALL APINIT
!          IRELSE - AP RELEASE SWITCH
!                 =0,  AP SHOULD BE RELEASED BEFORE RETURNING
!                 =1,  AP SHOULD BE LEFT ASSIGNED
!          IN     - A SWITCH INDICATING WHETHER THE DATA IS ALREADY IN THE AP
!                 =0,  THE DATA IS NOT IN THE AP.
!                 =1,  THE DATA IS IN THE AP AT ADDRESS IN
!          IOUT   - A SWITCH INDICATING WHETHER THE DATA SHOULD BE LEFT IN THE AP
!                 =0,  THE DATA SHOULD BE MOVED BACK TO THE HOST
!                 =1,  THE DATA CAN BE LEFT IN THE AP WITHOUT AN APGET
!          NEXTAD - THE NEXT AVAILABLE FREE LOCATION IN THE AP.  NEXTAD IS SET
!                   TO THE NEXT LOCATION AFTER NSAMPS WHENEVER NEXTAD IS LESS
!                   THAN NSAMPS AND NSAMPS OF DATA WAS PUT INTO THE AP.
!          LAPSIZ - THE INTEGER*4 SIZE OF THE MAIN DATA OF THE AP.
!          IFREE  - A SIGNAL INDICATING THAT SOME SUBROUTINE HAS SOMETHING IN
!                   THE AP THAT MUST BE SAVED, THUS PREVENTING THE AP FROM BEING
!                   RELEASED.
!                 =0, NO SUBROUTINE HAS ANYTHING TO SAVE
!                 <>0, SOMETHINGS ARE IN THE AP THAT MUST BE SAVED - DO NOT
!                      RELEASE THE AP.
!          IUSEAP - A SIGNAL INDICATING WHETHER TO USE THE AP OR NOT
!                 =0, DON'T USE THE AP (THERE MIGHT NOT BE ONE!)
!                 =1, USE THE AP
!****
!****
!****
!  mod 18 Nov 93 - Change the buffer allocation when the data is
!     bigger than originally allocated (when the message about overwriting
!     came out.
!  mod 29 May 96 - Change the initial trace size allocation to be twice the
!                  trace length rather than 1.5 * the trace length.
!  mod 8 Nov 06 - Increase initial buffer size to nsamps*3 (see above), so 
!                 FFT of reals has some breathing space.
!   mod 19 Dec 06 - Large changes in nsamps still overwrites good stuff
!  1 Apr 09 - Large changes in nsamps still byting me, so allocate 65536 reals
!             (usigned int) to nextad, so in is always 65536 samples.
!
      INTEGER LSIZE
      INTEGER IASGND,IRELSE,IN,IOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP,IDECIM
      INTEGER NSAMPS
      DIMENSION ISIZE(2)
      EQUIVALENCE (ISIZE(1),LSIZE)
      DIMENSION BUFIN(1)
      SAVE nextad1

      IF(IUSEAP.EQ.0) GO TO 1000                                        ! IF NO AP, DO THE SIMULATOR
      IF(IASGND.EQ.1) GO TO 110
      IASGND=1
      CALL APINIT(0,0,ISTAT)                                             !  INITIALIZE THE AP
      IFREE=0                                                            ! IF ANYONE SAVED ANYTHING TOO BAD - APINIT CLEARS MEMORY!
      IN=0                                                              ! THE DATA CAN'T BE IN THE AP!
      NEXTAD=0                                                          ! LIKEWISE FOR AP ALLOCATION
      IF(ISTAT.GT.0) GO TO 100
      PRINT 10,ISTAT
   10 FORMAT(' ***  ERROR  ***  AP ASSIGNMENT FAILED. STATUS=',I4)
      STOP
  100 CONTINUE
      ISIZE(1)=0                                                        ! CLEAR THE UPPER 16 BITS
      CALL SIZEMD(ISIZE(2))                                             ! THE SIZE USES THE SIGN BIT TO GET TO 64K AP WORDS
      LAPSIZ=LSIZE
      IF(LAPSIZ.LE.0) LAPSIZ = mdsize
  110 IF(IDECIM.LE.0) IDECIM=1
!      IF(NEXTAD.EQ.0) NEXTAD=NSAMPS/IDECIM*2                            !  SET THE NEXT AVAILABLE AP ADDRESS
      IF( nextad .EQ. 0 ) nextad = 65536
      IF(IN.NE.0) RETURN                                                  ! THE DATA IS ALREADY IN THE AP
      IF( NSAMPS .GT. NEXTAD ) THEN
          PRINT 120
          nextad = nsamps + nsamps 
      ENDIF
  120 FORMAT(' ***  WARNING  ***  THE DATA GOING INTO THE AP ARE',
     *      ' OVERWRITING PART OF THE RESERVED AP MEMORY.')
      IN=1                                                               ! SET THE AP ADDRESS TO LOCATION 1
      JIN=IN                                                            ! CONVERT TO 16 BIT INTEGER
      JSAMPS=NSAMPS                                                     ! CONVERT TO 16 BIT INTEGER
      CALL APPUT(BUFIN,JIN,JSAMPS,IFMT) 
      CALL APWD                                                          ! WAIT FOR THE DATA TRANSFER
      RETURN
!****
!****   TAKE CARE OF THE AP SIMULATOR MEMORY
!****
 1000 IF( IASGND .EQ. 0 ) THEN
          IASGND = 1
          DO I=1, mdsize
 1001        A(I)=0.                        ! AND SOME PROCESS MAY COUNT ON THERE BEING ZEROES THERE
          ENDDO
         IFREE=0
         IN=0
         NEXTAD=0
         LAPSIZ = mdsize
      ENDIF
      IF( NEXTAD .EQ. 0 ) THEN
!          NEXTAD=NSAMPS+NSAMPS + nsamps
          nextad = 65536
          nextad1 = nextad
      ENDIF
      IF(IN.NE.0) RETURN                                                ! IS THE DATA ALREADY THERE?
      IF( NSAMPS .GT. NEXTAD1 ) THEN
          PRINT 120
          nextad = nsamps + nsamps + nsamps
          nextad1 = nextad
      ENDIF
      DO I=1,NSAMPS
 1020    A(I)=BUFIN(I)                                                      ! MOVE THE DATA TO THE SIMULATOR
      ENDDO
      IN=1

      RETURN
      END
