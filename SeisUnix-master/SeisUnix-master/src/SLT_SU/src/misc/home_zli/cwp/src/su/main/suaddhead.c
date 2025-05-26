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

#ifdef __convex__
        #define fseek2g(x,y,z)  fseek64(x,y,z);
#else
        #define fseek2g(x,y,z)  fseek(x,y,z);
#endif


main(int argc, char **argv)
{
	int ns;			/* number of samples			*/
	int ftn;		/* ftn=1 for Fortran			*/
	char junk[ISIZE];	/* to discard ftn junk  		*/
	char cbuf[40][80];
	void create_segyched(char cbuf[40][80]);
	int i;
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

	fseek2g(infp,0,1);
        fseek2g(outfp,0,1);



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

void create_segyched(char cbuf[40][80]) {

sprintf(cbuf[0], "%-79s\n",
"C 1 CLIENT                        COMPANY                       CREW NO     ");
sprintf(cbuf[1], "%-79s\n",
"C 2 LINE            AREA                       MAP ID                       ");
sprintf(cbuf[2], "%-79s\n",
"C 3 REEL NO           DAY-START OF REEL     YEAR      OBSERVER              ");
sprintf(cbuf[3], "%-79s\n",
"C 4 INSTRUMENT  MFG            MODEL            SERIAL NO                   ");
sprintf(cbuf[4], "%-79s\n",
"C 5 DATA TRACES/RECORD        AUXILIARY TRACES/RECORD         CDP FOLD      ");
sprintf(cbuf[5], "%-79s\n",
"C 6 SAMPLE INTERVAL         SAMPLES/TRACE       BITS/IN      BYTES/SAMPLE   ");
sprintf(cbuf[6], "%-79s\n",
"C 7 RECORDING FORMAT        FORMAT THIS REEL        MEASUREMENT SYSTEM      ");
sprintf(cbuf[7], "%-79s\n",
"C 8 SAMPLE CODE                                                             ");
sprintf(cbuf[8], "%-79s\n",
"C 9 GAIN  TYPE                                                              ");
sprintf(cbuf[9], "%-79s\n",
"C10 FILTERS                                                                 ");
sprintf(cbuf[10], "%-79s\n",
"C11 SOURCE  TYPE            NUMBER/POINT        POINT INTERVAL              ");
sprintf(cbuf[11], "%-79s\n",
"C12     PATTERN                            LENGTH        WIDTH              ");
sprintf(cbuf[12], "%-79s\n",
"C13 SWEEP  START     HZ  END     HZ  LENGTH      MS  CHANNEL NO     TYPE    ");
sprintf(cbuf[13], "%-79s\n",
"C14 TAPER  START LENGTH       MS  END LENGTH       MS  TYPE                 ");
sprintf(cbuf[14], "%-79s\n",
"C15 SPREAD  OFFSET        MAX DISTANCE        GROUP INTERVAL                ");
sprintf(cbuf[15], "%-79s\n",
"C16 GEOPHONES  PER GROUP     SPACING     FREQUENCY     MFG          MODEL   ");
sprintf(cbuf[16], "%-79s\n",
"C17      TYPE                              LENGTH        WIDTH              ");
sprintf(cbuf[17], "%-79s\n",
"C18 TRACES SORTED BY               PROJECT                LINE ID           ");
sprintf(cbuf[18], "%-79s\n",
"C19 AMPLITUDE RECOVERY                                                      ");
sprintf(cbuf[19], "%-79s\n",
"C20 MAP PROJECTION                      ZONE ID       COORDINATE UNITS      ");
sprintf(cbuf[20], "%-79s\n",
"C21 FIELD SUM       NAVIGATION SYSTEM               RECORDING PARTY         ");
sprintf(cbuf[21], "%-79s\n",
"C22 CABLE TYPE                   DEPTH        SHOOTING DIRECTION            ");
sprintf(cbuf[22], "%-79s\n",
"C23                                                                         ");
sprintf(cbuf[23], "%-79s\n",
"C24                                                                         ");
sprintf(cbuf[24], "%-79s\n",
"C25                                                                         ");
sprintf(cbuf[25], "%-79s\n",
"C26                                                                         ");
sprintf(cbuf[26], "%-79s\n",
"C27                                                                         ");
sprintf(cbuf[27], "%-79s\n",
"C28                                                                         ");
sprintf(cbuf[28], "%-79s\n",
"C29                                                                         ");
sprintf(cbuf[29], "%-79s\n",
"C30                                                                         ");
sprintf(cbuf[30], "%-79s\n",
"C31                                                                         ");
sprintf(cbuf[31], "%-79s\n",
"C32                                                                         ");
sprintf(cbuf[32], "%-79s\n",
"C33                                                                         ");
sprintf(cbuf[33], "%-79s\n",
"C34                                                                         ");
sprintf(cbuf[34], "%-79s\n",
"C35                                                                         ");
sprintf(cbuf[35], "%-79s\n",
"C36                                                                         ");
sprintf(cbuf[36], "%-79s\n",
"C37                                                                         ");
sprintf(cbuf[37], "%-79s\n",
"C38                                                                         ");
sprintf(cbuf[38], "%-79s\n",
"C39                                                                         ");
sprintf(cbuf[39], "%-79s\n",
"C40 END EBCDIC                                                              ");
}

