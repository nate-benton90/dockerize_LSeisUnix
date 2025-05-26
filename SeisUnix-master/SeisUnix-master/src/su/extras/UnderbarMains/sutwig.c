/* SUTWIG: $Revision: 2.29 $ ; $Date: 89/09/20 19:37:41 $		*/

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
SUTWIG - full-featured wiggle trace program for data plotting		\n\
									\n\
sutwig <stdin [optional parameters] | tube				\n\
									\n\
Optional Parameters:							\n\
....LABELING ...							\n\
	title	= null		plot title				\n\
	label1	= Time 		time axis label				\n\
	label2	= Trace		trace axis label 			\n\
	tmin = tr.delrt 	tic label for first time sample		\n\
	dt = tr.dt or .004	time tic labeling increment 		\n\
	xmin = 1st tr.tracl	tic label for first trace		\n\
	dx = (1st-2nd)tr.tracl	trace tic labeling increment		\n\
	titlsz = 4		title print size			\n\
	lablsz = 4		label print size			\n\
	ticsz  = 3		tic labeling print size			\n\
....MISCELLANEOUS ...							\n\
	ntict   = 5		number of tics on time axis		\n\
	nticx   = 4		number of tics on trace axis		\n\
	tlines	= 1		flag for timing lines (=0 for none)	\n\
	fill	= 1		flag for positive fill (=0 for no fill)	\n\
	ltic    = 0.05		length of tic mark (inches)		\n\
	plotfat	= 0		line thickness of traces		\n\
	axisfat	= 0		line thickness of box & tics		\n\
	hcopy	= 0		honors user/default parameters		\n\
				= 1   forces thesis format		\n\
....SIZE & LOCATION ...							\n\
	sizet	= 6.0		length of t-axis (inches)		\n\
	sizex	= 4.7		length of x-axis (...)			\n\
	margint	= 0.1		top/bot gap between box and traces (...)\n\
	marginx	= 0.1		side gap between box and traces (...)	\n\
	zerot	= 1.0		base of plot to bottom of screen (...)	\n\
	zerox	= 0.6		left of plot to left of screen (...)	\n\
....DETAIL BOX ...							\n\
	db    = 0		flag for detail box (=1 for box)	\n\
	top   = nt/4		time sample at top of detail box	\n\
	bot   = nt/2		time sample at bottom of detail box	\n\
	left  = ntr/2		trace on left side of detail box	\n\
	right = 3*ntr/4		trace on right side of detail box	\n\
	dbfat = 4		line thickness for detail box		\n\
....GAINING ...								\n\
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
 *	CWP: Chris
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sutwig.c,v $";
static string revid =
	"   $Revision: 2.29 $ ; $Date: 89/09/20 19:37:41 $";




/* Set plotting defaults */
#define TITLE 	"";
#define LABEL1	"Time";
#define LABEL2	"Trace";
#define FILL	1;
#define OVERLAP	2.0;
#define ZEROT	1.0;
#define ZEROX	0.6;
#define SIZET	6.0;
#define SIZEX	4.5;
#define MARGINT	0.1;
#define MARGINX	0.1;
#define NTICT	5;
#define NTICX	4;
#define LTIC	0.05;
#define PLOTFAT	0;
#define AXISFAT	0;
#define TITLSZ	4;
#define LABLSZ	4;
#define TICSZ 	3;
#define TLINES	1;
#define DB	0;
#define DBFAT	4;


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
	float dt;		/* time sample rate (and tic increment)	*/
	float dx;		/* tic label increment for horiz axis	*/
	float tmin;		/* minimum time (and tic label)		*/
	float xmin;		/* minimum tic label for horiz axis	*/
	int nt;			/* time samples per trace (from tr.ns)	*/
	int ntsize;		/* number of data bytes on a trace	*/
	int ntr;		/* traces in input data (from gettr)	*/
	int tracl2;		/* tracl of 2nd trace (for dx)		*/
	int ndata;		/* bytes allocated for mega-vector	*/
	string label1;		/* vertical axis label (default Time)	*/
	string label2;		/* horiz axis label (default Trace)	*/
	string title;		/* title on plot			*/
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
	if (!fgetpar("xmin", &xmin))	xmin = tr.tracl;
	if (!fgetpar("tmin", &tmin))	tmin = tr.delrt/1000.0;

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
	} while(gettr(&tr));


	if (!fgetpar("dx", &dx))	 dx = tracl2 - xmin;


	/* Gain */
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, SCALE, tmin, dt, WAGC, nt, ntr);


	/* Plot getpars */
	if (!sgetpar("title",  &title))		title  = TITLE;
	if (!sgetpar("label1", &label1))	label1 = LABEL1;
	if (!sgetpar("label2", &label2))	label2 = LABEL2;


	/* Plot */
	warn("nt = %d  ntr = %d", nt, ntr);	
	wigplot(dataptr,nt, ntr, title, label1, label2, tmin, dt, xmin, dx);

	endplot();


	return SUCCEED;
}



/* Wiggle plot subroutine for vertical plotting */
void wigplot(dataptr, nt, ntr, title, label1, label2, tmin, dt, xmin, dx)
float *dataptr;
int nt, ntr;
string title, label1, label2;
float tmin, dt, xmin, dx;
{
	char tval[8];		/* time value string			*/
	char xval[8];		/* x value string			*/
	string tfmt;		/* format string for time tic labels	*/
	string xfmt;		/* format string for x tic labels	*/
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
	int db;			/* 0=no detail box; 1=detail box	*/
	int dbfat;		/* line thickness for detail box	*/
	int dtict;		/* distance between time tics		*/
	int dticx;		/* distance between trace tics		*/
	int fill;		/* fill flag				*/
	int hcopy;		/* hardcopy flag			*/
	int lablsz;		/* label print size			*/
	int left,top;		/* left side and top for detail box	*/
	int ntict;		/* number of tics on time axis		*/
	int nticx;		/* number of tics on trace axis		*/
	int plotfat;		/* line thickness of traces		*/
	int right, bot;		/* right side and bottom for detail box	*/
	int ticsz;		/* tic labeling print size		*/
	int titlsz;		/* title print size			*/
	int tlines;		/* 1=timing lines (0=no timing lines)	*/
	register int i;		/* counter				*/
	void vertwig();		/* draw vertical wiggle traces		*/


	if (!igetpar("fill", &fill))		fill = FILL;

	if (!fgetpar("overlap", &overlap))	overlap = OVERLAP;

	if (!fgetpar("zerot", &zerot))		zerot = ZEROT;
	if (!fgetpar("zerox", &zerox))		zerox = ZEROX;

	if (!fgetpar("sizet", &sizet))		sizet = SIZET;
	if (!fgetpar("sizex", &sizex))		sizex = SIZEX;

	if (!fgetpar("margint", &margint))	margint = MARGINT;
	if (!fgetpar("marginx", &marginx))	marginx = MARGINX;

	if (!fgetpar("ltic", &ltic))		ltic = LTIC;

	if (!igetpar("plotfat", &plotfat))	plotfat = PLOTFAT;
	if (!igetpar("axisfat", &axisfat))	axisfat = AXISFAT;

	if (!igetpar("titlsz", &titlsz))	titlsz = TITLSZ;
	if (!igetpar("lablsz", &lablsz))	lablsz = LABLSZ;
	if (!igetpar("ticsz", &ticsz))		ticsz  = TICSZ;

	if (!igetpar("tlines", &tlines))	tlines = TLINES;

	/* Detail box parameters */
	if (!igetpar("db", &db))		db = DB;
	if (!igetpar("dbfat", &dbfat))		dbfat = DBFAT;
	if (!igetpar("left", &left))		left = ntr/2;
	if (!igetpar("top", &top))		top = nt/4;
	if (!igetpar("right", &right))		right = 3*ntr/4;
	if (!igetpar("bot", &bot))		bot = nt/2;

	if (!igetpar("ntict", &ntict))		ntict = NTICT;

	if (!igetpar("nticx", &nticx))		nticx = NTICX;

	if (!igetpar("hcopy", &hcopy))		hcopy = 0;
	if (hcopy) {
	   	titlsz = 3;
	   	lablsz = 3;
	   	ticsz = 2;
	   	sizet = 5.25;
	   	sizex = 4.4;
	   	zerot = 1.5;
	   	zerox = 1.;
	}

	scalet = -sizet/nt;
	scalex = sizex/MAX(ntr, 8);
	dtict = nt/ntict;
	dticx = MAX(ntr/nticx, 1);

	setscl(scalex, scalet);
	set0(zerox, zerot + sizet);
	setfat(axisfat);

	mx = marginx/scalex;  mt = margint/scalet; 

	/* Title */
	setcol(WHITE);  
	uText(0.5*ntr, mt + 0.45/scalet, titlsz, 0, title);

	/* Labels */
	setcol(ORANGE);  
	uText(-mx-0.3/scalex, 0.5*nt, lablsz, 3, label1); 
	uText(0.5*ntr, nt-mt-0.6/scalet, lablsz, 0, label2);

	/* Axis box */
	setcol(GREEN);  
	umove( -mx                  ,                  mt ); 
	udraw( -mx                  , (float) (nt-1) - mt );
	udraw( (float) (ntr-1) + mx , (float) (nt-1) - mt ); 
	udraw( (float) (ntr-1) + mx ,                  mt );
	udraw( -mx                  ,                  mt );

	/* Vertical axis tic marks  */
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
	for (i = 0; i < ntr; ++i) {
	        umove( (float) i , mt );
	        where( &xpos , &tpos );
	        draw( xpos , tpos + ltic );
	        if (!(i % dticx)) {
			where(&xpos, &tpos);
			draw(xpos, tpos + ltic);
	        }
	        umove( (float) i , - mt + (nt-1) );
	        where(&xpos, &tpos);
	        draw(xpos, tpos - ltic);
	        if (!(i % dticx)) {
			where(&xpos, &tpos);
			draw(xpos, tpos - ltic);
			(void) sprintf(xval, xfmt, xmin + i*dx);
			Text(xpos, tpos - 0.22, ticsz, 0, xval);
	        }
	}

	/* Draw wiggle traces */
	setcol(RED);  
	setfat(plotfat);
	setscl(scalex*overlap, scalet);
	for (i = 0; i < ntr; ++i) {
		setu0(-(float) i / overlap, 0.0);
		vertwig(dataptr + nt*i, nt, fill);
	}

	/* Detail box */
	setu0(0,0);
	setscl(scalex, scalet);
	setcol(GREEN);  
	if (db) { 
		setfat(dbfat); 
		umove( (float) (left -1) - .5 , (float) (top-1) );    
		udraw( (float) (right-1) + .5 , (float) (top-1) ); 
		udraw( (float) (right-1) + .5 , (float) (bot-1) ); 
		udraw( (float) (left -1) - .5 , (float) (bot-1) ); 
		udraw( (float) (left -1) - .5 , (float) (top-1) ); 
		setfat(axisfat); 
	} 

}
