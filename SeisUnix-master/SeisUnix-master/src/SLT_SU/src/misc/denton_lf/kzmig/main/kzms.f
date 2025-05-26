ccccccc
c  Kirchhoff prestack/poststack 2d/3d depth migration subroutine
c  author:  Zhenyue liu	and Z. Li	      	8/94
c	input:
c		trace(nt)		input trace to be migrated
c		nt			number of samples per trace
c		t0			time of first sample in s  
c		dt			time sampling interval in s
c		sx			source x location of trace		
c		sy			source y location of trace		
c		gx			receiver x location of trace		
c		gy			receiver y location of trace		
c          	imute			sample index of mute time of the trace
c		ts(nz,nxy) 		residual traveltime table at source
c		tg(nz,nxy) 		residual traveltime table at receiver
c		tb(nz,nr) 		referance traveltime table  
c		pb(nz,nr) 		reference slope table 
c		mig(nzo,nxy)		migration output
c		nxy			number of locations to output
c		nz 			number of depth samples per trace 
c						in table
c		nr 			number of radial samples in reference 
c               dr                      radial sampling interval in reference 
c               fz                      first depth sample in reference 
c		nzo			number of depth samples per trace 
c						to output
c               dzo                     output depth sampling interval   
c               fzo                     first output depth sample  
c		x(nxy)			x locations of migration output
c		y(nxy)			y locations of migration output
c		aperx			migration aperature in x 
c		apery			migration aperature in y
c		fold(nz,nxy)		fold of stack at output traces
c		f0			first frequency of highcut filters
c		df			frequency interval of highcut filters
c		nf			number of highcut filters
c		ftaper			tapering lenght of highcut filters
c		ifcut(nf)		highcut indice 				
c		ltaper(nf)		tapering length in samples 	
c		tracef(nt,nf)		work buffer
c		wsave(nsave)		fft coefficients
c		nsave			length of wsave (at least 2*nfft+15)
c		tit(nz+1,nxy)		work array
c		wt(nz+1,nxy)		work array
c		ampt(nz+1,nxy)		work array
c		sqrtf(nfft)		work array
c		work1(nfft,nf)		work array
c		work2(nfft)		work array
c		tahd			taper and 2d/3d fileer 
c					>0 2d; oherwise 3d
c		nfft			fft length of trace
c               kxmax                   maximum wavenumber in line (0.5/dx)
c               kymax                   maximum wavenumber cross line (0.5/dy)
c               angmax                  maximum angle (from vertical) of 
c					migration in degrees 
c	output:
c		mig(nzo,nxy)		migration output
c		fold(nz,nxy)		fold of stack at output traces
c		ifcut(nf)		highcut indice (computed at first call) 
c		ltaper(nf)		tapering length in samples (computed
c						at first call) 
c		wsave(nsave)		fft coefficients (computed at first
c						call)
c
	subroutine pszm3d(trace,nt,t0,dt,sx,sy,gx,gy,imute,
     2				mig,x,y,nxy,nzo,
     3				dzo,fzo,aperx,apery,fold,
     4				f0,df,nf,ftaper,
     5				ifcut,ltaper,tracef,
     6				wsave,nsave,work1,work2,sqrtf,tahd,nfft,
     7                          kxmax,kymax,angmax,
     8				ts,tg,tit,wt,ampt,nz,dz,fz,tb,pb,nr,dr)
	real trace(nt),tracef(nt,nf),t0,dt,sx,sy,gx,gy
	real x(nxy),y(nxy),fold(nz,nxy)
	integer imute, nxy
	real mig(nzo,nxy)
 	real ts(nz,nxy),tg(nz,nxy),tb(nz,nr),pb(nz,nr),
     1		tit(nz+1,nxy),wt(nz+1,nxy),ampt(nz+1,nxy)
	real diss,disg,ar,srs,srs0,srg,srg0,dr2,zi,psd,pgd,tsd,tgd,px,py
	integer	jr,izi,jrs,jrg
	real amp
	real aperx,apery
	integer nt,nz
	real kxmax,kymax,angmax,dz,z0,f0,df,ftaper
	real kxmaxo,kymaxo,f0o
	integer nf,tahd
	real wsave(nsave),work1(nfft,nf),work2(nfft),sqrtf(nfft)
	integer ifcut(nf),ltaper(nf)
	real cosas, sinas, cosag, sinag  
	integer iw, iti, iz, iz0, iang, i1, i 
	real s1, s2
	real omsz
	
 	odt = 1./dt
	odzo = 1./dzo
	odz = 1.0/dz
	rdz = dzo*odz
	zi0 = 1+(fzo-fz)*odz-rdz

	do itr=1,nxy
		tit(nz+1,itr) = 0.
		wt(nz+1,itr) = 1.
		ampt(nz+1,itr) = 0.
	end do
	shift = 1.5 - t0/dt
  
ccc midpoint locations
	xm = 0.5*(sx+gx)
	ym = 0.5*(sy+gy)

cccc compute some constants 
 
	if(df.ne.0.) then
		odf = 1./df
	else 
		odf = 0.
	end if

cccc design and apply highcut filters

	if(abs(f0-0.5/dt).le.0.001 .and. nf.eq.1) then
		do it=1,nt
			tracef(it,1) = trace(it)
		end do
	else
		call fhigcut(trace,nt,f0,df,nf,ftaper,dt,
     1			ifcut,ltaper,tracef,sqrtf,tahd,
     2			wsave,nsave,work1,work2,nfft)
	end if


	aperxx = aperx + 0.00001
	aperyy = apery + 0.00001
	epsx = kxmax*0.001
	epsy = kymax*0.001

	kxmaxo = kxmax/df
	kymaxo = kymax/df
	f0o = 1 - f0/df

	do iw=1,nf
		do it=1,imute
			tracef(it,iw) = 0.
			work1(it,iw) = 0.
		end do
		do it=imute+1,nt-1
			work1(it,iw) = tracef(it+1,iw) - tracef(it,iw)	
		end do
		tracef(nt,iw) = 0.
		work1(nt,iw) = 0.
	end do

  
	if(angmax.gt.89.9) then
		otan2 = 0.
	else if (angmax.lt.0.1) then
		otan2 = 999999.
	else
		otan2 = 2./tan(angmax*3.141592654/180.)
	end if

cccc migration over output traces
cccc
c$dir force_parallel
cc$doacross 
	do itr=1,nxy
 	   	yi = y(itr)
 	        xi = x(itr)

	   	if(abs(ym-yi).le.aperyy .and.
     1 	           abs(xm-xi).le.aperxx) then
 
ccccc             angle aperature calculation 
		    dr2 = (xi-xm)*(xi-xm)+(yi-ym)*(yi-ym)
		    tmp = sqrt(dr2)*otan2
	            tmp1 = (tmp - fzo)*odzo + 1. 
		    iang = tmp1
		    izo0 = max0(1,iang)
 	            tmp1 = (tmp - fz)*odz   
		    iang = tmp1
		    iz0 = max0(1,iang)

cccc		  azimuth angles calculation
		    diss = sqrt((xi-sx)*(xi-sx)+(yi-sy)*(yi-sy))
 		    cosas = (xi-sx)/(diss+0.01*dr)
		    sinas = (yi-sy)/(diss+0.01*dr)
 		    disg = sqrt((xi-gx)*(xi-gx)+(yi-gy)*(yi-gy))
 		    cosag = (xi-gx)/(disg+0.01*dr)
		    sinag = (yi-gy)/(disg+0.01*dr)
 
cccc		some constants for reference quantities
		    ar = diss/dr
		    jr = ar
		    jr = min0(jr,nr-2)
		    srs = ar-jr
		    srs0 = 1.0-srs
		    jrs = jr+1
 		    ar = disg/dr
		    jr = ar
		    jr = min0(jr,nr-2)
		    srg = ar-jr
		    srg0 = 1.0-srg
		    jrg = 1+jr
		
	write(9,*) "iz0=",iz0,"itr=",itr,"imute=",imute
		    do iz = iz0,nz
       			tsd = srs0*tb(iz,jrs)+srs*tb(iz,jrs+1)
  		        tgd = srg0*tb(iz,jrg)+srg*tb(iz,jrg+1)
 		        tit(iz,itr) = (ts(iz,itr)+tg(iz,itr)
     1				+tsd+tgd)*odt+shift 
 			ampt(iz,itr) = 0.5*(tsd+tgd+2.0*dt)
     1				/((tsd+dt)*(tgd+dt))

		write(9,*) "tit=",tit(iz,itr),"itr=",itr,"imute=",imute
ccccc 			if(tit(iz,itr) .le. 2*nt ) then
 			if(tit(iz,itr).le.nt
     1                     .and.tit(iz,itr).gt.imute) then
 		    	    fold(iz,itr) = fold(iz,itr)+1.0
  			end if

  		    end do 
  	 
		    if(nf.gt.1) then
 		       do iz = iz0,nz
			  psd = srs0*pb(iz,jrs)+srs*pb(iz,jrs+1)
			  pgd = srg0*pb(iz,jrg)+srg*pb(iz,jrg+1)
 			  px = psd*cosas+pgd*cosag
			  py = psd*sinas+pgd*sinag
 			  slopex = abs(px)+epsx
			  slopey = abs(py)+epsy
			  fxmax = kxmaxo/slopex+f0o
 			  fymax = kymaxo/slopey+f0o
 			  wt(iz,itr) = min(fxmax,fymax)
		          wt(iz,itr) = min(wt(iz,itr),float(nf))		
			  wt(iz,itr) = max(wt(iz,itr),1.0)
		       end do
 
 		       do izo = izo0,nzo
			  zi = zi0+izo*rdz 
			  izi = zi
			  sz = zi-izi
			  omsz = 1. - sz
 		          ti = omsz*tit(izi,itr)
     1                         +sz*tit(izi+1,itr)
			  iw =  omsz*wt(izi,itr)
     1                         +sz*wt(izi+1,itr) 
 			  amp = omsz*ampt(izi,itr)
     1                         +sz*ampt(izi+1,itr)
			  iti = ti  
			  res = ti - iti
			  iti = max0(iti,imute)
			  iti = min0(iti,nt)
cccc			  if(iti.gt.imute .and. iti.lt.nt) then
 			     mig(izo,itr) = mig(izo,itr)
     1				    	+ ( tracef(iti,iw) + res* 
     2				            work1(iti,iw) )*amp
cccc			  end if
 
 		       end do
		    else
 		       do izo = izo0,nzo
			  zi = zi0+izo*rdz 
			  izi = zi
 			  sz = zi-izi
			  omsz = 1.0 - sz	
 		          ti = omsz*tit(izi,itr)
     1                          +sz*tit(izi+1,itr)
 			  amp = omsz*ampt(izi,itr)
     1                          +sz*ampt(izi+1,itr) 
			  iti = ti
	  		  res = ti - iti
			  iti = max0(iti,imute)
			  iti = min0(iti,nt)
cccc			  if(iti.gt.imute .and. iti.lt.nt) then
			     mig(izo,itr) = mig(izo,itr)
     1				    	+ ( tracef(iti,1) + res* 
     2				            work1(iti,1) )*amp
cccc			  end if
		       end do
		    end if
		 end if
	end do

	return
	end

	
	subroutine fhigcut(trace,nt,f0,df,nf,ftaper,dt,
     1		ifcut,ltaper,tras,sqrtf,tahd,
     2		wsave,nsave,work1,work2,nfft)
	real trace(nt),tras(nt,nf),wsave(nsave),work1(nfft)
	real work2(nfft),sqrtf(nfft)
	integer ifcut(nf),ltaper(nf),nf,nt
	real ftaper,f0,df,dt,tr,ti
	integer ifirst
	real sfft,scale

	data ifirst/1/
	if(ifirst.eq.1) then
		ifirst = 999
		if(2*nfft+15.gt.nsave) then
			call msgsc(" rffti error; check nfft ")
			stop 
		end if
		call rffti(nfft,wsave)
		sfft = 1./nfft
		delf = 1./(nfft*dt)

		if(ftaper .gt. f0) ftaper = f0
		tmp = ftaper/delf + 1.
		itaper = tmp
		if(itaper.gt.nfft/2) itaper=nfft/2

 	   	if(tahd .gt. 0) then
			cos45 = 0.5*sqrt(2.)
			scale = 2./nfft
			do k=1,nfft
				sqrtf(k) = sqrt( (k-1)*scale)*cos45/nfft
			end do
		else
			scale = 2./(nfft*nfft)
		end if

		
		
		do iw=1,nf
			f = f0 + (iw-1)*df
			tmp = f/delf + 1.	
			ii = tmp
			if(ii.gt.nfft/2) ii=nfft/2
			ifcut(iw) = ii
			ltaper(iw) = itaper
		end do
	end if

   	if(tahd .gt. 0) then
		scale = 2./nfft
	else
		scale = 2./(nfft*nfft)
	end if
	sfft = 1./nfft

	do it=1,nfft
		work1(it) = 0.
	end do
	do it=1,nt
		work1(it) = trace(it)
	end do
 	call rfftf(nfft,work1,wsave)
 
	work1(1) = 0. 
	if (tahd .gt. 0) then
c 	apply the sqrt(-iw) filter
		do k=2,nfft/2
	   		ir = 2 * k - 2  
           		ii = 2 * k - 1
	   		tr = work1(ir)*sqrtf(k)
	   		ti = work1(ii)*sqrtf(k)
 	   		work1(ir) = tr+ti 	
 	   		work1(ii) = ti-tr 	
		end do 
	else  
c 	apply the -iw filter
 		do k=2,nfft/2
	   		ir = 2 * k - 2  
          		ii = 2 * k - 1
	   		tr = work1(ir)*(k-1)*scale
	   		ti = work1(ii)*(k-1)*scale
 	   		work1(ir) = ti 	
 	   		work1(ii) = -tr 	
		end do 
	end if 
	work1(nfft) = 0.

	do iw=1,nf
 		n = 2*ltaper(iw)
		l = 2*ifcut(iw)-n
		do k=1,l-1
			work2(k) = work1(k)*sfft
		end do
		do k=l,nfft
			work2(k) = 0.
		end do	
		st = sfft/(n*n*n)
		ifc = ifcut(iw)-ltaper(iw)
		do k=1,n-1
			ir = 2*(ifc+k)-2 
			ii = ir + 1
			taper = (n-k)*(n-k)*(n+2*k)*st
			work2(ir) = work1(ir)*taper
			work2(ii) = work1(ii)*taper
		end do
		call rfftb(nfft,work2,wsave)
		do it=1,nt
			tras(it,iw) = work2(it)
		end do
	end do
	return
	end
	

