/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/**************************************************************************
Read Encapsulated PostScript from standard input and display in an X window
***************************************************************************
AUTHOR:  Dave Hale, Colorado School of Mines, 07/25/90
**************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <DPS/XDPS.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>

#define LBUF	1000

main()
{
	float llx,lly,urx,ury;
	int width,height,i;
	char buf[LBUF],string[256];
	char *scalestring="0.88889 1.10769 scale\n"; /* 72/81, 72/65 */
	Display *dpy;
	XEvent ev;
	GC gc;
	Window win;
	DPSContext ctxt;

	/* open display */
	if ((dpy=XOpenDisplay(NULL))==NULL) {
		fprintf(stderr,"Cannot connect to display %s\n",
			XDisplayName(NULL));
		exit(-1);
	}
	
	/* determine window size */
	fgets(buf,LBUF,stdin);
	sscanf(buf,"%*s %s",string);
	string[3] = '\0';
	if (strcmp(string,"EPS")!=0) {
		width = 612;
		height = 792;
	} else {
		while(fgets(buf,LBUF,stdin)!=NULL) { 
			if (buf[0]!='%' || buf[1]!='%') continue;
			sscanf(buf,"%s",string);
			if (strcmp(string,"%%BoundingBox:")==0) {
				sscanf(buf,"%*s %s",string);
				if (strcmp(string,"(atend)")==0) {
					width = 612;
					height = 792;
				} else {
					sscanf(buf,"%*s %f %f %f %f",
						&llx,&lly,&urx,&ury);
					width = urx-llx;
					height = ury-lly;
				}
				break;
			} else if (strcmp(string,"%%EndComments")==0) {
				width = 612;
				height = 792;
				break;
			}
		}
	}
	
	/* create and map window */
	win = XCreateSimpleWindow(dpy,DefaultRootWindow(dpy),
		100,100,width,height,1,
                BlackPixel(dpy,DefaultScreen(dpy)),
		WhitePixel(dpy,DefaultScreen(dpy)));
	XMapWindow(dpy,win);
	
	/* create graphics context */
	gc = XCreateGC(dpy,RootWindow(dpy,DefaultScreen(dpy)),0,NULL);
	XSetForeground(dpy,gc,BlackPixel(dpy,DefaultScreen(dpy)));
	XSetBackground(dpy,gc,WhitePixel(dpy,DefaultScreen(dpy)));

	/* create and set Display PostScript context */
	ctxt = XDPSCreateSimpleContext(dpy,win,gc,0,height,
		DPSDefaultTextBackstop,DPSDefaultErrorProc,NULL);
	if (ctxt==NULL) {
		fprintf(stderr,"Cannot create DPS context\n");
		exit(-1);
	}
	DPSSetContext(ctxt);
	DPSWaitContext(ctxt);
	for (i=0; i<1000000; ++i);  /* KLUDGE synchronization */
	
	/* scale */
	/* no longer necessary, as of AIX Version 3.003? */
	/*
	DPSWriteData(ctxt,scalestring,strlen(scalestring));
	*/

	/* read PostScript from standard input and write to window */
	while (fgets(buf,LBUF,stdin)!=NULL) {
		sscanf(buf,"%s",string);
		if (strcmp(string,"showpage")==0) break;
		DPSWriteData(ctxt,buf,strlen(buf));
	}
	DPSFlushContext(ctxt);

	/* any key press to exit */
	XSelectInput(dpy,win,KeyPressMask);
	while(True) {
            XNextEvent(dpy,&ev);
	    if (ev.type==KeyPress) break;
	}

	/* clean up */
	DPSDestroySpace(DPSSpaceFromContext(ctxt));
	XFlush(dpy);
}
