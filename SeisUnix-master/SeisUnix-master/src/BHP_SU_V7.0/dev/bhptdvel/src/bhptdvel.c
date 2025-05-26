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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "par.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 					                        ",
" BHPTDV - BHP Velocity Time/Depth Conversion                    ",
" 					                        ",
" BHPTDV  converts velocity traces from time to depth and vice versa",
" 					                        ",
" bhptdv < velin > velout [optional parameters]                 ",
" 					                        ",
" Required Parameters: none		                        ",
" 					                        ",
" Optional Parameters:			                        ",
" verbose=0       Use verbose=1 for long printout               ",
" type=time-depth Conversion type                               ",
"                 Valid types:                                  ",
"                   time-depth  interval-time --> interval-depth",
"                   depth-time  interval-depth --> interval-time",
"                   time-rms    interval-time --> rms           ",
"                   rms-time    rms --> interval-time           ",
"                                                               ",
" ntout=        Number of samples per trace in output           ",
"               If not specified, ntout is set large enough to  ",
"               map all samples in the first input velocity     ",
"               trace.                                          ",
" dt=0.004      Output time interval for depth-time conversion. ",
" dz=20.0       Output depth interval for time-depth conversion.",
" 					                        ",
NULL};

/**************** end self doc ********************************/

int main (int argc, char **argv)
{

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  return EXIT_SUCCESS;

}
