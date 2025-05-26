/* GETTRA: $Revision: 1.21 $; $Date: 89/05/25 16:10:35 $	*/

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

/* gettra - get disk trace by trace number
 *
 * Returns:
 *	integer number of traces in the disk file
 *
 * Synopsis:
 *	#include "segy.h"
 *	gettra(fd, traceptr , itr)
 *	segy *traceptr;
 *	int itr, fd;
 *
 * Credits:
 *	SEP: Einar
 *	CWP: Jack
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/gettra.c,v $";
static string revid =
	"   $Revision: 1.21 $; $Date: 89/05/25 16:10:35 $";




int gettra(fd, tp, itr)
int fd;
segy *tp;
int itr;
{
	static int nsfirst;	/* samples on first trace	*/
	static int nsegy;	/* total size of trace in bytes	*/
	static int ntr;		/* number of traces in file	*/
	static filetype ftype;	/* filetype (defined in cwp.h)	*/
	static bool first=true;	/* flag for first entry		*/
	int nread;		/* bytes read			*/
	long length;		/* length of file in bytes	*/
	long nseek;		/* offset of trace in bytes	*/

	if (first) {	/* first entry; get number of traces */
		first = false;

		/* Echo version on request */
		if (ID) {
			(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
			(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
		}

		ftype = statfil(fd);
		switch(ftype) {
		case TTY:
			warn("stdin not redirected");
			selfdoc();
		break;
		case DISK:	/* correct */
		break;
		default:
			err("%s: input must be disk file", __FILE__);
		break;
		}

		switch (nread = read(fd, (char *)tp, HDRBYTES)) {
		case -1:
			syserr("read failed on first header");
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
		ntr = length / nsegy;
	}


	if (itr >= ntr) {
		err("%s, trying to read off end of file", __FILE__);
	}

	/* Position file pointer at start of requested trace */
	nseek = itr * nsegy;
	if (-1L == lseek(fd, nseek, SEEK_SET)) {
		syserr("%s: lseek failed on requested trace", __FILE__);
	}

	switch (nread = read(fd, (char *)tp, (uint) nsegy)) {
	case -1:
		err("%s: bad read trace %d of %d", __FILE__, 1+(itr), ntr);
	break;
	default:
		if (nread != nsegy) {
		    err("%s: read only %d of %d bytes in trace",
						__FILE__, nread, nsegy);
		}
	break;
	}

	if (tp->ns != nsfirst) {
	    warn("%s: header ns field = %d differs from first trace = %d",
						__FILE__, tp->ns, nsfirst);
	}

	return ntr;
}

/* For FORTRAN */
int gettra_(fd, tp, itr)
segy *tp;
int *itr,*fd;
{
	return gettra(*fd, tp, *itr);
}
