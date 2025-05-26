#include "comva.h"
#include "par.h"

/* velocity conversion */
/*
	void vconvert(float *tin, float *vin, int nin, int ivtype, int ittype,
	float *tout, float *vout, int nout, int ovtype, int ottype) {

 input :
	tin	--	input time/depth
	vin	--	input velocity
	nin	--	number of input t and v pairs
	ivtype	--	input velocity type (0=rms; 1=average; 2=interval)
	ittype	--	input time/depth type (0=two-way time; 1=depth)
	tout	--	output time/depth
	nout	--	number of output t and v pairs
	ovtype	--	output velocity type (0=rms; 1=average; 2=interval)
	ottype	--	output time/depth type (0=two-way time; 1=depth)
 output:
	vout	--	output velocity

 author:
	Zhiming Li	      	9/8/92	
*/

void vconvert(float *tin, float *vin, int nin, int ivtype, int ittype,
	float *tout, float *vout, int nout, int ovtype, int ottype) {

	float *v1, *v2, *tz, sum;
	int *indx, i1, i;
	
	indx = (int*) malloc(nout*sizeof(int));

	if(ivtype==ovtype && ittype==ottype) {
		lin1d_(tin,vin,&nin,tout,vout,&nout,indx);
		free(indx);
	} else {
		v1 = (float*) malloc(nin*sizeof(float));
		tz = (float*) malloc(nin*sizeof(float));
		v2 = (float*) malloc(nout*sizeof(float));
		v1[0] = vin[0];	
		if(ivtype==0) {
			for(i1=1;i1<nin;i1++) {
				v1[i1] = vin[i1]*vin[i1]*tin[i1] - 
				 	vin[i1-1]*vin[i1-1]*tin[i1-1];
				if(v1[i1]>0.) {
					v1[i1] = 
					sqrt(v1[i1]/(tin[i1]-tin[i1-1])); 
				} else {
					v1[i1] = v1[i1-1];
				}
			}
		} else if(ivtype==1) {
			for(i1=1;i1<nin;i1++) {
				v1[i1] = vin[i1]*tin[i1] - 
				 	vin[i1-1]*tin[i1-1];
				if(v1[i1]>0.) {
					v1[i1] = v1[i1]/(tin[i1]-tin[i1-1]);
				} else {
					v1[i1] = v1[i1-1];
				}
			}
		} else {
			for(i1=1;i1<nin;i1++) {
				v1[i1] = vin[i1];
			}

		}

		if(ittype!=ottype) {
			/* time/depth conversion if needed */
			if(ittype==0) {
			/* convert to depth, tin is two-way time in ms */
				tz[0] = tin[0]*v1[0]*0.0005;
				for(i1=1;i1<nin;i1++) {
					tz[i1] = tz[i1-1] + 
						(tin[i1]-tin[i1-1])*
						(v1[i1]+v1[i1-1])*0.00025;
				}
			} else {
				/* convert to two-way time (ms), tin is depth */
				tz[0] = tin[0]/v1[0]*2000.;
				for(i1=1;i1<nin;i1++) {
					tz[i1] = tz[i1-1] + 
						(tin[i1]-tin[i1-1])/
						(v1[i1]+v1[i1-1])*4000.;
				}
			}
			lin1d_(tz,v1,&nin,tout,v2,&nout,indx);
		} else {
			lin1d_(tin,v1,&nin,tout,v2,&nout,indx);
		}

		vout[0] = v2[0];
		if(ovtype==0) {
			sum = 0.;
			for(i1=1;i1<nout;i1++) {
				sum = sum + v2[i1]*v2[i1]*(tout[i1]-tout[i1-1]);
				vout[i1] = sqrt( sum / tout[i1] );
			}
		} else if(ovtype==1) {
			sum = 0.;
			for(i1=1;i1<nout;i1++) {
				sum = sum + v2[i1] * (tout[i1]-tout[i1-1]);
				vout[i1] = sum / tout[i1];
			}
		} else {
			for(i1=1;i1<nout;i1++) {
				vout[i1] = v2[i1];
			}
		}

		free(v1);
		free(tz);
		free(v2);
		free(indx);
	}
}

