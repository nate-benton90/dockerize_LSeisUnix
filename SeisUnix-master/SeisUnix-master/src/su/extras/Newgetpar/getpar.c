
/*
 * GETPAR - system for specifying parameters for programs
 *
 * By Brian Sumner
 */

#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../include/cwp.h"

typedef union {
	int i_typ;
	float f_typ;
} num;

typedef struct {
	char *name;
	int type;
	int llen;
	union {
		int i_typ;
		float f_typ;
		char *s_typ;
		float *fl_typ;
		int *il_typ;
	} u;
} entry;

/* For token input */
#define STR_SIZE	1024

/* Max number of vector elements */
#define NUM_SIZE	256

/* Max number of associations */
#define	TABLE_SIZE	1024

/* Max size of @ file */
#define MAX_SIZE	32768

#define	INTEGER		1
#define	FLOAT		2
#define	STRING		3
#define	FLIST		4
#define	ILIST		5
#define	DONE		6
#define RELOC		7
#define	ENTRY		8
#define	ERROR		9

static char str[STR_SIZE];
static num nums[NUM_SIZE];
static entry *table[TABLE_SIZE];
static int entries = 0;

/*
 * GETNUM - get a number and determine it's type
 *
 * Parameters:
 *    sp	- pointer to pointer to string to get
 *    n		- index in nums to place number
 */

static int getnum(sp,n)
char **sp;
int n;
{

	char *s = *sp;
	char *t = str;
	int c;

	double atof();
	int atoi();

	c = *s;
	if (c == '-') {
		*t++ = '-';
		c = *++s;
	}

	while(isdigit(c)) {
		*t++ = c;
		c = *++s;
	}

	if (c == '.') {
		*t++ = '.';
		c = *++s;
		while (isdigit(c)) {
			*t++ = c;
			c = *++s;
		}
		if (c == 'e' || c == 'E') {
			*t++ = 'e';
			c = *++s;
			if (c == '+' || c == '-') {
				*t++ = c;
				c = *++s;
			}
			while (isdigit(c)) {
				*t++ = c;
				c = *++s;
			}
		}
		*t = '\0';
		nums[n].f_typ = atof(str);
		*sp = s;
		return(FLOAT);
	} else {
		*t = '\0';
		nums[n].i_typ = atoi(str);
		*sp = s;
		return(INTEGER);
	}
}

/*
 * GETLIST - get a list of numbers
 *
 * Parameters:
 *    sp	- pointer to pointer to string to get
 *    lenp	- pointer to length of list
 */

static int getlist(sp,lenp)
char **sp;
int *lenp;
{
	char *s = *sp;
	register int c;
	int typ;
	int len = 0;

	for (;;) {
		c = *s;
		while (isspace(c)) c = *++s;

		switch(c) {
		case '\0':
			return(ERROR);

		case '}':
			if (!len) return(ERROR);
			*lenp = len;
			*sp = s+1;
			return(typ);

		case '.': case '-': case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7': case '8': case '9':
			switch(getnum(&s,len)) {
			case FLOAT:
				if (!len) typ = FLOAT;
				else if (typ == INTEGER)
					nums[len].i_typ = (int) nums[len].f_typ;
				break;
			case INTEGER:
				if (!len) typ = INTEGER;
				else if (typ == FLOAT)
					nums[len].f_typ = (float) nums[len].i_typ;
				break;
			default:
				return(ERROR);
			}
			++len;
			break;
		default:
			return(ERROR);
		}
	}
}

/*
 * GETSTR - get a string delimited by "..."
 *
 * Parameters:
 *    sp	- pointer to pointer to string to get
 */

static int getstr(sp)
char **sp;
{

	register char *s = *sp;
	register char *t = str;
	register char c;

	for (;;) {
		c = *++s;
		switch (c) {
		case '\0':
			return(0);

		case '"':
			*t = '\0';
			*sp = s+1;
			return(1);

		case '\\':
			c = *++s;
			switch (c) {
			case '\\':
				*t++ = '\\';
				break;
			case 't':
				*t++ = '\t';
				break;
			case 'n':
				*t++ = '\n';
				break;
			case 'b':
				*t++ = '\b';
				break;
			default:
				*t++ = c;
				break;
			}
			break;
		default:
			*t++ = c;
			break;
		}
	}
}

/*
 * ENTER - enter item in table
 *
 * Parameters:
 *    tp	- token pointer
 */

static void enter(tp)
entry *tp;
{

	int i;
	entry *ep;

	/*
	 * First definition has precidence.  Delete storage
	 * if necessary.
	 */

	for (i=0;i<entries;++i) {
		ep = table[i];
		if (!strcmp(tp->name,ep->name)) {
			free(tp->name);
			if (tp->type == FLIST) free((*tp).u.fl_typ);
			else if (tp->type == ILIST)
				free((*tp).u.il_typ);
			return;
		}
	}

	i = entries;
	if (i == TABLE_SIZE) {
		err("getpar: Table overflow: %s",tp->name);
		return;
	}
	++entries;

	table[i] = ep = (entry *) malloc(sizeof(entry));
	ep->name = tp->name;
	ep->type = tp->type;
	ep->llen = tp->llen;
	switch(tp->type) {
	case FLOAT:
		(*ep).u.f_typ = (*tp).u.f_typ;
		break;
	case INTEGER:
		(*ep).u.i_typ = (*tp).u.i_typ;
		break;
	case STRING:
		(*ep).u.s_typ = (*tp).u.s_typ;
		break;
	case FLIST:
		(*ep).u.fl_typ = (*tp).u.fl_typ;
		break;
	case ILIST:
		(*ep).u.il_typ = (*tp).u.il_typ;
		break;
	}

	return;
}

/*
 * PARSE - parse string for token
 *
 * Parameters:
 *    sp	- pointer to pointer to string to be parsed
 *    tp	- pointer to token
 */

static int parse(sp, tp)
char **sp;
entry *tp;
{
	char *s = *sp;
	char *n = str;
	register int c;
	int relocf = 0;
	float *fp;
	int *ip;
	int i,len;

	/* Pass over blanks, tabs */

loop:
	c = *s;
	while (isspace(c)) c = *++s;

	/* Check for done */

	if (!c) return(DONE);

	/* Check for comment */

	if (c == '#') {
		c = *++s;
		while (c && c != '#') c = *++s;
		if (!c) return(ERROR);
		++s;
		goto loop;
	}

	/* Check for reloc */

	if (c == '@') {
		relocf = 1;
		c = *++s;
	}

	/* Get fortran style identifier */

	if (!isalpha(c)) return(ERROR);

	*n++ = c;
	c = *++s;
	while (isalnum(c) || c == '_') {
		*n++ = c;
		c = *++s;
	}

	*n = '\0';

	tp->name = (char *) malloc(strlen(str)+1);
	strcpy(tp->name,str);

	/* Return if reloc flag set */

	if (relocf) {
		*sp = s;
		return(RELOC);
	}

	/* Pass over blanks, tabs */

	while (isspace(c)) c = *++s;
	if (!c) return(ERROR);

	/* Check for cosmetic = sign */

	if (c != '=') return(ERROR);

	c = *++s;
	while (isspace(c)) c = *++s;
	if (!c) return(ERROR);

	/*
	 * Now the messy part.  We have strings, integers,
	 * reals, integer vectors, and real vectors.
	 */

	switch(c) {
	case '"':
		if (!getstr(&s)) return(ERROR);
		tp->type = STRING;
		(*tp).u.s_typ = (char *) malloc(strlen(str)+1);
		strcpy((*tp).u.s_typ,str);
		break;

	case '{':
		++s;
		switch(getlist(&s,&len)) {
		case FLOAT:
			tp->type = FLIST;
			(*tp).u.fl_typ = fp = (float *) malloc(len*sizeof(float));
			tp->llen = len;
			for (i=0;i<len;++i) *fp++ = nums[i].f_typ;
			break;
		case INTEGER:
			tp->type = ILIST;
			(*tp).u.il_typ = ip = (int *) malloc(len*sizeof(int));
			tp->llen = len;
			for (i=0;i<len;++i) *ip++ = nums[i].i_typ;
			break;
		default:
			return(ERROR);
		}
		break;
	
	case '.': case '-': case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7': case '8': case '9':
		switch(getnum(&s,0)) {
		case FLOAT:
			tp->type = FLOAT;
			(*tp).u.f_typ = nums[0].f_typ;
			break;
		case INTEGER:
			tp->type = INTEGER;
			(*tp).u.i_typ = nums[0].i_typ;
			break;
		default:
			return(ERROR);
		}
		break;
	default:
		return(ERROR);
	}

	*sp = s;
	return(ENTRY);
}

/*
 * HANDLE - handle string with parameter assignments
 *          This routine is recursive and can't use globals.
 *
 * Parameters:
 *    s		- string
 */

static void handle(s)
char *s;
{
	struct stat fs;
	int fd;
	char *rf;
	entry tok;

	for (;;) {
		switch(parse(&s,&tok)) {
		case RELOC:

			if ((fd = open(tok.name,O_RDONLY,0777)) < 0) {
				err("getpar: Couldn't open %s",tok.name);
				break;
			}

			fstat(fd,&fs);
			if (fs.st_size >= MAX_SIZE) {
				err("getpar: File %s too big\n",tok.name);
				break;
			}
			rf = (char *) malloc(fs.st_size+1);
			read(fd,rf,fs.st_size);
			close(fd);
			rf[fs.st_size] = '\0';
			handle(rf);
			free(rf);
			break;
	
		case ENTRY:
			enter(&tok);
			break;

		case DONE:
			return;

		case ERROR:
		default:
			err("getpar: Syntax error: %s",tok.name);
			return;
		}
	}
}

/*
 * INITGETPAR - load parameters
 *
 * Parameters:
 *    argc	- count of arguments
 *    argv	- array of arguments
 */

void initgetpar(argc,argv)
int argc;
char **argv;
{
	int i;
	xargc = argc; xargv = argv; /* For errpkge and askdoc */
	for (i=1;i<argc;++i) handle(argv[i]);
	return;
}

/*
 * GETPAR - get a value from table
 *
 * Parameters:
 *    nam	- name of item
 *    point	- pointer to where to put it
 *                NOTE: if pointers are of different size than
 *                      unsigned, we'll have trouble
 */

int getpar(nam,point)
char *nam;
unsigned point;
{
	int i;
	entry *ep;

	for(i=0;i<entries;++i) {
		ep = table[i];
		if (!strcmp(ep->name,nam)) {
			switch(ep->type) {
			case FLOAT:
				*((float *) point) = (*ep).u.f_typ;
				return(1);
			case INTEGER:
				*((int *) point) = (*ep).u.i_typ;
				return(1);
			case STRING:
				*((char **) point) = (*ep).u.s_typ;
				return(1);
			case FLIST:
				*((float **) point) = (*ep).u.fl_typ;
				return(ep->llen);
			case ILIST:
				*((int **) point) = (*ep).u.il_typ;
				return(ep->llen);
			}
		}
	}

	return(0);
}

#ifdef TRYMAIN
int ia[]={-1,-1,-1};
float fa[]={-1.0,-1.0,-1.0};

main(argc,argv)
int argc;
char **argv;
{

	float f;
	int i,len;
	float *fl;
	int *il;
	char *s;

	initgetpar(argc,argv);

	i = -1;
	getpar("i",&i);
	printf("i = %d\n",i);

	f = -1.0;
	getpar("f",&f);
	printf("f = %f\n",f);

	s = "hello";
	getpar("s",&s);
	printf("s = %s\n",s);

	il = ia;
	len = getpar("i_l",&il);
	printf("i_l[%d] = %d %d %d\n",len,il[0],il[1],il[2]);

	fl = fa;
	len = getpar("f_l",&fl);
	printf("f_l[%d] = %f %f %f\n",len,fl[0],fl[1],fl[2]);

	exit(0);
}
#endif
