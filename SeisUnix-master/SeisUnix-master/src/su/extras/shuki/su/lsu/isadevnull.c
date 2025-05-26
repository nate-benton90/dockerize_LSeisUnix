/*
 * isadevnull
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

isadevnull(fd)
int fd;
{
	struct stat sfd;
	struct stat sdn;

	if (-1 == fstat(fd, &sfd)) {
		err(__FILE__,__LINE__,"isadevnull: fstat failed\n");
	}

	if (-1 == stat("/dev/null", &sdn) ) {
		err(__FILE__,__LINE__,"isadevnull: stat failed\n");
	}

	if(sfd.st_rdev == sdn.st_rdev) return(1);

	return(0);

}
