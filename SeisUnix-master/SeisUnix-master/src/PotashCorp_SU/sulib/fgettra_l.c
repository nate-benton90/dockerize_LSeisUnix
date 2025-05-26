#include "header.h"

unsigned long fgettra_l(FILE *fp, segy *tp, unsigned long itr)
{
	int nread;
	if(lastfp != fp)  searchlist(fp);  /* search for match */
		
	
	if(infoptr == (struct insegyinfo *) NULL) {
		/* get first trace */
		if(0 >= fgettr(fp, tp)) return(0); /* error return */

		switch(infoptr->ftype) {
			case TTY:
				warn("stdin not redirected");
				break;
			case DISK:	/* correct */
				break;
			default:
				err("%s: input must be disk file",__FILE__);
		}
		{ struct stat st;
 			fstat(fileno(fp),&st);
			infoptr->ntr = (unsigned long)((off_t) st.st_size/(off_t)infoptr->nsegy);
		}
	} /* end first entry initialization */
	
	/* Check on requested trace number */
	if(itr >= infoptr->ntr) {
		err("%s: trying to read off end of file",__FILE__);
	}
	
	/* Position file pointer at start of requested trace */
	fseeko(fp, (off_t)itr*infoptr->nsegy, SEEK_SET);
	
	nread=fgettr(fp, tp); /* let fgettr do the work */
	if(nread != infoptr->nsegy)
		err("%s: read %d bytes in trace of %d bytes",
		    __FILE__,nread,infoptr->nsegy);
	
	if(tp->ns != infoptr->nsfirst)
		warn("%s: header ns field = %d differs from first trace = %d",
		     __FILE__,tp->ns,infoptr->nsfirst);
	
	return(infoptr->ntr);
}
