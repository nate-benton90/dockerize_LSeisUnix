/* SEGYWRITE: $Revision: 1.11 $ ; $Date: 92/11/06 08:30:04 $	*/

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
" SEGYWRITE - write an SEG-Y tape					",
" 									",
" segywrite <stdin [tape=RMTDEVICE] 					",
" 									",
" Required parameters:							",
" 	none								",
" 									",
" Optional parameter:							",
" 	verbose=0	silent operation				",
" 			=1 ; echo every 20 traces			",
" 	tape=RMTDEVICE	tape device to use, see suport.h		",
" 	buff=1		for buffered device (standard 9-trac reel tape drive)",
" 			=0 for unbuffered device (8mm EXABYTE tape drive)",
" 	hfile=header	ebcdic card image header file			",
" 	bfile=binary	binary header file				",
" 	trmin=1	first trace to write					",
" 	trmax=LONG_MAX	last trace to write				",
" 	buff=1		for buffered device (standard 9-trac reel tape drive)",
" 			=0 for unbuffered device (8mm EXABYTE tape drive)",
" 									",
" Note: The header files may be created with the segyhdrs code.		",
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
 *	   : John Stockwell (added EXABYTE functionality)
 * Notes:
 *	Brian's subroutine, float_to_ibm, that converts IEEE floating
 *	point to IBM floating point is NOT portable and must be
 *	altered for non-IEEE machines.  See the subroutine notes below.
 *
 *	The subroutines bhedsu_to_bhedtape and segysu_to_segytape are
 *	only stubs and are not portable.  On machines where shorts are
 *	not 2 bytes and/or longs are not 4 bytes, routines to convert
 *	SEGY 16 bit and 32 bit integers will be required.
 *
 *	The program, segyhdrs, can be used to make the ascii and binary
 *	files required by this code.
 *
 */

/* subroutine prototypes */
void float_to_ibm(int from[], int to[], int n);
void bhedsu_to_bhedtape(void *bhptr, void *tapebhptr); 
void segysu_to_segytape(void *trptr, void *tapetrptr, int nsegy); 

segytape tapetr;
bhedtape tapebh;
segy tr;
bhed bh;

main(int argc, char **argv)
{
	String tape;		/* name of raw tape device		*/
	String hfile;		/* name of ebcdic header file		*/
	String bfile;		/* name of binary header file		*/

	FILE *pipefp;		/* file pointer for popen read		*/
	FILE *tapefp;		/* file pointer for tape		*/
	FILE *binaryfp;		/* file pointer for bfile		*/

	int tapefd;		/* file discriptor for tape buff=0	*/

	int ns;			/* number of data samples		*/
	unsigned int nsegy;	/* size of whole trace in bytes		*/
	int itr;		/* current trace number			*/
	int trmax;		/* last trace to write			*/
	int trmin;		/* first trace to write			*/
	int verbose;		/* echo every 20th trace 		*/
	int buff;		/* buffered or unbuffered device 	*/
	char cmdbuf[BUFSIZ];	/* dd command buffer			*/
	char ebcbuf[EBCBYTES];	/* ebcdic data buffer			*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Get parameters */
	if (!getparstring("tape", &tape))       tape  = RMTDEVICE;
	if (!getparstring("hfile", &hfile))     hfile = "header";
	if (!getparstring("bfile", &bfile))     bfile = "binary";
	if (!getparint   ("trmin", &trmin))     trmin = 1;
	if (!getparint   ("trmax", &trmax))     trmax = LONG_MAX;
	if (!getparint   ("verbose", &verbose)) verbose = 0;
	if (!getparint   ("buff", &buff)) 	buff = 1;


	/* Open files - first the tape */
	if (buff) tapefd = eopen(tape, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else tapefp = efopen(tape, "w");
	if (verbose) warn("tape opened successfully ");

	/* - binary header file */
	binaryfp = efopen(bfile, "r");
	if (verbose) warn("binary file opened successfully ");


	/* Open pipe to use dd to convert ascii to ebcdic */
	sprintf(cmdbuf, "dd if=%s conv=ebcdic cbs=80 obs=3200", hfile);
	pipefp = epopen(cmdbuf, "r");

	/* Read ebcdic stream from pipe into buffer */
	efread(ebcbuf, 1, EBCBYTES, pipefp);

	/* Write ebcdic stream to tape */
	(buff) ? ewrite(tapefd, ebcbuf, EBCBYTES) :
		 efwrite(ebcbuf, 1, EBCBYTES, tapefp);

	/* Read binary file into bh structure */
	efread((char *) &bh, 1, BNYBYTES, binaryfp);
	bh.format = 1;	/* indicate SEG-Y data  */
	bh.ntrpr  = 1;  /* one trace per record */

	/* Compute trace size (can't use HDRBYTES here!) */
	ns = bh.hns;
	if (!ns) err("bh.hns not set in binary header");
	nsegy = ns*4 + 240;

	/* Convert from longs/shorts to bytes */
	bhedsu_to_bhedtape(&bh, &tapebh);

	/* Write binary structure to tape */
	(buff) ? ewrite(tapefd, (char *) &tapebh, BNYBYTES) :
		 efwrite((char *) &bh,1, BNYBYTES,tapefp);


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
			float_to_ibm((int *) tr.data, (int *) tr.data, ns);
			
			/* Convert from longs/shorts to bytes*/
			segysu_to_segytape(&tr, &tapetr, nsegy);
		
			/* Write the trace to tape */
			(buff) ? ewrite(tapefd, (char *) &tapetr, nsegy) :
				 efwrite((char *) &tapetr, 1, nsegy,tapefp);

			/* Echo under verbose option */
			if (verbose && itr % 50 == 0 )
				warn(" %d traces written to tape", itr);
		}
	}


	/* Clean up */
	(buff) ? eclose(tapefd) :
		 efclose(tapefp);
	if (verbose) warn("tape closed successfully");

	efclose(binaryfp);
	if (verbose) warn("binary file closed successfully");

	epclose(pipefp);


	return EXIT_SUCCESS;
}


/* float_to_ibm - convert between 32 bit IBM and IEEE floating numbers
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
void float_to_ibm(int from[], int to[], int n)
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
	fconv = from[i];
	if (fconv) {
            fmant = (0x007fffff & fconv) | 0x00800000;
            t = (int) ((0x7f800000 & fconv) >> 23) - 126;
            while (t & 0x3) { ++t; fmant >>= 1; }
            fconv = (0x80000000 & fconv) | (((t>>2) + 64) << 24) | fmant;
        }
	to[i] = fconv;
    }
    return;
}


/* Next two routines are just stubs */

/* Assumes sizeof(short) == 2, sizeof(long) == 4 */
void bhedsu_to_bhedtape(void *bhptr, void *tapebhptr)
{
	memcpy(tapebhptr, bhptr, BNYBYTES);
} 
/* Assumes sizeof(short) == 2, sizeof(long) == 4 */
void segysu_to_segytape(void *trptr, void *tapetrptr, int nsegy)
{
	memcpy(tapetrptr, trptr, nsegy);
} 
