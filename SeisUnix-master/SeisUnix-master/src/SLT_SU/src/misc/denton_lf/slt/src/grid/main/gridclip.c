/* GRIDCLIP grid clip program */
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"GRIDCLIP - grid clip program				\n"
"\n"
"gridrvse [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"pclip=       maximum positive clip value (default to grid max positive)\n"
"nclip=       maximum negative clip value (default to grid max negative)\n"
"zero=0.0     vaule to be used outside the passing zone defined by 	\n"
"             (min1, max1, min2, max2, min3, max3, min4, max4, min5, max5) \n"
"min1=1       minimum 1st index of the passing zone	\n" 
"max1=n1      maximum 1st index of the passing zone	\n" 
"min2=1       minimum 2nd index of the passing zone	\n" 
"max2=n2      maximum 2nd index of the passing zone	\n" 
"min3=1       minimum 3rd index of the passing zone	\n" 
"max3=n3      maximum 3rd index of the passing zone	\n" 
"min4=1       minimum 4th index of the passing zone	\n" 
"max4=n4      maximum 4th index of the passing zone	\n" 
"min5=1       minimum 5th index of the passing zone	\n" 
"max5=n5      maximum 5th index of the passing zone	\n" 
"rbeg=        replaced value at beginning of clip value at each trace \n"
"rend=        replaced value at end of each trace \n"
"             when rbeg and rend are given, at any trace with a value \n"
"             larger than pclip, the remaing portion starting from this \n"
"             value will be linearly interpolated from rbeg to rend \n"
"Notes:									\n"
" 1. This program will check grid values and clip data within		\n"
"    (nclip,pclip) range						\n"
" 2. gmin and gmax will be computed and updated in the output grid 	\n"
"    header								\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/16/94   		\n"    
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

    	int n1,n2,n3,n4,n5;
	
	float *trace;
	int ierr, i2, n, i1;

	float pclip, nclip;
	int ip=1, in=1;

	int i3, i4, i5;

	float gmin, gmax, zero;
	int min1, min2, min3, min4, min5;
	int max1, max2, max3, max4, max5;

	float rbeg, rend, tmp;
	float ir=0, ibeg;

	usghed usgh;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	if(!getparfloat("pclip",&pclip)) ip = 0;
	if(!getparfloat("nclip",&nclip)) in = 0;

	/* get input grid parameters */
	ierr = fgetusghdr(infp,&usgh);
        if(ierr!=0) err(" nonstandard grid file ");

	n1 = usgh.n1;
        n2 = usgh.n2;
        n3 = usgh.n3;
        n4 = usgh.n4;
        n5 = usgh.n5;

	if(n2==0) n2=1;
	if(n3==0) n3=1;
	if(n4==0) n4=1;
	if(n5==0) n5=1;

	if(!getparint("min1",&min1)) min1 = 1;
	if(!getparint("min2",&min2)) min2 = 1;
	if(!getparint("min3",&min3)) min3 = 1;
	if(!getparint("min4",&min4)) min4 = 1;
	if(!getparint("min5",&min5)) min5 = 1;
	if(!getparint("max1",&max1)) max1 = n1;
	if(!getparint("max2",&max2)) max2 = n2;
	if(!getparint("max3",&max3)) max3 = n3;
	if(!getparint("max4",&max4)) max4 = n4;
	if(!getparint("max5",&max5)) max5 = n5;
	if(!getparfloat("zero",&zero)) zero = 0.;

	ir = 0;
	if(getparfloat("rbeg",&rbeg) && getparfloat("rend",&rend)) ir = 1;

	min1 = min1 - 1;
	min2 = min2 - 1;
	min3 = min3 - 1;
	min4 = min4 - 1;
	min5 = min5 - 1;
	max1 = max1 - 1;
	max2 = max2 - 1;
	max3 = max3 - 1;
	max4 = max4 - 1;
	max5 = max5 - 1;

	n = n2 * n3 * n4 * n5;

	trace = (float*) emalloc(n1*sizeof(float));
	efseek(infp,0,0);

	for(i5=0;i5<n5;i5++) {
	for(i4=0;i4<n4;i4++) {
	for(i3=0;i3<n3;i3++) {
	for(i2=0;i2<n2;i2++) {
		efread(trace,sizeof(float),n1,infp);
		if(in==1) {
			for(i1=0;i1<n1;i1++) {
				if(trace[i1]<nclip) trace[i1] = nclip;
			}
		}
		if(ir==0) {
			if(ip==1) {
			for(i1=0;i1<n1;i1++) {
				if(trace[i1]>pclip) trace[i1] = pclip;
			}
			}
		} else {
			ibeg = 0;
			for(i1=0;i1<n1;i1++) {
				if(trace[i1]>pclip) {
					ibeg=i1;
					break;
				}
			}
			if(ibeg!=0) {
				if(ibeg!=n1-1) {
					tmp = (rend - rbeg)/(n1-ibeg-1);
				} else {
					tmp = 0.;
				}
				for(i1=ibeg;i1<n1;i1++) {
					trace[i1] = rbeg + (i1-ibeg)*tmp;
				}
			}
		}

		if(i5<min5 || i5>max5 || 
		   i4<min4 || i4>max4 ||
		   i3<min3 || i3>max3 ||
		   i2<min2 || i2>max2 ) {
			for(i1=0;i1<n1;i1++) trace[i1] = zero;
		} else {
			for(i1=0;i1<min1;i1++) trace[i1] = zero;
			for(i1=max1+1;i1<n1;i1++) trace[i1] = zero;
		}
		if(i2==0 && i3==0 && i4==0 && i5==0 ) {
			gmin = trace[0];
			gmax = trace[0];
		}
		for(i1=0;i1<n1;i1++) {
			if(trace[i1]>gmax) gmax = trace[i1];
			if(trace[i1]<gmin) gmin = trace[i1];
		}
		efwrite(trace,sizeof(float),n1,outfp);
	}
	}
	}
	}
	
	usgh.gmin = gmin;
	usgh.gmax = gmax;
	ierr = fputusghdr(outfp,&usgh);
        if(ierr!=0) err("error in output gridheader");


	efclose(outfp);
	efclose(infp);

	free(trace);

	exit(0);
}
