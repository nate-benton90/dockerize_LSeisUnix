/* velocity card format conversion */

#include "velo.h"
#include "usu.h"


char *sdoc = 
"WGC2HVEL - convert wgc ascii velocity cards to HVEL cards 	 		\n"
"\n"
"wgc2hvel [parameters] <wgc-cards >hvel-cards				\n" 
"\n"
"Required parameters:						 	\n"
"none									\n"
"\n"
"Optional parameters:							\n"
"mintrace=-99999        minimum trace number of velocity cards to output \n"
"maxtrace=99999         maximum trace number of velocity cards to output \n"
"minline=-99999         minimum line number of velocity cards to output \n"
"maxline=99999          maximum line number of velocity cards to output \n"
"\n"
"Notes:									\n"
"1. This program converts WGC's ASCII velocity (6 t-v pairs per row) cards to HANDVEL (cdplbl)	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	7/26/1999   		\n"    
;

main(int argc, char **argv)
{
    char *cbuf; 
    char *velo, *time, trace[5], line[5];
    FILE *infp=stdin,*outfp=stdout;
    float  *times, *vrms; 
	int mintrace,maxtrace,minline,maxline;
	int n1=8192;
	int ic, icmax=9999999;
	int iwgc=0, iline, itrace, iline0, itrace0, cdplbl;
	int nvt, nxin, i;
	float fvelo, ftime;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);


	if(!getparint("mintrace",&mintrace)) mintrace = -99999;
	if(!getparint("maxtrace",&maxtrace)) maxtrace = 99999;
	if(!getparint("minline",&minline)) minline = -99999;
	if(!getparint("maxline",&maxline)) maxline = 99999;

/* memory allocation */
    cbuf = (char*)malloc(81*sizeof(char));
    times = (float*)malloc(n1*sizeof(int));
    vrms = (float*)malloc(n1*sizeof(int));
    velo = (char*) malloc(6);
    time = (char*) malloc(6);

	nxin = 0;
	nvt = 0;

/* read input velf file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
       	if ( cbuf[0]=='W' && cbuf[1]=='G' && cbuf[2]=='C' )  {
	  		strncpy(line,&cbuf[10],5);
	  		iline = atoi(line);
	  		strncpy(trace,&cbuf[32],5);
	  		itrace = atoi(trace);
			iwgc = 1;
			/*
			fprintf(stderr,"iline=%d itrace=%d \n",iline,itrace);
			*/
			if(nvt>0) {
				cdplbl = iline0*10000+itrace0;	
				printhvel(cdplbl,nvt,times,vrms,outfp);
				fprintf(stderr,
	"VELF2HVEL at cdplbl=%d line=%d trace=%d nvt=%d\n",cdplbl,iline0,itrace0,nvt); 
				nvt = 0;
				nxin = nxin + 1;
			}
		} else if(iwgc==1) {
			iline0 = iline;
			itrace0 = itrace;
			iwgc = 2;
		} else if(iwgc==2) {
			if(iline>=minline && iline<=maxline && itrace>=mintrace && itrace<=maxtrace) {
	  			for(i=0;i<6;i++) {
	     			strncpy(time,&cbuf[8+i*12],6);
	     			strncpy(velo,&cbuf[14+i*12],6);
	     			ftime = atof(time);
	     			fvelo = atof(velo);

	     			if (fvelo == 0.) break; 
	     			times[nvt] = ftime*1000.;
	     			vrms[nvt] = fvelo;
	     			nvt = nvt + 1;
	     		}
			}
		}

	}

    if (nvt>0) {
	    nxin = nxin + 1;
		cdplbl = iline0*10000+itrace0;	
		printhvel(cdplbl,nvt,times,vrms,outfp);

		fprintf(stderr,
	"VELF2HVEL at cdplbl=%d line=%d trace=%d nvt=%d\n",cdplbl,iline0,itrace0,nvt); 
	}

     fprintf(stderr,"\n");
     fprintf(stderr,"WGC2HVEL conversion done for %d locations \n",nxin);
     free(times);
     free(vrms);
     free(cbuf);
}

