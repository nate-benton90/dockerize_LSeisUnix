/* velocity card format conversion */

#include "velo.h"
#include "cwp.h"


char *sdoc = 
"HVELSORT - sort DISCO HANDVEL cards  		\n"
"\n"
"hvelsort [parameters] <handvel-cards >hvelsort-cards			\n" 
"\n"
"Required parameters:						 	\n"
"none \n"
"\n"
"Optional parameters:						 	\n"
"nvfmax=4096   maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=256    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"\n"
"Notes:									\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	7/26/99   		\n"    
;

main(int argc, char **argv)
{
    	int n1, n2;
    	FILE *infp=stdin,*outfp=stdout;
		int *cdp, ncdp, *nps;
		float *ts, *vs;
		float *fcdp;
		int i, icdp, *indx, ntv;

    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

/* memory allocation */
		if (!getparint("ntvmax",&n1)) n1=256;
		if (!getparint("nvfmax",&n2)) n2=4096;

    	cdp = (int*)malloc(n2*sizeof(int));
    	nps = (int*)malloc(n2*sizeof(int));
    	ts = (float*)malloc(n1*n2*sizeof(float));
    	vs = (float*)malloc(n1*n2*sizeof(float));
    	fcdp = (float*)malloc(n2*sizeof(float));
    	indx = (int*)malloc(n2*sizeof(int));

		hvelread(infp,cdp,ts,vs,&ncdp,nps,n1,n2);

		for(i=0;i<ncdp;i++) {
			fcdp[i] = cdp[i];
			indx[i] = i;
		}

		qkisort(ncdp,fcdp,indx);

		for(i=0;i<ncdp;i++) {
			icdp = cdp[indx[i]];
			ntv = nps[indx[i]];
			printhvel(icdp,ntv,ts+indx[i]*n1,vs+indx[i]*n1,outfp);
		}
}

