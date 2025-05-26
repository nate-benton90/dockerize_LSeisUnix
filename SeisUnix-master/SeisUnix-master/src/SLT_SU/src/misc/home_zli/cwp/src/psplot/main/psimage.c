/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSIMAGE - PostScript IMAGE plot of a uniformly-sampled function f(x1,x2)\n"
"\n"
"psimage n1= [optional parameters] <binaryfile >postscriptfile\n"
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
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [bclip,wclip] are clipped\n"
"wclip=-clip            data values outside of [bclip,wclip] are clipped\n"
"d1s=1.0                factor by which to scale d1 before imaging\n"
"d2s=1.0                factor by which to scale d2 before imaging\n"
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
"titlesize=24            font size for title\n"
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
	int n1,n2,n1tic,n2tic,nfloats,bbox[4],
		i1,i2,npar,grid1,grid2,style,
		n1c,n2c,n1s,n2s,i1beg,i1end,i2beg,i2end,i1c,i2c,
		nz,iz,ns,i1step,i2step,verbose;
	float labelsize,titlesize,perc,clip,bperc,wperc,bclip,wclip,
		d1,f1,d2,f2,*z,*temp,zscale,zoffset,zi,
		xbox,ybox,wbox,hbox,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		p1beg,p1end,p2beg,p2end,matrix[6],
		d1s,d2s;
	unsigned char *cz,*czp,*sz,*szp;
	char *label1="",*label2="",*title="",
		*labelfont="Helvetica",*titlefont="Helvetica-Bold",
		*styles="seismic",*grid1s="none",*grid2s="none";
	FILE *infp=stdin;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if (!getparint("n1",&n1)) err("must specify n1!\n");
	d1 = 1.0;  getparfloat("d1",&d1);
	f1 = d1;  getparfloat("f1",&f1);
	x1min = (d1>0.0)?f1:f1+(n1-1)*d1;
	x1max = (d1<0.0)?f1:f1+(n1-1)*d1;

	/* get parameters describing 2nd dimension sampling */
	if (!getparint("n2",&n2)) {
		if (fseek(infp,0L,2)!=0)
			err("must specify n2 if in a pipe!\n");
		nfloats = eftell(infp)/sizeof(float);
		efseek(infp,0L,0);
		n2 = nfloats/n1;
	}
	d2 = 1.0;  getparfloat("d2",&d2);
	f2 = d2;  getparfloat("f2",&f2);
	x2min = (d2>0.0)?f2:f2+(n2-1)*d2;
	x2max = (d2<0.0)?f2:f2+(n2-1)*d2;

	/* read binary data to be plotted */
	nz = n1*n2;
	z = ealloc1float(nz);
	if (fread(z,sizeof(float),nz,infp)!=nz)
		err("error reading input file!\n");

	/* if necessary, determine clips from percentiles */
	if (getparfloat("clip",&clip)) {
		bclip = clip;
		wclip = -clip;
	}
	if ((!getparfloat("bclip",&bclip) || !getparfloat("wclip",&wclip)) &&
		!getparfloat("clip",&clip)) {
		perc = 100.0;  getparfloat("perc",&perc);
		temp = ealloc1float(nz);
		for (iz=0; iz<nz; iz++)
			temp[iz] = z[iz];
		if (!getparfloat("bclip",&bclip)) {
			bperc = perc;	getparfloat("bperc",&bperc);
			iz = (nz*bperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			bclip = temp[iz];
		}
		if (!getparfloat("wclip",&wclip)) {
			wperc = 100.0-perc;  getparfloat("wperc",&wperc);
			iz = (nz*wperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			wclip = temp[iz];
		}
		free1float(temp);
	}
	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("bclip=%g wclip=%g",bclip,wclip);

	/* get scaled sampling intervals */
	d1s = 1.0;  getparfloat("d1s",&d1s);
	d2s = 1.0;  getparfloat("d2s",&d2s);
	d1s = fabs(d1s);  d1s *= d1;
	d2s = fabs(d2s);  d2s *= d2;

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

	/* adjust x1beg and x1end to fall on sampled values */
	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1-1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	/* adjust x2beg and x2end to fall on sampled values */
	i2beg = NINT((x2beg-f2)/d2);
	i2beg = MAX(0,MIN(n2-1,i2beg));
	x2beg = f2+i2beg*d2;
	i2end = NINT((x2end-f2)/d2);
	i2end = MAX(0,MIN(n2-1,i2end));
	x2end = f2+i2end*d2;

	/* allocate space for image bytes */
	n1c = 1+abs(i1end-i1beg);
	n2c = 1+abs(i2end-i2beg);
	cz = ealloc1(n1c*n2c,sizeof(char));

	/* convert data to be imaged into unsigned characters */
	zscale = (wclip!=bclip)?255.0/(wclip-bclip):1.0e10;
	zoffset = -bclip*zscale;
	i1step = (i1end>i1beg)?1:-1;
	i2step = (i2end>i2beg)?1:-1;
	czp = cz;
	for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
		for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
			zi = zoffset+z[i1+i2*n1]*zscale;
			if (zi<0.0) zi = 0.0;
			if (zi>255.0) zi = 255.0;
			*czp++ = (unsigned char)zi;
		}
	}
	free1float(z);

	/* determine sampling after scaling */
	n1s = MAX(1,NINT(1+(n1c-1)*d1/d1s));
	d1s = (n1s>1)?d1*(n1c-1)/(n1s-1):d1;
	n2s = MAX(1,NINT(1+(n2c-1)*d2/d2s));
	d2s = (n2s>1)?d2*(n2c-1)/(n2s-1):d2;

	/* if necessary, interpolate to scaled sampling intervals */
	if (n1s!=n1c || n2s!=n2c) {
		sz = ealloc1(n1s*n2s,sizeof(char));
		intl2b(n2c,d2,0.0,n1c,d1,0.0,cz,n2s,d2s,0.0,n1s,d1s,0.0,sz);
		free1(cz);
	} else {
		sz = cz;
	}
		
	/* determine axes pads */
	p1beg = (x1end>x1beg)?-fabs(d1s)/2:fabs(d1s)/2;
	p1end = (x1end>x1beg)?fabs(d1s)/2:-fabs(d1s)/2;
	p2beg = (x2end>x2beg)?-fabs(d2s)/2:fabs(d2s)/2;
	p2end = (x2end>x2beg)?fabs(d2s)/2:-fabs(d2s)/2;

	/* begin PostScript */
	beginps();
	newpage("1",1);
	
	/* convert axes box parameters from inches to points */
	xbox *= 72.0;
	ybox *= 72.0;
	wbox *= 72.0;
	hbox *= 72.0;

	/* save graphics state */
	gsave();

	/* translate coordinate system by box offset */
	translate(xbox,ybox);

	/* determine image matrix */
	if (style==NORMAL) {
		matrix[0] = 0;  matrix[1] = n1s;  matrix[2] = n2s;
		matrix[3] = 0;  matrix[4] = 0;  matrix[5] = 0;
	} else {
		matrix[0] = n2s;  matrix[1] = 0;  matrix[2] = 0;
		matrix[3] = -n1s;  matrix[4] = 0;  matrix[5] = n1s;
	}

	scale(wbox,hbox);

	/* draw the image (before axes so grid lines are visible) */
	image(n2s,n1s,8,matrix,sz);

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
		x1beg,x1end,p1beg,p1end,
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
