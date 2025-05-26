#include "usgrid.h"
#include "su.h"

char *sdoc = 
"SUGRD2HDR - Convert a grid value to a trace header value \n"
"\n"
"sugrd2hdr [parameters] <input-data >output-data 		\n" 
"\n"
"Required parameters:							\n"
"key1=           trace header key word (segy) to match 1st grid dimension \n"
"key2=           trace header key word (segy) to match 2nd grid dimension \n"
"key3=           trace header key word (segy) to match 3rd grid dimension \n"
"key=            trace header key word (segy) to store grid value	\n"
"grid=           name of the grid to be converted into trace header value \n"	
"Optional parameters:							\n"
"o1=             starting position of 1st grid dimension (same unit as key1)\n"
"d1=             increment of 1st grid dimension (same unit as key1) 	\n"
"n1=             number of samples of 1st grid dimension 	\n"
"o2=             starting position of 2nd grid dimension (same unit as key2)\n"
"d2=             increment of 2nd grid dimension (same unit as key2) 	\n"
"n2=             number of samples of 2nd grid dimension 	\n"
"o3=             starting position of 3rd grid dimension (same unit as key3)\n"
"d3=             increment of 3rd grid dimension (same unit as key3) 	\n"
"n3=             number of samples of 3rd grid dimension 	\n"
"Note ---------- the above 9 parameters will be default to grid header values\n"
"outval=-999999  value to be used outside the grid			\n"
"                  =-999999 grid value at the grid edge will be used	\n"
"                  otherwise, outval used outside the grid	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	10/3/96   \n"
;

void get3dval(float *grid, float o1, float o2, float o3, 
	float d1, float d2, float d3, int n1, int n2, int n3,
	int outval, int i1, int i2, int i3, int *ival);

void changeval(String type, Value *val, int f);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout, *gridfp;
	String key1, type1, key2, type2, key3, type3, key, type;
	char *gridname;
	Value val1, val2, val3, val;
	int indx1, indx2, indx3, indx;

	int outval;

	float *grid, o1, o2, o3, d1, d2, d3;
	int n1, n2, n3; 
	int ival;
	int i1, i2, i3;

	usghed usgh; 

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

    	if(!getparstring("key1",&key1)) err(" key1 missing"); 
    	if(!getparstring("key2",&key2)) err(" key2 missing"); 
    	if(!getparstring("key3",&key3)) err(" key3 missing"); 
    	if(!getparstring("key",&key)) err(" key missing"); 
    	if(!getparstring("grid",&gridname)) err(" grid missing"); 
    	if (!getparint("outval",&outval)) outval = -999999;

	gridfp = efopen(gridname,"r");
	if(fgetusghdr(gridfp,&usgh)!=0) err(" grid header error ");
	if(!getparfloat("o1",&o1)) o1 = usgh.o1;
	if(!getparfloat("o2",&o2)) o2 = usgh.o2;
	if(!getparfloat("o3",&o3)) o3 = usgh.o3;
	if(!getparfloat("d1",&d1)) d1 = usgh.d1;
	if(!getparfloat("d2",&d2)) d2 = usgh.d2;
	if(!getparfloat("d3",&d3)) d3 = usgh.d3;
	if(!getparint("n1",&n1)) n1 = usgh.n1;
	if(!getparint("n2",&n2)) n2 = usgh.n2;
	if(!getparint("n3",&n3)) n3 = usgh.n3;
	if(n3<1) n3=1;

	/* make file size to be able to exceed 2 G */
	file2g(infp);
	file2g(outfp);

	/* read in first trace */
        if (!fgettr(infp,&tr))  err("can't get first trace");

	type1  = hdtype(key1);
       	indx1 = getindex(key1);
	type2  = hdtype(key2);
       	indx2 = getindex(key2);
	type3  = hdtype(key3);
       	indx3 = getindex(key3);
	type  = hdtype(key);
       	indx = getindex(key);

	/* memory allocations */
	grid = (float*) emalloc(n1*n2*n3*sizeof(float));
	efseek(gridfp,0,0);
	efread(grid,sizeof(float),n1*n2*n3,gridfp);

	/* loop over input traces */
	do {
		gethval(&tr, indx1, &val1);
		i1 = vtoi(type1,val1);
		gethval(&tr, indx2, &val2);
		i2 = vtoi(type2,val2);
		gethval(&tr, indx3, &val3);
		i3 = vtoi(type2,val3);
	
		get3dval(grid,o1,o2,o3,d1,d2,d3,n1,n2,n3,outval,
				i1,i2,i3,&ival);

		changeval(type,&val,ival);
		puthval(&tr,indx,&val);
		fputtr(outfp,&tr);
			
	} while(fgettr(infp,&tr)); 

	free(grid);
	return 0;

}

void get3dval(float *grid, float o1, float o2, float o3, 
	float d1, float d2, float d3, int n1, int n2, int n3,
	int outval, int i1, int i2, int i3, int *ival) {

	float f1, f2, f3;
	float r1, r2, r3;
	float tmp1, tmp2, tmp;
	float xx1, xx2, yy1, yy2, zz1, zz2;

	int ii1, ii2, ii3; 
	int jj1, jj2, jj3; 
	

	f1 = (i1-o1)/d1;
	f2 = (i2-o2)/d2;
	f3 = (i3-o3)/d3;

	ii1 = f1;
	ii2 = f2;
	ii3 = f3;

	r1 = f1 - ii1;
	r2 = f2 - ii2;
	r3 = f3 - ii3;

 
	if( (ii1<0 || ii1>=n1 || ii2<0 || ii2>=n2 || ii3<0 || ii3>=n3)
	   && outval!=-999999 ) {
		*ival = outval;
	} else {
		if(ii1<0) {
			ii1 = 0; jj1 = 0; xx1 = 0.5; xx2 = 0.5;
		} else if(ii1>=n1-1) {
			ii1 = n1 - 1; jj1 = n1 - 1; xx1 = 0.5; xx2 = 0.5;
		} else {
			jj1 = ii1 + 1; xx1 = 1. - r1; xx2 = r1; 
		}
		if(ii2<0) {
			ii2 = 0; jj2 = 0; yy1 = 0.5; yy2 = 0.5;
		} else if(ii2>=n2-1) {
			ii2 = n2 - 1; jj2 = n2 - 1; yy1 = 0.5; yy2 = 0.5;
		} else {
			jj2 = ii2 + 1; yy1 = 1. - r2; yy2 = r1; 
		}
		if(ii3<0) {
			ii3 = 0; jj3 = 0; zz1 = 0.5; zz2 = 0.5;
		} else if(ii3>=n3-1) {
			ii3 = n3 - 1; jj3 = n3 - 1; zz1 = 0.5; zz2 = 0.5;
		} else {
			jj3 = ii3 + 1; zz1 = 1. - r3; zz2 = r3; 
		}

		tmp1 = ( grid[ii1+(ii2+ii3*n2)*n1]*yy1
		       + grid[ii1+(jj2+ii3*n2)*n1]*yy2 )*zz1 + 		
		       ( grid[ii1+(ii2+jj3*n2)*n1]*yy1
		       + grid[ii1+(jj2+jj3*n2)*n1]*yy2 )*zz2;
		tmp2 = ( grid[jj1+(ii2+ii3*n2)*n1]*yy1
		       + grid[jj1+(jj2+ii3*n2)*n1]*yy2 )*zz1 + 		
		       ( grid[jj1+(ii2+jj3*n2)*n1]*yy1
		       + grid[jj1+(jj2+jj3*n2)*n1]*yy2 )*zz2;
		tmp = tmp1*xx1 + tmp2*xx2;

/*
	fprintf(stderr,"tmp1=%g tmp2=%g \n",tmp1,tmp2);
	fprintf(stderr,"g1=%g g2=%g \n",
		grid[ii1+(ii2+ii3*n2)*n1],grid[jj1+(ii2+ii3*n2)*n1]);
	fprintf(stderr,"ii1=%d jj1=%d ii2=%d jj2=%d \n",ii1,jj1,ii2,jj2);
	fprintf(stderr,"ii3=%d jj3=%d tmp=%g \n",ii3,jj3,tmp);
*/


		*ival = tmp;
	}	
}

void changeval(String type, Value *val, int f) {

	switch (*type) {
        case 's':
                err("can't change char header word");
        break;
        case 'h':
                val->h = f;
        break;
        case 'u':
                val->u = f;
        break;
        case 'l':
                val->l = f;
        break;
        case 'v':
                val->v = f;
        break;
        case 'i':
                val->i = f;
        break;
        case 'p':
                val->p = f;
        break;
        case 'f':
                val->f = f;
        break;
        case 'd':
                val->d = f;
        break;
        default:
                err("unknown type %s", type);
        break;
        }
}
