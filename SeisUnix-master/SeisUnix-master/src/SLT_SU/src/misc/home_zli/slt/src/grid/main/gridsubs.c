/* GRIDWIND grid window program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDSUBS - grid substitute program				\n"
"\n"
"gridwind <grid.input subgrid= subval= >grid.output		\n" 
"\n"
"Required parameters:							\n"
"subgrid=           name of grid containing grid values equal to subval \n"
"subval=            value to be used to substitute input grid values 	\n"
"                   whose positions are the same as those in subgrid  	\n"   	
"                   with values of subval				\n" 
"op=0               when op=0 the place where subgrid=subval, the input \n"
"                   grid value will be set to subval 		\n"
"                   when op=1 the place where grid.input=subval, the input \n"
"                   grid value will be replaced by subgrid value \n"
"Optional parameters:							\n"
"tol=0.             tolerance used when checking grid values againt subval \n" 
"\n"
"AUTHOR:		Zhiming Li,       ,	11/12/93   \n"		    
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout, *subfp;
	char *subgrid; 

    	int n1,n2,n3,n4,n5;
    	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient,gtype, op;

    	int n1s,n2s,n3s,n4s,n5s;
    	float o1s,o2s,o3s,o4s,o5s,d1s,d2s,d3s,d4s,d5s;
	float scales; 
	int dtypes;
	float ocdp2s,dcdp2s,oline3s,dline3s,gmins,gmaxs;
	int orients,gtypes;
	
	int ierr, i1, i2, i3, i4, i5;
	float *trace, *traces, subval, tol, diff;

	ghed gh, ghs;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	if(!getparstring("subgrid",&subgrid)) 
		err(" subgrid missing ");
	if(!getparfloat("subval",&subval)) 
		err(" subval missing ");

	if(!getparint("op",&op)) op=0; 

	if(!getparfloat("tol",&tol)) tol = 0.;

	/* get input grid parameters */
    	ierr = fgetghdr(infp,&gh);
	if(ierr==0) {
		fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
               	 		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                		&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);
	} else {
		err(" input grid is nonstandard grid file"); 
	}

	subfp = efopen(subgrid,"r");
    	ierr = fgetghdr(subfp,&ghs);
	if(ierr==0) {
		fromghdr(&ghs,&scales,&dtypes,&n1s,&n2s,&n3s,&n4s,&n5s,
               	 		&d1s,&d2s,&d3s,&d4s,&d5s,
				&o1s,&o2s,&o3s,&o4s,&o5s,
                		&dcdp2s,&dline3s,&ocdp2s,&oline3s,&gmins,&gmaxs,
				&orients,&gtypes);
	} else {
		err(" subgrid is nonstandard grid file"); 
	}

	if(n1!=n1s || n2!=n2s || n3!=n3s) 
		err("grid sizes of input.grid and subgrid not the same ");	

	if(n5==0) n5=1;
	if(n4==0) n4=1;
	if(n3==0) n3=1;

	trace = (float*) malloc(n1*sizeof(float)); 
	traces = (float*) malloc(n1*sizeof(float)); 

	efseek(infp,0,0);
	efseek(subfp,0,0);

	for(i5=0;i5<n5;i5++) {
		for(i4=0;i4<n4;i4++) {
			for(i3=0;i3<n3;i3++) {
				for(i2=0;i2<n2;i2++) {

				efread(trace,sizeof(float),n1,infp);
				efread(traces,sizeof(float),n1,subfp);
				if(op==0) {
					for(i1=0;i1<n1;i1++) {
						diff = traces[i1] - subval;
						if(fabs(diff)<=tol)
							trace[i1] = subval;
					}
				} else {
					for(i1=0;i1<n1;i1++) {
						diff = trace[i1] - subval;
						if(fabs(diff)<=tol)
							trace[i1] = traces[i1];
					}
				}
				efwrite(trace,sizeof(float),n1,outfp);
				for(i1=0;i1<n1;i1++) {
					if(gmin>trace[i1]) gmin = trace[i1];
					if(gmax<trace[i1]) gmax = trace[i1];
				}
				}
			}
		}
	}

	free(trace);
	free(traces);
	fflush(outfp);

	toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
          	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
               	&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
		&orient,&gtype);

    	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) err(" error put grid header in output ");

	efclose(subfp);
	efclose(outfp);
	efclose(infp);

	exit(0);
}
