h15756
s 00002/00001/00052
d D 1.5 88/11/15 14:02:47 shuki 5 4
c 
e
s 00002/00002/00051
d D 1.4 88/06/06 13:12:20 shuki 4 3
c Cancel ns in trace headers
e
s 00002/00000/00051
d D 1.3 88/05/25 14:53:53 shemer 3 2
c with SccsId[]
e
s 00011/00013/00040
d D 1.2 88/05/11 14:32:18 shuki 2 1
c 
e
s 00053/00000/00000
d D 1.1 88/04/14 13:52:39 shuki 1 0
c date and time created 88/04/14 13:52:39 by shuki
e
u
U
f e 0
t
T
I 1
#include "../include/su.h"
#include "../include/hdrs.h"

D 5
char *sdoc = "suahed nt= dt= save= < input > output \n\
E 5
I 5
char *sdoc = "suahed nt= dt= < input > output \n\
E 5
 syahed puts a segy header on each trace in inputfile\n\
 dt is sample interval in seconds ";

int xargc;
char **xargv;
bool verbose;
I 3
char *SccsId[]="%W%\t%G%\n";

E 3

main(ac,av)
int ac; char **av;
{
	int infd,outfd,nb;
	float dt;
	Sutrace tr;
	Subhed bh;

	xargc = ac; xargv = av;
	verbose = true;

	infd = input();
	outfd = output();

D 2
	if(!igetpar("nt",&nb)) err(__FILE__,__LINE__,"need nt=");
	tr.data = (float*) malloc(nb*sizeof(float));
	tr.ns = nb;
	if(!fgetpar("dt",&dt)) err(__FILE__,__LINE__,"need dt=");
	if(dt>0.1) warn(__FILE__,__LINE__,"dt=%f seconds",dt);
E 2
I 2
	if(!igetpar("nt",&(bh.ns))) err(__FILE__,__LINE__,"need nt=");
	bh.esize = sizeof(float);
E 2

D 2
	tr.dt = dt*1000000;
E 2
I 2
	tr.data = (float*) malloc(bh.ns*bh.esize);
E 2

D 2
	igetpar("idt",&(tr.dt));
E 2
I 2
D 4
	tr.ns = bh.ns;
E 4
I 4
/* 	tr.ns = bh.ns; */
E 4
E 2

D 2
	nb = HDRBYTES + tr.ns*sizeof(float);
E 2
I 2
	if(!fgetpar("dt",&dt)) err(__FILE__,__LINE__,"need dt=");
	if(dt>0.1) warn(__FILE__,__LINE__,"dt=%f seconds",dt);
	bh.dt = dt*1000000;
D 4
	tr.dt = bh.dt;
E 4
I 4
/* 	tr.dt = bh.dt; */
E 4
E 2

D 2
	hislog(outfd);
/* 	hisclose(outfd); */
E 2
I 2
/* 	nb = HDRBYTES + tr.ns*sizeof(float); */
	nb = bh.ns*sizeof(float);
E 2

D 2
	bh.ns = tr.ns;
	bh.esize = sizeof(float);
/* 	bh.dt = tr.dt; */
E 2
I 2
	hislog(outfd);
E 2

	putbh(outfd,&bh);

I 5
	tr.tracl = 0;
E 5
	while(pread(infd,tr.data,nb)) {
		puttr(outfd,&tr) ;
		tr.tracl++;
	}
	exit(0);
/* 	hlog(""); */
}
E 1
