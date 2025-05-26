#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SEGYREAD - read an SEG-Y tape					\n"
" 								\n"
" segyread >stdout [optional parameters]			\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	tape=		tape device, or name of disk file;      \n"
"			default to <stdin if tape= not given 	\n"
" 	verbose=0	silent operation			\n"
" 			= 1 ; echo every 20 traces		\n"
"	clean=1		clean trace header beyond standard 180  \n"
"			bytes;					\n"
"			= 0 ; do nothing                        \n"	
" 	hfile=		file to store ebcdic block (as ascii)	\n"
"			(if not given, this file is not created)\n"
" 	bfile=		file to store binary block		\n"
"			(if not given, this file is not created)\n"
" 	over=0		quit if bhed.format not equal 1		\n"
" 			= 1; override and attempt conversion  	\n"
" 	trmin=1		first trace to read			\n"
" 	trmax=LONG_MAX	last trace to read			\n"
"	nt=bh.hns	defaults to the number of samples per   \n"
"			trace from binary header; if a value is \n"
"			given, that value will be used to read  \n"
"			in the number of samples per trace; if  \n"
"			nt>bh.hns, the data will be padded with \n"
"			zeroes, if nt<bh.hns, data may be       \n"
"			truncated	                        \n"
"			(if nt is specified or default is used, \n"
"			 it will overwrite the value in both the\n"
"			 the binary header and in the trace     \n"
"			 header)		         	\n"
"			= 0; number of samples will depend on  \n"
"			trace header; will not modify headers   \n"
" 								\n"
" Note: If you have a tape with multiple sequences of binary	\n"
"	header, ebcdic header, traces, use the RMTDEVICE that	\n"
"	invokes the no-rewind option and issue multiple segyread\n"
"	commands (making an appropriate shell script if you	\n"
"	want to save all the headers).  Consider using >> if	\n"
"	you want a single trace file in the end.  Similar	\n"
"	considerations apply for multiple reels of tapes,	\n"
"	but use the standard rewind on end of file.		\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Brian, Chris
 *	      : Zhimig Li --- add ascii and binary headers to output data
 *
 * Notes:
 *	The library subroutine, conv_float, that converts IBM floating
 *	point to IEEE floating point is NOT portable and must be
 *	altered for non-IEEE machines.  However, it has been written with
 *	an eye towards making that alteration rather painless once you
 *	know the byte order for the target machine.
 *
 *	A direct read by dd would suck up the entire tape; hence the
 *	dancing around with buffers and files.
 *
 */

segytrace tr;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
    string  tape;	      /* name of raw tape device	 */
    string  bfile;	      /* name of binary header file	 */
    string  hfile;	      /* name of ascii header file	 */
    int     bfd;	      /* file descriptor for bfile	 */
    FILE   *tfp;	      /* file pointer for tape	         */
    FILE   *fp;	              /* file pointer for popen write	 */
    uint    nsegy;	      /* size of whole trace in bytes    */
    int     itr;	      /* current trace number	         */
    int     trmin;	      /* first trace to read		 */
    int     trmax;	      /* last trace to read		 */
    int     ns,nt;	      /* number of data samples	         */
    int     over;	      /* flag for bhed.float override	 */
    int     verbose;	      /* flag for echoing traces read    */
    int     clean;            /* flag for trace header           */
    bool    nsflag;	      /* flag for error in tr.ns	 */
    char    cmdbuf[BUFSIZ];   /* dd command buffer               */
    char    ebcbuf[EBCBYTES]; /* ebcdic data buffer              */
    char    hdr_buf[10];      /* 1st 10 bytes of header in ascii */
    char    tmp_buf[3600];    /* temp. buffer to read in header  */
    int     i;
    
    /* initialize */
    initargs(argc, argv);
    askdoc(1); /* stdin not used */
    
    /* make sure stdout is a file or pipe */
    switch (filestat(STDOUT)) {
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
    
    /* set filenames */
    if (!getparstring("tape",  &tape))	{
	tfp = stdin;
    } else {
	/* Open files - first the tape */
	tfp = efopen(tape, "r");
    }
    
    /* set parameters */
    if (!getparint("trmin", &trmin))	trmin = 1;
    if (!getparint("trmax", &trmax))	trmax = LONG_MAX;
    if (!getparint("verbose", &verbose)) 	verbose = 0;
    if (!getparint("clean", &clean))        clean = 1;
    
    /* check if user wants to override binary format value */
    if (!getparint("over", &over))		over = 0;
    
    /* read the ebcdic raw bytes from the tape into the buffer */
    efread(ebcbuf, 1, EBCBYTES, tfp);
    
    /* if needed, save ebcbuf into hfile */
    if (getparstring("hfile", &hfile)) {
	/* Open pipe to use dd to convert ebcdic to ascii */
	sprintf(cmdbuf, 
		"dd ibs=3200 of=%s conv=ascii cbs=80 count=1", hfile);
	fp = epopen(cmdbuf, "w");
	/* Write ascii stream from buffer into pipe */
	efwrite(ebcbuf, EBCBYTES, 1, fp);
	epclose(fp);
    }
    
    /* convert ebcdic to ascii for output data */	
    tascii_((unsigned char*)ebcbuf, (unsigned char*)&ch, EBCBYTES, 0);
    
    if (strncmp((char*)&ch, "C 1 CLIENT",10)!=0 ) {
	memcpy((char *)&ch, "C 1 CLIENT", 10);
    }
    
    /* read binary header from tape to bhed structure */
    efread((char *) &bh, 1, BNYBYTES, tfp);
    
    if (bh.format != 1)
	(over) ? warn("ignore bh.format ... continue") :
	    err("format not IBM floating point");
    
    /* bh.format = -1; */   /* indicate that file is no longer SEG-Y */
    
    /* set nt parameter */
    if (!getparint("nt", &nt)) 	        nt = bh.hns;
    
    /* compute length of trace (can't use sizeof here!) */
    if (!bh.hns) {
	warn("samples/trace not set in binary header \n");
	if ( nt==0 ) {
	    warn("samples/trace in 1st trace used \n"); 
	}
	else {
	    warn("nt in input used for samples/trace \n"); 
	}
    }
    
    if ((nt != bh.hns) && (nt != 0)) {
	warn("samples/trace reset in binary header =%d \n",nt);	
	bh.hns = nt;
    }
    
    /* save the binary file, if needed */
    if (getparstring("bfile", &bfile)) {
	/* - the binary data file */
	bfd = eopen(bfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	/* Write binary header from bhed structure to binary file */
	ewrite(bfd, (char *) &bh, BNYBYTES);
	eclose(bfd);
    }
    
    /* output ascii and binary headers to stdout */
    puthdr(&ch,&bh);
    
    /* read the traces */
    nsflag = false;
    itr = 0;
    
    while (efread((char *) &tr, 1, HDRBYTES, tfp) && (itr < trmax)) {
	
	/* check first 10 bytes to look for ebcdic header,
	   if found, this probably indicates a tape switch */
	tascii_((unsigned char*)&tr, &hdr_buf, 10, 0); 
	
	if ((strncmp(hdr_buf, "C 1 CLIENT", 10) == 0) ||
	    (strncmp(hdr_buf, "C CLIENT  ", 10) == 0)) { 
	      /* read in the rest of the header */
	    efread(tmp_buf, 1, 3600 - HDRBYTES, tfp);
	}
	else {
	    /* read in the trace data */
	    nsegy = tr.ns * 4;
	    efread((char *)&tr + 240, 1, nsegy, tfp);

	    /* Check bh.hns with tr.ns */
	    if (bh.hns != tr.ns) {
		
		nsflag = true;
		/* print warning message */
		warn("discrepant tr.ns = %d with bh.hns = %d\n"
		     "\t... noted on trace %d", tr.ns, bh.hns, itr + 1);
		
		/* If user wants to leave things the way they are (nt=0);
		   otherwise, modify number of samples per trace */
		if (nt != 0) { 
		    if ((unsigned short)nt > tr.ns) {
			for (i = tr.ns; i < nt; i++)
			    tr.data[i] = 0.0;
		    }
		    nsegy = nt * 4;
		    tr.ns = nt;
		}
	    }
	    
	    /* convert and write desired traces */
	    if (++itr >= trmin) {
		/* Convert IBM floats to native floats */
		conv_float((char *)tr.data, (char *)tr.data, tr.ns, 1);
		
		/* clean up trace header beyond 180 bytes */
		if (clean == 1)
		    bzero((char *)&tr + 180, 60);
		
		/* write the trace to disk */
		efwrite((char *)&tr, 1, HDRBYTES + nsegy, 
			stdout);
		
		/* echo under verbose option */
		if (verbose && itr % 20 == 0)
		    warn(" %d traces from tape", itr);
	    }
	} 
    } /* while loop */
    
    /* re-iterate error in case not seen during run */
    if ((nsflag) && (nt != 0))
	warn("discrepancy found in header and trace ns values\n"
	     "\theader value (%d) was used to extract traces", nt);
    
    /* clean up */
    efclose(tfp);
    
    return EXIT_SUCCESS;
}
