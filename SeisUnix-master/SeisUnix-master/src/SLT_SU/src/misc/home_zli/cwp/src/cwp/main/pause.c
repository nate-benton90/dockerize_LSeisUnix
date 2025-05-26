/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/* pause - prompt and wait for user signal to continue
 *
 * Note:
 *	Default prompt is "press return key to continue" which is
 *	evoked by calling pause with no arguments.  The word,
 *	"continue", is replaced by any arguments handed to pause.
 *	Thus, the command "pause do plot" will evoke the prompt,
 *	"press return key to do plot".
 *
 * Credits:
 *	CWP: Shuki
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
*/

#include "cwp.h"

main(argc, argv)
int argc; char **argv;
{
	char do_what[BUFSIZ];
	int i;

	if (argc == 1) {
		strcpy(do_what, "continue");
	} else {
		strcpy(do_what, argv[1]);
		for (i = 2; i < argc; i++) {
			strcat(do_what, " ");
			strcat(do_what, argv[i]);
		}
	}
	printf("\npress return key to %s\n", do_what);
	getchar();
	exit(0);
}
