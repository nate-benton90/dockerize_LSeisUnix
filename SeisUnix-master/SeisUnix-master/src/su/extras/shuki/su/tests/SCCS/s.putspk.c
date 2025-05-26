h26433
s 00000/00000/00015
d D 1.2 88/11/15 14:19:43 shuki 2 1
c 
e
s 00015/00000/00000
d D 1.1 88/05/05 07:25:31 shuki 1 0
c date and time created 88/05/05 07:25:31 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
putspk(f,t,nt)
float *f,t;
{
	int i,i0;

/* 	fprintf(stderr,"putspk: nt=%d t=%f\n",nt,t); */

	i0 = t;

	if(i0<0) return;
	if(i0>nt-2) return;
	f[i0+1] += t - i0;
	f[i0]   += 1.0 - f[i0+1];
}
E 1
