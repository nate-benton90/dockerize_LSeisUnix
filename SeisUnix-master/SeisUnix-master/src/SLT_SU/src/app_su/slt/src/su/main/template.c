
#include <math.h>

#include "su.h"
#include "segy.h"
#include "par.h"

char    *sdoc =
"\n Author: Reginald H. Beardsley "__DATE__" rhb@acm.org"
"\n";

main(int argc, char* argv[] ){

   segytrace in;   /* input volume */
   segytrace out;

   FILE* infp  = stdin;
   FILE* outfp = stdout;

   int i;

   initargs(argc, argv);
   askdoc(1);

   /*----------------*/
   /* get parameters */
   /*----------------*/

   /*------------------------*/
   /* loop over input traces */
   /*------------------------*/

   if( !fgettr( infp ,&in ) ){
      err( "Unable to read first trace!" );
   }

   do {

      memcpy( &out ,&in ,240 );

      for( i=0; i<in.ns; i++ ){

      }

      /*----------------*/
      /* output results */
      /*----------------*/

      if( outfp ){
         fputtr(outfp, &ep);
      }


   } while (fgettr(infp, &in));

   return 0;

}

