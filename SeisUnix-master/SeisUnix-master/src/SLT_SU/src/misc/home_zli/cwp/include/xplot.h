/* Copyright (c) Colorado School of Mines, 1990. */
/* All rights reserved.                       */

/* include file for X graphics */

#ifndef XPLOT_H
#define XPLOT_H


/* INCLUDES */

#include <X11/Xlib.h>
#include <X11/Xutil.h>


/* DEFINES */

/* axes drawing */
#define NONE 0
#define DOT 1
#define DASH 2
#define SOLID 3
#define NORMAL 0
#define SEISMIC 1


/* FUNCTION PROTOTYPES */
#ifdef __cplusplus  /* if C++, specify external C linkage */
extern "C" {
#endif



/* windows */
Window xNewWindow (Display *dpy, int x, int y, int width, int height,
	int border, int background, char *name);

/* axes drawing */
void xDrawAxesBox (Display *dpy, Window win,
	int x, int y, int width, int height,
	float x1beg, float x1end, float p1beg, float p1end,
	float d1num, float f1num, int n1tic, int grid1, char *label1,
	float x2beg, float x2end, float p2beg, float p2end,
	float d2num, float f2num, int n2tic, int grid2, char *label2,
	char *labelfont, char *title, char *titlefont, 
	char *axescolor, char *titlecolor, char *gridcolor,
	int style);
void xSizeAxesBox (Display *dpy, Window win, 
	char *labelfont, char *titlefont, int style,
	int *x, int *y, int *width, int *height);

/* images */
XImage *xNewImage (Display *dpy, unsigned long pmin, unsigned long pmax,
	int width, int height, unsigned char *bytes);

/* rubberbanding box */
void xRubberBox (Display *dpy, Window win, XEvent event,
	int *x, int *y, int *width, int *height);

/* colormaps */
Status xCreateRGBDefaultMap (Display *dpy, XStandardColormap *scmap);
unsigned long xGetFirstPixel (Display *dpy);
unsigned long xGetLastPixel (Display *dpy);
Colormap xCreateRGBColormap (Display *dpy, Window win);
Colormap xCreateGrayColormap (Display *dpy, Window win);
Colormap xCreateHueColormap (Display *dpy, Window win);

/* dump data to xplots */
void dump2xplot(float *d, int n1, int n2, int dtype, char *title);
void dump2xgraph(float *d, int *n, int m, char *title, 
		char *label1, char *label2, char *style);
void dump2xplotn(float *d,int n1,int n2,int dtype,char *title,
	float f1, float f2, float d1, float d2, char *label1, char *label2,
	char *grid1, char *grid2);

/* restore foreground x graphics to background */
void fg2bg(Display *dpy, Window win);

#ifdef __cplusplus  /* if C++ (external C linkage is being specified) */
}
#endif

#endif /* XPLOT_H */
