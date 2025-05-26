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

	if(xargv != NULL) fprintf(stderr, "\n%s: ", xargv[0]);

	va_start(args);

	filnam = va_arg(args, char *);
	linnum = va_arg(args, int );
	format = va_arg(args, char *);

	fprintf(stderr,"ERR(FILE=%s,LINE=%d): ",filnam,linnum);

/* 	vfprintf(stderr,format,args); */
	_doprnt(format, args, stderr);

	fprintf(stderr, "\n");

	exit(1);
}

/* For FORTRAN */
void err_(va_alist)
va_dcl
{
	err(va_alist);
}

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

	if(xargv != NULL) fprintf(stderr, "\n%s: ", xargv[0]);

	va_start(args);

	filnam = va_arg(args, char *);
	linnum = va_arg(args, int );
	format = va_arg(args, char *);

	fprintf(stderr,"WARN(FILE=%s,LINE=%d): ",filnam,linnum);

/* 	vfprintf(stderr,format,args); */
	_doprnt(format, args, stderr);

	fprintf(stderr, "\n");
}

/* For FORTRAN */
void warn_(va_alist)
va_dcl
{
	warn(va_alist);
}
