h04305
s 00030/00023/00239
d D 1.3 88/11/15 14:08:43 shuki 3 2
c 
e
s 00001/00000/00261
d D 1.2 88/05/29 07:23:31 shuki 2 1
c SccsId
e
s 00261/00000/00000
d D 1.1 88/04/14 13:56:59 shuki 1 0
c date and time created 88/04/14 13:56:59 by shuki
e
u
U
f e 0
t
T
I 1
#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT HUGE
#endif
#include <stdio.h>
#include "../include/su.h"
/* #define MAX(x,y) ((x)>(y))?(x):(y) */
/* #define MIN(x,y) ((x)<(y))?(x):(y) */
#define NBLOCK 65536
char *sdoc="curve [in= symbol=(number/cdot/string) symbol_size= radius=	\n\
	axis=n title= xlabel=X ylabel=Y verbose=false			\n\
	n= scale=]						\n";
I 2
char *SccsId="%W% %G%\n";
E 2

bool verbose;

int xargc; char **xargv;
main(ac,av)
int ac; char **av;
{
D 3
	bool	cdots,lines,numbers;
E 3
I 3
	bool	cdots,lines,numbers,raw;
E 3
	int	i,n,infd,nalloc,dnalloc,
		axis_flag=0,
		symbol_size,lsymbol=1,nsymbol=1,*isymbol;
	float	radius,xmax,xmin,ymax,ymin,xscale,yscale,scale,sxzero,syzero,
		xo,yo,xsize,ysize,wx,wy,margin,swx,swy;
	char	*in,*symbol,*label,*title,*xlabel,*ylabel;
	struct st_xy {float x,y;} *xy;

	xargc = ac; xargv = av;

	/* ALLOCATE MEMORY */
	in	= (char*) malloc(128);
	symbol	= (char*) malloc(256);
	label	= (char*) malloc(64);
	xlabel	= (char*) malloc(64);
	ylabel	= (char*) malloc(64);
	title	= (char*) malloc(256);

	/* SELFDOC */
	if(isatty(0)&&ac==1) selfdoc();

	/* INPUT FILE */
D 3
	if(sgetpar("in",in)) {	/* in= specified */
E 3
I 3
	if(sgetpar("in",&in)) {	/* in= specified */
E 3
		infd = open(in,0);
		if(infd<0) err(__FILE__,__LINE__,"can't open %s\n",in);
	} else if (!isatty(0)) {	/* stdin redirected */
		infd = 0;
D 3
		gname(0,in);
E 3
I 3
		in = gname(0);
E 3
	} else if (xargc>1) {		/* Maybe xargv[1] */
		infd = open(xargv[1],0);
		if(infd<0) {
			err(__FILE__,__LINE__,"can't open %s\n",xargv[1]);
		}
		in = xargv[1];
	}

	/* GET OPTIONS AND PARAMETERS */
	verbose = false;
D 3
	while( (i=getopt(xargc,xargv,"v"))!=EOF) {
E 3
I 3
	raw = false;
	while( (i=getopt(xargc,xargv,"vr"))!=EOF) {
E 3
		switch(i) {
		case 'v':
			verbose = true;
			break;
I 3
		case 'r':
			raw = true;
			break;
E 3
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
	bgetpar("verbose",&verbose);

D 3
	lsymbol = sgetpar("symbol",symbol);
E 3
I 3
	lsymbol = sgetpar("symbol",&symbol);
E 3
	if(lsymbol) {
		isymbol = (int*) malloc(lsymbol*sizeof(int));
		for(i=2,nsymbol=1,isymbol[0]=0;i<=lsymbol;i++) {
			if( (symbol[i]!=',') && (symbol[i-1]==',') ) {
				symbol[i-1] = (char)0;
				isymbol[nsymbol++] = i;
			}
		}
	}

	cdots = lsymbol?false:true;	 bgetpar("cdots",&cdots);

	if(!bgetpar("lines",&lines))		lines = false;
	if(!bgetpar("numbers",&numbers))	numbers = false;

D 3
	symbol_size = 6;	igetpar("symbol_size",&symbol_size);
E 3
I 3
	symbol_size = 2;	igetpar("symbol_size",&symbol_size);
E 3

D 3
	if(cdots) if(!fgetpar("radius",&radius)) radius=0.0001*symbol_size;
E 3
I 3
	if(cdots) if(!fgetpar("radius",&radius)) radius=0.005*symbol_size;
E 3

D 3
	if(sgetpar("axis",label)) {
E 3
I 3
	if(sgetpar("axis",&label)) {
E 3
		if(*label=='y') axis_flag=1;
		else axis_flag=0;
	}
D 3
	if(!sgetpar("title",title)) title = in;
	if(!sgetpar("xlabel",xlabel)) strcpy(xlabel,"X");
	if(!sgetpar("ylabel",ylabel)) strcpy(ylabel,"Y");
E 3
I 3
	title = in; sgetpar("title",&title);
	xlabel = "X"; sgetpar("xlabel",&xlabel);
	ylabel="Y"; sgetpar("ylabel",&ylabel);
E 3


	if(verbose) {
		fprintf(stderr,"Input file in=%s title=%s\n",in,title);
		fprintf(stderr,"symbol=%s\tsymbol_size=%d\tradius=%f\n",
			symbol,symbol_size,radius);
		fprintf(stderr,"xlabel=%s\tylabel=%s\n",xlabel,ylabel);
	}

	dnalloc = NBLOCK;
	nalloc = dnalloc;
	xy = (struct st_xy*) malloc(nalloc*sizeof(struct st_xy));

	for(n=0; pread(infd,xy+n,sizeof(struct st_xy));) {
		if(++n==nalloc) {
			nalloc += dnalloc;
			xy = (struct st_xy*)
					realloc(xy,nalloc*sizeof(struct st_xy));
			if(xy==NULL) err(__FILE__,__LINE__,"Can't reallocate %d bytes\n",
					nalloc*sizeof(struct st_xy));
		}
	}
	if(verbose) fprintf(stderr,"n=%d nalloc=%d\n",n,nalloc);

	for(i=0,xmin=MAXFLOAT,ymin=MAXFLOAT,xmax= -MAXFLOAT,ymax= -MAXFLOAT;i<n;i++) {
		xmax=MAX(xmax,xy[i].x);
		ymax=MAX(ymax,xy[i].y);
		xmin=MIN(xmin,xy[i].x);
		ymin=MIN(ymin,xy[i].y);
	}
	if(axis_flag) {
		if(xmax<0.0) xmax = 0.0;
		if(ymax<0.0) ymax = 0.0;
		if(xmin>0.0) xmin = 0.0;
		if(ymin>0.0) ymin = 0.0;
	}

D 3
	xsize = 5.0;	fgetpar("xsize",&xsize);
	ysize = 5.0;	fgetpar("ysize",&ysize);
E 3
I 3
	xsize = 4.0;	fgetpar("xsize",&xsize);
	ysize = 4.0;	fgetpar("ysize",&ysize);
E 3

	wx = xmax-xmin;
	wy = ymax-ymin;

	xscale = (wx!=0.0)?(xsize/wx):1.0;
	yscale = (wy!=0.0)?(ysize/wy):1.0;

	if(verbose) fprintf(stderr,"RAW: xscale=%f yscale=%f\n",xscale,yscale);

D 3
	scale  = MIN(xscale,yscale); fgetpar("scale",&scale);
E 3
I 3
	scale  = MIN(xscale,yscale);
	fgetpar("scale",&scale);
E 3

	if( !fgetpar("xscale",&xscale) && !fgetpar("xsize",&xsize) &&
	    !fgetpar("yscale",&yscale) && !fgetpar("ysize",&ysize) ) {
D 3
		xscale = scale;
		yscale = scale;
E 3
I 3
		if(!raw) {
			xscale = scale;
			yscale = scale;
		}
E 3
	}

	if(verbose)
		fprintf(stderr,
		"xmin=%f\txmax=%f\nymin=%f\tymax=%f\nxscale=%f\tyscale=%f\tscale=%f\n"
			,xmin,xmax,ymin,ymax,xscale,yscale,scale);

	for(i=0;i<n;i++) {
		xy[i].x = (xy[i].x-xmin)*xscale;
		xy[i].y = (xy[i].y-ymin)*yscale;
	}

	swx = wx*xscale;
	swy = wy*yscale;

	sxzero = -xmin*xscale;
	syzero = -ymin*yscale;

	xo = 1.0;	fgetpar("xo",&xo);
	yo = 1.0;	fgetpar("yo",&yo);

	set0(xo,yo);
	setscl(1.0,1.0);

	/* FRAME */
	margin = 0.25;
	yellow();
	umove(-margin,-margin);
	udraw(swx+margin,-margin);
	udraw(swx+margin,swy+margin);
	udraw(-margin,swy+margin);
	udraw(-margin,-margin);

D 3
	uText(-2.0*margin,0.5*swy,6,90,ylabel);
	uText(0.5*swx,swy+2.0*margin,6,0,xlabel);
E 3
I 3
/* 	uText(-2.0*margin,0.5*swy,6,90,ylabel); */
/* 	uText(0.5*swx,swy+2.0*margin,6,0,xlabel); */
E 3

	sprintf(label,"%g",xmin);
D 3
	utext(0.0,swy+margin+0.2,6,90,label);
E 3
I 3
/* 	utext(0.0,swy+margin+0.2,6,90,label); */
E 3
	umove(0.0,swy+margin); udraw(0.0,swy+margin+0.1);

	sprintf(label,"%g",xmax);
D 3
	utext(swx,swy+margin+0.2,6,90,label);
E 3
I 3
/* 	utext(swx,swy+margin+0.2,6,90,label); */
E 3
	umove(swx,swy+margin); udraw(swx,swy+margin+0.1);

	sprintf(label,"%g",ymin);
D 3
	utext(swx+margin+0.2,0.0,6,0,label);
E 3
I 3
/* 	utext(swx+margin+0.2,0.0,6,0,label); */
E 3
	umove(swx+margin,0.0); udraw(swx+margin+0.1,0.0);

	sprintf(label,"%g",ymax);
D 3
	utext(swx+margin+0.2,swy,6,0,label);
E 3
I 3
/* 	utext(swx+margin+0.2,swy,6,0,label); */
E 3
	umove(swx+margin,swy); udraw(swx+margin+0.1,swy);

	/* AXIS */
	if(axis_flag) {
		umove(sxzero,-margin); udraw(sxzero,swy+margin);
		umove(-margin,syzero); udraw(swx+margin,syzero);
	}

	/* TITLE */
D 3
	uText(-4.0*margin,0.5*swy,8,90,title);
E 3
I 3
/* 	uText(-4.0*margin,0.5*swy,8,90,title); */
E 3

	/* DRAW THE CURVE */
	red();
	umove(xy[0].x,xy[0].y);
	for(i=0;i<n;i++) {
		if(lines)	udraw(xy[i].x,xy[i].y);
		if(lsymbol)
			utext(xy[i].x,xy[i].y,6,0,symbol+isymbol[i%nsymbol]);
		if(numbers) {
			sprintf(symbol,"%d",i);
			utext(xy[i].x,xy[i].y,6,0,symbol);
		}
		if(cdots) ucircle(xy[i].x,xy[i].y,radius);
	}
	exit(0);
}

ucircle(x,y,r)
/*
 * plot an circle arount (x,y) with radius r,
 */
float x,y,r;
{
	float dalpha,alpha,scalex,scaley;
	int n,i;

	/* get the scale */
	getscls(&scalex,&scaley);

	n = 100*r*sqrt(fabs(scalex*scaley));
	if(n>1000) n=1000;
	if(n<10) n=10;
	dalpha = 6.283185/n;
/*	if(fill) {
		xp = (float *) malloc((unsigned) (n*4));
		yp = (float *) malloc((unsigned) (n*4));
		for(i=0;i<n;i++) {
			alpha = dalpha*i;
			xp[i] = x+r*cos(alpha);
			yp[i] = y+r*sin(alpha);
		}
		uarea(xp,yp,n);
		free((char *) xp); free((char *) yp);
	} else { */
		umove(x+r,y);
		for(i=1;i<=n;i++) {
			alpha = dalpha*i;
			udraw(x+r*cos(alpha),y+r*sin(alpha));
		}
/* 	} */
}
E 1
