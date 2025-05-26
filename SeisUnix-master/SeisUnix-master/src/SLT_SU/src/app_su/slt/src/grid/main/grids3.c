/* GRIDWIND grid window program */
#include "grid.h"
#include "gridhd.h"
#include "par.h"

char *sdoc = 
"GRIDS3 - grid substitute with 3 grid inputs				\n"
"\n"
"gridwind <grid.input defgrid= subgrid= defval= >grid.output		\n" 
"\n"
"Required parameters:							\n"
"defgrid=           name of grid containing grid values equal to defval \n"
"defval=            value to be used to define where to start substitute \n"
"                   input grid values with subgrid values 		\n"
"subgrid=           name of grid to be used to replace input grid values \n"
"                   below the position defined by defval 		\n"
"Optional parameters:							\n"
"tol=0.             tolerance used when checking grid values againt defval \n" 
"idef=0.            0=use the first occurance of defval in defgrid	\n"  
".                  1=use the last occurance of defval in defgrid	\n"  
"\n"
"AUTHOR:		Zhiming Li,       ,	1/20/95   \n"		    
;

int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout, *subfp, *deffp;
	char *subgrid, *defgrid; 

    	int n1,n2,n3,n4,n5;
    	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float scale; 
	int dtype;
	float ocdp2,dcdp2,oline3,dline3,gmin,gmax;
	int orient,gtype;

    	int n1s,n2s,n3s,n4s,n5s;
    	float o1s,o2s,o3s,o4s,o5s,d1s,d2s,d3s,d4s,d5s;
	float scales; 
	int dtypes;
	float ocdp2s,dcdp2s,oline3s,dline3s,gmins,gmaxs;
	int orients,gtypes;

    	int n1d,n2d,n3d,n4d,n5d;
    	float o1d,o2d,o3d,o4d,o5d,d1d,d2d,d3d,d4d,d5d;
	float scaled; 
	int dtyped;
	float ocdp2d,dcdp2d,oline3d,dline3d,gmind,gmaxd;
	int orientd,gtyped;
	
	int ierr, i0,i1, i2, i3, i4, i5, idef;
	float *trace, *traces, defval, tol, diff;
	float *traced; 

	ghed gh, ghs, ghd;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	if(!getparstring("subgrid",&subgrid)) 
		err(" subgrid missing ");

	if(!getparstring("defgrid",&defgrid)) 
		err(" defgrid missing ");
	if(!getparfloat("defval",&defval)) 
		err(" defval missing ");

	if(!getparint("idef",&idef)) idef = 0;

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

	deffp = efopen(defgrid,"r");
    	ierr = fgetghdr(deffp,&ghd);
	if(ierr==0) {
		fromghdr(&ghd,&scaled,&dtyped,&n1d,&n2d,&n3d,&n4d,&n5d,
               	 		&d1d,&d2d,&d3d,&d4d,&d5d,
				&o1d,&o2d,&o3d,&o4d,&o5d,
                		&dcdp2d,&dline3d,&ocdp2d,&oline3d,&gmind,&gmaxd,
				&orientd,&gtyped);
	} else {
		err(" defgrid is nonstandard grid file"); 
	}
	if(n1!=n1s || n2!=n2s || n3!=n3s || n1!=n1d || n2!=n2d | n3!=n3d)
	err("grid sizes of input.grid, subgrid and defgrid not the same ");	

	if(n5==0) n5=1;
	if(n4==0) n4=1;
	if(n3==0) n3=1;

	trace = (float*) malloc(n1*sizeof(float)); 
	traces = (float*) malloc(n1*sizeof(float)); 
	traced = (float*) malloc(n1*sizeof(float)); 

	efseek(infp,0,0);
	efseek(subfp,0,0);
	efseek(deffp,0,0);

	for(i5=0;i5<n5;i5++) {
		for(i4=0;i4<n4;i4++) {
			for(i3=0;i3<n3;i3++) {
				for(i2=0;i2<n2;i2++) {

				efread(trace,sizeof(float),n1,infp);
				efread(traces,sizeof(float),n1,subfp);
				efread(traced,sizeof(float),n1,deffp);

				i0 = n1;
				if(idef==0) {
					for(i1=0;i1<n1;i1++) {
						diff = traced[i1] - defval;
						if(fabs(diff)<=tol) {
							i0 = i1;
							break;
						}
					} 
				} else {
					for(i1=n1-1;i1>=0;i1--) {
						diff = traced[i1] - defval;
						if(fabs(diff)<=tol) {
							i0 = i1;
							break;
						}
					} 
				}

				for(i1=i0;i1<n1;i1++) {
					trace[i1] = traces[i1];
				}

				for(i1=0;i1<n1;i1++) {
					if(gmin>trace[i1]) gmin=trace[i1];
					if(gmax<trace[i1]) gmax=trace[i1];
				}
				efwrite(trace,sizeof(float),n1,outfp);
				}
			}
		}
	}

	free(trace);
	free(traces);
	free(traced);
	fflush(outfp);

	toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
          	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
               	&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
		&orient,&gtype);

    	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) err(" error put grid header in output ");

	efclose(deffp);
	efclose(subfp);
	efclose(outfp);
	efclose(infp);

	exit(0);
}
