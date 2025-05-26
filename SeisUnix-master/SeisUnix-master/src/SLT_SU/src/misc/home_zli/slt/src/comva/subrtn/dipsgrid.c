#include "comva.h"

/* input:
	dfile	---	DIPS card file name
	z0	---	first depth of dips grid
	dz	---	depth interval of dips grid
	nz	---	number of depth intervals of dips grid
	fcdp	---	first cdp number of dips grid
	dcdp	---	cdp number increment of dips grid
	ncdp	---	number of cdps of dips grid
   output:
	dipl	---	dips to the left
	dipr	---	dips to the right

   author:	Zhiming Li	9/2/92	      
*/

void dipsgrid(char *dfile, float *dipl, float *dipr, 
	float z0, float dz, int nz, int fcdp, int dcdp, int ncdp) { 

	float *dl, *dr, *zs;
	int ncdps, *nps, *cdps;
	int maxp=256, maxcdp=128;

	float *z, *dlz, *drz;
	int iz, icdp, *indx;
	int i, i1, i2, nn, incdp, np;
	float res, cdp;
	float *cdpf, *sorts;
	FILE *infp;

	infp = efopen(dfile,"r");

	zs = (float*) malloc(maxp*maxcdp*sizeof(float));
	z = (float*) malloc(nz*sizeof(float));
	dl = (float*) malloc(maxp*maxcdp*sizeof(float));
	dr = (float*) malloc(maxp*maxcdp*sizeof(float));
	nps = (int*) malloc(maxcdp*sizeof(int));
	cdps = (int*) malloc(maxcdp*sizeof(int));

	/* read in DIPS cards */
	dipsread(infp, cdps, zs, dl, dr, &ncdps, nps, maxp, maxcdp);


	/*
	for(i1=0;i1<ncdps;i1++)
		for(i2=0;i2<nps[i1];i2++) 
			fprintf(stderr,"%g %g %g %d %d\n",
				zs[i1*maxp+i2],
				dl[i1*maxp+i2],
				dr[i1*maxp+i2],
				i2,
				cdps[i1]);
	*/
	efclose(infp);

	/* interpolate to output depth */
	for(iz=0;iz<nz;iz++) z[iz] = z0 + iz*dz;
	dlz = (float*) malloc(nz*ncdps*sizeof(float));
	drz = (float*) malloc(nz*ncdps*sizeof(float));
	indx = (int*) malloc(nz*sizeof(float));
	
	for(icdp=0;icdp<ncdps;icdp++) {
		np = nps[icdp];
		lin1d_(zs+icdp*maxp,dl+icdp*maxp,&np,z,dlz+icdp*nz,&nz,indx);
		lin1d_(zs+icdp*maxp,dr+icdp*maxp,&np,z,drz+icdp*nz,&nz,indx);
	}



	free(z);
	free(indx);
	free(zs);
	free(dl);
	free(dr);
	free(nps);

	/* sort input velf in cdp-ascending order */
        cdpf = (float*) malloc(ncdps*sizeof(float));
	indx = (int*) malloc(ncdps*sizeof(int));
        sorts = (float*) malloc(ncdps*nz*sizeof(float));

        for(i=0;i<ncdps;i++) {
                indx[i] = i;
                cdpf[i] = cdps[i];
        }
        if(ncdps>1) qkisort(ncdps,cdpf,indx);
        for(i=0;i<ncdps;i++) cdpf[i] = (float) cdps[indx[i]];

        for(i=0;i<ncdps;i++)
                for(iz=0;iz<nz;iz++) sorts[iz+i*nz] = dlz[iz+indx[i]*nz];
        for(iz=0;iz<ncdps*nz;iz++) dlz[iz] = sorts[iz];
        for(i=0;i<ncdps;i++)
                for(iz=0;iz<nz;iz++) sorts[iz+i*nz] = drz[iz+indx[i]*nz];
        for(iz=0;iz<ncdps*nz;iz++) drz[iz] = sorts[iz];

        free(cdps);
	free(sorts);
        free(indx);

	/* interpolate along cdps */ 
	for(icdp=0;icdp<ncdp;icdp++) {

		cdp = fcdp + icdp*dcdp;
		if(ncdps<2) {
			i1 = 0; i2=0; res=0;
		} else {
			nn = 1;
                        bisear_(&ncdps,&nn,cdpf,&cdp,&incdp);
                        if (incdp < 1 || cdp < cdpf[0] ) {
                                i1 = 0; i2 = 0; res = 0.;
                        } else if(incdp >= ncdps) {
                                i1 = ncdps-1; i2 = ncdps-1; res = 0.;
                        } else {
                                i1 = incdp-1;
                                i2 = incdp;
                                res =(cdp-cdpf[i1])
                                        /(cdpf[i2]-cdpf[i1]);
                        }
                }
		for(iz=0;iz<nz;iz++) {
                        dipl[iz+icdp*nz] = dlz[i1*nz+iz] +
                                        res*(dlz[i2*nz+iz]-dlz[i1*nz+iz]);
                        dipr[iz+icdp*nz] = drz[i1*nz+iz] +
                                        res*(drz[i2*nz+iz]-drz[i1*nz+iz]);
		}
	}

	free(cdpf);
	free(dlz);
	free(drz);

}

/* anci interface for c++ caller */
void dipsgrid_cpp(char *dfile, float *dipl, float *dipr, 
	double z0, double dz, int nz, int fcdp, int dcdp, int ncdp) { 

	float iz0=z0, idz=dz;
	dipsgrid(dfile,dipl,dipr,iz0,idz,nz,fcdp,dcdp,ncdp);
}
