/* SUCONTOUR: $Revision: 2.18 $ ; $Date: 89/09/20 19:35:03 $		*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUCONTOUR - contouring program for data plotting 			\n\
									\n\
sucontour <stdin [optional parameters] | tube				\n\
									\n\
Optional Parameters:							\n\
....LABELING ...							\n\
	title	= null		plot title				\n\
	ylabel	= Y axis 	vertical axis label			\n\
	xlabel	= X axis	horizontal axis label 			\n\
	titlsz	= 4		title print size			\n\
	lablsz	= 4		axis label print size			\n\
	ticsz	= 3		tic label print size			\n\
   	nytic 	= 5		number of tics on x-axis		\n\
   	nxtic 	= 5		number of tics on x-axis		\n\
	ymin	= 0.		minumum y value for tics		\n\
	dy	= 1.		y increment for tics (0 = no tic labels)\n\
	xmin	= 0.		minumum x value for tics		\n\
	dx	= 1.		x increment for tics(0 = no tic labels)	\n\
....MISCELLANEOUS ...							\n\
   	grid 	= tics		tic marks only (=lines for grid lines)	\n\
   	dash 	= 0		all contours are solid lines		\n\
				=1 to cycle thru dashed contours	\n\
   	c0 	= min 	 	minimum contour value			\n\
   	nc 	= 5	 	number of contours			\n\
   	cntrcol = all 		all contours one color (1,2,...6)	\n\
....SIZE & LOCATION ...							\n\
	zeroy	= 1.0		base of plot to bot. of screen		\n\
	zerox	=  .6		left of plot to left of screen		\n\
   	sizey 	= 6. 		vertical box size (inches)		\n\
   	sizex 	= 4.5		horizontal box size (inches)		\n\
									\n\
Gain parameters- default is quantile balance (qbal = 1) 		\n\
									\n\
	out(t) = scale*BAL*CLIP{AGC{(t**tpow*exp(epow*t)*in(t))**gpow}}	\n\
	 	tpow=0.0 epow=0.0 gpow=1.0 agc=0 wagc=20		\n\
		trap=0.0 clip=0.0 qclip=1.0 qbal=1 pbal=0 scale=1.0	\n\
See self-doc for sugain for more information on gain parameters 	\n\
";
/*************************************************************************/

/* Credits:
 *	SEP: Dave, Stew, Jon
 *	CWP: Chris
 *
 * Caveat:
 *	still needs documentation and cleanup in subplot
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sucontour.c,v $";
static string revid =
	"   $Revision: 2.18 $ ; $Date: 89/09/20 19:35:03 $";




#define TPOW	0.0
#define EPOW	0.0
#define GPOW	1.0
#define AGC 	0
#define WAGC	20
#define TRAP	0.0
#define CLIP	0.0
#define QCLIP	1.0	/* used in qbal					*/
#define QBAL	1	/* default is balance by maximum magnitude 	*/
#define PBAL	0
#define SCALE	1.0	

segy tr;


main(argc, argv)
int argc; char **argv;
{
	float *dataptr;		/* mega-vector of data from the segys	*/
	float ymin;		/* first value on trace (from tr.delrt)	*/
	float dy;		/* sampling rate on 'trace' (from tr.dt)*/
	int ny;			/* samples per 'trace' (from tr.ns)	*/
	int nx;			/* traces in input data (from gettr)	*/
	int xmin;		/* first trace number from tr.tracl	*/
	int nybytes;		/* number of data bytes on a trace	*/
	int nydata;		/* floats allocated for mega-vector	*/
	string title;		/* title on plot			*/
	string ylabel;		/* vertical axis label 			*/
	string xlabel;		/* horizontal axis label		*/
	void subplot();		/* isolate vplot commands		*/
	void gain();		/* see su/lib/gainpkge.c		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Prevent bytes from spilling on screen */
	if (isatty(STDOUT)) {
		err("must redirect or pipe byte code output");
	}


	/* Get info from first trace */ 
	if (!gettr(&tr)) err("can't get first trace");
	ny = tr.ns;
	xmin = tr.tracl;
	nybytes = ny * FSIZE;

	/* Alloc block of memory for data */
	/* Allocate block of memory for data float mega-vector */
	nydata = MAX(NFALLOC, ny); /* alloc at least one trace */
	dataptr = vec(nydata);


	/* Loop over input traces & put them into data mega-vector */
	nx = 0;
	do {
		++nx;
		if (nx*ny > nydata) { /* need more memory */	
			nydata <<= 1; /* ask for double   */
			dataptr = re_vec(dataptr, nydata);
		}
		bcopy(tr.data, dataptr + (nx - 1)*ny, nybytes); 
	} while (gettr(&tr));


	/* Set ymin and dy temporarily for gain */
	ymin = tr.delrt/1000.0;	/* convert from millisecs */
	dy = tr.dt/1000000.;	/* convert from microseconds */

	/* Gain */
	gain(dataptr, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, SCALE, ymin, dy, WAGC, ny, nx);

	/* Plot getpars */
	if (!sgetpar("title",  &title))		title = "";
	if (!sgetpar("ylabel", &ylabel))	ylabel = "Y axis";
	if (!sgetpar("xlabel", &xlabel))	xlabel = "X axis";


	/* Call the plotting program as a subroutine */
	warn("ny = %d  nx = %d", ny, nx);	
	subplot(dataptr, ny, nx, 1, title, ylabel, xlabel);

	endplot();


	return SUCCEED;
}


/* 
Technical reference:  Cottafava,G. and Le Moli,G., 1969, Automatic contour map:
	Communications of the ACM, volume 12, number 7, July, 1969.
Dave Hale, 12/05/82
Modified 2/18/83  S. Levin  changed to seplib;  made colors and fatness repeat.
			    also made changes to be more consistent with lint.
			    Low order floating point bit operations are still
			    inherently non-portable.
Modified 2/28/83  S. Levin  changed Cplotlib calls to honor Hale's 2/24/83
			    revisions.
Modified 2/28/83  Jon	    recognizes n1 as a synonym for nx
Modified 3/15/83  Jon	    but n1 takes precedence over nx
Modified 9/25/85  Stewart A. Levin  Low-order VAX bit flagging revision for convex
*/

/* why are these out here ??? */
#define MAXNC 200
int   cntrcol[MAXNC], cntrfat[MAXNC];
float    c[MAXNC];

#define GARBAGE -777
void subplot(dataptr,ny,nx,n3,title,ylabel,xlabel)
int ny, nx, n3;
float *dataptr;
string title, ylabel, xlabel;
{
	int axiscol;		/*					*/
	int axisfat;		/*					*/
	int dashflag;		/* flag for dashing contours		*/
	int ic;			/*					*/
	int ix;			/*					*/
   	int lablsz;		/* letter size for axis labels		*/
	int lines;		/* flag for grid lines			*/
   	int minusone;		/* -1					*/
	int nc;			/* number of contours			*/
	int ntemp;		/*					*/
   	int nytic;		/* number of tics on y-axis		*/
	int nxtic;		/* number of tics on x-axis		*/
   	int one;		/* +1					*/
   	int ticsz;		/* letter size for tic labels		*/
	int titlsz;		/* letter size for title		*/
	float c0;		/* minimum contour value		*/
	float dash;		/* dash length for dashed line		*/
	float dc;		/* contour interval			*/
	float dummy;		/*					*/
	float dy;		/*					*/
	float dynum;		/*					*/
	float dytic;		/*					*/
	float dxtic;		/*					*/
   	float dx;		/*					*/
	float dxnum;		/*					*/
	float gap;		/* gap length for dashed line		*/
	float yc;		/* current y position			*/
	float yinch;		/* vertical size (inches)		*/
	float ymax;		/*					*/
	float ymaxc;		/*					*/
	float ymin;		/*					*/
   	float yminc;		/*					*/
	float ynum;		/*					*/
	float ytic;		/*					*/
	float yticlabels;	/* flag: 0=no tic labels , 1=tic labels	*/
	float y0;		/*					*/
   	float y0tic;		/*					*/
	float y0num;		/*					*/
	float xc;		/* current x position 			*/
	float xinch;		/* horizontal size (inches)		*/
	float xmax;		/*					*/
	float xmaxc;		/*					*/
	float xmin;		/*					*/
	float xminc;		/*					*/
   	float xnum;		/*					*/
	float xtic;		/*					*/
	float xticlabels;	/* flag: 0=no tic labels , 1=tic labels	*/
	float x0;		/*					*/
   	float x0num;		/*					*/
	float x0tic;		/*					*/
   	float **z;		/*					*/
	float zmax;		/*					*/
	float zmin;		/*					*/
	float zeroy;		/* y-coord of left side of plot 	*/
	float zerox;		/* x-coord of bottom side of plot	*/
 /* yminc is for contour plot, ymin for axis, etc.. */
	string grid;		/* flag for tics or grid lines		*/
   	char buf[80];		/*					*/


	/* set constants */
	one = 1;
   	minusone = - one;   

	/* allocate memory for z then load in data values */
   	z = (float **) malloc((uint) nx*4);
   	for (ix = 0; ix < nx; ix++)
      		z[ix] = dataptr + ix * ny;

	/* getpars and parameters for x (horizontal) axis */
   	if (!fgetpar("dx",  &dx))	dx = 1.0;
	xticlabels = 1;
	if (dx == 0.0) {
		xticlabels = 0;
		dx=1.0;
	}

	/* getpar xmin to avoid user confusion between x0 and zerox */
   	if (!fgetpar("xmin",  &x0))     x0 = 0;
   	if (!fgetpar("sizex",  &xinch)) xinch = 4.5;
   	xmin = x0;
   	xmax = x0 + (nx - 1) * dx;
   	if (!igetpar("nxtic",  &nxtic)) nxtic = 5;
   	get_scl (xmin, xmax, nxtic, &dummy, &dummy, &dxnum);
   	fgetpar("dxnum",  &dxnum);
   	for (x0num = (int) (xmin / dxnum) * dxnum - dxnum; 
					x0num < xmin; x0num += dxnum);
   	fgetpar("x0num",  &x0num);


	/* getpars and parameters for y (vertical) axis */
   	if (!fgetpar("dy",  &dy))	dy = 1.0;
	yticlabels = 1;
	if (dy == 0.0) {
		yticlabels = 0;
		dy=1.0;
	}

	/* getpar ymin to avoid user confusion between y0 and zeroy */
   	if (!fgetpar("ymin",  &y0))     y0 = 0;
   	if (!fgetpar("sizey",  &yinch)) yinch = 6.;
   	ymin = y0;
   	ymax = y0 + (ny - 1) * dy;
   	if (!igetpar("nytic",  &nytic))	nytic = 5;
   	get_scl (ymin, ymax, nytic, &dummy, &dummy, &dynum);
   	fgetpar("dynum",  &dynum);
   	for (y0num = (int) (ymin / dynum) * dynum - dynum;
					y0num < ymin; y0num += dynum);	
	fgetpar("y0num",  &y0num);

	/*  more getpars  */
   	if (!fgetpar("zeroy",  &zeroy))   zeroy = 1.0;
   	if (!fgetpar("zerox",  &zerox))   zerox = .6;
   	if (!igetpar("titlsz",  &titlsz)) titlsz = 4;
   	if (!igetpar("lablsz",  &lablsz)) lablsz = 4;
   	if (!igetpar("ticsz",  &ticsz))   ticsz = 3;
   	if (!sgetpar("grid", &grid))      grid = "tics";
   	if (!igetpar("dash",  &dashflag)) dashflag = 0;

	/*  used to be getpars  */
   	axiscol = 3;   			/* igetpar("axiscol",  &axiscol); */
   	axisfat = 0;   			/* igetpar("axisfat",  &axisfat); */

	/*  number of contours  */
   	if (!igetpar("nc",  &nc)) nc = 5;
   	if (nc > MAXNC) {
      		fprintf (stderr, "Maximum number of contours is MAXNC\n");
      		exit (-1);
   	}

	/*  find min and max of z, then calc contour interval  */
   	minmax (z, ny, nx, &zmin, &zmax);
   	get_scl (zmin, zmax, nc, &dummy, &dummy, &dc);
   		fgetpar("dc",  &dc);

	/* what is this crap ??? */
/*   	for (c0 = (int) (zmin / dc) * dc - dc; c0 < zmin; c0 += dc);
   		fgetpar("c0",  &c0);
   	for (ic = 0; ic < MAXNC; ic++) {
      		c[ic] = 1.e30;
      		cntrcol[ic] = GARBAGE;
      		cntrfat[ic] = GARBAGE;
   	}
*/

	/*  find c0 & load contour values into vector 'c'  */
   	for (c0 = (int) (zmin / dc) * dc - dc; c0 < zmin; c0 += dc);
   		fgetpar("c0",  &c0);
    	for (ic = 0; ic < nc; ic++)  c[ic] = c0 + ic * dc;


	/* get or calc number of contour colors */
   	if (igetpar("cntrcol",  cntrcol)) {	/* cntrcol as a unit */
      		ntemp = 0;
      		while (cntrcol[ntemp] != GARBAGE)
	 		ntemp++;
      		for (ic = ntemp; ic < nc; ic++)
	 		cntrcol[ic] = cntrcol[ic % ntemp];
   	} else {
      		for (ic = 0; ic < nc; ic++) 
	 		cntrcol[ic] = 6 - ic % 6; 
   	}


	/* get or set fatness of contour lines  */
   	if (igetpar("cntrfat",  cntrfat)) {	/* cntrfat as a unit */
      		ntemp = 0;
		/* after this loop ntemp = #cntrfat values read */
      		while (cntrfat[ntemp] != GARBAGE)
	 		ntemp++;	
      		for (ic = ntemp; ic < MAXNC; ic++)
	 		cntrfat[ic] = cntrfat[ic % ntemp];
   	} else {
      		for (ic = 0; ic < MAXNC; ic++)
	 		cntrfat[ic] = 0;
   	}

   	/* initialization for plotting axes */
      	setcol (axiscol);
	/*       ********* setfat (axisfat); */
      	setscl (xinch / (xmax - xmin), yinch / (ymax - ymin));
      	set0 (zerox, zeroy);
      	setu0 (xmin, ymin);
      	dytic = dynum / nytic;
      	for (y0tic = y0num - nytic * dytic; y0tic < ymin; y0tic += dytic);
      	dxtic = dxnum / nxtic;
      	for (x0tic = x0num - nxtic * dxtic; x0tic < xmin; x0tic += dxtic);
      	lines = (grid[0] == 'l');

      	/* draw plot frame */
 	umove (xmin, ymin);
	udraw (xmin, ymax);
	udraw (xmax, ymax);
	udraw (xmax, ymin);
	udraw (xmin, ymin);

      	/* tic marks for (vertical) y-axis */
	for (ynum = y0num; ynum <= ymax; ynum += dynum) {
		if (fabs (ynum) < (ymax - ymin) / 10000) ynum = 0.0;

      		/* tics on right with labels */
	    	umove (xmax, ynum);
	    	where (&xc, &yc);
	    	draw (xc + .1, yc);
	    	if ( lines ) udraw (xmin, ynum);
		sprintf(buf, "%g", (float) ynum );
		if (yticlabels == 1)
	 		text ( xc+.2,yc, ticsz, 0, buf); 

      		/* tics on left with no labels */
	    	umove (xmin, ynum);
	    	where (&xc, &yc);
	    	draw (xc - .1, yc);
	 }

      	 /* label for (vertical) y-axis */
	 umove (xmin,(ymax + ymin) / 2.);
	 where (&xc, &yc);
	 setcol(8);
	 Text ( xc-.35,yc, lablsz, 1, ylabel); 

      	 /* tic marks for (horizontal) x-axis */
      	 setcol (axiscol);
	 for (xnum = x0num; xnum <= xmax; xnum += dxnum) {
	    	if (fabs (xnum) < (xmax - xmin) / 10000) xnum = 0.0;

      		/* bottom tics with labels */
	    	umove (xnum,ymin);  		/* move to tic location */
	    	where (&xc,&yc);    		/* get current location */
	    	draw (xc,yc - .1);  		/* draw the tic */
	    	if ( lines ) udraw (xnum,ymax); /* optional grid lines */
      		/* load tic value into buf and label tic */
		sprintf(buf, "%g", (float) xnum );
		if (xticlabels == 1)
	 		Text ( xc,yc - .3, ticsz, 0, buf); 

      		/* top tics with no labels */
	    	umove (xnum,ymax);
	    	where (&xc,&yc); 
	    	draw (xc,yc + .1);
	 }

      	 /* label for (horizontal) x-axis */
	 umove ((xmax + xmin) / 2.,ymin);
	 where (&xc, &yc);
	 setcol(8);
	 Text ( xc,yc - .6, lablsz, 0, xlabel); 

   	/* plot title */
	umove ((xmax + xmin) / 2.,ymax);
      	where (&xc,&yc);
	Text (xc,yc+.5, titlsz, 0, title);

   	/* set plot scales and window for contour subroutine */
	 setcol(axiscol);
      	yminc = 0.;
      	ymaxc = ny - 1;
      	xminc = 0.;
      	xmaxc = nx - 1;
      	setscl (xinch / xmaxc, yinch / ymaxc);
      	set0 (zerox, zeroy);
      	setu0 (xminc, yminc);

   	/* contour */
      	for (ic = 0; ic < nc; ic++) {
		dash = ( ic % 6 -1 ) * .02;
		gap = dash/2.;
		if ( dash != 0.0 && dashflag == 1 ) {
			setdash(dash,gap,dash,gap);
		}
	 	setcol (cntrcol[ic]);
	 	contour (z, ny, nx, c[ic]);
      	}
}




/*
 		north (0)
		south (2)
*/
contour (z,nx,ny,c)
int nx,ny; float **z,c;
  {
	register int ix,iy,non; 
	int jx,jy,sset(),wset();
	register float zxymc,zemc,znmc; 
	float x,y,delta(),*pzxy;

	/* find all the intersections */
	non = 0;				/* clear intersection counter */
	for (iy=0; iy<ny-1; iy++)
	  {
		for (ix=0; ix<nx-1; ix++)
		  {
			pzxy = &z[iy][ix];
			zxymc = (*pzxy)-c;	/* z(x,y) - c */
			zemc = z[iy][ix+1]-c;	/* (z to the east) - c */
			znmc = z[iy+1][ix]-c;	/* (z to the north) - c */
#define OPPSIGN(A,B) (1==(((A)<0.)+((B)<0.)))
			if (OPPSIGN(zxymc,znmc)) /* if west edge intersected */
			  {
				setw (pzxy);	/* set the west bit */
				non++;		/* and increment counter */
			  }
			else			/* else */
				clrw (pzxy);	/* clear the west bit */
			if (OPPSIGN(zxymc,zemc)) /* if south edge intersected */
			  {
				sets (pzxy);	/* set the south bit */
				non++;		/* and increment counter */
			  }
			else			/* else */
				clrs (pzxy);	/* clear the south bit */
		  }
	  }
	for (ix=0,iy=ny-1; ix<nx-1; ix++)	/* northern boundary */
	  {
			pzxy = &z[iy][ix];
			zxymc = (*pzxy)-c;	/* z(x,y) - c */
			zemc = z[iy][ix+1]-c;	/* (z to the east) - c */
			if (OPPSIGN(zxymc,zemc)) /* if south edge intersected */
			  {
				sets (pzxy);	/* set the south bit */
				non++;		/* and increment counter */
			  }
			else			/* else */
				clrs (pzxy);	/* clear the south bit */
			clrw (pzxy);		/* clear the west bit */
	  }
	for (iy=0,ix=nx-1; iy<ny-1; iy++)	/* eastern boundary */
	  {
			pzxy = &z[iy][ix];
			zxymc = (*pzxy)-c;	/* z(x,y) - c */
			znmc = z[iy+1][ix]-c;	/* (z to the north) - c */
			if (OPPSIGN(zxymc,znmc)) /* if west edge intersected */
			  {
				setw (pzxy);	/* set the west bit */
				non++;		/* and increment counter */
			  }
			else			/* else */
				clrw (pzxy);	/* clear the west bit */
			clrs (pzxy);		/* clear the south bit */
	  }

	/* draw contours intersecting a boundary */
	for (ix=0,iy=ny-1; ix<nx-1 && non>0; ix++)	/* north boundary */
	  {
		if (sset(&z[iy][ix]))
		  {
			x = ix+delta(c,z[iy][ix],z[iy][ix+1]); y = iy;
			umove (y,x);
			clrs(&z[iy][ix]); non--; 
			jx = ix; jy = iy-1;
			while (connect(z,nx,ny,c,&jx,&jy))
				non--;
		  }
	  }
	for (ix=nx-1,iy=0; iy<ny-1 && non>0; iy++)	/* east boundary */
	  {
		if (wset(&z[iy][ix]))
		  {
			x = ix; y = iy+delta(c,z[iy][ix],z[iy+1][ix]);
			umove (y,x);
			clrw(&z[iy][ix]); non--; 
			jx = ix-1; jy = iy;
			while (connect(z,nx,ny,c,&jx,&jy))
				non--;
		  }
	  }
	for (ix=0,iy=0; ix<nx-1 && non>0; ix++)		/* south boundary */
	  {
		if (sset(&z[iy][ix]))
		  {
			x = ix+delta(c,z[iy][ix],z[iy][ix+1]); y = iy;
			umove (y,x);
			clrs(&z[iy][ix]); non--;
			jx = ix; jy = iy;
			while (connect(z,nx,ny,c,&jx,&jy))
				non--;
		  }
	  }
	for (ix=0,iy=0; iy<ny-1 && non>0; iy++)		/* west boundary */
	  {
		if (wset(&z[iy][ix]))
		  {
			x = ix; y = iy+delta(c,z[iy][ix],z[iy+1][ix]);
			umove (y,x);
			clrw(&z[iy][ix]); non--;
			jx = ix; jy = iy;
			while (connect(z,nx,ny,c,&jx,&jy))
				non--;
		  }
	  }

	/* draw interior contours */
	for (iy=0; iy<ny-1 && non>0; iy++)
	  {
		for (ix=0; ix<nx-1 && non>0; ix++)
		  {
			if (sset(&z[iy][ix]))	/* check south edge of cell */
			  {
				x = ix+delta(c,z[iy][ix],z[iy][ix+1]); y = iy;
				umove (y,x);
				clrs(&z[iy][ix]); non--; 	/* clear start */
				jx = ix; jy = iy;
				if (connect(z,nx,ny,c,&jx,&jy)) 
					sets(&z[iy][ix]);    /* finish = start */
				while (connect(z,nx,ny,c,&jx,&jy))
					non--;
			  }
		  }
	  }
  }

/* connect draws a line from one intersection of the cell (ix,iy)
   to another intersection of the cell, provided the latter intersection exists,
   and then clears the latter intersection and updates ix and iy.
   connect returns 0 if the latter intersection does not exist or if the 
   latter intersection is a grid boundary; otherwise returns 1.
*/
int connect (z,nx,ny,c,ix,iy)
int nx,ny,*ix,*iy; float **z,c;
  {
	register int jx,jy;
	float x,y,delta();

	jx = (*ix); jy = (*iy);

	if (sset(&z[jy+1][jx]))		/* if exiting north */
	  {
		jy++; x = jx+delta(c,z[jy][jx],z[jy][jx+1]); y = jy;
		udraw (y,x);
		clrs(&z[jy][jx]); 
		if (++(*iy)>=ny-1) return 0;
	  }
	else if (wset(&z[jy][jx+1]))	/* if exiting east */
	  {
		jx++; x = jx; y = jy+delta(c,z[jy][jx],z[jy+1][jx]);
		udraw (y,x);
		clrw(&z[jy][jx]);
		if (++(*ix)>=nx-1) return 0;
	  }
	else if (sset(&z[jy][jx]))	/* if exiting south */
	  {
		x = jx+delta(c,z[jy][jx],z[jy][jx+1]); y = jy;
		udraw (y,x);
		clrs(&z[jy][jx]);
		if (--(*iy)<0) return 0;
	  }
	else if (wset(&z[jy][jx]))	/* if exiting west */
	  {
		x = jx; y = jy+delta(c,z[jy][jx],z[jy+1][jx]);
		udraw (y,x);
		clrw(&z[jy][jx]);
		if (--(*ix)<0) return 0;
	  }
	else
		return 0;		/* no exit found */
	return(1);
  }

/* subroutines to set, clear, and check status of bits */
#define SOUTH 0x00000001
#define WEST 0x00000002
sets(i)
register int *i;
  {
	*i |= SOUTH;
  }
clrs(i)
register int *i;
  {
	*i &= ~SOUTH;
  }
int sset(i)
register int *i;
  {
	return ((*i)&SOUTH);
  }
setw(i)
register int *i;
  {
	*i |= WEST;
  }
clrw(i)
register int *i;
  {
	*i &= ~WEST;
  }
int wset(i)
register int *i;
  {
	return ((*i)&WEST);
  }

/* subroutine to compute (a-b)/(c-b) for use in linear interpolation */
float delta (a,b,c)
float a,b,c;
  {
	float t;
	t = c-b; /* avoids pathological comparison */
	if (t != 0.0) return (a-b)/t;
	else return 0.5;
  }

/* function places a tic value in a character string and
   returns the number of characters */
cntnum (strg,ticval)
float ticval;
string strg;
  {
/* 	extern char *sprintf(); */
/* 	sprintf (strg,"%1.5g\0",ticval); */
/* 	return(cntchr(strg)); */
  }

/* function returns the number of characters in a string */
cntchr (strg)
string strg;
  {
	int i=0,nc=0; char esc=033;
	while (strg[i] != '\0')
	  {
		if (strg[i++]==esc)
			i++;
		else
			nc++;
	  }
	return (nc);
  }

/* subroutine to determine readable scales for axes
   taken from Algorithm 463 -- Collected Algorithms from CACM */
static float vint[4] = { 1., 2., 5., 10. };
static float sqr[3] = { 1.414214, 3.162278, 7.071068 };
get_scl (xmin,xmax,n,xminp,xmaxp,dist)
int n;
float xmin,xmax,*xminp,*xmaxp,*dist;
  {
	int nal,i,m1,m2;
	float del,fn,fm1,fm2,a,b,al;
	double log10(),fabs(),pow();

	if (xmax < xmin || n <= 0)
	  {
		fprintf (stderr,"bad arguments passed to function get_scl\n");
		exit(-1);
	  }
	fn = n;

	/* roundoff error tolerance */
	del = .00002;
	
	/* find approximate interval size a */
	a = (xmax - xmin) / fn;
	al = log10(a);
	nal = al;
	if (a < 1.)
		nal--;
	
	/* a is scaled into variable named b between 1 and 10 */
		b = a / pow(10.,(double)nal);

	/* the closest permissible value for b is found */
	for (i = 0; i < 3 && b >= sqr[i]; i++);
		
	/* the interval size is computed */
	*dist = vint[i] * pow (10.,(double)nal);
	fm1 = xmin / *dist;
	m1 = fm1;
	if (fm1 < 0.) m1--;
	if (fabs(m1 + 1. - fm1) < del)
		m1++;
	
	/* the new minimum and maximum limits are found */
	*xminp = *dist * m1;
	fm2 = xmax / *dist;
	m2 = fm2 + 1.;
	if (fm2 < -1.)
		m2--;
	if (fabs(fm2 + 1. - m2) < del)
		m2--;
	*xmaxp = *dist * m2;

	/* adjust limits to account for roundoff if necessary */
	if (*xminp > xmin)
		*xminp = xmin;
	if (*xmaxp < xmax)
		*xmaxp = xmax;
  }

/*  this routine can be vectorized for increased speed */
minmax (f,n,m,pmin,pmax)
int n,m; float **f,*pmin,*pmax;
  {
	register int i,j; register float min,max,fij;
	min = 1.e30; max = (-1.e30);
	for (i=0; i<m; i++)
	  {
		for (j=0; j<n; j++)
		  {
			fij = f[i][j];
			min = (min<fij)?min:fij;
			max = (max>fij)?max:fij;
		  }
	  }
	*pmin = min; *pmax = max;
  }
