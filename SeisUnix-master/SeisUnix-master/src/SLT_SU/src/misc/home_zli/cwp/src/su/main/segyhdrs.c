/* SEGYHDRS: $Revision: 1.4 $ ; $Date: 90/12/18 14:12:42 $	*/

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

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SEGYHDRS - make SEG-Y ascii and binary headers for segywrite	\n"
" 								\n"
" segyhdrs ns= [bfile= hfile=] 					\n"
" 								\n"
" Required parameter:						\n"
" 	ns=the number of samples per trace			\n"
" 								\n"
" Optional parameters:						\n"
" 	bfile=binary	name of file containing binary block	\n"
" 	hfile=header	name of file containing ascii block	\n"
" 								\n"
" The hns and format fields in the binary header are set.  The	\n"
" other fields are zeroed out.  The header file is created as	\n"
" ascii and is translated to ebcdic by segywrite before being	\n"
" written to tape.  Its contents are formal but can be edited	\n"
" after creation as long as the forty line format is maintained.\n"
" 								\n"
" Caveat: This program breaks if a \"short\" isn't 2 bytes since\n"
"         the SEG-Y standard demands a 2 byte integer for ns.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Jack
 */


bhed bh;

main(int argc, char **argv)
{
	string hfile;	/* name of ascii header file	*/
	char buf[80];	/* buffer for ascii file lines	*/
	int hfd;	/* file descriptor for bfile	*/
	string bfile;	/* name of binary header file	*/
	int bfd;	/* file descriptor for bfile	*/
	short ns;	/* number of samples		*/
	int i;		/* counter			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(0); /* stdin not used */


	/* Get parameters */
	MUSTHGETPAR("ns", &ns);
	if (!getparstring("hfile", &hfile))	hfile = "header";
	if (!getparstring("bfile", &bfile))	bfile = "binary";


	/* Open files for writing */
	hfd = eopen(hfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	bfd = eopen(bfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);


	/* Create ascii header */
	sprintf(buf, "%-79s\n", "C      This tape was made at the");
	ewrite(hfd, (char *) buf, 80);
	sprintf(buf, "%-79s\n", "C");
	ewrite(hfd, (char *) buf, 80);
	sprintf(buf, "%-79s\n", "C      Center for Wave Phenomena");
	ewrite(hfd, (char *) buf, 80);
	sprintf(buf, "%-79s\n", "C      Colorado School of Mines");
	ewrite(hfd, (char *) buf, 80);
	sprintf(buf, "%-79s\n", "C      Golden, CO, 80401");
	ewrite(hfd, (char *) buf, 80);
	for (i = 0;  i < 35; i++) {
		sprintf(buf, "%-79s\n", "C");
		ewrite(hfd, (char *) buf, 80);
	}


	/* Create binary header: set hns and format fields */
	bzero(&bh, BNYBYTES);
	bh.hns = ns;
	bh.format = 1;


	/* Write binary header from bh structure to designated file */
	ewrite(bfd, (char *) &bh, BNYBYTES);


	/* Clean up */
	eclose(hfd);
	eclose(bfd);

	return EXIT_SUCCESS;
}
