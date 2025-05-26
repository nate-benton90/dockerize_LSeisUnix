h24886
s 00000/00000/00014
d D 1.2 88/11/15 14:04:08 shuki 2 1
c 
e
s 00014/00000/00000
d D 1.1 88/04/14 13:49:58 shuki 1 0
c date and time created 88/04/14 13:49:58 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
