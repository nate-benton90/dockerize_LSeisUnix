/* Copyright (c) 2001.*/
/* All rights reserved.                       */

/* SUSPLIT: $Date: 2001/06/9 22:36:46 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									       ",
" SUSPLIT - Creates files with traces containing the same header word specified    ",
" 									       ",
"   susplit <stdin > stdout     	                               ",
" 									       ",
"  key=fldr	       ",
" 						                                ",
NULL}; 

/* Credits:
 *	Institute of Earth Sciences Jaume Almera: Ramon Carbonell
 *      CSIC: 
 */
/**************** end self doc ****************************************/

segy tr;

int
main(int argc, char **argv)
{
       cwp_String key;      /* header key word from segy.h	*/
       Value val;	       /* value of key		       	*/
       int indx;	       /* index of key			*/
       int ival;	       /* ... cast to int			*/
       char tmpfl[80];              /* ... cast to int			*/
       cwp_String type;     /* type of key		      	*/
       FILE *infp=NULL;    /* pointer to input file		*/

       /* Initialize */
       initargs(argc, argv);
       requestdoc(1);
       
       /* Default parameters;  User-defined overrides */
       if (!getparstring("key", &key))		key = "fldr";
       type = hdtype(key);
       indx = getindex(key);
       /* get trace of infile */
       if (!gettr(&tr)) err("can't get first trace");	
       do {
              gethval(&tr, indx, &val);
              ival = vtoi(type, val);
              sprintf(tmpfl,"%i.su",ival);
/*              warn("Filename -> %s",tmpfl);   */
              infp = fopen(tmpfl, "a");
              fputtr(infp, &tr);
              fclose(infp);
       } while (gettr(&tr));
       return EXIT_SUCCESS; 
}
