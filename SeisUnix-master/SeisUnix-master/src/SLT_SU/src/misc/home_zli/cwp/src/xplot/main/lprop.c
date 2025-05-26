/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* list properties of root window of default screen of display */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
	
main(argc,argv)
int argc;
char **argv;
{
	Display *dpy;
	Atom *atom;
	int i,natoms;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL) {
		fprintf(stderr,"Cannot connect to display %s\n",
			XDisplayName(NULL));
		exit(-1);
	}

	/* list properties of root window */
	atom = XListProperties(dpy,DefaultRootWindow(dpy),&natoms);
	printf("Number of properties = %d\n",natoms);
	for (i=0; i<natoms; i++)
		printf("property[%d] = %s\n",i,XGetAtomName(dpy,atom[i]));

	/* close connection to X server */
	XCloseDisplay(dpy);
}
