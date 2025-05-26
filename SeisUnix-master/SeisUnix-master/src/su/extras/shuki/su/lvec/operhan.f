c	
C	DESTINATION: CONSTRUCTION OF A SYMMETRICAL OPERATOR FOR BAND
C	             PASS FILTER.
C
C	DESCRIPTION: BAND IS GIVEN BY FOUR FREQUENCIES: F1,F2,F3,F4.
C	             THE HIGH AND LOW CUT ENDS IN FREQUENCY DOMAIN
C		     ARE APPROXIMATED BY TWO HANNING WINDOWS:
C		    				F - F1 
C			H(F) = 0.5(1 - COS(3.14--------))
C					        F2 - F1
C		
C		    				F - F3 
C			H(F) = 0.5(1 + COS(3.14--------))
C					        F4 - F3
C
C		     
C		    FOR HI-CUT FILTER:       SET F1 = F2 = O.O; 
C		    FOR LOW-CUT FILTER:      SET F3 , F4 >= 1/(2*DT);
c
C	CALL OPERHAN(F,DT,MAXL,OPER,LOP,MID)
C
C	INPUT PARAMETERS: 
C		F(1),F(2),F(3),F(4) - FREQUENCIES (HZ.)
C		DT		    - SAMPLING INTERVAL (SEC.)
C		MAXL		    - MAX.LENGTH (IN SAMPLES)			
C				      OF FULL  SYMMETRICAL OPERATOR	
C
C	OUTPUT PARAMETERS:
C		OPER(1) - OPER(LOP)   KOEFFICIENTS OF OPERATOR
C		LOP		    - LENGTH (IN SAMPLES) OF OPERATOR,
C				      i.e. NUMBER OF KOEFFICIENTS
C		MID	  	    - INDEX OF MIDPOINT OF OPERATOR
c	FORTRAN						VALERY
C
C 	***************************************************************
C
        SUBROUTINE OPERHAN(F,DT,MAXL,OPER,LOP,MID)
	DIMENSION F(1),OPER(1)
	DIMENSION W(4)
	DATA EPSF / 0.01 /
	DATA PI,PI2 / 3.1415926,6.2831852 /
C
C                                                  ROUND OFF MAXL TO ODD
C
	MID = (MAXL + 1)/2
	LOP = 2 * MID - 1
	DO 10 N=1,MID
10	OPER(N) = 0.0	
        FMAX = 0.5/DT
	IF ( F(3) .GT. FMAX ) F(3) = FMAX
	IF ( F(4) .GT. FMAX ) F(4) = FMAX
    	OPER(MID) = DT * ( F(4) + F(3) - F(2) - F(1) )
	IF ( LOP .EQ. 1 ) RETURN          
C
C				if LENGTH OF OPERATOR = 1 then go out
C	
	MID1 = MID - 1
	DO 20 JF=1,4
20	W(JF) = DT * PI2 * F(JF)
	K1 = -1
C
C                                           LOOP  BY PAIR OF FREQUENCIES
C	
	DO 190 JF=1,3,2
	   DF = F(JF+1) - F(JF)
	   IF (ABS(DF) .GT. EPSF) THEN
C
C                                                 F1 <> F2  OR  F3 <> F4
C
	      DTDF = 2.0 * DT * DF  
 	      DTDF = DTDF * DTDF
C
C                                            LOOP BY SAMPLE OF OPERATOR
C                                                                       
 	      DO 50 N=1,MID1
		R = 1.0 - 1.0/DTDF/N/N
		RMULT = (1.0 - 0.25/R)/PI2/N	
	        SUMSIN = SIN(W(JF)*N) + SIN(W(JF+1)*N)
		OPER(MID-N) = OPER(MID-N) + K1 * SUMSIN * RMULT
		OPER(MID+N) = OPER(MID-N)
50	      CONTINUE
C
	  ELSE
C					
C						F1 = F2 OR F3 =F4
C
	    IF ((ABS(F(JF)).GT.EPSF).AND.(ABS(FMAX-F(JF)).GT.EPSF)) THEN
C
C						F2 > 0 OR F3 < FMAX
C
C
C                                            LOOP BY SAMPLE OF OPERATOR
C
 	         DO 70 N=1,MID1
		   OPER(MID-N) = OPER(MID-N) + K1 * SIN(W(JF)*N)/PI/N
		   OPER(MID+N) = OPER(MID-N)
70	         CONTINUE 	
	    ELSE
C					
C					        F2 = O OR F3 = FMAX
C
	    END IF
C
	  END IF
C	
	  K1 = - K1
C
190 	CONTINUE
C
	RETURN
	END
