
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

#define MAX_SURFACES 10
#define MAX_ISO      10

char *sdoc = 
 " supoptop   - extracts Landmark top & base isopop horizons from a\n"
"               velocity volume\n"
"\n"
"      usage:   supoptop <velocity.su\n"
"\n"
"Required parameters:\n"
"\n"
"      none\n"
"\n"
"Optional parameters:\n"
"\n"
"      iso=50      - isopop thresholds \n"
"\n"
"      zscale=1.0  - scaling factor to apply to tr.dt value\n"
"\n"
"Notes:\n"
"\n"
" Reginald H. Beardsley  "__DATE__"         rhb@acm.org\n"
"\n"
;

main(int argc, char **argv ){

   FILE *infp=stdin;

   FILE* bfp[MAX_ISO][MAX_SURFACES];
   FILE* tfp[MAX_ISO][MAX_SURFACES];

   char filename[PATH_MAX];

   float base[MAX_ISO][MAX_SURFACES];
   float top[MAX_ISO][MAX_SURFACES];

   float zscale = 1.0;

   float iso[MAX_ISO];
   float x;
   float y;

   float line;
   float xline;

   float dt = 0.0;

   int i;
   int j;
   int k;
   int n;

   int n_top[MAX_ISO];
   int n_base[MAX_ISO];
   int in_op[MAX_ISO];
   int n_iso;

   segytrace tr;

   memset( tfp ,0 ,sizeof(tfp) );
   memset( bfp ,0 ,sizeof(tfp) );
   memset( iso ,0 ,sizeof(iso) );

   memset( n_top ,0 ,sizeof(n_top)  );
   memset( n_base ,0 ,sizeof(n_base) );

   /*----------------*/
   /* get parameters */
   /*----------------*/

   initargs(argc,argv);
   askdoc(1);


   n_iso=countnparval( 1 ,"iso" );

   if( n_iso > MAX_ISO ){
      err( "Exceeded MAX_ISO" );
   }

   for( i=0; i<n_iso; i++ ){
      getnparfloat( i+1 ,"iso" ,&iso[i] );
   }

   getparfloat("zscale",&zscale);

   while( fgettr( infp ,&tr ) ){

      x = tr.sx;
      y = tr.sy;

      if( tr.scalco > 1 ){
         x *= tr.scalco;
         y *= tr.scalco;

      }else if( tr.scalco < 0 ){
         x /= fabs(tr.scalco);
         y /= fabs(tr.scalco);

      }

      line  = tr.fldr;
      xline = tr.ep;

      dt = tr.dt * zscale;


      for( j=0; j<n_iso; j++ ){
         n_top[j]  = 0;
         n_base[j] = 0;
         in_op[j] = 0;

      }

      memset( top  ,0 ,sizeof(top)  );
      memset( base ,0 ,sizeof(base) );

      for( i=0; i<tr.ns; i++ ){

         for( j=0; j<n_iso; j++ ){

            if( !in_op[j] && tr.data[i] >= iso[j] ){
               top[j][n_top[j]] = i*dt;
               n_top[j] += 1;
               in_op[j] = 1;
   
            }else if( in_op[j] && tr.data[i] < iso[j] ){
               base[j][n_base[j]] = i*dt;
               n_base[j] += 1;
               in_op[j] = 0;
   
            }
   
            if( n_base[j] == MAX_SURFACES || n_top[j] == MAX_SURFACES ){
               goto lost;
            }
         }
      }

      /*-------------------------*/
      /* write out horizon picks */
      /*-------------------------*/

lost:

      for( j=0; j<n_iso; j++ ){

         for( k=0; k<n_top[j]; k++ ){
   
            if( !tfp[k][j] ){
               sprintf( filename ,"top_%.2f_%d.xyz" ,iso[j],k+1 );
               if( !(tfp[k][j] = fopen( filename ,"w" )) ){
                  err( "Failed to open %s\n" ,filename );
               }
            }
   
            fprintf( tfp[k][j] ,"%20s"    ,""         );
            fprintf( tfp[k][j] ,"%10.1f " ,line       );
            fprintf( tfp[k][j] ,"%10.1f " ,xline      );
            fprintf( tfp[k][j] ,"%12.1f " ,x          );
            fprintf( tfp[k][j] ,"%12.1f " ,y          );
            fprintf( tfp[k][j] ,"%12.3f"  ,top[j][k]  );
   
            fprintf( tfp[k][j] ,"\n" );
   
         } 

         for( k=0; k<n_base[j]; k++ ){
   
            if( !bfp[k][j] ){
               sprintf( filename ,"base_%.2f_%d.xyz" ,iso[j],k+1 );
               if( !(bfp[k][j] = fopen( filename ,"w" )) ){
                  err( "Failed to open %s\n" ,filename );
               }
            }
   
            fprintf( bfp[k][j] ,"%20s"    ,""         );
            fprintf( bfp[k][j] ,"%10.1f " ,line       );
            fprintf( bfp[k][j] ,"%10.1f " ,xline      );
            fprintf( bfp[k][j] ,"%12.1f " ,x          );
            fprintf( bfp[k][j] ,"%12.1f " ,y          );
            fprintf( bfp[k][j] ,"%12.3f"  ,base[j][k] );
   
            fprintf( bfp[k][j] ,"\n" );
   
         } 
   
      }


   }

   return 0;

}
