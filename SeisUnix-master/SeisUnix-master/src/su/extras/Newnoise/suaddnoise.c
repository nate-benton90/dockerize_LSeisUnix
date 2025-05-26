#include "../include/cwp.h"
#include "../include/segy.h"
#include "../include/fconst.h"
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
Notes:									\n\
	Output = Input + sqrt(signal_power/noise_power) * Noise/sn	\n\
									\n\
	The Noise vector is tapered as 1/t.				\n\
	A zero trace is left as a zero trace since scaling is		\n\
	is proportional to the square root of the signal power.		\n\
									\n\
";
/*************************************************************************/

/* suaddnoise - add noise to traces
 *
 * Credits:
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
 *----------------------------------------------------------------------
 * Permission to use the SU programs is granted.  Courteous users will
 * retain this acknowlegment of origins at the Stanford Exploration
 * Project, Stanford University and the Center for Wave Phenomena, 
 * Colorado School of Mines.   Inquiries/bug reports should be sent to:
 * Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 * Golden, CO 80014
 *----------------------------------------------------------------------
 *
 * $Author: jkc $
 * $Source: /src/su/src/RCS/suaddnoise.c,v $
 * $Revision: 2.5 $ ; $Date: 88/11/26 15:56:46 $
*/

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suaddnoise.c,v $";
static string revid =
	"   $Revision: 2.5 $ ; $Date: 88/11/26 15:56:46 $";



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
	float dt;		/* sampling rate on trace		*/
	string stype;		/* noise type (gauss, white) as string	*/
	int itype;		/* ... as integer (for use in switch)	*/
	float sn;		/* signal to noise ratio		*/
	int seed;		/* random number seed			*/
	register float *noise;	/* noise vector				*/
	float noiscale;		/* scale for noise			*/
	float noipow;		/* noise power				*/
	float sigpow;		/* signal power				*/
	float normrand;		/* scale random numbers to [0,2]	*/
      	register float r1, r2;	/* random numbers in [-1, 1] (gauss)	*/
	register float magsq;	/* r1*r1 + r2*r2 			*/
	register float factor;	/* multiplier in Gauss algorithm	*/
        register float r;	/* random number in [-1, 1]  (white)	*/
	float n1, n2;
	long time();		/* system subroutine			*/
	long random();		/* system subroutine			*/
	int srandom();		/* system subroutine			*/


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


	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	if (!(dt = tr.dt)) MUSTFGETPAR("dt", &dt);
	dt /= 1000000.0;


	/* Allocate room for noise vector; allocate 1 extra in case */
	/* nt is odd since Knuth generates pairs of noise values    */
	noise = vector(nt + 1);


	/* Loop over traces */
	do {
		/* Compute signal power:  sigpow += tr[i]*tr[i] */
		svesq_(tr.data, ONE, &sigpow, &nt);


		/* Compute noise vector elements in [-1,1] and power */
		noipow = 0.0;
		switch (itype) {
		case GAUSS:
			for (i = 0; i < nt; i += 2) {

				do {
					r1    = normrand * random() - 1.0;
					r2    = normrand * random() - 1.0;
					magsq = r1 * r1 + r2 * r2;
				} while (magsq >= 1.0);

				factor     = sqrt(-2.0 * log(magsq)/magsq);
				n1 = noise[i]   = factor * r1 / ((i + 0.5)*dt);
				n2 = noise[i+1] = factor * r2 / ((i + 1.5)*dt);
				noipow += n1*n1 + n2*n2;
			}
		break;
		case WHITE:
			for (i = 0; i < nt; i++) {
				r        = normrand * random() - 1.0;
				n1 = noise[i] = r /((i + 0.5)*dt);
				noipow += n1 * n1;
			}
		break;
		default:	/* defensive programming */
			err("%d: mysterious itype = %d", __LINE__, itype);
		}


		/* Compute noise scale for desired noise/signal ratio */
		noiscale = sqrt(sigpow/noipow) / sn;


		/* Add scaled noise to trace:  tr[i] += noiscale*noise[i] */
		vsma_(noise, ONE, &noiscale, tr.data, ONE, tr.data, ONE, &nt);

		puttr(&tr);

	} while (gettr(&tr));


	return SUCCEED;
}
