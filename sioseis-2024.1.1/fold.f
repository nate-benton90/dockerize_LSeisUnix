      SUBROUTINE FOLD (LA,A,LB,B,LC,C)
!
!     THIS SUBROUTINE FINDS THE COMPLETE TRANSIENT CONVOLUTION OF THE
!     VECTORS A AND B
!
!     INPUTS ARE
!        LA=LENGTH OF THE VECTOR A    INTEGER*4
!         A=THE VECTOR A, A(1),A(2),...A(LA)      REAL*4
!        LB=LENGTH OF THE VECTOR B      INTEGER*4
!         B=THE VECTOR B, B(1),B(2),...B(LB)    REAL*4
!     OUTPUTS ARE
!        LC=LA+LB-1=LENGTH OF THE VECTOR C     INTEGER*4
!         C=THE COMPLETE TRANSIENT CONVOLUTION VECTOR C     REAL*4
!
!      Robinson, page 29
!
      DIMENSION A(LA),B(LB),C(LC)
      LC=LA+LB-1
      CALL ZERO(LC,C)
      DO I = 1, LA
         DO J = 1, LB
            K = I + J - 1
            C(K) = C(K) + A(I) * B(J)
         ENDDO
      ENDDO
      RETURN
      END
