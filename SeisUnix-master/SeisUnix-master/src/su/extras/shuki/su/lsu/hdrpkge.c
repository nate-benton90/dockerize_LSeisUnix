/* hdrpkge - routines to access the SEGY header via the hdr structure.
 *
 * gethval - get a header word by index
 * puthval - put a header word by index
 * gethdval - get a header word by name
 * puthdval - put a header word by name
 * hdtype   - get the data type of a header word by name
 * getkey   - get the name of a header from its index
 * getindex - get the index from the name
 *
 * Returns:
 *	gethval : void
 *	puthval : void
 *	gethdval: void
 *	puthdval: void
 *	hdtype  : pointer to a string designating the data type
 *	getkey  : pointer to a string with the name of the keyword (or NULL)
 *	getindex: integer index
 *
 * Synopsis:
 *	void gethval(trace_ptr, index, val_ptr)
 *	char *trace_ptr;	pointer to Sutrace
 *	int index;		index of key name in SEGY header
 *	value *val_ptr;		union of simple data types
 *
 *	void puthval(trace_ptr, index, val_ptr)
 *	char *trace_ptr;	pointer to Sutrace
 *	int index;		index of key name in SEGY header
 *	value *val_ptr;		union of simple data types
 *
 *	void gethdval(trace_ptr, key, val_ptr)
 *	char *trace_ptr;	pointer to Sutrace
 *	char *key;		key name in SEGY header
 *	value *val_ptr;		union of simple data types
 *
 *	void puthdval(trace_ptr, key, val_ptr)
 *	char *trace_ptr;	pointer to Sutrace
 *	char *key;		key name in SEGY header
 *	value *val_ptr;		union of simple data types
 *
 *	char *hdtype(key)
 *	char *key;		key name in SEGY header
 *
 *	char *getkey(index)
 *	int index;		zero-based index of keyword
 *
 *	int getindex(key)
 *	char *key;		key name in SEGY header
 *
 * Credits:
 *	SEP: Einar
 *	CWP: Jack, Shuki
 *
 * Notes:
 *	This package includes only those routines that directly access
 *	the "hdr" (formerly "kdat") structure.  In particular, it does
 *	not include routines such as printfval, printftype, printfhead
 *	that use the routines in this package to indirectly access the
 *	hdr structure.
 *
 *      Note that while gethdval and puthdval are more convenient to use
 *	than gethval and puthval, they incur an inefficiency in the
 *	common case of iterating code over a set of traces with a fixed
 *      key or keys.  In such cases, it is advisable to set the index
 *      or indices outside the loop using getindex.
 *
 *	Some history:
 *	In the original SEP package, it was necessary to call lookey
 *	(the old name for getindex) before using other elements of the
 *	package since lookey had the job of initializing the table of
 *	offsets (even though lookey, itself, did not use the table).
 *
 *	The SEP documentation and code often implied that lookey
 *	and gkey used the offset in bytes in the Sutrace header, actually
 *	they dealt with the index.
 *
 *	The shared "hdr" structure was formerly called "kdat" and the
 *	associated array was called "kd".
 *
 */

#include <stdio.h>

#include "../include/su.h"
#include "../include/hdrs.h"


void gethval(tr, index, valp)
Sutrace *tr;
int index;
value *valp;
{
	char *tp;

	tp = (char*) tr;

	switch(*(trhdr[index].type)) {
	case 'c':
		strcpy(valp->s, tp + trhdr[index].offs);
	break;
	case 'i':
		valp->i = *((int*)(tp + trhdr[index].offs));
	break;
	case 'f':
		valp->f = *((float*)(tp + trhdr[index].offs));
	break;
	default:
		err(__FILE__,__LINE__,"gethval: mysterious data type");
	break;
	}
}

void getbhval(abh, index, valp)
Sutrace *abh;
int index;
value *valp;
{
	char *tp;

	tp = (char*) abh;

	switch(*(bhdr[index].type)) {
	case 'c':
		strcpy(valp->s, tp + bhdr[index].offs);
	break;
	case 'i':
		valp->i = *((int*)(tp + bhdr[index].offs));
	break;
	case 'f':
		valp->f = *((float*)(tp + bhdr[index].offs));
	break;
	default:
		err(__FILE__,__LINE__,"getbhval: mysterious data type");
	break;
	}
}


void puthval(tr, index, valp)
Sutrace *tr;
int index;
value *valp;
{
	char *tp;

	tp = (char*) tr;

	switch(*(trhdr[index].type)) {
	case 'c':
		strcpy(tp + trhdr[index].offs, valp->s);
	break;
	case 'i':
		*((int*) (tp + trhdr[index].offs)) = valp->i;
	break;
	case 'f':
		*((float*) (tp + trhdr[index].offs)) = valp->f;
	break;
	default:
		err(__FILE__,__LINE__,"puthval: mysterious data type");
	break;
	}
}


void gethdval(tr, key, valp)
Sutrace *tr;
char *key;
value *valp;
{
	int index;
	char *tp;

	tp = (char*) tr;

	if ( -1 == (index = getindex(key)))
		err(__FILE__,__LINE__,"gethdval: illegal key word '%s'", key);

	switch(*(trhdr[index].type)) {
	case 'c':
		strcpy(valp->s, tp + trhdr[index].offs);
	break;
	case 'i':
		valp->i = *((int*)(tp + trhdr[index].offs));
	break;
	case 'f':
		valp->f = *((float*)(tp + trhdr[index].offs));
	break;
	default:
		err(__FILE__,__LINE__,"gethdval: mysterious data type");
	break;
	}
}


void puthdval(tr, key, valp)
Sutrace *tr;
char *key;
value *valp;
{
	int index;
	char *tp;

	tp = (char*) tr;

	if ( -1 == (index = getindex(key)))
		err(__FILE__,__LINE__,"puthdval: illegal key word '%s'", key);

	switch(*(trhdr[index].type)) {
	case 'c':
		strcpy(tp + trhdr[index].offs, valp->s);
	break;
	case 'i':
		*((int*) (tp + trhdr[index].offs)) = valp->i;
	break;
	case 'f':
		*((float*) (tp + trhdr[index].offs)) = valp->f;
	break;
	default:
		err(__FILE__,__LINE__,"puthdval: mysterious data type");
	break;
	}
}


char *hdtype(key)
char *key;
{
	int index;

	if ( -1 == (index = getindex(key))) {
		err(__FILE__,__LINE__,"hdtype: illegal key word '%s'", key);
	}

	return(trhdr[index].type);
}


char *getkey(index)
int index;
{
	if (index < TR_NK && index >= 0) {
		return(trhdr[index].key);
	} else {
		return((char *) NULL);
	}
}


int getindex(key)	/* get index for this key */
char *key;
{
	int i;

	for (i = 0; i < TR_NK; i++)
		if (!strcmp(trhdr[i].key, key))
			return(i);		/* key found */

	/* not found */
	return(-1);
}

hdpr(pfd, atr)
Sutrace *atr;
FILE *pfd;
{
	int i,j;
	value val;
	char *type,*key;

/* 	j = 0 ; */
	for (i=0;i<TR_NK;i++ ) {
		gethval(atr, i, &val);
		key = getkey(i);
		type = hdtype(key);
/* 		if(valcmp(type,val,0)) { Sun 4 aborts because 0 is not a value union */
		if(val.i) {
			fprintf(pfd," %s=",getkey(i));
			fprintfval(pfd,type,val);
/* 			if( (++j % 6 ) == 0 ) fprintf(pfd,"\n"); */
		}
	}
	fprintf(pfd,"\n");
}

bhpr(pfd,abh)
Subhed *abh;
FILE *pfd;
{
	int i;
	value val;
	char *type,*key;

/* 	fprintf(pfd,"bhpr() not ready\n"); */

	for(i=0;i<BH_NK;i++) {
		getbhval(abh,i,&val);
		if(val.i) {
			fprintf(pfd," %s=",bhdr[i].key);
			fprintfval(pfd,bhdr[i].type,val);
		}
	}
	fprintf(pfd,"\n");
}
