/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* list color map of root window of default screen */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
	
main(argc,argv)
int argc;
char **argv;
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
	printf("Root Window Colormap ID = %d\n",cmap);

	/* list colors */
	for (i=0; i<CellsOfScreen(scr); i++) {
		color.pixel = i;
		XQueryColor(dpy,cmap,&color);
		printf("pixel = %d \tred = %d \tgreen = %d \tblue = %d\n",
			color.pixel,color.red,color.green,color.blue);
	}

	/* close connection to X server */
	XCloseDisplay(dpy);
}
