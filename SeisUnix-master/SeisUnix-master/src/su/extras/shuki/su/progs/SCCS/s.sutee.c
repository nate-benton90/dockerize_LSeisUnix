h26904
s 00000/00000/00063
d D 1.3 88/11/15 14:02:57 shuki 3 2
c 
e
s 00002/00000/00061
d D 1.2 88/05/25 14:54:03 shemer 2 1
c with SccsId[]
e
s 00061/00000/00000
d D 1.1 88/04/14 13:52:46 shuki 1 0
c date and time created 88/04/14 13:52:46 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * sutee - tee to a pipe
 */

#include <fcntl.h>
#include <stdio.h>
#include "../include/su.h"

int xargc;
char **xargv;
I 2
char *SccsId[]="%W%\t%G%\n";

E 2

char *sdoc =
"					\n\
sutee \"system command\" > stdout	\n\
					\n";
bool verbose;

main(ac,av)
int ac; char **av;
{
	int infd,outfd;
	Sutrace tr;
	Subhed bh;
	int auxfd;

	xargc = ac; xargv = av;

	if(ac<2) {
		selfdoc();
	}

	/* OPEN FILES */
	outfd = output();
	infd = input();

	/* CREAT OUTPUT */
	auxfd = sucreat(av[ac-1],0664);	/* -a option not implemented yet */

	apass2(infd,outfd,auxfd);

	getbh(infd,&bh);
	tr.data = (float*)malloc(bh.ns*bh.esize);

	hislog(outfd); 			/* ADD HISTORY TO ASCII HEADER */
/* 	hispr(outfd, "\tpid=%d\n",getpid()); */

	hislog(auxfd); 			/* ADD HISTORY TO ASCII HEADER */
/* 	hispr(auxfd, "\t(tee) pid=%d\n",getpid()); */

	putbh(outfd,&bh);		/* WRITE BINARY HEADER */
	putbh(auxfd,&bh);		/* WRITE BINARY HEADER */

	while(gettr(infd,&tr)) { /* TRACE LOOP */
		puttr(outfd,&tr);
		puttr(auxfd,&tr);
	}

/* 	suclose(auxfd); */

	exit(0);
}
E 1
