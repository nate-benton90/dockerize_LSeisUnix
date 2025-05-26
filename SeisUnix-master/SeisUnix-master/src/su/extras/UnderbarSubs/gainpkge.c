/* GAINPKGE: $Revision: 1.23 $ ; $Date: 89/09/23 18:31:02 $	*/

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
#include "fconst.h"

/* gainpkge - apply various types of gain to data vectors
 *
 * gain		- calls other routines
 * dotpow	- multiply by t^tpow
 * doepow	- multiply by exp(epow*t)
 * dogpow	- dynamic compress data by power gpow
 * doagc 	- apply automatic gain control
 * dotrap	- zero outliers
 * doclip	- clip outliers
 * doqclip	- clip values above given quantile
 * doqbal	- quantile balance
 * dopbal	- power balance
 * doscale	- apply overall scale factor
 * quant	- get quantile from data vector
 *
 * Returns:
 *	quant: float
 *	All other functions in this package are void.
 *
 * Synopsis:
 *	void gain(dataptr, tpow, epow, gpow, agc, trap, clip, qclip,
 *				qbal, pbal, scale, tmin, dt, wagc, nt, ntr);
 *	float *dataptr, tpow, epow, gpow trap, clip, qclip, scale, tmin, dt
 *	int agc, qbal, pbal, wagc, nt, ntr
 *
 *	void dotpow(dataptr, tpow, tmin, dt, nt, ntr)
 *	float *dataptr, tpow, tmin, dt
 *	int nt, ntr
 *
 *	void doepow(dataptr, epow, tmin, dt, nt, ntr)
 *	float *dataptr, epow, tmin, dt
 *	int nt, ntr
 *
 *	void doagc(dataptr, wagc, nt, ntr)
 *	float *dataptr
 *	int wagc, nt, ntr
 *
 *	void dogpow(dataptr, gpow, nfloat)
 *	float *dataptr, gpow
 *	int nfloat
 *
 *	void dotrap(dataptr, trap, nfloat)
 *	float *dataptr, trap
 *	int nfloat
 *
 *	void doclip(dataptr, clip, nfloat)
 *	float *dataptr, clip
 *	int nfloat
 *
 *	void doqclip(dataptr, qclip, nfloat)
 *	float *dataptr, qclip
 *	int nfloat
 *
 *	void doqbal(dataptr, qclip, nfloat)
 *	float *dataptr, qclip
 *	int nfloat
 *
 *	void dopbal(dataptr, nfloat)
 *	float *dataptr
 *	int nfloat
 *
 *	void doscale(dataptr, scale, nfloat)
 *	float *dataptr, scale
 *	int nfloat
 *
 *	float quant(dataptr, quantile, nfloat)
 *	float *dataptr, quantile
 *	int nfloat
 *
 * Notes:
 *	This code is cloned from sugain and suagc.  See those codes for
 *	additional documentation.
 *
 * Operation order:
 * out(t) = scale * BAL{CLIP[AGC{[t^tpow * exp(epow * t) * in(t)]^gpow}]}
 *
 * Credits:
 *	SEP: Jon, Shuki
 *	CWP: Jack
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/gainpkge.c,v $";
static string revid =
	"   $Revision: 1.23 $ ; $Date: 89/09/23 18:31:02 $";



void gain(dataptr, tpow, epow, gpow, agc, trap, clip, qclip,
			qbal, pbal, scale, tmin, dt, wagc, nt, ntr)
float *dataptr;
float tpow, epow, gpow, trap, clip, qclip, scale, tmin, dt;
int agc, qbal, pbal, wagc, nt, ntr;
{
	int jon = 0;
	int nfloat;	/* total data count	*/

	void dotpow(), doepow(), dogpow(), doagc();
	void dotrap(), doclip(), doqclip(), doqbal(), dopbal(), doscale();


	/* Echo version on request */
	if (ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}


	nfloat = nt * ntr;

	/* Default parameters passed by sub, can override with getpar */
	fgetpar("tpow" , &tpow);
	fgetpar("epow" , &epow);
	fgetpar("gpow" , &gpow);
	fgetpar("trap" , &trap);
	fgetpar("clip" , &clip);
	fgetpar("qclip", &qclip);
	igetpar("wagc" , &wagc);
	igetpar("agc"  , &agc);
	igetpar("qbal" , &qbal);
	igetpar("pbal" , &pbal);
	fgetpar("scale", &scale);
	igetpar("jon"  , &jon);


	/* Data validation */
	if (trap < 0.0) err("trap = %f, must be positive", trap);
	if (clip < 0.0) err("clip = %f, must be positive", clip);
	if (qclip < 0.0 || qclip > 1.0) 
		err("qclip = %f, must be between 0 and 1", qclip);
	if (wagc <= 0) err("wagc = %d, must be positive", wagc);
	if (agc != 0 && agc != 1) err("agc = %d, flag must be 0 or 1", agc);
	if (jon != 0 && jon != 1) err("jon = %d, flag must be 0 or 1", jon);
	if (jon) { 
		tpow  = 2.0;
		gpow  = 0.5;
		qclip = 0.95;
	}


	/* Carry out gains, balances and scales */
	if (tpow != 0.0) {
		dotpow(dataptr, tpow, tmin, dt, nt, ntr);
	}
	if (epow != 0.0) {
		doepow(dataptr, epow, tmin, dt, nt, ntr);
	}
	if (agc) {
		doagc(dataptr, wagc, nt, ntr);
	}
	if (gpow != 1.0) {
		dogpow(dataptr, gpow, nfloat);
	}
	if (trap > 0.0) {
		dotrap(dataptr, trap, nfloat);
	}
	if (clip > 0.0) {
		doclip(dataptr, clip, nfloat);
	}
	if (qclip < 1.0 && !qbal) {
		doqclip(dataptr, qclip, nfloat);
	}
	if (qbal) {
		doqbal(dataptr, qclip, nfloat);
	}
	if (pbal) {
		dopbal(dataptr, nfloat);
	}
	if (scale != 1.0) {
		doscale(dataptr, scale, nfloat);
	}
	return;
}


/* Multiply by t^tpow = exp(tpow * log t) */
void dotpow(dataptr, tpow, tmin, dt, nt, ntr)
register float *dataptr;	/* pointer to data vector	*/
float tpow;			/* multiply data by t^tpow	*/
register float tmin;		/* first time on record		*/
register float dt;		/* sampling rate in seconds	*/
int nt;				/* number of samples		*/
int ntr;			/* number of traces		*/
{
	register float *timeptr;	/* ptr to time on trace	*/
	static float *tpowfac;		/* tpow values		*/
	register int i;			/* counter		*/

	tpowfac = vec(nt);

	/* First factor exceptional because the log argument is	*/
	/* zero in the common case where tmin = 0.0		*/
	tpowfac[0] = (tmin == 0.0 ? 1.0 : exp(tpow * log(tmin)));
	for (i = 1; i < nt; i++) {
		tpowfac[i] = exp(tpow * log(tmin + i * dt));
	}

	/* Tpow is done trace by trace */
	timeptr = dataptr;
	for (i = 0; i < ntr; i++) {
		vmul_(timeptr, ONE, tpowfac, ONE, timeptr, ONE, &nt);
		timeptr += nt;
	}
	return;
}


/* Exponential deattenuation  with deattenuation factor epow */
void doepow(dataptr, epow, tmin, dt, nt, ntr)
register float *dataptr;	/* pointer to data vector	*/
float epow;			/* coefficient of t in exponent	*/
register float tmin;		/* first time on record		*/
register float dt;		/* sampling rate in seconds	*/
int nt;				/* number of samples		*/
int ntr;			/* number of traces		*/
{
	register float *timeptr;	/* ptr to time on trace	*/
	static float *epowfac;		/* exponent stretches	*/
	register int i;			/* counter		*/

	epowfac = vec(nt);

	for (i = 0; i < nt; i++) {
		epowfac[i] = exp(epow * (tmin + i * dt));
	}

	/* Epow is done trace by trace */
	timeptr = dataptr;
	for (i = 0; i < ntr; i++) {
		vmul_(timeptr, ONE, epowfac, ONE, timeptr, ONE, &nt);
		timeptr += nt;
	}
	return;
}


/* Dynamic data compression */
void dogpow(dataptr, gpow, nfloat)
register float *dataptr;	/* pointer to data vector	*/
float gpow;			/* compression power 		*/
int nfloat;			/* total data count		*/
{
	if (gpow == 0.5) {
		vssqrt_(dataptr, ONE, dataptr, ONE, &nfloat);
	} else if (gpow == 2.0) {
		vssq_(dataptr, ONE, dataptr, ONE, &nfloat);
	} else {
		vspow_(dataptr, ONE, &gpow, dataptr, ONE, &nfloat);
	}
	return;
}


/* Zero out outliers */
void dotrap(dataptr,trap, nfloat)
register float *dataptr;	/* pointer to data vector	*/
register float trap;		/* zero if magnitude > trap	*/
int nfloat;			/* total data count		*/
{

	while (nfloat--) {
		if (ABS(*dataptr) > trap) *dataptr = 0.0;
		dataptr++;
	}
}


/* Hard clip outliers */
void doclip(dataptr, clip, nfloat)
register float *dataptr;	/* pointer to data vector	*/
register float clip;		/* hard clip if magnitude > clip	*/
int nfloat;			/* total data count		*/
{
	register float mclip = -clip;

	while (nfloat--) {
		if (*dataptr > clip) {
			*dataptr = clip;
		} else if (*dataptr < mclip) {
			*dataptr = mclip;
		}
		dataptr++;
	}
}


/* Quantile clip on magnitudes of trace values */
void doqclip(dataptr, qclip, nfloat)
register float *dataptr;	/* pointer to data vector	*/
float qclip;			/* quantile at which to clip	*/
int nfloat;			/* total data count		*/
{
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float clip;			/* ... value of rank[iq]	*/
	float quant();

	if (first) {
 		absdata = vec(nfloat);
 		iq = (int) (qclip * nfloat - 0.5); /* round */
		first = false;
	}

	/* Clip on value corresponding to qth quantile */
	vabs_(dataptr, ONE, absdata, ONE, &nfloat); 
 	clip = quant(absdata, iq, nfloat);
	doclip(dataptr, clip, nfloat);
}


/* Quantile balance */
void doqbal(dataptr, qclip, nfloat)
register float *dataptr;	/* pointer to data vector	*/
float qclip;			/* quantile at which to clip	*/
int nfloat;			/* total data count		*/
{
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float bal;			/* value used to balance trace	*/
	int balloc;			/* location of bal (not used)	*/

	if (qclip == 1.0) { /* balance by max magnitude on trace */
		maxmgv_(dataptr, ONE, &bal, &balloc, &nfloat);
		if (bal == 0.0) {
			return;
		} else {
			vsdiv_(dataptr, ONE, &bal, dataptr, ONE, &nfloat);
			return;
		}
	} else if (first) {
 		absdata = vec(nfloat);
 		iq = (int) (qclip * nfloat - 0.5); /* round */
		first = false;
	}

	/* Balance by quantile value (qclip < 1.0) */
	vabs_(dataptr, ONE, absdata, ONE, &nfloat); 
	bal = quant(absdata, iq, nfloat);
	if (bal == 0.0) {
		return;
	} else {
		vsdiv_(dataptr, ONE, &bal, dataptr, ONE, &nfloat);
		doclip(dataptr, 1.0, nfloat);
		return;
	}
}



/* Power balance */
void dopbal(dataptr, nfloat)
register float *dataptr;	/* pointer to data vector	*/
int nfloat;			/* total data count		*/
{
	float rmsq;

	rmvesq_(dataptr, ONE, &rmsq, &nfloat);
	if (rmsq == 0.0) return;
	vsdiv_(dataptr, ONE, &rmsq, dataptr, ONE, &nfloat);
	return;
}


/* Multiply by overall scale */
void doscale(dataptr, scale, nfloat)
register float *dataptr;	/* pointer to data vector	*/
float scale;			/* scale factor			*/
int nfloat;			/* total data count		*/
{
	vsmul_(dataptr, ONE, &scale, dataptr, ONE, &nfloat);
	return;
}


/* Automatic Gain Control */
#define EPS	3.8090232	/* exp(-EPS*EPS) = 5e-7, "noise" level	*/

void doagc(dataptr, wagc, nt, ntr)	
register float *dataptr;	/* pointer to data vector	*/
int wagc;			/* agc window in samples	*/
int nt;				/* number of samples		*/
int ntr;			/* number of traces		*/
{
	float u;		/* related to reciprocal of std dev	*/
	float usq;		/* u*u					*/
	register float wtmp;	/* storage for w[i]			*/
	float *w;		/* Gaussian window weights		*/
	register float *outdataptr;	/* agc'd data			*/
	register float *d2;	/* square of input data			*/
	register float stmp;	/* storage for s[i]			*/
	register float *s;	/* weighted sum of squares of the data	*/
	register int i;		/* counter 				*/
	float floati;		/* float(i)				*/
	register int j;		/* counter				*/
	register int k;		/* counter				*/
	float *timeptr;		/* pointer to time on trace		*/
	int itr;		/* trace counter			*/


	/* Gaussian is symmetric, so work with half */
	++wagc;
	wagc >>= 1;

	/* Compute Gaussian window weights */
	w = vec(wagc);
	u = EPS / ((float) wagc);
	usq = u*u;
	for (i = 0; i < wagc; i++) {
		floati = (float) i;
		w[i] = exp(-(usq*floati*floati));
	}

	/* Make room for sum of squares and weighted sum of squares */
	d2 = vec(nt);
	s = vec(nt);

	/* Make room for output data */
	outdataptr = vec(nt * ntr);

	/* Loop over traces */
	for (itr = 0; itr < ntr; itr++) {
		timeptr = dataptr;
		/* Put sum of squares of data in d2 */
		vsq_(timeptr, ONE, d2, ONE, &nt);

		/* Initialize s to d2 to get center point set */
		vmov_(d2, ONE, s, ONE, &nt);

		/* Compute weighted sum s; use symmetry of Gaussian */
		for (j = 1; j < wagc; j++) {
				wtmp = w[j];
				for (i = j; i < nt; i++)
					s[i] += wtmp*d2[i-j]; 
				k = nt - j;
				for (i = 0; i < k; i++)
					s[i] += wtmp*d2[i+j]; 
		}

		for (i = 0; i < nt; i++) {
			stmp = s[i];
			outdataptr[i + itr*nt] =
			   (stmp == 0.0 ? 0.0 : timeptr[i]/sqrt(stmp));
		}
		timeptr += nt;
	}
	bcopy(outdataptr, dataptr, nt * ntr * FSIZE); 
	return;
}


/*
 * QUANT - find k/n th quantile of a[]
 *
 * Works by reordering a so a[j] < a[k] if j < k.
 *
 * Parameters:
 *    a		- data
 *    k		- indicates quantile
 *    n		- number of points in data
 *
 * This is Hoare's algorithm worked over by SEP (#10, p100) and Brian.
 */

float quant(a, k, n)
float *a;
int k, n;
{
	register int i, j;
	int low, hi;
	register float ak, aa;

	low = 0; hi = n-1;

	while (low < hi) {
		ak = a[k];
		i = low;
		j = hi;
		do {
			while (a[i] < ak) i++;
			while (a[j] > ak) j--;
			if (i <= j) {
				aa = a[i]; a[i] = a[j]; a[j] = aa;
				i++;
				j--;
			}
		} while (i <= j);

		if (j < k) low = i;

		if (k < i) hi = j;
	}

	return(a[k]);
}
