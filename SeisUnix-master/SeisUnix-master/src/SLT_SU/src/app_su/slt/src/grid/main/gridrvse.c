/* GRIDRVSE grid axis reverse program */
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"GRIDRVSE - grid first axis reverse program				\n"
"\n"
"gridrvse [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"None									\n"
"Notes:									\n"
" 1. This program reverses the first axis of input grid file		\n"
"    so that the output will start at o1+(n1-1)*d1, and the		\n"
"    increment will be -d1						\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/16/94   		\n"    
;

int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

    	int n1,n2,n3,n4,n5;
    	float o1,d1;
	
	float *trace,*data;
	int ierr, i2, n, i1;

	usghed usgh;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input grid parameters */
	ierr = fgetusghdr(infp,&usgh);
        if(ierr!=0) err(" use program reverse for nonstandard grid file ");

	n1 = usgh.n1;
        n2 = usgh.n2;
        n3 = usgh.n3;
        n4 = usgh.n4;
        n5 = usgh.n5;
	o1 = usgh.o1;
	d1 = usgh.d1;

	if(n2==0) n2=1;
	if(n3==0) n3=1;
	if(n4==0) n4=1;
	if(n5==0) n5=1;

	n = n2 * n3 * n4 * n5;

	trace = (float*) emalloc(n1*sizeof(float));
	data = (float*) emalloc(n1*sizeof(float));
	efseek(infp,0,0);

	for(i2=0;i2<n;i2++) {
		efread(trace,sizeof(float),n1,infp);
		for(i1=0;i1<n1;i1++) data[i1] = trace[n1-i1-1];
		efwrite(data,sizeof(float),n1,outfp);
	}
	
	usgh.o1 = o1 + (n1-1)*d1;
	usgh.d1 = -d1;
	ierr = fputusghdr(outfp,&usgh);
        if(ierr!=0) err("error in output gridheader");


	efclose(outfp);
	efclose(infp);

	free(trace);
	free(data);

	exit(0);
}
