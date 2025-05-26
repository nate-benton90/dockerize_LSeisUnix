CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC         
C                       VCTRIS                                                  
C          VECTOR COMPLEX TRIDIAGONAL MATRIX SOLVER                             
C                                                                               
C                                                                               
C   THERE ARE M (N BY N) COMPLEX TRIDIAGONAL MATRIX EQUATIONS TO SOLVE,         
C EACH ONE HAS THE FOLLOWING FORMAT,                                            
C                                                                               
C    D(1) U(1)  0                  0        X(1)              Y(1)              
C    L(2) D(2) U(2)  0                      X(2)              Y(2)              
C     0   L(3) D(3) U(3)  0                 X(3)              Y(3)              
C          0   ............   0              .        =         .               
C               0   ...........    0         .                  .               
C                    0 L(N-1)D(N-1)U(N-1)   X(N-1)            Y(N-1)            
C     0                   0  L(N)  D(N)     X(N)              Y(N)              
C                                                                               
C   INPUT:                                                                      
C       M         ---  INT*4     NUMBER OF EQUATIONS                            
C       N         ---  INT*4     NUMBER OF UNKNOWNS IN EACH EQUATION            
C       U(M,N,2)  ---  REAL*4    UPPER DIAGONAL COEFFICIENTS                    
C       D(M,N,2)  ---  REAL*4    DIAGONAL COEFFICIENTS                          
C       L(M,N,2)  ---  REAL*4    LOWER DIAGONAL COEFFICIENTS                    
C       X(M,N,2)  ---  REAL*4    RIGHT-HAND SIZE VECTORS (Y)                    
C   OUTPUT:                                                                     
C       X(M,N,2)  ---  SOLUTION VECTORS                                         
C                                                                               
C   NOTE: AFTER VCTRIS, CONTENTS OF D IS DESTROYED                              
C         U(M,N,1),D(M,N,1),L(M,N,1) AND X(M,N,1) STORE REAL PART               
C         U(M,N,2),D(M,N,2),L(M,N,2) AND X(M,N,2) STORE IMAGINARY PART          
C                                                                               
C                                                                               
C     AUTHOR:    ZHIMING LI    3/91                                             
C                                                                               
      SUBROUTINE V2TRIS(M,N,U,D,L,X) 
      REAL U(M,N,2),D(M,N,2),L(M,N,2),X(M,N,2),C1,C2,T1,T2                      
      INTEGER M,N                                                               
C FORWARD ELIMINATION                                                           
      DO J=1,M                                                                  
         C1 = 1./(D(J,1,1)*D(J,1,1)+D(J,1,2)*D(J,1,2))                          
         D(J,1,1) = D(J,1,1)*C1                                                 
         D(J,1,2) = -D(J,1,2)*C1                                                
      END DO                                                                    
      DO I = 2,N                                                                
         DO J=1,M                                                               
            C1 = -L(J,I,1)*D(J,I-1,1)+L(J,I,2)*D(J,I-1,2)                       
            C2 = -L(J,I,1)*D(J,I-1,2)-L(J,I,2)*D(J,I-1,1)                       
            T1 = D(J,I,1)+C1*U(J,I-1,1)-C2*U(J,I-1,2)                           
            T2 = D(J,I,2)+C1*U(J,I-1,2)+C2*U(J,I-1,1)                           
            T = 1./(T1*T1+T2*T2)                                                
            D(J,I,1) = T1 * T                                                   
            D(J,I,2) = -T2 * T                                                  
            X(J,I,1) = X(J,I,1)+X(J,I-1,1)*C1-X(J,I-1,2)*C2                     
            X(J,I,2) = X(J,I,2)+X(J,I-1,2)*C1+X(J,I-1,1)*C2                     
         END DO                                                                 
      END DO                                                                    
C BACK SUBSTITUTION                                                             
      DO J=1,M                                                                  
         XX = X(J,N,1)                                                          
         X(J,N,1) = X(J,N,1)*D(J,N,1) - X(J,N,2)*D(J,N,2)                       
         X(J,N,2) = X(J,N,2)*D(J,N,1) + XX * D(J,N,2)                           
      END DO                                                                    
      DO I = N-1,1,-1                                                           
         DO J=1,M                                                               
            C1 = X(J,I,1)-X(J,I+1,1)*U(J,I,1)+X(J,I+1,2)*U(J,I,2)               
            C2 = X(J,I,2)-X(J,I+1,1)*U(J,I,2)-X(J,I+1,2)*U(J,I,1)               
            X(J,I,1) = C1*D(J,I,1) - C2*D(J,I,2)                                
            X(J,I,2) = C1*D(J,I,2) + C2*D(J,I,1)                                
         END DO                                                                 
      END DO                                                                    
      RETURN                                                                    
      END                                                                       
