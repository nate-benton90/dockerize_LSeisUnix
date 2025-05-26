/*
 * sucat - concate two data sets
 */

#include <fcntl.h>
#include "../include/su.h"

int xargc;
char **xargv;
char *SccsId[]="@(#)sucat.c	1.3\t11/15/88\n";


char *sdoc =
"					\n\
sucat data1 data2 ... > stdout		\n\
					\n\
output is data1data2			\n\
					\n";
bool verbose/*,hisout,bhout*/;

main(ac,av)
int ac; char **av;
{
	int infd,outfd,nt,j;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	if(ac<2) {
		selfdoc();
	}

	/* OPEN FILES */
	outfd = output();

	/* FIRST FILE */
	infd = suopen(av[1],O_RDONLY);
	apass(infd,outfd);
	getbh(infd,&bh);
	nt = bh.ns;
	tr.data = (float*)malloc(bh.ns*bh.esize);
	hislog(outfd); 			/* ADD HISTORY TO ASCII HEADER */
	hispr(outfd, "\tconcating ");
	for(j=1;j<ac;j++) hispr(outfd,"%s ",av[j]);
	hispr(outfd,"\n");
	putbh(outfd,&bh);		/* WRITE BINARY HEADER */
	while(gettr(infd,&tr)) puttr(outfd,&tr); /* TRACE LOOP */
	suclose(infd);

	/* THE REST OF THE FILES */
	for(j=2;j<ac;j++) {

		infd = suopen(av[j],O_RDONLY);

		/* ASCII HEADER */
		apass(infd,-1);

		/* BINARY HEADER */
		getbh(infd,&bh);
		if(bh.ns!=nt) err(__FILE__,__LINE__,"incompatible input files");

		/* TRACE LOOP */
		while(gettr(infd,&tr)) puttr(outfd,&tr);

		suclose(infd);
	}

	exit(0);
}
