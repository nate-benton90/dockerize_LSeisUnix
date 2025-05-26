	subroutine inmodp(alpha1,alpha2,beta1,beta2,rho1,rho2,
     : 		  rpa,rpp,rsa,rsp,tpa,tpp,tsa,tsp)
	integer na
	parameter(na=181)
	complex rp(na),rs(na),tp(na),ts(na)
	real    rpa(na),rpp(na),rsa(na),rsp(na)
	real    tpa(na),tpp(na),tsa(na),tsp(na)
	complex a,b,c,d,e,f,g,h,capd,cosi2,cosj1,cosj2,temp
	real    ang,dang,p,rho1,rho2,alpha1,alpha2,beta1,beta2 	
	real    b1sps,b2sps,b2s,b1s,cosi1
	real    pi,zero,one,two,temp1,temp2
	integer i

*	calculate particle displacement refl/trans coefficients
*	for plane p-wave incident on a plane solid/solid interface
*	[ref: Aki & Richards, 1980, vol.1, pp.149,150]
*
*	variables:	
*		na = number of incident angles [0 --> pi]
*		rp = complex p-wave reflection coeff.
*		rpa,rpp = amp and phase, respectively, for refl. p-wave
*		rs = complex s-wave reflection coeff.
*		rsa,rsp = amp and phase, respectively, for refl. s-wave
*		tp = complex p-wave transmission coeff.
*		tpa,tpp = amp and phase, respectively, for trans. p-wave
*		ts = complex s-wave transmission coeff.
*		tsa,tsp = amp and phase, respectively, for trans. s-wave
*		a,b,c,d,e,f,g,h,capd = local variables
*		ang = incident angle in radians
*		dang = incident angle increment [dictated by na]
*		p = ray parameter
*		rho1, rho2 = densities [NB: wave indident from medium 1]
*		alpha1, alpha2 = p-wave speeds 
*		beta1, beta2 = s-wave speeds
*		b1sps = beta1*beta1*p*p
*		b2sps = beta2*beta2*p*p
*		b1s = beta1*beta1
*		b2s = beta2*beta2
*		cosi1 = cosine of incident angle
*		cosi2 = cosine of trans p-wave angle
*		cosj1 = cosine of refl s-wave angle
*		cosj2 = cosine of trans s-wave angle
  

	pi = 3.1415926
	zero = 0.0
	one = 1.0
	two = 2.0

	dang = (pi/two - 1.0e-6)/na

	rpa(181)=1.0
	rsa(181)=1.0
       	tpa(181)=1.0
	tsa(181)=1.0

	do 1 i=0,na-1
		ang = float (i) * dang 
		p = sin(ang)/alpha1

		b1sps = beta1*beta1*p*p
		b2sps = beta2*beta2*p*p
		b1s = beta1*beta1
		b2s = beta2*beta2

		cosi1 = cos(ang)
		temp = cmplx(one-alpha2*alpha2*p*p,zero)
		cosi2 = csqrt(temp)
		temp = cmplx(one-beta1*beta1*p*p,zero)
		cosj1 = csqrt(temp)
		temp = cmplx(one-beta2*beta2*p*p,zero)
		cosj2 = csqrt(temp)

		a = rho2*(one-two*b2sps) - rho1*(one-two*b1sps)
		b = rho2*(one-two*b2sps) + two*rho1*b1sps
		c = rho1*(one-two*b1sps) + two*rho2*b2sps
		d = two*(rho2*b2s-rho1*b1s)

		e = b*cosi1/alpha1 + c*cosi2/alpha2
		f = b*cosj1/beta1 + c*cosj2/beta2
		g = a - d*cosi1*cosj2/(alpha1*beta2)
		h = a - d*cosi2*cosj1/(alpha2*beta1)
		capd = e*f + g*h*p*p

*
*		calc reflected p-wave coefficient amplitude and phase
*
		temp = (b*cosi1/alpha1 - c*cosi2/alpha2) * f
		temp = temp - (a + d*cosi1*cosj2/(alpha1*beta2))*h*p*p
*
*		rp(i) is complex reflection coeff for ith incident angle
*
		rp(i) = temp/capd
		temp1 = real(rp(i))*real(rp(i))
		temp2 = aimag(rp(i))*aimag(rp(i))
*
*		rpa(i) is real amplitude of rp(i)
*
		rpa(i) = sqrt(temp1 + temp2)
		temp1 = real(rp(i))
*
*		if refl coeff is pure imaginary, give it a small real part
*		to avoid numerical singularity of arctangent function
*
		if (temp1.eq.zero) temp1=1.0e-9
*
*		rpp(i) is real phase of rp(i); -pi<= rpp <= pi
*
		rpp(i) = atan2(aimag(rp(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rpa(i) = real(rp(i))
			rpp(i) = 0.0
		endif

*
*		calc's reflected s-wave coefficient amplitude and phase
*
		temp = a*b + c*d*cosi2*cosj2/(alpha2*beta2)
		rs(i) = -two*cosi1*p*temp/(beta1*capd)
		temp1 = real(rs(i))*real(rs(i))
		temp2 = aimag(rs(i))*aimag(rs(i))
		rsa(i) = sqrt(temp1 + temp2)
*
*		keep track of maximum in 181st element
*
		if(rsa(i).gt.rsa(181)) rsa(181)=rsa(i)
		temp1 = real(rs(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rsp(i) = atan2(aimag(rs(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rsa(i) = real(rs(i))
			rsp(i) = 0.0
		endif


*
*		calc transmitted p-wave coefficient amplitude and phase
*
		temp = two*rho1*cosi1*f
		tp(i) = temp/(alpha2*capd)
		temp1 = real(tp(i))*real(tp(i))
		temp2 = aimag(tp(i))*aimag(tp(i))
		tpa(i) = sqrt(temp1 + temp2)
*
*		keep track of maximum in 181st element
*
		if(tpa(i).gt.tpa(181)) tpa(181)=tpa(i)
		temp1 = real(tp(i))
		if (temp1.eq.zero) temp1=1.0e-9
		tpp(i) = atan2(aimag(tp(i)),temp1)


		if ( temp2 .eq. 0.0 ) then 
			tpa(i) = real(tp(i))
			tpp(i) = 0.0
		endif

*
*		calc transmitted s-wave coefficient amplitude and phase
*
		temp = two*rho1*cosi1*h*p
		ts(i) = temp/(beta2*capd)
		temp1 = real(ts(i))*real(ts(i))
		temp2 = aimag(ts(i))*aimag(ts(i))
		tsa(i) = sqrt(temp1 + temp2)
		if(tsa(i).gt.tsa(181)) tsa(181)=tsa(i)
		temp1 = real(ts(i))
		if (temp1.eq.zero) temp1=1.0e-9
		tsp(i) = atan2(aimag(ts(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			tsa(i) = real(ts(i))
			tsp(i) = 0.0
		endif

 1	continue
	return
	end




	subroutine inmodsv(alpha1,alpha2,beta1,beta2,rho1,rho2,
     :			  rpa,rpp,rsa,rsp,tpa,tpp,tsa,tsp)
	integer na
	parameter(na=181)
	complex rp(na),rs(na),tp(na),ts(na)
	real    rpa(na),rpp(na),rsa(na),rsp(na)
	real    tpa(na),tpp(na),tsa(na),tsp(na)
	complex a,b,c,d,e,f,g,h,capd,cosi2,cosi1,cosj2,temp
	real    ang,dang,p,rho1,rho2,alpha1,alpha2,beta1,beta2 	
	real    b1sps,b2sps,b2s,b1s,cosj1
	real    pi,zero,one,two,temp1,temp2
	integer i

*	calculate particle displacement refl/trans coefficients
*	for plane sv-wave incident on a plane solid/solid interface
*	[ref: Aki & Richards, 1980, vol.1, pp.149,150]
*
*	variables:	
*		na = number of incident angles [0 --> pi]
*		rp = complex p-wave reflection coeff.
*		rpa,rpp = amp and phase, respectively, for refl. p-wave
*		rs = complex s-wave reflection coeff.
*		rsa,rsp = amp and phase, respectively, for refl. s-wave
*		tp = complex p-wave transmission coeff.
*		tpa,tpp = amp and phase, respectively, for trans. p-wave
*		ts = complex s-wave transmission coeff.
*		tsa,tsp = amp and phase, respectively, for trans. s-wave
*		a,b,c,d,e,f,g,h,capd = local variables
*		ang = incident angle in radians
*		dang = incident angle increment [dictated by na]
*		p = ray parameter
*		rho1, rho2 = densities [NB: wave indident from medium 1]
*		alpha1, alpha2 = p-wave speeds 
*		beta1, beta2 = s-wave speeds
*		b1sps = beta1*beta1*p*p
*		b2sps = beta2*beta2*p*p
*		b1s = beta1*beta1
*		b2s = beta2*beta2
*		cosi1 = cosine of refl p-wave angle
*		cosi2 = cosine of trans p-wave angle
*		cosj1 = cosine of incident angle
*		cosj2 = cosine of trans s-wave angle
  
	pi = 3.1415926
	zero = 0.0
	one = 1.0
	two = 2.0

	dang = (pi/two - 1.0e-6)/na

	rpa(181)=1.0
	rsa(181)=1.0
       	tpa(181)=1.0
	tsa(181)=1.0

	do 1 i=0,na-1
		ang = float (i) * dang 
		p = sin(ang)/beta1

		b1sps = beta1*beta1*p*p
		b2sps = beta2*beta2*p*p
		b1s = beta1*beta1
		b2s = beta2*beta2

		temp = cmplx(one-alpha1*alpha1*p*p,zero)
		cosi1 = csqrt(temp)
		temp = cmplx(one-alpha2*alpha2*p*p,zero)
		cosi2 = csqrt(temp)
		cosj1 = cos(ang)
		temp = cmplx(one-beta2*beta2*p*p,zero)
		cosj2 = csqrt(temp)

		a = rho2*(one-two*b2sps) - rho1*(one-two*b1sps)
		b = rho2*(one-two*b2sps) + two*rho1*b1sps
		c = rho1*(one-two*b1sps) + two*rho2*b2sps
		d = two*(rho2*b2s-rho1*b1s)

		e = b*cosi1/alpha1 + c*cosi2/alpha2
		f = b*cosj1/beta1 + c*cosj2/beta2
		g = a - d*cosi1*cosj2/(alpha1*beta2)
		h = a - d*cosi2*cosj1/(alpha2*beta1)
		capd = e*f + g*h*p*p

		temp = a*b + c*d*cosi2*cosj2/(alpha2*beta2)
		rp(i) = -two*cosj1*p*temp/(alpha1*capd)
		temp1 = real(rp(i))*real(rp(i))
		temp2 = aimag(rp(i))*aimag(rp(i))
		rpa(i) = sqrt(temp1 + temp2)
		if(rpa(i).gt.rpa(181)) rpa(181)=rpa(i)
		temp1 = real(rp(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rpp(i) = atan2(aimag(rp(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rpa(i) = real(rp(i))
			rpp(i) = 0.0
		endif

		temp = (b*cosj1/beta1 - c*cosj2/beta2)*e
		temp = temp - (a + d*cosi2*cosj1/(alpha2*beta1))*g*p*p
		rs(i) = -temp/capd
		temp1 = real(rs(i))*real(rs(i))
		temp2 = aimag(rs(i))*aimag(rs(i))
		rsa(i) = sqrt(temp1 + temp2)
		if(rsa(i).gt.rsa(181)) rsa(181)=rsa(i)
		temp1 = real(rs(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rsp(i) = atan2(aimag(rs(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rsa(i) = real(rs(i))
			rsp(i) = 0.0
		endif

		temp = -two*rho1*cosj1*g*p
		tp(i) = temp/(alpha2*capd)
		temp1 = real(tp(i))*real(tp(i))
		temp2 = aimag(tp(i))*aimag(tp(i))
		tpa(i) = sqrt(temp1 + temp2)
		if(tpa(i).gt.tpa(181)) tpa(181)=tpa(i)
		temp1 = real(tp(i))
		if (temp1.eq.zero) temp1=1.0e-9
		tpp(i) = atan2(aimag(tp(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			tpa(i) = real(tp(i))
			tpp(i) = 0.0
		endif

		temp = two*rho1*cosj1*e
		ts(i) = temp/(beta2*capd)
		temp1 = real(ts(i))*real(ts(i))
		temp2 = aimag(ts(i))*aimag(ts(i))
		tsa(i) = sqrt(temp1 + temp2)
		if(tsa(i).gt.tsa(181)) tsa(181)=tsa(i)
		temp1 = real(ts(i))
		if (temp1.eq.zero) temp1=1.0e-9
		tsp(i) = atan2(aimag(ts(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			tsa(i) = real(ts(i))
			tsp(i) = 0.0
		endif

 1	continue
	return
	end



	subroutine inmodsh(alpha1,alpha2,beta1,beta2,rho1,rho2,
     :			  rsa,rsp,tsa,tsp)
	integer na
	parameter (na=181)
	complex rs(na),ts(na)
	real    rsa(na),rsp(na)
	real    tsa(na),tsp(na)
	complex capd,cosi2,cosj1,cosj2,temp
	real    ang,dang,p,rho1,rho2,alpha1,alpha2,beta1,beta2 	
	real    pi,zero,one,two,temp1,temp2
	integer i

*	calculate particle displacement refl/trans coefficients
*	for plane sh-wave incident on a plane solid/solid interface
*	[ref: Aki & Richards, 1980, vol.1, p.144]
*
*	variables:	
*		na = number of incident angles [0 --> pi]
*		rs = complex s-wave reflection coeff.
*		rsa,rsp = amp and phase, respectively, for refl. s-wave
*		ts = complex s-wave transmission coeff.
*		tsa,tsp = amp and phase, respectively, for trans. s-wave
*		ang = incident angle in radians
*		dang = incident angle increment [dictated by na]
*		p = ray parameter
*		rho1, rho2 = densities [NB: wave indident from medium 1]
*		alpha1, alpha2 = p-wave speeds 
*		beta1, beta2 = s-wave speeds
*		cosj1 = cosine of incident angle
*		cosj2 = cosine of trans s-wave angle

	pi = 3.1415926
	zero = 0.0
	one = 1.0
	two = 2.0

	dang = (pi/two - 1.0e-6)/na

	rsa(181)=1.0
	tsa(181)=1.0

	do 1 i=0,na-1
		ang = float (i) * dang 
		p = sin(ang)/beta1

		cosj1 = cos(ang)
		temp = cmplx(one-beta2*beta2*p*p,zero)
		cosj2 = csqrt(temp)

		temp = rho1*beta1*cosj1 + rho2*beta2*cosj2
		rs(i) = (rho1*beta1*cosj1 - rho2*beta2*cosj2)/temp
		temp1 = real(rs(i))*real(rs(i))
		temp2 = aimag(rs(i))*aimag(rs(i))
		rsa(i) = sqrt(temp1 + temp2)
		if(rsa(i).gt.rsa(181)) rsa(181)=rsa(i)
		temp1 = real(rs(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rsp(i) = atan2(aimag(rs(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rsa(i) = real(rs(i))
			rsp(i) = 0.0
		endif

		temp2 = two*rho1*beta1*cosj1
		ts(i) = temp2/temp
		temp1 = real(ts(i))*real(ts(i))
		temp2 = aimag(ts(i))*aimag(ts(i))
		tsa(i) = sqrt(temp1 + temp2)
		if(tsa(i).gt.tsa(181)) tsa(181)=tsa(i)
		temp1 = real(ts(i))
		if (temp1.eq.zero) temp1=1.0e-9
		tsp(i) = atan2(aimag(ts(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			tsa(i) = real(ts(i))
			tsp(i) = 0.0
		endif


 1	continue
	return
	end


	subroutine inmodsvfs(alpha,beta,rho,rpa,rpp,rsa,rsp)
	integer na
	parameter (na=181)
	complex rp(na),rs(na)
	real    rpa(na),rpp(na)
	real    rsa(na),rsp(na)
	complex capd,cosi,cosj,temp
	real    ang,dang,p,rho,alpha,beta 	
	real    pi,zero,one,two,temp1,temp2
	integer i

*	calculate particle displacement refl/trans coefficients
*	for plane sv-wave incident on an elastic free surface
*	[ref: Aki & Richards, 1980, vol.1, p.140]
*
*	variables:	
*		na = number of incident angles [0 --> pi/2]
*		rs = complex s-wave reflection coeff.
*		rsa,rsp = amp and phase, respectively, for refl. s-wave
*		rp = complex p-wave reflection coeff.
*		rpa,rpp = amp and phase, respectively, for refl. p-wave
*		ang = incident angle in radians
*		dang = incident angle increment [dictated by na]
*		p = ray parameter
*		rho1, rho2 = densities [NB: wave indident from medium 1]
*		alpha = p-wave speed 
*		beta = s-wave speed
*		cosi = cosine of incident angle / refl. s-wave
*		cosj = cosine of refl. p-wave angle

	pi = 3.1415926
	zero = 0.0
	one = 1.0
	two = 2.0

	dang = (pi/two - 1.0e-6)/na


	do 1 i=0,na-1
		ang = float (i) * dang 
		p = sin(ang)/beta

		cosi = cos(ang)
		temp = cmplx(one-alpha*alpha*p*p,zero)
		cosj = csqrt(temp)
		temp = one/(beta*beta) - two*p*p
		capd = temp*temp + 4.0*p*p*cosi*cosj/(alpha*beta)

		rp(i) = 4.0*p*cosj*temp/(alpha*capd)
		temp1 = real(rp(i))*real(rp(i))
		temp2 = aimag(rp(i))*aimag(rp(i))
		rpa(i) = sqrt(temp1 + temp2)
		temp1 = real(rp(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rpp(i) = atan2(aimag(rp(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rpa(i) = real(rp(i))
			rpp(i) = 0.0
		endif


		rs(i) = temp2/temp
		rs(i) = (temp*temp - 4.0*p*p*cosi*cosj/(alpha*beta))/capd
		temp1 = real(rs(i))*real(rs(i))
		temp2 = aimag(rs(i))*aimag(rs(i))
		rsa(i) = sqrt(temp1 + temp2)
		temp1 = real(rs(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rsp(i) = atan2(aimag(rs(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rsa(i) = real(rs(i))
			rsp(i) = 0.0
		endif


 1	continue
	return
	end


	subroutine inmodpfs(alpha,beta,rho,rpa,rpp,rsa,rsp)
	integer na
	parameter (na=181)
	complex rp(na),rs(na)
	real    rpa(na),rpp(na)
	real    rsa(na),rsp(na)
	complex capd,cosi,cosj,temp
	real    ang,dang,p,rho,alpha,beta 	
	real    pi,zero,one,two,temp1,temp2
	integer i

*	calculate particle displacement refl/trans coefficients
*	for plane p-wave incident on an elastic free surface
*	[ref: Aki & Richards, 1980, vol.1, p.140]
*
*	variables:	
*		na = number of incident angles [0 --> pi/2]
*		rs = complex s-wave reflection coeff.
*		rsa,rsp = amp and phase, respectively, for refl. s-wave
*		rp = complex p-wave reflection coeff.
*		rpa,rpp = amp and phase, respectively, for refl. p-wave
*		ang = incident angle in radians
*		dang = incident angle increment [dictated by na]
*		p = ray parameter
*		rho1, rho2 = densities [NB: wave indident from medium 1]
*		alpha = p-wave speed 
*		beta = s-wave speed
*		cosi = cosine of incident angle / refl. p-wave
*		cosj = cosine of refl. s-wave angle

	pi = 3.1415926
	zero = 0.0
	one = 1.0
	two = 2.0

	dang = (pi/two - 1.0e-6)/na


	do 1 i=0,na-1
		ang = float (i) * dang 
		p = sin(ang)/alpha

		cosi = cos(ang)
		temp = cmplx(one-beta*beta*p*p,zero)
		cosj = csqrt(temp)
		temp = one/(beta*beta) - two*p*p
		capd = temp*temp + 4.0*p*p*cosi*cosj/(alpha*beta)

		rs(i) = 4.0*p*cosi*temp/(beta*capd)
		temp1 = real(rs(i))*real(rs(i))
		temp2 = aimag(rs(i))*aimag(rs(i))
		rsa(i) = sqrt(temp1 + temp2)
		temp1 = real(rs(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rsp(i) = atan2(aimag(rs(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rsa(i) = real(rs(i))
			rsp(i) = 0.0
		endif


		rp(i) = temp2/temp
		rp(i) = (- temp*temp + 4.0*p*p*cosi*cosj/(alpha*beta))/capd
		temp1 = real(rp(i))*real(rp(i))
		temp2 = aimag(rp(i))*aimag(rp(i))
		rpa(i) = sqrt(temp1 + temp2)
		temp1 = real(rp(i))
		if (temp1.eq.zero) temp1=1.0e-9
		rpp(i) = atan2(aimag(rp(i)),temp1)

		if ( temp2 .eq. 0.0 ) then 
			rpa(i) = real(rp(i))
			rpp(i) = 0.0
		endif


 1	continue
	return
	end
