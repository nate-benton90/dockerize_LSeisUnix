#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUPHASE - Apply phase shift to input traces	\n"
" 								\n"
" suftt <stdin >sdout phase=0 					\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	phase=0             phase shift in degrees 	\n"
" Notes: \n"
"	forward fft sign is negative. (exp(-iwt) \n"
" \n"
" Z. Li,        1/2000 \n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	      : Li
 *
 */


#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

segy tr;

main(int argc, char **argv)
{
	float *rt;	/* real trace				*/
	complex *ct;	/* complex transformed trace		*/
	int nt;			/* number of points on input trace	*/
	int nfft;		/* transform length			*/
	int nf;			/* number of frequencies		*/
	int signf=-1, signi=1;		/* sign in exponent of transform	*/
	int i;
	FILE *infp=stdin, *outfp=stdout;
	float phase;
	float scaler, scalei;
	float fr, fi;
	float onfft;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */ 
	file2g(infp);
	file2g(outfp);
	if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns;

	/* optional parameter */
	if (!getparfloat("phase", &phase)) phase = 0.;
	scaler = cos(phase/180.*3.141592654);
	scalei = sin(phase/180.*3.141592654);


	/* Set up pfa fft */
	nfft = npfaro(nt, LOOKFAC * nt);
	if (nfft >= MIN(SU_NFLTS, PFA_MAX))  err("Padded nt=%d--too big", nfft);
	nf = nfft/2 + 1;
	onfft = 1./nfft;

	rt = ealloc1float(nfft);
	ct = ealloc1complex(nf);


	/* Main loop over traces */
	do {

		/* Load trace into rt (zero-padded) */
		memcpy((char*)rt, (char*)tr.data, nt*FSIZE);
		bzero(rt + nt, (nfft-nt)*FSIZE);

		/* FFT */
		pfarc(signf, nfft, rt, ct);

		/* Apply phase shift */
		for (i = 0; i < nf; ++i) {
			fr = ct[i].r*scaler - ct[i].i*scalei;
			fi = ct[i].r*scalei + ct[i].i*scaler;
			ct[i].r = fr;
			ct[i].i = fi;
		}

		/* Inverse FFT */
		pfacr(signi, nfft, ct, rt);

		/* Load back and scale for inverse fft */
		for (i = 0; i < nfft; i++) tr.data[i] = rt[i] * onfft;


		fputtr(outfp,&tr);

	} while (fgettr(infp,&tr));


	return EXIT_SUCCESS;
}
