/*
 * predictive error filtering (maximum entropy method)  
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)pefmem.c	1.1\t7/24/88\n";

static float opl,st,et;
static int iet,ns,lop,lop1,lse,shift=0;
static float dt;
static float *oper,*rab1,*rab2,*pst;

static char *lsdoc = 				
"PREDICTIVE ERROR FILTERING			\n\
 (MAXIMUM ENTROPY METHOD) 	    		\n\
						\n\
 supefmem [options] parameters <stdin >stdout	\n\
						\n\
						\n\
OPTIONS:					\n\
	-v	turn verbose on			\n\
		(by default: off)		\n\
						\n\
						\n\
PARAMETERS                                                      \n\
     opl=        operator length (msec) of predictive error filter\n\
                 (by default: 120)                              \n\
                                                                \n\
     st=  et=    start and end time (msec) of the trace window  \n\
                 for filter computation				\n\
                 (by default: trace start and trace end time)   \n\
                                                                \n";

/*******************************************************************/
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
        int c;
 
        
        SccsId = lSccsId;
 
 
        /* GET OPTIONS */
        while( (c=getopt(xargc,xargv,"v"))!=EOF) {
                switch(c) {
                case 'v':
                        verbose = true;
                        break;
                case '?':
                        warn(__FILE__,__LINE__,"getopt returned '?'");                     
                }
        }
 
        /* GET PARAMETERS */
                       
        opl=120.0;              fgetpar("opl",&opl);
        st=0.0;                fgetpar("st",&st);
                        iet = fgetpar("et",&et);

	if (verbose){

	/* OPTIONS&PARAMETERS PRINT */

		fprintf(stderr,"\tPREDICTIVE ERROR FILTERING\n\t(MAXIMUM ENTROPY METHOD)\n\tsupefmem program\n");
		fprintf(stderr,"\t\tOPTIONS:\n");
		
		fprintf(stderr,"\t\tPARAMETERS:\n");

		fprintf(stderr,"operator length(msec) opl=%-6.1f\n",opl);
		fprintf(stderr,"start time(msec) for filter computation st=%-7.1f\n",st);
		if(iet!=0)
		fprintf(stderr,"end time(msec) for filter computation et=%-7.1f\n",et);
	}

}

/*******************************************************************/
#ifdef HP
#define _BURG_ burg
#define _CONVV_   convv
#else
#define _BURG_ burg_
#define _CONVV_   convv_
#endif

/*******************************************************************/
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	int nst;

/*        *aatrout = atrin;  MEMORY ALOCATION for OUTTRACE in MAINSEQ */
           dt = abh->dt*0.001;   /* from microsec to msec */
           ns = abh->ns;           /* number of trace samples */
           if(iet==0) et=(ns-1)*dt; /* default end time for filter
				       computation */
           lse = (et - st)/dt + 1.5;  /* length(in samples) of 
                                         window for filter computation */
	   if (lse<0){
	      warn(__FILE__,__LINE__,"st<et condition is violated");
	      selfdoc();
	   }

	   nst = st/dt+0.5;	/* st-sample */
	   pst = atrin->data;
	   pst = pst + nst;	/* pointer to st-sample */

	   if(opl > (et-st)) opl = et - st;
	   lop = opl/dt+0.5;  /* operator length in samples */
	   lop1 = lop - 1;
	   
                /* DYNAMIC MEMORY ALLOCATION        */
	   
		/*predict.error operator */
	   oper = (float*) malloc(lop*sizeof(float));

		/* working arrays */
	   rab1 = (float*) malloc(lse*sizeof(float));
	   rab2 = (float*) malloc(lse*sizeof(float));

}

/*******************************************************************/
/* TRACE SEQUENTIAL FILTERING PROCESSING */
trseq(itr,trin,trout,abh)
int itr;
Sutrace *trin,*trout;
Subhed *abh;
{
	int i;

	   /* inverse filter computation
		(MAXIMUM ENTROPY METHOD) */
	   _BURG_(&lse,pst,rab1,rab2,&lop1,oper);

	   /*  pred.error operator forming */
	   for(i=lop1-1; i>=0; i--) oper[i+1] = -oper[i];
	   oper[0] = 1.0;

        /*  CONVOLUTION of trace with pred.error operator */
	   _CONVV_(&lop,&shift,oper,&ns,trin->data,trout->data);

	return(1);
}

/*******************************************************************/
postp(){}

/*******************************************************************/
