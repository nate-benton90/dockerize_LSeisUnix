
#include "velo.h"
#include "par.h"

/* print vs3d cards */
/* zhiming li */

void printvs3d(float s, float l,int nvt, float *times,float *vrms,FILE *outfp) {

	int jc,it,ip,npout;

	float *tmp1, *tmp2, p1, p2;
	int nv;


	tmp1 = (float*) emalloc(nvt*sizeof(float));
	tmp2 = (float*) emalloc(nvt*sizeof(float));
	
	tmp1[0] = times[0];
	tmp2[0] = vrms[0]; 
	nv = 0;
	for(it=1;it<nvt;it++) {
		if(times[it]>tmp1[nv]) {
			nv = nv + 1;
			tmp1[nv] = times[it];
			tmp2[nv] = vrms[it];
		} else {
warn("Warning --- time inversions removed at time=%f it=%d time_pre=%f\n",
				times[it],it+1,tmp1[nv]);
		}
	}
	nv = nv + 1;
	if(nv!=nvt) {
warn( "Warning --- %d time inversions removed at s=%g l=%g \n",nvt-nv,s,l); 
	}
	
		
	for (jc=0;jc<(nv+2)/3;jc++) {
       		if( (jc+1)*3 < nv ) {
           		npout = 3;
           	} else {
              		npout = nv - jc*3;
           	}
		if(jc==0) {
           		if(fabs(s)>=99999999.0 || fabs(l)>=99999999.0) { 
				fprintf(outfp, "VS3D    %8.3g%8.3g",s,l);
			} else if(fabs(s)>=999999.9 || fabs(l)>=999999.9) { 
				fprintf(outfp, "VS3D    %-8d%-8d",(int)s,(int)l);
			} else {
           			fprintf(outfp, "VS3D    %8.1f%8.1f",s,l);
			}
		} else {
           		fprintf(outfp, "VS3D                    ");
		}
           	for(ip=0;ip<npout;ip++) {
               		p1 = tmp1[jc*3+ip];
               		p2 = tmp2[jc*3+ip];
               		fprintf(outfp,"%8.1f%8.1f",p1,p2);
           	}
           	fprintf(outfp,"\n");
       	}
	free(tmp1);
	free(tmp2);

}

