
# velocity.spec --
#
#	Tksu velocity analysis category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[dzdv]

Cat:	Velocity Analysis
Desc:	determine dzdv, depth derivative with respect to velocity
Port:	stdin bin r req 	input common image gathers, primary amps
Port:	stdout bin w req 	output of dz/dv at the imaged points
Port:	afile bin r req		input common image gathers, extra amps
Port:	dfile bin w req		output of imaged depths in common image gathers
Port:	par par r -		optional par file input
Param:	nx int 0< req		number of migrated traces
Param:	nz int 0< req		number of points in migrated traces
Param:	dx float - req		horizontal spacing of migrated traces
Param:	dz float - req		vertical spacing of output trace
Param:	fx float - req		x-coordinate of first migrated trace
Param:	fz float - req		z-coordinate of first point in migrated trace
Param:	off0 float - req	first offset in common image gathers
Param:	noff int 0< req 	number of offsets in common image gathers
Param:	doff float - req	offset increment in common image gathers
Param:  dup-cip float-list - req triplets of x,z,r values, one for each CIG
LDesc:	x= x-value, z= z-value at zero-offset, r= r-parameter for CIG
Param:	nxw int 0< 0		window width along x for calculation of dz/dv
Param:	nzw int 0< 0		window width along z for calculation of dz/dv

[surelan]

Cat:	Velocity Analysis
Desc:	compute residual-moveout semblance for cdp gathers
Port:	stdin su r req		input migrated traces sorted by cdp
Port:	stdout su w req		output semblance traces
Port:	par par r -		optional par file input
Param:	nr int 0< 51		number of r-parameter samples
Param:	dr float 0< .01		r-parameter sampling interval
Param:	fr float - -0.25	first value of r-parameter
Param:	smute float 1< 1.5	samples with RMO stretch >smute are zeroed
Param:	dzratio int 0< 5	ratio of output/input depth sampling intervals
Param:	nsmooth int 0<= dzratio*2+1 length of semblance smoothing window
Param:	verbose int 0<=1 0	if 1, report diagnostics

[suvelan]

Cat:	Velocity Analysis
Desc:	comput stacking velocity semblance for cdp gathers
Port:	stdin su r req		input traces sorted by cdp
Port:	stdout su w req		output semblance traces
Port:	par par r -		optional par file input
Param:	nv int 0< 50		number of velocities
Param:	dv float - 50.		velocity sampling interval
Param:	fv float 0< 1500. 	first velocity
Param:	anis1 float - 0		numerator of extended quartic term
Param:	anis2 float - 0		in denominator of extended quartic term
Param:	smute float 1< 1.5	samples with NMO stretch > smute are zeroed
Param:	dtratio int 0< 5	ratio of output/input time sampling intervals
Param:	nsmooth int 0<= dtratio*2+1 length of semblance smoothing window
Param:	pwr float - 1.		semblance value to the power
Param:	verbose int 0<=1 0	if 1, report diagnostics

[velpert]

Cat:	Velocity Analysis
Desc:	estimate velocity parameter perturbation from CIG depths covariance
Port:	stdin bin r req		input of imaged depths in CIGs
Port:	dzfile bin r req	input of dz/dv at imaged depths in CIGs
Port:	stdout text w req	output of 3 estimated parameters
LDesc:	Output of 3 estimated parameters: correction to velocity (dlambda);
LDesc:	deviation of image depths to old and new velocity estimates;
LDesc:	sensitivity of velocity parameter to errors in imaged depths.
Port:	par par r -		optional par file input
Param:	noff int 0< req		number of offsets
Param:	ncip int 0< req		number of common image gathers
Param:	moff int 0< noff	number of first offsets used in velocity calc

