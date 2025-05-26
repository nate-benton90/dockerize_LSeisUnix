char *sdoc = 
"MIPICK - Mute pick using X IMAGE plot of uniformly-sampled function f(x1,x2)\n"
"\n"
"mipick n1= [optional parameters] <binaryfile\n"
"\n"
"cmap=gray              gray, hue, or rgb colormaps may be specified\n"
"See mutepick \n"
"\n"
" author: Zhiming Li                            9/12/91                 \n"
"\n";

#include "comva.h"
#include "par.h"
#include "xplot.h"
#include <X11/Xatom.h>
#include <X11/keysym.h>

/* functions defined and used internally */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iby, float *y1b, float *y2b);
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out);
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb);
void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	char *pcard, int ppos, 
	float *xtp, float *tp, int ntp, 
	float *xbt, float *bt, int nbt, int fex);
void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *topmutecolr, char *bottommutecolr,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *xtp, float *tp, int *ntp, float *xbt, float *bt, int *nbt, 
	int pkey, int tkey, GC gc, int *savebg);

static Display *dpy;
static Window win;

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n1tic,n2tic,nfloats,
		i1,i2,grid1,grid2,style,
		n1c,n2c,i1beg,i1end,i2beg,i2end,i1c,i2c,
		nz,iz,i1step,i2step,verbose,
		xbox,ybox,wbox,hbox,
		xb,yb,wb,hb,
		x,y,width,height,
		i,j,nx,ny,nxb,nyb,ixb,iyb,
		imageOutOfDate,winwidth=-1,winheight=-1,
		showloc=0,ppos,ntp=0,nbt=0,tkey=0,pkey,fex,savebg;
	float labelsize,titlesize,perc,clip,bperc,wperc,bclip,wclip,
		d1,f1,d2,f2,*z,*temp,zscale,zoffset,zi,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1begb,x1endb,x2begb,x2endb,
		*xtp,*tp,*xbt,*bt;
	unsigned char *cz,*czp,*czb,*czbp,*czbi=NULL;
	char *label1="time (or depth)",*label2="offset (or cdp)",
		title[80],*ttl, 
		*labelfont="Erg14",*titlefont="Rom22",
		*styles="seismic",*grid1s="none",*grid2s="none",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="blue",*cmap="gray",keybuf[256],*mpicks,
		*topmutecolor="blue",*bottommutecolor="red",
		*pcard="MUTE";
	FILE *infp=stdin, *mpicksfp;
/*
	Display *dpy;
	Window win;
*/
	XEvent event;
	KeySym keysym;
	XComposeStatus keystat;
	XImage *image=NULL;
	GC gci;
	int scr;
	unsigned long black,white,pmin,pmax;

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

	/* set up file to save mouse picks */
	if (!getparstring("mpicks", &mpicks)) {
		mpicksfp = stderr;
		fex = 0;
	} else {
		/* if file exist */
		if((mpicksfp = fopen(mpicks,"r"))!=NULL) {
			fclose(mpicksfp);
			mpicksfp = efopen(mpicks,"a");
			fex = 1;
		} else {
			mpicksfp = efopen(mpicks,"w");
			fex = 0;
		}
	}
	if (!getparint("ppos", &ppos)) ppos = 1;


	/* 256 picks maximum */
	xtp = (float*) malloc(256*sizeof(float));
	tp = (float*) malloc(256*sizeof(float));
	xbt = (float*) malloc(256*sizeof(float));
	bt = (float*) malloc(256*sizeof(float));

	/* read binary data to be plotted */
	nz = n1*n2;
	z = ealloc1float(nz);
	if (fread(z,sizeof(float),nz,infp)!=nz)
		err("error reading input file");

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

	/* get colormap specification */
	getparstring("cmap",&cmap);

	/* get axes parameters */
	xbox = 50; getparint("xbox",&xbox);
	ybox = 50; getparint("ybox",&ybox);
	wbox = 550; getparint("wbox",&wbox);
	hbox = 700; getparint("hbox",&hbox);
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

	if(!getparstring("title",&ttl)) {
		ttl = (char*) malloc(30*sizeof(char));
                strcpy(ttl,"Mute Picking ");
	}
	
	sprintf(title,"%s at position = %5d \0",ttl,ppos);

	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;
	getparstring("titlecolor",&titlecolor);
	getparstring("labelcolor",&labelcolor);
	getparstring("gridcolor",&gridcolor);
	getparstring("topmutecolor",&topmutecolor);
        getparstring("bottommutecolor",&bottommutecolor);


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
	cz = ealloc1(n1c*n2c,sizeof(unsigned char));

	/* convert data to be imaged into signed characters */
	zscale = (wclip!=bclip)?255.0/(wclip-bclip):1.0e10;
	zoffset = -bclip*zscale;
	i1step = (i1end>i1beg)?1:-1;
	i2step = (i2end>i2beg)?1:-1;
	if (style==NORMAL) {
		for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
			czp = cz+n1c*n2c-(i2c+1)*n1c;
			for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	} else {
		czp = cz;
		for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
			for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	}
	free1float(z);
	
	/* initialize zoom box parameters */
	nxb = nx = (style==NORMAL ? n1c : n2c);
	nyb = ny = (style==NORMAL ? n2c : n1c);
	ixb = iyb = 0;
	czb = cz;
	x1begb = x1beg;  x1endb = x1end;
	x2begb = x2beg;  x2endb = x2end;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));
	scr = DefaultScreen(dpy);
	black = BlackPixel(dpy,scr);
	white = WhitePixel(dpy,scr);
	
	/* create window */
	win = xNewWindow(dpy,xbox,ybox,wbox,hbox,black,white,"xipick");

	/* if necessary, create private colormap with gray scale */
	if (STREQ(cmap,"gray")) {
		XSetWindowColormap(dpy,win,xCreateGrayColormap(dpy,win));
	} else if (STREQ(cmap,"hue")) {
		XSetWindowColormap(dpy,win,xCreateHueColormap(dpy,win));
	} else if (STREQ(cmap,"rgb")) {
                XSetWindowColormap(dpy,win,xCreateRGBColormap(dpy,win));
        }
	
	/* determine min and max pixels from standard colormap */
	pmin = xGetFirstPixel(dpy);
	pmax = xGetLastPixel(dpy);
		
	/* make GC for image */
	gci = XCreateGC(dpy,win,0,NULL);
	
	/* set normal event mask */
	XSelectInput(dpy,win,
		StructureNotifyMask |
		ExposureMask |
		KeyPressMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask |
		Button1MotionMask |
		Button2MotionMask);
	
	/* map window */
	XMapWindow(dpy,win);
					
	/* determine good size for axes box */
	xSizeAxesBox(dpy,win,
		labelfont,titlefont,style,
		&x,&y,&width,&height);
	
	/* clear the window */
	XClearWindow(dpy,win);
	
	/* note that image is out of date */
	imageOutOfDate = 1;
	savebg = 1;

	/* main event loop */
	while(True) {
		XNextEvent(dpy,&event);

		/* if window was resized */
		if (event.type==ConfigureNotify &&
			(event.xconfigure.width!=winwidth ||
			 event.xconfigure.height!=winheight)) {
			winwidth = event.xconfigure.width;
			winheight = event.xconfigure.height;
							
			/* determine good size for axes box */
			xSizeAxesBox(dpy,win,
				labelfont,titlefont,style,
				&x,&y,&width,&height);

			/* clear background */
                        if(savebg) XSetWindowBackground(dpy,win,0L);
			
			/* clear the window */
			XClearWindow(dpy,win);
			
			/* note that image is out of date */
			imageOutOfDate = 1;

		/* else if window exposed */
		} else if (event.type==Expose) {
			
			/* clear all expose events from queue */
			while (XCheckTypedEvent(dpy,Expose,&event));
			
			/* if necessary, make new image */
			if (imageOutOfDate) {
				if (czbi!=NULL) free1(czbi);
				czbi = newInterpBytes(nxb,nyb,czb,
					width,height);
				if (image!=NULL) XDestroyImage(image);
				image = xNewImage(dpy,pmin,pmax,
					width,height,czbi);
				imageOutOfDate = 0;
			}

			/* clear background */
                        if(savebg) XSetWindowBackground(dpy,win,0L);
	
			/* draw image (before axes so grid lines visible) */
			XPutImage(dpy,win,gci,image,0,0,x,y,
				image->width,image->height);
			
			/* draw axes on top of image */
			xDrawAxesBox(dpy,win,
				x,y,width,height,
				x1begb,x1endb,0.0,0.0,
				d1num,f1num,n1tic,grid1,label1,
				x2begb,x2endb,0.0,0.0,
				d2num,f2num,n2tic,grid2,label2,
				labelfont,title,titlefont,
				labelcolor,titlecolor,gridcolor,
				style);

		/* else if key down */
		} else if (event.type==KeyPress) {

			XLookupString(&event,keybuf,0,&keysym,&keystat);
			/* add, delete or insert picks */
			pkey =99;
			if (keysym==XK_a) {
				pkey = 0; 
				tkey = 0;
			} else if ( keysym==XK_A ) {
				pkey = 0; 
				tkey = 1;
			} else if ( keysym==XK_d ) {
				pkey = 1; 
				tkey = 0;
			} else if ( keysym==XK_D ) {
				pkey = 1; 
				tkey = 1;
			} else if ( keysym==XK_i ) {
				pkey = 2; 
				tkey = 0;
			} else if ( keysym==XK_I ) {
				pkey = 2; 
				tkey = 1;
			} else if (keysym==XK_p) {
				pkey = 3;
			}
			if (pkey<=3) {
				xMousePicks(dpy,win,event,style,
					topmutecolor,bottommutecolor,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					xtp,tp,&ntp,xbt,bt,&nbt,pkey,tkey,gci,
					&savebg);
			} else if (keysym==XK_s) {
				xMousePrint(dpy,win,event,style,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					pcard,ppos,xtp,tp,ntp,xbt,bt,nbt,fex);
			} else if (keysym==XK_Q) {
			/* This is the exit from the event loop */
				break;
			} else {
				continue;
			}


		/* else if button down (1 == zoom, 2 == mouse tracking */
		} else if (event.type==ButtonPress) {
			/* if 1st button: zoom */
			if (event.xbutton.button==Button1) {

				savebg = 1;
				/* track pointer and get new box */
				xRubberBox(dpy,win,event,&xb,&yb,&wb,&hb);
			
				/* if new box has tiny width or height */
				if (wb<4 || hb<4) {
				
					/* reset box to initial values */
					x1begb = x1beg;
					x1endb = x1end;
					x2begb = x2beg;
					x2endb = x2end;
					nxb = nx;
					nyb = ny;
					ixb = iyb = 0;
					if (czb!=cz) free1(czb);
					czb = cz;
			
				/* else, if new box has non-zero width
				/* and height */
				} else {
			
					/* calculate new box parameters */
					if (style==NORMAL) {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x1begb,x1endb,
						    nyb,iyb,x2endb,x2begb,
						    &nxb,&ixb,&x1begb,&x1endb,
						    &nyb,&iyb,&x2endb,&x2begb);
					} else {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x2begb,x2endb,
						    nyb,iyb,x1begb,x1endb,
						    &nxb,&ixb,&x2begb,&x2endb,
						    &nyb,&iyb,&x1begb,&x1endb);
					}
			
					/* make new bytes in zoombox */
					if (czb!=cz) free1(czb);
					czb = ealloc1(nxb*nyb,
						sizeof(signed char));
					for (i=0,czbp=czb; i<nyb; i++) {
					    czp = cz+(iyb+i)*nx+ixb;
					    for (j=0; j<nxb; j++)
						    *czbp++ = *czp++; 
					}
				}
			
				/* clear area and force an expose event */
				XClearArea(dpy,win,0,0,0,0,True);
			
				/* note that image is out of date */
				imageOutOfDate = 1;
		
			/* else if 2nd button down: display mouse coords */
			} else if (event.xbutton.button==Button2) {

				showloc = 1;
				xMouseLoc(dpy,win,event,style,showloc,
					  x,y,width,height,x1begb,x1endb,
					  x2begb,x2endb);
			} else if (event.xbutton.button==Button3) {
                                pkey = 0;
				xMousePicks(dpy,win,event,style,
					topmutecolor,bottommutecolor,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					xtp,tp,&ntp,xbt,bt,&nbt,pkey,tkey,gci,
					&savebg);
			} else {
				continue;
			}

		/* else if pointer has moved */
		} else if (event.type==MotionNotify) {
			
			/* if button2 down, show mouse location */
			if (showloc)
				xMouseLoc(dpy,win,event,style,True,
					x,y,width,height,
					x1begb,x1endb,x2begb,x2endb);

		/* else if button2 released, stop tracking */
		} else if (event.type==ButtonRelease &&
			   event.xbutton.button==Button2) {
			showloc = 0;
		}

	} /* end of event loop */

	/* close connection to X server */
	XCloseDisplay(dpy);
	return EXIT_SUCCESS; 
}

/* update parameters associated with zoom box */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iyb, float *y1b, float *y2b)
{
	/* if width and/or height of box are zero, just copy values */
	if (wb==0 || hb==0) {
		*nxb = nx; *ixb = ix; *x1b = x1; *x2b = x2;
		*nyb = ny; *iyb = iy; *y1b = y1; *y2b = y2;
		return;		
	} 
	
	/* clip box */
	if (xb<x) {
		wb -= x-xb;
		xb = x;
	}
	if (yb<y) {
		hb -= y-yb;
		yb = y;
	}
	if (xb+wb>x+w) wb = x-xb+w;
	if (yb+hb>y+h) hb = y-yb+h;
	
	/* determine number of samples in rubber box (at least 2) */
	*nxb = MAX(nx*wb/w,2);
	*nyb = MAX(ny*hb/h,2);
	
	/* determine indices of first samples in box */
	*ixb = ix+(xb-x)*(nx-1)/w;
	*ixb = MIN(*ixb,ix+nx-*nxb);
	*iyb = iy+(yb-y)*(ny-1)/h;
	*iyb = MIN(*iyb,iy+ny-*nyb);
	
	
	/* determine box limits to nearest samples */
	*x1b = x1+(*ixb-ix)*(x2-x1)/(nx-1);
	*x2b = x1+(*ixb+*nxb-1-ix)*(x2-x1)/(nx-1);
	*y1b = y1+(*iyb-iy)*(y2-y1)/(ny-1);
	*y2b = y1+(*iyb+*nyb-1-iy)*(y2-y1)/(ny-1);
}

/* return pointer to new interpolated array of bytes */
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out)
{
	unsigned char *bout;
	float d1in,d2in,d1out,d2out,f1in,f2in,f1out,f2out;
	
	f1in = f2in = f1out = f2out = 0.0;
	d1in = d2in = 1.0;
	d1out = d1in*(float)(n1in-1)/(float)(n1out-1);
	d2out = d2in*(float)(n2in-1)/(float)(n2out-1);
	bout = ealloc1(n1out*n2out,sizeof(unsigned char));
	intl2b(n1in,d1in,f1in,n2in,d2in,f2in,bin,
		n1out,d1out,f1out,n2out,d2out,f2out,bout);
	return bout;
}
	
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb)
{
	XGCValues values;
	static XFontStruct *fs=NULL;
	static XCharStruct overall;
	static GC gc;
	int dummy,xoffset=10,yoffset=10;
	float x1,x2;
	char string[256];

	/* if first time, get font attributes and make gc */
	if (fs==NULL) {
		fs = XLoadQueryFont(dpy,"fixed");
		gc = XCreateGC(dpy,win,0,&values);
		XSetFont(dpy,gc,fs->fid);
		overall.width = 1;
		overall.ascent = 1;
		overall.descent = 1;
	}

	/* erase previous string */
	XClearArea(dpy,win,xoffset,yoffset,
		overall.width,overall.ascent+overall.descent,False);

	/* if not showing, then return */
	if (!show) return;

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	/* draw string indicating mouse location */
	sprintf(string,"(%0.4g,%0.4g)",x1,x2);
	XTextExtents(fs,string,strlen(string),&dummy,&dummy,&dummy,&overall);
	XDrawImageString(dpy,win,gc,xoffset,yoffset+overall.ascent,
		string,strlen(string));
}

void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	char *pcard, int ppos, 
	float *xtp, float *tp, int ntp, float *xbt, float *bt, int nbt, int fex)
{
	int ic,ip,np,npout,np2; 
	int p1,p2,p3;
	float *xpout, *tpout, *btout, res;
	int *indx;
	static int first=1;
	/* output picks */
	if(first==1 && fex==0 ) {
	first = 999;
	fprintf(mpicksfp,
"1---5---10---15----21----27----33----39----45----51----57----63----69----75\n");
	fprintf(mpicksfp,
"NAME  ppos        px1   tp1   bp1   px2   tp2   bt2   px3   tp3   bp3\n");
	
	fprintf(mpicksfp,"\n");
	}

	/* find output pick x positions */
	np2 = ntp + nbt; 
	xpout = (float*) malloc(np2*sizeof(float)); 
	tpout = (float*) malloc(np2*sizeof(float)); 
	btout = (float*) malloc(np2*sizeof(float)); 
	indx = (int*) malloc(np2*sizeof(int)); 

	for(ip=0;ip<ntp;ip++) xpout[ip]=xtp[ip];
	for(ip=ntp;ip<np2;ip++) xpout[ip]=xbt[ip-ntp];
	/* sort x position in increasing order */ 
	qksort(np2,xpout);
	/* delete x positions that are too close together */
	tpout[0]=xpout[0];
	npout = 0;
	for(ip=1;ip<np2;ip++) {
		if(fabs(xpout[ip]-tpout[npout])>1.) {
			npout = npout + 1; 
			tpout[npout] = xpout[ip];
		}
	}
	npout=npout+1;
	for(ip=0;ip<npout;ip++) xpout[ip] = tpout[ip]; 

	/* interpolate tp and bt at output x locations */
	if ( ntp > 0 ) {
		bisear_(&ntp,&npout,xtp,xpout,indx);
		for(ip=0;ip<npout;ip++) {
			np = indx[ip] - 1;
			if(xpout[ip] < xtp[0]) {
				tpout[ip] = tp[0];
			} else if ((xpout[ip] > xtp[ntp-1]) || np >= ntp-1) {
				tpout[ip] = tp[ntp-1];
			} else {
				res = (xpout[ip]-xtp[np])/(xtp[np+1]-xtp[np]);
				tpout[ip] = tp[np]+res*(tp[np+1]-tp[np]);
			} 
		}
	}
	if ( nbt > 0 ) {	
		bisear_(&nbt,&npout,xbt,xpout,indx);
		for(ip=0;ip<npout;ip++) {
			np = indx[ip] - 1;
			if(xpout[ip] < xbt[0]) {
				btout[ip] = bt[0];
			} else if ((xpout[ip] > xbt[nbt-1]) || np >= nbt-1) {
				btout[ip] = bt[nbt-1];
			} else {
				res = (xpout[ip]-xbt[np])/(xbt[np+1]-xbt[np]);
				btout[ip] = bt[np]+res*(bt[np+1]-bt[np]);
			} 
		}
	}

	for (ic=0;ic<(npout+2)/3;ic++) {
	   if( (ic+1)*3 < npout ) {
	      np = 3;  
	   }
	   else { 
	      np = npout - ic*3;
	   }
	   fprintf(mpicksfp, "%-5s%5d     ",pcard,ppos);
	   for(ip=0;ip<np;ip++) {
	         p1 = xpout[ic*3+ip];	
	         p2 = tpout[ic*3+ip];	
	         p3 = btout[ic*3+ip];	
		 if ( ntp > 0 && nbt >> 0 ) { 
	         	fprintf(mpicksfp,"%6d%6d%6d",p1,p2,p3);
		 } else if ( ntp > 0 && nbt <= 0 ) {
	         	fprintf(mpicksfp,"%6d%6d      ",p1,p2);
		 } else if ( ntp <= 0 && nbt > 0 ) {
	         	fprintf(mpicksfp,"%6d      %6d",p1,p3);
		 }
	   }
	   fprintf(mpicksfp,"\n");
	}
	fflush(mpicksfp);
	free(xpout);
	free(tpout);
	free(btout);
	free(indx);
}

void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *topmutecolor, char *bottommutecolor,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *xtp, float *tp, int *ntp, float *xbt, float *bt, int *nbt, 
	int pkey, int tkey, GC gc, int *savebg)
{
	float x1,x2;
	int ipicks;
	int xx1,yy1,xx2,yy2,temp,ip;
	int dismin,dis;
	int ipmin, ipins;
	
	GC gctop, gcbot;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
        int scr;

	

	/* first time, save bitmap of window to background for retrival */ 
	if(*savebg) {
		fg2bg(dpy,win);
		*savebg = 0;
	}

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	/* save x1 and x2 */
	if( pkey == 0 ) {
		if(tkey==0) {
			ipicks = *ntp;
			tp[ipicks] = x1;
			xtp[ipicks] = x2;
			*ntp = *ntp + 1;
		} else if (tkey==1) {
			ipicks = *nbt;
			bt[ipicks] = x1;
			xbt[ipicks] = x2;
			*nbt = *nbt + 1;
		}
	/* delete nearest picks from x1 and x2 picks*/
	} else if ( pkey==1 ) {
		if( tkey==0 && *ntp > 0 ) {
			ipicks = *ntp;	
			dismin = (x1-tp[0])*(x1-tp[0])+
		       	         (x2-xtp[0])*(x2-xtp[0]);
			ipmin = 0;
			for(ip=1;ip<ipicks;ip++) {
		   		dis=(x1-tp[ip])*(x1-tp[ip])+
		       		    (x2-xtp[ip])*(x2-xtp[ip]);
				if(dis<dismin) {
					dismin = dis;
					ipmin = ip;
				}
			}
			ipicks = ipicks - 1;
			for(ip=ipmin;ip<ipicks;ip++) {
				tp[ip] = tp[ip+1];
				xtp[ip] = xtp[ip+1];
			}
			*ntp = ipicks;
		} else if(tkey==1 && *nbt > 0 ) {
			ipicks = *nbt;	
			dismin = (x1-bt[0])*(x1-bt[0])+
		       	         (x2-xbt[0])*(x2-xbt[0]);
			ipmin = 0;
			for(ip=1;ip<ipicks;ip++) {
		   		dis=(x1-bt[ip])*(x1-bt[ip])+
		       		    (x2-xbt[ip])*(x2-xbt[ip]);
				if(dis<dismin) {
					dismin = dis;
					ipmin = ip;
				}
			}
			ipicks = ipicks - 1;
			for(ip=ipmin;ip<ipicks;ip++) {
				bt[ip] = bt[ip+1];
				xbt[ip] = xbt[ip+1];
			}
			*nbt = ipicks;
		}
	/* insert the pick into x1 and x2 picks */
	} else if ( pkey==2 ) {
		if ( tkey == 0 ) {
			ipicks = *ntp;	
			dismin=(x2-xtp[0])*(x2-xtp[0]);
			ipmin = 0;
			for(ip=1;ip<ipicks;ip++) {
		   		dis=(x2-xtp[ip])*(x2-xtp[ip]);
				if(dis<dismin) {
					dismin = dis;
					ipmin = ip;
				}
			}
			if(x2>xtp[ipmin]) { 
				ipins = ipmin+1;
				ipicks +=1;
				for(ip=ipicks;ip>ipins;ip--) {
					xtp[ip] = xtp[ip-1];
					tp[ip] = tp[ip-1];
				}
				tp[ipins] = x1;
				xtp[ipins] = x2;
			} else if(x2<xtp[ipmin]) {
				ipins = ipmin;
				ipicks +=1;
				for(ip=ipicks;ip>ipins;ip--) {
					xtp[ip] = xtp[ip-1];
					tp[ip] = tp[ip-1];
				}
				xtp[ipins] = x2;
				tp[ipins] = x1;
			}
			*ntp = ipicks;
		}
		if ( tkey == 1 ) {
			ipicks = *nbt;	
			dismin=(x2-xbt[0])*(x2-xbt[0]);
			ipmin = 0;
			for(ip=1;ip<ipicks;ip++) {
		   		dis=(x2-xbt[ip])*(x2-xbt[ip]);
				if(dis<dismin) {
					dismin = dis;
					ipmin = ip;
				}
			}
			if(x2>xbt[ipmin]) { 
				ipins = ipmin+1;
				ipicks +=1;
				for(ip=ipicks;ip>ipins;ip--) {
					xbt[ip] = xbt[ip-1];
					bt[ip] = bt[ip-1];
				}
				xbt[ipins] = x2;
				bt[ipins] = x1;
			} else if(x2<xbt[ipmin]) {
				ipins = ipmin;
				ipicks +=1;
				for(ip=ipicks;ip>ipins;ip--) {
					xbt[ip] = xbt[ip-1];
					bt[ip] = bt[ip-1];
				}
				xbt[ipins] = x2;
				bt[ipins] = x1;
			}
			*nbt = ipicks;
		}
	}
	/* draw lines between picks */
       	XClearWindow(dpy, win);

	if (*ntp > 1 || *nbt > 1 ) {
                /* get screen */
                scr = DefaultScreen(dpy);
                /* determine window's current colormap */
                XGetWindowAttributes(dpy,win,&wa);
                cmap = wa.colormap;
        }

	if(*ntp>1) {
		/* create graphics contexts */
                gctop = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,topmutecolor,&scolor,&ecolor))
                        XSetForeground(dpy,gctop,ecolor.pixel);
                else
                        XSetForeground(dpy,gctop,1L);
                XSetLineAttributes(dpy,gctop,2,LineSolid,CapButt,JoinMiter);

		for(ip=1;ip<*ntp;ip++) {
			yy1=(tp[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(xtp[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(tp[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(xtp[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gctop,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gctop);

	}

	if(*nbt>1) {
		/* create graphics contexts */
                gcbot = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,bottommutecolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcbot,ecolor.pixel);
                else
                        XSetForeground(dpy,gcbot,1L);
                XSetLineAttributes(dpy,gcbot,2,LineSolid,CapButt,JoinMiter);

		for(ip=1;ip<*nbt;ip++) {
			yy1=(bt[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(xbt[ip-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(bt[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(xbt[ip]-x2begb)/(x2endb-x2begb)*width+x;
			XDrawLine(dpy,win,gcbot,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
                XFreeGC(dpy,gcbot);

	}
}
