h11627
s 00000/00000/00085
d D 1.4 88/11/15 14:04:22 shuki 4 3
c 
e
s 00003/00003/00082
d D 1.3 88/05/24 13:13:32 valer 3 2
c spec(0:(nf-1))
e
s 00002/00002/00083
d D 1.2 88/05/24 05:28:20 shuki 2 1
c spec(0)
e
s 00085/00000/00000
d D 1.1 88/05/18 16:25:36 valer 1 0
c date and time created 88/05/18 16:25:36 by valer
e
u
U
t
T
I 1
	subroutine winhan(f,df,nf,spec)
c
c************************************************************************
c	DESTINATION:
c	   generating of amplitude spectrum in form
c	   of HANNING window:
c					        freq-f(1)
c			     { 0.5*(1 - cos(3.14---------) ,
c					        f(2)-f(1)
c				  for[f(1) < freq < f(2)]
c
c		spec(freq) = { 1 ,for[f(2) < freq < f(3)]
c
c					        freq-f(3)
c			     { 0.5*(1 + cos(3.14---------) ,
c					        f(4)-f(3)
c				  for[f(3) < freq < f(4)]
c
c			     { 0 ,for[freq < f(1)],[f(4) < freq]
c
c	ENTRANCE:
c	   f(1),f(2),f(3),f(4)	frequencies (Hz) set HANNING window;
c	   df			frequency sampling interval (Hz)
c				of spectrum;
c	   nf			number of frequency samples
c				in spectrum;
c
c	RESULT:
D 3
c	   spec			amplitude spectrum (length nf)
E 3
I 3
c	   spec(0:(nf-1))		amplitude spectrum (length nf)
E 3
c
c	FORTRAN						VALERY
c
c************************************************************************
D 2
	 dimension f(1),spec(0)
E 2
I 2
D 3
	 dimension f(1),spec(1)
E 3
I 3
	 dimension f(1),spec(0:(nf-1))
E 3
E 2
c
D 2
c			NOTE!	SPEC(0)
E 2
I 2
D 3
c			NOTE!	SPEC(0) corrected to spec(1) to allow compilation - shuki
E 3
I 3
c			NOTE! SPEC(0)
E 3
E 2
c
        DATA PI / 3.1415926 /
	kf=nf - 1
	kl0=f(1)/df + 0.5
	if (kl0.gt.kf) kl0=kf
c
c			FREQ < F(1)
	if (kl0.gt.0) then
		do 2 i=0,kl0
2		spec(i)=0.0
	endif
	kl=f(2)/df + 0.5
	if (kl.gt.kf) kl=kf
c
c			F(1) < FREQ < F(2)
	if (kl0.lt.kl) then
		r21=pi/(f(2)-f(1))
		fleft=f(1)
		do 4 i=kl0,kl
		ff=i*df - fleft
4		spec(i)=0.5 - 0.5*cos(r21*ff)
	endif
	kh=f(3)/df + 0.5
	if (kh.gt.kf) kh=kf
c
c			F(2) < FREQ < F(3)
	if (kl.lt.kh) then
		do 6 i=kl,kh
6		spec(i)=1.0
	endif
	kh0=f(4)/df + 0.5
	if (kh0.gt.kf) kh0=kf
c
c			F(3) < FREQ < F(4)
	if (kh.lt.kh0) then
		r43=pi/(f(4)-f(3))
		fleft=f(3)
		do 8 i=kh,kh0
		ff=i*df - fleft
8		spec(i)=0.5 + 0.5*cos(r43*ff)
	endif
c
c			F(4) < FREQ
	if (kh0.lt.kf) then
		do 10 i=kh0,kf
10		spec(i)=0.0
	endif
	return
	end
E 1
