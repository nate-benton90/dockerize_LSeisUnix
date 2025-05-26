# synthetics.spec --
#
#	Tksu synthetics category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[elasyn]

Cat:	Synthetics
Cat:	Multicomponent
Desc:	synthetic seismograms for triangulated elastic media
Port:	stdin bin r req		input rayends file
Port:	xfile bin w x_compon.bin output file storing x-component traces
Port:	zfile bin w z_compon.bin output file storing z-component traces
Port:	infofile text w -	file storing useful information
Port:	par par r -		optional par file input
Param:	xg float-list - req	x coordinates of receiver surface
Param:	zg float-list - req	z coordinates of receiver surface
Param:	ng int 1<= 101		number of receivers (uniform dist along surf)
Param:	krecord int 0<= 1	integer index of receiver surface
Param:	nt int 1<= 251		number of time samples
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	ft float - 0.0		first time sample (sec)
Param:	inter int 0<=1 1	interpolation flag: 0=linear, 1=parabolic
Param:	reftrans int 0<=1 0	if 1, use complex refl/trans coefficients
Param:	nameref int -1<= -1	interface to reflect rays from
LDesc:	If >0, only rays reflecting from interface `nameref' to interface
LDesc:	`krecord' are recorded.
LDesc:	If 0, only direct rays to interface `krecord' are recorded.
LDesc:	If -1, all rays reaching interface `krecord' are recorded.
Param:	lscale int - -		if defined, restricts range of extrapolation
Param:	fpeak float 0<= 0.1/dt	peak frequency of Ricker wavelet (Hz)

[gbbeam]

Cat:	Synthetics
Desc:	generate gaussian beam synthetic seismograms for a sloth model
Port:	stdin bin r req		input rayends file
Port:	stdout su w req		synthetic traces
Port:	infofile text w -	file storing useful information
Port:	par par r -		optional par file input
Param:	xg float-list - req	x coordinates of receiver surface
Param:	zg float-list - req	z coordinates of receiver surface
Param:	ng int 1<= 101		number of receivers (uniform dist along surf)
Param:	krecord int 0<= 1	integer index of receiver surface
Param:	amp float-list - 1.0	array of amplitudes
Param:	ang float-list - 0.0	array of angles corresponding to amplitudes
Param:	bw float 0<= 0.0	beamwidth at peak frequency
Param:	nt int 1<= 251		number of time samples
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	ft float - 0.0		first time sample (sec)
Param:	reftrans int 0<=1 0	if 1, use complex refl/trans coefficients
Param:	prim int 0<=1 0		if 0, direct hits; if 1, single-reflected rays
Param:	atten int 0<=2 0	attenuation: 0=none, 1=noncausal, 2=causal
Param:	lscale int - -		if defined, restricts range of extrapolation
Param:	aperture float - -	maximum angle of receiver aperture
Param:	fpeak float 0<= 0.1/dt	peak frequency of Ricker wavelet (Hz)

[kaperture]

Cat:	Synthetics
Desc:	generate the K domain of a line scatterer for a seismic array
Port:	stdout su w req		synthetic trace output
Port:	par par r -		optional par file input
Port:	outpar par w /dev/tty	output par file
LDesc:	The output par file contains xmin, xmax, ymin, ymax, and npairs.
Param:	x0 float - 1000		point scatterer X location
Param:	z0 float - 1000		point scatterer Z location
Param:	nshot int 1<= 1		number of shots
Param:	sxmin float - 0		first shot X location
Param:	szmin float - 0		first shot Z location
Param:	dsx float - 100		X-steps in shot location
Param:	dsz float - 0		Z-steps in shot location
Param:	ngeo int 1<= 1		number of receivers
Param:	gxmin float - 0		first receiver X location
Param:	gzmin float - 0		first receiver Z location
Param:	dgx float - 100		X-steps in receiver location
Param:	dgz float - 0		Z=steps in receiver location
Param:	fnyq float 0<= 125	nyquist frequency (Hz)
Param:	fmax float 0<= 125	maximum frequency (Hz)
Param:	fmin float 0<= 5	minimum frequency (Hz)
Param:	nfreq int 1<= 2		number of frequencies
Param:	both int 0<=1 0		if 1, give negative frequencies too
Param:	nstep int 0<= 60	number of points on nyquist circle
Param:	c float 0< 5000		velocity of medium

[normray]

Cat:	Synthetics
Desc:	dynamic ray tracing for normal incidence rays in a sloth model
Port:	stdin bin r req		input sloth model
Port:	stdout bin w req	output rayends
Port:	par par r -		optional par file input
LDesc:	Because of the complexity of the input parameters, an input par file
LDesc:	is recommended.
Port:	rayfile bin w -		ray-edge intersections (x,z points)
Port:	wavefile bin w -	ray (x,z) points uniformly sampled in time
Port:	infofile text w -	file storing useful information
Port:	fresnelfile bin w fresnelfile.bin used to plot fresnel volumes
Port:	outparfile par w outpar	output parameters for plotting software
Param:	caustic int 0<=1 0	caustic rays flag
LDesc:	If 0, show all rays.  If 1, show only caustic rays.
Param:	nonsurface int 0<=1 0	nonsurface rays flag
LDesc:	If 0, show rays which reach surface.  If 1, show all rays.
Param:	surface int 0<=1 0	surface shooting flag
LDesc:	If 0, shoot rays from subsurface.  If 1, shoot from surface.
Param:	nrays int 1<= -		number of locations to shoot rays from
Param:	dangle float - -	increment of ray angle for one location
Param:	nangle int 1<= -	number of rays shot from one location
Param:	ashift float - -	shift first taking off angle
Param:	xs1 float - -		x of shooting location
Param:	zs1 float - -		z of shooting location
Param:	nangle int 1<= 101	number of takeoff angles
Param:	fangle float - -45	first takeoff angle (degrees)
Param:	nxz int 1<= 101		number of (x,z) pairs in optional rayfile
Param:	nt int 1<= 101		number of (x,z) pairs in optional wavefile
Param:	krecord int - -		save only rays with index krecord
Param:	prim int 0<=1 0		if 0, direct hits; if 1, single-reflected rays
Param:	ffreq float - -1	fresnel volume frequency
Param:	dup-refseq int-list - 0,0	ray sequence to follow
LDesc:	the ray sequence may include reflection (1), transmission (0),
LDesc:	or ray stop (-1).

[rayt2d]

Cat:	Synthetics
Desc:	calculate traveltime tables by 2D paraxial ray tracing
Port:	vfile bin r req		file containing velocity v[nx][nz]
Port:	tfile bin w req		file containing traveltimes t[nxs][nxo][nzo]
Port:	pvfile bin r pvfile	velocity variations pv[nxo][nzo]
Port:	tvfile bin w tvfile	traveltime variations tv[nxs][nxo][nzo]
Port:	csfile bin w csfile	cosine tables cs[nxs][nxo][nzo]
Port:	par par r -		optional par file input
Param:	dt float 0< 0.008	time sample interval in ray tracing (sec)
Param:	nt int 1<= 401		number of time samples in ray tracing
Param:	fz float - 0		first depth sample in velocity
Param:	nz int 1<= 101		number of depth samples in velocity
Param:	dz float 0< 100		depth interval in velocity
Param:	fx float - 0		first lateral sample in velocity
Param:	nx int 1<= 101		number of lateral samples in velocity
Param:	dx float 0< 100		lateral interval in velocity
Param:	fzo float - fz		first depth sample in traveltime table
Param:	nzo int 1<= nz		number of depth samples in traveltime table
Param:	dzo float 0< dz		depth interval in traveltime table
Param:	fxo float - fx		first lateral sample in traveltime table
Param:	nxo int 1<= nx		number of lateral samples in traveltime table
Param:	dxo float 0< dx		lateral interval in traveltime table
Param:	surf string - 0,0;99999,0 recording surface `x1,z1;x2,z2;x3,z3;...'
Param:	fxs float - fx		x-coordinate of first source
Param:	nxs int 1<= 1		number of sources
Param:	dxs float 0< 2*dxo	x-coordinate increment of sources
Param:	aperx float - nx*dx/2	ray tracing aperture in x-direction
Param:	fa float - -60		first take-off angle of rays (degrees)
Param:	na int 1<= 61		number of rays
Param:	da float - 2		increment of take-off angle (degrees)
Param:	amin float - 0		minimum emergence angle (degrees)
Param:	amax float - 90		maximum emergence angle (degrees)
Param:	fac float - 0.01	factor to detemine radius for extrapolation
Param:	ek int 0<=1 1		if 1, implement eikonal in shadow zones
Param:	ms int 1<= 10		report information every ms sources
Param:	restart string - n	job is restarted (y = yes, n = no)
Param:	npv int 0<=1 0		if 1, specify pvfile, tvfile, csfile

[suaddevent]

Cat:	Synthetics
Desc:	add a linear or hyperbolic moveout event to seismic data 
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	type enum-nmo - nmo	hyperbolic or linear event
Param:	t0 float 0<= 1		zero-offset intercept time (seconds)
Param:	vel float 0< 3000	moveout velocity
Param:	amp float - 1		amplitude
Param:	dt float 0<= "from header" sample rate (seconds)

[enum-nmo]
Desc:	moveout event choice
nmo	hyperbolic event
lmo	linear event

[suaddnoise]

Cat:	Synthetics
Desc:	add noise to traces
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	sn float 0<= 20		signal to noise ratio
Param:	noise enum-noise - gauss	noise probability distribution
Param:	seed int 1<= "from clock" random number seed
Param:	f float-list 0<= -	array of filter frequencies (Hz)
Param:	amps float-list - -	array of filter amplitudes
Param:	dt float 0< "from header"	time sampling interval (sec)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[enum-noise]
Desc:	probability distribution choice
gauss	gaussian distribution
flat	uniform distribution

[suaddstatics]

Cat:	Synthetics
Desc:	add random statics on seismic data
Port:	datafile su r req 	trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	shift float 0<= req	random static from -shift to +shift (ms)
Param:	sources int 1<= req	number of source locations
Param:	receivers int 1<= req	number of receiver locations
Param:	cmps int 1<= req	number of common midpoint locations
Param:	maxfold int 1<= req	maximum fold of input data
Param:	dt float 0< "from header"	time sampling interval (ms)
Param:	seed int - "process ID"	seed for random number generator
Param:	verbose int 0<=1 0	if 1, report diagnostics

[sufdmod2]

Cat:	Synthetics
Desc:	finite-difference modeling (2nd order) for acoustic wave equation
Port:	stdin bin r req		input velocity[nx][nz]
Port:	dfile bin r -		input density[nx][nz]
Port:	stdout su w req		output waves[nx][nz] for time steps
Port:	vsfile su w -		output vertical line of seismograms[nz][nt]
Port:	hsfile su w -		output horizontal line of seismograms[nx][nt]
Port:	ssfile su w -		output source point seismograms[nt]
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of x samples (slow dimension)
Param:	nz int 1<= req		number of z samples (fast dimension)
Param:	xs float-list - req	array of x coordinates of source
Param:	zs float-list - req	array of z coordinates of source
Param:	tmax float 0<= req	maximum time (sec)
Param:	nt int 1<= 1+tmax/dt	number of time samples (dt chosen for stability)
Param:	mt int 1<= 1		number of time steps (dt) per output time step
Param:	dx float 0< 1.0		x sampling interval
Param:	fx float - 0.0		first x sample
Param:	dz float 0< 1.0		z sampling interval
Param:	fz float - 0.0		first z sample
Param:	fmax float 0<= vmin/(10*h) maximum frequency in source wavelet (Hz)
Param:	fpeak float 0<= fmax/2	peak frequency in Ricker wavelet (Hz)
Param:	vsx float - -		x coordinate of vertical line of seismograms
Param:	hsz float - -		z coordinate of horizontal line of seismograms
Param:	verbose int 0<=2 0	two levels of diagnostic messages
Param:	abs int-list 0<=1 1,1,1,1 absorbing boundary conditions
LDesc:	flag to enable absorbing boundary conditions on
LDesc:	top, left, bottom, right sides of model

[sufdmod2_pml]

Cat:	Synthetics
Desc:	FD acoustic modeling (2nd order) with PML boundary conditions
Port:	stdin bin r req		input velocity[nx][nz] function
Port:	dfile bin r -		input density[nx][nz]
Port:	stdout su w req		output waves[nx][nz] for time steps
Port:	vsfile su w - 		output vertical line of seismograms[nz][nt] 
Port:	hsfile su w - 		output horizontal line of seismograms[nx][nt]
Port:	ssfile su w - 		output source point seismograms[nt]
Port:	par par r -		optional par file input
Param:	nx int 0< req		number of x samples (2nd dimension)
Param:  nz int 0< req		number of z samples (1st dimension)
Param:	xs float-list - req	array of x coordinates of source
Param:	zs float-list - req	array of z coordinates of source
Param:	tmax float 0<= req	maximum time (sec)
Param:	nt int 0< 1+tmax/dt	number of time samples
Param:	mt int 0< 1		number of time steps (dt) per output time step
Param:	dx float - 1.		x sampling interval
Param:	fx float - 0.		first x sample
Param:	dz float - 1.		z sampling interval
Param:	fz float - 0.		first z sample
Param:	fmax float - vmin/(10.*h) maximum frequency in source wavelet (Hz)
Param:	fpeak float 0<= .5*fmax	peak frequency in Ricker wavelet (Hz)
Param:	vsx float - -		x coordinate of vertical line of seismograms
Param:	hsz float - -		z coordinate of horizontal line of seismograms
Param:	verbose int 0<=2 0	two levels of diagnostic messages
Param:	abs int-list 0<=1 1,1,1,1 absorbing boundary conditions
LDesc:	flag to enable absorbing boundary conditions on
LDesc:	top, left, bottom, right sides of model
Param:	pml_max float - 1000.0	PML absorption parameter
Param:	pml_thick int 0<= 0	half-thickness of PML layer (0 = PML disabled)

[sugoupillaud]

Cat:	Synthetics
Desc:	calculate 1D impulse response of elastic Goupillaud medium
Port:	stdin su r req		input reflectivity series as an SU trace
Port:	stdout su w req		output synthetic trace
Port:	par par r -		optional par file input
Param:	l int 1<= 1		source layer number
LDesc:	Source is located at the top of layer l.
Param:	k int 1<= 1		receiver layer number
LDesc:	Receiver is located at the top of layer k.
Param:	tmax int 1<= -		number of samples in output trace
LDef:	2*tr.ns-1-(l-1)-(k-1)
Param:	pV int -1<=1 1		pressure/velocity flag
LDesc:	If 1, output displacement field.  If 0, output pressure field.
Param:	verbose int 0<=1 0	if 1, report warnings

[suimp2d]

Cat:	Synthetics
Desc:	generate shot records for line scatterer in 3D using Born integral eqn
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nshot int 1<= 1		number of shots
Param:	nrec int 1<= 1		number of receivers
Param:	c float 0< 5000		velocity of medium
Param:	dt float 0< 0.004	sampling rate (sec)
Param:	nt int 1<= 256		number of samples
Param:	x0 float - 1000		point scatterer x location
Param:	z0 float - 1000		point scatterer z location
Param:	sxmin float - 0		first shot x location
Param:	szmin float - 0		first shot z location
Param:	gxmin float - 0		first receiver x location
Param:	gzmin float - 0		first receiver z location
Param:	dsx float - 100		x-step in shot location
Param:	dsz float - 0		z-step in shot location
Param:	dgx float - 100		x-step in receiver location
Param:	dgz float - 0		z-step in receiver location

[suimp3d]

Cat:	Synthetics
Desc:	generate shot records for point scatterer in 3D using Born integral eqn
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nshot int 1<= 1		number of shots
Param:	nrec int 1<= 1		number of receivers
Param:	c float 0< 5000		velocity of medium
Param:	dt float 0< 0.004	sampling rate (sec)
Param:	nt int 1<= 256		number of samples
Param:	x0 float - 1000		point scatterer x location
Param:	y0 float - 0		point scatterer y location
Param:	z0 float - 1000		point scatterer z location
Param:	sxmin float - 0		first shot x location
Param:	symin float - 0		first shot y location
Param:	szmin float - 0		first shot z location
Param:	gxmin float - 0		first receiver x location
Param:	gymin float - 0		first receiver y location
Param:	gzmin float - 0		first receiver z location
Param:	dsx float - 100		x-step in shot location
Param:	dsy float - 0		y-step in shot location
Param:	dsz float - 0		z-step in shot location
Param:	dgx float - 100		x-step in receiver location
Param:	dgy float - 0		y-step in receiver location
Param:	dgz float - 0		z-step in receiver location

[sukdsyn2d]

Cat:	Synthetics
Desc:	Kirchhoff depth synthesis of 2D seismic data from migrated section
Port:	stdin su r req		input migrated section
Port:	ttfile bin r req	input traveltime tables
Port:	stdout su w req		output seismic traces
Port:	jpfile text w stderr	job print filename
Port:	par par r -		optional par file input
Param:	fzt float - req		first traveltime depth sample
Param:	nzt int 1<= req		number of traveltime depth samples
Param:	dzt float - req		traveltime depth interval
Param:	fxt float - req		first traveltime lateral sample
Param:	nxt int 1<= req		number of traveltime lateral samples
Param:	dxt float - req		traveltime lateral interval
Param:	fs float - req		x-coordinate of first source
Param:	ns int 1<= req		number of sources
Param:	ds float - req		x-coordinate increment of sources
Param:	fz float - req		first z-coordinate in migrated section
Param:	dz float - req		vertical spacing of migrated section
Param:	nz int 1<= req		number of depth points in migrated section
Param:	fx float - req		first x-coordinate in migrated section
Param:	dx float - req		horizontal spacing of migrated section
Param:	nx int 1<= req		number of lateral points in migrated section
Param:	nt int 1<= 501		number of time samples
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	nxo int 1<= 1		number of source-receiver offsets
Param:	dxo float - 25		offset sampling interval
Param:	fxo float - 0		first offset
Param:	nxs int 1<= 1		number of shotpoints
Param:	dxs float - 25		shotpoint sampling interval
Param:	fxs float - 0		first shotpoint
Param:	fmax float 0<= 1/(4*dt) maximum frequency in migrated section (Hz)
Param:	aperx float - nxt*dxt/2	modeling lateral aperture
Param:	angmax float - 60	modeling angle aperture from vertical (degrees)
Param:	v0 float 0< 1500	reference velocity at surface
Param:	dvz float - 0.0		reference velocity vertical gradient
Param:	ls int 0<=1 1		line source flag
Param:	mtr int 1<= 100		print information every mtr traces

[sunull]

Cat:	Synthetics
Desc:	create null (all zeros) traces
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nt int 1<= req		number of samples per trace
Param:	ntr int 1<= 5		number of null traces to create
Param:	dt float 0< 0.004	time sampling interval (sec)

[suplane]

Cat:	Synthetics
Desc:	create common offset data set with up to 3 planar reflectors
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	npl int 1<=3 3		number of planes
Param:	nt int 1<= 64		number of time samples
Param:	ntr int 1<= 32		number of traces
Param:	taper int 0<=1 0	if 1, taper planes to zero at end of offset
Param:	offset float 0<= 400	common offset
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	dip1 float - 0		plane 1 dip (ms/trace)
Param:	len1 int 1<= 3*ntr/4	plane 1 horizontal extent (traces)
Param:	ct1 int 0<= nt/2	plane 1 center pivot time (sample number)
Param:	cx1 int 0<= ntr/2	plane 1 center pivot trace
Param:	dip2 float - 4		plane 2 dip (ms/trace)
Param:	len2 int 1<= 3*ntr/4	plane 2 horizontal extent (traces)
Param:	ct2 int 0<= nt/2	plane 2 center pivot time (sample number)
Param:	cx2 int 0<= ntr/2	plane 2 center pivot trace
Param:	dip3 float - 8		plane 3 dip (ms/trace)
Param:	len3 int 1<= 3*ntr/4	plane 3 horizontal extent (traces)
Param:	ct3 int 0<= nt/2	plane 3 center pivot time (sample number)
Param:	cx3 int 0<= ntr/2	plane 3 center pivot trace
Param:	liner int 0<=1 0	parameter use flag
LDesc:  If 0, use given parameters.  If 1, set parameters for a 64x64
LDesc:	data set with separated dipping layers.

[sureflpsvsh]

Cat:	Synthetics
Desc:	reflectivity modeling of PSV or SH waves for layered earth
Port:	wfp su w -		output pressure seismogram
Port:	wfr su w -		output radial component seismogram
Port:	wfz su w -		output vertical component seismogram
Port:	wft su w -		output tangential component seismogram
Port:	outf text w info	output processing info file (verbose=2,3)
Port:	lobsfile bin - -	file containing lobs array
Port:	clfile bin r -		file containing cl array
Port:	ctfile bin r -		file containing ct array
Port:	qlfile bin r -		file containing ql array
Port:	qtfile bin r -		file containing qt array
Port:	rhofile bin r -		file containing rho array
Port:	tfile bin r -		file containing t array
Port:	par par r -		optional par file input

Param:	m0 float - req		seismic moment
Param:	p2w float - req		maximum ray parameter to compute
Param:	wtype int 1<=2 1	wave type: 1=PSV, 2=SH
Param:	stype int 1<=2 1	source flag
LDesc:	If 1, moment tensor components are given.
LDesc:	If 2, moment tensor is computed from fault plane mechanism parameters.
Param:	wfield int 1<=3 2	particle motion: 1=displ, 2=vel, 3=accel
LDesc:	If 1, output particle displacement.
LDesc:	If 2, output particle velocity.
LDesc:	If 3, output particle acceleration.
Param:	flt int 0<=1 0		if 1, apply earth flattening correction
Param:	vsp int 0<=1 0		VSP flag: 0 = surface data, 1 = VSP data
Param:	int_type int 1<=2 1	slowness integration: 1=trapezoid, 2=Filon
LDesc:	If 1, compute slowness integration using trapezoidal rule.
LDesc:	If 2, use a first order Filon scheme (faster but noisier).
Param:	verbose int 0<=3 0	verbosity level
LDesc:	If 1, report information to screen.  If 2, report information to
LDesc:	file.  If 3, report to both screen and file.

Param:	rand int 0<=1 0		if 1, include random velocity and q layers
Param:	qopt int 0<=1 0		if 1, apply a q-correction
Param:	win int 0<=1 0		if 1, apply Hanning window in frequency domain
Param:	wavelet_type int 1<=3 -	type of wavelet: 1=spike, 2=ricker, 3=akb

Param:	tsec float 0< 2.048	length of computed traces (sec)
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	nt int 1<= tsec/dt	number of samples per trace
Param:	nx int 1<= 60		number of traces per shot
Param:	nw int 1<= 100		number of frequencies to process
Param:	nor int 1<= 1		number of receiver depth levels (for VSP)
Param:	nlayers int 1<= 10	number of horizontal layers in the model
Param:	fref float 0< 1.0	reference frequency (Hz)
Param:	bx float - 0.0		first trace offset (km)
Param:	dx float - 0.05		offset increment (km)
Param:	pw1 float 0<= 0.0	Hanning window low cut (Hz)
Param:	pw2 float 0<= 0.0	Hanning window low pass (Hz)
Param:	pw3 float 0<= 0.0	Hanning window high pass (Hz)
Param:	pw4 float 0<= 0.0	Hanning window high cut (Hz)
Param:	fs float 0<= 0.07	Filon sampling parameter
Param:	np int 1<= 1300		number of ray parameters to compute
Param:	bp float 0<= 0.0	slowest ray parameter to compute
Param:	decay float - 50	decay factor at time series wraparound point
Param:	lobs float-list - -	array of layers receivers are on top of
Param:	cl float-list - -	array of compressional velocities (km/s)
Param:	ct float-list - -	array of shear velocities (km/s)
Param:	ql float-list - -	array of compressional Q values
Param:	qt float-list - -	array of shear Q values
Param:	rho float-list - -	array of densities
Param:	t float-list - -	array of layer thicknesses (km)

Param:	lsource int 1<= 1	layer on top of which source is located
Param:	h1 float - 1.0		vertical linear part of source
Param:	h2 float - 0.0		horizontal linear part of source
Param:	m1 float - -		moment tensor (1,1) component
Param:	m2 float - -		moment tensor (1,2) component
Param:	m3 float - -		moment tensor (2,2) component
Param:	delta float - 0.0	fault plane dip (degrees)
Param:	lambda float - 0.0	fault motion rake (degrees)
Param:	phis float - 0.0	fault plane azimuth (degrees)
Param:	phi float - 0.0		azimuth of receiver location (degrees)

Param:	tlag float 0<= 0.0	time lag to apply to seismograms (sec)
Param:	nf float 1<= nw		number of frequencies in output data
Param:	fpeak float 0<= 25.0	peak frequency for Ricker or akb wavelet (Hz)
Param:	red_vel float 0<= 0.0	reducing velocity (km/s)
LDesc:	if 0, reducing velocity is set to maximum compressional velocity
Param:	w1 float 0<= "15% of fmax" apply Hanning window to frequencies below w1
Param:	w2 float 0<= "85% of fmax" apply Hanning window to frequencies above w2
Param:	nfilters int 0<= 0	number of filters to apply to synthetics
Param:	filters_phase int-list 0<=1 - array of phase choices: 0=zero, 1=min
Param:	filters_type int-list 1<=3 - array of filter types
LDesc:	if 1, high-cut filter; if 2, low-cut filter; if 3, notch filter
Param:	dbpo float-list - -	array of filter slopes (db/octave)
Param:	f1 float-list - -	array of filter start frequencies (Hz)
Param:	f2 float-list - -	array of filter stop frequencies (Hz)
Param:	filphasefile string - -	file containing filters_phase array
Param:	filtypefile string - -	file containing filters_type array
Param:	dbpofile string - -	file containing dbpo array
Param:	f1file string - -	file containing f1 array
Param:	f2file string - -	file containing f2 array

Param:	nlint int 0<= 0		number of times layer interp is required
Param:	nintlayers int-list - - array of number of layers to interpolate
Param:	intlayers int-list - -	array of layers on which to start interpolation
Param:	intlayth float-list - -	array of layer thicknesses to interpolate
Param:	nintlayfile string - -	file containing nintlayers
Param:	intlayfile string - -	file containing intlayers
Param:	intlaythfile string - -	file containing intlayth

Param:	layer int 0<= 0		layer on which to insert random velocity layers
Param:	zlayer float 0<= 0.0	thickness of random layers
Param:	sdcl float 0<= 0.0	standard deviation for compressional velocities
Param:	sdct float 0<= 0.0	standard deviation for shear velocities
Param:	layern int 0<= 0	layer on which the q-option is invoked
Param:	wrefp float 0<= 1.0	reference frequency for compressional vels
Param:	wrefs float 0<= 1.0	reference frequency for shear vels
Param:	epsp float - 0.001	reference amplitude for compressional vels
Param:	epss float - 0.001	reference amplitude for shear vels
Param:	sigp float - 0.1	xxxxxx for compressional vels
Param:	sigs float - 0.1	xxxxxx for shear vels

[suspike]

Cat:	Synthetics
Desc:	make a small spike data set
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nt int 1<= 64		number of time samples
Param:	ntr int 1<= 32		number of traces
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	offset float 0<= 400	common offset
Param:	nspk int 1<=4 4		number of spikes
Param:	ix1 int 0<= ntr/4	spike 1 trace number
Param:	it1 int 0<= nt/4	spike 1 time sample
Param:	ix2 int 0<= ntr/4	spike 2 trace number
Param:	it2 int 0<= 3*nt/4	spike 2 time sample
Param:	ix3 int 0<= 3*ntr/4	spike 3 trace number
Param:	it3 int 0<= nt/4	spike 3 time sample
Param:	ix4 int 0<= 3*ntr/4	spike 4 trace number
Param:	it4 int 0<= 3*nt/4	spike 4 time sample

[susyncz]

Cat:	Synthetics
Desc:	create synthetic seismogram from piecewise constant v(z) function
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	ninf int 1<= 4		number of interfaces, not including top surface
Param:	dip float-list - -	dips of interfaces (degrees)
LDef:	5,10,15,20,... degrees
Param:	zint float-list - -	depths of interfaces at x=0
LDef:	100,200,300,400,... meters
Param:	v float-list 0< -	velocities below surface + interfaces
LDef:	1500,2000,2500,3000,3500,... m/s
Param:	rho float-list 0< -	densities below surface + interfaces
LDef:	1,1,1,1,1,...
Param:	nline int 1<= 1		number of (identical) lines
Param:	ntr int 1<= 32		number of traces
Param:	dx float - 10		trace interval
Param:	tdelay float 0<= 0	delay in recording time after source initiation
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	nt int 1<= 128		number of time samples

[susynlv]

Cat:	Synthetics
Desc:	create synthetic seismogram from linear velocity function
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nt int 1<= 101		number of time samples
Param:	dt float 0< 0.04	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	nxo int 1<= 1		number of source-receiver offsets
Param:	dxo float - 0.05	offset sampling interval (km)			
Param:	fxo float - 0.0		first offset (km)
Param:	xo float-list - fxo,fxo+dxo,... array of offsets (km)
Param:	nxm int 1<= 101		number of midpoints
Param:	dxm float - 0.05	midpoint sampling interval (km)
Param:	fxm float - 0.0		first midpoint (km)
Param:	nxs int 1<= 101		number of shotpoints
Param:	dxs float - 0.05	shotpoint sampling interval (km)
Param:	fxs float - 0.0		first shotpoint (km)
Param:	x0 float - 0.0		distance x at which v00 is specified
Param:	z0 float - 0.0		depth z at which v00 is specified
Param:	v00 float 0< 2.0	velocity at x0,z0 (km/sec)
Param:	dvdx float - 0.0	derivative of velocity with distance x
Param:	dvdz float - 0.0	derivative of velocity with depth z
Param:	fpeak float 0<= 0.2/dt	peak frequency of symmetric ricker wavelet (Hz)
Param:	dup-ref string - 1:1,2;4,2 reflector(s):  `amp:x1,z1;x2,z2;x3,z3;...'
Param:	smooth int 0<=1 0	if 1, smooth reflectors with cubic spline
Param:	er int 0<=1 0		if 1, use exploding reflector amplitudes
Param:	ls int 0<=1 0		if 1, use line source; if 0, use point source
Param:	ob int 0<=1 1		if 1, include obliquity factors
Param:	tmin float 0<= 10.0*dt	minimum time of interest (sec)
Param:	ndpfz int 1<= 5		number of diffractors per Fresnel zone
Param:	verbose int 0<=1 0	if 1, print useful information

[susynlvcw]

Cat:	Synthetics
Desc:	create synthetic seismogram, linear velocity, for mode converted waves
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	nt int 1<= 101		number of time samples
Param:	dt float 0< 0.04	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	nxo int 1<= 1		number of source-receiver offsets
Param:	dxo float - 0.05	offset sampling interval (km)			
Param:	fxo float - 0.0		first offset (km)
Param:	xo float-list - fxo,fxo+dxo,... array of offsets (km)
Param:	nxm int 1<= 101		number of midpoints
Param:	dxm float - 0.05	midpoint sampling interval (km)
Param:	fxm float - 0.0		first midpoint (km)
Param:	nxs int 1<= 101		number of shotpoints
Param:	dxs float - 0.05	shotpoint sampling interval (km)
Param:	fxs float - 0.0		first shotpoint (km)
Param:	x0 float - 0.0		distance x at which v00 is specified
Param:	z0 float - 0.0		depth z at which v00 is specified
Param:	v00 float 0< 2.0	velocity at x0,z0 (km/sec)
Param:	gamma float 0< 1.0	velocity ratio, upgoing/downgoing
Param:	dvdx float - 0.0	derivative of velocity with distance x
Param:	dvdz float - 0.0	derivative of velocity with depth z
Param:	fpeak float 0<= 0.2/dt	peak frequency of symmetric ricker wavelet (Hz)
Param:	dup-ref string - 1:1,2;4,2 reflector(s):  `amp:x1,z1;x2,z2;x3,z3;...'
Param:	smooth int 0<=1 0	if 1, smooth reflectors with cubic spline
Param:	er int 0<=1 0		if 1, use exploding reflector amplitudes
Param:	ls int 0<=1 0		if 1, use line source; if 0, use point source
Param:	ob int 0<=1 1		if 1, include obliquity factors
Param:	sp int 0<=1 1		if 1, account for amplitude spreading
Param:	tmin float 0<= 10.0*dt	minimum time of interest (sec)
Param:	ndpfz int 1<= 5		number of diffractors per Fresnel zone
Param:	verbose int 0<=1 0	if 1, print useful information

[susynvxz]

Cat:	Synthetics
Desc:	create common offset synthetic from v(x,z) via Kirchhoff-style modeling
Port:	stdin bin r req		input velocity grid v[nx][nz]
Port:	stdout su w req		common offset trace output
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of velocity x samples (slow dimension)
Param:	nz int 1<= req		number of velocity z samples (fast dimension)
Param:	nxb int 0<= nx		band centered at midpoint
Param:	nxd int 0<= 1		skipped number of midpoints
Param:	dx float 0< 100		x sampling interval (m)
Param:	fx float - 0.0		first x sample
Param:	dz float 0< 100		z sampling interval (m)
Param:	fz float - 0.0		first z sample
Param:	nt int 1<= 101		number of time samples
Param:	dt float 0< 0.04	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	nxo int 1<= 1		number of source-receiver offsets
Param:	dxo float - 50		offset sampling interval (m)			
Param:	fxo float - 0.0		first offset (m)
Param:	nxm int 1<= 101		number of midpoints
Param:	dxm float - 50		midpoint sampling interval (m)
Param:	fxm float - 0.0		first midpoint (m)
Param:	fpeak float 0<= 0.2/dt	peak frequency of symmetric Ricker wavelet (Hz)
Param:	dup-ref string - 1:1,2;4,2  reflector(s): `amp:x1,z1;x2,z2;x3,z3;...'
Param:	smooth int 0<=1 0	if 1, smooth reflectors with cubic spline
Param:	ls int 0<=1 0		if 1, use line source; if 0, use point source
Param:	tmin float 0<= 10.0*dt	minimum time of interest (sec)
Param:	ndpfz int 1<= 5		number of diffractors per Fresnel zone
Param:	verbose int 0<=1 0	if 1, print useful information

[susynvxzcs]

Cat:	Synthetics
Desc:	create common shot synthetics from v(x,z) via Kirchhoff-style modeling
Port:	stdin bin r req		input velocity grid v[nx][nz]
Port:	vpfile bin r -		slowness perturbation array v_p[nx][nz]
Port:	stdout su w req		common shot trace output
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of velocity x samples (slow dimension)
Param:	nz int 1<= req		number of velocity z samples (fast dimension)
Param:	nt int 1<= 501		number of time samples
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	nxg int 1<= 1		number of receivers
Param:	dxg float - 15		receiver sampling interval (m)			
Param:	fxg float - 0.0		first receiver (m)
Param:	nxd int 0<= 5		skipped number of receivers
Param:	nxs int 1<= 1		number of shots
Param:	dxs float - 50		shot sampling interval (m)			
Param:	fxs float - 0.0		first shot (m)
Param:	dx float 0< 50		x sampling interval (m)
Param:	fx float - 0.0		first x sample (m)
Param:	dz float 0< 50		z sampling interval (m)
Param:	nxb int 1<= nx/2	band width centered at midpoint
Param:	nxc int 0<= 0		horizontal range in which velocity is changed
Param:	nzc int 0<= 0		vertical range in which velocity is changed
Param:	pert int 0<=1 0		if 1, calculate time correction from v_p
Param:	fpeak float 0<= 0.2/dt	peak frequency of symmetric ricker wavelet (Hz)
Param:	dup-ref string - 1:1,2;4,2  reflector(s): `amp:x1,z1;x2,z2;x3,z3;...'
Param:	smooth int 0<=1 0	if 1, smooth reflectors with cubic spline
Param:	ls int 0<=1 0		if 1, use line source; if 0, use point source
Param:	tmin float 0<= 10.0*dt	minimum time of interest (sec)
Param:	ndpfz int 1<= 5		number of diffractors per Fresnel zone
Param:	verbose int 0<=1 0	if 1, print useful information

[triseis]

Cat:	Synthetics
Desc:	Gaussian beam synthetic seismograms from a sloth model
Port:	stdin tri r req		triangulated sloth (1/v^2) model
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	xs float-list - req	x coordinates of source surface s
Param:	zs float-list - req	z coordinates of source surface s
Param:	xg float-list - req	x coordinates of receiver surface g
Param:	zg float-list - req	z coordinates of receiver surface g
Param:	ns int 1<= 1		number of sources distributed along surface s
Param:	ds float - -		increment between source locations
Param:	fs float - 0.0		first source location (relative to start of s)
Param:	ng int 1<= 101		number of receivers distributed along surface g
Param:	dg float - -		increment between receiver locations
Param:	fg float - 0.0		first rcvr location (relative to start of g)
Param:	dgds float - 0.0	change in rcvr location w.r.t. source location
Param:	krecord int 1<= 1	index of receiver surface
Param:	kreflect int -1<= -1	index of reflecting surface in sloth model
Param:	prim int 0<=1 0		if 0, direct hits; if 1, single-reflected rays
Param:	bw float 0<= 0		beam width at peak frequency
Param:	nt int 1<= 251		number of time samples
Param:	dt float 0< 0.004	time sampling interval (sec)
Param:	ft float - 0.0		first time sample (sec)
Param:	nangle int 1<= 101	number of takeoff angles
Param:	fangle float - -45	first takeoff angle (degrees)
Param:	langle float - 45	last takeoff angle (degrees)
Param:	reftrans int 0<=1 0	if 1, use complex refl/trans coefficients
Param:	atten int 0<=2 0	attenuation: 0=none, 1=noncausal, 2=causal
Param:	lscale int - -		if defined, restricts range of extrapolation
Param:	fpeak float 0<= 0.1/dt	peak frequency of Ricker wavelet (Hz)
Param:	aperture float - -	maximum angle of receiver aperture

[xy2z]

Cat:	Synthetics
Desc:	convert (x,y) pairs to spike z values on a uniform x,y grid
Port:	stdin bin r req		input (x,y) pairs
Port:	stdout bin w req	uniformly sampled float array, nx1 by nx2
Port:	par par r -		optional par file input
Param:	npairs int 1<= req	number of pairs to input
Param:	scale float - 1.0	spike amplitude
Param:	nx1 int 1<= 100		number of samples in 1st (fast) dimension
Param:	nx2 int 1<= 100		number of samples in 2nd (slow) dimension
Param:	x1pad int 0<= 2		amount of zero padding in dimension 1
Param:	x2pad int 0<= 2		amount of zero padding in dimension 2

