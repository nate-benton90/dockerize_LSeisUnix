/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* fcat - fast cat with 1 read per file
 *
 * Credits:
 *	Shuki
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
*/

#include <sys/types.h>
#include <sys/stat.h>
#include "cwp.h"

#define MAXSIZE	8*1024*1024
char x[MAXSIZE];

main(ac,av)
int ac; char **av;
{
	register int fd, ic, n;
	struct stat buf;

	if (ac < 2) {
		fprintf(stderr,"usage: fcat file1 file2 ... > file3\n");
		exit(1);
	}

	for (ic = 1; ic < ac; ic++) {
		fd = open(av[ic], O_RDONLY);
		if (fd == -1) fprintf(stderr, "can't open %s\n", av[ic]);
		fstat(fd, &buf);
		n = buf.st_size;
		if (n > MAXSIZE) {
			fprintf(stderr, "%s is too big\n", av[ic]);
			exit(1);
		}
		if (read(fd, x, n) != n) {
			fprintf(stderr, "read error from %s\n", av[ic]);
			exit(1);
		}
		close(fd);
		if (write(STDOUT, x, n) != n) {
			fprintf(stderr, "write error from %s\n", av[ic]);
			exit(1);
		}
	}
	exit(0);
}
