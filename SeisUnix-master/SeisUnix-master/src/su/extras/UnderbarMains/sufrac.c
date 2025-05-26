/* SUFRAC: $Revision: 1.11 $ ; $Date: 89/09/20 19:35:32 $	*/

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
SUFRAC -- take general (fractional) time derivative 		\n\
	    or integral of data.  Input is TIME DOMAIN data.	\n\
								\n\
sufrac power= [optional parameters] <indata >outdata 		\n\
								\n\
Required parameters:						\n\
	power =		exponent of (-i*omega)	 		\n\
			=0  ==> costly do-nothing		\n\
			>0  ==> differentiation			\n\
			<0  ==> integration			\n\
	if dt is not set in header, then dt is mandatory	\n\
								\n\
Optional parameters:						\n\
	sign = -1	sign in front of i * omega		\n\
	dt    = (from header)	time sample rate (in seconds)	\n\
								\n\
The filter is applied in frequency domain.			\n\
								\n\
";
/*****************************************************************/


/* Credits:
 *	CWP: Chris, Jack
 *
 * Algorithm:
 *	g(t) = Re[INVFTT{ (-iw)^power FFT(f)}]
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid=
	"   $Source: /src/su/src/RCS/sufrac.c,v $";
static string revid =
	"   $Revision: 1.11 $ ; $Date: 89/09/20 19:35:32 $";


#define	FACMAX		12		/* For FFTPACK	*/
#define	piby2		1.57079632679490
#define pi		3.14159265358979
#define twopi		6.28318530717959

segy tr;


main(argc, argv)
int argc; char **argv;
{
	float power;		/* power of i omega applied to data	*/
	float amp;		/* amplitude associated with the power	*/
	float arg;		/* argument of power (omega > 0)	*/
	float cosarg;		/* cos(arg) 				*/
	float sinarg;		/* sin(arg) 				*/
	float *re;		/* real part of complex power 		*/
	float *im;		/* imaginary part of complex power	*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt in bytes				*/
	float dt;		/* sample rate in secs on input trace	*/
	float df;		/* frequency spacing (from dt)		*/
	float omega;		/* circular frequency			*/
	float sign;		/* sign in front of i*omega default -1	*/
	int nfft;		/* number of points in nfft		*/
	int nfftsize;		/* nfft in bytes			*/
	int nfby2;		/* nfft/2				*/
	float onfft;		/* 1 / nfft				*/
	int nfac;		/* number of factors of nfft		*/
	int facs[FACMAX];	/* contains factors of nfft		*/
	int nzeros;		/* number of padded zeroes in bytes	*/
	register int i;		/* counter				*/
	float *hcos;		/* hold cosines for fft			*/
	float *hsin;		/* hold sines for fft			*/
	register float *xr;	/* real part of trace			*/
	register float *xi;	/* imaginary part of trace		*/
	register float xrold;	/* temp for real part			*/
	register float xiold;	/* temp for imaginary part 		*/
	float *wr;		/* work area for fft			*/
	float *wi;		/* work area for fft			*/

	
	
	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Set parameters */
	if (!fgetpar("power", &power))		power =  0.0;
	if (!fgetpar("sign", &sign))		sign  = -1.0;


	/* Get info from first trace */
	if (!gettr(&tr))	err("can't get first trace");
	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}

	if (!fgetpar("dt", &dt))	dt = tr.dt/1000000.0;
	if (!dt)	err("dt field is zero and not getparred");
	nt = tr.ns;
	ntsize = nt * FSIZE;


	/* Set up for fft */
	fftfac_(&nt, &nfft, &nfac, facs);
	if (nfft > SU_NFLTS) {
		err("Padded nt=%d -- too big", nfft);
	}
	nfby2 = nfft / 2;
	onfft = 1.0 / (float) nfft;
	nfftsize = nfft * FSIZE;
	nzeros = nfftsize - ntsize;
	df = onfft / dt;


	/* Allocate fft arrays */
	hcos = vec(nfft);	hsin = vec(nfft);
	xr   = vec(nfft);	xi   = vec(nfft);
	wr   = vec(nfft);	wi   = vec(nfft);


	/* Set up tables */
	ffttab_(&nfft, hcos, hsin);


	/* Set up arrays for real and imag part of multiplier */
	re = vec(nfft);	im = vec(nfft);


	/* Set up args for complex power evaluation */
	arg = sign * piby2 * power;
	cosarg = cos(arg);
	sinarg = sin(arg);


	/* Evaluate complex power, n/2 gets done twice for n even */
	re[0] = 0.0;	im[0] = 0.0;
	for (i = 1; i <= nfby2; ++i) {
		omega = twopi * i * df;
		amp = pow((double) omega, (double) power);
		re[i] = amp * cosarg;
		im[i] = amp * sinarg;
		re[nfft - i] =  re[i];
		im[nfft - i] = -im[i];	/* im[n/2] = 0 for n even */
	}
		

	/* Loop over traces */
	do {
		/* Load traces into xr, xi (zero-padded) */
		bcopy(tr.data, xr, ntsize);
		bzero(xr + nt, nzeros);
		bzero(xi, nfftsize);

		/* Fft */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Multiply by factor, conjugate and scale for inverse */
		for (i = 0; i < nfft; ++i) {
			xrold = xr[i];
			xiold = xi[i];
			xr[i] =  (xrold*re[i] - xiold*im[i]) * onfft;
			xi[i] = -(xrold*im[i] + xiold*re[i]) * onfft;
		}

		/* Invert */
		fft_(xr, xi, wr, wi, &nfft, facs, &nfac, hcos, hsin);

		/* Take real part */
		bcopy(xr, tr.data, ntsize);

		puttr(&tr);

	} while (gettr(&tr));


	return SUCCEED;
}
