/* velocity filtering of VS3D card */


#include "velo.h"
#include "par.h"


char *sdoc = 
"VS3DFILTER - velocity filtering and extension of VS3D cards \n"
"\n"
"vs3dfilter [parameters] <vs3d-cards >vs3d-filtered  				\n" 
"\n"
"Required parameters:						 	\n"
"vs3d-cards=       Name of input dataset containing VS3D cards 		\n"
"vs3d-filtered=    Name of output dataset containing VS3D cards 		\n"
"\n"
"Optional parameters:							\n"
"op=0              0=median filtering; 1=mean filtering  \n" 
"wlen=3            window length (in samples) of filtering \n" 
"                  wlen=1 means no filtering \n"
"tmax=-9999        time (ms) of last sample to extend to \n"
"vtmax=-9999       velocity at last extended sample \n"
"                  when tmax or vtmax equals -9999, no extension \n"
"nvfmax=4096       maximum number of input velocity functions \n"
"ntvmax=256        maximum number of t-v pairs per velocity function \n"
"Notes: \n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	2/22/2002   		\n"    
;

main(int argc, char **argv)
{
   	FILE *infp=stdin,*outfp=stdout;
	int ntvmax,nvfmax,nxy,np;
   	float *xs, *ys, *tpicks, *vpicks;
	int *nps;
	int op=0, wlen=3, w1, h1, iw;
	float tmax, vtmax;
	float x, y;
	int mxy;

	float *to, *vo, *vw, *vi; 
	int ntv, it, ix;
	int i, j1, k1, m1;
	float tmp;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* required parameters */

   	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
	pairs each */
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparint("op",&op)) op = 0;
	if (!getparint("wlen",&wlen)) wlen = 3;
	if (!getparfloat("tmax",&tmax)) tmax = -9999;
	if (!getparfloat("vtmax",&vtmax)) vtmax = -9999;

   	/* arrays used to store all VS3D card's cdp, time and velocity */
   	xs = (float*)emalloc(nvfmax*sizeof(float));
   	ys = (float*)emalloc(nvfmax*sizeof(float));
   	tpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	vpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	nps = (int*)emalloc(nvfmax*sizeof(int));

	to = (float*)emalloc((ntvmax+1)*sizeof(float));
	vo = (float*)emalloc((ntvmax+1)*sizeof(float));
	vw = (float*)emalloc(ntvmax*sizeof(float));
	vi = (float*)emalloc(ntvmax*sizeof(float));

	bzero(nps,nvfmax*sizeof(int));
    	/* read in VS3D cards */
    	nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,ntvmax,nvfmax);
	fprintf(stderr," %d VS3D cards read \n",nxy);
   	if (nxy==0) err("No VS3D card input ! Job aborted");
	
/* filter and output vs3d cards */

	mxy = 0;
	for(ix=0;ix<nxy;ix++) {

		x = xs[ix]; 
		y = ys[ix]; 
		ntv = nps[ix];

		w1 = wlen/2*2+1;

		if(w1>ntv) w1 = ntv;
		h1 = w1 / 2; 

		for(it=0;it<ntv;it++) {
			to[it] = tpicks[it+ix*ntvmax]; 
			vo[it] = vpicks[it+ix*ntvmax]; 
			vi[it] = vpicks[it+ix*ntvmax]; 
		}

		if(w1>1) {
			
			iw = 50*w1/100.;

			for (it=0;it<ntv;it++) {
				i = 0;
				k1 = it - h1;

				for(j1=k1;j1<k1+w1;j1++) {
					m1=j1;
					if(m1<0)m1=0;
					if(m1>ntv-1)m1=ntv-1;
					vw[i] = vi[m1];
					i = i + 1;
				}

				if(op==0) {
					qkfind(iw,w1,vw);
					vo[it] = vw[iw];
				} else {
					tmp = 0.;
					for(i=0;i<w1;i++) {
						tmp = tmp + vw[i];
					}
					vo[it] = tmp/w1;
				}

			}
		}
		if(tmax!=-9999 && vtmax!=-9999) {
			if(tmax>to[ntv-1]) {
				to[ntv] = tmax;
				vo[ntv] = vtmax;
				ntv = ntv + 1;
			}
		}
		/* output VS3D cards */
		if(ntv>0) {
			printvs3d(x,y,ntv,to,vo,outfp);
			mxy = mxy + 1;
		}


	}

	fprintf(stderr," output %d VS3D locations \n",mxy); 
	free(xs);
	free(ys);
	free(tpicks);
	free(vpicks);
	free(nps);
	free(vi);
	free(vw);
	free(vo);
	free(to);

	return 0;

}
