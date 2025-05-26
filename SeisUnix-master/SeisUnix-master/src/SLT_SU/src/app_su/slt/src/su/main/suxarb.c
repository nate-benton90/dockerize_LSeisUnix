#include <stdio.h>

#include "segy.h"
#include "su.h"
#include "cwp.h"
#include "par.h"

void changeval(String type, Value *val, int f);
int InPolygon( float X0 ,float Y0 ,float* X ,float* Y ,int n );
int PolylineTransform( 
    float x0       /* X coordinate of input point */
   ,float y0       /* Y coordinate of input point */
   ,float* x       /* X coordinates of polyline vertices */
   ,float* y       /* Y coordinates of polyline vertices */
   ,float* s       /* arc length coordinate of polyline vertices */
   ,int n          /* number of vertices in polyline */
   ,float* il      /* arc length coordinate list for point */
   ,float* xl      /* crossline coordinate list for point */
);


char* sdoc[] = {

 "suxarb - extract arbitrary data based on polyline or polygon\n"
"\n"
"Required parameters\n"
"\n"
"x=          - list of X coordinates for vertices\n"
"y=          - list of Y coordinates for vertices\n"
"            x= & y= lists should be repeated for as many polygons \n"
"            as needed when pass != 0\n"
"\n"
"Optional parameters\n"
"\n"
"xkey=fldr   - X coordinate header key \n"
"ykey=ep     - Y coordinate header key\n"
"pass=0      - polyline mode\n"
"    =1      - pass all points interior to polygon\n"
"    =2      - pass all points exterior to polygon\n"
"\n"
"Optional parameters used in polyline mode only\n"
"\n"
"The following need to be given if the unit increments in the X & Y\n"
"directions are not equal.  For example, if fldr increments by 1 and \n"
"ep increments by 4 to form 25 x 25 m bins specify dx=25.0 & dy=6.25.\n"
"The output binning key will be converted to integers by the scaling\n"
" with the smaller of the two values.\n"
"\n"
"dx=1.0      - unit increment distance in X direction\n"
"dy=1.0      - unit increment distance in Y direction\n"
"\n"
"ilkey=tracl - key for resulting inline index in polyline mode\n"
"xlkey=tracr - key for resulting xline index in polyline mode\n"
"dw=1.0      - distance in X-Y coordinate units of extracted line\n"
"              to pass points to output.  Width of resulting line is\n"
"              2*dw.  Ignored if polygon mode is specified.\n"
"\n"
"Notes:  \n"
"In polyline mode, a single trace may be output multiple times if \n"
"it meets the acceptance criteria (distance from line segment < dw) \n"
"for multiple line segments. However, the headers will be distinct \n"
"and will associate the output trace with a line segment. This \n"
"behavior facilitates creation of 3D supergathers from polyline \n"
"output. Use susort after running in polyline mode.\n"
"\n"
"In polygon mode, the polygon closes itself from the last vertex to\n"
"the first.\n"
"\n"
"Example:\n"
"\n"
"  suxarb \\ \n"
"   <input.su \\ \n"
"   x=10,20,50 \\ \n"
"   y=0,30,60  \\ \n"
"   dw=10 \\ \n"
"   pass=0 \\ \n"
"   file=arb_out.su \n"
"\n"
"Reginald H. Beardsley        "__DATE__"            rhb@acm.org\n"
};

segy tr;

int main( int argc ,char* argv[] ){

   int i;
   int j;

   String xkey = "fldr";
   String xtype;
   Value  xval;
   int    xindex;

   String ykey = "ep";
   String ytype;
   Value  yval;
   int    yindex;
   
   String ilkey = "tracl";
   String iltype;
   Value  ilval;
   int    ilindex;

   String xlkey = "tracr";
   String xltype;
   Value  xlval;
   int    xlindex;
   
   String* outfile;

   float dx = 1.0;
   float dy = 1.0;

   float dw = 1.0;
   int nx;
   int ny;
   
   int nFiles;
   FILE** outfp;

   int nPolygons;
   int pass=0;

   char errString[1024];

   float** xpoly;
   float** ypoly;
   float** spoly;
   int*    npoly;

   float x0;
   float y0;
   float* il;
   float* xl;
   int nl;

   int inside;

   /*----------------*/
   /* get parameters */
   /*----------------*/

   initargs(argc,argv);
   askdoc(1);

   /*--------------------------*/
   /* get the header key names */
   /*--------------------------*/

   getparstring("xkey" ,&xkey); 
   getparstring("ykey" ,&ykey); 

   getparstring("ilkey" ,&ilkey); 
   getparstring("xlkey" ,&xlkey); 

   getparfloat("dw" ,&dw);
   getparfloat("dx" ,&dx);
   getparfloat("dy" ,&dy);

   getparint( "pass" ,&pass );

   /*----------------------------*/
   /* get the number of polygons */
   /*----------------------------*/

   nx = countparname( "x" );
   ny = countparname( "y" );

   if( pass == 0 ){

      if( nx > 1 || ny > 1 ){
         err( "only one set of x= & y= permitted if pass=0" );

      }else if( nx != 1 ){
         err( "x values missing for polyline" );

      }else if( ny != 1 ){
         err( "y values missing for polyline" );

      }else{
         nPolygons = 1;

      }

   }else if( nx > ny ){
      err( "y values missing for polygon" );

   }else if( ny > nx ){
      err( "x values missing for polygon" );


   }else{
      nPolygons = nx;

   }

   /*----------------------*/
   /* get output filenames */
   /*----------------------*/

   nFiles = countparname( "file" );

   if( pass == 1 &&  nFiles != nPolygons ){
      err( "One output filename must be given for each polygon" );

   }else if( pass == 2 && nFiles != 1 ){
      err( "Only one output filename may be given when pass=2" );

   }

   if( !(outfile=calloc(nFiles ,sizeof(char*) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );

   }

   if( !(outfp=calloc(nFiles ,sizeof(FILE*) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );

   }

   for( i=0; i<nFiles; i++ ){
      getnparstring( i+1 ,"file" ,&outfile[i] );

   }

   /*---------------------------------------*/
   /* allocate space for polygon boundaries */
   /*---------------------------------------*/

   if( !(xpoly=calloc(nPolygons ,sizeof(float*) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );
   
   }else if( !(ypoly=calloc(nPolygons ,sizeof(float*) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );

   }else if( !(spoly=calloc(nPolygons ,sizeof(float*) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );

   }else if( !(npoly=calloc( nx ,sizeof(int) )) ){
      sprintf( errString 
              ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
      err( errString );


   }
   
   /*---------------------------------------------*/
   /* allocate space for vertices of each polygon */
   /*---------------------------------------------*/

   for( i=0; i<nPolygons; i++ ){

      nx=countnparval( i+1 ,"x" ); 
      ny=countnparval( i+1 ,"y" ); 

      if( nx > ny ){
         sprintf( errString ,"Y values missing from polygon %d" ,i+1 );
         err( errString );

      }else if( ny > nx ){
         sprintf( errString ,"X values missing from polygon %d" ,i+1 );
         err( errString );

      }else if( !(xpoly[i]=calloc( nx+1 ,sizeof(float) )) ){
         sprintf( errString 
                 ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
         err( errString );

      }else if( !(ypoly[i]=calloc( nx+1 ,sizeof(float) )) ){
         sprintf( errString 
                 ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
         err( errString );

      }else if( !(spoly[i]=calloc( nx+1 ,sizeof(float) )) ){
         sprintf( errString 
                 ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
         err( errString );

      }else{

         /*-----------------------*/
         /* read polygon vertices */
         /*-----------------------*/

         npoly[i] = nx;
         getnparfloat( i+1 ,"x" ,xpoly[i] );
         getnparfloat( i+1 ,"y" ,ypoly[i] );

         /*-------------------------*/
         /* close polygon if needed */
         /*-------------------------*/

         if( pass != 0 

            && (xpoly[i][nx-1] != xpoly[i][0]
             || ypoly[i][ny-1] != ypoly[i][0])  ){

            xpoly[i][nx] = xpoly[i][0];
            ypoly[i][ny] = ypoly[i][0];
            npoly[i] = nx + 1;

         }else if( !(il=calloc( nx ,sizeof(float) )) ){
            sprintf( errString 
                    ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
            err( errString );

         }else if( !(xl=calloc( nx ,sizeof(float) )) ){
            sprintf( errString 
                    ,"calloc(3c) failed %s %d" ,__FILE__ ,__LINE__ );
            err( errString );

         }else{

            /*-------------*/
            /* scale units */
            /*-------------*/

            for( j=0; j<npoly[i]; j++ ){

               xpoly[i][j] *= dx;
               ypoly[i][j] *= dy;

            }

         }

         /*----------------------------------*/
         /* calculate arc length to vertices */
         /*----------------------------------*/

         for( j=1; j<npoly[i]; j++ ){

            spoly[i][j] = spoly[i][j-1]
                      + sqrt( pow((xpoly[i][j]-xpoly[i][j-1]) ,2.0) 
                            + pow((ypoly[i][j]-ypoly[i][j-1]) ,2.0) );

         }
      }
   }

   /*---------------------------------------------------*/
   /* report the polyline or polygon vertices to stderr */
   /*---------------------------------------------------*/

   for( i=0; i<nPolygons; i++ ){

      fprintf( stderr ,"Polygon: %4d " ,i+1        );
      if( nFiles > i ){
         fprintf( stderr ,"Filename: %s " ,outfile[i] );
      }

      fprintf( stderr ,"\n" );

      for( j=0; j<npoly[i]; j++ ){

         fprintf( stderr ,"%14s"        ,""          );
         fprintf( stderr ,"Vertex: %d " ,j+1         );
         fprintf( stderr ,"X: %f "      ,xpoly[i][j] );
         fprintf( stderr ,"Y: %f "      ,ypoly[i][j] );
         fprintf( stderr ,"S: %f "      ,spoly[i][j] );

         fprintf( stderr ,"\n" );
      }
      fprintf( stderr ,"\n" );
   }

   /*-----------------------*/
   /* open the output files */
   /*-----------------------*/

   for( i=0; i<nFiles; i++ ){
      if( !(outfp[i]=fopen( outfile[i] ,"w" )) ){
         sprintf(  errString 
                   ,"fopen(3c) failed %s %d" ,__FILE__ ,__LINE__ );
         err( errString );
      }

   }

   /*---------------------*/
   /* read in first trace */
   /*---------------------*/

   if (!fgettr(stdin ,&tr) ){
      err("can't get first trace");
   }

   xtype  = hdtype(xkey);
   xindex = getindex(xkey);

   ytype  = hdtype(ykey);
   yindex = getindex(ykey);

   iltype  = hdtype(ilkey);
   ilindex = getindex(ilkey);

   xltype  = hdtype(xlkey);
   xlindex = getindex(xlkey);
   
   /*----------------------------*/
   /* process all the input data */
   /*----------------------------*/

   do {
   
      gethval(&tr, xindex, &xval);
      x0 = vtof(xtype, xval);

      gethval(&tr, yindex, &yval);
      y0 = vtof(ytype, yval);

      if( pass == 0 ){

         x0 *= dx;
         y0 *= dy;

         /*---------------------------------*/
         /* polyline transform & extraction */
         /*---------------------------------*/

         nl = PolylineTransform( x0 
                           ,y0 
                           ,xpoly[0] 
                           ,ypoly[0] 
                           ,spoly[0]
                           ,npoly[0]
                           ,il
                           ,xl
                          );

         for( j=0; j<nl; j++ ){

            if( fabs(xl[j]) <= dw ){

               changeval( iltype ,&ilval ,(int) il[j] );
               puthval( &tr ,ilindex ,&ilval );

               changeval( xltype ,&xlval ,(int) xl[j] );
               puthval( &tr ,xlindex ,&xlval );
            
               fputtr( outfp[0] ,&tr );
            }

         }

      }else{

         /*--------------------*/
         /* polygon extraction */
         /*--------------------*/

         if( pass == 1 ){

            for( i=0; i<nPolygons; i++ ){

               inside = InPolygon( x0 
                                  ,y0 
                                  ,xpoly[i] 
                                  ,ypoly[i] 
                                  ,npoly[i] 
                                 );

                if( inside ){
                  fputtr( outfp[i] ,&tr );
  
                }

            }

         }else if( pass == 2 ){

            for( i=0; i<nPolygons; i++ ){

               inside = InPolygon( x0 
                                  ,y0 
                                  ,xpoly[i] 
                                  ,ypoly[i] 
                                  ,npoly[i] 
                                 );

               if( inside ){
                  goto next;
               }

            }

            fputtr( outfp[0] ,&tr );

         }

      }

      next:;
         
   } while (fgettr(stdin ,&tr));


   return( 0 );
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


/*====================================================================*\
   PolylineTransform() calculates the position of the input point in
   arc length - arc normal coordinates for the polyline specified by
   the input vertex list.  It returns the number of segments for which
   the point lies on a normal to the segment.

   The coordinates are calculated relative to all segments for which 
   input point lies on a normal to the segment.

   The line direction metric starts at the first point.  The line normal
   metric is positive on the right side of the line.

   Reginald H. Beardsley                            rhb@acm.org
\*====================================================================*/

#include <math.h>
#include <string.h>

int PolylineTransform( 
    float x0       /* X coordinate of input point */
   ,float y0       /* Y coordinate of input point */
   ,float* x       /* X coordinates of polyline vertices */
   ,float* y       /* Y coordinates of polyline vertices */
   ,float* s       /* arc length coordinate of polyline vertices */
   ,int n          /* number of vertices in polyline */
   ,float* il      /* arc length coordinate list for point */
   ,float* xl      /* crossline coordinate list for point */
   ){

   float a;   /* intercept of line */
   float b;   /* slope of line     */

   float x_;  /* X coordinate of intersection of normal & segment */
   float y_;  /* Y coordinate of intersection of normal & segment */

   float m_;  /* tangent of angle between m1 & m2                   */
   float m1;  /* vector from first point on segment to intersection */
   float m2;  /* vector from first point on segment to point        */

   float dx;  /* distance of point from line segement along normal  */

   int i;
   int nl = 0;

   memset( il ,0 ,sizeof(float)*n );
   memset( xl ,0 ,sizeof(float)*n );

   for( i=1; i<n; i++ ){

      if( x[i] != x[i-1] ){

         /*---------------------------*/
         /* non-vertical line segment */
         /*---------------------------*/

         b = (y[i] - y[i-1]) / (x[i] - x[i-1] );

         a = y[i-1] - x[i-1] * b;

         x_ = (x0 + (y0 - a) * b) / (1.0 + b*b);

         y_ = a + b * x_;

         m1 = (y_ - y[i-1]) / (x_ - x[i-1]);
         m2 = (y0 - y[i-1]) / (x0 - x[i-1]);
   
         m_ = (m1 - m2) / (1.0 + m1*m2);

         if( x[i] >= x_ && x_ >= x[i-1] 
          || x[i] <= x_ && x_ <= x[i-1] ){

            il[nl] = s[i-1] + sqrt( (y_-y[i-1])*(y_-y[i-1]) 
                                  + (x_-x[i-1])*(x_-x[i-1]) );

            dx = sqrt( (x0-x_)*(x0-x_) + (y0-y_)*(y0-y_) );

            if( m_ >= 0.0 ){
               xl[nl] = dx;

            }else{
               xl[nl] = dx * -1.0;

            }

            nl++;
         }

      }else{

         /*-----------------------*/
         /* vertical line segment */
         /*-----------------------*/

         if( y[i] >= y0 && y0 >= y[i-1] 
          || y[i] <= y0 && y0 <= y[i-1] ){
         
            x_ = x[i];
            y_ = y0;

            il[nl] = s[i-1] + fabs( y_ - y[i-1] );

            dx = fabs( x0 - x_);

            if( y0 >= y[i-1] && x0 > x_ ){
               xl[nl] = dx * -1.0;

            }else{
               xl[nl] = dx;

            }
            nl++;
         }
      }
   }

   return( nl );

}


void changeval(String type, Value *val, int f) {

	switch (*type) {
        case 's':
                err("can't change char header word");
        break;
        case 'h':
                val->h = f;
        break;
        case 'u':
                val->u = f;
        break;
        case 'l':
                val->l = f;
        break;
        case 'v':
                val->v = f;
        break;
        case 'i':
                val->i = f;
        break;
        case 'p':
                val->p = f;
        break;
        case 'f':
                val->f = f;
        break;
        case 'd':
                val->d = f;
        break;
        default:
                err("unknown type %s", type);
        break;
        }
}
