# model.spec --
#
#	Tksu model building category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[elatriuni]

Cat:	Model Building
Cat:	Anisotropy
Desc:	convert triangulated elastic model to uniformly sampled model
Port:	stdin bin r req		input elastic model (from elamodel)
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of x samples (slow dimension)
Param:	nz int 1<= req		number of z samples (fast dimension)
Param:	dx float 0< 1.0		x sampling interval
Param:	dz float 0< 1.0		z sampling interval
Param:	fx float - 0.0		first x sample
Param:	fz float - 0.0		first z sample
Port:	a1111file bin w a1111.bin	binary file to store a1111 component
Port:	a3333file bin w a3333.bin	binary file to store a3333 component
Port:	a1133file bin w a1133.bin	binary file to store a1133 component
Port:	a1313file bin w a1313.bin	binary file to store a1313 component
Port:	a1113file bin w a1113.bin	binary file to store a1113 component
Port:	a3313file bin w a3313.bin	binary file to store a3313 component
Port:	a1212file bin w a1212.bin	binary file to store a1212 component
Port:	a1223file bin w a1223.bin	binary file to store a1223 component
Port:	a2323file bin w a2323.bin	binary file to store a2323 component
Port:	rhofile bin w rho.bin		binary file to store density

[makevel]

Cat:	Model Building
Desc:	make a velocity function v(x,y,z)
Port:	stdout bin w req	velocity array (floats) dimensioned (nx,ny,nz)
Port:	vzfile bin r -		file containing v(z) profile
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of x samples (slow dimension)
Param:	ny int 1<= 1		number of y samples (mid dimension)
Param:	nz int 1<= req		number of z samples (fast dimension)
Param:	dx float 0< 1.0		x sampling interval
Param:	dy float 0< 1.0		y sampling interval
Param:	dz float 0< 1.0		z sampling interval
Param:	fx float - 0.0		first x sample
Param:	fy float - 0.0		first y sample
Param:	fz float - 0.0		first z sample
Param:	v000 float 0< 2.0	velocity at (x=0,y=0,z=0)
Param:	dvdx float - 0.0	velocity gradient with respect to x
Param:	dvdy float - 0.0	velocity gradient with respect to y
Param:	dvdz float - 0.0	velocity gradient with respect to z
Param:	vlens float - 0.0	velocity perturbation in parabolic lens
Param:	tlens float 0<= 0.0	thickness of parabolic lens
Param:	dlens float 0<= 0.0	diameter of parabolic lens
Param:	xlens float - -		x coordinate of center of parabolic lens
Param:	ylens float - -		y coordinate of center of parabolic lens
Param:	zlens float - -		z coordinate of center of parabolic lens
Param:	vran float 0<= 0.0	std dev of random perturbation
Param:	vzran float 0<= 0.0	std dev of random perturbation to v(z)
Param:	vzc float - 0.0		v(z) chirp amplitude
Param:	z1c float - fz		z at which to begin chirp
Param:	z2c float - fz+(nz-1)*dz z at which to end chirp
Param:	l1c float 0< dz		wavelength at beginning of chirp
Param:	l2c float 0< dz		wavelength at end of chirp
Param:	exc float - 1.0		exponent of chirp

[regrid3]

Cat:	Model Building
Desc:	rewrite a [ni3][ni2][ni1] grid to a [no3][no2][no1] 3D grid
Port:	stdin bin r req		input 3D array of floats
Port:	stdout bin w req	output 3D array of floats
Port:	par par r -		optional par file input
Param:	ni1 int 1<= 1		1st (fast) dimension of input grid
Param:	ni2 int 1<= 1		2nd (mid)  dimension of input grid
Param:	ni3 int 1<= 1		3rd (slow) dimension of input grid
Param:	no1 int 1<= 1		1st (fast) dimension of output grid
Param:	no2 int 1<= 1		2nd (mid)  dimension of output grid
Param:	no3 int 1<= 1		3rd (slow) dimension of output grid
Param:	verbose int 0<=1 0	if 1, report diagnostics

[sutetraray]

Cat:	Model Building
Desc:	3D tetrahedral wavefront construction ray tracing
Port:	stdin hz r req		input tetrafile (from tetramod)
Port:	stdout bin w req	output traveltime tables
Port:	par par r -		optional par file input
Param:	nxgd int 0< req		number of x samples (fast dimension)
Param:	nygd int 0< req		number of y samples (second dimension)
Param:	fxgd float - "xmin from tetrafile" first x sample
Param:	fygd float - "ymin from tetrafile" first y sample
Param:	shootupward int -1<=1 1 if 1, shoot upward; if -1, shoot downward
Param:	dxgd float 0< (xmax-xmin)/(nxgd-1) x sampling interval
Param:	dygd float 0< (ymax-ymin)/(nygd-1) y sampling interval
Param:	takeoff float 0<=90 30.0 max takeoff angle for shooting aperture (deg)
Param:	ntakeoff int 0< 5	number of samples in takeoff angle
Param:	nazimuth int 0< 15	number of samples in azimuth
Param:	maxntris int 0< 1000	max number of triangles allowed
Param:	maxnrays int 0< 600	max number of rays allowed
Param:	maxnsegs int 0< 100	max number of ray segments allowed
Param:	tmax float 0<= 10.	max traveltime traced (sec)
Param:	dtwf float 0< 0.05	wavefront step size for ray tracing
Param:	ntwf int 0< -		max number of wavefronts
LDef:	the maximum of tmax/dtwf+1 and ifwf2dump+nwf2dump+1
Param:	edgemax int 0< 4	max triangle length not to be split
LDesc:	maximum edge length (in units of dxgd) of triangle that will not be
LDesc:	split
Param:	fsx float - fxgd+nxgd*dxgd/2 first source in x
Param:	dsx float 0< dxgd	x increment in source
Param:	nsx int 0< 1		number of sources in x
Param:	fsy float - fygd+nygd*dygd/2 first source in y
Param:	dsy float 0< dygd	y increment in source
Param:	nsy int 0< 1		number of sources in y
Param:	nxt int 0< nxgd*2/2+1	number of x-samples in ttable for sukdmig3d
Param:	nyt int 0< nygd*2/2+1	number of y-samples in ttable for sukdmig3d
Param:	irefseq int-list - -	reflector sequence
Param:	ifwf2dump int 0<= 20	first wavefront to dump to wffile
Param:	nwf2dump int 0<= 1	number of wavefronts to dump to wffile
Param:	verbose int 0<=1 0	if 1, print some useful information
Port:	crfile bin w -		output cosines and raypaths (for sukdmig3d)
Port:	sttfile bin w -		output surface traveltimes (for visualization)
Port:	rayfile bin w -		output raypaths
Port:	wffile bin w -		output wavefronts
Port:	jpfile text w stderr	output job info

[suwellrf]

Cat:	Model Building
Desc:	convert well log depth, velocity, density into reflectivity R(t)
Port:	stdout su w req		output reflectivity traces
Port:	dvrfile bin r -		input depth, velocity, & density log values
LDesc:	dvrfile is only input file required if used (single file input)
Port:	dvfile bin r -		input depth and velocity log values
LDesc:	alternate input with drfile (2-file input)
Port:	drfile bin r -		input depth and density log values 
LDesc:	alternate input with dvfile (2-file input)
Port:	dfile bin r - 		input depth values
LDesc:	alternate input with vfile and rhofile (3-file input)
Port:	vfile bin r - 		input velocity log values 
LDesc:	alternate input with dfile and rhofile (3-file input)
Port:	rhofile bin r - 	input density log values
LDesc:	alternate input with dfile and vfile (3-file input)
Param:	nval int 0< req		number of triplets, pairs, or single inputs
Param:	dtout float - .004	output time sampling interval

[tetramod]

Cat:	Model Building
Desc:	tetrahedral model builder
Port:	stdout hz w req		output tetrafile
Port:	hzfile bin w req	output xhz,yhz,zhz,v0hz,v1hz for viewer3
Port:	par par r -		optional par file input
LDesc:	Because of the complexity of the parameter input, an input par file
LDesc:	is recommended.
Param:	nxhz int 1<= req	number of samples (2nd dimension) for horizons
Param:	nyhz int 1<= req	number of samples (1st dimension) for horizons
Param:	xmin float - 0		x of lower left point in the model
Param:	ymin float - 0		y of lower left point in the model
Param:	xmax float - 2		x of upper right point in the model
Param:	ymax float - 2		y of upper right point in the model
Param:	zmax float - "max z"	maximum z in the model
Param:	blt float - 1.0		bottom layer thickness
Param:	nhz int 0<= 1		number of layers in model (except model base)
Param:	ficth int -1<= -1	fictitious horizons in model
Param:	verbose int 0<=1 0	if 1, print some useful information
Param:	z00 float-list - 0,0.6,1.2,... z at (xmin,ymin) on each horizon
Param:	z01 float-list - 0,0.6,1.2,... z at (xmin,ymax) on each horizon
Param:	z10 float-list - 0,0.6,1.2,... z at (xmax,ymin) on each horizon
Param:	z11 float-list - 0,0.6,1.2,... z at (xmax,ymax) on each horizon
Param:	v00 float-list 0< 1,2,3,... v at (xmin,ymin) on each horizon
Param:	v01 float-list 0< 1,2,3,... v at (xmin,ymax) on each horizon
Param:	v10 float-list 0< 1,2,3,... v at (xmax,ymin) on each horizon
Param:	v11 float-list 0< 1,2,3,... v at (xmax,ymax) on each horizon
Param:	dvdz00 float-list - 0,0,0,... dvdz at (xmin,ymin) on each horizon
Param:	dvdz01 float-list - 0,0,0,... dvdz at (xmin,ymax) on each horizon
Param:	dvdz10 float-list - 0,0,0,... dvdz at (xmax,ymin) on each horizon
Param:	dvdz11 float-list - 0,0,0,... dvdz at (xmax,ymax) on each horizon
Param:	enum-tetramod string - - x, y, z, v or dvdz grid file for a horizon
LDesc:	replicate this parameter to define tetramod horizon files.

[enum-tetramod]
Desc:		File name parameters for tetramod horizon files
x0file		horizon 0 x grid file
y0file		horizon 0 y grid file
z0file		horizon 0 z grid file
v0file		horizon 0 v grid file
dvdz0file	horizon 0 dvdz grid file
x1file		horizon 1 x grid file
y1file		horizon 1 y grid file
z1file		horizon 1 z grid file
v1file		horizon 1 v grid file
dvdz1file	horizon 1 dvdz grid file
x2file		horizon 2 x grid file
y2file		horizon 2 y grid file
z2file		horizon 2 z grid file
v2file		horizon 2 v grid file
dvdz2file	horizon 2 dvdz grid file
x3file		horizon 3 x grid file
y3file		horizon 3 y grid file
z3file		horizon 3 z grid file
v3file		horizon 3 v grid file
dvdz3file	horizon 3 dvdz grid file
x4file		horizon 4 x grid file
y4file		horizon 4 y grid file
z4file		horizon 4 z grid file
v4file		horizon 4 v grid file
dvdz4file	horizon 4 dvdz grid file

[tri2uni]

Cat:	Model Building
Desc:	convert triangulated model to uniformly sampled model
Port:	stdin tri r req		input triangulated model
Port:	stdout bin w req	output uniformly gridded 2D model (floats)
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (slow) dimension
Param:	d1 float 0< 1.0		sampling interval in dimension 1
Param:	d2 float 0< 1.0		sampling interval in dimension 2
Param:	f1 float - 0.0		first sampled value in dimension 1
Param:	f2 float - 0.0		first sampled value in dimension 2

[trimodel]

Cat:	Model Building
Desc:	make a triangulated sloth (1/velocity^2) model
Port:	stdout tri w req	output triangulated model
Port:	par par r -		optional par file input
LDesc:	Because of the complexity of the parameter input, an input par file
LDesc:	is recommended.
Param:	xmin float - 0.0	minimum horizontal coordinate (x)
Param:	xmax float - 1.0	maximum horizontal coordinate (x)
Param:	zmin float - 0.0	minimum vertical coordinate (z)
Param:	zmax float - 1.0	maximum vertical coordinate (z)
Param:	dup-xedge float-list - - x coordinates of an edge
Param:	dup-zedge float-list - - z coordinates of an edge
Param:	dup-sedge float-list - - sloth coordinates of an edge
Param:	kedge int-list - -	array of indices used to identify edges
Param:	normray int 0<=1 0	if 1, generate ray parameters
Param:	normface int - -	interface to shoot rays to
Param:	nrays int 1<= -		number of locations to shoot rays to
Param:	sfill float-list - -	x, z, x0, z0, s00, dsdx, dsdz to fill a region
Param:	densfill float-list - - x, z, dens to fill a region
Param:	qfill float-list - -	x, z, Q-factor to fill a region
Param:	maxangle float - 5.0	maximum angle (deg) between adjacent edge segs

[triray]

Cat:	Model Building
Desc:	dynamic ray tracing for a triangulated sloth model
Port:	stdin tri r req		input triangulated model
Port:	stdout bin w req	output rayends
Port:	par par r -		optional par file input
Param:	xs float - (max-min)/2	x coordinate of source
Param:	zs float - min		z coordinate of source
Param:	nangle int 1<= 101	number of takeoff angles
Param:	fangle float - -45	first takeoff angle (degrees)
Param:	langle float - 45	last takeoff angle (degrees)
Port:	rayfile bin w -		ray-edge intersections (x,z points)
Param:	nxz int 1<= 101		number of (x,z) points in optional rayfile
Port:	wavefile bin w -	ray (x,z) points uniformly sampled in time
Param:	nt int 1<= 101		number of (x,z) points in optional wavefile
Port:	infofile text w -	file storing useful information
Port:	fresnelfile bin w fresnelfile.bin used to plot fresnel volumes
Port:	outparfile par w outpar	output parameters for plotting software
Param:	krecord int - -		save only rays with index krecord
Param:	prim int 0<=1 0		if 0, direct hits; if 1, single-reflected rays
Param:	ffreq float - -1	fresnel volume frequency
Param:	dup-refseq int-list - 1,0,0  ray reflection sequence (see help)

[uni2tri]

Cat:	Model Building
Desc:	convert uniformly sampled model to a triangulated model
Port:	stdin bin r req		input uniform grid of floats
Port:	stdout tri w req	output triangulated model
Port:	par par r -		optional par file input
LDesc:	Because of the complexity of the parameter input, an input par file
LDesc:	is recommended.
Port:	ifile tri r -		triangulated model file used as initial model
Port:	efile bin w emax.dat	error file (if verbose=2)
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (slow) dimension
Param:	d1 float 0< 1.0		sampling interval in dimension 1
Param:	d2 float 0< 1.0		sampling interval in dimension 2
Param:	f1 float - 0.0		first sampled value in dimension 1
Param:	f2 float - 0.0		first sampled value in dimension 2
Param:	errmax float - -	maximum sloth error
Param:	verbose int 0<=2	if 1, report max error; if 2, write to efile
Param:	mm int 0<= 0		output every mm-th intermediate model
Param:	mfile string - intmodel	base name for intermediate model files
Param:	method enum-uni2tri - 3	method to add vertices
Param:	tol int 0<= 10		closeness criterion (in samples)
Param:	dup-sfill float-list - - x, z, x0, z0, s00, dsdx, dsdz to fill a region

[enum-uni2tri]
Desc:	Method to add vertices to triangulated model
1	add 1 vertex at maximum error
2	add vertex to every triangle that exceeds errmax
3	like method 2, but avoid closely spaced vertices

[unif2]

Cat:	Model Building
Desc:	generate 2D uniform velocity grid from a layered model
Port:	stdin text r req	input ascii file of x,z values (see help)
Port:	stdout bin w req	output uniform velocity grid, dim nx by nz
Port:	tfile text w -		a sample input dataset for stdin
Port:	par par r -		optional par file input
Param:	ninf int 1<= 5		number of interfaces
Param:	nx int 1<= 100		number of samples in x (slow) dimension
Param:	nz int 1<= 100		number of samples in z (fast) dimension
Param:	dx float 0< 10.0	x sampling interval
Param:	dz float 0< 10.0	z sampling interval
Param:	fx float - 0.0		first x sample
Param:	fz float - 0.0		first z sample
Param:	npmax int 1<= 201	maximum number of points on interfaces
Param:	x0 float-list - 0,0,0,... distance x at which v00 is specified
Param:	z0 float-list - 0,0,0,... depth z at which v00 is specified
Param:	v00 float-list 0< 1500,2000,2500,... velocity at each x0,z0 point
Param:	dvdx float - 0.0	derivative of velocity with distance x
Param:	dvdz float - 0.0	derivative of velocity with depth z
Param:	method enum-uni - linear interpolation method

[enum-uni]
Desc:	Interpolation method for unif2 and unisam
linear	linear interpolation
mono	monotonic cubic interpolation
akima	Akima's cubic interpolation
spline	cubic spline interpolation

[unisam]

Cat:	Model Building
Desc:	uniformly sample a function y(x) specified as x,y pairs
Port:	xfile bin r -		array of x values (floats)
Port:	yfile bin r -		array of y values (floats)
Port:	xyfile bin r -		array of x,y pairs (floats)
Port:	stdout bin w req	output uniformly sampled array (floats)
Port:	par par r -		optional par file input
Param:	xin float-list - -	array of x values
Param:	yin float-list - -	array of y values
Param:	npairs int 1<= -	number of pairs in xfile/yfile or xyfile
Param:	nout int 1<= req	number of y values output to binary file
Param:	dxout float 0< 1.0	output x sampling interval
Param:	fxout float - 0.0	output first x
Param:	method enum-uni - linear interpolation method
Param:	isint int-list - -	where to apply sine interpolations
Param:	amp float-list - 0,0,... amplitude of sine interpolations
Param:	totalphase float-list - PI,PI,... total phase
Param:	nwidth int 0<= 0	if > 0, apply window smoothing

[unisam2]

Cat:	Model Building
Desc:	uniformly sample a 2D function f(x1,x2)
Port:	stdin bin r req		array of f(x1,x2) samples (nx1 by nx2 floats)
Port:	stdout bin w req	uniformly sampled array (n1 by n2 floats)
Port:	par par r -		optional par file input
Param:	x1 float-list - -	array of x1 values at which f(x1,x2) is sampled
Param:	nx1 int 1<= 1		number of input samples in 1st dimension
Param:	dx1 float 0< 1		input sampling interval in 1st dimension
Param:	fx1 float - 0		first input sample in 1st dimension
Param:	x2 float-list - -	array of x2 values at which f(x1,x2) is sampled
Param:	nx2 int 1<= 1		number of input samples in 2nd dimension
Param:	dx2 float 0< 1		input sampling interval in 2nd dimension
Param:	fx2 float - 0		first input sample in 2nd dimension
Param:	n1 int 1<= 1		number of output samples in 1st dimension
Param:	d1 float 0< 1		output sampling interval in 1st dimension
Param:	f1 float - 0		first output sample in 1st dimension
Param:	n2 int 1<= 1		number of output samples in 2nd dimension
Param:	d2 float 0< 1		output sampling interval in 2nd dimension
Param:	f2 float - 0		first output sample in 2nd dimension
Param:	method enum-uni - linear interpolation method

[velconv]

Cat:	Model Building
Desc:	velocity conversion
Port:	stdin bin r req		2D input array (nx*nt or nx*nz floats)
Port:	stdout bin w req	2D output array (nx*nt or nx*nz floats)
Port:	par par r -		optional par file input
Param:	intype enum-velconv - req input velocity type
Param:	outtype enum-velconv - req output velocity type
Param:	nt int 1<= all		number of time samples
Param:	dt float 0< 1.0		time sampling interval
Param:	ft float - 0.0		first time
Param:	nz int 1<= all		number of depth samples
Param:	dz float 0< 1.0		depth sampling interval
Param:	fz float - 0.0		first depth
Param:	nx int 1<= all		number of traces

[enum-velconv]
Desc:	Valid types for input and output velocity arrays
vintt	interval velocity as a function of time
vrmst	RMS velocity as a function of time
vintz	interval velocity as a function of depth
zt	depth as a function of time
tz	time as a function of depth

[vtlvz]

Cat:	Model Building
Desc:	create velocity as function of time for a linear v(z)
Port:	stdout bin w req	velocity array dimensioned nt
LDesc:	outputs an array of velocities (floats) v(t) = v0 exp(a t/2)
Port:	par par r -		optional par file input
Param:	nt int 1<= req		number of time samples
Param:	dt float 0< req		time sampling interval (sec)
Param:	v0 float 0< req		velocity at the surface (m/s or ft/s)
Param:	a float - req		velocity gradient (1/sec)

[wkbj]

Cat:	Model Building
Desc:	compute WKBJ ray theoretic parameters via finite differencing
Port:	stdin bin r req		2D velocity array v[nx][nz]
Port:	stdout bin w req	2D traveltime array t[nx][nz]
Port:	sfile bin w sfile	2D sigma array sg[nx][nz]
Port:	bfile bin w bfile	2D array of incidence angles bet[nx][nz]
Port:	afile bin w afile	2D array of propagation angles a[nx][nz]
Port:	par par r -		optional par file input
Param:	nx int 1<= req		number of x samples (slow dimension)
Param:	nz int 1<= req		number of z samples (fast dimension)
Param:	xs float - req		x coordinate of source
Param:	zs float - req		z coordinate of source
Param:	dx float - 1.0		x sampling interval
Param:	fx float - 0.0		first x sample
Param:	dz float - 1.0		z sampling interval
Param:	fz float - 0.0		first z sample

