#include "usu.h"
#include "usgrid.h"
#include "subc.h"
#include "par.h"

char *sdoc = 
"GRID2HORZ - 3D grid to horizon depth grid conversion			\n"
"\n"
"grid2horz <grid.file > horizon.grid [parameters]			\n" 
"\n"
"Required parameters:							\n"
"grid.file=         name of grid file 					\n"
"horizon.grid=      name of file of horizon depth grid 		\n"
"gridvalue=         grid value used to extract the depth of the horizon \n"
"Optional parameters:							\n"
"imode=1            =1 the depth of the horizon is given at the last \n"
"                   grid point where the grid value equals gridvalue \n"
"nullvalue=0        output value where no grid value equals gridvalue \n"
"tol=0.             tolerance \n"
"\n"
"NOTES:						 			\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/2/00   		\n"
;


int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

	usghed usgh;
	int ierr;
	float *grid, *horz;
	int i1, i2, i3;
	int n1, n2, n3;
	float o1, d1;
	float gridvalue;
	int imode;
	float tol=0, nullvalue=0.;
	float gmin, gmax;
	int ifirst;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* read in the grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err("non standard grid header input ");
	o1 = usgh.o1;
	d1 = usgh.d1;
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3;

	/* get input parameters */
	if (!getparfloat("nullvalue",&nullvalue)) nullvalue = 0.0;
	if (!getparfloat("tol",&tol)) tol = 0.0;
	if (!getparfloat("gridvalue",&gridvalue)) err(" gridvalue missing");
	if (!getparint("imode",&imode)) imode = 1;

	/* memory allocations */
        grid = (float*)malloc(n1*sizeof(float));
        horz = (float*)malloc(n2*n3*sizeof(float));

	ifirst = 0;
	fseek64(infp,0,0);

	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(grid,sizeof(float),n1,infp);
			if(imode==1) { 
				horz[i3*n2+i2] = nullvalue;
				for(i1=n1-1;i1>=0;i1--) {
					if(abs(gridvalue-grid[i1])<=tol) {
						horz[i3*n2+i2] = o1+d1*i1;
						if(ifirst==0) {
							gmin = horz[i3*n2+i2];
							gmax = horz[i3*n2+i2];
							ifirst = 1;
						} else {
							if(gmin>horz[i3*n2+i2])
							   gmin=horz[i3*n2+i2];
							if(gmax<horz[i3*n2+i2])
							   gmax=horz[i3*n2+i2];
						}
						break;
					}	
				}
			}
		}
	} 
	fwrite(horz,sizeof(float),n2*n3,outfp);

	usgh.o1 = usgh.o2; usgh.d1 = usgh.d2; usgh.n1 = usgh.n2;
	usgh.o2 = usgh.o3; usgh.d2 = usgh.d3; usgh.n2 = usgh.n3;
	usgh.n3 = 1; usgh.n4 = 1; usgh.n5 = 1;
	usgh.gmin = gmin; usgh.gmax = gmax;

	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err("non standard grid header output ");

	free(horz);
	free(grid);

	exit(0);
}
