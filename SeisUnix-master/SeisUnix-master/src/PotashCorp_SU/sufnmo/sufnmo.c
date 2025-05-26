/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* NMO: $Revision: 1.21 $ ; $Date: 1998/08/24 20:54:33 $		*/
 
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
"									     ",
" NMO - NMO for an arbitrary velocity function of time and CDP	     ",
"									     ",
" sufnmo <stdin >stdout [optional parameters]				     ",
"									     ",
" Required parameters:							     ",
" f=			filename of the nmo file (This is a su segy file)    ",
" fac=1.0		a factor to multiply velocities			     ",
"									     ",
"									     ",
" The seismic traces and velocity traces are matched to each other	     ",
" using the cdp header word in both					     ",
"									     ",
" Optional Parameters:							     ",
" smute=1.5		samples with NMO stretch exceeding smute are zeroed  ",
" lmute=25		length (in samples) of linear ramp for stretch mute  ",
" sscale=1		=1 to divide output samples by NMO stretch factor    ",
" invert=0		=1 to perform (approximate) inverse NMO	             ",
"									     ",
NULL};

/* Credits:
 *	SEP: Shuki, Chuck Sword
 *	CWP: Shuki, Jack, Dave Hale, Bjoern Rommel
 *      Modified: 08/08/98 - Carlos E. Theodoro - option for lateral offset
 *
 * Technical Reference:
 *	The Common Depth Point Stack
 *	William A. Schneider
 *	Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *	1984
 *
 * Trace header fields accessed: ns, dt, delrt, offset, cdp, sy
 */
/**************** end self doc *******************************************/

static void interpovv (int nt, int ncdp, float *cdp, 
	double **ovv,int cdpt, 
	double *ovvt);

segy tr,trv;

int
main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int ncdp;	/* number of cdps specified */
	float *cdp;	/* array[ncdp] of cdps */
	int icdp;	/* index into cdp array */
	int nvnmo;	/* number of vnmos specified */
	float *vnmo;	/* array[nvnmo] of vnmos */
	float *tnmo;	/* array[ntnmo] of tnmos */
	double **ovv;	/* array[ncdp][nt] of sloth (1/velocity^2) functions */
	double *ovvt;	/* array[nt] of sloth for a particular trace */
	float smute;	/* zero samples with NMO stretch exceeding smute */
	float osmute;	/* 1/smute */
	int lmute;	/* length in samples of linear ramp for mute */
	int itmute=0;	/* zero samples with indices less than itmute */
	int sscale;	/* if non-zero, apply NMO stretch scaling */
	int invert;	/* if non-zero, do inverse NMO */
	float sy;	/* cross-line offset component */
	int ixoffset;	/* indes for cross-line offset component */
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
	double temp;	/* temporary float */
	double tsq;	/* temporary float */
 	
	cwp_String f="";	/* su velocity file  */
	FILE *fp;		/* file pointer */
	float fac=1.0;
	
	/* hook up getpar */
	initargs(argc, argv);
	requestdoc(1);

	/* get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = ((double) tr.dt)/1000000.0;
	ft = tr.delrt/1000.0;
	sy  = tr.sy;
	
	MUSTGETPARSTRING("sufile",&sufile);
	if (!getparfloat("fac",&fac)) fac = 1.0;
	
	
	/* Read cdps */
	if((fp=fopen(sufile,"r"))==NULL)
                        err("cannot open file=%s\n",f);
	
	ncdp = fgettra(fp,&trv,0);
	nvnmo=trv.ns;

	ovv = ealloc2double(nt,ncdp);
	cdp = ealloc1float(ncdp);
	vnmo = ealloc1float(nvnmo);
	tnmo = ealloc1float(nvnmo);
	icdp=0;
	
	for(it=0;it<nvnmo;it++)
		tnmo[it]=ft+it*(double)trv.dt/1000000.0;
	
        do {
		cdp[icdp]=trv.cdp;
		for(it=0; it<nvnmo;it++) 
			vnmo[it] =fac*trv.data[it];
		for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
			intlin(nvnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
			ovv[icdp][it] = (double)1.0/(v*v);
		}
		icdp++;
	} while(fgettr(fp,&trv));

	fclose(fp);
        free1float(vnmo);
        free1float(tnmo);
/*      free1float(anis1);
        free1float(anis2); */
	

	/* get other optional parameters */
	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (!getparint("ixoffset",&ixoffset)) ixoffset=0; 
	  if (ixoffset==0) sy = 0.0;
	if (smute<=0.0) err("smute must be greater than 0.0");
	if (!getparint("lmute",&lmute)) lmute = 25;
	if (!getparint("sscale",&sscale)) sscale = 1;
	if (!getparint("invert",&invert)) invert = 0;

	/* allocate workspace */
	ovvt = ealloc1double(nt);
	ttn = ealloc1float(nt);
	atn = ealloc1float(nt);
	qtn = ealloc1float(nt);
	tnt = ealloc1float(nt);
	at = ealloc1float(nt);
	qt = ealloc1float(nt);

	/* interpolate sloth and anis function for first trace */
	interpovv(nt,ncdp,cdp,ovv,tr.cdp,ovvt);

	/* set old cdp and old offset for first trace */
	oldcdp = tr.cdp;
	oldoffset = tr.offset-1;

	if (ixoffset==1) warn("sy = %f",sy);

	/* loop over traces */
	do {
		/* verbose */
/*		fprintf(stderr," CDP # %10d ",tr.cdp);  */
		
		/* if necessary, compute new sloth and anis function */
		if (tr.cdp!=oldcdp && ncdp>1) {
			interpovv(nt,ncdp,cdp,ovv,tr.cdp,
				  ovvt);
			newsloth = 1;
		} else {
			newsloth = 0;
		}

		/* if sloth and anis function or offset has changed */
		if (newsloth || tr.offset!=oldoffset) {
			/* compute time t(tn) (normalized) */
			temp = (double)((float) tr.offset*(float) tr.offset + sy*sy)/(dt*dt);
			for (it=0,tn=ft/dt; it<nt; ++it,tn+=1.0) {
				tsq = temp*ovvt[it];
				ttn[it] = (float)sqrt(tn*tn + tsq);
				}
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
			memcpy( (void *) tr.data,
					(const void *) qtn, nt*FSIZE);
		
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
			memcpy( (void *) tr.data,
					(const void *) qt,nt*sizeof(float));
		}

		/* write output trace */
		puttr(&tr);

		/* remember offset and cdp */
		oldoffset = tr.offset;
		oldcdp = tr.cdp;

	} while (gettr(&tr));

	return EXIT_SUCCESS;
}


/* match cdp in trace to cdp in velocity file */
static void interpovv (int nt, int ncdp, float *cdp, double **ovv, 
                      int cdpt, double *ovvt)
{
	int indx=0;

	/* if before first cdp, err */
/*	fprintf(stderr," %d\n", cdpt); */
	if (cdpt<(int)cdp[0]) err(" trace CDP # %d is smaller than one in velocity file %d \n",cdpt,(int)cdp[0]);
	
	/* else if beyond last cdp, error*/
	if (cdpt>(int)cdp[ncdp-1]) err(" trace CDP # %d is larger than largets in velocity file %d ",cdpt,(int)cdp[ncdp-1]);
	
	/* else, copy */
	else {
		xindex(ncdp,cdp,cdpt,&indx);
		if((int)(cdp[indx])!=cdpt) err(" CDP # %d not in velocity file exiting \n",cdpt);
		memcpy( (void *) ovvt, (const void *) ovv[indx], nt*DSIZE);
	}
}
