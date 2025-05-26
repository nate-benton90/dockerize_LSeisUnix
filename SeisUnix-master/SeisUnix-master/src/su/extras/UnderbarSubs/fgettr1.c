/* FGETTR: $Revision: 1.55 $; $Date: 89/05/25 16:10:09 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include "segy.h"
#include "header.h"

/* fgettr, fgettr1, fgettr2 - get a segy trace from a file by descriptor
 * gettr, gettr1, gettr2 - get a segy trace from stdin
 *
 * Returns:
 *	int: number of bytes read on current trace (0 after last trace)
 *
 * Synopsis:
 *	int fgettr(fd, tp)	for standard float traces
 *	segy *tp;
 *
 *	int fgettr1(fd, tp)	for 1 byte traces as in suunpack1
 *	segy *tp;
 *
 *	int fgettr2(fd, tp)	for 2 byte traces as in suunpack2
 *	segy *tp;
 *
 *	gettr, gettr1 and gettr2 are implemented as macros in cwp.h:
 *
 *	gettr(tp)  is equivalent to fgettr(stdin, tp)
 *	gettr1(tp) is equivalent to fgettr1(stdin, tp)
 *	gettr2(tp) is equivalent to fgettr2(stdin, tp)
 *
 * Example:
 *	segy tr;
 *	...
 *	while (gettr(&tr)) {
 *		tr.offset = abs(tr.offset);
 *		puttr(&tr);
 *	}
 *	...
 *
 * Notes:
 *	To avoid having to maintain nearly identical codes, we make
 *	three versions from this source code.  The "1" variant is for
 *      segy traces with char data, the "2" variant is for short data.
 *	Use the compile options -DGETTR, -DGETTR1, and -DGETTR2
 *	respectively, for the three versions.
 *
 * The original SEP version contained code for asynchronous use of
 * TAPE on the Convex.  The tape handling in this code is untested.
 *
 * Credits:
 *	SEP: Einar, Stew
 *	CWP: Jack, Shuki
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/fgettr.c,v $";
static string revid =
	"   $Revision: 1.55 $ ; $Date: 89/05/25 16:10:09 $";



#ifdef GETTR1
#define NAME		"fgettr1"
#define BYTESPER	sizeof(char)	/* bytes per input datum */
int fgettr1(fd, tp)
#endif

#ifdef GETTR2
#define NAME		"fgettr2"
#define BYTESPER	sizeof(short)	/* bytes per input datum */
int fgettr2(fd, tp)
#endif

#ifdef GETTR
#define NAME		"fgettr"
#define BYTESPER	sizeof(float)	/* bytes per input datum */
int fgettr(fd, tp)
#endif

int fd;			/* file descriptor	*/
segy *tp;		/* pointer to segy trace */

{
	static ulong itr = 0;	/* number of traces read		*/
	static bool first = true;	/* to check if first entry	*/
	static filetype ftype;		/* filetype of fd		*/
	static int nsfirst;	/* number of samples from 1st header	*/
	static uint ndata;	/* number of data bytes from nsfirst	*/
	static uint nsegy; 	/* number of segy bytes from nsfirst	*/
	int nread;		/* number of bytes read			*/



	if (first) {
		first = false;

		if (ID) {
			(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
			(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
		}

		switch (ftype = statfil(fd)) {
		case TAPE:
			switch (nsegy =
				read(fd, (char *) tp, MAXSEGY)) {
			case -1:
				syserr("%s: first tape read", NAME);
			case 0:	/* no traces */
				return(0);
			default:
				nsfirst = tp->ns;
				if (nsfirst > SU_NFLTS) {
					err("%s: %s %d > %d %s", NAME,
					    "unable to handle", nsfirst,
					    SU_NFLTS, "samples per trace");
				}
				if (nsegy !=
					HDRBYTES + nsfirst * BYTESPER) {
					err("%s: %s", NAME,
					    "bad first header on tape");
				}
			break;
			}
		break;
		case PIPE:
			/* Get the header */
			switch (nread = pread(fd,(char *) tp,(uint) HDRBYTES)) {
			case -1:
				syserr("%s: first pipe read", NAME);
			case 0:	/* no traces */
				return(0);
			default:
				if (nread != HDRBYTES) {
					err("%s: %s", NAME,
					    "bad first header on pipe");
				}
			}

			/* Have the header, now for the data */
			nsfirst = tp->ns;
			if (nsfirst > SU_NFLTS) {
			    err("%s: %s %d>%d %s", NAME, "unable to handle",
			        nsfirst, SU_NFLTS, "samples per trace");
			}
			nsegy = HDRBYTES + BYTESPER * nsfirst;
			ndata = BYTESPER * nsfirst;

			switch (nread =
				 pread(fd, (char *) tp->data, ndata)) {
			case -1:
				syserr("%s: first trace pipe read", NAME);
			case 0:	/* no data on trace */
				return(HDRBYTES);
			default:
				if (nread != ndata) {
				    err("%s: %s %d bytes of %u", 
				        NAME, "first pipetrace: read only",
					nread, ndata);
				}
			break;
			}
		break;
		case DISK:
			/* Get the header */
			switch (nread = read(fd,(char*)tp,HDRBYTES)) {
			case -1:
				syserr("%s: first disk read",NAME);
			case 0:	/* no traces */
				return(0);
			default:
				if (nread != HDRBYTES) {
					err("%s: %s", NAME,
					    "bad first header on disk");
				}
			break;
			}

			/* Have the header, now for the data */
			nsfirst = tp->ns;
			if (nsfirst > SU_NFLTS) {
				err("%s: %s %d > %d %s", NAME,
				    "unable to handle", nsfirst, SU_NFLTS,
				    "samples per trace");
			}
			ndata = BYTESPER * nsfirst;
			nsegy = HDRBYTES + ndata;

			switch (nread =
			        read(fd, (char *) tp->data, ndata)) {
			case -1:
				syserr("%s: first trace disk read",NAME);
			case 0:	/* no data on trace */
				return(HDRBYTES);
			default:
				if (nread != ndata) {
				    err("%s: %s %d bytes of %u", 
				        NAME, "first disktrace: read only",
					nread, ndata);
				}
			break;
			}
		break;
		case TTY:
			err("%s: stdin can't be tty", NAME);
		default:
			err("%s: stdin is undefined filetype: %s",
						NAME, statprint(ftype));
		}

	} else { /* Not first entry */

		switch (ftype) {
		case TAPE:
			switch (nread = read(fd, (char *)tp, nsegy)) {
			case -1:
				syserr("%s: read tapetrace #%ld",NAME,itr);
			case 0:	/* finished */
				return(0);
			default:
				if (nread != nsegy) {
					err("%s: %s #%ld", NAME,
					    "tape read trace", itr);
				}
			}
		break;
		case PIPE:
			switch (nread = pread(fd, (char *) tp, nsegy)) {
			case -1:
				syserr("%s: %s #%ld", NAME,
				       "pipe read trace", itr);
			case 0:	/* finished */
				return(0);
			default:
				if (nread != nsegy) {
					err("%s: %s #%ld", NAME,
					    "pipe read trace", itr);
				}
			break;
			}
		break;
		case DISK:
			switch (nread = read(fd, (char *)tp, nsegy)) {
			case -1:
				syserr("%s: %s #%ld", NAME,
				       "disk read trace", itr);
			case 0:	/* finished */
				return(0);
			default:
				if (nread != nsegy) {
					err("%s: %s #%ld", NAME,
					    "disk read trace", itr);
				}
			break;
			}
		break;
		default:
		     err("%s: %s: mysterious filetype trace #%ld",
							NAME, __LINE__, itr);
		}

		if (tp->ns != nsfirst) {
			err("%s: on trace #%ld, %s (%d) %s (%d)", 
			     NAME, itr,
			     "number of samples in header",
			     tp->ns,
			     "differs from number for first trace",
			     nsfirst);
		}
	}

	itr++;
	return (int) nsegy;
}

/* For FORTRAN */
#ifdef GETTR1
#define NAME		"fgettr1"
#define BYTESPER	sizeof(char)	/* bytes per input datum */
int fgettr1_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	return(fgettr1(*fd, tp));
}
#endif

#ifdef GETTR2
#define NAME		"fgettr2"
#define BYTESPER	sizeof(short)	/* bytes per input datum */
int fgettr2_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	return(fgettr2(*fd, tp));
}
#endif

#ifdef GETTR
#define NAME		"fgettr"
#define BYTESPER	sizeof(float)	/* bytes per input datum */
int fgettr_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	return(fgettr(*fd, tp));
}
#endif


#ifdef TEST

/*********************** self documentation **********************/
string sdoc = "\
								\n\
tgettr <stdin >stdout						\n\
								\n\
	Test harness for gettr.c				\n\
	Changes cdp to abs(cdp)					\n\
	Contrast the following results:	 			\n\
	suplane offset=-100 | sugethw offset 			\n\
	suplane offset=-100 | tgettr | sugethw offset		\n\
								\n\
";
/*****************************************************************/

segy tr;

main(argc, argv)
int argc; char **argv;
{
	initargs(argc, argv);
	askdoc(1);

 	while (gettr(&tr)) {
 		tr.offset = abs(tr.offset);
 		puttr(&tr);
 	}

	return SUCCEED;
}
#endif
