h38743
s 00000/00000/00070
d D 1.3 88/11/15 14:02:49 shuki 3 2
c 
e
s 00002/00000/00068
d D 1.2 88/05/25 14:53:54 shemer 2 1
c with SccsId[]
e
s 00068/00000/00000
d D 1.1 88/04/14 13:52:40 shuki 1 0
c date and time created 88/04/14 13:52:40 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * sucat - concate two data sets
 */

#include <fcntl.h>
#include "../include/su.h"

int xargc;
char **xargv;
I 2
char *SccsId[]="%W%\t%G%\n";

E 2

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
E 1
