/* SUTAB: $Revision: 1.9 $ ; $Date: 2003/06/09 16:17:07 $			*/

#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									",
" SUTAB - print non zero header values and data for non-graphic terminals",
"									",
" sutab <stdin itmin=0 itmax=last_sample count=all			",
"									",
" Required parameters:							",
"	none								",
"							        	",
" Optional parameters: 							",
"	itmin=0			first time sample (zero-based) to plot	",
"	itmax= (last sample)	last time sample (zero-based) to plot	",
"	count= (all traces)	number of traces to plot		",
"							        	",
" Example:								",
"	sutab <DATA itmin=32 itmax=63 count=10				",
" Requests tab plot of samples 32 to 63 on the first 10 traces of DATA.	",
"							        	",
NULL};

/* Credits:
 *	CWP: Shuki Ronen, Jack K. Cohen
 *
 *
 * Trace header fields accessed: ns
 */
/**************** end self doc *******************************************/


segy tr;

int
main(int argc, char **argv)
{
	int itmin;		/* smallest sample (zero-based)	to plot	*/
	int itmax;		/* largest sample (zero-based) to plot	*/
	int nt;			/* number of samples			*/
	int count;		/* number of traces to plot		*/
	register int itr;	/* trace counter			*/
	cwp_Bool plotall;	/* plot all the traces			*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Set number of traces to plot */
	plotall = cwp_false;
	if (!getparint("count", &count)) plotall = cwp_true;


	/* Loop over traces */
	for (itr = 0; (plotall || itr < count) && gettr(&tr); itr++) {

		nt = (int) tr.ns;	/* Cast from unsigned */
		if (itr == 0) {	/* Awkward to do a gettr outside loop */
			if (!getparint("itmin", &itmin))	itmin = 0;
			if (!getparint("itmax", &itmax))	itmax = nt - 1;
			if (itmin >= nt - 1 || itmin < 0) {
				err("itmin=%d, require 0 < itmin < %d",
							itmin, nt - 1);
			}
			if (itmax >= nt) {
				itmax = nt - 1;
			}
			if (itmax < 0) {
				err("itmax=%d, require itmax > 0", itmax);
			}
			if (itmin > itmax) {
				itmin = itmax;
			}
		}

		printheader(&tr);

		tabplot(&tr, itmin, itmax);

	}


	return(CWP_Exit());
}
