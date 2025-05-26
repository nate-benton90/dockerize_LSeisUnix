/*
 * dotpow (tpow,n,p)
 *
 * tpow a single vector:
 *	p[j] *= ((j+1)/n)**tpow for j=0,n-1
 */

#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT HUGE
#endif
dotpow(tpow,n,p)
float tpow,*p;
int n;
{
	int j;
	static int first=1,maxn;
	static float *tp,lasttpow;
	float maxpow;

	/* MUST ALLOCATE THE FIRST TIME AROUND */
	if(first) {
		tp = (float*) malloc(n*sizeof(float));
		maxn = n;
	}

	/* MUST REALLOCATE IF BIGGER THAN EVER */
	if(maxn<n) {
		tp = (float*) realloc(tp,n*sizeof(float));
	}

	/* SOMETIMES MUST CALCUATE OPERTOR */
	if(first||lasttpow!=tpow||maxn<n) {

		/* CHECK TPOW FOR FLOATING EXCEPTIONS */
		maxpow = log(MAXFLOAT)/log((float)n);
		if(maxpow<=0.0)
			warn(__FILE__,__LINE__,"dotpow: invalid floating exception check (maxpow=%f)",
									maxpow);
		if(tpow>maxpow)
			warn(__FILE__,__LINE__,"dotpow: may underflow tpow=%f (>%f)",tpow,maxpow);
		if(tpow< -maxpow)
			warn(__FILE__,__LINE__,"dotpow: may overflow tpow=%f (<-%f)",tpow,maxpow);

		for(j=0;j<n;j++) {
			tp[j] = exp(tpow*log((1.0+j)/n));
		}
	}

	/* APPLY THE OPERATOR */
	for(j=0;j<n;j++)
		p[j] *= tp[j];

	/* REMEMEBR WHAT YOU HAVE */
	if(maxn<n) maxn = n;
	first=0;
	lasttpow = tpow;
}
