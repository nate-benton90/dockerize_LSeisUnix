/* GRIDSTACK grid stack program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDSTACK - grid stack program					\n"
"\n"
"gridstack [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"axistk=4           axis along which the stacking is taken place \n"
"                   (2<=axis<=5)		\n"
"\n"
"NOTE: \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	2/13/2002   \n"		    
;

int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

    	int n1,n2,n3,n4,n5;
    	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient,gtype;
	int axistk;
	int i1, i2, i3, i4, i5;
	
	int ierr;
	float *stack, *trace;

	ghed gh;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input grid parameters */
	file2g(infp);
    	ierr = fgetghdr(infp,&gh);
	if(ierr==0) {
		fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
               	 		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);
	} else {
		err(" nonstandard grid file"); 
	}

	/* get update parameters */
	if (!getparint("axistk", &axistk)) axistk = 4;
	if (axistk<2 || axistk>5) err(" check axistk=%d \n",axistk);
	
	trace = (float*) emalloc(n1*sizeof(float));
	if(axistk==2) {
		stack = (float*) emalloc(n1*sizeof(float));
		bzero(stack,n1*sizeof(float));
	} else if(axistk==3) {
		stack = (float*) emalloc(n1*n2*sizeof(float));
		bzero(stack,n1*n2*sizeof(float));
	} else if(axistk==4) {
		stack = (float*) emalloc(n1*n2*n3*sizeof(float));
		bzero(stack,n1*n2*n3*sizeof(float));
	} else if(axistk==5) {
		stack = (float*) emalloc(n1*n2*n3*n4*sizeof(float));
		bzero(stack,n1*n2*n3*n4*sizeof(float));
	}

	gmin = 1.e38;
	gmax = -1.e38;

	file2g(outfp);

	if(axistk==2) {

		for(i5=0;i5<n5;i5++) {
		for(i4=0;i4<n4;i4++) {
		for(i3=0;i3<n3;i3++) {

		for(i2=0;i2<n2;i2++) {
			efread(trace,sizeof(float),n1,infp);
			for(i1=0;i1<n1;i1++) 
				stack[i1] += trace[i1];
		}
		efwrite(stack,sizeof(float),n1,outfp);
		for(i1=0;i1<n1;i1++) {
			if(gmin>stack[i1]) gmin = stack[i1];
			if(gmax<stack[i1]) gmax = stack[i1];
		}

		}
		}
		}
		o2 = o2 + (n2-1)/2*d2;
		n2 = 1;

	} else if(axistk==3) {

		for(i5=0;i5<n5;i5++) {
		for(i4=0;i4<n4;i4++) {

		for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(trace,sizeof(float),n1,infp);
			for(i1=0;i1<n1;i1++) 
				stack[i1+i2*n1] += trace[i1];
		}
		}
		efwrite(stack,sizeof(float),n1*n2,outfp);
		for(i1=0;i1<n1*n2;i1++) {
			if(gmin>stack[i1]) gmin = stack[i1];
			if(gmax<stack[i1]) gmax = stack[i1];
		}

		}
		}
		o3 = o3 + (n3-1)/2*d3;
		n3 = 1;

	} else if(axistk==4) {

		for(i5=0;i5<n5;i5++) {

		for(i4=0;i4<n4;i4++) {
		for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(trace,sizeof(float),n1,infp);
			for(i1=0;i1<n1;i1++) 
				stack[i1+i2*n1+i3*n1*n2] += trace[i1];
		}
		}
		}
		efwrite(stack,sizeof(float),n1*n2*n3,outfp);
		for(i1=0;i1<n1*n2*n3;i1++) {
			if(gmin>stack[i1]) gmin = stack[i1];
			if(gmax<stack[i1]) gmax = stack[i1];
		}

		}
		o4 = o4 + (n4-1)/2*d4;
		n4 = 1;

	} else if(axistk==5) {

		for(i5=0;i5<n5;i5++) {
		for(i4=0;i4<n4;i4++) {
		for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(trace,sizeof(float),n1,infp);
			for(i1=0;i1<n1;i1++) 
				stack[i1+i2*n1+i3*n1*n2+i4*n1*n2*n3] += 
					trace[i1];
		}
		}
		}
		}
		efwrite(stack,sizeof(float),n1*n2*n3*n4,outfp);
		for(i1=0;i1<n1*n2*n3*n4;i1++) {
			if(gmin>stack[i1]) gmin = stack[i1];
			if(gmax<stack[i1]) gmax = stack[i1];
		}

		o5 = o5 + (n5-1)/2*d5;
		n5 = 1;

	}

	free(trace);
	free(stack);
	fflush(outfp);

	toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
               &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
               &dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,&orient,&gtype);
	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) warn(" fputghdr error \n");

	fclose(outfp);
	fclose(infp);

	exit(0);
}
