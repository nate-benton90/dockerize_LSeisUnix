static char RCSsgyout[] = "$Id: sgyout.c,v 1.1 2004/11/12 17:15:22 rbeards Exp $";
 
/*
 *
 * $Source: /app/seismic/NU_SU/app_su/      /src/su/main/RCS/sgyout.c,v $
 *
 * $Log: sgyout.c,v $
 * Revision 1.1  2004/11/12 17:15:22  rbeards
 * 64 bit clean
 *
 * Revision 1.0  2004/05/27 20:26:09  rbeards
 * Initial revision
 *
 * Revision 1.1  2004/05/27 14:58:06  rbeards
 * Initial revision
 *
 * Revision 1.1  2003/09/12 21:16:51  rbeards
 * Initial revision
 *
 * Revision 1.2  1996/04/17 19:32:04  stsssac
 * fixed to write to tape properly
 * 	the fix was done by Rick Ottolini
 *
 * Revision 1.1  1992/12/15  20:41:23  suadm
 * Initial revision
 *
 */
 
#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SGYOUT - convert dataset from SU (IEEE) SEGY to standard (IBM) SEGY  \n"
" 								\n"
" sgyout <stdin >stdout [optional parameters]			\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
"	disk=		output disk file; will also create a    \n"
"			.tpf file which is required when using  \n"
"			the GIN module in Disco			\n"
" 	tape=		output to tape device, see suport.h;	\n"
" 	clean=1		clean trace header beyond standard 180  \n"
" 			bytes;                                  \n"
" 			= 0; do nothing                         \n"
" 	verbose=0	silent operation			\n"
" 			= 1; echo every 20 traces		\n"
" 	convert=1	convert floating point data from ieee to ibm	\n"
" 			= 0; do not attemp conversion	\n"
" 	hfile=		ebcdic card image header file		\n"
"			(if not given, read from input header)	\n"
" 	bfile=		binary header file			\n"
"			(if not given, read from input header)	\n"
" 	trmin=1		first trace to read                     \n"
" 	trmax=LONG_MAX	last trace to read                      \n"
" 								\n"
" Note: The default output is stdout.  The disk and tape        \n"
"	parameters will override default; however, only either  \n"
"	the disk or tape parameter can be used, not both        \n"
" 								\n"
" Note: The header files may be created with the segyhdrs code.	\n"
" 								\n"
" Caveat: This program breaks if a \"short\" isn't 2 bytes or	\n"
"         a \"int\" isn't 4 bytes since the the SEG-Y standard	\n"
"         demands this, but C doesn't impose these sizes.	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Brian, Chris
 *            : Zhiming Li - added ascii and binary headers
 *            : Herb Lam   - cleaned up source code
 *                           added disk and clean parameters
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

segytrace tr;
segychdr  ch;
segybhdr  bh;

main(int argc, char **argv)
{
    FILE     *ofp;     /* file pointer for output     */
    FILE     *tpf_fp;  /* file pointer for .tpf file  */
    FILE     *hfp;     /* file pointer for popen read */
    FILE     *bfp;     /* file pointer for bfile      */
    FILE     *ifp;     /* file pointer for input      */

    string    disk;    /* name of output disk file    */
    string    tape;    /* name of raw tape device     */
    int       clean;   /* clean trace header          */
    int       verbose; /* echo every 20th trace       */
    char     *hfile;   /* name of ebcdic header file  */
    char     *bfile;   /* name of binary header file  */
    int       trmin;   /* first trace to read	      */
    int       trmax;   /* last trace to read	      */
  
    char      cmdbuf[BUFSIZ];	/* dd command buffer	       */
    char      ebcbuf[EBCBYTES];	/* ebcdic data buffer	       */
    char      ascbuf[EBCBYTES];	/* ascii data buffer	       */
    int       inhfile = 1;      /* check input ascii header    */
    int       inbfile = 1;      /* check input binary header   */
    unsigned  int nsamp;	/* number of samples per trace */
    int       itr = 0;	        /* current trace number	       */
    int       disco_tpf;        /* check for disk parameter    */
    char      tpf[50];          /* tpf file name for Disco     */

    /* initialize */
    initargs(argc, argv);
    askdoc(1);

    /* get parameters */
    if (getparstring("disk", &disk)) {
	ofp = efopen(disk, "w");
	disco_tpf = 1;
    }
    else if (getparstring("tape", &tape)) {
	ofp = efopen(tape, "w");
    } else {
	ofp = stdout;
    }

	ifp = stdin;
	file2g(ifp);
	file2g(ofp);


    /* unbuffer for tape output */
    setbuf (ofp,NULL);
    
    if (!getparint("clean", &clean))        clean   = 1;
    if (!getparint   ("verbose", &verbose)) verbose = 0;
    if (!getparstring("hfile", &hfile))     inhfile = 0;
    if (!getparstring("bfile", &bfile))     inbfile = 0;
    if (!getparint("trmin", &trmin))	    trmin   = 1;
    if (!getparint("trmax", &trmax))	    trmax   = INT_MAX;

    /* read from id headers */	
    gethdr(&ch, &bh);	
    
    /* ascii header file input optional */
    if (inhfile == 1) {
	/* open pipe to use dd to convert ascii to ebcdic */
	sprintf(cmdbuf, "dd if=%s conv=ebcdic cbs=80 obs=3200", hfile);
	hfp = epopen(cmdbuf, "r");
	/* read ebcdic stream from pipe into buffer */
	efread(ebcbuf, 1, EBCBYTES, hfp);
    } else {
	fascii_((unsigned char*)&ch, (unsigned char*)ebcbuf, EBCBYTES, 0);
    }
    
    /* binary header file input optional */
    if (inbfile == 1) {
	bfp = efopen(bfile, "r");
	/* read binary file into bh structure */
	efread((char *)&bh, 1, BNYBYTES, bfp);
    }
    
    bh.format = 1;	/* indicate SEG-Y data  */
    
    /* compute trace size (can't use HDRBYTES here!) */
    if (!bh.hns) 
	err("bh.hns not set in binary header");
    nsamp = (bh.hns * 4) + HDRBYTES;
    
    /* output headers to stdout */
    efwrite(ebcbuf, 1, EBCBYTES, ofp);
    efwrite((char *)&bh, 1, BNYBYTES, ofp);

    /* convert the traces */
    while (gettr(&tr) && itr < trmax) {
	
	/* set/check trace header words */
	tr.tracr = ++itr;
	if (tr.ns != bh.hns)
	    err("conflict: tr.ns = %d, bh.hns = %d: trace %d", 
		tr.ns, bh.hns, itr);
	
	/* convert and write desired traces */
	if (itr >= trmin) {
	    
	    /* convert internal floats to IBM floats */
	    conv_float((char *)tr.data, (char *) tr.data, bh.hns, 2);
	    
	    /* clean up trace header beyond 180 bytes */
	    if (clean == 1)
		bzero((char *)&tr + 180, 60);
	    
	    /* write the trace to tape */
	    efwrite((char *)&tr, 1, nsamp, ofp);
	    
	    /* echo under verbose option */
	    if (verbose && itr % 20 == 0 )
		warn("%d traces written to tape", itr);
	}
    }
    
    /* create .tpf file for used with Disco's gin module */
    if (disco_tpf == 1) {
        sprintf(tpf, "%s.tpf", disk);
        tpf_fp = efopen(tpf, "w");
        fprintf(tpf_fp, "     1     3200\n");
        fprintf(tpf_fp, "     1      400\n");
        fprintf(tpf_fp, "%6d   %6d\n", itr, nsamp);
        fprintf(tpf_fp, "     2       FM\n");
        efclose(tpf_fp);
    }

    /* clean up */
    efclose(ofp);
    if(inhfile==1) 
	epclose(hfp);
    if(inbfile==1) 
	efclose(bfp);
    
    return EXIT_SUCCESS;
}
