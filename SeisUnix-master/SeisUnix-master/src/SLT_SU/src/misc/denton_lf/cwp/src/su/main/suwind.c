/* SUWIND: $Revision: 1.5 $ ; $Date: 91/05/10 15:42:52 $		*/

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
SUWIND - window and time gate traces					\n\
									\n\
suwind <stdin >stdout [options]						\n\
									\n\
Required Parameters:							\n\
	none								\n\
									\n\
Optional Parameters:							\n\
	verbose = 0	=1 for verbose					\n\
...Options for horizontal windowing (trace selection):			\n\
	key = tracl	Key header word to window on (see segy.h)	\n\
	min = LONG_MIN	min value of key header word to pass		\n\
	max = LONG_MAX	max value of key header word to pass		\n\
	abs = 0		=1 to take absolute value of 			\n\
			key header word					\n\
	j = 1		Pass every j gathers spcified by key  ...	\n\
	s = 0		... based at s  				\n\
			will output when mod(key - s,j)=0 and 		\n\
			min<= key <=max        				\n\
	count = ULONG_MAX	... up to count traces			\n\
	bad = none	Delete traces with specified bad key		\n\
			values						\n\
	start = 1       starting trace to search (disk input only) 	\n\
	skip = 0        number of traces to skip per trace read 	\n\
                        (disk input only) 				\n\
...Options for vertical windowing (time gating):			\n\
	tmin = 0.0	min time to pass (in s)				\n\
	tmax = (from header)	max time to pass (in s)			\n\
	itmin = 0	min time sample to pass				\n\
	itmax =	(from header)  max time sample to pass			\n\
	nt = itmax-itmin+1	number of time samples to pass		\n\
       				(nt=itmax-itmin+1)			\n\
									\n\
Note: selecting times beyond the maximum in the data induces		\n\
      zero padding (up to SU_NFLTS).					\n\
									\n\
	On most 32 bit machines, LONG_MIN, LONG_MAX and ULONG_MAX are	\n\
	about -2E9,+2E9 and 4E9 and are defined in general in limits.h	\n\
...Options for i/o data name						\n\
        datain=		input data name (instead of stdin)		\n\
        dataout=	output data name (instead of stdout)		\n\
";
/**************** end self doc *******************************************/

/* Credits:
 *	SEP: Einar
 *	CWP: Shuki, Jack, Chris
 *
 * Note:
 *	On large data sets, the count parameter should be set
 *	if possible.  Otherwise, every trace in the data set
 *	will be examined.
 *
 * Caveat:
 *	A cast to int is made for the s,j selection branch.
 */


segytrace tr;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
	String key;	/* header key word from segy.h		*/
	Value val;	/* value of key				*/
	int ival;	/* ... cast to int			*/
	String type;	/* type of key				*/
	int index;	/* index of key				*/
	int min;	/* smallest key value to accept		*/
	int max;	/* largest key value to accept		*/
	int j;		/* take every jth trace ...		*/
	int s;		/* ... starting at the sth trace ...	*/
	uint count;	/* ... up to a total of count traces	*/
	Value *badptr;	/* pointer to list of bad traces	*/
	mixed mbadptr;	/* to arbitrate bad with getpars	*/
	uint nbad;	/* number of bad traces			*/
	bool isbad;	/* flag for bad trace			*/
	short ab;	/* absolute value flag (1=YES, 0=NO)	*/
	float tmin;	/* minimum time to pass			*/
	float tmax;	/* maximum time to pass			*/
	float dt;	/* sampling rate (secs)			*/
	int itmin;	/* smallest time sample (zero-based)	*/
	int itmax;	/* largest time sample (zero-based)	*/
	ushort nt;	/* number of time samples		*/
	int nzeros;	/* number of zeroes to pad		*/
	char *pzeros;	/* pointer to zero pad			*/
	short verbose;	/* if 1(yes) echo parameters to stderr	*/
	register int i;	/* counter				*/
	void mallocmix();	/* PARMS(mixed *, char *, uint)		*/
	void getparval_old();	/* PARMS(char *, char *, value *)	*/
	void mixtoval();	/* PARMS(value, mixed, char *, uint)	*/
	int start, skip;
	String datain, dataout;	
	FILE *infp, *outfp;
	float tmp;
	int ntin;
	int itwin=0, itmins=0, itmaxs=0;

	long long lofset;


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Default parameters;	User-defined overrides */
	if (!getparstring("key"     , &key))		key = "tracl";
	if (!getparint("min"     , &min))		min = INT_MIN;
	if (!getparint("max"     , &max))		max = INT_MAX;
	if (!getparint("j"       , &j))		j = 1;
	if (!getparint("s"       , &s))		s = 0;
	if (!getparuint("count"   , &count))	count = UINT_MAX;
	if (!getparshort("abs"     , &ab))		ab = 0;
	if (!getparshort("verbose" , &verbose))	verbose = 0;
	if (!getparint("start" , &start))	start = 1;
	if (!getparint("skip" , &skip))	skip = 0;
	if (!getparstring("datain" , &datain)) {
		infp = stdin;
		file2g(infp);
	} else {
		infp = efopen(datain,"r");
		file2g(infp);
	}


	/* get id headers for possible updates */
	fgethdr(infp,&ch,&bh);

	if (!getparstring("dataout", &dataout)) {
		outfp = stdout;
		file2g(outfp);
	} else {
		outfp = efopen(dataout,"wl");
	}

	type = hdtype(key);
	index = getindex(key);


	/* Getpar the bad vector--need a pointer of the specific type  */
	if (nbad = countparval("bad")) {
		mallocmix(&mbadptr, type, nbad);
		getparval_old("bad", type, mbadptr);

		/* Get pointer to value from mixed pointer */
		if (NULL == (badptr = (Value *)
			malloc((uint) (nbad*sizeof(Value)))))
		err("malloc failed");
		mixtoval(badptr, mbadptr, type, nbad);
	}


	/* Evaluate time bounds from getpars and first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	dt = 0.000001*(float)tr.dt;
	ntin = tr.ns;

	if (!getparint("itmin", &itmin))	itmin = 0;
	if (getparfloat("tmin", &tmin)) {
		itmins = 1;
		tmp = (tmin-tr.delrt*0.001)/dt;
		if(tmp<0) tmp = tmp - 0.5;
		if(tmp>0) tmp = tmp + 0.5;
		itmin = tmp;
		itwin = 1;
	} else {
		tmin = itmin*dt+tr.delrt*0.001;
	}

	if (!getparint("itmax", &itmax))	itmax = tr.ns - 1;
	if (getparfloat("tmax", &tmax)) {
		itmaxs = 1;
		tmp = (tmax-tr.delrt*0.001)/dt+0.5;
		itmax = tmp;
		itwin = 1;
	} else {
		tmax = itmax*dt+tr.delrt*.001;
	}

	if (getparushort("nt", &nt)) {
		itmax = itmin + nt - 1;
		tmax = tr.delrt*0.001+itmax*dt;
		itwin = 1;
	} else {
		nt = itmax - itmin + 1;
	}

	/*
	if (itmin < 0)
		err("itmin=%d should be positive", itmin);
	*/
	if (nt > SU_NFLTS)
		err("nt=%d exceeds SU_NFLTS=%d", nt, SU_NFLTS);
	if (itmin > itmax)
		err("itmin=%d, itmax=%d conflict", itmin, itmax);

	if(itmin>=0) { 
		nzeros = (nt - tr.ns) * FSIZE;
		pzeros = (char *) (tr.data + tr.ns - itmin);
	} else {
		nzeros = 0;
	}

	/* Echo parameters */
	if (verbose) {
		warn("key = %s", key);
		warn("type = %s", type);
		warn("min = %ld", min);
		warn("max = %ld", max);
		warn("j = %d", j);
		warn("s = %d", s);
		warn("count = %lu", count);
		warn("abs = %d", ab);
		for (i = 0; i < nbad; i++) {
			(void) fprintf(stderr, "badptr[%d] = ", i);
			fprintfval(stderr, type, badptr[i]);
			putc('\n', stderr);
		}
		warn("tmin=%f tmax=%f", tmin, tmax);
		warn("itmin=%d itmax=%d nt=%u",
						itmin, itmax, nt);
		if (nzeros) warn("Padding %d zeroes", nzeros/FSIZE);
	}

	/* update hns and output */
	bh.hns = nt;
	fputhdr(outfp,&ch,&bh);


	/* search for first trace position */
	if(start > 1) {
		lofset = (start-1);
        lofset = lofset*(tr.ns*sizeof(float)+240)+3600;
        fseek64(infp,lofset,0);
		fgettr(infp,&tr);
	}

	/* Main loop over traces */
	do {
		if (itwin==1) {
			if(itmins==1) {
				tmp = (tmin-tr.delrt*0.001)/dt;
				if(tmp<0) tmp = tmp - 0.5;
				if(tmp>0) tmp = tmp + 0.5;
				itmin = tmp;
			}
			if(itmaxs==1) {
				tmp = (tmax-tr.delrt*0.001)/dt+0.5;
				itmax = tmp;
			}
			/* Time window */
			if (itmin > 0) {
				for (i = itmin; i <= itmax; i++) {
					tr.data[i - itmin] = tr.data[i];
				}
			} else if(itmin<0) {
				for(i=tr.ns;i<=itmax;i++) tr.data[i] = 0.;
				for (i = itmax; i >= 0; i--) {
					tr.data[i - itmin] = tr.data[i];
				}
				for( i=itmin;i<0;i++) tr.data[i-itmin] = 0.;
			} 
			if (nzeros > 0) bzero(pzeros, nzeros);
		}

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

		/* If trace selected, put it out */
		if ((min <= ival) && (ival <= max) &&
		   !((ival - s) % j) && !isbad && count--) {
			tr.ns = nt;
			if(itwin==1) {
				tmp = tmin*1000 + 0.5;
				tr.delrt = tmp;
			}
			fputtr(outfp,&tr);
			if (!count) break; /* all done */
		}

		if(skip > 0) {
			lofset = skip;
                       	lofset = lofset*(ntin*sizeof(float)+240);
                       	fseek64(infp,lofset,1);
		}

	} while (fgettr(infp,&tr));


	return EXIT_SUCCESS;
}


/* Mixed malloc */
void mallocmix(mptr, type, n)
mixed *mptr;
String type;
uint n;
{
	switch(*type) {
	case 's':
		if (NULL == (mptr->s = (char *) malloc((uint) (n * DSIZE))))
			err("mallocmix failed on type %s", type);
	break;
	case 'h':
		if (NULL == (mptr->h = (short *) malloc(n * sizeof(short))))
			err("mallocmix failed on type %s", type);
	break;
	case 'u':
		if (NULL == (mptr->u = (ushort *) malloc(n * sizeof(ushort))))
			err("mallocmix failed on type %s", type);
	break;
	case 'l':
		if (NULL == (mptr->l = (long *) malloc(n * sizeof(long))))
			err("mallocmix failed on type %s", type);
	break;
	case 'v':
		if (NULL == (mptr->v = (ulong *) malloc(n * sizeof(ulong))))
			err("mallocmix failed on type %s", type);
	break;
	case 'i':
		if (NULL == (mptr->i = (int *) malloc(n * sizeof(int))))
			err("mallocmix failed on type %s", type);
	break;
	case 'p':
		if (NULL == (mptr->p = (uint *) malloc(n * sizeof(uint))))
			err("mallocmix failed on type %s", type);
	break;
	case 'f':
		if (NULL == (mptr->f = (float *) malloc(n * sizeof(float))))
			err("mallocmix failed on type %s", type);
	break;
	case 'd':
		if (NULL == (mptr->d = (double *) malloc(n * sizeof(double))))
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
	case 's': getparstring(name, (char**) valp);		break;
	case 'h': getparshort(name, (short*) valp);		break;
	case 'u': getparushort(name, (unsigned short*) valp);	break;
	case 'l': getparlong(name, (long*) valp);		break;
	case 'v': getparulong(name, (unsigned long*) valp);	break;
	case 'i': getparint(name, (int*) valp);			break;	
	case 'p': getparuint(name, (unsigned int*) valp);	break;
	case 'f': getparfloat(name, (float*) valp);		break;	
	case 'd': getpardouble(name, (double*) valp);		break;	
	default:
		err("getparval_old: %d: mysterious type %s", __LINE__, type);
	}
}


/* Get value ptr from mixed ptr */
void mixtoval(valp, mixptr, type, n)
Value *valp;
mixed mixptr;
String type;
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
	return;
}

