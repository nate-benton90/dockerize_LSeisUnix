# stack.spec --
#
#	Tksu trace stack category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[sudivstack]

Cat:	Trace Stack
Desc:	diversity stacking using average power or peak power in windows
Port:	stdin su r req 		trace input
Port:	stdout su w req 	stacked trace output
Port:	par par r -		optional par file input
Param:	winlen float 0< .064	window length (seconds)
Param:	peak int 0<=1 0		0 = average power, 1 = peak power

[suintvel]

Cat:	Trace Stack
Cat:	Conversion
Desc:	convert stacking velocities to interval velocities
Port:	par par r -		optional par file input
Port:	outpar par w /dev/tty	output parameter file
LDesc:	The output par file contains h=<layer thickness vector> and
LDesc:	v=<interval velocities vector>.
Param:	vs float-list 0< req	array of stacking velocities
Param:	t0 float-list 0<= req	array of normal incidence times

[sunmo]

Cat:	Trace Stack
Desc:	NMO for an arbitrary velocity function of time and CDP
Port:	stdin su r req		input traces
Port:	stdout su w req		output nmo-corrected traces
Port:	par par r -		optional par file input
Param:	dup-tnmo float-list 0<= 0 array of monotonically increasing NMO times
LDesc:	array of NMO times corresponding to velocities in array vnmo
LDesc:	if NMO is function of t & cdp, input tnmo and vnmo arrays for each cdp
Param:	dup-vnmo float-list 0< 2000 array of velocities corresponding to tnmo
LDesc:	if NMO is function of t & cdp, input tnmo and vnmo arrays for each cdp
Param:	dup-anis1 float-list - 0 1st anisotropy term array, one for each cdp
Param:	dup-anis2 float-list - 0 2nd anisotropy term array, one for each cdp
Param:	cdp float-list - -	array of cdps
Param:	smute float - 1.5	samples with NMO stretch > smute are zeroed
Param:	lmute int 0- 25 length (samples) of linear ramp for stretch mute
Param:	sscale int 0<=1 1	if non-zero, apply NMO stretch scaling
Param:	invert int 0<=1 0	if 1, perform (approximate) inverse NMO
Param:	ixoffset int 0<=1 0	if 1, read cross-line offset from trace header

[supws]

Cat:	Trace Stack
Desc:	phase-weighted stack (PWS) of adjacent traces with same header value
Port:	stdin su r req		input traces
Port:	stdout su w req		output traces
Port:	par par r -		optional par file input
Param:	key enum-thed - cdp	header key word to stack on
LDesc:	all adjacent traces having the same header value are summed together
Param:	pwr float - 1.		raise phase stack to power pwr
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	sl float 0<= 0		window length for smoothing phase stack (sec)
Param:	ps int 0<=1 0		if 0, output PWS; if 1, output phase stack
Param:	verbose int 0<=1 0	if 1, report additional information

[sustack]

Cat:	Trace Stack
Desc:	stack adjacent traces having the same header value
Port:	stdin su r req		input pre-stack traces
Port:	stdout su w req		output stacked traces
Port:	par par r -		optional par file input
Param:	key enum-thed - cdp	header key word to stack on
LDesc:	all adjacent traces having the same header value are summed together
Param:	normpow float 0<= 1.	determines sample scaling
LDesc:	each sample is divided by the normpow'th number of non-zero values
LDesc:	stacked; normpow=0 selects no division
Param:	verbose int 0<=1 0	if 1, report additional information

[sustkvel]

Cat:	Trace Stack
Cat:	Conversion
Desc:	convert interval velocities at constant dip to stacking velocities
Port:	par par r -		optional par file input
Port:	outpar par w /dev/tty	output parameter file
LDesc:	The output par file contains tv=<array of normal incidence times>
LDesc:	and v=<array of stacking velocities>.
Param:	v float-list 0< req	array of interval velocities
Param:	h float-list 0<= req	array of layer thicknesses
Param:	dip float - 0.0		constant dip of layers (degrees)

