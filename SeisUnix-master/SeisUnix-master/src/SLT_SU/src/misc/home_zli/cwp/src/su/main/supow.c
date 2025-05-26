/* SUPOW: $Revision: 1.5 $ ; $Date: 90/12/23 16:44:29 $			*/

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

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUPOW - compute sum of squares on evenly spaced rectangular grid	\n"
" 									\n"
" supow <stdin >stdout [optional parameters] 				\n"
" 									\n"
" Required parameters:							\n"
" 	none								\n"
" 									\n"
" Optional parameters:							\n"
" 	outpar=/dev/tty		output parameter file, contains:	\n"
" 				number of geophones (n1=)		\n"
" 				number of shotpoints (n2=)		\n"
" 									\n"
" 	ds=1		shot spacing					\n"
" 									\n"
" 	dg=1		geophone spacing				\n"
" 									\n"
" 	big=0		abort if ds and dg are such that the		\n"
" 			number of grid points exceeds a certain		\n"
" 			multiple of the number of traces in the		\n"
" 			original data set; big = 1 to unset		\n"
" 									\n"
" 	verbose=1	echo information to stderr, verbose = 0 	\n"
" 			to unset					\n"
" 									\n"
" Note:									\n"
" 	The output is computed on a rectangular grid ordered by		\n"
" 	shotpoint (sx) and then by geophone location (gx) within sx.	\n"
" 	The grid boundaries are defined by the smallest and largest sx	\n"
" 	and gx in the original data.  The density is defined by ds and	\n"
" 	dg.  Points not corresponding to an (sx, gx) pair in the	\n"
" 	original data are assigned zero power. 				\n"
" 									\n"
" 	Typical use:							\n"
"  		supow <stdin >stdout ds=200 dg=50 outpar=parfile	\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *
 *	SEP: Shuki
 */



#define	MULTIPLE	8	/* multiple of ntr to abort on if !big */

segy tr;

main(int argc, char **argv)
{
	struct sgp {	/* shot-geophone power structure	*/
		int s;		/* normalized shotpoint		*/
		int g;		/* normalized geophone number	*/
		float p;	/* power			*/
	} *tbp;
	uint sgpsize;		/* bytes in the sgp array	*/
	float power;		/* temp for power		*/
	int nt;			/* number of samples on trace	*/
	int ds;			/* shot spacing			*/
	int dg;			/* gephone spacing		*/
	int smin;		/* first shot point / ds	*/
	int smax;		/* last shot point / ds		*/
	int gmin;		/* first geophone station / dg	*/
	int gmax;		/* last geophone station / dg	*/
	int nshot;		/* number of shot points	*/
	int ngeo;		/* number of geophone stations	*/
	uint npoint;		/* nshot * ngeo			*/
	int ntr;		/* number of traces in file	*/
	int itr;		/* counter over traces		*/
	float *powvec;		/* array of powers output	*/
	string outpar;		/* name of parfile		*/
	FILE *outparfp;		/* ... its file pointer		*/
	int verbose;		/* echo info to stderr		*/
	int big;		/* do dense grids		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	ds = 1;			getparint   ("ds"     , &ds);
	dg = 1;			getparint   ("dg"     , &dg);
	outpar = "/dev/tty" ;	getparstring("outpar" , &outpar);
	big = 0;		getparint   ("big"    , &big);
	verbose = 0;		getparint   ("verbose", &verbose);

	if (verbose) warn("ds = %d\tdg = %d", ds, dg);

	/* Open file to save parameters */
	outparfp = efopen(outpar, "w");


	/* Allocate sgp array */
	ntr = gettra(&tr, 0);	/* Kludge: gettra returns ntr */
	tbp = (struct sgp *) emalloc(ntr * sizeof(struct sgp));
	nt = tr.ns;


	/* Fill sgp array; determine size of grid */
	gmin =  LONG_MAX; gmax =  LONG_MIN;
	smin =  LONG_MAX; smax =  LONG_MIN;
	itr = 0;
	do {
		register int i;
		register float val;

		tbp[itr].s = tr.sx/ds;
		tbp[itr].g = tr.gx/dg;
		power = 0.0;
		for (i = 0; i < nt; ++i) {
			val = tr.data[i];
			power += val * val;
		}
		tbp[itr].p = power;

		smin = MIN(tbp[itr].s, smin); smax = MAX(tbp[itr].s, smax);
		gmin = MIN(tbp[itr].g, gmin); gmax = MAX(tbp[itr].g, gmax);
		
		itr++;
	} while (gettr(&tr));
	nshot = smax - smin + 1;
	ngeo  = gmax - gmin + 1;
	npoint= nshot * ngeo;
	if (!big && npoint > MULTIPLE * ntr)
		err("too many points (%u), use big=1 to override", npoint);
	sgpsize = npoint * FSIZE;

	if (verbose) {
		warn("ntr=%d", ntr);
		warn("smin=%d\tsmax=%d\tnshot=%d", smin*ds, smax*ds, nshot);
		warn("gmin=%d\tgmax=%d\tngeo=%d", gmin*dg, gmax*dg, ngeo);
	}

	for (itr = 0; itr < ntr; itr++) {
		tbp[itr].s -= smin;
		tbp[itr].g -= gmin;
	}


	/* Allocate power vector initialized to zero */
	powvec = (float *) ecalloc(npoint, FSIZE);


	/* Fill powvec at actual (sx, gx) locations */
	for (itr = 0; itr < ntr; itr++) {
		powvec[tbp[itr].s * ngeo + tbp[itr].g] = tbp[itr].p;
	}


	/* Write the power file */
	efwrite(powvec, 1, sgpsize, stdout);


	/* Make par file for headerless power file */
	(void) fprintf(outparfp, "n1=%d\tn2=%d\tn3=1\n", ngeo, nshot);


	return EXIT_SUCCESS;
}
