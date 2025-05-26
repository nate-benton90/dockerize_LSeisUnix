/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"FARITH - File ARITHmetic -- perform simple arithmetic with binary files\n"
"\n"
"farith <infile >outfile [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"in=stdin       input file\n"
"out=stdout     output file\n"
"in2=           second input file (required for binary operations)\n"
"op=noop        noop for out = in\n"
"               neg  for out = -in\n"
"               abs  for out = abs(in)\n"
"               exp  for out = exp(in)\n"
"               log  for out = log(in)\n"
"               sqrt for out = sqrt(in)\n"
"               mean for out = mean(in) \n"
"               medium for out = medium(in) \n"
"               sqr  for out = in*in\n"
"               add  for out = in + in2\n"
"               sub  for out = in - in2\n"
"               mul  for out = in * in2\n"
"               div  for out = in / in2\n"
"lwin=1         number of samples used in mean or medium operation \n" 
"n1=1           number of samples per trace in mean and medium operations \n" 
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 07/07/89\n"
"\n";

#include "par.h"

main(int argc, char **argv)
{
	float x,x2,y;
	char *in,*in2,*out,*op="noop";
	FILE *infp,*in2fp,*outfp;
	int lwin, n1, i1,it,ii, lwinh;
	float *data, *trace, *outs;
	float tmp, scale;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if (getparstring("in",&in))
	{
		if ((infp=fopen(in,"r"))==NULL)
			err("Cannot open in=%s\n",in);
	}
	else
		infp = stdin;
	getparstring("op",&op);
	if (STREQ(op,"add") ||
		STREQ(op,"sub") ||
		STREQ(op,"mul") ||
		STREQ(op,"div")) {
		if (!getparstring("in2",&in2))
			err("Must specify in2 for op=%s\n",op);
		if ((in2fp=fopen(in2,"r"))==NULL)
			err("Cannot open in2=%s",in2);
	}
	if (getparstring("out",&out))
	{
		if ((outfp=fopen(out,"w"))==NULL)
			err("Cannot open out=%s",out);
	}
	else
		outfp = stdout;

	if (!getparint("n1",&n1)) n1=1;
	if (!getparint("lwin",&lwin)) lwin=1;
	lwinh = lwin/2;
	if (STREQ(op,"mean") || STREQ(op,"medium")) {
		data = (float*) emalloc(lwin*sizeof(float));
		trace = (float*) emalloc(n1*sizeof(float));
		outs = (float*) emalloc(n1*sizeof(float));
		scale = 1./lwin;
	}


	/* do the arithmetic operation */
	if (STREQ(op,"noop")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = x;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"neg")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = -x;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"abs")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = fabs(x);
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"exp")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = exp(x);
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"log")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = log(x);
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"sqrt")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = log(x);
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"sqr")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			y = x*x;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"mean")) {
		while(efread(trace,sizeof(float),n1,infp)!=0) {
			for(i1=0;i1<n1;i1++) {
				tmp = 0;
				for(it=0;it<lwin;it++) {
					ii=i1+it-lwin/2; 
					if(ii<0) ii = 0;
					if(ii>n1-1) ii = n1-1;
					tmp += trace[ii];
				}
				if(lwin>1) tmp = tmp*scale;
                        	outs[i1] = tmp;
			}
			efwrite(outs,sizeof(float),n1,outfp);
		}
	} else if (STREQ(op,"medium")) {
		while(efread(trace,sizeof(float),n1,infp)!=0) {
			for(i1=0;i1<n1;i1++) {
				for(it=0;it<lwin;it++) {
					ii=i1+it-lwin/2; 
					if(ii<0) ii = 0;
					if(ii>n1-1) ii = n1-1;
					data[it] = trace[ii]; 
				}
				if(lwinh>0) qkfind(lwinh,lwin,data);
                        	outs[i1] = data[lwinh];
			}
			efwrite(outs,sizeof(float),n1,outfp);
		}
	} else if (STREQ(op,"add")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			if (efread(&x2,sizeof(float),1,in2fp)==0)
				err("error or end of file reading in2 file");
			y = x+x2;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"sub")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			if (efread(&x2,sizeof(float),1,in2fp)==0)
				err("error or end of file reading in2 file");
			y = x-x2;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"mul")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			if (efread(&x2,sizeof(float),1,in2fp)==0)
				err("error or end of file reading in2 file");
			y = x*x2;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else if (STREQ(op,"div")) {
		while(efread(&x,sizeof(float),1,infp)!=0) {
			if (efread(&x2,sizeof(float),1,in2fp)==0)
				err("error or end of file reading in2 file");
			y = x/x2;
			efwrite(&y,sizeof(float),1,outfp);
		}
	} else {
		err("op=%s is not a valid operation",op);
	}

	if (STREQ(op,"mean") || STREQ(op,"medium")) {
		free(data);
		free(outs);
		free(trace);
	}

	exit(0);
}
