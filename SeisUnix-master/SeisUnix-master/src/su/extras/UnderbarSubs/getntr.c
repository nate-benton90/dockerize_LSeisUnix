/* GETNTR: $Revision: 1.4 $; $Date: 89/05/25 16:10:25 $	*/

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

/* getntr - get number of traces
 *
 * Returns:
 *	integer number of traces in the disk file
 *
 * Synopsis:
 *	getntr(fd, traceptr)
 *	int fd;
 *	segy *traceptr;
 *
 * Credits:
 *	CWP: Jack
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/getntr.c,v $";
static string revid =
	"   $Revision: 1.4 $; $Date: 89/05/25 16:10:25 $";




int getntr(fd, tp)
int fd;
segy *tp;
{
	int nsfirst;	/* samples on first trace	*/
	int nsegy;	/* total size of trace in bytes	*/
	filetype ftype;	/* filetype (defined in cwp.h)	*/
	int nread;	/* bytes read			*/
	long length;	/* length of file in bytes	*/

	/* Echo version on request */
	if (ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}

	ftype = statfil(fd);
	switch(ftype) {
	case TTY:
		err("%s: stdin not redirected", __FILE__);
	break;
	case DISK:	/* correct */
	break;
	default:
		err("%s: input must be disk file", __FILE__);
	break;
	}

	switch (nread = read(fd, (char *)tp, HDRBYTES)) {
	case -1:
		syserr("%s: read failed on first header", __FILE__);
	default:
		if (nread != HDRBYTES) {
			err("%s: read only %d bytes of header",
						__FILE__, nread);
		}
	break;
	}

	nsfirst = tp->ns;
	if (nsfirst > SU_NFLTS) {
		err("%s: trace too long: nsfirst=%d>SU_NFLTS=%d",
			__FILE__, nsfirst, SU_NFLTS);
	}

	nsegy = HDRBYTES + nsfirst * FSIZE;
	if (-1L == (length = lseek(fd, 0L, SEEK_END))) {
		syserr("%s: lseek failed", __FILE__);
	}

	if (-1L == lseek(fd, 0L, SEEK_SET)) { /* rewind */
		syserr("%s: lseek failed", __FILE__);
	}

	return length / nsegy;

}

/* For FORTRAN */
int getntr_(fd, tp)
segy *tp;
int *fd;
{
	return getntr(*fd, tp);
}



#ifdef TEST

segy tr;

main()
{
	printf("Number of traces is: %d\n", getntr(STDIN, &tr));
	printf("Repeat to check rewind ...\n");
	printf("Number of traces is: %d\n", getntr(STDIN, &tr));
}

#endif TEST
