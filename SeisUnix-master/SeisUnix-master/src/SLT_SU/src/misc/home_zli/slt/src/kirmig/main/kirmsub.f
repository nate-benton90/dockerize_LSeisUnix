
ccccc kirchhoff migration of input trace
c	subroutine kirmig(trace,xs,xr,tmin,nt,dt,
c     *			  migs,xminm,zminm,dxm,dzm,nxm,nzm,
c     *		          noff,offmin,doff,fold,
c     *			  ttbl,atbl,xmint,zmint,dxt,dzt,nxt,nzt,
c     *			  nst,smint,dst,ts,tr,as,ar,dxtol,
c     *			  sigxt,sigzt,inxt,inzt,iamp,mtrace,
c     *                   twk1,twk2,awk1,awk2,tscale,trwk,
c     *                   intps,intpx,intpz,mute,
c     *			  ascale,amptype,
c     *			  tfile,afile,aper,apanl,ananr,zw,
c     *			  tandps,idip,nxdip,dxdip)
c where
c	trace	---	input trace
c	xs	---	source location	
c	xr	---	receiver location	
c	tmin	---	time trace starts 
c	nt	---	number of samples per trace
c	dt	---	sampling interval		
c	migs	---	migrated offsets (array of (nzm,nxm,noff)) 
c	xminm	---	minimum lateral position of migration
c	zminm	---	minimum vertical position of migraiton
c	dxm	---	trace spacing in output migrated section
c	dzm	---	depth interval in output migrated section
c	nxm	---	number of traces per output migration section	
c	nzm	---	number of depths in output migration section
c	noff	---	number of offsets to output
c	offmin	---	minimum offset to output
c	doff	---	offset increment to output
c	fold	---	work array (noff)
c	ttbl	---	travel time table (array(nzt,nxt,nst))
c	atbl	---	amplitude table (array(nzt,nxt,nst))
c	xmint	---	minimum lateral position of time/amplitude table 
c	zmint	---	minimum vertical position of time/amplitude table
c	dxt	---	trace spacing in time/amplitude table
c	dzt	---	depth interval in time/amplitude table
c	nxt	---	number of traces per time/amplitude section	
c	nzt	---	number of depths per time/amplitude section
c	nst	---	number of time/amplitude sections 
c			(number of different source position in t/a tables)
c	smint	---	lateral position of first source of t/a table 
c	dst	---	source interval of t/a table 
c	ts	---	work buffer for time from source to reflector 
c	tr	---	work buffer for time from reflector to receiver
c	as	---	work buffer for amplitude of source to reflector
c	ar	---	work buffer for amplitude of reflector to receiver
c	dxtol	---	distance tolerance in t/a table reinterpolation
c			(use the previous interpolated result, if  
c				use ts and as, if |xs_now-xs_pre|<dxtol
c				use tr and ar, if |xr_now-xr_pre|<dxtol)
c	sigxt	---	work buffer (after first call, it must not be changed)
c			(lateral residul of t/a table relative to migs)
c	sigzt	---	work buffer (after first call, it must not be changed)
c			(vertical residul of t/a table relative to migs)
c	inxt	---	work buffer (after first call, it must not be changed)
c			(lateral indice of t/a table relative to migs)
c	sigzt	---	work buffer (after first call, it must not be changed)
c			(vertical indice of t/a table relative to migs)
c	iamp	---	amplitude application mode (0=no atbl not needed; 1=yes)
c	mtrace	---	acumulated number of migrated traces
c	twk1	---	work buffer
c	twk2	---	work buffer
c	awk1	---	work buffer
c	awk2	---	work buffer
c	tscale	---	time table scale
c	trwk	---	work buffer 
c	intps	---	mode of source interpolation in t/a table interpolation 
c			(0=no need for interpolation; 1=yes)
c	intpx	---	mode of trace interpolation in t/a table interpolation 
c			(0=no need for interpolation; 1=yes)
c	intpz	---	mode of depth interpolation in t/a table interpolation 
c			(0=no need for interpolation; 1=yes)
c	mute	---	mute time of the trace (in sample)
c	ascale	---	amplitude scale in t/a table
c	amptype	---	amplitude computation type (1=as+ar; 0 or 2=as*ar)
c	tfile	---	name of disk file storing the travel time table 
c			(ttbl not needed if tfile is not "null"; when tfile
c			is "null", ttbl must be used)
c	afile	---	name of disk file storing the amplitude table 
c			(atbl not needed if afile is not "null"; when afile
c			is "null", atbl must be used)
c			NOTE: tfile and afile should be used, only if there 
c			      is no enough memory to hold ttbl and atbl.	
c       aper    ---     lateral aperature of migration
c	apanl	---	aperature angle (in degrees) at left 
c	apanr	---	aperature angle (in degrees) at right 
c	zw	---	water bottom depth at input cdp location
c       tandps  ---     tan of dip grid array tandps(nz,nxdip,2) 
c			tandps(nz,nxdip,1) lower limit 
c			tandps(nz,nxdip,2) higher limit 
c       idip	---	dip limit flag (1=apply tandps; 0=no, tandps not used)
c       nxdip   ---     lateral dimension of tandps array
c       dxdip   ---     lateral spacing of tandps
c NOTE!!!:
c	all the time unit are ms if ttbl is given, otherwise (tfile is given)
c	time unit is s.
c
c
c	AUTHOR:	ZHIMING	LI	6/91 	      	
	
	subroutine kirmig(trace,xs,xr,tmin,nt,dt,
     *			  migs,xminm,zminm,dxm,dzm,nxm,nzm,
     *		          noff,offmin,doff,fold,
     *			  ttbl,atbl,xmint,zmint,dxt,dzt,nxt,nzt,
     *			  nst,smint,dst,ts,tr,as,ar,dxtol,
     *			  sigxt,sigzt,inxt,inzt,iamp,mtrace,
     *                    twk1,twk2,awk1,awk2,tscale,trwk,
     *                    intps,intpx,intpz,mute,
     *			  ascale,amptype,
     *			  tfile,afile,aper,apanl,apanr,zw,
     *			  tandps,idip,nxdip,dxdip)
	real trace(nt),xs,xr,tmin,dt,trwk(nt)
	real migs(nzm,nxm,noff),xminm,zminm,dxm,dzm,offmin,doff
	real tandps(nzm,nxdip,2)
	real ts(nzm,nxm),tr(nzm,nxm),as(nzm,nxm),ar(nzm,nxm),dxtol
	integer*2 ttbl(nzt,nxt,nst),atbl(nzt,nxt,nst)
        real twk1(nzt,nxt), twk2(nzt,nxt)
        real awk1(nzt), awk2(nzt,nxt)
	real xmint,zmint,dxt,dzt
	real smint,dst,tscale,ascale
	integer nt,nzm,nxm,noff,nzt,nxt,nst,mtrace
        real fold(noff)
	integer inxt(nxm),inzt(nzm),iamp,mute
	real sigxt(nxm),sigzt(nzm)
	real odt,ots,aper
	integer amptype
	character*(*) afile,tfile
	real to1,xmid,xmin,xmax
	integer ixmin,ixmax
	real zw
c initialization
	if ( mtrace .eq. 0 ) then
	   call zeroal(fold,noff,4)	 
	   call zeroal(migs,nzm*nxm*noff,4)	 
	   xsprev = -99999999999.
	   xrprev = -99999999999.
	   do ix=1,nxm
	      xt = (xminm+(ix-1)*dxm-xmint)/dxt + 1.
	      ixt = xt	
	      if(ixt.lt.1) ixt = 1
	      if(ixt.gt.nxt-1) ixt=nxt-1
	      inxt(ix) = ixt
	      sigxt(ix) = (xt-ixt)
	   end do
	   do iz=1,nzm	 
	      zt = (zminm+(iz-1)*dzm-zmint)/dzt + 1.
	      izt = zt
	      if(izt.lt.1) izt = 1
	      if(izt.gt.nzt-1) izt=nzt-1
	      inzt(iz) = izt
	      sigzt(iz) = zt - izt
	   end do
	   odt = 1./dt
	   ots = 1./tscale
	   ots = ots * odt
c   open time and amplitude files if needed
	   ltfile = lenstr(tfile)
	   if ( tfile(1:4) .ne. "null" ) then
	      itdisk = 30
	      open(unit=itdisk,file=tfile(1:ltfile),
     &		   form='unformatted',status='old',access='direc',
     &             recl=nzt*nxt*4)
	   else
	      itdisk = 0
	   end if 
	   lafile = lenstr(afile)
	   if ( afile(1:4) .ne. "null" ) then
	      iadisk = 60
	      open(unit=iadisk,file=afile(1:lafile),
     &		   form='unformatted',status='old',access='direc',
     &             recl=nzt*nxt*4)
	   else
	      iadisk = 0
	   end if 
	end if
c check if input offset is within the output offset range 
	offin = abs(xs-xr)
        offset = (offin - offmin)/doff+1.5  
        ioff = offset

	if (ioff.lt.1 .or. ioff.gt.noff) return 

	fold(ioff) = fold(ioff) + 1
	mtrace = mtrace + 1

cccc determine aperature
	xmid = 0.5 * (xr + xs)
	xmin = xmid - aper 
	xmax = xmid + aper
	xmin = (xmin-xminm)/dxm+1.
	ixmin = xmin
	if(ixmin.lt.1) then
		ixmin = 1
	else if(ixmin.gt.nxm) then
		ixmin = nxm
	end if	
	xmax = (xmax-xminm)/dxm+1.
	ixmax = xmax
	if(ixmax.lt.1) then
		ixmax = 1
	else if(ixmax.gt.nxm) then
		ixmax = nxm
	end if

c check if time and amplitude tables are to be computed 
        if (abs(xs-xsprev).gt.dxtol) then
           call tnatab(xs,ts,as,twk1,twk2,awk1,awk2,
     *                 xminm,zminm,dxm,dzm,nxm,nzm,
     *                 ttbl,atbl,xmint,zmint,dxt,dzt,nxt,nzt,
     *                 nst,smint,dst,inxt,inzt,sigxt,sigzt,iamp,
     *                 ots,intps,intpx,intpz,amptype,ascale,
     *                 itdisk,iadisk,1,nxm)
        end if
        if (abs(xr-xrprev).gt.dxtol) then
           call tnatab(xr,tr,ar,twk1,twk2,awk1,awk2,
     *                 xminm,zminm,dxm,dzm,nxm,nzm,
     *                 ttbl,atbl,xmint,zmint,dxt,dzt,nxt,nzt,
     *                 nst,smint,dst,inxt,inzt,sigxt,sigzt,iamp,
     *                 ots,intps,intpx,intpz,amptype,ascale,
     *                 itdisk,iadisk,1,nxm)
        end if

cccc  integration along time to avoid operator aliasing
        trwk(1) = trace(1)
        do it=2,nt-1
cccc       trwk(it) = 0.5*trace(it) + 0.25*(trace(it+1)+trace(it-1))
           trwk(it) = trace(it)
        end do
        trwk(nt) = trace(nt)
ccc water bottom mute
	anglel = apanl * 3.141592654 / 180.
	angler = apanr * 3.141592654 / 180.
	tananl = tan(anglel)
	tananr = tan(angler)

	tmp = (zw-zminm)/dzm + 1
	izw = tmp
	if ( izw .lt. 1 ) izw = 1
ccccc
c apply kirchhoff summatation 
	to1 = 1. - tmin*odt
ccc idip; amptype;iamp;
	if(idip.eq.0) then
		if(iamp.eq.0) then
		 call sum000(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		else if(iamp.eq.1 .and. amptype.ne.1 ) then
		 call sum001(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		else if(iamp.eq.1 .and. amptype.eq.1 ) then
		 call sum011(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		end if
	else 
		if(iamp.eq.0) then
		 call sum100(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		else if(iamp.eq.1 .and. amptype.ne.1 ) then
		 call sum101(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		else if(iamp.eq.1 .and. amptype.eq.1 ) then
		 call sum111(trwk,migs(1,1,ioff),ts,tr,as,ar,tandps,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
		end if
	end if
c update previour source and receiver locations 
	xsprev = xs
	xrprev = xr
	return
	end

ccc idip=0; amptype=0, or 1, or 2; iamp=0
	subroutine sum000(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t

        real ol, or, si

        ol = 1./(tananl*dzm)
        or = 1./(tananr*dzm)
        si = tananl * tananr

        do ix=ixmin,ixmax
                xi = xminm+(ix-1)*dxm-xmid
                if(xi.eq.0. .and. si.le.0.) then
                        izmin = 1
                        izmax = nzm
                else
                        zil = xi * ol
                        zir = xi * or
                        if(zil.le.0.) zil = nzm - 1
                        if(zir.le.0.) zir = nzm - 1
                        izil = zil + 1
                        izir = zir + 1
                        izmin = min0(izil,izir)
                        izmax = max0(izil,izir)
                        if (izmin.le.1) izmin = 2
                        izmin = min0(izmin,nzm)
                        izmax = min0(izmax,nzm)
                        if(izmin.eq.nzm) izmin=izmax + 1
                end if
                if (izmin.lt.izw) izmin = izw
	        do iz=izmin,izmax 
           	    	t = ts(iz,ix)+tr(iz,ix)+to1 
	            	it = t 
	            	if (it.ge.mute .and. it .le. nt)
     1                     migs(iz,ix)=migs(iz,ix)+trwk(it)
	        end do
        end do
	return
	end 
ccc idip=0; amptype=0 or 2; iamp=1
	subroutine sum001(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t, a

	real ol, or, si

	ol = 1./(tananl*dzm) 
	or = 1./(tananr*dzm) 
	si = tananl * tananr

        do ix=ixmin,ixmax
		xi = xminm+(ix-1)*dxm-xmid
		if(xi.eq.0. .and. si.le.0.) then 
			izmin = 1
			izmax = nzm
		else 
			zil = xi * ol
			zir = xi * or
			if(zil.le.0.) zil = nzm - 1
			if(zir.le.0.) zir = nzm - 1
			izil = zil + 1
			izir = zir + 1
			izmin = min0(izil,izir)
			izmax = max0(izil,izir)
			if (izmin.le.1) izmin = 2
			izmin = min0(izmin,nzm)
			izmax = min0(izmax,nzm)
			if(izmin.eq.nzm) izmin=izmax + 1
		end if
		if (izmin.lt.izw) izmin = izw 
	        do iz=izmin,izmax 
           	    	t = ts(iz,ix)+tr(iz,ix)+to1 
		    	a = as(iz,ix) * ar(iz,ix)
	            	it = t 
	            	if (it.ge.mute .and. it .le. nt)
     1                     migs(iz,ix)=migs(iz,ix)+trwk(it)*a
	        end do
        end do
	return
	end 
ccc idip=0; amptype=1; iamp=1
	subroutine sum011(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t, a

	real ol, or, si

	ol = 1./(tananl*dzm) 
	or = 1./(tananr*dzm) 
	si = tananl * tananr

        do ix=ixmin,ixmax
		xi = xminm+(ix-1)*dxm-xmid
		if(xi.eq.0. .and. si.le.0.) then 
			izmin = 1
			izmax = nzm
		else 
			zil = xi * ol
			zir = xi * or
			if(zil.le.0.) zil = nzm - 1
			if(zir.le.0.) zir = nzm - 1
			izil = zil + 1
			izir = zir + 1
			izmin = min0(izil,izir)
			izmax = max0(izil,izir)
			if (izmin.le.1) izmin = 2
			izmin = min0(izmin,nzm)
			izmax = min0(izmax,nzm)
			if(izmin.eq.nzm) izmin=izmax + 1
		end if
		if (izmin.lt.izw) izmin = izw 
	        do iz=izmin,izmax 
           	    	t = ts(iz,ix)+tr(iz,ix)+to1 
		    	a = as(iz,ix) + ar(iz,ix)
	            	it = t 
	            	if (it.ge.mute .and. it .le. nt)
     1                     migs(iz,ix)=migs(iz,ix)+trwk(it)*a
	        end do
        end do
	return
	end 
ccc idip=1; amptype=0, or 1, or 2; iamp=0
	subroutine sum100(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t
	real dxr
	integer ixdip
	real ol, or, si, oz

        ol = 1./(tananl*dzm)
        or = 1./(tananr*dzm)
        si = tananl * tananr
	dxr = dxm/dxdip
	oz = 1./dzm
	
        do ix=ixmin,ixmax
		xi = xminm+(ix-1)*dxm-xmid
                if(xi.eq.0. .and. si.le.0.) then
                        izmin = 1
                        izmax = nzm
                else
                        zil = xi * ol
                        zir = xi * or
                        if(zil.le.0.) zil = nzm - 1
                        if(zir.le.0.) zir = nzm - 1
                        izil = zil + 1
                        izir = zir + 1
                        izmin = min0(izil,izir)
                        izmax = max0(izil,izir)
                        if (izmin.le.1) izmin = 2
                        izmin = min0(izmin,nzm)
                        izmax = min0(izmax,nzm)
                        if(izmin.eq.nzm) izmin=izmax + 1
                end if
                if (izmin.lt.izw) izmin = izw
		xi = xi*oz
		tmp = ix * dxr
		ixdip = tmp + 1
		if(ixdip.lt.1) ixdip=1
		if(ixdip.gt.nxdip) ixdip=nxdip
	        do iz=izmin,izmax 
			tmp = xi/(iz-1.)

			if(tmp.ge.tandip(iz,ixdip,1)
     1			.and.tmp.le.tandip(iz,ixdip,2))then 
           	    		t = ts(iz,ix)+tr(iz,ix)+to1 
	            		it = t 
	            		if (it.ge.mute .and. it .le. nt)
     1                     	   migs(iz,ix)=migs(iz,ix)+trwk(it)
			end if
	        end do
        end do
	return
	end
ccc idip=1; amptype=0 or 2; iamp=1
	subroutine sum101(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t, a
	real dxr
	integer ixdip
	real ol,or,si,oz

	ol = 1./(tananl*dzm) 
	or = 1./(tananr*dzm) 
	si = tananl *tananr
	oz = 1./dzm


	dxr = dxm/dxdip 

        do ix=ixmin,ixmax
		xi = xminm+(ix-1)*dxm-xmid
                if(xi.eq.0. .and. si.le.0.) then
                        izmin = 1
                        izmax = nzm
                else
                        zil = xi * ol
                        zir = xi * or
                        if(zil.le.0.) zil = nzm - 1
                        if(zir.le.0.) zir = nzm - 1
                        izil = zil + 1
                        izir = zir + 1
                        izmin = min0(izil,izir)
                        izmax = max0(izil,izir)
                        if (izmin.le.1) izmin = 2
                        izmin = min0(izmin,nzm)
                        izmax = min0(izmax,nzm)
                        if(izmin.eq.nzm) izmin=izmax + 1
                end if
                if (izmin.lt.izw) izmin = izw
		xi = xi*oz
		tmp = ix * dxr
		ixdip = tmp + 1
		if(ixdip.lt.1) ixdip=1
		if(ixdip.gt.nxdip) ixdip=nxdip
	        do iz=izmin,izmax 
			tmp = xi/(iz-1.)
			if(tmp.ge.tandip(iz,ixdip,1)
     1			.and.tmp.le.tandip(iz,ixdip,2))then 

           	    		t = ts(iz,ix)+tr(iz,ix)+to1 
		    		a = as(iz,ix) * ar(iz,ix)
	            		it = t 
	            		if (it.ge.mute .and. it .le. nt)
     1                     	   migs(iz,ix)=migs(iz,ix)+trwk(it)*a
			end if
	        end do
        end do
	return
	end 
ccc idip=1; amptype=1; iamp=1
	subroutine sum111(trwk,migs,ts,tr,as,ar,tandip,
     1			 trnt,nzm,nxm,to1,zminm,xminm,
     2                   dzm,dxm,xmid,ixmin,ixmax,izw,mute,nt,
     3                   tananl,tananr,nxdip,dxdip)
	real trwk(nt), migs(nzm,nxm), ts(nzm,nxm), tr(nzm,nxm)
	real as(nzm,nxm), ar(nzm,nxm), tandip(nzm,nxdip,2)
	integer nt, nzm, nxm, ixmin, ixmax, izw,mute,nxdip
	real dzm,dxm,zminm,xminm,xmid,to1,tananl,tananr,dxdip

	real xi, zil, zir
	integer it, izil, izir, izmin, izmax
	real t, a
	real dxr
	integer ixdip
	real ol,or,si,oz

	ol = 1./(tananl*dzm)
	or = 1./(tananr*dzm)
	si = tananl * tananr
	oz = 1./dzm
	dxr = dxm/dxdip

        do ix=ixmin,ixmax
		xi = xminm+(ix-1)*dxm-xmid
                if(xi.eq.0. .and. si.le.0.) then
                        izmin = 1
                        izmax = nzm
                else
                        zil = xi * ol
                        zir = xi * or
                        if(zil.le.0.) zil = nzm - 1
                        if(zir.le.0.) zir = nzm - 1
                        izil = zil + 1
                        izir = zir + 1
                        izmin = min0(izil,izir)
                        izmax = max0(izil,izir)
                        if (izmin.le.1) izmin = 2
                        izmin = min0(izmin,nzm)
                        izmax = min0(izmax,nzm)
                        if(izmin.eq.nzm) izmin=izmax + 1
                end if
                if (izmin.lt.izw) izmin = izw
		xi = xi*oz
		tmp = ix * dxr
		ixdip = tmp + 1
		if(ixdip.lt.1) ixdip=1
		if(ixdip.gt.nxdip) ixdip=nxdip
	        do iz=izmin,izmax 
			tmp = xi/(iz-1.)
			if(tmp.ge.tandip(iz,ixdip,1)
     1			.and.tmp.le.tandip(iz,ixdip,2)) then

           	    		t = ts(iz,ix)+tr(iz,ix)+to1 
		    		a = as(iz,ix) + ar(iz,ix)
	            		it = t 
	            		if (it.ge.mute .and. it .le. nt)
     1                     	   migs(iz,ix)=migs(iz,ix)+trwk(it)*a
			end if
		end do
	end do
	return
	end 

c subroutine interpolates time and amplitude from tables ttbl and atbl
c (trilinear interpolation is used)
	subroutine tnatab(xx,tx,ax,twk1,twk2,awk1,awk2,
     *			  xminm,zminm,dxm,dzm,nxm,nzm,
     *			  ttbl,atbl,xmint,zmint,dxt,dzt,nxt,nzt,
     *			  nst,smint,dst,inxt,inzt,sigxt,sigzt,iamp,
     *			  ots,intps,intpx,intpz,amptype,ascale,
     *			  itdisk,iadisk,ixmin,ixmax)
	real xx,tx(nzm,nxm),ax(nzm,nxm)
	real twk2(nzt,nxt),twk1(nzt,nxt)
	real awk2(nzt,nxt),awk1(nzt)
	real xminm,zminm,dxm,dzm,ots
        integer*2 atbl(nzt,nxt,nst), ttbl(nzt,nxt,nst)  
	real xmint,zmint,dxt,dzt,ascale
	integer nzm,nxm,nxt,nzt,nst,iamp,amptype
	integer intps,intpx,intpz,ixmin,ixmax
	integer itdisk, iadisk
	real smint,dst
	integer inxt(nxm), inzt(nzm) 
	real sigxt(nxm), sigzt(nzm) 
	real tiny
cccc
        st = (xx-smint)/dst + 1
	is = st 
	if (is.lt.1) is=1
	if (is.gt.nst-1) is=nst-1
	sig3 = st - is
	osig3 = 1.0 - sig3
	is1 = is + 1
	n=nzt*nxt
	tiny=0.001
ccccccccccccc
	if (intps.eq.0) then
c	      call msgsc("no intp s \0",xx)
	      if ( itdisk .ne. 0 ) then
	         read(itdisk,rec=is) twk2
	      else
	         call tbcopy(ttbl(1,1,is),twk2,n,ots)
	      end if
	      if (iamp.eq.1) then
	         if ( iadisk .ne. 0 ) then
	            read(iadisk,rec=is) awk2
	         else
	            call abcopy(atbl(1,1,is),awk2,n) 
	         end if
	      end if
	else
	   if (abs(sig3).lt.tiny) then
	      if ( itdisk .ne. 0 ) then
	         read(itdisk,rec=is) twk2
	      else
	         call tbcopy(ttbl(1,1,is),twk2,n,ots)
	      end if
	      if (iamp.eq.1) then
	         if(iadisk .ne. 0 ) then
	            read(iadisk,rec=is) awk2
	         else
 		    call abcopy(atbl(1,1,is),awk2,n) 
	         end if
	      end if
	   else if (abs(osig3).lt.tiny) then
	      if ( itdisk .ne. 0 ) then
	         read(itdisk,rec=is1) twk2
	      else
	         call tbcopy(ttbl(1,1,is1),twk2,n,ots)
	      end if
	      if (iamp.eq.1) then
	         if(iadisk .ne. 0 ) then
	            read(iadisk,rec=is1) awk2
	         else
 		    call abcopy(atbl(1,1,is1),awk2,n) 
	         end if
	      end if
	   else
	      if ( itdisk .eq. 0 ) then
	         s1 = osig3*ots
	         s2 = sig3*ots
	         call irintp(ttbl(1,1,is),ttbl(1,1,is1),s1,s2,twk2,n)
	      else
		 read(itdisk,rec=is) twk1
		 read(itdisk,rec=is1) twk2
	         call rrintp(twk1,twk2,osig3,sig3,twk2,n)
	      end if
	      if (iamp.eq.1) then
	         if(iadisk.eq.0)then
              call irintp(atbl(1,1,is),atbl(1,1,is1),osig3,sig3,awk2,n)
		 else
		    read(iadisk,rec=is) twk1
		    read(iadisk,rec=is1) awk2
	            call rrintp(twk1,awk2,osig3,sig3,awk2,n)
		 end if
	      end if
	   end if
	end if
ccccccccccccC
	if (intpx.eq.0 .and. intpz.eq.0) then
c           call msgsc("no intp x and z \0",xx)
	   n = nxm*nzm
	   call rrcopy(twk2,tx,n)
	   if(iamp.eq.1) call rrcopy(awk2,ax,n)
	else
	   do ix=ixmin,ixmax
	      ixx = inxt(ix)
	      ixx1 = ixx + 1
	      sig2 = sigxt(ix)
	      osig2 = 1.0 - sig2
	      if(intpx.eq.0) then
	         call rrcopy(twk2(1,ixx),twk1,nzt)
	         if(iamp.eq.1) call rrcopy(awk2(1,ixx),awk1,nzt)
	      else
	         if (abs(sig2).lt.tiny) then
	            call rrcopy(twk2(1,ixx),twk1,nzt)
	            if(iamp.eq.1) call rrcopy(awk2(1,ixx),awk1,nzt)
	         else if(abs(osig2).lt.tiny) then
	            call rrcopy(twk2(1,ixx1),twk1,nzt)
	            if(iamp.eq.1) call rrcopy(awk2(1,ixx1),awk1,nzt)
	         else
	            call rrintp(twk2(1,ixx),twk2(1,ixx1),
     *			        osig2,sig2,twk1,nzt)      
	            if(iamp.eq.1)
     *	               call rrintp(awk2(1,ixx),awk2(1,ixx1),
     *			           osig2,sig2,awk1,nzt)
	         end if
	      end if
	      if (intpz.eq.0) then
		   do iz=1,nzm
		      tx(iz,ix) = twk1(iz,1)	
		   end do
		   if (iamp.eq.1) then
		      do iz=1,nzm
		         ax(iz,ix) = awk1(iz)	
		      end do
		   end if
	      else  
	         if(iamp.eq.1) then
	            do iz=1,nzm
	               izz = inzt(iz)
                       izz1 = izz+1
	               sig1 = sigzt(iz)
cccc	               osig1 = 1.0 - sig1
               tx(iz,ix)=twk1(izz,1)+sig1*(twk1(izz1,1)-twk1(izz,1))
               ax(iz,ix)=awk1(izz)+sig1*(awk1(izz1)-awk1(izz))
cccc	               tx(iz,ix)=twk1(izz,1)*osig1+sig1*twk1(izz1,1)
cccc	               ax(iz,ix)=awk1(izz)*osig1+sig1*awk1(izz1)
	            end do
	         else
	            do iz=1,nzm
	               izz = inzt(iz)
                       izz1 = izz+1
	               sig1 = sigzt(iz)
	               osig1 = 1.0 - sig1
	               tx(iz,ix)=twk1(izz,1)*osig1+sig1*twk1(izz1,1)
	            end do
		end if
	      end if
	   end do
	end if
	return
        end 
cccc
c     time table copy 
	subroutine tbcopy(v1,v2,n,ots)
	integer*2 v1(n)
	real v2(n),ots
	integer n
	do i=1,n
	   v2(i) = v1(i)*ots
	end do
	return
	end
c     amplitude table copy 
	subroutine abcopy(v1,v2,n)
	integer*2 v1(n)
	real v2(n)
	integer n
	do i=1,n
	   v2(i) = v1(i)
	end do
	return
	end
c real vetor copy
	subroutine rrcopy(v1,v2,n)
	real v1(n)
	real v2(n)
	integer n
	do i=1,n
	   v2(i) = v1(i)
	end do
	return
	end
ccc
c   time or amplitude table interpolations
c
	subroutine irintp(v1,v2,s1,s2,vv,n)
	integer*2 v1(n), v2(n)
	real vv(n), s1, s2
	integer n
	do i=1,n
	   vv(i) = v1(i)*s1+v2(i)*s2
	end do
	return
	end
	subroutine rrintp(v1,v2,s1,s2,vv,n)
	real v1(n), v2(n)
	real vv(n), s1, s2
	integer n
	do i=1,n
	   vv(i) = v1(i)*s1+v2(i)*s2
	end do
	return
	end
