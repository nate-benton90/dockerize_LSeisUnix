/* SUIMP3D: $Revision: 1.6 $ ; $Date: 90/11/15 10:43:33 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **************/
string sdoc = "\
							\n\
SUIMP3D - generate shot records for a point scatterer	\n\
							\n\
suimp3d [optional parameters] >stdout 			\n\
							\n\
Optional parameters					\n\
	nshot=1		number of shots			\n\
	nrec=1		number of receivers		\n\
	c=5000		speed				\n\
	dt=.004		sampling rate			\n\
	nt=256		number of samples		\n\
	x0=1000		point scatterer location	\n\
	z0=1000		point scatterer location	\n\
	sxmin=0		first shot location		\n\
	szmin=0		first shot location		\n\
	gxmin=0		first receiver location		\n\
	gzmin=0		first receiver location		\n\
	dsx=100		x-step in shot location		\n\
	dsz=0	 	z-step in shot location		\n\
	dgx=100		x-step in receiver location	\n\
	dgz=0		z-step in receiver location	\n\
							\n\
";
/**************** end self doc ***************************/

/* Credits:
 *	CWP: Norm, Jack
 *
 */


#define LOOKFAC	2	/* Look ahead factor for npfao	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */


segy tr;

main(int argc, char **argv)
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

	register float *rt;	/* real trace			*/
	register complex *ct;	/* complex transformed trace	*/
	int nfft;		/* size of fft 			*/
	int nfby2;		/* nfft/2			*/
	int nfby2p1;		/* nfft/2 + 1			*/
	int nzeros;		/* padded zeroes in bytes	*/
	float spread;		/* 3-D spreading factor		*/
	float isq;		/* temp for omega squared	*/

	register int i;		/* counter			*/
	register int s;		/* shot counter			*/
	register int g;		/* receiver counter		*/



	/* Initialize */
	initargs(argc, argv);
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
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);

	nfby2 = nfft / 2;
	nfby2p1 = nfby2 + 1;
	nzeros = (nfft - nt) * FSIZE;


	/* Allocate fft arrays */
	rt   = ealloc1float(nfft);
	ct   = ealloc1complex(nfby2p1);


	/* Set the constant in the response amplitude
	   including scale for inverse fft below      */
	k = (double) nfft / (4.0 * c * c * dt * dt * dt);


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
			spread = rs*rg;

			/* Distribute response over two surrounding samples */
			tr.data[jt] = k * ((jt + 1)*dt - t)/spread;
			tr.data[jt + 1] = k * (t - jt*dt)/spread;

			/* Load trace into rt (zero-padded) */
			memcpy((char*)rt, (char*)tr.data, ntsize);
			bzero(rt + nt, nzeros);

			/* FFT */
			pfarc(1, nfft, rt, ct);

			/* Multiply by omega^2 */
			for (i = 0; i < nfby2p1; ++i)
				ct[i] = crmul(ct[i], i*i);

			/* Invert and take real part */
			pfacr(-1, nfft, ct, rt);

			/* Load traces back in */
			memcpy((char*)tr.data, (char*)rt, ntsize);

			tr.gx = gx;
			tr.gy = gz;
			tr.tracl = s*nrec + g + 1;
			tr.offset = gx - sx;	/* for surface surveys */
			tr.cdp = 0.5*(gx + sx);	/* for surface surveys */

			puttr(&tr);
		} /* end loop on receivers */
	} /* end loop on shots */

	return EXIT_SUCCESS;
}
