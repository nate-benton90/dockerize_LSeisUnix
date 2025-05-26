#include <par.h>
#include <usgrid.h>

  void vsm3d(float ***vel, int n3, int n2, int n1, int iter, int depth,
         float r3, float r2, float r1, float mu, int slowness) ; 
 void wavel(int n1, int n2, int n3, float d1, float d2, float d3, 
	int time, float *wl, float ***v);


string sdoc = 
"\n"
"vsmoo3d - 3D grid velocity smoothing by the damped least squares\n"
"\n"
"\tvsmoo3d infile= outfile= [parameters] \n"
"\n"
"REQUIRED PARAMETERS: \n"
"\n"
"\tinfile= \t the input velocity file (vgrid or plain arrays)\n"
"                         (default to standard input) \n"
"\toutfile= \t the output velocity file (same format as the input)\n"
"                         (default to standard output) \n"
"\n"
"if they are not in the header of input data  n1, n2 and n3 are required  \n"
 "\t n1=   \t number of samples along 1st dimension \n"
"\t n2=   \t number of samples along 2nd dimension \n"
"\t n3=   \t number of samples along 3rd dimension \n"
 "\n"
"OPTIONAL PARAMETERS: \n"
"\n"
"Smoothing parameters (0 = no smootthing) \n"
"\t r1=0.0   \t   operator length in 1st dimension \n"
"\t r2=0.0   \t   operator length in 2nd dimension \n"
"\t r3=0.0   \t   operator length in 3rd dimension \n"
"\n"
"sample intervals (if they are not in the header of input data) \n"
"\td1=1.0\t  1st dimension \n"
"\td2=1.0\t  2nd dimension \n"
"\td3=1.0\t  3rd dimension \n"
"\n"
"iter=2 \t  number of iteration used \n"
 "time=0  \t  which dimension the time axis is (0 = no time axis)\n"
 "depth=1  \t  which dimension the depth axis is (ignored when time>0)\n"
"\n"
"mu=1 \t the relative weight at maximum depth (or time)\n"
"\n"
"verbose=0 \t =1 for printing minimum wavelengths \n"
"slowness=0 \t =1 smoothing on slowness; =0 smoothing on velocity \n"
"\n"
"Note:\n"
"1. The larger the smoothing parameters, the smoother the output velocity.\n"
"   These parameters are lengths of smoothing operators in each dimension.\n"
"2. iter controls the highest order of derivatives to be smoothed in the  \n"
"   output velocity. e.g., iter=2 means derivatives until 2nd order is smoothed.\n"
"3. mu is the multipler of smoothing parameters at the bottom compared to \n"
"   those at the surface. \n" 
"4. Minimum wavelengths of each dimension and the total may be printed  \n"
"   for the resulting output velocity is. To compute these paprameters for \n"
"   the input velocity, use r1=r2=r3=0. \n"
"5. Smoothing directly on slowness works better to preserve traveltime.  \n"
"   So the program optionally converts the input velocity into slowness \n" 
"   and smooths the slowness, then converts back into velocity.  \n"
"\n"
"AUTHOR: Zhenyue Liu, Colorado School of Mines, 07/08/93 \n"
"\n"
;
main(int argc,char **argv) {
 
      int  n1, n2, n3, depth, time, verbose, iter, slowness;
      float r1, r2, r3, d1, d2, d3, *wl, ***vel, mu;
      string infile, outfile;
 
      FILE *invp, *outvp;

      usghed ugh;
      int ierr, n;
      float gmin, gmax;

      /* initialization */
      initargs(argc,argv) ;
      askdoc(1) ;


      /*  get velocity file names      */
      if( getparstring("infile",&infile) ){
      	invp = fopen(infile,"r") ;
	  } else {
		invp = stdin;
	  }
      if( getparstring("outfile",&outfile) ) {
      	outvp= fopen(outfile,"w");
	  } else {
		outvp = stdout;
	  }
 
      /*   open input and output velocity files      */
	  file2g(invp);
	  file2g(outvp);


      /* read in grid header */
      ierr = fgetusghdr(invp, &ugh);
      if(ierr==0) {
      		n1 = ugh.n1;
      		n2 = ugh.n2;
      		n3 = ugh.n3;
		d1 = ugh.d1;
		d2 = ugh.d2;
		d3 = ugh.d3;
      } else {
      		/*-----------get required parameters-----------*/
      		if( !getparint("n1",&n1) ) n1 = 0 ;
      		if( n1 <= 0 ) err("sample number of 1st dimension invalid" ) ;
      		if( !getparint("n2",&n2) ) n2 = 0 ;
      		if( n2 <= 0 ) err("sample number of 2nd dimension invalid" ) ;
       		if( !getparint("n3",&n3) ) n3 = 0 ;
		if( n3 <= 0 ) err("sample number of 3rd dimension invalid" ) ;

       		/*-----------get optional parameters-----------*/
      		if( !getparfloat("d1",&d1) ) d1 = 1.0 ;
       		if( !getparfloat("d2",&d2) ) d2 = 1.0 ;
      		if( !getparfloat("d3",&d3) ) d3 = 1.0 ;
    }

  /*-----------get optional parameters----------*/
      /*   smoothing parameters    */
       if( !getparfloat("r1",&r1) || n1<4) r1 = 0. ;
       if( !getparfloat("r2",&r2) || n2<4) r2 = 0. ;
       if( !getparfloat("r3",&r3) || n3<4) r3 = 0. ;

/*	 scale smoothing parameters	*/
	r1 = (d1>0)?r1/d1:0;
	r2 = (d2>0)?r2/d2:0;
	r3 = (d3>0)?r3/d3:0;
	r1 = 0.5*r1*r1 ;
	r2 = 0.5*r2*r2 ;
	r3 = 0.5*r3*r3 ;
  
      /*   get iteration number for smoothing operator */
	if(!getparint("iter", &iter)) iter = 2;
	if(iter<=0 || iter>3) err("\t iter must be between 1 and 3!\n");

      /*   description for vertical dimension    */
      if(!getparint("time",&time )) time = 0;
      if(!getparint("depth",&depth) ) depth = 1;
	if(time) depth = time;

 
      /*   relative weight at bottom     */
      if(!getparfloat("mu",&mu) ) mu = 1.0;
      if(mu<1) err("mu must not be less than 1 \n");

      /*   smoothing on velocity or slowness     */
      if(!getparint("slowness",&slowness) ) slowness = 0;

      /*   allocate input file    */
      vel  = alloc3float(n1,n2,n3) ;
      wl = alloc1float(4);

      /*   read input velocity file     */
      fseek2g(invp,0,0);
      fread((char *)vel[0][0],sizeof(float),n1*n2*n3,invp);
  
      /*   perform smoothing operation    */
       vsm3d(vel,n3,n2,n1,iter,depth,r3,r2,r1,mu,slowness); 
 
      /*   write output velocity file     */
      fwrite((char *)vel[0][0],sizeof(float),n1*n2*n3,outvp) ;  

      /* find min and max values */
      n = n1 * n2 * n3;
      fminmax(vel[0][0], n, &gmin, &gmax);
      /* update grid header */
      if(ierr!=0) {
      	bzero(&ugh,100);
      	ugh.scale = 1.e-6;
	    ugh.dtype = 4;
      	ugh.n1 = n1;
      	ugh.n2 = n2;
      	ugh.n3 = n3;
      	ugh.d1 = d1;
      	ugh.d2 = d2;
      	ugh.d3 = d3;
      }
      ugh.gmin = gmin;
      ugh.gmax = gmax;
      ierr = 0;
      ierr = fputusghdr(outvp, &ugh);
      if(ierr!=0) err(" error in fputusghdr ");

      if(!getparint("verbose",&verbose)) verbose = 0;
	if(verbose) {
		wavel(n1,n2,n3,d1,d2,d3,time,wl,vel);
 		fprintf(stderr,"minimum wavelengths of smoothed velocity:\n");
		fprintf(stderr,"\tlambda1 = %g,\n", wl[1]);
		fprintf(stderr,"\tlambda2 = %g,\n", wl[2]);
		fprintf(stderr,"\tlambda3 = %g,\n", wl[3]);
		fprintf(stderr,"\tlambda = %g,\n", wl[0]);
  	}
      /*   close input and output files    */
      fclose(invp) ;
      fclose(outvp) ; 
	exit(0);
}
