h23551
s 00000/00000/00009
d D 1.2 88/11/15 14:02:01 shuki 2 1
c 
e
s 00009/00000/00000
d D 1.1 88/09/05 07:14:30 shuki 1 0
c date and time created 88/09/05 07:14:30 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
char *Malloc(n)
unsigned n;
{
	char *r;
	r = (char*)malloc(n);
	if(r==NULL) err(__FILE__,__LINE__,"Malloc(%d) failed\n",n);
	return(r);
}
E 1
