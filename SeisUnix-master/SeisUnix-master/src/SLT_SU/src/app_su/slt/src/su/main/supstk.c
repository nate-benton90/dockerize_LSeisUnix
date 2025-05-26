
/*====================================================================*\
   Reginald H. Beardsley                            rhb@acm.org
\*====================================================================*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "su.h"
#include "segy.h"

char*   sdoc[] = {
 "supstk - create one or more partial stack volumes from input"
,""
,"   Required keywords:"
,""
,"   outfiles=      list of partial stack volume filenames for output"
,"   ranges=        percentage range of live traces for each partial stack"
,"   At least one mute key must be specified."
,""
,"   Optional keywords:"
,""
,"   bottom=        header key for end of live data for mute."
,"                  The default is the end of the trace."
,""
,"   top=           header key for start of live data for mute. "
,"                  The default is the start of the trace."

,"   nomute=0       =1 ignore mutes and interpolate over all traces"
,""
,"   supstk reads an NMO corrected, CDP sorted SU format dataset one"
,"   gather at a time. It then writes out one or more partial stack"
,"   volumes using linear interpolation between the values specified"
,"   by the top and bottom header keys."
,""
,"   example:"
,""
,"   # create 3 partial stacks w/ equal fold"
,""
,"   supstk  <gathers.su                     \\"
,"      ranges=0-33,33-67,67-100             \\"
,"      outfiles=near.su,mid.su,far.su       \\"
,0
};

static segy** inPtr;

#define MAX_STACKS 100
#define MAX_FOLD   500

int main( int argc ,char* argv[] ){

   /*---------------*/
   /* file pointers */
   /*---------------*/

   FILE*  inFile=stdin;
   FILE** outFile=0;

   /*-----------------*/
   /* string pointers */
   /*-----------------*/

   char* outFilename[MAX_STACKS];
   char* rangeString[MAX_STACKS];
   char* bottom  = 0;
   char* top     = 0;
   char* null    = 0;

   /*-------------------*/
   /* gather dimensions */
   /*-------------------*/

   int ns;
   int fold;
   int maxFold = MAX_FOLD;

   /*--------------------*/
   /* internal variables */
   /*--------------------*/

   int i;
   int j;
   int k;
   int m;
   int n;
   int p;
   int q;
   int u;
   int v;

   int nOut;

   float sum[MAX_FOLD+1];
   float dt;

   float start[MAX_STACKS];
   float end  [MAX_STACKS];

   int nomute = 0;
   int mute;

   Value topMute;
   int   topType;
   int   topIndex;

   Value bottomMute;
   int   bottomType;
   int   bottomIndex;

   /*------------------------------*/
   /* SEG-Y input & output buffers */
   /*------------------------------*/

   segy*  trace = 0;
   segy*  stack = 0;

   /*-------------------------*/
   /* get required parameters */
   /*-------------------------*/

   initargs( argc, argv );
   requestdoc(1);

   if( !(nOut=countparval( "outfiles" )) ){
      err( "output file list not given" );
   }

   if( nOut != countparval( "ranges" ) ){
      err( "number of ranges does not match number of output files" );
   }

   if( nOut > MAX_STACKS ){
      err( "can only create %d partial stacks" ,MAX_STACKS );
   }

   for( i=0; i<nOut; i++ ){
      getnparstringarray( i+1 ,"outfiles" ,&(outFilename[i]) );
   }

   for( i=0; i<nOut; i++ ){
      getnparstringarray( i+1 ,"ranges" ,&(rangeString[i]) );
   }

   /*-------------------------*/
   /* get optional parameters */
   /*-------------------------*/

   getparint( "nomute" ,&nomute );

   if( getparstring( "top" ,&top ) ){
      topType  = *hdtype( top );
      topIndex = getindex( top );
   }

   if( getparstring( "bottom" ,&bottom ) ){
      bottomType  = *hdtype( bottom );
      bottomIndex = getindex( bottom );
   }

   if( !nomute && !top && !bottom ){
       err( "Either nomute or at least one mute key is required" );

   }

   if( getparstring( "inside" ,&null ) 
    || getparstring( "inside" ,&null ) ){
       err( "inside= & outside= are obsolete parameters" );
   }

   /*---------------------*/
   /* parse range strings */
   /*---------------------*/
  
   for( i=0; i<nOut; i++ ){

      j=strcspn( rangeString[i] ,"-" );
      rangeString[i][j] = '\0'; 
      sscanf( rangeString[i] ,"%f"         ,&(start[i]) );
      sscanf( &(rangeString[i][j+1]) ,"%f" ,&(end[i])   );

      /*----------------------------------*/
      /* convert from percent to fraction */
      /*----------------------------------*/

      start[i] *= 0.01;
      end[i]   *= 0.01;

      if( end[i] < start[i] ){
         err( "bad range for partial stack %d" ,i+1 );
      }

   }

   /*-------------------*/
   /* open output files */
   /*-------------------*/

   if( !(outFile=calloc( nOut ,sizeof(FILE*) )) ){
      err( "unable to allocate output FILE* array" );
   }

   for( i=0; i<nOut; i++ ){
      if( !(outFile[i]=fopen(outFilename[i] ,"w" )) ){
         err( "fopen failed for %s\n" ,outFilename[i] );
      }
   }

   /*-----------------------------------*/
   /* allocate initial space for gather */
   /*-----------------------------------*/

   if( !(trace=calloc( maxFold ,sizeof(segy) )) ){
      err( "calloc failed: %f %d" ,__FILE__ ,__LINE__ );
   }

   if( !(stack=calloc( nOut ,sizeof(segy) )) ){
      err( "calloc failed: %f %d" ,__FILE__ ,__LINE__ );
   }

   inPtr = &trace;

   /*---------------------*/
   /* get the first trace */
   /*---------------------*/

   if( !fgettr( inFile  ,&(trace[0]) ) ){
      err( "Unable to read first trace!" );
   }

   ns = trace[0].ns;
   dt = trace[0].dt*1e-3;

/*--------------------------------------------------------------------*\
   Read data in CDP gathers from the input file. Each time return the
   fold of the gather.
\*--------------------------------------------------------------------*/

   while( (fold=readGather( inFile ,inPtr ,&maxFold ,"cdp" )) > 0 ){

      /*--------------------------------*/
      /* setup the output trace headers */
      /*--------------------------------*/

      for( i=0; i<nOut; i++ ){
         memcpy( (char*)&(stack[i]) ,(char*)&(trace[0]) ,240 );
         stack[i].offset=0;
      }

      /*----------------------------------*/
      /* loop over all time/depth samples */
      /*----------------------------------*/

      for( i=0; i<ns; i++ ){

         memset( sum ,0 ,sizeof(sum) );

         if( !nomute ){

            /*---------------------*/
            /* find top mute index */
            /*---------------------*/
   
            u=0;
   
            if( top ){

               while( u < fold ){
                  gethval( &(trace[u]) ,topIndex ,&topMute );

                  switch( topType ){

                     case 'h':
                        mute = topMute.h;
                        break;

                     case 'u':
                        mute = topMute.u;
                        break;

                     case 'i': 
                        mute = topMute.i;
                        break;

                     case 'p': 
                        mute = topMute.p;
                        break;

                     case 'l': 
                        mute = topMute.l;
                        break;

                     case 'v': 
                        mute = topMute.v;
                        break;
                   
                     default:
                        err( "top mute key is unknown data type" );

                  }

                  if( i*dt > mute ){
                     u++;

                  }else{
                     break;

                  }
               }
            }

            /*------------------------*/
            /* find bottom mute index */
            /*------------------------*/

            v=0;

            if( bottom ){
      
               while( v < fold ){
                  gethval( &(trace[v]) ,bottomIndex ,&bottomMute );

                  switch( bottomType ){

                     case 'h':
                        mute = bottomMute.h;
                        break;

                     case 'u':
                        mute = bottomMute.u;
                        break;

                     case 'i': 
                        mute = bottomMute.i;
                        break;

                     case 'p': 
                        mute = bottomMute.p;
                        break;

                     case 'l': 
                        mute = bottomMute.l;
                        break;

                     case 'v': 
                        mute = bottomMute.v;
                        break;
                   
                     default:
                        err( "bottom mute key is unknown data type" );

                  }

                  if( i*dt < mute ){
                     v++;
                  }else{
                     break;
                  }
               }

            }
   
            /*----------------------------*/
            /* set summation loop indices */
            /*----------------------------*/

            if( top && !bottom ){
               j = 0;
               k = u;

            }else if( bottom && !top ){
               j = 0;
               k = v;

            }else if( v > u ){
               j = u;
               k = v;

            }else{
               j = v;
               k = u;

            } 

         }else{

            j = 0;
            k = fold;

         }

         /*-----------------------------*/
         /* run sum over unmuted traces */
         /*-----------------------------*/

         for( q=0; q < (k-j); q++ ){
            sum[q+1] = sum[q] + trace[j+q].data[i];
         }

         /*-------------------------------------*/
         /* linearly interpolate partial stacks */
         /*-------------------------------------*/

         for( p=0; p<nOut; p++ ){

            m = ceil(  (k-j) * start[p] );
            n = floor( (k-j) * end[p]   );

            stack[p].data[i] = sum[n] - sum[m];

            if( n-m ){
               stack[p].data[i] /= (n-m);
            }

         }

      }

      /*--------------------------------------*/
      /* write partial stacks to output files */
      /*--------------------------------------*/

      for( j=0; j<nOut; j++ ){
         fputtr( outFile[j] ,&(stack[j]) );
      }
   }

   return( 0 );
}
