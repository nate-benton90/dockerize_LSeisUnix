/* sutwig -- full featured wiggle trace plotter 
 *
 * Example: 
 *	sutwig < su_data | tube
 *
 * Credits:
 *	SEP: Shuki
 *	CWP: Chris
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
 * $Source: /src/su/src/RCS/sutwig.c,v $
 * $Revision: 1.29 $ ; $Date: 88/07/06 20:23:38 $
*/

/*********************** self documentation ******************************/
char *sdoc = "\
SUTWIG - full-featured wiggle trace program for data plotting		\n\
									\n\
sutwig <stdin [optional parameters] | tube				\n\
									\n\
Optional Parameters:							\n\
    LABELING ...							\n\
	title	= null		plot title				\n\
	label1	= Time 		time axis label				\n\
	label2	= Trace		trace axis label 			\n\
	tmin = tr.delrt		tic label for first time sample	(float)	\n\
	dt = tr.dt or .004	time tic labeling increment (float)	\n\
	xmin = 1st tr.tracl	tic label for first trace (float)	\n\
	dx = (1st-2nd)tr.tracl	trace tic labeling increment (float)	\n\
	titlsz = 4		title print size			\n\
	lablsz = 4		label print size			\n\
	ticsz  = 3		tic labeling print size			\n\
    MISCELLANEOUS ...							\n\
	ntict   = 5		number of tics on time axis		\n\
	nticx   = 4		number of tics on trace axis		\n\
	tlines	= 1		flag for timing lines (=0 for none)	\n\
	fill	= 1		flag for positive fill (=0 for no fill)	\n\
	ltic    = 0.05		length of tic mark (inches)		\n\
	plotfat	= 0		line thickness of traces		\n\
	axisfat	= 0		line thickness of box & tics		\n\
    SIZE & LOCATION ...							\n\
	sizet	= 6.0		length of t-axis (inches)		\n\
	sizex	= 4.7		length of x-axis (...)			\n\
	margint	= 0.1		top/bot gap between box and traces (...)\n\
	marginx	= 0.1		side gap between box and traces (...)	\n\
	zerot	= 1.0		base of plot to bottom of screen (...)	\n\
	zerox	= 0.6		left of plot to left of screen (...)	\n\
    DETAIL BOX ...							\n\
	db    = 0		flag for detail box (=1 for box)	\n\
	top   = nt/4		time sample at top of detail box (int)	\n\
	bot   = nt/2		time sample at bottom of detail box(int)\n\
	left  = ntr/2		trace on left side of detail box (int)	\n\
	right = 3*ntr/4		trace on right side of detail box (int)	\n\
	dbfat = 4		line thickness for detail box (int)	\n\
    GAINING ...								\n\
	overlap = 2	max deflection (in traces) is overlap*scale	\n\
	gain defaults (see sugain):					\n\
	tpow=0.0 epow=0.0 gpow=1.0 agc=0 wagc=20			\n\
	trap=0.0 clip=0.0 qclip=1.0 qbal=1 pbal=0 scale=1.0		\n\
";
/*************************************************************************/


/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/src/RCS/sutwig.c,v $";
static char revid[] =
	"   $Revision: 1.29 $ ; $Date: 88/07/06 20:23:38 $";


#include "../include/cwp.h"
#include "../include/segy.h"

#define	BLOCK 1024*1024

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
	int ntbytes;		/* number of data bytes on a trace	*/
	int ntr;		/* traces in input data (from gettr)	*/
	int tracl2;		/* tracl of 2nd trace (for dx)		*/
	uint dalloc;		/* incremental allocation 		*/
	uint databytes;		/* bytes allocated for mega-vector	*/
	char *label1;		/* vertical axis label (default Time)	*/
	char *label2;		/* horiz axis label (default Trace)	*/
	char *title;		/* title on plot			*/
	void gain();		/* see su/lib/gainpkge.c		*/
	void subplot();		/* isolate vplot commands		*/


	initgetpar(argc, argv); askdoc(1);


	/* Prevent bytes from spilling onto screen */
	if (isatty(STDOUT)) {
		err("must redirect or pipe byte code output");
	}


	/* Get info from first trace	*/ 
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	xmin = tr.tracl;	getpar("xmin", &xmin);
	ntbytes = nt * sizeof(float);

	tmin = tr.delrt/1000.0;	getpar("tmin", &tmin);
				/* tr.delrt is in millisecs */

	if (!getpar("dt", &dt)) {
	    if (tr.dt) {  /* is dt field set? */
		    dt = tr.dt / 1000000.0;
	    } else {		/* dt not set, assume 4 ms */
		    dt = 0.004;
		    warn("tr.dt not set, for labeling assume dt=%g", dt);
	    }
	}
	

	/* Allocate block of memory for data mega-vector */
	databytes = MAX(BLOCK, ntbytes); /* alloc at least one trace */
	dalloc = databytes;
	if (NULL == (dataptr = (float *) malloc(databytes))) {
		syserr("malloc failed on databytes = %d bytes", databytes);
	}

	/* Byte copy first trace (tr.data, length ntbytes) into data  */
	bcopy(tr.data, dataptr, ntbytes); 

	/* Loop over input traces & put them into data mega-vector  */
	for (ntr = 1; gettr(&tr); ntr++) {
		if (ntr == 1) tracl2 = tr.tracl;	/* needed for dx */
		if ((ntr+1)*ntbytes >= databytes) {  /* need more memory */	
			databytes += dalloc;
			if (NULL == (dataptr = (float *)
				realloc((char *) dataptr, databytes))) {
				syserr("Can't realloc %d bytes", databytes);
			}
		}
		bcopy(tr.data, dataptr + ntr*nt, ntbytes); 
	}

	dx = tracl2 - xmin;	getpar("dx", &dx);

	/* Gain */
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, SCALE, tmin, dt, WAGC, nt, ntr);

	/* Plot getpars */
	title = "";		getpar("title",  &title);
	label1 = "Time";	getpar("label1", &label1);
	label2 = "Trace";	getpar("label2", &label2);


	/* CALL THE PLOTTING PROGRAM AS A SUBROUTINE */
	warn("nt = %d  ntr = %d", nt, ntr);	
	subplot(dataptr,nt, ntr, title, label1, label2, tmin, dt, xmin, dx);

	endplot();
	exit(0);
}


/* Wiggle plot subroutine for vertical plotting */
void subplot(dataptr, nt, ntr, title, label1, label2, tmin, dt, xmin, dx)
float *dataptr;
int nt, ntr;
char *title, *label1, *label2;
float tmin, dt, xmin, dx;
{
	float ltic;		/* length of tic marks (inches)		*/
	float margint;		/* top/bot gap between box and traces	*/
	float marginx;		/* side gap between box and traces 	*/
	float mt;		/* margint/scalet			*/
	float mx;		/* marginx/scalex			*/
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
	int lablsz;		/* label print size			*/
	int left,top;		/* left side and top for detail box	*/
	int ntict;		/* number of tics on time axis		*/
	int nticx;		/* number of tics on trace axis		*/
	int overlap;		/* maximum trace overlap		*/
	int plotfat;		/* line thickness of traces		*/
	int right,bot;		/* right side and bottom for detail box	*/
	int ticsz;		/* tic labeling print size		*/
	int titlsz;		/* title print size			*/
	int tlines;		/* 1=timing lines (0=no timing lines)	*/
	register int i;		/* counter				*/


	fill = 1;		getpar("fill", &fill);

	overlap = 2.0;		getpar("overlap", &overlap);

	zerot = 1.0;		getpar("zerot", &zerot);
	zerox = 0.6;		getpar("zerox", &zerox);

	sizet = 6.0;		getpar("sizet", &sizet);
	sizex = 4.5;		getpar("sizex", &sizex);

	scalet = -sizet/nt;
	scalex = sizex/MAX(ntr, 8);

	margint = 0.1;		getpar("margint", &margint);
	marginx = 0.1;		getpar("marginx", &marginx);

	ltic = 0.05;		getpar("ltic", &ltic);

	plotfat = 0;		getpar("plotfat", &plotfat);
	axisfat = 0;		getpar("axisfat", &axisfat);

	titlsz = 4;		getpar("titlsz", &titlsz);
	lablsz = 4;		getpar("lablsz", &lablsz);
	ticsz = 3;		getpar("ticsz", &ticsz);

	tlines = 1;		getpar("tlines", &tlines);

	/* Detail box parameters */
	db = 0;			getpar("db", &db);
	dbfat = 4;		getpar("dbfat", &dbfat);
	left = ntr/2;		getpar("left", &left);
	top = nt/4;		getpar("top", &top);
	right = 3*ntr/4;	getpar("right", &right);
	bot = nt/2;		getpar("bot", &bot);

	ntict = 5;		getpar("ntict", &ntict);
	dtict = nt/ntict;

	nticx = 4;		getpar("nticx", &nticx);
	dticx = ntr/nticx;

	setscl(scalex, scalet);
	set0(zerox, zerot + sizet);
	setfat(axisfat);

	mx = marginx/scalex;  mt = margint/scalet; 

	/* TITLE */
	setcol(1);  
	uText(0.5*ntr, mt + 0.45/scalet, titlsz, 0, title);

	/* LABELS */
	setcol(8);  
	uText(-mx-0.3/scalex, 0.5*nt, lablsz, 3, label1); 
	uText(0.5*ntr, nt-mt-0.6/scalet, lablsz, 0, label2);

	/* AXIS BOX */
	setcol(3);  
	umove( -mx                  ,                  mt ); 
	udraw( -mx                  , (float) (nt-1) - mt );
	udraw( (float) (ntr-1) + mx , (float) (nt-1) - mt ); 
	udraw( (float) (ntr-1) + mx ,                  mt );
	udraw( -mx                  ,                  mt );

	/* VERTICAL AXIS TIC MARKS  */
	for (i = 0; i <= nt; i += dtict) {
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
		if ( dt > 1 ) {
			sprintf(title, "%g", (float) tmin + i*dt);
		} else {
			sprintf(title, "%.3g", (float) tmin + i*dt);
		}
	        text(xpos + 0.1, tpos, ticsz, 0, title);
	}

	/* HORIZ AXIS TIC MARKS  */
	for (i = 0; i <ntr; i++) {
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
		    if ( dx > 1 ) {
		        sprintf(title, "%g", (float) xmin + i*dx);
		    } else {
		        sprintf(title, "%.3g", (float) xmin + i*dx);
		    }
		    Text(xpos, tpos - 0.22, ticsz, 0, title);
	        }
	}

	/* DRAW N2 WIGGLE TRACES */
	setcol(2);  
	setfat(plotfat);
	setscl(scalex*overlap, scalet);
	for (i = 0; i < ntr; i++) {
		setu0(-(float) i / (float) overlap, 0.0);
		vertwig(dataptr + nt*i, nt, fill, overlap);
	}

	/* DETAIL BOX */
	setu0(0,0);
	setscl(scalex, scalet);
	setcol(3);  
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


#define WEPS	0.001	/* Patch to avoid filling zero width polygons */

/* Shuki's code and we don't understand it (jkc, chris) */
vertwig(data, n, fill, overlap)
float *data;
int n, fill, overlap;
{
	int lp = 0;
	static bool first = true;
	static float *xp, *yp;
	float s;
	register int i;

	if (first) {
		xp = vector(n + 2);
		yp = vector(n + 2);
		first = false;
	}
	
	s = (fill) ? WEPS : (float) overlap;
	
	if (s <= data[0] && s < data[1]) {
		yp[0] = 0.0;
		xp[0] = s;
		yp[1] = 0.0;
		xp[1] = data[0];
		lp = 2;
	}
	umove(data[0], 0.0);
	for (i = 1; i < n; i++) {
		udraw(data[i], (float) i);
		if (data[i] > s) {
			if (data[i-1] <= s) {
				yp[0] = (s-data[i])/(data[i]-data[i-1]) + i;
				xp[0] = s;
				lp = 1;
			}
			yp[lp] = i;
			xp[lp] = data[i];
			lp++;
		}
		if (data[i] < s && data[i-1] > s) {
			yp[lp] = (s - data[i]) / (data[i] - data[i-1]) + i;
			xp[lp] = s;
			lp++;
			if (lp > 2) uarea(xp, yp, lp, 0, 1, 1);
			lp = 0;
		} else if (i == n-1 && lp) {
			yp[lp] = i;
			xp[lp] = s;
			lp++;
			if (lp > 2) uarea(xp, yp, lp, 0, 1, 1);
		}
	}
}
