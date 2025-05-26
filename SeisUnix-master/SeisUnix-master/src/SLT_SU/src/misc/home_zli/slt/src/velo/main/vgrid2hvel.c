/* velocity grid to HANDVEL card conversion */

#include "velo.h"
#include "ghdr.h"
#include "gridhd.h"
#include "par.h"


char *sdoc = 
"VGRID2HVEL - convert velocity grid to DISCO HANDVEL cards  		\n"
"\n"
"vgrid2hvel [parameters] < vgrid  >hvel-cards 				\n" 
"\n"
"Required parameters:						 	\n"
"vgrid=                Name of velocity grid 				\n"
"hvel-cards=           Name of dataset to output HANDVEL cards 		\n"
"\n"
"Optional parameters:							\n"
"fcdpvgrid=from_header   First trace number of velocity grid 		\n"
"dcdpvgrid=from_header   Trace number increment of velocity grid	\n"
"ncdpvgrid=from_header   Number of trace per line of velocity grid	\n"
"flinevgrid=from_header  First line number of velocity grid 		\n"
"dlinevgrid=from_header  line number increment of velocity grid		\n"
"nlinevgrid=from_header  Number of cdp per line of velocity grid	\n"
"ftvgrid=from_header     First Time/depth (in ms/m or ft) of velocity grid\n"
"dtvgrid=from_header     Time/depth sampling interval (in ms/m or ft)	\n"
"                         of velocity grid \n"
"ntvgrid=from_header     Number of time/depth samples of velocity grid 	\n"
"cdp1=1                  first cdp Number of 3D master grid	\n"
"trace1=1                first trace (crossline) number of 3D master grid\n"
"line1=1                 first line number of 3D master grid	\n"
"dline=1                 line number increment of 3D master grid	\n"
"ncdpline=ncdpvgrid      Number of cdp per line in 3D master grid	\n"
"                        (cdp number starts with cdp1 and increases by 1  \n"
"                        in 3D master grid)  \n"
"fcdphvel=fcdpvgrid      First trace number to output HANDVEL cards	\n"
"dcdphvel=dcdpvgrid      Trace number increment to output HANDVEL cards	\n"
"ncdphvel=1              Number of traces per line to output HANDVEL cards\n"
"flinehvel=flinevgrid    First line number to output HANDVEL cards	\n"
"dlinehvel=dlinevgrid    Line number increment to output HANDVEL cards	\n"
"nlinehvel=1             Number of lines to output HANDVEL cards\n"
"fthvel=ftvgrid          First time/depth (in ms/m or ft) to output HANDVEL\n" 
"dthvel=10*dtvgrid       Time/depth interval (in ms/m or ft) to output \n"
"                         HANDVEL\n" 
"nthvel=ntvgrid/10       Number of times/depths to output HVEL		\n"
"template=               template name of desired output (t,cdp) locations \n"
"                        in hanvel cards format.                        \n"
"                        (When this parameter is supplied and when number \n"
"                        of cdps in template is more than 1, output hanvel \n"
"                        will be the same (t,cdp) as those defined in the \n"
"                        template, (fcdphvel, dcdphvel, ncdphvel, 	\n"
"                        flinehvel, dlinehvel, nlinehvel, 		\n"
"                        fthvel, dthvel, nthvel) will be ignored.    	\n"
"                        When this parameter is supplied and when number \n"
"                        of cdps in the template is 1, output hanvel \n"
"                        will be the same (t) as those defined in the \n"
"                        (fthvel, dthvel) will be ignored.)    	\n"
"                        nthvel will be the maximum number of t-v pairs to \n"
"                        output per cdp. It defaults to 4096.		\n"
"nvfmax=4096             maximum number of velocity functions in template \n"
"ntvmax=256              maximum number of t-v pairs per velocity functions  \n"
"                        in template   \n"
"ivtype=0                Input velocity grid type (0=rms; 1=avg; 2=int)	\n" 
"ovtype=0                Output HANDVEL velocity type (0=rms; 1=avg; 2=int)\n" 
"ittype=0                Input velocity grid time/depth type 	\n"
"                        (0=time 1=depth)\n" 
"ottype=0                Output HANDVEL time/depth type (0=time 1=depth	\n" 
"hcdptype=0              Output HANDVEL cdp type (0=cdp 1=cdplbl)		\n"	
"\n"
"Notes:									\n"
" 1. This program can be used to make velocity contour plot using Disco	\n"
"    plotting program.							\n" 
" 2. When input velocity grid is a nonstandard grid file, i.e., without	\n"
"    header, 9 grid parameters of input velocity grid must be supplied.	\n"
"\n"
"AUTHOR:         J. Cignoli, Zhiming Li,       ,	25-aug-1993   	\n";

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;

	int ncdpvgrid,ntvgrid,ncdphvel,nthvel;
	float fcdpvgrid,dcdpvgrid,ftvgrid,dtvgrid,tmp;
	float fcdphvel,dcdphvel,fthvel,dthvel;
	float flinevgrid, dlinevgrid;
	float flinehvel, dlinehvel;
	int nlinevgrid,cdp1,line1,dline,ncdpline, nlinehvel;
	int ivtype,ovtype;
	int ittype,ottype;
	int trace1, hcdptype;

    	float *tin, *tout, *vin, *vout;
	float o2, o3; 
	int ocdp;
	int it, i2vgrid, i3vgrid; 
	int i2, i3;

	ghed gh;
	int ierr;

	char *template;
	FILE *tmpfp;
	int *cdp, *nps, ncdp=0, icdp, nvfmax, ntvmax;
	float *ts, *vs;
	int io2, io3;
	

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	ierr = fgetghdr(infp,&gh);
	if (!getparfloat("fcdpvgrid",&fcdpvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"ocdp2",&fcdpvgrid);
		} else {
			err(" fcdpvgrid missing ");
		}  
	}
	if (!getparfloat("dcdpvgrid",&dcdpvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"dcdp2",&dcdpvgrid);
		} else {
			err(" dcdpvgrid missing ");
		}  
	}
	if (!getparint("ncdpvgrid",&ncdpvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"n2",&tmp);
			ncdpvgrid = (int)tmp;
		} else {
			err(" ncdpvgrid missing ");
		}  
	}
	if (!getparfloat("flinevgrid",&flinevgrid)) { 
		if(ierr==0) {
			getgval(&gh,"oline3",&flinevgrid);
		} else {
			err(" flinevgrid missing ");
		}  
	}
	if (!getparfloat("dlinevgrid",&dlinevgrid)) { 
		if(ierr==0) {
			getgval(&gh,"dline3",&dlinevgrid);
		} else {
			err(" dlinevgrid missing ");
		}  
	}
	if (!getparint("nlinevgrid",&nlinevgrid)) { 
		if(ierr==0) {
			getgval(&gh,"n3",&tmp);
			nlinevgrid = (int)tmp;
		} else {
			err(" ncdpvgrid missing ");
		}  
	}
	if (!getparfloat("ftvgrid",&ftvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"o1",&ftvgrid);
		} else {
			err(" ftvgrid missing ");
		}  
	}
	if (!getparfloat("dtvgrid",&dtvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"d1",&dtvgrid);
		} else {
			err(" dtvgrid missing ");
		}  
	}
	if (!getparint("ntvgrid",&ntvgrid)) { 
		if(ierr==0) {
			getgval(&gh,"n1",&tmp);
			ntvgrid = (int)tmp;
		} else {
			err(" ntvgrid missing ");
		}  
	}
	if (!getparint("ncdpline",&ncdpline)) ncdpline = ncdpvgrid;
	if (!getparint("cdp1",&cdp1)) cdp1 = 1;
	if (!getparint("trace1",&trace1)) trace1 = 1;
	if (!getparint("line1",&line1)) line1 = 1;
	if (!getparint("dline",&dline)) dline = 1;

	if (!getparfloat("fcdphvel",&fcdphvel)) fcdphvel = fcdpvgrid;
	if (!getparfloat("dcdphvel",&dcdphvel)) dcdphvel = dcdpvgrid;
	if (!getparint("ncdphvel",&ncdphvel)) ncdphvel = 1;
	if (!getparfloat("flinehvel",&flinehvel)) flinehvel = flinevgrid;
	if (!getparfloat("dlinehvel",&dlinehvel)) dlinehvel = dlinevgrid;
	if (!getparint("nlinehvel",&nlinehvel)) nlinehvel = 1;
	if (!getparfloat("fthvel",&fthvel)) fthvel = ftvgrid;
	if (!getparfloat("dthvel",&dthvel)) dthvel = 10. * dtvgrid;
	if (!getparint("nthvel",&nthvel)) nthvel = ntvgrid/10;

	if (!getparint("ivtype",&ivtype)) ivtype=0;
	if (!getparint("ovtype",&ovtype)) ovtype=0;
	if (!getparint("ittype",&ittype)) ittype=0;
	if (!getparint("ottype",&ottype)) ottype=0;
	if (!getparint("hcdptype",&hcdptype)) hcdptype=0;

	if(getparstring("template",&template)) {
                tmpfp = efopen(template,"r");
                if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
                if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
		if (!getparint("nthvel",&nthvel)) nthvel= 4096; 

                /* arrays used to store all VS3D card's x,y,time and velocity */
                cdp = (int*)emalloc(nvfmax*sizeof(int));
                nps = (int*)emalloc(nvfmax*sizeof(int));
		ts = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
		vs = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
		ncdp = 0;
		bzero(nps,nvfmax*sizeof(int));
		hvelread(tmpfp,cdp,ts,vs,&ncdp,nps,ntvmax,nvfmax);
	}


    	tin = (float*)malloc(ntvgrid*sizeof(float));
    	vin = (float*)malloc(ntvgrid*sizeof(float));
    	tout = (float*)malloc(nthvel*sizeof(float));
    	vout = (float*)malloc(nthvel*sizeof(float));

	for(it=0;it<ntvgrid;it++) tin[it] = ftvgrid + it*dtvgrid;

	if(ncdp==1) {
		nthvel = nps[0];
		for(it=0;it<nthvel;it++) tout[it] = ts[it];
	} else if(ncdp==0) {
		for(it=0;it<nthvel;it++) tout[it] = fthvel + it*dthvel;
	}

	if(ncdp>1) {
	    for(icdp=0;icdp<ncdp;icdp++) {
        	ocdp = cdp[icdp];
			if(hcdptype==0) {
	        	io3 = (ocdp-cdp1)/ncdpline*dline + line1;
            	/* io2 = ocdp - (io3-line1)/dline*ncdpline + trace1 - 1; */
            	io2 = ocdp - (io3-line1)/dline*ncdpline;
			} else {
				tmp = ocdp/10000;
				io3 = tmp;
				tmp = ocdp - io3*10000 + 0.5;
				io2 = tmp;
			}
           	tmp = (io3 - flinevgrid)/dlinevgrid + 0.5;
           	i3vgrid = (int) tmp;
           	tmp = (io2 - fcdpvgrid)/dcdpvgrid + 0.5;
           	i2vgrid = (int) tmp;
           	if(i3vgrid<0) i3vgrid=0;
            if(i3vgrid>=nlinevgrid) i3vgrid=nlinevgrid-1;
            if(i2vgrid<0) i2vgrid=0;
            if(i2vgrid>=ncdpvgrid)  i2vgrid=ncdpvgrid-1;
            efseek(infp,
            	(i2vgrid+i3vgrid*ncdpvgrid)*ntvgrid*sizeof(float),0);
            efread(vin,sizeof(float),ntvgrid,infp);
                /* time/depth conversion if needed */
			nthvel = nps[icdp];
			for(it=0;it<nthvel;it++) tout[it] = ts[it+icdp*ntvmax];
                vconvert(tin,vin,ntvgrid,ivtype,ittype,
                                tout,vout,nthvel,ovtype,ottype);
		/*
		for(it=0;it<ntvgrid;it++) 
			fprintf(stderr,"tin=%f vin=%f \n",tin[it],vin[it]);
		for(it=0;it<nthvel;it++) 
			fprintf(stderr,"tout=%f vout=%f \n",tout[it],vout[it]);
		*/

			printhvel(ocdp,nthvel,tout,vout,outfp);
fprintf(stderr,"Output HANDVEL at trace=%d line=%d cdp=%d i2vg=%d i3vg=%d\n",
                io2,io3,ocdp,i2vgrid+1,i3vgrid+1);
	    }
	} else {
	    for(i3=0;i3<nlinehvel;i3++) {
			o3 = flinehvel+i3*dlinehvel;
			io3 = o3;
			tmp = (o3 - flinevgrid)/dlinevgrid;
			i3vgrid = (int) tmp;
			if(i3vgrid<0) i3vgrid=0;
			if(i3vgrid>=nlinevgrid) i3vgrid=nlinevgrid-1;
	      	for(i2=0;i2<ncdphvel;i2++) {
				o2 = fcdphvel+i2*dcdphvel;
				io2 = o2;
				tmp = (o2 - fcdpvgrid)/dcdpvgrid;
				i2vgrid = (int) tmp;
				if(i2vgrid<0) i2vgrid=0;
				if(i2vgrid>=ncdpvgrid)  i2vgrid=ncdpvgrid-1;

				if(hcdptype==0) {
					tmp = o2 - trace1 + (o3-line1)/dline*ncdpline + cdp1; 
					ocdp = tmp;
				} else {
					ocdp = o3*10000+o2;
				}

				efseek(infp,
					(i2vgrid+i3vgrid*ncdpvgrid)*ntvgrid*sizeof(float),0);
				efread(vin,sizeof(float),ntvgrid,infp);
				/* time/depth conversion if needed */
				vconvert(tin,vin,ntvgrid,ivtype,ittype,
						tout,vout,nthvel,ovtype,ottype);
				printhvel(ocdp,nthvel,tout,vout,outfp);
fprintf(stderr,"Output HANDVEL at trace=%d line=%d cdp=%d i2vg=%d i3vg=%d\n",
                io2,io3,ocdp,i2vgrid+1,i3vgrid+1);
	      	}
	    }
	}
	
}
