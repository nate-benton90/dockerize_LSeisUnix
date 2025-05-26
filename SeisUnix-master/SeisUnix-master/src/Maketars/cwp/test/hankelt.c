/* compute Hankel transform of a jinc function (Bracewell, p. 249) */
#include "cwp.h"
#define N 512
#define RMAX 1
main()
{
	int i,n=N,nfft,nr,nk;
	float rmax=RMAX;
	float *f,*h,*e,*g,a,r,k,dr,dk;
	void *ht;

	a = 20.0*2.0*PI;
	nfft = npfar(2*(n-1));
	n = nfft/2+1;	
	nr = n;
	dr = rmax/(nr-1);
	nk = n;
	dk = 2.0*PI/(nfft*dr);
	f = alloc1float(nr);
	g = alloc1float(nr);
	h = alloc1float(nk);
	e = alloc1float(nk);
	fprintf(stderr,"nfft=%d  nr=%d  dr=%g  nk=%d  dk=%g\n",
		nfft,nr,dr,nk,dk);
	
	for (i=0,r=0.0; i<nr; ++i,r+=dr) {
		f[i] = (r==0.0 ? a*a/2.0 :a*j1(a*r)/r);
		/* f[i] *= 0.54+0.46*cos(PI*i/(nr-1)); */
	}		
	ht = hankelalloc(nfft);
	hankel0(ht,f,h);
	for (i=0,k=0.0; i<nk; ++i,k+=dk) {
		h[i] *= dr*dr;
		e[i] = (k<=a ? 1.0 : 0.0);
	}
	hankel0(ht,h,g);
	for (i=0; i<nr; ++i) {
		g[i] *= dk*dk;
		f[i] *= 2.0/(a*a);
		g[i] *= 2.0/(a*a);
	}
	hankelfree(ht);
	pplot1(stdout,"input to Hankel transform",nk,f);
	pplot1(stdout,"output from Hankel transform",nk,h);
}
