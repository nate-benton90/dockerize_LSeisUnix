.LP
The next table shows the SEGY record members
used by \fBSonicTool\fP.
(Note that the format information on page 10
of the manual for \fBSonicTool 1.5\fP is wrong.)
.TS H
box;
c c n n l.
type	name	offset	length	meaning
_
.TH
integer	fldr	8	1	receiver number (starting form 1)
integer	sdepth	48	1	source depth (positive)
short	scalco	70	1	scale factor for source depth
unsigned short	ns	114	1	number of samples in this trace
float array	trace	240	?	trace samples
.TE
.LP
