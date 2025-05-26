.LP
This section summarizes the properties
of Sub's \fIC-like\fP scripting language.
Sub gives special attention to arrays,
and particularly arrays of floating-point values.
That topic is so important that we've moved it
to a section of it's own.
.NH 2
Names
.LP
Variable and function names are limited by the same
restrictions as \fBC\fR.
Names are case-independent.
The only time \fBSub\fR retains alphabetic case
is in the contents of \fIstrings\fR.
.NH 2
Variables and Types
.LP
.B Sub
variables are always regular (named) variables.
Variables are  global by default.
They are created simply by being referenced.
.LP
The type of a variable is simply the type of
whatever value was last assigned to it.
A variable may assume many different types
during a computation.
The possible contents of a variable are:
.RS
.IP \fBNothing\fP
a value and a type; it is
the value of a variable which has not been given a value,
.IP \fBdouble\fP
a double-precision floating-point value,
.IP \fBstring\fP
a sequence of ASCII characters
(internally terminated by an ASCII zero),
.IP \fBRBlock\fP
a SEGY binary trace record,
consisting of a trace header and a trace time series
.IP \fBstream\fP
a pointer to an input or output stream
(such as \fBstdout\fR or a value returned
by \fBfopen()\fR or \fBpopen()\fR),
.IP \fBarray\fP
an array of values of any of these types,
or
.IP \fBfVector\fP
a special type of array containing only floating-point values.
The values in an \fBfVector\fP are stored
as single-precision values.
.RE
.NH 2
Functions
.LP
Function definitions begin with the keyword \fBfunc\fP.
Functions must be defined before they appear in the script.
Functions may have arguments and local variables.
Here's a simple function definition:
.DS B
.SM
.B
//
//	return the dot product of two ond-dimensional arrays.
//  	Using explicit indexing, as we do here, is very slow
//  	compared to vector manipulation.
//
func dot(a, b) {
	auto sum, i;
	sum = 0;
	for(i = 0; i < min(length(a), length(b)); i++)
		sum = sum + a[i] * b[i];
	return sumsq;
}
.DE
.LP
Functions in Sub are re-entrant (they may call themselves either
directly or indirectly).
.LP
If the \fBreturn\fP statement does not have an argument,
or if there is no \fBreturn\fP statement,
the function returns the value \fBNothing\fP.
.NH 2
Comments
.LP
.B Sub
supports three styles of comments:
.RS
.IP shell
from \fB#\fR to end-of-line,
.IP C
everything from \fB/*\fR to \fB*/\fR,
and
.IP C++
everything from \fB//\fR to end-of-line.
.RE
Comments cannot be nested.
Don't look for trouble.
.LP
.B Note:
Please avoid using shell-style comments in the future;
that style will be declared illegal as soon as I can get away with it
so that we can pass scripts through a preprocessor.
.NH 2
Statements
.LP
.B Sub
uses semicolons and curly braces in the same way C does.
All simple statements should end with semicolons.
Block statements should be wrapped in curly braces
(and do not have a teminating semicolon).
.I "Block statements"
include function definitions.
.NH 2
Operators
.NH 3
Numeric Operators
.LP
The operators `=', `*', `/', `-', and `+'
are available for operating on numeric values.
Also available are `%' (for modulo) and `^'
(for raised-to-the-power-of).
The increment and decrement operators, `++' and `--',
are available in both their postfix and prefix forms
and work just as in C.
.NH 3
String Operators
.LP
Only `=' and `+' are available for strings,
with `+' denoting concatenation.
Strings cannot be mixed with other types
at present;
thus the expression "3" + 7 is illegal
(and will be caught by the compiler whenever possible
and at run-time otherwise).
.NH 3
Comparison Operators
.LP
The comparison operators
`==', `>', `<', `>=', `<=', and `!='
are only implemented for numeric comparisons.
No other types may be used as arguments to these operators
at present.
(I'll try to fix this soon so we at least have
some string comparison support.)
.NH 3
Logical Operators
.LP
We currently support `&' and `&&' for \fIand\fP,
`|' and `||' for \fIor\fP,
and `!' for \fInot\fP.
At present
all of the operands in a logical expression are evaluated
(which is not generally true in C).
For now, don't do anything that depends on partial evaluation.
(If you don't understand what this is about,
you're safe from this error.)
.LP
There is no comma operator.
There are no pointers
and there are no address-related operators.
.NH 2
Control Flow
.LP
Sub supports
.RS
.IP "\fBif(\fItest\fB)\fR"
.IP "\fBif(\fItest\fB)...else\fR"
.IP "\fBwhile(\fItest\fB)\fR"
.IP "\fBfor(\fIinit\fB;\fItest\fB;\fIincr\fB)\fR"
.RE
These can be combined and nested just as in C.
.LP
We \fBdo not\fP yet support:
.RS
.IP "\fBbreak\fP"
.IP "\fBcontinue\fP"
.IP "\fBswitch(\fIvalue\fB){...}\fP"
.RE
.LP
There is no \fBgoto\fP statement
and there never will be.
.NH 2
Storage Management
.LP
Storage is managed automatically
and all assignments are
\fIassignments by value\fR,
which means that a new copy of the data is used.
There is no notion of pointers or of explicit memory management
by the user.
