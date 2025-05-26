/* SUSORT: $Revision: 1.15 $ ; $Date: 92/10/26 11:02:07 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado.edu)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

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
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar, Stew
 *	CWP: Shuki, Jack
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


#define NTRSTEP	1024	/* realloc() increment measured in traces */

segy tr;
static int nkey;	/* number of keys to sort on	*/
static String type;	/* header key types		*/

/* Prototypes */
Value negval(String type, Value val);   /* reverse sign of value	*/
int cmp_list();				/* qsort comparison function	*/

main(int argc, char **argv)
{
	static Value *val_list;	/* a list of the key values for each
				/* trace with each group headed by the
				/* trace number of that trace		*/
	static int *index;	/* header key indices			*/
  	static Bool *up;	/* sort direction (+ = up = ascending)	*/
	register Value *vptr;	/* location pointer for val_list	*/
	int ngroup;		/* size of unit in val_list (nkey + 1)	*/
	int nv;			/* number of groups in val_list		*/
	int nvsize;		/* size of group in val_list		*/
  	Value val;		/* value of a keyword			*/
	long ntr;		/* number of traces from gettr		*/
	long itr;		/* index of trace (0,1,2,...)		*/
	FileType ftypein;	/* filetype of stdin			*/
	FileType ftypeout;	/* filetype of stdout			*/
	int nsegy;		/* number of bytes on the trace		*/
	Bool ispipe;		/* ftypein == PIPE ?			*/
	Bool isdisk;		/* ftypein == DISK ?			*/
	FILE *datafp;		/* fp for data storage file		*/


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* The algorithm requires stdin to be rewound and
	/* random access to either stdin or stdout.    */
	ftypein = filestat(STDIN);
	ftypeout = filestat(STDOUT);
	isdisk = (ftypein == DISK) ? true : false;
	ispipe = (ftypein == PIPE || ftypein == FIFO) ? true : false;
	if (ispipe && ftypeout != DISK)
		err("OK IO types are: DISK->ANYTHING, PIPE->DISK");


	/* If pipe, prepare temporary file to hold data */
	if (ispipe)  datafp = etmpfile();


	/* Set number of sort keys */
	if (argc == 1) nkey = 1; /* no explicit keys: default cdp key */
	else  nkey = argc - 1;   /* one or more explicit keys */


	/* Allocate lists for key indices, sort directions and key types */
	index = ealloc1int(nkey);
  	up = (Bool *) ealloc1(nkey, sizeof(Bool));
	type = (char *) ealloc1(nkey, sizeof(char));
	
	
	/* Initialize index, type and up */
	if (argc == 1) {
		index[0] = getindex("cdp");
		up[0] = true;
		type[0] = 'l';
	} else {
		register int i;
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
		{ register int i;
		  for (i = 0; i < nkey; ++i) {
			gethval(&tr, index[i], &val);
			*vptr++ = up[i] ? val : negval(type + i, val);
				/* negative values give reverse sort */
		  }
		}
		if (ispipe) efwrite((char *)&tr, 1, nsegy, datafp);
	} while (gettr(&tr));
	
	if      (ispipe)  rewind(datafp);
	else /* disk */	  rewind(stdin);

	/* Sort the headers values */
	qsort(val_list, ntr, nvsize, cmp_list);

	if (isdisk) {
		/* run through sorted list and write output sequentially */
		register int i;
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup].l;
			gettra(&tr, itr);
			tr.tracl = tr.tracr = i + 1;
			puttr(&tr);
		}
	} else /* pipe */ {
		/* invert permutation and read input sequentially */
		register int i;
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup].l;
			val_list[itr*ngroup + 1].l = i;
	        }
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup + 1].l;
			efread(&tr, 1, nsegy, datafp);
			tr.tracl = tr.tracr = itr + 1;
			efseek(stdout, itr*nsegy, SEEK_SET);
			efwrite(&tr, 1, nsegy, stdout);
		}
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
		if (compare = valcmp(type + i, va, vb))
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
