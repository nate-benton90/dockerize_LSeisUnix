# filtering.spec --
#
#	Tksu filtering category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suband]

Cat:	Filtering 1D
Desc:	apply a sine-squared tapered bandpass filter (via sufilter)
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	f1 float 0<= 0.10*nyquist low cut frequency (Hz)
Param:	f2 float 0<= 0.15*nyquist low pass frequency (Hz)
Param:	f3 float 0<= 0.45*nyquist high pass frequency (Hz)
Param:	f4 float 0<= 0.50*nyquist high cut frequency (Hz)

[subfilt]

Cat:	Filtering 1D
Desc:	apply Butterworth bandpass filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	zerophase int 0<=1 1	filter type:  0 = minimum phase, 1 = zero phase
Param:	locut int 0<=1 1	if 0, do not apply low cut filter
Param:	hicut int 0<=1 1	if 0, do not apply high cut filter
Param:	fstoplo float 0<= "10% of nyquist" freq (Hz) in low cut stop band
Param:	astoplo float 0<= 0.05	upper bound on amp at fstoplo
Param:	fpasslo float 0<= "15% of nyquist" freq (Hz) in low cut pass band
Param:	apasslo float 0<= 0.95	lower bound on amp at fpasslo
Param:	fpasshi float 0<= "40% of nyquist" freq (Hz) in high cut pass band
Param:	apasshi float 0<= 0.95	lower bound on amp at fpasshi
Param:	fstophi float 0<= "55% of nyquist" freq (Hz) in high cut stop band
Param:	astophi float 0<= 0.05	upper bound on amp at fpasshi
Param:	verbose int 0<=1 0	if 1, report filter design info
Param:	dt float 0< "from header" trace sample rate (sec)

[suconv]

Cat:	Filtering 1D
Desc:	convolution with user-supplied filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	sufile su r -		file containing trace to use as filter
Port:	par par r -		optional par file input
Param:	filter float-list - -	array of filter coefficients f(t)

[sufilter]

Cat:	Filtering 1D
Desc:	apply a zero-phase, sine-squared tapered filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	f float-list - -	array of filter frequencies f(k) in Hz
LDef:	0.10*nyquist,0.15*nyquist,0.45*nyquist,0.50*nyquist
Param:	amps float-list - -	array of filter amplitudes amp(f(k))
LDef:	0,1,1,0 (a trapezoid-like bandpass filter)

[sufrac]

Cat:	Filtering 1D
Desc:	apply fractional time derivative or integral to time domain traces
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	power float - 0.0	power of derivative operator (-i*omega)
Param:	sign int -1<=1 -1	sign in front of i * omega
Param:	phasefac float - 0.0	apply a constant phase shift of phasefac * PI

[supef]

Cat:	Filtering 1D
Desc:	apply Wiener prediction error filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	minlag float 0< dt	first lag of prediction filter (sec)
Param:	maxlag float 0< (tmax-tmin)/20 last lag of prediction filter (sec)
Param:	pnoise float 0<= 0.001	relative additive noise level
Param:	mincorr float - tmin	start of autocorrelation window (sec)
Param:	maxcorr float - tmax	end of autocorrelation window (sec)
Param:	showwiener int 0<=1 0	if 1, show Wiener filter on each trace
Param:	mix float-list - 1.0	array of autocorrelation weights
LDesc:	array of weights for moving average of the autocorrelations

[sushape]

Cat:	Filtering 1D
Desc:	apply Wiener shaping filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	w float-list - req	input wavelet to be shaped (array of floats)
Param:	d float-list - req	desired output wavelet (array of floats)
Param:	nshape int 1<= trace	length of shaping filter (samples)
Param:	pnoise float 0<= 0.001	relative additive noise level
Param:	showshaper int 0<=1 0	if 1, show shaping filter on stderr

[sutvband]

Cat:	Filtering 1D
Desc:	time-variant bandpass filter (sine-squared taper)
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	tf float-list 0<= req	times for which frequencies are specified
Param:	dup-f float-list 0<= req  corner frequencies f1,f2,f3,f4 for each time
LDesc:	For each time in the tf array, specify the 4 corner frequencies of
LDesc:	the bandpass filter to apply at that time.

[suxcor]

Cat:	Filtering 1D
Desc:	correlation with a user-supplied filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	sufile su r -		file containing SU traces to use as filter
Port:	par par r -		optional par file input
Param:	filter float-list - -	user-supplied correlation filter f(t)
LDef:	filter is REQUIRED if sufile is not specified
Param:	first int 0<=1 1	first element flag
LDesc:	If first=1, supplied trace is default first element of correlation.
LDesc:	If first=0, it is the second.
Param:	panel int 0<=1 0	panel flag
LDesc:	If panel=0, use only the first trace of sufile as filter.
LDesc:	If panel=1, cross correlate trace-by-trace sufile with a gather.
Param:	ftwin int 0<= 0		first sample on first trace of window
Param:	ltwin int 0<= 0		first sample on last trace of window
Param:	ntwin int 1<= tr.ns	number of samples in correlation window
Param:	ntrc int 1<= 48		number of traces in a gather

[sudipfilt]

Cat:	Filtering 2D
Desc:	dip (i.e. slope) filter in f-k domain
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" time sample interval
Param:	dx float 0< "from header d1" x sample interval
Param:	slopes float-list 0<= 0	array of monotonically increasing slopes
Param:	amps float-list - 1	array of amplitudes assigned to slopes
Param:	bias float - 0		slope to make horizontal before filtering
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sufxdecon]

Cat:	Filtering 2D
Desc:	random noise attenuation by FX-deconvolution
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	taper float 0<= 0.1	length of taper (sec)
Param:	fmin float 0<= 6	minimum frequency to process (Hz)
Param:	fmax float 0<= 0.3/dt	maximum frequency to process (Hz)
Param:	twlen float 0<= "entire trace" time window length
Param:	ntrw int 1<= 10		number of traces in window
Param:	ntrf int 1<= 4		number of traces for filter (smaller than ntrw)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[suk1k2filter]

Cat:	Filtering 2D
Desc:	symmetric box-like K-domain filter for (z,x) data
Port:	stdin su r req 		depth trace input
Port:	stdout su w req 	depth trace output
Port:	par par r -		optional par file input
Param:	k1 float-list 0<= -	array of K1 filter wavenumbers
LDef:	n*.10,n*.15,n*.45,n*.50 where n is K1 nyquist
Param:	k2 float-list 0<= -	array of K2 filter wavenumbers
LDef:	n*.10,n*.15,n*.45,n*.50 where n is K2 nyquist
Param:	amps1 float-list - 0,1,1,0 array of K1 filter amplitudes
Param:	amps2 float-list - 0,1,1,0 array of K2 filter amplitudes
Param:	d1 float 0< tr.d1	sampling interval in 1st (fast) dimension
Param:	d2 float 0< tr.d2	sampling interval in 2nd (slow) dimension
Param:	quad int 0<=2 0		quadrant choice
LDesc:	If quad=0, filter is in all four quadrants.
LDesc:	If quad=1, filter is in quandrants 1 and 4.
LDesc:	If quad=2, filter is in quandrants 2 and 3.
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sumedian]

Cat:	Filtering 2D
Desc:	median filter about a user-defined polygonal curve
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	xfile bin r -		file containing position values of curve
Port:	tfile bin r -		file containing time values of curve
Port:	par par r -		optional par file input
Param:	xshift float-list - -	position values of points on curve
LDef:	REQUIRED if xfile is not specified
Param:	tshift float-list - -	time values of points on curve (sec)
LDef:	REQUIRED if tfile is not specified
Param:	nshift int 1<= -	number of (x,t) points defining median times
LDef:	REQUIRED if polygonal curve is defined in xfile and tfile
Param:	key enum-thed - tracl	trace header holding position value
Param:	mix float-list 0<= 0.6,1,1,1,0.6 array of weights for mix
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sukfilter]

Cat:	Filtering 2D
Desc:	radially symmetric K-domain filter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	k float-list 0<= -	array of K filter wavenumbers
LDef:	n*.10,n*.15,n*.45,n*.50 where n is K nyquist
Param:	amps float-list - 0,1,1,0 array of K filter amplitudes
Param:	d1 float 0< tr.d1	sampling interval in 1st (fast) dimension
Param:	d2 float 0< tr.d2	sampling interval in 2nd (slow) dimension

[sukfrac]

Cat:	Filtering 2D
Desc:	apply fractional powers of -i*abs(K) to 2D depth section
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	power float - 0.0	power of derivative operator (-i*wavenumber)
LDesc:	The filter component (k1,k2) in wavenumber domain is exp(i*K*power)
LDesc:	where K = sqrt(k1^2 + k2^2).
Param:	sign int -1<=1 -1	sign in front of i * K
Param:	d1 float 0< tr.d1	sampling interval in 1st (fast) dimension
Param:	d2 float 0< tr.d2	sampling interval in 2nd (slow) dimension
Param:	phasefac float - 0.0	apply a constant phase shift of phasefac * PI

