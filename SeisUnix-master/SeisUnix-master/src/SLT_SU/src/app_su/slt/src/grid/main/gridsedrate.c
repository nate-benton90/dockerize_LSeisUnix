
#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDSEDRATE - 3D sedimentation rate grid builder \n"
"\n"
"gridsedrate	 nz= fz= dz= hrz= hrz= [hrz=] age= age= [age=] >outfile \n"
"\n"
"Required Parameters:\n"
"outfile=       name of the output 3D attribute grid file (velocity) \n"
"hrz=           name of the 2D depth grids defining the layer boundaries \n"
"               (specified as hrz=grid1 hrz=grid2 ... hrz=gridN) \n"
"               (order from shallow to deep) \n"
"age=           age of horizon (MA)\n"
"\n"
"nz=            number of depth samples in output grid\n"
"dz=            output grid depth sample rate\n"
"fz=            first output grid depth sample\n"
"\n"
"Optional parameters:\n"
"\n"
"Decompaction to depositional volume using an exponential compaction\n"
"trend of the form:\n"
"    rho = (rho_min*rho_max)/((exp(-k_rho*(z-wd))+rho_min)\n"
"\n"
"All the following parameters must be supplied for this option\n"
"\n"
"rho_min=       minimum value for density\n"
"rho_max=       maximum value for density\n"
"k_rho=         density trend exponent\n"
"wd=            name of water bottom horizon grid\n"
"\n"
"\n";

int main(int argc, char **argv)
{

   FILE* hrzfp;
   FILE* outfp;

   char* hrz;
   char* outfile;
   
   float* zs   = 0;
   float* age  = 0;
   float* grid = 0;

   int nage = 0;
   int nhrz = 0;

   float d1;
   float o1;
   int   i1;
   int   n1;

   float d2;
   float o2;
   int   i2;
   int   n2;

   float d3;
   float o3;
   int   i3;
   int   n3;

   int i;
   int q;
   int k;

   float null=-999.0;
   float gmin;
   float gmax;
   float z;

   usghed usghout;
   usghed usghhrz;

   /*----------------*/
   /* hook up getpar */
   /*----------------*/

   initargs(argc,argv);
   askdoc(0);

   /*----------------*/
   /* get parameters */
   /*----------------*/

   if(getparstring("outfile",&outfile)) {
      outfp = efopen(outfile,"w");

   } else {
      outfp = stdout;

   }

   if( (nhrz = countparname("hrz")) == 1 ){
      err(" at least 2 horizons are needed \n");
   }

   if( (nage = countparname("age")) != nhrz ){
      err(" %d ages do not match %d horizons \n",nage ,nhrz );
   }

   if( !getparfloat( "fz" ,&o1 ) ){
      err( "fz= required" );
   }

   if( !getparfloat( "dz" ,&d1 ) ){
      err( "dz= required" );
   }

   if( !getparint( "nz" ,&n1 ) ){
      err( "nz= required" );
   }

   /*----------*/
   /* get ages */
   /*----------*/

   age = (float*) emalloc( nage*sizeof(float) );

   for( i=0; i<nage; i++ ){
      getnparfloat( i+1 ,"age" ,&(age[i]) );
   }

   for(i=0;i<nhrz;i++) {

      getnparstring(i+1,"hrz",&hrz);
      hrzfp=efopen(hrz,"r");
      fprintf( stderr ,"Processing %s\n" ,hrz );

      if( fgetusghdr(hrzfp,&usghhrz) ){
         err(" error reading %s gridheader\n" ,hrz);
      }

      file2g(hrzfp);
   
      /*--------------------*/
      /* memory allocations */
      /*--------------------*/

      if( zs == grid ){ 
   
         d2 = usghhrz.d1;
         d3 = usghhrz.d2;
   
         n2 = usghhrz.n1;
         n3 = usghhrz.n2;
   
         o2 = usghhrz.o1;
         o3 = usghhrz.o2;
      
         gmin = usghhrz.gmin;
         gmax = usghhrz.gmax;

         zs   = (float*) emalloc(n2*n3*nhrz*sizeof(float));
         grid = (float*) emalloc(n1*sizeof(float));
      }

      if(usghhrz.n1!=n2) err("check hrz %s header n1 ",hrz);
      if(usghhrz.n2!=n3) err("check hrz %s header n2 ",hrz);
      if(usghhrz.o1!=o2) err("check hrz %s header o1 ",hrz);
      if(usghhrz.o2!=o3) err("check hrz %s header o2 ",hrz);
      if(usghhrz.d1!=d2) err("check hrz %s header d1 ",hrz);
      if(usghhrz.d2!=d3) err("check hrz %s header d2 ",hrz);

      efseek(hrzfp,0,0);
      efread(zs+i*n2*n3,sizeof(float),n2*n3,hrzfp);
      efclose(hrzfp);
   }

   /*-------------*/
   /* output grid */
   /*-------------*/

   for(i3=0;i3<n3;i3++) {

      for(i2=0;i2<n2;i2++) {

         z = o1;
         k = i2 + i3*n2;
         q = 0;

         for(i1=0;i1<n1;i1++) {

            z += d1;

            while( q < nhrz-2 && z > zs[k + (q+1) * n2 * n3] ){
               q++;
#if 0
               if( zs[k + (q+1) * n2 * n3] == null ){
                  goto output;
               }
#endif
            }

            grid[i1] = 0.0;

            if( zs[k+q*n2*n3] != null 
             && zs[k+(q+1)*n2*n3] != null 
             && z > zs[k+q*n2*n3] 
             && z <= zs[k+(q+1)*n2*n3] 
             && (age[q+1]-age[q]) > 0 ){
               grid[i1] = 1e6*(zs[k+(q+1)*n2*n3] - zs[k+q*n2*n3]) / (age[q+1]-age[q]);

            }
         }

         if(i2==0 && i3==0) {
            gmin = grid[0];
            gmax = grid[0];
         }

         for(i1=0;i1<n1;i1++) {
            if(gmin>grid[i1]) gmin = grid[i1];
            if(gmax<grid[i1]) gmax = grid[i1];
         }

         output:

         efwrite(grid,sizeof(float),n1,outfp);

      }
   }

   memset( &usghout ,0 ,sizeof(usghout) );

   usghout.n1=n1;
   usghout.d1=d1;
   usghout.o1=o1;
   
   usghout.n2=n2;
   usghout.d2=d2;
   usghout.o2=o2;
   
   usghout.n3=n3;
   usghout.d3=d3;
   usghout.o3=o3;
   
   usghout.n4=1;
   usghout.n5=1;

   usghout.gtype=0;
   usghout.scale=1;
   usghout.dtype=4;

   usghout.gmin = gmin;
   usghout.gmax = gmax;

   if( fputusghdr(outfp,&usghout) ){
      err( "error writing output gridheader" );
   }
   
   return(0);
}
