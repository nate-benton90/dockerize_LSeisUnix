/* KAPERTURE: $Revision: 1.21 $ ; $Date: 89/09/20 19:36:38 $	*/

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

#include "par.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
KAPERTURE - generate the k domain for a line scatterer		\n\
								\n\
kaperture [optional parameters] >stdout 			\n\
								\n\
Optional parameters						\n\
	x0=1000		point scatterer location		\n\
	z0=1000		point scatterer location		\n\
	nshot=1		number of shots				\n\
	sxmin=0		first shot location			\n\
	szmin=0		first shot location			\n\
	dsx=100		x,z-steps in shot location		\n\
	dsz=0		x,z-steps in shot location		\n\
	ngeo=1		number of receivers			\n\
	gxmin=0		first receiver location			\n\
	gzmin=0		first receiver location			\n\
	dgx=100		x,z-steps in receiver location		\n\
	dgz=0		x,z-steps in receiver location		\n\
	fnyq=125	Nyquist frequency  (Hz)			\n\
	fmax=125	maximum frequency  (Hz)			\n\
	fmin=5		minimum frequency  (Hz)			\n\
	nfreq=2		number of frequencies   		\n\
	both=0		= 1 gives negative freqs too		\n\
	nsteps=60	points on Nyquist circle		\n\
	c=5000		speed					\n\
	outpar=/dev/tty	output parameter file, contains:	\n\
				xmin, xmax, ymin, ymax 		\n\
								\n\
Notes:								\n\
      nfreq=1 produces fmin					\n\
      nsteps=0 suppresses the Nyquist circle			\n\
								\n\
      The output file is created in Seb's plotting format:	\n\
      an integer representing the number of points on the	\n\
      curve followed by pairs of floats representing the curve.	\n\
      This is repeated nfreq times.  Each curve represents the	\n\
      ensemble of shots and receivers at a fixed frequency.	\n\
      The intent was to make data for the scatplot program.	\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack, Seb, Norm
 */


#define twopi		6.28318530717959
#define fourpi		12.5663706143592
#define EPS		1.0e-20
#define RATIO		1.2     /* ratio of (invisible) frame to circle */

/* Default parameter values */
#define NSTEPS		60
#define NFREQ		1
#define NSHOT		1
#define NGEO 		1
#define C    		5000.0
#define X0   		1000.0
#define Z0   		1000.0
#define SXMIN		0.0
#define SZMIN		0.0
#define GXMIN		0.0
#define GZMIN		0.0
#define DSX  		100.0
#define DSZ   		0.0
#define DGX   		100.0
#define DGZ   		0.0
#define FNYQ  		125.0
#define FMAX  		125.0
#define FMIN  		125.0


main(int argc, char **argv)
{
	float x0, z0;		/* point scatterer location	*/
	int nshot;		/* number of shots		*/
	float sxmin, szmin;	/* first shot location		*/
	float dsx, dsz;		/* x,z-steps in shot 		*/
	int ngeo;		/* number of receivers		*/
	float gxmin, gzmin;	/* first receiver location	*/
	float dgx, dgz;		/* x,z-steps in receiver 	*/
	float sx, sz;		/* scatterer - shot 		*/
	float gx, gz;		/* scatterer - receiver 	*/
	float rs;		/* distance scatterer to shot	*/
	float rg;		/* ... scatterer to receiver	*/
	float *x, *y;		/* kx, kz coordinates		*/
	float tmp[2];		/* temporary storage for kx, kz	*/
	int npaths;		/* nshot*ngeo			*/
	float fnyq;		/* Nyquist frequency 		*/
	float fmax, fmin;	/* maximum, minimum frequency	*/
	int nfreq;		/* number of frequencies	*/
	float df;		/* step in frequency 		*/
	float freq;		/* frequency			*/
	int both;		/* boolean for doing neg freqs	*/
	float c;		/* speed			*/
	float kscale;		/* scale factor per frequency	*/
	float knyqscale;	/* ... for Nyquist frequency	*/
	float phi;		/* angle for Nyquist circle	*/
	register int iphi;	/* ... and counter 		*/
	int nsteps;		/* ... and bound		*/
	int npoints;		/* number of kx-kz pairs	*/
	register int ipoint;	/* index for kx-kz pairs	*/
	float xmin, xmax;	/* x range for plotting		*/
	float ymin, ymax;	/* ... and z range		*/
	string outpar;		/* file holding output parfile	*/
	FILE *outparfp;		/* ... its file pointer		*/
	int s;			/* shot index			*/
	int g;			/* receiver index		*/
	register int f;		/* frequency counter		*/



	/* Initialize */
	initargs(argc, argv);
	askdoc(0);


	/* Get parameters */
	if (!igetpar("nfreq",  &nfreq))		nfreq = NFREQ;
	if (!igetpar("nshot",  &nshot))		nshot = NSHOT;
	if (!igetpar("ngeo",   &ngeo))		ngeo  = NGEO;
	if (!fgetpar("c",      &c))		c     = C;
	if (!fgetpar("x0",     &x0))		x0    = X0;
	if (!fgetpar("z0",     &z0))		z0    = Z0;
	if (!fgetpar("sxmin",  &sxmin))		sxmin = SXMIN;
	if (!fgetpar("szmin",  &szmin))		szmin = SZMIN;
	if (!fgetpar("gxmin",  &gxmin))		gxmin = GXMIN;
	if (!fgetpar("gzmin",  &gzmin))		gzmin = GZMIN;
	if (!fgetpar("dsx",    &dsx))		dsx   = DSX;
	if (!fgetpar("dsz",    &dsz))		dsz   = DSZ;
	if (!fgetpar("dgx",    &dgx))		dgx   = DGX;
	if (!fgetpar("dgz",    &dgz))		dgz   = DGZ;
	if (!fgetpar("fnyq",   &fnyq))		fnyq  = FNYQ;
	if (!fgetpar("fmax",   &fmax))		fmax  = FMAX;
	if (!fgetpar("fmin",   &fmin))		fmin  = FMIN;
	if (!igetpar("nsteps",  &nsteps))	nsteps = NSTEPS;
	if (!igetpar("both",  &both))		both   = 0;
	if (!sgetpar("outpar", &outpar))	outpar = "/dev/tty";


	/* Open file to save parameters */
	outparfp = efopen(outpar, "w");


	/* Allocate x, y arrays */
	npaths = nshot * ngeo;
	npoints = (both) ? 2 * npaths : npaths;
	x = ealloc1float(npoints);
	y = ealloc1float(npoints);


	/* Create the basic k-curve using ipoint = s*ngeo + g */
	for (ipoint = 0; ipoint < npaths; ++ipoint) {
		s = ipoint / ngeo;
		g = ipoint - s * ngeo;
		sx = x0 - (sxmin + s * dsx);
		sz = z0 - (szmin + s * dsz);
		gx = x0 - (gxmin + g * dgx);
		gz = z0 - (gzmin + g * dgz);
		rs = sqrt(sx * sx + sz * sz);
		if (!rs) rs = EPS; /* fudge to prevent divide by zero */
		rg = sqrt(gx * gx + gz * gz);
		if (!rg) rg = EPS; /* fudge to prevent divide by zero */

		/* Load values into x, y.  Reverse sign of kz to */
		/* agree with a positive downward z coordinate   */
		x[ipoint] = sx/rs + gx/rg;
		y[ipoint] = -(sz/rs + gz/rg);

		if (both) {  /* load negative values in back half */
			x[npaths + ipoint] = -x[ipoint];
			y[npaths + ipoint] = -y[ipoint];
		}
	}


	/* Scale and write in scatplot (x,y) format with count at head */
	df = (nfreq == 1) ? 0.0 : (fmax - fmin) / ((float) nfreq - 1.0);
	for (f = 0; f < nfreq; ++f) {
		freq = fmin + f * df;
		kscale = twopi * freq / c;
		efwrite(&npoints, ISIZE, 1, stdout);
		for (ipoint = 0; ipoint < npoints; ++ipoint) {
			tmp[0] = kscale * x[ipoint];
			tmp[1] = kscale * y[ipoint];
			efwrite(tmp, FSIZE, 2, stdout);
		}
	}


	/* Largest value of the magnitude of the gradient sum is two */
	knyqscale = fourpi * fnyq / c;
	ymin = xmin = -RATIO * knyqscale; 
	ymax = xmax =  RATIO * knyqscale; 


	/* Draw a circle with the Nyquist radius as a boundary */
	if (nsteps) {
		efwrite(&nsteps, ISIZE, 1, stdout);
		for (iphi = 0; iphi < nsteps; ++iphi) {
			phi = iphi * twopi/nsteps;
			tmp[0] = cos(phi) * knyqscale;
			tmp[1] = sin(phi) * knyqscale;
			efwrite(tmp, FSIZE, 2, stdout);
		}
	}
		

	/* Make par file */
	(void) fprintf(outparfp, "xmin=%f xmax=%f ymin=%f ymax=%f\n",
	                   xmin,   xmax,   ymin,   ymax);

	/* Clean up */
	free1float(x);
	free1float(y);


	return EXIT_SUCCESS;
}
