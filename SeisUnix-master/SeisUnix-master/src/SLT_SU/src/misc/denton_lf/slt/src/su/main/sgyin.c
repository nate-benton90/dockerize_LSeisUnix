static char RCSsgyin[] = "$Id: sgyin.c,v 1.1 1992/12/15 20:41:23 suadm Exp stgpkdh $";
 
/*
 *
 * $Source: /vol/SU/      /src/su/main/RCS/sgyin.c,v $
 *
 * $Log: sgyin.c,v $
 * Revision 1.1  1992/12/15  20:41:23  suadm
 * Initial revision
 *
 */
 
#include "su.h"
#include "wgc.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SGYIN - convert dataset from standard (IBM) SEGY to SU (IEEE) SEGY  \n"
" 								\n"
" sgyin <stdin >stdout [optional parameters]			\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	tape=		input tape device;                      \n"
"			default to <stdin if tape= not given 	\n"
"	clean=1		clean trace header beyond standard 180  \n"
"			bytes;					\n"
"			= 0; do nothing				\n"	
" 	verbose=0	silent operation			\n"
" 			= 1; echo every 20 traces		\n"
"                            print every bad traces 		\n"
" 	over=0		quit if bhed.format not equal 1		\n"
" 			= 1; override and attempt conversion  	\n"
" 	convert=1	= 1; convert float pt data (IBM->IEEE)	\n"
" 			= 0; do not attempt conversion		\n"
" 	hfile=		file to store ebcdic block (as ascii)	\n"
"			(if not given, this file is not created)\n"
" 	bfile=		file to store binary block		\n"
"			(if not given, this file is not created)\n"
" 	trmin=1		first trace to read			\n"
" 	trmax=LONG_MAX	last trace to read			\n"
"	nt=bh.hns	number of samples per trace 	\n"	
"			defaults to the number of samples per   \n"
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
"  rmbadtrace=0         remove bad trace whose number of samples \n"
"                       in the trace header is not the same as \n"
"                       that in the binary header (0=no 1=yes) \n"
"                       if 0, the trace will be output with nt \n"
"                       samples \n"
"  ibstart= starting byte of input header to be mapped to output heade \n"
"  ibyte=   number of bytes of input header to be mapped \n"
"  itype=   type of input key word data \n"
"           (=0 2-byte or 4-byte interge; =1 4-byte float)    \n"
"  obstart= starting byte of output header \n"
"  obyte=   number of bytes in output header word \n"
"  otype=   type of output key word data   \n"
"           (=0 2-byte or 4-byte interge; =1 4-byte float)    \n"
" 								\n"
" Note: If you have a tape with multiple sequences of binary	\n"
"	header, ebcdic header, traces, use the RMTDEVICE that	\n"
"	invokes the no-rewind option and issue multiple segyread\n"
"	commands (making an appropriate shell script if you	\n"
"	want to save all the headers).  Consider using >> if	\n"
"	you want a single trace file in the end.  Similar	\n"
"	considerations apply for multiple reels of tapes,	\n"
"	but use the standard rewind on end of file.		\n"
" \n"
"   ibstart, ibyte, itype, obstart, obyte and otype could be specified \n"
"   as arrays: ibyte=181,185,189 ibyte=4,4,4 itype=0,0,1 \n" 
"              obyte=1,5,9 obyte=4,4,4 otype=0,0,0 \n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack, Brian, Chris
 *	      : Zhimig Li - added ascii and binary headers to output data
 *            : Herb Lam  - cleaned up source code
 *                          added clean parameter
 *            : David DeBaun - added convert parameter
 *            : Zhiming Li - added ibstart,...,otype parameters
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
segytrace tro;
segychdr  ch;
segybhdr  bh;

main(int argc, char **argv)
{
    FILE     *ifp;     /* file pointer for input       */
    FILE     *hfp;     /* file pointer for popen write */
    int       bfd;     /* file descriptor for bfile    */

    string    tape;    /* name of raw tape device      */
    int       clean;   /* clean trace header           */
    int       verbose; /* echo every 20th trace        */
    int       over;    /* check format                 */
    int	      convert; /* convert ibm fpt to ieee fpt  */
    string    hfile;   /* name of ascii header file    */
    string    bfile;   /* name of binary header file   */
    int       trmin;   /* first trace to read	       */
    int       trmax;   /* last trace to read	       */
    int       nt;      /* number of data samples       */

    char      cmdbuf[BUFSIZ];	/* dd command buffer	              */
    char      ebcbuf[EBCBYTES];	/* ebcdic data buffer	              */
    int       itr = 0;	        /* current trace number		      */
    bool      nsflag = FALSE;	/* flag for error in tr.ns	      */
    char      hdr_buf[10];      /* 1st 10 bytes of header in ascii    */
    char      tmp_buf[3600];    /* temp. buffer to read in header     */
    unsigned  int nsamp;	/* number of samples per trace        */
    int       i;                /* loop counter to zero trace samples */
	int       *ibstart,*ibyte,*itype;
	int       *obstart,*obyte,*otype;
	int       nmap=0, imap;
	int       ibs,iby,ity,obs,oby,oty;
	short     itmp2;
	int       itmp4;
	float	  tmp;
	int		  ntg=0;
	int 	rmbadtrace, ibt, nbt;
	short *trace2b;
	int nibyte, nobyte;
	int iformat;
    
    /* initialize */
    initargs(argc, argv);
    askdoc(1); 
    
    /* make sure stdout is a file or pipe */
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
    
    /* set filenames */
    if (!getparstring("tape", &tape)) { 
	ifp = stdin;
	file2g(ifp);
    } else {
	/* open files - first the tape */
	ifp = efopen(tape, "r");
    }

    file2g(stdout); 

    /* set parameters */
    if (!getparint("clean", &clean))     clean   = 1;
    if (!getparint("verbose", &verbose)) verbose = 0;
    if (!getparint("over", &over))	 over    = 0;
    if (!getparint("convert", &convert)) convert    = 1;
    if (!getparint("trmin", &trmin))	 trmin   = 1;
    if (!getparint("trmax", &trmax))	 trmax   = LONG_MAX;
    if (!getparint("rmbadtrace",&rmbadtrace)) rmbadtrace=0;

	nmap = countparval("ibstart");
	if(nmap>0) {
		ibstart = (int*) malloc(nmap*sizeof(int));
		ibyte = (int*) malloc(nmap*sizeof(int));
		itype = (int*) malloc(nmap*sizeof(int));
		obstart = (int*) malloc(nmap*sizeof(int));
		obyte = (int*) malloc(nmap*sizeof(int));
		otype = (int*) malloc(nmap*sizeof(int));
		if(getparint("ibstart",ibstart)!=nmap) err(" check ibstart");
		if(getparint("ibyte",ibyte)!=nmap) err(" check ibyte");
		if(getparint("itype",itype)!=nmap) err(" check itype");
		if(getparint("obstart",obstart)!=nmap) err(" check obstart");
		if(getparint("obyte",obyte)!=nmap) err(" check obyte");
		if(getparint("otype",otype)!=nmap) err(" check otype");
	}
    
    /* read ebcdic and binary headers */
    efread(ebcbuf, 1, EBCBYTES, ifp);
    efread((char *)&bh, 1, BNYBYTES, ifp);

    iformat = bh.format;
    
    if (iformat != 1) {
	if(over==1) { warn("ignore bh.format=%d, assuming it's ibm floats ... continue \n",iformat); }
	else {
	    warn("bh.format=%d not ibm floating point format ... continue \n",iformat);
	    if(bh.format<1 || bh.format>3) err("format not supported: bh.format=%d",iformat);
	}
    }

    if (!convert) warn(
      "assuming data is IEEE floating point, no conversion will be done");
    
    /* set nt parameter */
    if (!getparint("nt", &nt)) {
		nt = bh.hns;
		ntg = 0;
	} else {
		ntg = 1;
	}
    
    /* if needed, save ebcbuf into hfile */
    if (getparstring("hfile", &hfile)) {
	/* Open pipe to use dd to convert ebcdic to ascii */
	sprintf(cmdbuf, 
		"dd ibs=3200 of=%s conv=ascii cbs=80 count=1", hfile);
	hfp = epopen(cmdbuf, "w");
	/* Write ascii stream from buffer into pipe */
	efwrite(ebcbuf, EBCBYTES, 1, hfp);
	epclose(hfp);
    }
    
    /* save the binary file, if needed */
    if (getparstring("bfile", &bfile)) {
	/* - the binary data file */
	bfd = eopen(bfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	/* Write binary header from bhed structure to binary file */
	ewrite(bfd, (char *)&bh, BNYBYTES);
	eclose(bfd);
    }
    
    /* convert ebcdic to ascii for output data */	
    tascii_((unsigned char*)ebcbuf, (unsigned char*)&ch, EBCBYTES, 0);
    
    if (strncmp((char*)&ch, "C 1 CLIENT",10) != 0 ) {
		memcpy((char *)&ch, "C 1 CLIENT", 10);
    }
    
    /* test if number of samples set in binary header */
    if (!bh.hns) {
	warn("samples/trace not set in binary header \n");
	if (nt == 0) 
	    warn("samples/trace in 1st trace used \n"); 
	else 
	    warn("nt in input used for samples/trace \n"); 
    }
    
    if ((nt != bh.hns) && (nt != 0)) {
	warn("samples/trace reset in binary header =%d \n",nt);	
	bh.hns = nt;
    }
    
    /* force format=1 --- floating-point in the output */
    bh.format = 1;
    /* output ascii and binary headers to stdout */
    puthdr(&ch, &bh);

    nbt = 0;

    trace2b = (short*)malloc(nt*sizeof(short));

    
    /* convert the traces */
    while (efread((char *)&tr, 1, HDRBYTES, ifp) && (itr < trmax)) {
	
	/* check first 10 bytes to look for ebcdic header,
	   if found, this probably indicates a tape switch */
	/*
	tascii_((unsigned char*)&tr, &hdr_buf, 10, 0); 
	if ((strncmp(hdr_buf, "C 1 CLIENT", 10) == 0) 
	    || (strncmp(hdr_buf, "C CLIENT  ", 10) == 0)
	    || (strncmp(hdr_buf, "C 1 ", 4) == 0)) { 
		fprintf(stderr," %
	    efread(tmp_buf, 1, 3600 - HDRBYTES, ifp);
	} else {
	*/
	    /* read in the trace data */
	    if(tr.ns==0) tr.ns = nt;

	    if(ntg==0) { 
    		nsamp = tr.ns;
 	    } else {
	   	nsamp = nt;
	    }
	    if(iformat==1 || iformat==2) {
		nibyte = nsamp*4;
	    } else {
		nibyte = nsamp*2;
	    }
	    nobyte = nsamp*4;

	    if(iformat==1 || iformat==2) {
	    	efread((char *)&tr + HDRBYTES, 1, nibyte, ifp);
	    } else {
	    	efread(trace2b, 1, nibyte, ifp);
	    }
	    ibt = 0;

	    /* Check bh.hns with tr.ns */
	    if (bh.hns != tr.ns) {
		
		nsflag = true;
		ibt = 1;
		nbt = nbt + 1;
		/* print warning message */
		if(verbose==1 || nbt<1000) warn("discrepant tr.ns = %d with bh.hns = %d\n"
		     "\t... noted on trace %d", tr.ns, bh.hns, itr + 1);
		
		/* if user wants to leave things the way they are (nt=0) */
		/* otherwise, modify number of samples per trace */
		if (nt != 0) {
		    if (nt > tr.ns) {
			for (i = tr.ns; i < nt; i++)
			    tr.data[i] = 0.0;
		    }
		    nsamp = nt;
		    nobyte = nsamp * 4;
		    tr.ns = nt;
		}
	    }
	    
	    /* convert and write desired traces */
	    if (++itr >= trmin) {
		/* Convert IBM floats to native floats */
		if (convert) {
		   if(iformat==3) {
			for (i = 0; i < nt; i++)
			    tr.data[i] = trace2b[i];
		   } else if(iformat!=2) {
                   	conv_float((char *)tr.data, (char *)tr.data, tr.ns, 1);
		   }
		}
		/* write the trace to disk */
		if(nmap==0) {
			/* clean up trace header beyond 180 bytes */
			if (clean == 1) bzero((char *)&tr + 180, 60);
			if (ibt==0 || rmbadtrace==0) 
			efwrite((char *)&tr, 1, nobyte + HDRBYTES, stdout);
		} else {
			bcopy((char*)&tr,(char*)&tro,nobyte+HDRBYTES);
			for(imap=0;imap<nmap;imap++) {
				ibs = ibstart[imap];
				iby = ibyte[imap];
				ity = itype[imap];
				obs = obstart[imap];
				oby = obyte[imap];
				oty = otype[imap];

/*
			fprintf(stderr,"ibs=%d iby=%d ity=%d obs=%d oby=%d oty=%d \n",
							ibs,iby,ity,obs,oby,oty);
*/

				if(iby==oby && ity==oty && ity!=1 ) {
					bcopy((char*)&tr+ibs-1,(char*)&tro+obs-1,iby);
				} else {
					if(ity==1) {
						conv_float((char*)&tr+ibs-1,(char*)&tmp,1,1);
					} else {
						if(iby==2) {
							bcopy((char*)&tr+ibs-1,(char*)&itmp2,iby);
							tmp = itmp2;
						} else if(iby==4) {
							bcopy((char*)&tr+ibs-1,(char*)&itmp4,iby);
							tmp = itmp4;
						}
					}
					if(oty==1) {
						bcopy((char*)&tmp,(char*)&tro+obs-1,oby);
					} else {
						tmp = tmp + 0.5;
						if(oby==2) {
							itmp2 = (short) tmp;
							bcopy((char*)&itmp2,(char*)&tro+obs-1,oby);
						} else {
							itmp4 = (int) tmp;
							bcopy((char*)&itmp4,(char*)&tro+obs-1,oby);
						}
					}
				}
			}
			/* clean up trace header beyond 180 bytes */
			if (clean == 1) bzero((char *)&tro + 180, 60);
			if (ibt==0 || rmbadtrace==0)
			efwrite((char *)&tro, 1, nobyte + HDRBYTES, stdout);
		}
		
		/* echo under verbose option */
		if (verbose && itr % 20 == 0)
		    warn(" %d traces from tape", itr);
	    }
	/*
	} 
	*/
    } /* while loop */
    
    /* re-iterate error in case not seen during run */
    if ((nsflag) && (nt != 0))
	warn("discrepancy found in header and trace ns values\n"
	     "\theader value (%d) was used to extract traces", bh.hns);
    
    /* clean up */
    efclose(ifp);

	if(nmap>0) {
		free(ibstart);
		free(ibyte);
		free(itype);
		free(obstart);
		free(obyte);
		free(otype);
	}
    
    return EXIT_SUCCESS;
}
