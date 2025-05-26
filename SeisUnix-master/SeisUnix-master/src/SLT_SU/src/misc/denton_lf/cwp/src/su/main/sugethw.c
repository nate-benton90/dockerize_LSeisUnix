/* SUGETHW: $Revision: 1.4 $ ; $Date: 90/12/08 15:47:20 $	*/

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

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUGETHW - sugethw writes the values of the selected key words	\n"
" 								\n"
" sugethw key1 key2 ... [output=] <infile [>outfile]		\n"
" 								\n"
" Required parameters:						\n"
" 	At least one key word.					\n"
" 								\n"
" Optional parameters:						\n"
" 	output=ascii	output written as ascii 		\n"
" 			=binary for output as binary floats	\n"
" 								\n"
" Output is written in the order of the keys on the command	\n"
" line for each trace in the data set.				\n"
" 								\n"
" Example:							\n"
" 	sugethw <stdin sx gx					\n"
" writes sx, gx values as ascii trace by trace to the terminal.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	SEP: Shuki
 *	CWP: Jack
 */


segy tr;

main(int argc, char **argv)
{
	String output;
	bool asciiout = true;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);

	file2g(stdin);


	/* Argument count check */
	if (argc == 1)  err("must specify key(s) as command line arguments");


	/* Set and check output disposition (ascii or binary) */
	if (!getparstring("output", &output))   output = "ascii";
	if ((!STREQ(output, "ascii")) && (!STREQ(output, "binary")))
		err("output parameter=%s, must be ascii or binary", output);
	if (STREQ(output, "binary"))  asciiout = false;


	/* Loop over traces writing selected header field values */
	while (gettr(&tr)) {
		register int i;

		for (i = 1; i < argc; ++i) {
			String key = argv[i];
			Value val;

			/* discard command line parameter strings */
			if (STREQ(key, "output=ascii") ||
			    STREQ(key, "output=binary"))
				continue;

			gethdval(&tr, key, &val);
			if (asciiout) {  /* ascii output */
				printf("%6s=", key);
				printfval(hdtype(key), val);
				putchar('\t');
			} else {  /* binary output */
				float fval = vtof(hdtype(key), val);
				efwrite((char *) &fval, FSIZE, 1, stdout);
			}
		}

		if (asciiout)  printf("\n\n");
	}

	return EXIT_SUCCESS;
}
