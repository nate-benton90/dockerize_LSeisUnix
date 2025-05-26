h03180
s 00000/00000/00025
d D 1.3 88/11/15 14:03:09 shuki 3 2
c 
e
s 00000/00000/00025
d D 1.2 88/05/25 14:54:12 shemer 2 1
c with SccsId[]
e
s 00025/00000/00000
d D 1.1 88/04/14 13:52:53 shuki 1 0
c date and time created 88/04/14 13:52:53 by shuki
e
u
U
f e 0
t
T
I 1
/* ZERO PADDING */
#include <stdio.h>
#include "../include/su.h"
extern int nalloc,dalloc;
extern bool verbose;
zpad2(acoff)
Section *acoff;
{
	int nxpad,ralloc;

	for(nxpad=1;nxpad<acoff->n2;nxpad*=2); lgetpar("nxpad",&nxpad);
	if(verbose) fprintf(stderr,"\tzpad: nxpad=%d\n",nxpad);

	ralloc = nxpad*acoff->n1*sizeof(float);
	if(ralloc>nalloc) {
		acoff->data = (float*) realloc(acoff->data,ralloc);
		if(acoff->data==NULL)
			err(__FILE__,__LINE__,"zpad: Can't realloc %d bytes",ralloc);
		nalloc = ralloc;
	}
	if(nxpad>acoff->n2)
		bzero(acoff->data+acoff->n1*acoff->n2,
			(nxpad-acoff->n2)*acoff->n1*sizeof(float));
	acoff->n2 = nxpad;
}
E 1
