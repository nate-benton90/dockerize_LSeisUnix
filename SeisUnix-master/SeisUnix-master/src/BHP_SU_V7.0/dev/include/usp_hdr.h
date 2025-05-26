
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
*/
/* Relevant members of USP header structures, modeled after SU hdr.h */

#define USPLINHDRBYTES 1004
#define USPTRCHDRBYTES 256
#define NUSPKEYS 142
#define NUM_SU_USP_NAME 15

static struct {
  char *key; char *type; int offs;
} usphdr[] = {
/* Line Header */
  {"NumTrc",  "i",  48},
  {"NumRec",  "i",  52},
  {"SmpInt",  "i",  56},
  {"NumSmp",  "i",  60},
  {"Format",  "h",  64},
  {"UnitFl",  "h", 140},
  {"FrstSP",  "h", 142},
  {"TmMsFS",  "f", 160},
  {"UnitSc",  "f", 348},
  {"HlhEnt",  "h", 1000},
  {"HlhByt",  "h", 1002},
/* Trace Header */
  {"SGRNum", "h",   0},
  {"SGRAmp", "h",   2},
  {"SGRDat", "i",   4},
  {"FlReFN", "h",   8},
  {"FlStNm", "h",  10},
  {"PREPIn", "h",  12},
  {"InStUn", "h",  14},
  {"InStAp", "h",  16},
  {"PREPRc", "h",  18},
  {"RcStUn", "h",  20},
  {"RcStAp", "h",  22},
  {"ToStAp", "h",  24},
  {"ToTmAA", "h",  26},
  {"ToStUn", "h",  28},
  {"ToTmAU", "h",  30},
  {"SrComp", "h",  32},
  {"RcComp", "h",  34},
  {"TrHdID", "h",  40},
  {"SrRcAz", "h",  42},
  {"SrPtXC", "i",  44},
  {"SrPtYC", "i",  48},
  {"RcPtXC", "i",  52},
  {"RcPtYC", "i",  56},
  {"SrRcMX", "i",  60},
  {"SrRcMY", "i",  64},
  {"CDPBCX", "i",  68},
  {"CDPBCY", "i",  72},
  {"InTrCn", "i",  76},
  {"FlDtEl", "h",  80},
  {"MulSkw", "h",  82},
  {"PerSPO", "h",  84},
  {"InlSPO", "h",  86},
  {"SrXAzm", "h",  88},
  {"TiLiI1", "h",  88},
  {"SrYRot", "h",  90},
  {"TiLiI2", "h",  90},
  {"SrZRot", "h",  92},
  {"TiLiI3", "h",  92},
  {"RcXAzm", "h",  94},
  {"TiLiI4", "h",  94},
  {"RcYRot", "h",  96},
  {"TiLiI5", "h",  96},
  {"RcZRot", "h",  98},
  {"TiLiI6", "h",  98},
  {"TVPT01", "h", 100},
  {"Horz01", "f", 100},
  {"TVPV01", "h", 102},
  {"TVPT02", "h", 104},
  {"Horz02", "f", 104},
  {"TVPV02", "h", 106},
  {"TVPT03", "h", 108},
  {"Horz03", "f", 108},
  {"TVPV03", "h", 110},
  {"TVPT04", "h", 112},
  {"Horz04", "f", 112},
  {"TVPV04", "h", 114},
  {"TVPT05", "h", 116},
  {"Horz05", "f", 116},
  {"TVPV05", "h", 118},
  {"TVPT06", "h", 120},
  {"Horz06", "f", 120},
  {"TVPV06", "h", 122},
  {"TVPT07", "h", 124},
  {"Horz07", "f", 124},
  {"TVPV07", "h", 126},
  {"TVPT08", "h", 128},
  {"Horz08", "f", 128},
  {"TVPV08", "h", 130},
  {"TVPT09", "h", 132},
  {"TVPV09", "h", 134},
  {"AvDaSt", "h", 134},
  {"TVPT10", "h", 136},
  {"TrSrSt", "h", 136},
  {"TVPV10", "h", 138},
  {"TVPT11", "h", 140},
  {"TVPV11", "h", 142},
  {"TVPT12", "h", 144},
  {"TVPV12", "h", 146},
  {"TVPT13", "h", 148},
  {"TVPV13", "h", 150},
  {"TVPT14", "h", 152},
  {"TVPV14", "h", 154},
  {"TVPT15", "h", 156},
  {"TVPV15", "h", 158},
  {"TVPT16", "h", 160},
  {"TVPV16", "h", 162},
  {"TVPT17", "h", 164},
  {"TVPV17", "h", 166},
  {"TVPT18", "h", 168},
  {"TVPV18", "h", 170},
  {"TVPT19", "h", 172},
  {"TVPV19", "h", 174},
  {"TVPT20", "h", 176},
  {"TVPV20", "h", 178},
  {"TVPT21", "h", 180},
  {"TVPV21", "h", 182},
  {"RedVel", "h", 180},
  {"RedTim", "h", 182},
  {"VPick1", "h", 184},
  {"VPick2", "h", 186},
  {"LRcCDP", "h", 188},
  {"CabDep", "h", 190},
  {"WDepDP", "h", 192},
  {"DPOPer", "h", 194},
  {"ShtDep", "h", 196},
  {"UphlTm", "h", 198},
  {"DpPtLt", "i", 200},
  {"DpPtLn", "i", 204},
  {"FoldNm", "h", 208},
  {"RecNum", "h", 210},
  {"TrcNum", "h", 212},
  {"SrcPnt", "h", 214},
  {"SrcLoc", "h", 216},
  {"PrRcNm", "h", 218},
  {"PrTrNm", "h", 220},
  {"SrPtEl", "h", 222},
  {"SrPrLt", "i", 224},
  {"SrPrLn", "i", 228},
  {"DstUsg", "h", 232},
  {"RecInd", "h", 234},
  {"DstSgn", "h", 236},
  {"GrpElv", "h", 238},
  {"LinInd", "h", 240},
  {"DphInd", "h", 242},
  {"DePtEl", "h", 244},
  {"RfSrEl", "h", 246},
  {"StaCor", "h", 248},
  {"DatShf", "h", 250},
  {"SoPtNm", "h", 252},
  {"SoPtAl", "c", 254},
  {"SoPtBi", "c", 255},
};

/* SU to USP header name correspondence */
static struct {
  char *su_name; char *usp_name;
} su_usp_name[] = {
{"tracl","TrcNum"},
{"fldr","FlReFN"},
{"ep","SoPtNm"},
{"cdp","DphInd"},
{"nhs","FoldNm"},
{"offset","DstSgn"},
{"gelev","GrpElv"},
{"selev","SrPtEl"},
{"sx","SrPtXC"},
{"sy","SrPtYC"},
{"gx","RcPtXC"},
{"gy","RcPtYC"},
{"sstat","InStAp"},
{"gstat","RcStAp"},
{"tstat","ToStAp"},
};


/* Prototypes */
void make_usp_linehdr(char *linehdr, segy *tr, int verbose);
void make_usp_tracehdr(segy *tr, char *tracehdr, int verbose);
void put_usp_hval(char *tr, int index, Value *valp, int verbose);
int get_usp_index(char *key, int nkeys, int verbose);
void map_su_to_usp_hdr(char *suname, char *uspname, segy *tr, char *uhdr, int value, int verbose);
void make_su_tracehdr(char *tracehdr, segy *tr, int verbose);
void map_usp_to_su_hdr(char *uspname, char *suname, char *uhdr, segy *tr, int value_in, int verbose);
void get_usp_hval(char *tr, int index, Value *valp, int verbose);
