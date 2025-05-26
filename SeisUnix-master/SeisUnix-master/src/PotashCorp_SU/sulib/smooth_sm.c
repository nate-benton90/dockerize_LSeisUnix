#include <stdio.h>
#include "su.h"
#include "par.h"

void smoothSegmentedArray(float *index,float *val,int n,int sm,int inc,int m)
/***********************************************************************
 smoothSegmentedArray -Smooth a segmented array 
***********************************************************************
 index float array of indexes 
   val  float array of values
   n    number of elements in arrays
   sm smoothing factor
   inc normal increment of index, if index[i]-index[i-1] > inc then new segment
   m method of smoothing
   	1 Gaussian 1d
	2 damped least squares
	3 Sawitzky-Golay
	4 Running average
	5 Running average with triangluar window
	6 median filter
***********************************************************************
Author: Balasz Nemeth
***********************************************************************/
{
	float *p=NULL;
	float *t=NULL;
	float *ts=NULL;
	int *ind=NULL;
	int s1;		/* segment start */
	int s2=0;		/* segment end */
	int si=0;		/* increment */
	int sq=0;		/* segment number */
	int ns;		/* datapoints in segment */
	float *filter=NULL;
	float *data=NULL;
	
	p=ealloc1float(n);
	t=ealloc1float(n);
	ts=ealloc1float(n);
	ind=ealloc1int(n);
	
	{ register int i;
		for(i=0;i<n;i++){
			p[i]=index[i];
			t[i]=val[i];
			ind[i]=i;
		}
	}
	
	qkisort(n,p,ind);	
	
	s1=0;
	while(s2<n-1) {
		ns=1;
		si=1;
		s2=s1+si;
		while(p[ind[s1]]+(float)si*inc==p[ind[s2]]) {
			si++;
			s2=s1+si;
			ns++;
			if(ns==n) break;
		}
		{ register int i,j;
			for(i=s1,j=0;i<s2;i++,j++)
				ts[j]=t[ind[i]];
			/* smoothing */
			switch (m) {
			/* Gaussian */
			case 1 :
				gaussian1d_smoothing (ns,sm,ts);
			break;
			/* damped least squares */
			case 2 :
				dlsq_smoothing(ns,1,0,1,0,ns,sm,20,0,(float**)ts);
			break;
			/* Sawitzky-Golay */ 
			case 3 :
				filter=ealloc1float(2*sm+1);
				data=ealloc1float(ns);
				memcpy((void*) data,(const void *) ts,ns*FSIZE);
				SG_smoothing_filter(2*sm+1,sm,sm,0,4,filter);
				conv (2*sm+1,-sm,filter,ns,0,data,ns,0,ts);
				free1float(filter);
				free1float(data);
			break;
			/* Running average */
			case 4 :
				filter=ealloc1float(2*sm+1);
				data=ealloc1float(ns);
				memcpy((void*) data,(const void *) ts,ns*FSIZE);
				rwa_smoothing_filter (1,sm,sm,filter);
				conv (2*sm+1,-sm,filter,ns,0,data,ns,0,ts);
				free1float(data);
				free1float(filter);
			break;
			/* Running average triangular window*/
			case 5 :
				filter=ealloc1float(2*sm+1);
				data=ealloc1float(ns);
				memcpy((void*) data,(const void *) ts,ns*FSIZE);
				rwa_smoothing_filter (2,sm,sm,filter);
				conv (2*sm+1,-sm,filter,ns,0,data,ns,0,ts);
				free1float(data);
				free1float(filter);
			break;
			/* median filter*/
			case 6 :
				data = ealloc1float(ns);
				if(!ISODD(sm)) sm++;
				filter = ealloc1float(sm);
		
				memcpy((void *) data,(const void *) ts,ns*FSIZE);
				
				{ int it,fl,*index,is,ifl;
				  float *sign;
					sign = ealloc1float(sm);
					index = ealloc1int(sm);
					fl=sm;
					/* No filtering before filter half length distance */
					for(it=fl/2,is=0;it<ns-(fl/2);it++,is++) {
						for(ifl=0;ifl<fl;ifl++) {
							index[ifl]=ifl;
							sign[ifl] = SGN(data[is+ifl]); 
							filter[ifl] = fabs(data[is+ifl]);
						}
						qkisort(fl,filter,index);
						ts[it] = sign[index[fl/2+1]]*filter[index[fl/2+1]];
					}
					free1float(sign);
					free1int(index);
				}
				free1float(data);
				free1float(filter);
				
			break;
			
			default :
				warn(" Non existing filter mode %d\n",m);
			break;
			
			}
				 	
			for(i=s1,j=0;i<s2;i++,j++)
				t[ind[i]]=ts[j];
		}
		/*fprintf(stderr," Segment # %d",sq);
		fprintf(stderr," %10.3f %10.3f %d\n",p[ind[s1]],p[ind[s2-1]],ns); */
		sq++;
		s1=s2;
	}
	
	{ register int i;
		for(i=0;i<n;i++)
			val[i]=t[i];
	}
	free1float(p);
	free1float(t);
	free1float(ts);
	free1int(ind);
}
