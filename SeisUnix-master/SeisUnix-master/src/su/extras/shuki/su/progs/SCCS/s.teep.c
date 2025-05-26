h26797
s 00000/00000/00013
d D 1.3 88/11/15 14:03:03 shuki 3 2
c 
e
s 00000/00000/00013
d D 1.2 88/05/25 14:54:06 shemer 2 1
c with SccsId[]
e
s 00013/00000/00000
d D 1.1 88/04/14 13:52:47 shuki 1 0
c date and time created 88/04/14 13:52:47 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
main()
{
	unsigned char i;
	FILE *p;

	p = popen("od -c | more","w");

	for(i=0;i<128;i++)
		fwrite(&i,1,1,p);
	pclose(p);
	exit(0);
}
E 1
