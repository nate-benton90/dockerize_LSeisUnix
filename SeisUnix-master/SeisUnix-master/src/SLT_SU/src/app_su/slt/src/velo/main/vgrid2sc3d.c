/* velocity scale card sc3d generation from VS3D card input */

#include "usgrid.h"
#include "velo.h"
#include "par.h"


char *sdoc = 
"VGRID2SC3D - calculate 3D scale SC3D cards from 3D vgrid input		\n"
"\n"
"vgrid2sc3d vs3d= <vgrid >sc3d	\n" 
"\n"
"Required parameters:						 	\n"
"vgrid=            Name of 3d velocity grid file 			\n"
"vs3d=             Name of dataset containing VS3D cards to which 	\n"
"                  vgrid is to be calibrated				\n" 
"sc3d=             Name of dataset to output SC3D cards 		\n"
"\n"
"Optional parameters:							\n"
"t0=               time (in ms) to add to output sc3d 			\n"
"v0=               velocity at t0					\n"
"nvfmax=4096       maximum number of velocity functions in input vs3d   \n" 
"                  dataset     						\n" 
"ntvmax=256        maximum number of t-v pairs per velocity functions 	\n"
"                  in input vs3d dataset     				\n" 
"verbose=1         1=print job progress; 0=no print 			\n" 
"\n"
" Notes:								\n"
" 1. velocity types (rms, or average, or interval) must be the same for \n"
"    vs3d cards and vgrid						\n"
" 2. coordinates of vs3d must be the same as those defined in vgrid  	\n"
"    header. The velocity function at a grid which is nearest to the   	\n"
"    position of vs3d card will be used to compute the scaling function \n"
" 3. vgrid is stored as v(t,x,y)					\n"
" 4. when vs3d cards start at a large time (i.e., check shot), user can	\n"
"    add t0,v0 to control the scaling function above the minimum time	\n"
"    of the vs3d cards							\n" 
" 5. if either t0 or v0 is not given, both of them are ignored		\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	6/25/94   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
	FILE *vs3dfp;
	char *vs3d;

	int ntvmax, nvfmax; 
	int verbose=1;

   	int n1,n2,i,ix,iy,it,nxy,np;
        int nx,ny,nt,ix1,ix2,iy1,iy2,it1,it2;
        float dx,dy,dt,fx,fy,ft,rx1,rx2,ry1,ry2,rt1,rt2;
	float tmp, tt, vv, x, y;
	float t0, v0;
	int itv0=0;

        int ierr;

        usghed usgh;

        float *xs, *ys, *tpicks, *vpicks, *v, *v11, *v12, *v21, *v22;
	float *scales, *times;
        int *nps;
	float *ts, *vs;
	int nts;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */
	if (!getparstring("vs3d",&vs3d)) 
		err(" vs3d missing ");
	vs3dfp = efopen(vs3d,"r"); 
	if (!getparint("verbose",&verbose)) verbose=1;

    	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
		pairs each */

	if (!getparint("nvfmax",&n2)) n2 = 4096;
	if (!getparint("ntvmax",&n1)) n1 = 256;

	if (getparfloat("t0",&t0) && getparfloat("v0",&v0)) itv0 = 1;

	/* open vgrid */
	ierr = fgetusghdr(infp,&usgh);
	if (ierr!=0) err(" input vgrid header error");

	nt = usgh.n1; nx = usgh.n2; ny = usgh.n3;
	dt = usgh.d1; dx = usgh.d2; dy = usgh.d3;
	ft = usgh.o1; fx = usgh.o2; fy = usgh.o3;

    	/* arrays used to store all VS3D card's cdp, time and velocity */
    	xs = (float*)emalloc(n2*sizeof(float));
    	ys = (float*)emalloc(n2*sizeof(float));
    	tpicks = (float*)emalloc(n1*n2*sizeof(float));
    	vpicks = (float*)emalloc(n1*n2*sizeof(float));
    	nps = (int*)emalloc(n2*sizeof(int));

    	/* arrays used to scale velocity */
    	v = (float*)emalloc(nt*sizeof(float));
    	v11 = (float*)emalloc(nt*sizeof(float));
    	v12 = (float*)emalloc(nt*sizeof(float));
    	v21 = (float*)emalloc(nt*sizeof(float));
    	v22 = (float*)emalloc(nt*sizeof(float));
    	times = (float*)emalloc((n1+1)*sizeof(float));
    	scales = (float*)emalloc((n1+1)*sizeof(float));
    	ts = (float*)emalloc((n1+1)*sizeof(float));
    	vs = (float*)emalloc((n1+1)*sizeof(float));
	
	bzero(nps,n2*sizeof(int));
    	/* read in VS3D cards */
    	nxy = 0;
	vs3dread(vs3dfp,xs,ys,tpicks,vpicks,&nxy,nps,n1,n2);

	fprintf(stderr," %d VS3D cards read \n",nxy);

   	if (nxy==0) err("No VS3D card input ! Job aborted");
	

	for(i=0;i<nxy;i++) {
		if(verbose==1) fprintf(stderr, 	
			" %d t-v pairs read at x=%f y=%f location\n",
			nps[i],xs[i],ys[i]);
		/* find out grid location to read vgrid */

		np = nps[i];
		nts = np + itv0;
		if(itv0==1) {
			ts[0] = t0;
			vs[0] = v0;
		}
		for(it=0;it<np;it++) {
			ts[it+itv0] = tpicks[it+i*n1];
			vs[it+itv0] = vpicks[it+i*n1];
		}
		tmp = (xs[i] - fx)/dx;
		ix1 = tmp;
		ix2 = tmp + 1;
		if(ix1>nx-1) {
			ix1 = nx-1;
			ix2 = nx-1;
			rx1 = 0.5;
			rx2 = 0.5;
		} else if(ix1<0 || nx==1) {
			ix1 = 0;
			ix2 = 0;
			rx1 = 0.5;
			rx2 = 0.5;
		} else {
			rx2 = tmp - ix1;
			rx1 = 1.0 - rx2;
		}
		tmp = (ys[i]-fy)/dy;
		iy1 = tmp;
		iy2 = tmp + 1;
		if(iy1>ny-1) {
			iy1 = ny-1;
			iy2 = ny-1;
			ry1 = 0.5;
			ry2 = 0.5;
		} else if(iy1<0 || ny==1) {
			iy1 = 0;
			iy2 = 0;
			ry1 = 0.5;
			ry2 = 0.5;
		} else {
			ry2 = tmp - iy1;
			ry1 = 1.0 - ry2;
		}

		efseek(infp,(ix1+iy1*nx)*nt*sizeof(float),0);
		efread(v11,sizeof(float),nt,infp);
		efseek(infp,(ix2+iy1*nx)*nt*sizeof(float),0);
		efread(v12,sizeof(float),nt,infp);
		efseek(infp,(ix1+iy2*nx)*nt*sizeof(float),0);
		efread(v21,sizeof(float),nt,infp);
		efseek(infp,(ix2+iy2*nx)*nt*sizeof(float),0);
		efread(v22,sizeof(float),nt,infp);

		for(it=0;it<nt;it++) { 
			v[it] = ry1*(rx1*v11[it]+rx2*v12[it]) +
			        ry2*(rx1*v21[it]+rx2*v22[it]);
		}

		for(it=0;it<nts;it++) {
			tt = ts[it];
			vv = vs[it];
			tmp = (tt - ft)/dt;
			it1 = tmp;
			it2 = it1 + 1;
			if(it1>=nt-1) {
				it1 = nt-1;
				it2 = nt-1;
				rt1 = 0.5;
				rt2 = 0.5;
			} else if(it1<0) {
				it1 = 0;
				it2 = 0;
				rt1 = 0.5;
				rt2 = 0.5;
			} else {
				rt2 = tmp - it1;
				rt1 = 1. - rt2;
			}

			tmp = v[it1]*rt1 + v[it2]*rt2;
			scales[it] = vv/tmp; 
			times[it] = ts[it];
		if(verbose==1) 
	fprintf(stderr," it=%d time=%g v.vs3d=%g v.vgrid=%g scale=%g\n",
			it+1,times[it],vv,tmp,scales[it]);
		}
		x = xs[i];
		y = ys[i];
				
		/* output sc3d cards */
		printsc3d(x,y,nts,times,scales,outfp);
	}

	free(xs);
	free(ys);
	free(tpicks);
	free(vpicks);
	free(nps);

	free(times);
	free(scales);
	free(v);
	free(v11);
	free(v12);
	free(v21);
	free(v22);
	free(ts);
	free(vs);
	
	return 0;
}
