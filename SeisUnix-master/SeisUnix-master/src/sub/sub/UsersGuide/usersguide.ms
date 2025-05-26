.nr PS 12
.nr VS 14
.nr FM 1.5i
.ds LF NER, Inc.
.ds RF -%-
.DA
.ds CH Sub User's Guide (v. 0.9)
.TL
Sub User's Guide (v. 0.9)
.AU
.LG
\fIDraft\fP
.SM
.sp
.sp
New England Research, Inc.
76 Olcott Drive
White River Junction, Vermont 05001
USA
.AB no
.LP
Sub combines
a complete, simple procedural language
with convenient support for reading, manipulating, and writing
SEGY binary trace record data streams.
Sub makes it easy for the user
to inspect, report, and modify the contents
of a SEGY data stream
by writing simple programs in Sub's internal language.
Sub also supports efficient trace-oriented manipulation
which makes it possible to perform sophisticated
time series processing.
.LP
This document describes Sub (v. 0.9).
Sub (v. 0.9) is the first public release and is a \fIbeta\fP release.
Although much of the code has seen substantial testing,
there are many new features which probably contain many new bugs.
This is also the first version of the \fBUser's Guide\fP.
Address questions, comments, and complaints to:
.DS C
Martin L. Smith
New England Research, Inc.
76 Olcott drive
White River Junction, Vermont 05001
USA

(802) 296-2401 (voice)
(802) 296-8333 (fax)
\fImartin@ner.com\fP (Internet)
.DE
.AE
.bp
.NH 1
Some Sub Examples
.so introexamples.ms
.bp
.NH 1
Sub Syntax
.so syntax.ms
.bp
.NH 1
Sub Arrays
.so fvector.ms
.bp
.NH 1
Sub Library
.so library.ms
.bp
.NH 1
Accessing Data Records
.so rblock.ms
.bp
.NH 1
Programming
.so programming.ms
.bp
.SH
Appendix A: SEGY Trace Record Members
.so app.segy.ms
.bp
.SH
Appendix B: SonicTool SEGY Members
.so app.st.ms


