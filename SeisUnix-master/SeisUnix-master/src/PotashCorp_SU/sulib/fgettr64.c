#include "su.h"
#include "segy.h"          
#include "suhdr.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define _LARGEFILE64_SOURCE
#define O_LARGEFILE     0100000

static struct insegyinfo64{
	int *infd;                   /* File descriptor ptr for search	 */
	struct insegyinfo *nextinfo; /* linked list pointer      */
	unsigned long itr;	     /* number of traces read	 */
	unsigned int nsfirst;	     /* samples from 1st header	 */
	unsigned short bytesper;     /* bytes per datum		 */
	unsigned int nsegy; 	     /* segy bytes from nsfirst	 */
	unsigned int ntr;            /* traces in input,if known */
	FileType ftype;		     /* file type of input *fd	 */
} *insegylist64 = (struct insegyinfo64 *) NULL;

static int *lastfd = (int *) NULL;
static struct insegyinfo64 *infoptr64, **oldinfoptr64;

static void searchlist64(int *fd)
{
	oldinfoptr64 = &insegylist64;
	for(infoptr64 = insegylist64; infoptr64 != ((struct insegyinfo64 *) NULL);
		infoptr64 = infoptr64->nextinfo64) {
			if(fd == infoptr64->infd) break;
			oldinfoptr64 = &infoptr64->nextinfo64;
		}
}

static ssize_t dataread64(segy *tp, struct insegyinfo64 *iptr, cwp_Bool fixed_length)
{
	unsigned int nsread = fixed_length?iptr->nsfirst:tp->ns;
	
	unsigned int databytes = infoptr64->bytesper*nsread;
	
	ssize_t nread = (unsigned int )read(iptr->infd,(char *)(&((tp->data)[0])),databytes);

	if(nread > 0 && nread != databytes) 
		err("%s: on trace #%ld, tried to read %d bytes, "
		    "read %d bytes ",
		    __FILE__, (infoptr64->itr)+1, databytes, nread);

	return(nread);
}

static int fgettr64_internal(int *fd, segy *tp, cwp_Bool fixed_length)
{
	ssize_t nread;	/* bytes seen by fread calls	*/

	/* search linked list for possible alternative */
	if(fd != lastfd)  searchlist64(fd);

	if (infoptr64 == ((struct insegyinfo64 *) NULL)) {
		
		/* initialize new segy input stream */
		unsigned int databytes;	/* bytes from nsfirst	*/

		/* allocate new segy input information table */
		*oldinfoptr64 = (struct insegyinfo64 *) malloc(sizeof(struct insegyinfo64));
		infoptr64 = *oldinfoptr64;
		infoptr64->nextinfo64 = (struct insegyinfo64 *) NULL;
		infoptr64->infp = fp;  /* save FILE * ptr */
		infoptr64->itr = 0;
		infoptr64->ntr = -1;
		
		switch (infoptr64->ftype = filestat(fd)) {
			case DIRECTORY:
				err("%s: segy input can't be a directory", __FILE__);
			case TTY:
				err("%s: segy input can't be tty", __FILE__);
			default:
				/* all others are ok */
			break;
		}

		/* Get the header */
		switch (nread = read(infoptr64->infd,td, HDRBYTES) {
		
			case 0:   return 0; /* no traces; trap in mains */
			
			default:  if (nread != HDRBYTES) err("%s: bad first header", __FILE__);
				
		}		

		/* Have the header, now go for the data */
		infoptr64->nsfirst = tp->ns;
		
		if (infoptr64->nsfirst > SU_NFLTS)
			err("%s: unable to handle %d > %d samples per trace",__FILE__, infoptr64->nsfirst, SU_NFLTS);

		switch (tp->trid) {
			case CHARPACK:
				infoptr64->bytesper = sizeof(char);   break;
			case SHORTPACK:
				infoptr64->bytesper = 2*sizeof(char); break;
			default:
				infoptr64->bytesper = sizeof(float);  break;
		}

		databytes = infoptr64->bytesper * tp->ns;

		infoptr64->nsegy = HDRBYTES + databytes;

		/* Inconvenient to bump nread here; do it in the switch */
		nread = dataread(tp, infoptr64, fixed_length);

		switch (nread) {
		
			case 0:   
				err("%s: no data on first trace", __FILE__);
			
			default:
				if (nread != databytes)
						err("%s: first trace: " "read only %d bytes of %u",__FILE__, nread, databytes);
			  	else nread += HDRBYTES;
		}

		if (infoptr64->ftype == DISK) { /* compute ntr */
			infoptr64->ntr=(unsigned int) lseek(fd,0,SEEK_END);
			infoptr64->ntr /=infoptr->nsegy;
			lseek(fd,infoptr->nsegy,SEEK_SET); /* reset fp */
	        }


	} else {		/* Not first entry */
		switch (nread = (int) efread(tp, 1, HDRBYTES, infoptr->infp)) {
		case 0: lastfp = infoptr->infp;
			return 0; /* finished */
		default:  if (nread != HDRBYTES)
				err("%s: on trace #%ld read %d bytes ",
				    "expected %d bytes",
				    __FILE__,(infoptr->itr)+1,nread,HDRBYTES);
		}

                nread += dataread(tp, infoptr, fixed_length);

		if (fixed_length && (tp->ns != infoptr->nsfirst))
			err("%s: on trace #%ld, "
			    "number of samples in header (%d) "
			    "differs from number for first trace (%d)", 
			    __FILE__, (infoptr->itr)+1, tp->ns,
			    infoptr->nsfirst);
	}

	++(infoptr->itr);
	lastfp = infoptr->infp;
	return (nread);
}

                                
int efopen64(const char ffname, int flags)
{
	int fd;

	fd=open(ffname,flags|O_LARGEFILE);
	if(fd>-1) return(fd);
	perror(ffname);
	err("File cannot be opened.\n");
}

ssize_t fgettr64(int fd,segy *tr)
/* read a segy trace */
{
	ssize_t nread;
	
	nread=read(fd,tr,(size_t)HDRBYTES);
	
	/* We only want to read the trace data if
	   nread==HDRBYTES, otherwise no data trace is available
	   this case return 0 
	   open is much smarter but we don't care rigth now */
	if(nread==HDRBYTES) {
		nread+=read(fd,tr,(size_t) *tr.ns*FSIZE);
		return(nread);
	} else {
		return(0);
	}
} 

ssize_t fgettra64(int fd,segy *tr,unsigned int it)
/* read a segy trace */
{
/* The assumption is made here that the trace lenght are equal */
/* and the pointer is always at the beginning of the trace */	
	
	ssize_t nread;
	
	nread=read(fd,tr,(size_t)HDRBYTES);
	
	/* We only want to read the trace data if
	   nread==HDRBYTES, otherwise no data trace is available
	   this case return 0 
	   open is much smarter but we don't care rigth now */
	if(nread==HDRBYTES) {
		nread+=read(fd,tr,(size_t) *tr.ns*FSIZE);
		return(nread);
	} else {
		return(0);
	}
}		
