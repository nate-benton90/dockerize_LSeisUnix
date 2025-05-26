/* velocity grid to VELF card conversion */

#include "ghdr.h"
#include "gridhd.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"VGRID2VELF - convert velocity grid to wgc VELF cards  			\n"
"\n"
"vgrid2velf [parameters] < vgrid  >velf-cards 				\n" 
"\n"
"Required parameters:						 	\n"
"vgrid=                Name of velocity grid 				\n"
"velf-cards=           Name of dataset to output VELF cards 		\n"
"\n"
"Optional parameters:							\n"
"fcdpvgrid=from_header First cdp number of velocity grid 		\n"
"dcdpvgrid=from_header cdp number increment of velocity grid		\n"
"ncdpvgrid=from_header Number of cdp locations of velocity grid		\n"
"ftvgrid=from_header   First Time/depth (in ms/m or ft)	of velocity grid\n"
"dtvgrid=from_header   Time/depth sampling interval (in ms/m or ft)	\n"
"                      of velocity grid					\n"
"ntvgrid=from_header   Number of time/depth samples of velocity grid 	\n"
"fcdpvelf=fcdpvgrid    First cdp number to output VELF cards 		\n"
"dcdpvelf=1            cdp number increment to output VELF cards	\n"
"ncdpvelf=1            Number of cdps to output VELF cards		\n"
"ftvelf=ftvgrid        First time/depth (in ms/m or ft) to output VELF	\n" 
"dtvelf=10*dtvgrid     Time/depth interval (in ms/m or ft) to output VELF\n" 
"ntvelf=ntvgrid/10     Number of times/depths to output VELF		\n"
"ivtype=0              Input velocity grid type (0=rms; 1=avg; 2=int)	\n" 
"ovtype=0              Output VELF velocity type (0=rms; 1=avg; 2=int)	\n" 
"ittype=0              Input velocity grid time/depth type (0=time 1=depth)\n" 
"ottype=0              Output VELF time/depth type (0=time 1=depth)	\n" 
"\n"
"Notes:									\n"
" 1. This program can be used to make velocity contour plot using WGC	\n"
"    plotting program.							\n" 
" 2. When input velocity grid is a nonstandard grid file, i.e., without	\n"
"    header, 6 grid parameters of input velocity grid must be supplied.	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	9/8/92   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;

	int ncdpvgrid,ntvgrid,ncdpvelf,ntvelf;
	float fcdpvgrid,dcdpvgrid,ftvgrid,dtvgrid,tmp;
	float fcdpvelf,dcdpvelf,ftvelf,dtvelf;
	int ivtype,ovtype;
	int ittype,ottype;

    	float *tin, *tout, *vin, *vout;
	float ocdp; 
	int icdp, p1, p2, ic, nc, ip, it, ivgrid; 

	ghed gh;
	int ierr;
	float vmax, vmin; 

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

	if (!getparfloat("fcdpvelf",&fcdpvelf)) fcdpvelf = fcdpvgrid;
	if (!getparfloat("dcdpvelf",&dcdpvelf)) dcdpvelf = 1.; 
	if (!getparint("ncdpvelf",&ncdpvelf)) ncdpvelf = 1;
	if (!getparfloat("ftvelf",&ftvelf)) ftvelf = ftvgrid;
	if (!getparfloat("dtvelf",&dtvelf)) dtvelf = 10. * dtvgrid;
	if (!getparint("ntvelf",&ntvelf)) ntvelf = ntvgrid/10;

	if (!getparint("ivtype",&ivtype)) ivtype=0;
	if (!getparint("ovtype",&ovtype)) ovtype=0;
	if (!getparint("ittype",&ittype)) ittype=0;
	if (!getparint("ottype",&ottype)) ottype=0;

    	tin = (float*)malloc(ntvgrid*sizeof(float));
    	vin = (float*)malloc(ntvgrid*sizeof(float));
    	tout = (float*)malloc(ntvelf*sizeof(float));
    	vout = (float*)malloc(ntvelf*sizeof(float));

	for(it=0;it<ntvgrid;it++) tin[it] = ftvgrid + it*dtvgrid;
	for(it=0;it<ntvelf;it++) tout[it] = ftvelf + it*dtvelf;

	for(icdp=0;icdp<ncdpvelf;icdp++) {
		ocdp = fcdpvelf + icdp * dcdpvelf;
		tmp = (ocdp - fcdpvgrid)/dcdpvgrid;
		ivgrid = (int) tmp;
		if(ivgrid<0) ivgrid=0;
		if(ivgrid>=ncdpvgrid)  ivgrid=ncdpvgrid-1;
		efseek(infp,ivgrid*ntvgrid*sizeof(float),0);
		efread(vin,sizeof(float),ntvgrid,infp);
		/* time/depth conversion if needed */
		vconvert(tin,vin,ntvgrid,ivtype,ittype,
				tout,vout,ntvelf,ovtype,ottype);
		for(ic=0;ic<ntvelf;ic=ic+5) {
			if(ic==0) {
				fprintf(outfp,"VELF %10d     ",(int)ocdp);
			} else {
				fprintf(outfp,"VELF                ");
			}
			nc = 5;
			if(ic+nc>ntvelf) nc = ntvelf - ic;
			for(ip=0;ip<nc;ip++) {
				p1 = (int) tout[ic+ip];
				p2 = (int) vout[ic+ip];
				fprintf(outfp,"%5d%5d",p1,p2);
			} 
			fprintf(outfp,"\n");
		}
		fprintf(stderr,"Output VELF at cdp=%d \n",(int)ocdp);
	}
	
}
