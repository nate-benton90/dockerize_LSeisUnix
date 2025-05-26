/* SUABSHW: $Revision: 1.3 $ ; $Date: 90/12/26 21:24:23 $	*/

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

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUABSHW - replace header key word by its absolute value	\n"
" 								\n"
" suabshw <stdin >stdout key=offset				\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameter:						\n"
" 	key=offset		header key word			\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Jack
 */


#define KEY	"offset"	/* Default key word to take abs() of */

segy tr;

main(int argc, char **argv)
{
	String key;	/* header key word from segy.h		*/
	String type;	/* ... its type				*/
	int index;	/* ... its index in hdr.h		*/
	Value val;	/* ... its value			*/
	void absval(String type, Value *valp);


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get key parameter */
	if (!getparstring("key", &key))	key = KEY;

	type = hdtype(key);
	index = getindex(key);

	while (gettr(&tr)) {
		gethval(&tr, index, &val);
		absval(type, &val);
		puthval(&tr, index, &val);
		puttr(&tr);
	}


	return EXIT_SUCCESS;
}


void absval(String type, Value *valp)
{
	switch (*type) {
	case 's': err("can't absval char header word"); break;
	case 'u':	/* do nothing if unsigned type */
	case 'v':
	case 'p':                                      break;
	case 'h': if (valp->h < 0) valp->h = -valp->h; break;
	case 'l': if (valp->l < 0) valp->l = -valp->l; break;
	case 'i': if (valp->i < 0) valp->i = -valp->i; break;
	case 'f': if (valp->f < 0) valp->f = -valp->f; break;
	case 'd': if (valp->d < 0) valp->d = -valp->d; break;
	default: err("unknown type %s", type);         break;
	}
}
