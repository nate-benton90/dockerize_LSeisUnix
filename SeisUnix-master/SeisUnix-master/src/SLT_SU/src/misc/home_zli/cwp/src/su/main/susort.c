/* SUSORT: $Revision: 1.13 $ ; $Date: 91/01/18 15:28:15 $	*/

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
#include <sys/ioctl.h>
#ifdef _IBMR2
#include <sys/tape.h>
#else
#include <sys/mtio.h>
#endif

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SUSORT - sort on any segy header keywords			\n"
" 								\n"
" susort <stdin >stdout [[+-]key1 [+-]key2 ...]			\n"
" 								\n"
" Susort supports any number of (secondary) keys with either	\n"
" ascending (+, the default) or descending (-) directions for 	\n"
" each.  The default sort key is cdp.				\n"
" 								\n"
" Note:	Only the following types of input/output are supported	\n"
"	Disk input --> any output				\n"
"	Tape input --> Disk output				\n"
"	Pipe input --> Disk output				\n"
" 								\n"
" Caveat: In the case of Pipe input a temporary file is made	\n"
"	to hold the ENTIRE data set.  This temporary is		\n"
"	either an actual disk file (usually in /usr/tmp) or in  \n"
"	SU_SCRATCHDIR set as an enviroment variable. 		\n"
" 								\n"
" Example:							\n"
" To sort traces by cdp gather and within each gather		\n"
" by offset with both sorts in ascending order:			\n"
" 								\n"
" 	susort <INDATA >OUTDATA cdp offset			\n"
" 								\n"
;
/**************** end self doc ***********************************/

/* Credits:
 *	SEP: Einar, Stew
 *	CWP: Shuki, Jack
 *	      : Zhiming 
 *
 * Caveats:
 *	Since the algorithm depends on sign reversal of the key value
 *	to obtain a descending sort, the most significant figure may
 *	be lost for unsigned data types.
 *
 */


#define NTRSTEP	1024	/* realloc() increment measured in traces */

segychdr ch;
segybhdr bh;
segytrace tr;
static int nkey;	/* number of keys to sort on	*/
static String type;	/* header key types		*/

/* Prototypes */
Value negval(String type, Value val);   /* reverse sign of value	*/
void rew(FILE *fp);			/* rewind tape file		*/
int cmp_list();				/* qsort comparison function	*/

main(int argc, char **argv)
{
	static Value *val_list;	/* a list of the key values for each
				/* trace with each group headed by the
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
	bool ispipe;		/* ftypein == PIPE ?			*/
	bool istape;		/* ftypein == TAPE ?			*/
	bool isdisk;		/* ftypein == DISK ?			*/
	FILE *datafp;		/* fp for data storage file		*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* The algorithm requires stdin to be rewound and
	/* random access to either stdin or stdout.    */
	ftypein = filestat(STDIN);
	ftypeout = filestat(STDOUT);
	isdisk = (ftypein == DISK) ? true : false;
	istape = (ftypein == TAPE) ? true : false;
	ispipe = (ftypein == PIPE || ftypein == FIFO) ? true : false;
	if ( (istape || ispipe) && ftypeout != DISK)
		err("OK IO types are: DISK->ANYTHING, TAPE->DISK, PIPE->DISK");


	/* If pipe, prepare temporary file to hold data */
	/* if (ispipe)  datafp = etmpfile(); */
	if (ispipe)  datafp = etempfile(NULL);


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


	/* get tape headers */
	gethdr(&ch,&bh);
	if ( strncmp(getkey(index[0]),"cdp",3) == 0 ) {
		bh.tsort = 2;
	} else if ( strncmp(getkey(index[0]),"offset",6) == 0 ) {
		bh.tsort = 3;
	}  
	/* put tape headers */
	puthdr(&ch,&bh);

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
	else if (isdisk)  rewind(stdin);
	else /* tape */	  rew(stdin);

	/* Sort the headers values */
	qsort(val_list, ntr, nvsize, cmp_list);

	if (isdisk || ispipe) {
		/* run through sorted list and write output sequentially */
		register int i;
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup].l;
			if(isdisk) {
				gettra(&tr, itr);
			} else if(ispipe) {
				fgettra(datafp, &tr, itr);
			}
			tr.tracl = tr.tracr = i + 1;
			puttr(&tr);
		}
	} else if (istape || ispipe) {
		/* invert permutation and read input sequentially */
		register int i;
		for (i = 0; i < ntr; ++i) {
			itr = val_list[i*ngroup].l;
			val_list[itr*ngroup + 1].l = i;
	        }
		if (istape) {
			for (i = 0; i < ntr; ++i) {
				itr = val_list[i*ngroup + 1].l;
				if (!gettr(&tr))  err("can't reread input");
				tr.tracl = tr.tracr = itr + 1;
				efseek(stdout, itr*nsegy+3600, SEEK_SET);
				efwrite(&tr, 1, nsegy, stdout);
			}
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

#ifndef _IBMR2
#define MT_BOT	 0xffffffff /* Guess--MT_BOT not documented in mtio.h	*/
#define WASATBOT (mtstat.mt_dsreg & MT_BOT)
#define NOTPOS	 (mtstat.mt_blkno != ((daddr_t) 0)) /* not on Gould	*/

void rew(FILE *fp)
{
	static struct mtop bsf /* backward space file	  */ = {MTBSF, 1};
	static struct mtop bsr /* backward space record	  */ = {MTBSR, 1};
	static struct mtop fsr /* forward space record	  */ = {MTFSR, 1};
	static struct mtop nop /* no op, sets status only */ = {MTNOP, 1};
	static struct mtop set /* rewind		  */ = {MTREW, 1};
	static struct mtget mtstat;
	int fd = fileno(fp);


	/* fast path--Note: I don't understand this (jkc) */
	if (fd == STDIN && WASATBOT) {
		if (-1 == ioctl(fd, MTIOCTOP, (char *) &set)) {
			err("%s: ioctl rewind failed", __FILE__);
		}
	} else {
		/* back up over tape mark */
		if (-1 == ioctl(fd, MTIOCTOP, (char *) &bsr)) {
			err("%s: ioctl bsr failed", __FILE__);
		}

		/* back up to BOT or before prev TM */
		if (-1 == ioctl(fd, MTIOCTOP, (char *) &bsf)) {
			err("%s: ioctl bsf failed", __FILE__);
		}

		/* sense */
		if (-1 == ioctl(fd, MTIOCTOP, (char *) &nop)) {
			err("%s: ioctl nop failed", __FILE__);
		}

		/* retrieve sense and status */
		if (-1 == ioctl(fd, MTIOCGET, (char *) &mtstat)) {
			err("%s: ioctl mtstat failed", __FILE__);
		}

		/* space forward if needed */
		if (!WASATBOT || NOTPOS) {
		    if (-1 == ioctl(fd, MTIOCTOP, (char *) &fsr)) {
			err("%s: ioctl fsr failed", __FILE__);
		    }
		}
	}
}

#else /* it's an IBMR2 */

void rew(FILE *fp)
{
	static struct stop set /* rewind */ = {STREW, 1};
	int fd = fileno(fp);


	/* doing my best -- see above non-IBM routine (jkc) */
	if (-1 == ioctl(fd, STIOCTOP, (char *) &set))
		err("%s: ioctl rewind failed", __FILE__);
}
#endif
