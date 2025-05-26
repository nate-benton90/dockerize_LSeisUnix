	strcpy(trhdr[0].key,"tracl");
	strcpy(trhdr[0].type,"int");
	trhdr[0].offs = (char*)&tr.tracl - (char*)&tr;

	strcpy(trhdr[1].key,"tracr");
	strcpy(trhdr[1].type,"int");
	trhdr[1].offs = (char*)&tr.tracr - (char*)&tr;

	strcpy(trhdr[2].key,"fldr");
	strcpy(trhdr[2].type,"int");
	trhdr[2].offs = (char*)&tr.fldr - (char*)&tr;

	strcpy(trhdr[3].key,"cdp");
	strcpy(trhdr[3].type,"int");
	trhdr[3].offs = (char*)&tr.cdp - (char*)&tr;

	strcpy(trhdr[4].key,"cdpt");
	strcpy(trhdr[4].type,"int");
	trhdr[4].offs = (char*)&tr.cdpt - (char*)&tr;

	strcpy(trhdr[5].key,"trid");
	strcpy(trhdr[5].type,"int");
	trhdr[5].offs = (char*)&tr.trid - (char*)&tr;

	strcpy(trhdr[6].key,"nvs");
	strcpy(trhdr[6].type,"int");
	trhdr[6].offs = (char*)&tr.nvs - (char*)&tr;

	strcpy(trhdr[7].key,"nhs");
	strcpy(trhdr[7].type,"int");
	trhdr[7].offs = (char*)&tr.nhs - (char*)&tr;

	strcpy(trhdr[8].key,"offset");
	strcpy(trhdr[8].type,"int");
	trhdr[8].offs = (char*)&tr.offset - (char*)&tr;

	strcpy(trhdr[9].key,"sx");
	strcpy(trhdr[9].type,"int");
	trhdr[9].offs = (char*)&tr.sx - (char*)&tr;

	strcpy(trhdr[10].key,"sy");
	strcpy(trhdr[10].type,"int");
	trhdr[10].offs = (char*)&tr.sy - (char*)&tr;

	strcpy(trhdr[11].key,"gx");
	strcpy(trhdr[11].type,"int");
	trhdr[11].offs = (char*)&tr.gx - (char*)&tr;

	strcpy(trhdr[12].key,"gy");
	strcpy(trhdr[12].type,"int");
	trhdr[12].offs = (char*)&tr.gy - (char*)&tr;

	strcpy(trhdr[13].key,"sstat");
	strcpy(trhdr[13].type,"int");
	trhdr[13].offs = (char*)&tr.sstat - (char*)&tr;

	strcpy(trhdr[14].key,"gstat");
	strcpy(trhdr[14].type,"int");
	trhdr[14].offs = (char*)&tr.gstat - (char*)&tr;

	strcpy(trhdr[15].key,"tstat");
	strcpy(trhdr[15].type,"int");
	trhdr[15].offs = (char*)&tr.tstat - (char*)&tr;

	strcpy(trhdr[16].key,"muts");
	strcpy(trhdr[16].type,"int");
	trhdr[16].offs = (char*)&tr.muts - (char*)&tr;

	strcpy(trhdr[17].key,"mute");
	strcpy(trhdr[17].type,"int");
	trhdr[17].offs = (char*)&tr.mute - (char*)&tr;

	strcpy(trhdr[18].key,"_ns");
	strcpy(trhdr[18].type,"int");
	trhdr[18].offs = (char*)&tr._ns - (char*)&tr;

	strcpy(trhdr[19].key,"_dt");
	strcpy(trhdr[19].type,"int");
	trhdr[19].offs = (char*)&tr._dt - (char*)&tr;

	strcpy(trhdr[20].key,"gain");
	strcpy(trhdr[20].type,"int");
	trhdr[20].offs = (char*)&tr.gain - (char*)&tr;

	strcpy(trhdr[21].key,"ungpow");
	strcpy(trhdr[21].type,"float");
	trhdr[21].offs = (char*)&tr.ungpow - (char*)&tr;

	strcpy(trhdr[22].key,"unscale");
	strcpy(trhdr[22].type,"float");
	trhdr[22].offs = (char*)&tr.unscale - (char*)&tr;

	strcpy(trhdr[23].key,"ntr");
	strcpy(trhdr[23].type,"int");
	trhdr[23].offs = (char*)&tr.ntr - (char*)&tr;

	strcpy(trhdr[24].key,"mark");
	strcpy(trhdr[24].type,"int");
	trhdr[24].offs = (char*)&tr.mark - (char*)&tr;


	TR_NK = 25;
