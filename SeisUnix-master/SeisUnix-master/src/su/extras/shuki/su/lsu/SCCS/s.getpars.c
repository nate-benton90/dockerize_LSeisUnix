h53714
s 00249/00196/00199
d D 1.3 88/11/15 14:01:35 shuki 3 2
c 
e
s 00229/00326/00166
d D 1.2 88/09/27 10:50:25 shuki 2 1
c Hashing using hsearch()
e
s 00492/00000/00000
d D 1.1 88/04/14 13:47:41 shuki 1 0
c date and time created 88/04/14 13:47:41 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * getpars.c
I 2
 *
 * History
D 3
 *    7/1986 Shuki Ronen:
 *       Complete rewrite of SEP's getpar().  Motivation: SEP's getpar too
 *       mangled.
E 3
I 3
 *      1986 Shuki Ronen:
 *           Complete rewrite of SEP's getpar().
E 3
 *    3/1987 Jack Cohen:
D 3
 *       index(). General cleanup. Comments.
E 3
I 3
 *           Cosmetics.
E 3
 *    3/1987 Shuki Ronen:
D 3
 *       maxgetpar()
E 3
I 3
 *           maxgetpar()
E 3
 *    7/1987 Shuki Ronen:
D 3
 *       Optional parfile name is argv[1].
E 3
I 3
 *           Optional parfile name is argv[1].
E 3
 *    9/1987 Shuki Ronen:
D 3
 *       Major revision: breaking the single getpar(type,name,ptr) to many
 *       Xgetpar(name,ptr) X=[ifzulh].  Motivation: the old (SEP's)
 *       specification could not be ported to machines sensitive to
 *       inconsistent argument calls (of interest at the moment
 *       Cray/Unicos).
E 3
I 3
 *           A major rewrite: breaking the single getpar(type,name,ptr)
 *           to many Xgetpar(name,ptr) X=[ifzulh].
 *           Motivation: the old (SEP's) specification could not be ported
 *           to machines sensitive to inconsistent argument calls, such as
 *           Cray/Unicos
E 3
 *    2/1988 Shuki Ronen:
D 3
 *       Added bgetpar()
 *    9/1988 Shuki Ronen:
 *       Major revision: hashing via hsearch(3)
E 3
I 3
 *           Added bgetpar()
E 3
E 2
 */
#include <stdio.h>
#include <ctype.h>
I 2
#include <search.h>
E 2

D 2
#define MAXPFSZ 65536
E 2
I 2
D 3
#define MAXPFSZ 10000
E 3
I 3
#define MAXPFSZ 65536
E 3
E 2
#define MAX(x,y) (x)>(y)?(x):(y)

D 2
typedef struct {
	char *name;	/* External name of parameter */
	char *asciival;	/* Ascii value of parameter */
} pointer_table;

E 2
typedef enum {false,true} bool;

extern int xargc;
extern char **xargv;

I 3
static ENTRY *argtbl;       /* symbol table */
E 3
D 2
static pointer_table *argtbl;	/* symbol table */
static int nargs;		/* number of args that parse */
static bool first = true;	/* first entry flag */
E 2
I 2
static int nargs;           /* number of args that parse */
static bool first = true;   /* first entry flag */
D 3
static int nmaxgetpar=0;
E 3
E 2

I 2
static char SccsId[]="%W% %G%\n";
E 2

int igetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
int *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;                 /* External name of parameter */
int *ptr;                   /* Pointer to parameter value */
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
E 3
E 2
	int atoi();

	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to long */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
E 2
I 2
	if (first) getparinit(); /* Tabulate command line and parfile */
D 3

	data = find_data(name);
	if(data==NULL) return(0);

	/* Convert vector ascii (e.g. 1,2,3 */
	for (n=0,aval=data; *aval; n++) {
E 3
I 3
	i = getparindex(name);   /* Get parameter index */
	if (i < 0) return(0);    /* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to long */
	for (n=0,aval=argtbl[i].data; *aval; n++) {
E 3
E 2
		*ptr++ = atoi(aval);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int lgetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
long *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
long *ptr;
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
E 3
E 2
	long atol();
I 3

E 3
D 2

E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to long */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	for (n=0,aval=data; *aval; n++) {
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	for (n=0,aval=argtbl[i].data; *aval; n++) {
E 3
E 2
		*ptr++ = atol(aval);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int ugetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
unsigned short *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
unsigned short *ptr;
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
	long atol();

E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
	int atoi();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
	long atol();

E 3
E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to long */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
		*ptr++ = (unsigned short) atol(aval);
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	for (n=0,aval=data; *aval; n++) {
		*ptr++ = (unsigned short)atoi(aval);
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	for (n=0,aval=argtbl[i].data; *aval; n++) {
		*ptr++ = (unsigned short) atol(aval);
E 3
E 2
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int hgetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
short *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
short *ptr;
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
E 3
E 2
	int atoi();
I 3

E 3
D 2

E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to long */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
		*ptr++ = atoi(aval);
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	for (n=0,aval=data; *aval; n++) {
		*ptr++ = (short)atoi(aval);
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	for (n=0,aval=argtbl[i].data; *aval; n++) {
		*ptr++ = atoi(aval);
E 3
E 2
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int fgetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
float *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
float *ptr;
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
E 3
E 2
	double atof();
I 3

E 3
D 2

E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to float */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
		*ptr++ = atof(aval);
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	for (n=0,aval=data; *aval; n++) {
		*ptr++ = (float)atof(aval);
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	for (n=0,aval=argtbl[i].data; *aval; n++) {
		*ptr++ = atof(aval);
E 3
E 2
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int zgetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
double *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
double *ptr;
E 2
{
D 2
	int i,n;
	int getparindex();
	char *aval;
E 2
I 2
D 3
	int n;
	char *aval,*data,*find_data();
E 3
I 3
	int i,n;
	int getparindex();
	char *aval;
E 3
E 2
	double atof();
I 3

E 3
D 2

E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Convert vector ascii (e.g. 1,2,3 to double */
	for (n=0,aval=argtbl[i].asciival; *aval; n++) {
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	for (n=0,aval=data; *aval; n++) {
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	for (n=0,aval=argtbl[i].data; *aval; n++) {
E 3
E 2
		*ptr++ = atof(aval);
		while (*aval++ != ',') {
			if (!*aval) break;
		}
	}
	return(n);
}

int sgetpar(name, ptr)
D 2
char *name;	/* External name of parameter */
char *ptr;	/* Pointer to parameter value */
E 2
I 2
char *name;
D 3
char *ptr;
E 3
I 3
char **ptr;
E 3
E 2
{
D 2
	int i;
	int getparindex();

E 2
I 2
D 3
	char *data,*find_data();
	double atof();
E 3
I 3
	int i;
	int getparindex();

E 3
E 2
	if (xargc == 1) return(0);
D 2
	if (first) getparinit();	/* Tabulate command line and parfile */
	i = getparindex(name);		/* Get parameter index */
	if (i < 0) return(0);		/* Not there */
	/* Copy the argument */
	strcpy(ptr,argtbl[i].asciival);
E 2
I 2
	if (first) getparinit();
D 3
	data = find_data(name);
	if(data==NULL) return(0);
	strcpy(ptr,data);
E 2
	return(strlen(ptr));
E 3
I 3
	i = getparindex(name);
	if (i < 0) return(0);
	*ptr = argtbl[i].data;
	return(strlen(*ptr));
E 3
}

D 2
int bgetpar(name,ptr)
E 2
I 2
D 3
int bgetpar(name, ptr)
E 3
I 3
int bgetpar(name,ptr)
E 3
E 2
char *name;
bool *ptr;
{
D 2
	int i;
	char buff[128];
E 2
I 2
D 3
	char *data,*find_data();
E 3
I 3
	int i;
	char buff[128];
E 3
E 2

D 2
	i = sgetpar(name,buff);
E 2
I 2
D 3
	data = find_data(name);
	if(data==NULL) return(0);
E 3
I 3
	i = sgetpar(name,buff);
E 3
E 2

D 2
	if(i)
	if(*buff=='y'||*buff=='Y'||!strcmp(buff,"on")||
		!strcmp(buff,"ON")||*buff=='1'||!strcmp(buff,"true")) {
E 2
I 2
D 3
	if(*data=='y'||*data=='Y'||
		!strcmp(data,"on")||!strcmp(data,"ON")||
		*data=='1'||
		!strcmp(data,"true")||!strcmp(data,"TRUE"))
	{
E 3
I 3
	if(i)
	if(*buff=='y'||*buff=='Y'||!strcmp(buff,"on")||
		!strcmp(buff,"ON")||*buff=='1'||!strcmp(buff,"true")) {
E 3
E 2
		*ptr = true;
D 2
	} else if(*buff=='n'||*buff=='N'||!strcmp(buff,"off")||
		!strcmp(buff,"OFF")||*buff=='0') {
E 2
I 2
D 3
	} else if(*data=='n'||*data=='N'||
		!strcmp(data,"off")||!strcmp(data,"OFF")||
		*data=='0'||
		!strcmp(data,"false")||!strcmp(data,"FALSE"))
	{
E 3
I 3
	} else if(*buff=='n'||*buff=='N'||!strcmp(buff,"off")||
		!strcmp(buff,"OFF")||*buff=='0') {
E 3
E 2
		*ptr = false;
	}
D 2
	return(i);
E 2
I 2
D 3
	return(1);
E 3
I 3
	return(i);
E 3
E 2
}

D 2
/*
 * Find the index of the argument.  We want later references to
 * a given name to replace earlier ones, hence backwards search.
 */
getparindex(name)
E 2
I 2
D 3
char *find_data(name)
E 3
I 3
/*
 * Find the index of the argument.  We want later references to
 * a given name to replace earlier ones, hence backwards search.
 */
getparindex(name)
E 3
E 2
char *name;
{
D 2
	int i;
	for (i = nargs - 1; i >= 0 ; i--) {
		if (!strcmp(name, argtbl[i].name)) break;
	}
	return(i);
E 2
I 2
D 3
	ENTRY item,*found_item,*hsearch();

	item.key = name;
	found_item = hsearch(item,FIND);
	if(found_item==NULL) return(NULL);
	return(found_item->data);
E 3
I 3
	int i;
	for (i = nargs - 1; i >= 0 ; i--) {
		if (!strcmp(name, argtbl[i].key)) break;
	}
	return(i);
E 3
E 2
}

I 2
D 3
maxgetpar()
{
	if (xargc == 1) return(0);
	if (first) getparinit();
	return(nmaxgetpar);
}


int ccount(c, s)
char c, *s;
{
	int i, count;
	for (i = 0, count = 0; s[i] != 0; i++)
		if(s[i] == c) count++;
	return(count);
}

E 3
E 2
getparinit()
{
D 2
	int i,j,nread;
	static int targc;	/* total number of args			*/
	static char **targv;	/* pointer to arg strings		*/
	static char *pfname;	/* name of parameter file		*/
	FILE *pffd;		/* file id of parameter file		*/
	int pflen;		/* length of parameter file in bytes	*/ 
	static int pfargc;	/* arg count from parameter file	*/
	bool parfile;		/* parfile existence flag		*/
	int argstrlen;
	char *argstr,*pargstr;	/* storage for command line and
						parameter file args	*/
	char *getpfname();	/* return name of parameter file	*/
	int white2null();	/* deliminate arg strings from parameter
				   file with (possibly multiple) NULLs
				   and return a count of the strings	*/
	int tabulate();		/* install symbol table			*/
/* 	char *malloc(); */
/* 	char *calloc(); */
E 2
I 2
D 3
	int i;
	char *cptr;
E 3
I 3
	int i,j,nread;
	static int targc;      /* total number of args   */
	static char **targv;   /* pointer to arg strings  */
	static char *pfname;   /* name of parameter file  */
E 3
	FILE *pffd;            /* file id of parameter file  */
D 3
	enum {no,yes,maybe} isparfile;
	int pflen,pfargc,nread,j;
	char *pfname,*pfbuff,**pfargv;
E 3
I 3
	int pflen;             /* length of parameter file in bytes */ 
	static int pfargc;     /* arg count from parameter file */
	bool parfile;          /* parfile existence flag  */
	int argstrlen;
	char *argstr,*pargstr; /* storage for command line and parameter
	                          file args */
	char *getpfname();     /* return name of parameter file */
	int white2null();      /* deliminate arg strings from parameter file
	                          with (possibly multiple) NULLs and return a
	                          count of the strings */
	int tabulate();        /* install symbol table   */
E 3
E 2

I 2
D 3
	if(first==false) return;

E 3
E 2
	first = false;

	/* Check if xargc was initiated */
D 2
	if(xargc<=0) {
		err(__FILE__,__LINE__,"getpar: xargc=%d--probably not initiated in main",xargc);
	}
E 2
I 2
	if(xargc<=0) err(__FILE__,__LINE__,
		"getpar: xargc=%d--probably not initiated in main",xargc);
E 2

D 2
	/* Space needed for command line arguments */
	for (i=1, argstrlen=0; i<xargc; i++) {
		argstrlen += strlen(xargv[i]) + 1;
	}
E 2
I 2
D 3
	hcreate(xargc+MAXPFSZ/4);
E 3
I 3
	/* Space needed for command line arguments */
	for (i=1, argstrlen=0; i<xargc; i++) {
		argstrlen += strlen(xargv[i]) + 1;
	}
E 3
E 2

D 2
	/* Get parfile name if there is one */
	if (pfname = getpfname()) {
		parfile = true;
	} else {
		parfile = false;
		/* But... allow argv[1] to be the par file */
			/* Can you open xargv[1] and isn't it too long? */
		if ( (NULL!=(pffd=fopen(xargv[1],"r"))) &&
		     (fseek(pffd,0,2)==0) && (ftell(pffd)<MAXPFSZ) ) {
			rewind(pffd);
			parfile = true;
				/* The parfile candidate should not contain
				   control or non ascii characters */
			while (EOF != (i = getc(pffd))) {
				if (iscntrl(i) || !isascii(i)) {
					parfile = false;	/* Flunked */
					break;
				}
			}
			fclose(pffd);
			if (parfile) pfname = xargv[1];
		}
	}
E 2
I 2
D 3
	/* Command Line: */
	tabulate(xargc,xargv);
E 3
I 3
	/* Get parfile name if there is one */
	if (pfname = getpfname()) {
		parfile = true;
	} else {
		parfile = false;
		/* But... allow argv[1] to be the par file */
			/* Can you open xargv[1] and isn't it too long? */
		if ( (NULL!=(pffd=fopen(xargv[1],"r"))) &&
		     (fseek(pffd,0,2)==0) && (ftell(pffd)<MAXPFSZ) ) {
			rewind(pffd);
			parfile = true;
			/* The parfile candidate should not contain
			   control or non ascii characters */
			while (EOF != (i = getc(pffd))) {
				if (iscntrl(i) || !isascii(i)) {
					parfile = false;	/* Flunked */
					break;
				}
			}
			fclose(pffd);
			if (parfile) pfname = xargv[1];
		}
	}
E 3
E 2

D 2
	if (parfile) {
	 	if (NULL == (pffd = fopen(pfname, "r")))
			err(__FILE__,__LINE__,"getpar: can't fopen %s", pfname);
		/* Get the length */
		if (-1 == fseek(pffd, 0L, 2)) {
			err(__FILE__,__LINE__,"getpar: fseek to end of %s failed",pfname);
		}
		pflen = ftell(pffd);
		rewind(pffd);
		argstrlen += pflen;
E 2
I 2
D 3
	/* Parameter File: */
	cptr = find_data("par");
	if(cptr==NULL) cptr = find_data("PAR");
	if(cptr!=NULL) {
		isparfile = yes;
		pfname = cptr;
E 3
I 3
	if (parfile) {
	 	if (NULL == (pffd = fopen(pfname, "r"))) err(__FILE__,__LINE__,
			"getpar: can't fopen %s", pfname);
		/* Get the length */
		if (-1 == fseek(pffd, 0L, 2)) err(__FILE__,__LINE__,
			"getpar: fseek to end of %s failed",pfname);
		pflen = ftell(pffd);
		rewind(pffd);
		argstrlen += pflen;
E 3
E 2
	} else {
D 2
		pflen = 0;
E 2
I 2
D 3
		isparfile = maybe;
		pfname = xargv[1];
E 3
I 3
		pflen = 0;
E 3
E 2
	}
I 2
D 3
	
	 pffd = fopen(pfname, "r");
	 if (NULL == pffd) {
		if(isparfile==yes)
			err(__FILE__,__LINE__,"getparinit: can't fopen %s", pfname);
		return;
	}
E 3
E 2

D 2
	/* Allocate space for command line and parameter file
		plus nulls at the ends to help with parsing. */
	if (NULL==(argstr=(char*)calloc((unsigned) 1+argstrlen+1,1)))
		err(__FILE__,__LINE__,"getpar: can't calloc space for %s",pfname);
E 2
I 2
D 3
	/* Get the length */
	if (-1 == fseek(pffd, 0L, 2))
		err(__FILE__,__LINE__,"getparinit: fseek to end of %s failed",pfname);
E 3
I 3
	/* Allocate space for command line and parameter file
		plus nulls at the ends to help with parsing. */
	if (NULL==(argstr=(char*)calloc((unsigned) 1+argstrlen+1,1)))
		err(__FILE__,__LINE__,"getpar: can't calloc space for %s",pfname);
E 3
E 2

D 2
	if(parfile) {
		/* Read the parfile */
		nread = fread(argstr + 1, 1, pflen, pffd);
		if (nread != pflen) {
		 	err(__FILE__,__LINE__,"getpar: fread only %d bytes out of %d from %s",
						nread,pflen,pfname);
		}
		if (EOF == fclose(pffd)) {
 		       err(__FILE__,__LINE__,"getpar: fclose failed on %s", pfname);
		}
E 2
I 2
D 3
	pflen = ftell(pffd);
	if(pflen>MAXPFSZ) {
		if(isparfile==yes)
			err(__FILE__,__LINE__,
			"getparinit: parfile %s too big (size=%d, MAXPFSZ=%d)"
			,pfname,pflen,MAXPFSZ);
		return;
	}
E 3
I 3
	if(parfile) {
		/* Read the parfile */
		nread = fread(argstr + 1, 1, pflen, pffd);
		if (nread != pflen) err(__FILE__,__LINE__,
			"getpar: fread only %d bytes out of %d from %s",
			nread,pflen,pfname);
		if (EOF == fclose(pffd)) err(__FILE__,__LINE__,
			"getpar: fclose failed on %s", pfname);
E 3
E 2

D 2
		/* Zap whites in parfile to help in parsing */
		pfargc = white2null(argstr, pflen);
E 2
I 2
D 3
	rewind(pffd);
E 3
I 3
		/* Zap whites in parfile to help in parsing */
		pfargc = white2null(argstr, pflen);
E 3
E 2

D 2
	} else {
		pfargc = 0;
E 2
I 2
D 3
	pfbuff = (char*) malloc((unsigned)(1+pflen+1));
	if(pfbuff==NULL) {
		if (isparfile=yes)
			err(__FILE__,__LINE__,
			"getparinit: can't malloc %d bytes for pfbuff",pflen);
			return;
E 3
I 3
	} else {
		pfargc = 0;
E 3
E 2
	}

D 2
	/* Total arg count */
	targc = pfargc + xargc - 1;
E 2
I 2
D 3
	/* Read the parfile */
	pfbuff[0] = 0;
	nread = fread(pfbuff+1,1,pflen,pffd);
	pfbuff[pflen+1] = 0;
E 3
I 3
	/* Total arg count */
	targc = pfargc + xargc - 1;
E 3
E 2

D 2
	/* Allocate space for total arg pointers */
	targv = (char **) malloc((unsigned) (targc*sizeof(char*)));

	if(parfile) {
		/* Parse the parfile.  Skip over multiple NULLs */
		for (j = 1, i = 0; j < pflen; j++) {
			if (argstr[j] && !argstr[j-1]) {
			       targv[i++] = argstr + j;
			}
E 2
I 2
D 3
	if (nread != pflen) {
		if (isparfile=yes) {
			err(__FILE__,__LINE__,
				"getparinit: fread only %d bytes out of %d from %s",
				nread,pflen,pfname);
		} else {
			warn(__FILE__,__LINE__,
				"getparinit: fread only %d bytes out of %d from %s",
				nread,pflen,pfname);
E 3
I 3
	/* Allocate space for total arg pointers */
	targv = (char **) malloc((unsigned) (targc*sizeof(char*)));

	if(parfile) {
		/* Parse the parfile.  Skip over multiple NULLs */
		for (j = 1, i = 0; j < pflen; j++) {
			if (argstr[j] && !argstr[j-1]) {
			       targv[i++] = argstr + j;
			}
E 3
E 2
		}
D 2
	} else {
		i = 0;
E 2
I 2
D 3
		return;
E 3
I 3
	} else {
		i = 0;
E 3
E 2
	}

D 2
	/* Copy command line arguments */
	for (j = 1, pargstr = argstr + pflen + 2; j < xargc; j++) {
		strcpy(pargstr,xargv[j]);
		targv[i++] = pargstr;
		pargstr += strlen(xargv[j]) + 1;
E 2
I 2
D 3
	if (EOF == fclose(pffd)) {
		err(__FILE__,__LINE__,
			"getparinit: fclose failed on %s", pfname);
E 3
I 3
	/* Copy command line arguments */
	for (j = 1, pargstr = argstr + pflen + 2; j < xargc; j++) {
		strcpy(pargstr,xargv[j]);
		targv[i++] = pargstr;
		pargstr += strlen(xargv[j]) + 1;
E 3
E 2
	}

D 2
	/* Allocate space for the pointer table */
	argtbl = (pointer_table*)
		malloc((unsigned) (targc*sizeof(pointer_table)));
E 2
I 2
D 3
	/* Zap whites in parfile to help in parsing */
	pfargc = white2null(pfbuff,pflen);
E 3
I 3
	/* Allocate space for the pointer table */
	argtbl = (ENTRY*) malloc((unsigned) (targc*sizeof(ENTRY)));
E 3
E 2

D 2
	/* Tabulate targv */
	tabulate(targc, targv);
E 2
I 2
D 3
	pfargv = (char**) malloc((unsigned)(pfargc*sizeof(char*)));
	if(pfargv==NULL) {
			err(__FILE__,__LINE__,
			"getparinit: can't malloc %d pointers for **pfargv",pfargc);
			return;
	}


	/* Parse the parfile.  Skip over multiple NULLs */
	for (j=1,i=0;j<pflen;j++) {
		if (pfbuff[j] && !pfbuff[j-1]) {
		       pfargv[i++] = pfbuff + j;
		}
	}
	tabulate(pfargc,pfargv);
E 3
I 3
	/* Tabulate targv */
	tabulate(targc, targv);
E 3
E 2
}

D 2
#define PFNAME "par="
char *getpfname()
E 2
I 2
D 3
tabulate(argc,argv)
int argc;
char **argv;
E 3
I 3
#define PFNAME "par="
char *getpfname()
E 3
E 2
{
D 2
	int i, pfnamelen;
E 2
I 2
D 3
	int i,m;
	char *cptr;
	ENTRY item;
E 3
I 3
	int i, pfnamelen;
E 3
E 2

D 2
	pfnamelen = strlen(PFNAME);
	for (i = xargc-1 ; i > 0 ; i--) {
		if(!strncmp(PFNAME, xargv[i], pfnamelen)
		    && strlen(xargv[i]) != pfnamelen) {
			return(xargv[i] + pfnamelen);
		}	
E 2
I 2
D 3
	for(i=argc-1;i>=0;i--) {
		cptr = (char *) index(argv[i], '=');
		if (cptr==NULL) continue;
		item.key =  argv[i];
		*cptr = (char)0;              /* Need a NULL terminator for item.key */
		if(hsearch(item,FIND)!=NULL) continue;
		item.data = cptr + 1;
		hsearch(item,ENTER);
		/* *cptr = '=';                  /* CANNOT Restore xargv !!! */
		m = ccount(',',item.data) + 1;
		nmaxgetpar = MAX(nmaxgetpar, m);
		nargs++;
E 3
I 3
	pfnamelen = strlen(PFNAME);
	for (i = xargc-1 ; i > 0 ; i--) {
		if(!strncmp(PFNAME, xargv[i], pfnamelen)
		    && strlen(xargv[i]) != pfnamelen) {
			return(xargv[i] + pfnamelen);
		}	
E 3
E 2
	}
I 3
	return(NULL);
E 3
D 2
	return(NULL);
E 2
}

#define iswhite(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')
I 3

E 3
D 2

E 2
/* Replace the whites by nulls.  If we see a non-white and the previous
D 2
   char is a null, this signals the start of a string and we bump the count.
*/
E 2
I 2
D 3
 * char is a null, this signals the start of a string and we bump the count. */
E 3
I 3
 * char is a null, this signals the start of a string and we bump the count.
 */
E 3
E 2
int white2null(str, len)
char *str; int len;
{
	int i,count;
	bool inquote = false;
	for (i = 1, count = 0; i < len; i++) {
D 2
		if (str[i]=='"') inquote=(inquote==true)?false:true;
E 2
I 2
D 3
		if (str[i]=='"') inquote=(inquote==true)?false:true; /* Toggle inquote */
E 3
I 3
		if (str[i]=='"') inquote=(inquote==true)?false:true;
E 3
E 2
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
I 3
	return(count);
}

int tabulate(argc, argv)
int argc;
char **argv;
{
	int i;
	char *eqptr;

	/* Tabulate arguments (if we need to optimize, try
	 * defining argvi = *(argv + i) in the i-loop and insert it
	 * in place of the argv[i]'s).
	 */
	for (i = 0, nargs = 0 ; i < argc; i++) {
		eqptr = (char *) index(argv[i], '=');
		if (eqptr) {
			argtbl[nargs].key = argv[i];
			argtbl[nargs].data = eqptr + 1;
			*eqptr = (char)0;

			/* Debugging dump */
/* 			fprintf(stderr, */
/* 			"argtbl[%d]: name=%s data=%s\n", */
/* 			nargs,argtbl[nargs].name,argtbl[nargs].data); */

			nargs++;
		}
	}
}


maxgetpar()
{
	static int m = 1;                 /* sometimes nargs = 0 */
	static bool localfirst = true;
	int i, mm;

	if (xargc == 1) return(0);
	if (first) getparinit();

	if (localfirst) {
		localfirst = false;
		for (i=0; i<nargs; i++) {
			mm = ccount(',',argtbl[i].data) + 1; /* 1,2,3 */
			m = MAX(m, mm);
		}
	}
	return(m);
}


int ccount(c, s)
char c, *s;
{
	int i, count;
	for (i = 0, count = 0; s[i] != 0; i++)
		if(s[i] == c) count++;
E 3
	return(count);
}
D 2


/*
int atoif(asciival, ptr, type)
char *asciival, *type;
mixed ptr;
{
	int n;

	for (n = 0; *asciival; n++) {
		switch (*type) {
		case 'h':
			*ptr.h++ = (short) atoi(asciival);
			break;
		case 'u':
			*ptr.u++ = (unsigned short) atoi(asciival);
			break;
		case 'l':
			*ptr.l++ = atol(asciival);
			break;
		case 'v':
			*ptr.v++ = (unsigned long) atol(asciival);
			break;
		case 'd':
		case 'i':
			*ptr.d++ = atoi(asciival);
			break;
		case 'f':
		case 'r':
			*ptr.f++ = (float) atof(asciival);
			break;
		case 'z':
		case 'g':
			*ptr.z++ = atof(asciival);
			break;
		default:
			err(__FILE__,__LINE__,"atoif: mysterious type %c", *type);
		}
		while (*asciival++ != ',') {
			if (!*asciival) break;
		}
	}
	return(n);
}
*/

int tabulate(argc, argv)
int argc;
char **argv;
{
	int i;
	char *eqptr;

	/* Tabulate arguments (if we need to optimize, try
	   defining argvi = *(argv + i) in the i-loop and insert it
	   in place of the argv[i]'s).
	*/
	for (i = 0, nargs = 0 ; i < argc; i++) {
		eqptr = (char *) index(argv[i], '=');
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


maxgetpar()
{
	static int m = 1;		/* sometimes nargs = 0 */
	static bool localfirst = true;
	int i, mm;

	if (first) igetpar("junkname",&i); /* Just to init argtbl */

	if (localfirst) {
		localfirst = false;
		for (i=0; i<nargs; i++) {
			mm = ccount(',',argtbl[i].asciival) + 1;/* 1,2,3 */
			m = MAX(m, mm);
		}
	}
	return(m);
}


int ccount(c, s)
char c, *s;
{
	int i, count;
	for (i = 0, count = 0; s[i] != 0; i++)
		if(s[i] == c) count++;
	return(count);
}


E 2
E 1
