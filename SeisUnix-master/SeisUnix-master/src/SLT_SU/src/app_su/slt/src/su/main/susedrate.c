
static const char ID[] = "Source file: \
   $RCSfile: susedrate.c,v $ \
   $Revision: 1.12 $ \
   $Date: 2005/04/27 12:43:42 $ ";


char *sdoc =
"SUSEDRATE - 2D arbline sedimentation rate computation \n"
"\n"
"susedrate	 <in.su hznfile= horizon_name=age  >out.su \n"
"\n"
"Required Parameters:\n"
"\n"
"\nhznfile=            Emerald City format horizon file"
"\nhzns=               number of horizons in file"
"\nhorizon_name=age    names and ages of horizons"
"\n"
"Optional parameters:\n"
"\n"
"\nsalt=        name of file containing salt polygons in SeisWorks"
"\n             fault polygon format"
"\n"
"\nzscale=1e-3  scale factor to apply to trace header dt value"
"\n"
"Decompaction to depositional volume using an exponential compaction\n"
"trend of the form:\n"
"    rho = (rho_min*rho_max)/((exp(-k_rho*(z-wd))+rho_min)\n"
"\n"
"All the following parameters must be supplied for this option\n"
"\n"
"\nrho_min=       minimum value for density"
"\nrho_max=       maximum value for density"
"\nk_rho=         density trend exponent"
"\n"
"\n The water bottom horizon must be supplied with an age of 0"
"\n"
"\nNotes: "
"\n"
"\n The horizons must extend the entire length of the input arbline "
"\n which cannot cross itself."
"\n"
"\nExample:"
"\n"
"\n susedrate <input.su >output.su \\"
"\nrho_min=1.8                     \\"
"\nrho_max=2.65                    \\"
"\nk_rho=1.2e-4                    \\"
"\nsalt=salt.ply                   \\"
"\nhzns=5                          \\"
"\nhzn_wb=0                        \\"
"\nhzn_a1=5.5                      \\"
"\nhzn_a2=7.5                      \\"
"\nhzn_a3=9.5                      \\"
"\nhzn_a4=15.5                     \\"
"\n"
"\n"
"\n";


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "su.h"
#include "par.h"

int InPolygon( float X0 ,float Y0 ,float* X ,float* Y ,int n );
int agecmp( const void* e1 ,const void* e2 );

segy tr;
segy out;

typedef struct {
   float* depth;
   float  age;
}HRZN;

#define MAX_HRZ 100

int main( int argc ,char* argv[] ){

   /*---------*/
   /* horizon */
   /*---------*/

   char    hrzname[1024];
   char*   hznfile;
   FILE*   hrzfp;
   int     nhrz = 0;
   float** hrz_z = 0;
   float   line;
   float   xline;

   HRZN    hrz[MAX_HRZ];

   /*------*/
   /* salt */
   /*------*/

   char*   salt;
   FILE*   saltfp;

   float   idx;
   float   first_x;
   float   first_y;
   float   first_z;

   /* polygon buffers */

   float*  saltpoly_i;
   float*  saltpoly_m;
   float*  saltpoly_x;
   float*  saltpoly_y;
   float*  saltpoly_z;

   /* input pointers */

   float*  iptr;
   float*  mptr;
   float*  xptr;
   float*  yptr;
   float*  zptr;

   /* polygon arrays */

   float** salt_x;
   float** salt_y;
   float** salt_z;
   float** salt_idx;
   float** salt_min;
   int*    salt_n;

   /* polygon counts */

   int     npolygons = 0;
   int     nlines;

   /* decompaction */

   float  rho;
   float  rho_min=0;
   float  rho_max=0;
   float  k_rho=0;

   float factor = 1.0;

   /*---------------------*/
   /* seismic trace input */
   /*---------------------*/

   segy** trace;
   segy*  trace_buf;
   int    n_traces;

   /*---------------*/
   /* miscellaneous */
   /*---------------*/

   char   buf[1024];

   float  scaleco;
   float  zscale = 1.0e-3;
   float  depth;

   double sx;
   double sy;
   double dx;

   double x;
   double y;
   double z;

   int i;
   int j;
   int k;
   int p;
   int start;

   char* ptr;

   FILE* dbgfp=0;

   int debug = 0;

   /*----------------*/
   /* hook up getpar */
   /*----------------*/

   initargs(argc,argv);
   askdoc(0);

   if( !getparint( "hrzns" ,&nhrz )  ){
      err( "number of horizons must be specified" );

   }else if( nhrz > MAX_HRZ ){
      fprintf( stderr ,"maximum horizons is %d\n" ,MAX_HRZ );
      exit(-1);

   }

   /*-----------------------------*/
   /* check compaction parameters */
   /*-----------------------------*/

   i = 0;

   if( getparfloat( "rho_min" ,&rho_min )){
      i++;
   }

   if( getparfloat( "rho_max" ,&rho_max )){
      i++;
   }

   if( getparfloat( "k_rho" ,&k_rho )){
      i++;
   }

   if( i > 0 && i < 3 ){
      fprintf( stderr ,"All compaction arguments must be specified\n" );
      exit(-1);
   }

/*--------------------------------------------------------------------*\
   Read trace headers. This is complicated by the use of a staticly
   defined data member in the SU trace structure . First find out
   how much space we need. Then allocate the space and read all the
   traces using a pointer to the appropriate location.
\*--------------------------------------------------------------------*/

   if( !(n_traces=fgettra( stdin ,&tr ,0 )) ){
      err( "Unable to get first trace!" );
   }

   if( tr.scalco < 0 ){
      scaleco = -1.0 / tr.scalco;

   }else if( tr.scalco > 0 ){
      scaleco = tr.scalco;

   }else{
      scaleco = 1.0;
   }

   if( !(trace=calloc( n_traces ,sizeof(segy*) )) ){
      err( "Unable to allocate trace table" );
   }
   if( !(trace_buf=calloc( n_traces ,240+tr.ns*sizeof(float) )) ){
      err( "Unable to allocate trace buffer" );
   }

   ptr = (char*)trace_buf;
   for( i=0; i<n_traces; i++ ){
      fgettra( stdin ,(segy*)ptr ,i );
      trace[i] = (segy*)ptr;
      ptr += 240+tr.ns*sizeof(float);
   }

/*--------------------------------------------------------------------*\
   Allocate the horizon buffer space
\*--------------------------------------------------------------------*/

   hrz_z=(float**)ealloc2( n_traces ,nhrz+1 ,sizeof(float));

   memset( (char*)hrz ,0 ,sizeof(hrz) );

   if( !getparstring( "hznfile" ,&hznfile ) ){
      err( "missing hznfile=" );
   }

   hrzfp=efopen( hznfile ,"r" );
   fprintf( stderr ,"Processing %s\n" ,hznfile );

   i=0;

   while( !feof(hrzfp) && fgets( buf ,sizeof(buf) ,hrzfp ) ){

      if( buf[0] == '#' || buf[0] == '@' ){
         continue;

      }else if( !strcmp( buf ,"EOD\n" ) ){
         i++;

      }else if( strcspn( buf ," \t" ) == strlen( buf ) ){
         sscanf( buf ,"%s" ,hrzname );

         if( !getparfloat( hrzname ,&hrz[i].age ) ){
            fprintf( stderr ,"missing age for %s" ,hrzname );
            exit(-1);
         }
 
         hrz[i].depth=hrz_z[i];

      }else{
         sscanf( buf ,"%f%f%f%f%f" ,&line ,&xline ,&x ,&y ,&depth );

         ptr = (char*)trace_buf;

         for( j=0; j<n_traces; j++ ){
            if(   ((segy*)ptr)->fldr == (int)rint(line)
               && ((segy*)ptr)->cdp  == (int)rint(xline) ){
               hrz_z[i][j] = depth;
            }
            ptr += 240+tr.ns*sizeof(float);
         }
      }

   }

   nhrz=i;

   qsort( hrz ,nhrz ,sizeof(HRZN) ,agecmp );

   if( (ptr=getenv( "HORIZ")) && (dbgfp=fopen( ptr ,"w")) ){

      for(i=0;i<nhrz;i++) {

         for( j=0; j<n_traces; j++ ){

            fprintf( dbgfp ,"%8d "    ,j               );
            fprintf( dbgfp ,"%8d "    ,i               );
            fprintf( dbgfp ,"%10.2f " ,hrz[i].age      );
            fprintf( dbgfp ,"%6.2f "  ,hrz[i].depth[j] );
            fprintf( dbgfp ,"%6.2f "  ,hrz_z[i][j]     );

            fprintf( dbgfp ,"\n" );
         }

         fprintf( dbgfp ,"\n" );

      }

      fclose( dbgfp );

   }

/*--------------------------------------------------------------------*\
   Read the salt polygons. This is complicated by the use of X-Y-Z
   coordinates for the polygons rather than the LINE-XLINE-Z used
   for the seismic trace loading.

   Determine approximate total number of lines in polygon file and
   allocate space for twice as many points. This wastes a bit of
   space, but not enough to worry about.  Note that we don't
   know how many points are in a polygon until we read the last point
   in the polygon.  The method of determining the file size is dumb,
   but so widely used in SU there's little point in doing better.
\*--------------------------------------------------------------------*/

   if( getparstring( "salt" ,&salt ) ){

      saltfp=efopen(salt,"r");
      fprintf( stderr ,"Processing %s\n" ,salt );

      fgets( buf ,sizeof(buf) ,saltfp );
      fseek( saltfp ,0 ,SEEK_END );
      nlines = ftell(saltfp) / strlen(buf);

      saltpoly_x  = (float*)ealloc1( 2*nlines ,sizeof(float) );
      saltpoly_y  = (float*)ealloc1( 2*nlines ,sizeof(float) );
      saltpoly_z  = (float*)ealloc1( 2*nlines ,sizeof(float) );
      saltpoly_i  = (float*)ealloc1( 2*nlines ,sizeof(float) );
      saltpoly_m  = (float*)ealloc1( 2*nlines ,sizeof(float) );

      salt_n  = (int*)  ealloc1(   nlines ,sizeof(int)   );

      salt_x   = (float**)ealloc1( nlines ,sizeof(float*) );
      salt_y   = (float**)ealloc1( nlines ,sizeof(float*) );
      salt_z   = (float**)ealloc1( nlines ,sizeof(float*) );
      salt_idx = (float**)ealloc1( nlines ,sizeof(float*) );
      salt_min = (float**)ealloc1( nlines ,sizeof(float*) );

      salt_x[0]   = saltpoly_x;
      salt_y[0]   = saltpoly_y;
      salt_z[0]   = saltpoly_z;
      salt_idx[0] = saltpoly_i;
      salt_min[0] = saltpoly_m;

      j = 0;
      k = 0;

      xptr = salt_x[0];
      yptr = salt_y[0];
      zptr = salt_z[0];

      iptr = salt_idx[0];
      mptr = salt_min[0];

      fseek( saltfp ,0 ,SEEK_SET );

      while( !feof( saltfp ) && fgets( buf ,sizeof(buf) ,saltfp ) ){

         sscanf( &(buf[0])  ,"%12lf" ,&x );
         sscanf( &(buf[12]) ,"%12lf" ,&y );
         sscanf( &(buf[24]) ,"%12lf" ,&z );

/*--------------------------------------------------------------------*\
         Check the value in card column 43 to see if we've reached the end
         of a polygon.  If so close the polygon by repeating the first 
         point.  
\*--------------------------------------------------------------------*/

         if( !(strncmp( &(buf[42]) ,"1" ,1 )) ){

            /*------------------*/
            /* start of polygon */
            /*------------------*/
          
            *xptr     = x;
            *yptr     = y;
            *zptr     = z;
            *mptr     = 1.0e30;

            first_x   = x;
            first_y   = y;
            first_z   = z;

            salt_x[j]   = xptr++;
            salt_y[j]   = yptr++;
            salt_z[j]   = zptr++;
            salt_idx[j] = iptr++;
            salt_min[j] = mptr++;

            k=1;

         }else if( !(strncmp( &(buf[42]) ,"3" ,1 )) ){

            /*----------------*/
            /* end of polygon */
            /*----------------*/

            *xptr++   = x;
            *yptr++   = y;
            *zptr++   = z;
            *mptr++   = 1.0e30;
             iptr++;

            *xptr++   = first_x;
            *yptr++   = first_y;
            *zptr++   = first_z;
            *mptr++   = 1.0e30;
             iptr++;

            salt_n[j] = k+2;
            j++;

         }else{

            /*------------------------------*/
            /* interior of polygon polyline */
            /*------------------------------*/

            *xptr++   = x;
            *yptr++   = y;
            *zptr++   = z;
            *mptr++   = 1.0e30;

            iptr++;
            k++;

         }

      }

      npolygons = j;

/*--------------------------------------------------------------------*\
      Determine the trace index to use for the X coordinate of the salt
      polygon vertices. The strategy is to find the minimum distance
      from the point to a trace.
\*--------------------------------------------------------------------*/

      ptr = (char*)trace_buf;

      for( i=0; i<n_traces; i++ ){

         sx = ((segy*)ptr)->sx*scaleco;      
         sy = ((segy*)ptr)->sy*scaleco;      

         for( j=0; j<npolygons; j++ ){

            for( k=0; k<salt_n[j]; k++ ){
           
               dx = sqrt( pow( (sx - salt_x[j][k]) ,2.0 )
                        + pow( (sy - salt_y[j][k]) ,2.0 ) );
               if( dx < salt_min[j][k] ){
                   salt_min[j][k] = dx;
                   salt_idx[j][k] = i;
               }

            }
         }

         ptr += 240+tr.ns*sizeof(float);

      }

/*--------------------------------------------------------------------*\
      dump out the salt polygons in X-Y and trace index coordinates
      so the coordinate transform can be checked.
\*--------------------------------------------------------------------*/

      if( (ptr=getenv( "POLY")) && (dbgfp=fopen( ptr ,"w")) ){

         for( i=0; i<npolygons; i++ ){

            for( j=0; j<salt_n[i]; j++ ){

               fprintf( dbgfp ,"%10.1f " ,salt_x[i][j] );
               fprintf( dbgfp ,"%10.1f " ,salt_y[i][j] );
               fprintf( dbgfp ,"%10.1f " ,salt_z[i][j] );
               fprintf( dbgfp ,"%10.1f " ,salt_min[i][j] );
               fprintf( dbgfp ,"%10.1f " ,salt_idx[i][j] );

               fprintf( dbgfp ,"%10.1f " 
                              ,trace[(int)salt_idx[i][j]]->sx*scaleco );
               fprintf( dbgfp ,"%10.1f " 
                              ,trace[(int)salt_idx[i][j]]->sy*scaleco );

               fprintf( dbgfp ,"\n" );

            }
            fprintf( dbgfp ,"\n" );
         }

         fclose( dbgfp );
      }

   }

/*--------------------------------------------------------------------*\
   Create an output attribute trace for each input trace header.  The
   output must be sampled exactly as the input was sampled so that
   SeisWorks can properly make the overlays.
\*--------------------------------------------------------------------*/

   ptr = (char*)trace_buf;

   for( j=0; j<n_traces; j++ ){

      idx = j;

      memcpy( (char*)&out ,ptr ,240 );
      memset( (char*)out.data ,0 ,out.ns*sizeof(float) );

      i = 1;

      start = (hrz[0].depth[j]-out.delrt)/(out.dt*zscale);

      if( start < 0 ){
         start = 0;
      }

      for( k=start; k<out.ns; k++ ){

         z = out.delrt+k*out.dt*zscale;

         while( i < nhrz-1 && z > hrz[i].depth[j] ){
            i++;
         }

         /*-------------------------------*/
         /* check the horizon depth order */
         /*-------------------------------*/

         if( hrz[i].depth[j] < hrz[i-1].depth[j] ){

            continue;
         }

         /*----------------------------------*/
         /* calculate the sedimentation rate */
         /*----------------------------------*/

         if( rho_min > 0.0 ){

            rho = (rho_max*rho_min) / ( (rho_max - rho_min)
                 * exp( -k_rho*(z-hrz[0].depth[j]) ) + rho_min );

            factor = rho /rho_min;

         }

         out.data[k] = factor * (hrz[i].depth[j]-hrz[i-1].depth[j])
                      / (hrz[i].age-hrz[i-1].age);

         /*--------------------------------*/
         /* apply the salt polygon masking */
         /*--------------------------------*/

         for( p=0; p<npolygons; p++ ){

            if( InPolygon( idx ,z ,salt_idx[p] ,salt_z[p] ,salt_n[p]) ){
               out.data[k] = 0.0;
            }
         }
      }

      /*------------------------*/
      /* write the output trace */
      /*------------------------*/

      fputtr( stdout ,&out );
      ptr += 240+tr.ns*sizeof(float);

   }

   return(0);

}

/*--------------------------------------------------------------------*\
   InPolygon() determines if a point is within a polygon denoted by
   a polyline with identical first and last points.

   The basic algorithm is from "Computational Geometry in C", by
   Joseph O'Rourke but has been completely rewritten to make it 
   suitable for industrial use.

   The function returns integer values indicating the location of
   the input point relative to the bounding polygon:

   0 - point is exterior to the polygon
   1 - point is a vertex of the polygon
   2 - point lies on a line segment
   3 - point is strictly interior to the polygon

   Reginald H. Beardsley                            rhb@acm.org
\*--------------------------------------------------------------------*/

#include <stdio.h>

int InPolygon( float X0 ,float Y0 ,float* X ,float* Y ,int n ){

   int i;            /* vertex          */
   int j;            /* adjacent vertex */

   int R = 0;        /* number of right crossings */
   int L = 0;        /* number of left crossings  */

   float x;

   for( i=0; i<n; i++ ){

      if( X[i] == X0 && Y[i] == Y0 ){
         return( 1 );

      }

      j = (i + n - 1) % n;

      if(   ((Y[i] > Y0) && (Y[j] <= Y0)) 
          ||((Y[j] > Y0) && (Y[i] <= Y0)) ){
         
         x = ((X[i]-X0) * (Y[j]-Y0) - (X[j]-X0) * (Y[i]-Y0))
            / ((Y[j]-Y0) - (Y[i]-Y0));

         if( x > 0 ){
            R++;
         }
      }

      if(   ((Y[i] > Y0) && (Y[j] <= Y0)) 
          ||((Y[j] > Y0) && (Y[i] <= Y0)) ){
         
         x = ((X[i]-X0) * (Y[j]-Y0) - (X[j]-X0) * (Y[i]-Y0))
            / ((Y[j]-Y0) - (Y[i]-Y0));

         if( x < 0 ){
            L++;
         }
      }

   }

   if( (R % 2) != (L % 2) ){
      return( 2 ); 
   }

   if( (R % 2) == 1 ){
      return( 3 );

   }else{
      return( 0 );

   }
}

int agecmp( const void* e1 ,const void* e2 ){

   if( ((HRZN*)e1)->age < ((HRZN*)e2)->age ){
      return( -1 );

   }else if( ((HRZN*)e1)->age > ((HRZN*)e2)->age ){
      return( 1 );

   }else{
      return( 0 );

   }
}
