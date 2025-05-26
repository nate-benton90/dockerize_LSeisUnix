* $Revision: 1.2 $  $Date: 89/01/29 14:14:33 $
* $Source: /src/u/src/Subs/RCS/svanlsubs.f,v $

************************************************************************
************************************************************************
**                                                                    **
** Subroutine package for SUSVANL                                     **
**                                                                    **
**                                                                    **
**                                                                    **
** Credits:                                                           **
**     CWP: Jeff 						      **
**                                                                    **
** This program belongs to the Center for Wave Phenomena              **
** Colorado School of Mines                                           **
**                                                                    **
** Copyright 1989 by Center for Wave Phenomena                        **
**                                                                    **
************************************************************************
************************************************************************


***********************************************************************
*								      *
*	SVANL - velocity analysis at one velocity		      *
*								      *
*	Parameters:						      *
*								      *
*		data(,)		Input data			      *
*		out(,)		Output coherency trace		      *
*		x(,)		Offsets				      *
*		x		Velocity			      *
*		nt		Number of time samples		      *
*		dt		Time sample rate		      *
*		nx		Number of traces		      *
*		lgate		Length of coherency calculation gate  *
*				in samples			      *
*		mode		Flag determining coherency measure    *
*								      *
*	Local variables:					      *
*								      *
*		temp(,)		Work array			      *
*		it		Time loop counter		      *
*		ming		First sample of gate		      *
*		maxg		Last sample of gate		      *
*								      *
***********************************************************************

	subroutine svanl(data,out,x,v,nt,dt,nx,lgate,mode)

c  Declare parameters

	integer nt, nx, lgate, mode
	real data(nt,nx), out(nt), x(nx)
	real v, dt

c  Declare local variables

	real temp(1024,1024)
	integer it, iug, ilg, ming, maxg

c	*** Some preparatory calculations ***
	if ( mod(lgate,2) .ne. 0) then
		iug = lgate/2
		ilg = -iug
	else
		ilg = -lgate/2
		iug = -(ilg + 1)
	endif

c	*** Apply nmo to data ***
	call nmo(data,temp,x,v,nt,dt,nx)

c	*** Main Loop, find coherency at each time***
	do 1000 it = 1, nt

		ming = it + ilg
		maxg = it + iug
		if ( ming .le. 0) ming = 1
c			*** Gate starts before first time so set start at ***	
c			*** beginning of trace. 			  ***

		if ( maxg .gt. nt) maxg = nt
c			*** Gate extends past last time so set end at end ***
c			*** of trace.					  ***

c		*** Call coherency routine ***
		if ( mode .eq. 1) then
c			*** Semblance ***
			out(it) =  smblnc(temp,nt,nx,ming,maxg)

		else if (mode .eq. 2) then
c			*** Unnormalized Crosscorrelation Sum ***
			out(it) = unccs(temp,nt,nx,ming,maxg)

		else if (mode .eq. 3) then
c			*** Statistically Normalized ***
c			*** Crosscorrelation Sum     ***
			out(it) = snccs(temp,nt,nx,ming,maxg)

		else if (mode .eq. 4) then
c			*** Energy Normalized    ***
c			*** Crosscorrelation Sum ***
			out(it)=(float(nx)*smblnc(temp,nt,nx,ming,maxg)
     &				 - 1.)/float(nx - 1)

		endif

1000		continue

	return
	end

***********************************************************************
*								      *
*	NMO - performs constant velocity nmo correction on data	      *
*								      *
*	Parameters:						      *
*								      *
*		datin(,)	Input Data			      *
*		datout(,)	Output Data			      *
*		x()		Offsets				      *
*		v		Velocity			      *
*		nt		Number of time samples		      *
*		dt		Time samle rate			      *
*		nx		Number of traces		      *
*								      *
*	Local variables:					      *
*								      *
*		tnmo()		Exact nmo times 		      *
*		gain()		Amount of stretch		      *
*		w()		Interpolation weights		      *
*		itnmo()		Base points for interpolations	      *
*		oldx		Previous offset 		      *
*		ovv		Scaled sloth: 1.0/(v*v*dt*dt)	      *
*		xxovv		x * x * ovv	 		      *
*		it		Time loop counter		      *
*		ix		Trace loop counter		      *
*								      *
*	Notes:							      *
*								      *
*	This routine is a modification of sunmo.c.		      *
*	sunmo.c handles time and space variable velocity fields.      *
*								      *
***********************************************************************

	subroutine nmo(datin,datout,x,v,nt,dt,nx)

c Declare parameters

	integer nt, nx
	real datin(nt,nx), datout(nt,nx), x(nx)
	real v, dt 

c Declare local variables

	integer it, ix, itnmo(0:1023)
	real tnmo(0:1023), gain(0:1023), w(2048)
	real oldx, ovv, xxovv
	
c	*** Convert to sloth and scale to unit time spacing ***

	ovv = 1.0/(v * v * dt * dt)

c	*** Main NMO loop over traces ***
	oldx = x(1) - 1.
	do 1000 ix = 1, nx
c		*** Use same nmo function if offset doesn't change ***
		if (x(ix) .ne. oldx) then

c			*** Calculate fractional nmo time index, tnmo ***
			xxovv = x(ix) * x(ix) * ovv

			do 1100 it = 0, nt-1   
				tnmo(it) = sqrt(it*it + xxovv)
1100				continue

c			*** Calculate amount of stretch ***
			gain(0) = tnmo(1) - tnmo(0)

			do 1200 it = 1, nt-1
				gain(it) = 0.5*(tnmo(it+1) - tnmo(it-1))
1200				continue
		   	gain(nt-1) = tnmo(nt-1) - tnmo(nt-2)

c			*** Calculate the interpolation coefficients ***
		    	call lintrp(itnmo, w, tnmo, nt, gain)
			endif
			

c		*** Perform the NMO ***
		call strtch(datout(1,ix), datin(1,ix), nt, itnmo, w)
1000		continue

	return
	end

***********************************************************************
*								      *
*	SMBLNC - compute semblance over a time gate		      *
*								      *
*	Parameters:						      *
*								      *
*		data(,)		Input data array		      *
*		nt		Number of time samples		      *
*		nx		Number of traces		      *
*		istart		Sample of start of time gate	      *
*		iend		Sample of end of time gate	      *
*								      *
*	Local variables:					      *
*								      *
*		bot		Denominator			      *
*		btemp		Temporary summing variable for bot    *
*		top		Numerator			      *
*		ttemp		Temporary summing variable for top    *
*		scale		Number of samples in gate	      *
*		it		Time counter			      *
*		ix		Trace counter			      *
*								      *
*		Notes:						      *
*								      *
*		Technical refernce: Hubral and Krey, SEG, 1980	      *
*					pp. 152 - 156.		      *
*								      *
***********************************************************************
	real function smblnc (data,nt,nx,istart,iend)

c  Declare parameters

	integer nt, nx, istart, iend
	real data(nt,nx)

c  Declare local variables

	real top, bot, ttemp, btemp, scale
	integer it

c	***Preliminary calculations***
	scale = float(nx)

c	***Initialize sums***
	top = 0.
	bot = 0.

c	***Main loop over time***
	do 1000 it = istart, iend

c		*** Initialize sums **
		ttemp = 0.
		btemp = 0.
	
		do 1100 ix = 1, nx	

c			*** Sum across traces ***
			ttemp = ttemp + data(it,ix)
			btemp = btemp + data(it,ix)*data(it,ix)
1100			continue

		ttemp = ttemp * ttemp

c		*** Sum over gate ***
		top = top + ttemp
		bot = bot + btemp

1000		continue

c	*** Final calculation ***
	if (bot .ne. 0.) then
		smblnc = top/(bot * scale)
	else
		smblnc = 0.
	endif

	return
	end

***********************************************************************
*								      *
*	UNCCS - compute unnormalized crosscorrelation sum over 	      *
*		a time gate					      *
*								      *
*	Parameters:						      *
*								      *
*		data(,)		Input data array		      *
*		nt		Number of time samples		      *
*		nx		Number of traces		      *
*		istart		Sample of start of time gate	      *
*		iend		Sample of end of time gate	      *
*								      *
*	Local variables:					      *
*								      *
*		sqosum		Square of sum across traces	      *
*		sumsqo		Sum of squares across traces	      *
*		it		Time counter			      *
*		ix		Trace counter			      *
*								      *
*		Notes:						      *
*								      *
*		Technical refernce: Hubral and Krey, SEG, 1980	      *
*					pp. 152 - 156.		      *
*								      *
***********************************************************************
	real function unccs (data,nt,nx,istart,iend)

c  Declare parameters

	integer nt, nx, istart, iend
	real data(nt,nx)

c  Declare local variables

	real sumosq, sqosum
	integer it


c	*** Initialize sum ***
	unccs = 0.

c	*** Main loop over time ***
	do 1000 it = istart, iend

c		*** Initialize sums **
		sqosum = 0.
		sumosq = 0.
	
		do 1100 ix = 1, nx	

c			*** Sum across traces ***
			sqosum = sqosum + data(it,ix)
			sumosq = sumosq + data(it,ix)*data(it,ix)
1100			continue

		sqosum = sqosum*sqosum

c		*** Sum over gate ***
		unccs = unccs + sqosum - sumosq

1000		continue

c	*** Final calculation ***
	unccs = unccs * 0.5

	return
	end


***********************************************************************
*								      *
*	SNCCS - compute stastically normalized crosscorrelation sum   *
*		over a time gate				      *
*								      *
*	Parameters:						      *
*								      *
*		data(,)		Input data array		      *
*		nt		Number of time samples		      *
*		nx		Number of traces		      *
*		istart		Sample of start of time gate	      *
*		iend		Sample of end of time gate	      *
*								      *
*	Local variables:					      *
*								      *
*		sumsqo()	Sum of squares along gate	      *
*		top		Numerator			      *
*		scale		Normalization scale		      *
*		it		Time counter			      *
*		ix		Trace counter			      *
*		jx		Trace counter			      *
*								      *
*		Notes:						      *
*								      *
*		Technical refernce: Hubral and Krey, SEG, 1980	      *
*					pp. 152 - 156.		      *
*								      *
***********************************************************************
	real function snccs (data,nt,nx,istart,iend)

c  Declare parameters

	integer nt, nx, istart, iend
	real data(nt,nx)

c  Declare local variables

	real sumosq(1024), scale, top
	integer it

c	*** Preliminary calculations ***
	do 1000 ix = 1, nx
		sumosq(ix) = 0.

		do 1100 it = istart, iend
			sumosq(ix)=sumosq(ix) + data(it,ix)*data(it,ix)
1100			continue
1000		continue

c	*** Initialize sum ***
	snccs = 0.

c	*** Main loop over time ***
	do 2000 it = istart, iend

		do 2100 ix = 1, nx-1

			do 2200 jx = 1, nx - ix

			     top = data(it,jx)*data(it,jx+ix)
			     scale = sqrt(sumosq(jx)*sumosq(jx+ix))
			     if (scale .ne. 0.) snccs=snccs + top/scale
2200			     continue

2100			continue

2000		continue


c	*** Final scaling calculation ***
	snccs = 2. * snccs /(float(nx) * float(nx-1))

	return
	end

