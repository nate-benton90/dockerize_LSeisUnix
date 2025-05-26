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
 */


#include <stdio.h>

void selfdoc()

{
	extern char *sdoc;
	FILE *fp;

	if (EOF == fflush(stdout)) {
		err(__FILE__,__LINE__,"selfdoc: fflush failed on stdout");
	}
	if (NULL == (fp = popen("more -12 1>&2", "w"))) {
		err(__FILE__,__LINE__,"selfdoc: popen failed on 'more' for writing");
	}
	fprintf(fp, "%s", sdoc);
	if (-1 == pclose(fp)) {
		err(__FILE__,__LINE__,"selfdoc: pclose failed on 'more'");
	}
	exit(1);
}

/* For FORTRAN */
void selfdoc_()
{
	selfdoc();
}
