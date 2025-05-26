#include "su.h"
#include "header.h"
#include "segy.h"
#include "suhdr.h"

#define LOOKFAC	2	/* Look ahead factor for npfaro	  */
#define PFA_MAX	720720	/* Largest allowed nfft	          */

/* Fast crosscorelation with FFT */
void do_facor(float *data1,float *data2, float *acor,int n,int f,int nc)
/* Inputs data1, data2 - arrays to be xcorrelated
          n            - dimension of data1 and data2
	  acor         - xcorrelation array
	  nc           - size of acor
	  f            - first element of acor 
	  		f=0            - asymmetric output
	  		f=-nc/2        - symmetric output
*/ 

{

	int nfft;
	int nf;
	int i,j;
	float snfft;
	complex *fft1;
	complex *fft2;
	float *pdata1;
	float *pdata2;


	/* Set up pfa fft */
	nfft = npfaro(MAX(nc,n),LOOKFAC*MAX(nc,n));
        if (nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
	snfft=1.0/nfft;
	nf=nfft/2+1;

	fft1 = ealloc1complex(nf);
	fft2 = ealloc1complex(nf);
	pdata1 = ealloc1float(nfft);
	pdata2 = ealloc1float(nfft);
	
	memcpy( (void *) pdata1, (const void *) data1, n*FSIZE);
	memset( (void *) &pdata1[n],  (int) '\0', (nfft-n)*FSIZE);
	memcpy( (void *) pdata2, (const void *) data2, n*FSIZE);
	memset( (void *) &pdata2[n],  (int) '\0', (nfft-n)*FSIZE);
	
	pfarc(1,nfft,pdata1,fft1);
	pfarc(1,nfft,pdata2,fft2);
	for(i=0;i<nf;i++) fft1[i] = cmul(fft1[i],conjg(fft2[i]));
	pfacr(-1,nfft,fft1,pdata1);
	sscal(nfft,snfft,pdata1,1);
	
	{ int fst,flt;
	  int ast,alt;
		for(i=0,j=nf-1;i<nf;i++,j--) {
			pdata2[i]=pdata1[j];
		}
		for(i=nf,j=nfft-1;i<nfft;i++,j--) {
			pdata2[i]=pdata1[j];
		}
		fst=MIN(MAX(nf+f,0),nfft-1);
		flt=MIN(fst+nc,nfft-1);
		ast=MIN(0,nf+f-1);
		
		for(i=fst,j=abs(ast);i<flt;i++,j++) 
			acor[j]=pdata2[i];
	}
		
	free1complex(fft1);
	free1complex(fft2);
	free1float(pdata1);
	free1float(pdata2);
}

void do_bccorr(float *data2,float *data1, float *acor,int n,int fi,int nc,
	float f1f,float f2f,float dt)
/* Bicohherence correlation
 data2 is cross correlated with data1
 dimensions n for both 
 acor xcorrelation array
 fi first value of acor
 nc number of elements in array
 f1f, f2f filter boundaries to smooth the spectrum
 typical value for f1f=0.5/dt*0.2 f2f=0.5/dt*0.8
 dt sampling rate
 Smoothing is needed to get a consistent estimate
*/
{
complex **spr_ealloc(int n);
void spr_free(complex **n);

	int nfft;
	int nf;
	int i,j;
	float snfft;
	complex *ffty;
	complex *fftx;
	float *pdatay;
	float *pdatax;
	float *filter;
	float *amps;
	float *f;
	
	complex *Pyy;		/* auto correlation */
	complex *Pxx;		/* auto correlation */
	complex *LAxy;		
	complex **Bxyx;		/* cross-bispectrum */
	complex **Bxxx;		/* auto-bispectrum */
	complex **Bccxyx;	/* bicoherence-correlation */
	complex **Bccxxx;	/* bicoherence-correlation */
	complex **Gxyx;		/* bicoherence-ratio */
	complex **tmpcmp;	/* tmporary complex storage */


	/* Set up pfa fft */
	nfft = npfar(n);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
	snfft=1.0/nfft;
	nf=nfft/2+1;
	if(abs(fi) > nc ) 
		err(" First element of correlation is out of bound\n");
	if(nc>n) 
		err(" Length of corr is larger than the length of input \n");
	

	ffty = ealloc1complex(nf);
	fftx = ealloc1complex(nf);
	Pyy = ealloc1complex(nf);
	Pxx = ealloc1complex(nf);
	Bxxx = spr_ealloc(nf);
	Bxyx = spr_ealloc(nf);
	pdatay = ealloc1float(nfft);
	pdatax = ealloc1float(nfft);
	
	
	/* build a filter to smooth the spectra */
	/* this is nedded to get a consistent estimate */
	filter = ealloc1float(nf);
	f=ealloc1float(4);
	amps=ealloc1float(4);
	f[0]=f1f-f1f*0.1; amps[0]=0.0;
	f[1]=f1f;        amps[1]=1.0;
	f[2]=f2f;        amps[2]=1.0;
	f[3]=f2f+f2f*0.1; amps[3]=0.0;
	polygonalFilter(f,amps,4,nfft,dt,filter);
	
	memcpy( (void *) pdatay, (const void *) data1, n*FSIZE);
	memset( (void *) &pdatay[n],  (int) '\0', (nfft-n)*FSIZE);
	memcpy( (void *) pdatax, (const void *) data2, n*FSIZE);
	memset( (void *) &pdatax[n],  (int) '\0', (nfft-n)*FSIZE);
	
	pfarc(1,nfft,pdatay,ffty);
	pfarc(1,nfft,pdatax,fftx);
	{ register int if1,if2;
		/* smooth the spectras */
		for(if1=0;if1<nf;if1++) crmul(ffty[if1],filter[if1]);
		for(if2=0;if2<nf;if2++) crmul(fftx[if2],filter[if2]);
				
		/* auto spectrums */ 
		for(if1=0;if1<nf;if1++) Pyy[if1] = cmul(ffty[if1],conjg(ffty[if1]));
		for(if2=0;if2<nf;if2++) Pxx[if2] = cmul(fftx[if2],conjg(fftx[if2]));
		
		tmpcmp = spr_ealloc(nf);
		
		/* The two bispectras */
		for(if1=0;if1<nf;if1++)
			for(if2=0;if2<nf-if1;if2++)
				tmpcmp[if1][if2] = cmul(fftx[if2],conjg(fftx[if1+if2]));
				
		for(if1=0;if1<nf;if1++)
			for(if2=0;if2<nf-if1;if2++) {
				Bxyx[if1][if2] = cmul(ffty[if1],tmpcmp[if1][if2]);
				Bxxx[if1][if2] = cmul(fftx[if1],tmpcmp[if1][if2]);
			}
		free1complex(ffty);
		free1complex(fftx);
		
		/* Two bicoherences */
		for(if1=0;if1<nf;if1++)
			for(if2=0;if2<nf-if1;if2++)
				tmpcmp[if1][if2] = cmul(Pxx[if2],Pxx[if1+if2]);

		Bccxyx = spr_ealloc(nf);
		Bccxxx = spr_ealloc(nf);
		for(if1=0;if1<nf;if1++)
			for(if2=0;if2<nf-if1;if2++) {
				Bccxyx[if1][if2]=cdiv(Bxyx[if1][if2],
					csqrt(cmul(Pyy[if1],tmpcmp[if1][if2])));
				Bccxxx[if1][if2]=cdiv(Bxxx[if1][if2],
					csqrt(cmul(Pxx[if1],tmpcmp[if1][if2])));
			}
		spr_free(tmpcmp);
		free1complex(Pyy);
		free1complex(Pxx);
		spr_free(Bxxx);
		spr_free(Bxyx);

		Gxyx = spr_ealloc(nf);
		/* Bicoherence ratio */
		for(if1=0;if1<nf;if1++)
			for(if2=0;if2<nf-if1;if2++)
				Gxyx[if1][if2] = cdiv(Bccxyx[if1][if2],Bccxxx[if1][if2]);

/*		for(if1=0;if1<nf;if1++) 
			for(if2=nf-if1;if2<nf;if2++)
				Gxyx[if1][if2] = Gxyx[nf-1-if2][nf-1-if1];
*/		
		spr_free(Bccxxx);
		spr_free(Bccxyx);
		LAxy = ealloc1complex(nf);
		/* Bicoherence correlation */
		for(if1=0;if1<nf;if1++) {
			for(if2=0;if2<nf-if1;if2++) {
				LAxy[if1] = cadd(LAxy[if1],Gxyx[if1][if2]);
			}
			crmul(LAxy[if1],(float)(if2));
		}

	}
	spr_free(Gxyx);
	
	pfacr(-1,nfft,LAxy,pdatay);
	sscal(nfft,snfft,pdatay,1);
	
	/* wraparound order */
	/* unwrap it */
	{ int fst,flt;
	  int ast,alt;
		for(i=0,j=nf-1;i<nf;i++,j--) {
			pdatax[i]=pdatay[j];
		}
		for(i=nf,j=nfft-1;i<nfft;i++,j--) {
			pdatax[i]=pdatay[j];
		}
		fst=MIN(MAX(nf+fi,0),nfft-1);
		flt=MIN(fst+nc,nfft-1);
		ast=MIN(1,nf+fi-1);
		
		for(i=fst,j=abs(ast);i<flt;i++,j++) 
			acor[j]=pdatax[i];
	}

	free1complex(LAxy);
	free1float(pdatay);
	free1float(pdatax);
	free1float(f);
	free1float(amps);
	free1float(filter);
}
complex **spr_ealloc(n)
/* This is a special matrix allocation function 
 only the lower trinagular are of the matrix is allocated
 and valid, marked with o on the diagram:
 	     oxxxx
	     oxxxx
	     oooxx
	     oooox
	     ooooo
 The valid number of elements: 
 		for even n:    (1+n)*n/2
		for odd n:    (1+n)*(n-1)/2+n+1/2
*/ 
{

	complex **f;
	int size;
	
	if(n%2==0) {
		size=(1+n)*n/2;
	} else {
		size=(1+n)*(n-1)/2+(n+1)/2;
	}
	
	f = ealloc1(n,sizeof(complex*));
	{ register int i;
		f[0] = ealloc1(size,sizeof(complex));
		for(i=1;i<n;i++) 
			f[i] = f[i-1]+n-i+1;
	}
	return(f);
}
void spr_free(complex **f)
{
	free1(f[0]);
	free1(f);
}

/* complex correlation */
void cxcor (int lx, int ifx, complex *x,
            int ly, int ify, complex *y, 
            int lz, int ifz, complex *z)
{
        int i,j;
	complex *xr;
	xr = alloc1complex(lx);
	for (i=0,j=lx-1; i<lx; ++i,--j)
	        xr[i] = conjg (x[j]);
	cconv(lx,1-ifx-lx,xr,ly,ify,y,lz,ifz,z);
	free1complex(xr);
}

/* complex convolution */
void cconv (int lx, int ifx, complex *x,
       	    int ly, int ify, complex *y,
	    int lz, int ifz, complex *z)
{
        int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,i,j,jlow,jhigh;
	complex sum;
  
	x -= ifx;  y -= ify;  z -= ifz; 
	for (i=ifz; i<=ilz; ++i) {
	        jlow = MAX(i-ily,ifx);/*  if (jlow<ifx) jlow = ifx; */
		jhigh = MIN(i-ify,ilx);/*  if (jhigh>ilx) jhigh = ilx; */
		for (j=jlow,sum=cmplx(0.,0.); j<=jhigh; ++j){
		        sum = cadd(sum,cmul(x[j],y[i-j]));
		}
		z[i] = sum;
	}
}
