/*
 * gpow - Gain by taking the trace at power gpow
 */
#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT HUGE
#endif
#include "../include/su.h"
dogpow(gpow,nsample,ibuf)
int nsample;
float *ibuf,gpow;
{
	int i;
	float maxval;
	/* check gpow for floating exceptions */
	for (i=0,maxval=0.0;i< nsample;i++ ) {
		maxval=MAX(maxval,ibuf[i]);
		maxval=MAX(maxval,-ibuf[i]);
	}
	maxval=pow(maxval,gpow);
	if (maxval > MAXFLOAT)
		warn(__FILE__,__LINE__," dogpow: may overflow gpow=%f maxval=%f",gpow,maxval);
	/* do the power */
	for(i=0;i<nsample;i++) {
		if (ibuf[i]< 0.0) {
			ibuf[i] = -ibuf[i];
			ibuf[i] = pow(ibuf[i],gpow);
			ibuf[i] = -ibuf[i];
		}
		else 
			ibuf[i] = pow(ibuf[i],gpow);
	}
}
