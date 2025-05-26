/*
 * pr - print non zero header values and data for non graphic terminals
 */
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
/* #include "../include/hdrs.h" */

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose,suout;
extern char *SccsId;
 
static char lSccsId[]="@(#)pr.c	1.6\t11/15/88\n";


static bool traces=true;
static int t0,t1;
static enum {ascii,floating,integer} outtype;
static enum {none,nonz,all,selected} dataout,headout;

static char *lsdoc = 
"supr [OPTIONS PARAMETERS key1 ke2 ... keyN] <stdin >stdout     \n\
                                                                \n\
OPTIONS:                                                        \n\
   v   verbose (default is no verbose)                \n\
   a   ascii output (default)                         \n\
   f   floating point binary output                   \n\
   i   integer binary output (turns off h)            \n\
       (f or i options turn off printing the ascii header)     \n\
   d   print data                                     \n\
   s   write the input su data on stdout                       \n\
   z   print non zero data                            \n\
                                                                \n\
PARAMETERS:                                                     \n\
                                                                \n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c,j;

	sdoc = lsdoc;
         SccsId = lSccsId;


	/* GET KEYS */
	dataout = none;
	headout = nonz;
	for(j=1;j<xargc;j++) {
		c = getindex(xargv[j]);
		if(c!= -1) {
			headout = selected;
			break;		/* ONE IS ENOUGH */
		}
	}

	/* GET OPTIONS */
	suout = false;
	while( (c=getopt(xargc,xargv,"adfhisvz"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'a':
			outtype = ascii;
			break;
		case 'f':
			outtype = floating;
			break;
		case 'i':
			outtype = integer;
			break;
		case 'd':
			dataout = all;
			break;
		case 'z':
			dataout = nonz;
			break;
		case 'h':
			traces = false;
			verbose = true;
			break;
		case 's':
			suout = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}


/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	float fval;
	int ival,i,j;
	value val;

	if(!traces) return(0);

	if(itr==0) {
		t0 = 0; 	igetpar("t0",&t0);
		t1 = abh->ns-1;	igetpar("t1",&t1);
		if ( t0 >= abh->ns -1 ) t0 = 0 ;
		if( t1 >= abh->ns ) t1 = abh->ns - 1 ;
	}

	if(headout==nonz) {

		fprintf(stdout,"TRACE %d: ",itr); hdpr(stdout,atrin);

	} else if(headout==selected) {

		for(i=1;i<xargc;i++) {

			if(*xargv[i]=='-') continue;

			j = getindex(xargv[i]);
			if(j== -1) continue;

			gethval(atrin,j,&val);

			if(outtype==ascii) {

				printf("%6s=",xargv[i]);
				printfval(hdtype(xargv[i]),val);
				printf("\t");

			} else if(outtype==floating) {

				fval = vtof(hdtype(xargv[i]),val);
				write(1,(char*)&fval,sizeof(float));

			} else if(outtype==integer) {

				ival = vtoi(hdtype(xargv[i]),val);
				write(1,(char*)&ival,sizeof(int));
			}
		}
		if(outtype==ascii) printf("\n");
	}

	if(dataout) prplot(atrin);

	if(suout) return(1);
	return(0);
}

prplot(atr)
Sutrace *atr;
{
	float max,min,scal,fval;
	int i,j,ival;

	if( t1 < t0) return ;

	max = min = atr->data[t0] ;
	for (i=t0;i<=t1;i++ ) {
		fval = atr->data[i] ;
		if(min > fval ) min = fval ;
		if(max < fval ) max = fval ;
	}
	if(min!=max) {
		scal = 60./(max-min);
	} else {
		fprintf(stdout,"\n\tZero Trace\n\n");
		return;
	}

	fprintf(stdout,"min=%e max=%e\n",min,max);

	for (i=t0;i<=t1;i++)
	{
		fprintf(stdout,"%5d %11.4e",i,atr->data[i]);
		ival = 1 + ( 0.5 + scal*(atr->data[i] - min ) ) ;
		for(j=0;j<ival;j++ )
		 	fputc(' ',stdout);
		fputc('*',stdout) ;
		fputc('\n',stdout);
	}
}

postp(){}
