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
"nx=                        number of traces per line in the input		\n"
"                           (number of traces per line in ammgrid must = nx) \n"
"ny=                        number of lines in the input		\n"
"                           (number of lines in ammgrid must = ny) \n"
"                           (nx and ny are needed when iamp>=1)		\n"
"ocdp2=from-ampgrid         trace number at first trace of ampgrid	\n"
"dcdp2=from-ampgrid         trace number increment along the inline \n"
"                           in ampgrid \n"
"oline3=from-ampgrid        line number at first trace of ampgrid \n"
"dline3=from-ampgrid        line number increment of ampgrid \n"
"ntagrid=from-vnmogrid      Number of time samples per trace in ampgrid \n"
"dtagrid=from-vnmogrid      Time interval (in ms) of ampgrid \n"
"ftagrid=from-vnmogrid      Minimum Time (in ms) of ampgrid \n"
"lwin=50                    Window length in samples to compute rms when \n"
"                           ntagrid, ftagrid, dtagrid are specified \n"
"                           and iamp=0, 2 or 3  					\n"
"interp=0                   interpolate zeor-rms points from non-zero \n"
"                           data points when iamp=2 or 3			\n"
"                           0=no 1=only along time; 			\n"
"                           2=along time then trace;	\n"
"                           3=along time, trace and then line \n"
"\n"
"Author:	Zhiming Li		      		6-6-94		\n"
"Notes:									\n"
"  1. when iamp=2 or 3, and rms of input trace is zero, a value will be	\n"
"     interpolated from adjacent nonzero points when interp=1			\n"
"  2. to apply a smooth scaling function, it is recommended to run this \n"
"     program twice, first compute ampgrid using iamp=2, then apply	\n"
"     vsmoo3d  program to smooth the ampgrid, run subalance again using \n"
"     iamp=1 and ampgrid input						\n" 
"  3. if ntagrid, dtagrid and ftagrid are specified, and iamp=0, or 2, or 3, \n"
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
	int interp, itr;
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

	long long lpos;



	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparint("iamp",&iamp)) iamp = 0;
	if(iamp!=0) { 
		if (!getparstring("ampgrid",&agfile)) 
			err(" File ampgrid must be specified ");
		if (!getparint("nx",&nx)) 
			err("nx missing");
		if (!getparint("ny",&ny)) 
			err("ny missing");
		if (iamp==1) {
			if((agfp = fopen(agfile,"r"))==NULL)
                	err("Input ampgrid file %s not found \n",agfile);
			file2g(agfp);
			/* obtain grid header info */
			ierr = fgetghdr(agfp, &gh);
			if(ierr==0) fromghdr(&gh,&scale,&dtype,
					&n1,&n2,&n3,&n4,&n5,
                   	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                   	&dcdp2,&dline3,&ocdp2,&oline3,
					&vmin,&vmax,&orient,&gtype);
			if(nx!=n2) err(" check nx \n");
			if(ny!=n3) err(" check ny \n");
		} else {
			agfp = fopen(agfile,"w");
			file2g(agfp);
			ierr=1;			
			n4 = 1;
			n5 = 1;
			n2 = nx;
			n3 = ny;
			if (!getparfloat("dcdp2",&dcdp2)) 
				err("dcdp2 missing \n");
			if (!getparfloat("ocdp2",&ocdp2)) 
				err("ocdp2 missing \n");
			if (!getparfloat("dline3",&dline3)) 
				err("dline3 missing \n");
			if (!getparfloat("oline3",&oline3)) 
				err("oline3 missing \n");
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

fprintf(stderr,"\n");
fprintf(stderr,"starting trace of ampgrid: ocdp2=%g\n",ocdp2);
fprintf(stderr,"starting line of ampgrid: oline3=%g\n",oline3);
fprintf(stderr,"trace increment of ampgrid: dcdp2=%g\n",dcdp2);
fprintf(stderr,"line increment of ampgrid: dline3=%g\n",dline3);
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
			fseek(agfp,0,0);
			fread(rms,sizeof(float),nx*ny,agfp);
		}
	}


	itmax = (tmax-ft)/dt+0.5;
	if(itmax>nt) itmax = nt;

	ix = 0;
	iy = 0;
	itr = 0;
	if(iamp>0) fseek2g(agfp,0,0);

	/* loop over traces */
	do {
		iy = itr/nx; 
		ix = itr - iy*nx;
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

		}

		/* apply the scale */
		if(iamp==0 || iamp==1) {
			if(ntagrid>1) {
				if(iamp==1) {
					/* read in scale function */
					/*
					lpos = (iy*nx+ix);
					lpos = lpos * ntagrid*sizeof(float);
					fseek2g(agfp,lpos,0);
					*/
					fread(aread,sizeof(float),ntagrid,agfp);
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

		itr = itr + 1;
	} while (fgettr(infp,&tr));


	/* interpolating scales from nonzero points */
	if(interp>=1 && iamp>=2) {

		/* fill zeros in time first */
		if(ntagrid>1) {
		for(iy=0;iy<ny;iy++) {
			for(ix=0;ix<nx;ix++) {
				for(ii=0;ii<ntagrid;ii++) {
					jj = (iy*nx+ix)*ntagrid + ii;
					if(rms[jj] == 0.) {
	/*
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
	*/
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
		if(nx>1 && interp>=2) {
		for(iy=0;iy<ny;iy++) {
			for(ii=0;ii<ntagrid;ii++) {
				for(ix=0;ix<nx;ix++) {
					jj = (iy*nx+ix)*ntagrid+ii;
					if(rms[jj] == 0.) {
	/*
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
	*/
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
		if(ny>1 && interp==3) {
		for(ix=0;ix<nx;ix++) {
			for(ii=0;ii<ntagrid;ii++) {
				for(iy=0;iy<ny;iy++) {
					jj = (iy*nx+ix)*ntagrid+ii;
					if(rms[jj] == 0.) {
	/*
	fprintf(stderr, "input rms zero at ix=%d iy=%d it=%d \n",ix+1,iy+1,ii+1);
	*/
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
		o2 = ocdp2;
		d2 = dcdp2; 
		n3 = ny;
		o3 = oline3;
		d3 = dline3;
		orient = 1;
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

