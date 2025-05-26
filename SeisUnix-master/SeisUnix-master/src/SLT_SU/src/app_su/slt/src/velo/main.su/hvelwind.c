/* velocity card format conversion */

#include "velo.h"


char *sdoc = 
"HVELWIND - Window DISCO HANDVEL cards  	 		\n"
"\n"
"hvelwind [parameters] <handvel-cards >handvel-windowed 			\n" 
"\n"
"Required parameters:						 	\n"
"None \n"
"\n"
"Optional parameters:						 	\n"
"cdptype=0     type of cdp number in HANDVEL card			\n"
"              =0 global sequential cdp number				\n"
"              =1 cdplbl number (line-xline)				\n"
"nvfmax=4096   maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=256    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"\n"
"the following two parameters are used when cdptype=0 \n"
"mincdp=-9999  minimum cdp number to output HANDVEL cards \n"
"maxcdp=9e+20  maximum cdp number to output HANDVEL cards \n"
" \n"
"the following four parameters are used when cdptype=1 \n"
"minln=-9999   minimum line number to output HANDVEL cards \n" 
"maxln=9e+20   maximum line number to output HANDVEL cards \n" 
"mintr=-9999   minimum trace number to output HANDVEL cards \n" 
"maxtr=9e+20   maximum trace number to output HANDVEL cards \n" 
"\n"
"AUTHOR:		Zhiming Li,       ,	8/15/97   		\n"    
;

main(int argc, char **argv)
{
   	int n1, n2;
   	FILE *infp=stdin,*outfp=stdout;
   	int  *cdps, *nps; 
   	float  *times, *vrms;
	int cdptype, ncdp, icdp, iout;
	int iline, itrace, cdp, ntv, it;
	float mincdp, maxcdp, minln, maxln, mintr, maxtr;
	int nout;

   	/* get parameters */
   	initargs(argc,argv);
	askdoc(1);

	if (!getparint("cdptype",&cdptype)) cdptype=0;

	if(cdptype==0) {
		if (!getparfloat("mincdp",&mincdp)) mincdp = -9999;
		if (!getparfloat("maxcdp",&maxcdp)) maxcdp = 9e+20;
	} else {
		if (!getparfloat("mintr",&mintr)) mintr = -9999;
		if (!getparfloat("maxtr",&maxtr)) maxtr = 9e+20;
		if (!getparfloat("minln",&minln)) minln = -9999;
		if (!getparfloat("maxln",&maxln)) maxln = 9e+20;
	}
/* memory allocation */
	if (!getparint("ntvmax",&n1)) n1=256;
	if (!getparint("nvfmax",&n2)) n2=4096;


   	times = (float*)malloc(n1*n2*sizeof(float));
   	vrms = (float*)malloc(n1*n2*sizeof(float));
	cdps = (int*) malloc(n2*sizeof(int));
	nps = (int*) malloc(n2*sizeof(int));

	hvelread(infp,cdps,times,vrms,&ncdp,nps,n1,n2);

	fprintf(stderr," %d HANDVEL cards read \n",ncdp);

	nout = 0;
	for(icdp=0;icdp<ncdp;icdp++) {
		ntv = nps[icdp];
		iout = 0;
		cdp = cdps[icdp];
		if(cdptype==0) {
			if(cdp>=mincdp && cdp<=maxcdp) iout=1;
		} else {
			iline = cdp/10000;
			itrace = cdp - iline * 10000;
			if(iline>=minln && iline<=maxln && 
			   itrace>=mintr && itrace<=maxtr) iout = 1;
		}

		if(iout==1) {
			nout = nout + 1;
			printhvel(cdp,ntv,times+icdp*n1,vrms+icdp*n1,outfp);
		}
	}
	fprintf(stderr," %d HANDVEL cards output \n",nout);

    free(times);
    free(vrms);
    free(nps);
    free(cdps);

}


