#include "par.h"


char *sdoc = 
"MAXMIN - Find maximum and minimum values of data \n"
"maxmin [parameters] <input-data \n" 
"\n"
"Required parameters: \n"
"n1			number of samples  \n"
"\n"
"Optional parameters: \n"
"n2=all			number of traces \n"
"\n"
"Author:    Z. Li				10/20/1990 \n"
"\n"
;

main(int argc, char **argv)
{
    int n1,i1,n2,i2;
    float *trace;
    float tmp;
    float fama,fami;
    FILE *infp=stdin;
    long long l2; 

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

    if (!getparint("n1",&n1)) err("must specify n1! \n");

    if (!getparint("n2",&n2)) 
      {
      fseek2g(infp,0L,2);
      l2 = ftell2g(infp);
      l2 = l2/sizeof(float)/n1;
      n2 = l2;
      fseek2g(infp,0L,0);
      }
    trace = (float*)malloc(n1*sizeof(float));
    for (i2=0;i2<n2;i2++)
       {
       fread(trace,sizeof(float),n1,infp);
       if (i2==0) 
	  {
	  tmp = trace[0];
	  if ( tmp < 0. ) tmp = - tmp;
	  fama = tmp;
	  fami = tmp;
	  }
       for(i1=0;i1<n1;i1++)  
	{
	tmp = trace[i1]; if(tmp < 0. ) tmp = -tmp;
	if ( fama < tmp ) fama = tmp;
	if ( fami > tmp ) fami = tmp;
	}
       }
    printf("max abs value= %f \n",fama); 
    printf("min abs value= %f \n",fami); 
}
