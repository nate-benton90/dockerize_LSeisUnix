#include <wgc.h>
#include <su.h>
#include <segy.h>

/*********************** self documentation **********************/
string sdoc =
"                                                               \n"
" SUCOPY - copy a dataset, can read and write to disk or tape 	\n"
"                                                               \n"
" sucopy <stdin >stdout [optional parameters]			\n"
"                                                               \n"
" Optional parameter:                                           \n"
"	verbose=0	silent operation;			\n"
"			= 1; echo every 20 traces		\n"
"	clean=1		clean trace header beyond standard 180	\n"
"			bytes;					\n"
"			= 0; do nothing				\n"
"	trmin=1		first trace to read			\n"
"	trmax=LONG_MAX	last trace to read			\n"
"	nt=bh.hns	defaults to the number of samples per	\n"
"			trace from binary header; if a value is	\n"
"			given, that value will be used to read 	\n"
"			in the number of samples per trace; if	\n"
"			nt>bh.hns, the data will be padded with	\n"
"			zeroes, if nt<bh.hns, data at the end	\n"
"			will be truncated			\n"
"			(if nt is specified or default is used, \n"
"			 it will overwrite the value in both the\n"
"			 binary header and in the trace header	\n"
"			= 0; number of samples will depend on 	\n"
"			trace header; will not modify headers	\n"
"                                                               \n"
;
/**************** end self doc ***********************************/

/* 
 * Credits:
 *            :  Herb Lam
 */

segy     tr;
segychdr ch;
segybhdr bh;

/* for errors */
extern int   errno;
extern char *sys_errlist[];

int copy_trace(FILE *tfp_in, FILE *tfp_out, int hns, int trmin, int trmax, 
	       int nt, int verbose, int clean);

main(int argc, char **argv)
{
    FILE   *tfp_in;  /* input file pointer           */
    FILE   *tfp_out; /* output file pointer          */
    int     trmin;   /* first trace to read          */
    int     trmax;   /* last trace to read           */
    int     nt;      /* number of data samples       */
    int     verbose; /* flag for echoing traces read */
    int     clean;   /* flag for trace header        */

    /* initialize */
    initargs(argc, argv);
    askdoc(1);
    
    /* get some of the parameters */
    if (!getparint("trmin", &trmin))        trmin = 1;
    if (!getparint("trmax", &trmax))        trmax = LONG_MAX;
    if (!getparint("verbose", &verbose))    verbose = 0;
    if (!getparint("clean", &clean))        clean = 1;
    
    tfp_in = stdin;
    tfp_out = stdout;

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

    /* read ebcdic (ascii) header from input and write to output */
    efread((void *)&ch, 1, EBCBYTES, tfp_in);
    efwrite((void *)&ch, 1, EBCBYTES, tfp_out);
    
    /* read binary header from input and write to output */
    efread((void *)&bh, 1, BNYBYTES, tfp_in);
    efwrite((void *)&bh, 1, BNYBYTES, tfp_out);
    
    /* get number of samples parameter */
    if (!getparint("nt", &nt))
	nt = bh.hns;
    
    /* check number of samples specified in binary header */
    if (!bh.hns) {
	warn("samples/trace not set in binary header \n");
	if (nt == 0)
	    warn("samles/trace in 1st trace used");
	else
	    warn("nt in input used for samples/trace");
    }
    
    /* warn user if reseting number of samples */
    if ((nt != bh.hns) && (nt != 0)) {
	warn("samples/trace reset in trace header =%d \n", nt);
	bh.hns = nt;
    }
    
    /* call copy_trace routine to copy data */
    if (copy_trace(tfp_in, tfp_out, bh.hns, trmin, trmax, 
		   nt, verbose, clean) != 0)
	err("error in reading or writing dataset");
    
    /* close file pointers */
    efclose(tfp_in);
    efclose(tfp_out);
 
    return EXIT_SUCCESS;
}

int copy_trace(FILE *tfp_in, FILE *tfp_out, int hns, int trmin, int trmax, 
	       int nt, int verbose, int clean)
{
    unsigned int nsegy;  /* size of whole trace in bytes */
    int itr;             /* current trace number */
    int i;               /* counter for loop */
    bool nsflag = false; /* flag for error in tr.ns */
    char hdr_buf[10];    /* 1st 10 bytes of header in ascii */
    char tmp_buf[3600];  /* temp. buffer to read in header */
    
    itr = 0;
    
    while (efread((char *)&tr, 1, HDRBYTES, tfp_in) && itr < trmax) {
	
	/* check first 10 bytes to look for ebcdic header,
	   if found, this probably indicates a tape switch */
	tascii_((unsigned char*)&tr, &hdr_buf, 10, 0);
	
	/* don't know if it is standard segy (ebcdic), or
	   su segy (ascii), need to check both */
	if ((strncmp(hdr_buf, "C 1 CLIENT", 10) == 0)
	    || (strncmp(hdr_buf, "C CLIENT  ", 10) == 0))
	    efread(tmp_buf, 1, 3600 - HDRBYTES, tfp_in);
	
	else if ((strncmp((char *)&tr, "C 1 CLIENT", 10) == 0)
		 || (strncmp((char *)&tr, "C CLIENT  ", 10) == 10))
	    efread(tmp_buf, 1, 3600 - HDRBYTES, tfp_in);
	
	else {
	    /* read in the trace data */
	    nsegy = tr.ns * 4;
	    efread((char *)&tr + 240, 1, nsegy, tfp_in);
	    
	    /* check hns (bh.hns field) with tr.ns */
	    if (hns != tr.ns) {
		
		nsflag = true;
		/* print warning message */
		warn("discrepant tr.ns = %d with bh.hns = %d\n"
		     "\t... noted on trace %d", tr.ns, hns, itr + 1);
		
		/* if user wants to leave things the way they are (nt=0);
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
		
		/* clean up trace header beyond 180 bytes */
		if (clean == 1)
		    bzero((char *)&tr + 180, 60);
		
		/* Write the trace to disk */
		efwrite((char *)&tr, 1, HDRBYTES + nsegy, tfp_out);
		
		/* Echo under verbose option */
		if (verbose && itr % 20 == 0)
		    warn(" %d traces from tape", itr);
	    }
	}
    } /* while */
    
    /* re-iterate error in case not seen during run */
    if ((nsflag) && (nt != 0))
	warn("discrepancy found in header and trace ns values\n"
	     "\theader value (%d) was used to extract traces", bh.hns);
    
    return EXIT_SUCCESS;
}

