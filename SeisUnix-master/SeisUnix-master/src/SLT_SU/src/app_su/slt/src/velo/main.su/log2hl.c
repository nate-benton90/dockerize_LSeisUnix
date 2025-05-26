/* LOG2HL program */
#include "par.h"

char *sdoc = 
"LOG2HL - Log to AVO Highlights conversion program					\n"
"\n"
"log2hl <log.file va.file= [parameters] >hl.file			\n" 
"\n"
"Required parameters:							\n"
"log.file=          file name of depth, impedance and possion ratio    \n"
"va.file=           file name of depth, average velocity		\n"
"hl.file=           file name of depth, va, r0 and h3 			\n"
"Optional parameters:							\n"
"maxp=250000        maximum number of rows in the ascii file	\n"
"\n"
"Notes:							\n"
"  Input ascii log file contains 3 numbers per row:			\n"	
"    first position is depth 	\n"
"    second position is impedance 	\n"
"    third position is poisson ratio 	\n"
"  Input ascii va file contains 2 numbers per row:		\n"
"    first position is depth	\n"
"    second position is average velocity	\n"
"AUTHOR:		Zhiming Li,       ,	2/5/96  		\n"
;


main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;
	FILE *vfp;

	char buf[81], *vafile;
	int maxp=250000,np,ip;
	float *is, *ps, *zs, *va, *r0, *h3, tmp;
	float *z, *v;
	int nzv, one=1, itmp;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */

	if( !getparint("maxp",&maxp) ) maxp=250000;
	if( !getparstring("va.file",&vafile) ) err(" va.file missing ");
	vfp = efopen(vafile,"r");

	/* memory allocations */
    zs = (float*)malloc(maxp*sizeof(float));
    is = (float*)malloc(maxp*sizeof(float));
    ps = (float*)malloc(maxp*sizeof(float));
    r0 = (float*)malloc(maxp*sizeof(float));
    h3 = (float*)malloc(maxp*sizeof(float));
    z = (float*)malloc(maxp*sizeof(float));
    v = (float*)malloc(maxp*sizeof(float));
    va = (float*)malloc(maxp*sizeof(float));

	
	gets(buf);
	np = 0;
	do {
		sscanf(buf,"%g %g %g \n",&zs[np],&is[np],&ps[np]);
		np = np + 1;
	} while(gets(buf));

	fprintf(stderr," total of %d rows read from input log file \n",np);

	fgets(buf,81,vfp);
	nzv = 0;
	do {
		sscanf(buf,"%g %g \n",&z[nzv],&v[nzv]);
		nzv = nzv + 1;
	} while(fgets(buf,81,vfp));

	fprintf(stderr," total of %d rows read from va file \n",nzv);

	for(ip=0;ip<np-1;ip++) {
		r0[ip] = (is[ip+1]-is[ip])/(is[ip+1]+is[ip]);
		tmp = 1.0-(ps[ip+1]+ps[ip])*0.5;
		h3[ip] = (ps[ip+1]-ps[ip]) / tmp / tmp;
	}

	r0[np-1] = r0[np-2];
	h3[np-1] = h3[np-2];

	for(ip=0;ip<np;ip++) {
		tmp = zs[ip];
		bisear_(&nzv,&one,z,&tmp,&itmp);
		itmp = itmp - 1;
		if(itmp<0 || nzv==1 || tmp<z[0]) {
			va[ip] = v[0] + 
				(tmp-z[0])*(v[1]-v[0])/(z[1]-z[0]);
		} else if(itmp>=nzv-1) {
			va[ip] = v[nzv-2] + 
				(tmp-z[nzv-2])*(v[nzv-1]-v[nzv-2])/(z[nzv-1]-z[nzv-2]);
		} else {
			va[ip] = v[itmp] +
				(tmp-z[itmp])*(v[itmp+1]-v[itmp])/(z[itmp+1]-z[itmp]);
		}

	}
	for(ip=0;ip<np;ip++) {
		printf("%g %g %g %g \n",zs[ip],va[ip],r0[ip],h3[ip]);
	}

	fprintf(stderr," log to avo highlights conversion done \n");
	free(va);
	free(v);
	free(zs);
	free(z);
	free(h3);
	free(r0);
	free(is);
	free(ps);

	exit(0);
}
