/*
 * WGC Code 4 tape to SU
 */
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include "../include/su.h"
#include "../include/wes4.h"

#define DEF_TAPE "/dev/nrmt0"
#define FREAD(buff,size,count,pfd) if(fread(buff,size,count,pfd)!=count) err("Fread Error");

int xargc; char **xargv;
bool verbose=false;
char *SccsId[]="@(#)w4read.c	1.9\t11/15/88\n";


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

char buff[65536];

main(ac,av)
int ac ; char **av ;
{
	int outfd,infd,i;
	char *in,instr[80],ddcom[160];
	FILE *pdd;
	Sutrace tr;
	Subhed bh;
	Wes4_tr wtr;
	int max_ref_time,sampling_interval;
	int nbpt,itr;

	xargc=ac; xargv=av;

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

	/* INPUT */
	if(isatty(STDOUT)) selfdoc();		/* SEE IF HE IS SERIUS */

	if(!isatty(STDIN)) {
		in = "";
		infd = STDIN;
		instr[0] = (char)0;
	} else {
		if(!sgetpar("in",&in)) in=DEF_TAPE;
		sprintf(instr,"<%s",in);
	}

	/* OUTPUT */
	outfd = output();

	/* ASCII (EBCDIC) HEADER */
/*
	hispr(outfd,"FILE 0 (IBM Label):\n");
	hispr(outfd,"===================\n");
	sprintf(buff,"dd conv=ascii bs=80 %s",instr);
	fprintf(stderr,"popen(%s)\n",buff);
	pdd = popen(buff,"r");
	if(pdd==NULL) err("popen(%s,r) failed",buff);

	for(i=0;i<3;i++) {
		FREAD(buff,80,1,pdd);
		buff[80] = (char)0;
		fprintf(stderr,"Just read:\t%s\n",buff);
		hispr(outfd,"%s\n",buff);
	}
	pclose(pdd);
*/

	hispr(outfd,"\nFILE 1 (WGC4 C Cards):\n");
	hispr(outfd,  "======================\n");
/* 	sprintf(buff,"dd conv=ascii bs=80 count=20 %s",instr); */
/* 	fprintf(stderr,"popen(%s)\n",buff); */
/* 	pdd = popen(buff,"r"); */

	sprintf(ddcom,"dd conv=ascii bs=80 count=1 %s",instr);
	buff[80] = (char)0;

	/* CLIENT CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.client,8);

	/* LINE CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.name,8);

	/* AREA CARD */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);
	bcopy(buff+10,bh.area,8);

	/* CARD 4 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);
	sscanf(buff+19,"%d",&max_ref_time);
	sscanf(buff+44,"%d",&sampling_interval);
	bh.dt = sampling_interval*1000;
	bh.ns = max_ref_time/sampling_interval;
	bh.esize = sizeof(float);
	tr.data = (float*)malloc(bh.ns*bh.esize);

	/* CARD 5 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 6 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 7 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 8 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 9 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 10 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 11 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 12 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 13 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 14 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	/* CARD 15 */
	pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
	hispr(outfd,"%s\n",buff);

	do {
		pdd = popen(ddcom,"r"); FREAD(buff,80,1,pdd); pclose(pdd);
		hispr(outfd,"%s\n",buff);
	} while(strncmp(buff+2,"EOF",3));

/* 	pclose(pdd); */

	hispr(outfd,"\n");

	if(*in) {
		infd = suopen(in,O_RDONLY);
		if(infd== -1) err("open error");
		if(isatape(infd))				/* SHOULD BE NO REWIND */
			if(index(instr,'n')==NULL)
				warn(__FILE__,__LINE__,"Tape input %s should be a no rewind device",in);
	} else {
		infd = STDIN;
	}

	hislog(outfd);

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
/* 		tr.ns = bh.ns; */
/* 		tr.dt = bh.dt; */
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
