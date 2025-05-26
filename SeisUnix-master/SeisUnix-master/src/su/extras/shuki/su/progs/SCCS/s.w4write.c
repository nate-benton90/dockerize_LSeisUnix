h35923
s 00003/00003/00218
d D 1.8 88/11/15 14:03:24 shuki 8 7
c 
e
s 00012/00002/00209
d D 1.7 88/08/02 11:33:27 shuki 7 6
c 
e
s 00002/00002/00209
d D 1.6 88/06/06 13:12:33 shuki 6 5
c Cancel ns in trace headers
e
s 00002/00000/00209
d D 1.5 88/05/25 14:54:23 shemer 5 4
c with SccsId[]
e
s 00001/00024/00208
d D 1.4 88/05/12 11:36:10 shuki 4 3
c 
e
s 00002/00002/00230
d D 1.3 88/05/10 14:34:44 shuki 3 2
c 
e
s 00001/00001/00231
d D 1.2 88/05/09 08:46:42 shuki 2 1
c 
e
s 00232/00000/00000
d D 1.1 88/05/09 08:13:55 shuki 1 0
c date and time created 88/05/09 08:13:55 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * SU to WGC Code 4 tape
 */
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include "../include/su.h"
#include "../include/wes4.h"

#define DEF_TAPE "/dev/nrmt0"
#define FWRITE(ptr,size,count,pfd) if(fwrite(ptr,size,count,pfd)!=count) err(__FILE__,__LINE__,"Fwrite Error");
#define WRITE(fd,ptr,n) if(write(fd,ptr,n)!=n)err(__FILE__,__LINE__,"Write Error");

int xargc; char **xargv;
bool verbose=true;
I 5
char *SccsId[]="%W%\t%G%\n";

E 5

D 2
char *sdoc = "w4write [in=/dev/nrmt0] > stdout     \n\
E 2
I 2
char *sdoc = "w4write <stdin [out=/dev/nrmt0]     \n\
E 2
	Reads IBM/WGC code 4 tapes and converts to SU \n";

char buff[65536];

main(ac,av)
int ac ; char **av ;
{
	int outfd,infd,tempfd;
D 8
	char out[80],outstr[80],ddcom[160],*tempname="/tmp/w4wXXXXXX";
E 8
I 8
	char *out,outstr[80],ddcom[160],*tempname="/tmp/w4wXXXXXX";
E 8
	FILE *pdd;
	int nbpt,nr;
	Sutrace tr;
	Subhed bh;
	Wes4_tr wtr;
I 7
	int ssi,file_no,reel_no;
E 7

	xargc=ac; xargv=av;

	/* INPUT */
	infd = input();

	/* TEMPORARY FILE NAME */
	mktemp(tempname);

	/* OUTPUT */
	if(!isatty(STDOUT)) {
D 8
		out[0] = (char)0;
E 8
I 8
		out = "";
E 8
		outfd = STDOUT;
		outstr[0] = (char)0;
	} else {
D 8
		if(!sgetpar("out",out)) sprintf(out,DEF_TAPE);
E 8
I 8
		if(!sgetpar("out",&out)) out=DEF_TAPE;
E 8
		sprintf(outstr,">%s",out);
	}

	apass(infd,-1);
	getbh(infd,&bh);
	tr.data = (float*)malloc(bh.ns*bh.esize);

D 4
	/* IBM LABEL */
/*
VOL13508590                               I. P. R. G.                           
HDR1RP.SU3641.DBS.P1 35085900010001       88123 000000000000IBM OS/VS 370       
HDR2U327600000030RP3641P1/S300            51985                                 
*/

	sprintf(buff,"dd conv=ebcdic bs=80 %s count=3",outstr);
	pdd = popen(buff,"w");
	if(pdd==NULL) err(__FILE__,__LINE__,"popen(%s,w) failed",buff);

D 3
	sprintf(buff,"VOL________                               I. P. R. G.                           ");
E 3
I 3
	sprintf(buff,"VOL11111190                               I. P. R. G.                           ");
E 3
	FWRITE(buff,80,1,pdd);

D 3
	sprintf(buff,"HDR1RP.%6s.DBS.P1 35085900010001       88123 000000000000IBM OS/VS 370       ",
E 3
I 3
	sprintf(buff,"HDR1RP.%6s.DBS.P1 11111100010001       88123 000000000000IBM OS/VS 370       ",
E 3
		bh.name);
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"HDR2U327600000030RP3641P1/S300            51985                                 ");
	FWRITE(buff,80,1,pdd);

	pclose(pdd);

E 4
	/* WGC4 EBCDIC HEADER */
/*
C CLIENT  NAPHTA                                                                
C LINE    SU3641                                                                
C AREA    KOCHAV                                                                
C MAX REF TIME(MS)  3000  SAMPLING INTERVAL  4  REEL SEQUENCE NUMBER  1  TYPE FX
C DATA TRACE NO.(MAX)  60  UNITS OF DISTANCE METERS  CDF ORDERED     RMS    1024
C TRACE HEADER LENGTH  200 CODE 4 RELEASE DATE/VERSION 8405/05.08               
C IDENT TYPE WGC           TRACE HEADER START TIME                              
C RECORDING SYSTEM SEGB        CHANNELS   124 SAMPLE INT   4 FILTER  UNKNOWN    
C COMMENT   MAIN OUTPUT                                                         
C EARLIEST TIME OF FIRST SAMPLE     -200                                        
C TIME VALUE MULTIPLIER      1 REPLACEMENT VELOCITY  1951 DATUM ELEVATION      0
C STATION RANGE   FIRST          30  LAST         120  INCREMENT           1    
C GEOMETRIC SPREADING - TIME VELOCITY PAIRS                                     
C GSTV   0     13641 3000    6  100 1700  420 1820  880 2800 1480 3400          
C GSTV                         2150 4000 3000 4400                              
C PROCESSING HISTORY                                                            
C S1059V01F 14- 4-88 12.12.46  RP.LP3641.EDIT.E1                            GEO1
C S3004V12F 02- 5-88 11.55.11  RP.SU3641.DBS.P1                             GEO1
C SUMMARY FOR RECORDING MEDIA  DEVICE SL  VOL 350859  DENSITY 1600     UNIT  485
C EOF        WESTERN GEOPHYSICAL COMPANY OF AMERICA        FORMAT COPYRIGHT 1986
*/

	/* FIRST CARD */
	sprintf(ddcom,"dd conv=ebcdic bs=80 >%s",tempname);
	pdd = popen(ddcom,"w");
	if(pdd==NULL) err(__FILE__,__LINE__,"popen(%s,w) failed",buff);
	sprintf(buff,"C CLIENT  %6s                                                                ",
		bh.client);
	FWRITE(buff,80,1,pdd);
	pclose(pdd);

	/* THE OTHER CARDS */
	sprintf(ddcom,"dd conv=ebcdic bs=80 >>%s",tempname);
	pdd = popen(ddcom,"w");
	if(pdd==NULL) err(__FILE__,__LINE__,"popen(%s,w) failed",buff);
	sprintf(buff,"C LINE    %6s                                                                ",
		bh.name);
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C AREA    %6s                                                                ",
		bh.area);
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C MAX REF TIME(MS) %5d  SAMPLING INTERVAL %2d  REEL SEQUENCE NUMBER  1  TYPE FX",
		bh.ns*bh.dt/1000,bh.dt/1000);
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C DATA TRACE NO.(MAX)  __  UNITS OF DISTANCE METERS  CDF ORDERED     RMS    ____");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C TRACE HEADER LENGTH  200 CODE 4 RELEASE DATE/VERSION 8405/05.08               ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C IDENT TYPE WGC           TRACE HEADER START TIME                              ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C RECORDING SYSTEM ____        CHANNELS   ___ SAMPLE INT  __ FILTER  UNKNOWN    ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C COMMENT   MAIN OUTPUT                                                         ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C EARLIEST TIME OF FIRST SAMPLE     ____                                        ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C TIME VALUE MULTIPLIER      _ REPLACEMENT VELOCITY  ____ DATUM ELEVATION      0");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C STATION RANGE   FIRST          ___  LAST         ___  INCREMENT           _   ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C GEOMETRIC SPREADING - TIME VELOCITY PAIRS                                     ");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C PROCESSING HISTORY                                                            ");
	FWRITE(buff,80,1,pdd);

D 4
	sprintf(buff,"C <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<NOT AVAILABLE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
E 4
I 4
	sprintf(buff,"C ****************************** NOT AVAILABLE *********************************");
E 4
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C SUMMARY FOR RECORDING MEDIA  DEVICE SL  VOL 350859  DENSITY 1600     UNIT  485");
	FWRITE(buff,80,1,pdd);

	sprintf(buff,"C EOF        WESTERN GEOPHYSICAL COMPANY OF AMERICA        FORMAT COPYRIGHT 1986");
	FWRITE(buff,80,1,pdd);

	pclose(pdd);

	tempfd = open(tempname,O_RDONLY);

	if(*out) {
		outfd = open(out,O_WRONLY|O_CREAT|O_TRUNC,0666);
		if(outfd== -1) err(__FILE__,__LINE__,"creat error");
		if(isatape(outfd))
			if(index(outstr,'n')==NULL)
				warn(__FILE__,__LINE__,"Tape input %s should be a no rewind device",out);
	} else {
		outfd = STDOUT;
	}

	while(nr=read(tempfd,buff,80)) {
		if(nr!=80) err(__FILE__,__LINE__,"Read Error from %s (%d bytes out of 80)",tempname,nr);
		WRITE(outfd,buff,80);
	}

	unlink(tempname);

	nbpt = bh.ns*2 + 400;

D 7
	wtr.code = 4;
E 7
I 7
	ssi = 201;		/* 101 for IBM floating point format */
	reel_no   = 1;
	file_no   = 1;
	wtr.code  = 4;
	wtr.trace = 1;

E 7
	while(gettr(infd,&tr)) {

I 7
		wtr.ssi     = ssi;
		wtr.id      = reel_no+10000*file_no;
		wtr.trace++;

E 7
		wtr.fr      = tr.fldr;
		wtr.cdf     = tr.cdp;
		wtr.trace   = tr.cdpt;
		wtr.sw      = tr.nvs;
D 7
		wtr.sw      = tr.nhs;
E 7
I 7
		if(!wtr.sw) wtr.sw = 1;
E 7
		wtr.offset  = tr.offset;
		wtr.sx      = tr.sx;
		wtr.sy      = tr.sy;
		wtr.gx      = tr.gx;
		wtr.gy      = tr.gy;
		wtr.sstat   = tr.gstat;
		wtr.gstat   = tr.sstat;
		wtr.mstat   = tr.tstat;
		wtr.tfirst  = tr.muts;
		wtr.tnzfirst = tr.mute;
D 6
		bh.ns        = tr.ns;
		bh.dt        = tr.dt;
E 6
I 6
/* 		bh.ns        = tr.ns; */
/* 		bh.dt        = tr.dt; */
E 6
		wtr.fgv      = tr.gain;

		f2h(tr.data,(short*)wtr.data,bh.ns);

		WRITE(outfd,&wtr,nbpt);
	}
}

f2h(f,h,n)
short *h;
float *f;
int n;
{
	int i;
	float amax,normfac;

	for(i=0,amax=0.0;i<n;i++)
		if(fabs(f[i])>amax) amax=fabs(f[i]);

	normfac = 32767.0/amax;
		
	for(i=0;i<n;i++)
		h[i] = (short) (normfac*f[i]);
}
E 1
