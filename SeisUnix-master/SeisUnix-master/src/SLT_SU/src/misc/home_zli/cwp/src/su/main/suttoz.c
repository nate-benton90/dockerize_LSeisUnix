/* SUTTOZ: $Revision: 1.3 $ ; $Date: 92/10/26 13:48:17 $		*/

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
char *sdoc[] = {
"									",
" SUTTOZ - resample from time to depth					",
"									",
" suttoz <stdin >stdout [optional parms]				",
"									",
" Optional Parameters:							",
" nz=                     number of depth samples			",
" dz=vmin*dt/2            depth sampling interval (defaults avoids aliasing)",
" fz=v(ft)*ft/2           first depth sample				",
" t=0.0                   times corresponding to interval velocities in v",
" v=1500.0                interval velocities corresponding to times in v",
" vfile=                  binary (non-ascii) file containing velocities v(t)",
" verbose=1               >0 to print depth sampling information	",
"									",
" Notes:								",
" The t and v arrays specify an interval velocity function of time.	",
" Linear interpolation and constant extrapolation is used to determine	",
" interval velocities at times not specified.  Values specified in t	",
" must increase monotonically.						",
"									",
" Alternatively, interval velocities may be stored in a binary file	",
" containing one velocity for every time sample.  If vfile is specified,",
" then the t and v arrays are ignored.					",
"									",
" Trace header fields accessed:  ns, dt, and delrt			",
" Trace header fields modified:  trid, ns, d1, and f1			",
NULL};
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Dave Hale
 */

/* functions defined and used internally */
static void maketz (int nt, float dt, float ft, float v[], 
	int nz, float dz, float fz, float t[]);
static void zttz(int nt, float dt, float ft, float zt[], float vft, float vlt, 
	int nz, float dz, float fz, float tz[]);

segy tr;

main(int argc, char **argv)
{
	int nt,it,nz,ntpar,nvpar,itpar,verbose;
	float dt,ft,dz,fz,t,vmin,vmax,*tpar,*vpar,*vt,*tz,*temp;
	char *vfile="";

	/* hook up getpar */
	initargs(argc,argv);
	requestdoc(1);

	/* get time sampling from first header */
	if (!gettr(&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = tr.delrt/1000.0;

	/* determine velocity function v(t) */
	vt = ealloc1float(nt);
	if (!getparstring("vfile",&vfile)) {
		ntpar = countparval("t");
		if (ntpar==0) ntpar = 1;
		tpar = ealloc1float(ntpar);
		if (!getparfloat("t",tpar)) tpar[0] = 0.0;
		nvpar = countparval("v");
		if (nvpar==0) nvpar = 1;
		if (nvpar!=ntpar) err("number of t and v values must be equal");
		vpar = ealloc1float(nvpar);
		if (!getparfloat("v",vpar)) vpar[0] = 1500.0;
		for (itpar=1; itpar<ntpar; ++itpar)
			if (tpar[itpar]<=tpar[itpar-1])
				err("tpar must increase monotonically");
		for (it=0,t=0.0; it<nt; ++it,t+=dt)
			intlin(ntpar,tpar,vpar,vpar[0],vpar[ntpar-1],
				1,&t,&vt[it]);
	} else {
		if (fread(vt,sizeof(float),nt,fopen(vfile,"r"))!=nt)
			err("cannot read %d velocities from file %s",nt,vfile);
	}

	/* determine minimum and maximum velocities */
	for (it=1,vmin=vmax=vt[0]; it<nt; ++it) {
		if (vt[it]<vmin) vmin = vt[it];
		if (vt[it]>vmax) vmax = vt[it];
	}

	/* get parameters */
	if (!getparfloat("dz",&dz)) dz = vmin*dt/2.0;
	if (!getparfloat("fz",&fz)) fz = vt[0]*ft/2.0;
	if (!getparint("nz",&nz)) nz = 1+(nt-1)*dt*vmax/(2.0*dz);
	if (!getparint("verbose",&verbose)) verbose = 1;

	/* if requested, print depth sampling */
	if (verbose) {
		fprintf(stderr,"Input:\n");
		fprintf(stderr,"\tnumber of time samples = %d\n",nt);
		fprintf(stderr,"\ttime sampling interval = %g\n",dt);
		fprintf(stderr,"\tfirst time sample = %g\n",ft);
		fprintf(stderr,"Output:\n");
		fprintf(stderr,"\tnumber of depth samples = %d\n",nz);
		fprintf(stderr,"\tdepth sampling interval = %g\n",dz);
		fprintf(stderr,"\tfirst depth sample = %g\n",fz);
	}

	/* allocate workspace */
	tz = ealloc1float(nz);
	temp = ealloc1float(nt);

	/* make t(z) function */
	maketz(nt,dt,ft,vt,nz,dz,fz,tz);
	
	/* loop over traces */
	do {
		/* update header fields */
		tr.trid = 30;
		tr.ns = nz;
		tr.d1 = dz;
		tr.f1 = fz;

		/* resample */
		bcopy(tr.data,temp,nt*sizeof(float));
		ints8r(nt,dt,ft,temp,0.0,0.0,nz,tz,tr.data);

		/* put this trace before getting another */
		puttr(&tr);

	} while(gettr(&tr));

	return EXIT_SUCCESS;
}

/* compute t(z) from v(t) */
static void maketz (int nt, float dt, float ft, float v[], 
	int nz, float dz, float fz, float t[])
{
	int it;
	float vft,vlt,*z;

	z = ealloc1float(nt);
	z[0] = 0.5*ft*v[0];
	for (it=1; it<nt; it++)
		z[it] = z[it-1]+0.5*dt*v[it-1];
	vft = v[0];
	vlt = v[nt-1];
	zttz(nt,dt,ft,z,vft,vlt,nz,dz,fz,t);
	free1float(z);
}

/* compute t(z) from z(t) */
static void zttz(int nt, float dt, float ft, float zt[], float vft, float vlt, 
	int nz, float dz, float fz, float tz[])
{
	int iz;
	float z,lt=ft+(nt-1)*dt,lz=fz+(nz-1)*dz;

	yxtoxy(nt,dt,ft,zt,nz,dz,fz,0.0,0.0,tz);
	for (iz=0,z=fz; z<=zt[0]; iz++,z+=dz)
		tz[iz] = 2.0*z/vft;
	for (iz=nz-1,z=lz; z>=zt[nt-1]; iz--,z-=dz)
		tz[iz] = lt+2.0*(z-zt[nt-1])/vlt;
}
