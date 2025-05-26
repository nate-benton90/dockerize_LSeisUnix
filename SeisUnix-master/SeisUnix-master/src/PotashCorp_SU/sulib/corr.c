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
/* complex convolution with fft*/
void fcconv (int lx,int ifx, complex *x,
       	     int ly,int ify, complex *y,
	     int lz,int ifz, complex *z)
/* 
	Inputs x, y - arrays to be convolved
	z output
	
          lx           - dimension of x (signal )
          ly           - dimension of y (impulse response )
          lz           - dimension of z  ( There is no point making lz > lx)
	  ifz	       - firs value of z
*/ 
#define CSIZE sizeof(complex)
{
	
	int nfft;
	int nf;
	int i;
	int lyp2;
	float snfft;
	complex *fft1;
	complex *fft2;


	/* Set up pfa fft */
	nfft = npfa(lx+ly);
        if (nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
	snfft=1.0/nfft;
	nf=nfft/2+1;

	fft1 = ealloc1complex(nfft);
	fft2 = ealloc1complex(nfft);
	
	lyp2=ly/2+1;
	ify+=lyp2; 
	
	memcpy( (void *) fft1, (const void *) x, lx*CSIZE);
	wrap_array(&fft1[0],lx,CSIZE,ifx);
	memset( (void *) &fft1[lx],  (int) '\0', (nfft-lx)*CSIZE);
	
	memcpy( (void *) fft2, (const void *) y, ly*CSIZE);
	wrap_array(&fft2[0],ly,CSIZE,ify);
	memset( (void *) &fft2[ly],  (int) '\0', (nfft-ly)*CSIZE);
	
	pfacc(1,nfft,fft1);
	pfacc(1,nfft,fft2);
	for(i=0;i<nfft;i++) fft1[i] = cmul(fft1[i],fft2[i]);
	pfacc(-1,nfft,fft1);
	for(i=0;i<nfft;i++)
		fft1[i] = crmul(fft1[i],snfft);
	
	memset( (void *) z,  (int) '\0', lz*CSIZE);
	if(ifz<0) {
		memcpy( (void *) &z[-ifz], (const void *) &fft1[lyp2], MIN(MIN(lx+ifz-lyp2,nfft),lz)*CSIZE);
	} else {
		memcpy( (void *) z, (const void *) &fft1[ifz+lyp2], MIN(MIN(lx-ifz+lyp2,nfft),lz)*CSIZE);
	}
	
		
	free1complex(fft1);
	free1complex(fft2);
}
/* 2D complex convolution with fft*/
void fcconv2d(int lx2,int lx1,int ifx2,int ifx1, complex **x,
	     int ly2,int ly1,int ify2,int ify1, complex **y,
	     int lz2,int lz1,int ifz2,int ifz1, complex **z)
/*
	x	2d array of signal
	lx1,lx2 dimension of x
	fx1,fx2 first element of x
	
	y	2d array of signal
	
	ly1,ly2 dimension of y
	fy1,fy2 first element of y
	
	z       2d output array
	
	lz1,lz2 dimension of z
	fz1,fz2 first element of z
*/
	
{
	int nfft1;
	int nfft2;
	int nf1;
	int nf2;
	int i;
	float snfft;
	complex **fftx;
	complex **ffty;


	/* Set up pfa fft */
	nfft1 = npfa(lx1+ly1);
        if (nfft1 >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft1);
	nf1=nfft1/2+1;
	nfft2 = npfa(lx2+ly2);
        if (nfft2 >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft2);
	nf2=nfft2/2+1;
        snfft = 1.0/(nfft1*nfft2);
	
	fftx = ealloc2complex(nfft1,nfft2);
	ffty = ealloc2complex(nfft1,nfft2);
	
	ify1+=ly1/2+1;
	ify2+=ly2/2+1;
	
	for(i=0;i<lx2;i++) {
		memcpy((void *) fftx[i], (const void *) x[i],lx1*CSIZE);
		wrap_array(&fftx[i][0],lx1,CSIZE,ifx1);
		memset((void *) &fftx[i][lx1],  (int) '\0', (nfft1-lx1)*CSIZE);
	}
	for(i=lx2;i<nfft2;i++)
		memset( (void *) fftx[i],  (int) '\0', nfft1*CSIZE);

	for(i=0;i<ly2;i++) {
		memcpy((void *) ffty[i], (const void *) y[i],ly1*CSIZE);
		wrap_array(ffty[i],ly1,CSIZE,ify1);
		memset((void *) &ffty[i][ly1],  (int) '\0', (nfft1-ly1)*CSIZE);
	}
	for(i=ly2;i<nfft2;i++)
		memset((void *) ffty[i],  (int) '\0', nfft1*CSIZE);

	wrap_array(&fftx[0][0],lx2,nfft1*CSIZE,ifx2);
	wrap_array(&ffty[0][0],ly2,nfft1*CSIZE,ify2);
                               
	pfa2cc(-1,2,nfft1,nfft2,&fftx[0][0]);  
        pfa2cc(-1,1,nfft1,nfft2,&fftx[0][0]);
	pfa2cc(-1,2,nfft1,nfft2,&ffty[0][0]);  
        pfa2cc(-1,1,nfft1,nfft2,&ffty[0][0]);
	
	{ int i1,i2;
		for(i1=0;i1<nfft1;i1++) 
			for(i2=0;i2<nfft2;i2++)
				fftx[i2][i1] = cmul(fftx[i2][i1],ffty[i2][i1]);
	} 
	pfa2cc(1,2,nfft1,nfft2,&fftx[0][0]);  
        pfa2cc(1,1,nfft1,nfft2,&fftx[0][0]);
	
	{ int i1,i2;
		for(i1=0;i1<nfft1;i1++) 
			for(i2=0;i2<nfft2;i2++)
				fftx[i2][i1] = crmul(fftx[i2][i1],snfft);
	} 
	
	{ int ly1p2=ly1/2+1,ly2p2=ly2/2+1;
	  int zi;
	  int az1,af1,lim1;
	  int az2,af2,lim2;
		
		for(zi=0;zi<lz2;zi++)
			memset((void *) z[zi],  (int) '\0', lz1*CSIZE);
 		
		if(ifz2 <0) {
			az2=-ifz2; af2=ly2p2;     lim2=MIN(MIN(lx2+ly2p2,nfft2),lz2+ifz2);
		} else {
			az2=0;     af2=ly2p2+ifz2;lim2=MIN(MIN(lx2+ly2p2,nfft2),lz2-ifz2);
			fprintf(stderr," %d %d\n",af2,nfft2);
		}
		
 		if(ifz1 <0) {
			az1=-ifz1; af1=ly1p2;     lim1=MIN(MIN(lx1+ly1p2,nfft1),lz1+ifz2);
		} else {
			az1=0;     af1=ly1p2+ifz1;lim1=MIN(MIN(lx1+ly1p2,nfft1),lz1-ifz2);
		}
		
		for(i=af2,zi=az2;i<lim2;i++,zi++) {
				memcpy((void *) &z[zi][az1], (const void *) &fftx[i][af1],lim1*CSIZE);
		}
	
	}
}

#undef CSIZE
