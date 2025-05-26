char *sdoc = 
"XWPICK - X WIGgle-trace plot and pick of f(x1,x2) via Bitmap\n"
"\n"
"xwpick n1= [optional parameters] <binaryfile\n"
"\n"
"X Functionality:\n"
"Button 1	Zoom with rubberband box\n"
"Button 2	Show mouse (x1,x2) coordinates while pressed\n"
"Button 3	Same as a key			\n"
"Q key		Quit (can also use Motif Action button)\n"
"a key		Append current mouse (x1,x2) location to end of picks \n"
"i key          Insert current mouse (x1,x2) location into middle of picks \n"
"d key          Delete current mouse (x1,x2) location from picks \n"
"p key          Display picks \n"
"s key          Append saved mouse (x1,x2) picks to pick file \n"
"c key          Clear all the previous mouse (x1,x2) picks  \n"
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
"wt=1                   =0 for no wiggle-trace; =1 for wiggle-trace\n"
"va=1                   =0 for no variable-area; =1 for variable-area fill\n"
"nbpi=72                number of bits per inch at which to rasterize\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"xbox=50                x in pixels of upper left corner of window\n"
"ybox=50                y in pixels of upper left corner of window\n"
"wbox=550               width in pixels of window\n"
"hbox=700               height in pixels of window\n"
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
"labelfont=Erg14        font name for axes labels\n"
"title=                 title of plot\n"
"titlefont=Rom22        font name for title\n"
"labelcolor=blue        color for axes labels\n"
"titlecolor=red         color for title\n"
"gridcolor=blue         color for grid lines\n"
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"mpicks=stderr          file to save mouse picks (picks will be appended to \n"
"                         end of the file. (default: standard error output) \n"
"mpcolor=red            color of picks on screen \n"
"pcard=XPICK            name of the card to write picks (1-5 characters)\n"
"ppos=1                 panel position of the picks (cdp if input cdp gather)\n"
"porder=0               order of picks to output(0=x1 x2; 1=x2 x1)      \n"
"x1pscale=1.            scale to apply to x1 picks before output to pick file\n"
"x2pscale=1.            scale to apply to x2 picks before output to pick file\n"
"NOTE: \n"
"picks output card format: \n"
"1---5---10---15---20---25---30---35---40---45---50---55---60---65---70 \n"
"pcard      ppos       p11  p21  p12  p22  p13  p23  p14  p24  p15  p25 \n"
"\n"
"Author:	Zhiming Li		      		10/91		\n"
"\n";

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
        char *pcard, int ppos, int porder, float *x1picks, float *x2picks,
        float x1pscale, float x2pscale, int npicks, int fex);
void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *mpcolor,
        FILE *mpicksfp, int x, int y, int width, int height,
        float x1begb, float x1endb, float x2begb, float x2endb,
        char *pcard, int ppos, int porder, float *x1picks, float *x2picks,
        float x1pscale, float x2pscale, int *npicks, GC gc, int pkey, 
	int *savebg, float p2beg, float p2end);

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
		i,j,nx,ny,
		imageOutOfDate,winwidth=-1,winheight=-1,
		showloc=0,porder,ppos,npicks=0,savebg,pkey,fex;
	float labelsize,titlesize,perc,clip,xcur,bias,
		d1,f1,d2,f2,*z,*temp,*x2,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1begb,x1endb,x2begb,x2endb,p2beg,p2end,
		*x1picks,*x2picks,x1pscale,x2pscale;
	char *label1="",*label2="",*title="",
		*labelfont="Erg14",*titlefont="Rom22",
		*styles="seismic",*grid1s="none",*grid2s="none",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="blue",keybuf[256],*mpicks,
		*pcard="XPICK",*mpcolor="red";
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

	if (!getparstring("pcard", &pcard)) pcard = "XPICK";
        if (!getparint("porder", &porder)) porder = 0;
        if (!getparint("ppos", &ppos)) ppos = 1;
        if (!getparfloat("x1pscale", &x1pscale)) x1pscale = 1.;
        if (!getparfloat("x2pscale", &x2pscale)) x2pscale = 1.;
        /* 256 picks maximum */
        x1picks = (float*) malloc(256*sizeof(float));
        x2picks = (float*) malloc(256*sizeof(float));


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
	getparstring("title",&title);
	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("seismic",styles)) style = SEISMIC;
	else err("Sorry, only style=seismic is currently available!");
	getparstring("titlecolor",&titlecolor);
	getparstring("labelcolor",&labelcolor);
	getparstring("gridcolor",&gridcolor);
	getparstring("mpcolor",&mpcolor);
	
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
			
			/* clear the background */
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
			/* add the pick to the pick-saved buffers */
			pkey = 99;
                        if (keysym==XK_a) {
				pkey = 0;
                        /* delete the pick */
                        } else if (keysym==XK_d) {
				pkey = 1;
                        /* insert the pick */
                        } else if (keysym==XK_i) {
				pkey = 2;
                        /* display the pick */
                        } else if (keysym==XK_p) {
				pkey = 3;
                        } else if (keysym==XK_c) {
				pkey = 5;
			}
			if(pkey<=5) {
                                xMousePicks(dpy,win,event,style,
					mpcolor,
                                        mpicksfp, x,y,width,height,
                                        x1begb,x1endb,x2begb,x2endb,
                                        pcard,ppos,porder,x1picks,x2picks,
                                        x1pscale,x2pscale,&npicks,gci,pkey,
					&savebg,p2beg,p2end);
			/* output the picks */
                        } else if (keysym==XK_s) {
                                xMousePrint(dpy,win,event,style,
                                        mpicksfp, x,y,width,height,
                                        x1begb,x1endb,x2begb,x2endb,
                                        pcard,ppos,porder,x1picks,x2picks,
                                        x1pscale,x2pscale,npicks,fex);
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
					 mpcolor,
                               	         mpicksfp, x,y,width,height,
                               	         x1begb,x1endb,x2begb,x2endb,
                                       	 pcard,ppos,porder,x1picks,x2picks,
                                         x1pscale,x2pscale,&npicks,gci,pkey,
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
			wt,nbpr,bits);
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
        char *pcard, int ppos, int porder, float *x1picks, float *x2picks,
        float x1pscale, float x2pscale, int npicks, int fex)
{
        int ic,ip,np;
        int p1,p2;
	static int first=1;

        /* output picks */
	if(first==1 && fex==0) {
		first = 999;
        	fprintf(mpicksfp,
"1---5---10---15---20---25---30---35---40---45---50---55---60---65---70\n");
        	fprintf(mpicksfp,
"pcard      ppos       px1  py1  px2  py2  px3  py3  px4  py4  px5  py5\n");
        	fprintf(mpicksfp,"\n");
	}
        for (ic=0;ic<(npicks+4)/5;ic++) {
           if( (ic+1)*5 < npicks ) {
              np = 5; 
           }
           else {
              np = npicks - ic*5;
           }
           fprintf(mpicksfp, "%-5s%10d     ",pcard,ppos);
           for(ip=0;ip<np;ip++) {
              if(porder==0) {
                 p1 = x1picks[ic*5+ip]*x1pscale;
                 p2 = x2picks[ic*5+ip]*x2pscale;
              } else {
                 p1 = x2picks[ic*5+ip]*x2pscale;
                 p2 = x1picks[ic*5+ip]*x1pscale;
              }
              fprintf(mpicksfp,"%5d%5d",p1,p2);
           }
           fprintf(mpicksfp,"\n");
        }

        fflush(mpicksfp);
}


void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char *mpcolor,
        FILE *mpicksfp, int x, int y, int width, int height,
        float x1begb, float x1endb, float x2begb, float x2endb,
        char *pcard, int ppos, int porder, float *x1picks, float *x2picks,
        float x1pscale, float x2pscale, int *npicks, GC gc, int pkey, 
	int *savebg, float p2beg, float p2end)
{
        float x1,x2;
        int ipicks;
        int xx1,yy1,xx2,yy2,temp,ip;
        int dismin,dis;
        int ipmin, ipins;
        static int fs=1;
	GC gcp;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
        int scr;


        /* save bitmap of window to background for retrival */
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

        /* save to x1picks and x2picks */
        if( pkey == 0 ) {
                ipicks = *npicks;
                x1picks[ipicks] = x1;
                x2picks[ipicks] = x2;
                *npicks = *npicks + 1;
        /* delete nearest picks from x1picks and x2picks*/
        } else if ( pkey==1 && *npicks > 0 ) {
                ipicks = *npicks;
		dismin=(x1-x1picks[0])*(x1-x1picks[0])+
                       (x2-x2picks[0])*(x2-x2picks[0]);
                ipmin = 0;
                for(ip=1;ip<ipicks;ip++) {
                        dis=(x1-x1picks[ip])*(x1-x1picks[ip])+
                                (x2-x2picks[ip])*(x2-x2picks[ip]);
                        if(dis<dismin) {
                                dismin = dis;
                                ipmin = ip;
                        }
                }
                ipicks = ipicks - 1;
                for(ip=ipmin;ip<ipicks;ip++) {
                        x1picks[ip] = x1picks[ip+1];
                        x2picks[ip] = x2picks[ip+1];
                }
                *npicks = ipicks;
        /* insert the pick into x1picks and x2picks */
        } else if ( pkey==2 ) {
                ipicks = *npicks;
                if (porder==0) {
                        dismin=(x1-x1picks[0])*(x1-x1picks[0]);
			ipmin = 0;
                        for(ip=1;ip<ipicks;ip++) {
                                dis=(x1-x1picks[ip])*(x1-x1picks[ip]);
                                if(dis<dismin) {
                                        dismin = dis;
                                        ipmin = ip;
                                }
                        }
                        if(x1>x1picks[ipmin]) {
                                ipins = ipmin+1;
                                ipicks +=1;
                                for(ip=ipicks;ip>ipins;ip--) {
                                        x1picks[ip] = x1picks[ip-1];
                                        x2picks[ip] = x2picks[ip-1];
                                }
                                x1picks[ipins] = x1;
                                x2picks[ipins] = x2;
                        } else if(x1<x1picks[ipmin]) {
                                ipins = ipmin;
                                ipicks +=1;
                                for(ip=ipicks;ip>ipins;ip--) {
                                        x1picks[ip] = x1picks[ip-1];
					x2picks[ip] = x2picks[ip-1];
                                }
                                x1picks[ipins] = x1;
                                x2picks[ipins] = x2;
                        }
                } else {
                        dismin=(x2-x2picks[0])*(x2-x2picks[0]);
                        ipmin = 0;
                        for(ip=1;ip<ipicks;ip++) {
                                dis=(x2-x2picks[ip])*(x2-x2picks[ip]);
                                if(dis<dismin) {
                                        dismin = dis;
                                        ipmin = ip;
                                }
                        }
                        if(x2>x2picks[ipmin]) {
                                ipins = ipmin+1;
				ipicks +=1;
                                for(ip=ipicks;ip>ipins;ip--) {
                                        x1picks[ip] = x1picks[ip-1];
                                        x2picks[ip] = x2picks[ip-1];
                                }
                                x1picks[ipins] = x1;
                                x2picks[ipins] = x2;
                        } else if(x2<x2picks[ipmin]) {
                                ipins = ipmin;
                                ipicks +=1;
                                for(ip=ipicks;ip>ipins;ip--) {
                                        x1picks[ip] = x1picks[ip-1];
                                        x2picks[ip] = x2picks[ip-1];
                                }
                                x1picks[ipins] = x1;
                                x2picks[ipins] = x2;
                        }
                }
                *npicks = ipicks;
        }
	/* delete all previous picks */
	if (pkey == 5) {
		ipicks = *npicks;
		for(ip=0;ip<ipicks;ip++) {
			x1picks[ip] = 0.;
			x2picks[ip] = 0.;
		}
		ipicks = 0;
		*npicks = 0;
	}
	ipicks = *npicks;
        /* draw lines between picks */
	if(ipicks>1) {
        	XClearWindow(dpy, win);
		/* get screen */
                scr = DefaultScreen(dpy);
                /* determine window's current colormap */
                XGetWindowAttributes(dpy,win,&wa);
                cmap = wa.colormap;
		gcp = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,mpcolor,&scolor,&ecolor))
                        XSetForeground(dpy,gcp,ecolor.pixel);
                else
                        XSetForeground(dpy,gcp,1L);
                XSetLineAttributes(dpy,gcp,2,LineSolid,CapButt,JoinMiter);

                for(ip=1;ip<ipicks;ip++) {
                        if (style==NORMAL) {
                        xx1=(x1picks[ip-1]-x1begb)/(x1endb-x1begb)*width+x;
                        yy1=(x2picks[ip-1]-x2endb-p2end)/
			    (x2begb+p2beg-x2endb-p2end)*height+y;
                        xx2=(x1picks[ip]-x1begb)/(x1endb-x1begb)*width+x;
                        yy2=(x2picks[ip]-x2endb-p2end)/
			    (x2begb+p2beg-x2endb-p2end)*height+y;
                        } else {
                        yy1=(x1picks[ip-1]-x1begb)/(x1endb-x1begb)*height+y;
                        xx1=(x2picks[ip-1]-x2begb-p2end)/
		  	    (x2endb+p2end-x2begb-p2beg)*width+x;
                        yy2=(x1picks[ip]-x1begb)/(x1endb-x1begb)*height+y;
                        xx2=(x2picks[ip]-x2begb-p2beg)/
			    (x2endb+p2end-x2begb-p2beg)*width+x;
                        }
                        XDrawLine(dpy,win,gcp,xx1,yy1,xx2,yy2);
                }
		/* free resources before returning */
                XFreeGC(dpy,gcp);
        }
}
