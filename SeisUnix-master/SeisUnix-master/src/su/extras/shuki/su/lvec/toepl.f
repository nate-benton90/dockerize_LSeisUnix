      SUBROUTINE TOEPL(N,R,G,A,AJ,F)
c
c	DESTINATION:
c	  LEVINSON RECURSION TO SOLVE THE NORMAL EQUATION SYSTEM
c	  (with TOEPLITZ MATRIX RR) on respect of F: |RR|*|F|=|G|
c
c	ENTRANCE:
c	   N         length of inverse filter operator F;
c	   R         autocorrelation (by length N) of input signal;
c          G         crosscorrelation (by length N) of input 
c                    with wanted output signal;
c          A,AJ      working arrays (by length N);
c
c	RESULT:
c          F         inverse filter operator by length N; 
c
      DIMENSION R(1),A(1),F(1),G(1),AJ(1)
      A(1)=1.
      AJ(1)=1.
      V=R(1)
      F(1)=G(1)/R(1)
      DO 1 J=2,N
      A(J)=0.
      F(J)=0.
      E=0.
      RLN=0.
      DO 2 I=2,J
      E=E+R(I)*A(J+1-I)
2     RLN=RLN+R(I)*F(J+1-I)
      CC=-E/V
      V=V+E*CC
      QQ=(G(J)-RLN)/V
      DO 3 I=2,J
3     AJ(I)=A(I)+CC*A(J+1-I)
      DO 4 I=2,J
4     A(I)=AJ(I)
      DO 5 I=1,J
5     F(I)=F(I)+QQ*A(J+1-I)
1     CONTINUE
      RETURN
      END
