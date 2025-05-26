/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* LCMAP: $Revision: 1.6 $ ; $Date: 1996/09/09 19:13:55 $	*/

/*********************** self documentation **********************/
/* 
 * LCMAP - List Color Map of root window of default screen 
 * 
 * Usage:   lcmap 
 * 
 */ 
/**************** end self doc ********************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>

void	
main(int argc, char**argv)
{
	Display *dpy;
	Screen *scr;
	XWindowAttributes attr;
	Colormap cmap;
	XColor color;
	int i;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL) {
		fprintf(stderr,"Cannot connect to display %s\n",
			XDisplayName(NULL));
		exit(-1);
	}
	scr = XDefaultScreenOfDisplay(dpy);

	/* determine colormap ID */
	if (!XGetWindowAttributes(dpy,RootWindowOfScreen(scr),&attr)) {
		fprintf(stderr,"Cannot get RootWindow attributes\n");
		exit(-1);
	}
	cmap = attr.colormap;
	printf("Root Window Colormap ID = %ld\n",cmap);

	/* list colors */
	for (i=0; i<CellsOfScreen(scr); i++) {
		color.pixel = i;
		XQueryColor(dpy,cmap,&color);
		printf("pixel = %d \tred = %d \tgreen = %d \tblue = %d\n",
		       (int) color.pixel,
		       (int) color.red,
		       (int) color.green,
		       (int) color.blue);
	}

	/* close connection to X server */
	XCloseDisplay(dpy);
}
