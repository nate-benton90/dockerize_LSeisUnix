# anisotropy.spec --
#
#	Tksu anisotropy category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[elacheck]

Cat:	Anisotropy
Cat:	Model Building
Desc:	get elastic coefficients of model (interactive program)
Port:	file bin r req		input model file (from elamodel)

[elamodel]

Cat:	Anisotropy
Cat:	Model Building
Desc:	make piecewise homogeneous 2D anisotropic model
Port:	stdout bin w req	output model file
Port:	par par r -		optional par file input
Param:	xmin float - 0.0	minimum horizontal coordinate (x)
Param:	xmax float - 1.0	maximum horizontal coordinate (x)
Param:	zmin float - 0.0	minimum vertical coordinate (z)
Param:	zmax float - 1.0	maximum vertical coordinate (z)
Param:	maxangle float - 5.0	maximum angle between edge segments (degrees)
Param:	dup-xedge float-list - - x coordinates of an edge
Param:	dup-zedge float-list - - z coordinates of an edge
Param:	kedge int-list - -	array of indices used to identify edges
Param:	dup-fill float-list - req elastic params to fill a closed region with
LDesc:	Each fill line defines the elastic parameters for a closed region.
LDesc:	The region may be isotropic, transversely isotropic or anisotropic.
LDesc:	See help for the format of the fill line.

[elaray]

Cat:	Anisotropy
Cat:	Model Building
Desc:	ray tracing for elastic anisotropic models
Port:	stdin bin r req		input model file (from elamodel)
Port:	stdout bin w req	output ray ends file
Port:	rayfile bin w -		ray x,z coords of ray-edge intersections
Port:	wavefile bin w -	ray x,z coords uniformly sampled in time
Port:	infofile text w -	file storing useful information
Port:	par par r -		optional par file input
Port:	outparfile par w outpar	output parameters for plotting software
Param:	xs float - (max-min)/2	x coordinate of source
LDef:	halfway across the model
Param:	zs float - min		z coordinate of source
LDef:	at top of model
Param:	nangle int 1<= 101	number of takeoff angles
Param:	fangle float - -45	first takeoff angle (degrees)
Param:	langle float - 45	last takeoff angle (degrees)
Param:	nxz int 1<= 101		number of (x,z) points in optional rayfile
Param:	mode int 0<=2 0		type of rays to shoot: 0=P, 1=SV, 2=SH
Param:	prim int 0<=1 1		0 = plot direct rays, 1 = plot reflected rays
Param:	dup-refseq int-list - 1,0,0  ray reflection sequence (see help)
Param:	krecord int 0<= -	index of interface for which to store rays
Param:	f0 float - 1		force impact strength
Param:	fdip float - 0		force dip w.r.t. vertical (degrees)
Param:	fazi float - 0		force azimuth w.r.t. positive x-axis (degrees)
Param:	reftrans int 0<=1 0	if 1, include reflection/transmission coeff
Param:	nt int 1<= -		number of (x,z) points in optional wavefile
Param:	tw float 0< -		traveltime associated with wavefront

[raydata]

Cat:	Anisotropy
Cat:	Model Building
Desc:	display ray data from elaray
Port:	stdin bin r req		input ray ends file
Port:	par par r -		optional par file input
Param:	kend int - -		index of interest
Param:	t int 0<=1 0		if 1, output x_t
Param:	px int 0<=1 0		if 1, output x_px
Param:	pz int 0<=1 0		if 1, output x_pz
Param:	vgx int 0<=1 0		if 1, output x_vgx
Param:	vgz int 0<=1 0		if 1, output x_vgz
Param:	pxvx int 0<=1 0		if 1, output x_pxvx
Param:	pol int 0<=1 0		if 1, output x_polangle
Param:	ascci int 0<=1 0	if 0, binary output; if 1, ascii output

[suea2df]

Cat:	Anisotropy
Cat:	Synthetics
Desc:	anelastic anisotropic 2D finite difference forward modeling
Port:	stdout text w req	diagnostic information
Port:	vsfile su w vsp.su	vertical line of seismograms[nz][nt]
Port:	hsfile su w ss.su	horizontal line of seismograms[nx][nt]
Port:	sofile text w -		ascii source file
Port:	efile bin w -		elastic constants file
Port:	snfile su w -		snapshots file
Port:	par par r -		optional par file input
Param:	nxl int 1<= req		number of locations to define model
Param:	xl float-list - req	model locations in horizontal direction
Param:	dup-zl float-list - req		model depths
Param:	dup-rhol float-list - req	model rho at xl,zl
Param:	dup-vpl float-list - req	model vp at xl,zl
Param:	dup-vsl float-list - req	model vs at xl,zl
Param:	dup-ql float-list - -		model q at xl,zl
Param:	dup-epl float-list - -		anisotropy ep at xl,zl
Param:	dup-dsl float-list - -		anisotropy ds at xl,zl
Param:	dup-anl float-list - -		anisotropy angle at xl,zl (degrees)
Param:	dt float 0< 0.001	time sampling interval (sec)
Param:	ft float - 0.0		first time (sec)
Param:	lt float - 1.0		last time (sec)
Param:	dx float 0< 10.0	x sampling interval
Param:	dz float 0< dx		z sampling interval
Param:	xmin float - -1000	first x coordinate
Param:	xmax float - 1000	last x coordinate
Param:	zmin float - 0		top z coordinate
Param:	zmax float - 1000	bottom z coordinate
Param:	sx float - 0		source x coordinate
Param:	sz float - 500		source z coordinate
Param:	stype string - p	source type
Param:	wtype string - dg	waave type
Param:	ts float 0< 0.05	source length (sec)
Param:	favg float 0< 50	source average frequency
Param:	qsw int 0<=1 0		if 1, include attenuation
Param:	asw int 0<=1 0		if 1, include anisotropy
Param:	msw int 0<=1 0		model type: 0=field, 1=layer
Param:	snaptime float-list - - times of snapshots
Param:	vsx float - -		x coordinate of vertical line of seismograms
Param:	hsz float - -		z coordinate of horizontal line of seismograms
Param:	bc int-list - 10,10,10,10 top, left, bottom, right B.C.'s (see help)
Param:	bc_a float - 0.95	initial taper value for absorbing boundary
Param:	bc_r float - 0.0	exponential factor for absorbing boundary
Param:	tsw int 0<=1 0		shear-stress-only flag (see help)
Param:	verbose int 0<=1 0	if 1, print progress

[sufctanismod]

Cat:	Anisotropy
Cat:	Model Building
Desc:	flux-corrected transport 2D elastic modeling in anisotropic media
Port:	stdout su w req 	trace output in su or bin format
LDesc:	if suhead=1, SU traces (with trace header) are output.
LDesc:	if suhead=0, headerless binary samples are output.
Port:	par par r -		optional par file input
Param:	reflxfile string - -	x-comp reflection seismogram output file
LDesc:	no output file created if reflxfile is not specified
Param:	reflyfile string - -	y-comp reflection seismogram output file
LDesc:	no output file created if reflyfile is not specified
Param:	reflzfile string - -	z-comp reflection seismogram output file
LDesc:	no output file created if reflzfile is not specified
Param:	vspxfile string - -	x-comp VSP seismogram output file
LDesc:	no output file created if vspxfile is not specified
Param:	vspyfile string - -	y-comp VSP seismogram output file
LDesc:	no output file created if vspyfile is not specified
Param:	vspzfile string - -	z-comp VSP seismogram output file
LDesc:	no output file created if vspzfile is not specified
Param:	suhead int 0<=1 1	if 1, SU trace output; if 0, binary output
Param:	receiverdepth int - 0	depth of horizontal receivers (gridpoints)
Param:	dofct int 0<=1 1	=1 apply FCT correction; =0 no FCT correction
Param:	eta0 float - .03	diffusion coefficient
Param:	eta float - .04		anti-diffusion coefficient
Param:	fctxbeg int - 0		x coord to begin applying FCT correction
Param:	fctzbeg int - 0		z coord to begin applying FCT correction
Param:	fctxend int - nx	x coord to stop applying FCT correction
Param:	fctzend int - nz	z coord to stop applying FCT correction
Param:	deta0dx float - 0	gradient of eta0 in x-direction
Param:	deta0dz float - 0	gradient of eta0 in z-direction
Param:	detadx float - 0	gradient of eta in x-direction
Param:	detadz float - 0	gradient of eta in z-direction
Param:	order int 2<=4 2	choice of 2nd or 4th order finite differencing
Param:	nt int 0< 200		number of time steps
Param:	dt float 0< .04		time step interval (sec)
Param:	nx int 0< 100		number of grid points in x-direction
Param:	nz int 0< 100		number of grid points in z-direction
Param:	dx float - .02		spatial step in x-direction
Param:	dz float - .02		spatial step in z-direction
Param:	sx int - nx/2		source x-coordinate (gridpoints)
Param:	sz int - nz/2		source z-coordinate (gridpoints)
Param:	fpeak float - 20	peak frequency of the wavelet (Hz)
Param:	wavelet enum-wavelet - 1	wavelet type
Param:	isurf enum-surface - 1	surface condition type
Param:	source enum-source - 1	source type
Param:	sfile string - -	input source file
Param:	dfile string - -	input density file
Param:	rho00 float - 2.	density at (0,0) if no dfile
Param:	drhodx float - 0.	x-direction density gradient if no dfile
Param:	drhodz float - 0.	z-direction density gradient if no dfile
Param:	afile string - -	input elastic parameter c11 file
Param:	aa00 float - 2.		elastic parameter c11 at (0,0) if no afile
Param:	daadx float - 0.	x-direction elastic gradient if no afile
Param:	daadz float - 0.	z-direction elastic gradient if no afile
Param:	cfile string - -	input elastic parameter c33 file
Param:	cc00 float - 2.		elastic parameter c33 at (0,0) if no cfile
Param:	dccdx float - 0.	x-direction elastic gradient if no cfile
Param:	dccdz float - 0.	z-direction elastic gradient if no cfile
Param:	ffile string - -	input elastic parameter c13 file
Param:	ff00 float - 2.		elastic parameter c13 at (0,0) if no ffile
Param:	dffdx float - 0.	x-direction elastic gradient if no ffile
Param:	dffdz float - 0.	z-direction elastic gradient if no ffile
Param:	lfile string - -	input elastic parameter c44 file
Param:	ll00 float - 2.		elastic parameter c44 at (0,0) if no lfile
Param:	dlldx float - 0.	x-direction elastic gradient if no lfile
Param:	dlldz float - 0.	z-direction elastic gradient if no lfile
Param:	nfile string - -	input elastic parameter c66 file
Param:	nl00 float - 2.		elastic parameter c66 at (0,0) if no nfile
Param:	dnndx float - 0.	x-direction elastic gradient if no nfile
Param:	dnndz float - 0.	z-direction elastic gradient if no nfile
Param:	movebc int 0<=1 0	=1 use moving boundary optimization
Param:	mbx1 int 0- 0		initial moving boundary left side if movebc=1
Param:	mbz1 int 0- 0		initial moving boundary top side if movebc=1
Param:	mbx2 int 0- nx		initial moving boundary right side if movebc=1
Param:	mbz2 int 0- nz		initial moving boundary bottom side if movebc=1

[enum-wavelet]
Desc:	wavelet type
1	AKB wavelet
2	Ricker wavelet
3	impulse
4	unity

[enum-surface]
Desc:	surface condition type
1	absorbing surface condition
2	free surface condition
3	zero surface condition

[enum-source]
Desc:	source type
1	point source
2	sources located on given reflector
3	sources located on given dipping reflector

[sumigpsti]

Cat:	Anisotropy
Cat:	Migration
Desc:	Phase shift migration for TI media with turning rays
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vnfile bin r -		binary file containing NMO velocities
Port:	vfile bin r -		binary file containing interval velocities
Port:	etafile	bin r -		binary file containing eta values
Port:	par par r -		optional par file input
Param:	dt float - "from header d1" time sampling interval (sec)
Param:	dx float - "from header d2" distance between successive cdps
Param:	ffil float-list - 0,0,.5/dt,.5/dt trapezoid window of frequencies
Param:	tmig float-list 0<= 0.	list of times in VT function
LDesc:	tmig values are ignored if vnfile is specified
Param:	vnmig float-list 0< 1500. list of NMO velocities in VT function
LDesc:	vnmig values are ignored if vnfile is specified
Param:	vmig float-list 0< 1500. list of interval velocities in VT function
LDesc:	vmig values ignored if vnfile is specified
Param:	etamig float-list - 0.	list of interval eta values in VT function
LDesc:	etamig values ignored if etafile is specified
Param:	nxpad int 0< 0		number of cdps to pad with zeros before fft
Param:	ltaper int 0< 0		length of linear taper for left and right edges
Param:	verbose int 0<=1 0	=1 for diagnostic printout

[susynlvfti]

Cat:	Anisotropy
Cat:	Synthetics
Desc:	Synthetic seismograms for linear velocity function in TI medium
Port:	stdout su w req		output synthetic traces
Port:	par par r -		optional par file input
Param:	nt int 0< 101		number of time samples
Param:	dt float - .04		time sampling interval (sec)
Param:	ft float - 0.		first time sample (sec)
Param:	nxo int 0< 1		number of source-receiver offsets
Param:	dxo float - .05		offset sampling interval (km)
Param:	fxo float - 0.		first offset (km)
Param:	xo float-list - fxo,fxo+dxo...	array of non-uniform offsets
Param:	nxm int 0< 101		number of midpoints
Param:	dxm float - .05		midpoint sampling interval (km)
Param:	fxm float - 0.		first midpoint (km)
Param:	nxs int 0< 101		number of shotpoints
Param:	dxs float - .05		shotpoint sampling interval (km)
Param:	fxs float - 0.		first shotpoint (km)
Param:	x0 float - 0.		x-distance at which v00 is specified
Param:	z0 float - 0.		z depth at which v00 is specified
Param:	v00 float - 2.		velocity at x0,z0 (km/sec)
Param:	dvdx float - 0.		derivative of velocity with distance (dv/dx)
Param:	dvdz float - 0.		derivative of velocity with depth (dv/dz)
Param:	fpeak float - .2/dt	peak frequency for Ricker wavelet (Hz)
Param:	dup-ref string - 1:1,2;4,2 reflector(s):  "amp:x1,z1;x2,z2;x3,z3;..."
Param:	smooth int 0<=1 0	=1 for piecewise cubic spline reflectors
Param:	er int 0<=1 0		=1 for exploding reflector amplitudes
Param:	ls int 0<=1 0		=1 for line source, =0 for point source
Param:	ob int 0<=1 0		=1 to include obliquity factors
Param:	tmin float - 10.*dt	minimum time of interest (sec)
Param:	ndpfz int 0< 5		number of diffractors per Fresnel zone
Param:	verbose int 0<=1 1	=1 to print some useful information
Param:	angxs float - 0.	angle of symmetry axis with vertical (deg)
Param:	a float - 1.		ratio of elastic coefficient c1111/c3333
Param:	f float - .4		ratio of elastic coefficient c1133/c3333
Param:	l float - .3		ratio of elastic coefficient c1313/c3333
Param:	delta float - 0		Thomsen's delta parameter (1986)
Param:	epsilon float - 0	Thomsen's epsilon parameter (1986)
Param:	ntries int 0< 40	iterations in Snell's law and offset searches
Param:	epsx float - .001	lateral offset tolerance
Param:	epst float - .0001 	reflection time tolerance
Param:	nitmax int 0< 12	max iterations in travel time integrations

[vel2stiff]

Cat:	Anisotropy
Desc:	transform vp/vs/rho/Thomsen/Sayers parameters to elastic stiffnesses
Port:	vpfile bin r req	file with P-wave velocities
Port:	vsfile bin r req	file with S-wave velocities
Port:	rhofile bin r req	file with densities
Port:	epsfile bin r -		file with Thomsen/Sayers epsilon
Port:	deltafile bin r -	file with Thomsen/Sayers delta
Port:	gammafile bin r -	file with Thomsen/Sayers gamma
Port:	c11_file bin w c11_file	output file of c11 components
Port:	c13_file bin w c13_file	output file of c13 components
Port:	c33_file bin w c33_file	output file of c33 components
Port:	c44_file bin w c44_file	output file of c44 components
Port:	c66_file bin w c66_file	output file of c66 components
Port:	par par r -		optional par file input
Param:	nx int 1<= 101		number of x samples (2nd, slow dimension)
Param:	nz int 1<= 101		number of z samples (1st, fast dimension)

