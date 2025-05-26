# smoothing.spec --
#
#	Tksu smoothing category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[smooth2]

Cat:	Smoothing
Desc:	smooth a 2D array via a damped least squares technique
Port:	stdin bin r req		uniformly sampled 2D array of floats
Port:	stdout bin w req	smoothed 2D array of floats
Port:	efile text w -		file to contain relative error (x1)
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (slow) dimension
Param:	r1 float 0<= 0		smoothing parameter in direction 1
Param:	r2 float 0<= 0		smoothing parameter in direction 2
Param:	win int-list 0<= 0,n1,0,n2 x1,x2,y1,y2 indices defining window range
Param:	rw float 0<= 0		smoothing parameter for window function

[smooth3d]

Cat:	Smoothing
Desc:	smooth a 3D array via a damped least squares technique
Port:	stdin bin r req		uniformly sampled 3D array of floats
Port:	stdout bin w req	smoothed 3D array of floats
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (mid) dimension
Param:	n3 int 1<= req		number of samples in 3rd (slow) dimension
Param:	r1 float 0<= 0		operator length in direction 1
Param:	r2 float 0<= 0		operator length in direction 2
Param:	r3 float 0<= 0		operator length in direction 3
Param:	d1 float 0< 1		sample interval in direction 1
Param:	d2 float 0< 1		sample interval in direction 2
Param:	d3 float 0< 1		sample interval in direction 3
Param:	iter int 1<= 2		number of iterations
Param:	time int 0<=3 0		which dimension the time axis is (0 = none)
Param:	depth int 1<=3 1	which dimension the depth axis is (if time=0)
Param:	mu float - 1		relative weight at maximum depth (or time)
Param:	verbose int 0<=1 0	if 1, print minimum wavelengths
Param:	slowness int 0<=1 0	smooth on slowness (0) or velocity (1)
Param:	vminc float 0<= 0	clip velocities below vminc before smoothing
Param:	vmaxc float 0<= 99999	clip velocities above vmaxc before smoothing

[smoothint2]

Cat:	Smoothing
Desc:	smooth non-uniformly sampled interfaces via damped least squares
Port:	stdin text r req	file containing original interfaces
Port:	stdout text w req	file containing smoothed interfaces
Port:	par par r -		optional par file input
Param:	ninf int 1<= 5		number of interfaces
Param:	r float 0<= 100		smoothing parameter
Param:	npmax int 1<= 101	maximum number of points in interfaces

[sumix]

Cat:	Smoothing
Desc:	compute weighted moving average on a panel of seismic data
Port:	stdin su r req		input traces
Port:	stdout su w req		output moving averaged traces
Port:	par par r -		optional par file input
Param:	mix float-list - .6,1,1,1,.6  array of weights for moving average
LDef:	array 0.6,1,1,1,0.6 causes averaging over 5 traces
LDesc:	number of values in mix array determines the number of traces averaged

