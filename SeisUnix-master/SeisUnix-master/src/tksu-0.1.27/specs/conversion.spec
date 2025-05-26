# conversion.spec --
#
#	Tksu file conversion category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[a2b]

Cat:	Conversion
Desc:	convert ascii floats to binary 				
Port:	stdin text r req	ascii file containing float values
Port:	stdout bin w req	binary file containing same float values
Port:	outpar par w /dev/tty	par file reporting line count (n=)
Port:	par par r -		optional par file input
Param:	n1 int 1<= 2		floats per line in input file

[b2a]

Cat:	Conversion
Desc:	convert binary floats to ascii				
Port:	stdin bin r req		binary file containing float values
Port:	stdout text w req	ascii file containing same float values
Port:	outpar par w /dev/tty	par file reporting line count (n=)
Port:	par par r -		optional par file input
Param:	n1 int 1<= 2		floats per line in output file

[bhedtopar]

Cat:	Conversion
Desc:	convert a binary tape header file to PAR file format	
Port:	stdin bhed r req 	input binary header
Port:	outpar par w /dev/tty	output par file
Port:	par par r -		optional par file input
Param:	swap int 0<=1 0		set to 1 to swap bytes

[dt1tosu]

Cat:	Conversion
Desc:	convert GPR data in X.dt1 format to SU format
Port:	stdin dt1 r req 	input dt1 file
Port:	stdout su w req 	trace output
Port:	outpar par w /dev/tty	output par file for binary header
Port:	par par r -		optional par file input
Param:	ns int 1<= "from header"	samples per trace
Param:	dt float 0< 0.8		sample rate (ns)
Param:	swap int 0<=1 0		set to 1 to swap bytes
Param:	verbose int 0<=2 0	1,2 = write header vals to outpar
Param:	list int 0<=1 0		1 = write header val explanation to stderr

[ftnstrip]

Cat:	Conversion
Desc:	convert Fortran binary records (with delimiters) to raw binary
Port:	stdin bin r req 	input Fortran records
Port:	stdout bin w req 	output binary stream (no record delims)

[ftnunstrip]

Cat:	Conversion
Desc:	convert raw binary stream to Fortran records (with delimiters)
Port:	stdin bin r req 	input binary stream (no record delims)
Port:	stdout bin w req 	Fortran records with record length n1
Port:	outpar par w /dev/tty	par file reports # of records out (n=)
Port:	par par r -		optional par file input
Param:	n1 int 1<= 1		number of elements per record

[h2b]

Cat:	Conversion
Desc:	convert 8 bit hexadecimal floats to binary		
Port:	stdin text r req 	binary data as from Postscript bitmaps
Port:	stdout bin w req 	output binary data
Port:	outpar par w /dev/tty	par file reporting line count (n=)

[mkparfile]

Cat:	Conversion
Desc:	convert ascii to par file format
Port:	stdin text r req	ascii input (see mkparfile help)
Port:	stdout par w req	par file output
Port:	par par r -		optional par file input
Param:	string1 string - par1	first par keyword
Param:	string2 string - par2	second par keyword

[recast]

Cat:	Conversion
Desc:	convert from one data type to another
Port:	stdin bin r req		binary input of type `in'
Port:	stdout bin r req	binary output of type `out'
Port:	outpar par w /dev/tty	output par file containing sample count (n1)
Port:	par par r -		optional par file input
Param:	in enum-recast - float	input sample data type
Param:	out enum-recast - double output sample data type

[enum-recast]
Desc:	Data types for recast
float	4-byte float
double	8-byte double
int	4-byte integer
char	signed 1-byte integer
uchar	unsigned 1-byte integer
short	2-byte integer
long	4-byte integer
ulong	unsigned 4-byte integer

[segdread]

Cat:	Conversion
Desc:	convert SEG-D format to SU trace format
Port:	tape segd r req		SEG-D input
Port:	stdout su w req		SU trace output
Port:	par par r -		optional par file input
Param:	use_stdio int 0<=1 0	0 = record devices; 1 = pipe, disk devices
Param:	verbose int 0<=2 0	1 = report every vblock traces; 2 = block info
Param:	vblock int 0<= 50	trace report interval
Param:	ptmin int 1<= 1		first shot to read
Param:	ptmax int 1<= INT_MAX	last shot to read
Param:	gain int 0<=1 0		gain flag:  1 = apply gain
Param:	aux int 0<=1 0		aux flag: 1 = recover auxiliary traces
Param:	errmax int 0<= 0	number of consecutive tape IO errors allowed
Param:	ns int 0<= 0		if > 0, replace computed ns with this value

[segyclean]

Cat:	Conversion
Desc:	zero out unassigned portion of trace header
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output

[segyhdrs]

Cat:	Conversion
Desc:	make SEGY ascii and binary headers for segywrite
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	hfile ahed w header	ascii header output
Port:	bfile bhed w binary	binary header output
Port:	par par r -		optional par file input
Param:	ns int 1<= "from header"	samples per trace
Param:	dt float 0< "from header"	sample rate (us)
Param:	enum-bhed int - -	key=value settings for binary header

[segyread]

Cat:	Conversion
Desc:	read SEGY tape
Port:	tape segy r req		SEGY file input
Port:	stdout su w req 	trace output
Port:	hfile ahed w header	ascii header output
Port:	bfile bhed w binary	binary header output
Port:	par par r -		optional par file input
Param:	verbose int 0<=1 0	1 = report every vblock traces
Param:	vblock int 0<= 50	trace report interval
Param:	buff int 0<=1 1		1 = buffered device
Param:	over int 0<=1 0		1 = override bad bhed format value
Param:	format int - 0		format value to use when over=1
Param:	conv int 0<=1 1		1 = convert samples to IEEE format
Param:	ns int 1<= "from header"	samples per trace
Param:	trmin int 1<= 1		first trace to read
Param:	trmax int 1<= -		last trace to read
Param:	endian int 0<=1 1	0 = little-endian machine
Param:	errmax int 0<= 0	number of consecutive tape IO errors allowed
Param:	remap enum-thed-list - - header fields to remap to
Param:	byte string-list - -	header fields (and formats) to remap from

[segywrite]

Cat:	Conversion
Desc:	write SEGY tape
Port:	stdin su r req 		trace input
Port:	tape segy w req		SEGY file output
Port:	hfile ahed r header	ascii header input
Port:	bfile bhed r binary	binary header input
Port:	par par r -		optional par file input
Param:	verbose int 0<=1 0	1 = report every vblock traces
Param:	vblock int 0<= 50	trace report interval
Param:	buff int 0<=1 1		1 = buffered device
Param:	conv int 0<=1 1		1 = convert samples to IBM format
Param:	trmin int 1<= 1		first trace to write
Param:	trmax int 1<= -		last trace to write
Param:	endian int 0<=1 1	0 = little-endian machine
Param:	errmax int 0<= 0	number of consecutive tape IO errors allowed
Param:	format int - -		override format value in binary header

[setbhed]

Cat:	Conversion
Desc:	create binary header file from key=value parameters
Port:	bfile bhed w binary	binary header output
Port:	par par r -		optional par file input
Param:	enum-bhed - - -		binary header key=value settings

[suaddhead]

Cat:	Conversion
Desc:	put headers on bare traces and set the tracl and ns fields
Port:	stdin bin r req 	bare trace input
Port:	stdout su w req 	su trace output
Port:	par par r -		optional par file input
Param:	ns int 1<= req		number of samples per trace (required)
Param:	ftn int 0<=1 0		unformatted data from C (0) or Fortran (1)

[suascii]

Cat:	Conversion
Desc:	print nonzero header values and data
Port:	stdin su r req 		trace input
Port:	stdout text w - 	ascii text output
Port:	par par r -		optional par file input
Param:	bare enum-suascii - 0	choice of output

[enum-suascii]
Desc:	select suascii output
0	print headers and data
1	print data only
2	print headers only

[suget]

Cat:	Conversion
Desc:	connect SU program to file descriptor for input stream.
Port:	stdout su w req		trace output
Port:	par par r -		optional par file input
Param:	fd int -1<=		file descriptor for input stream
Param:	verbose int 0<=1 0	if 1, provide message with each trace

[suoldtonew]

Cat:	Conversion
Desc:	convert su traces to xdr format
Port:	stdin su r req		trace input
Port:	stdout xdr w req	trace output in xdr format

[supaste]

Cat:	Conversion
Desc:	paste SEGY headers onto headerless traces
Port:	stdin bin r req		headerless trace input (floats)
Port:	head thed r headers	trace headers from sustrip
Port:	par par r -		optional par file input
Param:	ns int 1<= req		number of samples per trace
Param:	ftn int 0<=1 0		if 1, stdin consists of Fortran records
Param:	verbose int 0<=1 0	if 1, echo number of traces pasted

[suput]

Cat:	Conversion
Desc:	connect SU program to file descriptor for output stream
Port:	stdin su r req		trace input
Port:	par par r -		optional par file input
Param:	fd int -1<=		file descriptor for output stream
Param:	verbose int 0<=1 0	if 1, provide message with each trace

[sustrip]

Cat:	Conversion
Desc:	remove SEGY headers from SU traces
Port:	stdin su r req		trace input
Port:	stdout bin w req	headerless trace output
Port:	head thed w /dev/null	file to save headers in
Port:	outpar par w /dev/tty	output par file containing n1, n2, d1
Port:	par par r -		optional par file input
Param:	ftn int 0<=1 0		if 1, Fortran records are written to stdout

[triview]

Cat:	Conversion
Desc:	view 3D coordinates in 2D
Port:	stdin text r req	input list of 3D coordinates (ascii)
Port:	stdout text w req	output list of 2D coordinates (ascii)
Port:	par par r -		optional par file input
Param:	theta float 0<=180 0.0	viewing angle (elevation in degrees)
Param:	phi float 0<=360 0.0	viewing angle (azimuth in degrees)
Param:	xs0 float - 0.0		x translation of 2D coords
Param:	ys0 float - 0.0		y translation of 2D coords

[z2xyz]

Cat:	Conversion
Desc:	convert binary floats (Z) to ascii ordered triples (X Y Z)
Port:	stdin bin r req		binary (float) input
Port:	stdout text w req	ascii output
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of floats in 1st (fast) dimension

