/* SUVELAN: $Revision: 1.1 $ ; $Date: 90/11/15 10:43:46 $		*/

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

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUVELAN - compute stacking velocity semblance for cdp gathers\n"
"\n"
"suvelan <stdin >stdout [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"nv=50                   number of velocities\n"
"dv=50.0                 velocity sampling interval\n"
"fv=1500.0               first velocity\n"
"smute=1.5               samples with NMO stretch exceeding smute are zeroed\n"
"dtratio=5               ratio of output to input time sampling intervals\n"
"nsmooth=dtratio*2+1     length of semblance num and den smoothing window\n"
"verbose=0               =1 for diagnostic print on stderr\n"
"\n"
"Notes:\n"
"Semblance is defined by the following quotient:\n"
"\n"
"                 n-1                 \n"
"               [ sum q(t,j) ]^2      \n"
"                 j=0                 \n"
"       s(t) = ------------------     \n"
"                 n-1                 \n"
"               n sum [q(t,j)]^2      \n"
"                 j=0                 \n"
"\n"
"where n is the number of non-zero samples after muting.\n"
"Smoothing (nsmooth) is applied separately to the numerator and denominator\n"
"before computing this semblance quotient.\n"
"\n"
"Input traces should be sorted by cdp - suvelan outputs a group of\n"
"semblance traces every time cdp changes.  Therefore, the output will\n"
"be useful only if cdp gathers are input.\n"
"\n"
"Trace header fields accessed:  ns, dt, delrt, offset, cdp.\n"
"Trace header fields modified:  ns, dt, offset.\n"
"\n";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Dave
 */

segy tr;

main(int argc, char **argv)
{
	int nv;		/* number of velocities */
	float dv;	/* velocity sampling interval */
	float fv;	/* first velocity */
	int iv;		/* velocity index */
	int dtratio;	/* ratio of output to input sampling intervals */
	int nsmooth;	/* length in samples of num and den smoothing window */
	int nt;		/* number of time samples per input trace */
	float dt;	/* time sampling interval for input traces */
	float ft;	/* time of first sample input and output */
	int ntout;	/* number of output samples */
	float dtout;	/* time sampling interval for output traces */
	int it;		/* input time sample index */
	int itout;	/* output time sample index */
	int is;		/* time sample index for smoothing window */
	int ismin;	/* lower limit on is */
	int ismax;	/* upper limit on is */
	int itmute;	/* time sample index of first sample not muted */
	int iti;	/* time sample index used in linear interpolation */
	float ti;	/* normalized time for linear interpolation */
	float frac;	/* fractional distance from sample in interpolation */
	int gottrace;	/* =1 if an input trace was read */
	int done;	/* =1 if done with everything */
	int verbose;	/* =1 for diagnostic print */
	long cdp;	/* cdp from current input trace header */
	long cdpprev;	/* cdp from previous input trace header */
	float smute;	/* NMO stretch mute factor */
	float offset;	/* offset from input trace header */
	float offovs;	/* (offset/velocity)^2 */
	float tn;	/* time after NMO */
	float tnmute;	/* mute time after NMO */
	float nsum;	/* semblance numerator sum */
	float dsum;	/* semblance denominator sum */
	float v;	/* velocity */
	float temp;	/* temporary scalar */
	float *data;	/* array[nt] of input trace */
	float *sem;	/* array[ntout] of semblance */
	float **num;	/* array[nv][nt] of semblance numerators */
	float **den;	/* array[nv][nt] of semblance denominators */
	float **nnz;	/* array[nv][nt] for counting non-zero samples */

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters from the first trace */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = tr.dt/1000000.0;
	ft = tr.delrt/1000.0;
	cdp = tr.cdp;
	offset = tr.offset;

	/* get optional parameters */
	if (!getparint("nv",&nv)) nv = 50;
	if (!getparfloat("dv",&dv)) dv = 50.0;
	if (!getparfloat("fv",&fv)) fv = 1500.0;
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=1.0) err("smute must be greater than 1.0");
	if (!getparint("dtratio",&dtratio)) dtratio = 5;
	if (!getparint("nsmooth",&nsmooth)) nsmooth = dtratio*2+1;
	if (!getparint("verbose",&verbose)) verbose = 0;

	/* determine output sampling */
	ntout = 1+(nt-1)/dtratio;
	dtout = dt*dtratio;
	if (verbose) {
		fprintf(stderr,
			"\tnumber of output time samples is %d\n",ntout);
		fprintf(stderr,
			"\toutput time sampling interval is %g\n",dtout);
		fprintf(stderr,
			"\toutput time of first sample is %g\n",ft);
	}

	/* allocate memory */
	data = ealloc1float(nt);
	num = ealloc2float(nt,nv);
	den = ealloc2float(nt,nv);
	nnz = ealloc2float(nt,nv);
	sem = ealloc1float(ntout);

	/* zero accumulators */
	for (iv=0; iv<nv; ++iv) {
		for (it=0; it<nt; ++it) {
			num[iv][it] = 0.0;
			den[iv][it] = 0.0;
			nnz[iv][it] = 0.0;
		}
	}

	/* initialize flags */
	gottrace = 1;
	done = 0;

	/* remember previous cdp */
	cdpprev = tr.cdp;

	/* loop over input traces */
	do {

		/* if got a trace */
		if (gottrace) {

			/* determine offset and cdp */
			offset = tr.offset;
			cdp = tr.cdp;

			/* get trace samples */
			bcopy(tr.data,data,nt*sizeof(float));
		}

		/* if cdp has changed or no more input traces */
		if (cdp!=cdpprev || !gottrace) {

			/* set output trace header fields */
			tr.offset = 0;
			tr.cdp = cdpprev;
			tr.ns = ntout;
			tr.dt = dtout*1000000.0;

			/* loop over velocities */
			for (iv=0; iv<nv; ++iv) {

				/* compute semblance quotients */
				for (itout=0; itout<ntout; ++itout) {
					it = itout*dtratio;
					ismin = it-nsmooth/2;
					ismax = it+nsmooth/2;
					if (ismin<0) ismin = 0;
					if (ismax>nt-1) ismax = nt-1;
					nsum = dsum = 0.0;
					for (is=ismin; is<ismax; ++is) {
						nsum += num[iv][is]*
							num[iv][is];
						dsum += nnz[iv][is]*
							den[iv][is];
					}
					sem[itout] = (dsum!=0.0?nsum/dsum:0.0);
				}

				/* output semblances */
				bcopy(sem,tr.data,ntout*sizeof(float));
				puttr(&tr);

				/* zero accumulators */
				for (it=0; it<nt; ++it) {
					num[iv][it] = 0.0;
					den[iv][it] = 0.0;
					nnz[iv][it] = 0.0;
				}
			}

			/* diagnostic print */
			if (verbose) 
				fprintf(stderr,
				"\tsemblance output for cdp=%d\n",cdpprev);

			/* if no more input traces, break input trace loop */
			if (!gottrace) break;

			/* remember previous cdp */
			cdpprev = cdp;
		}

		/* loop over velocities */
		for (iv=0,v=fv; iv<nv; ++iv,v+=dv) {
			
			/* compute offset/velocity squared */
			offovs = (offset*offset)/(v*v);

			/* determine mute time after nmo */
			tnmute = sqrt(offovs/(smute*smute-1.0));
			itmute = (tnmute-ft)/dt;
			
			/* do nmo via quick and dirty linear interpolation
			/* (accurate enough for velocity analysis) and
			/* accumulate semblance numerator and denominator */
			for (it=itmute,tn=ft+itmute*dt; it<nt; ++it,tn+=dt) {
				ti = (sqrt(tn*tn+offovs)-ft)/dt;
				iti = ti;
				if (iti<nt-1) {
					frac = ti-iti;
					temp = (1.0-frac)*data[iti]+
						frac*data[iti+1];
					if (temp!=0.0) {
						num[iv][it] += temp;
						den[iv][it] += temp*temp;
						nnz[iv][it] += 1.0;
					}
				}
			}
		}

		/* get next trace (if there is one) */
		if (!gettr(&tr)) gottrace = 0;

	} while (!done);

	return EXIT_SUCCESS;
}
