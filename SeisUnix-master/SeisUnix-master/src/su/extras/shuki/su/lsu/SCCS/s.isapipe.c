h09405
s 00000/00000/00038
d D 1.4 88/11/15 14:01:51 shuki 4 3
c 
e
s 00020/00030/00018
d D 1.3 88/04/20 06:58:11 shuki 3 2
c 
e
s 00038/00013/00010
d D 1.2 88/04/19 17:08:35 shuki 2 1
c 
e
s 00023/00000/00000
d D 1.1 88/04/14 13:47:48 shuki 1 0
c date and time created 88/04/14 13:47:48 by shuki
e
u
U
f e 0
t
T
I 1
/*
D 2
 * isapipe
E 2
I 2
D 3
 * isapipe(fd)
 * 
 * checks whether a given file descriptor is associated with a pipe
 * returns 1 if so, 0 if no.
 * author - Stewart A. Levin  3/15/83
 * modified - S. Levin 3/20/83  Old algorithm failed a significant fraction
 *		of the time.  New one, much slower, uses /etc/pstat (8)
 *		to print inode and file tables which are then cross indexed
 *		with fstat() output to find the open file entry and check
 *		for the pipe flag 'P'.  One alternative could be to use
 *		ps output to look for other processes with the same parent
 *		but this could fail due to stopped or background jobs as
 *		well as a sh (1) bug that implements long pipelines as
 *		subpipes (and hence changes parents!).
 * modifed - S. Levin 3/21/83  Adapted pstat code directly in line to speed
 *		up access to the inode and file list.  Suggested by Jeff
 *		Thorson.  Also access system info once on the assumption
 *		that preallocated stdin, stdout, stderr files are the only
 *		pipes the user wouldn't know about.
 *
 * modified - S. Levin  5/6/83   mimic system utility tee by declaring
 *		errno extern and trying to seek on the file descriptor.
 *		We then check for error (-1 return value) and compare
 *		the errno value to " seek on a pipe " error number (29).
 *
 * modified - S. Levin 9/16/85   Convex returns errno 22 (EINVAL) when
 *		it should return 29 (ESPIPE) for seek on pipe.
E 3
I 3
 * isapipe
E 3
E 2
 */
I 3
/* The zero links test does stopped working on the Sun 4 */
/*
E 3
D 2

E 2
#include <stdio.h>
I 3
#include <sys/types.h>
#include <sys/stat.h>
isapipe(fd)
int fd;
{
	struct stat sfd;

	if (-1 == fstat(fd, &sfd)) {
		err(__FILE__,__LINE__,"isapipe: fstat failed");
	}

	if (!sfd.st_nlink) return(1);

	return(0);
}
*/
#include <stdio.h>
E 3
D 2
#include <sys/types.h>
#include <sys/stat.h>
E 2
I 2
#include <errno.h>
extern errno;
E 2

I 2
int
E 2
isapipe(fd)
int fd;
{
D 2
	struct stat sfd;
E 2
I 2
	extern long lseek();
	register long rc;
E 2

D 2
	if (-1 == fstat(fd, &sfd)) {
		warn(__FILE__,__LINE__,"isapipe: fstat failed\n");
		return(0);
	}

	if (!sfd.st_nlink) return(1); /* Only pipes have 0 links */

E 2
I 2
	rc = lseek(fd,0L,1);
D 3
	/*
	fprintf(stderr,"isapipe: fd=%d, lseek rc=%d, errno=%d\n",fd,rc,errno);
	*/
E 3
	if(-1 == rc)
	 	if(errno == ESPIPE || errno == EINVAL) return(1);
E 2
	return(0);
D 2

E 2
}
E 1
