# interpolate.spec --
#
#	Tksu trace interpolation modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suinterp]

Cat:	Interpolation
Desc:	interpolate traces using automatic event picking
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	ninterp int 0<= 1	number of interpolated traces per trace
LDesc:	ninterp is the number of traces to output between each pair of
LDesc:	input traces
Param:	nxmax int 2<= 500	maximum number of input traces
Param:	freq1 float 0<= 4.0	starting corner frequency of unaliased range
Param:	freq2 float 0<= 20.0	ending corner frequency of unaliased range
Param:	deriv int 0<=1 0	derivative flag
LDesc:	If deriv=1, take vertical derivative on pick section
Param:	linear int 0<=1 0	linear flag
LDesc:	If linear=1, use linear temporal interpolation.  Otherwise use
LDesc:	8-point sinc temporal interpolation.
Param:	lent int 1<= 5		number of samples to smooth for dip estimate
Param:	lenx int 1<= 1		number of traces to smooth for dip estimate
Param:	lagc float 0< 400	agc window width in ms for dip estimate
Param:	xopt int 0<=1 0		spatial derivative flag
LDesc:	If xopt=1, compute spatial derivative via FFT.  Otherwise, compute
LDesc:	spatial derivative via differences.
Param:	iopt enum-interp - 0	choice of output
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[enum-interp]
Desc:	suinterp output choice
0	interpolate (normal operation)
1	output low-pass model (useful for QC if interpolator fails)
2	output dip picks in units of samples/trace

[suresamp]

Cat:	Interpolation
Desc:	resample traces in time
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	nt int 1<= tr.ns	number of time samples on output
Param:	dt float 0< tr.dt/10^6	time sampling interval on output (sec)
Param:	tmin float - tr.delrt/1000	time of first sample on output (sec)

[resamp]

Cat:	Interpolation
Desc:	resample 1st (fast) dimension of a 2D array
Port:	stdin bin r req		input 2D array of floats, dim n1*n2
Port:	stdout bin w req	output 2D array of floats, dim n1r*n2r
Port:	par par r -		optional par file input
Param:	n1 int 1<= all		number of samples in 1st (fast) dimension
Param:	n2 int 1<= all		number of samples in 2nd (slow) dimension
Param:	d1 float 0< 1.0		sampling interval in 1st dimension
Param:	f1 float - d1		first sample in 1st dimension
Param:	n1r int 1<= n1		number of samples in dim 1 after resampling
Param:	d1r float 0< d1		sampling interval in dim 1 after resampling
Param:	f1r float - f1		first sample in dim 1 after resampling

[suttoz]

Cat:	Interpolation
Desc:	resample from time to depth, given interval velocity function v(t)
Port:	stdin su r req		input time traces
Port:	stdout su w req		output depth traces
Port:	vfile bin r -		file containing interval velocities
LDesc:	vfile should contain an array of interval velocities uniformly
LDesc:	sampled at the same rate as the input traces.
LDesc:	If vfile is specified, parameters v and t are ignored.
Port:	par par r -		optional par file input
Param:	nz int 0< -		number of depth samples output
Param:	dz float 0< vmin*dt/2	depth sampling interval
Param:	fz float 0< v(ft)*ft/2	first depth sample
Param:	t float 0< 0.		array of times t(k)
Param:	v float 0< 1500.	array of interval velocities v(t(k))
Param:	verbose int 0<=1 1	if 1, report diagnostics

