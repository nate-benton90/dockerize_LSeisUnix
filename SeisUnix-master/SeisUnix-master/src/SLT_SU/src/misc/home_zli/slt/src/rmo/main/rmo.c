/* RMO: $Revision: 1.3 $ ; $Date: 93/08/13 10:22:46 $		*/

#include "cwp.h"
#include "par.h"


#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

void taper(int nl, int n, int nh, float *r);
void bldmd_(complex *arayln, complex *fslice, complex *fpspac,
	int *nrcut, int *nr, int *noff);
void analy_(complex *arayln, complex *arayll, complex *fslice,
	float *wrkbuf, complex *fpspac, float *offset,
	float *pvstart, float *pvincs, int *npvsmp, int *noff,
	float *sfereq, float *prw);

void rmo(float **trg, float **trm, int nt, float dt, float *offset, int noff, 
        float tmin, float tmax, float rmin, float rmax, float rbeg, float rend,
	float fmin, float fmax,	float ttaper, float amp, int mult, float prw)

{
	float *rt;	/* real trace				*/
	complex *ct;	/* Fourier transformed trace		*/
	int nfft;		/* number of points on transformed trace */
	int nfby2p1;		/* nfft/2 + 1				*/
	int nt1; 		/* the first sample of window		*/
	int ntt;		/* the sample number of window		*/
	int it;			/* time index 			*/
	int nwbeg;		/* first frequency sample to be execcuted */
	int nwend;		/* last frequency sample to be executed */
	float dw;		/* frequency sample interval in HZ	*/
	float w;		/* frequency sample 		*/
	int iw;			/* frequency index		*/
	int ir;			/* residual moveout index	*/
	float dr;		/* increment of residuak moveout  */
	int nr;			/* number of residual moveout  */
	int nrbeg;		/* first residual-moveout sample of multiple */
	int nrend;		/* last residual-moveout sample of multiple */
	int ltaper;		/* time samples of trace taper	*/
	int ioff;		/* offset index			*/
	float doff;		/* maximum increment in offset squared	*/
	complex czero=cmplx(0.0,0.0);
	complex **ctrg;		/* complex trace gather		*/
	complex *cm;		/* frequency slice 		*/
	complex *fpspac;	/* modeling coefficient 	*/
	complex **arayln, **arayll;	/* matrices in LEAST-SQUIRES */
	float *wrkbuf;			/* work buffer		*/



	/* Set up the first sample  of window zero */
	nt1 = nint(tmin/dt);
	ntt = nint((tmax-tmin)/dt);
	ltaper = nint(ttaper/dt);


	/* Set up pfa fft */
	nfft = npfaro(ntt, LOOKFAC * ntt);
	nfby2p1 = nfft/2 + 1;
	dw = 1.0/(nfft*dt);

	rt = ealloc1float(nfft);
	ct = ealloc1complex(nfby2p1);
	ctrg  = ealloc2complex(nfby2p1,noff);
	cm = ealloc1complex(noff);
	wrkbuf = ealloc1float(noff);
	fpspac = ealloc1complex(noff);


	/* Fourier transform from t to w*/
	for(ioff=0; ioff<noff; ++ioff) {

		/* Load trace into rt (zero-padded) */
		for(it=0; it<ntt; ++it)
			 rt[it] = trg[ioff][it+nt1]/nfft;

		/* taper trace and pad zeros */
		taper(ltaper,ntt,ltaper,rt);
		bzero(rt + ntt, (nfft - ntt)*FSIZE);

		/* FFT */
		pfarc(1, nfft, rt, ct);

		/* load complex spectrum into a gather */
		for (iw = 0; iw < nfby2p1; ++iw)  ctrg[ioff][iw] = ct[iw];
	}

	/* scale the prewhiting factor by number of offset */
	prw *= noff;

	/* Build multiple model for each frequency slice */
	nwbeg = nint(fmin/dw);
	nwend = nint(fmax/dw);
	doff = MAX(offset[noff-1]-offset[noff-2],offset[0]-offset[1]);
	for(iw=nwbeg; iw<=nwend; ++iw){
		w = 2*PI*iw*dw;

		/* extract a frequency slice */
		for(ioff=0; ioff<noff; ++ioff) 
			cm[ioff] = ctrg[ioff][iw];
		/* avoid dividing zero  */
		w = (w<=0)?0.001:w;
		/* set increment for residual moveout  */
		dr = PI/w;
		/* cut the maximum and the minimum residual moveout to 
			avoid aliasing */
		rmax = (rmax<PI/(w*doff))?rmax:PI/(w*doff);
		rmin = (rmin>-PI/(w*doff))?rmin:-PI/(w*doff);
		rbeg = (rbeg>rmin)?rbeg:rmin;
		rend = (rend<rmax)?rend:rmax;
		/*set the number of residual moveout */
		nr = nint((rmax-rmin)/dr);
		/* make sure the equation systerm not underdetermined */
		nr = (nr<=noff)?nr:noff;

		/* allocate work buffers  for matrices */
		arayln = ealloc2complex(noff,nr);
		arayll = ealloc2complex(nr,nr);

		/*  invert data for model: Frequency vs. residual moveout */
		analy_(arayln[0],arayll[0],cm,wrkbuf,fpspac,offset,&rmin,
			&dr,&nr,&noff,&w,&prw);

		/* forward model to produce frequency domain multiple model */
		nrbeg = nint((rbeg-rmin)/dr);
		nrend = nint((rend-rmin)/dr);
		nrend =  (nrend<=nr)?nrend:nr;
		bldmd_(arayln[0],cm,fpspac,&nrbeg,&nrend,&noff);	

		/* place modeled frequency slice back */
		for(ioff=0; ioff<noff; ++ioff) 
			ctrg[ioff][iw] = cm[ioff];

		free2complex(arayln);
		free2complex(arayll);
	}
	
	

/* load multple model for each offset  and subtract from the original data */
	for(ioff=0; ioff<noff; ++ioff){
		for(iw=0; iw<nwbeg; ++iw) ct[iw] = czero;
		for(iw=nwbeg; iw<=nwend; ++iw) ct[iw] = ctrg[ioff][iw];
		for(iw=nwend+1; iw<nfby2p1; ++iw) ct[iw] = czero;
			 
		/* Inverse FFT */
		pfacr(-1, nfft, ct, rt);
		for(it=0;  it<ntt; ++it) trg[ioff][nt1+it] -= amp*rt[it];
		if(mult){
			/* output multiple model */
			for(it=0;it<ntt; ++it) 
				trm[ioff][it+nt1] += amp*rt[it];
		}
	}

	free1float(rt);
	free1float(wrkbuf);
	free1complex(ct);
	free1complex(cm);
	free2complex(ctrg);
	free1complex(fpspac);
}


	void taper(int nl, int n, int nh, float *r)
/* taper a trace at both ends */
{
        float temp;
        int i;

	nl =  (nl<n)?nl:n-1;
	nh =  (nh<n)?nh:n-1;

        for(i=0; i<nl; ++i){
                temp = 1.0*i/nl;
                temp = temp*temp*(3.0-2.0*temp);
                r[i] = temp*r[nl];
        }

        for(i=n-nh; i<n; ++i){
                temp = 1.0*(n-1-i)/(n-nh);
		temp = temp*temp*(3.0*temp-2.0);
		r[i] = temp*r[n-nh-1];
	}
}

