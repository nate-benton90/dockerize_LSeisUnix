
#include "su.h"
#include "segy.h"
#include "header.h"


/*********************** self documentation **********************/
string sdoc =
" 								 \n"
" SUSHIFT - apply static shift to input traces  		 \n"
" 								 \n"
" sushift <datain [parameters] >dataout			 	 \n"
" 							         \n"
" Required parameters:						 \n"
"  none								 \n"
" Optional parameters:						 \n"
"  datum=0	      constant datum to shift to (ms)		 \n" 	
" 							         \n"
" Note:	\n"
" shift is determined from sstat and gstat in trace header:	\n"
"   	shift = (sstat+gstat-datum),				\n"
" positive number shifts down, negative shifts up.		\n"
" After shift, the amount of shifting is output at tstat in header. \n"
"\n"
" Author:  Zhiming Li		3/12/1992                        \n"
" 							         \n"
;	
/**************** end self doc ***********************************/



segytrace trin, trout; 		/* defind input and output trace buffers */
segybhdr bh; 			/* define SEGY ascii header buffer */
segychdr ch; 			/* define SEGY binary header buffer */ 

/* main program */
main(int argc, char **argv)
{
	
	
	float *work; 		/* working buffer */
	int nt;			/* number of samples per trace */
	int it;			/* time index */
	FILE *infp=stdin;	/* standard input file pointer */
	FILE *outfp=stdout;	/* standard output file pointer */
	int datum, jt, shift,kt;
	float tmp, dt;
	

/* initialize */
	initargs(argc, argv);	/* initialize parameter reading */ 
	askdoc(1);		/* on-line self documentation */

/* make file size to be able to exceed 2 G */
	file2g(infp);
	file2g(outfp);

/* parameters fetching */

	if (!getparint("datum",&datum)) datum=0;

/* id header processing (optional) */
        fgethdr(infp,&ch,&bh); 	/* read segy id (ascii and binary) headers */ 
        fputhdr(outfp,&ch,&bh); /* output id headers */

/* processing */ 

	if (!fgettr(infp,&trin))  err("can't get first trace");
				/* read in the first trace, exit if fail */

	nt = trin.ns;		/* obtain number of samples per trace */
	dt = trin.dt;
	dt = dt/1000;


	/* memory allocation */
	work = (float*) malloc(nt*sizeof(float));


	/* Main loop over traces */
	do {
		/* get trace header value offset */
		shift = trin.sstat+trin.gstat;
		tmp = (shift - datum)/dt ;
		jt = tmp;

		for(it=0;it<jt;it++) work[it] = 0.;

		/* shift trace to work */
		for(it=0;it<nt;it++) {
			kt = it - jt;
			if(kt>=0 && kt <nt) {
				work[it] = trin.data[kt];
			} else {
				work[it] = 0.;
			}
		}

		/* copy work to trace output */
		for(it=0;it<nt;it++) trout.data[it] = work[it];
		/* copy trace header (240 bytes) to output */
		bcopy(&trin, &trout, HDRBYTES);
		/* update trace header value delrt */
		trout.tstat=jt*dt;
		/* output trace */
	      	fputtr(outfp,&trout); 
	} while (fgettr(infp,&trin));		/* reading traces */

	/* free space */
	free(work);

	/* close input and output */
	fclose(infp);
	fclose(outfp);

	return 0;
}
