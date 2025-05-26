/* SUPSCONTOUR: $Revision: 1.3 $ ; $Date: 90/11/20 12:59:29 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
string sdoc = "\
									\n\
SUPSCONTOUR - PostScript CONTOURing of a segy data set		 	\n\
									\n\
supscontour <stdin >postscript file [optional parameters]		\n\
							        	\n\
Optional parameters: 							\n\
							        	\n\
d1=tr.dt or 0.004      sampling interval in fast dim, often dt or dz	\n\
d2=1.0                 ... in slow dim, often unit step in trace or dx	\n\
f1=tr.delrt/1000.0     first sample in fast dim, often tmin or zmin	\n\
f2=1.0                 ... in slow dim, often first tracl or xmin	\n\
							        	\n\
x1=f1,f1+d1,...        array of monotonic sampled values in 1st dimension\n\
x2=f2,f2+d2,...        array of monotonic sampled values in 2nd dimension\n\
nc=5                   number of contour values				\n\
dc=(max-min)/nc        contour interval					\n\
fc=min+dc              first contour					\n\
c=fc,fc+dc,...         array of contour values				\n\
cwidth=1.0,...         array of contour line widths			\n\
cgray=0.0,...          array of contour grays (0.0=black to 1.0=white)	\n\
cdash=0.0,...          array of dash spacings (0.0 for solid)		\n\
xbox=1.5               offset in inches of left side of axes box	\n\
ybox=1.5               offset in inches of bottom side of axes box	\n\
wbox=6.0               width in inches of axes box			\n\
hbox=8.0               height in inches of axes box			\n\
x1beg=x1min            value at which axis 1 begins			\n\
x1end=x1max            value at which axis 1 ends			\n\
d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n\
f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n\
n1tic=1                number of tics per numbered tic on axis 1	\n\
grid1=none             grid lines on axis 1 - none, dot, dash, or solid	\n\
label1=                label on axis 1					\n\
x2beg=x2min            value at which axis 2 begins			\n\
x2end=x2max            value at which axis 2 ends			\n\
d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n\
f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n\
n2tic=1                number of tics per numbered tic on axis 2	\n\
grid2=none             grid lines on axis 2 - none, dot, dash, or solid	\n\
label2=                label on axis 2					\n\
labelfont=Helvetica    font name for axes labels			\n\
labelsize=12           font size for axes labels			\n\
title=                 title of plot					\n\
titlefont=Helvetica-Bold font name for title				\n\
titlesize=24           font size for title				\n\
style=seismic          normal (axis 1 horizontal, axis 2 vertical) or	\n\
                       seismic (axis 1 vertical, axis 2 horizontal)	\n\
scrdir=$SU_SCRATCHDIR  temporary directory to hold data			\n\
							        	\n\
";
/**************** end self doc *******************************************/

/* Credits:
 *
 *	CWP: Dave (pscontour), Jack & John (su tee shirt)
 *
 */


segy tr;



main(int argc, char **argv)
{
	char *psdata;		/* tmp file for psimage data		*/
	char cmd[BUFSIZ];	/* build psimage command for system call*/
	FILE *psdatafp;		/* fp for psimage input file		*/
	float d1;		/* time/depth sample rate 		*/
	float d2;		/* trace/dx sample rate 		*/
	float f1;		/* tmin/zmin				*/
	float f2;		/* tracemin/xmin	 		*/
	int ns;			/* number of samples on trace		*/
	int ntr;		/* number of traces			*/
	int itr;		/* counter over traces			*/
	char *scrdir;		/* temporary directory to hold data 	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	ns = tr.ns;
	if (!fgetpar("d1", &d1)) {
		if (tr.dt) {  /* is dt field set? */
			d1 = tr.dt / 1000000.0;
		} else {		/* dt not set, assume 4 ms */
			d1 = 0.004;
			warn("tr.dt not set, assuming dt=%g", d1);
		}
	}
	if (!fgetpar("d2", &d2)) d2 = 1.0; /* default count by traces */
	if (!getparfloat("f1", &f1)) f1 = tr.delrt/1000.0;
	if (!fgetpar("f2", &f2)) {
		if (tr.tracl) {
			f2 = tr.tracl;
		} else {
			f2 = 1.0;
		}
	}

	/* Prepare temporary file to hold psdata */
	/* tmpnam(psdata); */
	if (!getparstring("scrdir", &scrdir)) {
		scrdir = getenv("SU_SCRATCHDIR");
	}
	psdata = etempnam(scrdir,NULL);
	psdatafp = efopen(psdata, "w+");


	/* Loop over input traces & put them into the psdata file */
	ntr = 0;
	do {
		++ntr;
		efwrite(tr.data, FSIZE, ns, psdatafp);
	} while (gettr(&tr));
	erewind(psdatafp);


	/* System call to pscontour */
	sprintf(cmd, "pscontour <%s n1=%d n2=%d d1=%f d2=%f f1=%f f2=%f",
			   psdata, ns, ntr, d1, d2, f1, f2);

	for (--argc, ++argv; argc; --argc, ++argv) {
		if (strncmp(*argv, "d1=", 3) && /* skip those already set */
		    strncmp(*argv, "d2=", 3) &&
		    strncmp(*argv, "f1=", 3) &&
		    strncmp(*argv, "f2=", 3)) {
		    
			strcat(cmd, " ");   /* get a space in between args */
			strcat(cmd, "\"");  /* user quotes will be stripped */
			strcat(cmd, *argv); /* add the arg */
			strcat(cmd, "\"");  /* user quotes will be stripped */
		}
	}

	system(cmd);


	/* Clean up */
	efclose(psdatafp);
	eremove(psdata);


	return EXIT_SUCCESS;
}
