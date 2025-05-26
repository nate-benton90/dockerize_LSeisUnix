h56013
s 00003/00003/00254
d D 1.9 88/11/15 14:03:21 shuki 9 8
c 
e
s 00004/00002/00253
d D 1.8 88/08/02 11:33:24 shuki 8 7
c 
e
s 00002/00002/00253
d D 1.7 88/06/06 13:12:31 shuki 7 6
c Cancel ns in trace headers
e
s 00002/00000/00253
d D 1.6 88/05/25 14:54:22 shemer 6 5
c with SccsId[]
e
s 00001/00001/00252
d D 1.5 88/05/08 11:38:58 shuki 5 4
c 
e
s 00049/00029/00204
d D 1.4 88/05/08 06:24:28 shuki 4 3
c Separate dd's for each card
e
s 00003/00004/00230
d D 1.3 88/05/05 07:23:07 shuki 3 2
c 
e
s 00028/00040/00206
d D 1.2 88/05/05 06:32:18 shuki 2 1
c 
e
s 00246/00000/00000
d D 1.1 88/05/04 13:51:37 shuki 1 0
c date and time created 88/05/04 13:51:37 by shuki
e
u
U
f e 0
t
T
I 1
/*
D 5
 * WGC Code 4 tape to SEGY
E 5
I 5
 * WGC Code 4 tape to SU
E 5
 */
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include "../include/su.h"
#include "../include/wes4.h"

#define DEF_TAPE "/dev/nrmt0"
D 2
/* Wes4_tr wtr; */
Sutrace sutr;
E 2
I 2
#define FREAD(buff,size,count,pfd) if(fread(buff,size,count,pfd)!=count) err("Fread Error");

E 2
int xargc; char **xargv;
D 4
bool verbose=true;
E 4
I 4
bool verbose=false;
I 6
char *SccsId[]="%W%\t%G%\n";

E 6
E 4

D 2
char *sdoc = "wes4read [in=/dev/nrmt0] > stdout \n\
	Reads IBM/WGC code 4 tapes and converts to SU\n";
E 2
I 2
D 4
char *sdoc = "w4read [in=/dev/nrmt0] > stdout     \n\
	Reads IBM/WGC code 4 tapes and converts to SU \n";
E 4
I 4
char *sdoc = "w4read [in=/dev/nrmt0 -v] > stdout \n\
	Reads IBM/WGC code 4 tapes and converts to SU  \n\
	                                               \n\
PARAMETERS                                         \n\
	in= input file. If tape, must be a no rewind   \n\
	    tape device.                               \n\
	                                               \n\
OPTIONS                                            \n\
	-v verbose                                     \n\
                                                   \n";
E 4
E 2

char buff[65536];

main(ac,av)
int ac ; char **av ;
{
	int outfd,infd,i;
D 4
	char in[80],instr[80];
E 4
I 4
D 9
	char in[80],instr[80],ddcom[160];
E 9
I 9
	char *in,instr[80],ddcom[160];
E 9
E 4
	FILE *pdd;
	Sutrace tr;
	Subhed bh;
I 2
	Wes4_tr wtr;
E 2
	int max_ref_time,sampling_interval;
	int nbpt,itr;
D 2
	Wes4_tr wtr;
E 2

	xargc=ac; xargv=av;

I 4
	/* OPTIONS */
	while( (i=getopt(ac,av,"v"))!=EOF) {
		switch(i) {
		case 'v':
			verbose = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

E 4
	/* INPUT */
	if(isatty(STDOUT)) selfdoc();		/* SEE IF HE IS SERIUS */

	if(!isatty(STDIN)) {
D 9
		in[0] = (char)0;
E 9
I 9
		in = "";
E 9
		infd = STDIN;
		instr[0] = (char)0;
	} else {
D 9
		if(!sgetpar("in",in)) sprintf(in,DEF_TAPE);
E 9
I 9
		if(!sgetpar("in",&in)) in=DEF_TAPE;
E 9
		sprintf(instr,"<%s",in);
	}

D 3

E 3
	/* OUTPUT */
	outfd = output();

D 3
	hislog(outfd);

E 3
	/* ASCII (EBCDIC) HEADER */
I 8
/*
E 8
	hispr(outfd,"FILE 0 (IBM Label):\n");
D 4
	hispr(outfd,"========================\n");
E 4
I 4
	hispr(outfd,"===================\n");
E 4
	sprintf(buff,"dd conv=ascii bs=80 %s",instr);
D 8
/* 	fprintf(stderr,"popen(%s)\n",buff); */
E 8
I 8
	fprintf(stderr,"popen(%s)\n",buff);
E 8
	pdd = popen(buff,"r");
I 2
	if(pdd==NULL) err("popen(%s,r) failed",buff);

E 2
	for(i=0;i<3;i++) {
D 2
		fread(buff,80,1,pdd);
E 2
I 2
		FREAD(buff,80,1,pdd);
E 2
		buff[80] = (char)0;
D 8
/* 		fprintf(stderr,"Just read:\t%s\n",buff); */
E 8
I 8
		fprintf(stderr,"Just read:\t%s\n",buff);
E 8
		hispr(outfd,"%s\n",buff);
	}
	pclose(pdd);
I 8
*/
E 8

D 4
	hispr(outfd,"\nBeginning of file 1 (C Cards):\n");
	hispr(outfd,  "=================================\n");
	sprintf(buff,"dd conv=ascii bs=80 count=20 %s",instr);
E 4
I 4
	hispr(outfd,"\nFILE 1 (WGC4 C Cards):\n");
	hispr(outfd,  "======================\n");
/* 	sprintf(buff,"dd conv=ascii bs=80 count=20 %s",instr); */
E 4
/* 	fprintf(stderr,"popen(%s)\n",buff); */
D 4
	pdd = popen(buff,"r");
E 4
I 4
/* 	pdd = popen(buff,"r"); */
E 4

I 4
	sprintf(ddcom,"dd conv=ascii bs=80 count=1 %s",instr);
E 4
	buff[80] = (char)0;

D 4
	/* CARD 1 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
	FREAD(buff,80,1,pdd);
E 4
I 4
	/* CLIENT CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.client,8);

D 4
	/* CARD 2 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
	FREAD(buff,80,1,pdd);
E 4
I 4
	/* LINE CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.name,8);

D 4
	/* CARD 3 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
	FREAD(buff,80,1,pdd);
E 4
I 4
	/* AREA CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.area,8);

	/* CARD 4 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);
	sscanf(buff+19,"%d",&max_ref_time);
	sscanf(buff+44,"%d",&sampling_interval);
	bh.dt = sampling_interval*1000;
	bh.ns = max_ref_time/sampling_interval;
	bh.esize = sizeof(float);
	tr.data = (float*)malloc(bh.ns*bh.esize);

	/* CARD 5 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 6 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 7 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 8 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 9 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 10 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 11 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 12 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 13 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 14 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

	/* CARD 15 */
D 2
	fread(buff,80,1,pdd);
E 2
I 2
D 4
	FREAD(buff,80,1,pdd);
E 4
I 4
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
E 2
	hispr(outfd,"%s\n",buff);

D 2
	/* CARD 16 */
	fread(buff,80,1,pdd);
	hispr(outfd,"%s\n",buff);
E 2
I 2
	do {
D 4
		FREAD(buff,80,1,pdd);
E 4
I 4
		pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
E 4
		hispr(outfd,"%s\n",buff);
	} while(strncmp(buff+2,"EOF",3));
E 2

D 2
	/* CARD 17 */
	fread(buff,80,1,pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 18 */
	fread(buff,80,1,pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 19 */
	fread(buff,80,1,pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 20 */
	fread(buff,80,1,pdd);
	hispr(outfd,"%s\n",buff);

E 2
D 4
	pclose(pdd);
E 4
I 4
/* 	pclose(pdd); */
E 4

	hispr(outfd,"\n");

	if(*in) {
D 3
		infd = open(in,O_RDONLY);
E 3
I 3
		infd = suopen(in,O_RDONLY);
E 3
I 2
		if(infd== -1) err("open error");
E 2
		if(isatape(infd))				/* SHOULD BE NO REWIND */
			if(index(instr,'n')==NULL)
				warn(__FILE__,__LINE__,"Tape input %s should be a no rewind device",in);
	} else {
		infd = STDIN;
	}
I 3

	hislog(outfd);
E 3

	/* BINARY HEADER */
	putbh(outfd,&bh);

	nbpt = bh.ns*2 + 400;	/* <---- */

	/* TRACES */
	itr = 0;
	while(read(infd,&wtr,nbpt)) {

		if(wtr.code != 4) err("__FILE__,__LINE__,Not Western code 4 (code=%d != 4)\n",wtr.code);

		tr.tracl = ++itr;
/* 		tr.tracr = ; */
		tr.fldr = wtr.fr;
/* 		tr.tracf = wtr.fc; */
/* 		tr.ep = wtr.shotp; */
		tr.cdp = wtr.cdf;
		tr.cdpt = wtr.trace;
		tr.trid = 1;
		tr.nvs = wtr.sw;
		tr.nhs = wtr.sw;
/* 		tr.duse = 2; */
		tr.offset = wtr.offset ;
/* 		tr.gelev = wtr.ge; */
/* 		tr.selev = wtr.se; */
/* 		tr.sdepth = 0; */
/* 		tr.gdel = datum; */
/* 		tr.sdel = datum; */
/* 		tr.swdep = 0; */
/* 		tr.gwdep = 0; */
/* 		tr.scalel = 1; */
/* 		tr.scalco = 1; */
		tr.sx = wtr.sx;
		tr.sy = wtr.sy;
		tr.gx = wtr.gx;
		tr.gy = wtr.gy;
/* 		tr.counit = 1; */
/* 		tr.wevel = repl_vel ; */
/* 		tr.sweves = repl_vel ; */
/* 		tr.sut = 0; */
/* 		tr.gut = 0; */
		tr.gstat = wtr.sstat;
		tr.sstat = wtr.gstat;
		tr.tstat = wtr.mstat;
/* 		tr.laga = 0; */
/* 		tr.lagb = 0; */
/* 		tr.delrt = wtr.tfirst; */
		tr.muts = wtr.tfirst;
		tr.mute = wtr.tnzfirst;
D 7
		tr.ns = bh.ns;
		tr.dt = bh.dt;
E 7
I 7
/* 		tr.ns = bh.ns; */
/* 		tr.dt = bh.dt; */
E 7
		tr.gain = wtr.fgv;
/* 		tr.s_sta = wtr.sstat; */
/* 		tr.g_sta = wtr.gstat; */
/* 		tr.y_sta = wtr.mstat; */

		h2f((short*)wtr.data,tr.data,bh.ns);

		puttr(outfd,&tr);
	}
}

h2f(h,f,n)
short *h;
float *f;
int n;
{
	while(n--)
		*f++ = (float) *h++;
}
E 1
