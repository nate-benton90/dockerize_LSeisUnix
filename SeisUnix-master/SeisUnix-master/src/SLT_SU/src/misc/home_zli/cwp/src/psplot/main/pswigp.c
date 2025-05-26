/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSWIGP - PostScript WIGgle-trace plot of f(x1,x2) via Polygons\n"
"Best for few traces.  Use PSWIGB (Bitmap version) for many traces.\n"
"\n"
"pswigp n1= [optional parameters] <binaryfile >postscriptfile\n"
"\n"
"Required Parameters:\n"
"n1                     number of samples in 1st (fast) dimension\n"
"\n"
"Optional Parameters:\n"
"d1=1.0                 sampling interval in 1st dimension\n"
"f1=d1                  first sample in 1st dimension\n"
"n2=all                 number of samples in 2nd (slow) dimension\n"
"d2=1.0                 sampling interval in 2nd dimension\n"
"f2=d2                  first sample in 2nd dimension\n"
"x2=f2,f2+d2,...        array of sampled values in 2nd dimension\n"
"bias=0.0               data value corresponding to location along axis 2\n"
"perc=100.0             percentile for determining clip\n"
"clip=(perc percentile) data values < bias+clip and > bias-clip are clipped\n"
"xcur=1.0               wiggle excursion in traces corresponding to clip\n"
"fill=1			=0 for no fill; >0 for pos. fill; <0 for neg. fill\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
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
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 05/29/90\n"
"\n";

#include "par.h"
#include "psplot.h"

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n1tic,n2tic,nfloats,fill,bbox[4],
		i1,i2,npar,grid1,grid2,style,
		i1beg,i1end,if1p,n1p,
		p1fz,p1lz,n2in,nz,iz,i,nbytes,verbose;
	float labelsize,titlesize,bias,perc,clip,xcur,
		d1,f1,d2,f2,*x2,*z,zmin,zmax,
		xbox,ybox,wbox,hbox,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		*temp,p2beg,p2end,matrix[6],
		pscale,poffset,pxcur,px2,x1size,x2size;
	char *label1="",*label2="",*title="",
		*labelfont="Helvetica",*titlefont="Helvetica-Bold",
		*styles="seismic",*grid1s="none",*grid2s="none";
	FILE *infp=stdin;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if (!getparint("n1",&n1))
		err("Must specify number of samples in 1st dimension!\n");
	d1 = 1.0;  getparfloat("d1",&d1);
	f1 = d1;  getparfloat("f1",&f1);
	x1min = (d1>0.0)?f1:f1+(n1-1)*d1;
	x1max = (d1<0.0)?f1:f1+(n1-1)*d1;

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
	for (i2=1,x2min=x2max=x2[0]; i2<n2; i2++) {
		x2min = MIN(x2min,x2[i2]);
		x2max = MAX(x2max,x2[i2]);
	}

	/* read binary data to be plotted */
	nz = n1*n2;
	z = ealloc1float(nz);
	if (fread(z,sizeof(float),nz,infp)!=nz)
		err("Error reading input file!\n");

	/* if necessary, subtract bias */
	if (getparfloat("bias",&bias) && bias!=0.0)
		for (iz=0; iz<nz; iz++)
			z[iz] -= bias;
	
	/* if necessary, determine clip from percentile */
	if (!getparfloat("clip",&clip)) {
		perc = 100.0;  getparfloat("perc",&perc);
		temp = ealloc1float(nz);
		for (iz=0; iz<nz; iz++)
			temp[iz] = fabs(z[iz]);
		iz = (nz*perc/100.0);
		if (iz<0) iz = 0;
		if (iz>nz-1) iz = nz-1;
		qkfind(iz,nz,temp);
		clip = temp[iz];
		free1float(temp);
	}
	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("clip=%g",clip); 

	/* get fill parameter */
	fill = 1;  getparint("fill",&fill);

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
	getparstring("label1",&label1);
	getparstring("label2",&label2);
	getparstring("labelfont",&labelfont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);
	getparstring("title",&title);
	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;

	/* determine number of traces that fall within axis 2 bounds */
	x2min = MIN(x2beg,x2end);
	x2max = MAX(x2beg,x2end);
	for (i2=0,n2in=0; i2<n2; i2++)
		if (x2[i2]>=x2min && x2[i2]<=x2max) n2in++;

	/* determine pads for wiggle excursion along axis 2 */
	xcur = 1.0;  getparfloat("xcur",&xcur);
	xcur = fabs(xcur);
	if (n2in>1) xcur *= (x2max-x2min)/(n2in-1);
	p2beg = (x2end>x2beg)?-xcur:xcur;
	p2end = (x2end>x2beg)?xcur:-xcur;

	/* adjust x1beg and x1end to fall on sampled values */
	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1-1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	/* determine first sample and number of samples to plot */
	if1p = MIN(i1beg,i1end);
	n1p = MAX(i1beg,i1end)-if1p+1;

	/* begin PostScript */
	beginps();
	newpage("1",1);

	/* convert axes box parameters from inches to points */
	xbox *= 72.0;
	ybox *= 72.0;
	wbox *= 72.0;
	hbox *= 72.0;

	/* determine box size for first and second dimensions */
	x1size = (style==NORMAL)?wbox:hbox;
	x2size = (style==NORMAL)?hbox:wbox;

	/* determine scale and offset to map x2 units to plot units */
	pscale = x2size/(x2end+p2end-x2beg-p2beg);
	poffset = -(x2beg+p2beg)*pscale;
	pxcur = xcur*pscale;

	/* determine plot coordinates of first and last samples */
	p1fz = (x1end>x1beg)?0.0:x1size;
	p1lz = (x1end>x1beg)?x1size:0.0;

	/* save graphics state */
	gsave();

	/* translate coordinate system by box offset */
	translate(xbox,ybox);

	/* if style is not normal, rotate coordinate system */
	if (style!=NORMAL) {
		rotate(-90.0);
		translate(-hbox,0.0);
	}

	/* draw traces */
	for (i2=0; i2<n2; i2++,z+=n1) {

		/* skip traces not in bounds */
		if (x2[i2]<x2min || x2[i2]>x2max) continue;

		/* determine x2 plot coordinate of trace */
		px2 = poffset+x2[i2]*pscale;

		/* plot one trace */
		psWiggle(n1p,&z[if1p],-clip,clip,0.0,
			(int)(px2-pxcur),(int)(px2+pxcur),
			p1fz,p1lz,fill);
	}

	/* restore graphics state */
	grestore();

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
		x2beg,x2end,p2beg,p2end,
		d2num,f2num,n2tic,grid2,label2,
		labelfont,labelsize,
		title,titlefont,titlesize,
		style);

	/* end PostScript */
	showpage();
	endps();
}
