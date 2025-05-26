/* SUMODEL: $Revision: 1.6 $ ; $Date: 91/02/14 14:02:25 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUMODEL - convert a cshot model to su traces			\n\
								\n\
sumodel nintf= mname= dx= dz= [optional parameters] > out_file  \n\
								\n\
Creates an su data file of a cshot model for subsequent		\n\
processing and plotting.					\n\
								\n\
Required parameters:						\n\
	dx=		desired trace spacing			\n\
	dz=		desired depth sampling interval		\n\
	nintf=		number of interfaces in cshot model	\n\
	mname=		name of cshot model interface file	\n\
								\n\
Optional parameters:						\n\
	ns= (10 + max)	number of depth samples			\n\
			(defaults to 10 more than max depth)	\n\
	xlow= (left)	x coordinate of first trace		\n\
			(defaults to left edge of model)	\n\
	ntr= (cover)	number of traces			\n\
			(defaults to provide coverage of model)	\n\
	list=0		=1 to list model interface file		\n\
	lhalf=4		half length for sinc interpolator	\n\
                                                                \n\
Typical processing scheme:					\n\
	sumodel | suband | sutwig  ...				\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Craig
 *
 *
 */


segy tr;

#define MAXSPL 151
#define LHALF 4
#define YPEND 1.0e+30
	/* MAXSPL--maximum number of points allowed to define interfaces
	    LHALF--when an interface intercepts a trace at a depth which
		   is not an integer number of samples, the energy is
		   spread over LHALF samples on each side of the sample
		   nearest the actual depth.
		   =0 gives spikes at closest samples.
	    YPEND--value >= 1.0e+30 signals spline routine to apply
		   free end conditions, giving natural cubic spline	*/


main(int argc, char **argv)
{
	int nintf;	/* number of interfaces in cshot model		*/
	int *npts;	/* number of points defining each interface	*/
	int ns;		/* number of depth samples per trace		*/
	int ntr;	/* number of traces created			*/
	int itr;	/* trace counter				*/
	int iz;		/* sample nearest the interpolated depth z	*/
	int list;	/* flag for listing the model interface file	*/
	int lhalf;	/* half length of sinc interpolator		*/
	int i, j;	/* miscellaneous counters			*/

	float **xint;	/* x coordinates of points defining interfaces	*/
	float **zint;	/* z coordinates of points defining interfaces	*/
	float **z2;	/* second derivatives of splines		*/
	float xtemp;	/* temporary holder for reading model file	*/
	float ztemp;	/* temporary holder for reading model file	*/
	float dz;	/* depth sampling interval of traces		*/
	float dx;	/* horizontal trace spacing			*/
	float xlow;	/* x position of "left-most" trace		*/
	float xhigh;	/* x position of "right-most" trace		*/
	float xrange;	/* distance between end traces			*/
	float zmax;	/* maximum depth to last interface		*/
	float x;	/* horizontal location where depth needed	*/
	float z;	/* interpolated depth to interface at x		*/
	float fj;	/* sinc interpolated energy			*/

	FILE *mfile;	/* pointer to file containing model		*/

	string mname;	/* name of file containing model		*/

		/* functions specific to sumodel.c */
	float sinc(float x);	/* function to evaluate sinc(x)		*/
	void nrspline(float *x, float *y, int n,
		float yp1, float ypn,
		float *y2); /* tabulates second derivs of spline	*/
	void nrsplint(float *xa, float *ya, float *y2a,
		int n, float x,
		float *y);	 /* performs the actual interpolation	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(0); /* stdin not used */

        MUSTIGETPAR ("nintf",&nintf);
        MUSTSGETPAR ("mname",&mname);
        MUSTFGETPAR ("dx",&dx);
        MUSTFGETPAR ("dz",&dz);

	if (!igetpar("list", &list))	list = 0;
	if (!igetpar("lhalf", &lhalf))	lhalf = LHALF;

        mfile = efopen(mname,"r");	/* open model file for reading */

        npts = ealloc1int(nintf);	/* allocate storage */
        xint = ealloc2float(MAXSPL, nintf);
        zint = ealloc2float(MAXSPL, nintf);
        z2   = ealloc2float(MAXSPL, nintf);

        for (i=0; i<nintf; i++) {	/* read model file */
           for (j=0; j!=-1; j++) {
              fscanf(mfile,"%f %f", &xtemp, &ztemp);

              if (ztemp < 0.0) {	/* end of interface */
                 npts[i] = j;
                 j = -2;	/* set to -2, inc. to -1 at end of loop */
                 if (npts[i] < 2)
                    err ("Not enough points defining interface %d", i+1);
              }

              else if (j >= MAXSPL)
                 err ("Too many points defining interface %d", i+1);

	      else {			/* store the points */
		 xint[i][j] = xtemp;
		 zint[i][j] = ztemp;
	      }
           }
        }

	if (list)			/* list interface file */
	   for (i=0; i<nintf; i++) {
	      fprintf (stderr,"\ninterface %d,   npts=%d\n",i+1,npts[i]);
	      for (j=0; j<npts[i]; j++)
		 fprintf (stderr,"  xint=%8.2f,  zint=%8.2f\n",
					xint[i][j], zint[i][j]);
	   }

	if (!fgetpar("xlow", &xlow))	xlow = xint[0][0];

	if (!igetpar("ntr", &ntr)) {	/* calculate number of traces */
	   xhigh = xint[0][npts[0]-1];
	   xrange = ABS(xhigh-xlow);
	   ntr = xrange/dx + 1;
	}

	if (!igetpar("ns", &ns)) {	/* calculate number of samples */
	   zmax = 0.0;
	   for (j=0; j<npts[nintf-1]; j++)
	      if (zint[nintf-1][j] > zmax)
		 zmax = zint[nintf-1][j];
	   ns = zmax / dz + 11;
	}

	fprintf (stderr, "\nThese values will be used for the traces:\n");
	fprintf (stderr, "There are ns=%d samples, and ntr=%d traces.\n",
		 ns, ntr);
	fprintf (stderr, "The first trace is at xlow=%f  ", xlow);
	fprintf (stderr, "and the last trace is at %f\n\n", xlow+(ntr-1)*dx);

        /* Get spline coefficients using Numerical Recipes spline routine */

	for (i = 0; i < nintf; i++) {
	   if (npts[i] == 2)
	      z2[i][0] = z2[i][1] = 0.0;
	   else
	      nrspline (xint[i]-1,zint[i]-1,npts[i],YPEND,YPEND,z2[i]-1);
	}

        tr.ns = ns;		/* fill trace header keywords */
        tr.dt = dz;

	for (itr=0; itr<ntr; itr++) {
	   bzero(tr.data, ns*FSIZE);
	   x = xlow + itr*dx;
	   for (i=0; i<nintf; i++) {

			/* get depth to interface, z, at position x */
	      nrsplint (xint[i]-1, zint[i]-1, z2[i]-1, npts[i], x, &z);
	      iz = z/dz;  /* sample nearest actual depth */
	      if ((z-iz*dz)/dz > 0.5) iz++;

			/* spread sample over lhalf points on each side of iz */
	      for (j = (-lhalf); j <= lhalf; j++) {
		 if (iz+j >= 0 && iz+j < ns) {
		    fj = sinc (((iz+j)*dz-z) / dz);
		    tr.data[iz+j] += fj;
		 }
	      }
	   }
	   tr.tracl = itr+1;
	   puttr(&tr);
	}

	return EXIT_SUCCESS;
}

#define EPS 1.0e-5

float sinc(float x)     /* returns sinc(x)=sin(pi*x)/(pi*x) */
{
        float pix, result;
        pix = PI * x;
        if (ABS(pix) < EPS) pix=EPS;  /* stay away from zero */
        result = sin(pix) / pix;
        return (result);
}


		/* numerical recipes spline routine--get second derivs */
void nrspline(float *x, float *y, int n, float yp1, float ypn, float *y2)
{
	int i,k;
	float p,qn,sig,un,*u,*nrvec();
	void nrfree_vec();

	u=nrvec(1,n-1);
	if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0;
	else {
		y2[1] = -0.5;
		u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	for (i=2;i<=n-1;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0;
		y2[i]=(sig-1.0)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
		qn=un=0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	nrfree_vec(u,1,n-1);
}


		/* numerical recipes splint routine--evaluate spline */
void nrsplint(float *xa, float *ya, float *y2a, int n, float x, float *y)
{
	int klo,khi,k;
	float h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0.0) err("Bad XA input to routine NRSPLINT");
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}


	/* Numerical Recipes vector and free_vector routines--for arbitrary
	   offset vectors.  Cannot use SU's zero-offset vector routines
	   because spline routine expects these versions.		 */
float *nrvec(int nl, int nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;

	v=(float *)malloc((unsigned) (nh-nl+1)*sizeof(float))-nl;
	if (!v) err("allocation failure in nrvec()");
	return v;
}

void nrfree_vec(float *v, int nl, int nh)
/* free a float vector allocated with vec() */
{
	free((char*) (v+nl));
}
