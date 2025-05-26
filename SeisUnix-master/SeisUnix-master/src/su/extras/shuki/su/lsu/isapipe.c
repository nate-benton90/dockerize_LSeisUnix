/*
 * isapipe
 */
/* The zero links test does stopped working on the Sun 4 */
/*
#include <stdio.h>
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
#include <errno.h>
extern errno;

int
isapipe(fd)
int fd;
{
	extern long lseek();
	register long rc;

	rc = lseek(fd,0L,1);
	if(-1 == rc)
	 	if(errno == ESPIPE || errno == EINVAL) return(1);
	return(0);
}
