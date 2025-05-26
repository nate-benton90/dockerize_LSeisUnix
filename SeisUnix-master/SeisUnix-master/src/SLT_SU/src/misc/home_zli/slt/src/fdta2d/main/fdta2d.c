char *sdoc =
"fdta2d - Finite difference travel time and amplitude computation \n"
"\n"
"fdta2d vfile= tfile= nx= nz= [optional parameters]\n"
"\n"
"Required Parameters:\n"
"vfile=stdin            file containing velocities v(nz,nx)  (if not	\n"
"				specified, standard input is assumed ) \n"
"tfile=stdout           file containing times t(nzo,nxo,ns) (if not \n"
"				specified, standard output is assumed ) \n"
"nz=                    number of z samples (1st dimension)\n"
"nx=                    number of x samples (2nd dimension)\n"
"\n"
"Optional Parameters:\n"
"fx=0.0                 x coordinate of the upper left edge of model \n"
"fz=0.0                 z coordinate of the upper left edge of model \n"
"dx=1.0                 x sampling interval of model \n"
"dz=1.0                 z sampling interval of model \n"
"ns=1                   number of source locations to compute travel time \n"
"os=fx                  x coordinate of first source \n"
"ds=dx                  x coordinate increment of sources \n"
"fzs=fz                 z coordinate of first source \n"
"dzs=0.0                z coordinate increment of sources \n"
"fxo=fx                 first output x coordinate \n"
"fzo=fz                 first output z coordinate \n"
"dxo=dx                 output x interval \n"
"dzo=dz                 output z interval \n"
"nxo=nx                 number of output x samples \n"
"nzo=nz                 number of output z samples \n"
"ismz=1                 number of z samples used in vertically smoothing v \n"
"ismx=1                 number of x samples used in horizontally smoothing v \n"
"afile=null             file to contain amplitudes a(nzo,nxo,ns) \n"
"pfile=null             file to contain propagation angles p(nzo,nxo,ns) \n"
"kmheader=par.kirmig    header file for travel time and amplitude for KIRMIG \n"
"angmin=-75             minimum propagation angle allowed in amplitude table\n"
"angmax=75              maximum propagation angle allowed in amplitude table\n"
"amptype=0              type of amplitude computation			\n"
"                        0=simplified amplitude factor cos(angle)/sqrt(t) \n"
"                        1=Bleistein: WKBJ theory for common-offset mig\n"
"                        2=Vidale: amplitude by ray-tube theory)	\n"
"restart=n              job is restarted (=y yes; =n no)		\n"
"         \n"
"author:                Zhiming Li,        , 8/1/91 \n"
"\n";

#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "par.h"

void intnout(float *a1,int nz,int nx1,float dz,float x0,int nx,float dx,
	     int *ixo,int *izo,int nxo,int nzo,FILE *fp,
	     float *amin, float *amax); 

main(int argc, char **argv)
{
	int nx,nz,ns,nxo,nzo,*izo,*ixo,nr,na,amptype;
	float fx,fz,fxs,fzs,dx,dz,dxs,dzs,dxo,dzo,fxo,fzo,xsi,zsi;
	float *xs,*zs,ex,ez,*v, xo, zo, *smx, *smz;
	int ixs,izs;
	float *ss,*t1,*a1,*p1,tmp,x0,*work;
	int iamp,ismx,ismz;
	char *afile="", *vfile, *tfile;
	char *pfile="";
	char *restart; 
	string hfile;
	FILE *vfp, *tfp, *afp, *hfp, *pfp;
	int is,ix,iz;
	int nx1;
	float angmin, angmax;
	int is0, isize;
	float sx,sz,dxz,oxx,ozz;
	int isglob;

	float os0, smax0; 
	int ns0, ierr;

	float scale;
	int dtype, n1, n2, n3, n4, n5;
	float d1,d2,d3,d4,d5,o1,o2,o3,o4,o5,dcdp2,dline3,ocdp2,oline3;
	float gmin,gmax;
	float tgmin,tgmax,agmin,agmax,pgmin,pgmax;
	int itg=0, iag=0, ipg=0;
	ghed gh;

	
	/* hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(1);
	
	/* get required parameters */
	if (!getparint("nx",&nx)) err("must specify nx!\n");
	if (!getparint("nz",&nz)) err("must specify nz!\n");
	
	/* get optional parameters */
	if (!getparfloat("dx",&dx)) dx = 1.0;
	if (!getparfloat("fx",&fx)) fx = 0.0;
	if (!getparfloat("dz",&dz)) dz = 1.0;
	if (!getparfloat("fz",&fz)) fz = 0.0;
	if (!getparint("ns",&ns)) ns = 1;
	if (!getparfloat("os",&fxs)) fxs = fx;
	if (!getparfloat("ds",&dxs)) dxs = dx;
	if(dxs>20.*dx) 
		warn(" ds may be too large, especially for shallow data"); 
	if (!getparfloat("fzs",&fzs)) fzs = fz;
	if (!getparfloat("dzs",&dzs)) dzs = 0.;
	if (!getparint("nxo",&nxo)) nxo=nx;
	if (!getparint("nzo",&nzo)) nzo=nz;
	if (!getparfloat("fxo",&fxo)) fxo = fx;
	if (!getparfloat("fzo",&fzo)) fzo = fz;
	if (!getparfloat("dxo",&dxo)) dxo = dx;
	if (!getparfloat("dzo",&dzo)) dzo = dz;
	if (!getparint("ismx",&ismx)) ismx=1; 
	if (ismx<1) ismx=1;
	if (ismx>nx) ismx=nx;
	if (!getparint("ismz",&ismz)) ismz=1; 
	if (ismz<1) ismz=1;
	if (ismz>nz) ismz=nz;
	if (!getparstring("restart",&restart)) restart = "n"; 

	if( !getparstring("vfile",&vfile) ) {
		vfp = stdin;
	} else {
		vfp = fopen(vfile,"r"); 
	} 

	if (!getparfloat("os0",&os0)) os0 = fxs;
	if (!getparint("ns0",&ns0)) ns0 = ns;
	smax0 = os0 + (ns0-1)*dxs;


	/* obtain grid headers */
	ierr = fgetghdr(vfp,&gh);
	/* error checking */
	if(ierr==0) {
		getgval(&gh,"o1",&tmp);
		if(fabs(tmp-fz)>0.01) ierr=1;
		getgval(&gh,"o2",&tmp);
		if(fabs(tmp-fx)>0.01) ierr=1;
		getgval(&gh,"d1",&tmp);
		if(fabs(tmp-dz)>0.01) ierr=1;
		getgval(&gh,"d2",&tmp);
		if(fabs(tmp-dx)>0.01) ierr=1;
		getgval(&gh,"n1",&tmp);
		if((int)(0.5+tmp)!=nz) ierr=1;
		getgval(&gh,"n2",&tmp);
		if((int)(0.5+tmp)!=nx) ierr=1;
		if(ierr!=0) err(" check grid parameters of vfile ");  
	} else {
		warn(" input vfile non-standard, defaults used "); 
	}

	
	is0 = 0;
	isize = 0;
	if( !getparstring("tfile",&tfile) ) {
		tfp = stdout;
	} else {
		if((tfp = fopen(tfile,"r"))!=NULL) {
			fclose(tfp);
			tfp = fopen(tfile,"r+");	
		} else {
	   		tfp = fopen(tfile,"w");
		}
		if(restart[0]=='y') { 
			fseek(tfp,0,SEEK_END);
			isize = ftell(tfp)/sizeof(float)/nxo/nzo;
			is0 = (isize>0)?isize-1:0;
			/* if(isize==ns) return 0; */
		} else {
			fclose(tfp);
			tfp = fopen(tfile,"w");	
		}
		fseek(tfp,is0*nxo*nzo*sizeof(float),SEEK_SET);
	}

	getparstring("afile",&afile);
	getparstring("pfile",&pfile);
	if( !getparstring("kmheader",&hfile) ) hfile = "par.kirmig";

	if (!getparfloat("angmin",&angmin)) angmin = -75.;
	if (!getparfloat("angmax",&angmax)) angmax = 75.;
	angmin = angmin * 3.141592654 / 180.;
	angmax = angmax * 3.141592654 / 180.;
	if (!getparint("amptype",&amptype)) amptype=0; 

/* compute locations of sources */
	xs = (float*)malloc(ns*sizeof(float));
	zs = (float*)malloc(ns*sizeof(float));

	for (is=0;is<ns;is++) {
	   xs[is] = fxs + is*dxs;
	   zs[is] = fzs + is*dzs;
	}

	hfp = fopen(hfile,"w");
		
	/* ensure sources is in grid */
	ex = fx+(nx-1)*dx;
	ez = fz+(nz-1)*dz;

	if (fx>xs[0] || ex<xs[ns-1] || fz>zs[0] || ez<zs[ns-1]) 
		err("source lies outside of specified (x,z) grid\n");


/* computational grid will have the lateral spacing equal dz */
	x0 = 0.;
	tmp = (nx * dx + dz ) / dz ;
	nx1 = tmp;

/* slowness input */
	/* allocate space */
	v = (float*)malloc(nz*nx*sizeof(float));
	ss = (float*)malloc(nz*nx1*sizeof(float));

	/* read velocities */
	fseek(vfp,0,0);
	fread(v,sizeof(float),nx*nz,vfp);
	/* compute slowness scaled by dz (dz is the grid spacing to be used) */
	for(iz=0;iz<nz*nx;iz++) v[iz]=dz/v[iz];
	/* smooth slowness */
	if (ismx > 1 || ismz > 1 ) {
	   work = (float*)malloc(nz*nx*sizeof(float));
	   smx = (float*)malloc(ismx*sizeof(float));
           smz = (float*)malloc(ismz*sizeof(float));
	   smth2d_(v,work,smz,smx,&nz,&nx,&ismz,&ismx);
	   free(smx);
	   free(smz);
	   free(work);
	}
	/* slowness remap to computation grid */
	vlintp_ (v,&nz,&nx,&dx,&x0,ss,&nx1,&dz,&x0);
	free(v);

/* compute output z and x indice */
        izo = (int*) malloc(nzo*sizeof(int));
        ixo = (int*) malloc(nxo*sizeof(int));
	for (iz=0;iz<nzo;iz++) {
	   zo = fzo + iz*dzo;
	   if (zo<fz || zo>ez) 
		err("output lies outside of specified z grid\n");
	   zo = (zo - fz)/dz+0.5;
           izo[iz] = zo;
	} 
	for (ix=0;ix<nxo;ix++) {
	   xo = fxo + ix*dxo;
	   if (xo<fx || xo>ex) 
		err("output lies outside of specified x grid\n");
	   xo = (xo - fx)/dx+0.5;
           ixo[ix] = xo;
	} 
	   	
/* amplitude computation flag */	
	iamp = 0;
	if (afile[0]!='\0') {
		if((afp = fopen(afile,"r"))!=NULL) {
			fclose(afp);
			afp = fopen(afile,"r+");	
		} else {
	   		afp = fopen(afile,"w");
		}
	   	iamp = 1;
		if(restart[0]=='n') { 
			fclose(afp);
			afp = fopen(afile,"w");	
		}
	   	fseek(afp,is0*nxo*nzo*sizeof(float),SEEK_SET);
	}
	if (pfile[0]!='\0') {
		if((pfp = fopen(pfile,"r"))!=NULL) {
			fclose(pfp);
			pfp = fopen(pfile,"r+");	
		} else {
	   		pfp = fopen(pfile,"w");
		}
	   	iamp = 1;
		if(restart[0]=='n') { 
			fclose(pfp);
			pfp = fopen(pfile,"w");	
		}
	   	fseek(pfp,is0*nxo*nzo*sizeof(float),SEEK_SET);
		if(amptype==0) amptype = 3;
	}

	  

	/* allocate time array at computation grid */
	t1 = (float*)malloc(nz*nx1*sizeof(float));


	/* loop over sources */
        for (is=is0;is<ns;is++) {
	   xsi = (xs[is]-fx)/dz+0.5;
	   zsi = (zs[is]-fz)/dz+0.5;
	   ixs = xsi;
	   izs = zsi;

	   sx = ixs;
	   sz = izs;
	   dxz = 1.;
	   oxx = 0.;
	   ozz = 0.;

	   tmp = (xs[is]-os0)/dxs + 0.5;
	   isglob = tmp;

	   /* compute times */
	   time2d_ (ss,t1,&nz,&nx1,&izs,&ixs,&is); 
	   /* interpolation t back to dx and output*/
	   intnout(t1,nz,nx1,dz,x0,nx,dx,ixo,izo,nxo,nzo,tfp,&gmin,&gmax); 
	   if(itg==0) {
		tgmin = gmin;
		tgmax = gmax;
		itg = 1;
	   } else {
		if(tgmin>gmin) tgmin = gmin;
		if(tgmax<gmax) tgmax = gmax;
	   }

	   /* compute ampitudes and angles */
	   if ( iamp == 1 ) {
		/* amplitude array and angle array */
	   	a1 = (float*)malloc(nz*nx1*sizeof(float));
 	   	p1 = (float*)malloc(nz*nx1*sizeof(float));
	   	if(amptype==2) {
			work = (float*)malloc(nz*nx1*sizeof(float));
	   	} else {
		 	work = (float*)malloc(sizeof(float));
	   	}

	        amp2d_ (t1,a1,p1,&nz,&nx1,&angmin,&angmax,work,
			&amptype,&sx,&sz,&oxx,&ozz,&dxz,&dxz);

	        /* interpolate amplitude and output */
	        if (afile[0]!='\0') { 
	             	intnout(a1,nz,nx1,dz,x0,nx,dx,ixo,izo,nxo,nzo,afp,
				&gmin,&gmax); 
	             	if(iag==0) {
				agmin = gmin;
				agmax = gmax;
				iag = 1;
	   		} else {
				if(agmin>gmin) agmin = gmin;
				if(agmax<gmax) agmax = gmax;
			}
	   	}


	   	free(a1);
                free(work);


	        /* interpolate propagation angle and output */
	        if (pfile[0]!='\0') { 
	             	intnout(p1,nz,nx1,dz,x0,nx,dx,ixo,izo,nxo,nzo,pfp,
				&gmin,&gmax); 
	             	if(ipg==0) {
				pgmin = gmin;
				pgmax = gmax;
				ipg = 1;
	   		} else {
				if(pgmin>gmin) pgmin = gmin;
				if(pgmax<gmax) pgmax = gmax;
			}
	   	}
	   	free(p1);
	   }

   fprintf(stderr,"travel time computed at source=%d xs=%g \n",
		isglob+1,xs[is]);

	   if( fabs(smax0-xs[is]) < 0.1*dxs ) {
		scale = 1.e-6;
		dtype = 4;
		n1 = nzo;
		n2 = nxo;
		n3 = ns0;
		n4 = 1;
		n5 = 1;
		d1 = dzo;
		d2 = dxo;
		d3 = dxs;
		d4 = 0.;
		d5 = 0.;
		o1 = fzo; 
		o2 = fxo; 
		o3 = os0; 
		o4 = 0.;
		o5 = 0.;
		ocdp2 = 0.;
		oline3 = 0.;
		dcdp2 = 0.;
		dline3 = 0.;
		gmin = 0.;
		gmax = 0.;
		
		fflush(tfp);
		toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                	&dcdp2,&dline3,&ocdp2,&oline3,&tgmin,&tgmax);

		fwrite(&gh,sizeof(char),100,tfp);
		/*
		fputghdr(tfp,&gh);
		*/

		if (afile[0]!='\0') { 
			fflush(afp);
			putgval(&gh,"gmin",agmin);
			putgval(&gh,"gmax",agmax);

			/*
			fputghdr(afp,&gh);
			*/
			fwrite(&gh,sizeof(char),100,afp);
		}
		if (pfile[0]!='\0') {
			fflush(pfp);
			putgval(&gh,"gmin",pgmin);
			putgval(&gh,"gmax",pgmax);
			/*
			fputghdr(pfp,&gh);
			*/
			fwrite(&gh,sizeof(char),100,pfp);
		}

		/* output hfile */
		fprintf(hfp,"time/amplitude table parameter for KIRMIG \n"); 
		fprintf(hfp,"x0=%f z0=%f nx=%d nz=%d \n",fx,fz,nx,nz); 
		fprintf(hfp,"dx=%f dz=%f \n",dx,dz); 

		fprintf(hfp,"xmint=%f zmint=%f smint=%f \n",fxo,fzo,os0); 
		fprintf(hfp,"dxt=%f dzt=%f dst=%f \n",dxo,dzo,dxs); 
		fprintf(hfp,"nxt=%d nzt=%d nst=%d \n",nxo,nzo,ns0);
		if(amptype==3) {
			fprintf(hfp,"amptype=%d \n",0);
		} else {
			fprintf(hfp,"amptype=%d \n",amptype);
		}

	   }
	}
	
	fclose(hfp);
	fclose(tfp);
	if (afile[0]!='\0') fclose(afp);
	if (pfile[0]!='\0') fclose(pfp);
	/* free space */
        free(t1);
	free(ss);
	free(ixo);
	free(izo);
	
	return 0;
}

void intnout(float *a1,int nz,int nx1,float dz,float x0,int nx,float dx,
	     int *ixo,int *izo,int nxo,int nzo,FILE *fp,
		float *amin, float *amax) {
	float *a, *az;
	int ix, iz, ixx,izz;
	a = (float*) malloc(nx*nz*sizeof(float));
	az = (float*) malloc(nzo*sizeof(float));
	/* interpolation */
	vlintp_ (a1,&nz,&nx1,&dz,&x0,a,&nx,&dx,&x0);

	/* output */
	izz = izo[0];
	*amin = *amax = a[izz]; 
	for (ix=0;ix<nxo;ix++) {
		ixx = ixo[ix]*nz;
	       	for (iz=0;iz<nzo;iz++) {
	        	izz = izo[iz];
			az[iz] = a[ixx+izz];
			if(*amin>az[iz]) *amin = az[iz];
			if(*amax<az[iz]) *amax = az[iz];
	     	 }	 	
	         fwrite(az,sizeof(float),nzo,fp);
	}
	free(a);
	free(az);
}
