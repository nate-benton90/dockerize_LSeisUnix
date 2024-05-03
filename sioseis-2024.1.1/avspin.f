        SUBROUTINE AVSPINTR (APA, APB, AFN, ATM, SZA, BFN, BTM, SZB,
     +      SHFAC, LEN, RES, RTM, INXR, SR)
!$R  AV_____: Perform spatial interpolation of 2 piecewise linear functions.
!
!-------------------------------------------------------------------------------
!       VERITAS SOFTWARE LTD.                   CALGARY, ALBERTA, CANADA
!-------------------------------------------------------------------------------
!
!    Allow linear control point interpolation on the merged set of times.
!
!    See "AV_____" routine "AVINTRP".
!
!    This routine computes:
!         RES(RTM(I)/SR) = AFN(ATM(I)/SR)
!                        + (BFN(BTM(I)/SR) - AFN(ATM(I)/SR)) * SHFAC,
!                   for I = 0, LEN/SR, where the below definitions apply:
!
!    APA,APB = AP-120B scratch arrays.
!    AFN,BFN = functions to be spatially interpolated.
!    ATM,BTM = times associated with the functions.
!    SZA,SZB = sizes of functions (ie. # of elements in arrays AFN/ATM & BFN/BTM
!    SHFAC   = multiplier for shot point interpolation
!    LEN     = length of data + 1.
!    RES     = result function
!    RTM     = merged set of times associated with "RES".
!    INXR    = returned # of elements in RES and RTM.
!    SR      = sample rate of data.
!-------------------------------------------------------------------------------
!
! Revised by:   N.M.M.                          Date:   May, 1987
! Reason:       Modifications for 8192 sample limit. (This change amounts to
!               interpolating the functions at the sample rate of the data,
!               rather than at the data length.)
!-------------------------------------------------------------------------------
!
!        IMPLICIT NONE
        INTEGER  APA, APB, SZA, SZB, LEN, INXR, SR
        INTEGER  AFN(SZA), ATM(SZA), BFN(SZB), BTM(SZB),
     +           RES(200), RTM(200)
        REAL     SHFAC   
!
        INTEGER  INF, I, AFNADR, BFNADR, ATMADR, BTMADR, SHFADR, NSAMP,
     +           MAP, ISW
!
        REAL     TRACSC(0:1000)
        INTEGER  ITRCSC(0:1000)
        COMMON   /TRSCRBUF/ TRACSC
        EQUIVALENCE (ITRCSC,TRACSC)
!******************************************************************************
!
! Define a common block which is used to simulates the AP120-B data memory 
! which has the size of 32K 32-bit floating point words.
!
        REAL    APDATA(0:5000000)
        INTEGER IAPDATA(0:5000000)
        COMMON /apmem/ apdata                                           ! Veritas called this ap120bmd
        EQUIVALENCE (APDATA,IAPDATA)
!
! Define another common block which simulates the 16 S_PAD in the AP120-B
!
        INTEGER APSP(0:15)
        COMMON/AP120BSP/APSP
!
!******************************************************************************
        DATA     INF /0/, bignum/0./
!-------------------------------------------------------------------------------
!
!
!....   Put the 2 piecewise functions and some constants into AP scratch 
!....   location 0 - N.
       IF( len .EQ. 0 .OR. sr .EQ. 0 ) CALL EXIT
        NSAMP  = LEN / SR + 1
!        IF   (INF  .EQ.  0)      INF = MAP ('INF ')
        IF( bignum .EQ. 0. ) bignum = 2147483647.                       ! (=0x7FFFFFFF)
        AFNADR = 0
        BFNADR = SZA
        ATMADR = SZA + SZB
        BTMADR = ATMADR + SZA
        SHFADR = BTMADR + SZB
        DO 100 I = 1, SZA
                TRACSC(AFNADR+I-1) = AFN(I)
                TRACSC(ATMADR+I-1) = ATM(I) / SR
  100   CONTINUE
        DO 200 I = 1, SZB
                TRACSC(BFNADR+I-1) = BFN(I)
                TRACSC(BTMADR+I-1) = BTM(I) / SR
  200   CONTINUE
        TRACSC(SHFADR) = SHFAC
!        CALL MAPAPRA (AFNADR, TRACSC(AFNADR), 2*(SZA+SZB)+1)
        DO i = 0, (sza+szb)*2
           apdata(afnadr+i) = tracsc(afnadr+i)
        ENDDO
        IF (ATM(SZA) .GE. BTM(SZB)) ISW = 1
        IF (ATM(SZA) .LT. BTM(SZB)) ISW = 2
!
!....   Now call the VFC routine to do the spatial interpolation of these 2 
!....   functions.
!        CALL SPINTR (AFNADR, ATMADR, SZA, BFNADR, BTMADR, SZB, INF,
!     +          SHFADR, APA, APB, ISW, NSAMP)
!****  fill both ap scratch arrays with "INF" - changed by pch
       DO 220 i = 0,nsamp-1
          apdata(apa+i) = bignum
          apdata(apb+i) = bignum
  220  CONTINUE
        CALL VSSPINTR (AFNADR, ATMADR, SZA, BFNADR, BTMADR, SZB, INF,
     +          SHFADR, APA, APB, ISW, NSAMP)                             ! this is the ap simulator
!
!....   Now get the necessary control points from AP array APB
!        CALL MAPCPUIA (APB, ITRCSC, 2*(SZA+SZB)+1)
        DO i = 0, (sza+szb)*2
           itrcsc(i) = iapdata(apb+i) 
        ENDDO
!        INXR = ITRCSC(2*(SZA+SZB))
        INXR = tracsc(2*(SZA+SZB))
        DO 300 I = 1, INXR
!                RES(I) = ITRCSC(I-1)
                RES(I) = TRACSC(I-1)
!                RTM(I) = ITRCSC(SZA+SZB+I-1) * SR
                RTM(I) = TRACSC(SZA+SZB+I-1) * SR
  300   CONTINUE
        RETURN
        END
