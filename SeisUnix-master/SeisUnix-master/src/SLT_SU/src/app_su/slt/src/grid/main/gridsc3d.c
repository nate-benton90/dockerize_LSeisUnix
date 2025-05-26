/* GRIDSCALE grid scale program */

#include "usgrid.h"
#include "velo.h"
#include "par.h"

char *sdoc = 
"GRIDSC3D - 3d grid scale program					\n"
"\n"
"gridsc3d SC3D.cards= [parameters] <grid.input >grid.output		\n" 
"\n"
"Required parameters:							\n"
"SC3D.cards=        name of SC3D card dataset 				\n"
"grid.input=        input 3D grid to be scaled \n"
"grid.output=       scaled 3D grid \n"
"                   when grid.input is not given, the output will be    \n"
"                   3D grid of scale values interpolated from SC3D.cards \n"
"Optional parameters:							\n"
"orient=1           type of grid:		  			\n"
"                   1=grid stored as (t,x,y) order (time vector)	\n"
"                   4=grid stored as (x,y,t) order (time slice)		\n"
"ncdmax=4096        maximum number of (x,y) locations in input SC3D cards \n"
"nprmax=256         maximum number of t-scale pairs per input SC3D card \n"
"nf=0               number of closest scale functions used to     \n"
"                   interpolate scale at an output location       \n"
"                  =0   use all the functions whose distance from the   \n"
"                  output is less than dismax               \n"
"                  use nf=4 when input scale functions are about evenly \n"
"                  spread on the 3D area \n"
"                  use nf=0 when input scale functions are very unevenly \n"
"                  positioned on the 3D area \n"
"dismax=10000      maximum distance of which a velocity function can    \n"
"                  be used to interpolate output            \n"
"The following 9 parameters will be used to output 3D grid of the scale \n"
"function, the grid.input is ignored when these 9 parameter are given. 	\n"
"ft=                starting t position of output grid 			\n" 
"dt=                t increment of output grid 				\n" 
"nt=                number of t positions of output grid 		\n" 
"fx=                starting x position of output grid 			\n" 
"dx=                x increment of output grid 				\n" 
"nx=                number of x positions of output grid 		\n" 
"fy=                starting y position of output grid 			\n" 
"dy=                y increment of output grid 				\n" 
"ny=                number of y positions of output grid 		\n" 
"NOTES:						 			\n"
" 1. SC3D (3D scale) card has the following format                        \n"
"1--4----------16------24------32------40------48------56------64------72 \n"
"SC3D          x1      y1      t1      s1      t2      s2      t3      s3 \n"
"SC3D                          t4      s4      t5      s5      t6      s6 \n"
"SC3D                          t7      v7      t8      v8                 \n"
"SC3D          x2      y2      t1      s1      t2      s2      t3      s3 \n"
"SC3D                          t4      s4                                 \n"
"\n"
"    where (x, y) is the location of scale function, t is time and s is	\n"
"    the scale to be applied at time t.					\n" 
"\n"
" 2. units of x,y,t  in SC3D must be the same as those in input grid	\n"
"    header\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	5/10/93   \n"		    
;

void sc3dread(FILE *infp, float *xs, float *ys, float *ts, float *ss,
        int *nxy, int *nps, int maxp, int maxnxy);

int main(int argc, char **argv)
{
	FILE *infp, *outfp=stdout, *sc3dfp;
	char *sc3d;

    	int n1,n2,i,ix,iy,it,j0,nxy,nvs;
	int nx,ny,nt,np;
	float dx,dy,dt,fx,fy,ft,x,y;
	int orient=1; 

	int ierr;

	usghed usgh;

	float *xs, *ys, *ts, *ss, *time;
	int *nps, *indx;

	float *work, *s3d, *gin, *si;
	float gmin, gmax;
	float dismax;
	int nf;

	int noinput=0;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input parameters */
	if(!getparstring("SC3D.cards",&sc3d)) err("must specify SC3D.cards");
	if(!getparint("orient",&orient)) {
		if(!getparint("gtype",&orient)) {
			orient = 1;
		} else {
			/* compatible with old version */
			if(orient==1) orient=4;
			if(orient==0) orient=1;
		}
	}
	if(orient!=1 && orient!=4) err(" check parameter orient");
	sc3dfp = fopen(sc3d,"r");

	if( getparfloat("fx",&fx) 
	 && getparfloat("dx",&dx) 
	 && getparint("nx",&nx)  
	 && getparfloat("fy",&fy) 
	 && getparfloat("dy",&dy) 
	 && getparint("ny",&ny)  
	 && getparfloat("ft",&ft) 
	 && getparfloat("dt",&dt) 
	 && getparint("nt",&nt) ) {
		noinput = 1;
	} else {
		infp = stdin;
		noinput = 0;
	}

	/* get input grid parameters */
	if(noinput==0) {
    		ierr = fgetusghdr(infp,&usgh);
		if(ierr!=0) err(" input grid header error "); 
		if(usgh.orient!=0 && orient!=usgh.orient)
			err(" check orient in grid header "); 
		gmin = usgh.gmax;
		gmax = usgh.gmin;
		if(orient==1) {
			ft = usgh.o1; dt = usgh.d1; nt = usgh.n1;
			nvs = usgh.n1;
			fx = usgh.o2; dx = usgh.d2; nx = usgh.n2;
			fy = usgh.o3; dy = usgh.d3; ny = usgh.n3;
		} else if(orient==4) {
			ft = usgh.o3; dt = usgh.d3; nt = usgh.n3;
			nvs = usgh.n3;
			fx = usgh.o1; dx = usgh.d1; nx = usgh.n1;
			fy = usgh.o2; dy = usgh.d2; ny = usgh.n2;
		} 
	} else {
	/* update output grid parameters */
		nvs = nt;
                usgh.scale = 1.;
		usgh.dtype = 4;
		usgh.n4 = 1;
		usgh.n5 = 1;
		usgh.o4 = 0.;
		usgh.o5 = 0.;
		usgh.d4 = 1.;
		usgh.d5 = 1.;
		usgh.ocdp2 = 1;
		usgh.dcdp2 = 1;
		usgh.dline3 = 1;
		usgh.oline3 = 1;
		if(orient==1) {
                	usgh.o1 = ft; usgh.d1 = dt; usgh.n1 = nt;
                	usgh.o2 = fx; usgh.d2 = dx; usgh.n2 = nx;
                	usgh.o3 = fy; usgh.d3 = dy; usgh.n3 = ny;
			usgh.orient = 1;
		} else if(orient==4) {
                	usgh.o3 = ft; usgh.d3 = dt; usgh.n3 = nt;
                	usgh.o1 = fx; usgh.d1 = dx; usgh.n1 = nx;
                	usgh.o2 = fy; usgh.d2 = dy; usgh.n2 = ny;
			usgh.orient = 4;
		}
	}

	/* at most 4096 input (x,y) SC3D cards with at most 256 time-scal
                pairs each */

   if( !getparint("ncdmax",&n2) ) n2 = 4096;
   if( !getparint("nprmax",&n1) ) n1 = 256;
	
   if( !getparint("nf",&nf) ) nf = 0;
   if( !getparfloat("dismax",&dismax) ) dismax = 10000;

	/* arrays used to store all SCAL card's x, y, time and scale */
    xs = (float*)malloc(n2*sizeof(int));
    ys = (float*)malloc(n2*sizeof(int));
    ts = (float*)malloc(n1*n2*sizeof(float));
    ss = (float*)malloc(n1*n2*sizeof(float));
    nps = (int*)malloc(n2*sizeof(int));

	bzero(nps,n2*sizeof(int));
        /* read in SC3D cards */
        nxy = 0;
        sc3dread(sc3dfp,xs,ys,ts,ss,&nxy,nps,n1,n2);
	fprintf(stderr," %d SC3D cards read \n",nxy);
	if (nxy==0) err("No SC3D card input ! Job aborted");


        time = (float*)malloc(nvs*sizeof(float));
        for(i=0;i<nvs;i++) time[i] = ft + i*dt;

	si = (float*) emalloc(nxy*nvs*sizeof(float));
        indx = (int*) emalloc(nvs*sizeof(int));

	for(i=0;i<nxy;i++) {
                fprintf(stderr, "%d t/z-scale pairs read at x=%f y=%f location\n",
                        nps[i],xs[i],ys[i]);
                j0 = i*nvs;
                np = nps[i];
                /* interpolate sc3d to nvs times */
                lin1d_(ts+i*n1,ss+i*n1,&np,time,
                       si+j0,&nvs,indx);
	}
	free(indx);

	if(noinput==1) {
		gmin = si[0];
		gmax = si[0];
	}



	/* apply scale to input grid  and output */
	if(orient==1) {
		s3d = (float*) malloc(nt*sizeof(float));
		work = (float*) malloc(nxy*sizeof(float));
		gin = (float*) malloc(nt*sizeof(float));
		indx = (int*) malloc(nxy*sizeof(int));
		for(iy=0;iy<ny;iy++) {
           	y = fy + iy*dy;
           	for(ix=0;ix<nx;ix++) {
               	x = fx + ix*dx;
		if(nxy>1) {
			if(nf==0) {
				plint_(xs,ys,si,&nvs,&nxy,&x,&y,
				s3d,work,&dismax,indx);
			} else {
				np = nf;
             			intp2d_(xs,ys,si,&nvs,&nxy,&x,&y,
       	                	s3d,&np,indx,work);
			}
		} else {
			for(it=0;it<nvs;it++) s3d[it] = si[it];
		}
		if(noinput==0) {
			efread(gin,sizeof(float),nt,infp);
		} else {
			for(it=0;it<nt;it++) gin[it] = 1.0;
		}
		for(it=0;it<nt;it++) {
			gin[it] *= s3d[it];
			if(gmin>gin[it]) gmin = gin[it];
			if(gmax<gin[it]) gmax = gin[it];
		}
		efwrite(gin,sizeof(float),nt,outfp);
		}
  		}
	} else if(orient==4) {
		s3d = (float*) emalloc(nx*ny*nt*sizeof(float));
        	work = (float*) emalloc(nxy*sizeof(float));
        	indx = (int*) emalloc(nxy*sizeof(int));
        	gin = (float*) emalloc(nx*ny*sizeof(float));

		for(iy=0;iy<ny;iy++) {
        	y = fy + iy*dy;
            	for(ix=0;ix<nx;ix++) {
            	x = fx + ix*dx;
		j0 = ix*nt+iy*nx*nt;
		if(nf==0) {
               		plint_(xs,ys,si,&nvs,&nxy,&x,&y,
                   	s3d+j0,work,&dismax,indx);
		} else {
			np = nf;
                	intp2d_(xs,ys,si,&nvs,&nxy,&x,&y,
                   	s3d+j0,&np,indx,work);
		}
		}
		}

		for(it=0;it<nt;it++) {
			if(noinput==0) {
				efread(gin,sizeof(float),nx*ny,infp);
			} else {
				for(ix=0;ix<nx*ny;ix++) gin[ix] = 1.0;
			}
			for(iy=0;iy<ny;iy++) {
				for(ix=0;ix<nx;ix++) {
					j0 = iy*nx+ix;
					gin[j0] *= s3d[it+j0*nt];
					if(gmin>gin[j0]) gmin = gin[j0];
					if(gmax<gin[j0]) gmax = gin[j0];
				}
			}
			efwrite(gin,sizeof(float),nx*ny,outfp);
		}

	}	
		
	usgh.gmin = gmin;
	usgh.gmax = gmax;

    	ierr = fputusghdr(outfp,&usgh);

	if(ierr!=0) err("error in fputusghdr");
	
	exit(0);
}

/* read in SCAL cards from input file pointer infp (SCAL card dataset) */
/* input:                                                               */
/*      infp                    file pointer for SCAL card dataset      */
/*      maxp                    maximum number of t-s pairs per (x,y)   */
/*      maxnxy                  maximum number of (x,y)'s of SC3D cards */
/* ouput:                                                               */
/*      xs[maxnxy]              x location of SC3D card                 */
/*      ys[maxnxy]              y location of SC3D card                 */
/*      ts[maxnxy][maxp]        time of picks (ms)                      */
/*      ss[maxnxy][maxp]        velocity of picks                       */
/*      nxy                     total number of (x,y)'s found in SC3D cards*/
/*      nps[maxnxy]             number t-s pairs per (x,y)              */
/*                                                                      */
/* author:      Zhiming Li              5/93                            */

void sc3dread(FILE *infp, float *xs, float *ys, float *ts, float *ss,
        int *nxy, int *nps, int maxp, int maxnxy) {

        int icmax, ic, cdpnow=0, cdppre=-1, cdpchange=0, jc=0;
        int i1, i;
	float ftime, fscale;
        char *cbuf, x[9], y[9], scale[9], time[9];
        int cardfound=0;
        float xnow, ynow, xpre, ypre;

        icmax = maxnxy * maxp;

        cbuf = (char *) malloc(81*sizeof(char));

        /* rewind infp */
        efseek(infp,0,0);


        for (ic=0;ic<icmax;ic++) {
                if(feof(infp) !=0) break;
                bzero(cbuf,81);
                fgets(cbuf,81,infp);

                if(strncmp(cbuf, "SC3D",4)==0) {
                        cardfound = 1;

			x[8] = '\0';
                        strncpy(x,&cbuf[8],8);
                        if(strncmp(x, "        ",8)!=0) {
                                xnow = atof(x);
                        } else {
                                xnow = xpre;
                        }
			y[8] = '\0';
                        strncpy(y,&cbuf[16],8);
                        if(strncmp(y, "        ",8)!=0) {
                                ynow = atof(y);
                        } else {
                                ynow = ypre;
                        }

                        if(cdppre == -1) {
                                xpre = xnow;
                                ypre = ynow;
                                cdppre = 0;
                        }

                        if( (xpre==xnow && ypre==ynow) ) {
                                cdpnow = cdppre;
                        } else {
                                cdpnow = cdppre + 1;
                        }

                        /* if cdp changes */
                        if (cdpnow != cdppre ) {
                                nps[cdpchange] = jc;
                                xs[cdpchange] = xpre;
                                ys[cdpchange] = ypre;
                                cdpchange += 1;
                                jc = 0;
                                cdppre = cdpnow;
                                xpre = xnow;
                                ypre = ynow;
                        }

                        /* store read values in tpicks and vpicks arrays */
                        for(i=0;i<3;i++) {
				ftime = 0.;
				fscale = 0.;
				time[8] = '\0';
                                strncpy(time,&cbuf[24+i*16],8);
				sscanf(time,"%f",&ftime);
				scale[8] = '\0';
                                strncpy(scale,&cbuf[32+i*16],8);
				sscanf(scale,"%f",&fscale);

                                if(ftime==0. && fscale==0. && jc>0) break;

                                ts[jc+cdpchange*maxp] = ftime;
                                ss[jc+cdpchange*maxp] = fscale;
                                jc = jc + 1;
                        }
                }
        }
/* last input cdp location */
        if(cardfound==1) {
                xs[cdpchange] = xpre;
                ys[cdpchange] = ypre;
                nps[cdpchange] = jc;
                *nxy = cdpchange + 1;
        } else {
                *nxy = 0;
        }
        if(*nxy>maxnxy)
                err("number of (x,y) of SC3D cards exceeds %d \n",maxnxy);
        free(cbuf);
}

