/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/*
 * rec -- upload Wordstar files
 * Usage: rec file
 *
 * Credit: Jack
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

/* NOTE: This program kills the second NL sent by Wordstar.
         For robustness, this version checks before killing the 
	 "second newline" (in case file didn't come from Wordstar!).
*/

main(argc, argv) /* copy Wordstar input to file */
int argc;
char **argv;
{
	FILE *fp;
	char *name = *argv;
	struct stat buf;

	--argc;
	if (!argc) {
		copyto(stdout);
	} else if (**++argv == '-') {
		fprintf(stderr, "Usage: %s [file] (no -options)\n", name);
		exit(1);
	} else if (argc == 1) {
		if (!stat(*argv, &buf)) {
			fprintf(stderr, "%s: file %s exists.\n", name, *argv);
			exit(1);
		} 
		else if (!(fp = fopen(*argv, "w"))) {
			fprintf(stderr, "%s: can't open %s.\n", name, *argv);
			exit(1);
		} else {
			copyto(fp);
		}
	} else {
		fprintf(stderr, "Usage: %s [file] (one file only)\n", name);
	}
	exit(0);
}

copyto(file) /* copy input to output with end-of-line processing */
register FILE *file;
{
	register int c;

	while ( (c = getchar()) != EOF) {
		putc(c, file);
		if (c == '\n') {	 /* ignore second newline */
			c = getchar();
			if (c != '\n' && c != EOF) { /* check to make sure */
				putc(c, file);
			}
		}
	}
}
