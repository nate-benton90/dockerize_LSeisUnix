/* SUFRAC: $Revision: 1.5 $ ; $Date: 90/11/15 10:43:28 $	*/

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

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUFRAC -- take general (fractional) time derivative 		\n\
	    or integral of data.  Input is TIME DOMAIN data.	\n\
								\n\
sufrac power= [optional parameters] <indata >outdata 		\n\
								\n\
Required parameters:						\n\
	power=		exponent of (-i*omega)	 		\n\
			=0  ==> costly do-nothing		\n\
			>0  ==> differentiation			\n\
			<0  ==> integration			\n\
	if dt is not set in header, then dt is mandatory	\n\
								\n\
Optional parameters:						\n\
	sign=-1			sign in front of i * omega	\n\
	dt = (from header)	time sample rate (in seconds)	\n\
								\n\
The filter is applied in frequency domain.			\n\
								\n\
";
/**************** end self doc ***********************************/


/* Credits:
 *	CWP: Chris, Jack, Dave (pfas)
 *
 * Algorithm:
 *	g(t) = Re[INVFTT{ (-iw)^power FFT(f)}]
 */


#define	I		cmplx(0.0, 1.0)
#define	piby2		1.57079632679490
#define pi		3.14159265358979
#define twopi		6.28318530717959
#define LOOKFAC		2	/* Look ahead factor for npfao	  */
#define PFA_MAX		720720	/* Largest allowed nfft	          */

segy tr;

main(int argc, char **argv)
{
	float power;		/* power of i omega applied to data	*/
	float amp;		/* amplitude associated with the power	*/
	float arg;		/* argument of power 			*/
	complex exparg;		/* cexp(I arg)				*/
	register float *rt;	/* real trace				*/
	register complex *ct;	/* complex transformed trace		*/
	complex *filt;		/* complex power	 		*/
	int nt;			/* number of points on input trace	*/
	int ntsize;		/* nt in bytes				*/
	float dt;		/* sample spacing (secs) on input trace	*/
	float df;		/* frequency spacing (from dt)		*/
	float omega;		/* circular frequency			*/
	float sign;		/* sign in front of i*omega default -1	*/
	int nfft;		/* number of points in nfft		*/
	int nfby2;		/* nfft/2				*/
	int nfby2p1;		/* nfft/2 + 1				*/
	float onfft;		/* 1 / nfft				*/
	int nzeros;		/* number of padded zeroes in bytes	*/
	
	
	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Set parameters */
	if (!getparfloat("power", &power))	power =  0.0;
	if (!getparfloat("sign", &sign))	sign  = -1.0;


	/* Get info from first trace */
	if (!gettr(&tr))	err("can't get first trace");
	if (tr.trid && tr.trid != TREAL) {
		err("input is not seismic data, trid=%d", tr.trid);
	}

	if (!getparfloat("dt", &dt))	dt = (float)tr.dt/1000000.0;
	if (!dt)	err("dt field is zero and not getparred");
	nt = tr.ns;
	ntsize = nt * FSIZE;


	/* Set up for fft */
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))
		err("Padded nt=%d -- too big", nfft);

	nfby2 = nfft / 2;
	nfby2p1 = nfby2 + 1;
	onfft = 1.0 / (float) nfft;
	nzeros = (nfft - nt) * FSIZE;
	df = onfft / dt;


	/* Allocate fft arrays */
	rt   = ealloc1float(nfft);
	ct   = ealloc1complex(nfby2p1);
	filt = ealloc1complex(nfby2p1);


	/* Set up args for complex power evaluation */
	arg = sign * piby2 * power;
	exparg = cexp(crmul(I, arg));


	/* Evaluate complex power, put inverse fft scale in */
	{
		register int i;
		for (i = 0; i < nfft; ++i) {
			omega = twopi * i * df;
			amp = pow(omega, power) * onfft;
			filt[i] = crmul(exparg, amp);
		}
	}
		

	/* Loop over traces */
	do {
		/* Load trace into rt (zero-padded) */
		memcpy((char*)rt, (char*)tr.data, ntsize);
		bzero(rt + nt, nzeros);

		/* FFT */
		pfarc(1, nfft, rt, ct);


		/* Apply filter */
		{ register int i;
		for (i = 0; i < nfby2p1; ++i)  ct[i] = cmul(ct[i], filt[i]);
		}


		/* Invert */
		pfacr(-1, nfft, ct, rt);


		/* Load traces back in, recall filter had nfft factor */
		{ register int i;
		for (i = 0; i < nt; ++i)  tr.data[i] = rt[i];
		}


		puttr(&tr);

	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
