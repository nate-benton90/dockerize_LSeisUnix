/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"RESAMP - RESAMPle the 1st dimension of a 2-dimensional function f(x1,x2) \n"
"\n"
"resamp <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"\n"
"Optional Parameters:\n"
"n1=all                 number of samples in 1st (fast) dimension\n"
"n2=all                 number of samples in 2nd (slow) dimension\n"
"d1=1.0                 sampling interval in 1st dimension\n"
"f1=d1                  first sample in 1st dimension\n"
"n1r=n1                 number of samples in 1st dimension after resampling\n"
"d1r=d1                 sampling interval in 1st dimension after resampling\n"
"f1r=f1                 first sample in 1st dimension after resampling\n"
"\n"
"NOTE:  resamp currently performs NO ANTI-ALIAS FILTERING before resampling!\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 07/07/89\n"
"\n";

#include "par.h"

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,i1,i2,n1r,i1r;
	float d1,f1,d1r,f1r,x1r,*p,*pr,*xr;
	FILE *infp=stdin,*outfp=stdout;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters */
	if (!getparint("n1",&n1)) {
		if (fseek(infp,0L,2)==-1)
			err("must specify n1!\n");
		n1 = eftell(infp)/sizeof(float);
		efseek(infp,0L,0);
	}
	if (!getparint("n2",&n2)) n2 = -1;
	if (!getparfloat("d1",&d1)) d1 = 1.0;
	if (!getparfloat("f1",&f1)) f1 = d1;
	if (!getparint("n1r",&n1r)) n1r = n1;
	if (!getparfloat("d1r",&d1r)) d1r = d1;
	if (!getparfloat("f1r",&f1r)) f1r = f1;

	/* allocate space */
	p = ealloc1float(n1);
	pr = ealloc1float(n1r);
	xr = ealloc1float(n1r);

	/* compute output 1st dimension values */
	for (i1r=0,x1r=f1r; i1r<n1r; i1r++,x1r+=d1r)
		xr[i1r] = x1r;

	/* loop over 2nd dimension */
	for (i2=0; i2<n2 || n2<0; i2++) {

		/* read input data */
		if (efread(p,sizeof(float),n1,infp)!=n1) break;

		/* resample */
		ints8r(n1,d1,f1,p,0.0,0.0,n1r,xr,pr);

		/* write input data */
		efwrite(pr,sizeof(float),n1r,outfp);
	}
}
