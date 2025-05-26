func OnTrace()
{	rmsf = 1.0 / sqrt( sum(Tr.trace * Tr.trace)/length(Tr.trace));
	Tr.trace = rmsf * Tr.trace;
	fputrb(stdout, Tr);
}

