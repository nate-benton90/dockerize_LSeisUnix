/* GGRID compute gamma to evenly sampled grid points*/
#include "gridhd.h"
#include "comva.h"
#include "par.h"

char *sdoc = 
"GGRID - compute gamma values on an evenly sampled grid from GAMA card input \n"
"\n"
"ggrid [parameters] < GAMA.CARD >ggfile  				\n" 
"\n"
"Required parameters:							\n"
"nz=            number of depth points in the grid			\n"
"dz=            depth interval of the grid				\n"
"cdpmin=        minimum cdp number in the grid				\n"
"ncdp=          number of cdp's in the grid				\n"
"\n"
"Optional parameters:							\n"
"dcdp=1         cdp number increment in the grid			\n"
"fz=0           first depth of the grid					\n"
"ismz=1         number of depth samples used in smoothing output vertically \n"
"ismx=1         number of cdp points used in smoothing output horizontally \n"
"zg1=-dz        minimum depth where velocity does not change (gamma=1)	\n"
"\n"
"NOTE!!!: \n"
"  GAMA card format:							\n"
"  1---5----0----5----0----5----0----5----0----5----0----5----0----5----0 \n" 
"  GAMA       cdp1        z1   g1   z2   g2   z3   g3   z4   g4   z5   g5 \n" 
"  GAMA       cdp1        z6   g6   z7   g7   				  \n" 
"  GAMA       cdp2        z1   g1   z2   g2   				  \n" 
"  GAMA       cdpn        z1   g1   z2   g2   z3   g3   z4   g4   z5   g5 \n" 
" \n"
" where g1, g2, g3, ..., are actual gamma values multiplied by 10000     \n"
" gamma is defined as the ratio of V/Vmig, where V is the actual velocity \n"
" and Vmig is the migration velocity used in the previous iteration 	\n" 
"\n"
"\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	9/12/91   \n"		    
;

main(int argc, char **argv)
{
    char *cbuf, cdpread[5], depthread[5], gammaread[5]; 
    int ic, jc, icmax; 
    float *cdptable, *depth, *gamma, *gammatable, *z, *gzx, *work,*smx,*smz;
    int *indx;
    FILE *infp=stdin,*outfp=stdout;
    int npairs;
    int cdpnow, cdppre, cdpchange;
    int cdpmin, cdpmax, dcdp, ncdp;
    float fz, dz, fcdp, res;
    int iz, nz, i, icdp, i2, i1, i3;
    int ismx,ismz,nx;
    int incdp, nn;
    int n1,n2;
    float zg1;

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
    if (!getparint("dcdp",&dcdp)) dcdp = 1;
    cdpmax = cdpmin + (ncdp-1)*dcdp;

    if (!getparfloat("fz",&fz)) fz=0.;
    if (!getparint("ismz",&ismz)) ismz=1;
    if (!getparint("ismx",&ismx)) ismx=1;
    if (!getparfloat("zg1",&zg1)) zg1=-dz;
    nx = ncdp;
    ismz=(ismz/2)*2+1;
    ismx=(ismx/2)*2+1;
    if(ismz>nz) ismz=nz;
    if(ismx>nx) ismx=nx;
	

/* memory allocation */
    /* at most 100 cards with at most 100 depth-gamma pairs each */
    n1 = 100;
    n2 = 100;
    /* cdptable is used to store all gama card's cdp locations */
    cdptable = (float*)malloc(n2*sizeof(float));
    /* gammatable is used to store all gamma interpolated at read-in cdps */
    gammatable = (float*)malloc(nz*n2*sizeof(float));
    /* other arrays */
    gamma = (float*)malloc(n1*sizeof(float));
    depth = (float*)malloc(n1*sizeof(float));
    z = (float*)malloc(nz*sizeof(float));
    indx = (int*)malloc(nz*sizeof(int));
    cbuf = (char*)malloc(80*sizeof(char));
    gzx = (float*)malloc(nz*nx*sizeof(float));
    if(ismz>1 || ismx>1) {
    	work = (float*)malloc(nz*nx*sizeof(float));
    	smz = (float*)malloc(ismz*sizeof(float));
    	smx = (float*)malloc(ismx*sizeof(float));
    }

/* compute depth of output */
    for(iz=0;iz<nz;iz++) z[iz] = fz + iz*dz;

/* start to read GAMA cards */
    icmax = 10000;
    cdpnow  = 0;
    cdppre  = 0;
    cdpchange = 0;
    jc = 0;

    for (ic=0;ic<icmax;ic++) { 
       for(i=0;i<80;i++) cbuf[i]=' ';
       if (feof(infp) !=0 ) break;
       gets(cbuf);

       if(cbuf[0]=='G' && cbuf[1]=='A' && cbuf[2]=='M' && cbuf[3]=='A') {

	  cdpnow = 0;
	  strncpy(cdpread,&cbuf[10],5);
          cdpnow = atoi(cdpread);

	  if (cdppre == 0 ) cdppre = cdpnow ;
/* if cdp changes, build the gammatable */
	  if (cdpnow != cdppre && cdpnow !=0 && cdppre !=0 ) {
	     npairs = jc;
/*
	     fprintf(stderr,"npairs=%d  cdpchange=%d \n",npairs,cdpchange);
*/

	     /* linear interpolate gamma to desired depth */
    	     bisear_(&npairs,&nz,depth,z,indx);
    	     for(iz=0;iz<nz;iz++) {
		i1=indx[iz];
		if(i1<1 || z[iz] < depth[0]) {
			gammatable[iz+cdpchange*nz] = gamma[0];
		} else if ( i1 >= npairs ) {
			res = (z[iz] - depth[i1-1])/(depth[i1-1]-depth[i1-2]);
			gammatable[iz+cdpchange*nz] = gamma[i1-1]+res*
						(gamma[i1-1]-gamma[i1-2]);
		} else {
			res = (z[iz] - depth[i1-1])/(depth[i1]-depth[i1-1]);
			gammatable[iz+cdpchange*nz] = gamma[i1-1]+res*
						(gamma[i1]-gamma[i1-1]);
		} 
    	      }
/*
	     lin1d_(depth,gamma,&npairs,z,gammatable+cdpchange*nz,&nz,indx);
*/

/*
	     fprintf(stderr,"nz=%d \n",nz);
	     fprintf(stderr,"cdpchange=%d \n",cdpchange);
*/


	     cdptable[cdpchange] = cdppre ;

/*
	     fprintf(stderr,"cdptable=%f \n",cdptable[cdpchange]);
*/

	     cdpchange = cdpchange + 1;
	     jc = 0;
	     cdppre = cdpnow;
	  }
	  /* store read values in depth and gamma arrays */
	  for(i=0;i<5;i++) {

	     i2 = 0;
	     strncpy(depthread,&cbuf[20+i*10],5);
             i2 = atoi(depthread);
	     i3 = 0;
             strncpy(gammaread,&cbuf[25+i*10],5);
             i3 = atoi(gammaread);

	     if (i2==0 && i3==0) break; 

	     depth[jc] = i2;
	     gamma[jc] = i3*0.0001;
		/*
             fprintf(stderr,"cdp=%d depth=%f gamma=%f \n",
			cdpnow,depth[jc],gamma[jc]);
		*/
	     jc = jc + 1;
	  }
       }
    }
/* last input cdp location */
    cdptable[cdpchange] = cdpnow;
    npairs = jc;
    /* linear interpolate gamma to desired depth */
    bisear_(&npairs,&nz,depth,z,indx);
    for(iz=0;iz<nz;iz++) {
	i1=indx[iz];
	if(i1<1 || z[iz] < depth[0]) {
		gammatable[iz+cdpchange*nz] = gamma[0];
	} else if ( i1 >= npairs ) {
		res = (z[iz] - depth[i1-1])/(depth[i1-1]-depth[i1-2]);
		gammatable[iz+cdpchange*nz] = gamma[i1-1]+res*
					(gamma[i1-1]-gamma[i1-2]);
	} else {
		res = (z[iz] - depth[i1-1])/(depth[i1]-depth[i1-1]);
		gammatable[iz+cdpchange*nz] = gamma[i1-1]+res*
					(gamma[i1]-gamma[i1-1]);
	} 
    }
    cdpchange = cdpchange + 1;

    /* main loop over output traces */
    for(icdp=0;icdp<ncdp;icdp++) {
	/* get gamma at output cdp location from gammatable */
        fcdp = cdpmin+icdp*dcdp;
	if ( cdpchange < 2 ) {
	   i1 = 0; i2=0; res=0.;
	}
	else {
           /* search cdp index */
	   nn = 1;
	   bisear_(&cdpchange,&nn,cdptable,&fcdp,&incdp);
	   /* linear interpolation */
/*
	   fprintf(stderr,"incdp=%d \n",incdp);
*/
	   if (incdp < 1 || fcdp < cdptable[0] ) {
	      i1 = 0; i2 = 0; res = 0.;
	   } 
	   else if(incdp >= cdpchange) {
	      i1 = cdpchange-1; i2 = cdpchange-1; res = 0.;
           }	
	   else {
	      i1 = incdp-1; 
	      i2 = incdp; 
	      res =(fcdp-cdptable[i1])/(cdptable[i2]-cdptable[i1]);
           }
	}
	for(iz=0;iz<nz;iz++) { 
		if(z[iz]<=zg1) {
			gzx[iz+icdp*nz] = 1.0;
		} else { 
	   		gzx[iz+icdp*nz] = gammatable[i1*nz+iz] + 
			res*( gammatable[i2*nz+iz] - gammatable[i1*nz+iz] );
		}
	}
    }

    if(ismz>1 || ismx >1) smth2d_(gzx,work,smz,smx,&nz,&nx,&ismz,&ismx);

    fwrite(gzx,sizeof(float),nz*nx,outfp);
    /* write grid header */
    fflush(outfp);
    /* update grid header */ 
    bzero((char*)&gh,GHDRBYTES);

    /*
    gh.scale = 1.e-6; 
    gh.dtype = 4. * gh.scale;
    gh.n1 = (float)nz * gh.scale;
    gh.n2 = (float)ncdp * gh.scale;
    gh.n3 = gh.scale;
    gh.n4 = gh.scale;
    gh.n5 = gh.scale;
    gh.o1 = fz * gh.scale;
    gh.d1 = dz * gh.scale;
    gh.ocdp2 = cdpmin * gh.scale;
    gh.dcdp2 = dcdp * gh.scale;
    */
    putgval(&gh,"scale",1.e-6);
    putgval(&gh,"dtype",4.);
    putgval(&gh,"n1",(float)nz);
    putgval(&gh,"n2",(float)ncdp);
    putgval(&gh,"n3",1.);
    putgval(&gh,"n4",1.);
    putgval(&gh,"n5",1.);
    putgval(&gh,"o1",fz);
    putgval(&gh,"d1",dz);
    putgval(&gh,"ocdp2",(float)cdpmin);
    putgval(&gh,"dcdp2",(float)dcdp);
 
    fputghdr(outfp,&gh);
    fclose(outfp);
    free(gzx);
    if(ismz>1 || ismx >1) {
    	free(work);
    	free(smz);
    	free(smx);
    }
    free(gammatable);
    free(cdptable);
    return EXIT_SUCCESS;
}
