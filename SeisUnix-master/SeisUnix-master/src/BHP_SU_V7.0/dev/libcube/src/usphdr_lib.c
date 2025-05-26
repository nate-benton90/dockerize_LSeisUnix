
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
/******************************************************************
**
*
* KEYWORDS:  $RCSfile: usphdr_lib.c,v $
*            $Revision: 1.3 $
*            $Date: 2002/11/04 21:11:20 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "usp_hdr.h"
#include "hdr.h"
#include "bhp_hdr.h"

void map_su_to_usp_hdr(char *suname, char *uspname, segy *tr, char *uhdr, int value_in, int verbose)
{

  char *type_in;

  float fval;

  int index;
  int value;

  Value hval_in, hval_out;

  if(suname != NULL) {
    /* get su index, type, value */
    index = getindex(suname);
    type_in = hdr[index].type;
    gethval(tr,index,&hval_in);
    /* convert to int */
    value = vtoi(type_in,hval_in);
    /* get usp index, put value */
    hval_out.i = value;
    /* see if header needs scaling */
    if(!strcmp(suname,"sx") || !strcmp(suname,"sy") || !strcmp(suname,"gx") || !strcmp(suname,"gy")) {
      fval = (float)hval_out.i;
      scalhdr(tr,suname,&fval,STORE);
      hval_out.i = (int)fval;
    }
  }
  else
    hval_out.i = value_in;

  /* put value in usphdr */
  index = get_usp_index(uspname,NUSPKEYS,verbose);
  put_usp_hval(uhdr,index,&hval_out,verbose);

}

void make_usp_linehdr(char *linehdr, segy *tr, int verbose)
{

  static float fval=0.000001;

  map_su_to_usp_hdr("dt","SmpInt",tr,linehdr,0,verbose);
  map_su_to_usp_hdr("ns","NumSmp",tr,linehdr,0,verbose);
  map_su_to_usp_hdr("delrt","TmMsFS",tr,linehdr,0,verbose);
  map_su_to_usp_hdr(NULL,"Format",tr,linehdr,3,verbose);
  map_su_to_usp_hdr(NULL,"NumTrc",tr,linehdr,1,verbose);
  map_su_to_usp_hdr(NULL,"NumRec",tr,linehdr,999999,verbose);
  memcpy((short *)linehdr+348,&fval,4);
  map_su_to_usp_hdr(NULL,"HlhEnt",tr,linehdr,0,verbose);
  map_su_to_usp_hdr(NULL,"HlhByt",tr,linehdr,4,verbose);

}

void make_usp_tracehdr(segy *tr, char *tracehdr, int verbose)
{

  int i;

  memset(tracehdr,'\0',USPTRCHDRBYTES);

  for(i=0; i<NUM_SU_USP_NAME; i++)
    map_su_to_usp_hdr(su_usp_name[i].su_name,su_usp_name[i].usp_name,tr,tracehdr,0,verbose);

}

/* hacked versions of SU puthval and getindex which take header
   structure as an argument, layout must be same as hdr.h, i.e. name,format,offset
*/
void put_usp_hval(char *tr, int index, Value *valp, int verbose)
{
  
  char *tp = (char*) tr;

  switch(*(usphdr[index].type)) {
    case 'c': (void) strncpy(tp + usphdr[index].offs, valp->s, 1);  break;
    case 's': (void) strcpy(tp + usphdr[index].offs, valp->s);  break;
    case 'h': valp->h = valp->i; *((short*)  (tp + usphdr[index].offs)) = valp->h; break;
    case 'u': valp->u = valp->i; *((unsigned short*) (tp + usphdr[index].offs)) = valp->u; break;
    case 'i': *((int*)   (tp + usphdr[index].offs)) = valp->i; break;
    case 'p': *((unsigned int*)   (tp + usphdr[index].offs)) = valp->p; break;
    case 'l': *((long*)   (tp + usphdr[index].offs)) = valp->l; break;
    case 'v': *((unsigned long*)  (tp + usphdr[index].offs)) = valp->v; break;
    case 'f': valp->f = valp->i; *((float*)  (tp + usphdr[index].offs)) = valp->f; break;
    case 'd': *((double*) (tp + usphdr[index].offs)) = valp->d; break;
    default: err("%s: %s: mysterious data type", __FILE__,__LINE__); break;
  }

  return;
}

int get_usp_index(char *key, int nkeys, int verbose)
{
  register int i;

  for(i=0; i<nkeys; i++)
    if (STREQ(usphdr[i].key,key))
      return i;       /* key found */
    /* not found */
    return -1;
}

void map_usp_to_su_hdr(char *uspname, char *suname, char *uhdr, segy *tr, int value_in, int verbose)
{

  char *type_in;

  int index;
  int value;

  Value hval_in, hval_out;

  /* usp index, value */
  index = get_usp_index(uspname,NUSPKEYS,verbose);
  get_usp_hval(uhdr,index,&hval_in,verbose);
  type_in = usphdr[index].type;
  /* convert to int */
  value = vtoi(type_in,hval_in);

  /* get su index, type, write value */
  index = getindex(suname);
  /* set proper type into hval_out */
  if(!strcmp(hdr[index].type,"i"))
    hval_out.i = value;
  else if(!strcmp(hdr[index].type,"h"))
    hval_out.h = value;
  else if(!strcmp(hdr[index].type,"f"))
    hval_out.f = value;
  else if(!strcmp(hdr[index].type,"u"))
    hval_out.u = value;

  puthval(tr,index,&hval_out);

}

void make_su_tracehdr(char *tracehdr, segy *tr, int verbose)
{

  int i;

  for(i=0; i<NUM_SU_USP_NAME; i++)
    map_usp_to_su_hdr(su_usp_name[i].usp_name,su_usp_name[i].su_name,tracehdr,tr,0,verbose);

}

void get_usp_hval(char *tr, int index, Value *valp, int verbose)
{
	char *tp = (char*) tr;

	switch(*(usphdr[index].type)) {
	case 's': (void) strcpy(valp->s, tp + usphdr[index].offs);  break;
	case 'h': valp->h = *((short*)  (tp + usphdr[index].offs)); break;
	case 'u': valp->u = *((unsigned short*) (tp + usphdr[index].offs)); break;
	case 'i': valp->i = *((int*)   (tp + usphdr[index].offs)); break;
	case 'p': valp->p = *((unsigned int*)   (tp + usphdr[index].offs)); break;
	case 'l': valp->l = *((long*)   (tp + usphdr[index].offs)); break;
	case 'v': valp->v = *((unsigned long*)  (tp + usphdr[index].offs)); break;
	case 'f': valp->f = *((float*)  (tp + usphdr[index].offs)); break;
	case 'd': valp->d = *((double*) (tp + usphdr[index].offs)); break;
	default: err("%s: %s: mysterious data type", __FILE__,__LINE__); break;
	}

	return;
}
