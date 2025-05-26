/* extrac grid value along the well path  */

#include "velo.h"
#include "usgrid.h"
#include "subc.h"

char *sdoc = 
"grid2curve - compute grid values along a defined curve (z,x,y) 	\n"
"\n"
"grid2curve path= [parameters] <input >output			\n" 
"\n"
"Required parameters:						 	\n"
"input          name of the input 3D grid        \n" 
"path=          name of well path (md,z,x,y) file \n"
"output         name of the output (md,z,x,y,grid) ascii file   \n"
"\n"
"Optional parameters:						 	\n"
"npmax=10000    maximum number of rows in the well path file \n"
"Notes: \n"
"     number of rows in output equals number of rows in the well path file \n"
"Notes: \n"
" 1. md is the measured depth (distance along well path) \n"
" 2. z is the vertical (true) depth  \n"
" 3. the locations of 3d grid is determined by \n"
"    z: [o1, o1+(n1-1)*d1] \n"
"    x: [o2, o2+(n2-1)*d2] \n"
"    y: [o3, o3+(n3-1)*d3] \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/23/98   		\n"    
;

int main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout,*pathfp;
		char *path;
		float *l;
		double *mdp, *zp, *xp, *yp;
		float *gp;
		float *grid;
		int npmax;
		int np;
		int ir, i;
		int ix1, ix2, iy1, iy2, iz1, iz2;
		int ix, iy, iz;
		double res, g11, g12, g21, g22, g1, g2;
		double x, y, z;

		usghed usgh;
		int n1, n2, n3;
		float o1, o2, o3;
		float d1, d2, d3;
		int ierr;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparstring("path",&path)) err(" path file missing");
		if (!getparint("npmax",&npmax)) npmax = 10000;

		pathfp = efopen(path,"r");

		ierr = fgetusghdr(infp,&usgh);
		o1 = usgh.o1; d1 = usgh.d1; n1 = usgh.n1;
		o2 = usgh.o2; d2 = usgh.d2; n2 = usgh.n2;
		o3 = usgh.o3; d3 = usgh.d3; n3 = usgh.n3;

/* memory allocation */
		xp = (double*) malloc(npmax*sizeof(double));
		yp = (double*) malloc(npmax*sizeof(double));
		zp = (double*) malloc(npmax*sizeof(double));
		mdp = (double*) malloc(npmax*sizeof(double));
		gp = (float*) malloc(npmax*sizeof(float));
    	grid = (float*)malloc(n1*n2*n3*sizeof(float));
    	cbuf = (char*)malloc(134*sizeof(char));

		efseek(infp, 0, 0);
		efread(grid,sizeof(float),n1*n2*n3,infp);

    	for (ir=0;ir<npmax;ir++) {
       		if (feof(pathfp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		fgets(cbuf,134,pathfp);
			sscanf(cbuf,"%lf %lf %lf %lf\n",&mdp[ir],&zp[ir],&xp[ir],&yp[ir]);
			/*
		 	fprintf(stderr," path md=%lf z=%lf x=%lf y=%lf \n",
			mdp[ir],zp[ir],xp[ir],yp[ir]);
			*/
		}
		np = ir - 1;

		for (ir=0;ir<np;ir++) {
			z = (zp[ir] - o1)/d1;
			x = (xp[ir] - o2)/d2;
			y = (yp[ir] - o3)/d3;
			ix = x;
			iy = y;
			iz = z;
			ix1 = ix; if(ix1<0) ix1=0; if(ix1>n2-1) ix1=n2-1;
			ix2 = ix+1; if(ix2<0) ix2=0; if(ix2>n2-1) ix2=n2-1;
			iy1 = iy; if(iy1<0) iy1=0; if(iy1>n3-1) iy1=n3-1;
			iy2 = iy+1; if(iy2<0) iy2=0; if(iy2>n3-1) iy2=n3-1;
			iz1 = iz; if(iz1<0) iz1=0; if(iz1>n1-1) iz1=n1-1;
			iz2 = iz+1; if(iz2<0) iz2=0; if(iz2>n1-1) iz2=n1-1;
			res = z - iz;
			g11 = grid[iz1+ix1*n1+iy1*n1*n2]*(1.-res) + 
			      grid[iz2+ix1*n1+iy1*n1*n2]*res;
			g12 = grid[iz1+ix1*n1+iy2*n1*n2]*(1.-res) + 
			      grid[iz2+ix1*n1+iy2*n1*n2]*res;
			g21 = grid[iz1+ix2*n1+iy1*n1*n2]*(1.-res) + 
			      grid[iz2+ix2*n1+iy1*n1*n2]*res;
			g22 = grid[iz1+ix2*n1+iy2*n1*n2]*(1.-res) + 
			      grid[iz2+ix2*n1+iy2*n1*n2]*res;
			res = x - ix;
			g1 = g11*(1.-res) + g21*res;
			g2 = g12*(1.-res) + g22*res;
			res = y - iy;
			gp[ir] = g1*(1.-res) + g2*res;
		}

		for(ir=0;ir<np;ir++) {
			fprintf(outfp," %10.2f %10.2f %15.2f %15.2f %10.3f\n",
				mdp[ir],zp[ir],xp[ir],yp[ir],gp[ir]);
		}

     free(cbuf);
     free(mdp);
     free(zp);
     free(xp);
     free(yp);
     free(grid);
     free(gp);

     return (0);
}
