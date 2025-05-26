/* LOG2HL program */
#include "par.h"

char *sdoc = 
"LOGZ2T - convert a log from depth to time \n"
"\n"
"logz2t <logz.file tz.file= [parameters] >logt.file			\n" 
"\n"
"Required parameters:							\n"
"logz.file=         file name of (depth, log value) pairs   \n"
"tz.file=           file name of (time, depth) pairs	\n"
"logt.file=         file name of (time, log value) pairs 	\n"
"Optional parameters:							\n"
"dt=                output time sampling rate	\n"
"ft=                first output time			\n"
"nt=                number of output samples	\n"
"                   when the above three parameters are specified \n"
"                   output will be resampled evenly; otherwise	\n"
"                   the output time(s) will be directly converted from \n"
"                   the input depth(s) via the tz function	\n"
"bv=                boundary value (default to the edges of the input log)\n"
"maxp=250000        maximum number of rows in the ascii file	\n"
"\n"
"Notes:							\n"
"  1. Input ascii log file contains 2 numbers per row:			\n"	
"     first position is depth 	\n"
"     second position is log value (e.g., impedence, density, etc.) 	\n"
"  2. Input tz file contains 2 numbers per row:		\n"
"     first position is time	\n"
"     second position is depth \n"
"  3. Both log and tz are from the same datum		\n"
"  4. This program can also be used to convert log from time to depth, \n"
"     input logz.file is (time, log), tz.file is (depth, time), \n"
"     and output logt.file is (depth, log)	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/8/97  		\n"
;


main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;
	FILE *tzfp;
	char buf[81], *tzfile;
	int nz, ntz;
	float *z, *t;
	float *zlog, *pzlog, *ptlog, *tlog;
	int iteven,ibv,nt;
	float bv,ft,dt;
	int one=1, itmp;
	int maxp=250000;
	int iz, it;
	float tmp;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */

	if( getparint("nt",&nt) &&
		getparfloat("ft",&ft) &&
		getparfloat("dt",&dt) ) {
		iteven = 1;
	} else {
		iteven = 0;
	}
	if( !getparfloat("bv",&bv) ) {
		ibv = 0;
	} else {
		ibv = 1;
	}	
	if( !getparint("maxp",&maxp) ) maxp=250000;

	if( !getparstring("tz.file",&tzfile) ) err(" tz.file missing ");
	tzfp = efopen(tzfile,"r");

	/* memory allocations */
    zlog = (float*)malloc(maxp*sizeof(float));
    tlog = (float*)malloc(maxp*sizeof(float));
    pzlog = (float*)malloc(maxp*sizeof(float));
    ptlog = (float*)malloc(maxp*sizeof(float));
    t = (float*)malloc(maxp*sizeof(float));
    z = (float*)malloc(maxp*sizeof(float));

	
	gets(buf);
	nz = 0;
	do {
		sscanf(buf,"%g %g\n",&zlog[nz],&pzlog[nz]);
		nz = nz + 1;
	} while(gets(buf));
	fprintf(stderr," total of %d rows read from input log file \n",nz);

	fgets(buf,81,tzfp);
	ntz = 0;
	do {
		sscanf(buf,"%g %g \n",&t[ntz],&z[ntz]);
		ntz = ntz + 1;
	} while(fgets(buf,81,tzfp));
	fprintf(stderr," total of %d rows read from tz file \n",ntz);

	for(iz=0;iz<nz;iz++) {
		tmp = zlog[iz];
		bisear_(&ntz,&one,z,&tmp,&itmp);
		itmp = itmp - 1;
		if(itmp<0 || tmp<z[0]) {
			tlog[iz] = t[0] + 
				(tmp-z[0])*(t[1]-t[0])/(z[1]-z[0]);
		} else if(itmp>=ntz-1) {
			tlog[iz] = t[ntz-2] + 
				(tmp-t[ntz-2])*(t[ntz-1]-t[ntz-2])/(z[ntz-1]-z[ntz-2]);
		} else {
			tlog[iz] = t[itmp] +
				(tmp-z[itmp])*(t[itmp+1]-t[itmp])/(z[itmp+1]-z[itmp]);
		}
	}

	if(iteven==0) {
		for(iz=0;iz<nz;iz++) printf(" %g %g \n",tlog[iz],pzlog[iz]);
	} else {
		for(it=0;it<nt;it++) {
			tmp = ft + it*dt;
			bisear_(&nz,&one,tlog,&tmp,&itmp);
			itmp = itmp - 1;
			if(itmp<0 || tmp<tlog[0]) {
				if(ibv==1) {
					ptlog[it] = bv;
				} else {
					ptlog[it] = pzlog[0];
				}
			} else if(itmp>=nz-1) {
				if(ibv==1) {
					ptlog[it] = bv;
				} else {
					ptlog[it] = pzlog[nz-1];
				}
			} else {
				ptlog[it] = pzlog[itmp] +
					(tmp-tlog[itmp])*(pzlog[itmp+1]-pzlog[itmp])/
								(tlog[itmp+1]-tlog[itmp]);
			}
			printf(" %g %g \n",tmp,ptlog[it]);
		}
	}

	fprintf(stderr," depth to time conversion of log done \n");

	free(tlog);
	free(zlog);
	free(t);
	free(z);
	free(pzlog);
	free(ptlog);

	exit(0);
}
