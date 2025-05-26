
#include "usgrid.h"
#include "par.h"
#include "su.h"
#include "segy.h"
#include "header.h"

char    *sdoc =
      "GRID2SU - Converts grid files to SU format\n"
      "\n"
      "grid2su  <input.grid  >output.su         \n"
      "\n"
      "Converts        grids to SU trace format.  The gridheader\n"
      "information is written to the text line header in ASCII.\n"
      "dt=d1 ns=n1 ep=i2*d2+o2 fldr=i3*d3+o3 \n"
      "\n"
      "Note:  the indices, i2 & i3 are zero based, NOT one based\n"
      "\n" "\n" "AUTHOR:    Reginald H. Beardsley \n";

segytrace trace;
segybhdr  b_hdr;
segychdr  t_hdr;

int main(int argc, char **argv)
{
   FILE    *infp = stdin, *outfp = stdout;

   int      n1, n2, n3, n4, n5;

   int      i2, i3, i4, i5;
   int i;

   usghed   usgh;

   char buf[256];

   /*----------------*/
   /* initialization */
   /*----------------*/

   initargs(argc, argv);
   askdoc(1);

   /*---------------------------*/
   /* get input grid parameters */
   /*---------------------------*/

   if (fgetusghdr(infp, &usgh)){
      err(" nonstandard grid file ");
   }

   n1 = usgh.n1;
   n2 = usgh.n2;
   n3 = usgh.n3;
   n4 = usgh.n4;
   n5 = usgh.n5;

   if (n2 == 0){
      n2 = 1;
   }
   if (n3 == 0){
      n3 = 1;
   }
   if (n4 == 0){
      n4 = 1;
   }
   if (n5 == 0){
      n5 = 1;
   }

   
   /*---------------------------------------------------------*/
   /* make sure Zhiming's code will recognize the text header */
   /*---------------------------------------------------------*/
   
   sprintf( buf ,"C 1 CLIENT  grid converted to SU using grid2su" );
   sprintf( t_hdr.crd[0] ,"%-79s\n" ,buf );


   /*------------------------------------------------------------*/
   /* fill in the rest of the text header w/ the gridheader info */
   /*------------------------------------------------------------*/

   i=2;

   sprintf( buf ,"C%2d  scale=%g dtype=%d" 
                ,i ,usgh.scale ,usgh.dtype );
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   sprintf( buf ,"C%2d  n1=%d n2=%d n3=%d n4=%d n5=%d" 
                ,i ,usgh.n1 ,usgh.n2 ,usgh.n3 ,usgh.n4 ,usgh.n5 ); 
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   sprintf( buf ,"C%2d  d1=%g d2=%g d3=%g d4=%g d5=%g" 
                ,i ,usgh.d1 ,usgh.d2 ,usgh.d3 ,usgh.d4 ,usgh.d5 ); 
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   sprintf( buf ,"C%2d  o1=%g o2=%g o3=%g o4=%g o5=%g" 
                ,i ,usgh.o1 ,usgh.o2 ,usgh.o3 ,usgh.o4 ,usgh.o5 ); 
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   sprintf( buf ,"C%2d  dcdp2=%g dline3=%g ocdp2=%g oline3=%g"
                 ,i ,usgh.dcdp2 ,usgh.dline3 ,usgh.ocdp2 ,usgh.oline3);
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   sprintf( buf ,"C%2d  gmin=%g gmax=%g orient=%d gtype=%d"
                 ,i ,usgh.gmin ,usgh.gmax ,usgh.orient ,usgh.gtype);
   sprintf( t_hdr.crd[i-1] ,"%-79s\n" ,buf );
   i++;

   while( i <= 40 ){
      sprintf( t_hdr.crd[i-1] ,"C%2d%76s\n" ,i ,"" );
      i++;

   }

   efwrite( &t_hdr ,1 ,sizeof(t_hdr) ,outfp );

   b_hdr.hns = usgh.n1;
   b_hdr.hdt = usgh.d1;
   b_hdr.format = 1;

   efwrite( &b_hdr ,1 ,sizeof(b_hdr) ,outfp );

   efseek(infp, 0, 0);

   /*-------------------------*/
   /* set up the trace header */
   /*-------------------------*/

   memset( &trace ,0 ,sizeof(trace) );

   trace.ns = usgh.n1;
   trace.dt = usgh.d1;
   trace.trid = 1;

   /*-----------------------------------------*/
   /* loop over all the input grid dimensions */
   /*-----------------------------------------*/

   for (i5 = 0; i5 < n5; i5++) {

      for (i4 = 0; i4 < n4; i4++) {

         for (i3 = 0; i3 < n3; i3++) {

            for (i2 = 0; i2 < n2; i2++) {

               efread(trace.data ,sizeof(float), n1, infp);

               trace.fldr = usgh.o3 + i3*usgh.d3;
               trace.ep   = usgh.o2 + i2*usgh.d2;
               trace.tracl++;
               trace.tracr++;
               fputtr( outfp ,&trace );

            }

         }

      }

   }

   exit(0);
}
