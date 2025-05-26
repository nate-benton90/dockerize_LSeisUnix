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
 * $Source: /src/su/lib/RCS/selfdoc.c,v $
 * $Revision: 1.24 $ ; $Date: 88/06/02 16:26:35 $
 */


/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/lib/RCS/selfdoc.c,v $";
static char revid[] =
	"   $Revision: 1.24 $ ; $Date: 88/06/02 16:26:35 $";


#include <stdio.h>

void selfdoc()

{
	extern char *sdoc;
	FILE *fp;

	if (EOF == fflush(stdout)) {
		syserr("selfdoc: fflush failed on stdout");
	}
	if (NULL == (fp = popen("more -18 1>&2", "w"))) {
		syserr("selfdoc: popen failed on 'more' for writing");
	}
	fprintf(fp, "%s", sdoc);
	if (-1 == pclose(fp)) {
		syserr("selfdoc: pclose failed on 'more'");
	}
	exit(1);
}

/* For FORTRAN */
void selfdoc_()
{
	selfdoc();
}
