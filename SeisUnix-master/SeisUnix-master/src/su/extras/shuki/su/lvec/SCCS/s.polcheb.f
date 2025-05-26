h08617
s 00000/00000/00064
d D 1.3 88/11/15 14:04:33 shuki 3 2
c 
e
s 00006/00006/00058
d D 1.2 88/06/20 16:49:04 valer 2 1
c 
e
s 00064/00000/00000
d D 1.1 88/06/13 14:04:12 valer 1 0
c date and time created 88/06/13 14:04:12 by valer
e
u
U
t
T
I 1
c**********************************************************************
D 2
c       subroutine polcheb(lev,n,npol,poles)
E 2
I 2
c       subroutine polcheb(pas,n,npol,poles)
E 2
c
c       DESTINATION:
c		COPMUTATION of POLES LOCATION
c                        for Chebyshev recursive filter
c
c          For Chebyshev first-type n-order normalized filter
c
c                   2        1
c            |H (w)| = ---------------
c              n                 2      2
D 2
c                      1 + (1/lev -1)* T (w)
E 2
I 2
c                      1 + (1/pas -1)* T (w)
E 2
c                                       n
c
c	         coordinates  S  = SIGM  + j*OMEG  (k=1,...,n/2)
c	                       k       k         k
c	   and S     = SIGM         for n-odd case
c	        n/2+1      n/2+1
c	   of npol poles are calculated.
c	   
c       ENTRANCE:
D 2
c          lev          level at the passband edge of filter; 
E 2
I 2
c          pas          level at the passband edge of filter; 
E 2
c          n            order of filter;
c          npol         number of pairs of conjugate poles
c                          + one (real pole) in case of n-odd;
c
c       RESULT:
c	   n=0			in case of disorder;
c	   poles(1:2,npol)	real SIGM and image OMEG coordinate
c				  parts of npol poles.
c
c       FORTRAN                                         VALERY
c
c*********************************************************************
c
D 2
	subroutine polcheb(lev,n,npol,poles)
E 2
I 2
	subroutine polcheb(pas,n,npol,poles)
E 2
	dimension poles(2,1)
	data pi / 3.1415926 /
	if (del.gt.1.0) then
		n=0
		return
	endif
	if (n.eq.0) return
D 2
	r = (1.0+lev)/(1.0-lev)
	rn = 2.0/n
E 2
I 2
	r = (1.0+pas)/(1.0-pas)
	rn = 0.5/n
E 2
	g = r**rn
	r = 1.0/g
	sh = (r-g)*0.5
	ch = (g+r)*0.5
	if (mod(n,2).eq.1) then
		poles(1,npol) = sh
		poles(2,npol) = 0.0
		if (npol.eq.1) return
	endif
	npair = int(0.5*n)
	r = 0.5*pi/n
	do 20 k=1,npair
		arg = (2*k-1)*r
		poles(1,k) = sh*sin(arg)
		poles(2,k) = ch*cos(arg)
20	continue
	return
	end
E 1
