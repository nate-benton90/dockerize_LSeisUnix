/* SUPASTE: $Revision: 1.3 $ ; $Date: 92/10/26 11:23:47 $	*/

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
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 								",
" SUPASTE - paste existing SEGY headers on existing data	",
" 								",
" supaste <bare_data >segys  ns= head=headers ftn=0		",
"								",
" Required parameter:						",
"	ns=the number of samples per trace			",
" 								",
" Optional parameters:						",
" 	head=headers	file with segy headers			",
"	ftn=0		Fortran flag				",
"			0 = unformatted data from C		",
"			1 = ... from Fortran			",
" Caution:							",
"	An incorrect ns field will munge subsequent processing.	",
"								",
NULL};
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack
 */


segy tr;

main(int argc, char **argv)
{
	String head;		/* name of file holding headers		*/
	FILE *headfp;		/* ... its file pointer			*/
	int ns;			/* number of data samples on the segys	*/
	int ftn;		/* fortran flag				*/
	char junk[ISIZE];	/* to discard ftn junk  		*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Get parameters */
	if (!getparint   ("ns"  , &ns))	 err("must specify ns=");
	if (!getparstring("head", &head))	head = "headers";
	if (!getparint   ("ftn"  , &ftn))	ftn  = 0;
	if (ftn != 0 && ftn != 1)  err("ftn=%d must be 0 or 1", ftn);


	/* Open file with saved headers */
	headfp = efopen(head, "r");


	/* Reconstruct the segys--if the ftn option is	*/
	/* selected, omit the int before and after each	*/
	/* trace giving the length of the trace in bytes*/
	/* as per the Fortran unformatted record format.*/
	while (TRUE) {
		static int ntr=0; /* for user info only */

		/* Do read of header for the segy */
		if (!efread(&tr, HDRBYTES, 1, headfp)) {
			warn("ntr=%d", ntr);
			return EXIT_SUCCESS;
		}

		/* If Fortran data, read past the record size bytes */
		if (ftn) efread(junk, ISIZE, 1, stdin);

		/* Do read of data for the segy */
		switch (efread(tr.data, FSIZE, ns, stdin)) {
		case 0: /* oops, no data for this header */
			warn("header without data for trace #%d", ntr+1);
			return EXIT_FAILURE;
		default:
			puttr(&tr);
			++ntr;
		}

		/* If Fortran data, read past the record size bytes */
		if (ftn) efread(junk, ISIZE, 1, stdin);
	}


	return EXIT_SUCCESS;
}
