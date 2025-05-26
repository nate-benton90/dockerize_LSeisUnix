/* SUSHW: $Revision: 1.8 $ ; $Date: 92/10/26 13:54:35 $			*/

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

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" SUSHW - set header word using trace number, mod and integer divide	",
" 									",
" sushw <stdin >stdout key=cdp a=0 b=0 c=0 d=0 j=1			",
" 									",
" Required parameters:							",
" 	none (no op)							",
" 									",
" Optional parameters:							",
" 	key=cdp		header key word to set				",
" 	a=0		value on first trace				",
" 	b=0		increment within group 				",
" 	c=0		group increment		 			",
" 	d=0		trace number shift				",
" 	j=ULONG_MAX	number of elements in group			",
" 									",
" NOTES:								",
" The value of header word key is computed using the formula:		",
" 	i = itr + d							",
" 	val(key) = a + b * (i % j) + c * (i / j)			",
" where itr is the trace number (first trace has itr=0, NOT 1)		",
" 									",
"  Example:								",
"  	sushw <indata key=dt a=4000 |					",
"  	sushw key=sx a=6400 c=-100 j=32 |				",
"  	sushw key=offset a=200 b=200 j=32 |				",
"  	suchw key1=gx key2=offset key3=sx b=1 c=1 |			",
"  	suchw key1=cdp key2=gx key3=sx b=1 c=1 d=2 >outdata		",
" 									",
" In this example, we set every dt field to 4ms.  Then we set the first	",
" 32 shotpoint fields to 6400, the second 32 shotpoint fields to 6300 and",
" so forth.  Next we set each group of 32 offset fields to 200, 400, ...,",
" 6400.  Finally we use suchw to set the receiver point and cdp fields	",
" using the sx and offset fields (we choose to use the actual location	",
" for the cdp fields instead of the conventional 1, 2, 3, ...		",
" enumeration).								",
" 									",
NULL}; 
/**************** end self doc ****************************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 *
 * Caveat:
 *	All constants are cast to doubles.
 */


segy tr;

/* Prototypes */
double mod(double x, double y);
void setval(String type, Value *valp,
	double a, double b, double c, double i, double j);


main(int argc, char **argv)
{
	String key;
	String type;
	int index;
	double a, c, b, d, i, j;
	long itr = 0;
	Value val;


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Get parameters */
	if (!getparstring("key", &key))	 key = "cdp";
	if (!getpardouble("a"  , &a))	 a = 0;
	if (!getpardouble("b"  , &b))	 b = 0;
	if (!getpardouble("c"  , &c))	 c = 0;
	if (!getpardouble("d"  , &d))	 d = 0;
	if (!getpardouble("j"  , &j))	 j = ULONG_MAX;

	type = hdtype(key);
	index = getindex(key);

	while (gettr(&tr)) {
		i = (double) itr++ + d;
		setval(type, &val, a, b, c, i, j);
		puthval(&tr, index, &val);
		puttr(&tr);
	}


	return EXIT_SUCCESS;
}


void setval(
	String type,
	Value *valp,
	double a, double b, double c, double i, double j)
{
	switch (*type) {
	case 's':
		err("can't set char header word");
	break;
	case 'h':
		valp->h = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'u':
		valp->u = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'l':
		valp->l = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'v':
		valp->v = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'i':
		valp->i = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'p':
		valp->p = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'f':
		valp->f = a + b * mod(i, j) + c * ((int) (i/j));
	break;
	case 'd':
		valp->d = a + b * mod(i, j) + c * ((int) (i/j));
	default:
		err("unknown type %s", type);
	break;
	}
	return;
}


double mod(double x, double y)	/* As defined in Knuth, vol. 1	*/
{
	return y == 0.0 ? x : x - y * floor(x/y);
}
