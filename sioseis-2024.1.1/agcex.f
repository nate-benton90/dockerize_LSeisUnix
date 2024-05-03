      SUBROUTINE AGCEX(BUF,LBUF,IBUF,SCR,LSCR)
!     AGCEX IS THE EXECUTION PHASE OF THE SEISMIC REFLECTION PROCESS AGC.
!  THE USER'S PARAMETERS MUST BE IN
!  DISC FILE MUNIT (IN COMMON /ENER/) AND THE TRACE WITH TRACE HEADER
!  MUST BE IN MEMORY ARRAY BUF.  AGC WINDOW TIMES FOR TRACES BETWEEN
!  THOSE SHOTS OR RPS DESCRIBED BY THE USER ARE CALCULATED BY LINEAR
!  INTERPOLATION.
!
!  ARGUMENTS:
!  BUF    - THE TRACE TO BE PROCESSED, INCLUDING THE TRACE HEADER.  THE FIRST
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
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, JANUARY 1981
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mods:
!  13 April 1991 by pch - add user parameters PCTAGC and CENTER
!
      PARAMETER (NPARS=6)                                                ! THE LENGTH OF EACH PARAMETER LIST
      DIMENSION BUF(111),LBUF(111),IBUF(111),SCR(111),LSCR(111)
      INTEGER*2 IBUF
      COMMON /AGCC/ MUNIT,NLISTS
      COMMON /SIOAP/ IASGND,IRELSE,IN,IOUT,NEXTAD,LAPSIZ,IFREE,IUSEAP
      COMMON /APMEM/A(32766)
      COMMON /AGCCOM/ ISTART,IEND,OLEVEL,CLIP,NPTS,NDEAD,DEAD,MIDPT,
     &          agcpct
      COMMON /READT/ ILUN,NUMHDR
      INTEGER FNO
      LOGICAL FIRST
      SAVE
      DATA FIRST /.TRUE./, llnum/0/
!****
!****     FIND THE PARAMETER LIST (ON DISC) FOR THIS SHOT (RP)
!****
      IF(IBUF(15).EQ.2) RETURN                                          ! IS IT A DEAD TRACE
      ISIG=0
      IF(.NOT.FIRST) GO TO 50
      FIRST=.FALSE.
   10 CONTINUE                                                          ! GET THE FIRST PARAMETER LIST INT0 MEMORY ARRAY SCR
      OLEVEL=32767.
      CLIP=200000.
      dead=.1e-30
      IF(IASGND.EQ.0) GO TO 30
      IF(NEXTAD.LT.LAPSIZ-5) GO TO 30
      PRINT 20
   20 FORMAT(' ***  ERROR  ***  TOO MUCH AP REQUESTED. AGC')
      STOP
   30 CONTINUE
      CALL PODISC(MUNIT,1,0)                                            ! REWIND THE PARAMETER FILE
      CALL RDDISC(MUNIT,SCR,NPARS,ISTAT)
      ISIG=1                                                            ! SET SIGNAL INDICATING THAT PARAM LIST IS IN SCR
      FNO=LSCR(1)
      LNO=LSCR(2)
      MLISTS=1
   50 LNUM=LBUF(3)                                                      !  IS THE DATA ON TAPE SORTED BY SHOT
      IF(LBUF(7).NE.0) LNUM=LBUF(6)                                     !  OR BY RP
      IF(LNUM.EQ.LLNUM.AND.MLISTS.NE.1) GO TO 1000                      ! IS IT THE SAME AS THE LAST SHOT (RP)
      LLNUM=LNUM                                                        ! NO, IT'S NOT THE SAME - DO WE NEED NEW PARAMS
   70 IF(LNUM.GE.FNO) GO TO 100                                         ! IS THIS SHOT BEFORE THIS PARAMTER LIST
      IF(MLISTS.EQ.1) GO TO 500                                         ! IS IT BEFORE THE FIRST LIST
      IF(LNUM.LE.LNO) GO TO 10                                          ! IS IT IN OR BEFORE THE LAST LIST
      GO TO 500                                                         ! IT MUST BE BETWEEN THE 2 LISTS
  100 CONTINUE                                                          !  THE CURRENT SHOT (RP) IS >= LNO
      IF(LNUM.LE.LNO) GO TO 500                                         ! USE THE PARAMETERS OF THIS LIST
      IF(MLISTS.LT.NLISTS) GO TO 110                                    ! ANY MORE USER PARAM LISTS ON DISC
      IF(ISIG.EQ.0) GO TO 1000                                          ! IS THERE A LIST IN MEMORY
      GO TO 500                                                         ! YES THE LAST LIST IS IN SCR
!****
!****   GET ANOTHER USER PARAMETER LIST FROM DISC
!****
  110 CONTINUE                                                          ! SET THE PRESENT LIST INTO OLD SO WE CAN GET A NEW ONE IN SCR
      CALL RDDISC(MUNIT,SCR,NPARS,ISTAT)
      ISIG=1
      FNO=LSCR(1)
      LNO=LSCR(2)
      MLISTS=MLISTS+1
      GO TO 70
!****
!****     SAVE THE CURRENT LIST IN CUR AND LEVS
!****
  500 CONTINUE
      IF(ISIG.EQ.0) GO TO 600
      WINLEN=SCR(3)
      LPRINT=LSCR(4)
      agcpct = scr(5)
      center = scr(6)
      IF(LNUM.LT.FNO) GO TO 600                                         ! DON'T BOTHER IF SPATIALLY VARYING
      GO TO 1000
!****
!****     NO SPATIAL VARIATION ALLOWED
!****
  600 CONTINUE
!****
!****    SETUP THE INDEXES
!****
 1000 CONTINUE
      NSAMPS=IBUF(58)                                                   ! THE NUMBER OF DATA SAMPLES IN THE TRACE
      DELAY=BUF(46)                                                     ! THE FLOATING POINT DEEP WATER DELAY IN SECONDS
      SI=BUF(49)                                                        ! THE FLOATING POINT SAMPLE INTERVAL IN SECONDS
      IDELAY=DELAY/SI
      LTRNO=LBUF(4)                                                     ! THE TRACE NUMBER WITHIN THE SHOT
      IF(LBUF(7).NE.0) LTRNO=LBUF(7)                                    ! THE TRACE NUMBER WITHIN THE RP
      INLEN=WINLEN/SI                                                   ! THE WINDOW LENGTH IN SAMPLES
      IF(IUSEAP.EQ.0.OR.IN.EQ.0) GO TO 2000
!****
!****   DO THE AGC IN THE AP
!****
      PRINT *,' pctagc and center not available in the AP version'
      SCR(1)=1./FLOAT(INLEN)
      SCR(2)=.00001
      SCR(3)=OLEVEL
      CALL APWR                                                         ! WAIT FOR ANY PREVIOUS AP OPERATIONS TO BE COMPLETED
      CALL APPUT(SCR,NEXTAD,3,2)                                        ! PUT THE AGCAP PARAMETERS IN THE AP AT NEXTAD
      ISCR1=NEXTAD+3
       ISCR2=ISCR1+NSAMPS
      CALL FRSTNZ(IN,NSAMPS,N,ISCR1)                                    ! FIND THE FIRST LIVE VALUE
      IF( IAND(LPRINT,2) .NE. 0 ) 
     &    PRINT *,IN,INLEN,NSAMPS,NEXTAD,ISCR1,ISCR2,midpt,agcpct
      CALL APWD
      CALL AGCAP(IN,0,INLEN,IN,NSAMPS,NEXTAD,ISCR1,ISCR2)
      RETURN
!****
!****  DO THE AGC IN HOST MEMORY
!****
 2000 CONTINUE
      IF( buf(48) .NE. 0. ) THEN
          istart = (buf(48) - delay) / si + 1                           ! don't agc the muted data
          IF( istart .LT. 1 ) istart = 1
      ELSE
          istart = 1
      ENDIF
 2010 NDEAD=INLEN/2
      NPTS=INLEN
      midpt = center / si
      SPEC=0.
      IEND = NSAMPS
      IF(ISTART.GE.IEND) RETURN
      IF( IAND(LPRINT,2) .NE. 0 ) 
     &    PRINT *, ISTART,IEND,OLEVEL,CLIP,NDEAD,DEAD,MIDPT,SPEC,NPTS,
     &               agcpct
!****  Subroutine AGC is weird, it uses indeces ISTART and INDEX of the array
!****  rather than the passed arguments.  Therefore always send AGC the entire
!****  trace!
      IF(IN.EQ.0) GO TO 2020
      CALL AGC(A(in),A(in),A(NEXTAD),SPEC)
      RETURN
 2020 CONTINUE
      CALL AGC(BUF(NUMHDR+1),BUF(NUMHDR+1),SCR,SPEC)
      RETURN
      END
