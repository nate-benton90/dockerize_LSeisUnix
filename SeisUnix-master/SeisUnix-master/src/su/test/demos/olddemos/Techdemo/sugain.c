/*********************** self documentation *****************************/
char *sdoc = "\
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
	trap = 0.0	zero any value whose magnitude exceeds trapval 	\n\
	clip = 0.0	clip any value whose magnitude exceeds clipval 	\n\
	qclip = 1.0	clip by quantile on absolute values on trace	\n\
	qbal = 0	flag; 1 = balance traces by qclip and scale	\n\
	pbal = 0	flag; 1 = bal traces by dividing by rms value	\n\
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

/* sugain - apply various types of gain to display traces
 *
 * Credits:
 *	SEP: Jon
 *	CWP: Jack
 *
 * Technical Reference:
 *	Jon's second book, pages 233-236.
 *
 *----------------------------------------------------------------------
 * Permission to use the SU programs is granted.  Courteous users will
 * retain this acknowlegment of origins at the Stanford Exploration
 * Project, Stanford University and the Center for Wave Phenomena, 
 * Colorado School of Mines.   Inquiries/bug reports should be sent to:
 * Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 * Golden, CO 80014
 *----------------------------------------------------------------------
 *
 * $Author: jkc $
 * $Source: /src/su/src/RCS/sugain.c,v $
 * $Revision: 2.2 $ ; $Date: 88/11/20 22:33:08 $
*/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/src/RCS/sugain.c,v $";
static char revid[] =
	"   $Revision: 2.2 $ ; $Date: 88/11/20 22:33:08 $";



#include "../include/cwp.h"
#include "../include/segy.h"
#include "../include/fconst.h"

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
	void dotpow();	/* PARMS(flt tpow, flt tmin, flt dt, int nt)	*/
	void doepow();	/* PARMS(flt epow, flt tmin, flt dt, int nt)	*/
	void doagc();	/* PARMS(int wagc)				*/
	void dotrap();	/* PARMS(float trap, int nt)			*/
	void doclip();	/* PARMS(float clip, int nt)			*/
	void doqclip();	/* PARMS(float qclip, int nt)			*/
	void doqbal();	/* PARMS(float qclip, int nt)			*/
	float quant();	/* PARMS(float data, float q, int nt)		*/


	initgetpar(argc, argv); askdoc(1);
	

	/* Default parameters;	User-defined overrides */
	tpow     = 0.0;		fgetpar("tpow" , &tpow);
	epow     = 0.0;		fgetpar("epow" , &epow);
	gpow     = 1.0;		fgetpar("gpow" , &gpow);
	trap     = 0.0;		fgetpar("trap" , &trap);
	clip     = 0.0;		fgetpar("clip" , &clip);
	qclip    = 1.0;		fgetpar("qclip", &qclip);
	scale    = 1.0;		fgetpar("scale", &scale);
	wagc     = 20;		igetpar("wagc" , &wagc);
	agc      = 0;		igetpar("agc"  , &agc);
	pbal 	 = 0;		igetpar("pbal" , &pbal);
	qbal 	 = 0;		igetpar("qbal" , &qbal);
	jon 	 = 0;		igetpar("jon"  , &jon);

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
				dotpow(tpow, tmin, dt, nt);
			}
			if (epow != 0.0) {
				doepow(epow, tmin, dt, nt);
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
				doagc(wagc);
			}
			if (trap > 0.0) {
				dotrap(trap, nt);
			}
			if (clip > 0.0) {
				doclip(clip, nt);
			}
			if (qclip < 1.0 && !qbal) {
				doqclip(qclip, nt);
			}
			if (qbal) {
				doqbal(qclip, nt);
			}
			if (pbal) {
				rmsqv_(tr.data, ONE, &rmsq, &nt);
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
	exit(0);
}


/* Multiply by t^tpow = exp(tpow * log t) */
void dotpow(tpow, tmin, dt, nt)
float tpow;		/* multiply data by t^tpow	*/
register float tmin;	/* first time on record		*/
register float dt;	/* sampling rate in seconds	*/
int nt;			/* number of samples		*/
{
	static bool first = true;	/* first entry flag	*/
	static float *tpowfac;		/* tpow values		*/
	register int i;			/* counter		*/

	if (first) { /* first entry, set up array of tpow factors */
		tpowfac = vector(nt);

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
void doepow(epow, tmin, dt, nt)
float epow;		/* coefficient of t in exponent	*/
register float tmin;	/* first time on record		*/
register float dt;	/* sampling rate in seconds	*/
int nt;			/* number of samples		*/
{
	register int i;			/* counter		*/
	static bool first = true;	/* first entry flag	*/
	static float *epowfac;		/* exponent stretchs	*/

	if (first) {
		epowfac = vector(nt);
		for (i = 0; i < nt; i++) {
			epowfac[i] = exp(epow * (tmin + i * dt));
		}
		first = false;
	}
	vmul_(tr.data, ONE, epowfac, ONE, tr.data, ONE, &nt);
}


/* Zero out outliers */
void dotrap(trap, nt)
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
void doclip(clip, nt)
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
void doqclip(qclip, nt)
float qclip;	/* quantile at which to clip	*/
int nt;		/* number of sample points	*/
{
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float clip;			/* ... value of rank[iq]	*/

	if (first) {
		absdata = vector(nt);
 		iq = (int) (qclip * nt - 0.5); /* round, don't truncate */
		first = false;
	}

	/* Clip on value corresponding to qth quantile */
	vabs_(dataptr, ONE, absdata, ONE, &nt); 
 	clip = quant(absdata, iq, nt);
	doclip(clip, nt);
}


/* Quantile balance */
void doqbal(qclip, nt)
float qclip;	/* quantile at which to clip	*/
int nt;		/* number of sample points	*/
{
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	float maxv[2];			/* maxv[0] holds clip		*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float bal;			/* value used to balance trace	*/

	if (qclip == 1.0) { /* balance by max magnitude on trace */
		maxmgv_(tr.data, ONE, maxv, &nt);
		bal = maxv[0];
		if (bal == 0.0) {
			return;
		} else {
			vsdiv_(tr.data, ONE, &bal, tr.data, ONE, &nt);
			return;
		}
	} else if (first) {
		absdata = vector(nt);
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
		doclip(1.0, nt);
		return;
	}
}



/* Automatic Gain Control--lateral to suagc */
void doagc(wagc)	/* agc window in samples	*/
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


/*
 * QUANT - find k/n th quantile of a[]
 *
 * Works by reordering a so a[j] < a[k] if j < k.
 *
 * Parameters:
 *    a		- data
 *    k		- indicates quantile
 *    n		- number of points in data
 *
 * This is Hoare's algorithm worked over by SEP (#10, p100) and Brian.
 */

float quant(a, k, n)
float *a;
int k, n;
{
	register int i,j;
	int low, hi;
	register float ak, aa;

	low = 0; hi = n-1;

	while (low < hi) {
		ak = a[k];
		i = low;
		j = hi;
		do {
			while (a[i] < ak) i++;
			while (a[j] > ak) j--;
			if (i <= j) {
				aa = a[i]; a[i] = a[j]; a[j] = aa;
				i++;
				j--;
			}
		} while (i <= j);

		if (j < k) low = i;

		if (k < i) hi = j;
	}

	return(a[k]);
}
