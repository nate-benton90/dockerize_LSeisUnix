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
 *----------------------------------------------------------------------
 * Permission to use the SU programs is granted.  Courteous users will
 * retain this acknowlegment of origins at the Stanford Exploration
 * Project, Stanford University and the Center for Wave Phenomena, 
 * Colorado School of Mines.   Inquiries/bug reports should be sent to:
 * Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 * Golden, CO 80014
 *----------------------------------------------------------------------
 *
 * $Author: jkc $
 * $Source: /src/su/lib/RCS/fgettr.c,v $
 * $Revision: 1.47 $; $Date: 88/11/19 22:05:02 $
 */

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/lib/RCS/fgettr.c,v $";
static char revid[] =
	"   $Revision: 1.47 $ ; $Date: 88/11/19 22:05:02 $";


#include "../include/cwp.h"
#include "../include/segy.h"
#include "../include/hdr.h"	/* To get HDRBYTES */

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
	static int ndata;	/* number of data bytes from nsfirst	*/
	static int nsegy; 	/* number of segy bytes from nsfirst	*/
	int nread;		/* number of bytes read			*/

	if (first) {
		first = false;
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
				if (nsfirst > SY_NDAT) {
					err("%s: %s %d > %d %s", NAME,
					    "unable to handle", nsfirst,
					    SY_NDAT, "samples per trace");
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
			switch (nread = pread(fd,(char *) tp,HDRBYTES)) {
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
			if (nsfirst > SY_NDAT) {
			    err("%s: %s %d>%d %s", NAME, "unable to handle",
			        nsfirst, SY_NDAT, "samples per trace");
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
				    err("%s: %s %d bytes of %d", 
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
			if (nsfirst > SY_NDAT) {
				err("%s: %s %d > %d %s", NAME,
				    "unable to handle", nsfirst, SY_NDAT,
				    "samples per trace");
			}
			ndata = BYTESPER * nsfirst;
			nsegy = HDRBYTES + ndata;

			switch (nread =
			        read(fd, (char *) tp->data, (uint) ndata)) {
			case -1:
				syserr("%s: first trace disk read",NAME);
			case 0:	/* no data on trace */
				return(HDRBYTES);
			default:
				if (nread != ndata) {
				    err("%s: %s %d bytes of %d", 
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
			switch (nread = read(fd, (char *)tp, (uint)nsegy)) {
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
			switch (nread = read(fd, (char *)tp, (uint)nsegy)) {
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
		     err("%s: mysterious filetype trace #%ld", NAME, itr);
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
	return(nsegy);
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
char *sdoc = "\
								\n\
tgettr <stdin >stdout						\n\
								\n\
	Test harness for gettr.c				\n\
	Changes cdp to abs(cdp)					\n\
	Contrast the following results:	 			\n\
	suwind <DATA count=10 | suprhws cdp 			\n\
	suwind <DATA count=10 | tgettr | suprhws cdp 		\n\
								\n\
";
/*****************************************************************/

segy tr;

main(argc, argv)
int argc; char **argv;
{
	initgetpar(argc, argv); askdoc();

 	while (gettr(&tr)) {
 		tr.cdp = abs(tr.cdp);
 		puttr(&tr);
 	}

	exit(0);
}
#endif
