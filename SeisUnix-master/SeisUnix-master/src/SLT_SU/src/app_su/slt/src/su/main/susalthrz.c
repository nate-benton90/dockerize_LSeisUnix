
/*====================================================================*\

   This program scans a velocity volume in SU format and extracts
   Landmark horizons bounding a threshold value.

   The output format corresponds to a default Landmark format for
   importing X-Y-Z triples.

   Reginald H. Beardsley                            rhb@acm.org
\*====================================================================*/

#include "su.h"
#include "segy.h"
#include "par.h"

#define MAX_SURFACES 100

char *sdoc = 
"\n"
"Required parameters:                     \n"
;

main(int argc, char **argv ){

   FILE *infp=stdin;

   FILE* bfp[MAX_SURFACES];
   FILE* tfp[MAX_SURFACES];

   char filename[PATH_MAX];

   float base[MAX_SURFACES];
   float top[MAX_SURFACES];

   float zscale = 1.0;

   float salt=4500;
   float dt;

   float x;
   float y;

   float line;
   float xline;

   int i;
   int j;
   int k;
   int n;
   int p;

   int in_salt;

   segytrace tr;

   memset( tfp ,0 ,sizeof(tfp) );
   memset( bfp ,0 ,sizeof(tfp) );

   /*----------------*/
   /* get parameters */
   /*----------------*/

   initargs(argc,argv);
   askdoc(1);

   getparfloat("salt",&salt);
   getparfloat("zscale",&zscale);

   while( fgettr( infp ,&tr ) ){

      x = tr.sx;
      y = tr.sy;

      line  = tr.fldr;
      xline = tr.ep;
      dt    = tr.dt * zscale;

      j = 0;
      k = 0;

      in_salt = 0;

      /*-------------------------------------*/
      /* search for salt-sediment interfaces */
      /*-------------------------------------*/

      for( i=0; i<tr.ns; i++ ){

         if( !in_salt && tr.data[i] >= salt ){
            top[j++] = i*dt;
            in_salt = 1;

         }else if( in_salt && tr.data[i] < salt ){
            base[k++] = i*dt;
            in_salt = 0;

         }
      }

/*--------------------------------------------------------------------*\
  Note that the geometrical relationship defined here places salt
  between top[j] & base[j].  This is different from what Zhiming Li
  defined in his programs.
\*--------------------------------------------------------------------*/

      /*-------------------------------*/
      /* write out X-Y-Z to top  files */
      /*-------------------------------*/

      for( i=0; i<j; i++ ){

         if( !tfp[i] ){
            sprintf( filename ,"top_%d.xyz" ,i+1 );
            if( !(tfp[i] = fopen( filename ,"w" )) ){
               err( "Failed to open %s\n" ,filename );
            }
         }

         fprintf( tfp[i] ,"%20s "   ,""      );
         fprintf( tfp[i] ,"%9.1f "  ,line    );
         fprintf( tfp[i] ,"%9.1f "  ,xline   );
         fprintf( tfp[i] ,"%11.1f " ,x       );
         fprintf( tfp[i] ,"%11.1f " ,y       );
         fprintf( tfp[i] ,"%11.2f " ,top[i]  );

         fprintf( tfp[i] ,"\n" );
   
      }

      /*--------------------------------*/
      /* write out X-Y-Z to base  files */
      /*--------------------------------*/

      for( i=0; i<k; i++ ){

         if( !bfp[i] ){
            sprintf( filename ,"base_%d.xyz" ,i+1 );
            if( !(bfp[i] = fopen( filename ,"w" )) ){
               err( "Failed to open %s\n" ,filename );
            }
         }

         fprintf( bfp[i] ,"%20s "   ,""      );
         fprintf( bfp[i] ,"%9.1f "  ,line    );
         fprintf( bfp[i] ,"%9.1f "  ,xline   );
         fprintf( bfp[i] ,"%11.1f " ,x       );
         fprintf( bfp[i] ,"%11.1f " ,y       );
         fprintf( bfp[i] ,"%11.2f " ,base[i] );

         fprintf( bfp[i] ,"\n" );
   
      }

   }

   return 0;

}
