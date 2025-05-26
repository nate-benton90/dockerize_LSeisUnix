
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
* KEYWORDS:  $RCSfile: hdr_lib.c,v $
*            $Revision: 1.1 $
*            $Date: 2001/09/13 20:02:41 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: hdr_lib.c,v $
* Revision 1.1  2001/09/13 20:02:41  ahmilb
* Moved setval1 and scalhdr functions to hdr_lib
*
* Revision 1.3  2001/02/07 19:55:47  ahglim
* added bhproffread, bhptemplate
* changed bhpio, bhpread, bhpwrite to handle ENDIAN issues
*
* Revision 1.2  2001/02/06 02:38:22  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhp_hdr.h"

void setval1(cwp_String type, Value *valp, double a)
{

  switch (*type) {
    case 's':
      err("can't set char header word");
      break;
    case 'h':
      valp->h = a;
      break;
    case 'u':
      valp->u = a;
      break;
    case 'l':
      valp->l = (long)a;
      break;
    case 'v':
      valp->v = (unsigned long)a;
      break;
    case 'i':
      valp->i = a;
      break;
    case 'p':
      valp->p = a;
      break;
    case 'f':
      valp->f = a;
      break;
    case 'd':
      valp->d = a;
      default:
    err("unknown type %s", type);
    break;
  }

  return;

}

void scalhdr(segy *tr, cwp_String key, float *val, int dir)
{

  float factor;

  if( (!strcmp(key,"sx")) || (!strcmp(key,"sy")) || (!strcmp(key,"gx")) || (!strcmp(key,"gy")))
    factor = (!tr->scalco) ? 1 : tr->scalco;
  else if((!strcmp(key,"gelev")) || (!strcmp(key,"selev")) || (!strcmp(key,"sdepth")) ||
          (!strcmp(key,"gdel")) || (!strcmp(key,"sdel")) || (!strcmp(key,"swdep")) ||
          (!strcmp(key,"gwdep")))
    factor = (!tr->scalel) ? 1 : tr->scalel;
  else
    return;

  if(dir == LOAD)
    if (factor < 0)
      factor = -1./factor;
  if(dir == STORE) {
    if (factor < 0)
      factor = -factor;
    else
      factor = 1./factor;
  }
  *val *= factor;

}
