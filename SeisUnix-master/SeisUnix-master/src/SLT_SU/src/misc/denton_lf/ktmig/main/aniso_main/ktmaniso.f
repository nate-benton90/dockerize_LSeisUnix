ccccccc
c  Kirchhoff prestack/poststack 3d/2d migration subroutine
c     in the anisotropic media with eta parameter
c 
c	author:		zhiming li	      		3/93
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
c		fovt2(ntm)		(2/(vrms*t0))**2
c		mig(ntm,nxy)	migration output
c		nxy			number of locations to output
c		ntm			number of time samples per trace 
c						to output
c		x(nxy)		x locations of migration output
c		y(nxy)		y locations of migration output
c		tm(ntm)			tau/dt/2
c		aperx			migration aperature in x 
c		apery			migration aperature in y
c		fold(nxy)	fold of stack at output traces
c		w1(ntm)			work array
c		f0			first frequency of highcut filters
c		df			frequency interval of highcut filters
c		nf			number of highcut filters
c		ftaper			tapering lenght of highcut filters
c		ifcut(nf)		highcut indice 			
c		ltaper(nf)		tapering length in samples 	
c		tracef(nt,nf)	work buffer
c		wsave(nsave)	fft coefficients
c		nsave			length of wsave (at least 2*nfft+15)
c		work1(nfft,nf)	work array
c		work2(nfft,nxy)	work array
c		nfft			fft length of trace
c       kxmax           maximum wavenumber in line (0.5/dx)
c       kymax           maximum wavenumber cross line (0.5/dy)
c       dtau            output time sampling interval in s 
c       tau0            first output time in s 
c       angmax          maximum angle (from vertical) 
c	  					  of migration in degrees 
c       s2(ntm)         scale to be applied to correct for
c                         ray bending 
c       scs(ntm,nxy)     working array 
c       scg(ntm,nxy)     working array 
c       np              number of processors used
c       indxw(nindxw,np) working array
c       eta(ntm)        effective eta parameter
c	output:
c		mig(ntm,nxy)	migration output
c		fold(nxy)		fold of stack at output traces
c		ifcut(nf)		highcut indice (computed at first call)
c		ltaper(nf)		tapering length in samples (computed
c						at first call) 
c		wsave(nsave)		fft coefficients (computed at first
c						call)
c
	subroutine pstmeta(trace,nt,t0,dt,sx,sy,gx,gy,imute,
     2				fovt2,mig,nxy,ntm,x,y,
     3				tm,aperx,apery,fold,w1,
     4				f0,df,nf,ftaper,
     5				ifcut,ltaper,tracef,
     6				wsave,nsave,work1,work2,nfft,
     7              kxmax,kymax,dtau,tau0,angmax,
     8              indxw,nindxw,incdxw,np,
     9              resamp,ires,ntres,s2,scs,scg,eta)
		real trace(nt),tracef(nt,nf),t0,dt,sx,sy,gx,gy,w1(ntm)
		real fovt2(ntm),x(nxy),y(nxy),fold(nxy)
		integer imute
		real mig(ntm,nxy),tm(ntm),s2(ntm)
		real scs(ntm,nxy),scg(ntm,nxy)
		real aperx,apery
		integer nt,ntm,nxy
		real kxmax,kymax,angmax,dtau,tau0,f0,df,ftaper
		real kxmaxo,kymaxo,f0o
		integer nf
		real wsave(nsave),work1(nfft,nf),work2(nfft,nxy)
		integer indxw(nindxw,nxy), nindxw, incdxw 
		integer ifcut(nf),ltaper(nf)
		real cosa, sina, oaa, obb, cc, xp, yp
		integer iw, it, it0, iang
		real resamp(ntres,nf), eta(ntm)
		integer ires, ntres, np

		real dx2s, dy2s
		real dx2g, dy2g
		real sc1, sc2
		real coss, cosg

		real ts,tg,tmp,tmp2,s4,g4,fov4
		integer itmp, itmp2

		odt = 1./dt
		odtau = 1./dtau
		shift = 1.5 - t0/dt*ires

		imuter = imute * ires 

ccc midpoint locations
		xm = 0.5*(sx+gx)
		ym = 0.5*(sy+gy)

cccc compute some constants 
		do it=1,ntm
			w1(it) = fovt2(it)*tm(it)*dt 
		end do

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
     1			ifcut,ltaper,tracef,
     2			wsave,nsave,work1,work2,nfft)
		end if

		aperxx = aperx + 0.00001
		aperyy = apery + 0.00001
		kxmaxo = kxmax/df
		kymaxo = kymax/df
		epsx = kxmaxo*0.001
		epsy = kymaxo*0.001

		f0o = 1. - f0/df

		do it=1,ntm
			work1(it,1) = tm(it)*ires
		end do
		scale = 1./ires
		do iw=1,nf
		do ii=1,imuter
			resamp(ii,iw) = 0.
		end do
		do ii=imuter+1,ntres-1
			tmp = ii * scale + 1
			it = tmp 
			res = tmp - it
			if(it.lt.nt) then
			   resamp(ii,iw) = tracef(it,iw)*(1.-res)+
     1                                     tracef(it+1,iw)*res
	                else
			   resamp(ii,iw) = 0.
			end if 
		end do
cccc set last sample of resamp to be zero
		resamp(ntres,iw) = 0.
		end do

cccc compute zero-offset mute time 
ccccc		s02 = fovt2(1)*dtau*dtau*0.25 
		if(tau0.eq.0.) then
			s02 = fovt2(1)*dtau*dtau*0.25 
		else 
			s02 = fovt2(1)*tau0*tau0*0.25 
		end if
		tmute = (imute-1)*dt + t0
		tmp = tmute*tmute - ( (sx-gx)**2+(sy-gy)**2 )*s02 
		if(tmp.ge.0.) then
			tmute0 = sqrt(tmp)
		else 
			tmute0 = t0
		end if

		dis = (sx-gx)**2+(sy-gy)**2
		dis = sqrt(dis)
		if(dis.gt.0.) then
			cosa = (gx - sx)/dis
			sina = (gy - sy)/dis
		else 
			cosa = 1.
			sina = 0.
		end if 
		if(tmute.gt.0.) then
			oaa = 4.*s02/(tmute*tmute)
		else 
			oaa = 4.*s02/(dt*dt)
		end if
	
		if(tmute0.gt.0.) then	
			obb = 4 * s02/(tmute0*tmute0) 
			cc = tmute0*tmute0 
		else 
			obb = 4.*s02/(dt*dt)
			cc = dt*dt
		end if

		if(angmax.gt.89.9) then
			otan2 = 0.
		else if (angmax.lt.0.1) then
			otan2 = 999999.
		else
			otan2 = 2./tan(angmax*3.141592654/180.)
		end if

		oincd = 1./incdxw

c$ call schedctl(SCHEDMODE, SGS_FREE, 0)

cccc migration over output traces
c$par doall private(ixy,yi,xi,dys,dyg,dy2s,dy2g,dxs,dxg,dx2s)
c$par doall private(dx2g,d2s,d2g,xp,yp,tmp,t,it0,dov2,iang,iiend)
c$par doall private(ii,it,coss,ots,cosg,otg,fxmax,iwx)
c$par doall private(fymax,iwy,iw,itmp,itmp2,sc2,sc1,a)
c$par doall private(ts,tg,tmpe,fov4,s4,g4)
		do ixy=1,nxy
	   		yi = y(ixy)
	        xi = x(ixy)
	   		if(abs(ym-yi).le.aperyy .and. 
     1                abs(xm-xi).le.aperxx) then
	   	    dys = yi - sy
	   	    dyg = yi - gy
	   	    dy2s = dys * dys
	   	    dy2g = dyg*dyg
		    dxs = xi - sx
		    dxg = xi - gx
		    dx2s = dxs*dxs
		    dx2g = dxg*dxg
		    d2s = dx2s + dy2s
		    d2g = dx2g + dy2g
		    fold(ixy)=fold(ixy)+1.0
ccccc             mute time calculation
ccccc   (xp/aa)**2 + (yp/bb)**2 + (zp/bb)**2 = 1
ccccc     tp = 2.*zp/v
ccccc
	   	    xp = (xi-xm)*cosa + (yi-ym)*sina 
	   	    yp = -(xi-xm)*sina + (yi-ym)*cosa 
		    tmp = (1. - xp*xp*oaa - yp*yp*obb)*cc 
		    if(tmp.ge.0.) then
			t = (sqrt(tmp)-tau0)*odtau + 1.
			it0 = t
			it0 = max0(it0,1)
		    else
			it0 = 1
		    end if
ccccc             angle aperature calculation 
		    dov2 = ((xi-xm)**2+(yi-ym)**2)*s02
		    tmp = sqrt(dov2)*otan2
	            tmp = (tmp - tau0)*odtau + 1. 
		    iang = tmp
		    it0 = max0(it0,iang) 
        	tmp =(ntm-it0)*oincd + 1.
		    iiend = tmp
		    if(nf.gt.1) then
		       do ii=1,iiend
			      it = it0 + (ii-1)*incdxw 
     			  coss=d2s/(1. - s2(it)*d2s)
		          coss=1.+fovt2(it)*coss
				  tmpe = (1.+2.*eta(it))*fovt2(it)
				  fov4 = fovt2(it)*fovt2(it)
				  s4=-2.*eta(it)/(1.+tmpe*d2s)
				  s4=s4*d2s*d2s*fov4
				  coss=coss+s4 
				  if(coss.gt.0.) then
				  	coss = sqrt(coss)
				  else 
				  	coss = 99999.
				  end if
		          ots=1./coss
     			  cosg=d2g/(1. - s2(it)*d2g)
     		      cosg=1.+fovt2(it)*cosg
				  g4=-2.*eta(it)/(1.+tmpe*d2g)
				  g4=g4*d2g*d2g*fov4
				  cosg=cosg+g4 
				  if(cosg.gt.0.) then
				  	cosg = sqrt(cosg)
				  else 
				  	cosg = 99999.
				  end if
     		      otg=1./cosg
			      scs(ii,ixy) = coss+cosg
			      fxmax=kxmaxo/
     1			     (abs(w1(it)*(dxs*ots+dxg*otg))+epsx)+f0o
			      iwx = fxmax
			      fymax=kymaxo/
     1               (abs(w1(it)*(dys*ots+dyg*otg))+epsy)+f0o
			      iwy = fymax
			      iw = min0(iwx,iwy)
		          iw = min0(iw,nf)
			      indxw(ii,ixy) = max0(iw,1)
ccccc cos(as)+cos(ag)
			      work2(ii,ixy) = .5*(ots+otg)
		       end do
		       do it=it0,ntm
		          tmp = (it-it0)*oincd + 1.0
		          itmp = tmp
			      itmp2 = min0(itmp+1,iiend)
			      sc2 = tmp  - itmp
			      sc1 = 1. - sc2
			      a=sc1*work2(itmp,ixy)+sc2*work2(itmp2,ixy)
			      t=sc1*scs(itmp,ixy)+sc2*scs(itmp2,ixy)
			      t=t*work1(it,1)+shift
		          ii=t
			      ii = max0(ii,imuter)
			      ii = min0(ii,ntres)
			      iw = indxw(itmp,ixy)
			      mig(it,ixy) = mig(it,ixy)	+ resamp(ii,iw)*a
		       end do
            else 
		       do it=it0,ntm
			      scs(it,ixy) = d2s/(1. - s2(it)*d2s)
			      scg(it,ixy) = d2g/(1. - s2(it)*d2g)
                  ts=1.+fovt2(it)*scs(it,ixy)
				  tmpe = (1.+2.*eta(it))*fovt2(it)
				  fov4 = fovt2(it)*fovt2(it)
				  s4=-2.*eta(it)/(1.+tmpe*d2s)
				  s4=s4*d2s*d2s*fov4
				  ts=ts+s4 
				  if(ts.gt.0.) then
				  	ts = sqrt(ts)
				  else 
				  	ts = 99999.
				  end if
                  tg=1.+fovt2(it)*scg(it,ixy)
				  g4=-2.*eta(it)/(1.+tmpe*d2g)
				  g4=g4*d2g*d2g*fov4
				  tg=tg+g4 
				  if(tg.gt.0.) then
				  	tg = sqrt(tg)
				  else 
				  	tg = 99999.
				  end if
                  t=(ts+tg)*work1(it,1)+shift
                  ii=t
			      ii = max0(ii,imuter)
			      ii = min0(ii,ntres)
			      mig(it,ixy) = mig(it,ixy)
     1			    	+ resamp(ii,1)*0.5*(ts+tg)/(ts*tg)
		       end do
	        end if
          end if
        end do

		return
		end
