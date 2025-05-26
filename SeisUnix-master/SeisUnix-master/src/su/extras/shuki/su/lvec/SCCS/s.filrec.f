h36302
s 00000/00000/00086
d D 1.2 88/11/15 14:04:28 shuki 2 1
c 
e
s 00086/00000/00000
d D 1.1 88/06/13 14:04:11 valer 1 0
c date and time created 88/06/13 14:04:11 by valer
e
u
U
t
T
I 1
c
c**********************************************************************
c	subroutine filrec(la,a,lb,b,lx,x,idir,y) 
c
c	DESTINATION:
c		RECURSIVE FILTERING (direct realization).
c
c	   The output Y(k),input X(k) are connected 
c	   by the recursive relationship: 
c	   
c	          la                   lb
c	   Y(k) = SUM[a(i)*X(k-i+1)] + SUM[b(i)*Y(k-i)]   (1)
c	          i=1                  i=1
c	   expressed in one-sided z-transform as
c
c	          la        -i+1
c	          SUM{a(i)*z    }
c	          i=1 
c	   Y(z) = ----------------- * X(z)                 (2)
c	              lb        -i
c	          1 - SUM{b(i)*z  }
c	              i=1 
c	   
c	   For time-forward direction (idir=1)
c	       in eq.(1) index k runs from 1 to lx.
c	   For time-reversal direction (idir=-1) 
c	       in eq.(1) index k runs from lx to 1 and 
c	                 index (k-i) is changed to (k+i)
c	       in eq.(2) z-variable is changed to 1/z
c
c	ENTRANCE:
c	   la 		number of coefficients a 
c	   		in transfer function nominator;
c	   a(1:la)	coefficients in transfer function nominator;
c	   lb		number of coefficients b 
c                       in transfer function denominator;
c	   b(1:lb)	coefficients in transfer function denominator;
c	   lx		number of input(output) samples x;
c	   x(1:lx)	input series;
c	   idir		sign of time direction: +1  forward  
c						-1  reversal
c
c	RESULT:
c	   y(1:lx)	output series;
c
c   NOTE!	X and Y can't be COINCIDENT !!!!!
c
c	FORTRAN						VALERY
c
c***********************************************************************
c
	subroutine filrec(la,a,lb,b,lx,x,idir,y) 
	dimension a(1),b(1),x(1),y(1)
	if (idir.eq.-1) goto 100
	y(1)=a(1)*x(1)
	do 50 k=2,lx
	   sum=a(1)*x(k)
	   if (la.eq.1) goto 35
	   if (k.le.la) lat=k
	   do 30 i=2,lat
30	   sum=sum+a(i)*x(k-i+1)
35	   continue
	   if (lb.eq.0) goto 45
	   if (k.le.(lb+1)) lbt=k-1
	   do 40 i=1,lbt
40	   sum=sum+b(i)*y(k-i)
45	   continue
50	y(k)=sum
	return
100	y(lx)=a(1)*x(lx)
	lx1=lx-1
	do 150 k=lx1,1,-1
	   sum=a(1)*x(k)
           if (la.eq.1) goto 135
           if ((lx-k).le.la-1) lat=lx-k+1
           do 130 i=2,lat
130        sum=sum+a(i)*x(k+i-1)
135        continue
           if (lb.eq.0) goto 145
           if ((lx-k).le.lb) lbt=lx-k
           do 140 i=1,lbt
140        sum=sum+b(i)*y(k+i)
145        continue
150     y(k)=sum
        return
	end
E 1
