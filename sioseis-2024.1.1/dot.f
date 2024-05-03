      SUBROUTINE DOT(L,X,Y,P)
      DIMENSION X(1),Y(1)                         
C
C     DOT IS THE DOT PRODUCT OF TWO VECTORS
C     REFERENCE: EAR MULTICHANNEL VOLUME, P. 20.
C                                                
C     INPUTS ARE                                
C        L=LENGTH OF THE VECTORS X AND Y       
C        X=THE VECTOR X, X(1),X(2),...,X(L)   
C        Y=THE VECTOR Y Y(1),Y(2),...,Y(L)   
C     OUTPUTS ARE                           
C        P=DOT PRODUCT OF X AND Y
C
C                L                                                      DOT70160
C COMPUTES    P= SUM(X(I)*Y(I))   IF  L  GRTHN 0   
C                I=1                              
C                                                                       DOT70190
C             P= 0                IF  L  EQLSTHN 0
C                                                                       DOT70210
      P=0.0            
!      IF (L)3,3,1     
!    1 DO 2 I=1,L     
!    2 P=P+X(I)*Y(I) 
!    3 RETURN       
      IF( l .LE. 0 ) RETURN
      DO i = 1, l
         p = p + x(i) * y(i)
      ENDDO
      RETURN
      END                                                               DOT70270
