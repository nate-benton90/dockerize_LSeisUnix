/* velocity interpolation from VS3D card input */

#include "ghdr.h"
#include "gridhd.h"
#include "velo.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"VGRID3D - convert VS3D cards to 3D interval velocity grid files \n"
"\n"
"vgrid3d [parameters] <vs3d-cards >3d-vgrid 				\n" 
"\n"
"Required parameters:						 	\n"
"vs3d-cards=       Name of dataset containing VS3D cards 		\n"
"fx=               First x coordinate to output velocity grid 	\n"
"                  (x is the lateral position in the inline direction)	\n"
"fy=               First y coordinate to output velocity grid 	\n"
"                  (y is the lateral position in the crossline direction)\n"
"dx=               x increment to output velocity grid 		\n"
"dy=               y increment to output velocity grid 		\n"
"nx=               number of x positions to output velocity grid 	\n"
"ny=               number of y positions to output velocity grid 	\n"
"nvs=              number of velocity (time/depth) slices to output 	\n"
"dvs=              velocity time/depth slice interval 			\n" 
"                  (in ms or m or ft) to output				\n"
"3d-vgrid=         Name of 3d velocity grid file 			\n"
"\n"
"Optional parameters:							\n"
"nvfmax=4096       maximum number of velocity functions in input vs3d   \n" 
"                  dataset     						\n" 
"ntvmax=256        maximum number of t-v pairs per velocity functions 	\n"
"                  in input vs3d   \n" 
"                  dataset     						\n" 
"vitype=0          input velocity type (1=interval 0=rms)	\n"
"votype=1          output velocity type (1=interval 0=rms 2=avergage)	\n"
"ivs=0             velocity slice mode (0=time; 1=depth)	\n"
"tslice=1          =1 time/depth slices output (nx by ny by nvs), 	\n"
"                  =0 velocity vectors (nvs by nx by ny)		\n"
"smx=1             Number of x positions to smooth grid before output \n"
"smy=1             Number of y positions to smooth grid before output \n"
"sms=1             Number of time/depth slices to smooth grid before output \n"
"rminv=0           Remove velocity inversion in the input 		\n"
"                  0= no; 1=yes delete the t-v pair; 			\n"
"                  2=replaced with previous velocity;			\n" 
"vscale=1.0        Scale to be applied to output velocity		\n"
"vmin=1000.        Minimum acceptable output velocity			\n" 
"vmax=100000.      Maximum acceptable output velocity 			\n" 
"verbose=1         1=print job progress; 0=no print 			\n" 
"vintpr=0          print interval velocity computed at input VS3D 	\n"
"                  locations (0=no 1=yes)				\n"
"intype=1          rms velocity interpolation to output time interval 	\n"
"                  before converting to interval velocity (0=no l=yes)  \n"
"nf=3              number of closest velocity functions used to 	\n"
"                  interpolate velocity at an output location		\n" 
"                  =0   use all the functions whose distance from the 	\n"
"                  output is less than dismax 				\n"
"                  use nf=3 when input velocity functions are about evenly \n" 
"                  spread on the 3D area \n"
"                  use nf=0 when input velocity functions are very unevenly \n"
"                  positioned on the 3D area \n"
"dismax=10000      maximum distance of which a velocity function can	\n"
"                  be used to interpolate output			\n" 
"ocdp2=0           starting trace number of the velocity grid \n"  
"dcdp2=0           trace number increment of the velocity grid \n"  
"oline3=0          starting line number of the velocity grid \n"  
"dline3=0          line number increment of the velocity grid \n"  
"tlastpick=        time (ms) of last t-v pick \n"
"                  if specified, the time of the last t-v pair will be \n"
"                  replaced with tlastpick value; otherwise ignored. \n" 
"vlastpick=        velocity (m/s or ft/s) of last t-v pick \n"
"                  if specified, the velocity of the last t-v pair will be \n"
"                  replaced with vlastpick value; otherwise ignored. \n" 
"noxyintp=0        input vs3d card at every lateral positions of the \n"
"                  output grid, no lateral interpolation need;  \n"
"                  0=no; 1=yes; (usefull for vs3d cards from auto-picker\n"
"                  at every cdp locations) \n"
"\n"
" Notes:								\n"
" 1. x is the lateral position in the inline direction 			\n"
"    (NOT the West-East direction !!).					\n" 
"    y is the lateral position in the crossline direction 		\n"
"    (NOT the South-North direction !!).				\n"
" 2. VS3D card has the following format					\n"
"1--4----------16------24------32------40------48------56------64------72 \n"
"VS3D          x1      y1      t1      v1      t2      v2      t3      v3 \n" 
"VS3D                          t4      v4      t5      v5      t6      v6 \n" 
"VS3D                          t7      v7      t8      v8                 \n" 
"VS3D          x2      y2      t1      v1      t2      v2      t3      v3 \n" 
"VS3D                          t4      v4                                 \n" 
"\n"
" where (xi,yi) is the velocity analysis location, (ti,vi) are time and \n"
" stacking velocity pairs. 						\n"
" 3. Output velocity grid file contains nvs time/depth slices of 	\n"
" velocities, i.e., velocity cube is stored in disk as v(x,y,t/z).	\n" 
" (NOT v(t/z,x,y)!), unless tslice=0;					\n"
" 4. Velocity low bound (vmin) and upper bound (vmax) are used to clip 	\n"
" the output velocities.						\n" 
"\n"
"AUTHOR:	   Zhiming Li,       ,	6/25/92   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp;
	FILE *datafp;

	float fx,fy,dx,dy,dvs,tmax,dt,x,y,tmp;
	int nx,ny,nvs,ivs,votype,vitype, ivo;
	int smx,smy,sms,one;
	int n1,n2,nxy,nxny,np,nf;
	int *nps, *indx;
	int i, j, ix, iy, is, j0, ip;
    	float *xs, *ys, *tpicks, *vpicks, *vv;
    	float *work, *vx, *vtx, *vxy, *vi;
    	float *fsms,*fsmx,*fsmy,*depth,*time,*zs;
	float *vgrid;
	float vmin, vmax, vscale;
	int vintpr, intype, jj;
	float dismax;
	int rminv;
	int noxyintp=0;

	int verbose=1;

	int tslice;
	ghed gh;
	float gmin, gmax;
	int ierr;
	int orient=4, gtype=0;
	int ocdp2, dcdp2, oline3, dline3;
	float tlastpick, vlastpick;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	/* required parameters */
	if (!getparfloat("fx",&fx)) 
		err(" fx missing ");
	if (!getparfloat("fy",&fy)) 
		err(" fy missing ");
	if (!getparfloat("dx",&dx)) 
		err(" dx missing ");
	if (!getparfloat("dy",&dy)) 
		err(" dy missing ");
	if (!getparint("nx",&nx)) 
		err(" nx missing ");
	if (!getparint("ny",&ny)) 
		err(" ny missing ");
	if (!getparint("nvs",&nvs)) 
		err(" nvs missing ");
	if (!getparfloat("dvs",&dvs)) 
		err(" dvs missing ");
	/* optional parameters */
	if (!getparint("ivs",&ivs)) ivs = 0;
	if (!getparint("vitype",&vitype)) vitype = 0;
	if(vitype!=0 && vitype!=1 ) err("check vitype");
	if (!getparint("votype",&votype)) votype = 1;
	if (!getparint("smx",&smx)) smx = 1;
	if (!getparint("smy",&smy)) smy = 1;
	if (!getparint("sms",&sms)) sms = 1;
	if (!getparfloat("vscale",&vscale)) vscale = 1.0; 
	if (!getparfloat("vmin",&vmin)) vmin = 1000.0; 
	if (!getparfloat("vmax",&vmax)) vmax = 100000.0; 
	if (!getparint("tslice",&tslice)) tslice = 1;
	if (!getparint("vintpr",&vintpr)) vintpr = 0;
	if (!getparint("intype",&intype)) intype = 1;
	if (!getparint("verbose",&verbose)) verbose=1;
	if (!getparint("nf",&nf)) nf=3;
	if (!getparfloat("dismax",&dismax)) dismax=10000.;
	if (!getparint("rminv",&rminv)) rminv=0;
	if (!getparint("ocdp2",&ocdp2)) ocdp2=0;
	if (!getparint("dcdp2",&dcdp2)) dcdp2=0;
	if (!getparint("oline3",&oline3)) oline3=0;
	if (!getparint("dline3",&dline3)) dline3=0;
	if (!getparfloat("tlastpick",&tlastpick)) tlastpick=-99999.;
	if (!getparfloat("vlastpick",&vlastpick)) vlastpick=-99999.;
	if (!getparint("noxyintp",&noxyintp)) noxyintp=0;

	if (tslice==1) {
		orient = 4;
	} else if (tslice==0) {
		orient = 1;
	}

	if (votype==0) {
		gtype = 1;
	} else if(votype==1) {
		gtype = 3;
	} else if(votype==2) {
		gtype = 2;
	}

	/* if input and output velocity are the same, skip 
	    the step to	convert the velocity to interval velocity
		first */
	if(votype==vitype) {
		votype = 1;
		vitype = 1;
	}
	
    	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
		pairs each */

	if (!getparint("nvfmax",&n2)) n2 = 4096;
	if (!getparint("ntvmax",&n1)) n1 = 256;

   	/* arrays used to store all VELF card's cdp, time and velocity */
   	xs = (float*)emalloc(n2*sizeof(float));
   	ys = (float*)emalloc(n2*sizeof(float));
   	tpicks = (float*)emalloc(n1*n2*sizeof(float));
   	vpicks = (float*)emalloc(n1*n2*sizeof(float));
   	vv = (float*)emalloc(n1*sizeof(float));
   	time = (float*)emalloc(nvs*sizeof(float));
   	nps = (int*)emalloc(n2*sizeof(int));
	
	bzero(nps,n2*sizeof(int));
   	/* read in VS3D cards */
   	nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,n1,n2);

	fprintf(stderr," %d VS3D cards read \n",nxy);

	if(tlastpick!=-99999.) {
		for(i=0;i<nxy;i++) {
			tpicks[nps[i]-1+i*n1]=tlastpick;
		}
	}
	if(vlastpick!=-99999.) {
		for(i=0;i<nxy;i++) {
			vpicks[nps[i]-1+i*n1]=vlastpick;
		}
	}
	/*
	for(i=0;i<nxy;i++) {
		for(j=0;j<nps[i];j++)
			fprintf(stderr,"x=%f y=%f t=%f v=%f \n",
				xs[i],ys[i],tpicks[j+i*n1],vpicks[j+i*n1]); 
		fprintf(stderr,"\n");
	}
	*/

   	if (nxy==0) err("No VS3D card input ! Job aborted");

	if (rminv>0) removeinv(tpicks,vpicks,nps,nxy,n1,rminv);
	
	/* find out the maximum time */
	tmax = 0.; 
	for(i=0;i<nxy;i++) {
		if(tpicks[i*n1+nps[i]-1] > tmax) tmax = tpicks[i*n1+nps[i]-1];
	}
	/* compute constant time intervals*/
	dt = tmax/(nvs-1);
	for(i=0;i<nvs;i++) time[i] = i*dt;

	vi = (float*) emalloc(nxy*nvs*sizeof(float));
	work = (float*) emalloc(nvs*sizeof(float));
	indx = (int*) emalloc(nvs*sizeof(int));
	fsmx = (float*) emalloc(smx*sizeof(float));
	fsmy = (float*) emalloc(smy*sizeof(float));
	fsms = (float*) emalloc(sms*sizeof(float));
	if(tslice==0 && (smx>1 || smy>1) ) vgrid = (float*) emalloc(nvs*nx*ny*sizeof(float));

	depth = (float*) emalloc(nvs*sizeof(float));
	zs = (float*) emalloc(nvs*sizeof(float));
	for(j=0;j<nvs;j++) zs[j] = j * dvs;
	
	one = 1;

	for(i=0;i<nxy;i++) {
		if(verbose==1) fprintf(stderr, 	
			" %d t-v pairs read at x=%f y=%f location\n",
			nps[i],xs[i],ys[i]);
		j0 = i*nvs;
		np = nps[i];

		if(ivs==0) {
			if(intype==1) {	
			/* interpolate vs3d to nvs times */
				lin1d_(tpicks+i*n1,vpicks+i*n1,&np,zs,
                       			vi+j0,&nvs,indx);
				tmax = tpicks[i*n1+np-1];

				/* compute interval velocities using
					dix formula */
				if(vitype==0) {
					work[0] = vi[j0]*vi[j0];
					for(j=1;j<nvs;j++) {
					if(zs[j]<=tmax) {
						work[j]=(vi[j+j0]*vi[j+j0]*
								zs[j] -  	
				   			vi[j-1+j0]*vi[j-1+j0]*
								zs[j-1]) /dvs;
					} else {
						work[j] = work[j-1]; 
					}
					}
					for(j=0;j<nvs;j++) {
					if(work[j]<0.) 
		err("check interval velocity at t=%g x=%g y=%g location \n",
					j*dvs,xs[i],ys[i]); 

						vi[j0+j] = sqrt(work[j]);
					}
				}
			} else {
				if(vitype==0) {
					vv[0] = vpicks[i*n1];
					for(j=1;j<np;j++) {
					vv[j]=(vpicks[i*n1+j]*vpicks[i*n1+j]*
						tpicks[i*n1+j] -  	
					      vpicks[i*n1+j-1]*vpicks[i*n1+j-1]*
						tpicks[i*n1+j-1]) /  
				   	      (tpicks[i*n1+j]-tpicks[i*n1+j-1]);
					if(vv[j]<0.) 
		err("check interval velocity at t=%g x=%g y=%g location \n",
					tpicks[i*n1+j],xs[i],ys[i]); 

						vv[j] = sqrt(vv[j]);
					}
					bisear_(&np,&nvs,tpicks+i*n1,zs,indx);
					for(j=0;j<nvs;j++) {
						jj = indx[j] - 1;
					  if(jj<0 || zs[j]<= tpicks[i*n1] ) {
						vi[j0+j] = vv[0];
					  } else if(jj>np-2) {
						vi[j0+j] = vv[np-1];
					  } else if(abs(zs[j]-tpicks[i*n1+jj])
						<0.001*dvs){
						vi[j0+j] = vv[jj];
					  } else {
						vi[j0+j] = vv[jj+1];
					  }   
				    }
				} else {
					lin1d_(tpicks+i*n1,vpicks+i*n1,&np,zs,
                       				vi+j0,&nvs,indx);
				}
			}
		} else {
		/* output in depth */
			if(intype==1) {
                        /* interpolate vs3d to nvs times */
                                lin1d_(tpicks+i*n1,vpicks+i*n1,&np,time,
                                        vi+j0,&nvs,indx);
                                tmax = tpicks[i*n1+np-1];
                                /* compute interval velocities using
                                        dix formula */

				if(vitype==0) {
                                	work[0] = vi[j0]*vi[j0];
                                  for(j=1;j<nvs;j++) {
                                        if(time[j]<=tmax) {
                                                work[j]=(vi[j+j0]*vi[j+j0]*
                                                                time[j] -
                                                        vi[j-1+j0]*vi[j-1+j0]*
                                                                time[j-1]) /dt;
                                        } else {
                                                work[j] = work[j-1];
                                        }
				  }
                                  for(j=0;j<nvs;j++) {
				  if(work[j]<0.) 
		err("check interval velocity at t=%g x=%g y=%g location \n",
					j*dvs,xs[i],ys[i]); 
					work[j] = sqrt(work[j]);
				  }
				} else {
                                  for(j=0;j<nvs;j++)
                                                work[j]=vi[j+j0];
				}
			} else {
				if(vitype==0) {
				  vv[0] = vpicks[i*n1];
                                  for(j=1;j<np;j++) {
                                        vv[j]=(vpicks[i*n1+j]*vpicks[i*n1+j]*
                                                tpicks[i*n1+j] -
                                              vpicks[i*n1+j-1]*vpicks[i*n1+j-1]*
                                                tpicks[i*n1+j-1]) / 
                                              (tpicks[i*n1+j]-tpicks[i*n1+j-1]);

					if(vv[j]<0.) 
	err("check interval velocity at t=%g x=%g y=%g location \n",
			tpicks[i*n1+j],xs[i],ys[i]); 

					vv[j] = sqrt(vv[j]);
                                  }
				} else {
                                  for(j=1;j<np;j++) {
					vv[j] = vpicks[i*n1+j];
				  }
           			}

                                bisear_(&np,&nvs,tpicks+i*n1,time,indx);
                                for(j=0;j<nvs;j++) {
                                        jj = indx[j] - 1;
                                        if(jj<0 || time[j]<= tpicks[i*n1] ) {
                                                work[j] = vv[0];
                                        } else if(jj>np-2) {
                                                work[j] = vv[np-1];
                                        } else if(abs(time[j]-tpicks[i*n1+jj])
                                                <0.001*dt){
                                                work[j] = vv[jj];
                                        } else {
                                                work[j] = vv[jj+1];
                                        }  
                                }

			}
			depth[0] = time[0]*work[0]*0.5*0.001;
			for(j=1;j<nvs;j++) {
				depth[j] = depth[j-1]+
					(time[j]-time[j-1])*work[j]*0.5*0.001;
			}
			lin1d_(depth,work,&nvs,zs,vi+j0,&nvs,indx);
		}
		if(vintpr==1) {
			for(j=0;j<nvs;j++) {
				fprintf(stderr, "  time/depth=%g   vint=%g \n",
						 zs[j],vi[j+j0]);
			}
		}
		/* convert to rms velocity if needed */
		if(votype==0) {
			tmp = 0.;
			for(j=1;j<nvs;j++) {
				tmp = tmp + vi[j0+j]*vi[j0+j]*dvs;
				vi[j0+j] = sqrt(tmp/zs[j]);
			}
		/* convert to average velocity if needed */
		} else if(votype==2) {
			tmp = 0.;
			for(j=1;j<nvs;j++) {
				tmp = tmp + vi[j0+j]*dvs;
				vi[j0+j]=tmp/zs[j];
			}
		}
		/* check velocity range and apply scale */
		for(j=0;j<nvs;j++) {
			tmp = vi[j0+j]*vscale;
			if(tmp<vmin) tmp = vmin;	
			if(tmp>vmax) tmp = vmax;	
			vi[j0+j] = tmp;
		}
		/* smooth if needed */
		if(sms>1) smth2d_(vi+j0,work,fsms,fsmx,&nvs,&one,&sms,&one);

	}

	free(tpicks);
	free(vpicks);
	free(nps);
	free(zs);
	free(depth);
	free(time);
	free(fsms);
	
	
	/* output time/depth slices */
	free(work);
	free(indx);
	work = (float*) malloc(nxy*sizeof(float));
	vtx = (float*) malloc(nvs*nx*sizeof(float));
	vx = (float*) malloc(nx*sizeof(float));
	indx = (int*) malloc(nxy*sizeof(int));

	if(smx==1 && smy==1) {
		datafp = stdout;
	} else {
		datafp = etempfile(NULL);
	}

	np = nf;

	if(smx==1 && smy==1 && tslice==0 ) outfp = stdout;
	fprintf(stderr," Start x-y plane interpolation \n"); 

	/* 2d interpolation */
	for(iy=0;iy<ny;iy++) {
		y = fy + iy*dy;
		for(ix=0;ix<nx;ix++) {
			x = fx + ix*dx;
			if(noxyintp==0) {
				if(nf==0) {
					plint_(xs,ys,vi,&nvs,&nxy,&x,&y,
					vtx+ix*nvs,work,&dismax,indx);
				} else {
					intp2d_(xs,ys,vi,&nvs,&nxy,&x,&y,
					vtx+ix*nvs,&np,indx,work);
				}
			} else {
				bcopy(vi+(iy*nx+ix)*nvs,vtx+ix*nvs,nvs*sizeof(float));
			}
			/*
			fprintf(stderr," x-y plane interpolation done for x=%f y=%f\n",x,y); 
			*/
		}
		if(tslice==1) {	
			for(is=0;is<nvs;is++) {
				for(ix=0;ix<nx;ix++) vx[ix]=vtx[is+ix*nvs];
				ip = (is*nx*ny+iy*nx)*sizeof(float);
				efseek(datafp,ip,0);
				efwrite(vx,sizeof(float),nx,datafp);
			}
		} else {
			if(smx>1 || smy>1) {
				bcopy(vtx,vgrid+iy*nx*nvs,nx*nvs*sizeof(float));
			} else {
				efwrite(vtx,sizeof(float),nvs*nx,outfp); 
			}
		} 
		if(smx==1 && smy==1) {
			fminmax(vtx,nvs*nx,&gmin,&gmax);
			if(iy==0) {
				vmin = gmin;
				vmax = gmax;
			} else {
				if(vmin>gmin) vmin = gmin;
				if(vmax<gmax) vmax = gmax;
			}
		}
	}

	fprintf(stderr," x-y plane interpolation done \n"); 
	free(work);
	free(indx);
	free(vtx);
	free(xs);
	free(ys);
	free(vi);
	free(vx);
	
	if(smx>1 || smy>1) {
		if(tslice==1) rewind(datafp);
		outfp = stdout;
		nxny = nx * ny;
		vxy = (float*) emalloc(nxny*sizeof(float));
		work = (float*) emalloc(nxny*sizeof(float));
		for(is=0;is<nvs;is++) {
			if(tslice==1) {
				efread(vxy,sizeof(float),nxny,datafp);
			} else {
				for(iy=0;iy<ny;iy++)
					for(ix=0;ix<nx;ix++)
						vxy[ix+iy*nx]=
						   vgrid[(iy*nx+ix)*nvs+is];
			}
			smth2d_(vxy,work,fsmx,fsmy,&nx,&ny,&smx,&smy);
			if(tslice==1) {
				efwrite(vxy,sizeof(float),nxny,outfp);
			} else {
				for(iy=0;iy<ny;iy++)
					for(ix=0;ix<nx;ix++)
						vgrid[(iy*nx+ix)*nvs+is]=
							vxy[ix+iy*nx];
			}
			fminmax(vxy,ny*nx,&gmin,&gmax);
			if(is==0) {
				vmin = gmin;
				vmax = gmax;
			} else {
				if(vmin>gmin) vmin = gmin;
				if(vmax<gmin) vmax = gmax;
			}
		}
		free(vxy);
		free(work);
	}
	free(fsmx);
	free(fsmy);

	if(tslice==0) {
		if(smx>1 || smy>1) {
			for(i=0;i<nx*ny;i++) 
				efwrite(vgrid+i*nvs,sizeof(float),nvs,outfp);
		}
	} else {
		if(smx==1 && smy==1) outfp = datafp; 
	}

	fflush(outfp);


	bzero((char*)&gh,GHDRBYTES);
	gh.scale = 1.;
	if(tslice==1) {
		putgval(&gh,"n1",(float)nx);
		putgval(&gh,"n2",(float)ny);
		putgval(&gh,"n3",(float)nvs);
		putgval(&gh,"o1",fx);
		putgval(&gh,"o2",fy);
		putgval(&gh,"o3",0.);
		putgval(&gh,"d1",dx);
		putgval(&gh,"d2",dy);
		putgval(&gh,"d3",dvs);
	} else {
		putgval(&gh,"n1",(float)nvs);
		putgval(&gh,"n2",(float)nx);
		putgval(&gh,"n3",(float)ny);
		putgval(&gh,"o1",0.);
		putgval(&gh,"o2",fx);
		putgval(&gh,"o3",fy);
		putgval(&gh,"d1",dvs);
		putgval(&gh,"d2",dx);
		putgval(&gh,"d3",dy);
	}


	putgval(&gh,"dtype",4.);
	putgval(&gh,"gmin",vmin);
	putgval(&gh,"gmax",vmax);
	putgval(&gh,"gtype",gtype);
	putgval(&gh,"orient",orient);
	putgval(&gh,"ocdp2",ocdp2);
	putgval(&gh,"dcdp2",dcdp2);
	putgval(&gh,"oline3",oline3);
	putgval(&gh,"dline3",dline3);


	ierr = fputghdr(outfp,&gh);
	if(ierr!=0) err("error output grid header ");
	return 0;

}
