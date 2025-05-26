#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUBALANCE - Amplitude Balance 			\n"
"\n"
"subalance <stdin >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"                           (can also be specified as datain=stdin,      \n"
"                           instead of <stdin)				\n"
"stdout                     Name of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"Optional Parameters:\n"
"tmin=mute_time       	    minimum time (in ms) to compute input rms 	\n"
"                           amplitude (default to mute time)		\n" 
"tmax=max_time              maximum time (in ms) to compute input rms 	\n"
"                           amplitude (default to maximum time in data)	\n" 
"rmsout=1.                  desired output rms amplitude		\n"
"iamp=0                     amplitude scale mode			\n"
"                           (0=scale trace to desired rms of rmsout	\n"
"                            1=multiple amplitude factor ampgrid to data \n"
"                            2=compute and store 1/rms of data at ampgrid) \n"
"                            3=compute and store rms of data at ampgrid) \n"
"ampgrid=                   Name of amplitude grid to be applied/stored	\n"
"                           (stored as (t,x,y) order with grid header)	\n" 
"nx=                        number of cdp per line in the input		\n"
"ny=                        number of lines in the input		\n"
"                           (nx and ny are needed when iamp=2)		\n"
"cdp1=from-ampgrid          cdp number at first trace of velocity grid	\n"
"dcdpx=1                    cdp number increment of trace along the inline  \n"
"                           in ampgrid (nx is number of traces per \n"
"                           line in the velocity grid) 			\n"
"dcdpy=nx                   cdp number increment of trace along the cross \n"
"                           line of ampgrid (this is the number of cdp 	\n"
"                           per line in the 3D master grid)		\n"
"cdp0=1                     cdp number of the 3D master grid    	\n"
"ntagrid=from-vnmogrid      Number of time samples per trace in ampgrid \n"
"dtagrid=from-vnmogrid      Time interval (in ms) of ampgrid \n"
"ftagrid=from-vnmogrid      Minimum Time (in ms) of ampgrid \n"
"lwin=50                    Window length in samples to compute rms when \n"
"                           ntagrid, ftagrid, dtagrid are specified \n"
"                           and iamp=0, 2 or 3  					\n"
"interp=0                   interpolate zeor-rms points from non-zero \n"
"                           data points when iamp=2 or 3			\n"
"                           0=no 1=yes				\n"
"\n"
"Author:	Zhiming Li		      		6-6-94		\n"
"Notes:									\n"
"  1. Constant scale function extrapolation applied outside cdp range of \n"
"     ampgrid								\n" 
"  2. when iamp=2 or 3, and rms of input trace is zero, a value will be	\n"
"     interpolated from adjacent nonzero points when interp=1			\n"
"  3. to apply a smooth scaling function, it is recommended to run this \n"
"     program twice, first compute ampgrid using iamp=2, then apply	\n"
"     vsmoo3d  program to smooth the ampgrid, run subalance again using \n"
"     iamp=1 and ampgrid input						\n" 
"  4. if ntagrid, dtagrid and ftagrid are specified, and iamp=0, or 2, or 3, \n"
"     the program will compute rms of the input using the window length lwin.\n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;


main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	float *at;	/* array[nt] of amp for a particular trace */
	float *aread;	/* array[ntagrid] of amp for a particular trace */
	int *indx;	/* temporary used to interpolate ovv array */
	int lwin, ii, jj, it0, itn;
	int interp;
	FILE *agfp;

	int ntagrid;
	char *agfile, *datain, *dataout;
	int iamp=0;
	FILE *infp,*outfp;
	int cdp1, cdp0;
	int iy1, ix1;
	float dcdpx, dcdpy;
	float tmin, tmax, tmp, rmsout;
	float *rms, *tin, *tout;
	float ftagrid, dtagrid;
	int nx, ny, itmin, itmax, n;
	int cdp, ix, iy;
	int ixx, iyy, i1, i2;
	float dd1, dd2;
	

	int n1,n2,n3,n4,n5;
        float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
        float scale, ocdp2, oline3, dcdp2, dline3;
	float vmin, vmax;
        int dtype,ierr,orient=0,gtype=0;
	ghed gh;



	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparint("iamp",&iamp)) iamp = 0;
	if(iamp!=0) { 
		if (!getparstring("ampgrid",&agfile)) 
			err(" File ampgrid must be specified ");
		if (iamp==1) {
			if((agfp = fopen(agfile,"r"))==NULL)
                	err("Input ampgrid file %s not found \n",agfile);
			/* obtain grid header info */
			ierr = fgetghdr(agfp, &gh);
			if(ierr==0) fromghdr(&gh,&scale,&dtype,
					&n1,&n2,&n3,&n4,&n5,
                                	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                                	&dcdp2,&dline3,&ocdp2,&oline3,
					&vmin,&vmax,&orient,&gtype);
		} else {
			agfp = fopen(agfile,"w");
			if (!getparint("nx",&nx)) 
				err("nx missing");
			if (!getparint("ny",&ny)) 
				err("ny missing");
			ierr=1;			
			n4 = 1;
			n5 = 1;
			n2 = nx;
			n3 = ny;
		}
	}


	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	} 
	file2g(infp);

	if(iamp<2) {
		if (!getparstring("dataout",&dataout)) {
			outfp = stdout;
		} else {
			outfp = efopen(dataout,"w");
		} 
		file2g(outfp);
	}

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = tr.delrt/1000.0;

	/* get other optional parameters */
	if(iamp==1) {
		nx = n2;
		ny = n3;
	}	
	if(iamp!=0) {
		if (!getparint("cdp1",&cdp1)) {
			if(ierr==0) {
				cdp1 = ocdp2; 
			} else {
				cdp1 = 1;
			}
		}
		if(cdp1==0) err("cdp1 can not be zero");
		if (!getparfloat("dcdpx",&dcdpx)) dcdpx = 1;
		if(n3==0) n3 = 1;
		if (!getparfloat("dcdpy",&dcdpy)) dcdpy = n2;
		if (!getparint("cdp0",&cdp0)) cdp0 = 1;


		if(dcdpx==0.) dcdpx = 1;
		if(dcdpy==0.) dcdpy = nx;

		tmp = (cdp1 - cdp0) / dcdpy + 0.001; 
		iy1 = tmp;
		tmp = (cdp1 - cdp0 - iy1 * dcdpy)/dcdpx + 0.001;
		ix1 = tmp;

fprintf(stderr,"\n");
fprintf(stderr,"starting cdp of ampgrid on 3D master grid: cdp1=%d\n",cdp1);
fprintf(stderr,"starting trace of ampgrid on 3D master grid: ix1=%d\n",ix1+1);
fprintf(stderr,"ending trace of ampgrid on 3D master grid:   nx1=%d\n",ix1+nx);
fprintf(stderr,"starting line of ampgrid on 3D master grid:  iy1=%d\n",iy1+1);
fprintf(stderr,"ending line of ampgrid on 3D master grid:    ny1=%d\n",iy1+ny);
fprintf(stderr,"\n");
		
	}
	if (!getparint("ntagrid",&ntagrid)) {
		if(ierr==0) {
			ntagrid = n1; 
		} else {
			ntagrid = 1; 
		}
	}
	if(ntagrid==0) ntagrid = 1;

	if (!getparfloat("dtagrid",&dtagrid)) {
		if(ierr==0) {
			dtagrid = d1 * 0.001;
		} else {
			dtagrid = dt;
		}
	} else {
		dtagrid = dtagrid/1000.0;
	}
	if (!getparfloat("ftagrid",&ftagrid)) {
		if(ierr==0) {
			ftagrid = o1 * 0.001;
		} else {
			ftagrid = ft;
		}
	} else {
		ftagrid = ftagrid/1000.0;
	}
	if (!getparint("lwin",&lwin)) lwin=50;

	if(!getparfloat("tmin",&tmin)) {
		tmin = ft;
	} else {
		tmin = tmin / 1000.;
	}
	if(!getparfloat("tmax",&tmax)) {
		tmax = ft + (nt-1)*dt;
	} else {
		tmax = tmax/1000.;
	}
	if(!getparfloat("rmsout",&rmsout)) rmsout=1.;
	
	if(!getparint("interp",&interp)) interp=0;
	if(nx<1) nx=1;
	if(ny<1) ny=1;

	/* allocate workspace */
	at = (float*) emalloc(nt*sizeof(float));
	rms = (float*) emalloc(ntagrid*nx*ny*sizeof(float));
	bzero(rms,nx*ny*ntagrid*sizeof(float));
	tin  = (float*) emalloc(ntagrid*sizeof(float));
	tout = (float*) emalloc(nt*sizeof(float));
	aread = (float*) emalloc(ntagrid*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));

	if(iamp<=1) {
		for(it=0;it<ntagrid;it++) tin[it] = ftagrid + it*dtagrid;
		for(it=0;it<nt;it++) tout[it] = ft + it * dt;
	}

	if(iamp==1) {
		if(ntagrid==1) {
			rms = (float*) emalloc(nx*ny*sizeof(float));
			efseek(agfp,0,0);
			efread(rms,sizeof(float),nx*ny,agfp);
		}
	}


	itmax = (tmax-ft)/dt+0.5;
	if(itmax>nt) itmax = nt;

	/* loop over traces */
	do {

		if(iamp!=0) {
			cdp = tr.cdp;
			tmp = (cdp - cdp0)/dcdpy + 0.001;
			iy = tmp;
			tmp = (cdp-cdp0-iy*dcdpy )/dcdpx+0.001; 
			ix = tmp;
			iy = iy - iy1;
			ix = ix - ix1;

/*
	fprintf(stderr,"cdp=%d cdp1=%d dcdpx=%f dcdpy=%f ix=%d iy=%d \n",
			cdp, cdp1, dcdpx, dcdpy, ix, iy); 
*/

			if(iy<0) iy=0;
			if(iy>ny-1) iy=ny-1;
			if(ix<0) ix=0;
			if(ix>nx-1) ix=nx-1;

		} else {
			ix = 0;
			iy = 0;
		}

/* compute rms */
		if(iamp!=1) {
			tmp = tr.mute * 0.001;
			if(tmp<tmin) tmp = tmin;
			tmp = (tmp - ft)/dt;  
			itmin = tmp;
			if(itmin<0) itmin = 0;
			if(ntagrid==1) { 
				tmp = 0.;
				n = 0;
				for(it=itmin;it<itmax;it++) { 
					if(tr.data[it]!=0.) {
						tmp = tmp + tr.data[it]*tr.data[it];
						n = n + 1;
					}
				}
				if(n>0) tmp = sqrt(tmp/n);
				rms[iy*nx+ix] = tmp;
			} else {
				for(ii=0;ii<ntagrid;ii++) {
					tmp = ftagrid + ii*dtagrid;
					tmp = (tmp-ft)/dt; 
					jj = tmp;
					it0 = jj-lwin/2;
					itn = jj+lwin/2;
					if(itmin>it0) it0 = itmin;
					if(itmax<itn) itn = itmax;
					tmp = 0;
					n = 0;
					for(it=it0;it<=itn;it++) {
						if(tr.data[it]!=0.) {
							tmp = tmp + tr.data[it]*tr.data[it];
							n = n + 1;
						}
					}
					if(n>0) tmp = sqrt(tmp/n);
					rms[(iy*nx+ix)*ntagrid+ii] = tmp;
				}
			}

			/* compute inverse of rms */
			if(iamp==2 || iamp==0 ) {
				for(ii=0;ii<ntagrid;ii++) {
					if(rms[(iy*nx+ix)*ntagrid+ii]>0.)
					   	rms[(iy*nx+ix)*ntagrid+ii]=
							rmsout/rms[(iy*nx+ix)*ntagrid+ii];
				}
			}

/*			
		fprintf(stderr,"cdp=%d ix=%d iy=%d itmin=%d itmax=%d \n", 
			cdp, ix, iy, itmin, itmax); 
*/
		}

		/* apply the scale */
		if(iamp==0 || iamp==1) {
			if(ntagrid>1) {
				if(iamp==1) {
					/* read in scale function */
					efseek(agfp,(iy*nx+ix)*ntagrid*sizeof(float),0);
					efread(aread,sizeof(float),ntagrid,agfp);
				} else {
					for(ii=0;ii<ntagrid;ii++) aread[ii] = rms[ii];
				}
				lin1d_(tin,aread,&ntagrid,tout,at,&nt,indx);
				for(it=0;it<nt;it++) 
					tr.data[it] *= at[it];
			} else {
				tmp = rms[ix+iy*nx];
				for(it=0;it<nt;it++) 
					tr.data[it] *= tmp;
			}
			fputtr(outfp,&tr);
		}

	} while (fgettr(infp,&tr));


	/* interpolating scales from nonzero points */
	if(interp==1 && iamp>=2) {

		/* fill zeros in time first */
		if(ntagrid>1) {
		for(iy=0;iy<ny;iy++) {
			for(ix=0;ix<nx;ix++) {
				for(ii=0;ii<ntagrid;ii++) {
					jj = (iy*nx+ix)*ntagrid + ii;
					if(rms[jj] == 0.) {
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
						dd1 = 0;
						i1 = 0;
						for(it=ii+1;it<ntagrid;it++) {
							if(rms[(iy*nx+ix)*ntagrid+it] != 0.) {
								dd1 = it - ii;
								i1 = it;
								break;
							}
						}
						dd2 = 0;
						i2 = 0;
						for(it=ii-1;it>=0;it--) {
							if(rms[(iy*nx+ix)*ntagrid+it] != 0.) {
								dd2 = ii - it;
								i2 = it;
								break;
							}
						}
						if(dd1+dd2>0.) {
							i1 = i1 + (iy*nx+ix)*ntagrid;
							i2 = i2 + (iy*nx+ix)*ntagrid;
							rms[jj] =
								dd2/(dd1+dd2)*rms[i1]+dd1/(dd1+dd2)*rms[i2];
						}
					}
				}
			}
		}
		}

		/* now try x direction */
		if(nx>1) {
		for(iy=0;iy<ny;iy++) {
			for(ii=0;ii<ntagrid;ii++) {
				for(ix=0;ix<nx;ix++) {
					jj = (iy*nx+ix)*ntagrid+ii;
					if(rms[jj] == 0.) {
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
						dd1 = 0;
						i1 = 0;
						for(ixx=ix+1;ixx<nx;ixx++) {
							if(rms[(iy*nx+ixx)*ntagrid+ii]!=0.) {
								dd1 = ixx - ix;	
								i1 = ixx;
								break;
							}
						}
						dd2 = 0;
						i2 = 0;
						for(ixx=ix-1;ixx>=0;ixx--) {
							if(rms[(iy*nx+ixx)*ntagrid+ii]!=0.) {
								dd2 = ix - ixx;	
								i2 = ixx;
								break;
							}
						}
						if(dd1+dd2>0.) {
							i1 = (iy*nx+i1)*ntagrid+ii;
							i2 = (iy*nx+i2)*ntagrid+ii;
							rms[jj] =
								dd2/(dd1+dd2)*rms[i1]+dd1/(dd1+dd2)*rms[i2];
						}

					}
				}
			}
		}
		}

		/* last look at y direction */
		if(ny>1) {
		for(ix=0;ix<nx;ix++) {
			for(ii=0;ii<ntagrid;ii++) {
				for(iy=0;iy<ny;iy++) {
					jj = (iy*nx+ix)*ntagrid+ii;
					if(rms[jj] == 0.) {
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
						dd1 = 0;
						i1 = 0;
						for(iyy=iy+1;iyy<ny;iyy++) {
							if(rms[(iyy*nx+ix)*ntagrid+ii]!=0.) {
								dd1 = iyy - iy;	
								i1 = iyy;
								break;
							}
						}
						dd2 = 0;
						i2 = 0;
						for(iyy=iy-1;iyy>=0;iyy--) {
							if(rms[(iyy*nx+ix)*ntagrid+ii]!=0.) {
								dd2 = iy - iyy;
								i2 = iyy;
								break;
							}
						}
						if(dd1+dd2>0.) {
							i1 = (i1*nx+ix)*ntagrid+ii;
							i2 = (i2*nx+ix)*ntagrid+ii;
							rms[jj] =
								dd2/(dd1+dd2)*rms[i1]+dd1/(dd1+dd2)*rms[i2];
						}
					}
				}
			}
		}
		}
	}

	/* output grid */
	if (iamp>=2) {
		vmin = rms[0];
		vmax = rms[0];
		for(ix=0;ix<nx*ny*ntagrid;ix++) {
			if(vmin>rms[ix]) vmin = rms[ix];
			if(vmax<rms[ix]) vmax = rms[ix];
		}
			
		efwrite(rms,sizeof(float),nx*ny*ntagrid,agfp);
		scale = 1.0e-6;
		dtype = 4;
		n1 = ntagrid;
		o1 = ftagrid*1000;
		d1 = dtagrid*1000;
		n2 = nx;
		o2 = ix1+1;
		d2 = 1.0; 
		n3 = ny;
		o3 = iy1+1;
		d3 = 1.;
		orient = 1;
		ocdp2 = cdp1;
		dcdp2 = dcdpx;
		oline3 = iy1 + 1;
		dline3 = 1.;
		n4 = 1;
		n5 = 1;
		o4 = 0;
		o5 = 0;
		d4 = 1;
		d5 = 1;
		
		toghdr(&gh,&scale,&dtype,
			&n1,&n2,&n3,&n4,&n5,
                        &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                        &dcdp2,&dline3,&ocdp2,&oline3,
			&vmin,&vmax,&orient,&gtype);

		ierr = fputghdr(agfp, &gh);
		if(ierr!=0) warn("ampgrid header error");
		
	}
	

	return EXIT_SUCCESS;
}

