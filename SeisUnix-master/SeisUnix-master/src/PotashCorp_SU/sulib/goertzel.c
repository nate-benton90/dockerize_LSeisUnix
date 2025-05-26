#include "suhdr.h"

complex goertzel_r(float *a, int n, int k)
{

	int i;
	complex r;

	float w=2.0*PI/n*k;
	float sa=sin(w);
	float ca=cos(w);
	float a1 = 2.0*ca;
	float U1=0.0,U2=0.0,U0;
	float W1=0.0,W2=0.0,W0;
	float as;

	for(i=0;i<n;i++) {
		as = a[i]; 
		U0 = as + a1 * U1 - U2;
/*		W0 = as*(float)i + a1 * W1 - W2;  */
		U2 = U1;
		U1 = U0;
/*		W2 = W1;
		W1 = W0; */
	}
	
	{ float A,B,C,D,A2;
		A = U1 - U2*ca;
		B = U2 * sa;
/*		C = W1 - W2*ca;
		D = W2*sa; */
		A2 = w * (float)(n-1);
		U1 = cos(A2);
		U2 = -sin(A2);
		
		r.r = U1*A - U2*B;
		r.i = U2*A + U1*B;
	}
	return(r);	
}

complex goertzel_c(complex *a, int n, int k,int init)
{

	int i;
	complex r;

	static float w;
	static float sa;
	static float ca;
	static float a1;
	static float A2;
	complex U1,U2,U0;
	complex as;

	if (init) {
		w=2.0*PI/n*k;
		sa=sin(w);
		ca=cos(w);
		a1 = 2.0*ca;
		A2 = w * (float)(n-1);
	}
	
	U1=cmplx(0.0,0.0);
	U2=U1;	

	for(i=0;i<n;i++) {
		as = a[i]; 
		U0 = cadd(as,csub(crmul(U1,a1),U2));
		U2 = U1;
		U1 = U0;
	}
	{float A,B;
		complex W;
			A = (U1.r-U2.r*ca) - U2.i*sa;
			B = U2.r*sa + (U1.i-U2.i*ca);
			W  = cmplx(cos(A2),-sin(A2));
		r = cmul(W,cmplx(A,B));
	}
		
	return(r);	
}
