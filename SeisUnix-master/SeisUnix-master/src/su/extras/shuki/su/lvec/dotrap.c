#include <math.h>
dotrap(trap,n,p)
float trap,*p;
{
	register nn;
	register float *pp,t;
	pp = p;
	t = trap;
	nn = n;
	do {
		if( fabs( *pp ) > t ) *pp = 0.0;
		pp++;
	} while(--nn);
}
