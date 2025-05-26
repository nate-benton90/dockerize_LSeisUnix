h37322
s 00002/00005/00006
d D 1.2 88/11/15 14:01:38 shuki 2 1
c 
e
s 00011/00000/00000
d D 1.1 88/04/14 13:47:42 shuki 1 0
c date and time created 88/04/14 13:47:42 by shuki
e
u
U
f e 0
t
T
I 1
#include "../include/su.h"
filetype statfile();
char *strtype();
D 2
void gname(fd, buf)
E 2
I 2
char *gname(fd)
E 2
int fd ;
D 2
char *buf ;
E 2
{
D 2
/* 	strcpy(buf,"UNKNOWN"); */
	strcpy(buf,strtype(statfil(fd)));
/* 	warn(__FILE__,__LINE__,"gname stub was called (fd=%d, buf=%s )",fd,buf); */
E 2
I 2
	return(strtype(statfil(fd)));
E 2
}
E 1
