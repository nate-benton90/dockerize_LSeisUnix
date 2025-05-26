.LP
One of Sub's design goals
is to be readily adaptable to new data formats
(that is, more than just SEGY).
Because of this we sometimes refer to SEGY records
as \fBRBlock\fPs.
In the SEGY version,
which you are using now,
\fBrBlock\fPs are exactly the same thing as SEGY trace records.
.NH 2
Member Access
.LP
The \fBRBlock\fR type supports
a structure-like convention for access to individual
data elements (including the trace data samples).
If, for example, \fBrb\fR holds a trace record,
\fBrb.ns\fR provides (read or write) access
to the number-of-samples field.
.LP
In addition to the simple header fields,
the trace samples can be accessed through a pseudo-array.
If \fIrb\fR holds a trace record,
\fIrb.trace[\fIi\fB]\fR
will access the \fIi\fRth sample.
.LP
All of the member names and meanings for SEGY trace records
are summarized in Appendix A.
.LP
It is possible to change the size of the trace
by assigning a new \fBfVector\fP to it.
Thus
.DS B
.B
Tr.trace = fvector(10, 0);
.DE
would replace the original trace with a trace
with ten samples, all with the value \fB0\fP.
Whenever a length-changing assignment is made to a trace,
Sub
attempts to set the value of the member \fBns\fP
to correspond to the new length.
If you want \fBns\fP to have a value other than the current trace length,
you must assign to \fBns\fP \fIafter\fP changing the trace.
.DS B
.B
//  suppose z is a SEGY record and z.trace has 20 elements.
//  initially z.ns will have the value 20.
z.ns = 7;   	    	    // z.ns now has the value 7
z.trace = fvector(12, 0);  // z.ns now has the value 12
z.ns = 7;   	    	    // z.ns is seven again
.DE
.NH 2
SonicTool Notes
.LP
The fields understood by \fBSonicTool\fP are
summarized in \fBAppendix B\fP.
The format information on page 10
of the manual for \fBSonicTool 1.5\fP is wrong.
I think \fBAppendix B\fP is correct.
