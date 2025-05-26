/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/******************************************************************************
Xcwp.h:  header file for X cwp library
*******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 08/28/90
******************************************************************************/

#ifndef XCWP_H
#define XCWP_H

/* INCLUDES */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <values.h>

/* DATA STRUCTURES */
typedef struct {
	GC gc;
	float xshift,yshift;
	float xscale,yscale;
} *FGC;
typedef struct {
	float fx,fy;
} FXPoint;
typedef struct {
	float fx,fy;
	float fwidth,fheight;
} FXRectangle;

/* MACROS */
#define MapFX(fgc,fx) ((int)(((fgc)->xshift)+(fx)*((fgc)->xscale)))
#define MapFY(fgc,fy) ((int)(((fgc)->yshift)+(fy)*((fgc)->yscale)))
#define MapFWidth(fgc,fwidth) ((int)((fwidth)*((fgc)->xscale)))
#define MapFHeight(fgc,fheight) ((int)((fheight)*((fgc)->yscale)))
#define MapFAngle(fgc,fangle) ((int)((fangle)*64.0))
#define MapX(fgc,x) (((x)-((fgc)->xshift))/((fgc)->xscale))
#define MapY(fgc,y) (((y)-((fgc)->yshift))/((fgc)->yscale))
#define MapWidth(fgc,width) ((width)/((fgc)->xscale))
#define MapHeight(fgc,height) ((height)/((fgc)->yscale))
#define MapAngle(fgc,angle) ((angle)/64.0)

/* RESOURCES */
#define XcwpNaxesGrid "axesGrid"
#define XcwpCAxesGrid "AxesGrid"
#define XcwpRAxesGrid "AxesGrid"
#define XcwpNONE 0
#define XcwpDOT 1
#define XcwpDASH 2
#define XcwpSOLID 3
#define XcwpNaxesStyle "axesStyle"
#define XcwpCAxesStyle "AxesStyle"
#define XcwpRAxesStyle "AxesStyle"
#define XcwpNORMAL 0
#define XcwpSEISMIC 1
#define XcwpRFloat "Float"

/* CALLBACK REASONS */
#define XcwpCR_RESIZE 1
#define XcwpCR_EXPOSE 2
#define XcwpCR_INPUT 3

/* MISCELLANEOUS DEFINES */
#ifndef CHARSET
#define CHARSET ((XmStringCharSet)XmSTRING_DEFAULT_CHARSET)
#endif

/* FUNCTION PROTOTYPES */
int FMapFX (FGC fgc, float fx);
int FMapFY (FGC fgc, float fy);
int FMapFWidth (FGC fgc, float fwidth);
int FMapFHeight (FGC fgc, float fheight);
int FMapFAngle (FGC fgc, float fangle);
void FMapFPoint (FGC fgc, float fx, float fy, int *x_return, int *y_return);
void FMapFPoints (FGC fgc, FXPoint fpoints[], int npoints, 
	XPoint points_return[]);
float FMapX (FGC fgc, int x);
float FMapY (FGC fgc, int y);
float FMapWidth (FGC fgc, int width);
float FMapHeight (FGC fgc, int height);
float FMapAngle (FGC fgc, int angle);
void FMapPoint (FGC fgc, int x, int y, float *fx_return, float *fy_return);
void FMapPoints (FGC fgc, XPoint points[], int npoints, 
	FXPoint fpoints_return[]);
void FSetGC (FGC fgc, GC gc);
void FSetMap (FGC fgc, int x, int y, int width, int height,
	float fx, float fy, float fwidth, float fheight);
FGC FXCreateFGC (GC gc, int x, int y, int width, int height,
	float fx, float fy, float fwidth, float fheight);
void FXFreeFGC (FGC fgc);
void FXDrawPoint (Display *display, Drawable d, FGC fgc, float fx, float fy);
void FXDrawPoints (Display *display, Drawable d, FGC fgc, 
	FXPoint fpoints[], int npoints, int mode);
void FXDrawLine (Display *display, Drawable d, FGC fgc,
	float fx1, float fy1, float fx2, float fy2);
void FXDrawLines (Display *display, Drawable d, FGC fgc,
	FXPoint fpoints[], int npoints, int mode);
void FXDrawRectangle (Display *display, Drawable d, FGC fgc, 
	float fx, float fy, float fwidth, float fheight);
void FXDrawArc (Display *display, Drawable d, FGC fgc,
	float fx, float fy, float fwidth, float fheight, 
	float fangle1, float fangle2);
void FXDrawString (Display *display, Drawable d, FGC fgc, 
	float fx, float fy, char *string, int length);
void XcwpStringToFloat (XrmValue *args, int *nargs, 
	XrmValue *fromVal, XrmValue *toVal);
Widget XcwpCreateStringRadioButtons (Widget parent, char *label,
	int nstrings, char **strings, int first,
	void (*callback)(int selected, void *clientdata), void *clientdata);

#endif /* XCWP_H */
