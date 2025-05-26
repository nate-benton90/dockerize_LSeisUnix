/*

The FreeUSP License
-------------------

copyright 2001, Amoco Production Company
All rights reserved
an affiliate of BP America Inc.

IF YOU DO NOT WISH TO BE BOUND BY THESE TERMS AND CONDITIONS, YOU SHOULD NOT ACCESS OR USE THIS SITE.
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:


 - Redistribution of source code must retain the copyright notice and this list of conditions in a location that will be easily located by any user.

 - Redistributions in binary form must reproduce the copyright notice and this list of conditions in the documentation and/or other materials provided with the distribution.

 - Neither the name of Amoco Production Company (APC) nor the names of any of its affiliated companies or its contributors may be used, directly or indirectly, to endorse or promote products or services derived from any FreeUSP program(s) without specific prior written permission.

 - Any use of any FreeUSP software for profit must display the following acknowledgement: 

 "This product includes software developed by BP America Inc. and its contributors."

 - FREEUSP SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS".  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MECHANTABILITY, NON-INFRINGEMENT, TITLE AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APC OR ANY CONTRIBUTOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION, OR LOSS OF PROSPECTIVE ECONOMIC ADVANTAGE) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT ( INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBLITY OF SUCH DAMAGE.  BECAUSE SOME STATES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE LIMITATION MAY NOT APPLY TO YOU.  IN SUCH STATES, LIABILITY MUST BE LIMITED TO THE GREATEST EXTENT PERMITTED BY LAW. 

 - This Site is administered by BP from its offices in Houston, Harris County, Texas. BP makes no representation that materials or services at this Site are appropriate or available for use outside the United States, and access to them from territories where their contents are illegal is prohibited. You may not use or export or re-export the materials or services at this Site or any copy or adaptation in violation of any applicable laws or regulations [including without limitation United States export laws and regulations]. If you choose to access this Site from outside the United States, you do so on your own initiative and are responsible for compliance with applicable local laws. THESE TERMS WILL BE GOVERNED BY AND CONSTRUED IN ACCORDANCE WITH THE LAWS OF THE STATE OF TEXAS, WITHOUT GIVING EFFECT TO ANY CHOICE OF LAW RULES WHICH MAY DIRECT THE APPLICATION OF THE LAWS OF ANY OTHER JURISDICTION.

 - You may not use or otherwise export or reexport FreeUSP software except as authorized by United States law.  In particular, but without limitation, FreeUSP software may not be exported or reexported (i) into (or to a national or resident of) any U.S. embargoed country or (ii) to anyone on the U.S. Treasury Department's list of Specially Designated Nationals or the U.S. Department of Commerce's Table of Denial Orders.  By using FreeUSP software, you represent and warrant that you are not located in, under control of, or a national or resident of any such country or on any such list.  Further information  is available at http://www.bxa.doc.gov.  You have the responsibility to obtain any licenses to export, re-export or import as may be required by law.


 - You agree that APC does not make any representation or warranty that any single program (including subroutines), or any combination of FreeUSP programs, or any combination of a FreeUSP program with any other software, or any part of any program(s) (including the output of a program), when executed, will not infringe any patent in any country.  
 
 - You agree that if any single program (including subroutines), or any combination of FreeUSP programs, or any combination of a FreeUSP program with any other software, or any part of any such program(s) (including the output of a program), when executed, performs the steps of any patented process or product that is owned by APC (including its parent and any subsidiary) or licensed to APC, the availability of such program(s) or part from this website must not be construed or interpreted by you as permission by APC or the grant by APC of a license or immunity to perform or use such patented process or make, user, sell, or import such potential product.  In other words, access to FreeUSP is not the same as the grant of a patent license by the owner of that patent.

 - YOU AGREE THAT APC HAS NO OBLIGATION TO MAINTAIN, SUPPORT OR CORRECT ERRORS.

 - You further agree that you are not under any obligation to make any improvement, modification, repair, enhancement or derivative work to any FreeUSP program downloaded by you.  If you make any improvement, modification, repair, enhancement or derivative work (hereinafter "New Work"), you are not obligated to notify APC or to offer that New Work for distribution through this website.  APC has the complete discretion to accept or reject any offer by you to add your New Work to this website.  APC may reject a New Work for any reason or no reason at all.  APC hopes that you would offer any New Work for others to use with no conditions or with conditions similar to those listed above.  You agree to promptly notify us if any information that you submit is inaccurate, incomplete, or incorrect.

 - APC reserves the right to make any change to these terms and conditions and to any part of any FreeUSP program, including removal of such program from the website.  

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
#include "su.h"
#include "segy.h"
#include "header.h"
#include "usp_hdr.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"The FreeUSP License",
"-------------------",
"",
"copyright 2001, Amoco Production Company",
"All rights reserved",
"an affiliate of BP America Inc.",
"",
"IF YOU DO NOT WISH TO BE BOUND BY THESE TERMS AND CONDITIONS, YOU SHOULD NOT ACCESS OR USE THIS SITE.",
" ",
"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:",
"",
"",
" - Redistribution of source code must retain the copyright notice and this list of conditions in a location that will be easily located by any user.",
"",
" - Redistributions in binary form must reproduce the copyright notice and this list of conditions in the documentation and/or other materials provided with the distribution.",
"",
" - Neither the name of Amoco Production Company (APC) nor the names of any of its affiliated companies or its contributors may be used, directly or indirectly, to endorse or promote products or services derived from any FreeUSP program(s) without specific prior written permission.",
"",
" - Any use of any FreeUSP software for profit must display the following acknowledgement: ",
"",
" \"This product includes software developed by BP America Inc. and its contributors.\"",
"",
" - FREEUSP SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\".  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MECHANTABILITY, NON-INFRINGEMENT, TITLE AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APC OR ANY CONTRIBUTOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION, OR LOSS OF PROSPECTIVE ECONOMIC ADVANTAGE) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT ( INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBLITY OF SUCH DAMAGE.  BECAUSE SOME STATES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE LIMITATION MAY NOT APPLY TO YOU.  IN SUCH STATES, LIABILITY MUST BE LIMITED TO THE GREATEST EXTENT PERMITTED BY LAW. ",
"",
" - This Site is administered by BP from its offices in Houston, Harris County, Texas. BP makes no representation that materials or services at this Site are appropriate or available for use outside the United States, and access to them from territories where their contents are illegal is prohibited. You may not use or export or re-export the materials or services at this Site or any copy or adaptation in violation of any applicable laws or regulations [including without limitation United States export laws and regulations]. If you choose to access this Site from outside the United States, you do so on your own initiative and are responsible for compliance with applicable local laws. THESE TERMS WILL BE GOVERNED BY AND CONSTRUED IN ACCORDANCE WITH THE LAWS OF THE STATE OF TEXAS, WITHOUT GIVING EFFECT TO ANY CHOICE OF LAW RULES WHICH MAY DIRECT THE APPLICATION OF THE LAWS OF ANY OTHER JURISDICTION.",
"",
" - You may not use or otherwise export or reexport FreeUSP software except as authorized by United States law.  In particular, but without limitation, FreeUSP software may not be exported or reexported (i) into (or to a national or resident of) any U.S. embargoed country or (ii) to anyone on the U.S. Treasury Department's list of Specially Designated Nationals or the U.S. Department of Commerce's Table of Denial Orders.  By using FreeUSP software, you represent and warrant that you are not located in, under control of, or a national or resident of any such country or on any such list.  Further information  is available at http://www.bxa.doc.gov.  You have the responsibility to obtain any licenses to export, re-export or import as may be required by law.",
"",
"",
" - You agree that APC does not make any representation or warranty that any single program (including subroutines), or any combination of FreeUSP programs, or any combination of a FreeUSP program with any other software, or any part of any program(s) (including the output of a program), when executed, will not infringe any patent in any country.  ",
" ",
" - You agree that if any single program (including subroutines), or any combination of FreeUSP programs, or any combination of a FreeUSP program with any other software, or any part of any such program(s) (including the output of a program), when executed, performs the steps of any patented process or product that is owned by APC (including its parent and any subsidiary) or licensed to APC, the availability of such program(s) or part from this website must not be construed or interpreted by you as permission by APC or the grant by APC of a license or immunity to perform or use such patented process or make, user, sell, or import such potential product.  In other words, access to FreeUSP is not the same as the grant of a patent license by the owner of that patent.",
"",
" - YOU AGREE THAT APC HAS NO OBLIGATION TO MAINTAIN, SUPPORT OR CORRECT ERRORS.",
"",
" - You further agree that you are not under any obligation to make any improvement, modification, repair, enhancement or derivative work to any FreeUSP program downloaded by you.  If you make any improvement, modification, repair, enhancement or derivative work (hereinafter \"New Work\"), you are not obligated to notify APC or to offer that New Work for distribution through this website.  APC has the complete discretion to accept or reject any offer by you to add your New Work to this website.  APC may reject a New Work for any reason or no reason at all.  APC hopes that you would offer any New Work for others to use with no conditions or with conditions similar to those listed above.  You agree to promptly notify us if any information that you submit is inaccurate, incomplete, or incorrect.",
"",
" - APC reserves the right to make any change to these terms and conditions and to any part of any FreeUSP program, including removal of such program from the website.  ",
" BHPUSP2SU - Convert USP data to SU data                               ",
"  bhpusp2su < uspdata > sudata                                         ",
"  Required parameters: none                                            ",
"  Optional parameters:                                                 ",
"  verbose=0     Use 1 to see header mapping                            ",
"									",
"									",
NULL};

#define LINEHEADER 0

int main(int argc, char **argv)
{

  char *linehdr ;    /* USP line header */
  char *usptrace;    /* USP header and trace */
  char *type;        /* usp hdr type */

  float *uspdata;    /* USP trace as floats */
  float scalar;      /* multiplier to convert dt to seconds */

  int i;
  int verbose;
  int luin=0;       /* stdin */
  int uspbytes;     /* length of USP header + trace */
  int ntraces=0;    /* number of traces converted */
  int nsamp;        /* samples per trace from USP linehdr */
  int dt;           /* sampling interval from USP linehdr */
  int index;        /* usp hdr index */

  Value hval;       /* usp hdr value */

  segy tr;          /* SU header and trace */

  size_t n;

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(verbose) {
    fprintf(stderr,"Mapping %s to %s\n",su_usp_name[0].usp_name,su_usp_name[0].su_name);
    for(i=1; i<NUM_SU_USP_NAME; i++)
      fprintf(stderr,"        %s to %s\n",su_usp_name[i].usp_name,su_usp_name[i].su_name);
  }

  /* usp linehdr */
  linehdr = calloc(12000,sizeof(char));
  rtape(luin,linehdr,&n);
  index = get_usp_index("NumSmp",NUSPKEYS,verbose);
  get_usp_hval(linehdr,index,&hval,verbose);
  type = usphdr[index].type;
  nsamp = vtoi(type,hval);
  index = get_usp_index("SmpInt",NUSPKEYS,verbose);
  get_usp_hval(linehdr,index,&hval,verbose);
  type = usphdr[index].type;
  dt = vtoi(type,hval);
  index = get_usp_index("UnitSc",NUSPKEYS,verbose);
  get_usp_hval(linehdr,index,&hval,verbose);
  type = usphdr[index].type;
  scalar = vtof(type,hval);

  /* scalar determines actual dt */
  if(scalar == 0 && dt < 32)
    dt *= 1000;
  /* usp header + trace */
  uspbytes = USPTRCHDRBYTES + nsamp * sizeof(float);
  usptrace = calloc(uspbytes,sizeof(char));
  if(verbose)
    fprintf(stderr,"Allocated %d bytes for USP header and trace\n",uspbytes);

  tr.ns = nsamp;
  tr.dt = dt;
  uspdata = (float *)(usptrace + USPTRCHDRBYTES);
  /* loop over traces */
  for(;;) {
    rtape(luin,usptrace,&n);
    if(n == 0)
      break;
    ntraces++;
    make_su_tracehdr(usptrace,&tr,verbose);
    for(i=0; i<nsamp; i++)
      tr.data[i] = uspdata[i];
    puttr(&tr);
  }

  lbclos(luin);

  if(verbose)
    fprintf(stderr,"Converted %d traces\n",ntraces);

  return EXIT_SUCCESS;
}
