/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSGRAPH - PostScript GRAPHer\n"
"Graphs n[i] pairs of (x,y) coordinates, for i = 1 to nplot.\n"
"\n"
"psgraph n= [optional parameters] <binaryfile >postscriptfile\n"
"\n"
"Required Parameters:\n"
"n                      array containing number of points per plot\n"
"\n"
"Data formats supported:\n"
"	1. x1,y1,x2,y2,...,xn,yn\n"
"	2. y1,y2,...,yn (must give non-zero d1[]=)\n"
"	3. x1,x2,...,xn (must give non-zero d2[]=)\n"
"	4. nil (must give non-zero d1[]= and non-zero d2[]=)\n"
"  The formats may be repeated and mixed in any order, but if\n"
"  formats 2-4 are used, the d1 and d2 arrays must be specified including\n"
"  d1[]=0.0 d2[]=0.0 entries for any internal occurences of format 1.\n"
"  Also, if formats 2-4 are used with non-zero f1[] or f2[] entries, then\n"
"  the corresponding array(s) must be fully specified including f1[]=0.0\n"
"  and/or f2[]=0.0 entries for any internal occurences of format 1 or\n"
"  formats 2-4 where the zero entries are desired.\n"
"\n"
"Optional Parameters:\n"
"nplot=number of n's    number of plots\n"
"d1=0.0,...             x sampling intervals (0.0 if x coordinates input)\n"
"f1=0.0,...             first x values (not used if x coordinates input)\n"
"d2=0.0,...             y sampling intervals (0.0 if y coordinates input)\n"
"f2=0.0,...             first y values (not used if y coordinates input)\n"
"linewidth=1.0,...      line widths (in points) (0.0 for no lines)\n"
"linegray=0.0,...       line gray levels (black=0.0 to white=1.0)\n"
"mark=0,1,2,3,...       indices of marks used to represent plotted points\n"
"marksize=0.0,0.0,...   size of marks (0.0 for no marks)\n"
"xbox=1.5               offset in inches of left side of axes box\n"
"ybox=1.5               offset in inches of bottom side of axes box\n"
"wbox=6.0               width in inches of axes box\n"
"hbox=8.0               height in inches of axes box\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n"
"f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n"
"n1tic=1                number of tics per numbered tic on axis 1\n"
"grid1=none             grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=                label on axis 1\n"
"x2beg=x2min            value at which axis 2 begins\n"
"x2end=x2max            value at which axis 2 ends\n"
"d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n"
"f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n"
"n2tic=1                number of tics per numbered tic on axis 2\n"
"grid2=none             grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=                label on axis 2\n"
"labelfont=Helvetica    font name for axes labels\n"
"labelsize=12           font size for axes labels\n"
"title=                 title of plot\n"
"titlefont=Helvetica-Bold font name for title\n"
"titlesize=24           font size for title\n"
"style=normal           normal (axis 1 horizontal, axis 2 vertical) or\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"\n"
"Note:	n1 and n2 are acceptable aliases for n and nplot, respectively.\n"
"\n"
"Example:\n"
"psgraph n=50,100,20 d1=2.5,1,0.33 <datafile >psfile\n"
"  plots three curves with equally spaced x values in one plot frame\n"
"  x1-coordinates are x1(i) = f1+i*d1 for i = 1 to n (f1=0 by default)\n"
"  number of x2's and then x2-coordinates for each curve are read\n"
"  sequentially from datafile.\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 05/29/90\n"
"MODIFIED:  Jack K. Cohen 11/23/90 for different input data format\n"
"\n";

#include "par.h"
#include "psplot.h"

#define NPMAX 5000	/* Arbitrary maximum number of plots allowed	*/


main (argc,argv)
int argc; char **argv;
{
	int nplot,n[NPMAX],nn,iplot,n1tic,n2tic,nd1,nf1,nd2,nf2,
		mark[NPMAX],grid1,grid2,style,npar,bbox[4];
	register int i;
	float labelsize,titlesize,linewidth[NPMAX],linegray[NPMAX],
		marksize[NPMAX],d1[NPMAX],f1[NPMAX],d2[NPMAX],f2[NPMAX],
		x1beg,x2beg,x1end,x2end,xbox,ybox,wbox,hbox,
		x1min,x1max,x2min,x2max, d1num,f1num,d2num,f2num,
		xsize,ysize,xscale,yscale;
	char *label1="",*label2="",*title="",
		*labelfont="Helvetica",*titlefont="Helvetica-Bold",
		*styles="normal",*grid1s="none",*grid2s="none";
	float **x1data, **x2data;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters needed to interpret datafile */
	for (i=0; i<NPMAX; i++) {
		d1[i] = 0.0;
		f1[i] = 0.0;
		d2[i] = 0.0;
		f2[i] = 0.0;
	}
	nd1 = getparfloat("d1",d1);
 	nf1 = getparfloat("f1",f1);
	nd2 = getparfloat("d2",d2);
 	nf2 = getparfloat("f2",f2);

	if (!(nn = getparint("n",n)))  nn = getparint("n1",n);
	if (nn==0)  err("Must specify n, the number of points per plot!");
	nplot = nn; getparint("n2",&nplot); getparint("nplot",&nplot);
	if (nplot > NPMAX)  err("too many plots");
	for (i=nn; i<nplot; ++i)
		n[i] = n[nn-1];
	for (i=nd1; i<nplot; ++i)
		d1[i] = d1[nd1-1];
	for (i=nf1; i<nplot; ++i)
		f1[i] = f1[nf1-1];
	for (i=nd2; i<nplot; ++i)
		d2[i] = d2[nd2-1];
	for (i=nf2; i<nplot; ++i)
		f2[i] = f2[nf2-1];


	/* read, regularize and compute extreme values of data */
	x1data = (float **)ealloc1(nplot, sizeof(float*));
	x2data = (float **)ealloc1(nplot, sizeof(float*));
	x2max = x1max = -FLT_MAX;
	x2min = x1min =  FLT_MAX;
	for (iplot=0; iplot<nplot; ++iplot) {
		register int npoint = n[iplot];
		
		x1data[iplot] = ealloc1float(npoint);
		x2data[iplot] = ealloc1float(npoint);
		
		/* read data for this plot */
		if (d1[iplot] && d2[iplot]) { /* straight line */
			register int i;
			register float *px1data=x1data[iplot];
			register float *px2data=x2data[iplot];
			float x1,x2;
			
			for (i=0; i<npoint; ++i) {
				x1 = f1[iplot] + i*d1[iplot];
				x2 = f2[iplot] + i*d2[iplot];
				x1max = MAX(x1, x1max);
				x1min = MIN(x1, x1min);
				x2max = MAX(x2, x2max);
				x2min = MIN(x2, x2min);
				*px1data++ = x1;
				*px2data++ = x2;
			}
		} else if (d1[iplot]) { /* equally spaced x1's */
			register int i;
			register float *px1data=x1data[iplot];
			register float *px2data=x2data[iplot];
			float x1,x2;

			for (i=0; i<npoint; ++i) {
				efread(&x2, FSIZE, 1, stdin);
				x1 = f1[iplot] + i*d1[iplot];
				x1max = MAX(x1, x1max);
				x1min = MIN(x1, x1min);
				x2max = MAX(x2, x2max);
				x2min = MIN(x2, x2min);
				*px1data++ = x1;
				*px2data++ = x2;
			}
		} else if (d2[iplot]) { /* equally spaced x2's */
			register int i;
			register float *px1data=x1data[iplot];
			register float *px2data=x2data[iplot];
			float x1,x2;

			for (i=0; i<npoint; ++i) {
				efread(&x1, FSIZE, 1, stdin);
				x2 = f2[iplot] + i*d2[iplot];
				x1max = MAX(x1, x1max);
				x1min = MIN(x1, x1min);
				x2max = MAX(x2, x2max);
				x2min = MIN(x2, x2min);
				*px1data++ = x1;
				*px2data++ = x2;
			}
		} else { /* pairs */
			register int i;
			register float *px1data=x1data[iplot];
			register float *px2data=x2data[iplot];
			float x1,x2;

			for (i=0; i<npoint; ++i) {
				efread(&x1, FSIZE, 1, stdin);
				efread(&x2, FSIZE, 1, stdin);
				x1max = MAX(x1, x1max);
				x1min = MIN(x1, x1min);
				x2max = MAX(x2, x2max);
				x2min = MIN(x2, x2min);
				*px1data++ = x1;
				*px2data++ = x2;
			}
		}
	}

	/* cope with special cases */
	if (x1min==FLT_MAX) x1min = x1max = 0.0;
	if (x2min==FLT_MAX) x2min = x2max = 0.0;
	if (x1min == x1max) {
		x1min -= 1.0;
		x1max += 1.0;
	}
	if (x2min == x2max) {
		x2min -= 1.0;
		x2max += 1.0;
	}

	/* get plotting parameters */
	getparstring("label1",&label1);
	getparstring("label2",&label2);
	getparstring("title",&title);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;
	getparstring("labelfont",&labelfont);
	getparstring("titlefont",&titlefont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	xbox = 1.5; getparfloat("xbox",&xbox);
	ybox = 1.5; getparfloat("ybox",&ybox);
	wbox = 6.0; getparfloat("wbox",&wbox);
	hbox = 8.0; getparfloat("hbox",&hbox);

	x1beg = x1min; getparfloat("x1beg",&x1beg);
	x1end = x1max; getparfloat("x1end",&x1end);
	x2beg = x2min; getparfloat("x2beg",&x2beg);
	x2end = x2max; getparfloat("x2end",&x2end);
	d1num = 0.0; getparfloat("d1num",&d1num);
	f1num = x1min; getparfloat("f1num",&f1num);
	n1tic = 1; getparint("n1tic",&n1tic);
	getparstring("grid1",&grid1s);
	if (STREQ("dot",grid1s)) grid1 = DOT;
	else if (STREQ("dash",grid1s)) grid1 = DASH;
	else if (STREQ("solid",grid1s)) grid1 = SOLID;
	else grid1 = NONE;
	d2num = 0.0; getparfloat("d2num",&d2num);
	f2num = 0.0; getparfloat("f2num",&f2num);
	n2tic = 1; getparint("n2tic",&n2tic);
	getparstring("grid2",&grid2s);
	if (STREQ("dot",grid2s)) grid2 = DOT;
	else if (STREQ("dash",grid2s)) grid2 = DASH;
	else if (STREQ("solid",grid2s)) grid2 = SOLID;
	else grid2 = NONE;
	for (i=0; i<nplot; i++) {
		mark[i] = i%9;
		marksize[i] = 0.0;
		linewidth[i] = 1.0;
		linegray[i] = 0.0;
	}
	if (npar=getparint("mark",mark))
		for (i=npar; i<nplot; ++i)  mark[i] = mark[npar-1];
	if (npar=getparfloat("marksize",marksize))
		for (i=npar; i<nplot; ++i)  marksize[i] = marksize[npar-1];
	if (npar=getparfloat("linewidth",linewidth))
		for (i=npar; i<nplot; ++i)  linewidth[i] = linewidth[npar-1];
	if (npar=getparfloat("linegray",linegray))
		for (i=npar; i<nplot; ++i)  linegray[i] = linegray[npar-1];

	/* begin PostScript */
	beginps();
	newpage("1",1);

	/* convert box parameters from inches to points */
	xbox *= 72.0;
	ybox *= 72.0;
	wbox *= 72.0;
	hbox *= 72.0;

	/* set bounding box */
	psAxesBBox(
		xbox,ybox,wbox,hbox,
		labelfont,labelsize,
		titlefont,titlesize,
		style,bbox);
	boundingbox(bbox[0],bbox[1],bbox[2],bbox[3]);

	/* draw axes and title */
	psAxesBox(
		xbox,ybox,wbox,hbox,
		x1beg,x1end,0.0,0.0,
		d1num,f1num,n1tic,grid1,label1,
		x2beg,x2end,0.0,0.0,
		d2num,f2num,n2tic,grid2,label2,
		labelfont,labelsize,
		title,titlefont,titlesize,
		style);

	/* set clip */
	rectclip(xbox,ybox,wbox,hbox);

	/* determine axes sizes */
	xsize = (style==NORMAL)?wbox:hbox;
	ysize = (style==NORMAL)?hbox:wbox;

	/* translate coordinate system by box offset */
	translate(xbox,ybox);

	/* if style is not normal, rotate coordinate system */
	if (style!=NORMAL) {
		rotate(-90.0);
		translate(-hbox,0.0);
	}

	/* determine x and y scale factors */
	xscale = xsize/(x1end-x1beg);
	yscale = ysize/(x2end-x2beg);

	/* draw the plots */
	for (iplot=0; iplot<nplot; ++iplot) {
		register int j;
		register int ni = n[iplot];
		register float *px1data = x1data[iplot];
		register float *px2data = x2data[iplot];
		float *x1 = ealloc1float(ni);
		float *x2 = ealloc1float(ni);
		
				
		/* translate and scale */
		for (j=0; j<ni; ++j) {
			x1[j] = (*px1data++ - x1beg)*xscale;
			x2[j] = (*px2data++ - x2beg)*yscale;
		}

		/* plot */
		gsave();
		if (linewidth[iplot]!=0.0) {
			setlinewidth(linewidth[iplot]);
			setgray(linegray[iplot]);
			polyline(x1,x2,ni);
		}
		if (marksize[iplot]!=0.0) {
			for (j=0; j<ni; ++j)
			    markto(x1[j],x2[j],mark[iplot],marksize[iplot]);
		}
		grestore();

		free1float(x1);
		free1float(x2);
	}

	/* end PostScript */
	showpage();
	endps();
}
