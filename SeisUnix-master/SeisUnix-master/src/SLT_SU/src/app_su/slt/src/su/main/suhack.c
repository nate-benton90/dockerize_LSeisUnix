
#include <math.h>

#include "su.h"
#include "segy.h"
#include "par.h"

#ifdef PAR_CHK

PAR parlist[] = {
   {"foo" ,'i' ,"0" ,"2" }
  ,{"bar" ,'f' ,"0.01" ,"0.1" }
  ,{0 ,0 ,0 ,0 }
};
#endif

char    *sdoc =
"\n suhack <in >out"
"\n Optional parameters:"
"\n"
"\n foo=  - useless but legal parameter w/ range 0 - 2 allowed"
"\n bar=  - useless but legal parameter w/ range 0.01 - 0.1 allowed"
"\n"
"\n Author: Reginald H. Beardsley "__DATE__" rhb@acm.org"
"\n";

main(int argc, char* argv[] ){

   segytrace in;   /* input volume */
   segytrace out;

   FILE* infp  = stdin;
   FILE* outfp = stdout;

   int i;
   int j;
   int k=7;

   int foo;
   float bar;

   initargs(argc, argv);
   askdoc(1);

   /*----------------*/
   /* get parameters */
   /*----------------*/

   getparfloat( "bar" ,&bar );
   getparint( "foo" ,&foo );

   /*------------------------*/
   /* loop over input traces */
   /*------------------------*/

   memset( &in ,0 ,sizeof(in) );

   if( !fgettr( infp ,&in ) ){
      err( "Unable to read first trace!" );
   }

   do {

      memset( &out ,0 ,sizeof(out) );
      memcpy( &out ,&in ,240 );
      out.dt = 100;
      out.ns = in.ns / 4;

      for( i=1; i<in.ns; i++ ){
         in.data[i] += in.data[i-1];
      }

      for( i=in.ns-2; i>=0; i-- ){
         in.data[i] += in.data[i+1];
      }

      for( j=0; j<out.ns-1; j++ ){
         i = j*4;

         if( i < 60 ){
            /* clean up garbage at start */
            out.data[j] = 0.0; 

         }else if( i-k > 0 && i+k < in.ns ){
            out.data[j] = 2*in.data[i] - in.data[i-k] - in.data[i+k];
            out.data[j] /= k*k;
         }

      }

      out.data[0] = out.data[2];
      out.data[1] = out.data[2];

      out.data[out.ns-1] = out.data[out.ns-2];


      /*----------------*/
      /* output results */
      /*----------------*/

      if( outfp ){
         fputtr(outfp, &out);
      }

      memset( &in ,0 ,sizeof(in) );

   } while (fgettr(infp, &in));

   return 0;

}

