#include <stdio.h>
#include "su.h"
#include "par.h"

void segm_bnd(float *val,int n,float inc,float gap,int *smst,int *nsq)
/* Return the segmnet start and end index values of a segmented array */
/* the segmnet ends in indx[i]-indx[i-1]> (float)inc
   input:
   val  float array of values sorted
   n    number of elements in arrays
   inc normal increment of indx; if val[i]-val[i-1] > inc then new segment, unless
   gap is specified 
   gap segments with val[i]-val[i-1] < gap are merged 
   return:
   smst start index of the segments in array
   ns number of segmnets in smst
   array smst has to be allocated large enough to fit n values
*/
{
	int ss=0;		 /* segment start */
	int se=0;		/* segment end */
	int sp=0;		/* segment test */
	int si=0;		/* increment */
	int sq=0;		/* segment number */

	ss=0;
	while(se<n-1) {
		si=1;
		se=ss;
		sp=se+1;
		if(se!=n-1) {
			while(val[se]+inc==val[sp] || val[sp]-val[se]<=gap) {
				if(sp>n-1) break;
				si++;
				se++;
				sp=se+1;
/*				fprintf(stderr,"%d %d %d\n",ss,se,sp); */
			}
		}
		
		smst[sq]=ss;
		fprintf(stderr," Segment # %d",sq);
		fprintf(stderr," %f %f %d %d\n",val[ss],val[se],ss,se);
		sq++;
		ss=sp;
	}
	*nsq=sq;
}
