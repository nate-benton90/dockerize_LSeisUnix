/* velocity card format conversion */

#include "velo.h"
#include "cwp.h"


char *sdoc = 
"HVELSORT - sort DISCO HANDVEL cards      \n"
"\n"
"hvelsort [parameters] <handvel-cards >hvelsort-cards       \n" 
"\n"
"Required parameters:                     \n"
"none \n"
"\n"
"Optional parameters:                     \n"
"nvfmax=256000 maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=512    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"\n"
"Notes:                          \n"
"  Only CDPLABEL format is supported by this version.\n"
"\n"
"AUTHOR:    Zhiming Li,       ,  7/26/99        \n"    
"  rewritten by Reginald H. Beardsley 1/15/2004 \n"

;

typedef struct {
   int cdp;
   int n;
   float* t;
   float* v;
}FUNC;

int f_cmp( const void* e1 ,const void* e2 );

main(int argc ,char* argv[] ) {

   FUNC* f;
   char buf[1024];
   char tag[1024];

   int i;
   int j;
   int n1;
   int n2;

   /*----------------*/
   /* get parameters */
   /*----------------*/

   initargs(argc,argv);
   askdoc(1);

   /*-------------------*/
   /* memory allocation */
   /*-------------------*/

   if (!getparint("ntvmax",&n1)) n1=512;
   if (!getparint("nvfmax",&n2)) n2=256000;

   /*----------------------*/
   /* allocate index array */
   /*----------------------*/

   if( !(f=calloc( n2 ,sizeof(FUNC) )) ){
      err( "unable to allocate sort buffer" );
   }

   /*--------------------------*/
   /* allocate T-V pair arrays */
   /*--------------------------*/

   for( i=0; i<n2; i++ ){
      if( !(f[i].t=calloc(2*n1, sizeof(float))) ){
         err( "unable to allocate T-V buffer %d" ,i );
      }
      f[i].v=f[i].t+n1;
   }

   /*---------------------*/
   /* read the input data */
   /*---------------------*/

   i=0;
   j=0;

   while( !feof(stdin) && i < n2 && fgets( buf ,sizeof(buf) ,stdin ) ){


      if( strstr(buf ,"HANDVEL" ) ){
         i++;
         sscanf( buf ,"%s %d" ,tag ,&f[i].cdp );
         f[i-1].n = j;
         j=0;

      }else{
         sscanf( buf ,"%f%f%f%f%f%f%f%f" 
                     ,&f[i].t[j],&f[i].v[j]
                     ,&f[i].t[j+1],&f[i].v[j+1]
                     ,&f[i].t[j+2],&f[i].v[j+2]
                     ,&f[i].t[j+3],&f[i].v[j+3]);
         j+=4;
      }

   }

   f[i].n=j;
   n2=i+1;

   /*----------------*/
   /* sort the array */
   /*----------------*/

   qsort( f ,n2 ,sizeof(FUNC) ,f_cmp );

   for( i=1; i<n2; i++ ){

      printf( "%-8s" ,"HANDVEL" );
      printf( "%8d" ,f[i].cdp );

      for( j=0; j<f[i].n; j++ ){

         if( j%4 == 0 ){
            printf( "\n" );
         }

         if( f[i].t[j] || f[i].v[j] ){
            printf( "%8.1f" ,f[i].t[j] );
            printf( "%8.1f" ,f[i].v[j] );
         }

      }
      printf( "\n" );

   }
}

int f_cmp( const void* e1 ,const void* e2 ){

   if( ((FUNC*)e1)->cdp < ((FUNC*)e2)->cdp ){
      return( -1 );

   }else if( ((FUNC*)e1)->cdp > ((FUNC*)e2)->cdp ){
      return( 1 );

   }else{
      return( 0 );

   }
}
