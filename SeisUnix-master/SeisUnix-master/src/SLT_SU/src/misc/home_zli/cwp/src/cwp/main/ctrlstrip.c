/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* ctrlstrip - Strip non-graphic characters
 *
 *	Usage: ctrlstrip <dirtyfile >cleanfile
 *
 * Credits:
 *	Masscomp RTU Programming Manual
 *	CWP: Jack
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
*/


#include <stdio.h>
#include <ctype.h>

#define printable(c)	isprint((c)) || (c) == '\t' || (c) == '\n'

main()	/* ctrlstrip - Strip non-graphic characters */
{
	int c;

	while (EOF != (c = getchar())) {
		if (printable(c)) {
			putchar(c);
		}
	}

	return(0);
}
