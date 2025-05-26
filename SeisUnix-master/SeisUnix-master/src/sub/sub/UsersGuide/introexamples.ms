.LP
The quickest way to get an idea of what
Sub
is useful for is to examine some simple uses.
If we execute
.DS B
.SM
.B
sub stdscript1 < sample.segy
.DE
where \fBstdscript1\fP is a file containing
the text
.DS B
.SM
.B
.so stdscript1.inc
.DE
then we get the output (to standard error)
.DS B
.SM
.B
.so std1
.DE
.LP
This example shows several important characteristics
of a Sub script.
.IP "\fB(1) Scripts structured by functions:\fP"
The contents of \fBstdscript1\fP
specify a program which Sub interprets and executes.
We call such a program a \fIscript\fP.
This script consists of several function definitions:
\fBBegin\fP, \fBOnTrace\fP, and \fBEnd\fP.
These are automatically invoked (if they exist)
by the interpreter at different points in the program's life:
.RS
.IP
.B Begin()
is invoked before anything has been read from standard input.
.IP
.B OnTrace()
is invoked each time a SEGY trace record is read.
Further,
the contents of the record are made available in the
global variable \fBTr\fP.
.IP
.B End()
is invoked after an end-of-file has been encountered on
standard input.
.RE
.IP "\fB(2) Convenient data element access:\fP"
The script shows how components of the SEGY trace record
are invoked as \fImembers\fP of the trace record.
In this example \fBTr.trace\fP is the trace member of the
SEGY trace record contained in \fBTr\fP.
Accessing the member
\fBTr.dt\fP would return the (integer) sample interval in microseconds.
(See Appendix A for a complete list of SEGY trace record members.)
.IP "\fB(3) Convenient trace processing operations:\fP"
When we computed the sum of the squared trace values,
we exploited Sub's support for vector-based operations.
In this particular instance
where \fBTr.trace\fP is a vector of \fIn\fP elements,
the expression
.DS B
.SM
.B
Tr.trace * Tr.trace
.DE
results in a new vector of \fIn\fP elements,
each element of which is the square of the corresponding
element in \fBTr.trace\fP.
(This definition is \fBnot\fP the dot product of two vectors;
it is the element-by-element product.
To compute the dot product of two vectors, \fBa\fP and \fBb\fP,
use \fBsum(a * b)\fP.)
.LP
In the above
Sub
scanned the incoming SEGY stream and extracted information
from the traces contained in it.
We could just as easily have modified the values in the stream
and passed the new values onward in the data steam.
Here is a script that normalizes each trace so that its
average squared sample value is one and then
passes the modified data onward.
.DS B
.SM
.B
.so leadscript1.inc
.DE
If we execute
.DS B
.SM
.B
sub leadscript1 < sample.segy | sub stdscript1
.DE
then we get the output (to standard error)
.DS B
.SM
.B
.so lead1
.DE
.LP
In this example the (modified) SEGY data form the first script
(\fBleadscript1\fP)
is piped on a second copy of Sub running the script form the first
example.
The text output we see at the terminal
comes from the second script.
