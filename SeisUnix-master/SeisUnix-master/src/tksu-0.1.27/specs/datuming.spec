# datuming.spec --
#
#	Tksu datuming category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[sudatumfd]

Cat:	Datuming
Desc:	2D zero-offset finite difference acoustic wave-equation	datuming
Port:	stdin su r req 		trace input
Port:	stdout su w req 	redatumed trace output
Port:	vfile1 bin r req	velocity file used for thin-lens term
Port:	vfile2 bin r req	velocity file used for diffraction term
Port:	par par r -		optional par file input
Param:	nt int 1<= req		number of time samples per trace
Param:	nx int 1<= req		number of receivers per shot gather
Param:	nsx int 1<= req		number of shot gathers
Param:	nz int 1<= req		number of depth steps to take
Param:	dx float 0< req		x sampling interval (m)
Param:	dz float 0< req		depth sampling interval (m)
Param:	mx int 1<= req		number of x samples in velocity model
Param:	mz int 1<= req		number of depth samples in velocity model
Param:	dt float 0< .004	time sampling interval (sec)
Param:	buff int 0<= 5		shot gather side padding (number of traces)
Param:	tap_len 0<= 5		taper length (number of traces)
Param:	x_0 0<= 0		x coord of left corner of velocity model

[sudatumk2dr]

Cat:	Datuming
Desc:	Kirchhoff datuming of receivers for 2D prestack data
Port:	stdin su r req 		trace input (shot gathers)
Port:	stdout su w req 	common offset migrated output
Port:	ttfile bin r req	input traveltime tables
Port:	jpfile text w stderr	job print file name
Port:	par par r -		optional par file input

Param:	fzt float - req		first depth sample in ttime table
Param:	nzt int 1<= req		number of depth samples in ttime table
Param:	dzt float 0< req	depth interval in ttime table
Param:	fxt float - req		first lateral sample in ttime table
Param:	nxt int 1<= req		number of lateral samples in ttime table
Param:	dxt float 0< req	lateral interval in ttime table
Param:	fs float - req		x-coord of first source in ttime table
Param:	ns int 1<= req		number of sources in ttime table
Param:	ds float 0< req		x-coord source interval in ttime table

Param:	fxi float - req		x-coord of first surface location
Param:	nxi int 1<= req		number of input surface locations
Param:	dxi float 0< req	surface location x interval
Param:	sgn enum-sgn - req	sign of the datuming process
Param:	dt float 0< "from header" input trace sample rate
Param:	ft float - 0		first time sample of input data
Param:	surf(1) string - -	x,z coord list defining recording surface
Param:	surf(2) string - -	x,z coord list defining new datum

Param:	fzo float - fzt		z-coord of first point in output trace
Param:	dzo float 0< 0.2*dzt	depth spacing of output trace
Param:	nzo int 1<= 5*(nzt-1)+1	number of samples in output trace
Param:	fxso float - fxt	x-coord of first shot
Param:	dxso float 0< 0.5*dxt	shot x spacing
Param:	nxso int 1<= 2*(nxt-1)+1	number of shots
Param:	fxgo float - fxt	x-coord of first receiver
Param:	dxgo float 0< 0.5*dxt	receiver x spacing
Param:	nxgo int 1<= nxso	number of receivers per shot
Param:	fmax float 0<= 0.25/dt	frequency highcut for input traces (Hz)
Param:	offmax float 0<= 99999	max absolute offset allowed in migration
Param:	aperx float 0<= 0.5*nxt*dxt migration lateral aperture
Param:	angmax float 0<= 60	migration angular aperture from vertical (deg)
Param:	v0 float 0< 1500	reference velocity at surface
Param:	dvz float - 0		reference velocity vertical gradient
Param:	antiali int 0<=1 1	if 1, apply antialias filter
Param:	mtr int 1<= 100		report every mtr traces
Param:	ntr int 1<= 100000	max number of input traces to migrate

[enum-sgn]
Desc:	sign of the datuming process
-1	up
1	down

[sudatumk2ds]

Cat:	Datuming
Desc:	Kirchhoff datuming of sources for 2D prestack data	
Port:	stdin su r req 		trace input (receiver gathers)
Port:	stdout su w req 	common offset migrated output
Port:	ttfile bin r req	input traveltime tables
Port:	jpfile text w stderr	job print file name
Port:	par par r -		optional par file input

Param:	fzt float - req		first depth sample in ttime table
Param:	nzt int 1<= req		number of depth samples in ttime table
Param:	dzt float 0< req	depth interval in ttime table
Param:	fxt float - req		first lateral sample in ttime table
Param:	nxt int 1<= req		number of lateral samples in ttime table
Param:	dxt float 0< req	lateral interval in ttime table
Param:	fs float - req		x-coord of first source in ttime table
Param:	ns int 1<= req		number of sources in ttime table
Param:	ds float 0< req		x-coord source interval in ttime table

Param:	fxi float - req		x-coord of first surface location
Param:	nxi int 1<= req		number of input surface locations
Param:	dxi float 0< req	surface location x interval
Param:	sgn enum-sgn - req	sign of the datuming process
Param:	dt float 0< "from header" input trace sample rate
Param:	ft float - 0		first time sample of input data
Param:	surf(1) string - -	x,z coord list defining recording surface
Param:	surf(2) string - -	x,z coord list defining new datum

Param:	fzo float - fzt		z-coord of first point in output trace
Param:	dzo float 0< 0.2*dzt	depth spacing of output trace
Param:	nzo int 1<= 5*(nzt-1)+1	number of samples in output trace
Param:	fxso float - fxt	x-coord of first shot
Param:	dxso float 0< 0.5*dxt	shot x spacing
Param:	nxso int 1<= 2*(nxt-1)+1	number of shots
Param:	fxgo float - fxt	x-coord of first receiver
Param:	exgo float - fxgo+(nxgo-1)*dxgo x-coord of last receiver
Param:	dxgo float 0< 0.5*dxt	receiver x spacing
Param:	nxgo int 1<= nxso	number of receivers per shot
Param:	fmax float 0<= 0.25/dt	frequency highcut for input traces (Hz)
Param:	offmax float 0<= 99999	max absolute offset allowed in migration
Param:	aperx float 0<= nxt*dxt/2 migration lateral aperture
Param:	angmax float 0<= 60	migration angular aperture from vertical (deg)
Param:	v0 float 0< 1500	reference velocity at surface
Param:	dvz float - 0		reference velocity vertical gradient
Param:	antiali int 0<=1 1	if 1, apply antialias filter
Param:	mtr int 1<= 100		report every mtr traces
Param:	ntr int 1<= 100000	max number of input traces to migrate

