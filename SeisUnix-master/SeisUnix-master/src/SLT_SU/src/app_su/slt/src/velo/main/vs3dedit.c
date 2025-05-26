/* velocity edit of VS3D card */

#include "usgrid.h"
#include "velo.h"
#include "par.h"


char *sdoc = 
"VS3DEDIT - edit VS3D cards with a horizon constraint  \n"
"\n"
"vs3dedit [parameters] <vs3d-cards >vs3d-edit  				\n" 
"\n"
"Required parameters:						 	\n"
"vs3d-cards=       Name of input dataset containing VS3D cards 		\n"
"vs3d-edit=        Name of output dataset containing VS3D cards 		\n"
"hgrid=            Name of horizon grid where below the horizon VS3D	\n"
"                  cards are to be edited					\n"
"\n"
"Optional parameters:							\n"
"nvfmax=4096       maximum number of velocity functions in input vs3d   \n" 
"                  dataset     						\n" 
"ntvmax=256        maximum number of t-v pairs per velocity functions 	\n"
"                  in input vs3d   \n" 
"                  dataset     						\n" 
"bv=0              grid value in hgrid to indicate no horizon pick	\n"
"dis=0             distance from the edges of the horizon picks to exclude \n"
"                  input vs3d cards and create new ones 	\n" 
"out=2             output vs3d flag					\n"
"                  2=output at all input vs3d locations	\n"
"                  1=output vs3d only over the horizon area		\n" 
"                  0=output vs3d only where distance from horizon >= dis	\n"
"                 -1=output vs3d where distance from horizon < dis	\n" 
"bflag=0           flag of outputing velocity below horizon. \n"
"                  output velocity below the horizon extrapolated from \n"
"                  the nearest vs3d location with distance dis from \n"
"                  the horizon edge will be check against velocity above \n"
"                  the horizon		\n"
"                  0=only output those t-v pairs where extrapolated \n"
"                    velocity is greater than velocity above the horizon. \n"
"                  1=only output down to the horizon	\n"
"                 -1=do not output vs3d if the velocity above the horizon \n"
"                    is greater than the velocity extrapolated below the \n"
"                    horizon			\n"
"tmin=0            minimum time (ms) of velocity functions	\n"
"tmax=9000         maximum time (ms) of velocity functions	\n"
"nf=3              number of closest velocity functions (with distance \n"
"                  from horizon > dis) to be used to compute the velocity \n"
"                  below the horizon			\n"
"                  =0   use all the functions whose distance from the \n"
"                  output is less than dismax		\n"
"dismax=10000      maximum distance of which a velocity function can    \n"
"                  be used to interpolate output                        \n"
"\n"
"AUTHOR:	   Zhiming Li,       ,	2/25/96   		\n"    
;

void findxy(float *xs, float *ys, int nxy, float x, float y, 
		int *flag, int *jxy);

void findvs(float *times,float *vrms,int ntv,float *tpicks,float *vpicks,
			int nps,int itt,int bflag,int *npnew);

main(int argc, char **argv)
{
   	FILE *infp=stdin,*outfp=stdout;
	FILE *xyfp;
	int ntvmax,nvfmax,nxy,np;
   	float *xs, *ys, *tpicks, *vpicks;
	int *nps, *flag, *edgex, *edgey;
	float *ts,dis;
	int out,bflag,npnew,nf;

	usghed ugh;
	FILE *gfp;
	char *hgrid;
	float *grid;
	int n1,n2;
	float o1,o2,d1,d2,bv;
	float *xg, *yg;

	int ixy; 
	float x, y;
	float *times, *vrms;
	int ntv, it, ix,iy;

	float dd,xx,tmp,yy;
	int itt,jxy,mxy,ne,ie;

	float *xi, *yi;
	float *ti, *vi, *vo, *work, dismax;
	float tmin, tmax, dvdt0, dvdtn;
	int ni, nti, *indx;


   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* required parameters */

   	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
	pairs each */
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparint("out",&out)) out = 2;
	if (!getparint("bflag",&bflag)) bflag = 0;
	if (!getparfloat("tmin",&tmin)) tmin = 0.;
	if (!getparfloat("tmax",&tmax)) tmax = 9000.;
	if (!getparfloat("dismax",&dismax)) dismax = 10000.;
	if (!getparint("nf",&nf)) nf = 3;

   	/* arrays used to store all VS3D card's cdp, time and velocity */
   	xs = (float*)emalloc(nvfmax*sizeof(float));
   	ys = (float*)emalloc(nvfmax*sizeof(float));
   	tpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	vpicks = (float*)emalloc(ntvmax*nvfmax*sizeof(float));
   	nps = (int*)emalloc(nvfmax*sizeof(int));

   	flag = (int*)emalloc(nvfmax*sizeof(int));
	ts = (float*)emalloc(nvfmax*sizeof(float)); 
	times = (float*) emalloc(ntvmax*sizeof(float)); 
	vrms = (float*) emalloc(ntvmax*sizeof(float)); 

	bzero(nps,nvfmax*sizeof(int));
    /* read in VS3D cards */
    nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,ntvmax,nvfmax);
	fprintf(stderr," %d VS3D cards read \n",nxy);
   	if (nxy==0) err("No VS3D card input ! Job aborted");
	
	/* read in the horizon grid file */
	if(!getparstring("hgrid",&hgrid)) err(" hgrid must be specified ");
	if(!getparfloat("bv",&bv)) bv=0.;
	gfp = efopen(hgrid,"r");
	if(fgetusghdr(gfp,&ugh)!=0) err(" check hgrid header "); 
	o1 = ugh.o1; d1=ugh.d1; n1=ugh.n1; 
	o2 = ugh.o2; d2=ugh.d2; n2=ugh.n2; 
	grid = (float*) emalloc(n1*n2*sizeof(float));
	xg = (float*) emalloc(n1*sizeof(float));
	yg = (float*) emalloc(n2*sizeof(float));
   	edgex = (int*)emalloc(n1*n2*sizeof(int));
   	edgey = (int*)emalloc(n1*n2*sizeof(int));
	efseek(gfp,0,0);
	efread(grid,sizeof(float),n1*n2,gfp);

	if (!getparfloat("dis",&dis)) dis = 0.;

	for(iy=0;iy<n2;iy++) yg[iy] = o2+iy*d2;
	for(ix=0;ix<n1;ix++) xg[ix] = o1+ix*d1;

	dis = dis * dis;
	xyfp  = fopen("xy.flag.file","w");

	for(ixy=0;ixy<nxy;ixy++) {
		x = (xs[ixy]-o1)/d1+0.5;
		y = (ys[ixy]-o2)/d2+0.5;
		ix = x;
		iy = y;
		if(ix<0 || ix>=n1 || iy<0 || iy>=n2) { 
			warn("VS3D card %f %f outside hgrid \n",xs[ixy],ys[ixy]);
			if(ix<0) ix=0;
			if(ix>n1-1) ix=n1-1;
			if(iy<0) iy=0;
			if(iy>n2-1) iy=n2-1;
		}
		ts[ixy] = grid[ix+iy*n1];
		if(grid[ix+iy*n1]!=bv) {
			flag[ixy] = 1;
		} else {
			flag[ixy] = 0;
		}
	}
	/* detect edge */
	ne = 0;
	for(iy=0;iy<n2;iy++) {
		for(ix=0;ix<n1;ix++) {
			if(ix==0 || ix==n1-1 || iy==0 || iy==n2-1) {
				if(grid[ix+iy*n1]!=bv) {
					edgex[ne] = ix;
					edgey[ne] = iy;
					ne = ne + 1;
				}
			} else {
				if(grid[ix+iy*n1]!=bv) {
					if(grid[ix-1+iy*n1]==bv || 
					   grid[ix+1+iy*n1]==bv || 
					   grid[ix+(iy-1)*n1]==bv || 
					   grid[ix+(iy+1)*n1]==bv) {
							edgex[ne] = ix;
							edgey[ne] = iy;
							ne += 1;
					   } 
				} 
			}
		}
	}
	fprintf(stderr," %d number of horizon edge points detected \n",ne); 

	/* detect points whose distance is less than dis away from edge */
	if(dis>0.) {
		for(ixy=0;ixy<nxy;ixy++) {
			if(flag[ixy]==0) {
				for(ie=0;ie<ne;ie++) {
					ix = edgex[ie];
					iy = edgey[ie];
					xx = xg[ix];
					yy = yg[iy];
					tmp=(xs[ixy]-xx)*(xs[ixy]-xx)+(ys[ixy]-yy)*(ys[ixy]-yy);
					if(tmp<dis)  {
						flag[ixy] = -1;
						break;
					}
				}
			}
		}
	}

	tmp = (tmax-tmin)/50.+1.5;
	nti = tmp;
	ti = (float*) malloc(nti*sizeof(float));
	xi = (float*) malloc(nxy*sizeof(float));
	yi = (float*) malloc(nxy*sizeof(float));
	vi = (float*) malloc(nxy*nti*sizeof(float));
	/*
		indx = (int*) malloc(nti*sizeof(int));
	*/
	indx = (int*) malloc(nxy*sizeof(int));
	vo = (float*) malloc(nti*sizeof(float));
	work = (float*) malloc(nxy*sizeof(float));

	for (it=0;it<nti;it++) ti[it] = tmin + it*50.;
	ni = 0;

	for(ixy=0;ixy<nxy;ixy++) {
		if(flag[ixy]==0) {
				xi[ni] = xs[ixy];
				yi[ni] = ys[ixy];
				ntv = nps[ixy];
				dvdt0 = 0.;
				dvdtn = 0.;
				if(ntv > 1) {
				dvdtn = vpicks[ntv-1+ixy*ntvmax]-vpicks[ntv-2+ixy*ntvmax]; 
				dvdtn /= (tpicks[ntv-1+ixy*ntvmax]-tpicks[ntv-2+ixy*ntvmax]);
				}
				lin1dn_(tpicks+ixy*ntvmax,vpicks+ixy*ntvmax,&ntv,
						ti,vi+ni*nti,&nti,indx,&dvdt0,&dvdtn);
				ni += 1;
		}
	}

	for(ixy=0;ixy<nxy;ixy++)
		fprintf(xyfp,"%f %f %d \n",xs[ixy],ys[ixy],flag[ixy]);

	fclose(xyfp);
	mxy = 0;

/* output vs3d cards */
	for(ixy=0;ixy<nxy;ixy++) {
		if(out!=2 && flag[ixy]!=out) continue;

		x = xs[ixy];
		y = ys[ixy];
		ntv = nps[ixy];
		npnew = ntv;
		for(it=0;it<ntv;it++) {
			times[it] = tpicks[it+ixy*ntvmax]; 
			vrms[it] = vpicks[it+ixy*ntvmax]; 
		}
		if(flag[ixy]==1) {
			itt  = ntv;
			for(it=0;it<ntv;it++) {
				if( (float)times[it] > ts[ixy] ) {
					itt = it;
					break;
				}
			}
			if(itt<ntv) {
				if(nf!=0) {
					intp2d_(xi,yi,vi,&nti,&ni,&x,&y,vo,&nf,indx,work);
				} else {
					plint_(xi,yi,vi,&nti,&ni,&x,&y,vo,work,&dismax,indx);
				}

				/*
				findxy(xs,ys,nxy,x,y,flag,&jxy);
				findvs(times,vrms,ntv,tpicks+jxy*ntvmax,vpicks+jxy*ntvmax,
						nps[jxy],itt,bflag,&npnew);
				*/
				findvs(times,vrms,ntv,ti,vo,nti,itt,bflag,&npnew);

				/*
			    fprintf(stderr,"jxy=%d xs=%f ys=%f x=%f y=%f ts=%f\n",
					jxy,xs[jxy],ys[jxy],x,y,ts[ixy]);
				*/
			}
		} else if(flag[ixy]==-1) {
				itt = 0;
				/*
				findxy(xs,ys,nxy,x,y,flag,&jxy);
				findvs(times,vrms,ntv,tpicks+jxy*ntvmax,vpicks+jxy*ntvmax,
						nps[jxy],itt,0,&npnew);
				*/
				if(nf!=0) {
					intp2d_(xi,yi,vi,&nti,&ni,&x,&y,vo,&nf,indx,work);
				} else {
					plint_(xi,yi,vi,&nti,&ni,&x,&y,vo,work,&dismax,indx);
				}
				findvs(times,vrms,ntv,ti,vo,nti,itt,bflag,&npnew);

				/*
			    fprintf(stderr,"jxy=%d xs=%f ys=%f x=%f y=%f ts=%f\n",
					jxy,xs[jxy],ys[jxy],x,y,ts[ixy]);
				*/
		}
		if(npnew>0)	{
			printvs3d(x,y,npnew,times,vrms,outfp);
			mxy = mxy + 1;
		}	
		/*
		if((int)x==99655 && (int)y==77165) {
			for(it=0;it<nps[jxy];it++) {
				fprintf(stderr,"tpicks =%f vpicks=%f it=%d \n",
					tpicks[jxy*ntvmax+it],vpicks[jxy*ntvmax+it],it+1);
			}
			err(" inversion found ts=%f itt=%d ",ts[ixy],itt);
		}
		*/
	}

	fprintf(stderr," output %d VS3D locations \n",mxy); 
	return 0;

}

void findxy(float *xs, float *ys, int nxy, float x, float y, 
		int *flag, int *jxy) {

		float tmp, dd;
		int ixy, ii;

		ii = 0;
		*jxy = -1;
		for(ixy=0;ixy<nxy;ixy++) {
			if(ii==0 && flag[ixy]==0) {
				dd = (x-xs[ixy])*(x-xs[ixy])+(y-ys[ixy])*(y-ys[ixy]);
				ii = 1;
				*jxy = ixy;
			} else if(ii>0 && flag[ixy]==0) {
				tmp = (x-xs[ixy])*(x-xs[ixy])+(y-ys[ixy])*(y-ys[ixy]);
				if(tmp<dd) {
					*jxy = ixy;
					dd = tmp;
				}
			}
		}
}

void findvs(float *times,float *vrms,int ntv,float *tpicks,float *vpicks,
			int nps,int itt,int bflag,int *npnew) {

	float *t, *v;
	int nt, it, *indx;
	int mtt, i;
	float t0,tn;
	float v0,vn;

	if (bflag==1) {
		*npnew = itt;
		return;
	} else {
		*npnew = ntv;
	}
	nt = ntv - itt;
	t = (float*) malloc(nt*sizeof(float));
	v = (float*) malloc(nt*sizeof(float));
	indx = (int*) malloc(nt*sizeof(int));

	for(it=0;it<nt;it++)
		t[it] = times[it+itt];
	lin1d_(tpicks,vpicks,&nps,t,v,&nt,indx);

	/*
	for(it=0;it<nps;it++) {
		fprintf(stderr,"time=%f vrms=%f it=%d\n",tpicks[it],vpicks[it],it+1);
	}
	for(it=0;it<nt;it++) {
		fprintf(stderr,"t=%f v=%f it=%d\n",t[it],v[it],it+1);
	}
	*/

	if(itt==0) {
		v0 = v[0];
		t0 = t[0];
	} else {
		v0 = vrms[itt-1];
		t0 = times[itt-1];
	}
	mtt = -1;
	if(bflag==-1) {
		if(v[0]<v0) {
			*npnew = 0;
			return; 
		}
	}
	for(it=0;it<nt;it++) {
		if(v[it]>=v0) {
			mtt = it;
			break;
		}
	}
	if(mtt>=0) {
		tn = t[mtt];
		vn = v[mtt];
		for(it=0;it<mtt;it++) {
			v[it] = v0 + (t[it]-t0)*(vn-v0)/(tn-t0);
		}
		for(it=0;it<nt;it++) {
			vrms[it+itt] = v[it];
		}
	} else {
		for(it=0;it<nt;it++) {
			vrms[it+itt] = vrms[itt];
		}
	}

/*
	fprintf(stderr,"t0=%f v0=%f tn=%f vn=%f mtt=%d \n",t0,v0,tn,vn,mtt);
*/



/*
	for(it=0;it<ntv;it++) {
		fprintf(stderr,"time=%d vrms=%d it=%d\n",times[it],vrms[it],it+1);
	}
*/

	free(indx);
	free(t);
	free(v);
}
