/* Copyright (c) Colorado School of Mines, 1990. */
/* All rights reserved.                       */

char *sdoc =
"XGRAPH - X GRAPHer\n"
"Graphs n[i] pairs of (x,y) coordinates, for i = 1 to nplot.\n"
"\n"
"xgraph n= [optional parameters] <binaryfile \n"
"\n"
"Required Parameters:\n"
"n                      array containing number of points per plot\n"
"\n"
"Optional Parameters:\n"
"nplot=number of n's    number of plots\n"
"d1=0.0,...             x sampling intervals (0.0 if x coordinates input)\n"
"f1=0.0,...             first x values (not used if x coordinates input)\n"
"d2=0.0,...             y sampling intervals (0.0 if y coordinates input)\n"
"f2=0.0,...             first y values (not used if y coordinates input)\n"
"linewidth=1,1,...      line widths in pixels (0 for no lines)\n"
"linecolor=2,3,...      line colors (black=0, white=1, 2,3,4 = RGB, ...)\n"
"mark=0,1,2,3,...       indices of marks used to represent plotted points\n"
"marksize=0,0,...       size of marks in pixels (0 for no marks)\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"x2beg=x2min            value at which axis 2 begins\n"
"x2end=x2max            value at which axis 2 ends\n"
"\n"
"Optional resource parameters (defaults taken from resource database):\n"
"width=                 width in pixels of window\n"
"height=                height in pixels of window\n"
"nTic1=                 number of tics per numbered tic on axis 1\n"
"grid1=                 grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=                label on axis 1\n"
"nTic2=                 number of tics per numbered tic on axis 2\n"
"grid2=                 grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=                label on axis 2\n"
"labelFont=             font name for axes labels\n"
"title=                 title of plot\n"
"titleFont=             font name for title\n"
"titleColor=            color for title\n"
"axesColor=             color for axes\n"
"gridColor=             color for grid lines\n"
"style=                 normal (axis 1 horizontal, axis 2 vertical) or\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
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
"Note:	n1 and n2 are acceptable aliases for n and nplot, respectively.\n"
"\n"
"Example:\n"
"xgraph n=50,100,20 d1=2.5,1,0.33 <datafile\n"
"  plots three curves with equally spaced x values in one plot frame\n"
"  x1-coordinates are x1(i) = f1+i*d1 for i = 1 to n (f1=0 by default)\n"
"  number of x2's and then x2-coordinates for each curve are read\n"
"  sequentially from datafile.\n"
"\n"
"AUTHOR:  Dave Hale and Lydia Deng, Colorado School of Mines, 01/17/91\n"
"MODIFIED:  Dave Hale, Colorado School of Mines, 04/18/91\n"
"           fixed conversion of command line resource parameters\n"
"           Zhiming Li,       	12/18/92	\n"
"           added XIO error handler		\n"  
"\n";

#include <stdio.h>
#include "par.h"
#include "Xcwp/Xcwp.h"
#include "Xcwp/Axes.h"
#include <X11/keysym.h>

/* client data structures for callbacks */
typedef struct ExposeCDStruct {
	int nplot;
	int *n;
	float **data;
	int *linewidth;
	int *linecolor;
	int *mark;
	int *marksize;
} ExposeCD;

/* callback functions */

/*
void resizeCB (XcwpAxesWidget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
void exposeCB (XcwpAxesWidget w, 
	ExposeCD *clientdata,
	XcwpAxesCallbackStruct *calldata);
void inputCB (XcwpAxesWidget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
*/

void resizeCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
void exposeCB (Widget w, 
	ExposeCD *clientdata,
	XcwpAxesCallbackStruct *calldata);
void inputCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
void xQexit(Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);

/* functions defined and used internally */
static void xDrawMark(Display *dpy, Drawable d, GC gc,
	int x, int y, int index, int size);

/* XIO error handler */
int xIoErrorHandler();

#define NPMAX 5000	/* Arbitrary maximum number of plots allowed	*/

#define NMARKS 9
#define MPLUS 0
#define MASTERISK 1
#define MCROSS 2
#define MTRIANGLE 3
#define MSQUARE 4
#define MCIRCLE 5
#define MFILLEDTRIANGLE 6
#define MFILLEDSQUARE 7
#define MFILLEDCIRCLE 8
#define FLT_MAX MAXFLOAT

main (int argc, char **argv)
{
	int nplot,n[NPMAX],nn,iplot,npoint,nTic1,nTic2,
		i,j,npar,ni,bbox[4],nd1,nf1,nd2,nf2,
		linewidth[NPMAX],linecolor[NPMAX],
		mark[NPMAX],marksize[NPMAX],width,height;
	float d1[NPMAX],f1[NPMAX],d2[NPMAX],f2[NPMAX],
		x1beg,x2beg,x1end,x2end,
		x1min,x1max,x2min,x2max,
		xsize,ysize,xscale,yscale;
	char *label1="",*label2="",*title="",
		*labelFont="",*titleFont="",
		*axesColor="",*gridColor="",*titleColor="",
		*style="normal",*grid1="none",*grid2="none";
	float **data;
	XrmValue from,to;
	ExposeCD exposeCD;
	Widget toplevel,axes;
	Arg args[100];
	int nargs;

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
	data = (float **)malloc(nplot*sizeof(float*));
	x2max = x1max = -FLT_MAX;
	x2min = x1min =  FLT_MAX;
	for (iplot=0; iplot<nplot; ++iplot) {

		npoint = n[iplot];
		data[iplot] = (float*)malloc(npoint*2*sizeof(float));

		/* read data for this plot */
		if (d1[iplot] && d2[iplot]) { /* straight line */
			float x,y;
			register int i;
			float *pdata=data[iplot];

			for (i=0; i<npoint; ++i) {
				x = f1[iplot] + i*d1[iplot];
				y = f2[iplot] + i*d2[iplot];
				x1max = MAX(x, x1max);
				x1min = MIN(x, x1min);
				x2max = MAX(y, x2max);
				x2min = MIN(y, x2min);
				*pdata++ = x;
				*pdata++ = y;
			}
		} else if (d1[iplot]) { /* equally spaced x's */
			float x,y;
			register int i;
			float *pdata=data[iplot];

			for (i=0; i<npoint; ++i) {
				efread(&y, FSIZE, 1, stdin);
				x = f1[iplot] + i*d1[iplot];
				x1max = MAX(x, x1max);
				x1min = MIN(x, x1min);
				x2max = MAX(y, x2max);
				x2min = MIN(y, x2min);
				*pdata++ = x;
				*pdata++ = y;
			}
		} else if (d2[iplot]) { /* equally spaced y's */
			float x,y;
			register int i;
			float *pdata=data[iplot];

			for (i=0; i<npoint; ++i) {
				efread(&x, FSIZE, 1, stdin);
				y = f2[iplot] + i*d2[iplot];
				x1max = MAX(x, x1max);
				x1min = MIN(x, x1min);
				x2max = MAX(y, x2max);
				x2min = MIN(y, x2min);
				*pdata++ = x;
				*pdata++ = y;
			}
		} else { /* pairs */
			float x,y;
			register int i;
			float *pdata=data[iplot];

			for (i=0; i<npoint; ++i) {
				efread(&x, FSIZE, 1, stdin);
				efread(&y, FSIZE, 1, stdin);
				x1max = MAX(x, x1max);
				x1min = MIN(x, x1min);
				x2max = MAX(y, x2max);
				x2min = MIN(y, x2min);
				*pdata++ = x;
				*pdata++ = y;
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

	/* initialize toolkit intrinsics and set toplevel parameters */
	toplevel = XtInitialize(argv[0],"Xgraph",NULL,0,&argc,argv);
	nargs = 0;
	if (getparint("width",&width))
		{XtSetArg(args[nargs],XtNwidth,width); nargs++;}
	if (getparint("height",&height))
		{XtSetArg(args[nargs],XtNheight,height); nargs++;}
	XtSetValues(toplevel,args,nargs);

	/* create axes and set axes parameters */
	axes = XtCreateManagedWidget("axes",xcwpAxesWidgetClass, 
		toplevel,NULL,0);
	nargs = 0;
	if (getparstring("grid1",&grid1)) {
		from.addr = (caddr_t)grid1;
		XtConvert(axes,XtRString,&from,XcwpRAxesGrid,&to);
		if (to.addr) XtSetArg(args[nargs],XtNgrid1,*((int*)to.addr));
		nargs++;
	}
	if (getparstring("grid2",&grid2)) {
		from.addr = (caddr_t)grid2;
		XtConvert(axes,XtRString,&from,XcwpRAxesGrid,&to);
		if (to.addr) XtSetArg(args[nargs],XtNgrid2,*((int*)to.addr));
		nargs++;
	}
	if (getparint("nTic1",&nTic1))
		{XtSetArg(args[nargs],XtNnTic1,nTic1); nargs++;}
	if (getparint("nTic2",&nTic2))
		{XtSetArg(args[nargs],XtNnTic2,nTic2); nargs++;}
	if (getparstring("label1",&label1))
		{XtSetArg(args[nargs],XtNlabel1,label1); nargs++;}
	if (getparstring("label2",&label2))
		{XtSetArg(args[nargs],XtNlabel2,label2); nargs++;}
	if (getparstring("title",&title))
		{XtSetArg(args[nargs],XtNtitle,title); nargs++;}
	if (getparstring("style",&style)) {
		from.size = strlen(style);  from.addr = (caddr_t)style;
		XtConvert(axes,XtRString,&from,XcwpRAxesStyle,&to);
		if (to.addr) XtSetArg(args[nargs],XtNstyle,*((int*)to.addr));
		nargs++;
	}
	if (getparstring("axesColor",&axesColor)) {
		from.addr = (caddr_t)axesColor;
		XtConvert(axes,XtRString,&from,XtRPixel,&to);
		if (to.addr) XtSetArg(args[nargs],XtNaxesColor,
			*((unsigned long*)to.addr));
		nargs++;
	}
	if (getparstring("gridColor",&gridColor)) {
		from.addr = (caddr_t)gridColor;
		XtConvert(axes,XtRString,&from,XtRPixel,&to);
		if (to.addr) XtSetArg(args[nargs],XtNgridColor,
			*((unsigned long*)to.addr));
		nargs++;
	}
	if (getparstring("titleColor",&titleColor)) {
		from.addr = (caddr_t)titleColor;
		XtConvert(axes,XtRString,&from,XtRPixel,&to);
		if (to.addr) XtSetArg(args[nargs],XtNtitleColor,
			*((unsigned long*)to.addr));
		nargs++;
	}
	if (getparstring("labelFont",&labelFont)) {
		from.addr = (caddr_t)labelFont;
		XtConvert(axes,XtRString,&from,XtRFont,&to);
		if (to.addr) XtSetArg(args[nargs],XtNlabelFont,
			*((Font*)to.addr));
		nargs++;
	}
	if (getparstring("titleFont",&titleFont)) {
		from.addr = (caddr_t)titleFont;
		XtConvert(axes,XtRString,&from,XtRFont,&to);
		if (to.addr) XtSetArg(args[nargs],XtNtitleFont,
			*((Font*)to.addr));
		nargs++;
	}
	XtSetValues(axes,args,nargs);
	x1beg = x1min; getparfloat("x1beg",&x1beg);
	x1end = x1max; getparfloat("x1end",&x1end);
	x2beg = x2min; getparfloat("x2beg",&x2beg);
	x2end = x2max; getparfloat("x2end",&x2end);
	XcwpSetAxesValues(axes,x1beg,x1end,x2beg,x2end);

	/* get graph parameters */
	for (i=0; i<nplot; i++) {
		mark[i] = i%NMARKS;
		marksize[i] = 0;
		linewidth[i] = 1;
		linecolor[i] = 2+i%6;
	}
	if (npar=getparint("mark",mark))
		for (j=npar; j<nplot; j++)
			mark[j] = mark[npar-1];
	if (npar=getparint("marksize",marksize))
		for (j=npar; j<nplot; j++)
			marksize[j] = marksize[npar-1];
	if (npar=getparint("linewidth",linewidth))
		for (j=npar; j<nplot; j++)
			linewidth[j] = linewidth[npar-1];
	if (npar=getparint("linecolor",linecolor))
		for (j=npar; j<nplot; j++)
			linecolor[j] = linecolor[npar-1];

	/* add callbacks to axes widget */
	XtAddCallback(axes,XtNresizeCallback,resizeCB,NULL);
	exposeCD.nplot = nplot;
	exposeCD.n = &n[0];
	exposeCD.data = data;
	exposeCD.linewidth = &linewidth[0];
	exposeCD.linecolor = &linecolor[0];
	exposeCD.mark = &mark[0];
	exposeCD.marksize = &marksize[0];
	XtAddCallback(axes,XtNexposeCallback,exposeCB,&exposeCD);
	XtAddCallback(axes,XtNinputCallback,inputCB,NULL);
	XtAddCallback(axes,XtNinputCallback,xQexit,NULL);

	/* avoid XIO message */
	XSetIOErrorHandler(xIoErrorHandler);

	/* realize and go */
	XtRealizeWidget(toplevel);
	XtMainLoop();
}

void resizeCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *ca)
{
	/* printf("resize callback\n"); */
}

#define NCOLOR 8
static char *color[NCOLOR] = {
	"black",
	"white",
	"red",
	"green",
	"blue",
	"cyan",
	"magenta",
	"yellow",
};

void exposeCB (Widget w, 
	ExposeCD *cd,
	XcwpAxesCallbackStruct *ca)
{
	int nplot=cd->nplot;
	int *n=cd->n;
	float **data=cd->data;
	int *linewidth=cd->linewidth;
	int *linecolor=cd->linecolor;
	int *mark=cd->mark;
	int *marksize=cd->marksize;
	Position x=ca->x,y=ca->y;
	Dimension width=ca->width,height=ca->height;
	float x1beg=ca->x1beg,x1end=ca->x1end,
		x2beg=ca->x2beg,x2end=ca->x2end;
	int style=ca->style;
	Display *dpy=XtDisplay(w);
	Window win=XtWindow(w);
	XWindowAttributes wa;
	Colormap cmap;
	XColor scolor,ecolor;
	XRectangle rect;
	long black=BlackPixelOfScreen(XtScreen(w));
	GC gc;
	int iplot,icolor;
	float xmin,xmax,ymin,ymax,xbase,xscale,ybase,yscale;
	static int firstexpose=1;

	/* if first expose, check style and, if necessary, swap x,y coords */
	if (firstexpose) {
		if (style==XcwpSEISMIC) {
			for (iplot=0; iplot<nplot; ++iplot) {
				int ni=n[iplot],i;
				for (i=0; i<ni; ++i) {
					float temp=data[iplot][2*i];
					data[iplot][2*i] = data[iplot][2*i+1];
					data[iplot][2*i+1] = temp;
				}
			}
		}
		firstexpose = 0;
	}
	
	/* determine current colormap */
	XGetWindowAttributes(dpy,win,&wa);
	cmap = wa.colormap;

	/* create GC */
	gc = XCreateGC(dpy,win,0L,NULL);
	
	/* set clip to axes box */
	rect.x = x;  rect.y = y;  rect.width = width;  rect.height = height;
	XSetClipRectangles(dpy,gc,0,0,&rect,1,Unsorted);

	/* determine min and max coordinates */
	xmin = x;
	xmax = x+(float)width;
	ymin = y;
	ymax = y+(float)height;

	/* determine base and scale factors */
	if (style==XcwpNORMAL) {
		xscale = (float)width/(x1end-x1beg);
		xbase = x-x1beg*xscale;
		yscale = (float)height/(x2beg-x2end);
		ybase = y-x2end*yscale;
	} else {
		xscale = (float)width/(x2end-x2beg);
		xbase = x-x2beg*xscale;
		yscale = (float)height/(x1end-x1beg);
		ybase = y-x1beg*yscale;
	}
	
	/* loop over plots */
	for (iplot=0; iplot<nplot; ++iplot) {

		/* set line width */
		XSetLineAttributes(dpy,gc,
			linewidth[iplot],
			LineSolid,
			CapButt,
			JoinMiter);
		
		/* set line color */
		icolor = linecolor[iplot];
		if (icolor<0) icolor = 0;
		else if (icolor>=NCOLOR) icolor = NCOLOR-1;
		if (XAllocNamedColor(dpy,cmap,color[icolor],&scolor,&ecolor))
			XSetForeground(dpy,gc,ecolor.pixel);
		else
			XSetForeground(dpy,gc,black);
		
		/* draw lines between points */
		if (linewidth[iplot]!=0) {
			int xi,yi,in,xilast,yilast,inlast,i,ni=n[iplot];
			float *pdata;
			
			pdata = data[iplot];
			xi = xbase+xscale*(*pdata++);
			yi = ybase+yscale*(*pdata++);
			in = (xi>=xmin && xi<=xmax && yi>=ymin && yi<=ymax);
			for (i=1; i<ni; ++i) {
				xilast = xi;
				yilast = yi;
				inlast = in;
				xi = xbase+xscale*(*pdata++);
				yi = ybase+yscale*(*pdata++);
				in = (xi>=xmin && xi<=xmax && 
					yi>=ymin && yi<=ymax);
				if (in || inlast)
					XDrawLine(dpy,win,gc,
						xilast,yilast,xi,yi);
			}
		}

		/* draw marks at points */
		if (marksize[iplot]!=0) {
			int xi,yi,in,i,ni=n[iplot];
			float *pdata;
			
			pdata = data[iplot];
			for (i=0; i<ni; ++i) {
				xi = xbase+xscale*(*pdata++);
				yi = ybase+yscale*(*pdata++);
				in = (xi>=xmin && xi<=xmax && 
					yi>=ymin && yi<=ymax);
				if (in) xDrawMark(dpy,win,gc,xi,yi,
					mark[iplot],marksize[iplot]);
			}
		}
	}

	/* free GC */
	XFreeGC(dpy,gc);
}

void inputCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *ca)
{
	int x=ca->x,y=ca->y,width=ca->width,height=ca->height;
	float x1beg=ca->x1beg,x1end=ca->x1end,x2beg=ca->x2beg,x2end=ca->x2end;
	int style=ca->style;
	XEvent *event=ca->event;
	int xb,yb,wb,hb;
	float x1begn,x1endn,x2begn,x2endn;
	static int firstinput=1;
	static float x1begs,x1ends,x2begs,x2ends;

	/* if first input, save initial axes limits */
	if (firstinput) {
		x1begs = x1beg;
		x1ends = x1end;
		x2begs = x2beg;
		x2ends = x2end;
		firstinput = 0;
	}
	
	/* track pointer and get rubber box */
	xRubberBox(XtDisplay(w),XtWindow(w),*event,&xb,&yb,&wb,&hb);

	/* if new box has zero width or height */
	if (wb==0 || hb==0) {

		/* restore initial limits */
		XcwpSetAxesValues(w,x1begs,x1ends,x2begs,x2ends);
	
	/* else if non-zero zoom box */
	} else {
	
		/* clip box */
		if (xb<x) {
			wb -= x-xb;
			xb = x;
		}
		if (yb<y) {
			hb -= y-yb;
			yb = y;
		}
		if (xb+wb>x+width) wb = x-xb+width;
		if (yb+hb>y+height) hb = y-yb+height;
	
		/* determine axes limits */
		if (style==XcwpNORMAL) {
			x1begn = x1beg+(xb-x)*(x1end-x1beg)/width;
			x1endn = x1beg+(xb+wb-x)*(x1end-x1beg)/width;
			x2begn = x2end+(yb+hb-y)*(x2beg-x2end)/height;
			x2endn = x2end+(yb-y)*(x2beg-x2end)/height;
		} else {
			x1endn = x1beg+(yb+hb-y)*(x1end-x1beg)/height;
			x1begn = x1beg+(yb-y)*(x1end-x1beg)/height;
			x2begn = x2beg+(xb-x)*(x2end-x2beg)/width;
			x2endn = x2beg+(xb+wb-x)*(x2end-x2beg)/width;
		}
	
		/* set axes limits */
		XcwpSetAxesValues(w,x1begn,x1endn,x2begn,x2endn);
	}
	
	/* force an expose event */
	XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}

static void xDrawMark(Display *dpy, Drawable d, GC gc,
	int x, int y, int index, int size)
{
	XPoint points[4];
	switch (index%NMARKS) {
	case MPLUS: /* plus */
		XDrawLine(dpy,d,gc,x-(int)(0.5*size),y,x+(int)(0.5*size),y);
		XDrawLine(dpy,d,gc,x,y-(int)(0.5*size),x,y+(int)(0.5*size));
		break;
	case MASTERISK: /* asterisk */
		XDrawLine(dpy,d,gc,x-(int)(0.5*size),y,x+(int)(0.5*size),y);
		XDrawLine(dpy,d,gc,x-(int)(0.25*size),y-(int)(0.433*size),
			x+(int)(0.25*size),y+(int)(0.433*size));
		XDrawLine(dpy,d,gc,x+(int)(0.25*size),y-(int)(0.433*size),
			x-(int)(0.25*size),y+(int)(0.433*size));
		break;
	case MCROSS: /* X */
		XDrawLine(dpy,d,gc,x-(int)(0.5*size),y-(int)(0.5*size),
			x+(int)(0.5*size),y+(int)(0.5*size));
		XDrawLine(dpy,d,gc,x+(int)(0.5*size),y-(int)(0.5*size),
			x-(int)(0.5*size),y+(int)(0.5*size));
		break;
	case MTRIANGLE: /* triangle */
		points[0].x = x-(int)(0.5*size);  
		points[0].y = y+(int)(0.25*size);
		points[1].x = x+(int)(0.5*size);  
		points[1].y = y+(int)(0.25*size);
		points[2].x = x;           
		points[2].y = y-(int)(0.559*size);
		points[3].x = x-(int)(0.5*size);  
		points[3].y = y+(int)(0.25*size);
		XDrawLines(dpy,d,gc,points,4,CoordModeOrigin);
		break;
	case MSQUARE: /* square */
		XDrawRectangle(dpy,d,gc,x-(int)(0.5*size),y-(int)(0.5*size),
			size,size);
		break;
	case MCIRCLE: /* circle */
		XDrawArc(dpy,d,gc,x-(int)(0.5*size),y-(int)(0.5*size),
			size,size,0,360*64);
		break;
	case MFILLEDTRIANGLE: /* filled triangle */
		points[0].x = x-(int)(0.75*size);  
		points[0].y = y+(int)(0.375*size);
		points[1].x = x+(int)(0.75*size);  
		points[1].y = y+(int)(0.375*size);
		points[2].x = x;           
		points[2].y = y-(int)(0.838*size);
		points[3].x = x-(int)(0.75*size);  
		points[3].y = y+(int)(0.375*size);
		XFillPolygon(dpy,d,gc,points,4,Convex,CoordModeOrigin);
		break;
	case MFILLEDSQUARE: /* filled square */
		XFillRectangle(dpy,d,gc,x-(int)(0.5*size),y-(int)(0.5*size),
			size,size);
		break;
	case MFILLEDCIRCLE: /* filled circle */
		XFillArc(dpy,d,gc,x-(int)(0.5*size),y-(int)(0.5*size),
			size,size,0,360*64);
		break;
	}
}

int xIoErrorHandler() {
	exit(0);
}

void xQexit(Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata) {

	Display *dpy=XtDisplay(w);
        Window win=XtWindow(w);
	XEvent event;
        KeySym keysym;
        XComposeStatus keystat;
	char keybuf[256];
	
	
	fprintf(stderr,"enter xQexit \n");
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

	while(True) {
		XNextEvent(dpy,&event);
		fprintf(stderr,"event.type=%d \n",event.type);
		if (event.type==KeyPress) {
			XLookupString(&event,keybuf,0,&keysym,&keystat);
			if (keysym==XK_Q) {
				fprintf(stderr,"Q pressed \n");
				xIoErrorHandler(); 
			}
		} else {
			break;
		}
	}
	/* force an expose event */
	XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}
