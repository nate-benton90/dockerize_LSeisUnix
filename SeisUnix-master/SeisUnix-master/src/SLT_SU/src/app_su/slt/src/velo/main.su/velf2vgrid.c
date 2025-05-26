/* velocity interpolation from VELF card input */

#include "ghdr.h"
#include "gridhd.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"VELF2VGRID - convert wgc VELF cards to 2D velocity grid 	 	\n"
"\n"
"velf2vgrid [parameters] < velf-cards >vgrid 				\n" 
"\n"
"Required parameters:						 	\n"
"velf-cards=           Name of dataset containing VELF cards 		\n"
"fcdpvgrid=            First cdp number to output velocity grid 	\n"
"dcdpvgrid=            cdp number increment to output velocity grid	\n"
"ncdpvgrid=            Number of cdp locations to output velocity grid	\n"
"ntvgrid=              Number of time samples to output velocity grid 	\n"
"dtvgrid=              Time sampling interval (in ms) to output velocity grid\n"
"vgrid=                Name of velocity grid file 			\n"
"\n"
"Optional parameters:							\n"
"fx=0                  first x coordinate of the output velocity grid	\n"
"dx=12.5               x coordinate increment of the output velocity grid \n"
"vgtype=0              type of output velocity grid (0=rms; 1=avg; 2=int) \n"
"vmin=1400             minimum acceptable velocity output when vgtype>0 \n" 
"vmax=99999            maximum acceptable velocity output when vgtype>0 \n" 
"smcdp=1               Number of cdps to smooth grid before output  	\n"
"smtime=1              Number of time intervals to smooth grid before output\n"
"scale=1.0             Scale to be applied to the output velocity grid	\n"	
"transp=0              transpose velocity grid 				\n"
"                      (0=no   output ntvgrid by ncdpvgrid samples)	\n"
"                      (1=yes  output ncdpvgrid by ntvgrid samples)	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/19/92   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;

	int n1,n2,it,ntvgrid,ncdpvgrid,fcdpvgrid,dcdpvgrid;
	int smtime, smcdp, nps, i, icdp, i1, i2, incdp, nn, ncdpvelf;
	float dtvgrid, fcdp, res;
    	int *cdpsvelf, *npsvelf, *indx, *sortindex;
    	float *tpicks, *vpicks, *vg , *t, *cdps, *sorts, *cdpsort;
	float *vtx, *work, *smt, *smx, scale, *vgo;
	float *trs;
	float vmin, vmax;
	float fx,dx;
	int transp;
	int vgtype;

	ghed gh;
	int ierr;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	if (!getparint("fcdpvgrid",&fcdpvgrid)) 
		err(" fcdpvgrid missing ");
	if (!getparint("dcdpvgrid",&dcdpvgrid)) 
		err(" dcdpvgrid missing ");
	if (!getparint("ncdpvgrid",&ncdpvgrid)) 
		err(" ncdpvgrid missing ");
	if (!getparint("ntvgrid",&ntvgrid)) 
		err(" ntvgrid missing ");
	if (!getparfloat("dtvgrid",&dtvgrid)) 
		err(" dtvgrid missing ");
	if (!getparint("smcdp",&smcdp)) smcdp = 1;
	if (!getparint("smtime",&smtime)) smtime = 1;
	if (!getparfloat("scale",&scale)) scale = 1.0;
	if (!getparfloat("vmin",&vmin)) vmin = 1400.0;
	if (!getparfloat("vmax",&vmax)) vmax = 99999.0;
	if (!getparint("vgtype",&vgtype)) vgtype = 0;
	if (!getparfloat("fx",&fx)) fx = 0.0;
	if (!getparfloat("dx",&dx)) dx = 12.5;
	if (!getparint("transp",&transp)) transp = 0;

    	/* at most 128 cards with at most 256 time-vel pairs each */
    	n1 = 128;
    	n2 = 256;

    	/* arrays used to store all VELF card's cdp, time and velocity */
    	cdpsvelf = (int*)malloc(n2*sizeof(int));
    	npsvelf = (int*)malloc(n2*sizeof(int));
    	tpicks = (float*)malloc(n1*n2*sizeof(float));
    	vpicks = (float*)malloc(n1*n2*sizeof(float));
	
	bzero(npsvelf,n2*sizeof(int));
    	/* read in VELF cards */
    	velfread(infp, cdpsvelf, tpicks, vpicks, &ncdpvelf, npsvelf, n1, n2);

	fprintf(stderr," %d VELF cards read \n",ncdpvelf);
 
   	if (ncdpvelf==0) err("No VELF card input ! ");
	
	vg = (float*) malloc(ntvgrid*ncdpvelf*sizeof(float));
	vgo = (float*) malloc(ntvgrid*sizeof(float));
	indx = (int*) malloc(ntvgrid*sizeof(int));
	t = (float*) malloc(ntvgrid*sizeof(float));
	
    	/* compute time of output */
    	for(it=0;it<ntvgrid;it++) t[it] = it*dtvgrid;

    	/* compute NMO velocity at all time for VELF locations */
    	for(i=0;i<ncdpvelf;i++) {
		nps = npsvelf[i];
		fprintf(stderr,
			" %d t-v pairs read at cdp=%d \n",nps,cdpsvelf[i]);
		lin1d_(tpicks+i*n1,vpicks+i*n1,&nps,t,
			vg+i*ntvgrid,&ntvgrid,indx);

		if(vgtype>0) {
			vconvert(t,vg+i*ntvgrid,ntvgrid,0,0,
				 t,vgo,ntvgrid,vgtype,0);
			if(vgo[0] < vmin) {
				vg[i*ntvgrid] = vmin;
			} else if (vgo[0] > vmax) {
				vg[i*ntvgrid] = vmax;
			} else {
				vg[i*ntvgrid] = vgo[0];
			}
			for(it=1;it<ntvgrid;it++) {
				if(vgo[it]<vmin || vgo[it]>vmax) {
					vg[it+i*ntvgrid] = vg[it-1+i*ntvgrid];
				} else {
					vg[it+i*ntvgrid] = vgo[it];
				}
			} 

		} 
	}

	free(t);
	free(vgo);
	free(indx);
	free(vpicks);
        free(tpicks);
        free(npsvelf);
	
	sortindex = (int*) malloc(ncdpvelf*sizeof(int));
	cdps = (float*) malloc(ncdpvelf*sizeof(float));
	cdpsort = (float*) malloc(ncdpvelf*sizeof(float));
	sorts = (float*) malloc(ncdpvelf*ntvgrid*sizeof(float));

    	/* sort input velf in cdp-ascending order */
    	for(i=0;i<ncdpvelf;i++) {
		sortindex[i] = i;
		cdps[i] = cdpsvelf[i];
    	}
	free(cdpsvelf);
    	qkisort(ncdpvelf,cdps,sortindex);

    	for(i=0;i<ncdpvelf;i++) {
		cdpsort[i] = cdps[sortindex[i]];
		for(it=0;it<ntvgrid;it++) 
			sorts[it+i*ntvgrid] = vg[it+sortindex[i]*ntvgrid];
    	}

	free(vg);
	free(sortindex);
	free(cdps);

	vtx = (float*) malloc(ncdpvgrid*ntvgrid*sizeof(float));

    /* main loop over output cdps */
    	for(icdp=0;icdp<ncdpvgrid;icdp++) {
		fcdp = fcdpvgrid + icdp*dcdpvgrid;
		/* get velocity at output cdp location from velotb */
		if ( ncdpvelf < 2 ) {
	  		i1 = 0; i2=0; res=0.;
		} else {
        		/* search cdp index */
	 		nn = 1;
	   		bisear_(&ncdpvelf,&nn,cdpsort,&fcdp,&incdp);

	   		/* linear interpolation */
	   		if (incdp < 1 || fcdp < cdpsort[0] ) {
	      			i1 = 0; i2 = 0; res = 0.;
	  		} else if(incdp >= ncdpvelf) {
	   			i1 = ncdpvelf-1; i2 = ncdpvelf-1; res = 0.;
       			} else {
      				i1 = incdp-1; 
      				i2 = incdp; 
      				res =(fcdp-cdpsort[i1])
					/(cdpsort[i2]-cdpsort[i1]);
           		}
		}
		for(it=0;it<ntvgrid;it++) 
	  		vtx[it+icdp*ntvgrid] = 
					sorts[i1*ntvgrid+it] + 
			    		res*(sorts[i2*ntvgrid+it] -
						sorts[i1*ntvgrid+it]);
	}

    	free(sorts);
    	free(cdpsort);

    	if(smcdp>1 || smtime>1) {
    		work = (float*)malloc(ntvgrid*ncdpvgrid*sizeof(float));
    		smx = (float*)malloc(smcdp*sizeof(float));
    		smt = (float*)malloc(smtime*sizeof(float));
    	}
    	if(smtime>1 || smcdp >1) smth2d_(vtx,work,smt,smx,
					&ntvgrid,&ncdpvgrid,&smtime,&smcdp);

    	/* output velocity grid */
	if(scale!=1.0) {
		for(i1=0;i1<ntvgrid*ncdpvgrid;i1++) vtx[i1]=vtx[i1]*scale;
	}
	if(transp==0) {
    		fwrite(vtx,sizeof(float),ntvgrid*ncdpvgrid,outfp);
	} else {
		trs = (float*) malloc(ncdpvgrid*sizeof(float));
		for(i1=0;i1<ntvgrid;i1++) {
			for(i2=0;i2<ncdpvgrid;i2++) {
				trs[i2] = vtx[i2*ntvgrid+i1];
			}
    			fwrite(trs,sizeof(float),ncdpvgrid,outfp);
		}
		free(trs);
	}
	fflush(outfp);
	/* find out the maximum and the minimum velocities */
	fminmax(vtx,ntvgrid*ncdpvgrid,&vmin,&vmax);

	/* output header */
	bzero((char*)&gh,GHDRBYTES);
	gh.scale = 1.e-6;
	gh.dtype = 4. * gh.scale;
	if(transp==0) {
		gh.n1 = ntvgrid * gh.scale;
		gh.n2 = ncdpvgrid * gh.scale;
		gh.n3 = gh.scale;
		gh.d1 = dtvgrid * gh.scale;
		gh.d2 = dx * gh.scale;
		gh.d3 = gh.scale;
		gh.o1 = 0. * gh.scale;
		gh.o2 = fx * gh.scale;
		gh.o3 = 0. * gh.scale;
		gh.dcdp2 = dcdpvgrid * gh.scale;
		gh.ocdp2 = fcdpvgrid * gh.scale;
		gh.dline3 = gh.scale;
		gh.oline3 = 0.;
	} else {
		gh.n1 = ncdpvgrid * gh.scale;
		gh.n2 = gh.scale;
		gh.n3 = ntvgrid * gh.scale;
		gh.d1 = dx * gh.scale;
		gh.d2 = gh.scale;
		gh.d3 = dtvgrid * gh.scale;
		gh.o1 = fx * gh.scale;
		gh.o2 = 0.;
		gh.o3 = 0.;
		gh.dcdp2 = 0.;
		gh.ocdp2 = 0.;
		gh.dline3 = 0.;
		gh.oline3 = 0.;
	}
	gh.gmin = vmin * gh.scale;
	gh.gmax = vmax * gh.scale;
	ierr = fputghdr(outfp,&gh);
    	fclose(outfp);

	free(vtx);
    	if(smcdp>1 || smtime>1) {
		free(work);
		free(smx);
		free(smt);
	}

    	fprintf(stderr,"\n");
	if(vgtype==0) {
    	fprintf(stderr,
	" RMS velocity grid file created with following parameters: \n");
	} else if( vgtype==1 ) {
    	fprintf(stderr,
	" Average velocity grid file created with following parameters: \n");
	} else {
    	fprintf(stderr,
	" Interval velocity grid file created with following parameters: \n");
	}
    	fprintf(stderr,
	" fcdpvgrid=%d dcdpvgrid=%d ncdpvgrid=%d ntvgrid=%d dtvgrid=%f \n",
		fcdpvgrid, dcdpvgrid, ncdpvgrid, ntvgrid, dtvgrid);
	
	return 0;

}

