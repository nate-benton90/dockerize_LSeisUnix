/* SUTWIG2V: $Revision: 2.24 $ ; $Date: 89/09/23 19:04:50 $		*/

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

/************************ self documentation *****************************/
string sdoc = "\
									\n\
SUTWIG2V - full-featured wiggle trace program for data plotting		\n\
	   of two data files (over-and-under)				\n\
									\n\
sutwig2v data1 data2 [optional parameters] | tube			\n\
									\n\
NOTES: 1. No input redirect on input files, and input file 		\n\
          names MUST come before optional parameters.			\n\
       2. The data files MUST have equal nt and nx.	 		\n\
Optional Parameters:							\n\
....LABELING ...							\n\
	title1	= null		title for plot #1			\n\
	title2	= null		title for plot #2			\n\
	label1	= Time 		time axis label				\n\
	label2	= Trace		trace axis label 			\n\
	tmin = tr.delrt 	tic label for first time sample		\n\
	dt = tr.dt or .004	time tic labeling increment 		\n\
	xmin = 1st tr.tracl	tic label for first trace		\n\
	dx = (1st-2nd)tr.tracl	trace tic labeling increment		\n\
	titlsz = 3		title print size			\n\
	lablsz = 3		label print size			\n\
	ticsz  = 2		tic labeling print size			\n\
....MISCELLANEOUS ...							\n\
	gap	= 0.7		gap between plots (inches)		\n\
	ntict   = 5		number of tics on time axis		\n\
	nticx   = 4		number of tics on trace axis		\n\
	tlines	= 1		flag for timing lines (=0 for none)	\n\
	fill	= 1		flag for positive fill 			\n\
				(=0 for no fill)			\n\
	ltic    = 0.05		length of tic mark (inches)		\n\
	plotfat	= 0		line thickness of traces		\n\
	axisfat	= 0		line thickness of box & tics		\n\
....SIZE & LOCATION ...							\n\
	sizet	= 3.0		length of t-axis (inches)		\n\
	sizex	= 5.0		length of x-axis (...)			\n\
	margint	= 0.05		top/bot gap between box and 		\n\
				traces (...)				\n\
	marginx	= 0.05		side gap between box and traces (...)	\n\
	zerot	= 0.8		base of plot to bottom of screen (...)	\n\
	zerox	= 0.5		left of plot to left of screen (...)	\n\
....GAINING ...								\n\
	rel 	= 0		plots scaled independently 		\n\
				=1 for relative scaling (max required)	\n\
	max	= none		global max for relative scaling		\n\
				required if rel =1			\n\
	overlap = 2.0		max deflection (in traces) is 		\n\
				overlap*scale				\n\
									\n\
gain defaults (see sugain):						\n\
	tpow=0.0 epow=0.0 gpow=1.0 agc=0 wagc=20			\n\
	trap=0.0 clip=0.0 qclip=1.0 qbal=1 pbal=0 scale=1.0		\n\
";
/*************************************************************************/

/* Credits:
 *	CWP: Chris
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sutwig2v.c,v $";
static string revid =
	"   $Revision: 2.24 $ ; $Date: 89/09/23 19:04:50 $";




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
	float absmax;		/* absolute max sample on trace 	*/
	int absmaxloc;		/* ... its location (not used) 		*/
	float *dataptr;		/* mega-vector of data from the segys	*/
	float dt;		/* time sample rate (and tic increment)	*/
	float dx;		/* tic label increment for horiz axis	*/
	float givenmax;		/* maximum in data panel for absol scaling*/
	float scale;		/* scale for data gaining		*/
	float tmin;		/* minimum time (and tic label) for dat2*/
	float xmin;		/* minimum tic label for horiz axis	*/
	int fd1, fd2;		/* file descriptors of data1 and data2	*/
	int ffile;		/* file flag for plotting sub		*/
	int n;			/* nt*ntr				*/
	int nt;			/* time samples per trace (from tr.ns)	*/
	int ntsize;		/* number of data bytes on a trace	*/
	int ntr;		/* traces to be plotted			*/
	int relamp;		/* relative amplitude plotting flag	*/
	int tracl2;		/* tracl of 2nd trace (for dx)		*/
	int ndata;		/* bytes allocated for mega-vector	*/
	string label1;		/* vertical axis label (default Time)	*/
	string label2;		/* horiz axis label (default Trace)	*/
	string title1;		/* title on plot1			*/
	string title2;		/* title on plot2			*/
	void gain();		/* see su/lib/gainpkge.c		*/
	void wigplot();		/* isolate cplot commands		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(2); /* two file args required */


	/* Prevent bytes from spilling onto screen */
	if (isatty(STDOUT)) {
		err("must redirect or pipe byte code output");
	}


	/* Open two files given as arguments for reading */
	if (-1 == (fd1 = open(argv[1], 0))) {
		syserr("can't open %s\n", argv[1]);
	}
	if (-1 == (fd2 = open(argv[2], 0))) {
		syserr("can't open %s\n", argv[2]);
	}

/*  read in and plot first input file */

	/*  read first trace */ 
	if (!fgettr(fd1, &tr)) err("can't get first trace");
	ffile =1;		/* working on file 1 */
	nt = tr.ns;
	xmin = tr.tracl;	fgetpar("xmin", &xmin);
	ntsize = nt * FSIZE;

	tmin = tr.delrt/1000.0;	fgetpar("tmin", &tmin);
				/* tr.delrt is in millisecs */

	/* set or get dt for tic labelling */
	if (!fgetpar("dt", &dt)) {
	    if (tr.dt) {  /* is dt field set? */
		    dt = tr.dt / 1000000.0;
	    } else {		/* dt not set, assume 4 ms */
		    dt = 0.004;
		    warn("tr.dt not set, for labeling assume dt=%g", dt);
	    }
	}
	

	/* Allocate block of memory for data float mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	dataptr = vec(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr == 2) tracl2 = tr.tracl;/* needed for dx    */
		if (ntr*nt > ndata) {		/* need more memory */
			ndata <<= 1;		/* ask for double   */
			dataptr = re_vec(dataptr, ndata);
		}
		bcopy(tr.data, dataptr + (ntr - 1)*nt, ntsize); 
	} while(fgettr(fd1, &tr));

	dx = tracl2 - xmin;	fgetpar("dx", &dx);


	/* find if user wants relative scaling between plots */
	scale = 1.0;
	relamp = 0;		igetpar("rel",&relamp);
	if ( relamp == 1 ) {
		MUSTFGETPAR("max",&givenmax);
	}

	/* for rel amp plots find max value in data for scaling */
	if ( relamp == 1 ) {
		n = nt*ntr;
		maxmgv_(dataptr, ONE, &absmax, &absmaxloc, &n);
		scale = absmax/givenmax;
	}

	/* Gain */
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* Plot getpars */
	title1 = "";		sgetpar("title1",  &title1);
	label1 = "Time";	sgetpar("label1", &label1);
	label2 = "Trace";	sgetpar("label2", &label2);


	/* CALL THE PLOTTING PROGRAM AS A SUBROUTINE */
	warn("nt = %d  ntr = %d", nt, ntr);	
	wigplot(dataptr,nt, ntr, title1, label1, label2, 
			tmin, dt, xmin, dx, ffile);

/*  read in and plot second input file */

	/*  read first trace */ 
	if (!fgettr(fd2, &tr)) err("can't get first trace");
	ffile = 2;		/* working on file 2 */
	nt = tr.ns;
	xmin = tr.tracl;	fgetpar("xmin", &xmin);
	ntsize = nt * FSIZE;


	/* Allocate block of memory for data float mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	dataptr = vec(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr == 2) tracl2 = tr.tracl;/* needed for dx    */
		if (ntr*nt > ndata) {		/* need more memory */
			ndata <<= 1;		/* ask for double   */
			dataptr = re_vec(dataptr, ndata);
		}
		bcopy(tr.data, dataptr + (ntr - 1)*nt, ntsize); 
	} while(fgettr(fd2, &tr));

	dx = tracl2 - xmin;	fgetpar("dx", &dx);


	/* for rel amp plots find max value in data for scaling */
	if ( relamp == 1 ) {
		n = nt*ntr;
		maxmgv_(dataptr, ONE, &absmax, &absmaxloc, &n);
		scale = absmax/givenmax;
	}

	/* Gain */
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* Plot getpars */
	title2 = "";		sgetpar("title2",  &title2);


	/* CALL THE PLOTTING PROGRAM AS A SUBROUTINE */
	wigplot(dataptr,nt, ntr, title2, label1, label2, 
			tmin, dt, xmin, dx, ffile);

	endplot();


	return SUCCEED;
}


/* Wiggle plot subroutine for vertical plotting */
void wigplot(dataptr, nt, ntr, title, label1, label2, 
			tmin, dt, xmin, dx, ffile)
float *dataptr;
int nt, ntr;
string title, label1, label2;
float tmin, dt, xmin, dx;
int ffile;
{
	char tval[8];		/* time value string			*/
	char xval[8];		/* x value string			*/
	string tfmt;		/* format string for time tic labels	*/
	string xfmt;		/* format string for x tic labels	*/
	float gap;		/* vertical gap between plots		*/
	float ltic;		/* length of tic marks (inches)		*/
	float margint;		/* top/bot gap between box and traces	*/
	float marginx;		/* side gap between box and traces 	*/
	float mt;		/* margint/scalet			*/
	float mx;		/* marginx/scalex			*/
	float overlap;		/* maximum trace overlap		*/
	float scalet;		/* time axis scale			*/
	float scalex;		/* trace axis scale			*/
	float sizet;		/* length of t-axis (inches)		*/
	float sizex;		/* length of x-axis (inches)		*/
	float tpos;		/* temp for time position		*/
	float xpos;		/* temp for trace position		*/
	float zerot;		/* base of plot to bot. of screen	*/
	float zerox;		/* left of plot to left of screen	*/
	int axisfat;		/* line thickness of box & tics		*/
	int dtict;		/* distance between time tics		*/
	int dticx;		/* distance between trace tics		*/
	int fill;		/* fill flag				*/
	int lablsz;		/* label print size			*/
	int ntict;		/* number of tics on time axis		*/
	int nticx;		/* number of tics on trace axis		*/
	int plotfat;		/* line thickness of traces		*/
	int ticsz;		/* tic labeling print size		*/
	int titlsz;		/* title print size			*/
	int tlines;		/* 1=timing lines (0=no timing lines)	*/
	register int i;		/* counter				*/
	void vertwig();		/* draw vertical wiggle traces		*/

	gap = .7;		fgetpar("gap",&gap);

	fill = 1;		igetpar("fill", &fill);

	overlap = 2.0;		fgetpar("overlap", &overlap);

	zerot = .8;		fgetpar("zerot", &zerot);
	zerox = 0.5;		fgetpar("zerox", &zerox);

	sizet = 3.0;		fgetpar("sizet", &sizet);
	sizex = 5.0;		fgetpar("sizex", &sizex);

	scalet = -sizet/nt;
	scalex = sizex/MAX(ntr, 8);

	margint = 0.05;		fgetpar("margint", &margint);
	marginx = 0.05;		fgetpar("marginx", &marginx);

	ltic = 0.05;		fgetpar("ltic", &ltic);

	plotfat = 0;		igetpar("plotfat", &plotfat);
	axisfat = 0;		igetpar("axisfat", &axisfat);

	titlsz = 3;		igetpar("titlsz", &titlsz);
	lablsz = 3;		igetpar("lablsz", &lablsz);
	ticsz = 2;		igetpar("ticsz", &ticsz);

	tlines = 1;		igetpar("tlines", &tlines);

	ntict = 5;	igetpar("ntict", &ntict);
	dtict = nt/ntict;

	nticx = 4;	igetpar("nticx", &nticx);
	dticx = MAX(ntr/nticx, 1);
	
	/* Vertical location is different for the plots */ 
	if ( ffile == 1 ) {
		zerot = zerot + sizet + gap;
	}

	setscl(scalex, scalet);
	set0(zerox, zerot + sizet);
	setu0( 0.0, 0.0);
	setfat(axisfat);

	mx = marginx/scalex;  mt = margint/scalet; 

	/* Title */
	setcol(1);  
	uText(0.5*ntr, mt + 0.3/scalet, titlsz, 0, title);

	/* Labels */
	setcol(8);  
	uText(-mx-0.3/scalex, 0.5*nt, lablsz, 3, label1); 
	if ( ffile == 2 ) {
	   uText(0.5*ntr, nt-mt-0.4/scalet, lablsz, 0, label2);
	}

	/* Axis box */
	setcol(3);  
	umove( -mx                  ,                  mt ); 
	udraw( -mx                  , (float) (nt-1) - mt );
	udraw( (float) (ntr-1) + mx , (float) (nt-1) - mt ); 
	udraw( (float) (ntr-1) + mx ,                  mt );
	udraw( -mx                  ,                  mt );

	/* Vertical axis tic marks */
	tfmt = (dt >= 1) ? "%g" : "%.3g";
	for (i = 0; i < nt; i += dtict) {
		umove( -mx, (float) i );
		where( &xpos , &tpos );
		draw( xpos-ltic, tpos );
		if (tlines == 1) {
			umove( -mx , (float) i );
			udraw( (float) (ntr-1) + mx , (float) i); 
		}	
	        umove( mx + (ntr-1) , (float) i );
	        where( &xpos , &tpos);
	        draw( xpos + ltic , tpos );
	        where( &xpos , &tpos );
		(void) sprintf(tval, tfmt, tmin + i*dt);
	        text(xpos + 0.1, tpos, ticsz, 0, tval);
	}

	/* Horizontal axis tic marks  */
	xfmt = (dx >= 1) ? "%g" : "%.3g";
	for (i = 0; i < ntr; i++) {
	        umove( (float) i , mt );
	        where( &xpos , &tpos );
	        draw( xpos , tpos + ltic );
	        if (!(i % dticx)) {		/* top tics */
		    	where(&xpos, &tpos);
	    	    	draw(xpos, tpos + ltic);
	        }
	        umove( (float) i , - mt + (nt-1) );
	        where(&xpos, &tpos);
	        draw(xpos, tpos - ltic);
	        if (!(i % dticx)) {		/* bottom tics */
		    	where(&xpos, &tpos);
	    	    	draw(xpos, tpos - ltic);
		    	(void) sprintf(xval, xfmt, xmin + i*dx);
		    	if ( ffile == 2 )
		    		Text(xpos, tpos - 0.22, ticsz, 0, xval);
	        }
	}

	/* Draw wiggle traces */
	setcol(2);  
	setfat(plotfat);
	setscl(scalex*overlap, scalet);
	for (i = 0; i < ntr; i++) {
		setu0(-(float) i / overlap, 0.0);
		vertwig(dataptr + nt*i, nt, fill);
	}

}
