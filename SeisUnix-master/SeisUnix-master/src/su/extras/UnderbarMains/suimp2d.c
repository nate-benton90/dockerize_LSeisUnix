/* SUIMP2D: $Revision: 1.17 $ ; $Date: 89/09/20 19:36:21 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUIMP2D - generate shot records for a line scatterer		\n\
								\n\
suimp2d [optional parameters] >stdout 				\n\
								\n\
Optional parameters						\n\
	nshot = 1		number of shots			\n\
	nrec = 1		number of receivers		\n\
	c = 5000		speed				\n\
	dt = .004		sampling rate			\n\
	nt = 256		number of samples		\n\
	x0 = 1000		point scatterer location	\n\
	z0 = 1000		point scatterer location	\n\
	sxmin = 0		first shot location		\n\
	szmin = 0		first shot location		\n\
	gxmin = 0		first receiver location		\n\
	gzmin = 0		first receiver location		\n\
	dsx = 100		x-step in shot location		\n\
	dsz = 0	 		z-step in shot location		\n\
	dgx = 100		x-step in receiver location	\n\
	dgz = 0			z-step in receiver location	\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Norm, Jack
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suimp2d.c,v $";
static string revid =
	"   $Revision: 1.17 $ ; $Date: 89/09/20 19:36:21 $";



#define	FACMAX		12		/* For FFTPACK	*/


segy tr;


main(argc, argv)
int argc; char **argv;
{
	float c;		/* speed			*/
	float dt;		/* sampling rate		*/
	int nt;			/* number of samples		*/
	int ntsize;		/* ... in bytes			*/
	int nshot;		/* number of shots		*/
	int nrec;		/* number of receivers		*/
	float x0, z0;		/* point scatterer location	*/
	float sxmin, szmin;	/* first shot location		*/
	float gxmin, gzmin;	/* first receiver location	*/
	float dsx;		/* x-step in shot location	*/
	float dsz;		/* z-step in shot location	*/
	float dgx;		/* x-step in receiver location	*/
	float dgz;		/* z-step in receiver location	*/

	float sx, sz;		/* shot location		*/
	float gx, gz;		/* receiver location		*/
	float rs;		/* distance to shot		*/
	float rg;		/* distance to receiver		*/
	float d;		/* rs + rg			*/
	float t;		/* total travel time		*/
	int jt;			/* (int) arrival time on trace	*/
	float k;		/* constant part of response	*/

	float *hcos;		/* hold cosines for fft		*/
	float *hsin;		/* hold sines for fft		*/
	register float *xr;	/* real part of trace		*/
	register float *xi;	/* imaginary part of trace	*/
	float *wr;		/* work area for fft		*/
	float *wi;		/* work area for fft		*/
	int nfft;		/* size of fft 			*/
	int nfftsize;		/* ... in bytes			*/
	int nfby2;		/* nfft/2			*/
	int nfac;		/* number of factors of nfft	*/
	int facs[FACMAX];	/* contains factors of nfft	*/
	int nzeros;		/* padded zeroes in bytes	*/
	float xrold;		/* temporary for xr[i]		*/
	float xiold;		/* temporary for xi[i]		*/
	float spread;		/* 2.5-D spreading factor	*/
	float i32;		/* temp for omega to the 3/2	*/

	register int i;		/* counter			*/
	register int s;		/* shot counter			*/
	register int g;		/* receiver counter		*/



	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(0);


	/* Get parameters */
	if (!igetpar("nshot", &nshot))		nshot = 1;
	if (!igetpar("nrec", &nrec))		nrec  = 1;
	if (!igetpar("nt", &nt))		nt    = 256;
	if (!fgetpar("c", &c))			c     = 5000.0;
	if (!fgetpar("dt", &dt))		dt    = 0.004;
	if (!fgetpar("x0", &x0))		x0    = 1000.0;
	if (!fgetpar("z0", &z0))		z0    = 1000.0;
	if (!fgetpar("sxmin", &sxmin))		sxmin = 0.0;
	if (!fgetpar("szmin", &szmin))		szmin = 0.0;
	if (!fgetpar("gxmin", &gxmin))		gxmin = 0.0;
	if (!fgetpar("gzmin", &gzmin))		gzmin = 0.0;
	if (!fgetpar("dsx", &dsx))		dsx   = 100.0;
	if (!fgetpar("dsz", &dsz))		dsz   = 0.0;
	if (!fgetpar("dgx", &dgx))		dgx   = 100.0;
	if (!fgetpar("dgz", &dgz))		dgz   = 0.0;


	/* Set the constant header fields */
	tr.ns = nt;
	tr.dt = dt * 1000000.0;
	ntsize = nt * FSIZE;


	/* Set up for fft */
	fftfac_(&nt, &nfft, &nfac, facs);
	if (nfft > SU_NFLTS)	err("Padded nt=%d -- too big", nfft);
	nfby2 = nfft / 2;
	nfftsize = nfft * FSIZE;
	nzeros = nfftsize - ntsize;


	/* Allocate fft arrays */
	hcos = vec(nfft);  hsin = vec(nfft);
	xr   = vec(nfft);  xi   = vec(nfft);
	wr   = vec(nfft);  wi   = vec(nfft);


	/* Set up tables */
	ffttab_(&nfft, hcos, hsin);


	/* Set the constant in the response amplitude
	   including scale for inverse fft below      */
	k = sqrt((double) nfft) / (4.0 * sqrt(2.0*c*dt) * c * dt * dt);


	/* Create the traces */
	for (s = 0; s < nshot; ++s) {	/* loop over shots */
		sx = sxmin + s * dsx;
		sz = szmin + s * dsz;
		tr.sx = sx;
		tr.sy = sz;
		rs = sqrt((sx - x0)*(sx - x0) + (sz - z0)*(sz - z0));

		for (g = 0; g < nrec; ++g) {	/* loop over receivers */
			bzero(tr.data, ntsize);
			gx = gxmin + g * dgx;
			gz = gzmin + g * dgz;
			rg = sqrt((gx - x0)*(gx - x0) + (gz - z0)*(gz - z0));
			d = rs + rg;
			t = d/c;
			jt = t/dt;
			spread = sqrt(rs*rg*d);

			/* Distribute response over two surrounding samples */
			tr.data[jt] = k * ((jt + 1)*dt - t) / spread;
			tr.data[jt + 1] = k * (t - jt*dt) / spread;

			/* Load traces into xr, xi (zero-padded) */
			bcopy(tr.data, xr, ntsize);
			bzero(xr + nt, nzeros);
			bzero(xi, nfftsize);

			/* Fft */
			fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

			/* Formula requires multiplication by
			   abs(omega) to 3/2 power times
			   exp(i pi / 4) sgn omega)
			   Take conjugate for inverse fft     */
			xi[0] = xr[0] = 0.0;
			for (i = 1; i <= nfby2; ++i) {
				xrold = xr[i];
				xiold = xi[i];
				i32 = i * sqrt((double) i);
				xr[i] =  (xrold - xiold) * i32;
				xi[i] = -(xrold + xiold) * i32;
				xr[nfft - i] =  xr[i];
				xi[nfft - i] = -xi[i];
			}

			/* Invert and take real part */
			fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);
			bcopy(xr, tr.data, ntsize);

			tr.gx = gx;
			tr.gy = gz;
			tr.tracl = s*nrec + g + 1;
			tr.offset = gx - sx;	/* for surface surveys */
			tr.cdp = 0.5*(gx + sx);	/* for surface surveys */

			puttr(&tr);
		} /* end loop on receivers */
	} /* end loop on shots */

	return SUCCEED;
}
