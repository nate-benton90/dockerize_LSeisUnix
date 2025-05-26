/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* isatty - pass on return from isatty(2)
 *
 * Credits:
 *	CWP: Shuki
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
*/

#include <stdio.h>

main(argc, argv)
int argc; char **argv;
{
	int fildes;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s fildes\n", argv[0]);
		exit(1);
	}

	fildes = atoi(argv[1]);
	printf("isatty: saw fildes=%d, isatty? %d\n", fildes, isatty(fildes));

	exit(0);
}
