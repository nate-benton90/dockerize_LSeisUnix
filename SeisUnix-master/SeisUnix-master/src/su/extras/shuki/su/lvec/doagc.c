#include <math.h>
doagc(wagc,n,p)
float wagc,*p;
{
 	static int first=1;
	static float *s1,*s2,*s1e,ro,eps;
	register float *q1,*q2,*q3;
	int np;

	if(wagc==0.0) return;
	if(first) {
		eps = 1./wagc;
		ro = 1. - eps;
		s1 = (float*) malloc(n*sizeof(float));
		s2 = (float*) malloc(n*sizeof(float));
		s1e = s1 + n;
		first=0;
	}
	np = 2;

	/* slow equivalent */
	/*
	for(j=0;j<n;j++)
		s1[j] = fabs(p[j]);
	while (np--) {
		for(s2[0]=s1[0],j=1;j<n;j++)
			s2[j] = ro*s2[j-1] + eps*s1[j];
		for(s1[n-1]=s2[n-1],j=n-2;j>=0;j--)
			s1[j] = ro*s1[j+1] + eps*s2[j];
	}
	for(j=0;j<n;j++)
		p[j] /= s1[j];
	*/

	/* fast equivalent */
	for(q1=p,q2=s1;q2<s1e;q1++,q2++)
		*q2 = fabs(*q1);

	while (np--) {
		for ( s2[0]=s1[0],
			q1=s2+1,q2=s2,q3=s1+1;
			q3<s1e;
			q1++,q2++,q3++
		)
			*q1 = ro* *q2 + eps* *q3;

		for( s1[n-1]=s2[n-1],
			q1=s1+n-2,q2=s1+n-1,q3=s2+n-2;
			q1>=s1;
			q1--,q2--,q3--
		)
			*q1 = ro* *q2 + eps* *q3;
	}
	for(q1=p,q2=s1;q2<s1e;q1++,q2++)
		*q1 /= *q2;

}
