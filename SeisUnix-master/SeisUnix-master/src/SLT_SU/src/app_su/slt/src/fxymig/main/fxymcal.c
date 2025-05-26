
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"FXYMCAL - Calculation of Frequency Ranges for multiple-machine FXYMIG run	\n" 
"\n"
"fxymcal [parameters] <input-data 	\n"
"\n"
"Required parameters:							\n"
"njobs=                    number of jobs to run \n"
"\n"
"Optional parameters:							\n"
"nfft=trace_length*1.5     fft length of trace in samples \n"
"fmin=(0.5/dt)/20          minimum frequency to migrate (Hz) \n"
"fmax=(0.5/dt)*1/2         maximum frequency to migrate (Hz) at surface \n"
"fmaxend=fmax              maximum frequency to migrate at bottom \n"
"ratio=1,1,...,1           relative work ratio of the jobs \n" 
"                          for example, njobs=3 ratio=2,1,2 will make \n"
"                          the program to print frequency ranges for \n"
"                          the three jobs, assuming machines 1 and 3 are \n"
"                          going to do twice as much work as machine 2 \n"
"\n"
"Notes: \n"
"1. dt is the input trace sampling rate in secends \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	1/25/99   \n"		    
;

void radix_(int *ntmp, int *nfft);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin;
	int njobs, nfft, nt, ntmp;
	float fmin, fmax, fmaxend;
	int *ratio;
	float dt, df, dw, pi;
	int i1, i2, iwnum, iwmin, iwmax, i;
	float f1, f2, fjob, tmp, sum;
	int nfftq, iwend;
	int j;


  	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	if (!getparint("njobs",&njobs)) err(" njobs missing ");
	j = countparval("ratio");
	if ( njobs != j && j > 0 ) err(" check ratio ");

	ratio = (int *)malloc(njobs*sizeof(int));
	if(!getparint("ratio",ratio)) {
		for(i=0;i<njobs;i++) ratio[i] = 1;
	}

	/* make file size to be able to exceed 2 G */
	file2g(infp);
	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns; 
	dt = (float)tr.dt/1000000.; 

	/* optional parameters */
	if(!getparfloat("fmin",&fmin)) fmin = 0.05 * 0.5 / dt;
	if(!getparfloat("fmax",&fmax)) fmax = 1./2.0 * 0.5 / dt;
	if(!getparfloat("fmaxend",&fmaxend)) fmaxend = fmax;
	if(!getparint("nfft",&nfft)) nfft = nt * 3 / 2;

	ntmp = (nfft+1)/2*2;
	radix_(&ntmp,&nfft);
	nfftq = nfft/2+1;

	pi = 3.141592654;
	df = 1./(nfft*dt); 
	dw = 2.*pi/(nfft*dt); 
	tmp = fmin*2.*pi/dw;
	iwmin = tmp;
	if(iwmin<1) iwmin=1;
	if(iwmin>=nfftq) iwmin=nfftq-1;
	tmp = fmax*2.*pi/dw;
	iwmax = tmp;
	if(iwmax<1) iwmax=1;
	if(iwmax>=nfftq) iwmax=nfftq-1;
	tmp = fmaxend*2.*pi/dw;
	iwend = tmp;
	if(iwend<1) iwend=1;
	if(iwend>=nfftq) iwend=nfftq-1;

	if(iwmax<iwmin) iwmax = iwmin;
	if(iwend<iwmin) iwend = iwmin;

	tmp = (iwmax+iwend)/2. - iwmin;
	sum = 0.;
	for(i=0;i<njobs;i++) sum = sum + ratio[i];
	fjob = tmp / sum;

	printf(" ======= FXYMCAL ======= \n");
	printf(" ifmin_global=%d ifmax_global=%d ifend_global=%d nf_global=%d \n",
		iwmin,iwmax,iwend,iwmax-iwmin+1);

	i1 = iwmin;
	for(i=0;i<njobs;i++) {
		f1 = i1 * df;
		tmp = fjob * ratio[i] + 0.5;
		iwnum = tmp;
		i2 = i1 + iwnum -1;
		j = iwnum;
		if(i2>iwend && iwend<iwmax) {
			if(i1<iwend) {
				j = iwend - i1;
				i2 = iwend;
				do {
					i2 = i2 + 1;
					j = (iwend-i1) + 
						(2*(iwmax-iwend)-(i2-iwend))*(i2-iwend)/(iwmax-iwend)/2;
				} while (j<iwnum && i2<=iwmax);
			} else {
				i2 = i1;
				j = 0;
				do { 
					i2 = i2 + 1;
					j = (2*(iwmax-iwend)-(i1-iwend)-(i2-iwend))*
						(i2-i1)/(iwmax-iwend)/2;
				} while (j<iwnum && i2<=iwmax);
			}
		}
		if(i2>iwmax) i2 = iwmax;
		if(i==njobs-1) i2 = iwmax;
		iwnum = i2 - i1 + 1;
		f2 = i2*df;
		printf("ijob=%-2d:  ifmin=%-5d    ifmax=%-5d    nf=%-5d   fmin_i=%f fmax_i=%f \n",i+1,i1,i2,iwnum,f1,f2);
		i1 = i2 + 1;
	}

	return 0;
}
