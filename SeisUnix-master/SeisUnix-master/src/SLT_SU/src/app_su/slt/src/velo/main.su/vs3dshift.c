/* time shift of VS3D card */


#include "usgrid.h"
#include "velo.h"
#include "par.h"


char *sdoc = 
"VS3DSHIFT - time shift VS3D cards \n"
"\n"
"vs3dshift [parameters] <vs3d-cards >vs3d-shifted  				\n" 
"\n"
"Required parameters:						 	\n"
"vs3d-cards=       Name of input dataset containing VS3D cards 		\n"
"vs3d-shifted=     Name of output dataset containing VS3D cards 		\n"
"datumgrid=        Name of datum grid where vs3d-cards are shifted to	\n"
"or datum=         single value of datume (when datumgrid is given, \n"
"                  datum will be ignored) \n"
"\n"
"Optional parameters:							\n"
"nvfmax=4096       maximum number of velocity functions in input vs3d   \n" 
"                  dataset     						\n" 
"ntvmax=256        maximum number of t-v pairs per velocity functions 	\n"
"                  in input vs3d   \n" 
"                  dataset     						\n" 
"vitype=1          input velocity type (1=interval 0=rms 2=average) \n"
"                  SEE THE NOTES BELOW ON HOW TO SPECIFY vitype FOR \n"
"                  JUST TIME-SHIFT (LEAVE VELOCITY UNCHANGED) \n" 
"Notes: \n"
" 1. When vitype=1, the output VS3D cards will be just a simple time \n"
"    shift (time of VS3D cards will be subtracted by the time in \n"
"    datumgrid). Other types of input velocities will be converted to \n"
"    interval velocity first using Dix's formula, shifted, then \n"
"    converted back to the same type of input velocity. \n"
" 2. If user intend to just shift the time to the new datum, but leave \n"
"    the velocity unchanged (for example, to interpolate/smooth rms \n"
"    or average velocity below water bottom), vitype=1 should be used. \n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	3/07/2000   		\n"    
;

void bilint_(int *n1, int *nx,int *ny,float *x0,float *y0,
	float *dx,float *dy,float *x,float *y,float *f,float *g);

void lin1d_(float *ti,float *vi,int *ni,float *to,float *vo,int *no,int *indx);

main(int argc, char **argv)
{
   	FILE *infp=stdin,*outfp=stdout;
	int ntvmax,nvfmax,nxy,np;
   	float *xs, *ys, *tpicks, *vpicks;
	int *nps, vitype;

	usghed ugh;
	FILE *gfp;
	char *datumgrid;
	float datum;
	float *dgrid;
	int n1,n2;
	float o1,o2,d1,d2;
	float tg, tmp, tmp2;

	int ixy, mxy; 
	float x, y;
	float *t, *v;
	float *to, *vo, *ti, *vi; 
	int *indx;
	float *ito, *ivo;
	int ntv, it, ix,iy;
	int one, npnew;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* required parameters */

   	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
	pairs each */
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparint("vitype",&vitype)) vitype = 1;

   	/* arrays used to store all VS3D card's cdp, time and velocity */
   	xs = (float*)emalloc(nvfmax*sizeof(float));
   	ys = (float*)emalloc(nvfmax*sizeof(float));
   	tpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	vpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	nps = (int*)emalloc(nvfmax*sizeof(int));

	t = (float*)emalloc(ntvmax*sizeof(float));
	v = (float*)emalloc(ntvmax*sizeof(float));
	to = (float*)emalloc(ntvmax*sizeof(float));
	vo = (float*)emalloc(ntvmax*sizeof(float));
	ti = (float*)emalloc(ntvmax*sizeof(float));
	vi = (float*)emalloc(ntvmax*sizeof(float));
	indx = (int*)emalloc(ntvmax*sizeof(int));
	ito = (float*)emalloc(ntvmax*sizeof(float));
	ivo = (float*)emalloc(ntvmax*sizeof(float));

	bzero(nps,nvfmax*sizeof(int));
    /* read in VS3D cards */
    nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,ntvmax,nvfmax);
	fprintf(stderr," %d VS3D cards read \n",nxy);
   	if (nxy==0) err("No VS3D card input ! Job aborted");
	
	/* read in the datum grid file */
	if(!getparstring("datumgrid",&datumgrid)) {
		if(!getparfloat("datum",&datum)) {
			err(" datumgrid or datum must be specified ");
		}
		n1=1; n2=1;
	} else {
		gfp = efopen(datumgrid,"r");
		if(fgetusghdr(gfp,&ugh)!=0) err(" check datumgrid header "); 
		o1 = ugh.o1; d1=ugh.d1; n1=ugh.n1; 
		o2 = ugh.o2; d2=ugh.d2; n2=ugh.n2; 
		dgrid = (float*) emalloc(n1*n2*sizeof(float));
		efseek(gfp,0,0);
		efread(dgrid,sizeof(float),n1*n2,gfp);
	}
/* output vs3d cards */
	one = 1;
	mxy = 0;

	for(ixy=0;ixy<nxy;ixy++) {
		x = xs[ixy];
		y = ys[ixy];

		/* find the datum time at the (x,y) location */
		if(n1==1 && n2==1) {
			tg = datum;
		} else {
			bilint_(&one,&n1,&n2,&o1,&o2,&d1,&d2,&x,&y,dgrid,&tg);
		}

		ntv = nps[ixy];
		for(it=0;it<ntv;it++) {
			t[it] = tpicks[it+ixy*ntvmax]; 
			v[it] = vpicks[it+ixy*ntvmax]; 
		}

		/* obtain interval velocity */
		if(vitype==1) {
			for(it=0;it<ntv;it++) {
				ti[it] = t[it];
				vi[it] = v[it];
			}
		} else if(vitype==0) {
			ti[0] = t[0];
			vi[0] = v[0];
			for(it=1;it<ntv;it++) {
				ti[it] = t[it];
				tmp = v[it]*v[it]*t[it] - v[it-1]*v[it-1]*t[it-1];
				if(t[it]<=t[it-1])
					err(" time inversion of VS3D at s=%f l=%f t=%f\n",
						x,y,t[it]);
				tmp = tmp / (t[it]-t[it-1]);
				if(tmp<0.) 
					err(" Vint<0 of VS3D at s=%f l=%f t=%f \n",x,y,t[it]);
				vi[it] = sqrt(tmp);
			}
		} else if(vitype==2) {
			ti[0] = t[0];
			vi[0] = v[0];
			for(it=1;it<ntv;it++) {
				ti[it] = t[it];
				tmp = v[it]*t[it] - v[it-1]*t[it-1];
				if(t[it]<=t[it-1])
					err(" time inversion of VS3D at s=%f l=%f f=%f\n",
						x,y,t[it]);
				tmp = tmp / (t[it]-t[it-1]);
				if(tmp<0.) 
					err(" Vint<0 of VS3D at s=%f l=%f t=%f \n",x,y,t[it]);
				vi[it] = tmp;
			}
		}

		/* linear interpolate interval velocity to output time locations */

		to[0] = tg;
		npnew = 1;
		for(it=0;it<ntv;it++) {
			if(ti[it]>tg+1.) {
				to[npnew] = ti[it];
				npnew += 1;
			}
		}
		lin1d_(ti,vi,&ntv,to,vo,&npnew,indx);

		/* shift time to tg */
		for(it=0;it<npnew;it++) to[it] = to[it] - tg;

		/* convert the velocity back to its input type */
		if(vitype==1) {
			for(it=0;it<npnew;it++) {
				ito[it] = to[it];
				ivo[it] = vo[it];
			}
		} else if(vitype==0) {
			ito[0] = to[0];
			ivo[0] = vo[0];
			tmp = vo[0]*vo[0]*to[0];
			for(it=1;it<npnew;it++) {
				ito[it] = to[it]; 
				tmp = tmp + vo[it]*vo[it]*(to[it]-to[it-1]);
				tmp2 = sqrt(tmp/to[it]);
				ivo[it] = tmp2; 
			}
		} else if(vitype==2) {
			ito[0] = to[0];
			ivo[0] = vo[0];
			tmp = vo[0]*to[0];
			for(it=1;it<npnew;it++) {
				ito[it] = to[it]; 
				tmp = tmp + vo[it]*(to[it]-to[it-1]);
				tmp2 = tmp/to[it];
				ivo[it] = tmp2; 
			}
		}

		/* output VS3D cards */
		if(npnew>0) {
			printvs3d(x,y,npnew,ito,ivo,outfp);
			mxy = mxy + 1;
		}


	}

	fprintf(stderr," output %d VS3D locations \n",mxy); 
	free(xs);
	free(ys);
	free(tpicks);
	free(vpicks);
	free(nps);
	free(t);
	free(v);
	free(vi);
	free(ti);
	free(vo);
	free(to);
	free(ivo);
	free(ito);
	free(indx);

	return 0;

}
