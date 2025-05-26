/* VGRID2ASCII converts a velocity grid file to an ascii file */
#include "gridhd.h"
#include "comva.h"
#include "par.h"

string sdoc = 
"VGRID2ASCII - convert velocity grid file to ascii file \n"
"\n"
"vgrid2ascii <velgrid [parameters] >ascii.file 				\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"\n"
"Optional parameters:							\n"
"nz=from-header      number of depth points in the grid			\n"
"fz=from-header      starting depth in the grid				\n"
"dz=from-header      depth interval in the grid				\n"
"ncdp=from-header    number of cdp positions in the grid		\n"
"fcdp=from-header    starting cdp number in the grid			\n"
"dcdp=from-header    cdp number increment in the grid			\n"
"\n"
"NOTE:									\n"
" 1. When input velgrid is nonstandard, i.e., without header, the above \n"
"    six grid parameters must be given.				 	\n"
"									\n"
"AUTHOR:		Zhiming Li,       ,	8/26/92   		\n"		    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin, *outfp=stdout;
	float *vel; 
	int ncdp,nz;
	float fz, fcdp, dz, dcdp;
	int icdp,iv,i,j,nj;

	float scale;
	int dtype;
	int n1,n2,n3,n4,n5;
	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
	float dcdp2,dline3,ocdp2,oline3,gmin,gmax;
	ghed gh;
	int ierr, orient, gtype;



    	/* initialization and self-documentation */
    	initargs(argc,argv);
    	askdoc(1);

	/* get grid header parameters */
	ierr = fgetghdr(infp,&gh);
	if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
				&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
				&dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
				&orient,&gtype);

	/* get optional parameters */

    	if (!getparint("nz",&nz)) {
		if(ierr==0) {
			nz = n1;
		} else {
			err(" nz must be specified ");
		}
	}
    	if (!getparint("ncdp",&ncdp)) {
		if(ierr==0) {
			ncdp = n2;
		} else {
			err(" ncdp must be specified ");
		}
	}
    	if (!getparfloat("fz",&fz)) {
		if(ierr==0) {
			fz = o1;
		} else {
			err(" fz must be specified ");
		}
	}
    	if (!getparfloat("dz",&dz)) {
		if(ierr==0) {
			dz = d1;
		} else {
			err(" dz must be specified ");
		}
	}
    	if (!getparfloat("fcdp",&fcdp)) {
		if(ierr==0) {
			fcdp = ocdp2;
		} else {
			err(" fcdp must be specified ");
		}
	}
    	if (!getparfloat("dcdp",&dcdp)) {
		if(ierr==0) {
			dcdp = dcdp2;
		} else {
			err(" dcdp must be specified ");
		}
	}

	/* error checking */
	if(ierr==0) {
		if(nz!=n1 || ncdp!=n2 || fz!=o1 || dz!=d1 || 
			fcdp!=ocdp2 || dcdp!=dcdp2 ) 
			err(" check input grid header file "); 
	} else {
		warn(" input grid nonstandard; user input parameters used");
	}



    	fseek(infp,0,0);
	/* output ascii file */
	vel = (float*) malloc(nz*sizeof(float));

	fprintf(outfp," === Velocity Ascii File === \n"); 
	fprintf(outfp," number of velocities (depths) per cdp = %d \n",nz); 
	fprintf(outfp," number of cdps                        = %d \n",ncdp); 
	fprintf(outfp," minimum depth                         = %g \n",fz); 
	fprintf(outfp," minimum cdp number                    = %g \n",fcdp); 
	fprintf(outfp," depth interval                        = %g \n",dz); 
	fprintf(outfp," cdp number increment                  = %g \n",dcdp); 
	fprintf(outfp," number of velocity values per line    = 10 \n"); 
	fprintf(outfp,"\n"); 
	for(icdp=0;icdp<ncdp;icdp++) {
		fprintf(outfp," velocities at cdp = %g \n",fcdp+icdp*dcdp);
		
		efread(vel,sizeof(float),nz,infp);
		for(i=0;i<nz;i=i+10) {
			nj = 10;
			if(i+nj>nz) nj=nz-i;
			for(j=0;j<nj;j++) {
				iv = vel[i+j];	
				fprintf(outfp,"%7d",iv);
			}
			fprintf(outfp,"\n");
		}
	}

	return EXIT_SUCCESS;
}
