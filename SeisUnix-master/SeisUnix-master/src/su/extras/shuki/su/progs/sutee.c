/*
 * sutee - tee to a pipe
 */

#include <fcntl.h>
#include <stdio.h>
#include "../include/su.h"

int xargc;
char **xargv;
char *SccsId[]="@(#)sutee.c	1.3\t11/15/88\n";


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
