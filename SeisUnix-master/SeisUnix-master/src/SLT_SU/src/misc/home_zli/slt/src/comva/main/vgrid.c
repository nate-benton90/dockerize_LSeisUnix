/* VGRID compute velocity to evenly sampled grid points*/
#include "gridhd.h"
#include "comva.h"
#include "par.h"

char *sdoc = 
"VGRID - compute velocity on an evenly sampled grid from VELO card input \n"
"\n"
"vgrid [parameters] < VELO.CARD >vgfile  				\n" 
"\n"
"Required parameters:							\n"
"VELO.CARD      name of input file containing VELO and DVDZ cards       \n"
"nz=            number of depth (or time) points in the grid		\n"
"dz=            depth (or time) interval of the grid			\n"
"cdpmin=        minimum cdp number in the grid				\n"
"ncdp=          number of cdp's in the grid				\n"
"\n"
"Optional parameters:							\n"
"dcdp=1         cdp number increment in the grid			\n"
"dvdzgrid=NONE  output file of interval velocity gradient grid 		\n"
"               (if not specified, the file will not be created)	\n"
"ivcon=1        velocity conversion flag (1=input Va -> output Vi 	\n"
"					    or input Vi -> output Va) 	\n"
"					  0=input Va -> output Va 	\n"
"					    or input Vi -> output Vi) 	\n"
"invtyp=0	input velocity type (0=average; 1=interval)		\n"
"fz=0           first depth (or time) of the grid			\n"
"ismzv=1        number of depth samples used in smoothing output v \n"
"ismxv=1        number of cdp points used in smoothing output v  \n"
"ismzg=1        number of depth samples used in smoothing output v gradient \n"
"ismxg=1        number of cdp points used in smoothing output v gradient \n"
"vghdr=vg_hdr   separate header file name for velocity grid	\n" 
"dvdzghdr=dvdzg_hdr 					\n"
"               separate header file name for velocity gradient grid	\n" 
"\n"
"\n"
"NOTE!!!: \n"
" 1. VELO card format: (right-column adjusted)		\n"
"1---5---10---15----21----27----33----39----45----51----57----63----69----75 \n"
"VELO   cdp         z1    v1    z2    v2    z3    v3    z4    v4    z5    v5 \n"
" \n"
" where zi and vi, i=1,2,3,..., are depth (or time) velocity pairs.  \n"
"\n"
" 2. Interval Velocity Gradient DVDZ Card Format: (right-adjust) \n"
"1---5---10---15----21----27----33----39----45----51----57----63----69----75 \n"
"DVDZ   cdp        zt1   zb1 dvdz1   zt2   zb2 dvdz2   zt3   zb3 dvdz3   \n"
"\n"
"where          cdp indicates cdp number of current panel \n"
"               zti, i=1,2,..., are depth of top of gradient analysis zone \n"
"               zbi, i=1,2,..., are depth of bottom of gradient analysis zone\n"
"               dvdzi, i=1,2,..., are interval velocity gradient at [zti,zbi]\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	10/4/91   \n"		    
;

main(int argc, char **argv)
{
    int n1, n2, i1, i2;
    float *zpicks, *vpicks, *velotb, *vzx;
    float *ztops, *zbots, *dvdzs, *dvdztb, *dvdzzx;
    float *z, *work, *smx, *smz;
    float *cdpsort, *sorts, *cdps;
    int *sortindex, *cdpsvelo, *cdpsdvdz, *npsvelo, *npsdvdz;
    int ncdpvelo, ncdpdvdz;
    FILE *infp=stdin,*outfp=stdout,*tmpfp;
    int nps, ivcon, indx;
    int cdpmin, dcdp, ncdp, cdpmax;
    float fz, dz, fcdp, res;
    int iz, nz, i, icdp;
    int ismxv,ismzv,nx;
    int ismxg,ismzg,invtyp,otvtyp;
    int incdp, nn, idvdz, indvdz;
    char *dvdzgrid, *dvdzghdr="dvdzg_hdr", *vghdr="vg_hdr";
    char *buf;
    FILE *dvdzfp, *vhfp, *dvdzhfp;

    int n3=1,n4=1,n5=1;
    float o1=0.,o2=0.,o3=0.,o4=0.,o5=0.,d1=0.,d2=0.,d3=0.,d4=0.,d5=0.;
    float scale=1.e-6, ocdp2=0., oline3=0., dcdp2=0., dline3=0.;
    float gmin=0, gmax=0.;
    int dtype=4,ierr=0;
    int orient=0,gtype=0;
    ghed gh;

  

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

    if (!getparint("nz",&nz)) err("number of depth (nz) must be given \n");
    if (!getparfloat("dz",&dz)) err("depth interval (dz) must be given \n");
    if (!getparint("cdpmin",&cdpmin)) 
	err("minimum output cdp (cdpmin) must be given \n");
    if (!getparint("ncdp",&ncdp)) 
	err("number of output cdp (ncdp) must be given \n");
    if (!getparint("dcdp",&dcdp)) dcdp=1;
    cdpmax = cdpmin + (ncdp-1)*dcdp;
    if (!getparfloat("fz",&fz)) fz=0.;
    if (!getparint("ivcon",&ivcon)) ivcon=1;
    if (!getparint("ismzv",&ismzv)) ismzv=1;
    if (!getparint("ismxv",&ismxv)) ismxv=1;
    if (!getparint("ismzg",&ismzg)) ismzg=1;
    if (!getparint("ismxg",&ismxg)) ismxg=1;
    if (!getparint("invtyp",&invtyp)) invtyp=0;
    if (invtyp==0 && ivcon==1) {
	otvtyp = 1;
    } else if (invtyp==1 && ivcon==1) {
	otvtyp = 0;
    } else if (ivcon==0) {
	otvtyp = invtyp;
    }	 
    getparstring("vghdr",&vghdr);
    getparstring("dvdzghdr",&dvdzghdr);
    nx = ncdp;

    ismzg=(ismzg/2)*2+1;
    ismxg=(ismxg/2)*2+1;
    if(ismzg>nz) ismzg=nz;
    if(ismxg>nx) ismxg=nx;
    ismzv=(ismzv/2)*2+1;
    ismxv=(ismxv/2)*2+1;
    if(ismzv>nz) ismzv=nz;
    if(ismxv>nx) ismxv=nx;
	
/* memory allocation */
    /* at most 128 cards with at most 256 depth-vel pairs each */
    n1 = 128;
    n2 = 256;

    /* cdpsdvdz is used to store all DVDZ card's cdp locations */
    cdpsdvdz = (int*)malloc(n2*sizeof(int));
    npsdvdz = (int*)malloc(n2*sizeof(int));
    ztops = (float*)malloc(n1*n2*sizeof(float));
    zbots = (float*)malloc(n1*n2*sizeof(float));
    dvdzs = (float*)malloc(n1*n2*sizeof(float));

    z = (float*)malloc(nz*sizeof(float));

    dvdztb = (float*)malloc(nz*n2*sizeof(float));
    dvdzzx = (float*)malloc(nz*nx*sizeof(float));
  
    /* arrays for sorting input cards in cdp ascending order */
    sortindex = (int*)malloc(n2*sizeof(int));
    cdpsort = (float*)malloc(n2*sizeof(float));
    cdps = (float*)malloc(n2*sizeof(float));
    sorts = (float*)malloc(nz*n2*sizeof(float));

    if(ismzg>1 || ismxg>1) {
    	work = (float*)malloc(nz*nx*sizeof(float));
    	smz = (float*)malloc(ismzg*sizeof(float));
    	smx = (float*)malloc(ismxg*sizeof(float));
    }

    /* compute depth of output */
    for(iz=0;iz<nz;iz++) z[iz] = fz + iz*dz;

    /* read in DVDZ and VELO cards to a temp file */
    tmpfp = etempfile(NULL);
    buf = (char*) malloc(81*sizeof(char));
    do {
	bzero(buf,81);
	fgets(buf, 81, infp); 
	buf[80] = '\n';
	fwrite(buf, 1, 81, tmpfp);
    } while(!feof(infp));
    free(buf);


    /* read in DVDZ cards */
    dvdzread(tmpfp, cdpsdvdz, ztops, zbots, dvdzs, &ncdpdvdz, npsdvdz, n1, n2);
    /*
    for(i=0;i<ncdpdvdz;i++) {
	for(iz=0;iz<npsdvdz[i];iz++)
		fprintf(stderr,"zt=%6.0f zb=%6.0f dvdz=%6.3f cdp=%d \n",
		ztops[iz+i*n1],zbots[iz+i*n1],dvdzs[iz+i*n1],cdpsdvdz[i]);
    }
    */

    /* compute gradient at z for all DVDZ locations */
    for(i=0;i<ncdpdvdz;i++) {
	indx = i * n1;		
	nps = npsdvdz[i];
    	dvdzint(ztops+indx,zbots+indx,dvdzs+indx,nps,z,dvdztb+i*nz,nz);
    }

    /* sort input dvdz in cdp-ascending order */
    for(i=0;i<ncdpdvdz;i++) {
		sortindex[i] = i;
		cdps[i] = cdpsdvdz[i];
    }
    if(ncdpdvdz>0) qkisort(ncdpdvdz,cdps,sortindex);
    for(i=0;i<ncdpdvdz;i++) {
	cdpsort[i] = cdps[sortindex[i]];
	for(iz=0;iz<nz;iz++) sorts[iz+i*nz] = dvdztb[iz+sortindex[i]*nz];
    }
    for(i=0;i<ncdpdvdz;i++) {
	cdps[i] = cdpsort[i];
	for(iz=0;iz<nz;iz++) dvdztb[iz+i*nz] = sorts[iz+i*nz];
    }

    /* interpolate dvdz to every cdp location */
    if(ncdpdvdz==0) {
	bzero(dvdzzx,nz*nx*sizeof(float));
	indvdz = 0;
    } else {
	indvdz = 1;
	/* main loop over output cdp positions */
        for(icdp=0;icdp<ncdp;icdp++) {
        /* get dvdz at output cdp location from dvdztb */
        	fcdp = cdpmin+icdp*dcdp;
        	if ( ncdpdvdz < 2 ) {
         		i1 = 0; i2=0; res=0.;
        	} else {
           	/* search cdp index */
           		nn = 1;
           		bisear_(&ncdpdvdz,&nn,cdps,&fcdp,&incdp);
           	/* linear interpolation */
           		if (incdp < 1 || fcdp < cdps[0] ) {
              			i1 = 0; i2 = 0; res = 0.;
           		} else if(incdp >= ncdpdvdz) {
              			i1 = ncdpdvdz-1; i2 = ncdpdvdz-1; res = 0.;
           		} else {
              			i1 = incdp-1;
              			i2 = incdp;
              			res =(fcdp-cdps[i1])/(cdps[i2]-cdps[i1]);
           		}
        	}
        	for(iz=0;iz<nz;iz++)
           		dvdzzx[iz+icdp*nz] = dvdztb[i1*nz+iz] +
                        	res*(dvdztb[i2*nz+iz]-dvdztb[i1*nz+iz]);

    	}
    }

    /* smoothing if needed */
    if(ismzg>1 || ismxg >1) {
	smth2d_(dvdzzx,work,smz,smx,&nz,&nx,&ismzg,&ismxg);
    	free(smz);
    	free(smx);
    	free(work);
    }
    free(dvdztb);
    free(dvdzs);
    free(ztops);
    free(zbots);
    free(npsdvdz);
    free(cdpsdvdz);

    if(ismzv>1 || ismxv>1) {
    	work = (float*)malloc(nz*nx*sizeof(float));
    	smz = (float*)malloc(ismzv*sizeof(float));
    	smx = (float*)malloc(ismxv*sizeof(float));
    }
 	
    /* cdpsvelo is used to store all VELO card's cdp locations */
    cdpsvelo = (int*)malloc(n2*sizeof(int));
    npsvelo = (int*)malloc(n2*sizeof(int));
    zpicks = (float*)malloc(n1*n2*sizeof(float));
    vpicks = (float*)malloc(n1*n2*sizeof(float));

    velotb = (float*)malloc(nz*n2*sizeof(float));
    vzx = (float*)malloc(nz*nx*sizeof(float));

    /* read in VELO cards */
    veloread(tmpfp, cdpsvelo, zpicks, vpicks, &ncdpvelo, npsvelo, n1, n2);
    /*
    for(i=0;i<ncdpvelo;i++) {
	for(iz=0;iz<npsvelo[i];iz++)
		fprintf(stderr,"z=%6.0f v=%6.0f cdp=%d \n",
			zpicks[iz+i*n1],vpicks[iz+i*n1],cdpsvelo[i]);
    }
    */
    
    if (ncdpvelo==0) err("No VELO card input ! ");

    /* compute interval velocity at z for all VELO locations */
    for(i=0;i<ncdpvelo;i++) {
 	indx = i * n1;		
	nps = npsvelo[i];
	idvdz = (cdpsvelo[i]-cdpmin)/dcdp;
	if(idvdz<0) {
		idvdz = 0;
	} else if ( idvdz>ncdp-1) {
		idvdz = ncdp-1;
	}
        vconint(zpicks+indx,vpicks+indx,nps,z,velotb+i*nz,nz,
		invtyp,otvtyp,dvdzzx+idvdz*nz); 
    }

    /* sort input velo in cdp-ascending order */
    for(i=0;i<ncdpvelo;i++) {
		sortindex[i] = i;
		cdps[i] = cdpsvelo[i];
    }
    qkisort(ncdpvelo,cdps,sortindex);
    for(i=0;i<ncdpvelo;i++) {
	cdpsort[i] = cdps[sortindex[i]];
	for(iz=0;iz<nz;iz++) sorts[iz+i*nz] = velotb[iz+sortindex[i]*nz];
    }
    for(i=0;i<ncdpvelo;i++) {
	cdps[i] = cdpsort[i];
	for(iz=0;iz<nz;iz++) velotb[iz+i*nz] = sorts[iz+i*nz];
    }

    /* free sort arrays */
    free(sorts);
    free(sortindex);
    free(cdpsort);
    free(cdpsvelo);
    
    /* main loop over output traces */
    for(icdp=0;icdp<ncdp;icdp++) {
		/* get velocity at output cdp location from velotb */
        	fcdp = cdpmin+icdp*dcdp;
		if ( ncdpvelo < 2 ) {
	   		i1 = 0; i2=0; res=0.;
		} else {
           	/* search cdp index */
	   		nn = 1;
	   		bisear_(&ncdpvelo,&nn,cdps,&fcdp,&incdp);
	   	/* linear interpolation */
	   		if (incdp < 1 || fcdp < cdps[0] ) {
	      			i1 = 0; i2 = 0; res = 0.;
	   		} else if(incdp >= ncdpvelo) {
	      			i1 = ncdpvelo-1; i2 = ncdpvelo-1; res = 0.;
           		} else {
	      			i1 = incdp-1; 
	      			i2 = incdp; 
	      			res =(fcdp-cdps[i1])/(cdps[i2]-cdps[i1]);
           		}
		}
/*
	fprintf(stderr,"fcdp=%f incdp=%d i1=%d i2=%d \n",fcdp,incdp,i1,i2);
*/
		for(iz=0;iz<nz;iz++) 
	   		vzx[iz+icdp*nz] = velotb[i1*nz+iz] + 
			    	res*(velotb[i2*nz+iz]-velotb[i1*nz+iz]);
    }
    free(velotb);

    if(ismzv>1 || ismxv >1) smth2d_(vzx,work,smz,smx,&nz,&nx,&ismzv,&ismxv);

    /* prepare grid header */
    bzero( (char*)&gh,GHDRBYTES);
    o1 = fz;
    ocdp2 = cdpmin;
    d1 = dz;
    dcdp2 = dcdp;
    n1 = nz;
    n2 = ncdp;
    toghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
		&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                &dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,&orient,&gtype);

    /* output dvdz grid and header if needed */
    if(indvdz==1 && getparstring("dvdzgrid",&dvdzgrid)){
	dvdzfp = fopen(dvdzgrid,"w");
    	fwrite(dvdzzx,sizeof(float),nz*nx,dvdzfp);
	/* find max and min grid values */
	fminmax(dvdzzx,nz*nx,&gmin,&gmax);
	/* update header */
	putgval(&gh,"gmin",gmin);
	putgval(&gh,"gmax",gmax);
        /* add header to dvdzgrid */
        fflush(dvdzfp);        
	fputghdr(dvdzfp,&gh);
    	fclose(dvdzfp);
	dvdzhfp = fopen(dvdzghdr,"w");
	fprintf(dvdzhfp,"header file for dvdz grid \n");
	fprintf(dvdzhfp,"fz=%f dz=%f nz=%d\n",fz,dz,nz);
	fprintf(dvdzhfp,"fcdp=%d dcdp=%d ncdp=%d\n",cdpmin,dcdp,ncdp);
	fprintf(dvdzhfp,"gmin=%g gmax=%g\n",gmin,gmax);
	fclose(dvdzhfp);
    }
    free(dvdzzx);

    /* output velocity grid and header */
    fwrite(vzx,sizeof(float),nz*nx,outfp);
    /* add header to dvdzgrid */
    fflush(outfp);        
    /* find max and min grid values */
    fminmax(vzx,nz*nx,&gmin,&gmax);
    /* update header */
    putgval(&gh,"gmin",gmin);
    putgval(&gh,"gmax",gmax);
    fputghdr(outfp,&gh); 
    fclose(outfp);
    vhfp = fopen(vghdr,"w");
    fprintf(vhfp,"header file for velocity grid \n");
    fprintf(vhfp,"fz=%f dz=%f nz=%d\n",fz,dz,nz);
    fprintf(vhfp,"fcdp=%d dcdp=%d ncdp=%d\n",cdpmin,dcdp,ncdp);
    fprintf(vhfp,"gmin=%g gmax=%g\n",gmin,gmax);
    fclose(vhfp);

    free(vzx);
    if(ismzv>1 || ismxv >1) {
    	free(work);
    	free(smz);
    	free(smx);
    }
    free(vzx);
    free(cdps);

    return EXIT_SUCCESS;
}

