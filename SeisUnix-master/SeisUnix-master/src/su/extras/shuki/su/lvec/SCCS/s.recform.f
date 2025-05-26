h49625
s 00000/00000/00139
d D 1.2 88/11/15 14:04:36 shuki 2 1
c 
e
s 00139/00000/00000
d D 1.1 88/06/13 14:04:15 valer 1 0
c date and time created 88/06/13 14:04:15 by valer
e
u
U
t
T
I 1
c***********************************************************************
c	SUBROUTINE recform(freq,dt,wl,wh,n,npol,poles,ncof,cof)
c
c	DESTINATION:
c		RECursive operator is FORMed in the cascade form:
c	            (one-sided z-transform expression)
c
c                 la        -i+1
c            npol SUM{a(i)*z    }
c             |-| i=1                 
c       H(z)= | | -----------------
c             k=1     lb        -i
c                 1 - SUM{b(i)*z  }
c                     i=1
c
c	   for n-order filter described by npol poles,
c	   i.e. npol cascades
c	
c	ENTRANCE:
c          freq(1:4)    frequencies (Hz) of band-pass range
c                          fl0<=fl<=fh<=fh0;
c                          for low-pass filter:  fl0,fl<=0.0;
c                          for high-pass filter: fh,fh0>=1/(2dt);
c          dt           time-sampling interval (sec);
c          wl           corrected low frequency*dt (rad);
c			   (result of subroutine like precheb)
c          wh           corrected high frequency*dt (rad).
c			   (result of subroutine like precheb)
c          n            order of filter;
c			   (result of subroutine like precheb)
c          npol         number of pairs of conjugate poles
c                          + one (real pole) in case of n-odd;
c			   (result of subroutine like polcheb)
c          poles(1:2,npol)      real SIGM and image OMEG coordinate
c                                 parts of npol poles;
c			   (result of subroutine like polcheb)
c
c	RESULT:
c	   ncof(1:2,   	   numbers la of coefficients a in nominator
c	        1:npol)       and lb of coefficients b in denominator
c			      of transfer function H(z) for each cascade;
c			      for low-pass and high-pass filter:     
c			      conj.poles la=3,lb=2; real pole la=2,lb=1;
c			      for band-pass filter:
c			      conj.poles la=5,lb=4; real pole la=3,lb=2;
c	   cof(1:5,1:2,    coefficients a(i) in nominator cof(i,1,*)
c	        1:npol)       and b(i) in denominator cof(i,2,*)
c			      of transfer function H(z) for each cascade.
c
c	FORTRAN					VALERY
c
c**********************************************************************
c

	subroutine recform(freq,dt,wl,wh,n,npol,poles,ncof,cof)
	dimension freq(1),poles(2,1)
	dimension ncof(2,1),cof(5,2,1)
	data epsf / 0.01 /
        fnyq = 0.5/dt
	if (mod(n,2).eq.0) goto 100
c					n-ODD - REAL POLE
        if (freq(2).gt.epsf) goto 10
c                                       LOW-PASS filter
	b0 = 2.0-wh*poles(1,npol)
	cof(1,1,npol) = wh/b0
	cof(2,1,npol) = cof(1,1,npol)
	ncof(1,npol) = 2
	cof(1,2,npol) = (2.0+wh*poles(1,npol))/b0
	ncof(2,npol) = 1
	goto 99
10	if (freq(3).lt.(fnyq-epsf)) goto 20
c                                       HIGH-PASS filter
	b0 = 0.5*wl-poles(1,npol)
	cof(1,1,npol) = 1.0/b0
	cof(2,1,npol) = -cof(1,1,npol)
        ncof(1,npol) = 2 
	cof(1,2,npol) = -(0.5*wl+poles(1,npol))/b0
        ncof(2,npol) = 1 
	goto 99
20	continue
c					BAND-PASS filter
	wm = sqrt(wl*wh)
	rw = wh-wl
	b0 = wm+4.0-2.0*rw*poles(1,npol)
	cof(1,1,npol) = 2.0*rw/b0
	cof(2,1,npol) = 0.0
	cof(3,1,npol) = -cof(1,1,npol)
	ncof(1,npol) = 3
	cof(1,2,npol) = (8.0-2.0*wm)/b0
	cof(2,2,npol) = -(wm+4.0+2.0*rw*poles(1,npol))/b0
	ncof(2,npol) = 2
99	if (npol.eq.1) return
100	npair = int(0.5*n)
	do 199 k=1,npair
c					PAIRS of CONJUG.POLES
	sig = poles(1,k)
	am2 = sig*sig+poles(2,k)*poles(2,k)
        if (freq(2).gt.epsf) goto 110
c                                       LOW-PASS filter
	b0 = 4.0-4.0*wh*sig+wh*wh*am2
	cof(1,1,k) = wh*wh/b0
	cof(2,1,k) = 2.0*cof(1,1,k)
	cof(3,1,k) = cof(1,1,k)
	ncof(1,k) = 3
	cof(1,2,k) = (8.0-2.0*wh*wh*am2)/b0
	cof(2,2,k) = -(4.0+4.0*wh*sig+wh*wh*am2)/b0
	ncof(2,k) = 2
	goto 199
110	if (freq(3).lt.(fnyq-epsf)) goto 120
c                                       HIGH-PASS filter
	b0 = wl*wl-2.0*wl*sig+4.0*am2
	cof(1,1,k) = 4.0/b0
	cof(2,1,k) = -2.0*cof(1,1,k)
	cof(3,1,k) = cof(1,1,k)
	ncof(1,k) = 3
	cof(1,2,k) = (8.0*am2-2.0*wl*wl)/b0
	cof(2,2,k) = -(wl*wl+2.0*wl*sig+4.0*am2)/b0
	ncof(2,k) = 2
	goto 199
120	continue
c					BAND-PASS filter
	wmp = sqrt(wl*wh)+4.0
	wmm = wmp-8.0
	rw = wh-wl
	b0 = wmp*wmp-4.0*rw*wmp*sig+4.0*rw*rw
	cof(1,1,k) = 4.0*rw*rw/b0
	cof(2,1,k) = 0.0
	cof(3,1,k) = -2.0*cof(1,1,k)
	cof(4,1,k) = 0.0
	cof(5,1,k) = cof(1,1,k)
	ncof(1,k) = 5
	cof(1,2,k) = (8.0*rw*wmm*sig-4.0*wmp*wmm)/b0
	cof(2,2,k) = -(4.0*wmm*wmm+2.0*wmp*wmp-8.0*rw*rw*am2)/b0
	cof(3,2,k) = -(4.0*wmm*wmp+8.0*rw*wmm*sig)/b0
	cof(4,2,k) = -(wmp*wmp+4.0*rw*wmp*sig+4.0*rw*rw*am2)/b0
	ncof(2,k) = 4
199	continue
	return
	end
E 1
