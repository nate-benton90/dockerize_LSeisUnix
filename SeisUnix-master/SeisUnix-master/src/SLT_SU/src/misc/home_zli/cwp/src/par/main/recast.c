/* Copyright (c) Colorado School of Mines, 1990.*/
/* All rights reserved.                       */

/* RECAST - recast binary data from type in to type out */

#include "par.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" RECAST - RECAST data type						",
" 									",
" recast <stdin [optional parameters]  >stdout 				",
" 									",
" Required parameters:							",
" 	none								",
" 									",
" Optional parameters:							",
" 	in=float		input type	(float)			",
" 				=double		(double)		",
" 				=int		(int)			",
" 	out=double		output type	(double)		",
" 				=float		(float)			",
" 				=int		(int)			",
" 				=nint		nearest (int)		",
" 	outpar=/dev/tty		output parameter file, contains the	",
"				number of values (n1=)			",
" 									",
" Note: in=nint is the same as in=int 					",
" 									",
NULL};
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: John,Jack
 */

/* Caveat: still need to do short int, long int, long double  */

main(int argc, char **argv)
{
	char *in;		/* input data type			*/
	char *out;		/* output data type			*/
	char *outpar;		/* name of file holding output parfile	*/
	FILE *outparfp;		/* ... its file pointer			*/
	int n = 0;		/* number of input points	 	*/

	float xf;		/* binary float				*/
	double xd;		/* binary double			*/
	int xi;			/* binary integer			*/

	/* Hook up getpar */
	initargs(argc, argv);
	requestdoc(1);


	/* Get parameters and do set up */
	if (!getparstring("outpar", &outpar))	outpar = "/dev/tty" ;
	outparfp = efopen(outpar, "w");

	/* Get input and output data types */
	if (!getparstring("in", &in))		in = "float" ;
	if (!getparstring("out", &out))		out = "double" ;
	
	/* Check in and out to see if types supported */
	if ( !(	STREQ(in,"float") 
		|| STREQ(in,"double") 
		|| STREQ(in,"int") 
		|| STREQ(in,"nint")
		|| STREQ(out,"float") 
		|| STREQ(out,"double") 
		|| STREQ(out,"int")
		|| STREQ(out,"nint") 
		) 
	) err("%s or %s is an unsupported type",in,out);

	/* Using nint as an input makes no sense set in=int */
	if (	STREQ(in,"nint") )	in="int";

	
	/* Read floats, write doubles */
	if (STREQ(in,"float") && STREQ(out,"double") ) {

		/* Loop over data converting float to double */
		while (efread(&xf,FSIZE, 1, stdin)) {
			++n;
			xd = xf;
			efwrite(&xd, DSIZE, 1, stdout);
		}

	/* Read floats, write integers */
	} else if (STREQ(in,"float") && STREQ(out,"int") ) {

		/* Loop over data converting float to integer */
		while (efread(&xf,FSIZE, 1, stdin)) {
			++n;
			xi = xf;
			efwrite(&xi, ISIZE, 1, stdout);
		}

	/* Read floats, write nearest integers */
	} else if (STREQ(in,"float") && STREQ(out,"nint") ) {

		/* Loop over data converting float to nearest integer */
		while (efread(&xf,FSIZE, 1, stdin)) {
			++n;
			xi = NINT(xf);
			efwrite(&xi, ISIZE, 1, stdout);
		}

	/* read doubles, write floats */
	} else if (STREQ(in,"double") && STREQ(out,"float") ) {

		/* Loop over data converting double to float */
		while (efread(&xd, DSIZE, 1, stdin)) {
			++n;
			xf = xd;
			efwrite(&xf, FSIZE, 1, stdout);
		}

	/* Read doubles, write integers */
	} else if (STREQ(in,"double") && STREQ(out,"int") ) {

		/* Loop over data converting float to integer */
		while (efread(&xd,DSIZE, 1, stdin)) {
			++n;
			xi = xd;
			efwrite(&xi, ISIZE, 1, stdout);
		}

	/* Read doubles, write nearest integers */
	} else if (STREQ(in,"double") && STREQ(out,"nint") ) {

		/* Loop over data converting float to nearest integer */
		while (efread(&xd,DSIZE, 1, stdin)) {
			++n;
			xi = NINT(xd);
			efwrite(&xi, ISIZE, 1, stdout);
		}

	/* read integers, write floats */
	} else if (STREQ(in,"int") && STREQ(out,"float") ) {

		/* Loop over data converting integer to float */
		while (efread(&xi, ISIZE, 1, stdin)) {
			++n;
			xf = xi;
			efwrite(&xf, FSIZE, 1, stdout);
		}

	/* read integers, write doubles */
	} else if (STREQ(in,"int") && STREQ(out,"double") ) {

		/* Loop over data converting integer to double */
		while (efread(&xi, ISIZE, 1, stdin)) {
			++n;
			xd = xi;
			efwrite(&xd, DSIZE, 1, stdout);
		}


	} else if (STREQ(in,out) ) {
		if (STREQ(in,"float")) {
			/* Loop over data write float to float */
			while (efread(&xf, FSIZE, 1, stdin)) {
				++n;
				efwrite(&xf, FSIZE, 1, stdout);
			}

		} else if (STREQ(in,"double") ) {
			/* Loop over data write float to float */
			while (efread(&xd, DSIZE, 1, stdin)) {
				++n;
				efwrite(&xd, DSIZE, 1, stdout);
			}

		} else if (STREQ(in,"int") ) {
			/* Loop over data write float to float */
			while (efread(&xi, ISIZE, 1, stdin)) {
				++n;
				efwrite(&xi, ISIZE, 1, stdout);
			}
		
		}
	}
	

	/* Make par file */
	fprintf(outparfp, "n=%d\n", n);


	return EXIT_SUCCESS;
}
