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
static bool nolabel;
char *SccsId[]="@(#)syread.c	1.6\t11/15/88\n";


char *sdoc = "syread [OPTIONS PARAMETERS] > stdout \n\
	Reads IBM/SEGY tapes and converts to SU  \n\
	                                               \n\
PARAMETERS                                         \n\
	in= input file. If tape, must be a no rewind   \n\
	    tape device.                               \n\
	                                               \n\
OPTIONS                                            \n\
	-v verbose                                     \n\
	-n nolabel                                     \n\
	-f 4 byte IBM floating point format on tape    \n\
	-s 2 byte integer format on tape               \n\
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
	Segy_tr sytr;
	Segy_bh sybh;
	int nbpt,itr,nr;
	enum {ibmfloat,shortint} informat;

	xargc=ac; xargv=av;

	nolabel = false;
	informat = shortint;

	/* OPTIONS */
	while( (i=getopt(ac,av,"vnfs"))!=EOF) {
		switch(i) {
		case 'n':
			nolabel = true;
			break;
		case 'v':
			verbose = true;
			break;
		case 'f':
			informat = ibmfloat;
			break;
		case 's':
			informat = shortint;
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
	if(nolabel==false) {
		hispr(outfd,"FILE 0 (IBM Label):\n");
		hispr(outfd,"===================\n");
		sprintf(buff,"dd conv=ascii bs=80 %s",instr);
		pdd = popen(buff,"r");
		if(pdd==NULL) err(__FILE__,__LINE__,"popen(%s,r) failed",buff);

		for(i=0;i<3;i++) {
			pread(fileno(pdd),buff,80);
			buff[80] = (char)0;
			hispr(outfd,"%s\n",buff);
		}
		pclose(pdd);
	}

	hispr(outfd,"\nFILE 1 (SEGY EBCDIC HEADER):\n");
	hispr(outfd,  "======================\n");
	sprintf(ddcom,"dd conv=ascii bs=3200 count=1 %s",instr);
	pdd = popen(ddcom,"r");
	pread(fileno(pdd),buff,3200);
	pclose(pdd);
/* 	for(i=0;i<3200/80;i++) { */
/* 		hispr(stderr,"%s\n",buff+i*80); */
/* 	} */
	fprintf(stderr,"%s\n",buff);

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
	bh.dt = sybh.hdt; 
	bh.esize = sizeof(float);
	if(bh.ns==0) {
		if(!igetpar("ns",&bh.ns))
			err("Number of samples not on tape, please enter ns=");
	}
	putbh(outfd,&bh);

	tr.data = (float*) malloc(bh.ns*sizeof(float));
        
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
	itr = 0;
	while(nr=read(infd,&sytr,nbpt)) {
                
		if(nr == -1) perror(av[0]);
		if(nr!=nbpt) warn(__FILE__,__LINE__,"nr=%d != nbpt=%d",nr,nbpt);

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
/* 		tr.ns = bh.ns; */
/* 		tr.dt = bh.dt; */
		tr.gain = sytr.gain;

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
