/* SUNMO: $Revision: 1.8 $ ; $Date: 90/12/07 13:35:44 $			*/

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
"SUNMO - NMO for an arbitrary velocity function of time and CDP\n"
"\n"
"sunmo <stdin >stdout [optional parameters]\n"
"\n"
"Optional Parameters:\n"
"vnmo=2000               NMO velocities corresponding to times in tnmo\n"
"tnmo=0                  NMO times corresponding to velocities in vnmo\n"
"cdp=0                   CDPs for which vnmo and tnmo are specified\n"
"smute=1.5               samples with NMO stretch exceeding smute are zeroed\n"
"lmute=25                length (in samples) of linear ramp for stretch mute\n"
"sscale=1                =1 to divide output samples by NMO stretch factor\n"
"invert=0                =1 to perform (approximate) inverse NMO\n"
"\n"
"Notes:\n"
"For constant-velocity NMO, specify only one vnmo=constant.\n"
"\n"
"For NMO with a velocity function of time only, specify the arrays\n"
"         vnmo=v1,v2,... tnmo=t1,t2,...\n"
"where v1 is the velocity at time t1, v2 is the velocity at time t2, ...\n"
"The times specified in the tnmo array must be monotonically increasing.\n"
"Linear interpolation and constant extrapolation of the specified velocities\n"
"is used to compute the velocities at times not specified.\n"
"\n"
"For NMO with a velocity function of time and CDP, specify the array\n"
"         cdp=cdp1,cdp2,...\n"
"and, for each CDP specified, specify the vnmo and tnmo arrays as described\n"
"above.  The first (vnmo,tnmo) pair corresponds to the first cdp, and so on.\n"
"Linear interpolation and constant extrapolation of 1/velocity^2 is used\n"
"to compute velocities at CDPs not specified.\n"
"\n"
"NMO interpolation error is less than 1% for frequencies less than 60% of\n"
"the Nyquist frequency.\n"
"\n"
"Exact inverse NMO is impossible, particularly for early times at large\n"
"offsets and for frequencies near Nyquist with large interpolation errors.\n"
"\n"
"Trace header fields accessed:  ns, dt, delrt, offset, cdp.\n"
"\n";
/**************** end self doc *******************************************/

/* Credits:
 *	SEP: Shuki, Chuck
 *	CWP: Shuki, Jack, Dave
 *
 * Technical Reference:
 *	The Common Depth Point Stack
 *	William A. Schneider
 *	Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *	1984
 */

static void interpovv (int nt, int ncdp, float *cdp, float **ovv, 
	float cdpt, float *ovvt);

segy tr;

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int ncdp;	/* number of cdps specified */
	float *cdp;	/* array[ncdp] of cdps */
	int icdp;	/* index into cdp array */
	int jcdp;	/* index into cdp array */
	int nvnmo;	/* number of vnmos specified */
	float *vnmo;	/* array[nvnmo] of vnmos */
	int ntnmo;	/* number of tnmos specified */
	float *tnmo;	/* array[ntnmo] of tnmos */
	float **ovv;	/* array[ncdp][nt] of sloth (1/velocity^2) functions */
	float *ovvt;	/* array[nt] of sloth for a particular trace */
	float smute;	/* zero samples with NMO stretch exceeding smute */
	float osmute;	/* 1/smute */
	int lmute;	/* length in samples of linear ramp for mute */
	int itmute;	/* zero samples with indices less than itmute */
	int sscale;	/* if non-zero, apply NMO stretch scaling */
	int invert;	/* if non-zero, do inverse NMO */
	long oldoffset;	/* offset of previous trace */
	long oldcdp;	/* cdp of previous trace */
	int newsloth;	/* if non-zero, new sloth function was computed */
	float tn;	/* NMO time (time after NMO correction) */
	float v;	/* velocity */
	float *qtn;	/* NMO-corrected trace q(tn) */
	float *ttn;	/* time t(tn) for NMO */
	float *atn;	/* amplitude a(tn) for NMO */
	float *qt;	/* inverse NMO-corrected trace q(t) */
	float *tnt;	/* time tn(t) for inverse NMO */
	float *at;	/* amplitude a(t) for inverse NMO */
	float acdp;	/* temporary used to sort cdp array */
	float *aovv;	/* temporary used to sort ovv array */
	float temp;	/* temporary float */

	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = tr.delrt/1000.0;

	/* get velocity functions, linearly interpolated in time */
	ncdp = countparval("cdp");
	if (ncdp>0) {
		if (countparname("vnmo")!=ncdp)
			err("a vnmo array must be specified for each cdp");
		if (countparname("tnmo")!=ncdp)
			err("a tnmo array must be specified for each cdp");
	} else {
		ncdp = 1;
	}
	cdp = ealloc1float(ncdp);
	if (!getparfloat("cdp",cdp)) cdp[0] = 0.0;
	ovv = ealloc2float(nt,ncdp);
	for (icdp=0; icdp<ncdp; ++icdp) {
		nvnmo = countnparval(icdp+1,"vnmo");
		ntnmo = countnparval(icdp+1,"tnmo");
		if (nvnmo!=ntnmo && !(ncdp==1 && nvnmo==1 && ntnmo==0))
			err("number of vnmo and tnmo values must be equal");
		if (nvnmo==0) nvnmo = 1;
		if (ntnmo==0) ntnmo = 1;
		vnmo = ealloc1float(nvnmo);
		tnmo = ealloc1float(nvnmo);
		if (!getnparfloat(icdp+1,"vnmo",vnmo)) vnmo[0] = 2000.0;
		if (!getnparfloat(icdp+1,"tnmo",tnmo)) tnmo[0] = 0.0;
		for (it=1; it<ntnmo; ++it)
			if (tnmo[it]<=tnmo[it-1])
				err("tnmo values must increase monotonically");
		for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
			intlin(ntnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
			ovv[icdp][it] = 1.0/(v*v);
		}
		free1float(vnmo);
		free1float(tnmo);
	}

	/* sort (by insertion) sloth functions by increasing cdp */
	for (jcdp=1; jcdp<ncdp; ++jcdp) {
		acdp = cdp[jcdp];
		aovv = ovv[jcdp];
		for (icdp=jcdp-1; icdp>=0 && cdp[icdp]>acdp; --icdp) {
			cdp[icdp+1] = cdp[icdp];
			ovv[icdp+1] = ovv[icdp];
		}
		cdp[icdp+1] = acdp;
		ovv[icdp+1] = aovv;
	}

	/* get other optional parameters */
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=0.0) err("smute must be greater than 0.0");
	if (!getparint("lmute",&lmute)) lmute = 25;
	if (!getparint("sscale",&sscale)) sscale = 1;
	if (!getparint("invert",&invert)) invert = 0;

	/* allocate workspace */
	ovvt = ealloc1float(nt);
	ttn = ealloc1float(nt);
	atn = ealloc1float(nt);
	qtn = ealloc1float(nt);
	tnt = ealloc1float(nt);
	at = ealloc1float(nt);
	qt = ealloc1float(nt);

	/* interpolate sloth function for first trace */
	interpovv(nt,ncdp,cdp,ovv,(float)tr.cdp,ovvt);

	/* set old cdp and old offset for first trace */
	oldcdp = tr.cdp;
	oldoffset = tr.offset-1;

	/* loop over traces */
	do {
		/* if necessary, compute new sloth function ovv(t) */
		if (tr.cdp!=oldcdp && ncdp>1) {
			interpovv(nt,ncdp,cdp,ovv,(float)tr.cdp,ovvt);
			newsloth = 1;
		} else {
			newsloth = 0;
		}

		/* if sloth function or offset has changed */
		if (newsloth || tr.offset!=oldoffset) {
		
			/* compute time t(tn) (normalized) */
			temp = (tr.offset*tr.offset)/(dt*dt);
			for (it=0,tn=ft/dt; it<nt; ++it,tn+=1.0)
				ttn[it] = sqrt(tn*tn+temp*ovvt[it]);
			
			/* compute inverse of stretch factor a(tn) */
			atn[0] = ttn[1]-ttn[0];
			for (it=1; it<nt; ++it)
				atn[it] = ttn[it]-ttn[it-1];
			
			/* determine index of first sample to survive mute */
			osmute = 1.0/smute;
			for (it=0,itmute=0; it<nt && atn[it]<osmute; ++it)
				itmute++;
			
			/* if inverse NMO will be performed */
			if (invert) {
							
				/* compute tn(t) from t(tn) */
				yxtoxy(nt-itmute,1.0,ft/dt+itmute,&ttn[itmute],
					nt-itmute,1.0,ft/dt+itmute,
					ft/dt-nt,ft/dt+nt,&tnt[itmute]);
			
				/* adjust mute time */
				itmute = 1.0+ttn[itmute]-ft/dt;
				itmute = MIN(nt-2,itmute);
								
				/* compute a(t) */
				if (sscale) {
					for (it=itmute+1; it<nt; ++it)
						at[it] = tnt[it]-tnt[it-1];
					at[itmute] = at[itmute+1];
				}
			}
		}
		
		/* if forward (not inverse) nmo */
		if (!invert) {
	
			/* do nmo via 8-point sinc interpolation */
			ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&ttn[itmute],&qtn[itmute]);
			
			/* apply mute */
			for (it=0; it<itmute; ++it)
				qtn[it] = 0.0;
			
			/* apply linear ramp */
			for (it=itmute; it<itmute+lmute && it<nt; ++it)
				qtn[it] *= (float)(it-itmute+1)/(float)lmute;
			
			/* if specified, scale by the NMO stretch factor */
			if (sscale)
				for (it=itmute; it<nt; ++it)
					qtn[it] *= atn[it];
			
			/* copy NMO corrected trace to output trace */
			bcopy(qtn,tr.data,nt*sizeof(float));
		
		/* else inverse nmo */
		} else {
	
			/* do inverse nmo via 8-point sinc interpolation */
			ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&tnt[itmute],&qt[itmute]);
			
			/* apply mute */
			for (it=0; it<itmute; ++it)
				qt[it] = 0.0;
			
			/* if specified, undo NMO stretch factor scaling */
			if (sscale)
				for (it=itmute; it<nt; ++it)
					qt[it] *= at[it];
			
			/* copy inverse NMO corrected trace to output trace */
			bcopy(qt,tr.data,nt*sizeof(float));
		}

		/* write output trace */
		puttr(&tr);

		/* remember offset and cdp */
		oldoffset = tr.offset;
		oldcdp = tr.cdp;

	} while (gettr(&tr));

	return EXIT_SUCCESS;
}


/* linearly interpolate/extrapolate sloth between cdps */
static void interpovv (int nt, int ncdp, float *cdp, float **ovv, 
	float cdpt, float *ovvt)
{
	static int index=0;
	int it;
	float a1,a2;

	/* if before first cdp, constant extrapolate */
	if (cdpt<=cdp[0]) {
		for (it=0; it<nt; ++it)
			ovvt[it] = ovv[0][it];
	
	/* else if beyond last cdp, constant extrapolate */
	} else if (cdpt>=cdp[ncdp-1]) {
		for (it=0; it<nt; ++it)
			ovvt[it] = ovv[ncdp-1][it];
	
	/* else, linearly interpolate */
	} else {
		xindex(ncdp,cdp,cdpt,&index);
		a1 = (cdp[index+1]-cdpt)/(cdp[index+1]-cdp[index]);
		a2 = (cdpt-cdp[index])/(cdp[index+1]-cdp[index]);
		for (it=0; it<nt; ++it)
			ovvt[it] = a1*ovv[index][it]+a2*ovv[index+1][it];
	}
}
