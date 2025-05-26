/* SUADDNOISE: $Revision: 2.16 $ ; $Date: 89/09/20 19:34:22 $		*/

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
#include "fconst.h"
#include "header.h"
#include <sys/timeb.h>

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUADDNOISE - add noise to traces					\n\
									\n\
suaddnoise <stdin >stdout  sn=3  noise=gauss  seed=from_clock		\n\
									\n\
Required parameters:							\n\
	none								\n\
									\n\
Optional parameters:							\n\
	sn = 3			signal to noise ratio			\n\
	noise = gauss		noise probability distribution		\n\
				= white for uniform; default Gaussian	\n\
	seed = from_clock	random number seed (integer)		\n\
									\n\
NOTES:									\n\
	Output = Input + sqrt(signal_power/noise_power) * Noise/sn	\n\
									\n\
	Here signal_power and noise_power are global to the data set.	\n\
									\n\
	Synthetics that are mostly zeroes need sn rather small.		\n\
									\n\
";
/*************************************************************************/

/* Credits:
 *	CWP: Jack, Brian
 *	For the Gaussian noise algorithm:
 *	Donald E. Knuth, "The Art of Computer Programming", Volume 2,
 *	Algorithm P, page 104.
 *
 * Notes:
 *	The Knuth algorithm involves a potentially infinite loop.  If
 *	the code is sound, this has zero probability.  If the code is
 *	to be revised, be aware that bugs could produce infinite run time.
 *
 *	Knuth gives better Gaussian algorithms, so look there if
 *	optimization ever becomes an issue.  We felt that for a program
 *	whose raison d'ete is creating synthetic data, the simpler the
 *	algorithm, the better.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suaddnoise.c,v $";
static string revid =
	"   $Revision: 2.16 $ ; $Date: 89/09/20 19:34:22 $";



/* Default signal to noise ratio */
#define SN		3

/* Noise probability distributions */
#define	GAUSS	0
#define	WHITE	1


segy tr;

main(argc, argv)
int argc; char **argv;
{
	register int i;		/* counter 				*/
	int nt;			/* number of points on trace		*/
	int ntsize;		/* number of data bytes on a trace	*/
	int ntr;		/* number of traces			*/
	int itr;		/* counter up to ntr 			*/
	string stype;		/* noise type (gauss, white) as string	*/
	int itype;		/* ... as integer (for use in switch)	*/
	float sn;		/* signal to noise ratio		*/
	int seed;		/* random number seed			*/
	float *dataptr;		/* mega-vector of data from the segys	*/
	float *noise;		/* noise vector				*/
	int ndata;		/* floats allocated for mega-vectors	*/
	float noiscale;		/* scale for noise			*/
	float noipow;		/* noise power				*/
	float sigpow;		/* signal power				*/
	float normrand;		/* scale random numbers to [0,2]	*/
	FILE *fphdr;		/* fp for header storage file		*/
      	register float r1, r2;	/* random numbers in [-1, 1] (gauss)	*/
	register float magsq;	/* r1*r1 + r2*r2 			*/
	register float factor;	/* multiplier in Gauss algorithm	*/
        register float r;	/* random number in [-1, 1]  (white)	*/
	long time();		/* system subroutine			*/
	long random();		/* system subroutine			*/
	int srandom();		/* system subroutine			*/
	FILE *tmpfile();	/* system subroutine			*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get noise type */
	if (!sgetpar("noise", &stype))	stype = "gauss";

	if      (STREQ(stype, "gauss")) itype = GAUSS;
	else if (STREQ(stype, "white")) itype = WHITE;
	else     err("noise=\"%s\", must be gauss or white", stype);


	/* Get signal to noise ratio */
	if (!fgetpar("sn", &sn))	sn = SN;
	if (sn <= 0) err("sn=%d must be positive", sn);


	/* Set seed */
	if (!igetpar("seed", &seed)) { /* if not supplied, use clock */
		if (-1 == (seed = (int) time((long *) 0))) {
			syserr("time() failed to set seed");
		}
	}
	srandom(seed);


	/* Factor to map returns from random() to [0.0, 2.0] */
	normrand = 2.0 / (pow(2.0, 31.0) - 1.0);


	/* Prepare temporary file to hold headers */
	fphdr = tmpfile();


	/* Get info from first trace and store first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	ntsize = nt * FSIZE;


	/* Allocate memory for data float mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	dataptr = vec(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr*nt > ndata) {	/* need more memory */
			ndata <<= 1;	/* ask for double   */
			dataptr = re_vec(dataptr, ndata);
		}
		bcopy(tr.data, dataptr + (ntr - 1)*nt, ntsize); 
		efwrite((char *)&tr, 1, HDRBYTES, fphdr);
	} while (gettr(&tr));
	rewind(fphdr);


	/* Allocate room for noise vector; allocate 1 extra in case    */
	/* ndata is odd since Knuth generates pairs of noise values    */
	noise = vec(ndata + 1);


	/* Compute signal power:  sigpow += dataptr[i]*dataptr[i] */
	svesq_(dataptr, ONE, &sigpow, &ndata);


	/* Compute noise vector elements in [-1,1] and noise power */
	noipow = 0.0;
	switch (itype) {
	case GAUSS:
		for (i = 0; i < ndata; i += 2) {

			do {
				r1    = normrand * random() - 1.0;
				r2    = normrand * random() - 1.0;
				magsq = r1 * r1 + r2 * r2;
			} while (magsq >= 1.0);

			factor     = sqrt(-2.0 * log(magsq)/magsq);
			noise[i]   = factor * r1;
			noise[i+1] = factor * r2;
			noipow    += factor * factor * magsq;
		}
	break;
	case WHITE:
		for (i = 0; i < ndata; i++) {
			r        = normrand * random() - 1.0;
			noise[i] = r;
			noipow  += r * r;
		}
	break;
	default:	/* defensive programming */
		err("%d: mysterious itype = %d", __LINE__, itype);
	}


	/* Compute noise scale for desired noise/signal ratio */
	noiscale = sqrt(sigpow/noipow) / sn;


	/* Add scaled noise to trace:  dataptr[i] += noiscale*noise[i] */
	vsma_(noise, ONE, &noiscale, dataptr, ONE, dataptr, ONE, &ndata);


	/* Output the result by pulling traces off data mega-vector  */
	for(itr = 0; itr < ntr; ++itr) {
		bcopy(dataptr + itr*nt, tr.data, ntsize); 
		efread((char *) &tr, 1, HDRBYTES, fphdr);
		tr.ntr = ntr;
		puttr(&tr);
	}


	return SUCCEED;
}
