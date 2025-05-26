/* SUSLOPEF: $Revision: 1.2 $ ; $Date: 90/12/18 20:01:08 $		*/

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

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUSLOPEF - SLOPE Filter in the frequency-wavenumber domain	\n"
" 								\n"
" slopef <infile >outfile [optional parameters]			\n"
"								\n"
" Required Parameters:						\n"
" none                     					\n"
"								\n"
" Optional parameters:						\n"
" dt=(header or 1.0)	time sampling interval			\n"
" dx=1.0		trace spacing				\n"
" slopes=0.0		monotonically increasing slopes		\n"
" amps=1.0		amplitudes corresponding to slopes	\n"
" bias=0.0		slope made horizontal before filtering	\n"
"								\n"
" Notes:							\n"
" Linear interpolation and constant extrapolation are used to	\n"
" determine amplitudes for slopes that are not specified.	\n"
" Linear moveout is removed before slope filtering to make	\n"
" slopes equal to bias appear horizontal.  This linear moveout	\n"
" is restored after filtering.  The bias parameter may be	\n"
" useful for spatially aliased data.  The slopes parameter is	\n"
" compensated for bias, so you need not change slopes when you	\n"
" change bias.							\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *
 *	CWP: Dave (algorithm), Jack (reformatting for SU)
 */


#define PFA_MAX	720720	/* Largest allowed nfft	          */

/* prototype */
void slopefilter (int nslopes, float slopes[], float amps[], float bias,
	int nt, float dt, int ntr, float dx, FILE *datafp);

segy tr;

main(int argc, char **argv)
{
	int nt;		/* number of time samples			*/
	float dt;	/* time sampling interval			*/
	int ntr;	/* number of traces				*/
	float dx;	/* trace spacing (spatial sampling interval)	*/
	int nslopes;	/* number of slopes specified			*/
	float *slopes;	/* slopes at which amplitudes are specified	*/
	int namps;	/* number of amplitudes specified		*/
	float *amps;	/* amplitudes corresponding to slopes		*/
	float bias;	/* slope bias					*/
	FILE *hdrfp;	/* fp for header storage file			*/
	FILE *datafp;	/* fp for trace storage file			*/


	/* Hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);


	/* Get info from first trace */ 
	if (!gettr(&tr))  err("can't get first trace");
	nt = tr.ns;


	/* Get parameters */
	dt = (float) tr.dt/1000000.0;
	if (!dt) getparfloat("dt", &dt);
	if (!dt) dt = 1.0;
	if (!getparfloat("dx", &dx)) dx = 1.0;
	slopes = alloc1float(countparval("slopes"));
	amps = alloc1float(countparval("amps"));
	if (!(nslopes = getparfloat("slopes", slopes))) {
		nslopes = 1;
		slopes[0] = 0.0;
	}
	if (!(namps = getparfloat("amps", amps))) {
		namps = 1;
		amps[0] = 1.0;
	}
	if (!getparfloat("bias", &bias)) bias = 0.0;


	/* Check parameters */
	if (nslopes != namps)
		err("number of slopes (%d) must equal number of amps(%d)",
			nslopes, namps);
	{ register int i;
	  for (i=1; i<nslopes; ++i)
		if (slopes[i] <= slopes[i-1])
			err("slopes must be monotonically increasing");
	}


	/* Store traces and headers in tmpfile while getting a count */
	hdrfp  = etmpfile();
	datafp = etmpfile();
	ntr = 0;
	do { 
		++ntr;
		efwrite(&tr, 1, HDRBYTES, hdrfp);
		efwrite(tr.data, FSIZE, nt, datafp);
	} while (gettr(&tr));


	/* Apply slope filter */
	slopefilter(nslopes,slopes,amps,bias,nt,dt,ntr,dx,datafp);


	/* Output filtered traces */
	rewind(hdrfp);
	rewind(datafp);
	{ register int itr;
	  for (itr = 0; itr < ntr; ++itr) {
		efread(&tr, 1, HDRBYTES, hdrfp);
		efread(tr.data, FSIZE, nt, datafp);
		puttr(&tr);
	  }
	}

}




void slopefilter (int nslopes, float slopes[], float amps[], float bias,
	int nt, float dt, int nx, float dx, FILE *datafp)
/******************************************************************************
apply slope filter in frequency-wavenumber domain
*******************************************************************************
Input:
nslopes		number of slopes (and amplitudes) specified
slopes		slopes at which amplitudes are specified (see notes below)
amps		amplitudes corresponding to slopes (see notes below)
bias		linear moveout slope before and after filtering
nt		number of time samples
dt		time sampling interval
nx		number of traces
dx		trace space (spatial sampling interval)
datafp		file pointer to data to be filtered

Output:
datafp		file pointer to filtered data
*******************************************************************************
Notes:
Linear interpolation and constant extrapolation are used to
determine amplitudes for slopes that are not specified.
******************************************************************************/
{
	int ntfft;		/* nt after padding for FFT */
	int nxfft;		/* nx after padding for FFT */
	float sfft;		/* scale factor for FFT */
	int nw;			/* number of frequencies */
	float dw;		/* frequency sampling interval */
	float fw;		/* first frequency */
	int nk;			/* number of wavenumbers */
	float dk;		/* wavenumber sampling interval */
	float fk;		/* first wavenumber */
	float w,k;		/* frequency and wavenumber */
	int it,ix,iw,ik;	/* sample indices */
	float slope,amp;	/* slope and amplitude for particular w,k */
	complex **cpfft;	/* complex FFT workspace */
	float **pfft;		/* float FFT workspace */
	float phase;		/* phase shift for bias */
	complex cshift;		/* complex phase shifter for bias */

	/* determine lengths and scale factors for prime-factor FFTs */
	ntfft = npfar(nt);
	nxfft = npfa(nx);
	if (ntfft >= MIN(SU_NFLTS, PFA_MAX)) err("Padded nt=%d--too big",ntfft);
	if (nxfft >= MIN(SU_NFLTS, PFA_MAX)) err("Padded nx=%d--too big",nxfft);
	sfft = 1.0/(ntfft*nxfft);
	
	/* determine frequency and wavenumber sampling */
	nw = ntfft/2+1;
	dw = 2.0*PI/(ntfft*dt);
	fw = 0.000001*dw; /* non-zero to avoid divide by zero w */
	nk = nxfft;
	dk = 2.0*PI/(nxfft*dx);
	fk = 0.0;

	/* allocate real and complex workspace for FFTs */
	cpfft = alloc2complex(nw,nk);
	pfft = alloc2float(ntfft,nxfft);

	/* copy data from input to FFT array and pad with zeros */
	rewind(datafp);
	for (ix=0; ix<nx; ix++) {
		efread(pfft[ix], FSIZE, nt, datafp);
		for (it=nt; it<ntfft; it++)
			pfft[ix][it] = 0.0;
	}
	for (ix=nx; ix<nxfft; ix++)
		for (it=0; it<ntfft; it++)
			pfft[ix][it] = 0.0;
	
	/* Fourier transform t to w */
	pfa2rc(1,1,ntfft,nx,pfft[0],cpfft[0]);
	
	/* do linear moveout bias via phase shift */
	for (ix=0; ix<nx; ix++) {
		for (iw=0,w=0.0; iw<nw; iw++,w+=dw) {
			phase = -ix*dx*w*bias;
			cshift = cmplx(cos(phase),sin(phase));
			cpfft[ix][iw] = cmul(cpfft[ix][iw],cshift);
		}
	}
	
	/* Fourier transform x to k */
	pfa2cc(-1,2,nw,nxfft,cpfft[0]);
	
	/* loop over wavenumbers */
	for (ik=0; ik<nk; ik++) {
	
		/* determine wavenumber */
		k = (ik<=nk/2) ? ik*dk : (ik-nk)*dk;
		
		/* loop over frequencies */
		for (iw=0,w=fw; iw<nw; iw++,w+=dw) {
		
			/* determine biased slope */
			slope = k/w+bias;
			
			/* linearly interpolate to find amplitude */
			intlin(nslopes,slopes,amps,amps[0],amps[nslopes-1],
				1,&slope,&amp);
			
			/* include fft scaling */
			amp *= sfft;
			
			/* filter real and imaginary parts */
			cpfft[ik][iw].r *= amp;
			cpfft[ik][iw].i *= amp;
		}
	}

	/* Fourier transform k to x */
	pfa2cc(1,2,nw,nxfft,cpfft[0]);

	/* undo linear moveout bias via phase shift */
	for (ix=0; ix<nx; ix++) {
		for (iw=0,w=0.0; iw<nw; iw++,w+=dw) {
			phase = ix*dx*w*bias;
			cshift = cmplx(cos(phase),sin(phase));
			cpfft[ix][iw] = cmul(cpfft[ix][iw],cshift);
		}
	}

	/* Fourier transform w to t */
	pfa2cr(-1,1,ntfft,nx,cpfft[0],pfft[0]);
	
	/* copy filtered data from FFT array to output */
	rewind(datafp);
	for (ix=0; ix<nx; ix++)
		efread(pfft[ix], FSIZE, nt, datafp);

	/* free workspace */
	free2complex(cpfft);
	free2float(pfft);
}
