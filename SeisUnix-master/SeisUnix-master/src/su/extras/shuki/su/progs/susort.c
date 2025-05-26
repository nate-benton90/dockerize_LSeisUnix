/* susort - sort on any segy header keywords
 *
 * Example:
 *	To sort traces by cdp gather and within each gather by shot
 *	point with both sorts in ascending order:
 *
 *	susort <INDATA >OUTDATA cdp sx
 *
 * Credits:
 *	SEP: Einar, Stew
 *	CWP: Shuki, Jack
 *
 * Caveats:
 *	Since the algorithm depends on sign reversal of the key value
 *	to obtain a descending sort, the most significant figure may
 *	be lost for unsigned data types.
 *
 *	The flat list, val_list, should be replaced by a meaningful data
 *	structure.
 *
*/

/*********************** self documentation **********************/
char *sdoc = "\
								\n\
susort [[+-]key1 [+-]key2 ...] <stdin >stdout		\n\
								\n\
	supports any number of (secondary) keys with either	\n\
	ascending (+, the default) or descending (-) directions	\n\
	for each.  The default sort key is cdp.			\n\
								\n\
	The input must be either a diskfile or tapefile.    	\n\
	In the latter case, the output must be a diskfile.	\n\
								\n\
";
/*****************************************************************/

#include <stdio.h>
#include "../include/su.h"

#define NTRSTEP	1000	/* realloc() increment measured in traces */

int xargc;
char **xargv;
bool verbose;		/* unsupported */
char *SccsId[]="@(#)susort.c	1.3\t11/15/88\n";


static int nkey;	/* number of keys to sort on	*/
static char *type;	/* header key types		*/

main(argc, argv)
int argc; char **argv;
{
	Sutrace tr;
	Subhed bh;
	static int *index;	/* header key indices			*/
  	static bool *up;	/* sort direction (+ = up = ascending)	*/
	static value *val_list;	/* a list of the key values for each
				   trace with each group headed by the
				   trace number of that trace		*/
	register value *vptr;	/* location pointer for val_list	*/
	int ngroup;		/* size of unit in val_list (nkey + 1)	*/
	int nv;		/* bytes allocated for val_list		*/
	int nvstep;		/* incremental allocation for val_list	*/
  	value val;		/* value of a keyword			*/
	long ntr;		/* number of traces from gettr		*/
	long itr;		/* index of trace (0,1,2,...)		*/
  	int nindex;		/* bytes allocated for index		*/
  	int nup;		/* bytes allocated for up		*/
	filetype ftypein;	/* filetype of stdin			*/
	filetype ftypeout;	/* filetype of stdout			*/
	bool disk_in;		/* is stdin a diskfile?			*/
	int nsegy;		/* number of bytes on the trace		*/
	int nwrite;		/* number of bytes written		*/
	register int i;		/* for loop counter			*/
	int cmp_list();		/* comparison function for qsort	*/
	value negval();

	int infd,outfd;

	xargc = argc; xargv = argv;

	infd = input();
	outfd = output();

	/* Stdin must be rewound.  This means stdin is a diskfile or
	 * tapefile.  We also random access to either stdin or stdout.
	 * Thus, if stdin is a tapefile, we demand that stdout be a
	 * diskfile
	 */
	ftypein = statfil(infd);
	ftypeout = statfil(outfd);
	if (ftypein == DISK)
		disk_in = true;
	else if (ftypein == TAPE && ftypeout == DISK)
		disk_in = false;	/* but output is seekable */
	else
		err(__FILE__,__LINE__,"if stdin is not a diskfile, we must go tape to disk");


	/* Set number of sort keys */
	if (argc == 1)	/* no explicit keys: default cdp key */
		nkey = 1;
	else		/* one or more explicit keys */
		nkey = argc - 1;

	/* Allocate room for key indices and sort directions */
  	nindex = nkey * sizeof(int);
  	if (NULL == (index = (int *) malloc(nindex)))
  		err(__FILE__,__LINE__,"unable to allocate %d bytes", nindex);
  	nup = nkey * sizeof(bool);
  	if (NULL == (up = (bool *) malloc(nup)))
  		err(__FILE__,__LINE__,"unable to allocate %d bytes", nup);

	/* Allocate room for key types */
	if (NULL == (type = (char *) malloc(nkey)))
		err(__FILE__,__LINE__,"unable to allocate %d bytes", nkey);

	/* Allocate room for list of trace numbers and key values */
	ngroup = nkey + 1;
	nv = NTRSTEP * ngroup * sizeof(value);
	if (NULL == (val_list = (value *) malloc(nv)))
		err(__FILE__,__LINE__,"unable to allocate %d bytes", nv);
	vptr = val_list;
	nvstep = nv;

	/* Initialize index, type and up */
	if (argc == 1) {
		index[0] = getindex("cdp");
		up[0] = true;
		type[0] = 'l';
	} else {
		for (i = 0; i < nkey; i++) {
			switch(**++argv) { /* sign char of next arg */
			case '+':
				up[i] = true;
				++*argv;   /* discard sign char in arg */
			break;
			case '-':
				up[i] = /*-*/false;
				++*argv;   /* discard sign char in arg */
			break;
			default:
				up[i] = true;
			break;
			}
			index[i] = getindex(*argv);
			type[i] = hdtype(*argv)[0]; /* need single char */
		}
	}

	apass(infd,outfd);

	getbh(infd,&bh);

	tr.data = (float*)malloc(bh.ns*bh.esize);

	hislog(outfd);
/* 	hisclose(outfd); */

	putbh(outfd,&bh);

	/* Run through traces once to collect header values */
	ntr = 0;
	while (gettr(infd,&tr)) {
		itr = ntr++;
		/* realloc if out of room */
		if (0 == (ntr % NTRSTEP)) {
			nv += nvstep;
			if (NULL == (val_list =
			    (value *) realloc((char *) val_list, nv)))
				err(__FILE__,__LINE__,"unable to allocate %d bytes", nv);
			vptr = val_list + itr * ngroup;
		}

		/* enter trace index in list and then key values */
		vptr++->l = itr;	/* insert number and advance */
		for (i = 0; i < nkey; i++) {
			gethval(&tr, index[i], &val);
			*vptr++ = up[i] ? val : negval(type + i, val);
				/* note: negation reverses sort direction */
		}
	}
	rew(infd);

	/* Sort the headers values */
	qsort((char *) val_list, ntr, ngroup*sizeof(value), cmp_list);

	if (disk_in) {
		/* run through sorted list and write output sequentially */
		for (i = 0; i < ntr; i++) {
			itr = val_list[i*ngroup].l;
			gettra(infd,&tr,itr);
			puttr(outfd,&tr);
		}
	} else {
		/* invert permutation and read input sequentially */
		for (i = 0; i < ntr; i++) {
			itr = val_list[i*ngroup].l;
			val_list[itr*ngroup + 1].l = i;
	        }
	        for (i = 0; i < ntr; i++) {
			itr = val_list[i*ngroup + 1].l;
		        if (!(nsegy = gettr(infd,&tr)))
				err(__FILE__,__LINE__,"unable to re-read input\n");
		        tr.tracl = tr.tracr = itr + 1;
		        if (-1 == lseek(outfd, (long) itr*nsegy, 0)) {
			    err(__FILE__,__LINE__,"lseek error on output disk file");
			}
			nwrite = write(outfd, (char *) &tr, nsegy);
			switch (nwrite) {
			case -1:
				err(__FILE__,__LINE__,"error writing trace #%d", i);
			default:
				if (nwrite != nsegy) {
				    err(__FILE__,__LINE__,"error writing trace #%d", i);
				}
			}
	        }
	}
	exit(0);
}


/* Comparison routine for qsort */
int cmp_list(a, b)
register value *a, *b;
{
	register int i;
	value va, vb;
	int compare;

	/* Can order as soon as any components are unequal */
	for (i = 0; i < nkey; i++) {
		va = *++a; vb = *++b; /* advance and dereference */
		if (compare = valcmp(type + i, va, vb))
			return(compare);
        }
        return(0);
}


value negval(ltype, val)
char *ltype;
value val;
{
	switch(*ltype) {
	case 'h':
		val.h = -val.h;
	break;
	case 'u': 
		val.u = (short) -val.u;
	break;
	case 'l':
		val.l = -val.l;
	break;
	case 'v':
		val.u = (long) -val.u;
	break;
	case 'i':
		val.i = -val.i;
	break;
	case 'f':
		val.f = -val.f;
	break;
	case 'z':
		val.z = -val.z;
	break;
	default: err(__FILE__,__LINE__,"mysterious type %s", ltype);
	}
	return(val);
}
