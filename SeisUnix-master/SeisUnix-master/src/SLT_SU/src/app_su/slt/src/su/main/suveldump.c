

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

   int i;
   float Z;
   float T;
   float Vavg;

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

      Z = 0.0;
      Vavg = 0.0;

      for( i=0; i<in.muts; i++ ){

         T = i*in.dt*1.0e-3;

         if( i ){
            Vavg = 2*Z / (T*1.0e-3);  

         }else{
            Vavg = in.data[0];

         }

         printf( "%d " ,in.tracf );
         printf( "%d " ,in.fldr );
         printf( "%d " ,in.ep );

         printf( "%8.2f " ,T );
         printf( "%8.2f " ,Z );
         printf( "%8.2f " ,in.data[i] );
         printf( "%8.2f " ,Vavg );

         printf( "\n" );

         Z += 0.5*in.dt*1.0e-6*in.data[i];

      }

      printf( "\n" );

   } while (fgettr(infp, &in));

   return 0;

}

