/* general purpose data windowing program */

#include "par.h"


char *sdoc = 
"WINDOW - general-purpose data windowing program			\n"
"\n"
"window [parameters] <input-data >windowed-data				\n" 
"\n"
"Required parameters:							\n"
"n1			number of samples in 1st dimension of input data \n"
"\n"
"Optional parameters:							\n"
"n2=all			number of samples in 2nd dimension of input data \n"
"n3=1			number of samples in 3rd dimension of input data \n"
"o1=1			minimum index of sample in 1st dimension to output \n"
"o2=1			minimum index of sample in 2nd dimension to output \n"
"o3=1			minimum index of sample in 3rd dimension to output \n"
"d1=1			sample increment in 1st dimension to output \n"
"d2=1			sample increment in 2nd dimension to output \n"
"d3=1			sample increment in 3rd dimension to output \n"
"n1o=(n1-o1)/d1+1	number of samples in 1st dimension to output \n"
"n2o=(n2-o2)/d2+1	number of samples in 2nd dimension to output \n"
"n3o=(n3-o3)/d3+1	number of samples in 3rd dimension to output \n"
"dsort=0 		perform trace sort from 3-D data (output one trace \n"
"			   per plane) (0=n 1=y) \n"
"d0=1			first output trace position of the first plane \n" 
"			(effective only dsort=1) 	\n"
"dd=1			increment of trace position of the next plane \n" 
"			(effective only dsort=1)	\n"
"			when dsort=1, output traces are \n"
"				      trace d0 from 1st plane, \n"
"				      trace d0+dd from 2nd plane, \n"
"					...,  			\n"
"				      trace d0+(n3-1)*dd from n3-th plane\n"
"\n"
"AUTHOR:		Zhiming Li		10/25/90   		\n"
;

main(int argc, char **argv)
{
    int n1,n2,n3,o1,o2,o3,d1,d2,d3;
    int i1,i2,i3,n123,n1o,n2o,n3o;
    int dd,d0,dsort;
    float *trace,*traceo;
    FILE *infp=stdin,*outfp=stdout;
    long long m1, m2, m3;
    off_t lofset;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

    file2g(infp);

    if (!getparint("n1",&n1)) err("Must specify n1!\n");
    if (!getparint("n2",&n2)) 
      {
	m1 = 0;
	bcopy(&m1,&lofset,8);
        fseek64(infp,lofset,2);
	lofset = ftell64(infp);
	bcopy(&lofset,&m1,8);
	m1 = m1/sizeof(float)/n1;
        n2 = m1;
	m1 = 0;
	bcopy(&m1,&lofset,8);
        fseek64(infp,lofset,0);
      }
    if (!getparint("n3",&n3)) 
      {
	m1 = 0;
	bcopy(&m1,&lofset,8);
        fseek64(infp,lofset,2);
	lofset = ftell64(infp);
	bcopy(&lofset,&m1,8);
        m1 = m1/sizeof(float)/(n1*n2);
	n3 = m1;
	m1 = 0;
	bcopy(&m1,&lofset,8);
        fseek64(infp,lofset,0);
      }
    if (!getparint("o1",&o1)) o1=1;
    if (!getparint("o2",&o2)) o2=1;
    if (!getparint("o3",&o3)) o3=1;
    if (!getparint("d1",&d1)) d1=1;
    if (!getparint("d2",&d2)) d2=1;
    if (!getparint("d3",&d3)) d3=1;
    if (!getparint("dsort",&dsort)) dsort=0;
    if (!getparint("d0",&d0)) d0=1;
    if (!getparint("dd",&dd)) dd=1;
    /* error checking */
    m1 = 0;
    bcopy(&m1,&lofset,8);
    fseek64(infp,lofset,2);
    lofset = ftell64(infp);
    bcopy(&lofset,&m1,8);
    m1 = m1/sizeof(float);
    n123 = m1;
    m1 = 0;
    bcopy(&m1,&lofset,8);
    fseek64(infp,lofset,0);
    if ( n123 < n1*n2*n3 ) err("check input n1*n2*n3 \n");
    if (o1<1) o1=1; 
    if (o1>n1) o1=n1; 
    if (d1<1) d1=1; 
    if (d1>n1) d1=n1;
    if (o2<1) o2=1;
    if (o2>n2) o2=n3; 
    if (d2<1) d2=1;
    if (d2>n2) d2=n2;
    if (o3<1) o3=1;
    if (o3>n3) o3=n3; 
    if (d3<1) d3=1; 
    if (d3>n3) d3=n3;
    if (!getparint("n1o",&n1o)) n1o = (n1-o1)/d1+1;
    if (n1o >(n1-o1)/d1+1) n1o = (n1-o1)/d1+1;   
    if (!getparint("n2o",&n2o)) n2o = (n2-o2)/d2+1;
    if (n2o >(n2-o2)/d2+1) n2o = (n2-o2)/d2+1;   
    if (!getparint("n3o",&n3o)) n3o = (n3-o3)/d3+1;
    if (n3o >(n3-o3)/d3+1) n3o = (n3-o3)/d3+1;   
    /* allocate space */
    trace = (float*)malloc(n1*sizeof(float));
    traceo = (float*)malloc(n1o*sizeof(float));
    
    /* subsampling input data */
    if (dsort == 0 )
    {
       for(i3=o3;i3<=o3+(n3o-1)*d3;i3=i3+d3)
          {
          m3 = (i3-1)*n2;
          for(i2=o2;i2<=o2+(n2o-1)*d2;i2=i2+d2)
	     {
             m2 = m3 + (i2-1);  
	     m1 = m2*n1*4;
	     bcopy(&m1,&lofset,8);
	     fseek64(infp,lofset,0);
             if (fread(trace,sizeof(float),n1,infp)!=n1)
             err("Error reading input\n");
	     for(i1=0;i1<n1o;i1++) traceo[i1] = trace[o1-1+i1*d1];
             /* write output data */
             fwrite(traceo,sizeof(float),n1o,outfp);
	     }
          }
     }
     else
     {
       for(i3=0;i3<n3;i3++)
          {
          m3 = i3*n2;
	  i2 = d0 + i3*dd;
          m2 = m3 + (i2-1);  
	  m1 = m2*n1*4;
	  if (m2 < n2*n3)
	     {
	     bcopy(&m1,&lofset,8);
	     fseek64(infp,lofset,0);
             if (fread(trace,sizeof(float),n1,infp)!=n1);
             /* write output data */
             fwrite(trace,sizeof(float),n1,outfp);
	     }
          }
     }
}
