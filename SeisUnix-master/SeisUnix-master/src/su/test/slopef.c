char *sdoc =
"SUSLOPEF - SLOPE Filter in the frequency-wavenumber domain\n"
"\n"
"slopef <infile >outfile nt= nx= [optional parameters]\n"
"\n"
"Required Parameters:\n"
"nt                     number of time samples\n"
"nx                     number of traces\n"
"\n"
"Optional Parameters:\n"
"dt=1.0                 time sampling interval\n"
"dx=1.0                 trace spacing\n"
"slopes=0.0             monotonically increasing slopes\n"
"amps=1.0               amplitudes corresponding to slopes\n"
"bias=0.0               slope made horizontal before filtering\n"
"\n"
"Notes:\n"
"Linear interpolation and constant extrapolation are used to determine\n"
"amplitudes for slopes that are not specified.\n"
"Linear moveout is removed before slope filtering to make slopes equal\n"
"to bias appear horizontal.  This linear moveout is restored after\n"
"filtering.  The bias parameter may be useful for spatially aliased\n"
"data.  The slopes parameter is compensated for bias, so you need not\n"
"change slopes when you change bias.\n"
"\n";

#include "par.h"

/* prototypes for functions defined and used below */
void slopefilter (int nslopes, float slopes[], float amps[], float bias,
	int nt, float dt, int nx, float dx, float **p);

/* the main program */
main (int argc, char **argv)
{
	int nt;			/* number of time samples */
	float dt;		/* time sampling interval */
	int nx;			/* number of traces */
	float dx;		/* trace spacing (spatial sampling interval) */
	int ix;			/* trace index */
	int i;			/* general-purpose index */
	float **p;		/* pointer to input/output data */
	int nslopes;		/* number of slopes specified */
	float *slopes;		/* slopes at which amplitudes are specified */
	int namps;		/* number of amplitudes specified */
	float *amps;		/* amplitudes corresponding to slopes */
	float bias;		/* slope bias */
	FILE *infp=stdin;	/* input file pointer = standard input */
	FILE *outfp=stdout;	/* output file pointer = standard output */

	/* hook up getpar to handle the parameters */
	initargs(argc,argv);

	/* get required parameters */
	if (!getparint("nt",&nt)) selfdoc();
	if (!getparint("nx",&nx)) selfdoc();
	
	/* get optional parameters */
	if (!getparfloat("dt",&dt)) dt = 1.0;
	if (!getparfloat("dx",&dx)) dx = 1.0;
	slopes = alloc1float(countparval("slopes"));
	amps = alloc1float(countparval("amps"));
	if ((nslopes=getparfloat("slopes",slopes))==0) {
		nslopes=1;
		slopes[0] = 0.0;
	}
	if ((namps=getparfloat("amps",amps))==0) {
		namps=1;
		amps[0] = 1.0;
	}
	if (!getparfloat("bias",&bias)) bias = 0.0;

	/* check parameters */
	if (nslopes!=namps)
		err("number of slopes must equal number of amps!");
	for (i=1; i<nslopes; i++)
		if (slopes[i]<=slopes[i-1])
			err("slopes must be monotonically increasing!");

	/* allocate pointer for all input/output data */
	p = alloc2float(nt,nx);

	/* read all of the data */
	fread(p[0],sizeof(float),nt*nx,infp);
	
	/* apply slope filter */
	slopefilter(nslopes,slopes,amps,bias,nt,dt,nx,dx,p);

	/* write all of the data */
	fwrite(p[0],sizeof(float),nt*nx,outfp);
}

void slopefilter (int nslopes, float slopes[], float amps[], float bias,
	int nt, float dt, int nx, float dx, float **p)
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
p		data to be filtered

Output:
p		filtered data
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
	for (ix=0; ix<nx; ix++) {
		for (it=0; it<nt; it++)
			pfft[ix][it] = p[ix][it];
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
	for (ix=0; ix<nx; ix++)
		for (it=0; it<nt; it++)
			p[ix][it] = pfft[ix][it];

	/* free workspace */
	free2complex(cpfft);
	free2complex(pfft);
}
