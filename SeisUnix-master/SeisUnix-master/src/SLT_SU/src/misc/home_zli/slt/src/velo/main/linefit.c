/* linear fit of data points */

#include "velo.h"
#include "subc.h"


char *sdoc = 
"LINEFIT - fit a line y=a+bx through data points (xi, yi) \n"
"\n"
"linefit [parameters] <input >output 						  \n" 
"\n"
"Required parameters:						 	\n"
" input         file containing 2-columns (xi, yi) data points \n"
" output        file containing a and b values  \n"
"              \n" 
"\n"
"Optional parameters:						 	\n"
"nrmax=100000   maximum number of rows in the input file \n"
"xmin=          minimum x value of xi to be used in the line fitting \n" 
"               (default to the minimum data value in the input) \n"
"xmax=          maximum x value of xi to be used in the line fitting \n" 
"               (default to the maximum data value in the input) \n"
"plot=1         1=create a x-window graph; 0=no   \n"
"\n "
"AUTHOR:		Zhiming Li,       ,	6/10/98   		\n"    
;

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
		float *xin, *yin, *xy;
		float *x, *y;
		float xmin, xmax;
		float x0, xn;
		float a, b;
		int imin, imax;
		int nrmax, plot;
		int ir, nr, i, n;
		char cmd[2048];
		FILE *cmpfp;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparint("nrmax",&nrmax)) nrmax=100000;
		imin = 1;
		if (!getparfloat("xmin",&xmin)) imin=0;
		imax = 1;
		if (!getparfloat("xmax",&xmax)) imax=0;
		if (!getparint("plot",&plot)) plot=1;

/* memory allocation */
    	cbuf = (char*)malloc(134*sizeof(char));
    	yin = (float*)malloc(nrmax*sizeof(float));
		xin = (float*) malloc(nrmax*sizeof(float));
    	y = (float*)malloc(nrmax*sizeof(float));
		x = (float*) malloc(nrmax*sizeof(float));


		n = 0;
    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%f %f \n",&xin[ir],&yin[ir]);
			if(imin==0 && imax==0) {
				x[n] = xin[ir];
				y[n] = yin[ir];
				n += 1;
			} else {
				if((imin==0 || xin[ir]>=xmin) && (imax==0 || xin[ir]<=xmax)) {
					x[n] = xin[ir];
					y[n] = yin[ir];
					n += 1;
				}	
			}
		}
		nr = ir-1;

		linefit(x,y,n,&a,&b);

		fprintf(outfp," Linear Fit y=a+bx of Input Data Points (xi,yi) \n");
		fprintf(outfp,"   Number of Input Data Points = %d \n", nr);
		fprintf(outfp,"   Number of Points in linefit = %d \n", n);
		fprintf(outfp,"   Computed value of a         = %f \n", a);
		fprintf(outfp,"   Computed value of b         = %f \n", b);

		if(plot==1) {
			x0 = xin[0];
			xn = xin[0];
			xy = (float*) malloc(2*(nr+2)*sizeof(float));
			for(i=0;i<nr;i++) { 
				xy[i*2] = xin[i]; 
				xy[i*2+1] = yin[i]; 
				if(x0>xin[i]) x0 = xin[i];
				if(xn<xin[i]) xn = xin[i];
			}
			xy[2*nr] = x0;
			xy[2*nr+1] = a + b * x0;
			xy[2*nr+2] = xn;
			xy[2*nr+3] = a + b * xn;
sprintf(cmd,"xgraph n=%d,2 title=linefit label1=x label2=y grid1=solid grid2=solid nTic1=10 nTic2=10 marksize=4,0 linewidth=0,1",nr);
			cmpfp = epopen(cmd,"w");
			efwrite(xy,sizeof(float),(nr+2)*2,cmpfp);
			efclose(cmpfp);
		}

     free(cbuf);
     free(xin);
     free(yin);
     free(x);
     free(y);

     return (0);
}
