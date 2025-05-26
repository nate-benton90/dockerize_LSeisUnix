
/* Copyright (c) Colorado School of Mines, 2003.*/

/* All rights reserved.                       */

/* SUHISTOGRAM: $Revision: 1.1 $ ; $Date: 2005/02/03 14:45:21 $        */

#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char    *sdoc[] = {
"                                                            ",
" SUHISTOGRAM - create histogram of input amplitudes   ",
"                                                            ",
"    suhistogram <in.su >out.dat                             ",
"                                                            ",
" required parameters:                                       ",
"",
" min=    - minimum bin ",
" max=    - maximum bin ",
" bins=   - number of bins ",
"                                                            ",
" Notes:                                                     ",
" This produces a two column ASCII output for use w/ gnuplot.",
" Extreme values are counted in the end bins.",
"                                                            ",
NULL
};

/*
 * Author: Reginald H. Beardsley  2005   rhb@acm.org
 * 
 * 
 */

/**************** end self doc ********************************/

segy     tr;

int      main(int argc, char **argv)
{

   int i;  
   int j;  
   int bins;
   float min;
   float max;
   float bin;

   int* histogram;
    
   /* Initialize */
   initargs(argc, argv);
   requestdoc(1);

   /* Get info from first trace */
   if( !gettr(&tr) ){
      err("Can't get first trace \n");
   }

   /* Get parameters */
   if( !getparfloat("min", &min) ){
      err("min must be specified\n");
   }

   if( !getparfloat("max", &max) ){
      err("max must be specified\n");
   }

   if( !getparint("bins", &bins) ){
      err("bins must be specified\n");
   }

   bin = (max-min) / bins;

   histogram=emalloc( bins*sizeof(int) );

   /* Loop over traces */
   do{
      for( i=0; i<tr.ns; i++ ){

         j = (tr.data[i] - min) / bin;
         j = j < 0 ? 0 : j;
         j = j > bins-1 ? bins-1 : j;

         histogram[j]++;

      }

   }while( gettr(&tr) );

   for( i=0; i<bins; i++ ){

      printf( "%15f " ,min+i*bin    );
      printf( "%15d " ,histogram[i] );
      printf( "\n" );
   } 

   return (EXIT_SUCCESS);
}
