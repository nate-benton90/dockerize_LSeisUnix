# statics.spec --
#
#	Tksu statics category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[grm]

Cat:	Statics
Desc:	generalized reciprocal refraction analysis for a single layer	
Port:	stdin text r req 	4 columns: x, y, forward & reverse time
Port:	stdout text w req 	GRM analysis output (see help)
Port:	par par r -		optional par file input
Param:	nt int 1<= req		number of arrival time pairs
Param:	dx float 0< req		geophone spacing (m)
Param:	v0 float 0< req		velocity in weathering layer (m/s)
Param:	abtime float 0<= req	a-b time (ms), or if 0, use last time
Param:	XY float - optimal	value that overrides optimum XY
Param:	XYmax float - 2*dx*10	max offset allowed in optimum XY search (m)
Param:	depthres float - 0.5	x increment for vertical depth search (m)

[suresstat]

Cat:	Statics
Desc:	surface-consistent source and receiver statics calculation
Port:	stdin su r req		input moveout-corrected shot gather traces
Port:	ssol text w req		source statics as function of sx and gx
Port:	rsol text w req		receiver statics as function of sx and gx
Port:	par par r -		optional par file input
Param:	ntcc int 0< 250		number of samples in cross-correlation window
Param:	ntpick int 0< 50 	number of samples in picking window
Param:	n_o int - 7		near-offset position relative to the shot
Param:	niter int 0< 5		number of iterations
Param:	ns int 0< 240		number of shots
Param:	nr int 0< 335		number of receivers
Param:	nc int 0< 574		number of common midpoints
Param:	sfold int 0< 96		shot gather fold
Param:	rfold int 0< 96		receiver gather fold
Param:	cfold int 0< 48		cmp gather fold
Param:	sub int 0<=1 0		if 1, subtract supertrace 1 from supertrace 2
Param:	mode int 0<=1 0		choice of peak
LDesc:	If 0, use the global maximum in cross-correlation.
LDesc:	If 1, choose the peak `perc' smaller than the global maximum.
Param:	perc float 0< 10.	percent of global max used (only if mode=1)
Param:	icmpshift int - 9 	shift applied to icmp index

[sustatic]

Cat:	Statics
Desc:	apply elevation static corrections with values from headers or files
Port:	stdin su r req		input traces
Port:	stdout su w req		output static-corrected traces
Port:	sou_file bin r -	input file for source statics if hdrs=2
Port:	rec_file bin r -	input file for receiver statics if hdrs=2
Port:	par par r -		optional par file input
Param:	v0 float 0< v1		weathering velocity
Param:	v1 float 0< req		subweathering velocity
Param:	hdrs int 0<=2 0		where field statics values come from
LDesc:	If 0, calculate field statics using header field sut in ms.
LDesc:	If 1, fetch statics from headers.
LDesc:	If 2, read statics from sou_file and rec_file.
Param:	sign int -1<=1 1	direction of static shift (-1 = shift up)
Param:	ns int 0< 240		number of sources in sou_file when hdrs=2
Param:  nr int 0< 335		number of receivers in rec_file when hdrs=2
Param:	no int 0< 96		number of offsets when hdrs=2

[sustaticrrs]

Cat:	Statics
Desc:	apply elevation static corrections with residual refraction statics
Port:	stdin su r req		input traces
Port:	stdout su w req		output traces
Port:	sou_file bin r -	input file for source statics if hdrs=2
Port:	rec_file bin r -	input file for receiver statics if hdrs=2
Port:	blvl_file bin r -	base of near-surface model file when hdrs=3
Port:	refr_file bin r -	horizontal reference data file when hdrs=3
Port:	vfile bin r -		near-surface velocity model file when hdrs=3
Port:	par par r -		optional par file input

Param:	v0 float 0< "from header"	weathering velocity
Param:	v1 float 0< "from header"	subweathering velocity
Param:	hdrs enum-statics - 0	type of statics corrections applied
Param:	sign int -1<=1 1	direction of static shift (-1 = shift up)
Param:	ns int 0< 240		number of sources when hdrs=2
Param:	nr int 0< 335		number of receivers when hdrs=2
Param:	no int 0< 96		number of offsets when hdrs=2
Param:	nsamp int 0< -		number of midpoints on line when hdrs=3 or 4
Param:	fx float - -		1st x location in vel model when hdrs=3 or 4
Param:	dx float - -		midpoint interval when hdrs=3 or 4
Param:	V_r float - -		replacement velocity when hdrs=3
Param:	mx int 0< -		number of lateral velocity samples when hdrs=3
Param:	mz int 0< - 		number of vertical velocity samples when hdrs=3
Param:	dzv float 0< -		velocity model depth interval when hdrs=3

[enum-statics]
Desc:	Type of statics corrections to apply
0	field statics are calculated (see documentaton)
1	statics calc not performed; statics correction applied from header
2	surface-consistent statics obtained from files sou_file and rec_file
3	residual refraction and average refraction statics calculated
4	residual refraction statics are applied
5	average refraction statics are applied

