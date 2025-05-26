/*
 * chw - Change Header Word using up to two existing header word fields
 */
/*
 * Example:
 *	suchw key1=tracr key2=tracr a=1000 <infile >outfile
 *	(adds 1000 to tracr value)
 *
 * Caveat:
 *	The constants a, b, c, d are read in as doubles.
 *	It is implicitly assumed that the data types of the
 *	keys used are the same.
 *
 */

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
static char lSccsId[]="@(#)chw.c	1.4\t11/15/88\n";


static char *lsdoc = 
"suchw [OPTIONS PARAMETERS] <stdin >stdout		\n\
							\n\
header word key1 is computed using the formula:		\n\
val(key1) = (a + b * val(key2) + c * val(key3)) / d	\n\
							\n\
OPTIONS:						\n\
	-v	turn verbose on				\n\
							\n\
PARAMETERS:						\n\
	key1=cdp					\n\
	key2=cdp					\n\
	key3=cdp					\n\
	a=0						\n\
	b=1						\n\
	c=0						\n\
	d=1						\n\
							\n";

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
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
}

 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
Subhed *abh;
{
	static char *key1,*key2,*key3;
	static char *type1;
	static int index1, index2, index3;
	value val1, val2, val3;
	static double a, c, b, d;
	void changeval();

	if(itr==0) {

		/* GET PARAMETERS */
		key1="cdp";	sgetpar("key1",&key1);
		key2="cdp";	sgetpar("key2",&key2);
		key3="cdp";	sgetpar("key3",&key3);
		a = 0;			zgetpar("a"   , &a);
		b = 1;			zgetpar("b"   , &b);
		c = 0;			zgetpar("c"   , &c);
		d = 1;			zgetpar("d"   , &d);

		type1 = hdtype(key1);
		index1 = getindex(key1);
		index2 = getindex(key2);
		index3 = getindex(key3);
	}

	gethval(atr, index2, &val2);
	gethval(atr, index3, &val3);
	changeval(type1, &val1, &val2, &val3, a, b, c, d);
	puthval(atr, index1, &val1);

	return(1);
}

void changeval(type1, valp1, valp2, valp3, a, b, c, d)
char *type1;
value *valp1, *valp2, *valp3;
double a, b, c, d;
{
	switch(*type1) {
	case 's':
		err(__FILE__,__LINE__,"can't change char header word");
	break;
	case 'h':
		valp1->h = (a + b * valp2->h + c * valp3->h)/d;
	break;
	case 'u':
		valp1->u = (a + b * valp2->u + c * valp3->u)/d;
	break;
	case 'l':
		valp1->l = (a + b * valp2->l + c * valp3->l)/d;
	break;
	case 'v':
		valp1->v = (a + b * valp2->v + c * valp3->v)/d;
	break;
	case 'i':
		valp1->i = (a + b * valp2->i + c * valp3->i)/d;
	break;
	case 'f':
		valp1->f = (a + b * valp2->f + c * valp3->f)/d;
	break;
	case 'z':
		valp1->z = (a + b * valp2->z + c * valp3->z)/d;
	break;
	default:
		err(__FILE__,__LINE__,"unknown type %s", type1);
	break;
	}
}

postp(){}
