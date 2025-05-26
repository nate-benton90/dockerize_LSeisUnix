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
	_doprnt(format, args, stderr);
	fprintf(stderr, "\n");
	exit(1);
}
