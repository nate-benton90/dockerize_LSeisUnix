#include <stdio.h>
#include <math.h>
#include "../include/su.h"
int xargc; char **xargv;
char *sdoc="suinfo <stdin >stdout\n\
	If either the input or the output are seekable, suinfo adds	\n\
	information to the trace header. If the input is a pipe, suinfo	\n\
	prints the information on the stderr.				\n\
	The information includes: ntr[, nsperg, ngpers, ncdp, noffset,	\n\
	dcdp, ds, dg]							\n\
	* features in [] are not yet implemented			\n";
bool verbose=false;
char *SccsId="%W% %G%\n";
/*
will specify it at run time. This problem is shared by the migration
programs (in the DMO there is a cludge to find out the cdp interval
on a common offset section). A solution may be grabbing more of
the unass' and makeing up a program that will fill them. What is
needed is:

(1)	dcdpco	=	cdp interval on a common offset section
(2)	dcdpst	=	cdp interval on the stack
(3)	ds	=	shot interval on a geophone gather
(4)	dg	=	geophone interval on a shot profile
(5)	dh	=	half offset inteval on a cdp gather
(6)	ncdpmax	=	max number of cdp's on a CO section -- prestack
			number of cdp's on the stack -- poststack
(7)	nsmax	= 	no of shots on a phone gather
(8)	ngmax	=	no of phones on a shot profile
(9)	nhmax	=	max number of offsets in a cdp gather
(10)	ntr	=	total no of traces in the data

(1)-(5) and (10) should be long ints. (6)-(9) may be short ints.
I plan to put up this program (say suinfo, which will be related to
sysort) unless you'll tell me that you need it soon and are going to do it.
*/

main(ac,av)
int ac; char **av;
{
	int ntr;
	float sx,sy,gx,gy,sxmin,sxmax,symin,symax,gxmin,gxmax,gymin,gymax;
	filetype outtype;
	int infd,outfd;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;

	infd = input();
	outfd = output();

	apass(infd,-1);

	getbh(infd,&bh);

	tr.data = (float*)malloc(bh.ns*bh.esize);

	sxmin = HUGE;
	symin = HUGE;
	gxmin = HUGE;
	gymin = HUGE;

	sxmax = -HUGE;
	symax = -HUGE;
	gxmax = -HUGE;
	gymax = -HUGE;

	ntr = 0;
	while( gettr(infd,&tr)) {

		ntr++;

		sx = (float)tr.sx;
		sy = (float)tr.sy;
		gx = (float)tr.gx;
		gy = (float)tr.gy;

		sxmin = MIN(sxmin,sx);
		symin = MIN(symin,sy);
		gxmin = MIN(gxmin,gx);
		gymin = MIN(gymin,gy);

		sxmax = MAX(sxmax,sx);
		symax = MAX(symax,sy);
		gxmax = MAX(gxmax,gx);
		gymax = MAX(gymax,gy);

	}

	outtype = statfil(outfd);

	if(verbose || outtype == TTY ) {
		fprintf(stderr,"ntr=%d\n",ntr);
		fprintf(stderr,"sxmin=%g\tsxmax=%g\n",sxmin,sxmax);
		fprintf(stderr,"symin=%g\tsymax=%g\n",symin,symax);
		fprintf(stderr,"gxmin=%g\tgxmax=%g\n",gxmin,gxmax);
		fprintf(stderr,"gymin=%g\tgymax=%g\n",gymin,gymax);
	}

	if (outtype==TTY) exit(0);

	surewind(infd);

	apass(infd,outfd);
	hislog(outfd);
	getbh(infd,&bh);
	bh.ntr = ntr;
	putbh(outfd,&bh);
	while(gettr(infd,&tr)) {
		puttr(outfd,&tr);
	}

	exit(0);
}
