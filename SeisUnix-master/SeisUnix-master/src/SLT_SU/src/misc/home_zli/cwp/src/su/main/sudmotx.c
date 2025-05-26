/* SUDMOTX: $Revision: 1.6 $ ; $Date: 90/11/04 14:38:34 $		*/

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
"SUDMOTX - DMO via T-X domain (Kirchhoff) method for common-offset gathers\n"
"\n"
"sudmotx <stdin >stdout cdpmin= cdpmax= dxcdp= noffmix= [optional parms]\n"
"\n"
"Required Parameters:\n"
"cdpmin                  minimum cdp (integer number) for which to apply DMO\n"
"cdpmax                  maximum cdp (integer number) for which to apply DMO\n"
"dxcdp                   distance between successive cdps\n"
"noffmix                 number of offsets to mix (see notes)\n"
"\n"
"Optional Parameters:\n"
"offmax=3000.0           maximum offset\n"
"tmute=2.0               mute time at maximum offset offmax\n"
"vrms=1500.0             RMS velocity at mute time tmute\n"
"verbose=0               =1 for diagnostic print\n"
"\n"
"Notes:\n"
"Input traces should be sorted into common-offset gathers.  One common-\n"
"offset gather ends and another begins when the offset field of the trace\n"
"headers changes.\n"
"\n"
"The cdp field of the input trace headers must be the cdp bin NUMBER, NOT\n"
"the cdp location expressed in units of meters or feet.\n"
"\n"
"The number of offsets to mix (noffmix) should typically equal the ratio of\n"
"the shotpoint spacing to the cdp spacing.  This choice ensures that every\n"
"cdp will be represented in each offset mix.  Traces in each mix will\n"
"contribute through DMO to other traces in adjacent cdps within that mix.\n"
"\n"
"The defaults for offmax and vrms are appropriate only for metric units.\n"
"If distances are measured in feet, then these parameters should be\n"
"specified explicitly.\n"
"\n"
"offmax, tmute, and vrms need not be specified precisely.\n"
"If these values are unknown, then one should overestimate offmax\n"
"and underestimate tmute and vrms.\n"
"\n"
"No muting is actually performed.  The tmute parameter is used only to\n"
"determine parameters required to perform DMO.\n"
"\n"
"Trace header fields accessed:  nt, dt, delrt, offset, cdp.\n"
"\n";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Dave
 *
 * Technical Reference:
 *      A non-aliased integral method for dip-moveout
 *      Dave Hale
 *      submitted to Geophysics, June, 1990
 */

void maketa (float dx, float dt, float offmax, float tmute, float vrms,
	int nsmax, int *nsp, float *ts, float *as);
void makeds (int ns, float *ts, float *as, int lds, int ifds, float *ds);
void dmotx (int ns, float *ts, float *as, float offset, float x, float dx,
	int itmute, int nt, float dt, float ft, float *p, float *q);

segy tr,tro;

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int cdpmin;	/* minimum cdp to process */
	int cdpmax;	/* maximum cdp to process */
	float dxcdp;	/* cdp sampling interval */
	int noffmix;	/* number of offsets to mix */
	float offmax;	/* maximum offset */
	float tmute;	/* mute time at far offset */
	float vrms;	/* rms velocity at mute time */
	int nsmax;	/* maximum number of time shifts per trace in DMO */
	int ns;		/* actual number of time shifts per trace in DMO */
	float *p;	/* input trace */
	float **q;	/* output DMO-corrected traces */
	float *temp;	/* temporary array */
	float *ts;	/* table of time shifts for DMO */
	float *as;	/* table of amplitudes for DMO */
	float offset;	/* source-receiver offset of current trace */
	float oldoffset;/* offset of previous trace */
	int cdp;	/* cdp number of current trace */
	int ncdp;	/* number of cdps */
	int icdp;	/* cdp index */
	int jcdp;	/* cdp index */
	int jcdplo;	/* lower bound for jcdp */
	int jcdphi;	/* upper bound for jcdp */
	int ntrace;	/* number of traces processed in current mix */
	int itrace;	/* trace index */
	int noff;	/* number of offsets processed in current mix */
	int gottrace;	/* non-zero if an input trace was read */
	int done;	/* non-zero if done */
	float *ds;	/* shaping filter to complete DMO processing */
	int lds=125;	/* length of shaping filter */
	int ifds=-100;	/* time index of first sample in shaping filter */
	int verbose;	/* =1 for diagnostic print */
	FILE *hfp;	/* file pointer for temporary header file */

	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = (float)tr.delrt/1000.0;

	/* get parameters */
	if (!getparint("cdpmin",&cdpmin)) err("must specify cdpmin");
	if (!getparint("cdpmax",&cdpmax)) err("must specify cdpmax");
	if (cdpmin>cdpmax) err("cdpmin must be less than cdpmax");
	if (!getparfloat("dxcdp",&dxcdp)) err("must specify dxcdp");
	if (!getparint("noffmix",&noffmix)) err("must specify noffmix");
	if (!getparfloat("offmax",&offmax)) offmax=3000.0;
	if (!getparfloat("tmute",&tmute)) tmute=2.0;
	if (!getparfloat("vrms",&vrms)) vrms=1500.0;
	if (!getparint("nsmax",&nsmax)) nsmax=400;
	if (!getparint("verbose",&verbose)) verbose=0;
	
	/* determine number of cdps */
	ncdp = cdpmax-cdpmin+1;

	/* allocate workspace */
	q = ealloc2float(nt,ncdp);
	p = ealloc1float(nt);
	temp = ealloc1float(nt);
	ts = ealloc1float(nsmax);
	as = ealloc1float(nsmax);
	ds = ealloc1float(lds);
	
	/* tabulate time shifts and amplitudes for dmo */
	maketa(dxcdp,dt,offmax,tmute,vrms,nsmax,&ns,ts,as);
	if (verbose) 
		fprintf(stderr,"\tDMO will be performed via %d time shifts\n",
			ns);
	
	/* compute shaping filter for dmo horizontal reflection response */
	makeds(ns,ts,as,lds,ifds,ds);
	
	/* open temporary file for headers */
	/*hfp = tmpfile();*/
	hfp = etempfile(NULL);
	
	/* initialize */
	oldoffset = tr.offset;
	gottrace = 1;
	done = 0;
	ntrace = 0;
	noff = 0;
	for (icdp=0; icdp<ncdp; ++icdp)
		for (it=0; it<nt; ++it)
			q[icdp][it] = 0.0;

	/* loop over traces */
	do {
		
		/* if got a trace */
		if (gottrace) {

			/* determine offset and cdp */
			offset = tr.offset;
			cdp = tr.cdp;
		
			/* update number of offsets mixed */
			if (offset!=oldoffset) noff++;

			/* get trace samples */
			bcopy(tr.data,p,nt*sizeof(float));
		}
		
		/* if a mix of offsets is complete */
		if (noff==noffmix || !gottrace) {
			
			/* update number of offsets mixed */
			if (!gottrace) noff++; 
			
			/* apply shaping filter to complete dmo processing */
			for (icdp=0; icdp<ncdp; ++icdp) {
				conv(lds,ifds,ds,nt,0,q[icdp],nt,0,temp);
				bcopy(temp,q[icdp],nt*sizeof(float));
			}
			
			/* rewind trace header file */
			fseek(hfp,0L,SEEK_SET);
			
			/* loop over all output traces */
			for (itrace=0; itrace<ntrace; ++itrace) {
			
				/* read trace header and determine cdp index */
				efread(&tro,HDRBYTES,1,hfp);
				icdp = tro.cdp-cdpmin;
				
				/* get dmo-corrected data */
				bcopy(q[icdp],tro.data,nt*sizeof(float));
				
				/* write output trace */
				puttr(&tro);
			}
			
			/* report */
			if (verbose) 
				fprintf(stderr,"\tCompleted mix of "
					"%d offsets with %d traces\n",
					noff,ntrace);
			
			/* if no more traces, break */
			if (!gottrace) break;
			
			/* rewind trace header file */
			fseek(hfp,0L,SEEK_SET);
			
			/* reset number of offsets and traces */
			noff = 0;
			ntrace = 0;
			
			/* zero dmo accumulators */
			for (icdp=0; icdp<ncdp; ++icdp)
				for (it=0; it<nt; ++it)
					q[icdp][it] = 0.0;
		}
				
		/* if cdp is not within range of cdps to process, skip it */
		if (cdp<cdpmin || cdp>cdpmax) continue;
		
		/* save trace header and update number of traces */
		efwrite(&tr,HDRBYTES,1,hfp);
		ntrace++;
		
		/* determine output traces potentially modified by input */
		icdp = cdp-cdpmin;
		jcdplo = MAX(0,icdp-0.5*ABS(offset/dxcdp));
		jcdphi = MIN(ncdp-1,icdp+0.5*ABS(offset/dxcdp));
		
		/* loop over potentially modified output traces */
		for (jcdp=jcdplo; jcdp<=jcdphi; ++jcdp) {
		
			/* do dmo for one output trace */
			dmotx(ns,ts,as,offset,(jcdp-icdp)*dxcdp,dxcdp,
				0,nt,dt,ft,p,q[jcdp]);
		}

		/* remember offset */
		oldoffset = offset;

		/* get next trace (if there is one) */
		if (!gettr(&tr)) gottrace = 0;
		
	} while (!done);

	return EXIT_SUCCESS;
}

void maketa (float dx, float dt, float offmax, float tmute, float vrms,
	int nsmax, int *ns, float *ts, float *as)
/*****************************************************************************
make time shifts and amplitudes for non-aliased (t,x) domain DMO
******************************************************************************
Input:
dx		midpoint sampling interval (see notes)
dt		time sampling interval (see notes)
offmax		maximum offset (see notes)
tmute		mute time at maximum offset (see notes)
vrms		RMS velocity at mute time (see notes)
nsmax		maximum number of time shifts

Output:
ns		number of shifts
ts		array[ns] of time shifts (normalized by sampling interval dt)
as		array[ns] of amplitudes corresponding to time shifts
******************************************************************************
Notes:
dx, dt, offmax, tmute, and vrms must be greater than 0.0.

dx, dt, offmax, tmute, and vrms need not be specified precisely.
If these values are unknown, then one should overestimate dt and
offmax, and underestimate dx, tmute, and vrms.

The time shifts, ts, are computed to ensure that the DMO operator
is not aliased in time or space.

The number of time shifts, ns, is determined such that the steepest
reflection slope at the mute time (tmute) for the largest offset
is properly handled.  A typical value for ns is about ns=200, so
a reasonable value for nsmax is nsmax=400, just to be safe.  The
computation cost for (t,x) domain DMO is linearly proportional to ns.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/04/90
*****************************************************************************/
{
	int nsi,is,itaper,ltaper;
	float alpha,dalpha,dalphan,tsmax;
	
	/* ensure positive arguments */
	dx = ABS(dx);
	dt = ABS(dt);
	offmax = ABS(offmax);
	tmute = ABS(tmute);
	vrms = ABS(vrms);
	
	/* determine nominal increment in alpha = sqrt(ts) */
	dalphan = MIN(sqrt(2.0*(tmute/dt)*pow((dx/offmax),2.0)),1.0);
	
	/* determine maximum time shift for steepest reflection slope */
	tsmax = (tmute/dt)*(1.0-1.0/sqrt(1.0+pow(offmax/(vrms*tmute),2.0)));
	
	/* recursively compute time shifts and amplitudes */
	alpha = 0.0;
	dalpha = dalphan;
	for (nsi=0; alpha*alpha<=tsmax && nsi<nsmax; ++nsi) {
		ts[nsi] = alpha*alpha;
		as[nsi] = dalpha;
		alpha = alpha+dalpha;
		dalpha = sqrt(alpha*alpha+1.0)-alpha;
		if (dalpha>dalphan) dalpha = dalphan;
	}
	
	/* taper the amplitudes with a raised cosine */
	ltaper = nsi/3;
	for (is=nsi-ltaper,itaper=1; is<nsi; ++is,++itaper) 
		as[is] *= 0.54+0.46*cos(PI*itaper/ltaper);
	
	*ns = nsi;
}

void makeds (int ns, float *ts, float *as, int lds, int ifds, float *ds)
/*****************************************************************************
make shaping filter to correct DMO horizontal reflection response
******************************************************************************
Input:
ns		number of shifts
ts		array[ns] of time shifts (normalized by sampling interval)
as		array[ns] of amplitudes corresponding to time shifts
lds		length of shaping filter
ifds		index of first sample of shaping filter (see notes)

Output:
ds		array[lds] containing shaping filter
******************************************************************************
Notes:
Reasonable values for lds and ifds are lds=125 and ifds=-100.
The maximum permissible lds is the dimension of dd, di, and work below.

This function must be kept consistent with that used to perform DMO.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/04/90
*****************************************************************************/
#define LDMAX 400
#define LDSMAX 300
{
	int ld=LDMAX,ifd=1-ld,i,is;
	float one=1.0,d[LDMAX],dd[LDSMAX],di[LDSMAX],work[LDSMAX];
	
	/* compute dmo horizontal reflection response d(t) */
	for (i=0; i<ld; ++i)
		d[i] = 0.0;
	d[ld-1] = as[0];
	for (is=1; is<ns; ++is) {
		i = (int)ts[is];
		d[ld-i-1] += 2.0*as[is];
	}
	
	/* compute autocorrelation of d(t) */
	xcor(ld,ifd,d,ld,ifd,d,lds,0,dd);
	
	/* compute crosscorrelation of d(t) and desired impulse i(t) */
	xcor(ld,ifd,d,1,0,&one,lds,ifds,di);
	
	/* solve symmetric toeplitz system of equations for filter */
	stoepf(lds,dd,di,ds,work);
}

void dmotx (int ns, float *ts, float *as, float offset, float x, float dx,
	int itmute, int nt, float dt, float ft, float *p, float *q)
/*****************************************************************************
apply DMO in (t,x) domain for one input trace and one output trace
******************************************************************************
Input:
ns		number of shifts
ts		array[ns] of time shifts (normalized by sampling interval)
as		array[ns] of amplitudes corresponding to time shifts
offset		source-receiver offset of input trace
x		midpoint distance from input trace to output trace
dx		midpoint sampling interval
itmute		index of the first time sample that is not muted
nt		number of time samples
dt		time sampling interval
ft		first time sample
p		array[nt] containing input trace
q		array[nt] containing accumulated DMO output trace

Output:
q		array[nt] containing accumulated DMO output trace
******************************************************************************
Notes:
Time shifts and amplitudes (ts and as) must be computed by maketa().
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/04/90
*****************************************************************************/
{
	int it1,it2,its,it,is;
	float h,xb,ftodt,x1,x2,scale1,scale2,asi,tsi;
	
	/* compute half-offset */
	h = offset/2.0;
	
	/* round midpoint distance to nearest multiple of sampling interval */
	xb = ABS(NINT(x/dx)*dx);
	
	/* normalize the time of first sample */
	ftodt = ft/dt;
	
	/* if rounded midpoint distance is zero */
	if (xb==0.0) {
		
		/* compute midpoint distance at right edge of midpoint bin */
		x1 = xb+0.5*ABS(dx);
		
		/* compute scale factor used to determine index it1 below */
		scale1 = 1.0/(1.0-sqrt(MAX(0.0,1.0-x1*x1/MAX(h*h,x1*x1))));
		
		/* accumulate output for first (zero) time shift */
		asi = as[0];
		for (it=itmute; it<nt; ++it)
			q[it] += asi*p[it];
		
		/* loop over non-zero time shifts */
		for (is=1; is<ns; ++is) {
			tsi = ts[is];
			asi = as[is];
			
			/* round the normalized time shift to lower int */
			its = tsi;
			
			/* compute index of first time sample for this shift */
			it1 = scale1*tsi-ftodt;
			if (it1<itmute+its) it1 = itmute+its;
			
			/* if index is beyond last time sample, break */
			if (it1>=nt) break;
			
			/* accumulate shifted and weighted p(t) in q(t) */
			for (it=it1; it<nt; ++it)
				q[it-its] += 2.0*asi*p[it];
		}
	
	/* else if non-zero time shifts exist */
	} else if (ns>1) {
		
		/* compute midpoint distances at right and left edges of bin */
		x1 = xb+0.5*ABS(dx);
		x2 = xb-0.5*ABS(dx);
		
		/* if midpoint distance exceeds half-offset, return */
		if (x2*x2>h*h) return;
		
		/* compute scale factors for it1 and it2 below */
		scale1 = 1.0/(1.0-sqrt(MAX(0.0,1.0-x1*x1/MAX(h*h,x1*x1))));
		scale2 = 1.0/(1.0-sqrt(MAX(0.0,1.0-x2*x2/MAX(h*h,x2*x2))));
		
		/* loop over time shifts */
		for (is=1; is<ns; ++is) {
			tsi = ts[is];
			asi = as[is];
			
			/* round normalized time shift to lower int */
			its = tsi;
			
			/* compute indices of first and last samples */
			it1 = scale1*tsi-ftodt;
			if (it1<itmute+its) it1 = itmute+its;
			it2 = scale2*tsi-ftodt;
			if (it2>nt) it2 = nt;
			
			/* if first sample index is out of bounds, break */
			if (it1>=nt) break;
			
			/* compute contribution to q(t) for this shift */
			for (it=it1; it<it2; ++it)
				q[it-its] += asi*p[it];
		}
	}
}
