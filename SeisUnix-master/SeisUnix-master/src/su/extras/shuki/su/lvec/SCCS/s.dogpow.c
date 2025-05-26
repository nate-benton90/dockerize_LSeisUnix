h58646
s 00025/00029/00008
d D 1.2 88/11/15 14:04:04 shuki 2 1
c 
e
s 00037/00000/00000
d D 1.1 88/04/14 13:49:54 shuki 1 0
c date and time created 88/04/14 13:49:54 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * gpow - Gain by taking the trace at power gpow
 */
#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT HUGE
#endif
#include "../include/su.h"
D 2
      dogpow(gpow,nsample,ibuf)
       int nsample;
       float *ibuf,gpow;
   /*                                    */ 
   /* check gpow for floating exceptions */       
    {
       int i;
      float maxval;
        for (i=0,maxval=0.0;i< nsample;i++ )
            {
               maxval=MAX(maxval,ibuf[i]);
              maxval=MAX(maxval,-ibuf[i]);
           }    
               maxval=pow(maxval,gpow);
           if (maxval > MAXFLOAT)
                 warn(__FILE__,__LINE__," dogpow: may overflow gpow=%f maxval=%f",gpow,maxval);  
    /* do the power */
       for(i=0;i<nsample;i++)
          {
              if (ibuf[i]< 0.0) 
                  {
                    ibuf[i]=-ibuf[i];  
                     ibuf[i]= pow(ibuf[i],gpow);          
                     ibuf[i]=-ibuf[i];
                  }
           else 
                 ibuf[i] =pow(ibuf[i],gpow); 
          }
       }
E 2
I 2
dogpow(gpow,nsample,ibuf)
int nsample;
float *ibuf,gpow;
{
	int i;
	float maxval;
	/* check gpow for floating exceptions */
	for (i=0,maxval=0.0;i< nsample;i++ ) {
		maxval=MAX(maxval,ibuf[i]);
		maxval=MAX(maxval,-ibuf[i]);
	}
	maxval=pow(maxval,gpow);
	if (maxval > MAXFLOAT)
		warn(__FILE__,__LINE__," dogpow: may overflow gpow=%f maxval=%f",gpow,maxval);
	/* do the power */
	for(i=0;i<nsample;i++) {
		if (ibuf[i]< 0.0) {
			ibuf[i] = -ibuf[i];
			ibuf[i] = pow(ibuf[i],gpow);
			ibuf[i] = -ibuf[i];
		}
		else 
			ibuf[i] = pow(ibuf[i],gpow);
	}
}
E 2
E 1
