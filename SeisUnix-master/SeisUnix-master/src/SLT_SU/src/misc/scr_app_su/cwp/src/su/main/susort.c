/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/* SUSORT: $Revision: 1.24 $ ; $Date: 1996/09/18 19:47:27 $	*/

#include "su.h"
#include "segy.h"
#include <signal.h>

/*********************** self documentation **********************/
char *sdoc[] = {
" 								",
" SUSORT - sort on any segy header keywords			",
" 								",
" susort <stdin >stdout [[+-]key1 [+-]key2 ...]			",
" 								",
" Susort supports any number of (secondary) keys with either	",
" ascending (+, the default) or descending (-) directions for 	",
" each.  The default sort key is cdp.				",
" 								",
" Note:	Only the following types of input/output are supported	",
"	Disk input --> any output				",
"	Pipe input --> Disk output				",
" 								",
" Note: If the the CWP_TMPDIR environment variable is set use	",
"	its value for the path; else use tmpfile()		",
" 								",
" Example:							",
" To sort traces by cdp gather and within each gather		",
" by offset with both sorts in ascending order:			",
" 								",
" 	susort <INDATA >OUTDATA cdp offset			",
" 								",
" Caveat: In the case of Pipe input a temporary file is made	",
"	to hold the ENTIRE data set.  This temporary is		",
"	either an actual disk file (usually in /tmp) or in some	",
"	implementations, a memory buffer.  It is left to the	",
"	user to be SENSIBLE about how big a file to pipe into	",
"	susort relative to the user's computer.			",
" 								",
NULL};

/* Credits:
 *	SEP: Einar, Stew
 *	CWP: Shuki, Jack
 *        : Zhiming 
 *
 * Caveats:
 *	Since the algorithm depends on sign reversal of the key value
 *	to obtain a descending sort, the most significant figure may
 *	be lost for unsigned data types.  The old SEP support for tape
 *	input was removed in version 1.16---version 1.15 is in the
 *	Portability directory for those who may want to input SU data
 *	stored on tape.
 *
 */
/**************** end self doc ***********************************/


#define NTRSTEP	1024	/* realloc() increment measured in traces */

segychdr ch;
segybhdr bh;
segy tr;
static int nkey;	/* number of keys to sort on	*/
static String type;	/* header key types		*/

/* Prototypes */
Value negval(String type, Value val);   /* reverse sign of value	*/
int cmp_list();				/* qsort comparison function	*/
static void closefiles(void);		/* signal handler		*/

/* Globals (so can trap signal) defining temporary disk files */
char tracefile[BUFSIZ];	/* filename for trace storage file	*/
FILE *tracefp;		/* fp for trace storage file		*/


int
main(int argc, char **argv)
{
	static Value *val_list;	/* a list of the key values for each    */
				/* trace with each group headed by the	*/
				/* trace number of that trace		*/
	static int *index;	/* header key indices			*/
  	static bool *up;	/* sort direction (+ = up = ascending)	*/
	register Value *vptr;	/* location pointer for val_list	*/
	int ngroup;		/* size of unit in val_list (nkey + 1)	*/
	int nv;			/* number of groups in val_list		*/
	int nvsize;		/* size of group in val_list		*/
  	Value val;		/* value of a keyword			*/
	long ntr;		/* number of traces from gettr		*/
	long itr;		/* index of trace (0,1,2,...)		*/
	filetype ftypein;	/* filetype of stdin			*/
	filetype ftypeout;	/* filetype of stdout			*/
	int nsegy;		/* number of bytes on the trace		*/
	int ispipe=0;	/* ftypein == PIPE ?			*/
	int isdisk=0;	/* ftypein == DISK ?			*/
	char *tmpdir;		/* directory path for tmp files		*/
	int istmpdir=0;/* true for user given path		*/

	long long loffset;
	off_t lofset;
	int i;


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Look for user-supplied tmpdir form environment */
	if (!(tmpdir = getenv("CWP_TMPDIR"))) tmpdir="";
	if (!STREQ(tmpdir, "") && access(tmpdir, WRITE_OK))
		err("you can't write in %s (or it doesn't exist)", tmpdir);


	/* The algorithm requires stdin to be rewound and */
	/* random access to either stdin or stdout.    */
	ftypein = filestat(STDIN);
	ftypeout = filestat(STDOUT);
	if( ftypein == DISK ) isdisk = 1;
	if( ftypein == PIPE || ftypein == FIFO ) ispipe = 1;

	if( ispipe == 0 && isdisk ==0 ) isdisk = 1;

	if (ispipe && ftypeout != DISK)
		err("OK IO types are: DISK->ANYTHING, PIPE->DISK");

	/* If pipe, prepare temporary file to hold data */
	if (ispipe==1) {
		if (STREQ(tmpdir,"")) {
			/*tracefp = etempfile(NULL); */
			tracefp = etmpfile();
		} else {	/* user-supplied tmpdir from environment */
			/*tracefp = etempfile(tmpdir); */
			tracefp = etmpfile();
		}			
	}
	/* Set number of sort keys */
	if (argc == 1) nkey = 1; /* no explicit keys: default cdp key */
	else  nkey = argc - 1;   /* one or more explicit keys */


	/* Allocate lists for key indices, sort directions and key types */
	index = ealloc1int(nkey);
  	up = (bool *) ealloc1(nkey, sizeof(bool));
	type = (char *) ealloc1(nkey, sizeof(char));
	
	
	/* Initialize index, type and up */
	if (argc == 1) {
		index[0] = getindex("cdp");
		up[0] = true;
		type[0] = 'l';
	} else {
		for (i = 0; i < nkey; ++i) {
			switch (**++argv) { /* sign char of next arg */
			case '+':
				up[i] = true;
				++*argv;   /* discard sign char in arg */
			break;
			case '-':
				up[i] = false;
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

	/* Allocate list of trace numbers + key values */
	ngroup = nkey + 1;
	nvsize = ngroup*sizeof(Value);
	nv = NTRSTEP;  /* guess at required number */
	val_list = (Value *) ealloc1(nv, nvsize);
	vptr = val_list;

	/* allow large file access */
	file2g(stdin);
	file2g(stdout);
	if(ispipe==1) file2g(tracefp);

	/* Run through traces once to collect header values */
	ntr = 0;
	if (!(nsegy = gettr(&tr)))  err("can't get first trace");

	do {
		itr = ntr++;
		/* realloc if out of room */
		if (0 == (ntr % NTRSTEP)) {
			nv += NTRSTEP;
			val_list = (Value *) erealloc(val_list, nv*nvsize);
			vptr = val_list + itr * ngroup;
		}

		/* enter trace index in list and then key values */
		vptr++->l = itr;	/* insert number and advance */

	 	for (i = 0; i < nkey; ++i) {
			gethval(&tr, index[i], &val);
			*vptr++ = up[i] ? val : negval(type + i, val);
				/* negative values give reverse sort */
	 	}

		if (ispipe==1) efwrite((char *)&tr, 1, nsegy, tracefp);
	} while (gettr(&tr));

	if (ispipe==1)  rewind(tracefp);
	else /* disk */	  rewind(stdin);

	/* Sort the headers values */
	qsort(val_list, ntr, nvsize, cmp_list);

	if (isdisk==1) {
		/* run through sorted list and write output sequentially */

		for (i = 0; i < ntr; ++i) {

			itr = val_list[i*ngroup].l;
			gtra64(&tr, itr);
			/*
			tr.tracl = tr.tracr = i + 1;
			*/
			puttr(&tr);
		}
	} else /* pipe */ {
		/* invert permutation and read input sequentially */
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup].l;
			val_list[itr*ngroup + 1].l = i;
	        }
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup + 1].l;
			efread(&tr, 1, nsegy, tracefp);
			/*
			tr.tracl = tr.tracr = itr + 1;
			*/
			loffset = itr;
			loffset = loffset * nsegy;
			bcopy(&loffset,&lofset,8); 
			fseek64(stdout, lofset, SEEK_SET);
			fwrite(&tr, 1, nsegy, stdout);
		}
	}

	/* Clean up */
	if (ispipe==1) {
		efclose(tracefp);
	}
	
	return EXIT_SUCCESS;
}


/* Comparison routine for qsort */
int cmp_list(register Value *a, register Value *b)
{
	register int i;
	Value va, vb;
	int compare;

	/* Can order as soon as any components are unequal */
	for (i = 0; i < nkey; ++i) {
		va = *++a; vb = *++b; /* advance and dereference */
		if ((compare = valcmp(type + i, va, vb)))
			return compare;
        }
        return 0;
}


/* Reverse sign of value */
Value negval(String type, Value val)
{
	switch (*type) {
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
	case 'p':
		val.p = (int) -val.p;
	break;
	case 'f':
		val.f = -val.f;
	break;
	case 'd':
		val.d = -val.d;
	break;
	default: err("%d: mysterious type %s", __LINE__, type);
	}

	return val;
}


/* for graceful interrupt termination */
static void closefiles(void)
{
	efclose(tracefp);
	eremove(tracefile);
	exit(EXIT_FAILURE);
}
