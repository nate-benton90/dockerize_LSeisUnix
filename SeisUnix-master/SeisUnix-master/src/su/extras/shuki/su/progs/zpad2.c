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
