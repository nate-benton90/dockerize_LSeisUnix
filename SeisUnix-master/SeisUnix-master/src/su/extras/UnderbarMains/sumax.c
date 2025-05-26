/* SUMAX: $Revision: 2.10 $ ; $Date: 89/09/22 11:00:53 $			*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include "segy.h"
#include "fconst.h"

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
	output = ascii 							\n\
	       = binary	(for binary floats)				\n\
									\n\
	verbose = 0 (applies only to the output=ascii option)		\n\
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
/*****************************************************************/

/* Credits:
 *	SEP: Shuki
 *	CWP: Jack
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sumax.c,v $";
static string revid =
	"   $Revision: 2.10 $ ; $Date: 89/09/22 11:00:53 $";



segy tr;

main(argc, argv)
int argc; char **argv;
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


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);

	verbose = 0;	igetpar("verbose", &verbose);

	/* Set and check output disposition (ascii or binary) */
	output = "ascii";	sgetpar("output", &output);
	if ((!STREQ(output, "ascii")) && (!STREQ(output, "binary"))) {
	    err("output parameter=%s, must be \"ascii\" or \"binary\"",
								output);
	}

	while (gettr(&tr)) {
		nt = tr.ns;
		maxmgv_(tr.data, ONE, &absmax, &absmaxloc, &nt);
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
	

	return SUCCEED;
}
