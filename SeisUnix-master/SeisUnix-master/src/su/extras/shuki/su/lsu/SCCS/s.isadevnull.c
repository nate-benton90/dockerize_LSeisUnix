h64608
s 00000/00000/00027
d D 1.4 88/11/15 14:01:49 shuki 4 3
c 
e
s 00002/00004/00025
d D 1.3 88/04/20 06:58:08 shuki 3 2
c 
e
s 00001/00001/00028
d D 1.2 88/04/19 16:29:17 shuki 2 1
c 
e
s 00029/00000/00000
d D 1.1 88/04/14 13:47:45 shuki 1 0
c date and time created 88/04/14 13:47:45 by shuki
e
u
U
f e 0
t
T
I 1
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
D 3
		warn(__FILE__,__LINE__,"isadevnull: fstat failed\n");
		return(0);
E 3
I 3
		err(__FILE__,__LINE__,"isadevnull: fstat failed\n");
E 3
	}

D 2
	if (0 == stat("/dev/null", &sdn) ) {
E 2
I 2
	if (-1 == stat("/dev/null", &sdn) ) {
E 2
D 3
		warn(__FILE__,__LINE__,"isadevnull: stat failed\n");
		return(0);
E 3
I 3
		err(__FILE__,__LINE__,"isadevnull: stat failed\n");
E 3
	}

	if(sfd.st_rdev == sdn.st_rdev) return(1);

	return(0);

}
E 1
