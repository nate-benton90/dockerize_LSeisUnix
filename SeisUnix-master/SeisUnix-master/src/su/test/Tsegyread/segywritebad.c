/* SEGYWRITE: $Revision: 1.8 $ ; $Date: 91/09/05 08:49:15 $	*/

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
String sdoc =
" 								\n"
" SEGYWRITE - write an SEG-Y tape				\n"
" 								\n"
" segywrite <stdin [tape=RMTDEVICE] 				\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	verbose=0	silent operation			\n"
" 			= 1 ; echo every 20 traces		\n"
" 	tape=RMTDEVICE	tape device to use, see suport.h	\n"
" 	hfile=header	ebcdic card image header file		\n"
" 	bfile=binary	binary header file			\n"
" 	trmin=1	first trace to write				\n"
" 	trmax=LONG_MAX	last trace to write			\n"
" 								\n"
" Note: The header files may be created with the segyhdrs code.	\n"
" 								\n"
" Caveat: This program breaks if a \"short\" isn't 2 bytes or	\n"
"         a \"long\" isn't 4 bytes since the the SEG-Y standard	\n"
"         demands this, but C doesn't impose these sizes.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Brian, Chris
 *
 * Notes:
 *	The library subroutine, conv_float, that converts IEEE
 *	floating point to IBM floating point is NOT portable and must be
 *	altered for non-IEEE machines.  However, it has been written with
 *	an eye towards making that alteration rather painless once you
 *	know the byte order for the target machine.
 *
 *	The program, segyhdrs, can be used to make the ascii and binary
 *	files required by this code.
 *
 *
 */


segy tr;
bhed bh;

main(int argc, char **argv)
{
	String tape;	/* name of raw tape device	*/
	String hfile;	/* name of ebcdic header file	*/
	String bfile;	/* name of binary header file	*/
	FILE *fp;	/* file pointer for popen read	*/
	int tfd;	/* file descriptor for tape	*/
	int bfd;	/* file descriptor for bfile	*/
	int ns;	        /* number of data samples	*/
	unsigned
	 int nsegy;	/* size of whole trace in bytes */
	int itr;	/* current trace number		*/
	int trmax;	/* last trace to write		*/
	int trmin;	/* first trace to write		*/
	int verbose;	/* echo every 20th trace 	*/
	char cmdbuf[BUFSIZ];	/* dd command buffer	*/
	char ebcbuf[EBCBYTES];	/* ebcdic data buffer	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	if (!getparstring("tape", &tape))       tape  = RMTDEVICE;
	if (!getparstring("hfile", &hfile))     hfile = "header";
	if (!getparstring("bfile", &bfile))     bfile = "binary";
	if (!getparint   ("trmin", &trmin))     trmin = 1;
	if (!getparint   ("trmax", &trmax))     trmax = LONG_MAX;
	if (!getparint   ("verbose", &verbose)) verbose = 0;


	/* Open files - first the tape */
	tfd = eopen(tape, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	/* - binary header file */
	bfd = eopen(bfile, O_RDONLY, 0666);


	/* Open pipe to use dd to convert ascii to ebcdic */
	sprintf(cmdbuf, "dd if=%s conv=ebcdic cbs=80 obs=3200", hfile);
	fp = epopen(cmdbuf, "r");


	/* Read ebcdic stream from pipe into buffer */
	efread(ebcbuf, 1, EBCBYTES, fp);


	/* Write ebcdic stream to tape */
	ewrite(tfd, ebcbuf, EBCBYTES);


	/* Read binary file into bh structure */
	eread(bfd, (char *) &bh, BNYBYTES);
	bh.format = 1;	/* indicate SEG-Y data  */
	bh.ntrpr  = 1;  /* one trace per record */


	/* Compute trace size (can't use HDRBYTES here!) */
	if (!getparint("ns", &ns))  ns = bh.hns;
	if (!ns) err("bh.hns not set in binary header");
	nsegy = ns*4 + 240;


	/* Write binary structure to tape */
	ewrite(tfd, (char *) &bh, BNYBYTES);


	/* Copy traces from stdin to tape */
	itr = 0;
	while (gettr(&tr) && itr < trmax) {

		/* Set/check trace header words */
		tr.tracr = ++itr;
		if (tr.ns != ns)
			err("conflict: tr.ns = %d, bh.ns = %d: trace %d",
					tr.ns, ns, itr);

		/* Convert and write desired traces */
		if (itr >= trmin) {

			/* Convert internal floats to IBM floats */
			conv_float((char *) tr.data, (char *) tr.data, ns, 2);

			/* Write the trace to tape */
			ewrite(tfd, (char *) &tr, nsegy);

			/* Echo under verbose option */
			if (verbose && itr % 20 == 0 )
				warn(" %d traces written to tape", itr);
		}
	}


	/* Clean up */
	eclose(tfd);
	eclose(bfd);
	epclose(fp);


	return EXIT_SUCCESS;
}
