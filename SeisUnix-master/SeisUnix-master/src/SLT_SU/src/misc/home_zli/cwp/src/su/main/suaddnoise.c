/* SUADDNOISE: $Revision: 1.27 $ ; $Date: 90/12/23 23:54:18 $		*/

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
#include "header.h"
#include <time.h>
#include <signal.h>

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUADDNOISE - add noise to traces					\n"
" 									\n"
" suaddnoise <stdin >stdout  sn=20  noise=gauss  seed=from_clock	\n"
" 									\n"
" Required parameters:							\n"
" 	if any of f1,f2,f3,f4 are specified by the user and if dt is	\n"
" 	not set in header, then dt is mandatory				\n"
" 									\n"
" Optional parameters:							\n"
" 	sn=20			signal to noise ratio			\n"
" 	noise=gauss		noise probability distribution		\n"
" 				=flat for uniform; default Gaussian	\n"
" 	seed=from_clock		random number seed (integer)		\n"
" 							        	\n"
" 	f1=0.0			left  low  corner frequency (Hz)	\n"
" 	f2=0.0			left  high corner frequency (Hz)	\n"
" 	f3= nyquist		right low  corner frequency (Hz)	\n"
" 	f4= nyquist		right high corner frequency (Hz)	\n"
" 	dt= (from header)	time sampling interval (sec)		\n"
"       scrdir=$SU_SCRATCHDIR   scratch directory                       \n"
" 									\n"
" NOTES:								\n"
" 	Output = Signal +  scale * Noise				\n"
" 									\n"
" 	scale = (1/sn) * (absmax_signal/sqrt(2))/sqrt(energy_per_sample)\n"
" 									\n"
" 	If the signal is already band-limited, f1,f2,f3,f4 can be used	\n"
" 	as in suband to bandlimit the noise traces to match the signal	\n"
" 	band prior to computing the scale defined above.		\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Jack, Brian, Ken
 *
 * Notes:
 *	At S/N = 2, the strongest reflector is well delineated, so to
 *	see something 1/nth as strong as this dominant reflector
 *	requires S/N = 2*n.
 *
 *	My understanding of "signal" has forced some declarations
 *	to be global that would otherwise be made in the local block
 *	that handles the band-limiting.
 */


/* Default signal to noise ratio */
#define SN	20

/* Noise probability distributions */
#define	GAUSS	0
#define	FLAT	1


/* Prototype */
static void trapsig(void);

/* Globals (so can trap signal) defining temporary disk file */
static char *bandoutfile;           /* output file for suband	*/
static FILE *bandoutfp;		    /* fp for output file	*/


segy tr;

main(int argc, char **argv)
{
	int nt;			/* number of points on trace		*/
	int databytes;		/* ... in bytes 			*/
	int ntr;		/* number of traces			*/
	string stype;		/* noise type (gauss, flat) as string	*/
	int itype;		/* ... as integer (for use in switch)	*/
	float sn;		/* signal to noise ratio		*/
	unsigned int seed;	/* random number seed			*/
	FILE *hdrfp;		/* fp for header storage file		*/
	FILE *sigfp;		/* fp for data ("signal")		*/
	int nfloats;		/* number of floats in "signal"		*/
	float *noise;		/* noise vector				*/
	float noiscale;		/* scale for noise			*/
	float absmaxsig;	/* absolute maximum in signal		*/
	float noipow;		/* a measure of noise power		*/
	float f1;		/* left lower corner frequency		*/
	float f2;		/* left upper corner frequency		*/
	float f4;		/* right lower corner frequency		*/
	float f3;		/* right upper corner frequency		*/
        char * scrdir;          /* scratch dir to put temporary data set */

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get noise type */
	if (!getparstring("noise", &stype))	stype = "gauss";

	if      (STREQ(stype, "gauss")) itype = GAUSS;
	else if (STREQ(stype, "flat"))  itype = FLAT;
	else     err("noise=\"%s\", must be gauss or flat", stype);


	/* Get signal to noise ratio */
	if (!getparfloat("sn", &sn))	sn = SN;
	if (sn <= 0) err("sn=%d must be positive", sn);


	/* Set seed */
	if (!getparuint("seed", &seed)) { /* if not supplied, use clock */
		if (-1 == (seed = (uint) time((time_t *) NULL))) {
			err("time() failed to set seed");
		}
	}
	(itype == GAUSS) ? srannor(seed) : sranuni(seed);

        if( !getparstring("scrdir",&scrdir) ) {
            scrdir = getenv("SU_SCRATCHDIR") ;
        }


	/* Prepare temporary files to hold headers and data */
	/*
	hdrfp = etmpfile();
	sigfp = etmpfile();
	*/
	hdrfp = etempfile(NULL);
	sigfp = etempfile(NULL);


	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	databytes = nt * FSIZE;


	/* Loop over input traces & write headers and data to tmp files */
	ntr = 0;
	do {
		++ntr;
		efwrite(&tr, 1, HDRBYTES, hdrfp);
		efwrite(tr.data, 1, databytes, sigfp);
	} while (gettr(&tr));
	nfloats = ntr * nt;


	/* Compute absmax of signal over entire data set */
	rewind(sigfp);
	absmaxsig = 0.0;
	{ register int i;
	  for (i = 0; i < nfloats; ++i) {
		float sigval;
		efread(&sigval, FSIZE, 1, sigfp);
		absmaxsig = MAX(absmaxsig, ABS(sigval));
	  }
	}


	/* Compute noise vector elements in [-1, 1] */
	noise = ealloc1float(nfloats);
	switch (itype) {
		register int i;
	case GAUSS: /* frannor gives elements in N(0,1)--ie. pos & negs */
		for (i = 0; i < nfloats; ++i)  noise[i] = frannor();
	break;
	case FLAT: /* franuni gives elements in [0, 1] */
		for (i = 0; i < nfloats; ++i)  noise[i] = 2.0*franuni() - 1.0;
	break;
	default:	/* defensive programming */
		err("%d: mysterious itype = %d", __LINE__, itype);
	}


	/* Band limit noise traces if user getpars any of the f's */
	if (getparfloat("f1", &f1) || getparfloat("f2", &f2) ||
	    getparfloat("f3", &f3) || getparfloat("f4", &f4) ) {

		/* Set up call to suband */
		char cmdbuf[BUFSIZ];	    /* build suband command	*/
		FILE *bandinfp;		    /* fp for input file	*/
		FILE *fp;                   /* fp for pipe to suband	*/
		int nsegy = HDRBYTES + databytes;
		char *segybuf = ealloc1(nsegy, 1);


		/* Trap signals so can remove tmpnam file */
		signal(SIGINT,  (void *) trapsig);
		signal(SIGQUIT, (void *) trapsig);
		signal(SIGHUP,  (void *) trapsig);
		signal(SIGTERM, (void *) trapsig);

		/* Prepare temporary files to hold traces */
		/*bandinfp  = etmpfile();*/
		bandinfp  = etempfile(NULL);
		/*bandoutfp = efopen(etmpnam(bandoutfile), "w+");*/
		bandoutfile = etempnam(scrdir,NULL) ;
		bandoutfp = efopen(bandoutfile, "w+");

		/* Paste headers on noise traces and put in tmpfile */
		rewind(hdrfp);
		{ register int itr;
		  for (itr = 0; itr < ntr; ++itr) {
			efread(&tr, 1, HDRBYTES, hdrfp);
			memcpy(tr.data, noise + itr*nt, databytes); 
			fputtr(bandinfp, &tr);
		  }
		}

		/* Pipe to suband - suband handles the getpars */
		sprintf(cmdbuf, "suband >%s", bandoutfile);
		fp = epopen(cmdbuf, "w");
		rewind (bandinfp);
		{ register int itr;
		  for (itr = 0; itr < ntr; ++itr) {
			efread(segybuf, 1, nsegy, bandinfp);
			efwrite(segybuf, 1, nsegy, fp);
		  }
		}
		efclose(bandinfp);
		epclose(fp);

		/* Load bandlimited traces back into noise vector */
		rewind(bandoutfp);
		{ register int itr;
		  for (itr = 0; itr < ntr; ++itr) {
			fgettr(bandoutfp, &tr);
			memcpy(noise + itr*nt, tr.data, databytes); 
		  }
		}
		efclose(bandoutfp);
		eremove(bandoutfile);

	} /* End optional bandlimiting */
		


	/* Compute noise power */
	noipow = 0.0;
	{ register int i;
	  for (i = 0; i < nfloats; ++i) {
		register float noiseval = noise[i];
		noipow += noiseval * noiseval;
	  }
	}


	/* Compute noise scale for desired noise/signal ratio */
	absmaxsig /= sqrt(2.0);  /* make it look like a rmsq value   */
	noipow /= nfloats;	 /* make it the square of rmsq value */
	noiscale = absmaxsig / (sn * sqrt(noipow));


	/* Add scaled noise to trace and output sum */
	rewind(hdrfp);
	rewind(sigfp);
	{ register int itr;
	  for (itr = 0; itr < ntr; ++itr) {
		register int trshift = itr*nt;
		register int i;

		efread(&tr, 1, HDRBYTES, hdrfp);
		efread(tr.data, 1, databytes, sigfp);
		for (i = 0; i < nt; ++i)
			tr.data[i] += noiscale * noise[trshift + i];

		puttr(&tr);
	  }
	}


	return EXIT_SUCCESS;
}



/* Signal handler to remove tmpnam file */
void trapsig(void)
{
	efclose(bandoutfp);
	eremove(bandoutfile);
	exit(EXIT_FAILURE);
}
