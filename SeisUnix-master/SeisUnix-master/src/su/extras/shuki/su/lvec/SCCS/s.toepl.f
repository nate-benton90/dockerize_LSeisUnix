h18201
s 00000/00000/00041
d D 1.3 88/11/15 14:04:18 shuki 3 2
c 
e
s 00002/00002/00039
d D 1.2 88/05/10 09:08:45 valer 2 1
c 
e
s 00041/00000/00000
d D 1.1 88/05/08 17:29:37 valer 1 0
c date and time created 88/05/08 17:29:37 by valer
e
u
U
t
T
I 1
      SUBROUTINE TOEPL(N,R,G,A,AJ,F)
c
c	DESTINATION:
c	  LEVINSON RECURSION TO SOLVE THE NORMAL EQUATION SYSTEM
c	  (with TOEPLITZ MATRIX RR) on respect of F: |RR|*|F|=|G|
c
c	ENTRANCE:
c	   N         length of inverse filter operator F;
D 2
c	   R         autocorrelation (by length N) of input sygnal;
E 2
I 2
c	   R         autocorrelation (by length N) of input signal;
E 2
c          G         crosscorrelation (by length N) of input 
D 2
c                    with wanted output sygnal;
E 2
I 2
c                    with wanted output signal;
E 2
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
E 1
