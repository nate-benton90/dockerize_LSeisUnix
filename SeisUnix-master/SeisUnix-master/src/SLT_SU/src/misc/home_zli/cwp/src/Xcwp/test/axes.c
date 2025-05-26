/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/*****************************************************************************
axes.c : test the Axes widget  
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 08/28/90
*****************************************************************************/

#include <stdio.h>
#include "Xcwp/Xcwp.h"
#include "Xcwp/Axes.h"

void resizeCB (XcwpAxesWidget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
void exposeCB (XcwpAxesWidget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);
void inputCB (XcwpAxesWidget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *calldata);

main(int argc, char *argv[])
{
	Widget toplevel,axes;

	toplevel = XtInitialize(argv[0],"AxesTest",NULL,0,&argc,argv);
	axes = XtCreateManagedWidget("axes",xcwpAxesWidgetClass, 
		toplevel,NULL,0);
	XtAddCallback(axes,XtNresizeCallback,resizeCB,NULL);
	XtAddCallback(axes,XtNexposeCallback,exposeCB,NULL);
	XtAddCallback(axes,XtNinputCallback,inputCB,NULL);
	XcwpSetAxesValues(axes,0.0,3.1415,-2.0,2.0);
	XtRealizeWidget(toplevel);
	XtMainLoop();
}

void resizeCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *ca)
{
	printf("resize callback\n");
}

void exposeCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *ca)
{
	Position x=ca->x,y=ca->y;
	Dimension width=ca->width,height=ca->height;
	GC gc;
	
	/* create GC */
	gc = XCreateGC(XtDisplay(w),XtWindow(w),0L,NULL);
	XSetForeground(XtDisplay(w),gc,BlackPixelOfScreen(XtScreen(w)));
	
	/* draw something */
	XFillRectangle(XtDisplay(w),XtWindow(w),gc,x,y,width,height);
		
	/* free GC */
	XFreeGC(XtDisplay(w),gc);
}

void inputCB (Widget w, 
	caddr_t clientdata,
	XcwpAxesCallbackStruct *ca)
{
	int x=ca->x,y=ca->y,width=ca->width,height=ca->height;
	float x1beg=ca->x1beg,x1end=ca->x1end,x2beg=ca->x2beg,x2end=ca->x2end;
	float p1beg=ca->p1beg,p1end=ca->p1end,p2beg=ca->p2beg,p2end=ca->p2end;
	XEvent *event=ca->event;
	int xe,ye;
	float xf,yf;
	
	/* NOTE:  this test assumes seismic style! */
	xe = event->xbutton.x;
	ye = event->xbutton.y;
	xf = x2beg+p2beg+(x2end+p2end-x2beg-p2beg)*(xe-x)/width;
	yf = x1beg+p1beg+(x1end+p1end-x1beg-p1beg)*(ye-y)/height;
	printf("x=%g y=%g\n",xf,yf);
}

