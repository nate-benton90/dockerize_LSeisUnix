/*====================================================================*\

   This program scans a velocity volume in SU format and extracts
   Landmark horizons bounding a threshold value.

   The output format corresponds to a default Landmark format for
   importing X-Y-Z triples.

   Reginald H. Beardsley                            rhb@acm.org
\*====================================================================*/

#include "usgrid.h"
#include "su.h"
#include "segy.h"
#include "par.h"

#define MAX_SURFACES 100

char *sdoc = 
 " gridsalthrz    - extracts Landmark top & base salt horizons from a \n"
"                 velocity volume grid\n"
"\n"
"      usage:   gridsalthrz <velocity.grid\n"
"\n"
"Required parameters:\n"
"\n"
"      none\n"
"\n"
"Optional parameters:\n"
"\n"
"      salt=4500   - minimum velocity threshold for salt interface\n"
"\n"
"Notes:\n"
" The program outputs the horizons in the ordering expected by \n"
" Zhiming Li's program, gridsalt.  In the future. I hope to correct \n"
" this so that a geometrically more sensible order is used as it \n"
" will greatly simplify the code of several programs.\n"
"\n"
" Reginald H. Beardsley        "__DATE__"            rhb@acm.org\n"
"\n"
;

int main(int argc, char **argv ){

   FILE *infp=stdin;

   FILE* bfp[MAX_SURFACES];
   FILE* tfp[MAX_SURFACES];

   char filename[PATH_MAX];

   float base[MAX_SURFACES];
   float top[MAX_SURFACES];

   float zscale = 1.0;

   float salt=4500;
   float x;
   float y;

   int i;
   int j;
   int k;
   int m;
   int n;
   int p;

   int in_salt;

   float xline;
   float line;

   float      d1, d2, d3, d4, d5;
   float      o1, o2, o3, o4, o5;
   int      i1, i2, i3, i4, i5;
   int      n1, n2, n3, n4, n5;

   float   *trace;
   int      ierr;


   usghed   usgh;
   memset( tfp ,0 ,sizeof(tfp) );
   memset( bfp ,0 ,sizeof(tfp) );

   /*----------------*/
   /* get parameters */
   /*----------------*/

   initargs(argc,argv);
   askdoc(1);

   getparfloat("salt",&salt);

   getparfloat("zscale",&zscale);

   /* get input grid parameters */
   if (fgetusghdr(infp, &usgh))
      err(" nonstandard grid file ");

   n1 = usgh.n1;
   n2 = usgh.n2;
   n3 = usgh.n3;
   n4 = usgh.n4;
   n5 = usgh.n5;

   o1 = usgh.o1;
   o2 = usgh.o2;
   o3 = usgh.o3;
   o4 = usgh.o4;
   o5 = usgh.o5;

   d1 = usgh.d1;
   d2 = usgh.d2;
   d3 = usgh.d3;
   d4 = usgh.d4;
   d5 = usgh.d5;

   if (n2 == 0)
      n2 = 1;
   if (n3 == 0)
      n3 = 1;
   if (n4 == 0)
      n4 = 1;
   if (n5 == 0)
      n5 = 1;

   trace = (float *) emalloc(n1 * sizeof(float));

   efseek(infp, 0, 0);

   for (i5 = 0; i5 < n5; i5++) {
      for (i4 = 0; i4 < n4; i4++) {
         for (i3 = 0; i3 < n3; i3++) {
            for (i2 = 0; i2 < n2; i2++) {

               efread(trace, sizeof(float), n1, infp);

               in_salt = 0;

               x = 0.0;
               y = 0.0;
               xline = o2 + i2 * d2;
               line  = o3 + i3 * d3;

               j = 0;
               k = 0;

               for (i1 = 0; i1 < n1; i1++) {

                  if( !in_salt && trace[i1] >= salt ){
                     top[j++] = o1 + i1 * d1;
                     in_salt = 1;
         
                  }else if( in_salt && trace[i1] < salt ){
                     base[k++] = o1 + (i1-1) * d1;
                     in_salt = 0;

                  }
   
                  if( k == MAX_SURFACES || j == MAX_SURFACES ){
                     goto lost;
                  }
               }

               /*-------------------------------*/
               /* write out X-Y-Z to top  files */
               /*-------------------------------*/

               lost:

               m = j;

               if( m == 1 ){

                  if( !tfp[0] ){
                     sprintf( filename ,"top_%d.xyz" ,1 );
                     if( !(tfp[0] = fopen( filename ,"w" )) ){
                        err( "Failed to open %s\n" ,filename );
                     }
                  }

                  fprintf( tfp[0] ,"%20s"   ,""      );
                  fprintf( tfp[0] ,"%10.1f " ,line    );
                  fprintf( tfp[0] ,"%10.1f " ,xline   );
                  fprintf( tfp[0] ,"%12.1f " ,x       );
                  fprintf( tfp[0] ,"%12.1f " ,y       );
                  fprintf( tfp[0] ,"%12.3f" ,top[0]  );

                  fprintf( tfp[0] ,"\n" );

               }else if( m > 1 ){
            
                  for( j=0; j<m; j++ ){
            
                     if( !tfp[j] ){
                        sprintf( filename ,"top_%d.xyz" ,j+1 );
                        if( !(tfp[j] = fopen( filename ,"w" )) ){
                           err( "Failed to open %s\n" ,filename );
                        }
                     }
            
                     fprintf( tfp[j] ,"%20s"   ,""      );
                     fprintf( tfp[j] ,"%10.1f " ,line    );
                     fprintf( tfp[j] ,"%10.1f " ,xline   );
                     fprintf( tfp[j] ,"%12.1f " ,x       );
                     fprintf( tfp[j] ,"%12.1f " ,y       );
                     fprintf( tfp[j] ,"%12.3f" ,top[j]  );
            
                     fprintf( tfp[j] ,"\n" );
            
                  } 
            
               }

               /*-------------------------------*/
               /* write out X-Y-Z to base files */
               /*-------------------------------*/

               n = k;

               if( n == 1 && m == 1 ){

                  if( !bfp[1] ){
                     sprintf( filename ,"base_%d.xyz" ,2 );
                     if( !(bfp[1] = fopen( filename ,"w" )) ){
                        err( "Failed to open %s\n" ,filename );
                     }
                  }

                  fprintf( bfp[1] ,"%20s"   ,""      );
                  fprintf( bfp[1] ,"%10.1f " ,line    );
                  fprintf( bfp[1] ,"%10.1f " ,xline   );
                  fprintf( bfp[1] ,"%12.1f " ,x       );
                  fprintf( bfp[1] ,"%12.1f " ,y       );
                  fprintf( bfp[1] ,"%12.3f" ,base[0] );

                  fprintf( bfp[1] ,"\n" );
            
               }else{
            
                  for( j=0; j<n; j++ ){
            
                     if( !bfp[j] ){
                        sprintf( filename ,"base_%d.xyz" ,j+1 );
                        if( !(bfp[j] = fopen( filename ,"w" )) ){
                           err( "Failed to open %s\n" ,filename );
                        }
                     }
            
                     fprintf( bfp[j] ,"%20s"   ,""      );
                     fprintf( bfp[j] ,"%10.1f " ,line    );
                     fprintf( bfp[j] ,"%10.1f " ,xline   );
                     fprintf( bfp[j] ,"%12.1f " ,x       );
                     fprintf( bfp[j] ,"%12.1f " ,y       );
                     fprintf( bfp[j] ,"%12.3f" ,base[j] );
            
                     fprintf( bfp[j] ,"\n" );
            
                  } 
            
               }
               
            }
         }
      }

   }

   exit(0);
}

