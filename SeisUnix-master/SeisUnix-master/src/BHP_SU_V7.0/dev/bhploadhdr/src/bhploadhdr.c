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
* KEYWORDS:  $RCSfile: bhploadhdr.c,v $
*            $Revision: 1.9 $
*            $Date: 2003/12/09 22:00:59 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhploadhdr.c,v $
* Revision 1.9  2003/12/09 22:00:59  ahmilb
* Added interp parameter.
*
* Revision 1.8  2002/10/03 13:48:33  ahmilb
* Update self-doc to show old-style and new-style parameters.
*
* Revision 1.7  2002/09/26 15:59:32  ahmilb
* Use common souce for bhploadhdr and bhphorizon.
* Upgrade to more than 2 keys and multiple header loads per pass.
*
* Revision 1.6  2002/05/07 18:59:30  ahmilb
* Re-code search algorithm.
*
* Revision 1.4  2001/09/13 18:31:58  ahmilb
* Modify grid_def structure.
* Move grid-building code to build_grid function in libcube/src/interp_lib.c
* Move scalhdr, bilin, and extrapolate functions to libcube/src/interp_lib.c
*
* Revision 1.3  2001/04/18 18:46:22  ahmilb
* Added extrapolation parameter
*
* Revision 1.2  2001/02/06 03:40:41  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhp_interp.h"
#include "bhp_hdr.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPLOADHDR Load Trace Header(s) from ASCII file                       ",
" 									",
" Required Parameters: None                                             ",
" 									",
" Optional parameters:						        ",
" keys=ep,cdp   Headers to use as keys for matching traces to picks file",
" hdrs=d1       Trace headers to which picks/horizons are loaded        ",
"               At least 2 keys must be specified, and any number of    ",
"               headers may be loaded at one time.                      ",
"   OR                                                                  ",
" key=ep,cdp,d1 Old-style specification of keys and single header.      ",
"   OR                                                                  ",
" key=key1,key2,hdr1,hdr2 Old-style specification of keys and 2 headers.",
" file=horz.dat ASCII file containing header key values and pick values ",
" extrap=no     For traces whose keys values are outside the limits of  ",
"               the horizon data in the ASCII file, extrap=no loads a   ",
"               null header value. extrap=yes loads the nearest         ",
"               header value                                            ",
" interp=no     For traces whose key values are within the limits of the",
"               data but for which there is no horizon value,           ",
"               interp=no returns a null horizon value. The value       ",
"               returned for interp=yes depends on the number of keys.    ",
"               For 2 keys, interp=yes returns a bilinearly-iterpolated value.",
"               For >2 keys, interp=yes returns the nearest non-null value.",
" bias=0        The bias value is added to each header value            ",
" scalar=1      Each header value is multiplied by scalar before adding bias.",
" null=-999.25  Value to use as null                                    ",
NULL}; 

int main(int argc, char **argv)
{

  char cmdbuf[BUFSIZ];  /* UNIX command */
  char args[BUFSIZ];    /* command line args */

  FILE *pipefp;         /* command pipe */

  int i;

  segy tr;

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* call bhphorizon */
  strcpy(cmdbuf,"bhphorizon runmode=loadhdr ");
  for(i=1; i<argc; i++) {
    strcat(cmdbuf,argv[i]);
    strcat(cmdbuf," ");
  }
  fprintf(stderr,"Executing %s\n",cmdbuf);
  pipefp = epopen(cmdbuf,"w");
  while(gettr(&tr))
    fputtr(pipefp,&tr);
  epclose(pipefp);

  return EXIT_SUCCESS;
}
