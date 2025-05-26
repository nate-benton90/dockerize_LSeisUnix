h61890
s 00000/00000/00043
d D 1.2 88/11/15 14:01:56 shuki 2 1
c 
e
s 00043/00000/00000
d D 1.1 88/04/14 13:47:52 shuki 1 0
c date and time created 88/04/14 13:47:52 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
