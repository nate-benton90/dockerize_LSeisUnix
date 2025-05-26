/* SUZOEP: $Revision: 1.3 $ ; $Date: 89/05/25 23:11:56 $		*/

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

#include "../include/cwp.h"
#include "../include/fconst.h"

/*************************** self documentation **************************/
string sdoc = "\
									\n\
SUZOEP - graph/list elastic reflection coefficients			\n\
									\n\
suzoep [optional parameters] | pen					\n\
suzoep out=1 [optional parameters] >& file				\n\
									\n\
Optional parameters:							\n\
	out = 0		output is cplot meta-code file for plotting	\n\
		 	1 = output ascii values of amp and phase	\n\
	coef = 1	specifies coefficient to be displayed		\n\
		 	1 = p reflection coeff				\n\
		 	2 = s reflection coeff				\n\
		 	3 = p transmission coeff			\n\
		 	4 = s transmission coeff			\n\
	case = 1	specifies incident wave and type of interface	\n\
			(layer 1 is incident medium)			\n\
		 	1 = p-wave incident on solid/solid interface 	\n\
		        2 = sv-wave incident on solid/solid interface   \n\
		        3 = sh-wave incident on solid/solid interface   \n\
		        4 = p-wave incident on free surface     	\n\
		        5 = sv-wave incident on free surface    	\n\
   MODEL...								\n\
	p1 = 1000	p-wave speed in layer 1 			\n\
	s1 = 500	s-wave speed in layer 1				\n\
	r1 = 2.33	density in layer 1				\n\
	p2 = 2000	p-wave speed in layer 2				\n\
	s2 = 1000	s-wave speed in layer 2				\n\
	r2 = 2.67	density in layer 2				\n\
   LABELING ...								\n\
	titlsz = 4		title print size			\n\
	lablsz = 4		label print size			\n\
	ticsz  = 3		tic labeling print size			\n\
   SIZE & LOCATION ...							\n\
	sizex	= 7.5		length of x axis (inches)		\n\
	sizey	= 4.0		length of y axis (...)			\n\
	zerox	= 1.8		left of plot to left of screen (...)	\n\
	zeroy	= 2.0		base of plot to bottom of screen (...)	\n\
	hcopy	= 0		hardcopy sizing flag 			\n\
				= 1 change defaults for hardcopy	\n\
									\n\
";
/*************************************************************************/

/* Credits:
 *	CWP: Chris
 *
 * suzoep plots or lists the isotropic elastic angular refl/trans
 * coefficients (for displ/vel/accel which are equal) for a plane
 * wave (p,sv, or sh) incident on a plane interface (solid/solid
 * or free surface)
 *
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

#define PWAVE	1
#define SVWAVE	2
#define SHWAVE	3
#define PWAVEFS	4
#define SVWAVEFS 5

#define PREFL	1
#define SREFL	2
#define PTRANS	3
#define STRANS	4


main(argc, argv)
int argc; char **argv;
{
	float alpha1,alpha2;
	float beta1,beta2;
	float rho1,rho2;

/*
*	rpa[181], rsa[181] .... first 180 elements of ##a are amplitude
*	values at .5 degree increments, with 181 element being the max  
*
*	rpp[181], rsp[181] .... first 180 elements of ##p are phase 
*	values at .5 degree increments, with 181 element being the max  
*/

	float rpa[181],rpp[181],rsa[181],rsp[181];
	float tpa[181],tpp[181],tsa[181],tsp[181];
	int whichcase,out;
	int whichcoef,i;
	float *amp,*phase;

	float *data;		/* mega-vector to contain data set	*/
	float givenmax;		/* user given maximum for plotting	*/
	float gmax1;		/* global absolute max before gaining	*/
	float gmax2;		/* global absolute max after gaining	*/
	float absmax[2];	/* holds results of maxmgv call		*/
	float scale = 1.0;	/* scale for gaining			*/
	float tmin, dt;		/* for gain sub 	*/
	char *title; 
	char *label1; 
	char *label2;
	int n,nt;		/* length of input traces		*/
	int ntbytes;		/* ... in bytes				*/
	int ntr;		/* traces in input data			*/
	void gain();

	initgetpar(argc, argv); askdoc(0);

	/* Must get the coefficient of interest */
	whichcoef = 1;			igetpar("coef", &whichcoef);
	out = 0;			igetpar("out", &out);

	/* Get the case of interest */
	whichcase=PWAVE;		igetpar("case", &whichcase);

	/* Get model parameters */
	alpha1=1000;			fgetpar("p1", &alpha1);
	alpha2=2000;			fgetpar("p2", &alpha2);
	beta1=500;			fgetpar("s1", &beta1);
	beta2=1000;			fgetpar("s2", &beta2);
	rho1=2.33;			fgetpar("r1", &rho1);
	rho2=2.67;			fgetpar("r2", &rho2);

	/* Calc the coeffs in fortran subs */
	switch ( whichcase ) {
		case PWAVE: 
			inmodp_(&alpha1,&alpha2,&beta1,&beta2,&rho1,&rho2,  
				rpa,rpp,rsa,rsp,tpa,tpp,tsa,tsp);  

			/* Set title */
			if ( whichcoef == PREFL ) {  
		title = "P onto solid interface -- P refl. coeff.";
			} else if ( whichcoef == SREFL ) {
		title = "P onto solid interface -- S refl. coeff.";
			} else if ( whichcoef == PTRANS ) { 
		title = "P onto solid interface -- P trans. coeff.";
			} else if ( whichcoef == STRANS ) { 
		title = "P onto solid interface -- S trans. coeff.";
			}

			 break;
		case SVWAVE:
			inmodsv_(&alpha1,&alpha2,&beta1,&beta2,&rho1,&rho2, 
				rpa,rpp,rsa,rsp,tpa,tpp,tsa,tsp); 

			/* Set title */
			if ( whichcoef == PREFL ) {  
		title = "SV onto solid interface -- P refl. coeff.";
			} else if ( whichcoef == SREFL ) {
		title = "SV onto solid interface -- S refl. coeff.";
			} else if ( whichcoef == PTRANS ) { 
		title = "SV onto solid interface -- P trans. coeff.";
			} else if ( whichcoef == STRANS ) { 
		title = "SV onto solid interface -- S trans. coeff.";
			}

			break;
		case SHWAVE:
			inmodsh_(&alpha1,&alpha2,&beta1,&beta2,&rho1,&rho2, 
				rsa,rsp,tsa,tsp); 

			/* Set title */
			if ( whichcoef == PREFL ) {  
		title = "SH onto solid interface -- P refl. coeff.";
			} else if ( whichcoef == SREFL ) {
		title = "SH onto solid interface -- S refl. coeff.";
			} else if ( whichcoef == PTRANS ) { 
		title = "SH onto solid interface -- P trans. coeff.";
			} else if ( whichcoef == STRANS ) { 
		title = "SH onto solid interface -- S trans. coeff.";
			}

			break;
		case PWAVEFS:
			inmodpfs_(&alpha1,&beta1,&rho1,rpa,rpp,rsa,rsp); 

			/* Set title */
			if ( whichcoef == PREFL ) {  
		title = "P onto free surface -- P refl. coeff.";
			} else if ( whichcoef == SREFL ) {
		title = "P onto free surface -- S refl. coeff.";
			} else if ( whichcoef == PTRANS ) { 
		title = "P onto free surface -- P trans. coeff.";
			} else if ( whichcoef == STRANS ) { 
		title = "P onto free surface -- S trans. coeff.";
			}

			break;
		case SVWAVEFS:
			inmodsvfs_(&alpha1,&beta1,&rho1,rpa,rpp,rsa,rsp); 

			/* Set title */
			if ( whichcoef == PREFL ) {  
		title = "SV onto free surface -- P refl. coeff.";
			} else if ( whichcoef == SREFL ) {
		title = "SV onto free surface -- S refl. coeff.";
			} else if ( whichcoef == PTRANS ) { 
		title = "SV onto free surface -- P trans. coeff.";
			} else if ( whichcoef == STRANS ) { 
		title = "SV onto free surface -- S trans. coeff.";
			}

			break;
		default:	/* defensive programming */
			err("mysterious case %d", whichcase);
	}


	/*  Set some constants  */	
	nt = 180; 
	ntr = 1; 

	/* Allocate block of memory for data mega-vector */
	amp = (float*) malloc( (uint) nt * sizeof( float ) );
	phase = (float*) malloc( (uint) nt * sizeof( float ) );

	/* Load specific amp and phase into generic vectors */
	switch ( whichcoef ) {
		case PREFL: 

			for ( i = 0; i <= nt; i++ ) {
				amp[i] = rpa[i];
				phase[i] = rpp[i];
			}

			 break;

		case SREFL:

			for ( i = 0; i <= nt; i++ ) {
				amp[i] = rsa[i];
				phase[i] = rsp[i];
			}

			break;


		case PTRANS:

			for ( i = 0; i <= nt; i++ ) {
				amp[i] = tpa[i];
				phase[i] = tpp[i];
			}

			 break;


		case STRANS:

			for ( i = 0; i <= nt; i++ ) {
				amp[i] = tsa[i];
				phase[i] = tsp[i];
			}

			 break;

		default:	/* defensive programming */

			err("mysterious coef %d", whichcoef);
	}

	/* check if printed output is desired */
	if ( out == 1 ) {

		/* print title and model */
		fprintf(stderr,"%s\n",title);
		sprintf( title, "Model: p1=%g s1=%g r1=%g   p2=%g s2=%g r2=%g", 
			alpha1,beta1,rho1,alpha2,beta2,rho2); 
		fprintf(stderr,"%s\n",title);

		/* print inc ang, amp and phase */
		for ( i = 0; i <= nt; i++ ) {
			if ( i%2 == 0 ) { 
		fprintf(stderr,"inc. ang. = %g     amp = %g     phase = %g\n",
				(float) i*.5,amp[i],phase[i]*57.29);
			}
		}

		/* done */
		exit(0);
	}
		


	/* Plot getpars */
	label1 = "Amplitude";
	label2 = "Incident Angle";

	/* find max value in data before gain for labels*/
	n = nt;
	maxmgv_(amp, ONE, absmax, &n);
	gmax1 = absmax[0];

	/* normalize amp to unity for wgl1 subroutine */
	gain(amp, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* find max value in amp after gain (should be one) */
	maxmgv_(amp, ONE, absmax, &n); 
	gmax2 = absmax[0]; 
	if ( gmax2 == 0.0 ) gmax2 = 1.0;

	/* Plot box, amp curve, amp label & tics and angle label & tics */
	plotamp(amp, nt, ntr, title, label1, label2, gmax1, gmax2 );  




	/* Plot getpars */
	label1 = "Phase";

	/* find max value in data before gain for labels*/
	n = nt;
	gmax1 =180.0; 

	/* normalize amp to unity for wgl1 subroutine */
	gain(phase, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, scale, tmin, dt, WAGC, nt, ntr);

	/* find max value in amp after gain (should be one) */
	maxmgv_(phase, ONE, absmax, &n); 
	gmax2 = absmax[0]; 
	if ( gmax2 == 0.0 ) gmax2 = 1.0;

	/* Plot phase curve, label & tics */
	plotphase(phase, nt, ntr, title, label1, label2, gmax1, gmax2,
			alpha1, alpha2, beta1, beta2, rho1, rho2 );  

	endplot ();
	exit(0);
}

plotamp(data,nt,nx,title,label1,label2,truemax,max)
/*	BEWARE: this subroutine was scavanged from graph.c and
	to maintain unformity from the user interface ...
	the users x direction is internally the t direction;
	the users y direction is internally the x direction ...
	Clearly, this needs to be fixed at some time.     chris 8/88
*/
char *title,*label1,*label2;
int nt,nx;
float *data,truemax,max;
{
	int i,plotfat,axisfat,dtict,ntict,nticx;
	float sizet,sizex,scalet,scalex,zerot,zerox,margint,marginx,dticx;
	float dash,gap,truedticx,xmin,dx;
	int titlsz,lablsz,ticsz,hcopy,dashflag;

	zerot = 1.8;		fgetpar("zerox",&zerot);
	zerox = 2.0;		fgetpar("zeroy",&zerox);

	sizet = 7.5;		fgetpar("sizex",&sizet);
	sizex = 4.0;		fgetpar("sizey",&sizex);

	xmin = 0.0;		fgetpar("xmin",&xmin);
	dx = .5;		fgetpar("dx",&dx);

	plotfat = 0;		fgetpar("plotfat",&plotfat);
	axisfat = 0;		fgetpar("axisfat",&axisfat);

	dashflag = 0; 		igetpar("dash",&dashflag);

	nticx = 4; 		igetpar("nticy",&nticx);
	ntict = 7;		fgetpar("nticx",&ntict);

	margint = 0.01;		fgetpar("marginx",&margint);
	marginx = 0.04;		fgetpar("marginy",&marginx);

	titlsz = 4;		igetpar("titlsz",&titlsz);
	lablsz = 4;		igetpar("lablsz",&lablsz);
	ticsz = 3;		igetpar("ticsz",&ticsz);

	hcopy = 0;		igetpar("hcopy",&hcopy);
	if ( hcopy == 1 ) {
   		sizet=4.5;
		sizex=2;
		zerox=4; 
		zerot=.8; 
		titlsz=3;
		lablsz=2;
		ticsz=2;
	}

	scalet = sizet/nt;	
	scalex = 0.5*sizex;
	setscl(scalet,scalex);
	set0(zerot, zerox + 0.5*sizex );

	/* TITLE */
	setcol(1);  
	uText( 0.5*nt, 1.0 + 1.0/titlsz, titlsz, 0, title );

	/* amplitude label */
	setcol(2);  
	uText(-23.0, 0.0, lablsz, 3, label1); 

	/* incident angle label */
	setcol(1);  
	uText(0.5*nt, -1.3, lablsz, 0, label2);

	/* AXIS BOX AND ZERO LINE*/
	setcol(1);  
	setfat(1);  
	umove( 0.0,        -1.0 );
	udraw( 0.0,         1.0 );
	udraw( (float)nt ,  1.0 );
	udraw( (float)nt , -1.0 );
	udraw( 0.0,        -1.0 );

	/* Angle tics */
	dtict = nt/ntict;
	if (dtict>10) dtict -= dtict%10;	/* shuki */
	for ( i = 0 ; i <= nt ; i += dtict ) {
	    sprintf(title,"%g", xmin + (float)i*dx);
	    uText( (float)i , -1.1, ticsz, 0, title );
	    umove( (float)i , -1.0 - marginx );
	    udraw( (float)i , -1.0           );
	    umove( (float)i ,  1.0           );
	    udraw( (float)i ,  1.0 + marginx );
	}

	/* Amplitude tics */
	setcol(1);
	dticx = max/nticx;
	truedticx = truemax/nticx;

	for ( i = -nticx ; i <= nticx ; i++ ) {
		umove( 0.0, i*dticx  / max );
		udraw( -2.0, i*dticx / max );
		sprintf( title, "%3.2f", i*truedticx );
		utext( -16.0, i*dticx / max , ticsz, 0, title);

	}


	/* draw amplitude curve */
	for ( i = 0; i < nx; i++ ) {
	 	setcol(2); 
	 	setfat(2); 
		wgl1( data + nt*i, nt, HUGE ); 
	}
}

/*
#define EPS 0.00001
	black -= EPS;	fgetpar("black",&black);
	zblack = (1.-2.*black)*overlap;
		wgl1(sytr[i].data,nt,zblack);
*/
wgl1(f,n,zblack)
float *f,zblack;
{
	int i,lp=0;
	static first=1;
	static float *xp,*yp;
	char *malloc();

	if(first) {
		xp = (float*) malloc(4*(n+2));
		yp = (float*) malloc(4*(n+2));
		first = 0;
	}
	if(zblack<=f[0] && zblack<f[1]) {
		xp[0] = 0.0;
		yp[0] = zblack;
		xp[1] = 0.0;
		yp[1] = f[0];
		lp = 2;
	}
	umove(0.0,f[0]);
	for(i=1;i<n;i++)
	{
		udraw((float)i,f[i]);
		if(f[i]>zblack) {
			if(f[i-1]<=zblack) {
				xp[0] = (zblack-f[i])/(f[i]-f[i-1]) + i;
				yp[0] = zblack;
				lp = 1;
			}
			xp[lp] = i;
			yp[lp] = f[i];
			lp++;
		}
		if(f[i]<zblack&&f[i-1]>zblack) {
			xp[lp] = (zblack-f[i])/(f[i]-f[i-1]) + i;
			yp[lp] = zblack;
			lp++;
			if(lp>2) uarea(xp,yp,lp,0,1,1);
			lp = 0;
		} else if (i==n-1 && lp) {
			xp[lp] = i;
			yp[lp] = zblack;
			lp++;
			if(lp>2) uarea(xp,yp,lp,0,1,1);
		}
	}
}

plotphase(data,nt,nx,title,label1,label2,truemax,max,
			alpha1, alpha2, beta1, beta2, rho1, rho2)
/*	BEWARE: this subroutine was scavanged from graph.c and
	to maintain unformity from the user interface ...
	the users x direction is internally the t direction;
	the users y direction is internally the x direction ...
	Clearly, this needs to be fixed at some time.     chris 8/88
*/
char *title,*label1,*label2;
int nt,nx;
float *data,truemax,max;
float alpha1,alpha2;
float beta1,beta2;
float rho1,rho2;
{
	int i,plotfat,axisfat,dtict,ntict,nticx;
	float sizet,sizex,scalet,scalex,zerot,zerox,margint,marginx,dticx;
	float dash,gap,truedticx,xmin,dx;
	int titlsz,lablsz,ticsz,hcopy,dashflag;

	zerot = 1.8;		fgetpar("zerox",&zerot);
	zerox = 2.0;		fgetpar("zeroy",&zerox);

	sizet = 7.5;		fgetpar("sizex",&sizet);
	sizex = 4.0;		fgetpar("sizey",&sizex);

	xmin = 0.0;		fgetpar("xmin",&xmin);
	dx = .5;		fgetpar("dx",&dx);

	plotfat = 0;		fgetpar("plotfat",&plotfat);
	axisfat = 0;		fgetpar("axisfat",&axisfat);

	dashflag = 0; 		igetpar("dash",&dashflag);

	nticx = 4; 		igetpar("nticy",&nticx);
	ntict = 7;		fgetpar("nticx",&ntict);

	margint = 0.01;		fgetpar("marginx",&margint);
	marginx = 0.04;		fgetpar("marginy",&marginx);

	titlsz = 4;		igetpar("titlsz",&titlsz);
	lablsz = 4;		igetpar("lablsz",&lablsz);
	ticsz = 3;		igetpar("ticsz",&ticsz);

	hcopy = 0;		igetpar("hcopy",&hcopy);
	if ( hcopy == 1 ) {
   		sizet=4.5;
		sizex=2;
		zerox = 4.0; 
		zerot=.8; 
		titlsz=3;
		lablsz=2;
		ticsz=2;
	}

	scalet = sizet/nt;	
	scalex = 0.5*sizex;
	setscl(scalet,scalex);
	set0(zerot, zerox + 0.5*sizex );

	/* Phase Label */
	setcol(3);  
	uText( nt + 20.0, 0.0, lablsz, 3, label1); 

	/* Phase tics */
	setcol(1);  
	dticx = max/nticx;
	truedticx = truemax/nticx;

	for ( i = -nticx ; i <= nticx ; i++ ) {
		umove( (float) nt , i*dticx  / max ); 
		udraw( (float) nt + 2.0, i*dticx / max ); 
		sprintf( title, "%g", i*truedticx ); 
		utext( nt + 6.0, i*dticx / max , ticsz, 0, title); 
	}


	/* DRAW PHASE AS WIGGLE TRACE */
	for ( i = 0; i < nx; i++ ) {
		dash = .1;
		gap = dash;
		setdash(dash,gap,dash,gap);
	 	setcol(3); 
	 	setfat(2); 
		wgl1( data + nt*i, nt, HUGE ); 
	}

	/* write model below plot */
	setcol(1);
	sprintf( title, "Model: p1=%g s1=%g r1=%g   p2=%g s2=%g r2=%g", 
			alpha1,beta1,rho1,alpha2,beta2,rho2); 
	uText(0.5*nt, -1.6, lablsz, 0, title);
}
