/*
 * suwind - window traces
 *
 * Examples:
 *	suwind <DATA key=tracl count=100 bad=45,52 | ...
 *	suwind <DATA key=trid bad=2 >LIVE
 *	suwind -m <DATA bad=3,4,5 | sugain ...
 *
 * Credits:
 *	SEP: Einar
 *	CWP: Shuki, Jack
 *
 * Caveat:
 *	A cast to int is made for the s,delta selection branch.
 *
*/

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

#define MININT -2147483648
#define MAXINT  2147483647
#define COUNT 	2*MAXINT   /* default count value		*/

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)wind.c	1.6\t11/15/88\n";


static char *lsdoc = 
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
";
static bool ab;         /* absolute value flag */
static int markonly;   /* 1 = flag to mark traces */
static char *key;    /* header key word from su.h */
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

void mtov();

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


	/* GET OPTIONS */
	verbose = false;
	markonly = 0;
	ab = false;
	while( (c=getopt(xargc,xargv,"vma"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case 'm':
			markonly = 1;
			break;
		case 'a':
			ab = true;
			break;
		case '?':
			warn(__FILE__,__LINE__,"getopt returned '?'");
		}
	}
	/* GET PARAMETERS */
	key="cdp";	sgetpar("key",&key);
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

		default:
		nbad = 0;

	}

/* 	badptr = (value*) realloc(mbadptr,(unsigned)(nbad*sizeof(double))); */
	badptr = (value*) malloc((unsigned) (nbad*sizeof(double)));

	mtov(badptr, mbadptr, nbad, type);


}

prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atrin,atrout,abh)
int itr;
Sutrace *atrin,*atrout;
Subhed *abh;
{
	value val;	/* value of key */
	int ival;	/* ... cast to int */
	bool isbad;	/* flag for bad trace */
	static int iskip=0;		/* counter */
	int i=0;		/* counter */
	value vtoabs();

/* Skip skip traces, then pass count traces, if i<max and i>min and  (i-val0)%delta==0 */

	if(iskip++ < skip) return(markonly);

	gethval(atrin, index, &val);
	isbad = false;
	for (i = 0; i < nbad; i++) {
		if (!valcmp(type, val, badptr[i])) {
			isbad = true;
			break;	/* found */
		}
	}

	if (ab) val = vtoabs(type, val);

	ival = vtoi(type, val);

	if((ival>=min)&&(ival<=max)&&((ival-val0)%delta==0)&&isbad==false&&count) {
		count--;
		if (markonly) atrin->mark = 1;
		return(1);
	}
	return(markonly);
}


void mtov(valp, mixptr, n, tp)
value *valp;
mixed mixptr;
int n;
char *tp;
{
	int i;

	warn(__FILE__,__LINE__,"mtov stub");
	return;

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

postp(){}
