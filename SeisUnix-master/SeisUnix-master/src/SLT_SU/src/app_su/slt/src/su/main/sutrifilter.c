
#include <math.h>

#include "su.h"
#include "segy.h"
#include "par.h"

char    *sdoc =
"\n SUTRIFILTER - apply Claerbout-Lumley triangle filter operator"
"\n"
"\n sutrifilter w=n  <in >out"
"\n"
"\n Required parameters:"
"\n"
"\n len=   filter length"
"\n"
"\n Optional parameters:"
"\n"
"\n -none"
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
   int k;
   int w;

   int len;

   int a;
   int b;

   float sum[SU_NFLTS];
   float tmp[SU_NFLTS];

   initargs(argc, argv);
   askdoc(1);

   /*----------------*/
   /* get parameters */
   /*----------------*/

   getparint( "len" ,&len );

   /*------------------------*/
   /* loop over input traces */
   /*------------------------*/

   memset( &in ,0 ,sizeof(in) );

   if( !fgettr( infp ,&in ) ){
      err( "Unable to read first trace!" );
   }

   /*---------------------*/
   /* check filter length */
   /*---------------------*/

   if( in.ns < len ){
      fprintf( stderr ,"filter too long for input\n" );
      exit(-1);

   }else{
      w = len / 2;
   }

   do {

      memset( tmp  ,0 ,sizeof(tmp) );
      memset( sum  ,0 ,sizeof(sum) );

      memset( &out ,0 ,sizeof(out) );
      memcpy( &out ,&in ,240 );

      /*--------------*/
      /* remove trend */
      /*--------------*/

      for( i=0; i<in.ns; i++ ){
         tmp[i] = in.data[i] 
                - (in.data[0] + i*(in.data[in.ns-1]-in.data[0])/in.ns);
      }

      /*---------------------*/
      /* forward integration */
      /*---------------------*/

      sum[0] = tmp[0];
 
      for( i=1; i<in.ns; i++ ){
         sum[i] = sum[i-1] + tmp[i];
      }

      /*----------------------*/
      /* backward integration */
      /*----------------------*/

      for( i=in.ns-2; i>=0; i-- ){
         sum[i] += sum[i+1];
      }

      /*-----------------*/
      /* leading segment */
      /*-----------------*/

      k = (w*w + w)/2;
      j = w;

      for( i=0; i<w; i++ ){

         k -= j;
         j--;

         out.data[i] = 2*sum[i] - sum[i+w] - sum[0];
         out.data[i] /= w*w - k;
         out.data[i] += (in.data[0] + i*(in.data[in.ns-1]-in.data[0])/in.ns);

      }

      /*-----------------*/
      /* central segment */
      /*-----------------*/

      for( i=w; i<in.ns-w; i++ ){

         out.data[i] = 2*sum[i] - sum[i-w] - sum[i+w];
         out.data[i] /= w*w;
         out.data[i] += (in.data[0] + i*(in.data[in.ns-1]-in.data[0])/in.ns);

      }

      /*------------------*/
      /* trailing segment */
      /*------------------*/

      k = 0;
      j = 0;

      for( i=in.ns-w; i<in.ns; i++ ){

         k += j;

         out.data[i] = 2*sum[i] - sum[i-w] + j*sum[in.ns-1];
         out.data[i] /= w*w - k;
         out.data[i] += (in.data[0] + i*(in.data[in.ns-1]-in.data[0])/in.ns);
         j++;

      }

      /*----------------*/
      /* output results */
      /*----------------*/

      if( outfp ){
         fputtr(outfp, &out);
      }

   } while (fgettr(infp, &in));

   return 0;

}
