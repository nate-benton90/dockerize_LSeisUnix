.NH 2
Arrays
.LP
An array is a set of elements accessed \fIvia\fP subscripts.
Array subscripts start at zero (like C) and go up
(although you are free to pretend that they start at one).
Subscripts use \fB[]\fP and multiple subscripts require
multiple \fB[]\fP pairs: \fBx[i][j][7]\fP.
.LP
Sub support the special index value \fB$\fP,
which equals the index of the last element in the array.
Expressions like
.DS B
.SM
.B
	a[$+1] = 77;
.LG
.DE
.LP
are legal and add an element to the end of an array.
.LP
An array's elements need not all have the same type.
An array element may itself contain an array.
.LP
Referring to a subscripted element
(such as \fIx[10]\fR or \fIz[i][j][k]\fR)
causes \fIall\fP intermediate elements which do not
already exist to be created
and assigned the empty type, \fBNothing\fR.
This process is called \fBarray-infilling\fP.
exist.
Any newly created values are set to the value \fBnothing\fP.
Array-nfilling works for any number of indices
and is a sharp sword.
.LP
In order to provide efficient support for
manipulating time series,
Sub applies special rules to certain types of array
manipulations.
The rules are are simple,
but they require some understanding of what Sub
thinks it is being told to do.
.NH 2
Simple Assignment
.LP
A simple Sub assignment statement
.DS B
\fBa = b;\fP
.DE
stuffs a copy of whatever is in \fBb\fP into \fBa\fP.
This new value completely obliterates whatever
was in \fBa\fP before the assignment.
.B1
.IP "\fBException\fP"
If \fBa\fP is actually a record member with some particular nature,
such as \fBx.trace\fP when \fBx\fP holds a SEGY trace record,
Sub knows it must preserve the member's character;
we can't replace a trace with a string, for example.
In this instance it will do its best to find some sort of assigment
that makes sense and do it (or die trying).
.B2
.LP
Assigning into an array element works the same way,
subject to \fIarray-infilling\fP.
This
.DS B
\fBay[5] = 33;\fP
.DE
causes \fBay[5]\fP to be created if necessary
and its contents set to \fB33\fP.
Array-infilling makes sure that
.DS B
\fBay[0], ay[1], ..., ay[5]\fP
.DE
exist.
Any newly created values are set to the value \fBnothing\fP.
.NH 2
Subranges
.LP
If \fBa\fP
is a Sub array,
then
.RS
.IP "\fBa\fP"
denotes the entire array.
.IP "\fBa[2]\fP"
denotes a single element of the array.
.IP "\fBa[3:28]\fP"
denotes a sub-array of the entire array.
We refer to sub-arrays as \fIsubranges\fP.
.IP
We support default values for the two indices
that specify a subrange.
If the first index is missing,
it's value defaults to \fB0\fP.
If the second index is missing,
it's value defaults to the value of the
last index in the array.
.RE
.LP
Subranges are useful both as values
(on the right side of an assignment statement)
and as targets
(on the left side of an assignment statement).
.NH 3
Value of a Subrange
.LP
When we \fIevaluate\fP a subrange,
it's value is just the array formed
by the elements in the subrange.
Note that the sub-array's indices start at \fB0\fP,
as in this example:
.DS B
.B
x = z[3:7];
//	now 	x[0] == z[3]
//		x[1] == z[4]
//	etc.
.DE
.NH 3
Assignment to a Subrange
.LP
When we assign a value to a subrange,
the rules change.
If we assign a scalar into a subrange,
.DS B
.B
x[12:98] = 3.142;
.DE
the value of the scalar (here \fB3.142\fP)
is stored into each member of the subrange.
.LP
If \fBz\fP is an array with ten values,
each of these expressions
.DS B
.B
z[:] 	= 0.0;
z[0:9]	= 0.0;
z[0:$]	= 0.0;	// note that $ works here
z[:9]	= 0.0;
.DE
sets all of the members of \fBz\fP to zero.
.KS
.LP
If we assign an array into a subrange,
the rules are a little different.
In this case,
if the array is shorter than the subrange
to which it is being assigned,
the array is extended \fIby adding zeroes\fP
until it is long enough.
Note that we \fBdo not\fP replicate
the last value in the array.
Here's an example:
.DS B
.B
//	create an array with the values 1, 2, 3, 4, 5.

z = sequence(1,5);

//	create an array with 12 entries and set them all to 1.

a = sequence(1,12);
a[:] = 1;

//	a now has twelve members, each holding the value 1.
//
//	now assign into the 7-element subrange a[2:8]

a[2:8] = z;

//	here's what a has now:
//		a[0] = a[1] = 1		unchanged
//		a[2] = 1,...		from z
//		a[6] = 5		from z
//		a[7] = 0		from z's extension
//		a[8] = 0		from z's extension
//		a[9] = ... = a[11] = 1	unchanged
.DE
.KE
.NH 2
fVectors
.LP
Sub
has two different types of arrays.
Conventional arrays can hold any collection
of Sub values,
including other arrays.
When you create an array by assignment,
.DS B
.B
b[22] = "help!";
.DE
you create a conventional array.
.LP
An \fBfVector\fP
is a special type of array that
contains only floating-point values.
These arrays can only be created by assignment
(the library contains functions to create new ones for you)
or by accessing one in a data record.
These arrays obey the normal rules for Sub arrays
but they also offer support for high-level numerical manipulation.
.KS
.NH 3
Manipulating fVectors
.LP
fVectors
currently understand how to add, subtract, multiply, and divide
with either another fVector or a scalar.
Each of these operations is executed component-wise.
Here are some examples:
.DS B
.B
mytr = Tr.trace;
//
//	mytr now holds an fVector
//
//	this will compute it's mean value and then subtract
//	it from the trace.
//
meanv = sum(mytr) / length(mytr);
mytr = mytr - meanv;
//
//	note that we subtracted a scalar (meanv) from a trace (mytr)
//	producing a new trace with zero mean.
//
//	Here's an estimate of the integral over [0,1] if
//		sqrt(exp{y(t)}) dt
//	give or take a few scale factors, using the vector-capability
//	of the transcendental functions.
//
estimated_integral = sum(sqrt(exp(mytr))) / size(mytr);
//
//	etc.
.DE
.LP
The library functions
\fBfVector\fP,
\fBsequence\fP,
and
\fBflatten\fP
are useful for creating
\fBfVector\fPs.
.KE
