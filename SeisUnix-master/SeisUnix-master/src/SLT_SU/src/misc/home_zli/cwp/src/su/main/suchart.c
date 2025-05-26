/* SUCHART: $Revision: 1.8 $ ; $Date: 90/11/20 17:34:01 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.colorado.mines.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUCHART - prepare data for x vs. y plot			\n"
" 								\n"
" suchart <stdin >stdout key1=sx key2=gx			\n"
" 								\n"
" Required parameters:						\n"
" 	none							\n"
" 								\n"
" Optional parameters:						\n"
" 	key1=sx  	abscissa 				\n"
" 	key2=gx		ordinate				\n"
"	outpar=null	name of parameter file			\n"
" 								\n"
" The output is the (x, y) pairs of binary floats		\n"
" 								\n"
" Examples:							\n"
" suchart <segy_data outpar=pfile >plot_data			\n"
" psgraph <plot_data par=pfile title=\"CMG\" \\			\n"
"	linewidth=0 marksize=2 mark=8 | pwin			\n"
" rm plot_data 							\n"
" 								\n"
" suchart <segy_data | psgraph n=1024 d1=.004 \\		\n"
"	linewidth=0 marksize=2 mark=8 | pwin			\n"
" 								\n"
" suchart <data | curve label1=shot label2=geo | vplot_pen	\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Jack
 *
 * Notes:
 *	The vtof routine from valpkge converts values to floats.
 */


segy tr;

main(int argc, char **argv)
{
	string key1,  key2;	/* x and y key header words	*/
	Value  val1,  val2;	/* ... their values		*/
	string type1, type2;	/* ... their types		*/
	int index1, index2;	/* ... their indices in hdr.h	*/
	float x, y;		/* temps to hold current x & y 	*/
	string outpar;		/* name of par file		*/
	register int npairs;	/* number of pairs found	*/


	/* Hook up getpars */
	initargs(argc, argv);
	askdoc(1);


	/* Prevent byte codes from spilling to screen */
	if (isatty(STDOUT)) err("must redirect or pipe binary output");


	/* Get parameters */
	if (!getparstring("key1", &key1))	key1 = "sx";
	if (!getparstring("key2", &key2))	key2 = "gx";

	type1 = hdtype(key1);
	type2 = hdtype(key2);

	index1 = getindex(key1);
	index2 = getindex(key2);


	/* Loop over traces */
	npairs = 0;
	while(gettr(&tr)) {

		gethval(&tr, index1, &val1);
		gethval(&tr, index2, &val2);

		x = vtof(type1, val1);
		y = vtof(type2, val2);

		efwrite(&x, FSIZE, 1, stdout);
		efwrite(&y, FSIZE, 1, stdout);

		++npairs;
	}


	/* Make parfile if needed */
	if (getparstring("outpar", &outpar))
		fprintf(efopen(outpar, "w"),
			"n=%d label1=%s label2=%s\n",
			npairs, key1, key2);

	return EXIT_SUCCESS;
}
