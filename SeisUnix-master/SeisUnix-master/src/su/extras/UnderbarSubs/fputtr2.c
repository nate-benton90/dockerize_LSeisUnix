/* FPUTTR: $Revision: 1.65 $; $Date: 89/05/25 16:10:13 $	*/

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

/* fputtr, fputtr1, fputtr2 - put a segy trace on a file by descriptor
 * puttr, puttr1, puttr2 - put a segy trace on stdout
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void fputtr(fd, tp)	for standard float traces
 *	segy *tp;
 *
 *	void fputtr1(fd, tp)	for 1 byte traces as in suunpack1
 *	segy *tp;
 *
 *	void fputtr2(fd, tp)	for 2 byte traces as in suunpack2
 *	segy *tp;
 *
 *	puttr, puttr1 and puttr2 are implemented as macros in cwp.h:
 *
 *	puttr(tp)  is equivalent to fputtr(stdin, tp)
 *	puttr1(tp) is equivalent to fputtr1(stdin, tp)
 *	puttr2(tp) is equivalent to fputtr2(stdin, tp)
 *
 * Example:
 *	segy tr;
 *	...
 *	while (gettr(&tr)) {
 *		tr.offset = abs(tr.offset);
 *		puttr(&tr);
 *	}
 *
 * Notes:
 *	There are three versions to make from this one source code, so
 *	as to avoid having to maintain nearly identical codes.  The
 *	"1" variant is for segy traces with char data, the "2" variant
 *	is for short data.  Use the compile options -DPUTTR, -DPUTTR1,
 *	and -DPUTTR2 respectively, for the three versions.
 *
 * Credits:
 *	SEP: Einar
 *	CWP: Jack, Shuki
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/fputtr.c,v $";
static string revid =
	"   $Revision: 1.65 $ ; $Date: 89/05/25 16:10:13 $";




#ifdef PUTTR1
#define NAME		"fputtr1"
#define BYTESPER	sizeof(char)	/* bytes per output datum */
void fputtr1(fd, tp)
#endif

#ifdef PUTTR2
#define NAME		"fputtr2"
#define BYTESPER	sizeof(short)	/* bytes per output datum */
void fputtr2(fd, tp)
#endif

#ifdef PUTTR
#define NAME		"fputtr"
#define BYTESPER	sizeof(float)	/* bytes per output datum */
void fputtr(fd, tp)
#endif

int fd;			/* file descriptor		*/
segy *tp;		/* pointer to segy trace	*/
{
	int nsegy;			/* number of bytes output	*/
	int nwrite;			/* number of bytes written	*/
	static ulong otr = 0;		/* running trace number		*/
	static bool first = true;	/* to check if first entry	*/
	static filetype ftype;		/* filetype of fd		*/


	if (first) {	/* First entry */
		first = false;

		if (ID) {
			(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
			(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
		}

		ftype = statfil(fd);
		switch (ftype) {
		case TAPE:	/* These three are correct filetypes */
		case DISK:
		case PIPE:
		break;
		case DIRECTORY:
			err("%s: stdout can't be a directory", NAME);
		case TTY:
			err("%s: stdout can't be tty", NAME);
		default:
			err("%s: stdout is undefined filetype: %s",
						NAME, statprint(ftype));
		}
	}

	otr++;
	nsegy = HDRBYTES + BYTESPER * tp->ns;
	nwrite = write(fd, (char *) tp, (uint) nsegy);
	switch (nwrite) {
	case -1:
		syserr("%s: error writing trace #%ld", NAME, otr);
	default:
		if (nwrite != nsegy) {
			errno = EFBIG;
			syserr("%s: error writing trace #%ld", NAME, otr);
		}
	}
	return;
}

/* For FORTRAN */
#ifdef PUTTR1
#define NAME		"fputtr1"
#define BYTESPER	sizeof(char)	/* bytes per output datum */
void fputtr1_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	fputtr1(*fd, tp);
}
#endif

#ifdef PUTTR2
#define NAME		"fputtr2"
#define BYTESPER	sizeof(short)	/* bytes per output datum */
void fputtr2_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	fputtr2(*fd, tp);
}
#endif

#ifdef PUTTR
#define NAME		"fputtr"
#define BYTESPER	sizeof(float)	/* bytes per output datum */
void fputtr_(fd, tp)
int *fd;
segy *tp;		/* pointer to segy trace */
{
	fputtr(*fd, tp);
}
#endif


#ifdef TEST

/*********************** self documentation **********************/
char *sdoc = "\
								\n\
tputtr <stdin >stdout						\n\
								\n\
	Test harness for puttr.c				\n\
	Changes cdp to abs(cdp)					\n\
	Contrast the following results:	 			\n\
	suwind <DATA count=10 | suprhws cdp 			\n\
	suwind <DATA count=10 | tputtr | suprhws cdp 		\n\
								\n\
";
/*****************************************************************/

segy tr;

main(argc, argv)
int argc; char **argv;
{
	initargs(argc, argv); askdoc();

 	while (gettr(&tr)) {
 		tr.cdp = abs(tr.cdp);
 		puttr(&tr);
 	}

	exit(0);
}
#endif
