h45778
s 00001/00001/00024
d D 1.2 88/11/15 14:03:36 shuki 2 1
c 
e
s 00025/00000/00000
d D 1.1 88/06/15 14:41:21 shemer 1 0
c date and time created 88/06/15 14:41:21 by shemer
e
u
U
t
T
I 1
#include <stdio.h>
D 2
#include "/src/su/include/su.h"
E 2
I 2
#include "../include/su.h"
E 2


putcdf(outfd,data,cdf,vel,i)
int outfd,i;
float cdf,vel;
float *data;
{
	int ix;
	static Sutrace tr;
	static bool first=true;

	if(first) {
		tr.data = (float*)malloc(i*sizeof(float));
		first = false;
	}
	tr.offset = vel;

		bcopy((char*)(data),
			(char*)tr.data,i*sizeof(float));
		tr.offset = vel;
		tr.cdp = cdf;
		puttr(outfd,&tr);
}
E 1
