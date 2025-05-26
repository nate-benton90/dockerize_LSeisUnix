/*
 * suteep - tee to a pipe
 */

#include <stdio.h>
#include "../include/su.h"

int xargc;
char **xargv;
char *SccsId[]="@(#)suteep.c	1.3\t11/15/88\n";


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
