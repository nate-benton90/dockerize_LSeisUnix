h26334
s 00001/00001/00059
d D 1.2 88/07/24 09:10:28 valer 2 1
c 
e
s 00060/00000/00000
d D 1.1 88/07/21 13:34:53 valer 1 0
c date and time created 88/07/21 13:34:53 by valer
e
u
U
t
T
I 1
************************************************************************
c	subroutine burg(lx,x,f,b,la,a)
c
c	DESTINATION:
c		COMPUTATION of Prediction Filter
c		(BURG's Algorithm of Maximum Entropy Method).
c
c	REFERENCE:
c		ENDERS A. ROBINSON. Multichannel Time Series Analysis, 
c			Second Edition 1983,Goose Pond Press.
c
c	ENTRANCE:
c	   lx		length of input time series;
c	   x(1:lx)	input time series;
c	   la		length of prediction filter;
c	   f(1:lx)	working array;
c	   b(1:lx)	working array;
c
c	RESULT:
D 2
c	   a(1:lx)	prediction (inverse) filter;
E 2
I 2
c	   a(1:la)	prediction (inverse) filter;
E 2
c
c	FORTRAN					VALERY
c
c**********************************************************************
c
	subroutine burg(lx,x,f,b,la,a)
	dimension x(1),f(1),b(1),a(1)
	v=0.0
	do 1 i=1,lx
	v=v+x(i)*x(i)
	f(i)=x(i)
1	b(i)=x(i)
	v=v/float(lx)
	do 5 n=1,la
	sn=0.0
	sd=0.0
	l=n+1
	do 2 j=l,lx
	sn=sn+f(j)*b(j-1)
2	sd=sd+f(j)*f(j)+b(j-1)*b(j-1)
	g=2.0*sn/sd
	v=v*(1.0-g*g)
	a(n)=g
	if(n.eq.1)go to 4
	nh=n/2
	do 3 k=1,nh
	kk=n-k
	aa=a(kk)-g*a(k)
	a(k)=a(k)-g*a(kk)
3	a(kk)=aa
4	m1=n+1
	bs=b(n)
	do 5 k=m1,lx
	ff=f(k)
	bb=bs
	bs=b(k)
	f(k)=ff-g*bb
5	b(k)=bb-g*ff
	return
	end
E 1
