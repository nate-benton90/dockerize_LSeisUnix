c**********************************************************************
c	subroutine precheb(freq,dt,pas,del,n,npol,wl,wh)
c
c	DESTINATION:
c		FREQuency CORRECTION and ORDER n EVALUATION
c			 for Chebyshev recursive filter
c
c	   For Chebyshev first-type n-order filter 
c	   in the shape
c	             
c	      |H(w)|
c	       ^
c	       |
c	      1-         *     *
c	       |        * *   * *
c          pas --------*---*-*---*
c	       |      *|         |*
c	       |     * |         | *
c	       |    *  |         |  *
c	       |   *   |         |   *
c          del - -*----|---------|-----*
c	       | *|    |         |     |  *
c              |--|----|---------|-----|----> w
c	       0  fl0  fl        fh    fh0
c
c	   order n is evaluated to satisfy the requirements of
c	    a)level pas at the passband edges fl,fh; 
c	    b)level del at the stopband edges fl0,fh0
c			(fl0,fh0 values may be corrected) .
c	!!    In case of n prescribed by user computation of
c		wl,wh,npol parameters is still necessary.
c
c	ENTRANCE:
c	   freq(1:4)	frequencies (Hz) of band-pass range
c			   fl0<=fl<=fh<=fh0;
c			   for low-pass filter:	 fl0,fl<=0.0;	
c			   for high-pass filter: fh,fh0>=1/(2dt);
c	   dt		time-sampling interval (sec);
c	   pas  	level at the passband edges fl and fh;
c	   del		level at the stopband edges fl0 and fh0;
c			   in case of n prescribed, del has no influence;
c	   n		prescribed order of filter;
c			   in unprescribed case to set n = 0;
c
c	RESULT:
c	   n		order of filter;
c			   in case of disorder n=0;
c	   npol		number of pairs of conjugate poles
c			   + one (real pole) in case of n-odd;
c	   wl		corrected low frequency*dt (rad);
c	   wh		corrected high frequency*dt (rad).
c
c	FORTRAN						VALERY
c
c*********************************************************************
c
	subroutine precheb(freq,dt,pas,del,n,npol,wl,wh)
	dimension freq(1)
	data pi / 3.1415926 /
	data epsf / 0.01 /
	fnyq = 0.5/dt
	if (freq(2).lt.epsf) then
c					LOW-PASS filter
		wl = 0.0
		if (freq(3).gt.(fnyq-epsf)) then
			n=0
			return
		end if
		rab = pi*freq(3)*dt
		wh = 2.0*tan(rab)
		if (freq(4).gt.(fnyq-epsf)) then
			wb0=10.0
		else
			rab = pi*freq(4)*dt
			wb0 = 2.0*tan(rab)/wh
		endif
		goto 20
	end if
	if (freq(3).gt.(fnyq-epsf)) then
c					HIGH-PASS filter
		wh = 1.0/epsf
		rab = pi*freq(2)*dt
		wl = 2.0*tan(rab)
		if (freq(1).lt.epsf) then
			wb0=10.0
		else
			rab = pi*freq(1)*dt
			wb0 = 0.5*wl/tan(rab)
		endif
		goto 20
	end if
c					BAND-PASS filter
	rab = pi*freq(2)*dt
        wl = 2.0*tan(rab)
	rab = pi*freq(3)*dt
        wh = 2.0*tan(rab)
	r1 = freq(1)/freq(2)
	r2 = freq(3)/freq(4)
	if (r1.gt.r2) r2 = r1
	fh0 = freq(3)/r2
	rab = pi*fh0*dt
	wh0 = 2.0*tan(rab)
	wb0 = (wh0*wh0-wl*wh)/(wh-wl)/wh0
c
20	if (n.gt.0) goto 40
c					n-EVALUATION
	rab = wb0+sqrt(wb0*wb0-1.0)
	rden = alog10(rab)
	if ((del.gt.pas).or.(pas.gt.1.0)) then
		n = 0
		return
	endif
	r1 = 1.0/del/del-1.0
	r2 = 1.0/pas/pas-1.0
	g = sqrt(r1/r2)
	rab = g+sqrt(g*g-1.0)
	rnom = alog10(rab)
	n = rnom/rden+0.5
	if (n.eq.0) n=1
40	npol = int(0.5*n)+mod(n,2)
	return
	end
