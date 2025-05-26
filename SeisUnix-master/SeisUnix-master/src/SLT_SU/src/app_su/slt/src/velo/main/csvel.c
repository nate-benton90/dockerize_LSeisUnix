/* compute checkshot from time-depth pairs in an ascii file */

#include "velo.h"


char *sdoc = 
"csvel - compute checkshot from an ascii (time,depth) file 	\n"
"\n"
"ascsm [parameters] <input >output			\n" 
"\n"
"Required parameters:						 	\n"
"NONE \n"
"\n"
"Optional parameters:						 	\n"
"tomin=0         minimum time (ms) to output checkshot \n"
"dto=100         time interval (ms) to output checkshot \n"
"nto=101         number of time samples to output checkshot \n"
"nrmax=100000    maximum number of rows in the input file \n"
"tis=3000        starting time (ms) of input (t,z) pairs to compute \n"
"                the linear trend for data beyond the maximum input time \n"
"timax=10000     maximum time (ms) of input file \n"
"NOTE \n"
"     1. input file consists of 2 columns (time in ms, depth) format \n" 
"     2. input depths for given interval of time (0.1ms) are averged to \n"
"        a single depth value before linear interpolation of input data \n"
"        to output checkshot										\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	5/22/98   		\n" 
;

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
		float tomin, dto, tis;
		float timax;
		int nto, nrmax, ir;
		int nti, it, i, i1;
		float *z10, *ti, *zi;
		int *i10;
		float tisum, zisum, tizisum, ti2sum, tmp, a, b;
		int one=1, n, n1;
		float t, z, to, zo, tomax;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparfloat("tomin",&tomin)) tomin = 0;
		if (!getparfloat("dto",&dto)) dto = 100;
		if (!getparint("nto",&nto)) nto = 101;
		if (!getparfloat("tis",&tis)) tis = 3000;
		if (!getparint("nrmax",&nrmax)) nrmax = 100000;
		if (!getparfloat("timax",&timax)) timax = 10000;

		tomax = tomin + (nto-1)*dto;

/* memory allocation */
		tmp = timax * 10. + 1.5;
		n1 = tmp;

		z10 = (float *) malloc(n1*sizeof(float)); 
		i10 = (int *) malloc(n1*sizeof(int));
    	cbuf = (char*)malloc(134*sizeof(char));
		zi = (float *) malloc(n1*sizeof(float)); 
		ti = (float *) malloc(n1*sizeof(float));

		for(i1=0;i1<n1;i1++) {
			z10[i1] = 0.;
			i10[i1] = 0;
		}

/* read (t,z) and averge z in 0.1ms interval */
    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%f %f \n", &t, &z);
			tmp = t * 10. + .5;
			it = tmp;
			if(it>=0 && it<=n1-1) {
				i10[it] += 1;
				z10[it] += z;
			}
		}

		if(i10[0]>0) {
			it = 0;
		} else {
			ti[0] = 0;
			zi[0] = 0.;
			it = 1;
		}

		for(i1=0;i1<n1;i1++) {
			if(i10[i1]>0) {
				ti[it] = i1*0.1;
				zi[it] = z10[i1]/i10[i1];
				/*
				fprintf(stderr,"it=%d i10=%d z10=%g i1=%d \n",
						it, i10[i1], z10[i1], i1);
				fprintf(stderr,"ti=%g zi=%g it=%d i10=%d i1=%d \n",
					ti[it],zi[it],it,i10[i1],i1);
				*/
				it = it + 1;
			}
		}
		nti = it;



/* compute linear trend beyond tis */
		if(ti[nti-1]<tomax) {
			tisum = 0.;
			zisum = 0.;
			ti2sum = 0.;
			tizisum = 0.;
			n = 0;
			for (it=0;it<nti;it++) {
				if(ti[it]>=tis) {
					tisum += (ti[it]-tis);
					zisum += zi[it];
					tizisum += (ti[it]-tis)*zi[it];
					ti2sum += (ti[it]-tis)*(ti[it]-tis);
					n += 1; 
				}
			}
			a = (zisum*ti2sum - tizisum*tisum)/(n*ti2sum-tisum*tisum);
			b = (zisum - n*a)/tisum; 
		}

		fprintf(stderr," linear trend computed: z(%g)=%g slope=%g ft/ms \n",
				tis,a, b);
		fprintf(stderr," maximum input time of t-z pairs = %g\n", ti[nti-1]);
/* output checkshots */
		for(it=0;it<nto;it++) {
			to = tomin + it*dto;
			if(to>ti[nti-1]) {
				zo = a + (to-tis)*b; 
			} else if(to==ti[nti-1]) {
				zo = zi[nti-1];
			} else {
				lin1d_(ti,zi,&nti,&to,&zo,&one,&i1);
			}

			fprintf(outfp,"%g %g \n",to,zo);
		}


    free(cbuf);
    free(i10);
    free(z10);
    free(ti);
    free(zi);

    return (0);
}
