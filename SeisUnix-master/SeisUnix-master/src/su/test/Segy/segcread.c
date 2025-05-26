/* SEGCREAD: $Revision: $	*/

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
#include "segc.h"
#include "segy.h"

/*********************** self documentation **********************/
String sdoc =
" 								\n"
" SEGCREAD - read an SEG-C tape					\n"
" 								\n"
" segcread >stdout [tape=RMTDEVICE] 				\n"
" 								\n"
" Required parameters:						\n"
"	rcd=pe		(PE) recording method			\n"
"	   =nr		(NRZI) recording method			\n"
" 								\n"
" Optional parameters:						\n"
" 	tape=RMTDEVICE	tape device to use, see suport.h	\n"
" 	bfile=binary	file to store binary header block	\n"
" 	verbose=no	silent operation			\n"
" 	       =yes	echo every 100 scans			\n"
" 								\n"
" Note: It is assumed that all data is recorded in gapless mode \n"
"       with no interblock gaps. If the data is recorded with   \n"
"       interblock gaps, differences in formatting may cause an \n"
"  	improper read of tape.					\n"
"								\n"
"       If you have a tape with multiple siesmic record files,  \n"
"	use the RMTDEVICE that invokes the no-rewind and issue	\n"
"	multiple segcread commands (making the appropriate 	\n"
"	shell script if you want to save all of the headers).	\n"
"	Similar considerations apply for multiple reels of	\n"
"	tapes, but use the standard rewind on end of file.	\n"
;
/************************** end self doc *************************/

/* Credits:
 *	CWP: Jack, Kerry
 *
 * Notes:
 *	The library subroutine, ibm_to_float, that converts IBM floating
 *	point to IEEE floating point is NOT portable and must be
 *	altered for non-IEEE machines.  See the notes in that code.
 *
 *	A direct read by dd would suck up the entire tape; hence the
 *	dancing around with buffers and files.
 *
 */


segc rd;
segy tr;
bhed bh;

main(int argc, char **argv)
{
	String rcd;	/* recording method identifier	*/
	String tape;	/* name of raw tape device	*/
	String bfile;	/* name of binary header file	*/
	String verbose;	/* flag for echoing scans read  */
	int tfd;	/* file descriptor for tape	*/
	int bfd;	/* file descriptor for bfile	*/
        int nch;	/* number of seismic channels	*/
	int nds;	/* number of data scans		*/
	int nby;	/* number of ind. gain bytes	*/
	int n1;		/* current scan number in data array	*/
	int n2;		/* current channel number in data array	*/
	float **data;	/* scan and channel input data array	*/
	float dv;	/* individual data value	*/
	char ganbuf[BUFSIZ];	/* ind. gain buffer	*/
	char trabuf[BUFSIZ];	/* byte trash buffer	*/

	/* Initialize */
	initargs(argc, argv);
	askdoc(0); /* stdin not used */


	/* Make sure stdout is a file or pipe */
	switch(filestat(STDOUT)) {
	case TTY:
		err("stdout can't be tty");
	break;
	case DIRECTORY:
		err("stdout must be a file, not a directory");
	break;
	case BADFILETYPE:
		err("stdout is illegal filetype");
	break;
	}


	/* Set filenames */
	if (!getparstring("tape",  &tape))	tape = RMTDEVICE;
	if (!getparstring("bfile", &bfile))	bfile = "binary";
	
	/* Set parameters */
	if (!getparstring("rcd", &rcd))		err("must specify %s=?",rcd);
	if (!getparstring("verbose", &verbose)) verbose = "no";

	/* Open files - first the tape */
	tfd = eopen(tape, O_RDONLY, 0444);

	/* Next the binary data file */
	bfd = eopen(bfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	/* If the phase encoded recording method is being used, 41 bytes of 
	   preamble must be striped away */
   	if (rcd = "pe")		eread(tfd, trabuf, 41); 
	    
	/* Read binary header from tape to segc structure */
	eread(tfd, (char *) &rd, BINARYC);

	/* Compute the number of channels being used in this data record */
	nch = (rd.numb / 128) * 30;
	
	/* Compute the number of data scans being used in this data record */
	nds = (rd.lenth * 1000) / rd.inter;
	
	/* Determine if a common gain constant is used; if not, individual
	   gain data for each channel must be read from the tape */
	if (rd.gainc = 0) {
		warn("no common gain constant being used..");
		nby = 4 * nch;
		eread(tfd, ganbuf, nby);
	}

	/* Write binary header from segc structure to binary file */
	ewrite(bfd, (char *) &rd, BINARYC);
	
	/* If there is no common gain constant, write individual gain data
	   for each channel to the binary file */
	if (rd.gainc = 0)	ewrite(bfd, ganbuf, nby);

	/* Read the seismic record into a two dimensional array where n1 is
	   the scan number and n2 is the channel number. A eight byte sync
	   group must be striped away at the begining of each scan */
	data = ealloc2float(n1,n2);	   
	for (n1=0; n1<nds; ++n1) {
		eread(tfd, trabuf, 8);
		for (n2=0; n2<nch; ++n2) {
			eread(tfd, (char *) &dv, 4);
			data [n1][n2] = dv;
		}
		/* echo under verbose option */
		if (verbose = "yes" && n1 % 100 == 0)
			warn(" %d scans from tape", n1);
	}
	
	/* 		



	/* Clean up */
	eclose(tfd);
	eclose(bfd);

	return EXIT_SUCCESS;
}
