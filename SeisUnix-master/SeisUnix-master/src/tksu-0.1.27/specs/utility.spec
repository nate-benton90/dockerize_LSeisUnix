# utility.spec --
#
#	Tksu utility category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[cat]

Cat:	Utility
Desc:	POSIX concatenate utility
Trans:	PositionalArgs
DocCmd:	man cat | col -b
Port:	file-1 bin r req	file input
Port:	file-2 bin r -		file input
Port:	file-3 bin r -		file input
Port:	file-4 bin r -		file input
Port:	file-5 bin r -		file input
Port:	stdout bin w req	concatenated output

[fcat]

Cat:	Utility
Desc:	fast concatenate with one read per file
Trans:	PositionalArgs
Port:	file-1 bin r req	file input
Port:	file-2 bin r -		file input
Port:	file-3 bin r -		file input
Port:	file-4 bin r -		file input
Port:	file-5 bin r -		file input
Port:	stdout bin w req	concatenated output

[ctrlstrip]

Cat:	Utility
Desc:	filter out non-printable ascii characters
Port:	stdin text r req	ascii input
Port:	stdout text w req	ascii output

[farith]

Cat:	Utility
Desc:	perform simple arithmetic with binary files
Port:	stdin bin r req		input containing floats
Port:	stdout bin w req	output containing floats
Port:	in2 bin r -		optional 2nd input file, or scalar
Port:	par par r -		optional par file input
Param:	n int 1<= size-of-stdin	fast dimension (op=cartprod)
Param:	isig int 1<= -		index at which signum acts (op=signum)
Param:	op enum-farith - noop	arithmetic operation to perform on input

[enum-farith]
Desc:	arithmetic operations for farith
noop	out = in
neg	out = -in
abs	out = abs(in)
exp	out = exp(in)
log	out = ln(in) (natural logarithm)
sqrt	out = (signed) sqrt(in)
sqr	out = in*in
pinv	out = (punctuated) 1/in
pinvsqr	out = (punctuated) 1/(in*in)
pinvsqrt out = (punctuated signed) 1/sqrt(in)
add	out = in + in2
sub	out = in - in2
mul	out = in*in2
div	out = in/in2
cartprod out = in x in2 (requires parameter n=)
signum	out[i] = { in[i] for i<isig; -in[i] for i >=isig }
slowp	out = 1/in - 1/in2 (slowness perturbation)
slothp	out = 1/in^2 - 1/in2^2 (sloth perturbation)

[maxdiff]

Cat:	Utility
Desc:	report absolute maximum difference in two SU data sets
Trans:	PositionalArgs
Port:	file-1 su r req		first data set
Port:	file-2 su r req		second data set
Port:	stdout text w /dev/tty	reported difference

[pause]

Cat:	Utility
Desc:	prompt and wait for user to signal to continue
Trans:	PositionalArgs
Param:	prompt-1 string - continue	prompt string to display

[psmanager]

Cat:	Utility
Cat:	Plotting
Desc:	manager for PostScript printing on HP 4MV and 5Si Mx Laserjet printers
Port:	stdin ps r req		input PostScript file to print
Port:	stdout ps w req		output
Port:	par par r -		optional par file input
Param:	papersize enum-papersize 0-3 0  paper size
Param:	orient int 0<=1 0	0 = portrait, 1 = landscape
Param:	tray int 1-3 3		HP paper tray: 3 = default bottom tray
Param:	manual int 0<=1 0	if 1, manual feed for tray=1
Param:	media enum-media 0-9 0	printing material used: 0 = regular paper
LDesc:	media parameter applies only to HP 5Si Mx Laserjet printer

[enum-papersize]

Desc:	papersize for HP printer
0	US letter (default)
1	US legal
2	A4
3	11x17

[enum-media]
Desc:	printing material used
0	regular paper (default)
1	transparency
2	letterhead
3	card stock
4	bond
5	labels
6	prepunched
7	recycled
8	preprinted
9	colored paper

[rmaxdiff]

Cat:	Utility
Desc:	report relative (percentage) maximum difference in two SU data sets
Trans:	PositionalArgs
Port:	file-1 su r req		first data set
Port:	file-2 su r req		second data set
Port:	stdout text w /dev/tty	reported difference

[sumax]

Cat:	Utility
Desc:	get trace by trace local/global maxima, minima, or absolute maxima
Port:	stdin su r req		trace input
Port:	stdout su w -		trace output, depending on output parameter
Port:	outpar par w /dev/tty	output par file from verbose or output=ascii
Port:	par par r -		optional par file input
Param:	output enum-sumax-1 - ascii	output file choice
Param:	mode enum-sumax-2 - maxmin	type of output
Param:	verbose int 0<=1 0	if 1, write extra information to outpar
LDesc:	If verbose=0, write global quantities to outpar.  If verbose=1,
LDesc:	output trace number, values, sample location.

[enum-sumax-1]
Desc:	output file choice for sumax
ascii	write ascii data to `outpar' par file
binary	write binary floats to stdout
segy	write SU traces to stdout

[enum-sumax-2]
Desc:	choice of output for sumax
maxmin	output both maxima and minima
max	output maxima only
min	output minima only
abs	output absolute maxima

[sumean]

Cat:	Utility
Desc:	get the mean values of data traces
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	outpar par w /dev/tty	output par file for mean values
Port:	par par r -		optional par file input
Param:	power float 0<= 2.0	raise samples to this power before taking mean
Param:	verbose int 0<=1 0	type of output to outpar
LDesc:	If verbose=0, write mean value of section to outpar.
LDesc:	If verbose=1, write mean value of each trace/section to outpar.

[supickamp]

Cat:	Utility
Desc:	pick amplitudes within user defined and resampled window
Port:	stdin su r req		trace input
Port:	stdout bin w req	ascii or binary output
Port:	outpar par w /dev/tty	output par file holding pick information
Port:	par par r -		optional par file input
Port:	t_xabove bin r -	time and x values for upper window corner
Port:	t_xbelow bin r -	time and x values for lower window corner
Param:	d2 float 0< -		sampling interval for slow dimension
Param:	x_above float-list - -	array of x values for upper window corner
Param:	t_above float-list - -	array of time values for upper window corner
Param:	wl float 0<= -		window width (none if not specified)
Param:	dt_resamp float 0< tr.dt resampling interval in pick window
Param:	tmin float - 0.0	minimum time on input trace (sec)
Param:	x2beg float - 0.0	first lateral position
Param:	format enum-pickamp - ascii	output format choice
Param:	verbose int 0<=1 1	if 1, write complete pick information to outpar
Param:	key enum-thed - -	header word specifying trace offset (x value)
Param:	arg1 string - max	output (1st dimension) to stdout
Param:	arg2 string - i2	output (2nd dimension) to stdout

[enum-pickamp]
Desc:	output format choice for supickamp
ascii	write ascii data to stdout
binary	write binary floats to stdout

[sushift]

Cat:	Utility
Desc:	shift/window traces in time
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	tmin float - "from first trace" minimum time to pass (sec)
Param:	tmax float - "from first trace" maximum time to pass (sec)
Param:	verbose int 0<=1 1	if 1, echo parameters

[tolog]

Cat:	Utility
Desc:	redirect text to log file (that is, to stderr)
Trans:	PositionalArgs
Port:	textfile-1 text r req	text input

[xless]

Cat:	Utility
Desc:	browse text with LESS in Xterm window
Trans:	PositionalArgs
Port:	textfile-1 text r req	text input
Param:	title-2 string - xless	xterm window title

