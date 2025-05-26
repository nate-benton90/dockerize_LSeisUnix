#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUAVOSTK - AVO stacking 			\n"
"\n"
"supstack <stdin  >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"                           (can also be specified as datain=stdin,      \n"
"                           instead of <stdin)				\n"
"stdout                     Name of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"t=                         times in ms (specified as 100,2000,8000)	\n" 
"offmin=                    minimum offsets (specified as 200,500,1000)	\n" 
"offmax=                    maximum offsets (specified as 500,3200,6200)\n"
"Optional Parameters:\n"
"nofo=3                     Number of offsets to output     \n"
"nofmax=120                 Maximum number of input offsets per cdp     \n"
"\n"
"Author:	Zhiming Li		      		w3-31-97		\n"
"Notes:									\n"
"  1. (ti,offmini,offmaxi, i=1,2,..,n) define the live data zone to be \n"
"     avo stacked \n"
"  2. within the live data zone, the offset range will be divided into \n"
"     nofo evenly spaced subsets	\n"
"\n";
/**************** end self doc *******************************************/


void astk(float *gather,float *offset,int *mute,float ft,float dt,int nt,
	int nofi,float *fo,float *dof,float *avostk,int nofo);

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int nofo, nofmax;
	char *datain, *dataout;
	FILE *infp,*outfp;
	int cdppre, cdp;
	float *gather, *avostk;
	float *t, *offmin, *offmax;
	int ntrip, i;
	float *omin, *omax, *dof, *fo;
	int iofi, iofo; 
	float *offset, to, tmp;
	int *mute;
	int indx, one;


	segytrace tr, tro;
	segybhdr bh;
	segychdr ch;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get parameters */
	ntrip = countparval("t"); 
	if(ntrip) {
		t = (float*) malloc(ntrip*sizeof(float));
		offmin = (float*) malloc(ntrip*sizeof(float));
		offmax = (float*) malloc(ntrip*sizeof(float));
		getparfloat("t",t);
		if(!getparfloat("offmin",offmin)) err(" offmin missing");
		if(!getparfloat("offmax",offmax)) err(" offmax missing"); 
	} else {
		err(" must specified t, offmin, offmax ");
	}

	if (!getparint("nofo",&nofo)) nofo = 3;
	if (!getparint("nofmax",&nofmax)) nofmax = 120;
	
	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	} 
	file2g(infp);

	if (!getparstring("dataout",&dataout)) {
		outfp = stdout;
	} else {
		outfp = efopen(dataout,"w");
	} 
	file2g(outfp);

	/* update output binary header */
	fgethdr(infp,&ch,&bh);
	bh.fold = nofo;
	fputhdr(outfp,&ch,&bh);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	ft = tr.delrt;
	dt = (float) tr.dt * 0.001;
	
	/* allocate workspace */
	gather = (float*) emalloc(nofmax*nt*sizeof(float));
	avostk = (float*) emalloc(nofo*nt*sizeof(float));
	offset = (float*) emalloc(nofmax*sizeof(float));
	dof = (float*) emalloc(nt*sizeof(float));
	fo = (float*) emalloc(nt*sizeof(float));
	omin = (float*) emalloc(nt*sizeof(float));
	omax = (float*) emalloc(nt*sizeof(float));
	mute = (int*) emalloc(nofmax*sizeof(int));

	/* compute the output offset increment for given time */
	one = 1;
	for(it=0;it<nt;it++) {
		to = ft + it*dt;
		if(to<t[0]) {
			omin[it] = offmin[0];
			omax[it] = offmax[0];
		} else if(to>t[ntrip-1]) {
			omin[it] = offmin[ntrip-1];
			omax[it] = offmax[ntrip-1];
		} else {
			bisear_(&ntrip,&one,t,&to,&indx);
			linin_(&ntrip,&one,t,&to,&indx,offmin,&omin[it]);
			linin_(&ntrip,&one,t,&to,&indx,offmax,&omax[it]);
		}
		dof[it] = (omax[it]-omin[it])/nofo;
		fo[it] = omin[it] + dof[it]*0.5;
/*
		if(it%10==0) 
		fprintf(stderr,"omin=%g omax=%g fo=%g dof=%g it=%d \n",
			omin[it],omax[it],fo[it],dof[it],it);
*/
	}

	/* set old cdp for first trace */
	bzero(avostk,nofo*nt*sizeof(float));
	bzero(gather,nofmax*nt*sizeof(float));
	bzero(offset,nofmax*sizeof(float));
	bzero(mute,nofmax*sizeof(int));
	cdppre = tr.cdp;
	iofi = 0;
	bcopy(&tr,&tro,240);

	/* loop over traces */
	do {

		if(iofi>nofmax) 
			err(" trace %d at cdp=%d exceeds max fold %d \n",
				iofi,tr.cdp,nofmax); 
		/* change of cdp */
		if (tr.cdp!=cdppre) {

			/* avo stack */
			bzero(avostk,nofo*nt*sizeof(float));
			astk(gather,offset,mute,ft,dt,nt,iofi,
				fo,dof,avostk,nofo);
			for(iofo=0;iofo<nofo;iofo++) {
				tmp = 0.5*(tro.sx + tro.gx);
				tro.sx = tmp;	
				tro.gx = tmp;
				tmp = 0.5*(tro.sy + tro.gy);
				tro.sy = tmp;	
				tro.gy = tmp;	
				tro.offset = 0;
				tro.cdpt = iofo;
				tro.mute = 0;
				for(it=0;it<nt;it++) 
					tro.data[it] = avostk[iofo*nt+it];
				fputtr(outfp,&tro);
			}
			cdppre = tr.cdp;
			iofi = 0;
			bcopy(&tr,&tro,240);
			bzero(gather,nofmax*nt*sizeof(float));
			bzero(offset,nofmax*sizeof(float));
			bzero(mute,nofmax*sizeof(int));
			for(it=0;it<nt;it++) gather[iofi*nt+it] = tr.data[it]; 
			mute[iofi] = tr.mute;
			offset[iofi] = tr.offset;
			iofi = iofi + 1;
		} else {
			for(it=0;it<nt;it++) gather[iofi*nt+it] = tr.data[it]; 
			mute[iofi] = tr.mute;
			offset[iofi] = fabs(tr.offset);
			iofi = iofi + 1;
		}

	} while (fgettr(infp,&tr));

	/* last gather */
	bzero(avostk,nofo*nt*sizeof(float));
	astk(gather,offset,mute,ft,dt,nt,iofi,fo,dof,avostk,nofo);
	for(iofo=0;iofo<nofo;iofo++) {
		tmp = 0.5*(tro.sx + tro.gx);
		tro.sx = tmp;	
		tro.gx = tmp;
		tmp = 0.5*(tro.sy + tro.gy);
		tro.sy = tmp;	
		tro.gy = tmp;	
		tro.offset = 0;
		tro.cdpt = iofo;
		tro.mute = 0;
		for(it=0;it<nt;it++) tro.data[it] = avostk[iofo*nt+it];
		fputtr(outfp,&tro);
	}

	free(offset);
	free(mute);
	free(fo);
	free(dof);
	free(omin);
	free(omax);
	free(gather);
	free(avostk);

	return EXIT_SUCCESS;
}

void astk(float *gather,float *offset,int *mute,float ft,float dt,int nt,
	int nofi,float *fo,float *dof,float *avostk,int nofo) 
{
	int it, itmp, iofi;
	float t, tmp;
	float *fold;

	fold = (float *) malloc(nt*nofo*sizeof(float));

	bzero(fold,nt*nofo*sizeof(float));

	for(iofi=0;iofi<nofi;iofi++) {

		for(it=0;it<nt;it++) {
			t = ft + it*dt;
/*
	if(it%10==0) fprintf(stderr,"t=%g mute=%d it=%d\n",t,mute[iofi],it);
*/
			if(t>mute[iofi]) {
				tmp = (offset[iofi] - fo[it])/dof[it] + 1.5;
				itmp = tmp;
				if(itmp<1) {
					tmp = tmp + 0.01; 
					itmp = tmp;
				} else if (itmp > nofo) {
					tmp = tmp - 0.01; 
					itmp = tmp;
				}
				itmp = itmp - 1;

/*
	if(it%10==0) fprintf(stderr,"itmp=%d it=%d iofi=%d \n",itmp,it,iofi);
*/
				if(itmp>=0&&itmp<nofo&&gather[it+iofi*nt]!=0.) {
					avostk[it+itmp*nt] = gather[it+iofi*nt];
					fold[it+itmp*nt] += 1.;
				}
			}
		}

	}

	for(iofi=0;iofi<nofo;iofi++) {
		for(it=0;it<nt;it++) {
			if(fold[it+iofi*nt]>1.) avostk[it+iofi*nt] /=
				fold[it+iofi*nt];
		}
	}

	free(fold);
}
