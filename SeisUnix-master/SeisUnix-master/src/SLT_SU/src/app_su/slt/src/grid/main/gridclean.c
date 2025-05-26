
#include <float.h>
#include <limits.h>

#include "usgrid.h"
#include "par.h"

char    *sdoc =
      "GRIDCLEAN - Removes NaNs & other bad values from grid files \n"
      "\n"
      "gridclean  <grid.input >grid.output         \n"
      "\n"
      "Required parameters:                     \n"
      "None                         \n"
      " \n"
      "Optional parameters:                     \n"
      "min=-FLT_MAX  - remove values less than min \n"
      "max=FLT_MAX   - remove values greater than max\n"
      "\n"
      " Notes: \n"
      " \n"
      " The data is scanned along the fast axis. If a NaN is found \n"
      " it is replaced w/ the previous value (next value at the \n"
      " first sample).  It reports the grid indices where it found \n"
      " the NaN on stderr\n"
      " \n"
      " Similar logic is applied to values less than min or greater \n"
      " than max.\n" 
      "\n" "\n" "AUTHOR:    Reginald H. Beardsley " __DATE__ "\n";

int main(int argc, char **argv)
{
   FILE    *infp = stdin, *outfp = stdout;

   int      n1, n2, n3, n4, n5;
   int      min1, min2, min3, min4, min5;
   int      max1, max2, max3, max4, max5;

   float   *trace;
   int      ierr, i2, i1;

   int      i3, i4, i5;

   float min = -FLT_MAX;
   float max =  FLT_MAX;

   float gmin =  FLT_MAX;
   float gmax = -FLT_MAX;

   usghed   usgh;

   /* initialization */
   initargs(argc, argv);
   askdoc(1);

   getparfloat( "min" ,&min );
   getparfloat( "max" ,&max );

   /* get input grid parameters */
   if (fgetusghdr(infp, &usgh))
      err(" nonstandard grid file ");

   n1 = usgh.n1;
   n2 = usgh.n2;
   n3 = usgh.n3;
   n4 = usgh.n4;
   n5 = usgh.n5;

   if (n2 == 0)
      n2 = 1;
   if (n3 == 0)
      n3 = 1;
   if (n4 == 0)
      n4 = 1;
   if (n5 == 0)
      n5 = 1;

   min1 = min1 - 1;
   min2 = min2 - 1;
   min3 = min3 - 1;
   min4 = min4 - 1;
   min5 = min5 - 1;
   max1 = max1 - 1;
   max2 = max2 - 1;
   max3 = max3 - 1;
   max4 = max4 - 1;
   max5 = max5 - 1;

   trace = (float *) emalloc(n1 * sizeof(float));

   efseek(infp, 0, 0);

   for (i5 = 0; i5 < n5; i5++) {
      for (i4 = 0; i4 < n4; i4++) {
         for (i3 = 0; i3 < n3; i3++) {
            for (i2 = 0; i2 < n2; i2++) {

               efread(trace, sizeof(float), n1, infp);

               for (i1 = 0; i1 < n1; i1++) {
                  if (trace[i1] != trace[i1] ){

                     fprintf( stderr ,"NaN: %d %d %d %d %d\n" ,i1, i2, i3, i4, i5 );
                     if( i1 > 0 ) {
                        trace[i1] = trace[i1 - 1];
                     }else{
                        trace[i1]= trace[i1+1];
                     }
                  }else if( trace[i1] < min ){
                     fprintf( stderr ,"Min: %d %d %d %d %d\n" ,i1, i2, i3, i4, i5 );
                     if( i1 > 0 ) {
                        trace[i1] = trace[i1 - 1];
                     }else{
                        trace[i1]= trace[i1+1];
                     }
                  }else if( trace[i1] > max ){
                     fprintf( stderr ,"Max: %d %d %d %d %d\n" ,i1, i2, i3, i4, i5 );
                     if( i1 > 0 ) {
                        trace[i1] = trace[i1 - 1];
                     }else{
                        trace[i1]= trace[i1+1];
                     }

                  }
                  gmin = gmin > trace[i1] ? trace[i1] : gmin;
                  gmax = gmax < trace[i1] ? trace[i1] : gmax;
               }
               efwrite(trace, sizeof(float), n1, outfp);
            }
         }
      }

   }

   usgh.gmin = gmin;
   usgh.gmax = gmax;

   if (fputusghdr(outfp, &usgh))
      err("error in output gridheader");

   efclose(outfp);
   efclose(infp);

   free(trace);

   exit(0);
}
