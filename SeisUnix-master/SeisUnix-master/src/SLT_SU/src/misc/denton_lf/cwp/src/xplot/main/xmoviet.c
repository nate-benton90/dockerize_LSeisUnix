/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "par.h"
#include "xplot.h"
#include <X11/Xatom.h>

/* self-documentation */
char *sdoc = 
"XMOVIE - X MOVIE (repeating loop version)\n"
"\n"
"xmovie <binaryfile n1= n2= n3= [optional parameters]\n"
"\n"
"Required Parameters:\n"
"n1                     number of samples in 1st (fast) dimension\n"
"n2                     number of samples in 2nd (slow) dimension\n"
"n3                     number of samples in 3rd (frame) dimension\n"
"\n"
"Optional Parameters:\n"
"r1=1                   integer replication factor for 1st dimension\n"
"r2=1                   integer replication factor for 2nd dimension\n"
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [bclip,wclip] are clipped\n"
"wclip=-clip            data values outside of [bclip,wclip] are clipped\n"
"cmap=gray              gray, hue, or default colormaps may be specified\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"\n"
"Note: The clip values are determined from the first frame of the movie.\n"
"\n";

main (int argc, char **argv)
{
	int n1,n2,n3,i1,i2,i3,nz,iz,r1,r2,verbose,
		width,height,widthpad,ix,ip,iy,ir,id;
	float perc,bperc,wperc,clip,bclip,wclip,*data,*z,*temp,**cube,
		dat,dmin,dmax,pmin,pmax,pbase,pscale;
	char *cmap="gray";
	unsigned char pix,*pdat,*prep;
	Display *dpy;
	Window root,win;
	XEvent event;
	int scr;
	XImage *image;
	GC gc;
	XGCValues *values;
	XSizeHints size_hints;
	XStandardColormap scmap;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters */
	if (!getparint("n1",&n1)) err("Must specify n1!\n");
	if (!getparint("n2",&n2)) err("Must specify n2!\n");
	if (!getparint("n3",&n3)) err("Must specify n3!\n");
	if (!getparint("r1",&r1)) r1 = 1;
	if (!getparint("r2",&r2)) r2 = 1;

	/* get colormap specification */
	getparstring("cmap",&cmap);

	/* allocate space */
	cube = ealloc2float(n1*n2,n3);

	/* read data */
	if (fread(cube[0],sizeof(float),n1*n2*n3,stdin)!=n1*n2*n3)
		err("Couldn't read %d floats--check n1,n2,n3\n",n1*n2*n3);

	/* set pointer to the first frame for determining clips */
	z = cube[0];
	nz = n1*n2;

        /* if necessary, determine clips from percentiles */
	if (getparfloat("clip",&clip)) {
		bclip = clip;
		wclip = -clip;
	}
	if ((!getparfloat("bclip",&bclip) || !getparfloat("wclip",&wclip)) &&
		!getparfloat("clip",&clip)) {
 		perc = 100.0; getparfloat("perc",&perc);
		temp = ealloc1float(nz);
		for (iz=0; iz<nz; iz++)
			temp[iz] = z[iz];
		if (!getparfloat("bclip",&bclip)) {
			bperc = perc; getparfloat("bperc",&bperc);
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
		free(temp);
	}						     

	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("bclip=%g wclip=%g",bclip,wclip);

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy,scr);

	/* determine width and height of image */
	width = n2*r2;
	height = n1*r1;
	widthpad = (1+(width-1)/(BitmapPad(dpy)/8))*BitmapPad(dpy)/8;

	/* allocate space for image bytes */
	pdat = ealloc1(n1*n2,sizeof(unsigned char));
	prep = ealloc1(widthpad*height,sizeof(unsigned char));

	/* make an X window */
	win = XCreateSimpleWindow(dpy,root,0,0,widthpad,height,
		0,BlackPixel(dpy,scr),WhitePixel(dpy,scr));
	size_hints.flags = PPosition|PSize;
	size_hints.x = 0;
	size_hints.y = 0;
	size_hints.width = widthpad;
	size_hints.height = height;
	XSetStandardProperties(dpy,win,"XMOVIE","XMOVIE",None,0,0,&size_hints);
	XMapWindow(dpy,win);

	/* make an X image and determine pixel scaling */
	image = XCreateImage(dpy,DefaultVisual(dpy,scr),
		DefaultDepth(dpy,scr),ZPixmap,
		0,prep,widthpad,height,BitmapPad(dpy),widthpad);
	gc = XCreateGC(dpy,win,0,values);

	/* if necessary, create private colormap with gray scale */
	/*
	if (STREQ(cmap,"gray")) {
		XSetWindowColormap(dpy,win,xCreateGrayColormap(dpy,win));
	} else if (STREQ(cmap,"hue")) {
		XSetWindowColormap(dpy,win,xCreateHueColormap(dpy,win));
	}
	*/
	
	/* determine min and max pixels from colormap */
	/*
	pmin = xGetFirstPixel(dpy);
	pmax = xGetLastPixel(dpy);
	*/
	pmin = 0;
	pmax = 216;

	/* determine pixel scaling */
	pscale = (pmax-pmin)/(wclip-bclip);
	pbase = pmin-bclip*pscale;
	dmin = MIN(bclip,wclip);
	dmax = MAX(bclip,wclip);

	/* loop forever */
	while (TRUE) {

		/* loop over frames */
		for (i3=0; i3<n3; ++i3) {

			/* set data pointer to the next frame */
			data = cube[i3];

			/* map data to pixels */
			for (ip=id=i2=0; i2<n2; ++i2) {
				for (i1=0; i1<n1; ++i1) {
					dat = data[id++];
					if (dat<dmin) dat = dmin;
					if (dat>dmax) dat = dmax;
					pix = pbase+dat*pscale;
					if (pix<pmin) pix = pmin;
					if (pix>pmax) pix = pmax;
					pdat[ip++] = pix;
				}
			}
	
			/* transpose, replicate, and pad scanlines */
			for (ix=iy=0; iy<height; ++iy) {
				for (i2=0,ip=iy/r1; i2<n2; ++i2,ip+=n1)
					for (ir=0; ir<r2; ++ir)
						prep[ix++] = pdat[ip];
				pix = prep[ix-1];
				for (ip=width; ip<widthpad; ++ip)
					prep[ix++] = pix;
			}

			/* display image to window */
			XPutImage(dpy,win,gc,image,0,0,0,0,widthpad,height);
		}
	}

	/* close connection to X server */
	XCloseDisplay(dpy);
}
