# inversion.spec --
#
#	Tksu inversion category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suinvvxzco]

Cat:	Inversion
Desc:	inversion of common-offset data for smooth v(x,z) plus perturbation
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	vfile bin r req		file containing velocity array v[nx][nz]
Port:	vpfile bin r -		file of slowness perturbation array vp[nx][nz]
Port:	par par r -		optional par file input
Param:	nx int 0< req		2nd (slow) dimension of velocity array
Param:	nz int 0< req		1st (fast) dimension of velocity array
Param:	nxm int 0< req		number of midpoints of input traces
Param:	dt float 0< tr.dt/10^6	time sampling interval (sec)
Param:	offs float - tr.offset	source-receiver offset
Param:	dxm float - tr.d2	midpoint sampling interval
Param:	fxm float - 0.		first midpoint in input trace
Param:	nxd int 0<= 5		skipped number of midpoints
Param:	dx float - 50.		x sampling interval of velocity
Param:	fx float - 0.		first x sample of velocity
Param:	dz float - 50.		z sampling interval of velocity
Param:	nxb int - nx/2		band centered at midpoints
Param:	nxc int - 0		horizontal range in which velocity is changed
Param:	nzc int - 0		vertical range in which velocity is changed
Param:	fxo float - 0.		x-coordinate of first output trace
Param:	dxo float - 15.		horizontal spacing of output traces
Param:	nxo int - 101		number of output traces
Param:	fzo float - 0.		z-coordinate of first point in output trace
Param:	dzo float - 15.		vertical spacing of output traces
Param:	nzo int - 101		number of points in output traces
Param:	fmax float - .25/dt	max frequency set for operator antialiasing (Hz)
Param:	ang float - 180.	max dip angle allowed in the image (degrees)
Param:	ls int 0<=1 0		if 1, line source; if 0, point source
Param:	pert int 0<=1 0		if 1, calculate time correction from vpfile
Param:	verbose int 0<=1 1	if 1, report diagnostics

[suinvzco3d]

Cat:	Inversion
Desc:	inversion of common-offset data with v(z) velocity function in 3D
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	vfile bin r req		file containing velocity array v[nz]
Port:	par par r -		optional par file input
Param:	nz int - req		number of z samples in velocity
Param:	nxm int - req		number of midpoints of intput traces
Param:	ny int - req		number of input lines
Param:	dt float 0< tr.dt/10^6	time sampling interval (sec)
Param:	offs float - tr.offset	source-receiver offset
Param:	dxm float - tr.d2	midpoint sampling interval
Param:	fxm float - 0.		first midpoint in input trace
Param:	nxd int - 5		skipped number of midpoints
Param:	dx float - 50.		x sampling interval of velocity
Param:	fx float - 0.		first x sample of velocity
Param:	dz float - 50.		z sampling interval of velocity
Param:	nxb int - nx/2		band centered at midpoints
Param:	fxo float - 0.		x-coordinate of first output trace
Param:	dxo float - 15.		horizontal spacing of output traces
Param:	nxo int - 101		number of output traces
Param:	fyo float - 0.		y-coordinate of first output trace
Param:	dyo float - 15.		y-corrdinate spacing of output traces
Param:	nyo int - 101		number of output traces in y direction
Param:	fzo float - 0.		z-coordinate of first point in output trace
Param:	dzo float - 15.		vertical spacing of output traces
Param:	nzo int - 101		number of points in output traces
Param:	fmax float - .25/dt	max frequency set for operator antialiasing (Hz)
Param:	ang float - 180.	max dip angle allowed in the image (degrees)
Param:	verbose int 0<=1 1	if 1, report diagnostics

