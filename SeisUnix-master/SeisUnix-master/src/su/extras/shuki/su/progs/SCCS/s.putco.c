h18590
s 00000/00000/00033
d D 1.4 88/11/15 14:02:44 shuki 4 3
c 
e
s 00003/00003/00030
d D 1.3 88/06/06 13:12:19 shuki 3 2
c Cancel ns in trace headers
e
s 00000/00000/00033
d D 1.2 88/05/25 14:53:51 shemer 2 1
c with SccsId[]
e
s 00033/00000/00000
d D 1.1 88/04/14 13:52:37 shuki 1 0
c date and time created 88/04/14 13:52:37 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
#include "../include/su.h"

/* putco(outfd,nt,nx,offset,cdp0,dcdp,atr) */
/* int outfd,nt,nx,offset,cdp0,dcdp; */
/* Sutrace *atr; */

putco(outfd,acoff)
int outfd;
Section *acoff;
{
	int ix;
	static Sutrace tr;
	static bool first=true;

	if(first) {
D 3
		tr.dt = acoff->d1*1000000;
		tr.ns = acoff->n1;
		tr.data = (float*)malloc(tr.ns*sizeof(float));
E 3
I 3
/* 		tr.dt = acoff->d1*1000000; */
/* 		tr.ns = acoff->n1; */
		tr.data = (float*)malloc(acoff->n1*sizeof(float));
E 3
		first = false;
	}
	tr.offset = acoff->val.i;

	for(ix=0;ix<acoff->n2;ix++) {
		bcopy((char*)(acoff->data+ix*acoff->n1),
			(char*)tr.data,acoff->n1*sizeof(float));
		tr.tracl = ix;
		tr.cdp = acoff->o2 + ix*acoff->d2;
		tr.sx = tr.cdp + 0.5*tr.offset;
		tr.gx = tr.cdp - 0.5*tr.offset;
		puttr(outfd,&tr);
	}
}
E 1
