.LP
This is a collection of specialized
discussions, tips, and examples.
.NH 2
Pipes
.LP
.B Popen()
and
.B pclose()
provide very substantial flexibility to scripts.
A child process which is \fBpopen\fR'd in mode "w" shares
\fBstdout\fR with the parent process
(\fBsub\fR in this case).
Thus a child process invoked in this manner can be used as
an output filter for the parent process.
Values written by the child process to its \fBstdout\fR
will emerge \fIas though\fR from the parent process' \fBstdout\fR.
A similar mechanism works for read pipelines.
.LP
Remember
that many programs
(such as \fBxgraph(1)\fR which is used in one of the examples)
do not do anything interesting until they have read everything
available from \fBstdin\fR.
These programs will not see end-of-file on \fBstdin\fR
until the \fBSub\fR script has called \fBpclose()\fR.
.NH 2
A Plotting Example
.LP
This script reads the first five traces in a \fBSonicTool\fP
data set,
computes the amplitude spectrum of each,
and displays the results using a pipe to
the public-domain plotting tool \fBxgraph\fP.
.DS B
.SM
.B
.so dftplot.inc
.DE
.NH 2
Vectors \fIvs\fP Loops
.LP
Explicit loops are slow.
If efficiency is an issue
(and usually not important for many Sub applications),
vectorized arithmetic is a lot faster than
wading through a loop.
On the other hand,
loops are more flexible.
.LP
Here are two scripts that perform the same simple calculation.
The first one uses a loop:
.DS B
.B
.SM
.so looper.inc
.DE
The second one uses vector operations:
.DS B
.B
.SM
.so flooper.inc
.DE
.LP
As the timing information in the examples shows,
the vectorized version was about \fI300\fP times faster.
