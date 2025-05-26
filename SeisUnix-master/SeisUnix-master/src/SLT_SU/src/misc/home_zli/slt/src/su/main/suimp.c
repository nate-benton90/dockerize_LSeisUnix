
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUIMP  - calculate impedance from reflectivity input		\n" 
"\n"
"suimp [parameters] <input-data >output-data   		\n" 
"\n"
"Required parameters:							\n"
"input-data             input normal-incident reflection coefficients 	\n" 
"output-data            output impedance (velocity*density)		\n" 
"i0=                    starting value of impedance		\n"
"Optional parameters:						\n"
"None		\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	1/4/97   \n"
;

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout;
	int it, nt;
	float *im, tmp1, tmp2, i0;


    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	if(!getparfloat("i0",&i0)) err("must specify i0");

	/* large file(s) */
	file2g(infp);
	file2g(outfp);
	
        if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns;

	im = (float*) emalloc(nt*sizeof(float));

	do {
		im[0] = i0;
		for(it=1;it<nt;it++) {
			tmp1 = 1. - tr.data[it-1];
			tmp2 = 1. + tr.data[it-1];
			if(tmp1==0.) {
				im[it] = im[it-1];
			} else {
				im[it] = tmp2/tmp1*im[it-1];
			}
		}
		for(it=0;it<nt;it++) tr.data[it] = im[it];
		fputtr(outfp,&tr);
	} while(fgettr(infp,&tr));

	return 0;

}
