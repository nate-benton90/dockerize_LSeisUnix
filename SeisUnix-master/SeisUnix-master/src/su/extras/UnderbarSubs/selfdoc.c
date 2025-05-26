/* SELFDOC: $Revision: 1.31 $ ; $Date: 89/05/25 16:11:02 $	*/

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

/* selfdoc - print self documentation string
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void selfdoc()
 *
 * Credits:
 *	SEP: Einar, Stew
 *	CWP: Jack, Shuki
 *
 * Example:
 *	if (xargc != 3) selfdoc();
 *
 *
 */


void selfdoc()

{
	extern string sdoc;
	FILE *fp;


	if (EOF == fflush(stdout)) {
		syserr("selfdoc: fflush failed on stdout");
	}
	if (NULL == (fp = popen("more -18 1>&2", "w"))) {
		syserr("selfdoc: popen failed on 'more' for writing");
	}
	(void) fprintf(fp, "%s", sdoc);
	if (-1 == pclose(fp)) {
		syserr("selfdoc: pclose failed on 'more'");
	}
	exit(FAIL);
}

/* For FORTRAN */
void selfdoc_()
{
	selfdoc();
}
