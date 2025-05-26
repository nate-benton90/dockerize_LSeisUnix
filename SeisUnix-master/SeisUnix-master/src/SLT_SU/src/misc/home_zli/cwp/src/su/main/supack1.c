/* SUPACK1: $Revision: 1.9 $ ; $Date: 90/10/29 18:26:34 $	*/

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
string sdoc = "\
								\n\
SUPACK1 - pack segy trace data into chars			\n\
								\n\
supack1 <segy_file >packed_file	gpow=0.5 			\n\
								\n\
Required parameters:						\n\
	none							\n\
						        	\n\
Optional parameter: 						\n\
	gpow=0.5	exponent used to compress the dynamic	\n\
			range of the traces			\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Jack, Shuki, Brian
 *
 * Caveats:
 *	This program is for single site use.  Use segywrite to make
 *	a portable tape.
 *
 *	We are storing the local header words, ungpow and unscale,
 *	required by suunpack1 as floats.  Although not essential
 *	(compare the handling of such fields as dt), it allows us
 *	to demonstrate the convenience of using the natural data type.
 *	In any case, the data itself is non-portable floats in general,
 *	so we aren't giving up any intrinsic portability.
 *	
 * Notes:
 *	ungpow and unscale are defined in segy.h
 *	trid = CHARPACK is defined in su.h and segy.h
 *
 */


#define GPOW	0.5	/* default power parameter */

segy tr;	/* on  input: SEGY hdr & (float) trace data */
		/* on output: data as signed chars          */

main(int argc, char **argv)
{
	float gpow;
	int nt;
	bool isone, ishalf;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get parameters */
	if (!fgetpar("gpow", &gpow)) gpow = GPOW;
	if (gpow <= 0.0) err("gpow = %g must be positive", gpow);
	isone = CLOSETO(gpow, 1.0);
	ishalf = CLOSETO(gpow, 0.5);

	/* Get number of time samples from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* Main loop over segy traces */
	do {
		/* Point output trace at the trace data and pack.
		/* Since the chars take less room than the floats,
		/* we don't overwrite.
		/*
		/* Note that the segy field tr.data is declared as
		/* floats, so we need to invent a pointer for the
		/* char array which is actually there. */

		register signed char *otr = (signed char *) tr.data;
		register int i;
		register float absmax;
		register float scale;

		/* Power transform to decrease dynamic range */
		if (!isone) {
			register float val;

			if (ishalf) {
				for (i = 0; i < nt; ++i) {
					val = tr.data[i];
					tr.data[i] = (val >= 0.0) ?
						sqrt(val) : -sqrt(-val);
				}
			} else {
				for (i = 0; i < nt; ++i) {
					val = tr.data[i];
					tr.data[i] = (val >= 0.0) ?
						pow(val, gpow) : -pow(-val, gpow);
				}
			}
		}

		/* Store "ungpow" factor */
		tr.ungpow = 1.0/gpow;

		/* Read trace data and get absmax */
		absmax = ABS(tr.data[0]);
		for (i = 1; i < nt; ++i)
			absmax = MAX(absmax, ABS(tr.data[i]));

		/* Compute scale factor and store "unscale" factor
		/* If max is zero, then put scale and unscale to zero too */
		scale = absmax ? SCHAR_MAX/absmax : 0.0;
		tr.unscale = absmax ? 1.0/scale : 0.0;

		/* Apply the scale and load in char data */
		for (i = 0; i < nt; ++i) { 
			tr.data[i] *= scale;
			otr[i] = (signed char) tr.data[i];
		}

		/* Write trace ID as the packed char code number */
		tr.trid = CHARPACK;

		/* Output the "segy" with chars in the data array */
		puttr(&tr);

	} while (gettr(&tr));

	
	return EXIT_SUCCESS;
}
