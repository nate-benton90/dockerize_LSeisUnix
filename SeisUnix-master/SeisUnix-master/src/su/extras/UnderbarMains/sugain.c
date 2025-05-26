/* SUGAIN: $Revision: 2.10 $ ; $Date: 89/09/20 19:35:36 $		*/

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

/*********************** self documentation *****************************/
string sdoc = "\
									\n\
SUGAIN - apply various types of gain to display traces 			\n\
									\n\
sugain <stdin >stdout [optional parameters]				\n\
							        	\n\
Required parameters:							\n\
	none (no-op)							\n\
							        	\n\
Optional parameters: 							\n\
	tpow = 0.0	multiply data by t^tpow		    		\n\
	epow = 0.0	multiply data by exp(epow*t)	       		\n\
	gpow = 1.0	take signed gpowth root of scaled data		\n\
	agc  = 0        flag; 1 = do automatic gain control		\n\
	wagc = 20	agc window in samples (use if agc=1)		\n\
	trap = 0.0	zero any value whose magnitude exceeds 		\n\
			trapval					 	\n\
	clip = 0.0	clip any value whose magnitude exceeds 		\n\
			clipval 					\n\
	qclip = 1.0	clip by quantile on absolute values on 		\n\
			trace						\n\
	qbal = 0	flag; 1 = balance traces by qclip and 		\n\
			scale						\n\
	pbal = 0	flag; 1 = bal traces by dividing by rms		\n\
			value						\n\
	scale = 1.0	multiply data by overall scale factor       	\n\
	jon = 0		flag; 1 means tpow=2, gpow=.5, qclip=.95	\n\
							        	\n\
Operation order:							\n\
							        	\n\
out(t) = scale * BAL{CLIP[AGC{[t^tpow * exp(epow * t) * in(t)]^gpow}]}	\n\
							        	\n\
Notes:									\n\
	The jon flag selects the parameter choices discussed in		\n\
	Claerbout's Imaging the Earth, pp 233-236.   			\n\
	Selected traces can be marked as not to be gained by		\n\
	using (e.g.) suwind to set the tr.mark header field to 1.	\n\
							        	\n\
";
/*************************************************************************/

/* Credits:
 *	SEP: Jon
 *	CWP: Jack
 *
 * Technical Reference:
 *	Jon's second book, pages 233-236.
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/src/RCS/sugain.c,v $";
static string revid =
	"   $Revision: 2.10 $ ; $Date: 89/09/20 19:35:36 $";




segy tr;

char tmpagcin[L_tmpnam];	/* tmp file for input to suagc	*/
char tmpagcout[L_tmpnam];	/* tmp file for output to suagc	*/


main(argc, argv)
int argc; char **argv;
{


	int jon;	/* flag to get Claerbout values			*/
	int agc;	/* agc flag					*/
	int pbal;	/* power balance flag				*/
	int qbal;	/* quantile balance flag			*/
	float tpow;	/* exponent of t 				*/
	float epow;	/* deattenutation coefficient			*/
	float gpow;	/* dynamic compression power			*/
	float rmsq;	/* root mean square of a trace			*/
	float trap;	/* zero any larger value magnitude than trapval	*/
	float clip;	/* clip any larger value magnitude than clipval	*/
	float qclip;	/* clip at qth quantile (100qth percentile)	*/
	float scale;	/* overall scale factor				*/
	int wagc;	/* size of agc window in samples		*/
	int nt;		/* number of samples on trace			*/
	float tmin;	/* delay recording time in secs			*/
	float dt;	/* sample rate in secs				*/
	void do_tpow();	/* PARMS(flt tpow, flt tmin, flt dt, int nt)	*/
	void do_epow();	/* PARMS(flt epow, flt tmin, flt dt, int nt)	*/
	void do_agc();	/* PARMS(int wagc)				*/
	void do_trap();	/* PARMS(float trap, int nt)			*/
	void do_clip();	/* PARMS(float clip, int nt)			*/
	void do_qclip();/* PARMS(float qclip, int nt)			*/
	void do_qbal();	/* PARMS(float qclip, int nt)			*/
	float quant();	/* PARMS(float data, int iq, int nt)		*/


	/* Initialize SU */
	initargs(argc, argv);
	if (!igetpar("ID", &ID))	ID = 0;
	if (ID) {
		(void) fprintf(stderr, "%s\n", progid);
		(void) fprintf(stderr, "%s\n", revid);
	}
	askdoc(1);
	

	/* Get parameters */
	if (!fgetpar("tpow" , &tpow))		tpow     = 0.0;
	if (!fgetpar("epow" , &epow))		epow     = 0.0;
	if (!fgetpar("gpow" , &gpow))		gpow     = 1.0;
	if (!fgetpar("trap" , &trap))		trap     = 0.0;
	if (!fgetpar("clip" , &clip))		clip     = 0.0;
	if (!fgetpar("qclip", &qclip))		qclip    = 1.0;
	if (!fgetpar("scale", &scale))		scale    = 1.0;
	if (!igetpar("wagc" , &wagc))		wagc     = 20;
	if (!igetpar("agc"  , &agc))		agc      = 0;
	if (!igetpar("pbal" , &pbal))		pbal 	 = 0;
	if (!igetpar("qbal" , &qbal))		qbal 	 = 0;
	if (!igetpar("jon"  , &jon))		jon 	 = 0;

	/* Data validation */
	if (trap < 0.0) err("trap = %f, must be positive", trap);
	if (clip < 0.0) err("clip = %f, must be positive", clip);
	if (qclip < 0.0 || qclip > 1.0) 
		err("qclip = %f, must be between 0 and 1", qclip);
	if (wagc < 0) err("wagc = %d, must be positive", wagc);
	if (agc != 0 && agc != 1) err("agc = %d, flag must be 0 or 1", agc);
	if (jon != 0 && jon != 1) err("jon = %d, flag must be 0 or 1", jon);
	if (jon) { 
		tpow  = 2.0;
		gpow  = 0.5;
		qclip = 0.95;
	}

	/* Main loop over traces */
	while (gettr(&tr)) {
		nt   = (int) tr.ns;
		tmin = (float) tr.delrt/1000.0;   /* millisecs to secs */
		dt   = (float) tr.dt/1000000.0;   /* microsecs to secs */
		if (!tr.mark) {
			if (tpow != 0.0) {
				do_tpow(tpow, tmin, dt, nt);
			}
			if (epow != 0.0) {
				do_epow(epow, tmin, dt, nt);
			}
			if (gpow != 1.0) {
				if (gpow == 0.5) {
					vssqrt_(tr.data, ONE,
						tr.data, ONE, &nt);
				} else if (gpow == 2.0) {
					vssq_(tr.data, ONE, tr.data, ONE, &nt);
				} else {
					vspow_(tr.data, ONE, &gpow,
					       tr.data, ONE, &nt);
				}
			}
			if (agc) {
				do_agc(wagc);
			}
			if (trap > 0.0) {
				do_trap(trap, nt);
			}
			if (clip > 0.0) {
				do_clip(clip, nt);
			}
			if (qclip < 1.0 && !qbal) {
				do_qclip(qclip, nt);
			}
			if (qbal) {
				do_qbal(qclip, nt);
			}
			if (pbal) {
				rmvesq_(tr.data, ONE, &rmsq, &nt);
				if (rmsq) {
					vsdiv_(tr.data, ONE, &rmsq,
					       tr.data, ONE, &nt);
				}
			}
			if (scale != 1.0) {
				vsmul_(tr.data, ONE, &scale, tr.data, ONE, &nt);
			}
		}
		puttr(&tr);
	}


	return SUCCEED;
}


/* Multiply by t^tpow = exp(tpow * log t) */
void do_tpow(tpow, tmin, dt, nt)
float tpow;		/* multiply data by t^tpow	*/
register float tmin;	/* first time on record		*/
register float dt;	/* sampling rate in seconds	*/
int nt;			/* number of samples		*/
{
	static bool first = true;	/* first entry flag	*/
	static float *tpowfac;		/* tpow values		*/
	register int i;			/* counter		*/

	if (first) { /* first entry, set up array of tpow factors */
		tpowfac = vec(nt);

		/* First factor exceptional because the log argument is	*/
		/* zero in the common case where tmin = 0.0		*/
		tpowfac[0] = (tmin == 0.0 ? 1.0 : exp(tpow * log(tmin)));
		for (i = 1; i < nt; i++) {
			tpowfac[i] = exp(tpow * log(tmin + i * dt));
		}
		first = false;
	} /* end first entry */

	vmul_(tr.data, ONE, tpowfac, ONE, tr.data, ONE, &nt);
}


/* Exponential deattenuation  with deattenuation factor epow */
void do_epow(epow, tmin, dt, nt)
float epow;		/* coefficient of t in exponent	*/
register float tmin;	/* first time on record		*/
register float dt;	/* sampling rate in seconds	*/
int nt;			/* number of samples		*/
{
	register int i;			/* counter		*/
	static bool first = true;	/* first entry flag	*/
	static float *epowfac;		/* exponent stretchs	*/

	if (first) {
		epowfac = vec(nt);
		for (i = 0; i < nt; i++) {
			epowfac[i] = exp(epow * (tmin + i * dt));
		}
		first = false;
	}
	vmul_(tr.data, ONE, epowfac, ONE, tr.data, ONE, &nt);
}


/* Zero out outliers */
void do_trap(trap, nt)
register float trap;	/* zero if magnitude > trap	*/
register int nt;	/* number of samples		*/
{
	register float *dataptr = tr.data;

	while (nt--) {
		if (ABS(*dataptr) > trap) *dataptr = 0.0;
		dataptr++;
	}
}


/* Hard clip outliers */
void do_clip(clip, nt)
register float clip;	/* hard clip if magnitude > clip	*/
register int nt;	/* number of samples			*/
{
	register float *dataptr = tr.data;
	register float mclip = -clip;

	while (nt--) {
		if (*dataptr > clip) {
			*dataptr = clip;
		} else if (*dataptr < mclip) {
			*dataptr = mclip;
		}
		dataptr++;
	}
}


/* Quantile clip on magnitudes of trace values */
void do_qclip(qclip, nt)
float qclip;	/* quantile at which to clip	*/
int nt;		/* number of sample points	*/
{
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float clip;			/* ... value of rank[iq]	*/

	if (first) {
		absdata = vec(nt);
 		iq = (int) (qclip * nt - 0.5); /* round, don't truncate */
		first = false;
	}

	/* Clip on value corresponding to qth quantile */
	vabs_(dataptr, ONE, absdata, ONE, &nt); 
 	clip = quant(absdata, iq, nt);
	do_clip(clip, nt);
}


/* Quantile balance */
void do_qbal(qclip, nt)
float qclip;	/* quantile at which to clip	*/
int nt;		/* number of sample points	*/
{
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float bal;			/* value used to balance trace	*/
	int balloc;			/* index of bal (not used)	*/

	if (qclip == 1.0) { /* balance by max magnitude on trace */
		maxmgv_(tr.data, ONE, &bal, &balloc, &nt);
		if (bal == 0.0) {
			return;
		} else {
			vsdiv_(tr.data, ONE, &bal, tr.data, ONE, &nt);
			return;
		}
	} else if (first) {
		absdata = vec(nt);
 		iq = (int) (qclip * nt - 0.5); /* round, don't truncate */
		first = false;
	}

	/* Balance by quantile value (qclip < 1.0) */
	vabs_(dataptr, ONE, absdata, ONE, &nt); 
	bal = quant(absdata, iq, nt);
	if (bal == 0.0) {
		return;
	} else {
		vsdiv_(tr.data, ONE, &bal, tr.data, ONE, &nt);
		do_clip(1.0, nt);
		return;
	}
}



/* Automatic Gain Control--lateral to suagc */
void do_agc(wagc)	/* agc window in samples	*/
int wagc;
{
	char cmd[BUFSIZ];	/* build suagc command for system call	*/
	FILE *fpagcin;		/* fp for suagc input file		*/
	int fdagcin;		/* ... its file id			*/
	FILE *fpagcout;		/* fp for suagc output file		*/
	int fdagcout;		/* ... its file id			*/
	char *tmpnam();		/* system subroutine			*/


	/* Prepare temporary files to hold traces to be agc'd */
	tmpnam(tmpagcin);
	if (NULL == (fpagcin = fopen(tmpagcin, "w+"))) {
		syserr("fopen failed on suagc input tmp file");
	}
	fdagcin = fileno(fpagcin);

	tmpnam(tmpagcout);
	if (NULL == (fpagcout = fopen(tmpagcout, "w+"))) {
		syserr("fopen failed on suagc output tmp file");
	}
	fdagcout = fileno(fpagcout);

	fputtr(fdagcin, &tr);

	/* System call to suagc */
	sprintf(cmd, "suagc <%s >%s wagc=%d", tmpagcin, tmpagcout, wagc);
	system(cmd);

	fgettr(fdagcout, &tr);

	/* Clean up temp files */
	if (EOF == fclose(fpagcin)) {
		syserr("fclose failed on suagc input tmp file");
	}
	if (-1 == unlink(tmpagcin)) {
		syserr("unlink of suagc input tmp file failed");
	}
	if (EOF == fclose(fpagcout)) {
		syserr("fclose failed on suagc ouput tmp file");
	}
	if (-1 == unlink(tmpagcout)) {
		syserr("unlink of suagc ouput tmp file failed");
	}

	return;
}
