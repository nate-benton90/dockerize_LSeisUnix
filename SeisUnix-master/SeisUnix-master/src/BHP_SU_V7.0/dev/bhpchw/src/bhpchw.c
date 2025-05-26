/*

LICENSE FOR BHP SU Suite of Programs

The following is the license that applies to the copy of the software hereby
provided to Licensee. BHP's Software Manager may be contacted at the following
address:

Colorado School of Mines
1500 Illinois Street
Golden, Colorado 80401
Attention: John Stockwell
e-mail: john@dix.mines.edu
Telephone: 303-273-3049

Copyright 2001 BHP Software. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software") to deal
in the Software, without restriction, except as hereinafter provided,
including without limitation the rights to use, copy, modify merge,
publish, and distribute the Software and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice and this permission notice appear in all copies of the
Software and that both the above copyright notice and this permission
notice appear in supporting documentation. No charge may be made for
any redistribution of the Software, including modified or merged versions
of the Software. The complete source code must be included
in any distribution. For an executable file, complete source code means the
source code for all modules it contains.

Modified or merged versions of the Software must be provided to the Software
Manager, regardless of whether such modified or merged versions are
distributed to others.

THE SOFTWARE IS PROVIDED 'AS IS" WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE
COPYRIGHT HOLDER INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM OR
ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OF PERFORMANCE OF
THIS SOFTWARE.

The name of the copyright holder shall not be used in advertising or
otherwise to promote the use or other dealings in this Software, without
prior written consent of the copyright holder.

*/
/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

/* SUCHW: $Revision: 1.1 $ ; $Date: 2002/07/29 14:29:44 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"									",
" SUCHW - Change Header Word using one or two header word fields	",
"									",
"  suchw <stdin >stdout [optional parameters]				",
"									",
" Required parameters:							",
" none									",
"									",
" Optional parameters:							",
" key1=cdp,...	output key(s) 						",
" key2=cdp,...	input key(s) 						",
" key3=cdp,...	input key(s)  						",
" a=0,...		overall shift(s)				",
" b=1,...		scale(s) on first input key(s) 			",
" c=0,...		scale on second input key(s) 			",
" d=1,...		overall scale(s)				",
"									",
" The value of header word key1 is computed from the values of		",
" key2 and key3 by:							",
"									",
"	val(key1) = (a + b * val(key2) + c * val(key3)) / d		",
"									",
" Examples:								",
" Shift cdp numbers by -1:						",
"	suchw <data >outdata a=-1					",
"									",
" Add 1000 to tracr value:						",
" 	suchw key1=tracr key2=tracr a=1000 <infile >outfile		",
"									",
" We set the receiver point (gx) field by summing the offset		",
" and shot point (sx) fields and then we set the cdp field by		",
" averaging the sx and gx fields (we choose to use the actual		",
" locations for the cdp fields instead of the conventional		",
" 1, 2, 3, ... enumeration):						",
"									",
"   suchw <indata key1=gx key2=offset key3=sx b=1 c=1 |			",
"   suchw key1=cdp key2=gx key3=sx b=1 c=1 d=2 >outdata			",
"									",
" Do both operations in one call:					",
"									",
" suchw<indata key1=gx,cdp key2=offset,gx key3=sx,sx b=1,1 c=1,1 d=1,2 >outdata",
"									",
NULL};

/* Credits:
 *	SEP: Einar Kjartansson
 *	CWP: Jack K. Cohen
 *      CWP: John Stockwell, 7 July 1995, added array of keys feature
 *      Delphi: Alexander Koek, 6 November 1995, changed calculation so
 *              headers of different types can be expressed in each other
 */
/**************** end self doc ***********************************/

/* prototype for function used internally */
void changeval(cwp_String type1, Value *valp1, cwp_String type2,
	       Value *valp2, cwp_String type3, Value *valp3,
		double a, double b, double c, double d);
void getbytes(int *hdr, int offset, int len, cwp_String type, Value *val);

segy tr;

int
main(int argc, char **argv)
{
	cwp_String key1[SU_NKEYS];	/* output key(s)		*/
	cwp_String key2[SU_NKEYS];	/* first input key(s)		*/
	cwp_String key3[SU_NKEYS];	/* second input key(s)		*/
	cwp_String type1[SU_NKEYS];	/* array of types for key1	*/
	cwp_String type2[SU_NKEYS];	/* array of types for key2	*/
	cwp_String type3[SU_NKEYS];	/* array of types for key3	*/
	int nkeys;			/* number of keys to be computed*/
	int n;				/* counter of keys getparred	*/
	int ikey;			/* loop counter of keys 	*/
	int index1[SU_NKEYS];		/* array of indexes for key1 	*/
	int index2[SU_NKEYS];		/*      ....        for key2	*/
	int index3[SU_NKEYS];		/*      ....        for key3	*/

	Value val1;			/* value of key1		*/
	Value val2;			/* value of key2		*/
	Value val3;			/* value of key3		*/

	double *a;			/* array of "a" values		*/
	double *b;			/* array of "b" values		*/
	double *c;			/* array of "c" values		*/
	double *d;			/* array of "d" values		*/

  /* allow all valid SU types for key2, and key3 instead of what's in segy.h; in addition, allow
     access to any header byte via key=byte, offset=n(0-239), and len=n(1-8 bytes) */
  cwp_String key2_type[SU_NKEYS];
  cwp_String key3_type[SU_NKEYS];
  int key2_offset[SU_NKEYS];
  int key3_offset[SU_NKEYS];
  int key2_len[SU_NKEYS];
  int key3_len[SU_NKEYS];
  int nb2=0, nb3=0;  /* number of "byte" entries for key2, key3 */
  int ntype2=0, ntype3=0;  /* number of key_type entries for key2, key3 */
  int nlen2=0, nlen3=0;  /* number of key_len entries for key2, key3 */
  int noff2=0, noff3=0;  /* number of key_offset entries for key2, key3 */
  int i;
  int verbose;

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

  if(!getparint("verbose",&verbose))
    verbose = 0;

	/* Get parameters */
	/* get key1's */
	if ((n=countparval("key1"))!=0){
		nkeys=n;
		getparstringarray("key1",key1);
	} else { /* set default */
		nkeys=1;
		key1[0]="cdp";	
	}

	/* get key2's */
	if ((n=countparval("key2"))!=0){
		if (n!=nkeys)
			err("number of key2's and key1's must be equal!");

		getparstringarray("key2",key2);
	} else { /* set default */
		key2[0]="cdp";	
	}

	/* get key3's */
	if ((n=countparval("key3"))!=0){
		if (n!=nkeys)
			err("number of key3's and key1's must be equal!");

		getparstringarray("key3",key3);
	} else { /* set default */
		key3[0]="cdp";	
	}

	/* get a's */
	if ((n=countparval("a"))!=0){
		if (n!=nkeys)
			err("number of a's and key1's must be equal!");

		a=ealloc1double(nkeys);
		getpardouble("a",a);
	} else { /* set default */
		a=ealloc1double(nkeys);
		for (ikey=0; ikey<nkeys; ++ikey)
			a[ikey]=0.;
	}

	/* get b's */
	if ((n=countparval("b"))!=0){
		if (n!=nkeys)
			err("number of b's and key1's must be equal!");

		b=ealloc1double(nkeys);
		getpardouble("b",b);
	} else { /* set default */
		b=ealloc1double(nkeys);
		for (ikey=0; ikey<nkeys; ++ikey)
			b[ikey]=1.;
	}

	/* get c's */
	if ((n=countparval("c"))!=0){
		if (n!=nkeys)
			err("number of c's and key1's must be equal!");

		c=ealloc1double(nkeys);
		getpardouble("c",c);
	} else { /* set default */
		c=ealloc1double(nkeys);
		for (ikey=0; ikey<nkeys; ++ikey)
			c[ikey]=0.;
	}

	/* get d's */
	if ((n=countparval("d"))!=0){
		if (n!=nkeys)
			err("number of d's and key1's must be equal!");

		d=ealloc1double(nkeys);
		getpardouble("d",d);
	} else { /* set default */
		d=ealloc1double(nkeys);
		for (ikey=0; ikey<nkeys; ++ikey)
			d[ikey]=1.;
	}

  /* count number of byte keys for key2, key3 */
  for(i=0; i<nkeys; i++) {
    if(!strcmp(key2[i],"byte"))
      nb2++;
    if(!strcmp(key3[i],"byte"))
      nb3++;
  }
  /* number of type, len, offset entries */
  ntype2 = countparval("key2_type");
  ntype3 = countparval("key3_type");
  nlen2 = countparval("key2_len");
  nlen3 = countparval("key3_len");
  noff2 = countparval("key2_offset");
  noff3 = countparval("key3_offset");
  if(ntype2 != 0 && ntype2 != nkeys)
    err("number of key2_type entries must equal total number of keys\n");
  else if(ntype2 != 0)
    getparstringarray("key2_type",key2_type);
  else {
    if(!strcmp(key2[0],"byte"))
      key2_type[0] = "b";
    else
      key2_type[0] = hdtype(key2[0]);
  }
  if(ntype3 != 0 && ntype3 != nkeys)
    err("number of key3_type entries must equal total number of keys\n");
  else if(ntype3 != 0)
    getparstringarray("key3_type",key3_type);
  else {
    if(!strcmp(key3[0],"byte"))
      key3_type[0] = "b";
    else
      key3_type[0] = hdtype(key3[0]);
  }
  if(nlen2 != nb2)
    err("Must have equal number of key2 byte keys and key2_len entries\n");
  else if(nlen2 != 0)
    getparint("key2_len",key2_len);
  if(nlen3 != nb3)
    err("Must have equal number of key3 byte keys and key3_len entries\n");
  else if(nlen3 != 0)
    getparint("key3_len",key3_len);
  if(noff2 != nb2)
    err("Must have equal number of key2 byte keys and key2_offset entries\n");
  else if(noff2 != 0)
    getparint("key2_offset",key2_offset);
  if(noff3 != nb3)
    err("Must have equal number of key3 byte keys and key3_offset entries\n");
  else if(noff3 != 0)
    getparint("key3_offset",key3_offset);
  if(verbose) {
    for(i=0; i<nkeys; i++) {
      fprintf(stderr,"KEY2: %s, TYPE: %s, ",key2[i],key2_type[i]);
      if(!strcmp(key2[i],"byte"))
        fprintf(stderr,"Offset %d, LEN: %d\n",key2_offset[i],key2_len[i]);
      else
        fprintf(stderr,"\n");
    }
  }

	for (ikey=0; ikey<nkeys; ++ikey) {
			
		/* get types and index values */
		type1[ikey]  = hdtype(key1[ikey]);
/*
		type2[ikey]  = hdtype(key2[ikey]);
		type3[ikey]  = hdtype(key3[ikey]);
*/
  type2[ikey]  = key2_type[ikey];
  type3[ikey]  = key3_type[ikey];

		index1[ikey] = getindex(key1[ikey]);
                if(strcmp(key2[ikey],"byte"))
                  index2[ikey] = getindex(key2[ikey]);
                if(strcmp(key3[ikey],"byte"))
                  index3[ikey] = getindex(key3[ikey]);
		}

	/* loop over traces */
	while (gettr(&tr)) {

		/* loop over key fields */
		for (ikey=0; ikey<nkeys; ++ikey) {
			
			/* get header values */
                        if(strcmp(key2[ikey],"byte"))
                          gethval(&tr, index2[ikey], &val2);
                        else
                          getbytes((int *)&tr,key2_offset[ikey],key2_len[ikey],key2_type[ikey],&val2);
                        if(strcmp(key3[ikey],"byte"))
			  gethval(&tr, index3[ikey], &val3);
                        else
                          getbytes((int *)&tr,key3_offset[ikey],key3_len[ikey],key3_type[ikey],&val3);

			changeval(type1[ikey], &val1, type2[ikey], &val2,
				type3[ikey], &val3, a[ikey], b[ikey], c[ikey],
				d[ikey]);
			puthval(&tr, index1[ikey], &val1);
		}
		puttr(&tr);
	}

	return EXIT_SUCCESS;
}


void changeval(cwp_String type1, Value *valp1, cwp_String type2,
	       Value *valp2, cwp_String type3, Value *valp3,
		double a, double b, double c, double d)
{
	double dval2=vtod( type2, *valp2);
	double dval3=vtod( type3, *valp3);
	double dval1=(a+b*dval2+c*dval3)/d;

	switch (*type1) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		valp1->h = (short) dval1;
	break;
	case 'u':
		valp1->u = (unsigned short) dval1;
	break;
	case 'l':
		valp1->l = (long) dval1;
	break;
	case 'v':
		valp1->v = (unsigned long) dval1;
	break;
	case 'i':
		valp1->i = (int) dval1;
	break;
	case 'p':
		valp1->p = (unsigned int) dval1;
	break;
	case 'f':
		valp1->f = (float) dval1;
	break;
	case 'd':
		valp1->d = (double) dval1;
	break;
	default:
		err("unknown type %s", type1);
	break;
	}
}

void getbytes(int *tr, int offset, int len, cwp_String type, Value *val)
{

  int i;

  float f;

  if(offset < 0 || offset >= HDRBYTES)
    err("%d is illegal byte offset\n",offset);

  /* temporary - require 4 bytes */
  if(len != 4)
    err("%d is illegal key_len\n",len);
  if(len < 1 || len > 8)
    err("%d is illegal key_len\n",len);
  
  if(!strcmp(type,"i"))
    memcpy(val,&tr[offset/sizeof(int)],sizeof(int));
  else if(!strcmp(type,"f"))
    memcpy(val,&tr[offset/sizeof(float)],sizeof(float));
}
