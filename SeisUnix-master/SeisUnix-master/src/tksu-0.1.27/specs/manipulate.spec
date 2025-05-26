# manipulate.spec --
#
#	Tksu trace manipulation category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[recip]

Cat:	Trace Manipulation
Desc:	sort then sum opposing offsets in cdp data
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output

[sucommand]

Cat:	Trace Manipulation
Desc:	pipe traces having the same key header word to command
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	key enum-thed - cdp	header key word to pipe on
Param:	command string - "suop nop" command piped into
Param:	dir enum-command - 0	header key trigger choice
Param:	verbose int 0<=1 0	if 1, report diagnostics

[enum-command]
Desc:	action that causes a new command to be executed (sucommand)
0	change of header key value
-1	break in monotonic decrease of header key
1	break in monotonic increase of header key

[suflip]

Cat:	Trace Manipulation
Desc:	flip a seismic section in various ways
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	flip enum-flip - 1	choice of flip operation
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()
Param:	verbose int 0<=1 0	if 1, report diagnostics

[enum-flip]
Desc:	suflip operation choice
1	flip 90 degrees clockwise (tr.dt is lost)
-1	flip 90 degrees counter-clockwise (tr.dt is lost)
0	transpose data
2	flip right-to-left
3	flip top-to-bottom

[suop]

Cat:	Trace Manipulation
Desc:	do unary arithmetic operations on SU traces
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	op enum-unop - abs	unary operation choice

[enum-unop]
Desc:	unary operation choice
abs	absolute value
ssqrt	signed square root
sqr	square
ssqr	signed square
sgn	signum function
exp	exponentiate
slog	signed natural logarithm
slog10	signed common logarithm
cos	cosine
sin	sine
tan	tangent
cosh	hyperbolic cosine
sinh	hyperbolic sine
tanh	hyperbolic tangent
norm	divide trace by its max abs value
db	20 * slog10()
neg	negate value
posonly	clip negative values to zero
negonly	clip positive values to zero
nop	no operation

[suop2]

Cat:	Trace Manipulation
Desc:	do a binary operation of two SU data sets
Trans:	PositionalArgs
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set
Port:	par par r -		optional par file input
Param:	op enum-binop - diff	binary operation to apply

[enum-binop]
Desc:	Binary operation to apply:  data(1) `op' data(2)
diff	difference of two panels (data(1) - data(2))
sum	sum of two panels
prod	product of two panels
quo	quotient of two panels (data(1) / data(2))
ptdiff	difference of panel data(1) and single trace data(2)
ptsum	sum of panel with single trace
ptprod	product of panel with single trace
ptquo	division of panel by single trace

[sudiff]

Cat:	Trace Manipulation
Desc:	take difference of two data sets (uses suop2)
Trans:	PositionalArgs
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[supermute]

Cat:	Trace Manipulation
Cat:	Sorting
Desc:	permute or transpose a 3D data cube
Port:	stdin su r req		trace input
Port:	stdout su w req		trace output
Port:	par par r - 		optional par file input
Param:	n1 int 0< tr.ns		1st (fast) dimension of input
Param:	n2 int 0< tr.ntr	2nd (mid) dimension of input
Param:	n3 int 0< 1		3rd (slow) dimension of input
Param:	o1 int 1<=3 1		input dim to be made the 1st (fast) output dim
Param:	o2 int 1<=3 2		input dim to be made the 2nd (med) output dim
Param:	o3 int 1<=3 3		input dim to be made the 3rd (slow) output dim
Param:	d1 float 0< 1		output interval in new fast direction
Param:	d2 float 0< 1		output interval in new med direction
Param:	d3 float 0< 1		output interval in new slow direction

[suprod]

Cat:	Trace Manipulation
Desc:	take product of two data sets (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suquo]

Cat:	Trace Manipulation
Desc:	take quotient of two data sets (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suptdiff]

Cat:	Trace Manipulation
Desc:	take difference of data set with single trace (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suptsum]

Cat:	Trace Manipulation
Desc:	take sum of data set with single trace (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suptprod]

Cat:	Trace Manipulation
Desc:	take product of data set with single trace (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		data set
Port:	data-2 su r req		single trace
Port:	stdout su w req		resultant data set

[suptquo]

Cat:	Trace Manipulation
Desc:	take quotient of data set with single trace (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		data set
Port:	data-2 su r req		single trace
Port:	stdout su w req		resultant data set

[suramp]

Cat:	Trace Manipulation
Desc:	linearly taper the start and/or end of traces to zero
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	tmin float - -		end of starting ramp (sec)
LDef:	tr.delrt/1000
Param:	tmax float - -		beginning of ending ramp (sec)
LDef:	(tr.nt - 1)/dt
Param:	dt float 0< "from header" sampling interval (sec)

[surecip]

Cat:	Trace Manipulation
Desc:	sum opposing offsets in prepared data
Port:	stdin su r req 		trace input
LDesc:	Input traces should first be sorted by cdp and offset.
Port:	stdout su w req 	trace output

[susum]

Cat:	Trace Manipulation
Desc:	take sum of two data sets (uses suop2)
Trans:	PositionalArgs
DocCmd:	sudoc sudiff
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suswapbytes]

Cat:	Trace Manipulation
Desc:	change SU trace byte order from big- to little-endian, and vice versa
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	format int 0<=1 0	0 = foreign to native, 1 = native to foreign
Param:	ns int 1<= "from header" number of samples per trace

[swapbytes]

Cat:	Trace Manipulation
Desc:	change the endian-ness of various data types
Port:	stdin bin r req 	binary input
Port:	stdout bin w req 	binary output
Port:	outpar par w /dev/tty	output par file containing sample count (n1)
Port:	par par r -		optional par file input
Param:	in enum-swapbytes - float data type to swap

[enum-swapbytes]
Desc:	choice of data types for swapbytes
float	4 bytes
double	8 bytes
short	2 bytes
ushort	2 bytes (unsigned short)
long	4 bytes
ulong	4 bytes (unsigned long)
int	4 bytes

[sutaper]

Cat:	Trace Manipulation
Desc:	taper the edge traces of a seismic section to zero
Port:	stdin su r req		trace input (must be a file)
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	ntaper int 1<= 5	taper width at edge (number of traces)

[suvcat]

Cat:	Trace Manipulation
Desc:	vertically concatenate data(2) onto data(1), trace by trace
Trans:	PositionalArgs
Port:	data-1 su r req		first data set
Port:	data-2 su r req		second data set
Port:	stdout su w req		resultant data set

[suzero]

Cat:	Trace Manipulation
Desc:	zero out samples within a time window
Port:	stdin su r req		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	itmin int 0<= 0		first time sample to zero out
Param:	itmax int 0<= req	last time sample to zero out (inclusive)

[transp]

Cat:	Trace Manipulation
Cat:	Sorting
Desc:	transpose an n1 by n2 element matrix
Port:	stdin bin r req 	binary input
Port:	stdout bin w req 	binary output
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of elements in 1st (fast) dimension
Param:	n2 int 1<= all		number of elements in 2nd (slow) dimension
Param:	nbpe int 1<= 4		number of bytes per element
Param:	verbose int 0<=1 0	if 1, report diagnostics

