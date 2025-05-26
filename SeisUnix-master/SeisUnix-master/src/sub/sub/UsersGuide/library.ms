.NH 2
Transcendental and Elementary Functions
.LP
Most of these are defined in the obvious way.
Trigonometric functions exect arguments in radians
(see the appropriate section 3 manual page for details).
.LP
Functions which claim to be \fIvectorized\fP can be applied
to multi-dimensional arrays of arbitrary complexity (!) subject
to the condition that each end-member of the array is a number.
(In other words, every element of the argument must be either a number
or another array and this requirement holds recursively.)
The return value is a new array of similar shape in which all of the
original numeric values have been replaced with the results of applying
the function to them.
.RS
.IP \fBsin(x)\fP 12
vectorized.
.IP \fBcos(x)\fP 12
vectorized.
.IP \fBatan(x)\fP 12
vectorized.
.IP \fBlog(x)\fP 12
vectorized.
.IP \fBlog10(x)\fP 12
vectorized.
.IP \fBexp(x)\fP 12
vectorized.
.IP \fBsqrt(x)\fP 12
vectorized.
.IP \fBint(x)\fP 12
vectorized;
truncates the argument by discarding any fractional part.
.IP \fBnint(x)\fP 12
vectorized;
rounds the argument to the nearest integer.
.IP \fBabs(x)\fP 12
vectorized.
.RE
.NH 2
Arithmetic Functions
.LP
.RS
.IP "\fBmax(\fIa, b, c, ...\fB)\fR"
returns the algebraic maximum of all of the numeric values
in all of the (arbitrarily complex) arguments.
There must be at least one argument.
.IP "\fBmin(\fIa, b, c, ...\fB)\fR"
Like \fBmax\fP but it returns the algebraic minimum.
.IP "\fBsum(\fIa, b, c, ...\fB)\fR"
Like \fBmin\fP but it returns the sum of all of the numeric
values.
.RE
.NH 2
Array Construction and Management Functions
.LP
.RS
.IP "\fBflatten(\fIa, b, c, ...\fB)\fR"
Returns a one-dimensional array constructed by flattening
all of its arguments.
Each argument can be either a single value or an array
of arbitrary complexity.
The original arrays are not altered
(since we have only call-by-value in Sub).
.IP
The values in the arrays need not be numeric.
If \fIall\fP of the end-members are numeric,
\fBflatten\fP returns an \fBfVector\fP;
otherwise it returns a conventional array.
.IP "\fBsequence(\fInumber\fB)\fR"
.IP "\fBsequence(\fIfrom, to\fB)\fR"
.IP "\fBsequence(\fIfrom, to, step\fB)\fR"
Each of these calls returns an initialized \fBfVector\fP.
The first form returns a vector initialized to the
values
.DS B
.B
.SM
1, ..., \fInumber\fP
.DE
The second returns a vector initialized to
.DS B
.B
.SM
\fIfrom\fP, \fIfrom\fP + 1, ..., \fIto\fP
.DE
The third returns a vector initialized to
.DS B
.B
.SM
\fIfrom\fP, \fIfrom\fP + \fIstep\fP, ..., \fIlast\fP
.DE
where \fIlast\fP is the final element in the sequence
which is not greater than \fIto\fP.
.IP "\fBfVector(\fIi\fB)\fR"
.IP "\fBfVector(\fIi, c\fB)\fR"
Returns an \fBfVector\fP with \fIi\fP elements.
If an optional second argument, \fIc\fP, is provided,
the array elements are initialized to that value,
otherwise they are initialized to 0.
.RE
.NH 2
Time Series Analysis
.LP
Time series analysis is currently limited to
forward and inverse digital fourier transform capability.
(Our \fIdft\fP's are based on routines written by Dave Hale,
and acquired from the Colorado School of Mines.)
Here's what we have:
.RS
.IP "\fBpfnext(\fIn\fB)\fR"
Our fourier transform code is limited to series lengths
with certain properties (see the source code for more
on this).
(It is not limited to just powers of two, however.)
This function accepts an arbitrary series length, \fIn\fP,
and returns the first \fIacceptable\fP length
which is not less than \fIn\fP.
We provide another function, \fBextend()\fP below,
which can be called to pad a series to the desired length.
.IP "\fBpfbest(\fIn\fB, \fInmax\fB)\fR"
This function accepts an initial series length, \fIn\fP,
and a maximum acceptable length, \fInmax\fP,
for an extended series
and returns an acceptable series length
which is at least as large as \fIn\fP,
is no larger than \fInmax\fP,
and which we believe (based on timing tests conducted
by the dft code's author)
will provide the most eficient calculation.
.IP "\fBextend(\fIy\fB, \fIn\fB [, \fIval\fB])\fR"
returns a  new series derived from \fIy\fP
which has \fIn\fP elements.
If the new series is longer than \fIy\fP,
the new elements are initialized to \fIval\fP
if \fIval\fP is provided,
and \fB0\fP otherwise.
.IP "\fBfdft(\fIy\fB)\fR"
accepts an input time series, which may be real or complex
(see below),
and returns its fourier transform.
The returned value consists of a two-element array;
the first element of the array is an \fBfVector\fP
which is the \fIreal\fP part of the transform,
and the second element of the array is the \fIimaginary\fP
part of the transform.
.IP
The input series, \fBy\fP,
can be either an \fBfVector\fP,
in which case it is taken to have no imaginary component,
or it can be a two-element array each of the components of which
is an array of real values,
in which case the arrays are interpreted just as in the above.
(Both \fBfVector\fPs and conventional arrays are acceptable.)
.IP "\fBidft(\fIy\fB)\fR"
provides the inverse of \fBfdft()\fP.
It's behavior is identical to that described above
except that the inverse transform is computed
and the results are divided by the number of samples in the series,
so that the combination of a forward and an inverse transform
is the identity operation.
.RE
.LP
Check out the examples given in a later section.
.NH 2
Function Argument Handling
.LP
Sub
provides two library functions that allow
a user-defined Sub function to determine the number
and value of its arguments.
.RS
.IP \fBnargs()\fR
returns the number of arguments passed to the current function.
.IP \fBntharg(\fIi\fB)\fR
returns the value of the \fIi\fPth argument.
\fBNtharg(0)\fP is the name of the current function;
\fBntharg(1)\fP is the value of the first argument.
It is an error to ask for arguments that aren't there.
.RE
.NH 2
Command Line Arguments
.LP
The Sub library currently lacks slick support
for parsing command-line arguments
(the stuff on the command line after the name
of the script file).
We do, however, provide two global variables:
.RS
.IP \fBArgc\fR
contains the number of command line arguments,
counting the name of the script file as the first.
This value is always greater than 0.
.IP \fBArgv\fR
is an array of strings holding the values of the command line arguments.
\fBArgv(0)\fP is the name of the script file and
\fBargv(1)\fP is the first subsequent argument (if any).
.RE
.NH 2
Type Predicates
.LP
These are functions that provide information
about the type (as opposed to the value)
of the current contents of a variable.
We don't currently cover all possible types
(but we could - it's just not a high priority).
.RS
.IP \fBisDouble(\fIarg\fB)\fP
returns true (1) if \fIarg\fP currently
holds a value of type \fBDouble\fP.
.IP \fBisString(\fIarg\fB)\fP
.IP \fBisNothing(\fIarg\fB)\fP
.IP \fBisArray(\fIarg\fB)\fP
.IP \fBisfVector(\fIarg\fB)\fP
.RE
.NH 2
Input/Output
.NH 3
Formatted I/O
.RS
.IP \fBfgets(\fIstream\fB)\fR
reads characters from \fIstream\fP until
end-of-file or a newline is encountered.
Returns a string with all of the characters read
(including a terminating newline if one was encountered).
.IP \fBprint(a,b,...)\fR
writes formatted forms of the arguments \fIa\fR, \fIb\fR,
\fIetc\fR to \fBstderr\fR.
.IP "\fBfprint(stream, a, b, ...)\fR"
writes formatted forms of \fIa\fR, \fIb\fR, \fIetc\fR,
to \fIstream\fR.
.RE
.NH 3
Token I/O
.RS
.IP "\fBgetTokenLine(\fIstream\fB)\fR"
.IP "\fBgetTokenLine(\fIstream, separators\fB)\fR"
The first form of this function reads the next line of input
from \fIstream\fP,
breaks it into tokens (see below),
and returns an array with one element for each token.
On end-of-file,
this function returns the value \fBNothing\fP.
.IP
A token is normally any set of characters bounded by whitespace
(spaces, tabs, and newlines).
In the second form,
the token separator characters are taken to be any characters
in the string \fIseparators\fR.
.RE
.NH 3
Record I/O
.RS
.IP "\fBfputrb(\fIobj1, obj2, ...\fB)\fR"
.IP "\fBfputrb(\fIstream, obj1, obj2, ...\fB)\fR"
sends each of the records \fIobj1\fP, \fIobj2\fP, ...,
to \fBstdout\fP.
As a special case,
if the first argument is a \fBstream\fP
(such as the value returned from \fBfopen()\fP),
that stream replaces \fBstdout\fP as the destination of the write.
.IP "\fBoutput(\fIobj\fB)\fR"
a synonym for \fBfputrb\fP
which is allowed for historical reasons.
This function will be removed one day.
.IP "\fBfgetrb()\fP
.IP "\fBfgetrb(\fIstream\fB)\fR"
reads the next record from \fIstream\fP
(\fBstdin\fP by default)
and returns it.
Returns \fBNothing\fP on end-of-file.
.RE
.NH 3
Opening and Closing Files
.RS
.IP "\fBfopen(filename, mode)\fR"
opens the file specified by the path \fIfilename\fR
for i/o in the direction specified by the string \fImode\fR
and returns a file pointer value suitable for passing to
\fBfprint\fR (below).
\fIMode\fR should be one of "r", "w", "a", "r+", "w+",
or "a+" where the quotes are required
(the first two are by far the most common);
see \fBfopen(3)\fR for more details.
.IP "\fBtmpfile()\fP"
Returns a \fBstream\fP opened in update mode
to a temporary file which will be automatically deleted
when the file is closed or the current process terminates.
.IP \fBfclose(stream)\fR
closes \fIstream\fR
(which must be a value returned by an earlier call of \fBfopen\fR).
.IP \fBfflush(stream)\fR
writes any buffered data to \fIstream\fR.
.RE
.NH 3
Opening and Closing Pipes
.RS
.IP "\fBpopen(cmd, mode)\fR"
opens a pipeline to the process \fIcmd\fR
in the direction (reading or writing) specified by \fImode\fR,
and returns a file pointer value suitable for passing to
\fBfprint\fR, \fIetc\fR.
.I Cmd
is executed to create the target process and can be any legal
\fBsh\fR(1) command string.
.I Mode
should be "w" for writing to the \fIcmd\fR
and "r" for reading from the \fIcmd\fR
(the quotes are required).
.IP \fBpclose(stream)\fR
closes \fIstream\fR
(which must be a value returned by an earlier call of \fBpopen()\fR).
This call closes the i/o stream and waits for the remote process
to exit
(see \fBpopen\fR(3)).
.RE
.NH 3
Manipulating Streams
.LP
These functions support various types of manipulation
of i/o streams.
Most of them correspond closely to entries
in the Unix manual section 3.
.RS
.IP "\fBfseek(\fIstream, offset, mode\fB)\fR"
positions \fIstream\fP to a new location determined
by \fIoffset\fP and \fImode\fP.
.RS
.IP "\fImode\fB == 0\fR"
new location is at \fIoffset\fP measured from the beginning
of the stream.
.IP "\fImode\fB == 1\fR"
new location is at \fIoffset\fP measured from the
current location.
.IP "\fImode\fB == 2\fR"
new location is at \fIoffset\fP measured from the
end of the stream.
.RE
.IP "\fBftell(\fIstream\fB)\fR"
returns the current offset of the
stream from its beginning.
.IP "\fBrewind(\fIstream\fB)\fR"
Equivalent to \fBfseek(\fIstream\fB,0, 1)\fP.
.IP "\fBfskip(\fIstream, bytes\fB)\fR"
Skips \fIbytes\fP on \fIstream\fP.
\fIStream\fP must be opened for input.
.RE
.NH 3
Bytewise I/O
.LP
.RS
.IP "\fBfgetbytes(\fIstream, n\fB)\fR"
Reads up to \fIn\fP bytes from \fIstream\fP,
which must be opened for input.
If any bytes are read,
this functions returns an array with one numeric
element for each byte.
Otherwise it returns the value \fBnothing\fP.
.IP "\fBfputbytes(\fIstream, a\fB)\fR"
Writes one byte to \fIstream\fP for each element
in the array \fIa\fP.
\fIStream\fP must be opened for output
and \fIa\fP must be an array of numeric values.
.RE
.NH 2
Miscellaneous Functions
.LP
.RS
.IP \fBtime()\fR
returns the current wall clock time in (double) seconds
since 0:00 GMT, January 1, 1970.
Granularity is system dependent
(see \fBgettimeofday(2)\fR).
.IP \fBrandom()\fR
.IP \fBrandom(\fIn\fB)\fR
returns a random number in
the half-open interval [0,1);
if the optional argument \fIn\fP is provided,
returns an \fBfVector\fP with \fIn\fP
random elements.
(The current random number generator is based upon
freely available code discussed in the source files.)
.IP \fBexit(x)\fR
causes \fBsub\fR to teminate and return the value \fIx\fR
to the shell that invoked it.
By convention \fBexit(0)\fR denotes success and
any other value indicates failure.
A script which exits by falling off the end
returns \fB0\fR.
.IP \fBsystem(\fIs\fB)\fR
executes the shell command \fIs\fP,
which must be a string,
waits until the command has completed,
and then returns the exit status of the shell
which invoked the command.
See \fBsystem(3)\fP.
.IP \fBstrlen(s)\fR
returns the number of characters in the string \fIs\fR
(actually a synonym for \fBsize()\fR).
.IP \fBsize(x)\fR
returns the size of \fIx\fR
(this might be useful in detecting bad header sizes, \fIetc\fR).
If \fIx\fR is a string
the returned value is the length of the string
(the trailing nul is not counted).
If \fIx\fR is an SEGY trace record
the returned value is the size of the record in bytes.
If \fIx\fR is an array
(either conventional or \fBfVector\fP),
the returned value is the number of elements.
If \fIx\fR has the type \fBNothing\fR,
the returned value is \fB0\fP.
In all other caes,
the returned value is \fB1\fP.
.IP \fBstrtonum(s)\fR
interprets the contents of \fIs\fR
(which had better be a string)
as a number and returns its value.
If \fIs\fR does not at least begin with a legitimitate numeric value,
this function will abort with an error message.
.IP "\fBfloattostr(f, fmt)\fR"
converts the value of \fIf\fR,
as a floating-point number,
into a string using the \fBprintf(3)\fR format
in the string \fIfmt\fR.
\fIFmt\fR should contain a floating-point format
string such as "%g", \fIetc\fR.
.IP "\fBinttostr(i, fmt)\fR"
converts the value of \fIf\fR,
as n integer,
into a string using the \fBprintf(3)\fR format
in the string \fIfmt\fR.
\fIFmt\fR should contain an integer format
string such as "%d", \fIetc\fR.
.RE
.NH 2
Predefined Values
.LP
We provide a set of constant values accessible through global variables.
.RS
.IP \fBpi\fR
3.14159265358979323846
.IP \fBenatural\fR
2.71828182845904523636
.IP \fBgamma\fR
0.57721566490153286060
(Euler's constant - for the snobs in the audience)
.IP \fBrad2deg\fR
57.29577951308232087860
(degrees per radian)
.IP \fBgolden\fR
1.61803398874989484820
(the golden mean - for the esthetes)
.IP \fBnothing\fR
the empty value, a pile of ashes.
.IP \fBhardware\fR
a string name for the hardware upon which we are running
(such as "sun", "cray").
.RE
.NH 2
Debugging Support
.IP \fBtraceback()\fP
causes the current function stack, including the values
of arguments on the stack, to be printed to \fBstderr\fP.
Execution resumes after the values are printed.
.IP \fBinternals(\fIselector\fB)\fR
returns the values of various internal quantities of interest
during debugging of the Sub interpreter.
These are surely of no interest to anyone else,
but here they are:
.RS
.IP \fBinternals(0)\fR
the current values of \fBsbrk(2)\fP.
.IP \fBinternals(1)\fR
the current alue of \fBnextTemp\fP from \fBopcode.c\fP.
.RE
