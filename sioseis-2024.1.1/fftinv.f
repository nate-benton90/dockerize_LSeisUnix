       SUBROUTINE FFTINV(A,M)
! COMPUTES AND RETURNS THE INVERSE FFT.  THIS IS BASICALLY THE ONE TAKEN
! FROM OPPENHEIM AND SCHAFER, PAGE 331.
! ARGUMENTS:
! A - THE COMPLEX INPUT ARRAY TO BE TRANSFORMED.  THIS IS ALSO THE OUTPUT!!
! M - THE POWER OF 2 OF THE FFT.  THERE MUST BE 2**M COMPLEX POINTS IN A.
!
      COMPLEX A(1),U,W,T
      DATA PI/3.1415926535/
      N=2**M
      NV2=N/2
      NM1=N-1
      J=1
      DO I=1,NM1    !   was do 7
         IF( i .LT. j ) THEN
             T=A(J)
             A(J)=A(I)
             A(I)=T
         ENDIF
         K=NV2
6        IF( k .LT. j ) THEN
             J=J-K
             K=K/2
             GOTO 6
         ENDIF
         J=J+K
7     ENDDO
      LE=1
      DO L=1,M    !  was DO 20
         LE1=LE
         LE=2*LE
         U=(1.,0.)
         B=PI/FLOAT(LE1)
         W=CMPLX(COS(B),-SIN(B))
         DO J=1,LE1    ! was do 20
            DO I=J,N,LE   ! was do 10
               IP=I+LE1
               T=A(IP)*U
               A(IP)=A(I)-T
               A(I)=A(I)+T
10          ENDDO
20          U=U*W
         ENDDO
      ENDDO    !   U is reset on each interation of the loop
      RETURN
      END
