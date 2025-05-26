h41130
s 00000/00000/00022
d D 1.3 88/11/15 14:01:49 shuki 3 2
c 
e
s 00001/00002/00021
d D 1.2 88/04/20 06:58:09 shuki 2 1
c 
e
s 00023/00000/00000
d D 1.1 88/04/14 13:47:46 shuki 1 0
c date and time created 88/04/14 13:47:46 by shuki
e
u
U
f e 0
t
T
I 1
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
D 2
		warn(__FILE__,__LINE__,"isadir: fstat failed\n");
		return(0);
E 2
I 2
		err(__FILE__,__LINE__,"isadir: fstat failed");
E 2
	}

	if ((sfd.st_mode & S_IFMT) == S_IFDIR) return(1);

	return(0);

}
E 1
