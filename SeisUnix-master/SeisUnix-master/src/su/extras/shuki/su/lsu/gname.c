#include "../include/su.h"
filetype statfile();
char *strtype();
char *gname(fd)
int fd ;
{
	return(strtype(statfil(fd)));
}
