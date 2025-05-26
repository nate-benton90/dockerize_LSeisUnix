c**********************************************************************
c       subroutine polcheb(pas,n,npol,poles)
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
c                      1 + (1/pas -1)* T (w)
c                                       n
c
c	         coordinates  S  = SIGM  + j*OMEG  (k=1,...,n/2)
c	                       k       k         k
c	   and S     = SIGM         for n-odd case
c	        n/2+1      n/2+1
c	   of npol poles are calculated.
c	   
c       ENTRANCE:
c          pas          level at the passband edge of filter; 
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
	subroutine polcheb(pas,n,npol,poles)
	dimension poles(2,1)
	data pi / 3.1415926 /
	if (del.gt.1.0) then
		n=0
		return
	endif
	if (n.eq.0) return
	r = (1.0+pas)/(1.0-pas)
	rn = 0.5/n
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
