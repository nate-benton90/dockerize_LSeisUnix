#include "cwp.h"

#define NX 10
#define NK 20
#define NTEST 100

main()
{
	int ik,ix,itest;
	float dx=1.413,knyq=PI/dx,dk=knyq/NK,fk=dk,xout=0.0;
	float fx,err[NK],x,k,errnow;
	complex yin[NX],yinl,yinr,yout;

	yinl = cmplx(0.0,0.0);
	yinr = cmplx(0.0,0.0);

	/* loop over k */
	for (ik=0,k=fk; ik<NK; ik++,k+=dk) {

		/* loop over tests */
		for (itest=0; itest<NTEST; itest++) {

			/* determine random first x */
			fx = (-NX/2+franuni())*dx;

			/* fill array with sine wave */
			for (ix=0,x=fx; ix<NX; ix++,x+=dx)
				yin[ix] = cmplx(sin(k*x),cos(k*x));

			/* interpolate (correct yout is 0.0 = sin(k*0.0) */
			ints8c(NX,dx,fx,yin,yinl,yinr,1,&xout,&yout);

			/* store percentage error */
			errnow = fcabs(csub(yout,cmplx(0.0,1.0)))*100.0;
			err[ik] = MAX(errnow,err[ik]);
		}
	}
	pp1d(stdout,"percentage error",NK,0,err);
}
