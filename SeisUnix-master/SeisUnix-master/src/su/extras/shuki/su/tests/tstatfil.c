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

