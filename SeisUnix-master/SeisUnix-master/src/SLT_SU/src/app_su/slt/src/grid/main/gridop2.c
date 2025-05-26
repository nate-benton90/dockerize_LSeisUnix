char *sdoc =
"GRIDOP2 - grid operations  -- perform simple arithmetic with grid files\n"
"\n"
"gridop2 <infile >outfile [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"in=stdin       input grid file\n"
"out=stdout     output grid file\n"
"in2=           second input grid file (required for operations)\n"
"op=noop        noop for out = in\n"
"               neg  for out = -in\n"
"               abs  for out = abs(in)\n"
"               exp  for out = exp(in)\n"
"               log  for out = log(in)\n"
"               sqrt for out = sqrt(in)\n"
"               sqr  for out = in*in\n"
"               sca  for out = in*scale \n"
"               inv  for out = 1/in \n"
"               sft  for out = in + scale \n"
"               rep  for out = scale \n"
"               clp  for out = sign(in) * clip[fabs(in)] using clip of scale\n"
"               add  for out = in + in2\n"
"               sub  for out = in - in2\n"
"               mul  for out = in * in2\n"
"               div  for out = in / in2\n"
"scale=1        scale to be applied when op=sca	or sft or rep	\n"
"zero=0.        when value of in2 equals 0., substituted with value zero \n"
"               so that division is possible. or when op=sqrt, this 	\n"
"               value will be used to replaced negative values; 	\n"
"               or when op=log, it will be used to replaced 0; 	\n"
"               or when op=inv, it will be used to replaced 0; 	\n"
"norm=0         normalize input(s) before operation		\n"
"               input(s) maximum absolute amplitude will be scaled to 1	\n"
"               before operation					\n" 
"               1=yes 0=no					\n"
"\n"
"AUTHOR:  Zhiming Li,         06/09/94			\n"
"         modified from CWP Dave Hale's farith program		\n"
"\n";
#include "usgrid.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed usgh, usgh2;
	float x,x2,y;
	char *in,*in2,*out,*op="noop";
	FILE *infp,*in2fp,*outfp;
	float *gmin, *gmax, scale=1., zero=0.;
	int ierr;
	float *gin, *gout, *gin2;
	int n1,n2,n3,i1,i2,i3;
	int norm=0;
	float scale1, scale2;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if (getparstring("in",&in))
	{
		if ((infp=fopen(in,"r"))==NULL)
			err("Cannot open in=%s\n",in);
	} else {
		infp = stdin;
	}
	file2g(infp);
	ierr = fgetusghdr(infp,&usgh);
    if(ierr!=0) err(" input grid header error ");

	scale1 = fabs( usgh.gmax );
	if(scale1 < fabs( usgh.gmin ) ) scale1 = fabs( usgh.gmin );
	if(scale1!=0) {
		scale1 = 1./scale1;
	} else {
		scale1 = 1.;
	}

	
	getparstring("op",&op);
	if (STREQ(op,"add") ||
		STREQ(op,"sub") ||
		STREQ(op,"mul") ||
		STREQ(op,"div")) {
		if (!getparstring("in2",&in2))
			err("Must specify in2 for op=%s\n",op);
		if ((in2fp=fopen(in2,"r"))==NULL)
			err("Cannot open in2=%s",in2);
		file2g(in2fp);
		ierr = fgetusghdr(in2fp,&usgh2);
        	if(ierr!=0) err(" input2 grid header error ");
		/* make sure headers of two inputs match */
		if( (usgh2.n1 != usgh.n1) ||
		    (usgh2.n2 != usgh.n2) ||
		    (usgh2.n3 != usgh.n3) ||
		    (usgh2.n4 != usgh.n4) ||
		    (usgh2.n5 != usgh.n5) )
		err(" grid headers of input and input2 does not match");

		scale2 = fabs( usgh2.gmax );
		if(scale2 < fabs( usgh2.gmin ) ) scale1 = fabs( usgh2.gmin );
		if(scale2!=0) {
			scale2 = 1./scale2;
		} else {
			scale2 = 1.;
		}
	}
	if (getparstring("out",&out)) {
		if ((outfp=fopen(out,"w"))==NULL)
			err("Cannot open out=%s",out);
	} else {
		outfp = stdout;
	}
	file2g(outfp);

	getparfloat("scale",&scale);
	getparfloat("zero",&zero);
	if( !getparint("norm",&norm) ) norm=0;

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
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=gin[i1];
			} else {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=gin[i1]*scale1;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"neg")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=-gin[i1];
			} else {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=
								-gin[i1]*scale1;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"abs")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=fabs(gin[i1]);
			} else {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=fabs(
								gin[i1]*scale1);
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"exp")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=exp(gin[i1]);
			} else {
				for(i1=0;i1<n1*n2;i1++) gout[i1]=exp(
								gin[i1]*scale1);
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"log")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				if(norm==0) {
					x = gin[i1];
				} else {
					x = gin[i1]*scale1;
				}
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
				if(norm==0) {
					x = gin[i1];
				} else {
					x = gin[i1]*scale1;
				}
				if(x<0.) x = zero;
				gout[i1] = sqrt(x);
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"inv")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				if(norm==0) {
					x = gin[i1];
				} else {
					x = gin[i1]*scale1;
				}
				if(x==0.) x = zero;
				gout[i1] = 1./x;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sqr")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1]*gin[i1];
			} else {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1]*gin[i1]
							*scale1*scale1;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sca")) {
		if(norm==1) scale = scale * scale1;
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]*scale;
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sft")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1] + scale;
			} else {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1]*scale1 + scale;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"rep")) {
		for(i3=0;i3<n3;i3++) {
			for(i1=0;i1<n1*n2;i1++) 
					gout[i1] =  scale;
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"clp")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1*n2;i1++) {
				y = gin[i1];
				if(norm==1) y = y * scale1;
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
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1]+gin2[i1];
			} else {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1]=gin[i1]*scale1+gin2[i1]*scale2;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"sub")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			if(norm==0) {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1] = gin[i1]-gin2[i1];
			} else {
				for(i1=0;i1<n1*n2;i1++) 
					gout[i1]=gin[i1]*scale1-gin2[i1]*scale2;
			}
			efwrite(gout,sizeof(float),n1*n2,outfp);
			fminmax(gout,n1*n2,gmin+i3,gmax+i3);
		}
	} else if (STREQ(op,"mul")) {
		for(i3=0;i3<n3;i3++) {
			efread(gin,sizeof(float),n1*n2,infp);
			efread(gin2,sizeof(float),n1*n2,in2fp);
			for(i1=0;i1<n1*n2;i1++) gout[i1] = gin[i1]*gin2[i1];
			if(norm==1) 
			for(i1=0;i1<n1*n2;i1++) gout[i1]=gout[i1]*scale1*scale2;
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
			if(norm==1) 
			for(i1=0;i1<n1*n2;i1++) gout[i1]=gout[i1]*scale1/scale2;
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
