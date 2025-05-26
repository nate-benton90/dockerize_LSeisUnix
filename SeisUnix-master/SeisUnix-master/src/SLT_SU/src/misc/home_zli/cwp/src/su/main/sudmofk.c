/* SUDMOFK: $Revision: 1.6 $ ; $Date: 90/06/14 14:38:34 $		*/

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
"SUDMOFK - DMO via F-K domain (log-stretch) method for common-offset gathers\n"
"\n"
"sudmofk <stdin >stdout cdpmin= cdpmax= dxcdp= noffmix= [optional parms]\n"
"\n"
"Required Parameters:\n"
"cdpmin                  minimum cdp (integer number) for which to apply DMO\n"
"cdpmax                  maximum cdp (integer number) for which to apply DMO\n"
"dxcdp                   distance between successive cdps\n"
"noffmix                 number of offsets to mix (see notes)\n"
"\n"
"Optional Parameters:\n"
"vmin=1500.0             minimum velocity used to determine maximum slope\n"
"tdmo=0.0                times corresponding to rms velocities in vdmo\n"
"vdmo=vmin               rms velocities corresponding to times in tdmo\n"
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
"The tdmo and vdmo arrays specify a velocity function of time that is\n"
"used to implement a first-order correction for depth-variable velocity.\n"
"The times in tdmo must be monotonically increasing.\n"
"\n"
"For each offset, the minimum time at which a non-zero sample exists is\n"
"used to determine a mute time.  Output samples for times earlier than this\n" 
"mute time will be zeroed.  Computation time may be significantly reduced\n"
"if the input traces are zeroed (muted) for early times at large offsets.\n"
"\n"
"Trace header fields accessed:  nt, dt, delrt, offset, cdp.\n"
"\n";
/**************** end self doc *******************************************/

/* Credits:
 *	CWP: Dave
 *
 * Technical Reference:
 *      Dip-Moveout Processing - SEG Course Notes
 *      Dave Hale, 1988
 */

void dmooff (float offset, int nxfft, float dx, 
	int nt, float dt, float ft,
	float vmin, int ntvdmo, float *tdmo, float *vdmo,
	float **ptx);
void maketu (float offset, int itmin, float vmin, 
	int ntvdmo, float *tdmo, float *vdmo,
	int nt, float dt, float ft, float **uoftp,
	int *nup, float *dup, float *fup, float **tofup, float *tconp);

segy tr,tro;

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	int cdpmin;	/* minimum cdp to process */
	int cdpmax;	/* maximum cdp to process */
	float dx;	/* cdp sampling interval */
	int nx;	        /* number of cdps to process */
	int nxfft;	/* number of cdps after zero padding for fft */
	int nxpad;	/* minimum number of cdps for zero padding */
	int ix;		/* cdp index, starting with ix=0 */
	int noffmix;	/* number of offsets to mix */
	float vmin;	/* minimum velocity */
	float *tdmo;	/* times at which rms velocities are specified */
	float *vdmo;	/* rms velocities at times specified in tdmo */
	int ntdmo;	/* number tnmo values specified */
	int itdmo;	/* index into tnmo array */
	int nvdmo;	/* number vnmo values specified */
	float **p;	/* traces for one offset - common-offset gather */
	float **q;	/* DMO-corrected and mixed traces to be output */
	float offset;	/* source-receiver offset of current trace */
	float oldoffset;/* offset of previous trace */
	int noff;	/* number of offsets processed in current mix */
	int ntrace;	/* number of traces processed in current mix */
	int itrace;	/* trace index */
	int gottrace;	/* non-zero if an input trace was read */
	int done;	/* non-zero if done */
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
	offset = tr.offset;

	/* get parameters */
	if (!getparint("cdpmin",&cdpmin)) err("must specify cdpmin");
	if (!getparint("cdpmax",&cdpmax)) err("must specify cdpmax");
	if (cdpmin>cdpmax) err("cdpmin must not be greater than cdpmax");
	if (!getparfloat("dxcdp",&dx)) err("must specify dxcdp");
	if (!getparint("noffmix",&noffmix)) err("must specify noffmix");
	if (!getparfloat("vmin",&vmin)) vmin = 1500.0;
	ntdmo = countparval("tdmo");
	if (ntdmo==0) ntdmo = 1;
	tdmo = ealloc1float(ntdmo);
	if (!getparfloat("tdmo",tdmo)) tdmo[0] = 0.0;
	nvdmo = countparval("vdmo");
	if (nvdmo==0) nvdmo = 1;
	if (nvdmo!=ntdmo) err("number of tdmo and vdmo must be equal");
	vdmo = ealloc1float(nvdmo);
	if (!getparfloat("vdmo",vdmo)) vdmo[0] = vmin;
	for (itdmo=1; itdmo<ntdmo; ++itdmo)
		if (tdmo[itdmo]<=tdmo[itdmo-1])
			err("tdmo must increase monotonically");
	if (!getparint("verbose",&verbose)) verbose=0;
	
	/* determine number of cdps to process */
	nx = cdpmax-cdpmin+1;
	
	/* allocate and zero common-offset gather p(t,x) */
	nxpad = 0.5*ABS(offset/dx);
	nxfft = npfaro(nx+nxpad,2*(nx+nxpad));
	p = ealloc2float(nt,nxfft+2);
	for (ix=0; ix<nxfft; ++ix)
		for (it=0; it<nt; ++it)
			p[ix][it] = 0.0;

	/* allocate and zero offset mix accumulator q(t,x) */
	q = ealloc2float(nt,nx);
	for (ix=0; ix<nx; ++ix)
		for (it=0; it<nt; ++it)
			q[ix][it] = 0.0;
		
	/* open temporary file for headers */
	/*hfp = tmpfile();*/
	hfp = etempfile(NULL);
	
	/* initialize */
	oldoffset = offset;
	gottrace = 1;
	done = 0;
	ntrace = 0;
	noff = 0;

	/* loop over traces */
	do {
		/* if got a trace, determine offset */
		if (gottrace) offset = tr.offset;
		
		/* if an offset is complete */
		if ((gottrace && offset!=oldoffset) || !gottrace) {
		
			/* do dmo for old common-offset gather */
			dmooff(oldoffset,nxfft,dx,nt,dt,ft,vmin,
				ntdmo,tdmo,vdmo,p);
			
			/* add dmo-corrected traces to mix */
			for (ix=0; ix<nx; ++ix)
				for (it=0; it<nt; ++it)
					q[ix][it] += p[ix][it];
			
			/* count offsets in mix */
			noff++;
							
			/* free space for old common-offset gather */
			free2float(p);
			
			/* if beginning a new offset */
			if (offset!=oldoffset) {
				
				/* allocate space for new offset */
				nxpad = 0.5*ABS(offset/dx);
				nxfft = npfaro(nx+nxpad,2*(nx+nxpad));
				p = ealloc2float(nt,nxfft+2);
				for (ix=0; ix<nxfft; ++ix)
					for (it=0; it<nt; ++it)
						p[ix][it] = 0.0;
			}
		}
		
		/* if a mix of offsets is complete */
		if (noff==noffmix || !gottrace) {
			
			/* rewind trace header file */
			fseek(hfp,0L,SEEK_SET);
			
			/* loop over all output traces */
			for (itrace=0; itrace<ntrace; ++itrace) {
			
				/* read trace header and determine cdp index */
				efread(&tro,HDRBYTES,1,hfp);
				
				/* get dmo-corrected data */
				bcopy(q[tro.cdp-cdpmin],tro.data,
					nt*sizeof(float));
				
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
			
			/* reset number of offsets and traces in mix */
			noff = 0;
			ntrace = 0;
			
			/* zero offset mix accumulator */
			for (ix=0; ix<nx; ++ix)
				for (it=0; it<nt; ++it)
					q[ix][it] = 0.0;
		}
			
		/* if cdp is not within range to process, skip it */
		if (tr.cdp<cdpmin || tr.cdp>cdpmax) continue;
	
		/* save trace header and update number of traces */
		efwrite(&tr,HDRBYTES,1,hfp);
		ntrace++;

		/* remember offset */
		oldoffset = offset;

		/* get trace samples */
		bcopy(tr.data,p[tr.cdp-cdpmin],nt*sizeof(float));

		/* get next trace (if there is one) */
		if (!gettr(&tr)) gottrace = 0;
		
	} while (!done);

	return EXIT_SUCCESS;
}

void dmooff (float offset, int nxfft, float dx, 
	int nt, float dt, float ft,
	float vmin, int ntvdmo, float *tdmo, float *vdmo,
	float **ptx)
/*****************************************************************************
perform dmo in w-k domain for one offset
******************************************************************************
Input:
offset		source receiver offset
nxfft		number of midpoints, zero-padded for prime factor fft
dx		midpoint sampling interval
nt		number of time samples
dt		time sampling interval
ft		first time
vmin		minimum velocity
ntvdmo		number of tdmo,vdmo pairs
tdmo		times at which rms velocities vdmo are specified
vdmo		rms velocities specified at times in tdmo
ptx		array[nxfft][nt] containing p(t,x) zero-padded for fft

Output:
ptx		array[nxfft][nt] containing p(t,x) after dmo
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/04/90
*****************************************************************************/
{
	int itmin,nu,nufft,nw,nk,ix,iu,iw,ik,it,iwn,iwnyq;
	float dw,dk,tcon,wwscl,scale,scales,amp,phase,fr,fi,pwr,pwi,fftscl,
		du,fu,w,k,
		*uoft,*tofu;
	complex czero=cmplx(0.0,0.0),**ptk,*pu,*pw;

	/* determine minimum time of first non-zero sample */
	for (ix=0,itmin=nt; ix<nxfft; ++ix) {
		for (it=0; it<itmin && ptx[ix][it]==0.0; ++it);
		itmin = it;
	}
	
	/* if all zeros, simply return */
	if (itmin>=nt) return;
	
	/* make stretch and compress functions t(u) and u(t) */
	maketu(offset,itmin,vmin,ntvdmo,tdmo,vdmo,nt,dt,ft,
		&uoft,&nu,&du,&fu,&tofu,&tcon);
	
	/* determine frequency sampling */
	nufft = npfao(2*nu,3*nu);
	nw = nufft;
	dw = 2.0*PI/(nufft*du);
	
	/* allocate workspace */
	pu = pw = ealloc1complex(nufft);
	
	/* determine wavenumber sampling and set pointers to complex p(t,k) */
	nk = nxfft/2+1;
	dk = 2.0*PI/(nxfft*dx);
	ptk = (complex**)ealloc1(nk,sizeof(complex*));
	for (ik=0; ik<nk; ++ik)
		ptk[ik] = (complex*)ptx[0]+ik*nt;
	
	/* compute fft scale factor */
	fftscl = 1.0/(nufft*nxfft);
	
	/* Fourier transform p(t,x) to p(t,k) */
	pfa2rc(-1,2,nt,nxfft,ptx[0],ptk[0]);

	/* loop over wavenumbers */
	for (ik=0,k=0.0; ik<nk; ++ik,k+=dk) {
		
		/* stretch p(t;k) to p(u) */
		ints8c(nt,dt,ft,ptk[ik],czero,czero,nu,tofu,pu);
		
		/* pad with zeros and Fourier transform p(u) to p(w) */
		for (iu=nu; iu<nufft; ++iu)
			pu[iu].r = pu[iu].i = 0.0;
		pfacc(1,nufft,pu);
		
		/* constants independent of w */
		iwnyq = nw/2;
		wwscl = pow(k*0.5*offset/tcon,2.0);
		
		/* zero p(w=0) (dc should be zero anyway) */
		pw[0].r = pw[0].i = 0.0;
		
		/* do dmo for frequencies between zero and Nyquist */
		for (iw=1,iwn=nw-1,w=dw; iw<iwnyq; ++iw,--iwn,w+=dw) {
			scales = 1.0+wwscl/(w*w);
			scale = sqrt(scales);
			amp = fftscl/scale;
			phase = w*tcon*(scale-1.0);
			fr = amp*cos(phase);
			fi = amp*sin(phase);
			pwr = pw[iw].r;
			pwi = pw[iw].i;
			pw[iw].r = pwr*fr-pwi*fi;
			pw[iw].i = pwr*fi+pwi*fr;
			pwr = pw[iwn].r;
			pwi = pw[iwn].i;
			pw[iwn].r = pwr*fr+pwi*fi;
			pw[iwn].i = pwi*fr-pwr*fi;
		}
	
		/* do dmo for the Nyquist frequency */
		w = iwnyq*dw;
		scales = 1.0+wwscl/(w*w);
		scale = sqrt(scales);
		amp = fftscl/scale;
		phase = w*tcon*(scale-1.0);
		fr = amp*cos(phase);
		fi = amp*sin(phase);
		pwr = pw[iwnyq].r;
		pwi = pw[iwnyq].i;
		pw[iwnyq].r = pwr*fr-pwi*fi;
		pw[iwnyq].i = pwr*fi+pwi*fr;
		
		/* Fourier transform p(w) to p(u) */
		pfacc(-1,nufft,pu);
		
		/* compress p(u) to p(t;k) */
		ints8c(nu,du,fu,pu,czero,czero,nt,uoft,ptk[ik]);
	}
	
	/* Fourier transform p(t,k) to p(t,x) */
	pfa2cr(1,2,nt,nxfft,ptk[0],ptx[0]);
	
	/* free workspace */
	free1float(tofu);
	free1float(uoft);
	free1complex(pu);
	free1(ptk);
}

void maketu (float offset, int itmin, float vmin, 
	int ntvdmo, float *tdmo, float *vdmo,
	int nt, float dt, float ft, float **uoftp,
	int *nup, float *dup, float *fup, float **tofup, float *tconp)
/*****************************************************************************
make stretch and compress functions t(u) and u(t)
******************************************************************************
Input:
offset		source receiver offset
itmin		index of minimum first non-zero sample for this offset
vmin		minimum velocity
ntvdmo		number of tdmo,vdmo pairs
tdmo		times at which rms velocities vdmo are specified
vdmo		rms velocities specified at times in tdmo
nt		number of time samples
dt		time sampling interval
ft		first time

Output:
uoftp		array[nt] of u(t)
nup		number of u (stretched t) samples
dup		u sampling interval
fup		first u
tofup		array[nu] of t(u)
tconp		time constant relating t(u) and u(t)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/04/90
*****************************************************************************/
{
	int it,iu,numax,nu;
	float tmin,dumin,et,eu,t1,t2,vdmolo,vdmohi,
		v2,v22,v44,gamma,
		v2m,v22m,v44m,gammam,t,dv2,vi2,vi4,v24,
		*uoft,du,fu,*tofu,tcon;
	
	/* determine maximum number of u */
	numax = 500+log((float)nt)*(float)(nt-1);
		
	/* allocate space for u(t) */
	uoft = ealloc1float(nt);
	
	/* determine t1 and t2, rounded to nearest sampled times */
	tmin = ft+itmin*dt;
	et = ft+(nt-1)*dt;
	t1 = MIN(et,MAX(ft+dt,tmin));
	if (offset!=0.0)
		t2 = MAX(t1,1.0/(1.0/et+2.0*vmin*vmin*dt/(offset*offset)));
	else
		t2 = t1;
	t1 = ft+NINT(t1/dt)*dt;
	t2 = ft+NINT(t2/dt)*dt;
	
	/* compute u(t) */
	vdmolo = vdmo[0];
	vdmohi = vdmo[ntvdmo-1];
	intlin(ntvdmo,tdmo,vdmo,vdmolo,vdmohi,1,&ft,&v2);
	v22 = v2*v2;
	v44 = v22*v22;
	gamma = 1.0;
	for (it=0,t=ft; it<nt; ++it,t+=dt) {
		v2m = v2;
		v22m = v22;
		v44m = v44;
		gammam = gamma;
		if (t>0.0) {
			intlin(ntvdmo,tdmo,vdmo,vdmolo,vdmohi,1,&t,&v2);
			v22 = v2*v2;
			vi2 = (t*v22-(t-dt)*v22m)/dt;
			vi4 = vi2*vi2;
			v44 = (dt*vi4+(t-dt)*v44m)/t;
		} else {
			v2 = v2m;
			v22 = v22m;
			v44 = v44m;
		}
		dv2 = (v2-v2m)/dt;
		v24 = v22*v22;
		gamma = 1.5*v44/v24-t*dv2/v2-0.5;
		if (t<=t1) {
			uoft[it] = t-t1;
		} else if (t>t1 && t<=t2) {
			du = t1*(gamma*log(t/(t-0.5*dt)) -
				gammam*log((t-dt)/(t-0.5*dt)));
			dumin = 0.1*dt*t1/t;
			uoft[it] = uoft[it-1]+MAX(dumin,du);
		} else if (t>t2) {
			uoft[it] = 2.0*uoft[it-1]-uoft[it-2];
		}
	}
	
	/* determine minimum u(t)-u(t-dt) */
	dumin = uoft[1]-uoft[0];
	for (it=1; it<nt; ++it)
		dumin = MIN(dumin,uoft[it]-uoft[it-1]);
	
	/* determine u sampling for t(u) to avoid aliasing */
	fu = 0.0;
	eu = uoft[nt-1];
	du = dumin;
	nu = 1+NINT((eu-fu)/du);
	if (nu>numax) {
		nu = numax;
		du = (eu-fu)/(nu-1);
	}
	
	/* allocate space for t(u) */
	tofu = ealloc1float(nu);
	
	/* compute t(u) by inverse linear interpolation of u(t) */
	yxtoxy(nt,dt,ft,uoft,nu,du,fu,ft,et,tofu);
	
	/* set time constant */
	tcon = t1;
	
	/* set returned values */
	*uoftp = uoft;
	*nup = nu;
	*dup = du;
	*fup = fu;
	*tofup = tofu;
	*tconp = tcon;
}
