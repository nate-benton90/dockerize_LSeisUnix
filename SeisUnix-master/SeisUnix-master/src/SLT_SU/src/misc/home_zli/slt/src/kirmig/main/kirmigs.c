/*
  RPC-DECL { kirmigs(in char* datain,out char* dataout,int lseekin,int lseekout,
	             char *timefile,char *ampfile,int iamp,
	   	     int nt,int nx,int nz,int lt,
	             float tmin,float x0,float z0,
	             float dt,float dx,float dz,
	             float smint,float xmint,float zmint,
	             float dst,float dxt,float dzt,
	             int nst,int nxt,int nzt,
	             float ofimin,float ofimax,
	             float ofomin,float dofo,int nofo,
		     inout int *mtrace,int i2p5,
		     int intps,int intpx,int intpz,int intype, float dcdp,
	             int amptype,int mlimit,int lagc,float tpow,
            	     float aper, float apanl, float apanr, 
		     float v0, int cdpx0, int dcdpx, char *dipfile,
		     float dxdip, float dxtol); }
    RPC-MAIN-DECL {
extern "C" {
    void initargs(int argc,char *argv[]) ;
}
    }

    RPC-MAIN-CODE {
	initargs(argc, argv);
    }
*/

#include <stdio.h>
#include <osfcn.h>
#include <math.h>
#include <sys/param.h>
#include "grid.h"
#include "su.h"
#include "segy.h"


segytrace tra;

extern "C" {
void *memcpy(void *trr, void *, int );
void f2p5_(...);
void kirmig_(...);
void agc_(...);
void tp_(...);
void bzero(void *, int );
void dipsgrid_cpp(char *dfile, float *dipl, float *dipr,
        double z0, double dz, int nz, int fcdp, int dcdp, int ncdp);
}



int kirmigs(char *datain, char *dataout, int lseekin, int lseekout, 
	    char *timefile, char *ampfile, int iamp,
	    int nt, int nx, int nz, int lt, 
	    float itmin, float iix0, float iz0, 
	    float idt, float idx, float idz,
	    float ismint, float ixmint, float izmint, 
	    float idst, float idxt, float idzt,
	    int nst, int nxt, int nzt,
	    float iofimin, float iofimax,
	    float iofomin, float idofo, int nofo,
	    int *mtrace, int i2p5,
	    int intps, int intpx, int intpz,int intype, float idcdp,
	    int amptype,int mlimit, int lagc, float itpow,
            float iaper, float iapanl, float iapanr, float iv0, 
	    int cdpx0, int dcdpx, char *dipfile, float idxdip, float idxtol) {

/* subroutine kmigs */

	int ix, iz, it, i;
	float tminl;
	float *trace, *trt, *migs, *fold, *tr, *ts, *ar, *as, *sigxt, *sigzt;
	int *inxt, *inzt;
	short *ttbl, *atbl;
	float ascale, tscale;
	long sx, gx;
	int ntrace, iofo;
	int ix0, iof, latbl;
	float xs, xr, tmp, *trz, *trr,dldt,dl,offout,dlm;
	float *twk1,*twk2,*awk1,*awk2,*trwk;
	FILE *tfp, *afp;
	FILE *infp, *outfp;
	int mute;
	int chkcdp;
	long sy, gy, icdp1;
	float offset;
	char *afile, *tfile;
	int lttbl,llimit;
	int lmem ;
	int lwin, mt, notrace, itmp;
	float rmso, zw;
	float *dips;
	int idip;
	int isize;

        /* convert double to float */
	float tmin = itmin , x0 = iix0 , z0 = iz0 ; 
	float dt = idt , dx = idx , dz =idz ;
	float smint = ismint , xmint = ixmint , zmint = izmint ;
	float dst = idst , dxt = idxt , dzt = idzt ;
	float ofimin = iofimin , ofimax = iofimax ;
	float ofomin = iofomin , dofo = idofo ;
	float dcdp = idcdp;
	float tpow = itpow; 
	float v0 = iv0;
	float aper = iaper;
	float apanl = iapanl;
	float apanr = iapanr;
	float dxdip  = idxdip;
	float dxtol = idxtol;

        char chost[MAXHOSTNAMELEN] ;
	int nxdip;

        gethostname(chost,MAXHOSTNAMELEN) ;
	fprintf(stderr,"starting migration at %s for\t%f < offsets <= %f \n",
			chost, ofomin-dofo*.5,ofomin+(nofo-.5)*dofo);
	
	tmp = nx * dx/dxdip;
	nxdip = (int)tmp;
	if(nxdip<1) nxdip = 1;
	if(nxdip>nx) nxdip = nx;
	dxdip = dx * nx/nxdip;
	 
	lwin = lagc/(int)(dt*1000.);

	/* in case the dp starts the migration again, do not remigrate */ 
	isize = 0;
	if((outfp = fopen(dataout,"r"))!=NULL) {
                 fseek(outfp,0L,SEEK_END);
                 isize= (int) ftell(outfp);
                 fclose(outfp);
        }
	if( isize==(nz*sizeof(float)+HDRBYTES)*nx*nofo ) {
	   fprintf(stderr,"output %s already migrated \n",dataout);
           return 0 ;
	}

	/* set mtrace to be -1 in case of error return to kirmig */
	*mtrace = -1;
	
        infp = fopen(datain,"r");
        if( infp == NULL ) {
	   fprintf(stderr,"kirmigs datain=%s not found\n",datain);
           return 1 ;
        }

        outfp = fopen(dataout,"w");
        if( outfp == NULL ) {
	   fprintf(stderr,"kirmigs dataout=%s not found\n",dataout);
           return 1 ;
        }

	/* check to see if there is any input data */
	efseek(infp,0L,2);
	notrace = 0;
        if((eftell(infp)-lseekin)<=0) {
		warn("no trace in input %s \n",datain); 
		notrace = 1;
		*mtrace = 0;
	}

	if (notrace==1) goto skipmig; 

        /* turn off buffering for outfp */
	/* setbuf(outfp,NULL); */

	/*
	fprintf(stderr,"within kirmigs mlimit=%d \n",mlimit);
	fprintf(stderr,"x0=%f z0=%f nx=%d nz=%d \n",x0,z0,nx,nz);
	fprintf(stderr,"dx=%f dz=%f \n",dx,dz);
	fprintf(stderr,"timefile=%s \n",timefile);
	fprintf(stderr,"ampfile=%s \n",ampfile);
	fprintf(stderr,"nt=%d dt=%f tmin=%f \n",nt,dt,tmin);
	fprintf(stderr,"ofomin=%f dofo=%f nofo=%d \n",ofomin,dofo,nofo);
	fprintf(stderr,"xmint=%f zmint=%f smint=%f \n",xmint,zmint,smint);
	fprintf(stderr,"dxt=%f dzt=%f dst=%f \n",dxt,dzt,dst);
	fprintf(stderr,"nxt=%d nzt=%d nst=%d \n",nxt,nzt,nst);
	fprintf(stderr,"lt=%d \n",lt);
	fprintf(stderr,"dcdp=%f dxtol=%f \n",dcdp,dxtol);
	fprintf(stderr,"lseekin=%d lseekout=%d \n",lseekin,lseekout);
	*/

	idip = 1;
	if(dipfile[0]=='N' && dipfile[1]=='U',
		dipfile[2]=='L' && dipfile[3]=='L') idip=0;

	/* memory allocations */
	lmem = (2*nzt*nxt + nofo*nx*nz + nofo + 4*nx*nz + nx + 2*nz 
		+ nzt + 2*lt + nt)*sizeof(float) 
	      +(nx + nz) * sizeof(int) + idip*2*nxdip*nz*sizeof(float);

	llimit = mlimit * 1024 * 1024;	
	if ( lmem > llimit ) {
	   fprintf(stderr,"Need at least memory size mlimit=%d (Bytes)\n",
	 	   lmem);
	   return 1;
	} 

        twk1 = (float*) malloc(nzt*nxt*sizeof(float));
	if( twk1 == 0 ) return 1 ;
        twk2 = (float*) malloc(nzt*nxt*sizeof(float));
	if( twk2 == 0 ) return 1 ;
	migs = (float*) malloc(nofo*nx*nz*sizeof(float));
	if( migs == 0 ) return 1 ;
	fold = (float*) malloc(nofo*sizeof(float));
	if( fold == 0 ) return 1 ;
	ts = (float*) malloc(nx*nz*sizeof(float));
	if( ts == 0 ) return 1 ;
	tr = (float*) malloc(nx*nz*sizeof(float));
	if( tr == 0 ) return 1 ;
	as = (float*) malloc(nx*nz*sizeof(float));
	if( as == 0 ) return 1 ;
	ar = (float*) malloc(nx*nz*sizeof(float));
	if( ar == 0 ) return 1 ;
	sigxt = (float*) malloc(nx*sizeof(float));
	if( sigxt == 0 ) return 1 ;
	sigzt = (float*) malloc(nz*sizeof(float));
	if( sigzt == 0 ) return 1 ;
	inxt = (int*) malloc(nx*sizeof(int));
	if( inxt == 0 ) return 1 ;
	inzt = (int*) malloc(nz*sizeof(int));
	if( inzt == 0 ) return 1 ;

	trt = (float*) malloc(nzt*sizeof(float));
	if( trt == 0 ) return 1 ;
	trz = (float*) malloc(nz*sizeof(float));
	if( trz == 0 ) return 1 ;
	trace = (float*) malloc(lt*sizeof(float));
	if( trace == 0 ) return 1 ;
        trwk = (float*) malloc(lt*sizeof(float));
	if( trwk == 0 ) return 1 ;
	trr = (float*) malloc(nt*sizeof(float));
	if( trr == 0 ) return 1 ;
	dips = (float*) malloc(idip*2*nxdip*nz*sizeof(float));
	if( dips == 0 ) return 1 ;


	lttbl = nst*nxt*nzt*sizeof(short);
	if ( lmem + lttbl > llimit ) {  
	   lttbl = 1 * sizeof(short);
	   tfile = (char*) malloc(strlen(timefile)+1);
	   sprintf(tfile,"%s\0",timefile);
	}
	else {
	   tfile = "null";
	}
	lmem +=lttbl;
	ttbl = (short*) malloc(lttbl);
	if( ttbl == 0 ) return 1 ;
        if ( iamp == 1 ) {
           latbl = nst*nxt*nzt*sizeof(short);
	   if ( lmem + latbl > llimit ) {  
	      latbl = 1 * sizeof(short);
	      afile = (char*) malloc(strlen(ampfile)+1);
	      sprintf(afile,"%s\0",ampfile);
	   }
	   else {
	      afile = "null";
	   }
           awk2 = (float*) malloc(nzt*nxt*sizeof(float));
	   lmem += nzt*nxt*sizeof(float); 
	   if( awk2 == 0 ) return 1 ;
           awk1 = (float*) malloc(nzt*sizeof(float));
	   lmem += nzt*sizeof(float); 
	   if( awk1 == 0 ) return 1 ;
        } else {
           latbl = 1 * sizeof(short);
	   afile = "null";
           awk2 = (float*) malloc(latbl);
	   lmem += latbl; 
	   if( awk2 == 0 ) return 1 ;
           awk1 = (float*) malloc(latbl);
	   lmem += latbl; 
	   if( awk1 == 0 ) return 1 ;
        }
	atbl = (short*) malloc(latbl);
	lmem += latbl; 
	if( atbl == 0 ) return 1 ;
	fprintf(stderr,"total memory used (Byte) =%d \n",lmem); 
	if(lttbl==1) fprintf(stderr,"travel time table disk i/o used \n"); 
	if(iamp==1 && latbl==1) 
	   fprintf(stderr,"amplitude table disk i/o used \n");

	/* read in travel time table and amplitude table */
	tfp = fopen(timefile,"r");
        if( tfp == NULL ) {
	   fprintf(stderr,"kirmigs: timefile=%s not found\n",timefile);
           return 1 ;
        }

	if ( iamp == 1 && latbl > 1 ) { 
	   afp = fopen(ampfile,"r");
           if( afp == NULL ) {
	       fprintf(stderr,"kirmigs: ampfile=%s not found\n",ampfile);
               return 1 ;
           }
	   /* find scale to scale amplitudes */
	   ascale = 0.;
           for(ix=0;ix<nxt*nst;ix++) {
              fread((char *)trt,sizeof(float),nzt,afp);
	      for(iz=0;iz<nzt;iz++) {
	         if(fabs(trt[iz]) > ascale) ascale=fabs(trt[iz]); 
	      }
	   }
	   /* read in amplitudes, scale and store in short *atbl */ 
	   fseek(afp,0,0);
	   if(ascale>0.) ascale = 32000./ascale;
           for(ix=0;ix<nxt*nst;ix++) {
              fread((char *)trt,sizeof(float),nzt,afp);
	      ix0 = ix*nzt;
	      for(iz=0;iz<nzt;iz++) {
	      tmp = trt[iz]*ascale; 
	      atbl[ix0+iz] = (short)tmp;
	      }
	   }
	}
	else {
	   ascale = 1.;
	}
	
	if(lttbl > 1 ) {
	   /* read in times, scale and store in short *ttbl */ 
           /* times in ms */
	   if (dt>=1.) {
              tscale = 32000./(nt*dt);
           }
           else {
              tscale = 1000. * 32000. / (nt*dt*1000.);
           }
           for(ix=0;ix<nxt*nst;ix++) {
              fread((char *)trt,sizeof(float),nzt,tfp);
	      ix0 = ix*nzt;
	      for(iz=0;iz<nzt;iz++) {
	         tmp = trt[iz]*tscale;
	         if ( tmp < 32000 ) {
	            ttbl[ix0+iz] = (short)tmp;
	         }
	         else {
	            ttbl[ix0+iz] = 32500;
	         }
	      }
	   }   

           if (dt>=1.) {
              tmin = tmin * 1000.;
              dl = dt * nt / lt;
	      dlm = dl;
           } else {
              tscale = tscale/1000.;
              dt = dt * 1000.;
              tmin = tmin * 1000.;
              dl = dt * nt / lt;
	      dlm = dl;
           }
	} else {
	   dl = dt*nt/lt;
	   dlm = dl * 1000.;
	   tscale = 1.;
	}
	
	

	/* Main loop over traces */
	ix = 0;
	dldt = dl/dt;
	tminl = tmin / dt; 
	ntrace =0;

/* skip lseekin bytes in infp */
	fseek(infp,lseekin,0);

/* see if cdp and offset are used in migration, instead of sx and gx */
/* read first trace */
	fgettr(infp,&tra);
	sy = tra.sy;
	gy = tra.gy;
	if ( sy == gy && dcdp==0. ) {
	   chkcdp = 0;
	} else {
	   chkcdp = 1;
	   if( dcdp == 0. ) {
		fprintf(stderr,"dcdp must be specified ! \n");
		return 1;
	   }
	   icdp1 = cdpx0;
	}
	
	/* read in dips grid */
	
	if(idip==1) {

		tmp = dcdpx*nx/nxdip;
		itmp = (int) tmp;	
		dipsgrid_cpp(dipfile,dips,dips+nz*nxdip,z0,dz,nz,
			cdpx0,itmp,nxdip);
		for(iz=0;iz<nxdip*nz*2;iz++) {
			tmp=dips[iz];
			if(tmp>=90.) {
		 		tmp = 89.9;
			} else if (tmp<=-90.) {
				tmp = -89.9;
			} 
			dips[iz]=tan(tmp*3.141592654/180.);
		}
	}

	/* initialize mtrace */
	*mtrace = 0;

	do {

                int nonzerotrace ;
		/* Load trace into trace (zero-padded) */
		memcpy(trr, tra.data, nt*sizeof(float));

	        /* check if this is a zero trace */
		nonzerotrace = 0 ;	
		for(it=0;it<nt;it++) {
		  if(trr[it] != 0.0) { nonzerotrace = 1; break ; } 
		} 
		if( nonzerotrace == 0 ) tra.trid = 0;
		if( tra.trid != 0 ) {

/* apply 2.5-D filter */
	            if(i2p5==1) f2p5_(trr,&nt);
/* apply agc */
		    if(lwin>0) {
                        mute = (tra.mute-tra.delrt)/tra.dt;
                        if(mute<0) mute=0;
                        if(mute>nt) mute=nt;
                        mt = nt - mute;
                        rmso = 2000.;
                        agc_(trr+mute,&mt,&lwin,trwk,&rmso);
                    }
/* apply tpow */
                    if(fabs(tpow)>0.0001) tp_(trr,&nt,&tpow,&tmin,&dt);


/* linearly interpolate input trace */
	            for(it=0;it<lt;it++) {
		       tmp = tminl+it*dldt; 
	               i = (int)tmp;
	               tmp = tmp - i;
	               if(i>=0 && i<nt-1) {
		          trace[it] = (1.-tmp)*trr[i]+tmp*trr[i+1];
		       }
		       else {
		          trace[it] = 0.;
		       }	
		    }
	

	            /* obtain source and receiver x's from trace hader */
		    if ( chkcdp == 0 ) {
		       sx = tra.sx;
		       gx = tra.gx;	
		       xs = sx;
		       xr = gx;
		       if(tra.scalco>1) {
				xs = xs*tra.scalco;
				xr = xr*tra.scalco;
		       } else if (tra.scalco<0) {
				xs = xs/(-tra.scalco);
				xr = xr/(-tra.scalco);
		       }		
		    }
		    else {
		       xs = (tra.cdp-icdp1)*dcdp + x0 - tra.offset/2.;
		       xr = (tra.cdp-icdp1)*dcdp + x0 + tra.offset/2.;

		    }
	            tmp = (tra.mute-tra.delrt)/dlm + 1.;
	            mute = (int)tmp ; 
		    tmp = tra.mute * 0.5 * v0 * 0.001;
                    tmp = tmp*tmp - 0.25 * fabs(xs-xr) * fabs(xs-xr);
                    if ( tmp > 0. ) {
                        zw = sqrt(tmp);
                    } else {
			zw = 0.;
		    }
		    ix = ix+ 1;

		    /* migration */
		    tmp = fabs(xs-xr);
		    offset = (tmp-ofomin)/dofo+1.5 ;   
		    iofo = (int)offset;
		    if ( (iofo<1 || iofo>nofo) && intype==1 ) break; 

/*
 fprintf(stderr,"xr=%f xs=%f at cdp=%d dcdp=%f icdp1=%d x0=%f offset=%d\n",
	xr,xs,tra.cdp,dcdp,icdp1,x0,tra.offset);	
*/

		}
		ntrace = ntrace + 1;
		if ( ntrace%1000 == 0 )
   	fprintf(stderr,"input %d traces processed at %s \n",ntrace,chost);

	        if(tmp>=ofimin && tmp<=ofimax && tra.trid==1 && mute<lt) {

		   	kirmig_(trace,&xs,&xr,&tmin,&lt,&dl,
                           	migs,&x0,&z0,&dx,&dz,&nx,&nz,
                           	&nofo,&ofomin,&dofo,fold,
                           	ttbl,atbl,&xmint,&zmint,&dxt,&dzt,&nxt,&nzt,
                           	&nst,&smint,&dst,ts,tr,as,ar,&dxtol,
                           	sigxt,sigzt,inxt,inzt,&iamp,mtrace,
                           	twk1,twk2,awk1,awk2,&tscale,trwk,
                           	&intps,&intpx,&intpz,&mute,
			   	&ascale,&amptype,
			   	tfile,afile,&aper,&apanl,&apanr,&zw,
				dips,&idip,&nxdip,&dxdip);
		}

	} while (fgettr(infp,&tra));


   fprintf(stderr,"input %d traces processed at %s \n",ntrace,chost);
	
        /* free spaces */
	free((char *)trt);
	free((char *)trr);
	free((char *)ttbl);
	free((char *)atbl);
	free((char *)twk1);
	free((char *)twk2);
	free((char *)awk1);
	free((char *)awk2);
	free((char *)trwk);
	free((char *)tr);
	free((char *)ts);
	free((char *)ar);
	free((char *)as);
	free((char *)sigxt);
	free((char *)sigzt);
	free((char *)inxt);
	free((char *)inzt);
	free((char *)trace);
	free((char*)dips);

	if(fabs(tpow)>0.0001) {
                trace = (float *) malloc(nz*sizeof(float));
                for(i=0;i<nz;i++) {
                        tmp = (z0+i*dz)/(z0+nz*0.5*dz);
                        tmp = fabs(tmp);
                        if(tmp==0.) {
                                trace[i] = 0.;
                        } else {
                                trace[i] = pow(tmp,-tpow);
                        }
                }
        }

	ascale=1./ascale;
	if ( amptype == 0 ) {
	   ascale = ascale/2.;
	}
	else {
	   ascale=ascale*ascale;
	}
	if ( iamp == 0 ) ascale = 1.;
	
	skipmig:

/* output traces */

	/* skip lseekout bytes in outfp */
	fseek(outfp,lseekout,0);
	
	/* if no input trace, zero output */
	if(notrace==1) {
		bzero((char*)&tra,(240+nz*sizeof(float)));
	}

	/* update trace headers */
	tra.ns = nz;
	tra.trid = 1;

	if ( dz < 30. ) {
	   tmp = dz * 1000.;
           tra.dt = (unsigned short) tmp;
	   tmp = z0 * 1000.;
	   tra.delrt = (unsigned short) tmp;
	} else if (dz <300.) {
	   tmp = dz * 100.;
           tra.dt = (unsigned short) tmp;
	   tmp = z0 * 100.;
	   tra.delrt = (unsigned short) tmp;
	} else {
           tra.dt = (unsigned short) dz;
	   tra.delrt = (unsigned short) z0;
	}
	tra.sy = 0;
	tra.gy = 0;
	/* scale x coordinate if needed */  
	itmp = (int) dx;
	tmp = dx - itmp;
	if(fabs(tmp)>0.01) {
		tra.scalco = -100;
	} else {
		tra.scalco = 1;
	} 

	for(iof=0;iof<nofo;iof++) {
	   offout = ofomin + iof*dofo;
	   if(notrace==0) {
	   	if(fold[iof]>1.) {
	      		tmp = ascale / fold[iof];
	   	}
	   	else {
	      		tmp = ascale;
	   	}
	   }
	   for(ix=0;ix<nx;ix++) {
/* update trace headers */
	      tra.offset = offout;
              tra.cdp = ix+1 ;
              tra.tracf = ix+1 ;
              xs = x0 + ix*dx - offout/2;  
              xr = x0 + ix*dx + offout/2;  
	      if (tra.scalco==-100) {
              	xs = xs * 100.;
              	xr = xr * 100.;
	      }
	      tra.sx = xs;
	      tra.gx = xr;
	      tra.ep = tra.sx;	
	      tra.fldr = tra.sx;	

	      if(notrace==0) {
	      	ix0 = ix*nz+iof*nx*nz;
	      	for (i=0;i<nz;i++) {
	         	trz[i] = migs[ix0+i]*tmp;
	      	}
	      	if(fabs(tpow)>0.0001)
                        for(i=0;i<nz;i++) trz[i] = trz[i]*trace[i];
	      	memcpy(tra.data, trz, nz*sizeof(float));
	      }
	      fputtr(outfp,&tra);
	      /* fflush(outfp); */
	   }
	} 

	if(notrace==0) {
		free((char *)migs);
		free((char *)fold);
		free((char *)trz);
		if(fabs(tpow)>0.0001) free((char *)trace);
	}

	fclose(infp);
	fclose(outfp);

   fprintf(stderr,
	"kirmig done at %s for\t%f < offsets <= %f  for %d live traces\n",
		  chost,ofomin-0.5*dofo,ofomin+(nofo-0.5)*dofo,*mtrace);

	return 0;
}
