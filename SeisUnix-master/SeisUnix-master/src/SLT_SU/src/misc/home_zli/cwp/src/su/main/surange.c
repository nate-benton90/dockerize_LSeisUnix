/* SURANGE: $Revision: 1.3 $ ; $Date: 90/12/20 09:18:55 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"
#include "header.h"
#include <signal.h>

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SURANGE - get max and min values for non-zero header entries	\n"
" 								\n"
" surange <stdin	 					\n"
" 								\n"
" Note: gives partial results if interrupted			\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Stew
 *	CWP: Jack
 *
 * Note: the use of "signal" is inherited from BSD days and may
 *       break on some UNIXs.  It is dicy in that the responsibility
 *	 for program termination is lateraled back to the main.
 *
 */


/* Prototypes */
void printrange(segy *tpmin, segy *tpmax);
static void closeinput(void);

segy tr, trmin, trmax;

main(int argc, char **argv)
{
	int ntr;		/* number of traces		*/
	Value val;		/* value of current keyword	*/
	Value valmin;		/* smallest seen so far		*/
	Value valmax;		/* largest seen so far		*/
	String type;		/* data type of keyword		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	bzero(&trmin, sizeof(segy));
	bzero(&trmax, sizeof(segy));
	signal(SIGINT, (void *) closeinput);
	signal(SIGTERM, (void *) closeinput);

	/* Do first trace outside loop to initialize mins and maxs */
	if (!gettr(&tr)) err("can't get first trace");
	{ register int i;
	  for (i = 0; i < SU_NKEYS; ++i) {
		gethval(&tr, i, &val);
		puthval(&trmin, i, &val);
		puthval(&trmax, i, &val);
	  }
	}


	ntr = 1;
	while (gettr(&tr)) {
		register int i;
	        for (i = 0; i < SU_NKEYS; ++i) {
			type = hdtype(getkey(i));
			gethval(&tr, i, &val);
			gethval(&trmin, i, &valmin);
			gethval(&trmax, i, &valmax);
			if (valcmp(type, val, valmin) < 0)
				puthval(&trmin, i, &val);
			if (valcmp(type, val, valmax) > 0)
				puthval(&trmax, i, &val);
		}
		++ntr;
	}

	printf("%d traces:\n", ntr);
	printrange(&trmin, &trmax);


	return EXIT_SUCCESS;
}



/* printrange - print non-zero header values ranges	*/
void printrange(segy *tpmin, segy *tpmax)
{
	register int i, j = 0;
	Value valmin, valmax;
	double dvalmin, dvalmax;
	String key;
	String type;

	for (i = 0; i < SU_NKEYS; ++i) {
		key = getkey(i);
		type = hdtype(key);
		gethval(tpmin, i, &valmin);
		gethval(tpmax, i, &valmax);
		dvalmin = vtod(type, valmin);
		dvalmax = vtod(type, valmax);
		if (dvalmin || dvalmax) {
			if (dvalmin < dvalmax) {
				printf(" %s=(", key);
				printfval(type, valmin);
				putchar(',');
				printfval(type, valmax);
				printf(") ");
			} else {
				printf(" %s=", key);
				printfval(type, valmin);
			}
			if ((++j % 5) == 0)
				putchar('\n');
		}
	}
	putchar('\n');
	return;
}


static void closeinput(void) /* for graceful interrupt termination */
{
	/* Close stdin and open /dev/null in its place.  Now we are reading
	/* from an empty file and the loops terminate in a normal fashion.  */
	efreopen("/dev/null", "r", stdin);
}
