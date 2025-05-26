/* resample of an ascii file */

#include "velo.h"


char *sdoc = 
"ARESAM - resample first column of an ascii multi-column file 	\n"
"\n"
"aresam [parameters] <input >output			\n" 
"\n"
"Required parameters:						 	\n"
" o1            starting coordinate of colume 1 to output    \n"
" d1            coordinate increment of colume 1 to output    \n"
" n1            number of data points to output  \n"
" Or, \n"
" c1file=       file of specifying new column-1 positions     \n"
"                      p1    \n"
"                      p2    \n"
"                      p3    \n"
"                      p4    \n"
"                      .     \n"
"                      .     \n"
"                      .     \n"
"                      pn    \n"
"\n"
"Optional parameters:						 	\n"
"n2=2           number of columns of input to resample \n"
"               maximum of 10						\n"
"nrmax=10000    maximum number of rows in the input file \n"
"nmax=10000     maximum number of rows in the c1file \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	5/8/98   		\n"    
;

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
    	float  *cc;
		int n1=0, n2=0, nr, i1, i2, ir, i;
		float o1, d1;
		int nrmax, nmax;
		float *xin, *xout, *yin, *yout;
		int *indx;
		char *c1file;
		FILE *c1fp;



    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if(!getparstring("c1file",&c1file)) {
			if (!getparint("n1",&n1)) err(" n1 missing \n");
			if (!getparfloat("d1",&d1)) err(" d1 missing \n");
			if (!getparfloat("o1",&o1)) err(" o1 missing \n");
		} else {
			c1fp = fopen(c1file,"r");
			if (!getparint("nmax",&nmax)) nmax = 10000;
		}
		if (!getparint("n2",&n2)) n2 = 2;
		if (n2>10) err(" n2=%s is larger than 10 \n",n2);
		if (!getparint("nrmax",&nrmax)) nrmax = 10000;
	
/* memory allocation */
    	cbuf = (char*)malloc(134*sizeof(char));
    	cc = (float*)malloc(10*sizeof(float));
    	yin = (float*)malloc(nrmax*(n2-1)*sizeof(float));
		xin = (float*) malloc(nrmax*sizeof(float));

		if(n1==0) {
			xout = (float*) malloc(nmax*sizeof(float));
    		for (ir=0;ir<nmax;ir++) {
       			if (feof(c1fp) !=0 ) break;
       			for(i=0;i<134;i++) cbuf[i]=' ';
       			fgets(cbuf,134,c1fp);
				sscanf(cbuf,"%f\n",&xout[ir]);
			}
			n1 = ir-1;
		} else {
			xout = (float*) malloc(n1*sizeof(float));
			for(i1=0;i1<n1;i1++) xout[i1] = o1 + i1*d1;
		}
   		yout = (float*)malloc(n1*(n2-1)*sizeof(float));
		indx = (int*) malloc(n1*sizeof(int));

    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%f %f %f %f %f %f %f %f %f %f\n",
					&cc[0],&cc[1],&cc[2],&cc[3],&cc[4],
					&cc[5],&cc[6],&cc[7],&cc[8],&cc[9]);
			xin[ir] = cc[0];
			for(i2=0;i2<n2-1;i2++) yin[ir+i2*nrmax] = cc[i2+1];
			/*
			fprintf(stderr,"%g %g \n",xin[ir],yin[ir]);
			*/
		}

		nr = ir-1;

		bisear_(&nr,&n1,xin,xout,indx);
		for(i2=0;i2<n2-1;i2++)
			linin_(&nr,&n1,xin,xout,indx,yin+i2*nrmax,yout+i2*n1);

		for(i1=0;i1<n1;i1++) {
			fprintf(outfp," %g",xout[i1]);
			for(i2=0;i2<n2-1;i2++) fprintf(outfp," %g",yout[i2*n1+i1]);
			fprintf(outfp,"\n");
		}

     free(cbuf);
     free(xin);
     free(xout);
     free(cc);
     free(indx);
     free(yout);
     free(yin);

     return (0);
}

