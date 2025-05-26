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
/* 		tr.dt = acoff->d1*1000000; */
/* 		tr.ns = acoff->n1; */
		tr.data = (float*)malloc(acoff->n1*sizeof(float));
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
