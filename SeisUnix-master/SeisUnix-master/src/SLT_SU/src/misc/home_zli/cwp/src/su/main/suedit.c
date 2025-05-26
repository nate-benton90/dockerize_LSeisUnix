static char RCSsuedit[] = "$Id: suedit.c,v 1.1 1992/12/16 17:00:19 suadm Exp stssmap $";
 
/*
 *
 * $Source: /vol/SU/cwp/src/su/main/RCS/suedit.c,v $
 *
 * $Log: suedit.c,v $
 * Revision 1.1  1992/12/16  17:00:19  suadm
 * Initial revision
 *
 */
 
/* SUEDIT: $Revision: 1.1 $; $Date: 1992/12/16 17:00:19 $	*/

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
#include "header.h"

/*********************** self documentation ******************************/
string sdoc =
" 									\n"
" SUEDIT - examine segy diskfiles and edit headers			\n"
" 									\n"
" suedit diskfile  (open for possible header modification if writable)	\n"
" suedit <diskfile  (open read only)					\n"
" 							        	\n"
" The following commands are recognized:				\n"
" number	read in that trace and print nonzero header words	\n"
" <CR>		go to trace one step away (step is initially -1)	\n"
" +		read in next trace (step is set to +1)			\n"
" -		read in previous trace (step is set to -1)		\n"
" dN		advance N traces (step is set to N)			\n"
" %		print some percentiles of the trace data		\n"
" r		print some ranks (rank[j] = jth smallest datum) 	\n"
" p [n1 [n2]]  	tab plot sample n1 to n2 on current trace		\n"
" g [tr1 tr2]  	wiggle plot (graph) the trace [traces tr1 to tr2]	\n"
" f		wiggle plot the Fourier transform of the trace		\n"
" ! key=val  	change a value in a field (e.g. ! tracr=101)		\n"
" ?		print help file						\n"
" q		quit							\n"
" 									\n"
" NOTE: sample numbers are 1-based (first sample is 1).			\n"
" 									\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar, Shuki, Stew
 *	CWP: Jack
 *
 */


segy tr;		/* a segy trace structure		*/
FILE *tty;		/* /dev/tty is used to read user input	*/
char userin[BUFSIZ];	/* buffer user requests			*/
int nt;			/* number of sample points on traces	*/
FILE *infp;		/* file pointer for trace file		*/
char tmpwig[L_tmpnam];	/* file for trace plots			*/

string help =
"					\n"
" n		read in trace #n	\n"
" <CR>		step			\n"
" +		next trace;   step -> +1\n"
" -		prev trace;   step -> -1\n"
" dN		adv N traces; step -> N	\n"
" %		percentiles		\n"
" r		ranks			\n"
" p [n1 [n2]]  	tabplot			\n"
" g [tr1 tr2]  	wiggle plot		\n"
" f		wig plot Fourier Transf \n"
" ! key=val  	modify field		\n"
" ?		print this file		\n"
" q		quit			\n"
"					\n"
;

#define SCREENFUL	19	/* default number of points in tab plot */

/* subroutine prototypes */
void editkey(void);
void wigplot(void);
void ftwigplot(void);
int cmp_indirect();
void userwait(void);

main(int argc, char **argv)
{
	int step = -1;		/* step +1/-1 for traversing data	*/
	int itr;		/* trace number (zero based)		*/
	int ntr;		/* number of traces in data set		*/
	int *rank;		/* permuted indices for indirect sort	*/
	int i;			/* counter				*/
	int iq;			/* index of qth quantile (100qth %tile)	*/
	bool write_ok;		/* is file writable?			*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	if (argc > 2)  err("only one filename argument is allowed");

	tty = efopen("/dev/tty", "r");

	/* Open file and print editing warnings if appropriate */
	if (!isatty(STDIN)) {	/* stdin was redirected to file */
		infp = stdin;
		write_ok = false;
		warn("! examine only (no header editing from STDIN)\n");

	} else {  	/* file is given by argument */

		/* First try for read and write */
		if (0 == (access(argv[1], READ_OK | WRITE_OK))) {
			infp = efopen(argv[1], "r+");
			write_ok = true;

		/* Then for just read */
		} else if (0 == (access(argv[1], READ_OK))) {
			infp = efopen(argv[1], "r");
			write_ok = false;
			warn("! %s is readonly (no header editing)\n",
								argv[1]);
		/* Admit defeat */
		} else {
			err("can't open %s for reading", argv[1]);
		}
	}

	/* Get information from first trace */
	ntr = fgettra(infp, &tr, 0);
	nt = tr.ns;

	/* Set up array for indirect sort requested by 'r' and '%' keys */
	rank = ealloc1int(nt);
	for (i = 0; i < nt; ++i)  rank[i] = i;

	printf("%d traces in input file\n", ntr);

	/* Start from last trace */
	itr = ntr - 1;
	fgettra(infp, &tr, itr);
	printheader(&tr);
	printf("> ");
	efflush(stdout);

	/* Get user directives and do requested tasks */
	while (NULL != fgets(userin, BUFSIZ, tty)) {
		switch(*userin) {
		case '0': /* check if digit */
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8':
		case '9': /* end check if digit */
			itr = eatoi(userin) - 1;
			fgettra(infp, &tr, itr);
			printheader(&tr);
		break;
		case 'q':
			return EXIT_SUCCESS;
		case 'p':
			{ static int p1, p2;
			  /* Get user inputs (avoid running off end of data) */
			  switch(sscanf(userin + 1, "%d %d", &p1, &p2)) {
			  case 2:	/* user specified final position */
				if (p2 < p1) {
					warn("need p1=%d < p2=%d", p1, p2);
					return;
				}
				p2 = MIN(p2, nt);
			  break;
			  default:
				p2 = MIN(p1 + SCREENFUL, nt);
			  break;
			  }
			  if (p1 >= nt || p1 <= 0) p1 = 1;
			  tabplot(&tr, p1-1, p2-1); /* 1-base -> 0-base */
			  p1 = p2;
			}
		break;
		case 'g':
			wigplot();
		break;
		case 'f':
			ftwigplot();
		break;
		case '+':
			++itr;
			if (itr > ntr - 1) {
				step = -1;
				itr = ntr - 2;
				printf("\nBounced off end of data:\n\n");
			}
			fgettra(infp, &tr, itr);
			printheader(&tr);
			step = 1;
		break;
		case '-':
			itr--;
			if (itr < 0) {
				step = 1;
				itr = 2;
				printf("\nBounced off end of data:\n\n");
			}
			fgettra(infp, &tr, itr);
			printheader(&tr);
			step = -1;
		break;
		case '\n':
			itr += step;
			if (itr < 0 || itr > ntr - 1) {
				step *= -1;
				itr += 2*step;
				printf("\nBounced off end of data:\n\n");
			}
			fgettra(infp, &tr, itr);
			printheader(&tr);
		break;
		case 'r':
			/* sort: rank[] holds rank of datum in tr */
			qsort(rank, nt, FSIZE, cmp_indirect);

			/* Could make table of desired i's and loop */
			i = 0;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			i = nt / 20;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			i = nt/2 - i;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			printf("\n");
			i = nt - 1 - i;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			i = nt - 1 - nt/20;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			i = nt - 1;
			printf(" rank[%d] = %8.2e", i+1, tr.data[rank[i]]);
			printf("\nmin is at sample %d,  max at %d\n",
					rank[0] + 1, rank[nt-1] + 1);
		break;
		case '%':
			/* sort: rank[] holds rank of datum in tr */
			qsort(rank, nt, FSIZE, cmp_indirect);

			/* round to qth quantile (100 qth percentile) */
			/* thus (q*nt - 1) + .5 (-1 for zero basing) */
			i = 1; iq = (int) (0.01*nt - 0.5);
			printf(" %dst percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 5; iq = (int) (0.05*nt - 0.5);
			printf(" %dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 25; iq = (int) (0.25*nt - 0.5);
			printf("%dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 50; iq = (int) (0.50*nt - 0.5);
			printf("%dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 75; iq = (int) (0.75*nt - 0.5);
			printf("%dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 95; iq = (int) (0.95*nt - 0.5);
			printf("%dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			i = 99; iq = (int) (0.99*nt - 0.5);
			printf("%dth percentile is %8.2e\n",
					i+1, tr.data[rank[iq]]);
			printf("min at sample %d equals %8.2e\n",
					rank[0] + 1, tr.data[rank[0]]);
			printf("max at sample %d equals %8.2e\n",
					rank[nt-1] + 1, tr.data[rank[nt-1]]);
		break;
		case 'd':
			step = eatoi(userin + 1);
			itr += step;
			if (itr < 0 || itr > ntr - 1) {
				step *= -1;
				itr += 2*step;
				printf("\nBounced off end of data:\n\n");
			}
			fgettra(infp, &tr, itr);
			printheader(&tr);
		break;
		case '!':
			if (write_ok) {
				editkey();
			} else {
				warn("file not writable");
			}
		break;
		case '?':
			printf("%s\n", help);
		break;
		default:
			warn("bad key %s\n%s", userin, help);
		break;
		}
		printf("> ");
		efflush(stdout);
	}

	return EXIT_SUCCESS;
}



/* Modify a header field value */
void editkey(void)
{
	string keyword;	/* buffer and then header key word	*/
	string keyval;	/* header key value in ascii		*/
	string ptr;	/* pointer to isolate key word		*/
	string type;	/* type of key word			*/
	int nsegy;	/* length of trace in bytes		*/
	value val;	/* numerical value of key word		*/

	/* char userin[] is "!    keyword  = keyval" */

	/* Strip the '!' and any leading spaces from buffer */
	for (keyword = userin + 1; isspace(*keyword); keyword++);

	/* Set keyval to start of val */
 	if (NULL == (keyval = strchr(keyword, '=') + 1)) {
		printf("command error: format is \"! key=val\"\n");
		return;
	}

	/* Null terminate keyword (back up over = and any spaces) */
	for (ptr = keyval - 2; isspace(*ptr); ptr--);
	(*(ptr + 1)) = '\0';

	/* Convert ascii keyval string to numeric val value */
	type = hdtype(keyword);
	errno = 0;
	atoval(type, keyval, &val);
	if (errno) {
	    syswarn("failed to convert %s to numeric, field not changed",
								keyval);
	}

	/* Insert value into header */
	puthdval(&tr, keyword, &val);

	/* Write back the trace with the new value */
	nsegy = nt * FSIZE + HDRBYTES;
	efseek(infp, -nsegy, SEEK_CUR);
	efwrite(&tr, 1, HDRBYTES, infp);

	/* Restore file pointer */
	efseek(infp, nsegy, SEEK_CUR);

	/* Show the user the new header value */
	printheader(&tr);

	return;
}


/* Wiggle plot of selected adjacent traces */
void wigplot(void)
{
	int n1;			/* first trace to be plotted		*/
	int n2;			/* last trace to be plotted		*/
	int i;			/* counter				*/
	char cmd[BUFSIZ];	/* build command for system call	*/
	FILE *wigfp;		/* fp for suwig input file		*/


	/* Prepare temporary file to hold traces to be plotted */
	wigfp = efopen(etmpnam(tmpwig), "w+");


	/* Parse request and subtract 1 for internal trace numbers */
	switch(sscanf(userin + 1, "%d %d", &n1, &n2)) {
	case 1: /* user specified remote trace to plot */
		--n1;
		fgettra(infp, &tr, n1);
		fputtr(wigfp, &tr); 
	break;
	case 2: /* user specified block of traces to plot */
		if (n2 < n1) {
			warn("must specify n1=%d < n2=%d", n1, n2);
			return;
		}
		for (i = n1 - 1; i <= n2 - 1; ++i) {
			fgettra(infp, &tr, i);
			fputtr(wigfp, &tr); 
		}
	break;
	default: /* no numbers given by user: plot current trace */
		fputtr(wigfp, &tr); 
	break;
	}

	/* Set up system call to suwig */
	rewind(wigfp);
	sprintf(cmd, "suwig <%s zerox=1.2 fill=0 | tube", tmpwig);
	system(cmd);

	/* Clean up temp file */
	efclose(wigfp);
	eremove(tmpwig);

	/* Prepare for next user request */
	userwait();	/* prompt and pause till user presses return  */
	sprintf(cmd, "%s/clg", CWPBIN);
	system(cmd);	/* clear screen */
	printheader(&tr);

	return;
}


/* Wiggle plot Fourier transform of current trace */
void ftwigplot()
{
	char cmd[BUFSIZ];	/* build command for system call	*/
	FILE *fftfp;		/* fp for sufft input file		*/


	/* Prepare temporary file to hold fft trace to be plotted */
	fftfp = efopen(tmpnam(tmpwig), "w+");

	fputtr(fftfp, &tr);

	/* Set up system call for suspec1 */
	rewind(fftfp);
	sprintf(cmd, "suspec1 <%s | suwig zerox=1.2 fill=0 | tube", tmpwig);
	system(cmd);

	/* Clean up temp file */
	efclose(fftfp);
	eremove(tmpwig);

	/* Prepare for next user request */
	userwait();	/* prompt and pause till user presses return  */
	sprintf(cmd, "%s/clg", CWPBIN);
	system(cmd);	/* clear screen */
	printheader(&tr);

	return;
}


/* Comparison function for qsort */
int cmp_indirect(int *r, int *s)
{
	float diff = tr.data[*r] - tr.data[*s];

	if      (diff > 0)	return(1);
	else if (diff < 0)	return(-1);
	else  /* diff == 0 */	return(0);
}


/* userwait - prompt and wait for user signal to continue */
void userwait(void)
{
	/* Note: leading newline helps some devices switch to ascii */
	fprintf(stderr, "\npress return key to continue\n");
	getc(tty);

	return;
}
