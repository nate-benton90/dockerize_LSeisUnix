/* SUGAIN: $Revision: 1.14 $ ; $Date: 90/12/22 08:14:32 $		*/

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
#include "subc.h"

/*********************** self documentation *****************************/
string sdoc =
" 									\n"
" SUGAIN - apply various types of gain to display traces		\n"
" 									\n"
" sugain <stdin >stdout [optional parameters]				\n"
" 							        	\n"
" Required parameters:							\n"
" 	none (no-op)							\n"
" 							        	\n"
" Optional parameters: 							\n"
" 	tpow=0.0	multiply data by t^tpow		    		\n"
" 	epow=0.0	multiply data by exp(epow*t)	       		\n"
" 	gpow=1.0	take signed gpowth root of scaled data		\n"
" 	agc=0		flag; 1 = do automatic gain control		\n"
" 	gagc=0		flag; 1 = ... with gaussian taper		\n"
" 	wagc=0.5	agc window in seconds (use if agc=1 or gagc=1)	\n"
" 	trap=0.0	zero any value whose magnitude exceeds trap 	\n"
" 	clip=0.0	clip any value whose magnitude exceeds clip	\n"
" 	qclip=1.0	clip by quantile on absolute values on trace	\n"
" 	qbal=0		flag; 1 = balance traces by qclip and scale	\n"
" 	pbal=0		flag; 1 = bal traces by dividing by rms	value	\n"
" 	jon=0		flag; 1 means tpow=2, gpow=.5, qclip=.95	\n"
" 	t=0.0           time (in ms) where scale is to be applied	\n" 
" 	scale=1.0	scale applied at t      			\n"
"                   (e.g., t=0.,1000,4000,6000 scale=1.,1.5,1.7,1.0)\n"
"   dbscale=    scale applied at t in db (if given, dbscale overwrites scale \n"
"   tshiftkey=  segy key word to be used to shift the trace before apply \n"
"               t-scale or t-dbscale (positive number shift up) \n"
"   hdbyte=     starting trace header byte location of the Scale \n"
"   hdtype=     scale type: -1=2-byte int; 0=4-byte int; 1=4-byte flt\n"
" 							        	\n"
" Operation order:							\n"
" 							        	\n"
" out(t) = scale(t)*BAL{CLIP[AGC{[t^tpow * exp(epow * t) * in(t)]^gpow}]}\n"
"          *Scale(hdbyte) \n"
" 							        	\n"
" Notes:								\n"
" 	The jon flag selects the parameter choices discussed in		\n"
" 	Claerbout's Imaging the Earth, pp 233-236.   			\n"
" 	Selected traces can be marked as not to be gained by		\n"
" 	using sumark to set the tr.mark header field to 1.		\n"
" 							        	\n"
;
/**************** end self doc *******************************************/

/* Credits:
 *	SEP: Jon
 *	CWP: Jack, Brian, Dave
 *            : Zhiming Li (add time varying scale)
 *
 * Technical Reference:
 *	Jon's second book, pages 233-236.
 */

/* subroutine prototypes */
void do_tpow(float tpow, register float tmin, register float dt, int nt);
void do_epow(float epow, register float tmin, register float dt, int nt);
void do_trap(register float trap, register int nt);
void do_clip(register float clip, register int nt);
void do_qclip(float qclip, int nt);
void do_qbal(float qclip, int nt);
void do_agc(int iwagc, int nt);
void do_gagc(int iwagc, int nt);
float quant(float *a, int k, int n);


#define	TPOW     0.0
#define	EPOW     0.0
#define	GPOW     1.0
#define	TRAP     0.0
#define	CLIP     0.0
#define	QCLIP    1.0
#define	SCALE    1.0
#define	WAGC     0.5
#define	AGC      0
#define	GAGC     0
#define	PBAL 	 0
#define	QBAL 	 0
#define	JON 	 0


segy tr;


main(int argc, char **argv)
{
	int jon;	/* flag to get Claerbout values			*/
	int agc;	/* agc flag					*/
	int gagc;	/* gaussian agc flag				*/
	int pbal;	/* power balance flag				*/
	int qbal;	/* quantile balance flag			*/
	float tpow;	/* exponent of t 				*/
	float epow;	/* deattenutation coefficient			*/
	float gpow;	/* dynamic compression power			*/
	float rmsq;	/* root mean square of a trace			*/
	float trap;	/* zero any larger value magnitude than trapval	*/
	float clip;	/* clip any larger value magnitude than clipval	*/
	float qclip;	/* clip at qth quantile (100qth percentile)	*/
	float wagc;	/* size of agc window in seconds		*/
	int iwagc;	/* ... half window in samples			*/
	int nt;		/* number of samples on trace			*/
	float tmin;	/* delay recording time in secs			*/
	float dt;	/* sample rate in secs				*/
	float *scale;	/* scale factor					*/
	String tshiftkey, tstype;
	Value tsval;
	int indxts, its=1, itshift;
	float *t;	/* time to apply scale 				*/
	int nscale; 	/* number of t,scale pairs			*/
	float *scalet;   /* scale at every time sample 			*/
	int itmp, one=1, it;
	float tmp;
	FILE *infp=stdin, *outfp=stdout;
	int hdbyte, hdrtype;
	int itmp4;
	short itmp2;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);
	
	/* large file extension */
	file64(infp);
	file64(outfp);	

	/* Get nt from first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt   = (int) tr.ns;
	dt   = (float) tr.dt/1000000.0;   /* microsecs to secs */
	tmin = (float) tr.delrt/1000.0;   /* millisecs to secs */
	if (!dt) getparfloat("dt", &dt);
	if (!dt) MUSTFGETPAR("dt", &dt);


	/* Get parameters */
	if (!getparfloat ("tpow" , &tpow))	tpow     = TPOW;
	if (!getparfloat ("epow" , &epow))	epow     = EPOW;
	if (!getparfloat ("gpow" , &gpow))	gpow     = GPOW;
	if (!getparfloat ("trap" , &trap))	trap     = TRAP;
	if (!getparfloat ("clip" , &clip))	clip     = CLIP;
	if (!getparfloat ("qclip", &qclip))	qclip    = QCLIP;
	if (!getparfloat ("wagc" , &wagc))	wagc     = WAGC;
	if (!getparint   ("agc"  , &agc))	agc      = AGC;
	if (!getparint   ("gagc" , &gagc))	gagc     = GAGC;
	if (!getparint   ("pbal" , &pbal))	pbal 	 = PBAL;
	if (!getparint   ("qbal" , &qbal))	qbal 	 = QBAL;
	if (!getparint   ("jon"  , &jon))	jon 	 = JON;
	if (!getparstring   ("tshiftkey"  , &tshiftkey))	its 	 = 0;
	if(its==1) {
		tstype = hdtype(tshiftkey);
		indxts = getindex(tshiftkey);
	}

	nscale = countparval("t");
	itmp = countparval("dbscale");
	if(itmp==0) itmp = countparval("scale");
	if(itmp!=nscale) err("check t and (db)scale ");
	if(nscale>0) {
		scale = (float*) malloc(nscale*sizeof(float));
		t = (float*) malloc(nscale*sizeof(float));
		getparfloat("t", t);
		scalet = (float*) malloc(nt*sizeof(float));	
		for(it=0;it<nscale;it++) t[it] = t[it] * 0.001;
		if(!getparfloat("dbscale", scale)) getparfloat("scale", scale);
		for(it=0;it<nt;it++) {
			if(nscale==1) {
				scalet[it] = scale[0];
			} else {
				if(its==0) {
					tmp = tmin + it*dt;
				} else {
					tmp = it*dt;
				}
				lin1d_(t,scale,&nscale,&tmp,
					scalet+it,&one,&itmp);
			}
		}
		if(getparfloat("dbscale", scale)) {
			for(it=0;it<nt;it++) {
				tmp = scalet[it] / 20.;
				scalet[it] = pow(10.,tmp);
			}
		}
	}

	if (getparint("hdbyte",&hdbyte) && getparint("hdtype",&hdrtype)) {
		if (hdbyte<0 || hdbyte > 240) err("check hdbyte");
		if (hdrtype!=-1 && hdrtype !=0 && hdrtype!=1 ) err("check hdtype");
	} else {
		hdbyte = -1;
	}

	/* Data validation */
	if (trap < 0.0) err("trap = %f, must be positive", trap);
	if (clip < 0.0) err("clip = %f, must be positive", clip);
	if (qclip < 0.0 || qclip > 1.0) 
		err("qclip = %f, must be between 0 and 1", qclip);
	if (agc || gagc) {
		iwagc = NINT(wagc/dt);
		if (iwagc < 1) err("wagc=%g must be positive", wagc);
		if (iwagc > nt) err("wagc=%g too long for trace", wagc);
		iwagc >>= 1;  /* windows are symmetric, so work with half */
	}
	if (jon) { 
		tpow  = 2.0;
		gpow  = 0.5;
		qclip = 0.95;
	}


	/* Main loop over traces */
	do {
		if (!tr.mark) {
			if (!CLOSETO(tpow, 0.0)) {
				do_tpow(tpow, tmin, dt, nt);
			}
			if (!CLOSETO(epow, 0.0)) {
				do_epow(epow, tmin, dt, nt);
			}
			if (!CLOSETO(gpow, 1.0)) {
				register int i;
				register float val;

				if (CLOSETO(gpow, 0.5)) {
					for (i = 0; i < nt; ++i) {
						val = tr.data[i];
						tr.data[i] = (val >= 0.0) ?
							sqrt(val) : -sqrt(-val);
					}
				} else if (CLOSETO(gpow, 2.0)) {
					for (i = 0; i < nt; ++i) {
						val = tr.data[i];
						tr.data[i] = val * ABS(val);
					}
				} else {
					for (i = 0; i < nt; ++i) {
						val = tr.data[i];
						tr.data[i] = (val >= 0.0) ?
							 pow(val, gpow) :
							-pow(-val, gpow);
					}
				}
			}
			if (agc)		   do_agc(iwagc, nt);
			if (gagc)		   do_gagc(iwagc, nt);
			if (trap > 0.0)		   do_trap(trap, nt);
			if (clip > 0.0)		   do_clip(clip, nt);
			if (qclip < 1.0 && !qbal)  do_qclip(qclip, nt);
			if (qbal)		   do_qbal(qclip, nt);
			if (pbal) {
				register int i;
				register float val;

				/* rmsq = sqrt (SUM( a()*a() ) / nt) */
				rmsq = 0.0;
				for (i = 0; i < nt; ++i) {
					val = tr.data[i];
					rmsq += val * val;
				}
				rmsq = sqrt(rmsq / nt);

				if (!CLOSETO(rmsq, 0.0)) {
					for (i = 0; i < nt; ++i)
						tr.data[i] /= rmsq;
				}
			}
			if (nscale>0) {
				if(its==1) {
					gethval(&tr, indxts, &tsval);
					itshift = vtoi(tstype,tsval);
					fprintf(stderr,"itshift=%d scale=%g %g\n",
						itshift,scalet[0],scalet[nt-1]);
					for (it=0;it<nt;++it) {
						itmp = it + (tr.delrt - itshift)*1000/tr.dt;
						if(itmp<0) {
							tmp = scalet[0];
						} else if(itmp>nt-1) {
							tmp = scalet[nt-1];
						} else {
							tmp = scalet[itmp];
						}
						tr.data[it] *= tmp;
					}
				} else {
					for (it=0;it< nt;++it)  tr.data[it] *= scalet[it];
				}
			}
			if (hdbyte!=-1) {
				if(hdrtype==-1) {
					bcopy((char*)&tr+hdbyte-1,(char*)&itmp2,2);
					tmp = itmp2;
				} else if (hdrtype==0) {
					bcopy((char*)&tr+hdbyte-1,(char*)&itmp4,4);
					tmp = itmp4;
				} else if (hdrtype==1) { 
					bcopy((char*)&tr+hdbyte-1,(char*)&tmp,4);
				}
				for (it=0;it<nt;++it) tr.data[it] *= tmp;
			}
		}
		puttr(&tr);

	} while(gettr(&tr));


	return EXIT_SUCCESS;
}


/* Multiply by t^tpow */
void do_tpow(
	float tpow,		/* multiply data by t^tpow	*/
	register float tmin,	/* first time on record		*/
	register float dt,	/* sampling rate in seconds	*/
	int nt			/* number of samples		*/
)
{
	static bool first = true;	/* first entry flag	*/
	static float *tpowfac;		/* tpow values		*/
	register int i;			/* counter		*/

	if (first) { /* first entry, set up array of tpow factors */
		register float t;

		tpowfac = ealloc1float(nt);
		if(tpow==0.) {
			for (i = 0; i < nt; ++i)  tpowfac[i] = 1.;
		} else {
			for (i = 0; i < nt; ++i)  {
				t = tmin + i*dt;
				tpowfac[i] = (t!=0.)? pow(fabs(t),tpow): 0.;
			/*
			tpowfac[i] = (t >= 0.0) ? pow(t, tpow) : -pow(-t, tpow);
			*/
			}
		}
		first = false;
	} /* end first entry */

	for (i = 0; i < nt; ++i)  tr.data[i] *= tpowfac[i];
}


/* Exponential deattenuation  with deattenuation factor epow */
void do_epow(
	float epow,		/* coefficient of t in exponent	*/
	register float tmin,	/* first time on record		*/
	register float dt,	/* sampling rate in seconds	*/
	int nt			/* number of samples		*/
)
{
	register int i;			/* counter		*/
	static bool first = true;	/* first entry flag	*/
	static float *epowfac;		/* exponent stretchs	*/

	if (first) {
		epowfac = ealloc1float(nt);
		for (i = 0; i < nt; i++) 
			epowfac[i] = exp(epow * (tmin + i * dt));

		first = false;
	}

	for (i = 0; i < nt; ++i)  tr.data[i] *= epowfac[i];
}


/* Zero out outliers */
void do_trap(
	register float trap,	/* zero if magnitude > trap	*/
	register int nt		/* number of samples		*/
)
{
	int i;

	for(i=0;i<nt;i++) {
		if (ABS(tr.data[i]) > trap) tr.data[i] = 0.0;
	}

}


/* Hard clip outliers */
void do_clip(
	register float clip,	/* hard clip if magnitude > clip	*/
	register int nt		/* number of samples			*/
)
{
	int i;
	for(i=0;i<nt;i++) {
		if(tr.data[i]>clip) { 
			tr.data[i] = clip;
		} else if(tr.data[i]<-clip) {
			tr.data[i] = -clip;
		}
	}
}


/* Quantile clip on magnitudes of trace values */
void do_qclip(
	float qclip,	/* quantile at which to clip	*/
	int nt		/* number of sample points	*/
)
{
	register int i;
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float clip;			/* ... value of rank[iq]	*/

	if (first) {
		absdata = ealloc1float(nt);
 		iq = (int) (qclip * nt - 0.5); /* round, don't truncate */
		first = false;
	}

	/* Clip on value corresponding to qth quantile */
	for (i = 0; i < nt; ++i)  absdata[i] = ABS(tr.data[i]);
 	clip = quant(absdata, iq, nt);
	do_clip(clip, nt);
}


/* Quantile balance */
void do_qbal(
	float qclip,	/* quantile at which to clip	*/
	int nt		/* number of sample points	*/
)
{
	register int i;
	register float *dataptr = tr.data;	/* ptr to trace data	*/
	static bool first = true;	/* first entry flag		*/
	static float *absdata;		/* absolute value trace		*/
	static int iq;			/* index of qclipth quantile	*/
	float bal;			/* value used to balance trace	*/

	if (CLOSETO(qclip, 1.0)) { /* balance by max magnitude on trace */
		bal = ABS(tr.data[0]);
		for (i = 1; i < nt; ++i)  bal = MAX(bal, tr.data[i]);

		if (CLOSETO(bal, 0.0)) {
			return;
		} else {
			for (i = 1; i < nt; ++i)  tr.data[i] /= bal;
			return;
		}
	} else if (first) {
		absdata = ealloc1float(nt);
 		iq = (int) (qclip * nt - 0.5); /* round, don't truncate */
		first = false;
	}

	/* Balance by quantile value (qclip < 1.0) */
	for (i = 0; i < nt; ++i)  absdata[i] = ABS(tr.data[i]);
	bal = quant(absdata, iq, nt);

	if (CLOSETO(bal, 0.0)) {
		return;
	} else {
		for (i = 1; i < nt; ++i)  tr.data[i] /= bal;
		do_clip(1.0, nt);
		return;
	}
}


/* Automatic Gain Control--standard box */
void do_agc(int iwagc, int nt)
{
	static bool first = true;
	static float *agcdata;
	register int i;
	double val;
	double sum;
	register int nwin;
	register float rms;


	/* allocate room for agc'd data */
	if (first) {
		first = false;
		agcdata = ealloc1float(nt);
	}


	/* compute initial window for first datum */
	sum = 0.0;
	for (i = 0; i <= iwagc; ++i) {
		val = tr.data[i];
		sum += val*val;
	}
	nwin = iwagc + 1;
	rms = sum/nwin;
	if(rms>0.) {
		agcdata[0] = tr.data[0]/sqrt(rms);
	} else {
		agcdata[0] = 0.;
	}

	/* ramping on */
	for (i = 1; i <= iwagc; ++i) {
		val = tr.data[i+iwagc];
		sum += val*val;
		++nwin;
		rms = sum/nwin;
		if(rms>0.) {
			agcdata[i] = tr.data[i]/sqrt(rms);
		} else {
			agcdata[i] = 0.;
		}
	}

	/* middle range -- full rms window */
	for (i = iwagc + 1; i <= nt - iwagc; ++i) {
		val = tr.data[i+iwagc];
		sum += val*val;
		val = tr.data[i-iwagc-1];
		sum -= val*val;
		rms = sum/nwin;
		if(rms>0.) {
			agcdata[i] = tr.data[i]/sqrt(rms);
		} else {
			agcdata[i] = 0.;
		}
	}

	/* ramping off */
	for (i = nt - iwagc + 1; i < nt; ++i) {
		val = tr.data[i-iwagc-1];
		sum -= val*val;
		--nwin;
		rms = sum/nwin;
		if(rms>0.) {
			agcdata[i] = tr.data[i]/sqrt(rms);
		} else {
			agcdata[i] = 0.;
		}
	}

	/* copy data back into trace */
	memcpy((char*)tr.data, (char*)agcdata, nt*FSIZE);

	return;
}


#define EPS	3.8090232	/* exp(-EPS*EPS) = 5e-7, "noise" level	*/

/* Automatic Gain Control--gaussian taper */
void do_gagc(int iwagc, int nt)
{
	static bool first=true;	/* first entry flag			*/
	static float *agcdata;	/* agc'd data				*/
	static float *w;	/* Gaussian window weights		*/
	static float *d2;	/* square of input data			*/
	static float *s;	/* weighted sum of squares of the data	*/
	float u;		/* related to reciprocal of std dev	*/
	float usq;		/* u*u					*/


	if (first) {
		first = false;

		/* Allocate room for agc'd data */
		agcdata = ealloc1float(nt);

		/* Allocate and compute Gaussian window weights */
		w = ealloc1float(iwagc);  /* recall iwagc is HALF window */
		u = EPS / ((float) iwagc);
		usq = u*u;
		{
			register int i;
			float floati;

			for (i = 1; i < iwagc; ++i) {
				floati = (float) i;
				w[i] = exp(-(usq*floati*floati));
			}
		}

		/* Allocate sum of squares and weighted sum of squares */
		d2 = ealloc1float(nt);
		s  = ealloc1float(nt);
	}


	/* Agc the trace */
	{
		register int i, j, k;
		register float val;
		register float wtmp;
		register float stmp;

		/* Put sum of squares of data in d2 and
		/* initialize s to d2 to get center point set */
		for (i = 0; i < nt; ++i) {
			val = tr.data[i];
			s[i] = d2[i] = val * val;
		}

		/* Compute weighted sum s; use symmetry of Gaussian */
		for (j = 1; j < iwagc; ++j) {
			wtmp = w[j];
			for (i = j; i < nt; ++i)  s[i] += wtmp*d2[i-j]; 
			k = nt - j;
			for (i = 0; i < k; ++i)   s[i] += wtmp*d2[i+j]; 
		}

		for (i = 0; i < nt; ++i) {
			stmp = s[i];
			agcdata[i] = (CLOSETO(stmp, 0.0) ?
					0.0 : tr.data[i]/sqrt(stmp));
		}

		/* Copy data back into trace */
		memcpy((char*)tr.data, (char*)agcdata, nt*FSIZE);
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

float quant(float *a, int k, int n)
{
	register int i, j;
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
