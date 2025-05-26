h58549
s 00000/00000/00074
d D 1.3 88/11/15 14:02:56 shuki 3 2
c 
e
s 00002/00000/00072
d D 1.2 88/05/25 14:54:02 shemer 2 1
c with SccsId[]
e
s 00072/00000/00000
d D 1.1 88/04/14 13:52:45 shuki 1 0
c date and time created 88/04/14 13:52:45 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * susub - subtract two data sets
 */

#include <fcntl.h>
#include "../include/su.h"

int xargc;
char **xargv;

char *sdoc =
"					\n\
susub data1 data2 > stdout		\n\
					\n\
output is data2-data1			\n\
					\n";
bool verbose/*,hisout,bhout*/;
I 2
char *SccsId[]="%W%\t%G%\n";

E 2

main(ac,av)
int ac; char **av;
{
	int infd1,infd2,outfd,in1,in2;
	Sutrace tr1,tr2;
	Subhed bh1,bh2;

	xargc = ac; xargv = av;

	if(ac!=3) {
		warn(__FILE__,__LINE__,"incorrect number of command line args");
		selfdoc();
	}

	/* OPEN FILES */
	infd1 = suopen(av[1],O_RDONLY);
	infd2 = suopen(av[2],O_RDONLY);
	outfd = output();

				/* SHOULD DIFF THEM */
	apass(infd2,outfd);	/* PASS ASCII HEADER */
	apass(infd1,-1);		/* PASS ASCII HEADER */

	getbh(infd1,&bh1);		/* READ BINARY HEADER */
	getbh(infd2,&bh2);		/* READ BINARY HEADER */
	if(bh1.ns!=bh2.ns) err(__FILE__,__LINE__,"incompatible input files");

	tr1.data = (float*)malloc(bh1.ns*bh1.esize);
	tr2.data = (float*)malloc(bh2.ns*bh2.esize);

	/* ADD HISTORY TO ASCII HEADER */
	hislog(outfd);
	hispr(outfd, "\tsubtracting %s from %s\n",av[1],av[2]);
/* 	hisclose(outfd); */

	putbh(outfd,&bh2);	/* WRITE BINARY HEADER */

	/* MAIN LOOP */
	for(;;) {

		in1 = gettr(infd1,&tr1);
		in2 = gettr(infd2,&tr2);
		
		if(in1==0&&in2!=0) warn(__FILE__,__LINE__,"EOF on %s",av[1]);
		if(in2==0&&in1!=0) warn(__FILE__,__LINE__,"EOF on %s",av[2]);
		if(in1==0||in2==0) break;
		
		vsub(tr1.data, tr2.data, bh1.ns);

		puttr(outfd,&tr1);

	}
	exit(0);
}
E 1
