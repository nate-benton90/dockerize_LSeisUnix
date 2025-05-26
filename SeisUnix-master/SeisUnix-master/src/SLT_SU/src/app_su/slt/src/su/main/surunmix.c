
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SURUNMIX - trace running mix within a gather 				\n" 
"\n"
"surunmix [parameters] <input-data >output-data 		\n" 
"\n"
"Required parameters:							\n"
"gathkey= 	            segy key word defining the gather \n"
"maxtrace=              maximum number of traces per gather \n"
"\n"
"Optional parameters:							\n"
"nmix=3                 number of input traces to mix per output trace 	\n"
"scale=1,1,1            scales to be applied to input traces before mixing \n"
"\n"
"Note:									\n"
"    1. Number of scales must match nmix \n"
"    2. Output trace will be at the center of input mixed traces, except \n"
"       at both edges of the gathers.   \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	3/31/99		\n"
;

void runmix(float *gather, float *out, float *work, int ntrace, int nt, 
	int nmix, float *scale);

main(int argc, char **argv)
{
	segytrace tr, tro;
	FILE *infp=stdin, *outfp=stdout;

	String gathkey, gathtype;
	Value gathval;
	int indxgath;

	int maxtrace, i, nmix=3;
	float *scale, *gather, *out, *work;
	int oldgath, newgath;
	int ntrace, change, it, nt;
	char *header;

  	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	if(!getparint("maxtrace",&maxtrace)) err(" maxtrace missing");
	if(!getparstring("gathkey",&gathkey)) err(" gathkey missing ");	
	gathtype = hdtype(gathkey);
	indxgath = getindex(gathkey);
	if(!getparstring("gathkey",&gathkey)) err(" gathkey missing ");	

	if(!getparint("nmix",&nmix)) nmix = 3;
	if(countparval("scale")==0) {
		scale = (float*) emalloc(nmix*sizeof(float));
		for(i=0;i<nmix;i++) scale[i] = 1.;
	} else if(countparval("scale")==nmix) {
		scale = (float*) emalloc(nmix*sizeof(float));
		getparfloat("scale",scale);
	} else {
		err(" number of scales: does not match nmix:%d ", 
			countparval("scale"),nmix);
	}
		
	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	/* read in first trace for nt */
    if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns;

	/* memory allocations */
	gather = (float*) emalloc(nt*maxtrace*sizeof(float)); 
	out = (float*) emalloc(nt*maxtrace*sizeof(float)); 
	work = (float*) emalloc(nt*maxtrace*sizeof(float)); 
	header = (char*) emalloc(240*maxtrace*sizeof(float)); 

	gethval(&tr,indxgath,&gathval);
	oldgath = vtoi(gathtype,gathval);
	ntrace = 0;

	/* loop over input traces */
	do {
		/* compute gather location */
		gethval(&tr,indxgath,&gathval);
		newgath = vtoi(gathtype,gathval);

		if(oldgath!=newgath) {
			change = 1;
			oldgath = newgath;
		} else {
			change = 0;
		}

		if(change==1) {
			runmix(gather,out,work,ntrace,nt,nmix,scale);
			for(i=0;i<ntrace;i++) {
				bcopy(header+i*240,&tro,240);
				for(it=0;it<nt;it++) tro.data[it] = out[i*nt+it];
				fputtr(outfp,&tro);
			}
			bcopy(&tr,header,240);
			for(it=0;it<nt;it++) gather[it] = tr.data[it]; 
			ntrace = 1;
		} else {
			bcopy(&tr,header+ntrace*240,240);
			for(it=0;it<nt;it++) gather[it+ntrace*nt] = tr.data[it]; 
			ntrace = ntrace + 1;
		}
	} while(fgettr(infp,&tr));

	/* last gather */
	runmix(gather,out,work,ntrace,nt,nmix,scale);
	for(i=0;i<ntrace;i++) {
		bcopy(header+i*240,&tro,240);
		for(it=0;it<nt;it++) tro.data[it] = out[i*nt+it];
		fputtr(outfp,&tro);
	}

	free(gather);
	free(out);
	free(work);
	free(header);
	free(scale);

	return 0;

}

void runmix(float *gather, float *out, float *work, int ntrace, int nt, 
	int nmix, float *scale) {

	int i, j, k, it;
	float *norm, tmp;

	for(i=0;i<ntrace*nt;i++) out[i] = 0.;
	norm = (float*) emalloc(ntrace*sizeof(float));
	for(i=0;i<ntrace;i++) norm[i] = 0.;

	for(j=0;j<nmix;j++) {
		tmp = scale[j];
		for(i=0;i<ntrace*nt;i++) work[i] = gather[i] * tmp;
		for(i=0;i<ntrace;i++) {
			k = i + nmix/2 - j;
			if(k>=0 && k<ntrace) {
				norm[i] = norm[i] + tmp;
				for(it=0;it<nt;it++)
					out[i*nt+it] = out[i*nt+it] + work[k*nt+it];
			}
		}
	}
	/* normalization */
	for(i=0;i<ntrace;i++) {
		if(norm[i]>0.) {
			tmp = 1./norm[i];
			for(it=0;it<nt;it++) out[i*nt+it] = out[i*nt+it]*tmp;
		}
	}

	free(norm);
}
