/* SUPSMOVIE: $Revision: 1.3 $ ; $Date: 92/10/26 15:06:39 $		*/

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
char *sdoc[] = {
" 									",
" SUPSMOVIE - PostScript MOVIE plot of a segy data set			",
" 									",
" supsmovie <stdin [optional parameters] | ...				",
" 							        	",
" Optional parameters: 							",
" 							        	",
" n2 is the number of traces per frame.  If not getparred then it	",
" is the total number of traces in the data set.  			",
" 							        	",
" n3 is the number of frames.  If not getparred then it			",
" is the total number of frames in the data set measured by ntr/n2	",
" 							        	",
" d1 is the sampling interval in the fast dimension.  If not getparred	",
" then for seismic time domain data d1=tr.dt/10^6 if set, else 0.004. 	",
" For other types of data d1=tr.d1 if set, else 1.0			",
" 							        	",
" d2 is the sampling interval in the slow dimension.  If not getparred	",
" d2=tr.d2 if set, else 1.0						",
" 							        	",
" f1 is the first sample in the fast dimension.  If not getparred	",
" then for seismic time domain data f1=tr.delrt/10^3 if set, else 0.0.	",
" For other types of data f1=tr.d1 if set else 0.0	 		",
" 							        	",
" f2 is the first sample in the slow dimension.  If not getparred	",
" f2=tr.f2 if set, else tr.tracr if set, else tr.tracl if set,		",
" else 1.0 for seismic time domain data, else 0.0 			",
" 							        	",
" Note that for seismic time domain data, the \"fast dimension\" is	",
" time and the \"slow dimension\" is usually trace number or range.	",
" Also note that \"foreign\" data tapes may have something unexpected	",
" in the d2,f2 fields, use segyclean to clear these if you can afford	",
" the processing time or use d2= f2= to over-ride the header values if	",
" not.									",
" 							        	",
" See the psmovie selfdoc for the remaining parameters.			",
" 							        	",
" On NeXT:     supsmovie < infile [optional parameters]  | open	       	",
" Caveat: only the Preview Application can handle multi-page PS output 	",
" 	  on NeXT					        	",
NULL};
/**************** end self doc *******************************************/

/* Credits:
 *
 *	CWP: Dave Hale and Zhiming Li (psmovie)
 *	     Jack K. Cohen (suxmovie)
 *	     John Stockwell (supsmovie)
 *
 * Notes:
 *	When n2 isn't getparred, we need to count the traces
 *	for psmovie.  In this case:
 *	we are using tmpfile because on many machines it is
 *	implemented as a memory area instead of a disk file.
 *	However, if your system really makes a disk file you
 *	should consider altering the code to remove the file
 *	on interrupt.  Two possibilities are trapping the
 *	interrupt with "signal" or using "tmpnam" followed
 *	by an immediate "remove" (aka unlink in old unix).
 *	Although we compute ntr, we don't allocate a 2-d array
 *	and content ourselves with copying trace by trace from
 *	the data "file" to the pipe into the plotting program.
 *	Although we could use tr.data, we allocate a trace buffer
 *	for code clarity.
 */


segy tr;

main(int argc, char **argv)
{
	char plotcmd[BUFSIZ];	/* build psmovie command for popen 	*/
	float *trbuf;		/* trace buffer			 	*/
	FILE *datafp;		/* fp for trace data file (if n2 < ntr)	*/
	FILE *plotfp;		/* fp for plot data			*/
	int nt;			/* number of samples on trace		*/
	int n2;			/* number of traces per frame		*/
	int n3;			/* number of frames in data		*/
	int ntr;		/* number of traces			*/
	float d1;		/* time/depth sample rate 		*/
	float d2;		/* trace/dx sample rate 		*/
	float f1;		/* tmin/zmin				*/
	float f2;		/* tracemin/xmin	 		*/
	bool seismic;		/* is this seismic data?		*/
	bool got_n2 = true;	/* was n2 getparred?			*/
	bool got_n3 = true;	/* was n3 getparred?			*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	seismic =  (tr.trid == 0 || tr.trid == TREAL);
	nt = tr.ns;

	if (!getparfloat("d1", &d1)) {
		if (seismic) {
			if (tr.dt) {
				d1 = (float) tr.dt / 1000000.0;
			} else {
				d1 = 0.004;
				warn("tr.dt not set, assuming dt=0.004");
			}
		} else { /* non-seismic data */
			if (tr.d1) {
				d1 = tr.d1;
			} else {
				d1 = 1.0;
				warn("tr.d1 not set, assuming d1=1.0");
			}
		}
	}

	if (!getparfloat("d2", &d2)) d2 = (tr.d2) ? tr.d2 : 1.0;

	if (!getparfloat("f1", &f1)) {
		if (seismic) {
			f1 = (tr.delrt) ? (float) tr.delrt/1000.0 : 0.0;
		} else {
			f1 = (tr.f1) ? tr.f1 : 0.0;
		}
	}

	if (!getparfloat("f2", &f2)) {
		if      (tr.f2)     f2 = tr.f2;
		else if (tr.tracr)  f2 = (float) tr.tracr;
		else if (tr.tracl)  f2 = (float) tr.tracl;
		else if (seismic)   f2 = 1.0;
		else 		    f2 = 0.0;
	}


	/* Allocate trace buffer */
	trbuf = ealloc1float(nt);

	/* Get or set n2 */
	if (!getparint("n2", &n2)) { /* count and store traces to set n2 */

		/* Create temporary "file" to hold data */
		datafp = etmpfile();

		/* Loop over input traces & put them into the data file */
		ntr = 0;
		do {
			++ntr;
			efwrite(tr.data, FSIZE, nt, datafp);
		} while (gettr(&tr));

		n2 = ntr;
		got_n2 = false;
		n3=1;
	}

	/* Get or set n3 */
	if (!getparint("n3", &n3) || !got_n2 ) 
			{ /* count and store frames to set n3 */

		/* Create temporary "file" to hold data */
		datafp = etmpfile();

		/* Loop over input frames & put them into the data file */
		ntr = 0;
		do {
			++ntr;
			efwrite(tr.data, FSIZE, nt, datafp);
		} while (gettr(&tr));

		n3 = ntr/n2;
		got_n3 = false;
	}


	/* Set up psmovie command line */
	sprintf(plotcmd, "psmovie n1=%d n2=%d n3=%d d1=%f d2=%f f1=%f f2=%f",
			   nt, n2, n3, d1, d2, f1, f2);

	for (--argc, ++argv; argc; --argc, ++argv) {
		if (strncmp(*argv, "d1=", 3) && /* skip those already set */
		    strncmp(*argv, "d2=", 3) &&
		    strncmp(*argv, "f1=", 3) &&
		    strncmp(*argv, "f2=", 3)) {
		    
			strcat(plotcmd, " ");   /* put a space between args */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
			strcat(plotcmd, *argv); /* add the arg */
			strcat(plotcmd, "\"");  /* user quotes are stripped */
		}
	}


	/* Open pipe to psmovie and send the traces */
	plotfp = epopen(plotcmd, "w");
	
	if (!got_n2 || !got_n3) { /* send out stored traces one by one */
		rewind(datafp);
		{ register int itr;
			for (itr = 0; itr < ntr; ++itr) {
				efread (trbuf, FSIZE, nt, datafp);
				efwrite(trbuf, FSIZE, nt, plotfp);
			}
		}
	} else { /* just pump out traces and let psmovie do the work */
		do {
			efwrite(tr.data, FSIZE, nt, plotfp);
		} while (gettr(&tr));
	}


	/* Clean up */
	epclose(plotfp);
	if (!got_n2) efclose(datafp);


	return EXIT_SUCCESS;
}
