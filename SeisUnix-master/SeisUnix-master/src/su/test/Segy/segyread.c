/* SEGYREAD: $Revision: 1.12 $ ; $Date: 92/11/06 08:29:46 $	*/

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
#include "segytape.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" SEGYREAD - read an SEG-Y tape						",
" 									",
" segyread >stdout [tape=RMTDEVICE] 					",
" 									",
" Required parameters:							",
" 	none								",
" 									",
" Optional parameters:							",
" 	tape=RMTDEVICE	tape device to use, see suport.h		",
"	buff=1		for buffered device (standard 9-trac reel tape drive)",
"                       =0 for unbuffered device (8mm EXABYTE tape drive)",
" 	verbose=0	silent operation				",
" 			= 1 ; echo every 50 traces			",
" 	hfile=header	file to store ebcdic block (as ascii)		",
" 	bfile=binary	file to store binary block			",
" 	over=0		quit if bhed format not equal 1			",
" 			= 1 ; override and attempt conversion		",
" 	ns=bh.hns	number of samples (use if bhed ns wrong)	",
" 	trmin=1		first trace to read				",
" 	trmax=LONG_MAX	last trace to read				",
" 									",
" Note: If you have a tape with multiple sequences of binary		",
"	header, ebcdic header, traces, use the RMTDEVICE that		",
"	invokes the no-rewind option and issue multiple segyread	",
"	commands (making an appropriate shell script if you		",
"	want to save all the headers).  Consider using >> if		",
"	you want a single trace file in the end.  Similar		",
"	considerations apply for multiple reels of tapes,		",
"	but use the standard rewind on end of file.			",
" 									",
" Note: For buff=1 (default) tape is accessed with 'write', for buff=0	",
" 	tape is accessed with fwrite.					",
" 									",
" Caveat: may be slow on an 8mm streaming (EXABYTE) tapedrive		",
" Warning: segyread or segywrite to 8mm tape is fragile. Allow sufficient",
"	   time between successive reads and writes.			",
" Bug: may return the error message \"efclose: fclose failed\"		",
" 	intermittently when segyreading/segywriting to 8mm (EXABYTE) tape,",
"	even if actual segyread/segywrite is successful.		",
NULL};
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Brian, Chris
 *	   : John Stockwell (added 8mm tape stuff)
 * Notes:
 *	Brian's subroutine, ibm_to_float, that converts IBM floating
 *	point to IEEE floating point is NOT portable and must be
 *	altered for non-IEEE machines.  See the subroutine notes below.
 *
 *	The subroutines bhedtape_to_bhedsu and segytape_to_segysu are
 *	only stubs and are not portable.  On machines where shorts are
 *	not 2 bytes and/or longs are not 4 bytes, routines to convert
 *	SEGY 16 bit and 32 bit integers will be required.
 *
 *	A direct read by dd would suck up the entire tape; hence the
 *	dancing around with buffers and files.
 *
 */

/* subroutine prototypes */
void ibm_to_float(int from[], int to[], int n);
void bhedtape_to_bhedsu(void *tapebhptr, void *bhptr); 
void segytape_to_segysu(void *tapetrptr, void *trptr, int nsegy); 

segytape tapetr;
bhedtape tapebh;
segy tr;
bhed bh;

main(int argc, char **argv)
{
	String tape;		/* name of raw tape device	*/
	String bfile;		/* name of binary header file	*/
	String hfile;		/* name of ascii header file	*/

	int tapefd;		/* file descriptor for tape	*/

	FILE *tapefp;		/* file pointer for tape	*/
	FILE *binaryfp;		/* file pointer for bfile	*/
	FILE *headerfp;		/* file pointer for hfile	*/
	FILE *pipefp;		/* file pointer for popen write	*/

	unsigned int nsegy;	/* size of whole trace in bytes 	*/
	int itr;		/* current trace number			*/
	int trmin;		/* first trace to read			*/
	int trmax;		/* last trace to read			*/
	int ns;			/* number of data samples		*/
	int over;		/* flag for bhed.float override		*/
	int verbose;		/* flag for echoing traces read		*/
	int buff;		/* flag for buffered/unbuffered device	*/
	Bool nsflag;		/* flag for error in tr.ns		*/
	char cmdbuf[BUFSIZ];	/* dd command buffer			*/
	char ebcbuf[EBCBYTES];	/* ebcdic data buffer			*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(0); /* stdin not used */


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
	if (!getparstring("hfile", &hfile))	hfile = "header";
	if (!getparstring("bfile", &bfile))	bfile = "binary";

	
	/* Set parameters */
	if (!getparint("trmin", &trmin))	trmin = 1;
	if (!getparint("trmax", &trmax))	trmax = LONG_MAX;
	if (!getparint("verbose", &verbose)) 	verbose = 0;
	if (!getparint("buff", &buff)) 		buff = 1;


	/* Check if user wants to override binary format value */
	if (!getparint("over", &over))		over = 0;


	/* Open files - first the tape */
	if (buff) tapefd = eopen(tape, O_RDONLY, 0444);
	else	  tapefp = efopen(tape, "r");
	if (verbose) warn("tape opened successfully");

	/* - the ebcdic header file in ascii */
	headerfp = efopen(hfile, "w");
	if (verbose) warn("header file opened successfully");

	/* - the binary data file */
	binaryfp = efopen(bfile, "w");
	if (verbose) warn("binary file opened successfully");


	/* Read the ebcdic raw bytes from the tape into the buffer */
	(buff) ? eread(tapefd, ebcbuf, EBCBYTES):
		 efread(ebcbuf, 1, EBCBYTES,tapefp);


	/* Open pipe to use dd to convert ascii to ebcdic */
	sprintf(cmdbuf, "dd ibs=3200 of=%s conv=ascii cbs=80 count=1", hfile);
	pipefp = epopen(cmdbuf, "w");


	/* Write ebcdic stream from buffer into pipe */
	efwrite(ebcbuf, EBCBYTES, 1, pipefp);


	/* Read binary header from tape to bhedtape structure */
	(buff) ? eread(tapefd, (char *) &tapebh, BNYBYTES):
		 efread((char *) &tapebh, 1, BNYBYTES,tapefp);

	if (tapebh.format != 1)
		(over) ? warn("ignoring bh.format ... continue") :
			 err("format not IBM floating point");
			 
	/* Convert from bytes to longs/shorts */
	bhedtape_to_bhedsu(&tapebh, &bh);
	bh.format = -1;   /* indicate that file is no longer SEG-Y */

	/* Compute length of trace (can't use sizeof here!) */
	if (!getparint("ns", &ns))  ns = bh.hns; /* let user override */
	if (!ns) err("samples/trace not set in binary header");
	nsegy = ns*4 + 240;

	/* Write binary header from bhed structure to binary file */
	efwrite( (char *) &bh,1, BNYBYTES, binaryfp);
	

	/* Read the traces */
	nsflag = false;
	itr = 0;
	while (itr < trmax) {
		int nread = (buff) ? eread(tapefd, (char *) &tapetr, nsegy) :
			 efread((char *) &tapetr, 1, nsegy, tapefp);
			 
	if (!nread) break; /* middle exit loop instead of mile-long while */

		/* Convert from bytes to longs/shorts */
		segytape_to_segysu(&tapetr, &tr, nsegy);
		
		/* Check tr.ns field */
		if (!nsflag && ns != tr.ns) {
			warn("discrepant tr.ns = %d with tape/user ns = %d\n"
				"\t... first noted on trace %d", 
				tr.ns, ns, itr + 1);
			nsflag = true;
		}

		/* Convert and write desired traces */
		if (++itr >= trmin) {
			/* Convert IBM floats to native floats */
			ibm_to_float((int *) tr.data, 
					(int *) tr.data, ns);

			/* Write the trace to disk */
			puttr(&tr);

			/* Echo under verbose option */
			if (verbose && itr % 50 == 0)
				warn(" %d traces from tape", itr);
		}
	}



	/* Re-iterate error in case not seen during run */
	if (nsflag) warn("discrepancy found in header and trace ns values\n"
		"the value (%d) was used to extract traces", ns);


	/* Clean up */
	(buff) ? eclose(tapefd):
		 efclose(tapefp);
	if (verbose) warn("tape closed successfully");

	efclose(binaryfp);
	if (verbose) warn("binary file closed successfully");

	efclose(headerfp);
	if (verbose) warn("header file closed successfully");

	epclose(pipefp);

	return EXIT_SUCCESS;
}


/* ibm_to_float - convert between 32 bit IBM and IEEE floating numbers
 *
 * Credits:
 *	CWP: Brian
 *
 * Parameters:
 *    from	- input vector
 *    to	- output vector, can be same as input vector
 *    len	- number of floats in vectors
 *    type	- conversion type
 *
 * Notes:
 *	Up to 3 bits lost on IEEE -> IBM
 *
 *	IBM -> IEEE may overflow or underflow, taken care of by 
 *	substituting large number or zero
 *
 *	Only integer shifting and masking are used.
 *
 *	This routine assumes a big-endian machine.  If yours is little
 *	endian you will need to reverse the bytes in ibm_to_float
 *	with something like
 *
 *	fconv = from[i];
 *	fconv = (fconv<<24) | ((fconv>>24)&0xff) |
 *		((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);
 *
 */

/* Assumes sizeof(int) == 4 */
void ibm_to_float(int from[], int to[], int n)
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
	fconv = from[i];
	if (fconv) {
            fmant = 0x00ffffff & fconv;
            t = (int) ((0x7f000000 & fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
            if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
            else if (t <= 0) fconv = 0;
            else fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
        }
	to[i] = fconv;
    }
    return;
}


/* Next two routines are just stubs */

/* Assumes sizeof(short) == 2, sizeof(long) == 4 */
void bhedtape_to_bhedsu(void *tapebhptr, void *bhptr)
{
	memcpy(bhptr, tapebhptr, BNYBYTES);
} 
/* Assumes sizeof(short) == 2, sizeof(long) == 4 */
void segytape_to_segysu(void *tapetrptr, void *trptr, int nsegy)
{
	memcpy(trptr, tapetrptr, nsegy);
} 
