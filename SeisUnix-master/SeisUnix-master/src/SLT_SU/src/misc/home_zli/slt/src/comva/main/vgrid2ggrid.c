/* VGRID2GGRID converts velocity grid file to gamma grid file */
#include "gridhd.h"
#include "comva.h"
#include "par.h"

string sdoc = 
"VGRID2GGRID - convert INTERVAL velocity grid file to gamma grid file \n"
"\n"
"vgrid2ggrid vgfile.old= nz= <vgfile.new >ggfile			\n" 
"\n"
"Required parameters:							\n"
"vgfile.new=      updated velocity grid file name			\n"
"vgfile.old=      old velocity grid file name		\n"
"ggfile=          gamma grid file name				\n"
"\n"
"Optional parameters:							\n"
"nz=from-header   number of depth points in the grid			\n"
"ismz=1           number of depth samples used to smooth output vertically \n"
"ismx=1           number of cdp points used to smooth output horizontally \n"
"zg1=-dz          minimum depth where gamma=1				\n" 
"                 where dz is the depth sampling interval of vgfile	\n"
"\n"
"NOTE:									\n"
" 1. The size of vgfile.old, vgfile.new and ggfile must be the same.	\n"
" 2. When nz is not given, it will be determined from the header value \n"
"    of the vgfile.old							\n"	
" 3. gamma is the ratio between average velocity of vgfile.new and    \n"
"    average velocity of vgfile.old					\n"
"									\n"
"AUTHOR:		Zhiming Li,       ,	10/4/91   \n"		    
;

main(int argc, char **argv)
{
    string vgfileold;
    FILE *infp=stdin,*outfp=stdout,*voldfp;
    int iz, nz, ix, nx;
    float *vnew, *vold, *gamma, *work, *smz, *smx;
    float sumold, sumnew;
    int ismz, ismx;
    int isize, jsize;
    float zg1, z;

    int n1,n2,n3,n4,n5;
    float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
    float scale, ocdp2, oline3, dcdp2, dline3,gmin,gmax;
    int dtype,ierr, orient,gtype;
    ghed gh;


    /* get parameters */
    initargs(argc,argv);
    askdoc(1);


    if (!getparstring("vgfile.old",&vgfileold)) 
	err("name of old velocity grid file (vgfile.old) must be given \n");

    voldfp = fopen(vgfileold,"r");
    ierr = fgetghdr(voldfp,&gh);
    if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                        &d1,&d2,&d3,&d4,&d5,
                        &o1,&o2,&o3,&o4,&o5,
                        &dcdp2,&dline3,&ocdp2,&oline3,&gmin,&gmax,
			&orient,&gtype);

    if (!getparint("nz",&nz)) {
	if(ierr==0) {
		nz = n1;
	} else {
		err("number of depth (nz) must be given \n");
	}
    }

    if (!getparint("ismz",&ismz)) ismz=1;
    if (!getparint("ismx",&ismx)) ismx=1;
    if (!getparfloat("zg1",&zg1)) zg1=-d1;

    ismz=(ismz/2)*2+1;
    ismx=(ismx/2)*2+1;
	
    /* check old and new velocity grid file sizes */
    fseek(voldfp,0L,2);
    isize = ftell(voldfp);
    fseek(voldfp,0,0);
    fseek(infp,0L,2);
    jsize = ftell(infp);
    fseek(infp,0,0);
    if(isize!=jsize) err("vgfile.new and vgfile.old sizes different !"); 

    if(ierr==0) {
	nx = n2;
    } else {
    	fseek(voldfp,0L,2);
        nx = ftell(voldfp)/sizeof(float)/nz;
        fseek(voldfp,0L,0);
    }

    if(ismz>nz) ismz=(nz/2)*2+1;
    if(ismx>nx) ismx=(nx/2)*2+1;

/* memory allocation */
    vold = (float*)malloc(nz*sizeof(float));
    vnew = (float*)malloc(nz*sizeof(float));
    gamma = (float*)malloc(nz*nx*sizeof(float));

    if(ismz>1 || ismx>1) {
        work = (float*)malloc(nz*nx*sizeof(float));
        smz = (float*)malloc(ismz*sizeof(float));
        smx = (float*)malloc(ismx*sizeof(float));
    }

/* main loop over x direction */
    for(ix=0;ix<nx;ix++) {
       	fread(vold,sizeof(float),nz,voldfp);
       	fread(vnew,sizeof(float),nz,infp);

       	/* compute average velocity and gamma */
       	sumnew = vnew[0];
       	sumold = vold[0];
	gamma[0+ix*nz] = sumnew/sumold;
       	for(iz=1;iz<nz;iz++) {
	  	sumnew += vnew[iz];
	  	sumold += vold[iz];
		gamma[iz+ix*nz] = sumnew/sumold;
	}
	for(iz=0,z=0;z<zg1;iz++,z=z+d1) gamma[iz+ix*nz] = 1.;
    }

    if(ismz>1 || ismx >1) smth2d_(gamma,work,smz,smx,&nz,&nx,&ismz,&ismx);

    fwrite(gamma,sizeof(float),nz*nx,outfp);
    fflush(outfp);
    if (ierr==0) {
        fminmax(gamma,nz*nx,&gmin,&gmax);
	putgval(&gh,"gmin",gmin);
	putgval(&gh,"gmax",gmax);
	fputghdr(outfp,&gh); 
    }
    fclose(outfp);

    if(ismz>1 || ismx >1) {
        free(work);
        free(smz);
        free(smx);
    }
    free(gamma);
    free(vnew);
    free(vold);

    return(0);
}
