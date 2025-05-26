h16414
s 00000/00000/00029
d D 1.6 88/11/15 14:01:52 shuki 6 5
c 
e
s 00014/00024/00015
d D 1.5 88/05/05 07:22:36 shuki 5 4
c 
e
s 00010/00004/00029
d D 1.4 88/04/26 09:07:15 shuki 4 3
c Tapeless sun 3's
e
s 00002/00005/00031
d D 1.3 88/04/20 06:58:12 shuki 3 2
c 
e
s 00001/00001/00035
d D 1.2 88/04/19 16:29:18 shuki 2 1
c 
e
s 00036/00000/00000
d D 1.1 88/04/14 13:47:49 shuki 1 0
c date and time created 88/04/14 13:47:49 by shuki
e
u
U
f e 0
t
T
I 1
/*
D 5
 * isatape
E 5
I 5
 * isatape(fd)  -  check if file descriptor corresponds to a tape drive
 *		   returns 1 if tape, 0 otherwise.
E 5
 */
D 5

#include <stdio.h>
E 5
#include <sys/types.h>
D 5
#include <sys/stat.h>
#include "../include/local.h"
#include "../include/su.h"
E 5
I 5
#include <sys/ioctl.h>
#include <sys/mtio.h>
#include <errno.h>
extern int errno;
E 5

D 5
isatape(fd)
E 5
I 5
int isatape(fd)
E 5
int fd;
{
D 5
	struct stat sfd;
	struct stat smt;
E 5
I 5
	struct mtget buf;
E 5

D 4
	if (-1 == fstat(fd, &sfd)) {
D 3
		warn(__FILE__,__LINE__,"isatape: fstat failed\n");
		return(0);
E 3
I 3
		err(__FILE__,__LINE__,"isatape: fstat failed\n");
E 3
	}
E 4
I 4
/* Some machines don't have tapes */
#ifdef NOTAPE
return(0);
#endif
E 4

D 2
	if (0 == stat(MTDEVICE, &smt) ) {
E 2
I 2
D 5
	if (-1 == stat(MTDEVICE, &smt) ) {
E 2
D 3
		warn(__FILE__,__LINE__,"isatape: fstat failed\n");
		return(0);
E 3
I 3
D 4
		err(__FILE__,__LINE__,"isatape: stat failed\n");
E 4
I 4
		warn(__FILE__,__LINE__,"isatape: stat failed\n");
		return(0);
E 5
I 5
	if(-1 == ioctl(fd,(int) MTIOCGET,(char *) &buf)) {
		if(errno == EBADF) {
				err("isatape: %d is not a valid file descriptor\n",fd);
			} else return(0);
E 5
	}
I 5
	if(buf.mt_type == ((short) 0)) return(0);
E 5

D 5
	if (-1 == fstat(fd, &sfd)) {
		err(__FILE__,__LINE__,"isatape: fstat failed\n");
E 4
E 3
	}

	/* Detect tape by comparing its major device number to
	   /dev/RMTDEVICE (as defined in local.h).
D 3
	   Complain and exit if raw interface not used.
E 3
	*/

	if ( HIGHBYTE(sfd.st_rdev) == HIGHBYTE(smt.st_rdev)) return(1);

	return(0);

E 5
I 5
	return(1);
E 5
}
E 1
