h31055
s 00000/00000/00015
d D 1.2 88/11/15 14:19:29 shuki 2 1
c 
e
s 00015/00000/00000
d D 1.1 88/05/05 07:25:26 shuki 1 0
c date and time created 88/05/05 07:25:26 by shuki
e
u
U
f e 0
t
T
I 1
#include "../include/su.h"
int xargc; char xargv;
bool verbose=true;
char *sdoc="";
main(ac,av)
int ac; char av;
{
	filetype type;
	xargv = av; xargc = ac;

	type = statfil(0);

	printf("statfil returned %d (%s)\n",type,strtype(type));
}

E 1
