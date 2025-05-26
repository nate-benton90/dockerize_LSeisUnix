# editing.spec --
#
#	Tksu editing category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suabshw]

Cat:	Editing
Desc:	replace header key word by its absolute value	
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	key enum-thed - offset	header key word

[suazimuth]

Cat:	Editing
Desc:	compute trace azimuth from sx,sy,gx,gy header fields
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	key enum-thed - otrav	header field to store computed azimuth in
Param:	scale float - 1		scale the azimuth by this amount
Param:	az int 0<=1 0		0: 0-180 degrees, 1: 0-360 degrees
Param:	sector float - 1	divide by this to convert azimuth to sector

[subset]

Cat:	Editing
Desc:	select a subset of samples from a 3D volume
Port:	stdin bin r req		3D volume of floats, dimension n1*n2*n3
Port:	stdout bin w req	3D subset of input volume, dim n1s*n2s*n3s
Port:	par par r -		optional par file input
Param:	n1 int 1<= nfloats	number of samples in 1st (fast) dimension
LDef:	nfloats (where nfloats is the total number of input samples)
Param:	n2 int 1<= nfloats/n1	number of samples in 2nd (mid) dimension
LDef:	nfloats/n1 (where nfloats is the total number of input samples)
Param:	n3 int 1<= nfloats/(n1*n2) number of samples in 3rd (slow) dimension
LDef:	nfloats/(n1*n2) (where nfloats is the total number of input samples)
Param:	id1s int 1<= 1		subsampling increment in dimension 1
Param:	id2s int 1<= 1		subsampling increment in dimension 2
Param:	id3s int 1<= 1		subsampling increment in dimension 3
Param:	if1s int 0<= 0		first subsample index in dimension 1
Param:	if2s int 0<= 0		first subsample index in dimension 2
Param:	if3s int 0<= 0		first subsample index in dimension 3
Param:	n1s int 1<= -		subsample count in dimension 1
LDef:	1 + (n1 - if1s - 1)/id1s
Param:	n2s int 1<= -		subsample count in dimension 2
LDef:	1 + (n2 - if2s - 1)/id2s
Param:	n3s int 1<= -		subsample count in dimension 3
LDef:	1 + (n3 - if3s - 1)/id3s
Param:	ix1s int-list 0<= -	specific indices of subsamples in dimension 1
LDef:	if1s, if1s+id1s, ...
LDesc:	as an alternative to id1s,if1s,n1s, explicitly list subsample indices
Param:	ix2s int-list 0<= -	specific indices of subsamples in dimension 2
LDef:	if2s, if2s+id2s, ...
LDesc:	as an alternative to id2s,if2s,n2s, explicitly list subsample indices
Param:	ix3s int-list 0<= -	specific indices of subsamples in dimension 3
LDef:	if3s, if3s+id3s, ...
LDesc:	as an alternative to id3s,if3s,n3s, explicitly list subsample indices

[suchw]

Cat:	Editing
Desc:	change header word using one or two header word fields
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	key1 enum-thed-list - cdp output header words (a + b*key2 + c*key3)/d
Param:	key2 enum-thed-list - cdp input header words
Param:	key3 enum-thed-list - cdp input header words
Param:	a float-list - 0	overall shift to apply
Param:	b float-list - 1	scale to apply to key2
Param:	c float-list - 0	scale to apply to key3
Param:	d float-list - 1	overall scale factor to divide by

[sucountkey]

Cat:	Editing
Desc:	count the number of unique values for a given keyword
Port:	stdin su r req 		trace input
Port:	stdout text w req 	output value counts
Port:	par par r -		optional par file input
Param:	key enum-thed-list - req keywords to be counted

[suedit]

Cat:	Editing
Desc:	interactively examine headers and samples in su trace file
Trans:	PositionalArgs
Port:	stdin su r -		if stdin, examine file read-only
Port:	file-1 su rw -		file to examine and modify

[sugethw]

Cat:	Editing
Desc:	report values of the selected header key words
Port:	stdin su r req		trace input
Port:	stdout text w /dev/tty	ascii text (or binary) output
Port:	par par r -		optional par file input
Param:	key enum-thed-list - req trace header key words (at least one)
Param:	output enum-sugethw - ascii output format: ascii, binary or geom
Param:	verbose int 0<=1 0	if 1, report diagnostics

[enum-sugethw]
Desc:	Choice of format for sugethw output
ascii	output written as ascii text for display
binary	output written as binary floats
geom	output written as ascii for geometry setting

[sukill]

Cat:	Editing
Desc:	zero out traces
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	min int 1<= req		first trace to kill (counting from 1)
Param:	count int 1<= 1		number of traces to kill

[sumute]

Cat:	Editing
Desc:	Mute above or below a given polygonal curve in x
Port:	stdin su r req		input traces
Port:	stdout su w req		muted output traces
Port:	xfile bin r -	file of x-values x(k) defining polygonal curve
Port:	tfile bin r -	file of time values t(k) defining polygonal curve
Port:	par par r -		optional par file input

Param:	xmute float-list - -	x-values x(k) defining polygonal curve
LDef:	xmute is REQUIRED if file `xfile' is not named
Param:	tmute float-list - -	time values t(k) defining polygonal curve
LDef:	tmute is REQUIRED if file `tfile' is not named
Param:	nmute int 0< -		number of x,t values in xfile and tfile
LDef:	nmute is REQUIRED if polygonal curve is defined in xfile and tfile
Param:	key enum-thed - offset	key header word holding x values
Param:	ntaper int 0<= 0	number of points to taper before hard mute
Param:	below int 0<=2 0	if 1, apply mute below the polygonal curve
LDesc:	If 0, apply mute above polygonal curve.
LDesc:	If 1, apply mute below polygonal curve.
LDesc:	If 2, apply a constant mute outside the time interval (xmute,tmute).
Param:	linvel float 0< 330	velocity value for linear mute
Param:	tm0 float - 0		time shift at x=0 for linear mute

[suquantile]

Cat:	Editing
Desc:	display some quantiles or ranks of a data set
Port:	stdin su r req		trace input
Port:	stdout text w /dev/tty	output quantile or rank report
Port:	par par r -		optional par file input
Param:	panel int 0<=1 1	do trace-by-trace (0) or whole data set (1)
Param:	quantiles int 0<=1 1	report quantiles (0) or ranks (1)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[surange]

Cat:	Editing
Desc:	report max and min values for non-zero header entries
Port:	stdin su r req		trace input
Port:	stdout text w /dev/tty	header entry report
LDesc:	reports the max and min values of all non-zero header entries

[sushw]

Cat:	Editing
Desc:	set header words in SU traces
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	infile bin r -		file containing header values
LDesc:	if given, infile should contain header values in the order specified
LDesc:	by key=key1,key2,...
Port:	par par r -		optional par file input
Param:	key enum-thed-list - cdp header key word(s) to set
Param:	a int-list - 0		value(s) on first trace
Param:	b int-list - 0		increment(s) within group
Param:	c int-list - 0		group increment(s)
Param:	d int-list - 0		trace number shift(s)
Param:	j int-list 1<= ULONG_MAX number of elements in group

[sutab]

Cat:	Editing
Desc:	print non-zero header values and traces in a tab-plot
Port:	stdin su r req		trace input
Port:	stdout text w /dev/tty	text-formatted plot
Port:	par par r -		optional par file input
Param:	itmin int 0<= 0		first time sample to plot (from 0)
Param:	itmax int 0<= "last sample" last time sample to plot (from 0)
Param:	count int 1<= "all traces"  number of traces to plot

[suvlength]

Cat:	Editing
Desc:	adjust variable length traces to common length
Port:	stdin su r req		input traces
Port:	stdout su w req		output common length traces
Port:	par par r -		optional par file input
Param:	ns int 0< "ns of first trace"	output trace length (in samples)

[suwind]

Cat:	Editing
Desc:	window traces by key word
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	verbose int 0<=1 0	verbosity
Param:	key enum-thed - tracl	key header word to window on
Param:	min int - LONG_MIN	min value of key header word to pass
Param:	max int - LONG_MAX	max value of key header word to pass
Param:	abs int 0<=1 0		if 1, take absolute value of key header word
Param:	j int 1<= 1		trace subsampling interval
LDesc:	pass every j-th trace, starting with trace s, up to count traces
Param:	s int 0<= 0		first subsampled trace index
LDesc:	pass every j-th trace, starting with trace s, up to count traces
Param:	count int 1<= ULONG_MAX number of traces to pass
LDesc:	pass every j-th trace, starting with trace s, up to count traces
Param:	reject int - -		skip traces with specified key values
Param:	accept int - -		pass traces with specified key values
Param:	dt float 0< "from header" time sampling interval (sec)
Param:	tmin float 0<= 0	min time to pass
Param:	tmax float 0<= "from header" max time to pass
Param:	itmin int 0<= 0		min time sample to pass
Param:	itmax int 0<= "from header" max time sample to pass
Param:	nt int 1<= -		number of time samples to pass
LDef:	nt = itmax - itmin + 1

[suxedit]

Cat:	Editing
Desc:	interactively examine headers and samples in su trace file
Trans:	PositionalArgs
Port:	stdin su r -		if stdin, examine file read-only
Port:	file-1 su rw -		file to examine and modify

[unglitch]

Cat:	Editing
Desc:	zero outliers in data; invokes sugain with qclip=.99
Port:	stdin su r req		input traces
Port:	stdout su w req		unglitched output traces

