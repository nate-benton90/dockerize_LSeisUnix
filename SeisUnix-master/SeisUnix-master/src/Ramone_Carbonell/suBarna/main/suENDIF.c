/* Copyright (c) Colorado School of Mines, 2000.*/
/* All rights reserved.                       */

/* SUFILTER: $Revision: 1.15 $ ; $Date: 1998/08/24 20:10:26 $        */


#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"								              ",
" SUENDIF  - Closes the IF opened by SUIF resseting the processing header   ",
"         tr.ifflag to 0                                                                                               ",
"									",
" suENDIF <stdin >stdout [no parameters]                	                            ",
"									",
"                                                                                                                            ",
"									",
NULL};

/* Credits:
 *      ICTJA: Ramon Carbonell
 *
 * Trace header field accessed: trfflag
 */
/**************** end self doc ***********************************/

/* Prototype of function used internally */

segy tr;

void main(int argc, char **argv)
{
    /* Initialize */
    initargs(argc, argv);
    requestdoc(1);
    if ( !fgettr(stdin, &tr)) err("Can't read first trace !!!");
    do {              
         tr.ifflg=1;
         puttr(&tr);
      } while (gettr(&tr)); 
}
