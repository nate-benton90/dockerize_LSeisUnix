char *sdoc =
"GRIDPLOT - grid plots  -- perform x-window/postscript plot \n"
"\n"
"gridplot <infile [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"in=stdin       input grid file		\n"
"op=xgraph                xgraph of in		\n"
"               ximage    ximage of in		\n"
"               xwigb     xwigb of in		\n"
"               psgraph   psgraph of in		\n"
"               psimage   psimage of in		\n"
"               pswigb    pswigb of in		\n"
"d1=            sampling interval anlong 1st axis (default from header)	\n"
"               if 0, a histogram will be generated 	\n"
"d2=            sampling interval anlong 2nd axis (default from gridheader)\n"
"bclip=         max clip value (default from header)		\n" 
"wclip=         min clip value (default from header)		\n" 
"\n"
"AUTHOR:  Zhiming Li,         06/09/94			\n"
"\n";
#include "usgrid.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed usgh, usgh2;
	float x,x2,y;
	char *in,*op="xgraph";
	FILE *infp,*in2fp,*outfp;
	float *gmin, *gmax, scale=1., zero=0.;
	int ierr;
	float *gin, *gout, *gin2;
	int n1,n2,n3,i1,i2,i3;

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
	ierr = fgetusghdr(infp,&usgh);
        if(ierr!=0) err(" input grid header error ");

	
	getparstring("op",&op);
	if (STREQ(op,"add") ||
		STREQ(op,"sub") ||
		STREQ(op,"mul") ||
		STREQ(op,"div")) {
		if (!getparstring("in2",&in2))
			err("Must specify in2 for op=%s\n",op);
		if ((in2fp=fopen(in2,"r"))==NULL)
			err("Cannot open in2=%s",in2);
		ierr = fgetusghdr(in2fp,&usgh2);
        	if(ierr!=0) err(" input2 grid header error ");
		/* make sure headers of two inputs match */
		if( (usgh2.n1 != usgh.n1) ||
		    (usgh2.n2 != usgh.n2) ||
		    (usgh2.n3 != usgh.n3) ||
		    (usgh2.n4 != usgh.n4) ||
		    (usgh2.n5 != usgh.n5) )
		err(" grid headers of input and input2 does not match");
	}
	if (getparstring("out",&out))
	{
		if ((outfp=fopen(out,"w"))==NULL)
			err("Cannot open out=%s",out);
	}
	else
		outfp = stdout;

	getparfloat("scale",&scale);
	getparfloat("zero",&zero);

	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = 1;
	if(usgh.n3!=0) n3 = n3*usgh.n3;
	if(usgh.n4!=0) n3 = n3*usgh.n4;
	if(usgh.n5!=0) n3 = n3*usgh.n5;

	gin = (float*) emalloc(n1*n2*sizeof(float));
	gin2 = (float*) emalloc(n1*n2*sizeof(float));
	gout = (float*) emalloc(n1*n2*sizeof(float));
	gmin = (float*) emalloc(n3*sizeof(float));
	gmax = (float*) emalloc(n3*sizeof(float));


	/* do the arithmetic operation */
	if (STREQ(op,"noop")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1]=gin[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"neg")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = -gin[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"abs")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = fabs(gin[i1]);
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"exp")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = exp(gin[i1]);
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"log")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				x = gin[i1];
				if(x==0.) x = zero;
				gout[i1] = log(x);
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sqrt")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				if(x==0.) x = zero;
				gout[i1] = sqrt(x);
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sqr")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]*gin[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sca")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]*scale;
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sft")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1] + scale;
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"clp")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				y = gin[i1];
				if(fabs(y)>scale) { 
					if(y>0.) {
						gout[i1] = scale;
					} else {
						gout[i1] = -scale;
					}
				} else { 
					gout[i1] = y;
				}
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"add")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]+gin2[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sub")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]-gin2[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"mul")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]*gin2[i1];
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"div")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			for(i1=0;i1<n1*n2;i1++) {
				x = gin2[i1];
				if(x==0.) x = zero;
				gout[i1] = gin[i1]/x;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else {
		err("op=%s is not a valid operation",op);
	}

	fminmax(gmin,n3,&x,&y);
	usgh.gmin = x;
	fminmax(gmax,n3,&x,&y);
	usgh.gmax = y;

	ierr = fputusghdr(outfp,&usgh);
	if(ierr!=0) err("error in output gridheader"); 

	exit(0);
}
