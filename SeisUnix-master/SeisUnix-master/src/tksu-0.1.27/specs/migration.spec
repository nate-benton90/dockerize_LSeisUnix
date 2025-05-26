# migration.spec --
#
#	Tksu migration category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[sugazmig]

Cat:	Migration
Desc:	phase-shift migration for zero-offset data
Port:	stdin su r req 		trace input
Port:	stdout su w req 	migrated trace output
Port:	vfile bin r -		interval velocity array v(t)
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	ft float - 0.		first time sample (sec)
Param:	ntau int 0< nt		number of migrated time samples
Param:	dtau float 0< dt	migrated time sampling interval (sec)
Param:	ftau float - ft		first migrated time sample (sec)
Param:	tmig float-list 0<= 0.	array of times t(k) of velocity function
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	vmig float-list 0< 1500. array of interval velocities v(t(k))
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	verbose int 0<1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigfd]

Cat:	Migration
Desc:	45-90 degree finite difference migration for zero-offset data
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nz int 0< req		number of depth samples
Param:	dz float 0< req		depth sampling interval
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	dip int 45<=90 65	maximum angle of dip reflector (degrees)
LDesc:	the maximum dip angle may be one of 45,65,79,80,87,89,90
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigffd]

Cat:	Migration
Desc:	Fourier finite difference migration for zero-offset data
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nz int 0< req		number of depth samples
Param:	dz float - req		depth sampling interval
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	ft float - 0.		first time sample (sec)
Param:	fz float - 0.		first depth sample
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumiggbzo]

Cat:	Migration
Desc:	migration using Gaussian beams for zero-offset data
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nz int 0< req		number of depth samples
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	dz float 0< 1.		depth sampling interval
Param:	fmin float - .025/dt	minimum frequency
Param:	fmax float - 10*fmin	maximum frequency
Param:	amax float -90<=90 60.	maximum emergence angle (degrees)
Param:	amin float -90<=90 amax	minimum emergence angle (degrees)
Param:	bwh float - .5*vavg/fmin beam half-width
LDef:	0.5*vavg/fmin, where vavg is average velocity

[sumigps]

Cat:	Migration
Desc:	migration by phase-shift with turning rays
Port:	stdin su r req		input traces
LDesc:	input traces must be sorted by increasing or decreasing cdp
Port:	stdout su w req		output migrated traces
Port:	vfile bin r -		velocity file containing v(t) array
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	ffil float-list 0<= -	trapezoidal window migration frequencies
LDef:	four corner frequencies 0,0,.5/dt,.5/dt
Param:	tmig float-list 0< 0.	array of times t(k) of velocity function
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	vmig float-list 0< 1500. array of interval velocities v(t(k))
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	nxpad int 0< 0		number of cdps to pad with zeroes for FFT
Param:	ltaper int 0< 0		linear taper length for left and right edges
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigpspi]

Cat:	Migration
Desc:	phase-shift + interpolation migration with laterally varying velocity
Port:	stdin su r req		input zero-offset traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nz int 0< req		number of depth samples
Param:	dz float 0< req		depth sampling interval
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigsplit]

Cat:	Migration
Desc:	split-step depth migration for zero-offset data
Port:	stdin su r req		input zero-offset traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nz int 0< req		number of depth samples
Param:	dz float 0< req		depth sampling interval
Param:	dt float 0< "from header" time sampling interval (sec)
LDef:	from header tr.dt, otherwise 0.004 if tr.dt is zero
Param:	dx float 0< "from header" midpoint sampling interval
LDef:	from header tr.d2, otherwise 1.0 if tr.d2 is zero
Param:	ft float - 0.		first time sample
Param:	fz float - 0.		first depth sample
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigtk]

Cat:	Migration
Desc:	T-K domain migration for common-midpoint stacked data
Port:	stdin su r req		input CMP stacked traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r -		file containing interval velocity v(t)
Port:	par par r -		optional par file input
Param:	dxcdp float 0< req	distance between successive cdps
Param:	fmax float - fnyquist	maximum frequency
Param:	tmig float-list - 0.	array of times t(k) of velocity function
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	vmig float-list - 1500.	array of interval velocities v(t(k))
LDesc:	tmig and vmig are ignored if vfile is specified
Param:	nxpad int 0< 0		number of cdps to pad with zeroes for FFT
Param:	ltaper int 0< 0		linear taper length for left and right edges
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sukdmig2d]

Cat:	Migration
Desc:	Kirchhoff depth migration of 2D poststack/prestack data
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output common offset migrated traces
Port:	ttfile bin r req	input traveltime table
Port:	jpfile text w stderr	job print file name
Port:	tvfile bin r tvfile	file of traveltime variation tables
Port:	csfile bin r csfile	file of cosine tables
Port:	dataout1 bin w dataout1 additional migration output of extra amps
Port:	par par r -		optional par file input
Param:	fzt float - req		first depth sample in traveltime table
Param:	nzt int 0< req		number of depth samples in traveltime table
Param:	dzt float 0< req	depth interval in traveltime table
Param:	fxt float - req		first lateral sample in traveltime table
Param:	nxt int 0< req		number of lateral samples in traveltime table
Param:	dxt float 0< req	lateral interval in traveltime table
Param:	fs float - req		x-coordinate of first source
Param:	ns int 0< req		number of sources
Param:	ds float - req		x-coordinate increment of sources
Param:	dt float 0< "from header"	time sampling interval (sec)
Param:	ft float - "from header (ft)"	first time sample (sec)
Param:	dxm float 0< "from header (d2)"	midpoints sampling interval
Param:	fzo float - fzt		z-coordinate of first point in output trace
Param:	dzo float 0< .2*dzt	vertical spacing of output trace
Param:	nzo int 0< 5*(nzt-1)+1	number of points in output trace
Param:	fxo float - fxt		x-coordinate of first output trace
Param:	dxo float 0< .5*dxt	horizontal spacing of output trace
Param:	nxo int 0< 2*(nxt-1)+1	number of output traces
Param:	off0 float - 0		first offset in output
Param:	doff float - 99999	offset increment in output
Param:	noff int 0< 1		number of offsets in output
Param:	fmax float 0<= .25/dt	frequency highcut for input traces (Hz)
Param:	offmax float 0<= 3000	maximum absolute offset allowed in migration
Param:	aperx float 0<= nxt*dxt/2	migration lateral aperature
Param:	angmax float 0<=90 60	migration angle aperture from vertical (deg)
Param:	v0 float 0< 1500.	reference velocity at surface
Param:	dvz float - 0.		reference velocity vertical gradient
Param:	ls int 0<=1 1		if 0, point source; if 1, line source
Param:	mtr int 1<= 100		print verbal information at every mtr traces
Param:	ntr int 1<= 100000	max number of input traces to be migrated
Param:	npv int 0< 0		compute quantities for velocity analysis
LDesc:  npv > 0 requires the use of tvfile, csfile, and dataout1 files

[sukdmig3d]

Cat:	Migration
Desc:	Kirchhoff depth migration of 3D poststack/prestack data
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output common offset migrated traces
Port:	ttfile bin r req	input traveltime table
Port:	crfile bin r -		input for cos theta and ray paths
Port:	jpfile text w stderr	job print file name
Port:	par par r -		optional par file input
Param:	fxgd float - f1		first x-sample in tt table
Param:	nxt int 0< ns		number of x samples in tt table
Param:	dxgd float 0< d1	x-interval in tt table
Param:	fygd float - f2		first y-sample in tt table
Param:	nyt int 0< ntr		number of y samples in tt table
Param:	dygd float 0< d2	y-interval in tt table
Param:	ixsf int - sdel		x in dxgd of first source
Param:	nxs int 0< nhs		number of sources in x
Param:	ixsr int - swdep	ratio of source & gd spacing
Param:	iysf int - gdel		y in dygd of first source
Param:	nys int 0< nvs		number of sources in y
Param:	iysr int - gwdep	ratio of source & gd spacing
Param:	fzs float - sdepth/1000	first depth sample in tt table
Param:	nzs int 0< duse		number of depth samples in tt table
Param:	dzs float 0< ep/1000	depth interval in tt table
Param:	nxgd int 0< selev	x size of the traveltime region
Param:	nygd int 0< gelev	y size of the traveltime region
Param: 	multit int 0<= scalel	number of multivalued traveltimes
Param:	dt float 0< dt		stime sampling interval for input data
Param:	ft float - ft		first time sample of input data
Param:	dxm float 0< d2		midpoint spacing of input data
Param:	fzo float - fzs		z-coord of first point in output trace
Param:	dzo float 0< .2*dzs	vertical spacing of output trace
Param:	nzo int 0< 5*(nzs-1)+1	number of points in output trace
Param:	fxo float - fxgd	x-coordinate of first output trace
Param:	dxo float 0< .5*dxgd	horizontal spacing of output trace
Param:	nxo int 0< 2*(nxgd-1)+1	number of output traces
Param:	fyo float - fygd	y-coordinate of first output trace
Param:	dyo float 0< .5*dygd	horizontal spacing of output trace
Param:	nyo int 0< 2*(nygd-1)+1	number of output traces
Param:	fxoffset float - 0	first x-offset in output
Param:	fyoffset float - 0	first y-offset in output
Param:	dxoffset float - 99999	x-offset increment in output
Param:	dyoffset float - 99999	y-offset increment in output
Param:	nxoffset int 0< 1	number of x-offsets in output
Param:	nyoffset int 0< 1	number of y-offsets in output
Param:	xoffsetmax float 0<= 99999 max absolute x-offset for migration
Param:	yoffsetmax float 0<= 99999 max absolute y-offset for migration
Param:	xaper float 0<= nxt*dxgd/2.5  migration lateral aperature in x
Param:	yaper float 0<= nyt*dygd/2.5  migration lateral aperature in y
Param:	angmax float 0<=90 60	max angle to handle (degrees)
Param:	fmax float 0<= .25/dt	max frequency in data
Param:	pptr int 0<= 100	print verbal info at every pptr traces
Param:	ntrmax int 0<= 100000	max number of input traces to be migrated
Param:	ls int 0<=1 0		if 0, point source; if 1, line source

[sumigtopo2d]

Cat:	Migration
Desc:	Kirchhoff depth migration of 2D poststack/prestack data with topography
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output of common offset migrated data
Port:	ttfile bin r req	input traveltime table
Port:	jpfile text w stderr	job print file name
Port:	par par r -		optional par file input
Param:	fzt float - req		first depth sample in traveltime table
Param:	nzt int 0< req		number of depth samples in traveltime table
Param:	dzt float 0< req		depth interval in traveltime table
Param:	fxt float - req		first lateral sample in traveltime table
Param:	nxt int 0< req		number of lateral samples in traveltime table
Param:	dxt float 0< req		lateral interval in traveltime table
Param:	fs float - req		x-coordinate of first source
Param:	ns int 0< req		number of sources
Param:	ds float 0< req		x-coordinate increment of sources
Param:	fxi float - req		x-coordinate of first input trace
Param:	dxi float 0< req		horizontal spacing of input data
Param:	nxi int 0< req		number of input trace location in surface
Param:	dt float 0< tr.dt/10^6	time sampling interval (sec)
Param:	ft float - tr.ft	first time sample (sec)
Param:	dxm float 0< tr.d2	midpoints sampling interval
Param:	surf string - 0,0	array of xi,zi recording surface coordinates
Param:	fzo float - fzt		z-coordinate of first point in output trace
Param:	dzo float 0< .2*dzt	vertical spacing of output trace
Param:	nzo int 0< 5*(nzt-1)+1	number of points in output trace
Param:	fxo float - fxt		x-coordinate of first output trace
Param:	dxo float 0< .5*dxt	horizontal spacing of output trace
Param:	nxo int 0< 2*(nxt-1)+1	number of output traces
Param:	off0 float - 0		first offset in output
Param:	doff float - 99999	offset increment in output
Param:	noff int 0< 1		number of offsets in output
Param:	fmax float 0< .25/dt	frequency highcut for input traces (Hz)
Param:	offmax float - 3000	maximum absolute offset allowed in migration
Param:	aperx float 0<= nxt*dxt/2 migration lateral aperature
Param:	angmax float 0<=90 60	migration angle aperature from vertical
Param:	v0 float 0< 1500.	reference velocity at surface (m/s)
Param:	dvz float - 0.		reference velocity vertical gradient
Param:	ls int 0<=1 1		if 0, point source; if 1, line source
Param:	mtr int 0<= 100		print verbal information at every mtr traces
Param:	ntr int 0<= 100000	max number of input traces to be migrated

[sustolt]

Cat:	Migration
Desc:	Stolt migration for stacked data or common offset gathers
Port:	stdin su r req		input (sorted common-offset gathers or stacked)
Port:	stdout su w req		output migrated traces
Port:	par par r -		optional par file input
Param:	cdpmin int 0< req	minimum cdp number for which to apply DMO
Param:	cdpmax int 0< req	maximum cdp number for which to apply DMO
Param:	dxcdp float - req	distance between adjacent cdp bins (m)
Param:	noffmix int 0< 1	number of offsets to mix for unstacked data
Param:	tmig float-list - 0.	array of times t(k) of velocity function
Param:	vmig float-list - 1500.	array of rms velocities v(t(k)
Param:	smig float 0<= 1.	stretch factor (0.6 typical for increasing vrms)
Param:	vscale float - 1.	scale factor to apply to velocities
Param:	fmax float 0<= nyquist	maximum frequency in input data (Hz)
Param:	lstaper int 0<= 0	length of side tapers (# of traces)
Param:	lbtaper int 0<= 0	length of bottom taper (# of samples)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumigprefd]

Cat:	Migration
Desc:	2D prestack common-shot 45-90 degree finite-difference migration
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nxo int 0< req		number of total horizontal output samples
Param:	nxshot int 0< req	number of shot gathers to be migrated
Param:	nz int 0< req		number of depth samples
Param:	dx float 0< req		horizontal sampling interval
Param:	dz float 0< req		depth sampling interval
Param:	dip int 45<=90 79	maximum dip to migrate (degrees)
LDesc:	valid maximum dip values are 45, 65, 79, 80, 87, 89, or 90
Param:	Fmax float 0<= 25.	peak frequency of Ricker wavelet used as source
Param:	f1 float - 5.		Hamming window frequency 1 (Hz)
Param:	f2 float - 10.		Hamming window frequency 2 (Hz)
Param:	f3 float - 40.		Hamming window frequency 3 (Hz)
Param:	f4 float - 50.		Hamming window frequency 4 (Hz)
Param:	lpad int - 9999		no. of traces to pad left of depth section
LDef:	9999 is a special value to indicate full aperture
Param:	rpad int - 9999		no. of traces to pad right of depth section
LDef:	9999 is a special value to indicate full aperture

[sumigpreffd]

Cat:	Migration
Desc:	2D prestack common-shot Fourier finite-difference migration
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nxo int 0< req		number of total horizontal output samples
Param:	nxshot int 0< req	number of shot gathers to be migrated
Param:	nz int 0< req		number of depth samples
Param:	dx float 0< req		horizontal sampling interval
Param:	dz float 0< req		depth sampling interval
Param:	Fmax float 0<= 25.	peak frequency of Ricker wavelet used as source
Param:	f1 float - 5.		Hamming window frequency 1 (Hz)
Param:	f2 float - 10.		Hamming window frequency 2 (Hz)
Param:	f3 float - 40.		Hamming window frequency 3 (Hz)
Param:	f4 float - 50.		Hamming window frequency 4 (Hz)
Param:	lpad int - 9999		no. of traces to pad left of depth section
LDef:	9999 is a special value to indicate full aperture
Param:	rpad int - 9999		no. of traces to pad right of depth section
LDef:	9999 is a special value to indicate full aperture

[sumigprepspi]

Cat:	Migration
Desc:	2D prestack common-shot phase-shift-plus-interpolation migration
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nxo int 0< req		number of total horizontal output samples
Param:	nxshot int 0< req	number of shot gathers to be migrated
Param:	nz int 0< req		number of depth samples
Param:	dx float 0< req		horizontal sampling interval
Param:	dz float 0< req		depth sampling interval
Param:	Fmax float 0<= 25.	peak frequency of Ricker wavelet used as source
Param:	f1 float - 5.		Hamming window frequency 1 (Hz)
Param:	f2 float - 10.		Hamming window frequency 2 (Hz)
Param:	f3 float - 40.		Hamming window frequency 3 (Hz)
Param:	f4 float - 50.		Hamming window frequency 4 (Hz)
Param:	lpad int - 9999		no. of traces to pad left of depth section
LDef:	9999 is a special value to indicate full aperture
Param:	rpad int - 9999		no. of traces to pad right of depth section
LDef:	9999 is a special value to indicate full aperture

[sumigpresp]

Cat:	Migration
Desc:	2D prestack common-shot split-step Fourier migration
Port:	stdin su r req		input traces
Port:	stdout su w req		output migrated traces
Port:	vfile bin r req		velocity file containing v[nz][nx]
Port:	par par r -		optional par file input
Param:	nxo int 0< req		number of total horizontal output samples
Param:	nxshot int 0< req	number of shot gathers to be migrated
Param:	nz int 0< req		number of depth samples
Param:	dx float 0< req		horizontal sampling interval
Param:	dz float 0< req		depth sampling interval
Param:	Fmax float 0<= 25.	peak frequency of Ricker wavelet used as source
Param:	f1 float - 5.		Hamming window frequency 1 (Hz)
Param:	f2 float - 10.		Hamming window frequency 2 (Hz)
Param:	f3 float - 40.		Hamming window frequency 3 (Hz)
Param:	f4 float - 50.		Hamming window frequency 4 (Hz)
Param:	lpad int - 9999		no. of traces to pad left of depth section
LDef:	9999 is a special value to indicate full aperture
Param:	rpad int - 9999		no. of traces to pad right of depth section
LDef:	9999 is a special value to indicate full aperture

