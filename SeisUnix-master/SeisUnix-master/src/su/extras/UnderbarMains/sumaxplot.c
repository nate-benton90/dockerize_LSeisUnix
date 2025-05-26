/* SUMAXPLOT: $Revision: 1.7 $ ; $Date: 89/09/20 19:36:52 $		*/

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
SUMAXPLOT - Graph of trace-by-trace maximum amplitude for a 		\n\
	    panel of SU data traces (with optional running average)	\n\
									\n\
sumaxplot <data [optional parameters] | tube				\n\
									\n\
Optional Parameters:							\n\
	m	= 1		running average length for amp plot	\n\
				( must be odd, positive integer )	\n\
...LABELING... 								\n\
	title   = null		plot title				\n\
	label1	= Max Amp 	time axis label				\n\
	label2	= Trace		trace axis label 			\n\
	xmin    = 1		tic label for first trace		\n\
	dx      = 1		tic labeling increment			\n\
	max   	= (from data)	max on amplitude graph			\n\
	nticx   = 4		# tics on trace axis 			\n\
	ntica   = 2		# tics on amplitude axis 		\n\
...MISCELLANEOUS ...							\n\
	plotfat	= 0		line thickness of plot			\n\
	axisfat	= 0		line thickness of box & tics		\n\
...SIZE & LOCATION ...							\n\
	titlsz = 4		title print size			\n\
	lablsz = 4		label print size			\n\
	ticsz  = 3		tic labeling print size			\n\
	sizex	= 8.0		length of x axis (inches)		\n\
	sizey	= 5.0		length of y axis (...)			\n\
	zerox	= 0.5		left of plot to left of screen (...)	\n\
	zeroy	= 1.0		base of plot to bottom of screen (...)	\n\
	hcopy	= 0		hardcopy sizing flag 			\n\
				= 1 change defaults for hardcopy	\n\
				( sizex=4.5 sizey=3.5 			\n\
				titlsz=3 lablsz=2 ticsz=2 )		\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Chris
 *
 * Caveats:
 *	The graph sub is very messy because user (x,y)
 *	is internal (t,x)...
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sumaxplot.c,v $";
static string revid =
	"   $Revision: 1.7 $ ; $Date: 89/09/20 19:36:52 $";



/* Set gain defaults (default scale to largest amp) */
#define TPOW	0.0
#define EPOW	0.0
#define GPOW	1.0
#define AGC 	0
#define WAGC	20
#define TRAP	0.0
#define CLIP	0.0
#define QCLIP	1.0
#define QBAL	1	
#define PBAL	0

segy tr;

main(argc, argv)
int argc; char **argv;
{
	float *data;		/* mega-vector to contain data set	*/
	float givenmax;		/* user given maximum for plotting	*/
	float gmax1;		/* global absolute max before gaining	*/
	float gmax2;		/* global absolute max after gaining	*/
	float *trmaxs;		/* collection of trace-by-trace maxs	*/
	float *tmpvec1;		/* temporary vector			*/
	float *tmpvec2;		/* temporary vector			*/
	float absmax[2];	/* holds results of maxmgv call		*/
	float scale = 1.0;	/* scale for gaining			*/
	float tmin, dt;		/* for gain sub 	*/
	float xmin, dx;		/* for gain sub 	*/
	string title; 
	string label1; 
	string label2;
	int m;			/* running average length for amp curve	*/
	int n, j;
	int itr;
	int ndata;		/* allocation parameter			*/
	int nt;			/* length of input traces		*/
	int ntsize;		/* ... in bytes				*/
	int ntr;		/* traces in input data			*/
	int tracl2;		/* traces in input data			*/
	void gain();		/* see su/lib/gainpkge.c		*/
	void subplotg();	/* isolate vplot commands		*/

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
	/* Read first trace */ 
	if (!gettr(&tr)) err("can't get first trace");

	/* Get number of time samples & calc some constants */	
	nt = tr.ns; 
	xmin = tr.tracl;	fgetpar("xmin", &xmin);
	ntsize = nt * FSIZE;

	tmin = tr.delrt/1000.0;	fgetpar("tmin", &tmin);
				/* tr.delrt is in millisecs */

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
	data = vec(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr == 2) tracl2 = tr.tracl;/* needed for dx    */
		if (ntr*nt > ndata) {		/* need more memory */
			ndata <<= 1;		/* ask for double   */
			data = re_vec(data, ndata);
		}
		bcopy(tr.data, data + (ntr - 1)*nt, ntsize); 
	} while(gettr(&tr));

	dx = tracl2 - xmin;	fgetpar("dx", &dx);


	/* Find max value in data before gain for labels */
	n = nt*ntr;
	maxmgv_(data, ONE, absmax, &n);
	gmax1 = absmax[0];


	/* Check if user has given a max */
	givenmax = 0.0; 	fgetpar("max", &givenmax);
	if (givenmax) {
		scale = gmax1 / givenmax;
		gmax1 = givenmax;
	} 


	/* Normalize data to unity for wgl1 subroutine */
	gain(data, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* Find global max value in data after gain (should be one) */
	maxmgv_(data, ONE, absmax, &n); 
	gmax2 = absmax[0]; 


	/* Alloc space for vector of trace maxima */
	trmaxs = vec(ntr);
	tmpvec1 = vec(nt);
	tmpvec2 = vec(ntr);


	/* Build a 'trace' of maximum value on each trace */
	for (itr = 0; itr < ntr ; itr++) { 
		bcopy(data + itr*nt, tmpvec1, ntsize); 
		maxmgv_(tmpvec1, ONE, absmax, &nt); 
		trmaxs[itr] = absmax[0]; 
	} 


	/* get running average parameter */
	if (!igetpar("m", &m))	m = 1;
	if ( m <= 0 ) err("m = %d ... must be odd, positive integer/n",m);

	/* Compute m-point running average of max amp curve */
	for (itr = 0; itr < ntr ; itr++) { 
	    if ( itr - (m+1)/2 + 1 >= 0 && itr + (m+1)/2 -1 < ntr ) {
	        for ( j = -(m+1)/2 + 1 ; j <= (m+1)/2 - 1 ; j++ ) {
		    tmpvec2[itr] = tmpvec2[itr] + trmaxs[itr + j] / m; 
	        } 
	    } else {
	        tmpvec2[itr] = trmaxs[itr]; 
	    } 
	} 



	/* Echo some info to user */
	if (givenmax) {
		warn("global data max = %g    given plot max = %g",
			scale*givenmax, givenmax);	
	} else {
		warn("global max = %g ", gmax1, gmax2);	
 	}


	/* Plot getpars */
	title = "";		sgetpar("title", &title);	
	label1 = "Max Amp";	sgetpar("label1", &label1);	
	label2 = "Trace";	sgetpar("label2", &label2);


	/* Graph plot */
	nt = ntr;
	ntr = 1;
	tmin = xmin;
	dt = dx;
	subplotg(tmpvec2, nt, ntr, title, label1, label2, gmax1, gmax2,  
								tmin, dt );   

	endplot ();


	return SUCCEED;
}

/* plot subroutine for amplitude graphing */
void subplotg(data,nt,nx,title,label1,label2,truemax,max,tmin,dt)
string title, label1, label2;
int nt,nx;
float tmin,dt,*data,truemax,max;
{
	int i,plotfat,axisfat,dtict,ntict,nticx;
	float sizet,sizex,scalet,scalex,zerot,zerox,margint,marginx,dticx;
	float truedticx;
	int titlsz,lablsz,ticsz,hcopy;
	char tval[8];		/* tic value string			*/

	zerot = 0.5;		fgetpar("zerox",&zerot);
	zerox = 1.0;		fgetpar("zeroy",&zerox);

	sizet = 8.0;		fgetpar("sizex",&sizet);
	sizex = 5.0;		fgetpar("sizet",&sizex);

	plotfat = 1;		igetpar("plotfat",&plotfat);
	axisfat = 0;		igetpar("axisfat",&axisfat);

	nticx = 2; 		igetpar("ntica",&nticx);
	ntict = 4;		igetpar("nticx",&ntict);

	margint = 0.01;		fgetpar("marginx",&margint);
	marginx = 0.04;		fgetpar("marginy",&marginx);

	titlsz = 4;		igetpar("titlsz",&titlsz);
	lablsz = 4;		igetpar("lablsz",&lablsz);
	ticsz = 3;		igetpar("ticsz",&ticsz);

	hcopy = 0;		igetpar("hcopy",&hcopy);
	if ( hcopy == 1 ) {
		sizet=4.5; 
		sizex=3.5;
		titlsz=3; 
		lablsz=2; 
		ticsz=2;
	}

	scalet = sizet/nt;	
	scalex = 0.5*sizex;
	setscl(scalet,scalex);
	set0(zerot, zerox + 0.5*sizex );
	setu0(0,0);
	setfat(axisfat);

	/* TITLE */
	setcol(1);  
	uText(0.5*nt, 1.0 + 1.0/titlsz, titlsz, 0, title);

	/* LABELS */
	setcol(8);  
	uText(-.3/scalet , 0.0, lablsz, 3, label1); 
	uText(0.5*nt, -1.0 - .35/scalex, lablsz, 0, label2);

	/* AXIS BOX AND ZERO LINE*/
	setcol(3);  
	umove( 0.0,        -1.0 );
	udraw( 0.0,         1.0 );
	udraw( (float) (nt-1) ,  1.0 );
	udraw( (float) (nt-1) , -1.0 );
	udraw( 0.0,        -1.0 );
	umove( 0.0,         0.0 );
	udraw( (float) (nt-1),   0.0 );

	/* Horiz tics */
	dtict = nt/ntict;
/* 	if (dtict > 10) dtict -= dtict%10;	 shuki */
	for (i = 0 ; i < nt ; i += dtict) {
	    if (dt > 1) {
		sprintf(tval, "%g", (float) tmin + i*dt);
	    } else {
		sprintf(tval, "%.3g", (float) tmin + i*dt);
	    }
	    uText( (float)i , -1.0 - 2.0*marginx, ticsz, 0, tval );
	    umove( (float)i , -1.0 - marginx );
	    udraw( (float)i , -1.0           );
	    umove( (float)i ,  1.0           );
	    udraw( (float)i ,  1.0 + marginx );
	}

	/* Amplitude tics */
	dticx = max/nticx;
	truedticx = truemax/nticx;

	for ( i = -nticx ; i <= nticx ; i++ ) {
		umove( (float) (nt-1) , i*dticx  / max );
		udraw( (1.0 + margint) * nt, i*dticx / max );
		sprintf( tval, "%.3g", i*truedticx );
		utext( (1.0 + 2*margint) * nt, i*dticx / max , ticsz, 0, tval);
	}


	/* DRAW N2 WIGGLE TRACES */
	for ( i = 0; i < nx; i++ ) {
		setfat(plotfat);
	 	setcol(2); 
		wgl1( data + nt*i, nt ); 
	}
}

wgl1(f,n)
float *f;
int n;
{
	int i;

	umove(0.0,f[0]);
	for(i=1;i<n;i++) {
		udraw((float)i,f[i]);
	}
}
