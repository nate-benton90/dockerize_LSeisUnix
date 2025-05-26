/*
 * isadir
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

isadir(fd)
int fd;
{
	struct stat sfd;

	if (-1 == fstat(fd, &sfd)) {
		err(__FILE__,__LINE__,"isadir: fstat failed");
	}

	if ((sfd.st_mode & S_IFMT) == S_IFDIR) return(1);

	return(0);

}
