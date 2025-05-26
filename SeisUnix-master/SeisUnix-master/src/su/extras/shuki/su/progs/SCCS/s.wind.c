h54202
s 00002/00002/00298
d D 1.6 88/11/15 14:03:06 shuki 6 5
c 
e
s 00006/00000/00294
d D 1.5 88/05/25 14:54:10 shemer 5 4
c with SccsId[]
e
s 00080/00060/00214
d D 1.4 88/05/24 06:54:33 shuki 4 3
c umainseq
e
s 00013/00067/00261
d D 1.3 88/05/19 07:15:41 shuki 3 2
c Took time window out and fixed a bug in bad list
e
s 00004/00000/00324
d D 1.2 88/05/17 09:33:38 shuki 2 1
c Patch mtov
e
s 00324/00000/00000
d D 1.1 88/04/14 13:52:52 shuki 1 0
c date and time created 88/04/14 13:52:52 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * suwind - window traces
 *
 * Examples:
 *	suwind <DATA key=tracl count=100 bad=45,52 | ...
 *	suwind <DATA key=trid bad=2 >LIVE
 *	suwind -m <DATA bad=3,4,5 | sugain ...
D 3
 *	suwind <DATA tmin=0.5 tmax=2 | sugain ...
E 3
 *
 * Credits:
 *	SEP: Einar
 *	CWP: Shuki, Jack
 *
 * Caveat:
D 3
 *	A cast to int is made for the s,j selection branch.
E 3
I 3
 *	A cast to int is made for the s,delta selection branch.
E 3
 *
*/

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

#define MININT -2147483648
#define MAXINT  2147483647
D 4
#define COUNT 	2*MAXINT		/* default count value		*/
E 4
I 4
#define COUNT 	2*MAXINT   /* default count value		*/
E 4

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
I 5
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";
E 5

I 5

E 5
D 4
static bool ab;		/* absolute value flag			*/
static bool markonly;	/* 1 = flag to mark traces		*/

static char key[50];	/* header key word from segy.h		*/
static char *type;	/* type of key				*/
static int index;	/* index of key				*/
static long min;	/* smallest key value to accept		*/
static long max;	/* largest key value to accept		*/
D 3
static int j;		/* take every jth trace ...		*/
static int s;		/* ... starting at the sth trace ...	*/
E 3
I 3
static int delta;		/* take every deltath trace ...		*/
static int skip;		/* ... starting at the skipth trace ...	*/
E 3
static unsigned long count;	/* ... up to a total of count traces	*/
static mixed mbadptr;	/* to arbitrate bad with getpar		*/
static unsigned nbad;	/* number of bad traces			*/
static value *badptr;	/* pointer to list of bad traces	*/
D 3
static float tmin,tmax;
E 3
static float dt;
D 3
static int itmin,itmax,nt;
static int nzeros;
E 3

void mtov();

E 4
D 3
/*
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
*/

E 3
static char *lsdoc = 
D 4
"									\n\
suwind <stdin >stdout [OPTIONS PARAMETERS]				\n\
									\n\
OPTIONS:								\n\
	-v	turn verbose on						\n\
	-m	pass everything but mark headers 			\n\
	-a	take absolute value of key header word			\n\
									\n\
PARAMETERS:								\n\
	key=cdp		Key header word to window on (see su.h)		\n\
	min=MININT	min value of key header word to pass		\n\
	max=MAXINT	max value of key header word to pass		\n\
D 3
	j=1 s=0		Pass every j-th trace starting at s		\n\
						(if((key-s)%j)==0	\n\
E 3
I 3
	delta=1		Trace pass increment						\n\
	skip=0		Number of traces to skip at the beginning	\n\
				Pass count traces, if((value(key)-skip)%delta)==0	\n\
E 3
	count=MAXUINT	total number of traces to pass			\n\
	bad=none	Delete traces with specified bad key values	\n\
									\n\
D 3
	tmin=0.0	min time to pass				\n\
	tmax=		max time to pass (default= from header)		\n\
	itmin=0		min time sample to pass				\n\
	itmax=		max time sample to pass (default= from header)	\n\
	nt=		number of time samples to pass. (nt=itmax-itmin+1)\n\
									\n\
E 3
MININT, MAXINT and MAXUNIT are about -2E9,+2E9 and 4E9 (with 32 bits)	\n\
E 4
I 4
"                                                           \n\
suwind <stdin >stdout [OPTIONS PARAMETERS]                  \n\
                                                            \n\
OPTIONS:                                                    \n\
 -v turn verbose on                                         \n\
 -m pass everything but mark headers                        \n\
 -a take absolute value of key header word                  \n\
                                                            \n\
PARAMETERS:                                                 \n\
  key=cdp       Key header word to window on (see su.h)     \n\
  min=MININT    min value of key header word to pass        \n\
  val0=0        min if min= is specified                    \n\
  max=MAXINT    max value of key header word to pass        \n\
  delta=1       Trace pass increment                        \n\
  skip=0        Number of traces to skip at the beginning   \n\
  count=MAXUINT total number of traces to pass              \n\
  bad=none      Delete traces with specified bad key values \n\
                                                            \n\
FORMULA                                                     \n\
  i = value(key)                                            \n\
  Skip skip traces, then pass count traces, if i<max and    \n\
    i>min and  (i-val0)%delta==0                            \n\
                                                           \n\
MININT, MAXINT and MAXUNIT are about -2E9,+2E9 and 4E9     \n\
E 4
";
I 4
static bool ab;         /* absolute value flag */
static int markonly;   /* 1 = flag to mark traces */
D 6
static char key[50];    /* header key word from su.h */
E 6
I 6
static char *key;    /* header key word from su.h */
E 6
static char *type;      /* type of key */
static int index;       /* index of key */
static long min;        /* smallest key value to accept */
static long max;        /* largest key value to accept */
static int delta;       /* take every deltath trace ... */
static int skip;        /* ... starting at the skipth trace ... */
static unsigned long count; /* ... up to a total of count traces */
static mixed mbadptr;   /* to arbitrate bad with getpar */
static unsigned nbad;   /* number of bad traces  */
static value *badptr;   /* pointer to list of bad traces */
static int val0;
E 4

I 4
void mtov();

E 4
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
I 5
        SccsId = lSccsId;

E 5

	/* GET OPTIONS */
	verbose = false;
D 4
	markonly = false;
E 4
I 4
	markonly = 0;
E 4
	ab = false;
	while( (c=getopt(xargc,xargv,"vma"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'm':
D 4
			markonly = true;
E 4
I 4
			markonly = 1;
E 4
			break;
		case 'a':
			ab = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
	/* GET PARAMETERS */
D 4
	strcpy(key, "cdp");	sgetpar("key"     , key);
	min = MININT;		lgetpar("min"     , &min);
	max = MAXINT;		lgetpar("max"     , &max);
D 3
	j = 1;			igetpar("j"       , &j);
	s = 0;			igetpar("s"       , &s);
E 3
I 3
	delta = 1;			igetpar("delta"       , &delta);
	skip = 0;			igetpar("skip"       , &skip);
E 3
	count = COUNT;		lgetpar("count"   , &count);
E 4
I 4
D 6
	strcpy(key, "cdp");	sgetpar("key"  , key);
E 6
I 6
	key="cdp";	sgetpar("key",&key);
E 6
	if(igetpar("min"  , &min)) {
		val0 = min;
	} else {
		min = MININT;
		val0 = 0;
	}
	max = MAXINT;		igetpar("max"  , &max);
	delta = 1;			igetpar("delta", &delta);
	skip = 0;			igetpar("skip" , &skip);
	count = COUNT;		igetpar("count", &count);
E 4

	/* KEYWORD TYPE AND INDEX */
	type = hdtype(key);
	index = getindex(key);

	/* BAD */
	nbad = maxgetpar();
	switch(*type) {

		case 'l':
		mbadptr.l = (long *) malloc((unsigned) (nbad*sizeof(long)));
		nbad = lgetpar("bad", mbadptr);
		break;

		case 'd':
		mbadptr.d = (int *) malloc((unsigned) (nbad*sizeof(int)));
		nbad = igetpar("bad", mbadptr);
		break;

		case 'h':
		mbadptr.h = (short *) malloc((unsigned) (nbad*sizeof(short)));
		nbad = hgetpar("bad", mbadptr);
		break;

		case 'u':
		mbadptr.u = (unsigned short *)
			malloc((unsigned) (nbad*sizeof(unsigned short)));
		nbad = ugetpar("bad", mbadptr);
		break;

		case 'f':
		mbadptr.f = (float *) malloc((unsigned) (nbad*sizeof(float)));
		nbad = fgetpar("bad", mbadptr);
		break;

		case 'z':
		mbadptr.z = (double *) malloc((unsigned) (nbad*sizeof(double)));
		nbad = zgetpar("bad", mbadptr);
		break;
I 3

		default:
		nbad = 0;

E 3
	}

/* 	badptr = (value*) realloc(mbadptr,(unsigned)(nbad*sizeof(double))); */
	badptr = (value*) malloc((unsigned) (nbad*sizeof(double)));

	mtov(badptr, mbadptr, nbad, type);

I 2

E 2
}

I 4
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}
E 4

/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
D 4
trseq(itr,atr,abh)
E 4
I 4
trseq(itr,atrin,atrout,abh)
E 4
int itr;
D 4
Sutrace *atr;
E 4
I 4
Sutrace *atrin,*atrout;
E 4
Subhed *abh;
{
D 3
	char *pzeros;
E 3
D 4
	value val;	/* value of key				*/
	int ival;	/* ... cast to int			*/
	bool isbad;	/* flag for bad trace			*/
	int i;		/* counter				*/
E 4
I 4
	value val;	/* value of key */
	int ival;	/* ... cast to int */
	bool isbad;	/* flag for bad trace */
	static int iskip=0;		/* counter */
	int i=0;		/* counter */
E 4
	value vtoabs();

D 3
	if(itr==0) {
		/* TMIN & TMAX */
		dt = 0.000001*abh->dt;

		itmin=0;		igetpar("itmin"   , &itmin);
		if(fgetpar("tmin",&tmin)) {
			itmin=tmin/dt;
		} else {
			tmin = itmin*dt;
		}

		itmax=abh->ns-1;		igetpar("itmax"   , &itmax);
		if(fgetpar("tmax",&tmax)) {
			itmax=tmax/dt;
		} else {
			tmax =itmax*dt;
		}

		if(igetpar("nt",&nt)) {
			itmax = itmin+nt-1;
			tmax =itmax*dt;
		} else {
			nt =itmax-itmin+1;
		}

		if(itmin<0 || itmin>itmax)
			err(__FILE__,__LINE__,"Illegal itmin=%d or itmax=%d",itmin,itmax);

		nzeros = (nt-abh->ns)*sizeof(float);

		pzeros = (char*) (atr->data + abh->ns - itmin);

	} /* END OF IF(ITR++) */

	/* TIME WINDOW */
	if(itmin>0) {
		for(i=itmin;i<=itmax;i++) {
			atr->data[i-itmin] = atr->data[i];
		}
	}
	if(nzeros>0) bzero(pzeros,nzeros);
	abh->ns = nt;

	/* TRACE WINDOW */
E 3
D 4
	gethval(atr, index, &val);
E 4
I 4
/* Skip skip traces, then pass count traces, if i<max and i>min and  (i-val0)%delta==0 */

	if(iskip++ < skip) return(markonly);

	gethval(atrin, index, &val);
E 4
	isbad = false;
	for (i = 0; i < nbad; i++) {
		if (!valcmp(type, val, badptr[i])) {
			isbad = true;
			break;	/* found */
		}
	}

	if (ab) val = vtoabs(type, val);

	ival = vtoi(type, val);
D 3
	if ((min <= ival) && (ival <= max) && !((ival - s) % j) && !isbad && count) {
E 3
I 3
D 4
	if ((min <= ival) && (ival <= max) && !((ival - skip) % delta) && !isbad && count) {
E 3
		--count;
/* 		if (markonly) atr->mark = 1; */
E 4
I 4

	if((ival>=min)&&(ival<=max)&&((ival-val0)%delta==0)&&isbad==false&&count) {
		count--;
		if (markonly) atrin->mark = 1;
E 4
		return(1);
D 4
	} else { /* trace not selected */
		if (markonly) return(1);
E 4
	}
D 4

	return(0);
E 4
I 4
	return(markonly);
E 4
}


void mtov(valp, mixptr, n, tp)
value *valp;
mixed mixptr;
int n;
char *tp;
{
	int i;
I 2

	warn(__FILE__,__LINE__,"mtov stub");
	return;
E 2

	switch(*tp) {
	case 'h':
		for (i = 0; i < n; i++) {
			(valp++)->h = *mixptr.h++;
		}
	break;
	case 'u':
		for (i = 0; i < n; i++) {
			(valp++)->u = *mixptr.u++;
		}
	break;
	case 'l':
		for (i = 0; i < n; i++) {
			(valp++)->l = *mixptr.l++;
		}
	break;
	case 'v':
		for (i = 0; i < n; i++) {
			(valp++)->v = *mixptr.v++;
		}
	break;
	case 'i':
		for (i = 0; i < n; i++) {
			(valp++)->i = *mixptr.d++;
		}
	break;
	case 'f':
		for (i = 0; i < n; i++) {
			(valp++)->f = *mixptr.f++;
		}
	break;
	case 'z':
		for (i = 0; i < n; i++) {
			(valp++)->z = *mixptr.z++;
		}
	break;
	default:
		err(__FILE__,__LINE__,"mysterious type %s", type);
		break;
	}
}


value vtoabs(tp, val)
char *tp;
value val;
{
	switch(*tp) {
	case 'u': 
	case 'v':
	break;
	case 'h':
		val.h = ABS(val.h);
	break;
	case 'l':
		val.l = ABS(val.l);
	break;
	case 'i':
		val.i = ABS(val.i);
	break;
	case 'f':
		val.f = ABS(val.f);
	break;
	case 'z':
		val.z = ABS(val.z);
	break;
	default: err(__FILE__,__LINE__,"mysterious type %s", tp);
	}
	return(val);
}
I 4

postp(){}
E 4
E 1
