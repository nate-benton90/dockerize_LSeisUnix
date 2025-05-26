

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SU3DCHART - plot x-midpoints vs. y-midpoints for 3-D data	\n"
" 								\n"
" su3dchart <stdin >stdout					\n"
" 								\n"
" Optional parameter:						\n"
"	outpar=null	name of parameter file			\n"
" 								\n"
" The output is the (x, y) pairs of binary floats		\n"
" 								\n"
" Example:							\n"
" su3dchart <segy_data outpar=pfile >plot_data			\n"
" psgraph <plot_data par=pfile \\				\n"
"	linewidth=0 marksize=2 mark=8 | pwin			\n"
" rm plot_data 							\n"
" 								\n"
" su3dchart <segy_data | psgraph n=1024 d1=.004 \\		\n"
"	linewidth=0 marksize=2 mark=8 | pwin			\n"
" 								\n"
" su3dchart <data | curve | vplot_pen				\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Shuki
 *
 */


segy tr;

main(int argc, char **argv)
{
	float sx, sy, gx, gy, mx, my;
	string outpar;
	register int npairs;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	npairs = 0;
	while (gettr(&tr)) {

		sx = tr.sx;
		sy = tr.sy;
		gx = tr.gx;
		gy = tr.gy;

		mx = 0.5*(sx + gx);
		my = 0.5*(sy + gy);

		efwrite(&mx, FSIZE, 1, stdout);
		efwrite(&my, FSIZE, 1, stdout);

		++npairs;
	}


	/* Make parfile if needed */
	if (getparstring("outpar", &outpar))
		fprintf(efopen(outpar, "w"), "n=%d\n", npairs);
	
	return EXIT_SUCCESS;
}
