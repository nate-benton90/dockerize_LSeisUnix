static char RCSsumark[] = "$Id: sumark.c,v 1.1 1992/12/16 17:00:19 suadm Exp stssmap $";
 
/*
 *
 * $Source: /vol/SU/cwp/src/su/main/RCS/sumark.c,v $
 *
 * $Log: sumark.c,v $
 * Revision 1.1  1992/12/16  17:00:19  suadm
 * Initial revision
 *
 */
 
/* SUMARK: $Revision: 1.1 $ ; $Date: 1992/12/16 17:00:19 $		*/

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

/*********************** self documentation ******************************/
string sdoc = "\
									\n\
SUMARK - mark selected traces						\n\
									\n\
sumark <stdin >stdout [options]						\n\
									\n\
Required Parameters:							\n\
	none								\n\
									\n\
Optional Parameters:							\n\
	mark = 1	=integer to have different mark value		\n\
	verbose = 0	=1 for verbose					\n\
	key = cdp	Key header word to window on (see segy.h)	\n\
	min = LONG_MIN	min value of key header word to mark		\n\
	max = LONG_MAX	max value of key header word to mark		\n\
	abs = 0		=1 to take absolute value of key header		\n\
			word						\n\
	j = 1		mark every j-th trace ...			\n\
	s = 0		... based at s  (if ((key - s)%j) == 0)		\n\
	count = ULONG_MAX	... up to count traces			\n\
	bad = none	don't mark traces with specified bad 		\n\
			key values					\n\
									\n\
On most 32 bit machines, LONG_MIN, LONG_MAX and ULONG_MAX are		\n\
about -2E9,+2E9 and 4E9 and are defined in general in limits.h		\n\
";
/**************** end self doc *******************************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Shuki, Jack, Chris
 *
 * Caveat:
 *	A cast to int is made for the s,j selection branch.
 */



segy tr;

main(int argc, char **argv)
{
	int mark;	/* value used for mark			*/
	String key;	/* header key word from segy.h		*/
	Value val;	/* value of key				*/
	int ival;	/* ... cast to int			*/
	String type;	/* type of key				*/
	int index;	/* index of key				*/
	long min;	/* smallest key value to accept		*/
	long max;	/* largest key value to accept		*/
	int j;		/* take every jth trace ...		*/
	int s;		/* ... starting at the sth trace ...	*/
	ulong count;	/* ... up to a total of count traces	*/
	Value *badptr;	/* pointer to list of bad traces	*/
	mixed mbadptr;	/* to arbitrate bad with getpars	*/
	uint nbad;	/* number of bad traces			*/
	bool isbad;	/* flag for bad trace			*/
	short ab;	/* absolute value flag (1=YES, 0=NO)	*/
	short verbose;	/* if 1(yes) echo parameters to stderr	*/
	register int i;	/* counter				*/
	void mallocmix();	/* PARMS(mixed *, char *, uint)		*/
	void getparval_old();	/* PARMS(char *, char *, value *)	*/
	void mixtoval();	/* PARMS(value, mixed, char *, uint)	*/


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Default parameters;	User-defined overrides */
	key = "cdp";		sgetpar("key"     , &key);
	min = LONG_MIN;		lgetpar("min"     , &min);
	max = LONG_MAX;		lgetpar("max"     , &max);
	mark = 1;		igetpar("mark"    , &mark);
	j = 1;			igetpar("j"       , &j);
	s = 0;			igetpar("s"       , &s);
	count = ULONG_MAX;	vgetpar("count"   , &count);
	ab = 0;			hgetpar("abs"     , &ab);
	verbose = 0;		hgetpar("verbose" , &verbose);

	type = hdtype(key);
	index = getindex(key);


	/* Getpar the bad vector--need a pointer of the specific type  */
	if (nbad = countparval("bad")) {
		mallocmix(&mbadptr, type, nbad);
		getparval_old("bad", type, mbadptr);

		/* Get pointer to value from mixed pointer */
		if (NULL == (badptr = (Value *)
			malloc((uint) (nbad*sizeof(Value)))))
		syserr("malloc failed");
		mixtoval(badptr, mbadptr, type, nbad);
	}


	/* Echo parameters */
	if (verbose) {
		fprintf(stderr, "mark = %d\n", mark);
		fprintf(stderr, "key = %s\n", key);
		fprintf(stderr, "type = %s\n", type);
		fprintf(stderr, "min = %ld\n", min);
		fprintf(stderr, "max = %ld\n", max);
		fprintf(stderr, "j = %d\n", j);
		fprintf(stderr, "s = %d\n", s);
		fprintf(stderr, "count = %lu\n", count);
		fprintf(stderr, "abs = %d\n", ab);
		for (i = 0; i < nbad; i++) {
			fprintf(stderr, "badptr[%d] = ", i);
			fprintfval(stderr, type, badptr[i]);
			putc('\n', stderr);
		}
	}


	/* Main loop over traces */
	while (gettr(&tr)) {
		/* Trace window */
		gethval(&tr, index, &val);
		isbad = false;
		for (i = 0; i < nbad; i++) {
			if (!valcmp(type, val, badptr[i])) {
				isbad = true;
				break;	/* found */
			}
		}

		if (ab) val = valtoabs(type, val);

		ival = vtoi(type, val);

		/* Mark selected traces */
		if ((min <= ival) && (ival <= max) &&
		   !((ival - s) % j) &&
		   !isbad && count > 0) {
			tr.mark = mark;
			count--;
		}

		puttr(&tr);
	}
	

	return EXIT_SUCCESS;
}


/* Mixed malloc */
void mallocmix(mptr, type, n)
mixed *mptr;
string type;
uint n;
{
	switch(*type) {
	case 's':
		if (NULL == (mptr->s = (char *) malloc((uint) (n * DSIZE))))
			err("mallocmix failed on type %s", type);
	break;
	case 'h':
		if (NULL == (mptr->h = (short *)
			malloc((uint) (n * sizeof(short)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'u':
		if (NULL == (mptr->u = (ushort *)
			malloc((uint) (n * sizeof(ushort)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'l':
		if (NULL == (mptr->l = (long *)
			malloc((uint) (n * sizeof(long)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'v':
		if (NULL == (mptr->v = (ulong *)
			malloc((uint) (n * sizeof(ulong)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'i':
		if (NULL == (mptr->i = (int *)
			malloc((uint) (n * sizeof(int)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'p':
		if (NULL == (mptr->p = (uint *)
			malloc((uint) (n * sizeof(uint)))))
			err("mallocmix failed on type %s", type);
	break;
	case 'f':
		if (NULL == (mptr->f = (float *) malloc((uint) (n * FSIZE))))
			err("mallocmix failed on type %s", type);
	break;
	case 'd':
		if (NULL == (mptr->d = (double *) malloc((uint) (n * DSIZE))))
			err("mallocmix failed on type %s", type);
	default:
		err("mallocmix: %d: mysterious type %s", __LINE__, type);
	}
	return;
}


/* Value getpar */
void getparval_old(name, type, valp)
String name;
String type;
Value *valp;
{
	switch(*type) {
	case 's': sgetpar(name, (char**)valp);
	case 'h': hgetpar(name, (short*)valp);
	case 'u': ugetpar(name, (ushort*)valp);
	case 'l': lgetpar(name, (long*)valp);
	case 'v': vgetpar(name, (unsigned long*)valp);
	case 'i': igetpar(name, (int*)valp);
	case 'p': pgetpar(name, (unsigned int*)valp);
	case 'f': fgetpar(name, (float*)valp);
	case 'd': dgetpar(name, (double*)valp);
	default:
		err("getparval_old: %d: mysterious type %s", __LINE__, type);
	}
}


/* Get value ptr from mixed ptr */
void mixtoval(valp, mixptr, type, n)
value *valp;
mixed mixptr;
string type;
uint n;
{
	register int i;

	switch(*type) {
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
			(valp++)->i = *mixptr.i++;
		}
	break;
	case 'p':
		for (i = 0; i < n; i++) {
			(valp++)->p = *mixptr.p++;
		}
	break;
	case 'f':
		for (i = 0; i < n; i++) {
			(valp++)->f = *mixptr.f++;
		}
	break;
	case 'd':
		for (i = 0; i < n; i++) {
			(valp++)->d = *mixptr.d++;
		}
	break;
	default:
		err("mixtoval: %d: mysterious type %s", __LINE__, type);
	break;
	}
}

