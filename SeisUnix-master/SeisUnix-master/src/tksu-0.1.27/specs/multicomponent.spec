# multicomponent.spec --
#
#	Tksu multicomponent category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[sueipofi]

Cat:	Multicomponent
Desc:	eigenimage (SVD) based polarization filter for 3-component data
Port:	stdin su r req 		input in sets of 3 adjacent traces
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	wl float 0<= 0.1	SVD time window length (sec)
Param:	pwr float - 1.0		exponent of filter weights
Param:	interp enum-sueipofi - cubic	interpolation between weights
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	file string - polar	base name for additional output files
Param:	rl1 int 0<=1 0		if 1, rectilinearity along 1st principal axis
Param:	rl2 int 0<=1 0		if 1, rectilinearity along 2nd principal axis
Param:	pln int 0<=1 0		if 1, planarity

[enum-sueipofi]
Desc:	Choice of interpolation between weights
linear
cubic

[suhrot]

Cat:	Multicomponent
Desc:	horizontal rotation of 3-component data
Port:	stdin su r req		input in sets of 3 adjacent traces
Port:	stdout su w req		output traces
Port:	xfile bin r -		file containing x values if x array not used
Port:	afile bin r -		file containing a values if a array not used
Port:	par par r -		optional par file input
Param:	angle enum-angle - rad	rotation angle units
Param:	inv int 0<=1 0		0 = clockwise rotation; 1 = counter-clockwise
Param:	verbose int 0<=1 0	if 1, echo angle for each 3-component station
Param:	a float-list - -	array of rotation angles
Param:	x float-list - 0,...	array of corresponding header values
Param:	key enum-thed - tracf	header word defining 3-component station
Param:	n int 0- 0		number of x and a values in afile, xfile

[enum-angle]
Desc:	rotation angle units
rad	radians (2*PI per unit circle)
deg	degrees (360 per unit circle)
gon	grads (400 per unit circle)

[supofilt]

Cat:	Multicomponent
Desc:	polarization filter for 3-component data
Port:	stdin su r req		input traces
Port:	stdout su w req		output traces
Port:	par par r - 		optional par file input
Port:	dfile su r req		input 3-components of direction of polarization
Port:	wfile su r req		input weighting polarization parameter
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	smooth int 0<=1	1	=1 smooth filter operators; =0 no smoothing
Param:	sl float 0<= .05	smoothing window length (sec)
Param:	wpow float - 1.		raise weighting function to power wpow
Param:	dpow float - 1.		raise directivity functions to power dpow
Param:	verbose int 0<=1 0	if 1, report diagnostics

[supolar]

Cat:	Multicomponent
Desc:	polarization analysis of 3-component data
Port:	stdin su r req		input 3-component traces, in V, H1, H2 order
Port:	par par r - 		optional par file input
Param:	file string - polar	base name for output attribute files
Param:	dt float 0< "from header" time sampling increment (sec)
Param:	wl float 0< 0.1		correlation window length (sec)
Param:	win enum-win - boxcar	correlation window shape
Param:	rl int 0<=3 1		output rectilinearity attribute
Param:	rlq float - 1.0		contrast parameter for rectilinearity
Param:	dir int 0<=1 1		output 3-component polization directions
Param:	tau int 0<=1 0		output global polarization parameter
Param:	ellip int 0<=1 0	output ellipticities e21, e31 and e32
Param:	pln int 0<=1 0		output planarity measure
Param:	f1 int 0<=1 0		output flatness or oblateness coefficient
Param:	l1 int 0<=1 0		output linearity coefficient
Param:	amp int 0<=1 0		output various amplitude parameters (see help)
Param:	theta int 0<=3 0	output incidence angle of principal axis
Param:	phi int 0<=3 0		output horizontal azimuth of principal axis
Param:	angle enum-angle - rad	angular units for theta and phi
Param:	all int 0<=3 0		if nonzero, set all output flags to this value
Param:	verbose int 0<=1 0	if 1, report additional information

[enum-win]
Desc:	Correlation window shape for supolar
boxcar
hanning
bartlett
welsh

