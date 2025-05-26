h60826
s 00004/00002/00111
d D 1.4 88/11/15 14:01:53 shuki 4 3
c 
e
s 00006/00002/00107
d D 1.3 88/10/13 12:22:07 shuki 3 2
c print xargv[0] only if is not NULL
e
s 00000/00072/00109
d D 1.2 88/04/19 16:52:42 shuki 2 1
c 
e
s 00181/00000/00000
d D 1.1 88/04/14 13:47:50 shuki 1 0
c date and time created 88/04/14 13:47:50 by shuki
e
u
U
f e 0
t
T
I 1
/* err - print warning on application program error and die
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void err(filnam, linnum, format, args)
 *	format - a printf format string ("\n" not needed)
 *      args   - the variables referenced in the format string
 *
 * Notes:
 *	For portability, this routine uses varargs and the "format"
 *      and "args" are bundled as per vararg.h.
 *
 * Author:
 *	Shuki
 *
 * Example:
 *	err(__FILE__,__LINE__,"Cannot divide %f by %f", x, y);
*/

#include <stdio.h>

extern int xargc;
extern char **xargv;

#include <varargs.h>
/*VARARGS0*/
void err(va_alist)
va_dcl
{
	va_list args;
	char *format,*filnam;
	int linnum;

	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nerr: fflush failed on stdout");
	}
D 3
	fprintf(stderr, "\n%s: ", xargv[0]);
E 3
I 3

	if(xargv != NULL) fprintf(stderr, "\n%s: ", xargv[0]);

E 3
	va_start(args);

	filnam = va_arg(args, char *);
	linnum = va_arg(args, int );
	format = va_arg(args, char *);

	fprintf(stderr,"ERR(FILE=%s,LINE=%d): ",filnam,linnum);

D 4
	vfprintf(stderr,format,args);
E 4
I 4
/* 	vfprintf(stderr,format,args); */
	_doprnt(format, args, stderr);
E 4

	fprintf(stderr, "\n");

	exit(1);
}

/* For FORTRAN */
void err_(va_alist)
va_dcl
{
	err(va_alist);
}

D 2
void syserr(va_alist)
va_dcl
{
	va_list args;
	char *format;
	extern int errno, sys_nerr;
	extern char *sys_errlist[];

	if (EOF == fflush(stdout)) {
		err(__FILE__,__LINE__,"syswarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	_doprnt(format, args, stderr);
	if (errno > 0 && errno < sys_nerr) { 
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	}
	fprintf(stderr, "\n");
	exit(1);
}


/* I don't know why the following does not work:
#include <varargs.h>

void err(va_alist)
va_dcl
{
	warn(va_alist);
	exit(1);
}
*/
E 2
/* warn - print warning on application program error
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void warn(filnam,linnum,format, args)
 *	format - a printf format string ("\n" not needed)
 *      args   - the variables referenced in the format string
 *
 * Notes:
 *	For portability, this routine uses varargs and the "format"
 *      and "args" are bundled as per vararg.h.
 *
 * Example:
 *	warn(__FILE__,__LINE__,"fmax = %f exceeds half nyquist= %f", fmax, 0.25/dt);
*/

void warn(va_alist)
va_dcl
{
	va_list args;
	char *format,*filnam;
	int linnum;

	if (EOF == fflush(stdout)) {
		fprintf(stderr, "\nerr: fflush failed on stdout");
	}
D 3
	fprintf(stderr, "\n%s: ", xargv[0]);
E 3
I 3

	if(xargv != NULL) fprintf(stderr, "\n%s: ", xargv[0]);

E 3
	va_start(args);

	filnam = va_arg(args, char *);
	linnum = va_arg(args, int );
	format = va_arg(args, char *);

	fprintf(stderr,"WARN(FILE=%s,LINE=%d): ",filnam,linnum);

D 4
	vfprintf(stderr,format,args);
E 4
I 4
/* 	vfprintf(stderr,format,args); */
	_doprnt(format, args, stderr);
E 4

	fprintf(stderr, "\n");
}

/* For FORTRAN */
void warn_(va_alist)
va_dcl
{
	warn(va_alist);
D 2
}
/* syswarn - print warning on a system error
 *
 * Credits: 
 *	Kernighan and Pike, "The UNIX Programming Environment",
 *	page 207.  Also Rochkind, "Advanced UNIX Programming",
 *      page 13.
 *      CWP: Jack, Shuki
 *
 */

int syswarn(va_alist)
va_dcl
{
	va_list args;
	char *format;
	extern int errno, sys_nerr;
	extern char *sys_errlist[];

	if (EOF == fflush(stdout)) {
		err(__FILE__,__LINE__,"syswarn: fflush failed on stdout");
	}
	fprintf(stderr, "\n%s: ", xargv[0]);
	va_start(args);
	format = va_arg(args, char *);
	_doprnt(format, args, stderr);
	if (errno > 0 && errno < sys_nerr) { 
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	}
	fprintf(stderr, "\n");
	return(errno);
}


/* For FORTRAN */
int syswarn_(va_alist)
va_dcl
{
	return(syswarn(va_alist));
E 2
}
E 1
