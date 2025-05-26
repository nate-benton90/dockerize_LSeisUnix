/* GRIDTRSP grid transpose program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDTRSP - 3D grid transpose program					\n"
"\n"
"gridtrsp [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"oaxes=312       output axes order					\n"
"                123=  no transpose; output is same as input		\n"
"                231=  output axes (1,2,3) are 2,3,1 of input axes      \n" 
"                132=  output axes (1,2,3) are 1,3,2 of input axes      \n" 
"                312=  output axes (1,2,3) are 3,1,2 of input axes      \n" 
"                213=  output axes (1,2,3) are 2,1,3 of input axes      \n" 
"\n"
"AUTHOR:		Zhiming Li,       ,	9/14/93   		\n"    
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

    	int n1,n2,n3,n4,n5;
    	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient, gtype=0;
	
    	int n1o,n2o,n3o,n4o,n5o;
    	float o1o,o2o,o3o,o4o,o5o;
	float d1o,d2o,d3o,d4o,d5o;
	int i1,i2,i3,i4,i5;

	int ierr, iseek, oaxes, ltrace, larray;
	float *trace, tmp, *array;
	float sscale;

	ghed gh;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input grid parameters */
    	ierr = fgetghdr(infp,&gh);
	if(ierr==0) {
		fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
               	 		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);
	} else {
		err(" use program transp for nonstandard grid file"); 
	}

	/* get update parameters */
	if (!getparint("oaxes", &oaxes)) oaxes = 312;

	if(n2==0) n2=1;
	if(n3==0) n3=1;
	if(n4==0) n4=1;
	if(n5==0) n5=1;

	if (oaxes==123) {
		trace = (float*) malloc(n1*sizeof(float));
		for(i2=0;i2<n2*n3*n4*n5;i2++) {
			efread(trace,sizeof(float),n1,infp);
			efwrite(trace,sizeof(float),n1,outfp);
		}
		free(trace);
		fflush(outfp);
		ierr = fputghdr(outfp,&gh);
		if(ierr!=0) warn(" fputghdr error \n");
	} else if(oaxes==213) {
		trace = (float*) malloc(n2*sizeof(float));
		array = (float*) malloc(n1*n2*sizeof(float));
		for(i3=0;i3<n3*n4*n5;i3++) {
			efread(array,sizeof(float),n1*n2,infp);
			for(i1=0;i1<n1;i1++) {
				for(i2=0;i2<n2;i2++) 
					trace[i2] = array[i1+i2*n1];
				efwrite(trace,sizeof(float),n2,outfp);
			}
		}
		free(trace);
		free(array);
		n1o = n2;
		n2o = n1;
		n3o = n3;
		d1o = d2;
		d2o = d1;
		d3o = d3;
		o1o = o2;
		o2o = o1;
		o3o = o3;
		if(orient==1) {
			orient = 3;
		} else if (orient==2) {
		        orient = 5;
		} else if (orient==3) {
		      	orient = 1;
		} else if (orient==4) {
		       	orient = 6;
		} else if (orient==5) {
		       	orient = 2;
		} else if (orient==6) {
		       	orient = 4;
		}
	
		fflush(outfp);
		toghdr(&gh,&scale,&dtype,&n1o,&n2o,&n3o,&n4,&n5,
               		&d1o,&d2o,&d3o,&d4,&d5,&o1o,&o2o,&o3o,&o4,&o5,
               		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);
		ierr = fputghdr(outfp,&gh);
		if(ierr!=0) warn(" fputghdr error \n");
	} else if(oaxes==132) {
		trace = (float*) malloc(n1*sizeof(float));
		for(i5=0;i5<n5;i5++) {
			for(i4=0;i4<n4;i4++) {
				for(i2=0;i2<n2;i2++) {
					for(i3=0;i3<n3;i3++) {
						iseek = sizeof(float)*n1*
							(i2+i3*n2 
							+ i4*n3*n2+i5*n4*n3*n2);
						efseek(infp,iseek,0);
						efread(trace,sizeof(float),
							n1,infp);
						efwrite(trace,sizeof(float),
							n1,outfp);
					}
				}
			}
		}
		free(trace);
		n1o = n1;
		n2o = n3;
		n3o = n2;
		d1o = d1;
		d2o = d3;
		d3o = d2;
		o1o = o1;
		o2o = o3;
		o3o = o2;
		if(orient==1) {
			orient = 2;
		} else if (orient==2) {
		        orient = 1;
		} else if (orient==3) {
		      	orient = 4;
		} else if (orient==4) {
		       	orient = 3;
		} else if (orient==5) {
		       	orient = 6;
		} else if (orient==6) {
		       	orient = 5;
		}
		fflush(outfp);
		toghdr(&gh,&scale,&dtype,&n1o,&n2o,&n3o,&n4,&n5,
               		&d1o,&d2o,&d3o,&d4,&d5,&o1o,&o2o,&o3o,&o4,&o5,
               		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);
		ierr = fputghdr(outfp,&gh);
		if(ierr!=0) warn(" fputghdr error \n");
	} else if(oaxes==312) {
		trace = (float*) malloc(n3*sizeof(float));
		array = (float*) malloc(n1*n3*sizeof(float));
		for(i5=0;i5<n5;i5++) {
			for(i4=0;i4<n4;i4++) {
				for(i2=0;i2<n2;i2++) {
					for(i3=0;i3<n3;i3++) {
						iseek = sizeof(float)*n1*
							(i2+i3*n2 
							+ i4*n3*n2+i5*n4*n3*n2);
						efseek(infp,iseek,0);
						efread(array+i3*n1,
							sizeof(float),n1,infp);
					}
					for(i1=0;i1<n1;i1++) {
						for(i3=0;i3<n3;i3++)
							trace[i3]=
								array[i3*n1+i1];
						efwrite(trace,sizeof(float),
							n3,outfp);
					}
				}
			}
		}
		free(trace);
		free(array);
		n1o = n3;
		n2o = n1;
		n3o = n2;
		d1o = d3;
		d2o = d1;
		d3o = d2;
		o1o = o3;
		o2o = o1;
		o3o = o2;
		if(orient==1) {
			orient = 5;
		} else if (orient==2) {
		        orient = 3;
		} else if (orient==3) {
		      	orient = 6;
		} else if (orient==4) {
		       	orient = 1;
		} else if (orient==5) {
		       	orient = 4;
		} else if (orient==6) {
		       	orient = 2;
		}
		fflush(outfp);
		toghdr(&gh,&scale,&dtype,&n1o,&n2o,&n3o,&n4,&n5,
               		&d1o,&d2o,&d3o,&d4,&d5,&o1o,&o2o,&o3o,&o4,&o5,
               		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);
		ierr = fputghdr(outfp,&gh);
		if(ierr!=0) warn(" fputghdr error \n");
	} else if(oaxes==231) {
		trace = (float*) malloc(n2*sizeof(float));
		array = (float*) malloc(n1*n2*sizeof(float));
		for(i5=0;i5<n5;i5++) {
			for(i4=0;i4<n4;i4++) {
				for(i3=0;i3<n3;i3++) {
					iseek = sizeof(float)*n1*n2*(i3 
							+ i4*n3+i5*n4*n3);
					efseek(infp,iseek,0);
					efread(array,sizeof(float),n1*n2,infp);
					for(i1=0;i1<n1;i1++) {

						for(i2=0;i2<n2;i2++)
							trace[i2]=
								array[i2*n1+i1];

						iseek = sizeof(float)*n2*(i3+ 
							i1*n3+i4*n3*n1+
							i5*n4*n3*n1);
						efseek(outfp,iseek,0);
						efwrite(trace,sizeof(float),
							n2,outfp);
					}
				}
			}
		}
		free(trace);
		free(array);
		n1o = n2;
		n2o = n3;
		n3o = n1;
		d1o = d2;
		d2o = d3;
		d3o = d1;
		o1o = o2;
		o2o = o3;
		o3o = o1;
		if(orient==1) {
			orient = 4;
		} else if (orient==2) {
		        orient = 6;
		} else if (orient==3) {
		      	orient = 2;
		} else if (orient==4) {
		       	orient = 5;
		} else if (orient==5) {
			orient = 1;
		} else if (orient==6) {
			orient = 3;
		}
		fflush(outfp);
		toghdr(&gh,&scale,&dtype,&n1o,&n2o,&n3o,&n4,&n5,
               		&d1o,&d2o,&d3o,&d4,&d5,&o1o,&o2o,&o3o,&o4,&o5,
               		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);
		ierr = fputghdr(outfp,&gh);
		if(ierr!=0) warn(" fputghdr error \n");
	}

	efclose(outfp);
	efclose(infp);

	exit(0);
}
