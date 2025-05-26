/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* b2a - convert binary floats to ascii */

#include "par.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" B2A - convert binary floats to ascii				\n"
" 								\n"
" b2a <stdin >stdout 						\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	n1=2		floats per line in output file 		\n"
" 								\n"
" 	outpar=/dev/tty	output parameter file, contains the	\n"
"			number of lines (n=)			\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack
 */


main(int argc, char **argv)
{
	string outpar;		/* name of file holding output parfile	*/
	FILE *outparfp;		/* ... its file pointer			*/
	int n1;			/* number of floats per line		*/
	int n1read;		/* number of items read			*/
	int n2 = 0;		/* number of lines in input file 	*/
	float *x;		/* binary floats			*/


	/* Hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters and do set up */
	if (!getparstring("outpar", &outpar))	outpar = "/dev/tty" ;
	outparfp = efopen(outpar, "w");
	if (!getparint("n1", &n1))		n1 = 2;
	x = ealloc1float(n1);


	/* Loop over data converting to ascii */
	while (n1read = efread(x, FSIZE, n1, stdin)) {
		register int i1;

		if (n1read != n1)
			err("out of data in forming line #%d", n2+1);
		for (i1 = 0; i1 < n1; ++i1)  printf(" %11.4e", x[i1]);
		putchar('\n');
		++n2;
	}


	/* Make par file */
	fprintf(outparfp, "n=%d\n", n2);


	return EXIT_SUCCESS;
}
