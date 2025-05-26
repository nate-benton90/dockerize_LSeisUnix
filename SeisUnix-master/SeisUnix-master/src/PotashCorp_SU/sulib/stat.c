#include "suhdr.h"


float fract_D( float *ar, int n,int minl,int maxl,int dl)

/* compute the fractal dimensions of an array
   with the divider method */
/* a - array
   n - length 
   minl minimum lenght 
   maxl maximum length 
 */
{
	int l,i1,i2,i;
	double sum;
	float *ll,*ls,*sig;
	float a,b,siga,sigb,chi2,q;
	int nm;
	
	nm = (maxl-minl)/dl;
	
	ll = ealloc1float(nm);
	ls = ealloc1float(nm);
	
	for(i=0,l=minl;i<nm;i++,l+=dl) {
		sum=0.0;
		i1=0;
		i2=l;	
		while(i2<n) {
			sum += distance((float)i1,ar[i1],(float)i2,ar[i2]);
			i1=i2;
			i2+=l;
		}
		ll[i] = log((float)l);
		ls[i] = log(sum);
	}
	fit(ll,ls,nm,sig,0,&a,&b,&siga,&sigb,&chi2,&q);
	return(1.0-b);
}
   
