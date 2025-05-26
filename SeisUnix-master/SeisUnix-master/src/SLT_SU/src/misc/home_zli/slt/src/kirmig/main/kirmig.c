/* KIRMIG: $Revision: 1.1 $ ; $Date: 91/04/29 $	*/


#include <unistd.h>
#include <XSYS/subs.h> 
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "su.h"
#include "segy.h"
#include "header.h"  


/*********************** self documentation **********************/
char *sdoc[] = {
"                                                               ",
" KIRMIG - Kirchhoff migration 					",
" 								",
" kirmig datain= dataout= [parameters]				",
" 							        ",
" Required parameters:						",
" datain=         input data name             			",
" dataout=        output data name             			",
" x0=             horizontal (x) coordinate at left edge of output zone",
" z0=             vertical (z) coordinate at top edge of output zone ",
" nx=             number x positions in migration output zone      ",
" nz=             number z positions in migration output zone      ",
" dx=             x interval in migration output zone              ",
" dz=             z interval in migration output zone              ",
" timefile=       travel time table file                    	",
" Optional parameters: 						",
" ampfile=        amplitude table file (if not given, uniform amplitude) ",
" nt=all          number of samples per trace to migrate    	",
" dt=default      sampling interval                         	",
" tmin=default    minimum time of input data                	",
" ofimin=0        minimum input offset to migrate         	",
" ofimax=999999   maximum input offset to migrate         	",
" ofomin=0        minimum output offset of migration       	",
" dofo=999999     output offset increment of migration       	",
" nofo=1          number of output offsets of migration       	",
" xmint=x0        minimum x coordinate of time/amplitude tables ",
" zmint=z0        minimum z coordinate of time/amplitude tables ",
" smint=x0        minimum source (s) coordinate of time/amplitude tables ",
" dxt=dx          x coordinate interval of time/amplitude tables ",
" dzt=dz          z coordinate interval of time/amplitude tables ",
" dst=dx          s coordinate interval of time/amplitude tables ",
" nxt=nx          number of x coordinates of time/amplitude tables ",
" nzt=nz          number of z coordinates of time/amplitude tables ",
" nst=nx          number of s coordinates of time/amplitude tables ",
" lt=4*nt         number of samples/trace to interpolate for migration ",
" i2p5=1          2.5-d compensation mode; 1=yes; 0=no ",
" nm=1            number of sub-processes to complete the job ",
" intype=0        input data type (1=common offsets with ONLY positive ",
"                 offsets and sorted in ascending order; 0=other types)  ",
" dcdp=0.         input cdp interval. When dcdp=0. and sy=gy=0, sx and gx ",
"                 used to locate source and receiver x positions (straight ",
"                 line). When dcdp not 0, cdp and offset used to ",
"                 compute source and receiver x positions (crooked line). ", 
"                 Specify non-zero dcdp to use cdp and offset to compute "
"                 source and receiver positions. fcdpout at x0 position "
"                 must be the same as that in the input data, when dcdp "
"                 is not zero.						" 
" sort=0          0=nm datasets not created. The job will first sort ",
"                   input dataset into nm small datasets. After the job ",
"                   is completed, these nm datasets will be deleted.	",
"                 1=nm datasets have been created in previous run. This ",
"                   is either a restart of the job or a new run. After  ",
"                   job is completed, these nm datasets will be deleted.",
"                 2=Same as 0, except that these nm datasets will NOT be ",
"                   deleted after the completion of the job.		",
"                   (This option should be used in the first iteration  ",
"                   of this iterative depth migration) These nm datasets",
"                   can then be used again for subsequent iterations. 	",
"                 3=Same as 1, except that these nm datasets will NOT be",
"                   deleted after the completion of the job.		",
" amptype=0       amplitude computation type; (see fdta2d for details)  ",
" mlimit=48       memory limit in megabytes to run the program		",
" fcdpout=1       cdp number of first output trace 			",
" dcdpout=1       cdp number increment of output traces 		",
" lagc=0          agc window length in ms (=0, input will not be agc) 	",
"                 gained before migration). Useful for noisy data input.",
" tpow=0.         apply t**tpow to data before migration. (for agc data ",
"                 data input, tpow should be -2.0; when lagc is not 0, 	",
"                 tpow will be default to -2.0)                         ",
" aper=nx*dx/4    lateral aperature of migration. (input trace will be  ",
"                 mapped only to the region: Xmid-aper <= x <= Xmid+aper,",
"                 where Xmid is the midpoint position of input trace)	",
" apanl=-60.      aperature angle to the left (al)              	",
" apanr=60.       aperature angle to the right (ar)             	",
"                 input trace will be mapped only to region shown below:",
"                      --------------------------               	",
"                               .|.                             	",
"                              . | .                            	",
"                             .  |  .                           	",
"                            .   |   .                          	",
"                           . al | ar .                         	",
" v0=1500.        surface velocity (or water velocity for marine data) 	",
" datains=datain  name prefix used by nm split input datasets   	",
"                 (the job will split input dataset 'datain' into 	",
"                 nm small datasets called 'datainsI', I=1,2,...nm) 	",
"                 It allows user to store nm small datasets on another 	",
"                 file system, when the file system where the input 	",
"                 dataset resides is full. 				",
" dipfile=NULL    dip limit (DIPS card) file (=NULL --- no dip limit)	",
" dxdip=50*dx     lateral spacing to interpolate DIPS input		", 
" dxtol=1.0       tolerance of lateral distance to recompute travel time",
"Notes:									",
" 1. Total memory requirment (mlimit) is:				",
"     memory (in Bytes) = [2*nzt*nxt+nx*nz*nofo/nm+4*nx*nz]*4 		",
"	                + nst*nzt*nxt*itable*2				",
"     where itable=2 if both time and ampltitude tables are used	",
"                  1 if no amplitude table is specified or		",
"                    disk i/o used for amplitude table			",
"                  0 if disk i/o used for 				",
"                    time and amplitude tables (NOT RECOMMENDED)	",  
"    The program will use memory to store time and amplitude		",
"    tables, if mlimit is larger than the above computation.		",
"    It will try use disk i/o for amplitude table, if mlimit		",
"    is not big enough. Disk i/o for time table will also be used	",
"    if mlimit is still not big enough. When disk i/o are used		",
"    for both time and amplitude tables (itable=0), and yet the 	",
"    mlimit is still not sufficient, the job will be cancelled.		",
" 2. Parameter nm can reduce the memory requirement, in the 		",
"    expense of more total CPU time. 					",
" 3. Use sort=1, or sort=3 only when the input dataset had been 	",
"    sorted into nm datasets and nm remains the same as that used	",
"    in the previous run. The names of the nm datasets are:		",
"      datainsI, I=1,2,...,nm (datains is the name specified by 	",
"    parameter datains).						", 
"    A typical use of this parameter sort would be:			",
"          sort=2	for the 1st iteration of kirmig			",
"          sort=3	for all other iterations except last 		",
"          sort=1       for the last iteration of kirmig		",
"    AGAIN ===> nm MUST BE THE SAME FOR ALL ITERATIONS!!!		",
" 4. Total disk space requirement for output is:			",
"	(nz*4+240)*nx*nofo + 3600	(Bytes)				",
" 5. Total work disk space requirement for sorting nm different 	",
"    datasets is: (nz*4+240)*nx*nofo 	(Bytes)				",
"    and these nm datasets are in the same directory as datains.	",
" 6. Check memory and disk spaces before running the job.		",
" 7. The number of machines to run the job is controlled by user's 	",
"    .machines file in the directory where the job is submitted. If 	",
"    this .machines file does not exit, it will use system's database.	",
" 8. DIPS format:							",
"1---5---10---15----21----27----33----39----45----51----57----63----69 	",
"DIPS   cdp        zt1  dipl  dipr   zt2  dipl  dipr   zt3  dipl  dipr 	",
"    where zt is depth, dipl is left dip limit, dipr is right dip limit.",
" Author:  Zhiming Li		5/5/1991                        	",
NULL};

/**************** end self doc ***********************************/


segytrace tra;


main(int argc, char **argv)
{
	int nt, ntmax, nx, nz;
	int nzt, nxt, nst;
	float smint, xmint, zmint, dst, dxt, dzt;
	float x0, z0, dx, dz, dt, tmin;
	float ofimin, ofomin, dofo, ofimax, ofomax;
	float ofomini, dcdp;
	int nofo, lt, ntrace, i2p5;
	int nofoi,amptype;
	string ampfile, timefile;
	int iamp, *mtrace;
	FILE *infp, *outfp, *testfp;
	string datain, dataout, datains;
	char **dataini, **dataouti;
	char *dipfile="NULL";
	int im, nm, kofo, lseekout;
	int iofo, ix, ltrace;
	int intps,intpx,intpz,isize;
	int iofpm, intype, *lseekini; 
	int sort, mlimit;
	int dcdpout, fcdpout, tracl;
	int lagc;
        float tpow, aper, apan, apanl, apanr, v0, dxdip, dxtol;
	int lmem, idip, nxdip, llimit;
	float tmp;


        segychdr sgycrd ;
        segybhdr sgybin ;
	ghed gh; 
	float fhread;
	int ihread;
	int ierr=1;
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

/* copy tape header and binary header from datain to dataout */
	if (!getparstring("datain", &datain)) err("must specify datain"); 
	if (!getparstring("dataout", &dataout)) err("must specify dataout"); 
	if (!getparstring("datains", &datains)) datains=datain;

	if((infp = fopen(datain,"r"))==NULL)
		err("Input dataset %s not found \n",datain);

	outfp = efopen(dataout,"w");
       
        fgethdr(infp,&sgycrd,&sgybin) ;
	if(strncmp((char*)&sgycrd, "C 1 CLIENT",10)!=0) 
		err(" Input Not IEEE SEGY");

	/* Get info from first trace */ 
	if (!fgettr(infp,&tra))  err("can't get first trace");
	ntmax = tra.ns;

	fclose(infp);

/* parameters fetching */
	if (!getparint("nt", &nt)) nt = ntmax; 
	if ( nt > ntmax ) nt = ntmax;
	if (!getparfloat("dt", &dt)) dt = tra.dt/1000000.0;
	   if (!dt) err("invalid sampling interval");
	if (!getparfloat("tmin", &tmin)) tmin = tra.delrt/1000.0;

	if (!getparfloat("x0", &x0)) err("x0 missing from parameter list ");
	if (!getparfloat("dx", &dx)) dx = 0.0;
           if(dx <= 0.) err("x sampling interval invalid", dx);
	if (!getparint("nx", &nx))	nx = 0;
           if(nx <= 0) err("number of output x positions invalid", nx);
	if (!getparfloat("z0", &z0)) err("z0 missing from parameter list ");
	if (!getparfloat("dz", &dz))	dz = 0.0;
           if(dz <= 0.) err("z sampling interval invalid", dz);
	if (!getparint("nz", &nz))	nz = 0;
           if(nz <= 0) err("number of output z positions invalid", nz);

	if (!getparfloat("ofimin", &ofimin)) ofimin = 0.;   
	if (!getparfloat("ofimax", &ofimax)) ofimax = 999999.;   

	if (!getparfloat("ofomin", &ofomin)) ofomin = 0.;   
	if (!getparfloat("dofo", &dofo)) dofo = 999999.;   
	if (!getparint("nofo", &nofo)) nofo = 1;   
	ofomax = ofomin + (nofo-1)*dofo;

/* get parameters for travel time table and amplitude table */
	if (!getparstring("timefile", &timefile)) err("must specify timefile"); 

	/*check to see if timefile exist */
	if((testfp = fopen(timefile,"r"))==NULL)
		err("Input timefile %s not found \n",timefile);

	/* get header of time file */ 
	ierr = fgetghdr(testfp,&gh);
	if(ierr!=0) warn(" warning: non-standard timefile input \n");
	fclose(testfp);

	/* get dipfile */
	idip = 0;
	if(getparstring("dipfile", &dipfile)) {
		idip = 1;
		if((testfp = fopen(dipfile,"r"))==NULL)
			err("Input dipfile %s not found \n",dipfile);
		fclose(testfp);
	}

	if(!getparfloat("dxdip", &dxdip)) dxdip = 50. * dx;

	/*check to see if ampfile exist */
	iamp = 1;
	if (!getparstring("ampfile", &ampfile)) iamp=0;   
	if(iamp==1) {
		if((testfp = fopen(ampfile,"r"))==NULL)
			err("Input ampfile %s not found \n",ampfile);
		fclose(testfp);
	}

	/* check the following nine parameters */
	if (!getparfloat("zmint", &zmint)) {
		if(ierr==0) {
			getgval(&gh,"o1",&zmint);
		} else {
			zmint = z0;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"o1",&fhread);
			if(fabs(fhread-zmint)>0.01) 
				err(" check zmint & timefile ");	
		}
	}
	if (!getparfloat("xmint", &xmint)) {
		if(ierr==0) {
			getgval(&gh,"o2",&xmint);
		} else {
			xmint = x0;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"o2",&fhread);
			if(fabs(fhread-xmint)>0.01) 
				err(" check xmint & timefile ");	
		}
	}
	if (!getparfloat("smint", &smint)) {
		if(ierr==0) {
			getgval(&gh,"o3",&smint);
		} else {
			smint = x0;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"o3",&fhread);
			if(fabs(fhread-smint)>0.01)
				 err(" check smint & timefile ");	
		}
	}
	if (!getparfloat("dzt", &dzt)) {
		if(ierr==0) {
			getgval(&gh,"d1",&dzt);
		} else {
			dzt = dz;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"d1",&fhread);
			if(fabs(fhread-dzt)>0.01) 
				err(" check dzt & timefile");	
		}
	}
	if (!getparfloat("dxt", &dxt)) {
		if(ierr==0) {
			getgval(&gh,"d2",&dxt);
		} else {
			dxt = dx;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"d2",&fhread);
			if(fabs(fhread-dxt)>0.01)
				 err(" check dxt & timefile");	
		}
	}
	if (!getparfloat("dst", &dst)) {
		if(ierr==0) {
			getgval(&gh,"d3",&dst);
		} else {
			dst = dx;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"d3",&fhread);
			if(fabs(fhread-dst)>0.01) 
				err(" check dst & timefile");	
		}
	}
	if (!getparint("nzt", &nzt)) {
		if(ierr==0) {
			getgval(&gh,"n1",&fhread);
			fhread = fhread + 0.5;
			nzt = (int) fhread;
		} else {
			nzt = nz;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"n1",&fhread);
			fhread = fhread + 0.5;
			ihread = (int) fhread;
			if(ihread!=nzt) err(" check nzt & timefile ");	
		}
	}
	if (!getparint("nxt", &nxt)) {
		if(ierr==0) {
			getgval(&gh,"n2",&fhread);
			fhread = fhread + 0.5;
			nxt = (int) fhread;
		} else {
			nxt = nx;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"n2",&fhread);
			fhread = fhread + 0.5;
			ihread = (int) fhread;
			if(ihread!=nxt) err(" check nxt & timefile ");	
		}
	}
	if (!getparint("nst", &nst)) {
		if(ierr==0) {
			getgval(&gh,"n3",&fhread);
			fhread = fhread + 0.5;
			nst = (int) fhread;
		} else {
			nst = nx;
		}
	} else {
		if(ierr==0) {
			getgval(&gh,"n3",&fhread);
			fhread = fhread + 0.5;
			ihread = (int)fhread;
			if(ihread!=nst) err(" check nst & timefile ");	
		}
	}

	if (smint>x0) warn("=====> smint > x0 <=====\n");
        if (zmint>z0) warn("=====> zmint > z0 <=====\n");
        if (xmint>x0) warn("=====> xmint > x0 <=====\n");
        if ((smint+(nxt-1)*dxt)<(x0+(nx-1)*dx))
         warn ("=====> (smint+(nxt-1)*dxt) < (x0+(nx-1)*dx) <=====\n");
        if ((xmint+(nxt-1)*dxt)<(x0+(nx-1)*dx))
         warn ("=====> (xmint+(nxt-1)*dxt) < (x0+(nx-1)*dx) <=====\n");
        if ((zmint+(nzt-1)*dzt)<(z0+(nz-1)*dz))
         warn ("=====> (zmint+(nzt-1)*dzt) < (z0+(nz-1)*dz) <=====\n");

	if (!getparint("nm", &nm)) nm = 1;   
	if (!getparint("i2p5", &i2p5)) i2p5 = 1;   
	if (!getparint("intype", &intype)) intype = 0 ;   
	if (!getparfloat("dcdp", &dcdp)) dcdp = 0. ;   
	if (!getparint("sort", &sort)) sort = 0 ;   
	if (!getparint("amptype", &amptype)) amptype = 0 ;   
	if (!getparint("mlimit", &mlimit)) mlimit = 48 ;   
	if (!getparint("fcdpout", &fcdpout)) fcdpout = 1 ;   
	if (!getparint("dcdpout", &dcdpout)) dcdpout = 1 ;   
	if (!getparint("lagc", &lagc)) lagc = 0 ;
        if (!getparfloat("tpow", &tpow)) {
                if(lagc==0) {
                        tpow = 0. ;
                } else {
                        tpow = -2. ;
                }
        }
        if (!getparfloat("aper", &aper)) aper=nx*dx/4.;
        if (!getparfloat("apan", &apan)) apan=60.;
	if (!getparfloat("apanl", &apanl)) apanl=-apan;
        if (!getparfloat("apanr", &apanr)) apanr=apan;

	if (apanl<-89.9) apanl = -89.9;
        if (apanl>89.9) apanl = 89.9;
        if (apanl>-0.1 && apanl <0.1) {
		if(apanl>=0) {
			apanl = 0.1;
		} else {
			apanl = -0.1;
		}
	}
	if (apanr<-89.9) apanr = -89.9;
        if (apanr>89.9) apanr = 89.9;
        if (apanr>-0.1 && apanr <0.1) {
		if(apanr>=0) {
			apanr = 0.1;
		} else {
			apanr = -0.1;
		}
	}

        if (!getparfloat("v0", &v0)) v0=1500.;
        if (!getparfloat("dxtol", &dxtol)) dxtol=1.;
	if(dxtol<0.) dxtol = - dxtol;


/* update binary header */
        sgybin.hns = nz ;
        sgybin.ntrpr = nx ;
        sgybin.hdt = (short) ( dz < 1. ? dz*1000. : dz );
        sgybin.fold = nofo;
        sgybin.tsort = 3;
	
	
        fputhdr(outfp,&sgycrd,&sgybin) ;

	/* set length of migration trace to lt to intepolate input traces */
	if (!getparint("lt", &lt)) lt = 4*nt;   
	if (lt<nt) lt=nt;
        lt = lt/nt*nt;
	
/* number of offsets per subprocess to migrate */
	iofpm = (nofo+nm-1)/nm;

  	intpx = 1;
        if(x0==xmint && dx==dxt && nx<=nxt) intpx=0;
        intpz = 1;
        if(z0==zmint && dz==dzt && nz==nzt) intpz=0;
        intps = 1;
        if(x0==smint && dx==dst && nx<=nst) intps=0;
	
	fprintf(stderr,"x0=%f z0=%f nx=%d nz=%d \n",x0,z0,nx,nz);
        fprintf(stderr,"dx=%f dz=%f \n",dx,dz);
        fprintf(stderr,"timefile=%s \n",timefile);
        fprintf(stderr,"ampfile=%s \n",ampfile);
        fprintf(stderr,"nt=%d dt=%f tmin=%f \n",nt,dt,tmin);
        fprintf(stderr,"ofomin=%f dofo=%f nofo=%d \n",ofomin,dofo,nofo);
        fprintf(stderr,"xmint=%f zmint=%f smint=%f \n",xmint,zmint,smint);
        fprintf(stderr,"dxt=%f dzt=%f dst=%f \n",dxt,dzt,dst);
        fprintf(stderr,"nxt=%d nzt=%d nst=%d \n",nxt,nzt,nst);
        fprintf(stderr,"lt=%d i2p5=%d \n",lt,i2p5);
	fprintf(stderr,"intps=%d intpx=%d intpz=%d\n",intps,intpx,intpz);


	tmp = nx * dx / dxdip;
	nxdip = (int)tmp;
	/* compute memory requirements */
	lmem = (2*nzt*nxt + iofpm*(nx*nz+1) + 4*nx*nz + nx + 2*nz
                + nzt + 2*lt + nt)*sizeof(float)
              +(nx + nz) * sizeof(int) + idip*2*nxdip*nz*sizeof(float);
	lmem = lmem + nzt*nxt*nst*sizeof(short) +
		iamp * (nzt*nxt*nst*sizeof(short)+nzt*(nxt+1)*sizeof(float));

        fprintf(stderr," \n");
        fprintf(stderr," KIRMIG needs memory size=%d \n",lmem);
        fprintf(stderr," \n");
	
        llimit = mlimit * 1024 * 1024;
        if ( lmem > llimit ) {
	   fprintf(stderr,"travel time table disk i/o used \n");
	   if(iamp==1) fprintf(stderr,"amplitude table disk i/o used \n");
	   lmem = lmem - nzt*nxt*nst*sizeof(short) -
		iamp * (nzt*nxt*nst*sizeof(short)+nzt*(nxt+1)*sizeof(float));
           if ( lmem > llimit ) {
           	err("Need at least memory size mlimit=%d (Bytes)\n",lmem);
	   }
        }



	mtrace = (int *)malloc(nm*sizeof(int));
	lseekini = (int *)malloc(nm*sizeof(int));
	dataini = (char**) malloc(nm*sizeof(char *));
	dataouti = (char**) malloc(nm*sizeof(char *));

       	for(im=0;im<nm;im++) {
              dataouti[im] = (char*) malloc(strlen(dataout)+10);
              sprintf(dataouti[im],"%s%d",dataout,im+1) ;
        }

/* test to see if output offsets were created in the previous run */
        for(im=0;im<nm;im++) {
              nofoi = iofpm;
              kofo = im * iofpm + 1;
              if (kofo+nofoi-1 > nofo ) nofoi = nofo - kofo + 1;
              isize=0;
              if((infp = fopen(dataouti[im],"r"))!=NULL) {
                 fseek(infp,0L,SEEK_END);
                 isize= (int) ftell(infp);
                 fclose(infp);
              }
              if( isize!=(nz*sizeof(float)+HDRBYTES)*nx*nofoi ) {
                        isize = 0;
                        break;
              }
        }
        if(isize!=0) goto no_migration;


/* sort data into common offsets for each machine*/
	if ( nm > 1 && intype == 0 ) {
	   FILE ** fdataini  ;

	   fdataini = (FILE**) malloc(sizeof(FILE *)*nm);
	   for(kofo=1;kofo<=nm;kofo++) {
	      dataini[kofo-1] = (char*) malloc(strlen(datains)+10);
	      sprintf(dataini[kofo-1],"%s%d",datains,kofo) ;
	      if(sort==0 || sort==2) 
			fdataini[kofo-1] = efopen(dataini[kofo-1],"w") ;
           }

	   if ( sort == 0 || sort==2 ) {
	      infp = efopen(datain,"r");
	      fseek(infp,EBCBYTES+BNYBYTES,SEEK_SET);
	      while( fgettr(infp,&tra) ) {
	         float offset = tra.offset;
                 /* offset = tra.gx - tra.sx; */
                 iofo =  (int) ( ( fabs(offset) - ofomin ) /dofo  + 1.5 );
	         im = (iofo+iofpm-1)/iofpm;
	
	         if(iofo>0 && iofo<=nofo) fputtr(fdataini[im-1],&tra);
	      }
	      fclose(infp);
	   }
	   for(im=0; im<nm ; im++) {
	      if(sort==0 || sort==2) fclose(fdataini[im]) ;
              lseekini[im] = 0 ;
           }
	} 
	else if(nm>1 && intype == 1 ) {

	/* find out the average number of traces per machine to mig */
	   int ntrpm, trlen,jm;
	   infp = efopen(datain,"r");
	   fseek(infp,0L,SEEK_END);
	   trlen = ntmax*sizeof(float)+HDRBYTES; 
	   ntrpm = ((int)ftell(infp)-(EBCBYTES+BNYBYTES) )/trlen/nm; 
           ntrpm -= 1 ; /* to be "in average" before the offset to point to */

	/* find out where to seek */
	   lseekini[0] = EBCBYTES+BNYBYTES;
	   for( im=0; im<nm; im++ ) {
               int before = (im==0) ? 1 : 0 ;
	       fseek(infp,lseekini[im],SEEK_SET);
	       while( fgettr(infp,&tra) ) {
	          float offset = tra.offset;
		  if (offset<0.) 
			err("Negative offsets not allowed for intype=1 \n");
		  /* offset = tra.gx - tra.sx; */
                  iofo = (int)( ( fabs(offset) - ofomin ) /dofo + .5 ) ;
	          jm = iofo/iofpm;
                  if( jm < im ) {
                     /* smaller offset: increment the seek position */
                     before = 1 ;
                     lseekini[im] += trlen ;
                  } else if( jm == im && before ) {
                     /* initialize next machine seek position */
                     if( im < nm-1 ) lseekini[im+1] = lseekini[im]+trlen*ntrpm;
                     break ;
                  } else {
                     /* bigger or equal offset: I have to go back of 2 traces */
                     lseekini[im] -= trlen ;
	             fseek(infp,-2*trlen,SEEK_CUR);
                  } 
               }
               dataini[im] = datain ;
	   }
	   fclose(infp);
	} 
	else {
           dataini[0] = datain ;
           lseekini[0] = EBCBYTES+BNYBYTES ;
        }

        kofo = 0;

        dispatcher_attributes(argv[0],0) ;
	init_dispatch();
	
	for(im=0; im<nm ; im++) {
	      kofo = im * iofpm + 1;
              ofomini = ofomin + (kofo-1)*dofo;
              mtrace[im] = 0 ;
	      nofoi = iofpm;
	      if (kofo+nofoi-1 > nofo ) nofoi = nofo - kofo + 1; 
/* temporary dataset */
	      dataouti[im] = (char*) malloc(strlen(dataout)+10);
              sprintf(dataouti[im],"%s%d",dataout,im+1) ;
	      lseekout=0;
	      isize=0;
	      if((infp = fopen(dataouti[im],"r"))!=NULL) {
	         fseek(infp,0L,SEEK_END);
		 isize= (int) ftell(infp);
		 fclose(infp);
	      }

	      if ( nofoi<= 0 ) continue;	

	      if( isize!=(nz*sizeof(float)+HDRBYTES)*nx*nofoi ) {
                 
	         fprintf(stderr,"start dispatch im=%d \n",im+1);

	         dispatch("kirmigs",dataini[im],dataouti[im],
                                    lseekini[im],lseekout,
                                    timefile,ampfile,iamp,
                                    nt,nx,nz,lt,
                                    tmin,x0,z0,
                                    dt,dx,dz,
                                    smint,xmint,zmint,
                                    dst,dxt,dzt,
                                    nst,nxt,nzt,
                                    ofimin,ofimax,
                                    ofomini,dofo,nofoi,
                                    mtrace+im,i2p5,
				    intps,intpx,intpz,intype,dcdp,
				    amptype,mlimit,lagc,tpow,aper,apanl,apanr,
				    v0,fcdpout,dcdpout,dipfile,dxdip,dxtol);
	      }

	}
	wait_dispatched();

	no_migration:

   	fprintf(stderr,"Starting merge of %d offset datasets \n",nm);

	ntrace = 0;
        for(im=0; im<nm; im++ ) {
	    if(mtrace[im]==0) {
		warn(" zero trace migrated at im=%d \n",mtrace[im]);
	    } else if(mtrace[im]<0) {
		err(" error occured at im=%d",mtrace[im]);
	    }
	    ntrace = ntrace + mtrace[im];
        }
/* remove temporary input common-offset data sets */

       if( nm > 1 && intype == 0 && sort<=1 ) {
  	   for(kofo=0;kofo<nm;kofo++) {
            	unlink(dataini[kofo]); 
            /*  eremove(dataini[kofo]); */
           }
        }

	/* if( ntrace == 0 ) return 1 ; */

/* copy data from temporary datasets to output */
	fseek(outfp,EBCBYTES+BNYBYTES,SEEK_SET);

	tracl = 1;
	for(im=0;im<nm;im++) {
	      infp = efopen(dataouti[im],"r");
	      ltrace = nz*sizeof(float) + HDRBYTES;
	      nofoi = iofpm;
	      kofo = im * iofpm + 1;
	      if (kofo+nofoi-1 > nofo ) nofoi = nofo - kofo + 1; 
	      for(iofo=0;iofo<nofoi;iofo++) {
	         for(ix=0;ix<nx;ix++) {
		    fread((char *)&tra,sizeof(char),ltrace,infp);
		    tra.cdp = fcdpout + ix * dcdpout;
		    tra.cdpt = iofo + 1;
		    tra.mute = 0;
		    tra.muts = 0;
		    tra.mutb = 0;
		    tra.dz = dz;
		    tra.fz = z0;
		    tra.tracl = tracl;
		    fwrite((char *)&tra,sizeof(char),ltrace,outfp);
		    tracl = tracl + 1;
	         }
	      }
	      fclose(infp);
	      eremove(dataouti[im]);
	}
	      fclose(outfp);

   fprintf(stderr,"KIRMIG COMPLETED for total number of traces =%d \n",ntrace);
	
        return 0;
}
