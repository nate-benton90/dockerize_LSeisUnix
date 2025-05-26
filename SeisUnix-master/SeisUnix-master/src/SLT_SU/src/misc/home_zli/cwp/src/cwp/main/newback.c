/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* Toggle backspace between delete and backspace
 *
 * Credit: Kochan & Wood, Topics in C Programming, Page 264
 * CWP: Jack
 *
 * Caveat: compilers that are neither BSD nor Sys V might need a flag like -DBSD
 *         In any case, this is a convenience code, not something crucial.
 */

#include "cwp.h"
#include <sys/param.h>

#define BS	'\010'
#define DEL	'\177'

#ifdef BSD

#include <sgtty.h>
#define GET	TIOCGETP
#define SET	TIOCSETP
#define ERASE	term.sg_erase
#define IFILE	sgttyb

#else

#include "termio.h"
#define GET	TCGETA
#define SET	TCSETA
#define ERASE	term.c_cc[VERASE]
#define IFILE	termio

#endif


main()
{
	struct IFILE term;

	/* Put current state in term */
	if (-1 == ioctl(STDIN, GET, &term))
		perror("stdin is not a tty");

	/* Toggle backspace */
	ERASE = (ERASE == BS ? DEL : BS);

	/* Change terminal state */
	if (-1 == ioctl(STDIN, SET, &term))
		perror("couldn't reset backspace");


	return EXIT_SUCCESS;
}
