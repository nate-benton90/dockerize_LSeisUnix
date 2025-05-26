/*
 *
 * $Source: /vol/SU/cwp/src/psplot/main/RCS/pscontour.c,v $
 *
 * $Log: pscontour.c,v $
 * Revision 1.1  1992/12/16  00:01:48  suadm
 * Initial revision
 *
 */
 
/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSCONTOURL - PostScript CONTOURing of a two-dimensional function f(x1,x2)\n"
"\n"
"pscontourl n1= [optional parameters] <binaryfile >postscriptfile\n"
"\n"
"Required Parameters:\n"
"n1                     number of samples in 1st (fast) dimension\n"
"\n"
"Optional Parameters:\n"
"d1=1.0                 sampling interval in 1st dimension\n"
"f1=d1                  first sample in 1st dimension\n"
"x1=f1,f1+d1,...        array of monotonic sampled values in 1st dimension\n"
"n2=all                 number of samples in 2nd (slow) dimension\n"
"d2=1.0                 sampling interval in 2nd dimension\n"
"f2=d2                  first sample in 2nd dimension\n"
"x2=f2,f2+d2,...        array of monotonic sampled values in 2nd dimension\n"
"nc=5                   number of contour values\n"
"dc=(max-min)/nc        contour interval\n"
"fc=min+dc              first contour\n"
"c=fc,fc+dc,...         array of contour values\n"
"cwidth=1.0,...         array of contour line widths\n"
"cgray=0.0,...          array of contour grays (0.0=black to 1.0=white)\n"
"cdash=0.0,...          array of dash spacings (0.0 for solid)\n"
"nclabel=nc             number of contours to label (0 no contour label)\n"
"fclabel=1              first index of contours to label \n"
"dclabel=1              index increment of contours to label \n"
"clabelsize=6           font size of contour label   \n"
"xbox=1.5               offset in inches of left side of axes box\n"
"ybox=1.5               offset in inches of bottom side of axes box\n"
"wbox=6.0               width in inches of axes box\n"
"hbox=8.0               height in inches of axes box\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"d1num=0.0              numbered tic inter to labelval on axis 1 (0.0 for automatic)\n"
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
"labelsize=18           font size for axes labels\n"
"title=                 title of plot\n"
"titlefont=Helvetica-Bold font name for title\n"
"titlesize=24           font size for title\n"
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"Note.\n"
"The line width of unlabeled contours is designed as a quarter of that of \n"
"labeled contours.\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 05/29/90\n"
"REVISER  Zhenyue Liu, 6/20/93\n"
"\n";

#include "par.h"
#include "psplot.h"

/* maximum number of contours */
#define NCMAX 200
void psContourl(float c,int n1,float x1[],int n2,float x2[],float z []
		,float lcs, float *w);

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,nc,n1tic,n2tic,nfloats,bbox[4],
		i1,i2,iz,ic,npar,grid1,grid2,style;
	float labelsize,titlesize,cwidth[NCMAX],cgray[NCMAX],cdash[NCMAX],
		d1,f1,d2,f2,dc,fc,*x1,*x2,c[NCMAX],*z,zmin,zmax,
		xbox,ybox,wbox,hbox,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1size,x2size,x1scale,x2scale;
	float labelcsize, lcsize, *w;
	int  labelcf, nlabelc, labelcrate;
	char *label1="",*label2="",*title="",
		*labelfont="Helvetica",*titlefont="Helvetica-Bold",
		*styles="seismic",*grid1s="none",*grid2s="none";
	FILE *infp=stdin;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if ((n1=countparval("x1"))==0 && !getparint("n1",&n1))
		err("Must specify n1!\n");
	x1 = ealloc1float(n1);
	if (!getparfloat("x1",x1)) {
		d1 = 1.0;  getparfloat("d1",&d1);
		f1 = d1;  getparfloat("f1",&f1);
		for (i1=0; i1<n1; i1++)
			x1[i1] = f1+i1*d1;
	}
	/*
	for (i1=1,x1min=x1max=x1[0]; i1<n1; i1++) {
		x1min = MIN(x1min,x1[i1]);
		x1max = MAX(x1max,x1[i1]);
	}
	*/
	x1min = x1[0];
	x1max = x1[n1-1];

	/* get parameters describing 2nd dimension sampling */
	if ((n2=countparval("x2"))==0 && !getparint("n2",&n2)) {
			if (fseek(infp,0L,2)!=0)
				err("must specify n2 if in a pipe!\n");
			nfloats = eftell(infp)/sizeof(float);
			efseek(infp,0L,0);
			n2 = nfloats/n1;
	}
	x2 = ealloc1float(n2);
	if (!getparfloat("x2",x2)) {
		d2 = 1.0;  getparfloat("d2",&d2);
		f2 = d2;  getparfloat("f2",&f2);
		for (i2=0; i2<n2; i2++)
			x2[i2] = f2+i2*d2;
	}
	/*
	for (i2=1,x2min=x2max=x2[0]; i2<n2; i2++) {
		x2min = MIN(x2min,x2[i2]);
		x2max = MAX(x2max,x2[i2]);
	}
	*/
	x2min = x2[0];
	x2max = x2[n2-1];

	/* read binary data to be contoured */
	z = ealloc1float(n1*n2);
	w =  alloc1float(n1*n2);
	if (fread(z,sizeof(float),n1*n2,infp)!=n1*n2)
		err("error reading input file!\n");

	/* determine data min and max */
	for (i2=0,zmin=zmax=z[0]; i2<n2; i2++) {
		for (i1=0,iz=i2*n1; i1<n1; i1++,iz++) {
			zmin = MIN(zmin,z[iz]);
			zmax = MAX(zmax,z[iz]);
		}
	}

	/* get contouring parameters */
	if ((nc=getparfloat("c",c))==0) {
		nc = 5;  getparint("nc",&nc);
		dc = (zmax-zmin)/nc;  getparfloat("dc",&dc);
		fc = zmin+dc;  getparfloat("fc",&fc);
		for (ic=0; ic<nc; ic++)
			c[ic] = fc+ic*dc;
	}
	for (ic=0; ic<nc; ic++) {
		cwidth[ic] = 1.0;
		cgray[ic] = 0.0;
		cdash[ic] = 0.0;
	}
	if ((npar=getparfloat("cwidth",cwidth))!=0)
		for (ic=npar; ic<nc; ic++)
			cwidth[ic] = cwidth[npar-1];
	if ((npar=getparfloat("cgray",cgray))!=0)
		for (ic=npar; ic<nc; ic++)
			cgray[ic] = cgray[npar-1];
	if ((npar=getparfloat("cdash",cdash))!=0)
		for (ic=npar; ic<nc; ic++)
			cdash[ic] = cdash[npar-1];
  	labelcf = 1; getparint("fclabel",&labelcf);
	labelcrate = 1; getparint("dclabel",&labelcrate);
 	nlabelc = nc; getparint("nclabel",&nlabelc);
  	labelcsize = 6; getparfloat("clabelsize",&labelcsize);
  
	/* get axes parameters */
	xbox = 1.5; getparfloat("xbox",&xbox);
	ybox = 1.5; getparfloat("ybox",&ybox);
	wbox = 6.0; getparfloat("wbox",&wbox);
	hbox = 8.0; getparfloat("hbox",&hbox);
	x1beg = x1min; getparfloat("x1beg",&x1beg);
	x1end = x1max; getparfloat("x1end",&x1end);
	d1num = 0.0; getparfloat("d1num",&d1num);
	f1num = x1min; getparfloat("f1num",&f1num);
	n1tic = 1; getparint("n1tic",&n1tic);
	getparstring("grid1",&grid1s);
	if (STREQ("dot",grid1s)) grid1 = DOT;
	else if (STREQ("dash",grid1s)) grid1 = DASH;
	else if (STREQ("solid",grid1s)) grid1 = SOLID;
	else grid1 = NONE;
	getparstring("label1",&label1);
	x2beg = x2min; getparfloat("x2beg",&x2beg);
	x2end = x2max; getparfloat("x2end",&x2end);
	d2num = 0.0; getparfloat("d2num",&d2num);
	f2num = 0.0; getparfloat("f2num",&f2num);
	n2tic = 1; getparint("n2tic",&n2tic);
	getparstring("grid2",&grid2s);
	if (STREQ("dot",grid2s)) grid2 = DOT;
	else if (STREQ("dash",grid2s)) grid2 = DASH;
	else if (STREQ("solid",grid2s)) grid2 = SOLID;
	else grid2 = NONE;
	getparstring("label2",&label2);
	getparstring("labelfont",&labelfont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);
	getparstring("title",&title);
	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;

	/* begin PostScript */
	beginps();
	newpage("1",1);

	/* convert axes box parameters from inches to points */
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
	x1size = (style==NORMAL)?wbox:hbox;
	x2size = (style==NORMAL)?hbox:wbox;

	/* translate coordinate system by box offset */
	translate(xbox,ybox);

	/* if style is not normal, rotate coordinate system */
	if (style!=NORMAL) {
		rotate(-90.0);
		translate(-hbox,0.0);
	}

	/* determine x1 and x2 scale factors */
	x1scale = x1size/(x1end-x1beg);
	x2scale = x2size/(x2end-x2beg);

	/* translate coordinate system by beginning axes values */
	translate(-x1beg*x1scale,-x2beg*x2scale);

	/* scale x1 and x2 coordinates */
	for (i1=0; i1<n1; i1++)
		x1[i1] *= x1scale;
	for (i2=0; i2<n2; i2++)
		x2[i2] *= x2scale;

	/* draw contours */
	for (ic=0; ic<nc; ic++) {
		setlinewidth(cwidth[ic]);
		setgray(cgray[ic]);
		if (cdash[ic]!=0.0)
			setdash(&cdash[ic],1,0.0);
		else
			setdash(&cdash[ic],0,0.0);
		lcsize = 0.;
		if(nlabelc>0) {
			if((ic-labelcf+1)%labelcrate==0 && ic>=labelcf-1  
				&& ic<labelcf-1+labelcrate*nlabelc) {
				setlinewidth(cwidth[ic]);
				lcsize = labelcsize;
			}
			else { 
				lcsize = 0.;
				setlinewidth(0.25*cwidth[ic]);
			}
		}
		psContourl(c[ic],n1,x1,n2,x2,z,lcsize,w);
	}

	/* end PostScript */
	showpage();
	endps();
}
