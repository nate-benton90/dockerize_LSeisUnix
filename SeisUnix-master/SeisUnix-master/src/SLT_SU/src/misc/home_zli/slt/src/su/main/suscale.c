#include "usgrid.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUSCALE - apply 4-D (time,offset,xline,line) scaling function to data	\n"
"\n"
"suscale <stdin >stdout sf4d= [optional parameters] \n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input data 			\n"
"stdout                     Name of output data 		\n"
"sf4d                       Name of the 4D (time,xline,line,offset) \n"
"                              scaling function stored in grid file format \n"
"Optional Parameters:\n"
"\n"
"Notes:	\n"
" 1. input data trace headers used to determine	trace position \n"
"         tracl --- crossline number (trace within line) \n" 
"         tracr --- line number							\n"
"         cdpt  --- offset number						\n"
"         delrt --- time of first sample				\n"
"		  dt    --- sampling interval					\n"
"		  ns    --- number of samples per trace 		\n"
" 2. input grid headers used to determine grid position	\n"
"         o1, d1, n1 --- start, increment and number of samples  \n" 
"         o2, d2, n2 --- start, increment and number of crosslines \n" 
"         o3, d3, n3 --- start, increment and number of lines \n" 
"         o4, d4, n4 --- start, increment and number of offset bin \n" 
"                                (offset index, not offset value)	\n"
"Author:	Zhiming Li		      		1-6-97		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

main(int argc, char **argv)
{
	FILE *gfp, *infp=stdin, *outfp=stdout;
	char *sf4d;
	int n1, n2, n3, n4, ierr;
	float o1,o2,o3,o4,d1,d2,d3,d4;
	float *grid, *scale,*so;
	float *tin, *tout, ot, dt;
	int i1, i11, i12, i21, i22;
	int ix, iy, ix1, ix2, iy1, iy2, io, it, nt;
	float wx1, wx2, wy1, wy2;
	float x, y, o; 
	int *indx;

	usghed usgh;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("sf4d",&sf4d)) err(" sf4d missing ");
	gfp = efopen(sf4d,"r");
	ierr = fgetusghdr(gfp, &usgh);
	if(ierr!=0) err(" error getting input grid header ");
	n1 = usgh.n1; o1 = usgh.o1; d1 = usgh.d1;
	n2 = usgh.n2; o2 = usgh.o2; d2 = usgh.d2;
	n3 = usgh.n3; o3 = usgh.o3; d3 = usgh.d3;
	n4 = usgh.n4; o4 = usgh.o4; d4 = usgh.d4;

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	ot = tr.delrt;
	it = tr.dt / 1000;
	dt = it;
	nt = tr.ns;

	
	/* get other optional parameters */

	/* read in grid */
	grid = (float*) emalloc(n1*n2*n3*n4*sizeof(float));
	scale = (float*) emalloc(n1*sizeof(float));
	efseek(gfp,0,0);
	efread(grid,sizeof(float),n1*n2*n3*n4,gfp);

	indx = (int*) emalloc(nt*sizeof(int));
	tin = (float*) emalloc(n1*sizeof(float));
	tout = (float*) emalloc(nt*sizeof(float));
	so = (float*) emalloc(nt*sizeof(float));

	for(i1=0;i1<n1;i1++) tin[i1] = o1 + i1*d1;
	for(it=0;it<nt;it++) tout[it] = ot + it*dt;

	bisear_(&n1,&nt,tin,tout,indx);

	/* loop over traces */
	do {
		x = tr.tracl;
		y = tr.tracr;
		o = tr.cdpt;
		x = (x-o2)/d2;
		ix = x;
		y = (y-o3)/d3;
		o = (o-o4)/d4;
		io = o;
		if(io<0) io = 0; if(io>n4-1) io = n4-1;
		if(ix<0) {
			ix1 = 0; ix2 = 0; wx1 = 0.5; wx2 = 0.5;
		} else if(ix>=n2-1) {
			ix1 = n2-1; ix2 = n2-1; wx1 = 0.5; wx2 = 0.5;
		} else {
			ix1 = ix; ix2 = ix + 1; wx2 = x - ix; wx1 = 1. - wx2;
		}
		if(iy<0) {
			iy1 = 0; iy2 = 0; wy1 = 0.5; wy2 = 0.5;
		} else if(ix>=n3-1) {
			iy1 = n3-1; iy2 = n3-1; wy1 = 0.5; wy2 = 0.5;
		} else {
			iy1 = iy; iy2 = iy + 1; wy2 = y - iy; wy1 = 1. - wy2;
		}

		i11 = (ix1+iy1*n2+io*n2*n3)*n1;
		i12 = (ix1+iy2*n2+io*n2*n3)*n1;
		i21 = (ix2+iy1*n2+io*n2*n3)*n1;
		i22 = (ix2+iy2*n2+io*n2*n3)*n1;

		for(i1=0;i1<n1;i1++)
			scale[i1] = (grid[i11+i1]*wx1 + grid[i21+i1]*wx2)*wy1 +
				(grid[i12+i1]*wx1 + grid[i22+i1]*wx2)*wy2;
		
		linin_(&n1,&nt,tin,tout,indx,scale,so);
		for(it=0;it<nt;it++) tr.data[it] *= so[it];

		fputtr(outfp,&tr);
	} while (fgettr(infp,&tr));

	free(grid);
	free(indx);
	free(so);
	free(scale);
	free(tin);
	free(tout);

	return EXIT_SUCCESS;
}
