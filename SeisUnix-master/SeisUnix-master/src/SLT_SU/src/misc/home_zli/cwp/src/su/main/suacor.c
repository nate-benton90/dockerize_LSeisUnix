/* SUACOR: $Revision: 1.2 $ ; $Date: 90/11/15 10:43:04 $		*/

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

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUACOR - auto-correlation\n"
"\n"
"suacor <stdin >stdout [optional parms]\n"
"\n"
"Optional Parameters:\n"
"ntout=101               number of time samples output\n"
"norm=1                  if non-zero, normalize maximum absolute output to 1\n"
"\n"
"Notes:\n"
"The auto-correlation is computed for lags 0, 1, ..., ntout-1.\n"
"\n"
"Trace header fields accessed:  ns\n"
"Trace header fields modified:  ns and delrt\n"
"\n";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Dave
 */

segy tr;

main(int argc, char **argv)
{
	int nt,ntout,it,norm;
	float scale,*temp;

	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* get parameters */
	if (!getparint("ntout",&ntout)) ntout=101;
	if (!getparint("norm",&norm)) norm = 1;
	
	/* allocate workspace */
	temp = ealloc1float(ntout);
	
	/* loop over traces */
	do {
		xcor(nt,0,tr.data,nt,0,tr.data,ntout,0,temp);
		if (norm) {
			scale = 1.0/(temp[0]==0.0?1.0:temp[0]);
			for (it=0; it<ntout; it++)
				temp[it] *= scale;
		}
		memcpy((char*)tr.data, (char*)temp, ntout*sizeof(float));
		tr.ns = ntout;
		tr.delrt = 0;
		puttr(&tr);
	} while(gettr(&tr));

	return EXIT_SUCCESS;
}
