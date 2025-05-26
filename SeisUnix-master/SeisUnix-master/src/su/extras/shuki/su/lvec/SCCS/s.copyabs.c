h19351
s 00000/00000/00008
d D 1.2 88/11/15 14:04:00 shuki 2 1
c 
e
s 00008/00000/00000
d D 1.1 88/04/14 13:49:50 shuki 1 0
c date and time created 88/04/14 13:49:50 by shuki
e
u
U
f e 0
t
T
I 1
#include <math.h>
copyabs(p,q,n)
register n;
register float *p,*q;
{
	while(n--)
		*q++ = fabs(*p++);
}
E 1
