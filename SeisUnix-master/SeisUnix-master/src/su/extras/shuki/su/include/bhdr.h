	strcpy(bhdr[0].key,"jobid");
	strcpy(bhdr[0].type,"int");
	bhdr[0].offs = (char*)&bh.jobid - (char*)&bh;

	strcpy(bhdr[1].key,"lino");
	strcpy(bhdr[1].type,"int");
	bhdr[1].offs = (char*)&bh.lino - (char*)&bh;

	strcpy(bhdr[2].key,"reno");
	strcpy(bhdr[2].type,"int");
	bhdr[2].offs = (char*)&bh.reno - (char*)&bh;

	strcpy(bhdr[3].key,"ntrpr");
	strcpy(bhdr[3].type,"int");
	bhdr[3].offs = (char*)&bh.ntrpr - (char*)&bh;

	strcpy(bhdr[4].key,"nart");
	strcpy(bhdr[4].type,"int");
	bhdr[4].offs = (char*)&bh.nart - (char*)&bh;

	strcpy(bhdr[5].key,"dt");
	strcpy(bhdr[5].type,"int");
	bhdr[5].offs = (char*)&bh.dt - (char*)&bh;

	strcpy(bhdr[6].key,"dto");
	strcpy(bhdr[6].type,"int");
	bhdr[6].offs = (char*)&bh.dto - (char*)&bh;

	strcpy(bhdr[7].key,"ns");
	strcpy(bhdr[7].type,"int");
	bhdr[7].offs = (char*)&bh.ns - (char*)&bh;

	strcpy(bhdr[8].key,"nso");
	strcpy(bhdr[8].type,"int");
	bhdr[8].offs = (char*)&bh.nso - (char*)&bh;

	strcpy(bhdr[9].key,"format");
	strcpy(bhdr[9].type,"int");
	bhdr[9].offs = (char*)&bh.format - (char*)&bh;

	strcpy(bhdr[10].key,"fold");
	strcpy(bhdr[10].type,"int");
	bhdr[10].offs = (char*)&bh.fold - (char*)&bh;

	strcpy(bhdr[11].key,"tsort");
	strcpy(bhdr[11].type,"int");
	bhdr[11].offs = (char*)&bh.tsort - (char*)&bh;

	strcpy(bhdr[12].key,"vscode");
	strcpy(bhdr[12].type,"int");
	bhdr[12].offs = (char*)&bh.vscode - (char*)&bh;

	strcpy(bhdr[13].key,"hsfs");
	strcpy(bhdr[13].type,"int");
	bhdr[13].offs = (char*)&bh.hsfs - (char*)&bh;

	strcpy(bhdr[14].key,"hsfe");
	strcpy(bhdr[14].type,"int");
	bhdr[14].offs = (char*)&bh.hsfe - (char*)&bh;

	strcpy(bhdr[15].key,"hslen");
	strcpy(bhdr[15].type,"int");
	bhdr[15].offs = (char*)&bh.hslen - (char*)&bh;

	strcpy(bhdr[16].key,"hstyp");
	strcpy(bhdr[16].type,"int");
	bhdr[16].offs = (char*)&bh.hstyp - (char*)&bh;

	strcpy(bhdr[17].key,"schn");
	strcpy(bhdr[17].type,"int");
	bhdr[17].offs = (char*)&bh.schn - (char*)&bh;

	strcpy(bhdr[18].key,"hstas");
	strcpy(bhdr[18].type,"int");
	bhdr[18].offs = (char*)&bh.hstas - (char*)&bh;

	strcpy(bhdr[19].key,"hstae");
	strcpy(bhdr[19].type,"int");
	bhdr[19].offs = (char*)&bh.hstae - (char*)&bh;

	strcpy(bhdr[20].key,"htatyp");
	strcpy(bhdr[20].type,"int");
	bhdr[20].offs = (char*)&bh.htatyp - (char*)&bh;

	strcpy(bhdr[21].key,"hcorr");
	strcpy(bhdr[21].type,"int");
	bhdr[21].offs = (char*)&bh.hcorr - (char*)&bh;

	strcpy(bhdr[22].key,"bgrcv");
	strcpy(bhdr[22].type,"int");
	bhdr[22].offs = (char*)&bh.bgrcv - (char*)&bh;

	strcpy(bhdr[23].key,"rcvm");
	strcpy(bhdr[23].type,"int");
	bhdr[23].offs = (char*)&bh.rcvm - (char*)&bh;

	strcpy(bhdr[24].key,"mfeet");
	strcpy(bhdr[24].type,"int");
	bhdr[24].offs = (char*)&bh.mfeet - (char*)&bh;

	strcpy(bhdr[25].key,"nvs");
	strcpy(bhdr[25].type,"int");
	bhdr[25].offs = (char*)&bh.nvs - (char*)&bh;

	strcpy(bhdr[26].key,"ntr");
	strcpy(bhdr[26].type,"int");
	bhdr[26].offs = (char*)&bh.ntr - (char*)&bh;

	strcpy(bhdr[27].key,"id");
	strcpy(bhdr[27].type,"int");
	bhdr[27].offs = (char*)&bh.id - (char*)&bh;

	strcpy(bhdr[28].key,"esize");
	strcpy(bhdr[28].type,"int");
	bhdr[28].offs = (char*)&bh.esize - (char*)&bh;

	strcpy(bhdr[29].key,"version");
	strcpy(bhdr[29].type,"int");
	bhdr[29].offs = (char*)&bh.version - (char*)&bh;

	strcpy(bhdr[30].key,"name");
	strcpy(bhdr[30].type,"char");
	bhdr[30].offs = (char*)bh.name - (char*)&bh;

	strcpy(bhdr[31].key,"area");
	strcpy(bhdr[31].type,"char");
	bhdr[31].offs = (char*)bh.area - (char*)&bh;

	strcpy(bhdr[32].key,"client");
	strcpy(bhdr[32].type,"char");
	bhdr[32].offs = (char*)bh.client - (char*)&bh;


	BH_NK = 33;
