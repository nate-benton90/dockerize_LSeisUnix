/*
#define vfprintf(stderr, format, args)	_doprnt(format, args, stderr)
*/

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
 * $Source: /src/su/lib/RCS/errpkge.c,v $
 * $Revision: 1.6 $ ; $Date: 88/05/13 22:57:56 $
*/


/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/lib/RCS/errpkge.c,v $";
static char revid[] =
	"   $Revision: 1.6 $ ; $Date: 88/05/13 22:57:56 $";


#include "../include/cwp.h"

#include <varargs.h>
void err(va_alist)
va_dcl
{
	va_list args;
	char *format;

	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nerr: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(1);
}



void warn(va_alist)
va_dcl
{
	va_list args;
	char *format;

	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nwarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
}



extern int errno, sys_nerr;
extern char *sys_errlist[];

void syserr(va_alist)
va_dcl
{
	va_list args;
	char *format;

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
	exit(1);
}

int syswarn(va_alist)
va_dcl
{
	va_list args;
	char *format;

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
	return(errno);
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


#ifdef TRYMAIN
main(argc, argv)
int argc; char **argv;
{
	int fd;
	float fmax, dt=0.004, nyq,  x=1.0, y=0.0;

	xargc = argc; xargv = argv;

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
