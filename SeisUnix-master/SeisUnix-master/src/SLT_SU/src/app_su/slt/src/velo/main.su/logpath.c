/* compute (z,x,y,log) */

#include "velo.h"


char *sdoc = 
"logpath - compute path(md,z,x,y) of a well log and output (md,z,x,y,log) 	\n"
"\n"
"logpath path= [parameters] <input >output			\n" 
"\n"
"Required parameters:						 	\n"
"input          name of the input well log (md,log)            \n" 
"path=          name of well path (md,z,x,y) file \n"
"output         name of the output (md,z,x,y,log) file   \n"
"\n"
"Optional parameters:						 	\n"
"nrmax=10000    maximum number of rows in the input file \n"
"npmax=100      maximum number of rows in the well path file \n"
"Notes: \n"
"     number of rows in output equals number of rows in input \n"
"Notes: \n"
" 1. md is the measured depth (distance along well path) \n"
" 2. z is the vertical (true) depth  \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	5/27/98   		\n"    
;

void dlinin(int n, int nnew, float *x, float *xnew, int *indx, 
	double *y, double *ynew);

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout,*pathfp;
		char *path;
		float *l;
		float *mdp, *md;
		double *zp, *xp, *yp, *z, *x, *y;
		int nrmax, npmax;
		int nr, np;
		int ir, i;
		int *indx;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparint("nrmax",&nrmax)) nrmax = 10000;
		if (!getparint("npmax",&npmax)) npmax = 100;
		if (!getparstring("path",&path)) err(" path file missing");

		pathfp = efopen(path,"r");

/* memory allocation */
		xp = (double*) malloc(npmax*sizeof(double));
		yp = (double*) malloc(npmax*sizeof(double));
		zp = (double*) malloc(npmax*sizeof(double));
		x = (double*) malloc(nrmax*sizeof(double));
		y = (double*) malloc(nrmax*sizeof(double));
		z = (double*) malloc(nrmax*sizeof(double));
    	cbuf = (char*)malloc(134*sizeof(char));
		mdp = (float*) malloc(npmax*sizeof(float));
		md = (float*) malloc(nrmax*sizeof(float));
		l = (float*) malloc(nrmax*sizeof(float));
		indx = (int*) malloc(nrmax*sizeof(int));

    	for (ir=0;ir<npmax;ir++) {
       		if (feof(pathfp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		fgets(cbuf,134,pathfp);
			sscanf(cbuf,"%g %lf %lf %lf\n",&mdp[ir],&zp[ir],&xp[ir],&yp[ir]);
			/*
		 	fprintf(stderr," path md=%g z=%lf x=%lf y=%lf \n",
			mdp[ir],zp[ir],xp[ir],yp[ir]);
			*/
		}
		np = ir - 1;

    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%g %g \n",&md[ir],&l[ir]);
		}
		nr = ir - 1;

/*
		fprintf(stderr," %d rows of path file and  %d rows of input read \n",
				np, nr);
*/

		bisear_(&np,&nr,mdp,md,indx);
		dlinin(np,nr,mdp,md,indx,zp,z);
		dlinin(np,nr,mdp,md,indx,xp,x);
		dlinin(np,nr,mdp,md,indx,yp,y);

		for(ir=0;ir<nr;ir++) {
			fprintf(outfp," %10.2f %10.2f %15.2f %15.2f %10.3f\n",
				md[ir],z[ir],x[ir],y[ir],l[ir]);
		}

     free(cbuf);
     free(mdp);
     free(zp);
     free(xp);
     free(yp);
     free(indx);
     free(md);
     free(z);
     free(x);
     free(y);
     free(l);

     return (0);
}

void dlinin(int n, int nnew, float *x, float *xnew, int *indx, 
	double *y, double *ynew) {

	int i, ii;
	double resd;

	for (i=0;i<nnew;i++) {
		ii = indx[i] - 1;
		resd = xnew[i] - x[ii];
		if (ii<0 || xnew[i]<x[0]) {
			ynew[i] = y[0];
		} else if(ii>=n-1) {
			ynew[i] = y[n-1];
		} else {
			ynew[i] = y[ii] + resd*(y[ii+1]-y[ii])/(x[ii+1]-x[ii]); 
		}
	}

}
