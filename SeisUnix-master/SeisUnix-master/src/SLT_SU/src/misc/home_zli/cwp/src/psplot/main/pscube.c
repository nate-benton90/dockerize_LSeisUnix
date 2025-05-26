/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSCUBE - PostScript IMAGE plot of a data cube\n"
"\n"
"pscube n1= n2= [optional parameters] <binaryfile >postscriptfile\n"
"\n"
"Required Parameters:\n"
"n1                     number of samples in 1st (fast) dimension\n"
"n2                     number of samples in 2nd (slow) dimension\n"
"\n"
"Optional Parameters:\n"
"n3=all                 number of samples in third dimension 	\n"
"d1=1.0                 sampling interval in 1st dimension\n"
"f1=d1                  first sample in 1st dimension\n"
"d2=1.0                 sampling interval in 2nd dimension\n"
"f2=d2                  first sample in 2nd dimension\n"
"d3=1.0                 sampling interval in 3rd dimension\n"
"f3=d3                  first sample in 3rd dimension\n"
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [wclip,bclip] are clipped\n"
"wclip=-clip            data values outside of [wclip,bclip] are clipped\n"
"d1s=1.0                factor by which to scale d1 before imaging\n"
"d2s=1.0                factor by which to scale d2 before imaging\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"xbox=1.5               offset in inches of left side of axes box\n"
"ybox=1.5               offset in inches of bottom side of axes box\n"
"size1=4.0              size in inches of 1st axes (vertical) \n"
"size2=4.0              size in inches of 2nd axes (horizontal) \n"
"size3=4.0              size in inches of 3rd axes (projected) \n"
"itop=1                 time/depth slice index to display on top of cube \n"
"                       (1<=itop<=n1)					\n"
"ifront=1               vertical section index to display at front of cube \n"
"                       (1<=ifront<=n3)					\n"
"iside=n2               vertical section index to display at side of cube \n"
"                       (1<=iside<=n2)					\n"
"angle=45.              projection angle of cube in degrees (0<angle<90) \n"
"                       (angle between 2nd axis and projected 3rd axis)  \n"
"d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n"
"f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n"
"n1tic=1                number of tics per numbered tic on axis 1\n"
"grid1=none             grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=                label on axis 1\n"
"d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n"
"f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n"
"n2tic=1                number of tics per numbered tic on axis 2\n"
"grid2=none             grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=                label on axis 2\n"
"d3num=0.0              numbered tic interval on axis 3 (0.0 for automatic)\n"
"f3num=x3min            first numbered tic on axis 3 (used if d2num not 0.0)\n"
"n3tic=1                number of tics per numbered tic on axis 3\n"
"grid3=none             grid lines on axis 3 - none, dot, dash, or solid\n"
"label3=                label on axis 3\n"
"labelfont=Helvetica    font name for axes labels\n"
"labelsize=12           font size for axes labels\n"
"title=                 title of plot\n"
"titlefont=Helvetica-Bold font name for title\n"
"titlesize=24           font size for title\n"
"\n"
"AUTHOR:  Zhiming Li & Dave Hale,  CSM, 07/01/90 \n"
"\n";

#include "par.h"
#include "psplot.h"

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n3,n1tic,n2tic,n3tic,nfloats,bbox[4],
		i1,i2,npar,grid1,grid2,grid3,style,
		i3,i3p1,
		n1c,n2c,n1s,n2s,i1beg,i1end,i2beg,i2end,i1c,i2c,
		i3beg,i3end,
		i1begcu,i1endcu,i2begcu,i2endcu,
		nz,iz,ns,i1step,i2step,verbose;
	int n1cu,n2cu,i,j;
	int itop, iside, ifront;
	float labelsize,titlesize,perc,clip,bperc,wperc,bclip,wclip,
		d1,f1,d2,f2,d3,f3,*z,*temp,zscale,zoffset,zi,
		xbox,ybox,wbox,hbox,
		x1beg,x1end,x2beg,x2end,x3beg,x3end,
		x1begcu,x1endcu,x2begcu,x2endcu,
		x1min,x1max,x2min,x2max,x3min,x3max,
		x1mincu,x1maxcu,x2mincu,x2maxcu,
		d1num,f1num,d2num,f2num,d3num,f3num,
		p1beg,p1end,p2beg,p2end,matrix[6],
		p3beg,p3end,
		d1s,d2s,d3s;
	float *front,*side,*top,*cube,d1cu,d2cu,angle,size1,size2,size3,
	      vsize,hsize,size3new,anglenew,tmp;
	float hf,ht,wf,ws;
	int n1f,n1t,n2f,n2ss;
	unsigned char *cz,*czp,*sz,*szp;
	char *label1="",*label2="",*label3="",*title="",
		*labelfont="Helvetica",*titlefont="Helvetica-Bold",
		*styles="seismic",*grid1s="none",*grid2s="none",*grid3s="none";
	FILE *infp=stdin;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if (!getparint("n1",&n1)) err("must specify n1!\n");
	d1 = 1.0;  getparfloat("d1",&d1);
	f1 = d1;  getparfloat("f1",&f1);

	/* get parameters describing 2nd dimension sampling */
	if (!getparint("n2",&n2)) err("must specify n2!\n");
	if (!getparint("n3",&n3)) {
		if (fseek(infp,0L,2)!=0)
			err("must specify n3 if in a pipe!\n");
		nfloats = eftell(infp)/sizeof(float);
		efseek(infp,0L,0);
		n3 = nfloats/n1/n2;
	}
	d2 = 1.0;  getparfloat("d2",&d2);
	f2 = d2;  getparfloat("f2",&f2);
	d3 = 1.0;  getparfloat("d3",&d3);
	f3 = d3;  getparfloat("f3",&f3);

	if(!getparfloat("angle",&angle)) angle=45.; 
	angle=angle*3.141692654/180.;
	if(!getparfloat("size1",&size1)) size1=4.;
	if(!getparfloat("size2",&size2)) size2=4.;
	if(!getparfloat("size3",&size3)) size3=4.;
	if(!getparint("itop",&itop)) itop=1; 
	if(!getparint("iside",&iside)) iside=n2;
	if(!getparint("ifront",&ifront)) ifront=1;
	d1cu = n1-1; d1cu=size1/d1cu;
	d2cu = n2-1; d2cu=size2/d2cu;
	hf = size1;
	wf = size2;
	n1f = n1;
	n2f = n2;
	htop(size3, &ht, d1cu, angle, &n1t); 
	wside(size3, &ws, d2cu, angle, &n2ss);
	cubesize(hf,ht,wf,ws,&vsize,&hsize,n1f,n1t,n2f,n2ss,&n1cu,&n2cu);

	tmp = ht*ht + ws*ws;
	size3new = sqrt(tmp);
	anglenew = atan( ht/ws );  

	x1mincu = (d1>0.0)?f1:f1+(n1cu-1)*d1;
	x1maxcu = (d1<0.0)?f1:f1+(n1cu-1)*d1;
	x2mincu = (d2>0.0)?f2:f2+(n2cu-1)*d2;
	x2maxcu = (d2<0.0)?f2:f2+(n2cu-1)*d2;

	x1min = (d1>0.0)?f1:f1+(n1-1)*d1;
	x1max = (d1<0.0)?f1:f1+(n1-1)*d1;
	x2min = (d2>0.0)?f2:f2+(n2-1)*d2;
	x2max = (d2<0.0)?f2:f2+(n2-1)*d2;
	x3min = (d3>0.0)?f3:f3+(n3-1)*d3;
	x3max = (d3<0.0)?f3:f3+(n3-1)*d3;

	/* read binary data to be plotted */
	nz = n1*n2 + n1*n3 + n3*n2;
	z = ealloc1float(nz);

	/* read in front panel */
	efseek(infp,(ifront-1)*n1*n2*sizeof(float),0);
	if (fread(z,sizeof(float),n1*n2,infp)!=n1*n2)
		err("error reading input file!\n");
	/* read in side panel */
	for(j=0;j<n3;j++) {
		efseek(infp,((iside-1)*n1+j*n1*n2)*sizeof(float),0);
		if (fread(z+n1*n2+j*n1,sizeof(float),n1,infp)!=n1)
			err("error reading input file!\n");
	}
	/* read in top panel */
	for (j=0;j<n2;j++) {
	   	for(i=0;i<n3;i++) {
			efseek(infp,(j*n1+i*n1*n2+itop-1)*sizeof(float),0);
			fread(z+n1*n2+n1*n3+j*n3+i,sizeof(float),1,infp);
		}
	}
			

	/* copy data into panels */
	front = ealloc1float(n1*n2);
	side = ealloc1float(n1*n3);
	top = ealloc1float(n3*n2);
	for(i=0;i<n1*n2;i++) front[i] = z[i];
	for (j=0;j<n3;j++)
	   for(i=0;i<n1;i++)
	      side[i+j*n1] = z[n1*n2+i+j*n1];
	for (j=0;j<n2;j++)
	   for(i=0;i<n3;i++)
	      top[i+j*n3] = z[n1*n2+n1*n3+i+j*n3];
	free1float(z);
	

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
		free1float(z);
	} else {
		free1float(z);
	}

	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("bclip=%g wclip=%g",bclip,wclip);

	/* get scaled sampling intervals */
	d1s = 1.0;  getparfloat("d1s",&d1s);
	d2s = 1.0;  getparfloat("d2s",&d2s);
	d3s = 1.0;  
	d1s = fabs(d1s);  d1s *= d1;
	d2s = fabs(d2s);  d2s *= d2;
	d3s *= d3;

	/* get axes parameters */
	xbox = 1.5; getparfloat("xbox",&xbox);
	ybox = 1.5; getparfloat("ybox",&ybox);

	x1beg = x1min; /* getparfloat("x1beg",&x1beg); */
	x1end = x1max; /* getparfloat("x1end",&x1end); */
	d1num = 0.0; getparfloat("d1num",&d1num);
	f1num = x1min; getparfloat("f1num",&f1num);
	n1tic = 1; getparint("n1tic",&n1tic);
	getparstring("grid1",&grid1s);
	if (STREQ("dot",grid1s)) grid1 = DOT;
	else if (STREQ("dash",grid1s)) grid1 = DASH;
	else if (STREQ("solid",grid1s)) grid1 = SOLID;
	else grid1 = NONE;
	getparstring("label1",&label1);

	x2beg = x2min; /* getparfloat("x2beg",&x2beg); */ 
	x2end = x2max; /* getparfloat("x2end",&x2end); */
	d2num = 0.0; getparfloat("d2num",&d2num);
	f2num = x2min; getparfloat("f2num",&f2num);
	n2tic = 1; getparint("n2tic",&n2tic);
	getparstring("grid2",&grid2s);
	if (STREQ("dot",grid2s)) grid2 = DOT;
	else if (STREQ("dash",grid2s)) grid2 = DASH;
	else if (STREQ("solid",grid2s)) grid2 = SOLID;
	else grid2 = NONE;
	getparstring("label2",&label2);

	x3beg = x3min; /* getparfloat("x3beg",&x3beg); */
	x3end = x3max; /* getparfloat("x3end",&x3end); */
	d3num = 0.0; getparfloat("d3num",&d3num);
	f3num = x3min; getparfloat("f3num",&f3num);
	n3tic = 1; getparint("n3tic",&n3tic);
	getparstring("grid3",&grid3s);
	if (STREQ("dot",grid3s)) grid3 = DOT;
	else if (STREQ("dash",grid3s)) grid3 = DASH;
	else if (STREQ("solid",grid3s)) grid3 = SOLID;
	else grid3 = NONE;
	getparstring("label3",&label3);


	getparstring("labelfont",&labelfont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);
	getparstring("title",&title);
	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	style = SEISMIC;

/*    setup cube map  */
	cube = ealloc1float(n1cu*n2cu);
	cubeimage(n1,n2,n3,size1,size2,size3new,anglenew,front,side,top,cube);	

	free1float(front);
	free1float(side);
	free1float(top);

        hbox = vsize;
	wbox = hsize;

	/* adjust x1beg and x1end to fall on sampled values */

	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	i1begcu = NINT((x1mincu-f1)/d1);
	i1begcu = MAX(0,MIN(n1cu,i1begcu));
	x1begcu = f1+i1begcu*d1;
	i1endcu = NINT((x1maxcu-f1)/d1);
	i1endcu = MAX(0,MIN(n1cu-1,i1endcu));
	x1endcu = f1+i1endcu*d1;

	/* adjust x2beg and x2end to fall on sampled values */
	i2beg = NINT((x2beg-f2)/d2);
	i2beg = MAX(0,MIN(n2-1,i2beg));
	x2beg = f2+i2beg*d2;
	i2end = NINT((x2end-f2)/d2);
	i2end = MAX(0,MIN(n2-1,i2end));
	x2end = f2+i2end*d2;

	i2begcu = NINT((x2mincu-f2)/d2);
	i2begcu = MAX(0,MIN(n2cu,i2begcu));
	x2begcu = f2+i2begcu*d2;
	i2endcu = NINT((x2maxcu-f2)/d2);
	i2endcu = MAX(0,MIN(n2cu-1,i2endcu));
	x2endcu = f2+i2endcu*d2;

	/* adjust x3beg and x3end to fall on sampled values */
	i3beg = NINT((x3beg-f3)/d3);
	i3beg = MAX(0,MIN(n3-1,i3beg));
	x3beg = f3+i3beg*d3;
	i3end = NINT((x3end-f3)/d3);
	i3end = MAX(0,MIN(n3-1,i3end));
	x3end = f3+i3end*d3;

	/* allocate space for image bytes */
	n1c = 1+abs(i1endcu-i1begcu);
	n2c = 1+abs(i2endcu-i2begcu);

	/* convert data to be imaged into unsigned characters */
	zscale = (wclip!=bclip)?255.0/(wclip-bclip):1.0e10;
	zoffset = -bclip*zscale;
	i1step = (i1endcu>i1begcu)?1:-1;
	i2step = (i2endcu>i2begcu)?1:-1;

	/* determine sampling after scaling */
	n1s = MAX(1,NINT(1+(n1c-1)*d1/d1s));
	d1s = (n1s>1)?d1*(n1c-1)/(n1s-1):d1;
	n2s = MAX(1,NINT(1+(n2c-1)*d2/d2s));
	d2s = (n2s>1)?d2*(n2c-1)/(n2s-1):d2;

	/* convert axes box parameters from inches to points */
	xbox *= 72.0;
	ybox *= 72.0;
	wbox *= 72.0;
	hbox *= 72.0;


	cz = ealloc1(n1c*n2c,sizeof(char));
	czp = cz;

	for (i1c=0,i1=i1begcu; i1c<n1c; i1c++,i1+=i1step) {
		for (i2c=0,i2=i2begcu; i2c<n2c; i2c++,i2+=i2step) {
			zi = zoffset+cube[i1+i2*n1cu]*zscale;
			if (zi<0.0) zi = 0.0;
			if (zi>255.0) zi = 255.0;
			*czp++ = (unsigned char)zi;
		}
	}

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
	p3beg = (x3end>x3beg)?-fabs(d3s)/2:fabs(d3s)/2;
	p3end = (x3end>x3beg)?fabs(d3s)/2:-fabs(d3s)/2;

	/* begin PostScript */
	beginps();
	newpage("1",1);
	

	/* save graphics state */
	gsave();

	/* translate coordinate system by box offset */
	translate(xbox,ybox);

	/* determine image matrix */

	matrix[0] = n2s;  matrix[1] = 0;  matrix[2] = 0;
	matrix[3] = -n1s;  matrix[4] = 0;  matrix[5] = n1s;

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
	size1 *= 72.0; 
	size2 *= 72.0; 
	size3new *= 72.0; 

	psAxes3(xbox,ybox,size1,size2,size3new,anglenew,
		x1beg,x1end,p1beg,p1end,
		d1num,f1num,n1tic,grid1,label1,
		x2beg,x2end,p2beg,p2end,
		d2num,f2num,n2tic,grid2,label2,
		x3beg,x3end,p3beg,p3end,
		d3num,f3num,n3tic,grid3,label3,
		labelfont,labelsize,
		title,titlefont,titlesize,
		(itop-1)*d1+f1,(iside-1)*d2+f2,(ifront-1)*d3+f3);


	/* end PostScript */
	showpage();
	endps();
	
}
