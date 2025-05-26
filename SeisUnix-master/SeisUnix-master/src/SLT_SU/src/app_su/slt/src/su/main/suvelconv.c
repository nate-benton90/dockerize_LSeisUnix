
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\nsuvelconv  - convert a velocity model in SU format"
"\n"
"\n   suvelconv converts velocity volumes in SU format from time to "
"\n   depth or from depth to time using linear interpolation."
"\n"
"\nRequired parameters:"
"\n"
"\n   dt=     - desired output time sample rate in milliseconds"
"\n   tmax=   - maximum output time in milliseconds"
"\n"
"\n   or:"
"\n"
"\n   dz=     - desired output depth sample rate in feet or meters "
"\n   zmax=   - maximum output depth"
"\n"
"\n   If dt is specifed the input is converted from depth to time"
"\n   and if dz is specified the input is converted from time to depth."
"\n"
"\nOptional parameters:"
"\n"
"\n   vin=0   - input velocity type 0=interval, 1=rms, 2=average"
"\n   vout=0  - input velocity type 0=interval, 1=rms, 2=average"
"\n"
"\n   vmin=   - minimum threshold value.  All values less than this "
"\n             value are treated as missing data.  The last good "
"\n             value is used instead."
"\n"
"\n   vmax=   - maximum threshold value.  All values greater than this "
"\n             value are treated as missing data.  The last good "
"\n             value is used instead."
"\n"
"\nNotes:  "
"\n"
"\n   The depth and velocity units must match.  All inputs are "
"\n   converted to interval velocity at the input sampling and "
"\n   functions w/ non-physical velocities are dropped from the "
"\n   output."
"\n"
"\n   Reginald H. Beardsley     "__DATE__"            rhb@acm.org"
"\n";
/**************** end self doc *******************************************/


segytrace in;
segytrace out;

main(int argc, char* argv[] ){

   FILE* infp=stdin;
   FILE* outfp=stdout;
   
   float dt = 0.0;
   float dz = 0.0;
   
   float tmax = 0.0;
   float zmax = 0.0;
   
   float vmin =     0.0;
   float vmax = 15000.0;

   float max  = 0.0;
   float min  = 0.0;

   int vin  = 0;
   int vout = 0;
   
   float* Z = 0;
   float* T = 0;
   float* V = 0;

   float T_;
   float Vrms;
   float Vrms_;
   float dZ;

   float a;
   float b;
   float c;

   float dt_in = 0.0;
   float dz_in = 0.0;

   int i;
   int j;

   int k = 0;             /* output trace index */

   /*--------------------*/
   /* Process parameters */
   /*--------------------*/

   initargs(argc, argv);
   askdoc(1);
   
   getparfloat( "dt" ,&dt );
   getparfloat( "dz" ,&dz );
   
   getparfloat( "tmax" ,&tmax );
   getparfloat( "zmax" ,&zmax );
   
   getparfloat( "vmin" ,&vmin );
   getparfloat( "vmax" ,&vmax );
   
   if( dt != 0.0 && dz != 0.0 ){
      err( "Specify only dt= or dz=" );
   }
   
   if( tmax != 0.0 && zmax != 0.0 ){
      err( "Specify only tmax= or zmax=" );
   }

   if( tmax != 0.0 && dz != 0.0 ){
      err( "Specify zmax= or dt= " );
   }

   if( zmax != 0.0 && dt != 0.0 ){
      err( "Specify tmax= or dz= " );
   }

   getparint( "vin" ,&vin );
   
   if( vin < 0 || vin > 2 ){
      err( "Invalid vin" );
   }

   getparint( "vout" ,&vout );
   
   if( vout < 0 || vout > 2 ){
      err( "Invalid vout" );
   }
   
   /*-----------------*/
   /* Get first trace */
   /*-----------------*/

   if(!fgettr(infp,&in)){
      err(" error getting first trace");
   }

   /*-----------------------------------------------------------------*/
   /* Allocate space for input sample ordinates in output coordinates */
   /*-----------------------------------------------------------------*/

   if( dt != 0.0 && !(T=malloc( in.ns * sizeof( float ))) ){
      err( "malloc failed for T" );
   }

   if( dz != 0.0 && !(Z=malloc( in.ns * sizeof( float ))) ){
      err( "malloc failed for Z" );
   }
   
   if( !(V=malloc( in.ns * sizeof( float ))) ){
      err( "malloc failed for V" );
   }
   
   /*------------------*/
   /* loop over traces */
   /*------------------*/

   do {

      memcpy( &out ,&in ,sizeof(out) );

      /*------------------------------*/
      /* convert to interval velocity */
      /*------------------------------*/

      if( vin == 0 ){

         for( i=0; i<in.ns; i++ ){

            V[i] = in.data[i];

         }

      }else if( vin == 1 ){

         /*------------*/
         /* Vrms input */
         /*------------*/

         if( T ){

            dZ    = in.dt*1.0e-6;
            Vrms_ = 0.0;
            T_    = 0.0;

            for( i=0; i<in.ns; i++ ){

               Vrms = in.data[i];
               Vrms *= Vrms;

               if( Vrms_ == 0.0 ){
                  Vrms_ = Vrms;
               }

               a = 2 * dZ;
               b = - (Vrms - Vrms_) * T_;
               c = - Vrms * 2 * dZ;

               V[i] = (-b + sqrt( b*b - 4*a*c )) / (2*a);

               T_   += 2 * dZ /V[i];
               Vrms_ = Vrms;

            }

         }else if( Z ){

            V[0] = in.data[0];

            for( i=1; i<in.ns; i++ ){

               V[i] = sqrt( (pow(in.data[i],2.0)*in.dt*i 
                           - pow(in.data[i-1],2.0)*in.dt*(i-1)) 
                           / in.dt );

            }

         }

      }else if( vin == 2 ){

         /*------------------------*/
         /* Average velocity input */
         /*------------------------*/

         if( T ){

            for( i=0; i<in.ns; i++ ){

               V[i] = (in.dt*1.0e-3)
                    / ( i*in.dt*1.0e-3/in.data[i] 
                   -(i-1)*in.dt*1.0e-3/in.data[i-1] );
            }

         }else if( Z ){

            for( i=0; i<in.ns; i++ ){
   
               V[i] = (    i*2*in.dt*in.data[i] 
                     - (i-1)*2*in.dt*in.data[i-1] ) / in.dt;
   
            }

         }

      }else{

         err( "Invalid vin" );

      }

      k++;

      /*------------------------------------------*/
      /* check interval velocities against bounds */
      /*------------------------------------------*/

      if( V[0] < vmin || V[0] > vmax ){

         if( V[1] >= vmin && V[1] <= vmax ){
            V[0] = V[1];

         }else{
            fprintf( stderr, "Bad data: skipping function %k\n" );
            goto next;
         }
      }

      for( i=1; i<in.ns; i++ ){

         if( V[i] < vmin || V[i] > vmax ){

            fprintf( stderr ,"Changed trace %d sample %d " ,k ,i );
            fprintf( stderr ,"Vint from %f to %f\n" ,V[i] ,V[i-1] );

            V[i] = V[i-1];
         }
      }

/*--------------------------------------------------------------------*\
    Resample the input using linear interpolation of interval 
    velocity.  The output format may be the same as the input
    function.
\*--------------------------------------------------------------------*/

      if( T ){

         /*----------------------------*/
         /* convert from depth to time */
         /*----------------------------*/

         dz_in = 2*in.dt * 1.0e-3;
         out.ns = ceil(tmax / dt) + 1;
         memset( T ,0 ,out.ns*sizeof(float) );

         T[0]   = 0.0;
         out.dt = dt * 1.0e3;

         for( i=1; i<in.ns; i++ ){

            T[i] = T[i-1] + dz_in / V[i];

         }

         if( vout == 0 ){

            /*-------------*/
            /* Vint output */
            /*-------------*/

            i = 1;

            for( j=1; j<out.ns; j++ ){

               while( i<in.ns && j*dt*1.0e-3 > T[i] ){
                  i++;
               }

               out.data[j] = V[i-1] 
                           + (j*dt*1.0e-3 - T[i-1]) / (T[i] - T[i-1])
                           * (V[i] - V[i-1]);

               /*---------------------------------*/
               /* guard against loss of precision */
               /*---------------------------------*/

               max = V[i-1] < V[i] ? V[i] : V[i-1];
               min = V[i-1] > V[i] ? V[i] : V[i-1];

               out.data[j] > max ? max : out.data[j];
               out.data[j] < min ? min : out.data[j];

            }

         }else if( vout == 1 ){
 
            /*-------------*/
            /* Vrms output */
            /*-------------*/

            i = 1;

            for( j=1; j<out.ns; j++ ){

               while( i<in.ns && j*dt*1.0e-3 > T[i] ){
                  i++;
               }

               T_  = (j*dt*1.0e-3;

               V_  = V[i-1] + (V[i] - V[i-1])
                     * (T_ - T[i-1]) / (T[i] - T[i-1]);

               out.data[j] = sqrt( V_*V_*dt*1.0e-3 
                                  + out.data[j-1]*out.data[j-1]
                                  * (j-1)*dt*1.0e-3 ) / T_ ;

            }

         }else if( vout == 2 ){

            /*-------------*/
            /* Vavg output */
            /*-------------*/


            i = 1;

            for( j=1; j<out.ns; j++ ){

               while( i<in.ns && j*dt*1.0e-3 > T[i] ){
                  i++;
               }

               T_  = (j*dt*1.0e-3;

               V_  = V[i-1] + (V[i] - V[i-1])
                     * (T_ - T[j-1]) / (T[i] - T[i-1]);

               out.data[j] = (i*dz_in*1.0e-3 + (T_ - T[i-1])*V_ ) /T_;

            }

         }
            

      }else if( Z ){

         /*----------------------------*/
         /* convert from time to depth */
         /*----------------------------*/

         memset( Z ,0 ,sizeof(Z) );

         Z[0]   = 0.0;
         dt_in  = in.dt * 1.0e-6;
         out.ns = ceil(zmax / dz) + 1;

         if( vout == 0 ){

            /*-------------*/
            /* Vint output */
            /*-------------*/

            for( i=1; i<in.ns; i++ ){

               Z[i] = Z[i-1] + dt_in * V[i];

            }

            i = 1;

            for( j=1; j<out.ns; j++ ){

               while( i<in.ns && j*dz > Z[i] ){
                  i++;
               }

               out.data[j] = V[i-1] 
                           + (j*dz - Z[i-1]) / (Z[i] - Z[i-1])
                           * (V[i] - V[i-1]);

               /*---------------------------------*/
               /* guard against loss of precision */
               /*---------------------------------*/

               max = V[i-1] < V[i] ? V[i] : V[i-1];
               min = V[i-1] > V[i] ? V[i] : V[i-1];

               out.data[j] > max ? max : out.data[j];
               out.data[j] < min ? min : out.data[j];
            }

         }else if( vout == 1 ){

            /*-------------*/
            /* Vrms output */
            /*-------------*/

         }else if( vout == 2 ){

            /*-------------*/
            /* Vavg output */
            /*-------------*/

         }


      }

      fputtr(outfp,&out);

      next:;

   } while (fgettr(infp,&in));
   
   return EXIT_SUCCESS;
}
