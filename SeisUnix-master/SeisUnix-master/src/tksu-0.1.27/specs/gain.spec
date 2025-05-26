# gain.spec --
#
#	Tksu trace gain category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suagc]

Cat:	Trace Gain
Desc:	perform agc gain correction; invokes sugain with gagc=1
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output

[sudivcor]

Cat:	Trace Gain
Desc:	divergence (spreading) correction				
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	vfile bin r -		optional file containing vrms values
Port:	par par r -		optional par file input
Param:	trms float-list 0<= 0	monotonically increasing traveltimes
Param:	vrms float-list 0< 1500	RMS velocities matching times in trms array

[sugain]

Cat:	Trace Gain
Desc:	apply various types of gain to traces
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	panel int 0<=1 0	if 1, gain whole data set (vs. trace by trace)
Param:	tpow float - 0.		multiply samples by t^tpow
Param:	epow float - 0.		multiply samples by exp(epow*t)
Param:	gpow float - 1.		take signed gpowth power of scaled data
Param:	agc int 0<=1 0		if 1, apply agc
Param:	gagc int 0<=1 0		if 1, apply agc with gaussian taper
Param:	wagc float - .5		agc window width (sec) (if agc or gagc = 1)
Param:	trap float 0<= none	zero any value whose magnitude exceeds trap
Param:	clip float 0<= none	clip any value whose magnitude exceeds clip
Param:	qclip float 0<=1 -	quantile to clip on (note: not percentile)
Param:	qbal int 0<=1 0		if 1, balance traces by qclip and scale
Param:	pbal int 0<=1 0		if 1, balance traces by dividing by rms
Param:	mbal int 0<=1 0		if 1, balance traces by subtracting mean
Param:	scale float - 1.	multiply data by overall scale factor
Param:	bias float - 0.		bias data by adding an overall bias value
Param:	jon int 0<=1 0		if 1, set tpow=2, gpow=.5 and qclip=.95
LDesc:	if 1, apply Claerbout's choice of gain parameters:
LDesc:	tpow=2, gpow=0.5, qclip=0.95
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sunormalize]

Cat:	Trace Gain
Desc:	balance traces by their rms, max or median norms
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	dt float 0< "from header" trace sample interval (sec)
Param:	ns int 1<= "from header"  number of samples per trace
Param:	norm enum-sunormalize - rms norm to balance traces by
Param:	t0 float - 0.0		starting time for window (sec)
Param:	t1 float - ns*dt	ending time for window (sec)

[enum-sunormalize]
Desc:	type of norm to be applied in sunormalize
rms	RMS (L2) norm
max	maximum (L-infinity) norm
med	median (L1) norm

[supgc]

Cat:	Trace Gain
Desc:	apply agc function to all traces preserving relative amps spatially
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	ntrscan int 0< 200	number of traces to scan for gain function
Param:	lwindow float - 1.	length of time window (sec)

[suweight]

Cat:	Trace Gain
Desc:	weight traces by header parameter
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	key enum-thed - offset	header field containing weight w1
LDesc:	the weight applied to the trace is (a + b*w1) where w1 is from
LDesc:	header field `key'
Param:	a float - 1.0		constant weighting parameter (for key)
Param:	b float - .0005		variable weighting parameter (for key)
Param:	key2 enum-thed - -	alternate header field containing weight w2
LDesc:	the weight applied to the trace is scale*w2 where w2 is from
LDesc:	header field `key2'
Param:	scale float - .0001	weighting parameter (for key2)

