/*
 * isatape(fd)  -  check if file descriptor corresponds to a tape drive
 *		   returns 1 if tape, 0 otherwise.
 */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mtio.h>
#include <errno.h>
extern int errno;

int isatape(fd)
int fd;
{
	struct mtget buf;

/* Some machines don't have tapes */
#ifdef NOTAPE
return(0);
#endif

	if(-1 == ioctl(fd,(int) MTIOCGET,(char *) &buf)) {
		if(errno == EBADF) {
				err("isatape: %d is not a valid file descriptor\n",fd);
			} else return(0);
	}
	if(buf.mt_type == ((short) 0)) return(0);

	return(1);
}
