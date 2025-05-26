/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

#include "cwp.h"

#define NX 10
#define NK 20
#define NTEST 100

main()
{
	int ik,ix,itest;
	float dx=1.413,knyq=PI/dx,dk=knyq/NK,fk=dk,xout=0.0;
	float fx,xin[NX],yin[NX],err[NK],x,k,yout,errnow;

	/* loop over k */
	for (ik=0,k=fk; ik<NK; ik++,k+=dk) {

		/* loop over tests */
		for (itest=0; itest<NTEST; itest++) {

			/* determine random first x */
			fx = (-NX/2+franuni())*dx;

			/* fill array with sine wave */
			for (ix=0,x=fx; ix<NX; ix++,x+=dx) {
				xin[ix] = x;
				yin[ix] = sin(k*x);
			}

			/* interpolate (correct yout is 0.0 = sin(k*0.0) */
			intlin(NX,xin,yin,0.0,0.0,1,&xout,&yout);

			/* store percentage error */
			errnow = fabs(yout)*100.0;
			err[ik] = MAX(errnow,err[ik]);
		}
	}
	pp1d(stdout,"percentage error",NK,0,err);
}
