#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SEGYWRITE - write an SEG-Y tape				\n"
" 								\n"
" segywrite <stdin [optional parameters]			\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	tape=		tape device to use, see suport.h	\n"
"			default to >stdout if tape= not given)	\n" 
" 	verbose=0	silent operation			\n"
" 			= 1; echo every 20 traces		\n"
" 	hfile=		ebcdic card image header file		\n"
"			(if not given, read from input header)	\n"
" 	bfile=		binary header file			\n"
"			(if not given, read from input header)	\n"
" 	trmin=1		first trace to write			\n"
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
 *            : Zhiming LI	add ascii and binary headers
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
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
    string  tape;	      /* name of raw tape device      */
    char   *hfile;	      /* name of ebcdic header file   */
    char   *bfile;	      /* name of binary header file   */
    FILE   *fp;	              /* file pointer for popen read  */
    FILE   *tfp;	      /* file pointer for tape	      */
    FILE   *bfp;	      /* file pointer for bfile	      */
    int     ns;	              /* number of data samples	      */
    uint    nsegy;	      /* size of whole trace in bytes */
    int     itr;	      /* current trace number	      */
    int     trmax;	      /* last trace to write	      */
    int     trmin;	      /* first trace to write	      */
    int     verbose;	      /* echo every 20th trace 	      */
    char    cmdbuf[BUFSIZ];   /* dd command buffer	      */
    char    ebcbuf[EBCBYTES]; /* ebcdic data buffer	      */
    char    ascbuf[EBCBYTES]; /* ascii data buffer	      */
    int     inhfile=1;
    int     inbfile=1;
    int     intape=1;
    
    /* initialize */
    initargs(argc, argv);
    askdoc(1);

    /* get parameters */
    if (!getparstring("tape", &tape))       intape  = 0;
    
    if (!getparstring("hfile", &hfile))     inhfile = 0;
    if (!getparstring("bfile", &bfile))     inbfile = 0;
    if (!getparint   ("trmin", &trmin))     trmin = 1;
    if (!getparint   ("trmax", &trmax))     trmax = LONG_MAX;
    if (!getparint   ("verbose", &verbose)) verbose = 0;
    
    /* open files - first the tape */
    if(intape==1) {
	tfp = efopen(tape, "w");
    } else {
	tfp = stdout;
    }
    
    /* read from id headers */	
    bzero((char*)&bh,BNYBYTES);
    gethdr(&ch,&bh);	
    
    /* - ascii header file input optional */
    if(inhfile==1) {
	/* Open pipe to use dd to convert ascii to ebcdic */
	sprintf(cmdbuf, "dd if=%s conv=ebcdic cbs=80 obs=3200", hfile);
	fp = epopen(cmdbuf, "r");
	/* Read ebcdic stream from pipe into buffer */
	efread(ebcbuf, 1, EBCBYTES, fp);
    } else {
	fascii_((unsigned char*)&ch, (unsigned char*)ebcbuf, EBCBYTES, 0);
    }
    
    /* write ebcdic stream to tape */
    efwrite(ebcbuf, 1, EBCBYTES, tfp);
    
    /* - binary header file input optional */
    if(inbfile==1) {
	bfp  = efopen(bfile,"r");
	/* Read binary file into bh structure */
	efread((char *)&bh,1,BNYBYTES,bfp);
    }
    
    /* bh.ntrpr  = 1; */ /* one trace per record */
    bh.format = 1;	/* indicate SEG-Y data  */
    
    /* compute trace size (can't use HDRBYTES here!) */
    ns = bh.hns;
    if (!ns) err("bh.hns not set in binary header");
    nsegy = ns*4 + 240;
    
    /* write binary structure to tape */
    efwrite((char *)&bh, 1, BNYBYTES, tfp);

    /* copy traces from stdin to tape or stdout */
    itr = 0;
    while (gettr(&tr) && itr < trmax) {
	
	/* set/check trace header words */
	tr.tracr = ++itr;
	if (tr.ns != ns)
	    err("conflict: tr.ns = %d, bh.ns = %d: trace %d",
		tr.ns, ns, itr);
	
	/* convert and write desired traces */
	if (itr >= trmin) {
	    
	    /* convert internal floats to IBM floats */
	    conv_float((char *) tr.data, (char *) tr.data, ns, 2);
	    
	    /* write the trace to tape */
	    efwrite((char *) &tr, 1, nsegy, tfp);
	    
	    /* echo under verbose option */
	    if (verbose && itr % 20 == 0 )
		warn(" %d traces written to tape", itr);
	}
    }
    
    /* clean up */
    efclose(tfp);
    if(inbfile==1) efclose(bfp);
    if(inhfile==1) epclose(fp);
    
    return EXIT_SUCCESS;
}
