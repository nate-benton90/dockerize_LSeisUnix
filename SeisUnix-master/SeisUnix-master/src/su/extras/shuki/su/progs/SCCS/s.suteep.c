h24351
s 00000/00000/00066
d D 1.3 88/11/15 14:03:02 shuki 3 2
c 
e
s 00002/00000/00064
d D 1.2 88/05/25 14:54:05 shemer 2 1
c with SccsId[]
e
s 00064/00000/00000
d D 1.1 88/04/14 13:52:47 shuki 1 0
c date and time created 88/04/14 13:52:47 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * suteep - tee to a pipe
 */

#include <stdio.h>
#include "../include/su.h"

int xargc;
char **xargv;
I 2
char *SccsId[]="%W%\t%G%\n";

E 2

char *sdoc =
"					\n\
suteep \"system command\" > stdout	\n\
					\n";

bool verbose;

main(ac,av)
int ac; char **av;
{
	int infd,outfd;
	Sutrace tr;
	Subhed bh;
	FILE *pfd;
	int ipfd;

	xargc = ac; xargv = av;

	if(ac<2) {
		selfdoc();
	}

	/* OPEN PIPE */
	pfd = popen(av[ac-1],"w");
	ipfd = sufileno(pfd);

	/* OPEN FILES */
	outfd = output();
	infd = input();

	apass2(infd,outfd,ipfd);

	getbh(infd,&bh);
	tr.data = (float*)malloc(bh.ns*bh.esize);

	hislog(outfd); 			/* ADD HISTORY TO ASCII HEADER */
/* 	hispr(outfd, "\tpid=%d\n",getpid()); */

	hislog(ipfd); 			/* ADD HISTORY TO ASCII HEADER */
/* 	hispr(ipfd, "\t(teep) pid=%d\n",getpid()); */

	putbh(outfd,&bh);		/* WRITE BINARY HEADER */
	putbh(ipfd,&bh);		/* WRITE BINARY HEADER */

	while(gettr(infd,&tr)) { /* TRACE LOOP */
		puttr(outfd,&tr);
		puttr(ipfd,&tr);
	}

	close(ipfd);
	pclose(pfd);

	exit(0);
}
E 1
