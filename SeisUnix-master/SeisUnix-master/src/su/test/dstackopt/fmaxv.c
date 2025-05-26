#define BIG 1e10
float fmaxv(xc,nc)
float *xc; int nc;
{
		extern dtmax;
		float *ddd,max,tmax; int i,imax;

		/* calculate the maximum by parabola matching */
		for(i=0,max= -BIG;i<nc;i++) {
			if(xc[i] > max) {
				max = xc[i];
				imax = i;
			}
		}
		ddd = xc + imax - 1;
		if(imax == 0) tmax = -dtmax;
		else if(imax == nc-1) tmax = dtmax;
		else tmax = imax+(ddd[0]-ddd[2])/(ddd[0]-2*ddd[1]+ddd[2])/2 - dtmax;
		return(tmax);
}
