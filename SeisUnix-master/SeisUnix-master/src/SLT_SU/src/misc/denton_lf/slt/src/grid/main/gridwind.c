/* GRIDWIND grid windoe program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDWIND - grid window program						\n"
"\n"
"gridwind [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"n1=same-as-header  number of samples along 1st dimension to output	\n"
"n2=same-as-header  number of samples along 2nd dimension to output	\n"
"n3=same-as-header  number of samples along 3nd dimension to output 	\n"
"n4=same-as-header  number of samples along 4th dimension to output	\n"
"n5=same-as-header  number of samples along 5th dimension to output	\n"
"d1=1               sample rate (in samples) along 1st dimension to output \n"
"d2=1               sample rate (in samples) along 2nd dimension to output \n"
"d3=1               sample rate (in samples) along 3rd dimension to output \n"
"d4=1               sample rate (in samples) along 4th dimension to output \n"
"d5=1               sample rate (in samples) along 5th dimension to output \n"
"o1=1               starting sample index along 1st dimension to output \n"
"o2=1               starting sample index along 2nd dimension to output \n"
"o3=1               starting sample index along 3rd dimension to output \n"
"o4=1               starting sample index along 4th dimension to output \n"
"o5=1               starting sample index along 5th dimension to output \n"
"\n"
"NOTE: \n"
" 1. n1, n2, n3, n4 and n5 will default to those values found in the 	\n"
"    input grid header.							\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/26/92   \n"		    
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

    	int n1,n2,n3,n4,n5;
    	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient,gtype;
	
    	int n1o,n2o,n3o,n4o,n5o;
    	int o1o,o2o,o3o,o4o,o5o;
	int d1o,d2o,d3o,d4o,d5o;
	int i1,i2,i3,i4,i5;

	long long iseek;
	int ierr;
	char *trace,tmp[4];
	float ftmp;

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
		err(" use program window for nonstandard grid file"); 
	}

	/* get update parameters */
	if (!getparint("n1", &n1o)) n1o = n1;
	if (!getparint("n2", &n2o)) n2o = n2;
	if (!getparint("n3", &n3o)) n3o = n3;
	if (!getparint("n4", &n4o)) n4o = n4;
	if (!getparint("n5", &n5o)) n5o = n5;
	if (!getparint("d1", &d1o)) d1o = 1;
	if (!getparint("d2", &d2o)) d2o = 1;
	if (!getparint("d3", &d3o)) d3o = 1;
	if (!getparint("d4", &d4o)) d4o = 1;
	if (!getparint("d5", &d5o)) d5o = 1;
	if (!getparint("o1", &o1o)) o1o = 1;
	if (!getparint("o2", &o2o)) o2o = 1;
	if (!getparint("o3", &o3o)) o3o = 1;
	if (!getparint("o4", &o4o)) o4o = 1;
	if (!getparint("o5", &o5o)) o5o = 1;
	/* error check */
	if(o1o<1) o1o=1; if(o1o>n1) o1o=n1;
	if(o2o<1) o2o=1; if(o2o>n2) o2o=n2;
	if(o3o<1) o3o=1; if(o3o>n3) o3o=n3;
	if(o4o<1) o4o=1; if(o4o>n4) o4o=n4;
	if(o5o<1) o5o=1; if(o5o>n5) o5o=n5;
	if(o1o+(n1o-1)*d1o>n1) n1o = (n1-o1o)/d1o+1;
	if(o2o+(n2o-1)*d2o>n2) n2o = (n2-o2o)/d2o+1;
	if(o3o+(n3o-1)*d3o>n3) n3o = (n3-o3o)/d3o+1;
	if(o4o+(n4o-1)*d4o>n4) n4o = (n4-o4o)/d4o+1;
	if(o5o+(n5o-1)*d5o>n5) n5o = (n5-o5o)/d5o+1;
	
	trace = (char*) malloc(n1*dtype);

	file2g(outfp);

	if (dtype==4) {
		gmin = 1.e38;
		gmax = -1.e38;
	}


	for(i5=o5o-1;i5<o5o+(n5o-1)*d5o;i5=i5+d5o) {
	for(i4=o4o-1;i4<o4o+(n4o-1)*d4o;i4=i4+d4o) {
	for(i3=o3o-1;i3<o3o+(n3o-1)*d3o;i3=i3+d3o) {
	for(i2=o2o-1;i2<o2o+(n2o-1)*d2o;i2=i2+d2o) {
		iseek = dtype*n1;
		iseek = iseek*( i2 + i3*n2 
					+ i4*n3*n2 
					+ i5*n4*n3*n2 );
		fseek2g(infp,iseek,0);
		efread(trace,dtype,n1,infp);
		for(i1=o1o-1;i1<o1o+(n1o-1)*d1o;i1=i1+d1o) { 
			bcopy(&trace[i1*dtype],&tmp[0],dtype);
			if(dtype==4) {
				bcopy(&trace[i1*dtype],&ftmp,4);
				if(gmin>ftmp) gmin=ftmp;
				if(gmax<ftmp) gmax=ftmp;
			}
			efwrite(&tmp[0],dtype,1,outfp);
		}
	}
	}
	}
	}

	free(trace);
	fflush(outfp);

	o1 = o1 + (o1o-1)*d1;
	o2 = o2 + (o2o-1)*d2;
	o3 = o3 + (o3o-1)*d3;
	o4 = o4 + (o4o-1)*d4;
	o5 = o5 + (o5o-1)*d5;
	ocdp2 = ocdp2 + (o2o-1)*dcdp2;
	oline3 = oline3 + (o3o-1)*dline3;

	d1 = d1 * d1o;
	d2 = d2 * d2o;
	d3 = d3 * d3o;
	d4 = d4 * d4o;
	d5 = d5 * d5o;
	dcdp2 = dcdp2 * d2o;
	dline3 = dline3 * d3o;

	toghdr(&gh,&scale,&dtype,&n1o,&n2o,&n3o,&n4o,&n5o,
               &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
               &dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,&orient,&gtype);
	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) warn(" fputghdr error \n");

	efclose(outfp);
	efclose(infp);

	exit(0);
}
