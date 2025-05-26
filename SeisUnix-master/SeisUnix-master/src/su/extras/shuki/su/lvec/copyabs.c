#include <math.h>
copyabs(p,q,n)
register n;
register float *p,*q;
{
	while(n--)
		*q++ = fabs(*p++);
}
