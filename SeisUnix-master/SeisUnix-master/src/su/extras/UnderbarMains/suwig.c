/* SUWIG: $Revision: 2.27 $ ; $Date: 89/09/20 19:37:59 $		*/

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
#include "cplot.h"

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUWIG - no-frills wiggle trace program for data plotting		\n\
									\n\
suwig <stdin [optional parameters] | tube				\n\
									\n\
Optional Parameters:							\n\
	fill	= 0		flag for positive fill (=1 for fill)	\n\
	sizet	= 6.0		length of t-axis (inches)		\n\
	sizex	= 4.7		length of x-axis (inches)		\n\
	zerot	= 1.0		base of plot to bottom of screen	\n\
	zerox	= 0.6		left of plot to left of screen		\n\
	plotfat	= 0		line thickness of traces		\n\
	overlap = 2.0		max deflection (in traces) is 		\n\
				overlap*scale				\n\
									\n\
gain defaults (see sugain):						\n\
	tpow=0.0 epow=0.0 gpow=1.0 agc=0 wagc=20			\n\
	trap=0.0 clip=0.0 qclip=1.0 qbal=1 pbal=0 scale=1.0		\n\
";
/*************************************************************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Chris, Jack
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/suwig.c,v $";
static string revid =
	"   $Revision: 2.27 $ ; $Date: 89/09/20 19:37:59 $";



/* Set plotting defaults */
#define	 FILL		0
#define	 OVERLAP	2.0
#define	 ZEROT		1.0
#define	 ZEROX		0.6
#define	 SIZET		6.0
#define	 SIZEX		4.7
#define	 PLOTFAT	0

/* Set gain defaults (balance by maximum magnitude) */
#define TPOW	0.0
#define EPOW	0.0
#define GPOW	1.0
#define AGC 	0
#define WAGC	20
#define TRAP	0.0
#define CLIP	0.0
#define QCLIP	1.0
#define QBAL	1	/* default is balance by maximum magnitude 	*/
#define PBAL	0
#define SCALE	1.0

segy tr;


main(argc, argv)
int argc; char **argv;
{
	float *dataptr;		/* mega-vector of data from the segys	*/
	float dt;		/* sample rate				*/
	float tmin;		/* first time on trace			*/
	int nt;			/* time samples per trace (from tr.ns)	*/
	int ntsize;		/* number of data bytes on a trace	*/
	int ntr;		/* traces in input data (from gettr)	*/
	int ndata;		/* floats allocated for mega-vector	*/
	void gain();		/* see su/lib/gainpkge.c		*/
	void wigplot();		/* vertical wiggle plot			*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Prevent bytes from spilling onto screen */
	if (isatty(STDOUT)) {
		err("must redirect or pipe byte code output");
	}


	/* Get info from first trace	*/ 
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	ntsize = nt * FSIZE;


	/* Allocate block of memory for data float mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	dataptr = vec(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr*nt > ndata) {	/* need more memory */
			ndata <<= 1;	/* double ndata	    */
			dataptr = re_vec(dataptr, ndata);
		}
		bcopy(tr.data, dataptr + (ntr - 1)*nt, ntsize); 
	} while (gettr(&tr));


	/* Gain */
	tmin = tr.delrt/1000.0;
	dt = tr.dt/1000000.0;	if (!dt) dt=0.004;
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, SCALE, tmin, dt, WAGC, nt, ntr);


	/* Plot */
	warn("nt = %d  ntr = %d", nt, ntr);	
	wigplot(dataptr, nt, ntr);

	endplot();


	return SUCCEED;
}


/* Wiggle plot subroutine for vertical plotting */
void wigplot(dataptr, nt, ntr)
float *dataptr;
int nt, ntr;
{
	float scalet;		/* time axis scale			*/
	float scalex;		/* trace axis scale			*/
	float sizet;		/* length of t-axis (inches)		*/
	float sizex;		/* length of x-axis (inches)		*/
	float zerot;		/* base of plot to bot. of screen	*/
	float zerox;		/* left of plot to left of screen	*/
	int fill;		/* fill flag				*/
	float overlap;		/* maximum trace overlap		*/
	int plotfat;		/* line thickness of traces		*/
	register int i;		/* counter				*/
	void vertwig();		/* draw vertical wiggle traces		*/


	/* Get parameters */
	if (!igetpar("fill", &fill))		fill = FILL;

	if (!fgetpar("overlap", &overlap))	overlap = OVERLAP;

	if (!fgetpar("zerot", &zerot))		zerot = ZEROT;
	if (!fgetpar("zerox", &zerox))		zerox = ZEROX;

	if (!fgetpar("sizet", &sizet))		sizet = SIZET;
	if (!fgetpar("sizex", &sizex))		sizex = SIZEX;

	scalet = -sizet/nt;
	scalex = sizex/MAX(ntr, 8);

	if (!igetpar("plotfat", &plotfat))	plotfat = PLOTFAT;

	setscl(scalex, scalet);
	set0(zerox, zerot + sizet);

	/* Draw wiggle traces */
	setcol(RED);  
	setfat(plotfat);
	setscl(scalex*overlap, scalet);
	for (i = 0; i < ntr; i++) {
		setu0(-(float) i / overlap, 0.0);
		vertwig(dataptr + nt*i, nt, fill);
	}
}
