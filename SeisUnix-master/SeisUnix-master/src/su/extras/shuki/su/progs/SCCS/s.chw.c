h60711
s 00004/00004/00148
d D 1.4 88/11/15 14:02:26 shuki 4 3
c 
e
s 00005/00000/00147
d D 1.3 88/05/25 14:53:39 shemer 3 2
c with SccsId[]
e
s 00015/00003/00132
d D 1.2 88/05/25 06:52:55 shuki 2 1
c umainseq
e
s 00135/00000/00000
d D 1.1 88/04/14 13:52:26 shuki 1 0
c date and time created 88/04/14 13:52:26 by shuki
e
u
U
f e 0
t
T
I 1
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
I 3
extern char *SccsId;
static char lSccsId[]="%W%\t%G%\n";
E 3

I 3

E 3
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
I 3
        SccsId = lSccsId;

E 3

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

I 2
 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
    *aatrout = atrin;
}
 
E 2
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
D 2
trseq(itr,atr,abh)
int itr;
Sutrace *atr;
E 2
I 2
trseq(itr,atr,atrout,abh)
int itr;    
Sutrace *atr,*atrout;
E 2
Subhed *abh;
{
D 4
	static char key1[50], key2[50], key3[50];
E 4
I 4
	static char *key1,*key2,*key3;
E 4
	static char *type1;
	static int index1, index2, index3;
	value val1, val2, val3;
	static double a, c, b, d;
	void changeval();

	if(itr==0) {

		/* GET PARAMETERS */
D 4
		strcpy(key1, "cdp");	sgetpar("key1", key1);
		strcpy(key2, "cdp");	sgetpar("key2", key2);
		strcpy(key3, "cdp");	sgetpar("key3", key3);
E 4
I 4
		key1="cdp";	sgetpar("key1",&key1);
		key2="cdp";	sgetpar("key2",&key2);
		key3="cdp";	sgetpar("key3",&key3);
E 4
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
I 2

postp(){}
E 2
E 1
