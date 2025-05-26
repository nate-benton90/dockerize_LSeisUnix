char *sdoc = 
"MWPICK - Mute pick using X WIGgle-trace plot of f(x1,x2) via Bitmap\n"
"\n"
"mwpick n1= [optional parameters] <binaryfile\n"
"itoff=0   output offset-time when itoff=0; output time-offset when \n"
"          itoff=1 \n"
"\n"
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
	float x1, float x2,
	float y1, float y2,
	float *x1b, float *x2b,
	float *y1b, float *y2b);
static XImage *newBitmap (Display *dpy, int width, int height,
	int n1, float d1, float f1, int n2, float *x2, float *z,
	float x1beg, float x1end, float x2beg, float x2end,
	float xcur, float clip, int wt, int va,
	float *p2begp, float *p2endp);
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float p2beg, float p2end);
void xMousePrint(Display *dpy, Window win, XEvent event, int style,
        FILE *mpicksfp, int x, int y, int width, int height,
        float x1begb, float x1endb, float x2begb, float x2endb,
        char *pcard, int ppos, int itoff, 
        float *xtp, float *tp, int ntp,
        float *xbt, float *bt, int nbt, int fex);
void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *topmutecolr, char *bottommutecolr,
        FILE *mpicksfp, int x, int y, int width, int height,
        float x1begb, float x1endb, float x2begb, float x2endb,
        float *xtp, float *tp, int *ntp, float *xbt, float *bt, int *nbt,
        int pkey, int tkey, GC gc, int *savebg,
	float p2beg, float p2end);

static Display *dpy;
static Window win;

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n1tic,n2tic,nfloats,wt,va,
		i1,i2,grid1,grid2,style,
		nz,iz,verbose,
		xbox,ybox,wbox,hbox,
		xb,yb,wb,hb,
		x,y,width,height,
		i,j,nx,ny,itoff,
		imageOutOfDate,winwidth=-1,winheight=-1,
		showloc=0,ppos,ntp=0,nbt=0,tkey=0,pkey,fex, savebg;
	float labelsize,titlesize,perc,clip,xcur,bias,
		d1,f1,d2,f2,*z,*temp,*x2,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1begb,x1endb,x2begb,x2endb,p2beg,p2end,
		*xtp,*tp,*xbt,*bt;
	char *label1="time (or depth)",*label2="offset (or cdp)",
              title[80],*ttl,
		*labelfont="Erg14",*titlefont="Rom22",
		*styles="seismic",*grid1s="none",*grid2s="none",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="blue",keybuf[256],*mpicks,
		*topmutecolor="blue",*bottommutecolor="red",
		*pcard="MUTE", strppos[5];
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
	unsigned long black,white;

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

	if (!getparint("itoff",&itoff)) itoff = 0;

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
	if (verbose) warn("clip=%g\n",clip);

	/* get wiggle-trace-variable-area parameters */
	wt = 1;  getparint("wt",&wt);
	va = 1;  getparint("va",&va);
	xcur = 1.0;  getparfloat("xcur",&xcur);

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
	if (STREQ("seismic",styles)) style = SEISMIC;
	else err("Sorry, only style=seismic is currently available!");
	getparstring("titlecolor",&titlecolor);
	getparstring("labelcolor",&labelcolor);
	getparstring("gridcolor",&gridcolor);
	getparstring("topmutecolor",&topmutecolor);
	getparstring("bottommutecolor",&bottommutecolor);
	
	/* initialize zoom box parameters */
	x1begb = x1beg;  x1endb = x1end;
	x2begb = x2beg;  x2endb = x2end;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));
	scr = DefaultScreen(dpy);
	black = BlackPixel(dpy,scr);
	white = WhitePixel(dpy,scr);
	
	/* create window */
	win = xNewWindow(dpy,xbox,ybox,wbox,hbox,black,white,"xwpick");
		
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
	
	/* clear the window */
	XClearWindow(dpy,win);
					
	/* determine good size for axes box */
	xSizeAxesBox(dpy,win,
		labelfont,titlefont,style,
		&x,&y,&width,&height);
	
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
				if (image!=NULL) {
					free1(image->data);
					XDestroyImage(image);
				}
				image = newBitmap(dpy,width,height,
					n1,d1,f1,n2,x2,z,
					x1begb,x1endb,x2begb,x2endb,
					xcur,clip,wt,va,
					&p2beg,&p2end);
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
				x2begb,x2endb,p2beg,p2end,
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
					&savebg,p2beg,p2end);
			/* output the picks */
                        } else if (keysym==XK_s) {
                                xMousePrint(dpy,win,event,style,
                                        mpicksfp, x,y,width,height,
                                        x1begb,x1endb,x2begb,x2endb,
                                        pcard,ppos,itoff,
										xtp,tp,ntp,xbt,bt,nbt,fex);
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
			
				/* else, if new box has non-zero width
				/* if new box has zero width or height */
				} else {
			
					/* calculate new box parameters */
					zoomBox(x,y,width,height,
						xb,yb,wb,hb,
						x2begb,x2endb,
						x1begb,x1endb,
						&x2begb,&x2endb,
						&x1begb,&x1endb);
				}

				/* clear background */
                                if(savebg) XSetWindowBackground(dpy,win,0L);

				/* clear area and force an expose event */
				XClearArea(dpy,win,0,0,0,0,True);
			
				/* note that image is out of date */
				imageOutOfDate = 1;
			
			/* else if 2nd button down: display mouse coords */
			} else if (event.xbutton.button==Button2) {

				showloc = 1;
				xMouseLoc(dpy,win,event,style,showloc,
					  x,y,width,height,x1begb,x1endb,
					  x2begb,x2endb,p2beg,p2end);

			/* else if 3rd button down: track and pick */
	                } else if (event.xbutton.button==Button3) {
				pkey = 0;
                                xMousePicks(dpy,win,event,style,
					topmutecolor,bottommutecolor,
                                        mpicksfp, x,y,width,height,
                                        x1begb,x1endb,x2begb,x2endb,
                                        xtp,tp,&ntp,xbt,bt,&nbt,pkey,tkey,gci,
					&savebg,p2beg,p2end);
			} else {
				continue;
			}

		/* else if pointer has moved */
		} else if (event.type==MotionNotify) {
			
			/* if button2 down, show mouse location */
			if (showloc)
				xMouseLoc(dpy,win,event,style,True,
					x,y,width,height,x1begb,x1endb,
					x2begb,x2endb,p2beg,p2end);

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
	float x1, float x2,
	float y1, float y2,
	float *x1b, float *x2b,
	float *y1b, float *y2b)
{
	/* if width and/or height of box are zero, just copy values */
	if (wb==0 || hb==0) {
		*x1b = x1; *x2b = x2;
		*y1b = y1; *y2b = y2;
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
	
	/* determine box limits */
	*x1b = x1+(xb-x)*(x2-x1)/w;
	*x2b = x1+(xb+wb-x)*(x2-x1)/w;
	*y1b = y1+(yb-y)*(y2-y1)/h;
	*y2b = y1+(yb+hb-y)*(y2-y1)/h;
}

/* return pointer to new image bitmap of rasterized wiggles */
static XImage *newBitmap (Display *dpy, int width, int height,
	int n1, float d1, float f1, int n2, float *x2, float *z,
	float x1beg, float x1end, float x2beg, float x2end,
	float xcur, float clip, int wt, int va,
	float *p2begp, float *p2endp)
{
	int widthpad,nbpr,i1beg,i1end,if1r,n1r,b1fz,b1lz,i2,i,n2in;
	float x2min,x2max,p2beg,p2end,bscale,boffset,bxcur,bx2;
	unsigned char *bits;
	int scr=DefaultScreen(dpy);
	int endian;


	endian = 1;
        

	/* determine bitmap dimensions and allocate space for bitmap */
	widthpad = (1+(width-1)/(BitmapPad(dpy)/8))*BitmapPad(dpy)/8;
	nbpr = 1+(widthpad-1)/8;
	bits = ealloc1(nbpr*height,sizeof(unsigned char));
	for (i=0; i<nbpr*height; ++i) bits[i] = 0;

	/* determine number of traces that fall within axis 2 bounds */
	x2min = MIN(x2beg,x2end);
	x2max = MAX(x2beg,x2end);
	for (i2=0,n2in=0; i2<n2; i2++)
		if (x2[i2]>=x2min && x2[i2]<=x2max) n2in++;

	/* determine pads for wiggle excursion along axis 2 */
	xcur = fabs(xcur);
	if (n2in>1) xcur *= (x2max-x2min)/(n2in-1);
	p2beg = (x2end>x2beg)?-xcur:xcur;
	p2end = (x2end>x2beg)?xcur:-xcur;

	/* determine scale and offset to map x2 units to bitmap units */
	bscale = (width-1)/(x2end+p2end-x2beg-p2beg);
	boffset = -(x2beg+p2beg)*bscale;
	bxcur = xcur*bscale;

	/* adjust x1beg and x1end to fall on sampled values */
	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1-1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	/* determine first sample and number of samples to rasterize */
	if1r = MIN(i1beg,i1end);
	n1r = MAX(i1beg,i1end)-if1r+1;

	/* determine bits corresponding to first and last samples */
	b1fz = (x1end>x1beg)?0:height-1;
	b1lz = (x1end>x1beg)?height-1:0;

	/* rasterize traces */
	for (i2=0; i2<n2; i2++,z+=n1) {

		/* skip traces not in bounds */
		if (x2[i2]<x2min || x2[i2]>x2max) continue;

		/* determine bitmap coordinate of trace */
		bx2 = boffset+x2[i2]*bscale;

		/* rasterize one trace */
		rfwtva(n1r,&z[if1r],-clip,clip,va?0:clip,
			(int)(bx2-bxcur),(int)(bx2+bxcur),b1fz,b1lz,
			wt,nbpr,bits,endian);
	}

/* !!! inserted by Z. Li to fix problem of SUN's display colormap different 
       from rs-6000's colormap !!! */ 
	for (i=0; i<nbpr*height; ++i) bits[i] = 255 - bits[i];
	
	/* return axis 2 pads */
	*p2begp = p2beg;  *p2endp = p2end;
	
	/* return pointer to image */
	return XCreateImage(dpy,DefaultVisual(dpy,scr),
		1,XYBitmap,0,bits,widthpad,height,BitmapPad(dpy),nbpr);
}	

void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float p2beg, float p2end)
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
		x2 = p2end+x2endb+(p2beg+x2begb-x2endb-p2end)*
			(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = p2beg+x2begb+(p2end+x2endb-x2begb-p2beg)*
			(event.xmotion.x-x)/width;
	}

	/* draw string indicating mouse location */
	sprintf(string,"(%0.3g,%0.3g)",x1,x2);
	XTextExtents(fs,string,strlen(string),&dummy,&dummy,&dummy,&overall);
	XDrawImageString(dpy,win,gc,xoffset,yoffset+overall.ascent,
		string,strlen(string));
}

void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	char *pcard, int ppos, int itoff, 
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
	if(itoff==0) {
	fprintf(mpicksfp,
"NAME       ppos   px1   tp1   bp1   px2   tp2   bp2   px3   tp3   bp3 \n");
	} else {
	fprintf(mpicksfp,
"NAME       ppos   tp1   px1         tp2   px2         tp3   px3       \n");
	}
	
	fprintf(mpicksfp,"\n");
	}
	/* find output pick x positions */
	np2 = ntp + nbt; 
	xpout = (float*) malloc(np2*sizeof(float)); 
	tpout = (float*) malloc(np2*sizeof(float)); 
	btout = (float*) malloc(np2*sizeof(float)); 
	indx = (int*) malloc(np2*sizeof(int)); 

	if(itoff==0) {
		for(ip=0;ip<ntp;ip++) xpout[ip]=xtp[ip];
		for(ip=ntp;ip<np2;ip++) xpout[ip]=xbt[ip-ntp];
	} else {
		np2 = ntp;
		for(ip=0;ip<ntp;ip++) xpout[ip]=tp[ip];
	}
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
		if(itoff==0) {
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
		} else {
			bisear_(&ntp,&npout,tp,xpout,indx);
			for(ip=0;ip<npout;ip++) {
				np = indx[ip] - 1;
				if(xpout[ip] < tp[0]) {
					tpout[ip] = xtp[0];
				} else if ((xpout[ip] > tp[ntp-1]) || np >= ntp-1) {
					tpout[ip] = xtp[ntp-1];
				} else {
					res = (xpout[ip]-tp[np])/(tp[np+1]-tp[np]);
					tpout[ip] = xtp[np]+res*(xtp[np+1]-xtp[np]);
					fprintf(stderr," xtp=%g tp=%g \n",xtp[np],tp[np]);
				} 
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
	   fprintf(mpicksfp, "%-5s%10d",pcard,ppos);
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
	int pkey, int tkey, GC gc, int *savebg,
	float p2beg, float p2end)
{
	float x1,x2;
	int ipicks;
	int xx1,yy1,xx2,yy2,temp,ip;
	int dismin,dis;
	int ipmin, ipins;
	static int fs=1;
	GC gctop, gcbot;
	XGCValues *values;
	XColor scolor,ecolor;
	XWindowAttributes wa;
        Colormap cmap;
	int scr;

	/* if needed, save bitmap of window to background for retrival */ 
	if(*savebg) {
		fg2bg(dpy,win);
		*savebg = 0;
	}


	/* convert mouse location to (x1,x2) coordinates */

	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = p2end+x2endb+(p2beg+x2begb-x2endb-p2end)*
			(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = p2beg+x2begb+(p2end+x2endb-x2begb-p2beg)*
			(event.xmotion.x-x)/width;
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
	
		x2 = p2end+x2endb+(p2beg+x2begb-x2endb-p2end)*
			(event.xmotion.y-y)/height;

		for(ip=1;ip<*ntp;ip++) {
			yy1=(tp[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(xtp[ip-1]-x2begb-p2beg)/
			    (x2endb+p2end-x2begb-p2beg)*width+x;
			yy2=(tp[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(xtp[ip]-x2begb-p2beg)/
			    (x2endb+p2end-x2begb-p2beg)*width+x;
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
			xx1=(xbt[ip-1]-x2begb-p2beg)/
			    (x2endb+p2end-x2begb-p2beg)*width+x;
			yy2=(bt[ip]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(xbt[ip]-x2begb-p2beg)/
			    (x2endb+p2end-x2begb-p2beg)*width+x;
			XDrawLine(dpy,win,gcbot,xx1,yy1,xx2,yy2);
		}
		/* free resources before returning */
       		XFreeGC(dpy,gcbot);
	}
}

