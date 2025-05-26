/* SUPOW: $Revision: 2.8 $ ; $Date: 89/06/15 13:03:45 $			*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUPOW - compute sum of squares on evenly spaced rectangular grid	\n\
									\n\
supow <stdin >stdout [optional parameters] 				\n\
									\n\
Required parameters:							\n\
	none								\n\
									\n\
Optional parameters:							\n\
	outpar = /dev/tty	output parameter file, contains:	\n\
				output filename (in=)			\n\
				number of geophones (n1=)		\n\
				number of shotpoints (n2=)		\n\
									\n\
	ds = 1		shot spacing					\n\
									\n\
	dg = 1		geophone spacing				\n\
									\n\
	big = 0		abort if ds and dg are such that the		\n\
			number of grid points exceeds a certain		\n\
			multiple of the number of traces in the		\n\
			original data set; big = 1 to unset		\n\
									\n\
	verbose = 1	echo information to stderr, verbose = 0 	\n\
			to unset					\n\
									\n\
Note:									\n\
	The output is computed on a rectangular grid ordered by		\n\
	shotpoint (sx) and then by geophone location (gx) within sx.	\n\
	The grid boundaries are defined by the smallest and largest sx	\n\
	and gx in the original data.  The density is defined by ds and	\n\
	dg.  Points not corresponding to an (sx, gx) pair in the	\n\
	original data are assigned zero power. 				\n\
									\n\
	Typical use:							\n\
 		supow <stdin >stdout ds=200 dg=50 outpar=parfile	\n\
									\n\
";
/*************************************************************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/supow.c,v $";
static string revid =
	"   $Revision: 2.8 $ ; $Date: 89/06/15 13:03:45 $";



#define	MULTIPLE	8	/* multiple of ntr to abort on if !big */

segy tr;

main(argc, argv)
int argc; char **argv;
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
	string outname;		/* output file			*/
	string outpar;		/* name of parfile		*/
	FILE *outparfp;		/* ... its file pointer		*/
	int verbose;		/* echo info to stderr		*/
	int big;		/* do dense grids		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get parameters */
	ds = 1;			igetpar("ds"     , &ds);
	dg = 1;			igetpar("dg"     , &dg);
	outpar = "/dev/tty" ;	sgetpar("outpar" , &outpar);
	big = 0;		igetpar("big"    , &big);
	verbose = 0;		igetpar("verbose", &verbose);

	if (verbose) warn("ds = %d\tdg = %d", ds, dg);

	/* Open file to save parameters */
	if (NULL == (outparfp = fopen(outpar, "w"))) {
		syserr("can't open outpar=\"%s\"", outpar);
	}


	/* Allocate sgp array */
	ntr = gettra(STDIN, &tr, 0);	/* Kludge: gettra returns ntr */
	if (NULL == (tbp = (struct sgp *) malloc(ntr * sizeof(struct sgp))))
		syserr("malloc failed on sgp structure");
	nt = tr.ns;


	/* Fill sgp array; determine size of grid */
	gmin =  LONG_MAX; gmax =  LONG_MIN;
	smin =  LONG_MAX; smax =  LONG_MIN;
	itr = 0;
	do {
		tbp[itr].s = tr.sx/ds;
		tbp[itr].g = tr.gx/dg;
		svesq_(tr.data, ONE, &power, &nt);
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
	powvec = (float *) calloc(npoint, FSIZE);


	/* Fill powvec at actual (sx, gx) locations */
	for (itr = 0; itr < ntr; itr++) {
		powvec[tbp[itr].s * ngeo + tbp[itr].g] = tbp[itr].p;
	}


	/* Write the power file */
	ewrite(STDOUT, (char *) powvec, sgpsize);


	/* Make par file for headerless power file */
	outname = getname(STDOUT);
	(void) fprintf(outparfp, "in=%s\tn1=%d\tn2=%d\tn3=1\n",
						outname, ngeo, nshot);


	return SUCCEED;
}
