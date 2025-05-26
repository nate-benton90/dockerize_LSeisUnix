# transform.spec --
#
#	Tksu transform category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[entropy]

Cat:	Transform
Cat:	Utility
Desc:	compute the entropy of a signal
Port:	stdin float r req 	input signal
Port:	stdout float w req 	output entropy value
Port:	par par r -		optional par file input
Param:	n int 0< req	  	number of values in input signal

[mrafxzwt]

Cat:	Transform
Desc:	multi-resolution analysis of f(x,z) by wavelet transform
Port:	stdin bin r req		input f(x,z)
Port:	stdout bin w req	output MRA file
Port:	reconfile bin w -	reconstructed data file
Port:	reconmrafile bin w -	reconstructed data file in MRA domain
Port:	dfile bin w -		diff between input file and reconfile
Port:	dmrafile bin w -	diff between MRA file and reconmrafile
Port:	par par r -		optional par file input
Param:	n1 int 0< req		size of 1st (fast) dimension
Param:	n2 int 0< req		size of 2nd (slow) dimension
Param:	p1 int 0< -		largest int such that 2**p1 <= n1
Param:	p2 int 0< - 		largest int such that 2**p2 <= n2
Param:	order int 4<=20 6	order of Daubechies wavelet used (even number)
Param:	mralevel1 int 0<= 3	max multi-resolution analysis level 1st dim
Param:	mralevel2 int 0<= 3	max multi-resolution analysis level 2nd dim
Param:	trunc float 0<=100 0	truncation level (%) of the reconstruction
Param:	verbose int 0<=1 0	if 1, print some useful information
Param:	dconly int 0<=1 0	if 1, keep only dc component of MRA
Param:	nc1 int 0<= n1/2	center of trimmed image in 1st dimension
LDesc:	nc1 is used only if n1 is not an integer power of 2
Param:	nc2 int 0<= n2/2	center of trimmed image in 2nd dimension
LDesc:	nc2 is used only if n2 is not an integer power of 2

[suamp]

Cat:	Transform
Desc:	output amp, phase, real or imag trace from (freq, x) data
Port:	stdin su r req 		complex trace input, in frequency domain
Port:	stdout su w req 	real trace output, in frequency domain
Port:	par par r -		optional par file input
Param:	mode enum-amp - amp	output selector

[enum-amp]
Desc:	suamp output choice
amp	output amplitude traces
phase	output phase traces
real	output real parts
imag	output imaginary parts

[suattributes]

Cat:	Transform
Desc:	complex trace attribute analysis
Port:	stdin su r req 		trace input
Port:	stdout su w req 	attribute trace output
Port:	par par r -		optional par file input
Param:	mode enum-attrib - amp	choice of attribute
Param:	unwrap float 0<= -	phase unwrap parameter

[enum-attrib]
Desc:	attribute trace choice
amp	amplitude envelope traces
phase	phase traces
freq	frequency traces

[suenv]

Cat:	Transform
Desc:	complex trace attribute analysis (via suattributes)
Port:	stdin su r req 		trace input
Port:	stdout su w req 	attribute trace output
Param:	mode enum-attrib - amp	choice of attribute
Param:	unwrap float 0<= -	phase unwrap parameter

[suhilb]

Cat:	Transform
Desc:	Hilbert transform
Port:	stdin su r req 		trace input
Port:	stdout su w req 	Hilbert transformed trace output

[sufft]

Cat:	Transform
Desc:	Fourier transform real time traces to complex frequency traces
Port:	stdin su r req 		real trace input
Port:	stdout su w req 	fft-transformed complex trace output
Port:	par par r -		optional par file input
Param:  sign int -1<=1 1 	sign in exponent of fft
Param:  dt float 0< "from header"  time sampling interval (sec)
Param:  verbose int 0<=1 1	if 0, stop advisory messages

[suifft]

Cat:	Transform
Desc:	Inverse Fourier transform complex frequency traces to real traces
Port:	stdin su r req 		complex frequency trace input
Port:	stdout su w req 	real trace output
Port:	par par r -		optional par file input
Param:  sign int -1<=1 -1	sign in exponent of inverse fft

[sugabor]

Cat:	Transform
Desc:	Gabor transform-like multifilter of seismic trace data
Port:	stdin su r req 		real input seismic trace data
Port:	stdout su w req 	time-frequency instantaneous amp traces
Port:	par par r -		optional par file input
Param:  dt float 0< "from header" time sampling interval (sec)
Param:  fmin float - 0 		minimum frequency of filter array (Hz)
Param:  fmax float - nyquist 	maximum frequency of filter array (Hz)
Param:  beta float 0< 3.0 	ln(filter peak amp/filter endpoint amp)
Param:  band float 0< nyquist/20  filter bandwidth (Hz)
Param:  alpha float 0< beta/band^2 filter width parameter
Param:  verbose int 0<=1 0	if, report diagnostics

[suharlan]

Cat:	Transform
Desc:	Signal-noise separation by Harlan invertible linear transformation
Port:	stdin su r req 		trace input file
Port:	signal su w out_signal 	output file for extracted signal
Port:	noise su w out_noise 	output file for extracted noise
Port:	par par r -		optional par file input
Param:  niter int 0<=1 1	number of requested iterations
Param:  anenv int 0<=1 1	if 0, no analytic envelopes (not recommended)
Param:  scl int 0<=1 0		if 1, scale output traces (not recommended)
Param:  plot int 0<=3 3         plot type
LDesc:	plot type: 0 = no plots, 1 = 1D plots, 2 = 2D plots, 3 = all plots.
Param:  norm int 0<=1 1		if 0, do not normalize reliability values
Param:  verbose int 0<=1 1	if 1, report processing info
Param:  rgt int 1<=2 2		random generator type: 1=uniform, 2=gaussian
Param:  sts int 0<=1 1		if 0, no smoothing (not recommended)
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()
Param:  dx float 0< 20		offset sampling interval (m)
Param:  fx float 0- 0		offset on first trace (m)
Param:  dt float 0< .004	time sampling interval (sec)
Param:  gopt enum-gopt - 1	transform algorithm
Param:  pmin1 float 0<= 400	minimum moveout for fwd transform (ms)
LDesc:  minimum moveout at farthest offset for forward transform (ms)
Param:  pmax1 float 0<= 400	maximum moveout for fwd transform (ms)
LDesc:  maximum moveout at farthest offset for forward transform (ms)
Param:  pmin2 float 0<= pmin1	minimum moveout for inv transform (ms)
LDesc:  minimum moveout at farthest offset for inverse transform (ms)
Param:  pmax2 float 0<= pmax1	maximum moveout for inv transform (ms)
LDesc:  maximum moveout at farthest offset for inverse transform (ms)
Param:  np int 0< 100		number of p-values for tau-p transform
Param:  prewhite float 0<= .01	prewhitening value (suggested .01 to .1)
Param:  offref float 0<= 2000 	reference offset for p values (m)
Param:  depthref float 0<= 500  reference depth for Foster/Mosher tau-p 
LDesc:  reference depth for Foster/Mosher tau-p if gopt=4
Param:  pmula float - pmax1	maximum p value preserved in data (ms)
Param:  pmulb float - pmax1	minimum p value muted in data (ms)
Param:  ninterp int 0<= 0	number of traces to interpolate in input data
Param:  nintlh int 0<= 50	number of intervals (bins) in histograms
Param:  sditer int 0< 5		number of steepest descent iterations for ps
Param:  c float 0<= .04		maximum noise allowed in signal sample (%)
Param:  rel1 float 0<= .5  reliability value for signal extraction 1st pass
Param:  rel2 float 0<= .75 reliability value for signal extraction 2nd pass
Param:  r1 int 0<= 10	number of points in damped lsq vertical smoothing
Param:  r2 int 0<= 2	number of points in damped lsq horizontal smoothing

[enum-gopt]
Desc:	Moveout choice for Radon transform
1	parabolic transform
2	Foster/Mosher transform
3	linear transform
4	absolute value of linear transform

[sulog]

Cat:	Transform
Desc:	time axis log-stretch of seismic traces
Port:	stdin su r req 		unstretched trace input
Port:	stdout su w req 	log-stretched trace output
Port:   outpar par w /dev/tty	output parameter file
Port:	par par r -		optional par file input
Param:  ntmin int 0<= nt/10	minimum time sample of interest
Param:  m int 0< 3		length of stretched data
Param:  ntau int 0< "power of 2"  length of stretched data override
LDesc:  ntau = nextpower(m*nt) where nt=number of samples in trace

[suilog]

Cat:	Transform
Desc:	time axis inverse log-stretch of seismic traces
Port:	stdin su r req		log-stretched trace input
Port:   stdout su w req		restored trace output
Port:	par par r -		optional par file input
Param:  nt int 0< req		number of samples in unstretched trace
Param:  ntmin int 0- req	minimum time sample of interest
Param:  dt float 0< req		sampling interval of unstretched data

[suradon]

Cat:	Transform
Desc:	forward or reverse Radon transform
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	choose enum-choose 0<=4 0 transform direction and mode
Param:	gopt enum-gopt - 1	transform type (choice of moveout)
Param:	offref float - 2000	reference max offset for max, min moveouts
Param:	interoff float - 0	intercept offset for associated tau-p times
Param:	pmin float - -200	minimum moveout on reference offset (ms)
Param:  pmax float - 400	maximum moveout on reference offset (ms)
Param:	dp float - 16		moveout increment on reference offset (ms)
Param:	pmula float - 80	moveout for multiples, max time (ms)
LDesc:	moveout on reference offset where multiples begin at maximum time (ms)
Param:	pmulb float - 200	moveout for multiples, zero time (ms)
LDesc:	moveout on reference offset where multiples begin at zero time (ms)
Param:	depthref float - 500	reference depth for hyperbolic transform
Param:	nwin int 0< 1		number of windows to use through mute zone
Param:	f1 float 0<= 60		high-end frequency before taper off (Hz)
Param:	f2 float 0<= 80		high-end frequency (Hz)
Param:	prewhite float 0< .1	prewhitening factor in percent
Param:	cdpkey enum-thed - cdp	header word defining ensemble
Param:	offkey enum-thed - offset header word with spatial information
Param:	nxmax int 0< 120	maximum number of input traces per ensemble
Param:	ninterp int 0<= 0	trace interpolation factor
LDesc:	number of traces to interpolate between input traces before transform
Param:	freq1 float 0<= 3.	low-end frequency for picking (Hz)
Param:	freq2 float 0<= 20.	high-end frequency for picking (Hz)
Param:	lagc int 0<= 400	length of AGC operator for picking (ms)
Param:	lent int 0<= 5		length of time smoother for picker (samples)
Param:	lenx int 0<= 1		length of space smoother for picker (samples)
Param:	xopt int 0<=1 1		spatial derivative type
LDesc:	If 1, use differences for spatial derivative (irregular spacing).
LDesc:	If 0, use FFT derivative for spatial derivative (regular spacing).

[enum-choose]
Desc:	transform direction and mode
0	compute forward Radon transform
1	compute data minus multiples
2	compute estimate of multiples
3	compute forward and reverse Radon transforms
4	compute inverse Radon transform

[sureduce]

Cat:	Transform
Desc:	convert seismic traces to display in reduced time
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output seimic traces in reduced time
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	rv float 0< 8.0		reducing velocity (km/s)

[suspecfk]

Cat:	Transform
Desc:	F-K Fourier spectrum of input data
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output f-k domain amp spectrum of traces
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	dx float - tr.d2	spatial sampling interval
Param:	verbose int 0<=1 0	if 1, report information
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[suspecfx]

Cat:	Transform
Desc:	1D Fourier amplitude spectrum (t->f) of seismic traces
Port:	stdin su r req		input time-domain seismic traces
Port:	stdout su w req		output frequency-domain amp spectrum of traces

[suspeck1k2]

Cat:	Transform
Desc:	2D wavenumber (K1,K2) Fourier spectrum of 2D X-domain data
Port:	stdin su r req		input 2D spatial X-domain data
Port:	stdout su w req		output 2D transformed wavenumber data
Port:	par par r -		optional par file input
Param:	d1 float - tr.d1	spatial sampling interval in fast dimension
Param:  d2 float - tr.d2	spatial sampling interval in slow dimension
Param:	verbose int 0<=1 0	if 1, report information
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sutaup]

Cat:	Transform
Desc:	forward and inverse T-X and F-K global slant stacks
Port:	stdin su r req		input data
Port:	stdout su w req 	output data
Port:	par par r -		optional par file input
Param:	option enum-optn - 1	transform type
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	nx int 0< ntr		number of horizontal samples (traces)
Param:	dx float 0< 1		horizontal sampling interval (m)
Param:	npoints int 0< 71	number of points for rho filter
Param:	pmin float - 0		minimum slope for Tau-P transform (s/m)
Param:	pmax float - 1/500	maximum slope for Tau-P transform (s/m)
Param:	np int 0< nx		number of slopes for Tau-P transform
Param:	ntau int 0< nt		number of time samples for Tau-P transform
Param:	fmin float 0<= 3	minimum frequency of interest (Hz)
Param:	verbose int 0<=1 0	if 1, report information
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[enum-optn]
Desc:	transform type
1	forward F-K transform
2	forward T-X transform
3	inverse F-K transform
4	inverse T-X transform

[sutsq]

Cat:	Transform
Desc:	time axis time-squared stretch of seismic traces
Port:	stdin su r req		input seismic traces
Port:	stdout su w req		output time-squared traces
Port:	par par r -		optional par file input
Param:	tmin float 0< .1*nt*dt	minimum time sample of interest (forward only)
Param:	dt float 0< .004	output sampling interval (inverse only)
Param:	flag int -1<=1 1	flag indicating inverse or forward transform
LDesc:	If 1, forward stretch time to time-squared.
LDesc:	If -1, inverse stretch time-squared to time.

