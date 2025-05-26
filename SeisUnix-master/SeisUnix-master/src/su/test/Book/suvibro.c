/* SUVIBRO: $Revision: 1.4 $ ; $Date: 91/10/24 11:59:09 $	*/

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

#define TWOPI 2.0*PI


/*********************** self documentation **********************/
String sdoc = "\
SUVIBRO - Generates a Vibroseis sweep (linear FM sweep)		\n\
								\n\
suvibro [optional parameters] > out_data_file  			\n\
								\n\
Optional Parameters:						\n\
	f1=10.0		sweep frequency at start		\n\
	f2=60.0		sweep frequency at end			\n\
	tv=14.0		sweep length				\n\
	t1=2.0		length of taper at start (see note)	\n\
	t2=2.0		length of taper at end (see note)	\n\
	dt=0.004	sampling interval			\n\
								\n\
Note: The default tapers are cosine envelopes.  To eliminate the\n\
      taper, choose t1=t2=0.0.					\n\
                                                                \n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Michel
 */


segy tr;

main(int argc, char **argv)
{
	float f1, f2;
	float tv;
	float t1, t2;
	float dt;
	float ak;
	int nt;
	int mt1, mt2, mt3;


	/* Initialize */
	initargs(argc, argv);
	askdoc(0);
	

	/* set parameters and fill header fields */
	if (!getparfloat("f1", &f1))	f1 = 10.0;
	if (!getparfloat("f2", &f2))	f2 = 60.0;
	if (!getparfloat("tv", &tv))	tv = 14.0;
	if (!getparfloat("t1", &t1))	t1 = 2.0;
	if (!getparfloat("t2", &t2))	t2 = 2.0;
	if (!getparfloat("dt", &dt))	dt = 0.004; tr.dt = dt*1000000.0;

	
	if ((nt = tv/dt + 1) >= SU_NFLTS) err("nt=tv/dt=%d -- too big", nt);
	if (t1 + t2 > tv) {
		warn("t1+t2 > tv -- default values of t1 and t2 are used");
		t1 = 2.0;
		t2 = 2.0;
	}
	
	
	tr.ns = nt;
	tr.tracl = 1;
	tr.sfs = f1;
	tr.sfe = f2;
	tr.slen = 1000.0 * tv;
	tr.styp = 1; /* linear sweep id code */

	
	ak = (f2 - f1)/tv;
	mt1 = t1/dt + 1;
	mt2 = t2/dt + 1;
	mt3 = nt - mt2;
	
	if (f1 < f2) warn("%f, seconds, %f-%f Hz Up-Sweep\n", tv, f1, f2);
	else         warn("%f, seconds, %f-%f Hz Down-Sweep\n", tv, f1, f2);

	/* Tapering function (cosine bells) */
	{ register int it;
	  
	  for (it = 0; it < nt; ++it) {
	  	float am1, am2;
		float wt, tvb;
		
		am1 = 0.0;
		am2 = 0.0;
		if (mt1 > 0) am1 = (float) (mt1 - it) / (float) (mt1+1);
		if (mt2 > 0) am2 = (float) (it - mt3) / (float) mt2;
	
		if (it < mt1)
			wt = (1.0 + cos(PI*am1))/2.0;
		else if (it > mt3)
			wt = (1.0 + cos(PI*am2))/2.0;
		else wt = 1.0;
 
		
		/* Vibroseis signal */
		tvb = it*dt;
		tr.data[it] = wt * sin(TWOPI*(f1 + ak*tvb/2.0)*tvb);

	  }
	}

	puttr(&tr);
	
	return EXIT_SUCCESS;
}
