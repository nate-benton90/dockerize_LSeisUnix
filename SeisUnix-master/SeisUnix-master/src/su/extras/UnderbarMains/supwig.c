/* SUPWIG: $Revision: 2.17 $ ; $Date: 89/09/20 19:37:12 $		*/

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

/*************************** self documentation **************************/
string sdoc = "\
									\n\
SUPWIG -- Display data as pseudo-3D, hidden-line plot			\n\
									\n\
supwig <stdin [optional parameters] | pen				\n\
									\n\
Optional Parameters:							\n\
	title	= null	plot title					\n\
	titlsz	= 4	title print size				\n\
	plotfat = 0	line thickness of traces			\n\
	(zc,xc) = (2, )	position of lower left hand corner of 	 	\n\
                	plot from the corner of the page in 		\n\
			inches.						\n\
			NOTE: xc defaults so as to center the 		\n\
			plot.						\n\
	sz	= 1	vertical scale.					\n\
	sizex	= 6	width of plot (inches).				\n\
	xlength = 8     width of plot device in inches 			\n\
			(for centering)					\n\
	alpha	= 45	apparent angle in degrees; |alpha| < 89		\n\
	uflag	= 1	1 = plot upper side of the surface,		\n\
			0 = do not plot upper side. 			\n\
	dflag	= 0	the same but for the down side. 		\n\
	hcopy	= 0	= 1 for harcopy sizing .. will not work 	\n\
			    for all data sets because 'true length' 	\n\
			    depends on the ratio nt:ntr (so ...  	\n\
			    sz=1, sizeex=5, and zc=4 can still be 	\n\
			    changed by user)				\n\
GAINING ...								\n\
	gain defaults (see sugain):					\n\
	tpow=0.0 epow=0.0 gpow=1.0 agc=0 wagc=20			\n\
	trap=0.0 clip=0.0 qclip=1.0 qbal=1 pbal=0 scale=1.0		\n\
";
/*************************************************************************/

/* Credits:
 *	CWP: Chris
 *	SEP: Shuki
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/supwig.c,v $";
static string revid =
	"   $Revision: 2.17 $ ; $Date: 89/09/20 19:37:12 $";




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
	string title, label1, label2;
	int n3;
	float *data;		/* mega-vector to contain data set	*/
	float tmin, dt;
	int ndata;		/* allocation parameter			*/
	int nt;			/* time samples per trace		*/
	int ntsize;		/* ... in bytes			*/
	int ntr;		/* traces in input data			*/
	void subplot();
	void gain();
	void slant();
	void plotup();
	void plotdown();
	void advance();
	void advdown();
	string gain_flag();
	string vplot_flag();
	string save_flag();


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);

	n3 = 1;

	/* Read first trace */ 
	if (!gettr(&tr)) err("can't get first trace\n");

	/*  get number of time samples & calc some constants  */	
	nt = tr.ns; 
	ntsize = nt * FSIZE;


	/* Set tmin and dt  */	
	if (!fgetpar("tmin", &tmin))	tmin = tr.delrt/1000.0;
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
		if (ntr*nt > ndata) {	/* need more memory */
			ndata <<= 1;	/* ask for double   */
			data = re_vec(data, ndata);
		}
		bcopy(tr.data, data + (ntr - 1)*nt, ntsize); 
	} while(gettr(&tr));


	/* Print nt, ntr */
	warn("nt=%d   ntr=%d\n", nt, ntr);	

	/* TITLE */
	if (!sgetpar("title", &title)) title = " ";

	/* LABELS */
	if (!sgetpar("label1", &label1)) label1 = " ";
	if (!sgetpar("label2", &label2)) label2 = " ";

	/* Gain */
	gain(data, TPOW, EPOW, GPOW, AGC, TRAP, CLIP, QCLIP,
			QBAL, PBAL, SCALE, tmin, dt, WAGC, nt, ntr);

	/* CALL THE PLOTTING PROGRAM AS A SUBROUTINE */
	subplot(data,nt,ntr,n3,title,label1,label2);

	endplot ();


	return SUCCEED;
}

#define E 1e-5
#define pi 3.14159265

float alpha,cosa,sina,	/* alpha=apparant angle in degrees		*/
	zc,xc,		/* absolute lower left of the plot in inches	*/
	dx,dz, xstart = 0.0,xend, scaley, sz, xlength = 8.0; 	
int di;
float *maxmask,*minmask,oldmask;

void subplot(data,n1,n2,n3,title,label1,label2)
string title, label1, label2;
int n1,n2,n3;
float *data;
{
	int ix,iy,i3,nx,ny,uflag,dflag,titlsz,plotfat,hcopy;
	float y,dy,zmax,zmin,fm,scale,s1,s2,tana,scalex,scalez;
	char titletemp[80];
	float sizex;

	nx = n1;	/* nt */
	ny = n2;	/* ntr */
	scalez = 1;

	maxmask = (float *) malloc((uint) nx*4 + 4);
	minmask = (float *) malloc((uint) nx*4 + 4);

	if (!igetpar("uflag", &uflag))		uflag = 1;
	if (!igetpar("dflag", &dflag))		dflag = 0;
	if (!igetpar("titlsz", &titlsz))	titlsz = 6;
	if (!igetpar("plotfat", &plotfat))	plotfat = 0;
	fgetpar("xlength", &xlength);

	sizex = 6.;
	alpha = 45.;
	sz = 1.;
	zc = 2.;

	if (!igetpar("hcopy", &hcopy))		hcopy = 0;
	if ( hcopy == 1 ) {
   		sizex = 5;
		alpha = 45;
		sz = 1;
		zc = 4;
	} 		/* xc set below ! */

	fgetpar("sizex", &sizex);	/* can override hcopy */
	fgetpar("alpha", &alpha);
	fgetpar("sz", &sz);
	fgetpar("zc", &zc);

	alpha *= pi/180.;
	cosa = cos(alpha);
	sina = sin(alpha);
	tana = sina/cosa;
	
	s1 = sizex*nx/(1. + (float)(ny/nx));
	s2 = 4*nx*nx/ny/tana;
	scale = MIN(s1,s2);

	di = 1;
	xend = xstart + scale*ny/nx;
	dy = 1./ny;
	dx = (xend-xstart)/(nx-1);
	scalex = dy/di;
	scaley = dx/cosa;

	/* Center it! */
	xc = (xlength - scalex*scale*ny/nx - ny*dy*scaley*cosa)/2.;
	if (xc<0.0) xc = 0.0;
	fgetpar("xc",&xc);
	if (hcopy == 1) xc = 0.4;

	zmax = zmin = 0.;
	for(i3 = 0;i3 < n3; i3++) 
		for(iy = 0; iy < ny; ++iy)
			for(ix = 0; ix < nx; ++ix)
			{
				y = iy*dy*scaley;
				fm = data[(i3*ny+iy)*nx+ix] + y*sina;
				if (fm < zmin) zmin = fm;
				else if (fm > zmax) zmax = fm;
			}
	if (zmax == zmin)
		fprintf(stderr,"thplot: input file is zero");
	else scalez = sz/(zmax-zmin);
	dz = scaley*dy*sina/scalez;

/*	erase(); */
	setcol(2);   /* color for traces */

	for(i3=0;i3<n3;i3++)
	{
 		setfat(plotfat); 
		setscl (scalex,scalez);
		/* initial a mask */
		for(ix=0;ix<=nx;++ix)
		{
			maxmask[ix] = 10.;
			minmask[ix] = -10.;
		}
		/* make a pseudo 3-D plot */
		for (iy = 0;iy < ny; ++iy)
		{
			y = iy*dy*scaley;
			slant(y);
			if (dflag == 1) plotdown(data,nx,i3*ny+iy);
			if (uflag == 1) plotup(data,nx,i3*ny+iy);
		}
		setscl(1.,1.);
		setcol(1);    /* color for title */
		strcpy(titletemp,title);
		text(2.5,1.3,titlsz,0,titletemp);
	}
	return;
}

void slant(y)	/*	computes z0,x0 and calls set0		*/
float y;
{
	float y0,x0;
	y0 = zc + y*sina;
	x0 = xc + y*cosa;
	set0(x0,y0);
	return;
}

void plotup(data,nx,iy)	/*	plots a constant y cut in f(x,y)	*/
int nx,iy; float *data;
{
	int ix;
	float xnew,xold,fnew,fold;

	/* update the maxmask */
	for(ix=0;ix<=nx-di;++ix)
		maxmask[ix] = maxmask[ix+di] + dz;
	for(ix=nx-di+1;ix<=nx;++ix)
		maxmask[ix] =  10.;

	umove(xstart,data[iy*nx]);

	for(ix=1;ix<nx;++ix)
	{
		xnew = xstart + dx*ix;
		xold = xnew - dx;
		fnew = data[iy*nx+ix];
		fold = data[iy*nx+ix-1];
		advance(xold,fold,xnew,fnew,ix);
	}
	umove (xend,0.0);
	return;
}

void plotdown(data,nx,iy)
int nx,iy; float *data;
{
	int ix;
	float xnew,xold,fnew,fold;

	for(ix=0;ix<=nx-di;++ix)
		minmask[ix] = minmask[ix+di] + dz;
	for(ix=nx-di+1;ix<=nx;++ix)
		minmask[ix] =  -10.;

	umove(xstart,data[iy*nx]);

	for(ix=1;ix<nx;++ix)
	{
		xnew = xstart + dx*ix;
		xold = xnew - dx;
		fnew = data[iy*nx+ix];
		fold = data[iy*nx+ix-1];
		advdown(xold,fold,xnew,fnew,ix);
	}
	umove (xend,0.0);
	return;
}

void advance(x1,f1,x2,f2,i)	/* advance from (x1,f1) to (x2,f2) */
float x1,f1,x2,f2;
int i;
{
	float x,z,fc,intersect();

	/* start and end in a seen area:			 */
	if ( -f1 < maxmask[i-1]+E && -f2 < maxmask[i]+E ) 
	{ 
		oldmask = maxmask[i];
		maxmask[i] = - f2; 
		udraw (x2,f2);
	}

	/* start and end in  dead area:				 */
	else if ( -f1 > maxmask[i-1] && -f2 > maxmask[i] ) 
		umove (x2,f2);

	/* start in a seen area and end in a dead area:		 */
	else if ( -f1 < oldmask && -f2 > maxmask[i] )
	{					
		z = intersect(-f1,-f2,oldmask,maxmask[i]);
		x =  (1.-z)*x1 + z*x2; fc = (1.-z)*f1 + z*f2;
		udraw(x,fc); umove(x2,f2);
	}

	/* start in a dead area and end in a seen area:		 */
	else if ( -f1 > maxmask[i-1] && -f2 < maxmask[i] )
	{
		z = intersect(-f1,-f2,maxmask[i-1],maxmask[i]);
		x =  (1.-z)*x1 + z*x2; fc = (1.-z)*f1 + z*f2;
		oldmask = maxmask[i]; maxmask[i] = - f2;
		umove(x,fc); udraw(x2,f2);
	}

	/* start and end in  dead area:				 */
	else	umove (x2,f2);

	return;
}
		/* advance from (x1,f1) to (x2,f2) */

void advdown(x1,f1,x2,f2,i)
float x1,f1,x2,f2;
int i;
{
	float x,z,fc,intersect();

	if ( -f1 > minmask[i-1]-E && -f2 > minmask[i]-E ) 
	{ 
		oldmask = minmask[i]; minmask[i] = - f2; 
		udraw (x2,f2);
	}

	else if ( -f1 < minmask[i-1] && -f2 < minmask[i] ) 
		umove (x2,f2);

	else if ( -f1 > oldmask && -f2 < minmask[i] )
	{					
		z = intersect(-f1,-f2,oldmask,minmask[i]);
		x =  (1.-z)*x1 + z*x2; fc = (1.-z)*f1 + z*f2;
		udraw(x,fc); umove(x2,f2);
	}

	else if ( -f1 < minmask[i-1] && -f2 > minmask[i] )
	{
		z = intersect(-f1,-f2,minmask[i-1],minmask[i]);
		x =  (1.-z)*x1 + z*x2; fc = (1.-z)*f1 + z*f2;
		oldmask = minmask[i]; minmask[i] = - f2;
		umove(x,fc); udraw(x2,f2);
	}

	else	umove (x2,f2);

	return;
}

float intersect(f1,f2,m1,m2)
float f1,f2,m1,m2;
{
	float z;
	z = (f1-m1)/(f1-m1+m2-f2);
	return z;
}
string gain_flag()	{ return("yes");}
string vplot_flag()	{ return("yes");}
string save_flag()	{ return("yes");}
