/* SUSTRIP: $Revision: 1.4 $ ; $Date: 90/11/17 15:16:18 $	*/

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
string sdoc =
" 								\n"
" SUSTRIP - remove the SEGY headers from the traces		\n"
" 								\n"
" sustrip <stdin >stdout head=/dev/null outpar=/dev/tty ftn=0	\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	head=/dev/null		file to save headers in		\n"
" 								\n"
" 	outpar=/dev/tty		output parameter file, contains:\n"
" 				number of samples (n1=)		\n"
" 				number of traces (n2=)		\n"
" 				sample rate in seconds (d1=)	\n"
" 								\n"
" 	ftn=0			Fortran flag			\n"
" 				0 = write unformatted for C	\n"
" 				1 = ... for Fortran		\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 */


segy tr;
segychdr ch;
segybhdr bh;

#ifdef __convex__
        #define fseek2g(x,y,z)  fseek64(x,y,z);
#else
        #define fseek2g(x,y,z)  fseek(x,y,z);
#endif

main(int argc, char **argv)
{
	string head;	/* name of file holding headers		*/
	FILE *headfp;	/* ... its file pointer			*/
	string outpar;	/* name of file holding output parfile	*/
	FILE *outparfp;	/* ... its file pointer			*/
	int ns;		/* number of data samples on the segys	*/
	int nsbytes;	/* ... in bytes				*/
	int ftn;	/* fortran flag				*/
	int ntr = 0;	/* number of traces written		*/
	FILE *infp=stdin, *outfp=stdout;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	switch(filestat(STDOUT)) {
	case BADFILETYPE:
		warn("stdout is illegal filetype");
		selfdoc();
	break;
	case TTY:
		warn("stdout can't be tty");
		selfdoc();
	break;
	}

	/* Get parameters */
	if (!getparstring("head"  , &head))	head   = "/dev/null";
	if (!getparstring("outpar", &outpar))	outpar = "/dev/tty" ;
	if (!getparint   ("ftn"   , &ftn))	ftn = 0;
	if (ftn != 0 && ftn != 1)  err("ftn=%d must be 0 or 1", ftn);


	/* Open files to save headers and parameters */
	headfp = efopen(head, "w");
	if( (outparfp = fopen(outpar,"w"))==NULL ) outparfp = stderr;

	/*
	outparfp = efopen(outpar, "w");
	*/

	fseek2g(infp,0,1);
        fseek2g(outfp,0,1);

	/* read id header and save in head */
	gethdr(&ch,&bh);
	fputhdr(headfp,&ch,&bh);

	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	ns = tr.ns;
	nsbytes = ns * FSIZE;


	/* Write the data portion of the records--if the ftn	*/
	/* option is selected, write an int before and after	*/
	/* each trace giving the length of the trace in bytes	*/
	/* as per the Fortran unformatted record format.	*/
	do {

		if (ftn) efwrite(&nsbytes, ISIZE, 1, stdout);
		efwrite(tr.data, FSIZE, ns, stdout);
		if (ftn) efwrite(&nsbytes, ISIZE, 1, stdout);

		efwrite(&tr, 1, HDRBYTES, headfp);

		++ntr;

	} while (gettr(&tr));

	/* Make par file for headerless file */
	fprintf(outparfp, "n1=%d n2=%d d1=%f\n", tr.ns, ntr, 
		(float)tr.dt/1000000.0);


	return EXIT_SUCCESS;
}
