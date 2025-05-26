/* suapstat.c */
/* B.Nemeth */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUAPSTAT - Apply residual static correction                           ",
"                                                                       ",
" suapstat <stdin >stdout                                               ",
"                                                                       ",
" Required parameters:                                                  ",
"        none                                                           ",
"                                                                       ",
" Optional parameters:                                                  ",
"      st=\"sh.st\"		shot static file                        ",
"      rt=\"rv.st\"		receiver static file                    ",
NULL};

/* Segy data constans */
segy tr,outtr;;		/* SEGY trace */
float *data;		/* trace data */


int main( int argc, char *argv[] )
{
	int sign;		/* sign of static shift */
	int ns;			/* number of samples */
	float dt;		/* sample interval */
	float *tout;		/* output trace */

	char *st="";		/* shot static file name */
	char *rt="";		/* receiver static file name */
	FILE *stf, *rtf;	/* shot end receiver static files */
	int rv;
	int sh;
	int s_sh;
	int s_rv;
	float stv;
	float rtv;
	float stat;
	float tmin;
	int itime;
	int itr=0;
	int sshft;
	
	initargs(argc, argv);
   	requestdoc(1);

	if( !getparstring("st",&st)) st="sh.st";
	if( !getparstring("rt",&rt)) rt="rv.st"; 
	if(!getparint("sign",&sign)) sign=1;

	stf = efopen(st,"r");
	rtf = efopen(rt,"r");

        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
	ns=tr.ns;
	dt   = ((double) tr.dt)/1000000.0;
	tout = ealloc1float(ns);
	
		/* loop over traces */
	do {
		
		memset( (void *) outtr.data, (int) '\0', ns*FSIZE);

		/* rewind the files */
		rewind(stf);
		rewind(rtf);
		
		tmin = tr.delrt/1000.0;

		/* Shot statics */
	    	sh = tr.ep;
		do {
			if(feof(rtf)) err(" Shot %d not on shot static file\n"
					,sh);
			fscanf(stf," %d %f",&s_sh,&stv);
		} while(s_sh!=sh);

		/* Receiver statics */
	    	rv = tr.sdepth;
		do {
			if(feof(stf)) err(" Receiver %d not on receiver static file\n"
					,rv);
			fscanf(rtf," %d %f",&s_rv,&rtv);
		} while(s_rv!=rv);

		stat = sign*( rtv + stv);
		sshft = (int)(stat/dt+0.5);
		
		
		/* copy headers over to the new trace */
  		memcpy((void*) &outtr, (const void *) &tr,
			(size_t) (sizeof(segy)-SU_NFLTS*FSIZE));  
  
  		/* copy the data with the right amount of shift */
		if (sshft <=0) {
			memcpy((void*) &outtr, (const void *) &tr.data[sshft],
			(size_t) (ns-sshft)*FSIZE);
		} else {
			memcpy((void*) &outtr[sshft], (const void *) &tr.data,
			(size_t) (ns-sshft)*FSIZE);
			  
  
		/* set header values */
		outtr.sstat += (1000.0 * stv);
                outtr.gstat += (1000.0 * rtv);
                outtr.tstat += (1000.0 * stat);
 
		puttr(&outtr);
		itr++;
		if(itr%500==0) fprintf(stderr,"Number of traces corrected = %10d\n",
			itr);
		
	} while(gettr(&tr));
        return EXIT_SUCCESS;
}
