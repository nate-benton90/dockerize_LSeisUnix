/* SUCHW: $Revision: 1.4 $ ; $Date: 92/10/26 13:30:04 $		*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"								",
" SUCHW - change header word using one or two header word fields",
"								",
" suchw <stdin >stdout [optional parameters]			",
"								",
" Required parameters:						",
"	none							",
"								",
" Optional parameters:						",
"	key1=cdp	output key 				",
"	key2=cdp	input key  				",
"	key3=cdp	input key  				",
"	a=0		overall shift 				",
"	b=1		scale on first input key 		",
"	c=0		scale on second input key 		",
"	d=1		overall scale 				",
"								",
" The value of header word key1 is computed from the values of	",
" key2 and key3 by:						",
"								",
"	val(key1) = (a + b * val(key2) + c * val(key3)) / d	",
"								",
" Examples:							",
" Shift cdp numbers by -1:					",
"	suchw <data >outdata a=-1				",
"								",
" Add 1000 to tracr value:					",
" 	suchw key1=tracr key2=tracr a=1000 <infile >outfile	",
"								",
" We set the receiver point (gx) field by summing the offset	",
" and shot point (sx) fields and then we set the cdp field by	",
" averaging the sx and gx fields (we choose to use the actual	",
" locations for the cdp fields instead of the conventional	",
" 1, 2, 3, ... enumeration):					",
"	suchw <indata key1=gx key2=offset key3=sx b=1 c=1 |	",
"	suchw key1=cdp key2=gx key3=sx b=1 c=1 d=2 >outdata	",
"								",
NULL};
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 *
 * Caveat:
 *	The constants a, b, c, d are read in as doubles.
 *	It is implicitly assumed that the data types of the
 *	keys used are the same.
 */


segy tr;

main(int argc, char **argv)
{
	String key1, key2, key3;
	String type1;
	int index1, index2, index3;
	Value val1, val2, val3;
	double a, c, b, d;
	void changeval(String type1,
		Value *valp1, Value *valp2, Value *valp3,
		double a, double b, double c, double d);
	FILE *infp=stdin, *outfp=stdout;


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	file2g(infp);
	file2g(outfp);

	/* Get parameters */
	if (!getparstring("key1", &key1))	key1 = "cdp";
	if (!getparstring("key2", &key2))	key2 = "cdp";
	if (!getparstring("key3", &key3))	key3 = "cdp";
	if (!getpardouble("a"   , &a))	a = 0;
	if (!getpardouble("b"   , &b))	b = 1;
	if (!getpardouble("c"   , &c))	c = 0;
	if (!getpardouble("d"   , &d))	d = 1;

	type1  = hdtype(key1);
	index1 = getindex(key1);
	index2 = getindex(key2);
	index3 = getindex(key3);

	while (gettr(&tr)) {
		gethval(&tr, index2, &val2);
		gethval(&tr, index3, &val3);
		changeval(type1, &val1, &val2, &val3, a, b, c, d);
		puthval(&tr, index1, &val1);
		puttr(&tr);
	}

	return EXIT_SUCCESS;
}


void changeval(String type1, Value *valp1, Value *valp2, Value *valp3,
		double a, double b, double c, double d)
{
	switch (*type1) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		valp1->h = (a + b * valp2->h + c * valp3->h)/d;
	break;
	case 'u':
		valp1->u = (a + b * valp2->u + c * valp3->u)/d;
	break;
	case 'l':
		valp1->l = (a + b * valp2->l + c * valp3->l)/d;
	break;
	case 'v':
		valp1->v = (a + b * valp2->v + c * valp3->v)/d;
	break;
	case 'i':
		valp1->i = (a + b * valp2->i + c * valp3->i)/d;
	break;
	case 'p':
		valp1->p = (a + b * valp2->p + c * valp3->p)/d;
	break;
	case 'f':
		valp1->f = (a + b * valp2->f + c * valp3->f)/d;
	break;
	case 'd':
		valp1->d = (a + b * valp2->d + c * valp3->d)/d;
	break;
	default:
		err("unknown type %s", type1);
	break;
	}
}
