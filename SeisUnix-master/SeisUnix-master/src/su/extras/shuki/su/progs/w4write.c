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
char *SccsId[]="@(#)w4write.c	1.8\t11/15/88\n";


char *sdoc = "w4write <stdin [out=/dev/nrmt0]     \n\
	Reads IBM/WGC code 4 tapes and converts to SU \n";

char buff[65536];

main(ac,av)
int ac ; char **av ;
{
	int outfd,infd,tempfd;
	char *out,outstr[80],ddcom[160],*tempname="/tmp/w4wXXXXXX";
	FILE *pdd;
	int nbpt,nr;
	Sutrace tr;
	Subhed bh;
	Wes4_tr wtr;
	int ssi,file_no,reel_no;

	xargc=ac; xargv=av;

	/* INPUT */
	infd = input();

	/* TEMPORARY FILE NAME */
	mktemp(tempname);

	/* OUTPUT */
	if(!isatty(STDOUT)) {
		out = "";
		outfd = STDOUT;
		outstr[0] = (char)0;
	} else {
		if(!sgetpar("out",&out)) out=DEF_TAPE;
		sprintf(outstr,">%s",out);
	}

	apass(infd,-1);
	getbh(infd,&bh);
	tr.data = (float*)malloc(bh.ns*bh.esize);

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

	sprintf(buff,"C ****************************** NOT AVAILABLE *********************************");
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

	ssi = 201;		/* 101 for IBM floating point format */
	reel_no   = 1;
	file_no   = 1;
	wtr.code  = 4;
	wtr.trace = 1;

	while(gettr(infd,&tr)) {

		wtr.ssi     = ssi;
		wtr.id      = reel_no+10000*file_no;
		wtr.trace++;

		wtr.fr      = tr.fldr;
		wtr.cdf     = tr.cdp;
		wtr.trace   = tr.cdpt;
		wtr.sw      = tr.nvs;
		if(!wtr.sw) wtr.sw = 1;
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
/* 		bh.ns        = tr.ns; */
/* 		bh.dt        = tr.dt; */
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
