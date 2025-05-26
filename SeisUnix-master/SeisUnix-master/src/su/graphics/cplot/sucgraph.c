/* SUCGRAPH: $Revision: 1.7 $ ; $Date: 90/10/19 10:33:25 $		*/
 
/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUCGRAPH - amplitude graph of su data traces				\n\
									\n\
sucgraph <data [optional parameters] | pen_program			\n\
									\n\
Optional Parameters:							\n\
...LABELING ...								\n\
	title = null		plot title				\n\
	label1 = Amplitude 	vertical (y) axis label			\n\
	label2 = Sample Number	horizontal (x) axis label		\n\
	titlsz = 4		title print size			\n\
	lablsz = 4		label print size			\n\
	ticsz  = 3		tic labeling print size			\n\
	xmin   = 1		tic label for first sample		\n\
	dx     = 1		tic labeling increment			\n\
...MISCELLANEOUS ...							\n\
	max   	= (from data)	max plotting amplitude			\n\
	zl   	= 1		draw reference line through amp=0	\n\
				(=0; no zero line )			\n\
	dash   	= 0		flag for no dashed lines in plot	\n\
				= 1  to cycle thru 6 dash settings	\n\
	nticy   = 2		number of tics on y axis 		\n\
	nticx   = 4		number of tics on x axis		\n\
	plotfat	= 0		line thickness of plot			\n\
	axisfat	= 0		line thickness of box & tics		\n\
...SIZE & LOCATION ...							\n\
	sizex	= 8.0		length of x axis (inches)		\n\
	sizey	= 5.0		length of y axis (...)			\n\
	zerox	= 0.5		left of plot to left of screen (...)	\n\
	zeroy	= 1.0		base of plot to bottom of screen (...)	\n\
	hcopy	= 0		hardcopy sizing flag 			\n\
				= 1 change defaults for hardcopy	\n\
				sizex=4.5 sizey=3.5 			\n\
				titlsz=3 lablsz=2 ticsz=2		\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Chris
 *	SEP: Shuki
 *
 */


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

main(int argc, char **argv)
{
	float *data;		/* mega-vector to contain data set	*/
	float givenmax;		/* user given maximum for plotting	*/
	float gmax1;		/* global absolute max before gaining	*/
	float gmax2;		/* global absolute max after gaining	*/
	float scale = 1.0;	/* scale for gaining			*/
	float tmin, dt;		/* for gain sub 	*/
	string title; 
	string label1; 
	string label2;
	int nfloats;		/* number of floats in data set		*/
	int nt;			/* length of input traces		*/
	int ntsize;		/* ... in bytes				*/
	int ndata;		/* allocation parameter			*/
	int ntr;		/* traces in input data			*/
	register int i;		/* counter				*/
	void subplot(		/* wiggle plot subroutine		*/
	float *data,
	int nt,
	int nx,
	string title,
	string label1,
	string label2,
	float truemax,
	float max);


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	/* Read first trace */ 
	if (!gettr(&tr)) err("can't get first trace\n");

	/* Get number of time samples & calc some constants */	
	nt = tr.ns; 
	ntsize = nt * FSIZE;


	/* Allocate block of memory for data mega-vector */
	ndata = MAX(NFALLOC, nt); /* alloc at least one trace */
	data = ealloc1float(ndata);


	/* Loop over input traces & put them into data mega-vector */
	ntr = 0;
	do {
		++ntr;
		if (ntr*nt > ndata) {	/* need more memory */
			ndata += NFALLOC;
			data = erealloc1float(data, ndata);
		}
		bcopy(tr.data, data + (ntr - 1)*nt, ntsize); 
	} while (gettr(&tr));


	/* Plot getpars */
	if (!sgetpar("title",  &title))		title = "";
	if (!sgetpar("label1", &label1))	label1 = "Amplitude";
	if (!sgetpar("label2", &label2))	label2 = "Sample Number";

	/* find max value in data before gain for labels */
	nfloats = nt*ntr;
	gmax1 =  ABS(data[0]);
	for (i = 1; i < nfloats; ++i)
		gmax1 = MAX(gmax1, ABS(data[i]));

	/* check if user has given a max*/
	if (!fgetpar("max",&givenmax))	givenmax = 0.0;
	if ( givenmax ) {
		scale = gmax1 / givenmax;
		gmax1 = givenmax;
	} 

	/* normalize data to unity for wgl1 subroutine */
	gain(data, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* find max value in data after gain (should be one) */
	gmax2 =  ABS(data[0]);
	for (i = 1; i < nfloats; ++i)
		gmax2 = MAX(gmax2, ABS(data[i]));

	/* echo some info to user */
	warn("nt=%d   ntr=%d",nt,ntr);	
	if ( givenmax ) {
		warn("global data max = %g    given plot max = %g",
			scale*givenmax,givenmax);	
	} else {
		warn("global max = %g ",gmax1,gmax2);	
	}

	/* CALL THE PLOTTING PROGRAM AS A SUBROUTINE */
	subplot(data, nt, ntr, title, label1, label2, gmax1, gmax2 );  

	endplot ();
	

	return EXIT_SUCCESS;
}



/*	BEWARE: this subroutine was scavanged from graph.c and
	to maintain unformity from the user interface ...
	the users x direction is internally the t direction;
	the users y direction is internally the x direction ...
	Clearly, this needs to be fixed at some time.     chris 8/88
*/
void subplot(
	float *data,
	int nt,
	int nx,
	string title,
	string label1,
	string label2,
	float truemax,
	float max)
{
	char tval[8];		/* time value string			*/
	char xval[8];		/* x value string			*/
	int i,plotfat,axisfat,dtict,ntict,nticx,zl;
	float sizet,sizex,scalet,scalex,zerot,zerox,margint,marginx,dticx;
	float dash,gap,truedticx,xmin,dx;
	int titlsz,lablsz,ticsz,hcopy,dashflag;

	zerot = 0.5;		fgetpar("zerox",&zerot);
	zerox = 1.0;		fgetpar("zeroy",&zerox);

	sizet = 8.0;		fgetpar("sizex",&sizet);
	sizex = 5.0;		fgetpar("sizey",&sizex);

	xmin = 1.0;		fgetpar("xmin",&xmin);
	dx = 1.0;		fgetpar("dx",&dx);

	plotfat = 0;		igetpar("plotfat",&plotfat);
	axisfat = 0;		igetpar("axisfat",&axisfat);

	dashflag = 0; 		igetpar("dash",&dashflag);

	zl = 1;			igetpar("zl",&zl);

	nticx = 2; 		igetpar("nticy",&nticx);
	ntict = 5;		igetpar("nticx",&ntict);

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
	setfat(axisfat);

	/* TITLE */
	setcol(1);  
	uText( 0.5*nt, 1.0 + 1.0/titlsz, titlsz, 0, title );

	/* LABELS */
	setcol(8);  
	uText(-.3/scalet , 0.0, lablsz, 3, label1); 
	uText(0.5*nt, -1.0 - .6/scalex, lablsz, 0, label2);

	/* draw axis box*/
	setcol(3);  
	umove( 0.0,        -1.0 );
	udraw( 0.0,         1.0 );
	udraw( (float)nt ,  1.0 );
	udraw( (float)nt , -1.0 );
	udraw( 0.0,        -1.0 );


	/* draw zero line */
	umove( 0.0,         0.0 );
	if ( zl == 1 )  udraw( (float)nt,   0.0 );

	/* Time tics */
	dtict = nt/ntict;
	if (dtict>10) dtict -= dtict%10;	/* shuki */
	for ( i = 0 ; i <= nt ; i += dtict ) {
	    sprintf(tval,"%g", xmin + (float)i*dx);
	    uText( (float)i , -1.0 - 3.0*marginx, ticsz, 0, tval);
	    umove( (float)i , -1.0 - marginx );
	    udraw( (float)i , -1.0           );
	    umove( (float)i ,  1.0           );
	    udraw( (float)i ,  1.0 + marginx );
	}

	/* Amplitude tics */
	dticx = max/nticx;
	truedticx = truemax/nticx;

	for ( i = -nticx ; i <= nticx ; i++ ) {
		umove( (float)nt , i*dticx  / max );
		udraw( (1.0 + margint) * nt, i*dticx / max );
		sprintf( xval, "%.3g", i*truedticx );
		utext( (1.0 + 2*margint) * nt, i*dticx / max , ticsz, 0, xval);
	}


	/* DRAW N2 WIGGLE TRACES */
	for ( i = 0; i < nx; i++ ) {
		if ( dashflag ) {
			dash = ( i % 6 ) * .05;
			gap = dash;
			if ( dash != 0.0 ) {
				setdash(dash,gap,dash,gap);
			}
		}
	 	setcol(6 - i % 6); 
		wgl1( data + nt*i, nt );  
	}
	return;
}


wgl1(float *f, int n)
{
	int i, lp=0;
	static bool first = true;
	static float *xp, *yp;

	if (first) {
		xp = ealloc1float(n+2);
		yp = ealloc1float(n+2);
		first = false;
	}
	umove(0.0,f[0]);
	for(i=1;i<n;i++) {
		udraw((float)i,f[i]);
	}
}
