/* SUUNPACK2: $Revision: 1.7 $ ; $Date: 90/10/29 18:04:46 $	*/

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
string sdoc = "\
								\n\
SUUNPACK2 - unpack segy trace data from shorts to floats	\n\
								\n\
suunpack2 <packed_file >unpacked_file				\n\
								\n\
suunpack2 is the approximate inverse of supack2			\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack, Shuki, Brian
 *
 * Caveats:
 *	This program is for single site use with supack2.  See the
 *	supack2 header comments.
 *
 * Notes:
 *	ungpow and unscale are defined in segy.h
 *	trid = SHORTPACK is defined in su.h and segy.h
 *
 */


segy tr;	/* on input: SEGY hdr & (short) trace data
		/* on output: data is floats */

main(int argc, char **argv)
{
	float ungpow;
	int nt;
	bool isone, istwo;

	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get info from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	if (tr.trid != SHORTPACK) err("Not short packed traces");
	nt = tr.ns;
	ungpow = tr.ungpow;
	isone = CLOSETO(ungpow, 1.0);
	istwo = CLOSETO(ungpow, 2.0);


	/* Main loop over segy traces */
	do {
		/* Point input short trace at the trace data and unpack.
		/* Since the floats take more room than the shorts,
		/* we load in from back end.
		/*
		/* Note that the segy field tr.data is declared as
		/* floats, so we need to invent a pointer for the
		/* short array which is actually there. */

		register int i;
		register float val;
		register short *itr = (short *) tr.data;

		if (istwo) {
			for (i = nt-1; i >= 0; --i) { 
				val = (float) itr[i];
				val *= tr.unscale;
				tr.data[i] = val * ABS(val);
			}
		} else if (isone) {
			for (i = nt-1; i >= 0; --i) { 
				val = (float) itr[i];
				val *= tr.unscale;
				tr.data[i] = val;
			}
		} else {
			for (i = nt-1; i >= 0; --i) { 
				val = (float) itr[i];
				val *= tr.unscale;
				tr.data[i] = (val >= 0.0) ?
					pow(val, ungpow) : -pow(-val, ungpow);
			}
		}


		/* Mark as seismic data and remove now unnecessary fields */
		tr.trid = 1;
		tr.ungpow = 0.0;
		tr.unscale = 0.0;


		/* Write out restored (unpacked) segy */
		puttr(&tr);


	} while (gettr(&tr));


	return EXIT_SUCCESS;
}
