/* HANDVEL velocity card modification  */

#include "velo.h"


char *sdoc = 
"HVELMOD - Modify DISCO HANDVEL cards  	 		\n"
"\n"
"hvelmod [parameters] <handvel-cards >handvel-modified 			\n" 
"\n"
"Required parameters:						 	\n"
"None \n"
"\n"
"Optional parameters:						 	\n"
"dtmin=2000    minimum time (ms) allowed between two adjacent times 	\n"
"              in a HANDVEL function					\n"
"              second t-v pairs will be removed if time interval between \n"
"              the two picks are less than dtmin 			\n"
"tmin=         minimum time (ms) to add to the velocity function 	\n"
"vmin=         velocity at tmin to add to the velocity function 	\n"
"tmax=         maximum time (ms) to add to the velocity function 	\n"
"vmax=         velocity at tmax to add to the velocity function 	\n"
"              (if tmin,vmin,tmax,vmax are not given, no action)	\n" 
"vimin=        minimum interval velocity allowed 			\n"
"vimax=        maximum interval velocity allowed 			\n"
"              (if vimin and vimax specified, Dix formula will be used  \n"
"              to compute the interval velocity. If the computed 	\n"
"              interval velocity exceeds the limit, the later t-v 	\n"
"              pairs will be deleted)					\n"
"nvfmax=4096   maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=256    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"\n"
"Note:         1. time interval check is done first, then interval 	\n"
"              velocity check.						\n"
"AUTHOR:		Zhiming Li,       ,	3/20/03   		\n"    
;

main(int argc, char **argv)
{
   	int n1, n2;
   	FILE *infp=stdin,*outfp=stdout;
   	int  *cdps, *nps; 
   	float  *times, *vrms;
   	float  *to, *vo, *tc, *vc;
	int ntv, cdp, ncdp, icdp, it;
	float dtmin, tmin, vmin, tmax, vmax;
	float vimin, vimax;
	float tmp, v1, v2, t1, t2;
	int nto, ntc;

   	/* get parameters */
   	initargs(argc,argv);
	askdoc(1);

	if (!getparfloat("dtmin",&dtmin)) dtmin=2000.;
	if (!getparfloat("tmin",&tmin)) tmin=-9999.;
	if (!getparfloat("vmin",&vmin)) vmin=-9999.;
	if (!getparfloat("tmax",&tmax)) tmax=-9999.;
	if (!getparfloat("vmax",&vmax)) vmax=-9999.;
	if (!getparfloat("vimin",&vimin)) vimin=-9999.;
	if (!getparfloat("vimax",&vimax)) vimax=-9999.;


	fprintf(stderr," ====== hvelmod ===== \n");
	fprintf(stderr," dtmin=%f vimin=%f vimax=%f \n",dtmin, vimin, vimax);
	fprintf(stderr," tmin=%f vmin=%f tmax=%f vmax=%f \n",tmin, vmin, tmax, vmax);

/* memory allocation */
	if (!getparint("ntvmax",&n1)) n1=256;
	if (!getparint("nvfmax",&n2)) n2=4096;


   	to = (float*)malloc((n1+2)*sizeof(float));
   	vo = (float*)malloc((n1+2)*sizeof(float));
   	times = (float*)malloc(n1*n2*sizeof(float));
   	vrms = (float*)malloc(n1*n2*sizeof(float));
	cdps = (int*) malloc(n2*sizeof(int));
	nps = (int*) malloc(n2*sizeof(int));
   	tc = (float*)malloc((n1+2)*sizeof(float));
   	vc = (float*)malloc((n1+2)*sizeof(float));

	hvelread(infp,cdps,times,vrms,&ncdp,nps,n1,n2);

	fprintf(stderr," %d HANDVEL cards read \n",ncdp);

	for(icdp=0;icdp<ncdp;icdp++) {
		ntv = nps[icdp];
		cdp = cdps[icdp];
		nto = 0;
		if(tmin!=-9999. && vmin!=-9999. && tmin<times[icdp*n1]) {
			to[0] = tmin;
			vo[0] = vmin;
		} else {
			to[0] = times[icdp*n1];
			vo[0] = vrms[icdp*n1];
		}
		for(it=0;it<ntv;it++) {
			if((times[icdp*n1+it]-to[nto])>dtmin) {
				nto = nto + 1;
				to[nto] = times[icdp*n1+it];
				vo[nto] = vrms[icdp*n1+it];
			}
		}
		if(tmax!=-9999. && vmax!=-9999. && (tmax-to[nto])>dtmin) {
			nto = nto + 1;
			to[nto] = tmax;
			vo[nto] = vmax;
		}
		nto = nto + 1;
		/* check interval velocity bounds */
		if(vimin!=-9999. && vimax!=-9999.) {
			t1 = to[0];
			v1 = vo[0];
			tc[0] = t1;
			vc[0] = v1;
			ntc = 0;
			for(it=1;it<nto;it++) {
				t2 = to[it];
				v2 = vo[it];
				tmp = (v2*v2*t2-v1*v1*t1)/(t2-t1);
				if(tmp>0.) {
					tmp = sqrt(tmp);
					if(tmp >= vimin && tmp<=vimax) {
						ntc = ntc + 1;
						tc[ntc] = t2;
						vc[ntc] = v2;
						t1 = t2;
						v1 = v2;
					}
				}
			}
			ntc = ntc + 1;
			for(it=0;it<ntc;it++) {
				to[it] = tc[it];
				vo[it] = vc[it];
			}
			nto = ntc;
		}
		printhvel(cdp,nto,to,vo,outfp);
		fprintf(stderr," HANDVEL card output at %d for %d t-v pairs \n",cdp,nto);
	}
	fprintf(stderr," %d HANDVEL cards output \n",ncdp);

    free(to);
    free(vo);
    free(times);
    free(vrms);
    free(nps);
    free(cdps);

}


