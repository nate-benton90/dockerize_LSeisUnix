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
"op=0               op=0 the place where subgrid=subval, the input \n"
"                   grid value will be set to subval; 		\n"
"                   op=1 the place where grid.input=subval, the input \n"
"                   grid value will be replaced by subgrid value; \n"
"                   op=-1 the input grid value in the subgrid=subval \n"
"                   zone will be linearly interpolated from the input values \n"
"                   at top and bottom of the subgrid=subval zone \n"
"                   op=-2 the input grid value in the subgrid=subval \n"
"                   zone will be copied from the input value at the top \n"
"                   of the subgrid=subval zone \n"
"                   op=-3 the input grid value in the subgrid=subval \n"
"                   zone will be copied from the input value at the bottom \n"
"                   of the subgrid=subval zone \n"
"                   op=-4 the input grid value in the subgrid=subval \n"
"                   zone will be replaced by gridreplace \n"
"gridreplace=       when op=-4, it must be specified \n"
"nullvalue=0        null grid value \n"
"nullreplace=0      replace the null grid value with the closest non-null \n"
"                   value in the trace or in the plane \n"
"                   0=no 1=yes	\n"
"Optional parameters:							\n"
"tol=0.             tolerance used when checking grid values againt subval \n" 
"Notes: \n"
"subgrid and grid.input must have the same dimensions \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	11/12/93   \n"		    
;

void nullrep(float *gridi, float *grido, float nullvalue, int n1, int n2);

int main(int argc, char **argv)
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
	float *gridi, *grido;
	float nullvalue;
	int nullreplace;
	int *subs, i10, i1n, j1;
	float gridreplace;

	ghed gh, ghs;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	if(!getparstring("subgrid",&subgrid)) 
		err(" subgrid missing ");
	if(!getparfloat("subval",&subval)) 
		err(" subval missing ");

	if(!getparint("op",&op)) op=0; 
	if(!getparint("nullreplace",&nullreplace)) nullreplace=0; 
	if(!getparfloat("nullvalue",&nullvalue)) nullvalue=0.; 
	if(op==-4) {
		if(!getparfloat("gridreplace",&gridreplace)) 
			err (" gridreplace missing ");
	}

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
	subs = (int*) malloc(n1*sizeof(int)); 
	gridi = (float*) malloc(n1*n2*sizeof(int)); 
	grido = (float*) malloc(n1*n2*sizeof(int)); 

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
				} else if(op==1) {
					for(i1=0;i1<n1;i1++) {
						diff = trace[i1] - subval;
						if(fabs(diff)<=tol)
							trace[i1] = traces[i1];
					}
				} else if(op<=-1) {
					for(i1=0;i1<n1;i1++) {
						diff = traces[i1] - subval;
						if(fabs(diff)<=tol) {
							subs[i1] = 1;
						} else {
							subs[i1] = 0;
						}
					}
					for(i1=0;i1<n1;i1++) {
						i1n = n1-1;
						i10 = 0;
						if(subs[i1]==1) {
							for(j1=i1+1;j1<n1;j1++) {
								if(subs[j1]==0) {
									i1n = j1;
									break;
								}
							}
							for(j1=i1-1;j1>=0;j1--) {
								if(subs[j1]==0) {
									i10 = j1;
									break;
								}
							}
							if(op==-1) {
								trace[i1] = trace[i10]+
											(i1-i10)*(trace[i1n]-trace[i10])
											/(i1n-i10);
							} else if(op==-2) {
								trace[i1] = trace[i10];
							} else if(op==-3) {
								trace[i1] = trace[i1n];
							} else if(op==-4) {
								trace[i1] = gridreplace;
							}
						}
					}
				}
				for(i1=0;i1<n1;i1++) gridi[i2*n1+i1] = trace[i1];
				}

				if(nullreplace==1) {
					nullrep(gridi,grido,nullvalue,n1,n2);
				} else {
					for(i1=0;i1<n1*n2;i1++) grido[i1] = gridi[i1];
				}

				if(i3==0&&i4==0&&i5==0) {gmin=grido[0];gmax=grido[0];}
				for(i1=0;i1<n1*n2;i1++) {
					if(gmin>grido[i1]) gmin = grido[i1];
					if(gmax<grido[i1]) gmax = grido[i1];
				}

				efwrite(grido,sizeof(float),n1*n2,outfp);
			}
		}
	}

	free(gridi);
	free(grido);
	free(trace);
	free(traces);
	free(subs);
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

void nullrep(float *gridi, float *grido, float nullvalue, int n1, int n2) {
	int i1, i2;
	int j1, i10, i1n;
	int j2, j20, j2n;
	int i20, i2n;
	int jj0, jjn;

	/* first search for the first dimension */
	for(i2=0;i2<n2;i2++) {
		for(i1=0;i1<n1;i1++) {
			i10 = 0;
			i1n = n1-1;
			jj0 = 0;
			jjn = 0;
			grido[i1+i2*n1] = gridi[i1+i2*n1];
			if(gridi[i2*n1+i1]==nullvalue) {
				for(j1=i1-1;j1>=0;j1--) {
					if(gridi[j1+i2*n1]!=nullvalue) {
						i10 = j1;
						jj0 = 1;
						break;
					}
				}
				for(j1=i1+1;j1<n1;j1++) {
					if(gridi[j1+i2*n1]!=nullvalue) {
						i1n = j1;
						jjn = 1;
						break;
					}
				}
				if(jj0==1 && jjn==0) {
					grido[i1+i2*n1] = gridi[i10+i2*n1];
				} else if(jj0==0 && jjn==1) {
					grido[i1+i2*n1] = gridi[i1n+i2*n1];
				} else {
					if( (i1-i10)<(i1n-i1) ) {
						grido[i1+i2*n1] = gridi[i10+i2*n1];
					} else {
						grido[i1+i2*n1] = gridi[i1n+i2*n1];
					}
				}
			}
		}
	}

	/* check along the second axis for the entire trace of nullvalue */
	for(i2=0;i2<n2;i2++) {
		if(grido[i2*n1] == nullvalue) {
			i20 = 0;
			i2n = n2 - 1;
			jj0 = 0;
			jjn = 0;
			for(j2=i2-1;j2>=0;j2--) {
				if(grido[j2*n1]!=nullvalue) {
					j20 = j2;
					jj0 = 1;
					break;
				}
			}
			for(j2=i2+1;j2<n2;j2++) {
				if(grido[j2*n1]!=nullvalue) {
					j2n = j2;
					jjn = 1;
					break;
				}
			}
			if(jj0==1 && jjn==0) {
				for(i1=0;i1<n1;i1++)
					grido[i2*n1+i1] = grido[j20*n1+i1];
			} else if(jj0==0 && jjn==1) {
				for(i1=0;i1<n1;i1++)
					grido[i2*n1+i1] = grido[j2n*n1+i1];
			} else {
				if((i2-j20)<(j2n-i2)) {
					for(i1=0;i1<n1;i1++)
						grido[i2*n1+i1] = grido[j20*n1+i1];
				} else {
					for(i1=0;i1<n1;i1++)
						grido[i2*n1+i1] = grido[j2n*n1+i1];
				}
			}
		}
	}
}
