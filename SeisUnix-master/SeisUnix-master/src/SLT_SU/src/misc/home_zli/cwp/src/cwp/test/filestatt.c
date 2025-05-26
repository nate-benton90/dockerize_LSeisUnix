/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* Test driver for function filestat
 *
 * Here are some tests using filestatt to analyse STDIN, STDOUT and
 * the first command line argument along with the expected results:
 *	filestatt filestatt.c
 *		expect: TTY, TTY, DISK
 *	filestatt <filestatt.c /usr/local | cat
 *		expect: DISK, PIPE, DIRECTORY
 *	cat filestatt.c | filestatt filestatt.c >/usr/tmp/junkxxx
 *		expect: PIPE, DISK, DISK
 *	filestatt /dev/null
 *		expect: TTY, TTY, DISK
 *	filestatt
 *		expect: TTY, TTY, "no filename given"
 */

#include "cwp.h"

main(int argc, char **argv)
{
	static filetype ftype;
	int fd;


	fprintf(stderr, "Checking filestat ...\n");
	/* Get filetype of stdin	*/
	switch (ftype = filestat(STDIN)) {
	case TTY:
		fprintf(stderr, "filetype(STDIN) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(STDIN) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(STDIN) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(STDIN) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(STDIN) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(STDIN) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "filestat(stdin) failed: ftype = %d\n", ftype);
	}


	/* Get filetype of stdout	*/
	switch (ftype = filestat(STDOUT)) {
	case TTY:
		fprintf(stderr, "filetype(STDOUT) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(STDOUT) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(STDOUT) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(STDOUT) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(STDOUT) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(STDOUT) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "filestat(stdout) failed: ftype = %d\n", ftype);
	}

	/* Get filetype of argv[1]	*/
	if (argc == 1) {
		fprintf(stderr, "no filename given\n");
		exit(1);
	}
	if (-1 == (fd = open(argv[1], O_RDONLY))) {
		fprintf(stderr, "can't open %s", argv[1]);
		exit(2);
	}

	switch (ftype = filestat(fd)) {
	case TTY:
		fprintf(stderr, "filetype(fd) = TTY\n");
	break;
	case DISK:
		fprintf(stderr, "filetype(fd) = DISK\n");
	break;
	case DIRECTORY:
		fprintf(stderr, "filetype(fd) = DIRECTORY\n");
	break;
	case TAPE:
		fprintf(stderr, "filetype(fd) = TAPE\n");
	break;
	case PIPE:
		fprintf(stderr, "filetype(fd) = PIPE\n");
	break;
	case BADFILETYPE:
		fprintf(stderr, "filetype(argv[1]) = BADFILETYPE\n");
	break;
	default:
	    fprintf(stderr, "filestat(argv[1]) failed: ftype = %d\n", ftype);
	}


	fprintf(stderr, "Checking printstat ...\n");
	/* Print filetype of stdin	*/
	fprintf(stderr, "filetype(STDIN) = %s\n", printstat(STDIN));


	/* Print filetype of stdout	*/
	fprintf(stderr, "filetype(STDOUT) = %s\n", printstat(STDOUT));


	/* Print filetype of argv[1]	*/
	if (argc == 1) {
		fprintf(stderr, "no filename given\n");
		exit(1);
	}
	if (-1 == (fd = open(argv[1], O_RDONLY))) {
		fprintf(stderr, "can't open %s", argv[1]);
		exit(2);
	}

	fprintf(stderr, "filetype(fd) = %s\n", printstat(fd));

	exit(0);
}
