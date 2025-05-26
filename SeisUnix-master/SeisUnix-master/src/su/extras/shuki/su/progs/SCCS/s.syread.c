h03955
s 00005/00005/00204
d D 1.6 88/11/15 14:03:32 shuki 6 5
c 
e
s 00045/00017/00164
d D 1.5 88/10/24 10:01:39 shuki 5 4
c 
e
s 00014/00005/00167
d D 1.4 88/06/19 15:29:22 shemer 4 3
c 
e
s 00020/00013/00152
d D 1.3 88/06/16 11:21:25 shemer 3 2
c nolabel
e
s 00002/00002/00163
d D 1.2 88/06/06 13:12:34 shuki 2 1
c Cancel ns in trace headers
e
s 00165/00000/00000
d D 1.1 88/05/29 16:18:57 shuki 1 0
c date and time created 88/05/29 16:18:57 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * SEGY tape to SU
 */
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include "../include/su.h"
#include "../include/segy.h"

#define DEF_TAPE "/dev/nrmt0"
/* #define FREAD(buff,size,count,pfd) if(fread(buff,size,count,pfd)!=count) err("Fread Error"); */

int xargc; char **xargv;
bool verbose=false;
I 3
static bool nolabel;
E 3
char *SccsId[]="%W%\t%G%\n";


D 4
char *sdoc = "w4segy [in=/dev/nrmt0 -v] > stdout \n\
E 4
I 4
D 5
char *sdoc = "w4segy [type=fl in=/dev/nrmt0 -v] > stdout \n\
E 5
I 5
char *sdoc = "syread [OPTIONS PARAMETERS] > stdout \n\
E 5
E 4
	Reads IBM/SEGY tapes and converts to SU  \n\
	                                               \n\
PARAMETERS                                         \n\
	in= input file. If tape, must be a no rewind   \n\
	    tape device.                               \n\
I 4
D 5
        type=fl in case of float                       \n\
E 5
E 4
	                                               \n\
OPTIONS                                            \n\
	-v verbose                                     \n\
I 4
D 5
        -n nolabel                                 \n\
E 5
I 5
	-n nolabel                                     \n\
	-f 4 byte IBM floating point format on tape    \n\
	-s 2 byte integer format on tape               \n\
E 5
E 4
                                                   \n";

char buff[65536];

main(ac,av)
int ac ; char **av ;
{
	int outfd,infd,i;
D 6
	char in[80],instr[80],ddcom[160];
E 6
I 6
	char *in,instr[80],ddcom[160];
E 6
	FILE *pdd;
	Sutrace tr;
	Subhed bh;
	Segy_tr sytr;
	Segy_bh sybh;
D 5
	/* int max_ref_time,sampling_interval; */
E 5
	int nbpt,itr,nr;
I 4
D 5
        char fl[5] ;
E 5
I 5
	enum {ibmfloat,shortint} informat;
E 5
E 4

	xargc=ac; xargv=av;

I 3
	nolabel = false;
I 5
	informat = shortint;
E 5

E 3
	/* OPTIONS */
D 3
	while( (i=getopt(ac,av,"v"))!=EOF) {
E 3
I 3
D 5
	while( (i=getopt(ac,av,"vn"))!=EOF) {
E 5
I 5
	while( (i=getopt(ac,av,"vnfs"))!=EOF) {
E 5
E 3
		switch(i) {
I 3
		case 'n':
			nolabel = true;
			break;
E 3
		case 'v':
			verbose = true;
			break;
I 5
		case 'f':
			informat = ibmfloat;
			break;
		case 's':
			informat = shortint;
			break;
E 5
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}

	/* INPUT */
	if(isatty(STDOUT)) selfdoc();		/* SEE IF HE IS SERIUS */

	if(!isatty(STDIN)) {
D 6
		in[0] = (char)0;
E 6
I 6
		in = "";
E 6
		infd = STDIN;
		instr[0] = (char)0;
	} else {
D 6
		if(!sgetpar("in",in)) sprintf(in,DEF_TAPE);
E 6
I 6
		if(!sgetpar("in",&in)) in=DEF_TAPE;
E 6
		sprintf(instr,"<%s",in);
	}

	/* OUTPUT */
	outfd = output();

	/* ASCII (EBCDIC) HEADER */
D 3
	hispr(outfd,"FILE 0 (IBM Label):\n");
	hispr(outfd,"===================\n");
	sprintf(buff,"dd conv=ascii bs=80 %s",instr);
/* 	fprintf(stderr,"popen(%s)\n",buff); */
	pdd = popen(buff,"r");
	if(pdd==NULL) err("popen(%s,r) failed",buff);
E 3
I 3
	if(nolabel==false) {
		hispr(outfd,"FILE 0 (IBM Label):\n");
		hispr(outfd,"===================\n");
		sprintf(buff,"dd conv=ascii bs=80 %s",instr);
		pdd = popen(buff,"r");
D 6
		if(pdd==NULL) err("popen(%s,r) failed",buff);
E 6
I 6
		if(pdd==NULL) err(__FILE__,__LINE__,"popen(%s,r) failed",buff);
E 6
E 3

D 3
	for(i=0;i<3;i++) {
		pread(fileno(pdd),buff,80);
		buff[80] = (char)0;
/* 		fprintf(stderr,"Just read:\t%s\n",buff); */
		hispr(outfd,"%s\n",buff);
E 3
I 3
		for(i=0;i<3;i++) {
			pread(fileno(pdd),buff,80);
			buff[80] = (char)0;
			hispr(outfd,"%s\n",buff);
		}
		pclose(pdd);
E 3
	}
D 3
	pclose(pdd);
E 3

	hispr(outfd,"\nFILE 1 (SEGY EBCDIC HEADER):\n");
	hispr(outfd,  "======================\n");
	sprintf(ddcom,"dd conv=ascii bs=3200 count=1 %s",instr);
	pdd = popen(ddcom,"r");
	pread(fileno(pdd),buff,3200);
	pclose(pdd);
/* 	for(i=0;i<3200/80;i++) { */
/* 		hispr(stderr,"%s\n",buff+i*80); */
D 6
		fprintf(stderr,"%s\n",buff);
E 6
/* 	} */
I 6
	fprintf(stderr,"%s\n",buff);
E 6

	if(*in) {
		infd = suopen(in,O_RDONLY);
		if(infd== -1) err("open error");
		if(isatape(infd))		/* SHOULD BE NO REWIND */
			if(index(instr,'n')==NULL)
				warn(__FILE__,__LINE__,"Tape input %s should be a no rewind device",in);
	} else {
		infd = STDIN;
	}

	hislog(outfd);

	/* BINARY HEADER */
	if(read(infd,&sybh,400)!=400) err(__FILE__,__LINE__,"XX");
	bh.ns = sybh.hns; 
I 3
	bh.dt = sybh.hdt; 
E 3
	bh.esize = sizeof(float);
	if(bh.ns==0) {
		if(!igetpar("ns",&bh.ns))
			err("Number of samples not on tape, please enter ns=");
	}
	putbh(outfd,&bh);

	tr.data = (float*) malloc(bh.ns*sizeof(float));
D 4

	nbpt = bh.ns*sizeof(short) + 240;	/* <---- */

E 4
I 4
        
D 5
        if (sgetpar("type",fl)) {
        nbpt = bh.ns * sizeof(float) +240;
	} else {nbpt = bh.ns*sizeof(short) + 240;	/* <---- */
           } /*end else */
E 4
	/* TRACES */
E 5
I 5
	switch(informat) {

		case ibmfloat:
			nbpt = bh.ns*4 +240;
			break;

		case shortint:
			nbpt = bh.ns*2 +240;
			break;

		default:
			err(__FILE__,__LINE__,"unknown informat");

	}

	/* READ TRACES */
E 5
	itr = 0;
	while(nr=read(infd,&sytr,nbpt)) {
D 4

E 4
I 4
                
E 4
D 5
		if(nr!=nbpt) err(__FILE__,__LINE__,"X");
E 5
I 5
		if(nr == -1) perror(av[0]);
		if(nr!=nbpt) warn(__FILE__,__LINE__,"nr=%d != nbpt=%d",nr,nbpt);
E 5

		tr.tracl = sytr.tracl;
		tr.fldr = sytr.fldr;
		tr.cdp = sytr.cdp;
		tr.cdpt = sytr.cdpt;
		tr.trid = sytr.trid;
		tr.nvs = sytr.nvs;
		tr.nhs = sytr.nhs;
		tr.offset = sytr.offset ;
		tr.sx = sytr.sx;
		tr.sy = sytr.sy;
		tr.gx = sytr.gx;
		tr.gy = sytr.gy;
		tr.gstat = sytr.gstat;
		tr.sstat = sytr.sstat;
		tr.tstat = sytr.tstat;
		tr.muts = sytr.muts;
		tr.mute = sytr.mute;
D 2
		tr.ns = bh.ns;
		tr.dt = bh.dt;
E 2
I 2
/* 		tr.ns = bh.ns; */
/* 		tr.dt = bh.dt; */
E 2
		tr.gain = sytr.gain;
I 4
D 5
                if(sgetpar("type",fl)) { 
                ibmflt(sytr.data,tr.data,bh.ns,1);
        }  else {
E 5
E 4

D 5
		h2f((short*)sytr.data,tr.data,bh.ns);
I 4
           } /* end else */
E 5
I 5
		switch(informat) {

			case ibmfloat:
				ibmflt(sytr.data,tr.data,bh.ns,1);
				break;

			case shortint:
				h2f((short*)sytr.data,tr.data,bh.ns);
				break;

			default:
				err(__FILE__,__LINE__,"unknown informat");

		}
E 5
E 4

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
