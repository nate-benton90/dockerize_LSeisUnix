/* GETPARS: $Revision: 1.30 $ ; $Date: 89/05/25 16:10:28 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1989.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (isis!csm9a!jkcohen)
 *----------------------------------------------------------------------
 */

#include "cwp.h"

/* hgetpar, ugetpar, lgetpar, vgetpar, igetpar, pgetpar, fgetpar,
 * zgetpar, sgetpar - get a parameter or vector of parameters of
 * a given data type
 *
 * maxgetpar - return the size of the longest parameter vector
 *
 * Returns:
 *	int: number of values read, zero if the parameter was not
 *	     specified.
 *
 * Synopsis:
 *	int hgetpar(name, ptr)	(Get a short parameter)
 *	string name;		(External name of parameter)
 *	short *ptr;		(Pointer to parameter value)
 *
 *	int ugetpar(name, ptr)	(Get an unsigned short parameter)
 *	string name;		(External name of parameter)
 *	unsigned short *ptr;	(Pointer to parameter value)
 *
 *	int lgetpar(name, ptr)	(Get a long parameter)
 *	string name;		(External name of parameter)
 *	long *ptr;		(Pointer to parameter value)
 *
 *	int vgetpar(name, ptr)	(Get an unsigned long parameter)
 *	string name;		(External name of parameter)
 *	unsigned long *ptr;	(Pointer to parameter value)
 *
 *	int igetpar(name, ptr)	(Get an int parameter)
 *	string name;		(External name of parameter)
 *	int *ptr;		(Pointer to parameter value)
 *
 *	int pgetpar(name, ptr)	(Get an unsigned int parameter)
 *	string name;		(External name of parameter)
 *	unsigned int *ptr;	(Pointer to parameter value)
 *
 *	int fgetpar(name, ptr)	(Get a float parameter)
 *	string name;		(External name of parameter)
 *	float *ptr;		(Pointer to parameter value)
 *
 *	int zgetpar(name, ptr)	(Get a double parameter)
 *	string name;		(External name of parameter)
 *	double *ptr;		(Pointer to parameter value)
 *
 *	int sgetpar(name, ptr)	(Get a string parameter)
 *	string name;		(External name of parameter)
 *	char **ptr;		(Pointer to parameter value)
 *
 *	unsigned maxgetpar()
 *
 * Examples:
 *      ...Here 0.0 is the default value, if getpar doesn't find x.
 *	x = 0.0; fgetpar("x", &x);
 *
 *	nv = igetpar("vi", &vi);
 *	for (i = 0, i < nv; i++) {
 *		...
 *
 * Notes:
 *	The functions: atohe, atoue, atole, atove, atoie, atope used
 *	below are versions of atoi that check for overflow.  The source
 *	file for these functions is atopkge.c.
 *
 *	Usage is illustrated in the DTEST code at the end of this file.
 *
 * Caveat:
 *	Fortran support is tenuous.
 *
 * Credits:
 *	SEP: Jon Claerbout, Rob Clayton
 *	CWP: Shuki, Jack
 *
 *
 *
 */

/* Embed Revision Control System identifier strings */
static string progid =
	"   $Source: /src/su/lib/RCS/getpars.c,v $";
static string revid =
	"   $Revision: 1.30 $ ; $Date: 89/05/25 16:10:28 $";



#define MAXPFSZ USHRT_MAX

typedef struct {
	string name;		/* external name of parameter	*/
	string asciival;	/* ascii value of parameter	*/
} pointer_table;


static pointer_table *argtbl;	/* symbol table			*/
static int nargs;		/* number of args that parse	*/
static bool first = true;	/* first entry flag		*/


int hgetpar(name, ptr)
string name;	/* external name of parameter	 */
short *ptr;	/* pointer to parameter value	 */
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of shorts found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	/* Echo version on request */
	if (first && ID) {
		(void) fprintf(stderr, "%s: %s\n", __FILE__, progid);
		(void) fprintf(stderr, "%s: %s\n", __FILE__, revid);
	}


	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atohe(aval);
		if (errno) syserr("%s: atohe failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int ugetpar(name, ptr)
string name;	/* external name of parameter	*/
ushort *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of u_shorts found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atoue(aval);
		if (errno) syserr("%s: atoue failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int lgetpar(name, ptr)
string name;	/* external name of parameter	*/
long *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of longs found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atole(aval);
		if (errno) syserr("%s: atole failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int vgetpar(name, ptr)
string name;	/* external name of parameter	*/
ulong *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of u_longs found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atove(aval);
		if (errno) syserr("%s: atove failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int igetpar(name, ptr)
string name;	/* external name of parameter	*/
int *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of ints found			*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atoie(aval);
		if (errno) syserr("%s: atoie failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int pgetpar(name, ptr)
string name;	/* external name of parameter	*/
int *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of u_ints found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to numeric */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		errno = 0;
		*ptr++ = atope(aval);
		if (errno) syserr("%s: atope failed", __FILE__);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int fgetpar(name, ptr)
string name;	/* external name of parameter	*/
float *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of floats found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to float */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		*ptr++ = (float) atof(aval);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int zgetpar(name, ptr)
string name;	/* external name of parameter	*/
double *ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int n;		/* number of doubles found		*/
	string aval;	/* ascii field of symbol		*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Convert vector ascii (e.g. 1,2,3) to double */
	for (n = 0, aval = argtbl[i].asciival; *aval; n++) {
		*ptr++ = atof(aval);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return n;
}

int sgetpar(name, ptr)
string name;	/* external name of parameter	*/
char **ptr;	/* pointer to parameter value	*/
{
	int i;		/* index of name in symbol table	*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return 0;
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return 0;	/* Not there */

	/* Copy the argument */
	*ptr = argtbl[i].asciival;
	return 1;
}

/*
 * Find the index of the argument.  We want later references to
 * a given name to replace earlier ones, hence backwards search.
 */
getparindex(name)
string name;
{
	int i;
	for (i = nargs - 1; i >= 0 ; i--) {
		if (!strcmp(name, argtbl[i].name)) break;
	}
	return i;
}

getparinit()
{
	static int targc;	/* total number of args			*/
	static char **targv;	/* pointer to arg strings		*/
	static string pfname;	/* name of parameter file		*/
	FILE *pffd;		/* file id of parameter file		*/
	int pflen;		/* length of parameter file in bytes	*/ 
	static int pfargc;	/* arg count from parameter file	*/
	bool parfile;		/* parfile existence flag		*/
	int argstrlen;
	string argstr, pargstr;	/* storage for command line and
						parameter file args	*/
	int nread;		/* bytes fread				*/
	int i, j;		/* counters				*/
	string getpfname();	/* return name of parameter file	*/
	int white2null();	/* deliminate arg strings from parameter
				   file with (possibly multiple) NULLs
				   and return a count of the strings	*/
	int tabulate();		/* install symbol table			*/


	first = false;		/* unset first entry flag		 */

	/* Check if xargc was initiated */
	if(!xargc)
		err("%s: xargc=%d -- not initiated in main", __FILE__, xargc);

	/* Space needed for command lines */
	for (i = 1, argstrlen = 0; i < xargc; i++) {
		argstrlen += strlen(xargv[i]) + 1;
	}

	/* Get parfile name if there is one */
	if (pfname = getpfname()) {
		parfile = true;
	} else {
		parfile = false;
		/* Allow argv[1] to be the par file */
		if ((NULL != (pffd = fopen(xargv[1],"r"))) &&
		     (fseek(pffd, 0, 2) == 0) &&
		     (ftell(pffd) < MAXPFSZ) ) {
			rewind(pffd);
			parfile = true;
			while (EOF != (i = getc(pffd))) {
				if (iscntrl(i) || !isascii(i)) {
					parfile = false;
					break;
				}
				fclose(pffd);
				if (parfile) pfname = xargv[1];
			}
		}
	}

	if (parfile) {
	 	if (NULL == (pffd = fopen(pfname, "r")))
			syserr("%s: can't fopen %s", __FILE__, pfname);
		/* Get the length */
		if (-1 == fseek(pffd, 0L, SEEK_END)) {
			syserr("%s: fseek to end of %s failed",
							__FILE__, pfname);
		}
		pflen = ftell(pffd);
		rewind(pffd);
		argstrlen += pflen;
	} else {
		pflen = 0;
	}

	/* Allocate space for command line and parameter file
		plus nulls at the ends to help with parsing. */
	if (NULL==(argstr=(char *)calloc((uint) (1+argstrlen+1),1)))
		syserr("%s: can't calloc space for %s", __FILE__, pfname);

	if(parfile) {
		/* Read the parfile */
		nread = fread(argstr + 1, 1, pflen, pffd);
		if (nread != pflen) {
	 	    syserr("%s: fread only %d bytes out of %d from %s",
					__FILE__,  nread, pflen, pfname);
		}
		if (EOF == fclose(pffd)) {
 		       syserr("%s: fclose failed on %s", __FILE__, pfname);
		}

		/* Zap whites in parfile to help in parsing */
		pfargc = white2null(argstr, pflen);

	} else {
		pfargc = 0;
	}

	/* Total arg count */
	targc = pfargc + xargc - 1;

	/* Allocate space for total arg pointers */
	targv = (char **) malloc((uint) (targc*sizeof(char*)));

	if(parfile) {
		/* Parse the parfile.  Skip over multiple NULLs */
		for (j = 1, i = 0; j < pflen; j++) {
			if (argstr[j] && !argstr[j-1]) {
			       targv[i++] = argstr + j;
			}
		}
	} else {
		i = 0;
	}

	/* Copy command line arguments */
	for (j = 1, pargstr = argstr + pflen + 2; j < xargc; j++) {
		strcpy(pargstr,xargv[j]);
		targv[i++] = pargstr;
		pargstr += strlen(xargv[j]) + 1;
	}

	/* Allocate space for the pointer table */
	argtbl = (pointer_table*)
		malloc((uint) (targc*sizeof(pointer_table)));

	/* Tabulate targv */
	tabulate(targc, targv);
}

#define PFNAME "par="
string getpfname()
{
	int i;
	int pfnamelen;

	pfnamelen = strlen(PFNAME);
	for (i = xargc-1 ; i > 0 ; i--) {
		if(!strncmp(PFNAME, xargv[i], pfnamelen)
		    && strlen(xargv[i]) != pfnamelen) {
			return xargv[i] + pfnamelen;
		}	
	}
	return NULL;
}

#define iswhite(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')

/* Replace the whites by nulls.  If we see a non-white and the previous
   char is a null, this signals the start of a string and we bump the count.
*/
int white2null(str, len)
string str;
int len;
{
	int i;
	int count;
	bool inquote = false;

	for (i = 1, count = 0; i < len; i++) {
		if (str[i]=='"') inquote=(inquote==true)?false:true;
		if (!inquote) {
			if (iswhite(str[i])) { /* Is this a new word ? */
				str[i] = NULL;
			} else if (!str[i-1]) { /* multiple whites */
				count++;
			}
		}
	}
	for (i = 1, inquote=false; i < len; i++) {
		if (str[i]=='"') inquote=(inquote==true)?false:true;
		if (inquote) {
			if (str[i+1]!='"') {
				str[i] = str[i+1];
			} else {
				str[i] = NULL;
				str[i+1] = NULL;
				inquote = false;
			}
		}
	}
	str[len] = NULL;
	return count;
}


int tabulate(argc, argv)
int argc;
char **argv;
{
	int i;
	string eqptr;

	/* Tabulate arguments (if we need to optimize, try
	   defining argvi = *(argv + i) in the i-loop and insert it
	   in place of the argv[i]'s).
	*/
	for (i = 0, nargs = 0 ; i < argc; i++) {
		eqptr = strchr(argv[i], '=');
		if (eqptr) {
			argtbl[nargs].name = argv[i];
			argtbl[nargs].asciival = eqptr + 1;
			*eqptr = (char)0;

			/* Debugging dump */
/* 			fprintf(stderr, */
/* 			"argtbl[%d]: name=%s asciival=%s\n", */
/* 			nargs,argtbl[nargs].name,argtbl[nargs].asciival); */

			nargs++;
		}
	}
}


uint maxgetpar()
{
	static int m = 1;		/* sometimes nargs = 0 */
	static bool localfirst = true;
	int i, mm;

	if (xargc == 1) return 0;

	if (first) (void) igetpar("junkname",&i); /* Just to init argtbl */

	if (localfirst) {
		localfirst = false;
		for (i=0; i<nargs; i++) {
			mm = ccount(',',argtbl[i].asciival) + 1;/* 1,2,3 */
			m = MAX(m, mm);
		}
	}
	return m;
}


int ccount(c, s)
char c, *s;
{
	int i, count;
	for (i = 0, count = 0; s[i] != 0; i++)
		if(s[i] == c) count++;
	return count;
}


/* For FORTRAN */
int hgetpar_(name, ptr)
string name;
short *ptr;
{
	return hgetpar(name, ptr);
}

int ugetpar_(name, ptr)
string name;
ushort *ptr;
{
	return ugetpar(name, ptr);
}

int lgetpar_(name, ptr)
string name;
long *ptr;
{
	return lgetpar(name, ptr);
}

int vgetpar_(name, ptr)
string name;
ulong *ptr;
{
	return vgetpar(name, ptr);
}

int igetpar_(name, ptr)
string name;
int *ptr;
{
	return igetpar(name, ptr);
}

int pgetpar_(name, ptr)
string name;
uint *ptr;
{
	return pgetpar(name, ptr);
}

int fgetpar_(name, ptr)
string name;
float *ptr;
{
	return fgetpar(name, ptr);
}

int zgetpar_(name, ptr)
string name;
double *ptr;
{
	return zgetpar(name, ptr);
}



#ifdef TEST
#define N 100
main(argc, argv)
int argc; char **argv;
{
	string name;
	short h, vh[N];
	ushort u, vu[N];
	long l, vl[N];
	ulong v, vv[N];
	int i, vi[N], nh, nu, nl, nv, ni, np, nf, nz;
	uint p, vp[N];
	float f, vf[N];
	double z, vz[N];

	xargc = argc;	xargv = argv;
	
	printf("first maxgetpar returned %d\n", maxgetpar());

	if (sgetpar("name", &name))	printf("name=%s\n", name);
	else				printf("name not specified\n");

	if (hgetpar("h", &h))		printf("h=%d\n", h);
	else				printf("h not specified\n");

	if (ugetpar("u", &u))		printf("u=%u\n", u);
	else				printf("u not specified\n");

	if (lgetpar("l", &l))		printf("l=%ld\n", l);
	else				printf("l not specified\n");

	if (vgetpar("v", &v))		printf("v=%lu\n", v);
	else				printf("v not specified\n");

	if (igetpar("i", &i))		printf("i=%d\n", i);
	else				printf("i not specified\n");

	if (pgetpar("p", &p))		printf("p=%u\n", p);
	else				printf("p not specified\n");

	if (fgetpar("f", &f))		printf("f=%g\n", f);
	else				printf("f not specified\n");

	if (zgetpar("z", &z))		printf("z=%g\n", z);
	else				printf("z not specified\n");


	printf("second maxgetpar returned %d\n", maxgetpar());

	nh = hgetpar("vh", vh);
	if (nh) {
		printf("vh=");
		for (i=0;i<nh;i++) printf("%d%c",vh[i],i==nh-1?'\n':',');
	}
	else			printf("vh not specified\n");

	nu = ugetpar("vu", vu);
	if (nu) {
		printf("vu=");
		for (i=0;i<nu;i++) printf("%u%c",vu[i],i==nu-1?'\n':',');
	}
	else			printf("vu not specified\n");

	nl = lgetpar("vl", vl);
	if (nl) {
		printf("vl=");
		for (i=0;i<nl;i++) printf("%ld%c",vl[i],i==nl-1?'\n':',');
	}
	else			printf("vl not specified\n");

	nv = vgetpar("vv", vv);
	if (nv) {
		printf("vv=");
		for (i=0;i<nv;i++) printf("%lu%c",vv[i],i==nv-1?'\n':',');
	}
	else			printf("vv not specified\n");

	ni = igetpar("vi", vi);
	if (ni) {
		printf("vi=");
		for (i=0;i<ni;i++) printf("%d%c",vi[i],i==ni-1?'\n':',');
	}
	else			printf("vi not specified\n");

	np = pgetpar("vp", vp);
	if (np) {
		printf("vp=");
		for (i=0;i<np;i++) printf("%u%c",vp[i],i==np-1?'\n':',');
	}
	else			printf("vp not specified\n");

	nf = fgetpar("vf", vf);
	if (nf) {
		printf("vf=");
		for (i=0;i<nf;i++) printf("%g%c",vf[i],i==nf-1?'\n':',');
	}
	else			printf("vf not specified\n");

	nz = zgetpar("vz", vz);
	if (nz) {
		printf("vz=");
		for (i=0;i<nz;i++) printf("%g%c",vz[i],i==nz-1?'\n':',');
	}
	else			printf("vz not specified\n");

	printf("last maxgetpar returned %d\n",maxgetpar());

	return 0;
}
#endif
