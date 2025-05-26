h18503
s 00000/00000/00197
d D 1.6 88/11/15 14:02:42 shuki 6 5
c 
e
s 00003/00003/00194
d D 1.5 88/06/06 13:12:18 shuki 5 4
c Cancel ns in trace headers
e
s 00006/00000/00191
d D 1.4 88/05/25 14:53:49 shemer 4 3
c with SccsId[]
e
s 00033/00018/00158
d D 1.3 88/05/23 10:19:42 shuki 3 2
c umainseq
e
s 00013/00015/00163
d D 1.2 88/04/20 07:25:19 shuki 2 1
c 
e
s 00178/00000/00000
d D 1.1 88/04/14 13:52:36 shuki 1 0
c date and time created 88/04/14 13:52:36 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * pr - print non zero header values and data for non graphic terminals
 */
D 2

E 2
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
/* #include "../include/hdrs.h" */

extern char *sdoc;
extern int xargc;
extern char **xargv;
D 3
extern bool verbose;
E 3
I 3
extern bool verbose,suout;
I 4
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 4
E 3

I 4

E 4
static bool traces=true;
static int t0,t1;
static enum {ascii,floating,integer} outtype;
static enum {none,nonz,all,selected} dataout,headout;

static char *lsdoc = 
D 2
"supr [OPTIONS PARAMETERS key1 ke2 ... keyN] <stdin >stdout	\n\
								\n\
OPTIONS:							\n\
	v	verbose (default is no verbose)			\n\
	a	ascii output (default)				\n\
	f	floating point binary output			\n\
	i	integer binary output (turns off h)		\n\
	(f or i options turn off printing the ascii header)	\n\
	d	print data 					\n\
	z	print non zero data				\n\
								\n\
PARAMETERS:							\n\
								\n\
								\n";
E 2
I 2
"supr [OPTIONS PARAMETERS key1 ke2 ... keyN] <stdin >stdout     \n\
                                                                \n\
OPTIONS:                                                        \n\
D 3
        v        verbose (default is no verbose)                \n\
        a        ascii output (default)                         \n\
        f        floating point binary output                   \n\
        i        integer binary output (turns off h)            \n\
        (f or i options turn off printing the ascii header)     \n\
        d        print data                                     \n\
        z        print non zero data                            \n\
E 3
I 3
   v   verbose (default is no verbose)                \n\
   a   ascii output (default)                         \n\
   f   floating point binary output                   \n\
   i   integer binary output (turns off h)            \n\
       (f or i options turn off printing the ascii header)     \n\
   d   print data                                     \n\
   s   write the input su data on stdout                       \n\
   z   print non zero data                            \n\
E 3
                                                                \n\
PARAMETERS:                                                     \n\
                                                                \n";
E 2

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c,j;

	sdoc = lsdoc;
I 4
         SccsId = lSccsId;

E 4

	/* GET KEYS */
	dataout = none;
	headout = nonz;
	for(j=1;j<xargc;j++) {
D 3

E 3
		c = getindex(xargv[j]);
		if(c!= -1) {
			headout = selected;
			break;		/* ONE IS ENOUGH */
		}
	}

	/* GET OPTIONS */
D 3
	while( (c=getopt(xargc,xargv,"vafidzh"))!=EOF) {
E 3
I 3
	suout = false;
	while( (c=getopt(xargc,xargv,"adfhisvz"))!=EOF) {
E 3
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
I 3
		case 's':
			suout = true;
			break;
E 3
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
}

D 3
trseqn(itr,atr,abh)
E 3
I 3

/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

trseq(itr,atrin,atrout,abh)
E 3
int itr;
D 3
Sutrace *atr;
E 3
I 3
Sutrace *atrin,*atrout;
E 3
Subhed *abh;
{
	float fval;
	int ival,i,j;
	value val;

	if(!traces) return(0);

	if(itr==0) {
		t0 = 0; 	igetpar("t0",&t0);
D 3
		t1 = atr->ns-1;	igetpar("t1",&t1);
		if ( t0 >= atr->ns -1 ) t0 = 0 ;
		if( t1 >= atr->ns ) t1 = atr->ns - 1 ;
E 3
I 3
D 5
		t1 = atrin->ns-1;	igetpar("t1",&t1);
		if ( t0 >= atrin->ns -1 ) t0 = 0 ;
		if( t1 >= atrin->ns ) t1 = atrin->ns - 1 ;
E 5
I 5
		t1 = abh->ns-1;	igetpar("t1",&t1);
		if ( t0 >= abh->ns -1 ) t0 = 0 ;
		if( t1 >= abh->ns ) t1 = abh->ns - 1 ;
E 5
E 3
	}

	if(headout==nonz) {

D 3
		fprintf(stdout,"TRACE %d: ",itr); hdpr(stdout,atr);
E 3
I 3
		fprintf(stdout,"TRACE %d: ",itr); hdpr(stdout,atrin);
E 3

	} else if(headout==selected) {

		for(i=1;i<xargc;i++) {

			if(*xargv[i]=='-') continue;

			j = getindex(xargv[i]);
			if(j== -1) continue;

D 3
			gethval(atr,j,&val);
E 3
I 3
			gethval(atrin,j,&val);
E 3

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

D 3
	if(dataout) prplot(atr);
E 3
I 3
	if(dataout) prplot(atrin);
E 3

I 3
	if(suout) return(1);
E 3
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
E 1
