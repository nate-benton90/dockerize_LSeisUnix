/* SUPACK2: $Revision: 2.7 $ ; $Date: 89/09/23 16:54:45 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"
#include "segy.h"
#include "fconst.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUPACK2 - pack segy trace data into short ints			\n\
								\n\
supack2 <segy_file >packed_file gpow=0.5 			\n\
								\n\
Required parameters:						\n\
	none							\n\
						        	\n\
Optional parameter: 						\n\
	gpow = 0.5	exponent used to compress the dynamic	\n\
			range of the traces			\n\
								\n\
";
/*****************************************************************/

/* Credits:
 *	CWP: Jack, Shuki
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
 *	trid = SHORTPACK is defined in cwp.h and segy.h
 *
 *	spack() is optimized in C because it contains a cast.
 *      This code is modeled after some suggestions by Brian.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/supack2.c,v $";
static string revid =
	"   $Revision: 2.7 $ ; $Date: 89/09/23 16:54:45 $";


#define GPOW		0.5	/* default power parameter */

segy tr;	/* on  input: SEGY hdr & (float) trace data */
		/* on output: data as shorts                */

main(argc, argv)
int argc;
char **argv;

{
	short *otr;	/* temp to hold a packed datum			*/
	float absmax;	/* absolute max on a trace			*/
	int absmaxloc;	/* zero-based index of absmax			*/
	float gpow;	/* power to control dynamic range		*/
	float scale;	/* normalize float data to USHRT_MAX + 1 levels	*/
	int nt;		/* number of time samples			*/
	void spack();	/* scale and pack to shorts: optimized sub	*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID)) ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);


	/* Get parameters */
	if (!fgetpar("gpow", &gpow)) gpow = GPOW;

	/* Get number of time samples from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;

	/* Main loop over segy traces */
	do {

		/* Power transform to decrease dynamic range */
		if (gpow != 1.0) {
			if (gpow == 0.5) {
			    vssqrt_(tr.data, ONE, tr.data, ONE, &nt);
			} else if (gpow == 2.0) {
			    vssq_(tr.data, ONE, tr.data, ONE, &nt);
			} else {
			    vspow_(tr.data, ONE, &gpow, tr.data, ONE, &nt);
			}
		}

		/* Store "ungpow" factor */
		tr.ungpow = 1.0/gpow;

		/* Read trace data and get absmax */
		maxmgv_(tr.data, ONE, &absmax, &absmaxloc, &nt);

		/* Compute scale factor and store "unscale" factor        */
		/* If max is zero, then put scale and unscale to zero too */
		scale = absmax ? SHRT_MAX/absmax : 0.0;
		tr.unscale = absmax ? 1.0/scale : 0.0;

		/* Point output trace at the trace data and pack.  */
		/* Since the shorts take less room than the floats,*/
		/* we don't overwrite.                             */
		otr = (short *) tr.data;
		spack(tr.data, 1, scale, otr, 1, nt);

		/* Write trace ID as the packed short code number */
		tr.trid = SHORTPACK;

		/* Output the "segy" with shorts in the data array */
		puttr2(&tr);

	} while (gettr(&tr));

	
	return SUCCEED;
}



/* scale and pack: optimizes the following loop:

	for (i = 0; i < tr.ns; i++) { 
		tr.data[i] *= scale;
		otr[i] = (short) tr.data[i];
	}
*/

#define ROLLPOW	3
#define NROLL	(1 << ROLLPOW)
#define ROLLMSK	(NROLL - 1)

void spack(a, i, scale, c, k, n)
register float *a;
register float scale;
register short *c;
register int n;
int i, k;
{
	register int r;

	if (i ==1 && k==1) {
		switch (n & ROLLMSK) {
		case 7: *a *= scale;	*c++ = (short) *a++;
		case 6: *a *= scale;	*c++ = (short) *a++;
		case 5: *a *= scale;	*c++ = (short) *a++;
		case 4: *a *= scale;	*c++ = (short) *a++;
		case 3: *a *= scale;	*c++ = (short) *a++;
		case 2: *a *= scale;	*c++ = (short) *a++;
		case 1: *a *= scale;	*c++ = (short) *a++;
		}

		for (r = n >> ROLLPOW; r--; ) {
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
			*a *= scale;	*c++ = (short) *a++;
		}
	} else {
		for (r = n; r--; ) {
			*a *= scale;	*c++ = (short) *a++;
			c += k;
			a += i;
		}
	}
	return;
}
