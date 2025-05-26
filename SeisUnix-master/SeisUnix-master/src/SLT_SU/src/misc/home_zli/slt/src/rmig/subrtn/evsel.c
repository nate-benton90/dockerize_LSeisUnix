#include "rmig.h"

/* event selections at receiver locations */
/* 
   input:
	ng		number of receiver locations
	ne		number of events (reflections)
	maxnr		maximum number of rays per event
	maxnes		maximum number of reflections points per receiver
	mmap		multiple reflections at one reflector (0=no 1=yes)
	sx[ng]		source locations of traces 
	gx[ng]		receiver locations of traces 
	xr[ne][maxnr]	x locations of rays at receivers
	ts[ne][maxnr]	travel time of rays
	xhs[ne][maxnr]	x locations of reflection points of rays
	zhs[ne][maxnr]	z locations of reflection points of rays
	nxt[ne]		number of rays per event
	iray[ne][maxnr] ray flag (1=live 0=dead)
	sortz		sort output ze in ascending order (0=no 1=yes)
	aper		migration aperature (lateral distance from 
			midpoint to output position)
   output:
	xe[ng][maxnes]  x locations of reflection points at locations gx 
	ze[ng][maxnes]  z locations of reflection points at locations gx 
	te[ng][maxnes]  travel time of rays at locations gx 
	nes[ng]  	number of reflection points at locations gx 
	ier[ng][maxnes] ray index of reflection point
	iee[ng][maxnes] event index of reflection point

   author:
	zhiming li	      		1/1/93
*/

void evsel(int ng, int ne, int maxnr, int maxnes, int mmap, 
	float *sx, float *gx,
	float *xr, float *ts, float *xhs, float *zhs, int *nxt, int *iray, 
	float *xe, float *ze, float *te, int *nes, int *ier, int *iee,
	int sortz, float aper)  {

	float tmp, tmp1, xmid, tmp2;
	int ie, ir, one, jg, me, ig, ies;

	int *indx2, *iepg, *itmp; 
	float *sort2;

	sort2 = (float*) malloc(maxnes*sizeof(float));
        indx2 = (int*) malloc(maxnes*sizeof(int));
        itmp = (int*) malloc(maxnes*sizeof(int));
        iepg = (int*) malloc(ne*ng*sizeof(int));

        bzero(nes,ng*sizeof(int));
        bzero(iepg,ng*ne*sizeof(int));
	one = 1;

        for(ie=0;ie<ne;ie++) {
        	for(ir=0;ir<nxt[ie];ir++) {
			if(iray[ie*maxnr+ir]==0) continue;
                	tmp = xr[ie*maxnr+ir];
                        bisear_(&ng,&one,gx,&tmp,&ig);
                        if(tmp>=gx[0] && tmp<=gx[ng-1]) {
                        	if(ig<ng) {
                                	tmp1 = gx[ig-1] - tmp;
                                        tmp2 = gx[ig] - tmp;
                                        if(fabs(tmp1)>fabs(tmp2)) ig = ig + 1;
                                }
                                jg = ig - 1;
                                xmid = 0.5*(sx[jg] + gx[jg]);
				ies = nes[jg];
                                tmp1 = xmid - xe[jg*maxnes+ies];
                                tmp2 = xmid - xhs[ie*maxnr+ir];

				if(fabs(tmp2)>aper) continue;
                                if( (mmap==0 && iepg[jg*ne+ie]!=0 &&
                                	fabs(tmp1)>fabs(tmp2)) ||
                                       	(mmap!=0)           ||
                                        (iepg[jg*ne+ie]==0) ) {

                                        ze[jg*maxnes+ies]=zhs[ie*maxnr+ir];
                                        xe[jg*maxnes+ies]=xhs[ie*maxnr+ir];
                                        te[jg*maxnes+ies]=ts[ie*maxnr+ir];
                                        iee[jg*maxnes+ies]=ie;
                                        ier[jg*maxnes+ies]=ir;
                                        nes[jg] = nes[jg] + 1;
                                        iepg[jg*ne+ie]=1;
                                }
                      	} 
              	}
      	}
	if(sortz==1) {
        	/* sort depth of event into ascending order */
        	for(ig=0;ig<ng;ig++) {
       		me = nes[ig];
                for(ie=0;ie<me;ie++) indx2[ie] = ie;
                	qkisort(me,ze+ig*maxnes,indx2);
                for(ie=0;ie<me;ie++) itmp[ie]=iee[ig*maxnes+indx2[ie]];
                for(ie=0;ie<me;ie++) iee[ig*maxnes+ie]=itmp[ie];
                for(ie=0;ie<me;ie++) itmp[ie]=ier[ig*maxnes+indx2[ie]];
                for(ie=0;ie<me;ie++) ier[ig*maxnes+ie]=itmp[ie];
                for(ie=0;ie<me;ie++) sort2[ie]=ze[ig*maxnes+indx2[ie]];
                for(ie=0;ie<me;ie++) ze[ig*maxnes+ie]=sort2[ie];
                for(ie=0;ie<me;ie++) sort2[ie]=xe[ig*maxnes+indx2[ie]];
                for(ie=0;ie<me;ie++) xe[ig*maxnes+ie] = sort2[ie];
                for(ie=0;ie<me;ie++) sort2[ie]=te[ig*maxnes+indx2[ie]];
                for(ie=0;ie<me;ie++) te[ig*maxnes+ie] = sort2[ie];
		}
    	}          

	free(sort2);
        free(indx2);
        free(iepg);
        free(itmp);

}
