/* SUADDHEAD: $Revision: 1.7 $ ; $Date: 90/12/05 10:10:05 $		*/

#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUADDHEAD - put headers on bare traces and set the tracl and ns fields\n"
" 									\n"
" suaddhead <stdin >stdout ns= ftn=0					\n"
" 									\n"
" Required parameter:							\n"
" 	ns=the number of samples per trace				\n"
" 									\n"
" Optional parameter:							\n"
"	head=           file to read headers in				\n"
"                       not supplied --  will generate headers 		\n"
"                       given        --  will read in headers and attach\n"
"                                        floating point arrays to form 	\n"
"                                        traces 			\n" 
"                       (head can be created via sustrip program)	\n"
" 	ftn=0		Fortran flag					\n"
" 			0 = data written unformatted from C		\n"
" 			1 = data written unformatted from Fortran	\n"
"       tsort=3         trace sorting code:				\n"
"                                1 = as recorded (no sorting)		\n"
"                                2 = CDP ensemble			\n"
"                                3 = single fold continuous profile	\n"
"                                4 = horizontally stacked		\n" 
"       ntrpr=1         number of data traces per record		\n"
"                       if tsort=2, this is the number of traces per cdp\n" 
" 									\n"
" Trace header fields set: ns, tracl					\n"
" Use sushw/suchw to set other needed fields.				\n"
" 									\n"
" Caution: An incorrect ns field will munge subsequent processing.	\n"
" Note:    n1 and nt are acceptable aliases for ns.			\n"
" 									\n"
" Example:								\n"
" suaddhead ns=1024 <bare_traces | sushw key=dt a=4000 >segy_traces	\n"
" 									\n"
" This command line adds headers with ns=1024 samples.  The second part	\n"
" of the pipe sets the trace header field dt to 4 ms.			\n"
" 									\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 *            : Zhiming Li	add ascii and binary headers
 */


segychdr ch;
segybhdr bh;
segytrace tr;

main(int argc, char **argv)
{
	int ns;			/* number of samples			*/
	int ftn;		/* ftn=1 for Fortran			*/
	char junk[ISIZE];	/* to discard ftn junk  		*/
	string head;    /* name of file holding headers         */
	FILE *headfp;
	int ihead;
	FILE *infp=stdin, *outfp=stdout;
	int tsort, ntrpr;
	int iread;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	if (!getparint("n1", &ns)
	 && !getparint("nt", &ns)
	 && !getparint("ns", &ns))  err("must specify ns=");
	if (!getparint("ftn", &ftn))	ftn = 0;
	if (ftn != 0 && ftn != 1)	err("ftn=%d must be 0 or 1", ftn);
	if (!getparstring("head"  , &head)) {
		ihead = 0;
	} else {
		ihead = 1;
		headfp = efopen(head, "r");
	}
	if (!getparint("ntrpr", &ntrpr)) ntrpr = 1;
	if (!getparint("tsort", &tsort)) tsort = 3;

	file2g(infp);
    file2g(outfp);


	/* create id headers */
	if(ihead==0) {
		idhdrs(&ch,&bh,ns);
		bh.format = 1;
		bh.ntrpr = ntrpr;
		bh.tsort = tsort;
		bh.fold = ntrpr;
	} else {
		fgethdr(headfp,&ch,&bh);
	}

	bh.hns = ns;

	puthdr(&ch,&bh);
		
	while (TRUE) {
		static int tracl = 0;	/* one-based trace number */

		/* If Fortran data, read past the record size bytes */
		if (ftn) efread(junk, ISIZE, 1, stdin);

		/* Do read of data for the segy */
		iread = fread((char *) tr.data, FSIZE, ns, stdin);
		if(iread!=ns) {
			return EXIT_SUCCESS;
		} else {
			if(ihead==0) {
				tr.tracl = ++tracl;
			} else {
				efread(&tr, 1, HDRBYTES, headfp);
			}
			tr.ns = ns;
			tr.trid = 1;
			puttr(&tr);
		}

		/* If Fortran data, read past the record size bytes */
		if (ftn) efread(junk, ISIZE, 1, stdin);
	}

}

