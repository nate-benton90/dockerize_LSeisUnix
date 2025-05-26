h20308
s 00000/00000/00010
d D 1.2 88/11/15 14:04:54 shuki 2 1
c 
e
s 00010/00000/00000
d D 1.1 88/04/14 14:01:00 shuki 1 0
c date and time created 88/04/14 14:01:00 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
char *index(s,t)
char *s;
int t;
{
	while(*s++) {
		if( *s==(char)t) return(s);
	}
	return(NULL);
}
E 1
