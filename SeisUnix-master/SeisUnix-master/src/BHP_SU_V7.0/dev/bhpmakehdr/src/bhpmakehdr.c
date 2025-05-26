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
* KEYWORDS:  $RCSfile: bhpmakehdr.c,v $
*            $Revision: 1.1 $
*            $Date: 2001/10/12 19:28:57 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
*
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" bhpmakehdr > stdout                                                   ",
" 									",
" bhpmakehdr calls SU modules suaddhead and sushw to construct trace    ",
" headers for C binary or Fortran direct access data.                   ",
" 									",
" Required parameters: none                                             ",
" 									",
" Optional parameters:						        ",
" 									",
" data=data.bin Input data                                              ",
" dt=4000       Sampling interval(micro-seconds)                        ",
" ns=1001       Samples per trace                                       ",
" line=ep       Trace header to use as primary key                      ",
" trace=cdp     Trace header to use as secondary key                    ",
" line1=1       First primary value, e.g. line number                   ",
" trace1=1      First secondary value, e.g. CDP number                  ",
" traces=100    Secondaries per primary, e.g. CDPs per line             ",
"                                                                       ",
NULL}; 

int main(int argc, char **argv)
{

  char *line, *trace;   /* header keys */
  char *data;           /* input data */
  char cmdbuf[BUFSIZ];  /* UNIX command pipe */
  char string[256];     /* scratch */

  int verbose;          /* Debug */
  int dt, ns;           /* sample rate, nsamp */
  int line1, trace1;    /* 1st line, trace numbers */
  int traces;           /* traces per line */

  FILE *pipefp;         /* command pipe */

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(!getparint("dt",&dt))
    dt = 4000;
  if(!getparint("ns",&ns))
    dt = 1001;
  if(!getparstring("line",&line))
    line = "ep";
  if(!getparstring("trace",&trace))
    trace= "cdp";
  if(!getparint("line1",&line1))
    line1 = 1;
  if(!getparint("trace1",&trace1))
    trace1 = 1;
  if(!getparint("traces",&traces))
    traces = 100;
  if(!getparstring("data",&data))
    data = "data.bin";

  if(verbose) {
    fprintf(stderr,"Sampling Interval in micro-seconds: %d\n",dt);
    fprintf(stderr,"Samples per Trace: %d\n",ns);
    fprintf(stderr,"Primary Header: %s\n",line);
    fprintf(stderr,"Secondary Header: %s\n",trace);
    fprintf(stderr,"First Line Number: %d\n",line1);
    fprintf(stderr,"First Trace Number: %d\n",trace1);
    fprintf(stderr,"Traces per Line: %d\n",traces);
    fprintf(stderr,"Input Data: %s\n",data);
  }

  /* write command */
  sprintf(cmdbuf,"makehdr ");
  sprintf(string,"%d",dt);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%d",ns);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%s",line);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%s",trace);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%d",line1);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%d",trace1);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%d",traces);
  strcat(string," ");
  strcat(cmdbuf,string);
  sprintf(string,"%s",data);
  strcat(cmdbuf,string);
  if(verbose == 1)
    fprintf(stderr,"Executing %s\n",cmdbuf);
  pipefp = epopen(cmdbuf,"w");
  epclose(pipefp);

  return EXIT_SUCCESS;
}
