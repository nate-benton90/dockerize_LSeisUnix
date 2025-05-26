func Begin()
{	Records = 0;
	sumavgsq = 0;
}

func OnTrace()
{	++Records;
	sumavgsq = sumavgsq + sum(Tr.trace * Tr.trace)/length(Tr.trace);
}

func End()
{	auto avgsq;
	if(Records == 0) exit(0);
	avgsq = sumavgsq/Records;
	print("Records: ", Records, "\\n");
	print("Average squared amplitude: ", avgsq, "\\n");
}

