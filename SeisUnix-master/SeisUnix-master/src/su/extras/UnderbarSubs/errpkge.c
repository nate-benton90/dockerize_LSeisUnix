/* ERRPKGE: $Revision: 1.14 $ ; $Date: 89/05/25 16:10:04 $	*/

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

#include <varargs.h>
#include "cwp.h"

/* errpkge - routines for reporting errors
 *
 * err     - print warning on application program error and die
 * warn    - print warning on application program error
 * syserr  - print warning on a system error and die
 * syswarn - print warning on a system error
 *
 * Returns:
 *	err  - void
 *	warn - void
 *	syserr  - void
 *	syswarn - int: errno
 *
 * Synopsis:
 *	void err(format, args)
 *	format - a printf format string ("\n" not needed)
 *      args   - the variables referenced in the format string
 *
 *	void warn(format, args)
 *	format - a printf format string ("\n" not needed)
 *      args   - the variables referenced in the format string
 *
 *	void syserr(format, args)
 *	format - a printf format string ("\n" not needed) usually
 *		 giving the name of the offending system call
 *      args   - the variables referenced in the format string, often
 *		 just an offending file name
 *
 *	int syswarn(format, args)
 *	format - a printf format string ("\n" not needed) usually
 *		 giving the name of the offending system call
 *      args   - the variables referenced in the format string, often
 *		 just an offending file name
 *
 * Notes:
 *	For portability, these routines use varargs and the "format"
 *      and "args" are bundled as per vararg.h.
 *
 * Credits: 
 *	Kernighan and Pike, "The UNIX Programming Environment", page 207.
 *	Also Rochkind, "Advanced UNIX Programming", page 13.
 *	SEP: Jeff Thorson, Stew
 *	CWP: Shuki, Jack
 *
 * Examples:
 *	err("Cannot divide %f by %f", x, y);
 *	warn("fmax = %f exceeds half nyquist= %f", fmax, 0.25/dt);
 *
 *	if (-1 == (fd = open(xargv[1], O_RDONLY)))
 *		syserr("can't open %s", xargv[1]);
 *	...
 *	if (-1 == close(fd))
 *		syserr("close failed");
 *
 *	...
 *	if (-1 == (fd = open(xargv[j], O_RDONLY)))
 *		syswarn("can't open %s, processing next file",
 *			xargv[j]);
 *	continue;
 *	...
 *
 *
 */


/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/errpkge.c,v $";
static string revid =
	"   $Revision: 1.14 $ ; $Date: 89/05/25 16:10:04 $";



/* For syserr() and syswarn() */
extern int errno, sys_nerr;
extern char *sys_errlist[];

static bool first = true;	/* to check if first entry	*/

void err(va_alist)
va_dcl
{
	va_list args;
	char *format;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first=false;


	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nerr: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(FAIL);
}



void warn(va_alist)
va_dcl
{
	va_list args;
	char *format;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nwarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	return;
}


void syserr(va_alist)
va_dcl
{
	va_list args;
	char *format;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	if (EOF == fflush(stdout)) {
		err("syswarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	if (errno > 0 && errno < sys_nerr) { 
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	}
	fprintf(stderr, "\n");
	exit(FAIL);
}

int syswarn(va_alist)
va_dcl
{
	va_list args;
	char *format;

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}
	first = false;


	if (EOF == fflush(stdout)) {
		err("syswarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	if (errno > 0 && errno < sys_nerr) { 
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	}
	fprintf(stderr, "\n");
	return errno;
}


/* For FORTRAN */
void err_(va_alist)
va_dcl
{
	err(va_alist);
}

void warn_(va_alist)
va_dcl
{
	warn(va_alist);
}

void syserr_(va_alist)
va_dcl
{
	syserr(va_alist);
}

int syswarn_(va_alist)
va_dcl
{
	return(syswarn(va_alist));
}


#ifdef TEST
main(argc, argv)
int argc; char **argv;
{
	int fd;
	float fmax, dt=0.004, nyq,  x=1.0, y=0.0;

	initargs(argc, argv);

	nyq = 1.0/(2.0*dt); fmax = nyq/2.0 + .01;

	fprintf(stderr, "\nTrue values:\n");
	fprintf(stderr, "fmax=%f, half nyquist=%f\n", fmax, 0.25/dt);
	fprintf(stderr, "filename=%s\n", xargv[1]);
	fprintf(stderr, "x=%f, y=%f\n\n", x, y);

 	warn("warn: fmax = %f exceeds half nyquist= %f", fmax, 0.25/dt);

  	if (-1 == (fd = open(xargv[1], O_RDONLY)))
  		syswarn("syswarn: can't open %s", xargv[1]);
  
 	err("err: Cannot divide %f by %f", x, y);

	exit(0);
}
#endif
