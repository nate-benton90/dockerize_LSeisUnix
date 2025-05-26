/* SUXGRAPH: $Revision: 1.3 $ ; $Date: 90/11/20 16:19:13 $		*/

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
string sdoc =
" 									\n"
" SUXGRAPH - X-Windows GRAPHer of a segy data set 			\n"
" 									\n"
" suxgraph <stdin  [optional parameters]				\n"
" 							        	\n"
" Optional parameters: 							\n"
" 							        	\n"
" d1=tr.dt or 0.004      sampling interval in fast dim, often dt or dz	\n"
" d2=1.0                 ... in slow dim, often unit step in trace or dx\n"
" f1=tr.delrt/1000.0     first sample in fast dim, often tmin or zmin	\n"
" f2=1.0                 ... in slow dim, often first tracl or xmin	\n"
" 							        	\n"
" See the xgraph selfdoc for the remaining parameters.			\n"
" 							        	\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *
 *	CWP: Dave, Jack
 *
 */


segy tr;


main(int argc, char **argv)
{
	char plotcmd[BUFSIZ];	/* build command for popen	 	*/
	float *trbuf;		/* trace buffer			 	*/
	FILE *datafp;		/* fp for trace data file		*/
	FILE *plotfp;		/* fp for plot data			*/
	float d1;		/* time/depth sample rate 		*/
	float d2;		/* trace/dx sample rate 		*/
	float f1;		/* tmin/zmin				*/
	float f2;		/* tracemin/xmin	 		*/
	int nt;			/* number of samples on trace		*/
	int ntr;		/* number of traces			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	if (!getparfloat("d1", &d1)) {
		if (tr.dt) {  /* is dt field set? */
			d1 = tr.dt / 1000000.0;
		} else {		/* dt not set, assume 4 ms */
			d1 = 0.004;
			warn("tr.dt not set, assuming dt=%g", d1);
		}
	}
	if (!getparfloat("d2", &d2)) d2 = 1.0; /* default count by traces */
	if (!getparfloat("f1", &f1)) f1 = tr.delrt/1000.0;


	/* Allocate trace buffer */
	trbuf = ealloc1float(nt);


	/* Create temporary "file" to hold data */
	datafp = etmpfile();
	/*datafp = etempfile(NULL); */

	/* Loop over input traces & put them into the xdata file */
	ntr = 0;
	do {
		++ntr;
		efwrite(tr.data, FSIZE, nt, datafp);
	} while (gettr(&tr));


	/* System call to xgraph */
	sprintf(plotcmd, "xgraph n=%d nplot=%d d1=%f f1=%f style=%s",
			   nt, ntr, d1, f1, "seismic");

	for (--argc, ++argv; argc; --argc, ++argv) {
		if (strncmp(*argv, "n=", 2) && /* skip those already set */
		    strncmp(*argv, "nplot=", 6) &&
		    strncmp(*argv, "d1=", 3) &&
		    strncmp(*argv, "f1=", 3) &&
		    strncmp(*argv, "style=", 6)) {
		    
			strcat(plotcmd, " ");   /* put a space between args */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
			strcat(plotcmd, *argv); /* add the arg */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
		}
	}


	/* Open pipe; read data to buf; write buf to plot program */
	plotfp = epopen(plotcmd, "w");
	{ register int itr;
	  rewind(datafp);
	  for (itr = 0; itr < ntr; ++itr) {
		efread (trbuf, FSIZE, nt, datafp);
		efwrite(trbuf, FSIZE, nt, plotfp);
	  }
	}


	/* Clean up */
	epclose(plotfp);
	efclose(datafp);


	return EXIT_SUCCESS;
}
