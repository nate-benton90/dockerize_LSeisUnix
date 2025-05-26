h37075
s 00007/00007/00148
d D 1.4 88/11/15 14:02:45 shuki 4 3
c 
e
s 00006/00000/00149
d D 1.3 88/05/25 14:53:52 shemer 3 2
c with SccsId[]
e
s 00014/00003/00135
d D 1.2 88/05/25 06:53:04 shuki 2 1
c umainseq
e
s 00138/00000/00000
d D 1.1 88/04/14 13:52:38 shuki 1 0
c date and time created 88/04/14 13:52:38 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * shw - Set Header Word using trace number, mod and div
 *
 * Example:
 *	sushw key=tracr a=1001 c=1 <infile >outfile
 *	(sets tracr to 1000 + trace number in file)
 *
 * Caveat:
 *	All constants are cast to doubles.
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
"sushw [OPTIONS PARAMETERS] <stdin >stdout		\n\
							\n\
header word key is computed using the formula:		\n\
							\n\
val(key) = a + b * ((itr+d) % j) + c * ((itr+d) / j)	\n\
where itr is the trace number in file, starting with 0	\n\
							\n\
OPTIONS:						\n\
	-v	turn verbose on				\n\
							\n\
PARAMETERS:						\n\
	key=cdp						\n\
	a=0						\n\
	b=0						\n\
	c=0						\n\
	d=0						\n\
	j=1						\n\
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
	char key[50];
E 4
I 4
	char *key;
E 4
	char *type;
	int index;
	double a, c, b, d, i, j;
	value val;
	void setval();

	if(itr==0) {

		/* GET PARAMETERS */
D 4
		strcpy(key, "cdp");	sgetpar("key", key);
		a = 0;			zgetpar("a"  , &a);
		b = 0;			zgetpar("b"  , &b);
		c = 0;			zgetpar("c"  , &c);
		d = 0;			zgetpar("d"  , &d);
		j = 1;			zgetpar("j"  , &j);
E 4
I 4
		key="cdp";	sgetpar("key", &key);
		a = 0;		zgetpar("a"  , &a);
		b = 0;		zgetpar("b"  , &b);
		c = 0;		zgetpar("c"  , &c);
		d = 0;		zgetpar("d"  , &d);
		j = 1;		zgetpar("j"  , &j);
E 4

		type = hdtype(key);
		index = getindex(key);
	}

	i = (double) itr++ + d;
	setval(type, &val, a, b, c, i, j);
	puthval(atr, index, &val);

	return(1);
}

void setval(type, valp, a, b, c, i, j)
char *type;
value *valp;
double a, c, b, i, j;
{
	double mod();

	switch(*type) {
	case 's':
		err(__FILE__,__LINE__,"can't set char header word");
	break;
	case 'h':
		valp->h = a + b*mod(i, j) + c*(i / j);
	break;
	case 'u':
		valp->u = a + b*mod(i, j) + c*(i / j);
	break;
	case 'l':
		valp->l = a + b*mod(i, j) + c*(i / j);
	break;
	case 'v':
		valp->v = a + b*mod(i, j) + c*(i / j);
	break;
	case 'i':
		valp->i = a + b*mod(i, j) + c*(i / j);
	break;
	case 'f':
		valp->f = a + b*mod(i, j) + c*(i / j);
	break;
	case 'z':
		valp->z = a + b*mod(i, j) + c*(i / j);
	default:
		err(__FILE__,__LINE__,"unknown type %s", type);
	break;
	}
}


double mod(x, y)	/* As defined in Knuth, vol. 1	*/
double x, y;
{
	return(y == 0.0 ? x : x - y * floor(x/y));
}
I 2

postp(){}
E 2
E 1
