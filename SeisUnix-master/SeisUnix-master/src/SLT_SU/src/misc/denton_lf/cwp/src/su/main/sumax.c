/* SUMAX: $Revision: 1.4 $ ; $Date: 91/02/14 14:43:48 $			*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*************************** self documentation **************************/
string sdoc = "\
									\n\
SUMAX - get trace by trace absolute maxima and global maximum magnitude	\n\
	and their indices 						\n\
									\n\
sumax <stdin >stdout [optional parameters]				\n\
									\n\
Required parameters:							\n\
	none								\n\
						        		\n\
Optional parameters: 							\n\
	output=ascii 							\n\
	      =binary	(for binary floats)				\n\
									\n\
	verbose=0 (applies only to the output=ascii option)		\n\
									\n\
		Under the ascii option, verbose=1, prints the trace	\n\
		number and maximum value on each trace.	 In addition,	\n\
		a message is printed giving the global maximum and its	\n\
		location.  With the default, verbose=0, only the global	\n\
		maximum is printed.		 			\n\
									\n\
		Under the binary option, the max on each trace is	\n\
		written as a float.					\n\
									\n\
NOTE: Under the verbose option, the sample locations are reported	\n\
      zero-based (i.e. first sample is sample 0).			\n\
									\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack
 *
 */


segy tr;

main(int argc, char **argv)
{
	string output;		/* format (ascii/binary) of output	*/
	int verbose;		/* flag to print extra information	*/
	int nt;			/* number of time points on trace	*/
	int itr = 0;		/* trace number				*/
	int gitr;		/* trace with global max 		*/
	int git;		/* zero-based index of global max	*/
	float gmax= -1.0;	/* global absolute maximum		*/
	float absmax;		/* absolute max on a trace		*/
	int absmaxloc;		/* zero-based index of absmax		*/
	FILE *infp=stdin;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	verbose = 0;	igetpar("verbose", &verbose);

	/* Set and check output disposition (ascii or binary) */
	output = "ascii";	sgetpar("output", &output);
	if ((!STREQ(output, "ascii")) && (!STREQ(output, "binary"))) {
	    err("output parameter=%s, must be \"ascii\" or \"binary\"",
								output);
	}

	file2g(infp);

	while (gettr(&tr)) {
		register int i;

		nt = tr.ns;

		absmax = ABS(tr.data[0]); absmaxloc = 0;
		for (i = 1; i < nt; ++i) {
			if (ABS(tr.data[i]) > absmax) {
				absmax = ABS(tr.data[i]);
				absmaxloc = i;
			}
		}
		if (STREQ(output, "ascii")) {
			if (verbose) printf("%d %e at point %d\n",
						++itr, absmax, absmaxloc);
			if (absmax > gmax) {
				gmax = absmax;
				gitr = itr;
				git  = absmaxloc;
			}
		} else {
		    efwrite((char *) &absmax, FSIZE, 1, stdout);
		}
	}

	if (STREQ(output, "ascii")) {
		if (verbose) printf("global max = ");
		printf("%e", gmax);
		if (verbose) printf(" at trace %d, point %d", gitr, git);
		printf("\n");
	}
	

	return EXIT_SUCCESS;
}
