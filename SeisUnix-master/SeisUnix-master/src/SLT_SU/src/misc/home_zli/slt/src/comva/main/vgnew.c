/* VGNEW update velocity grid file according to input gamma grid file*/
#include "gridhd.h"
#include "comva.h"
#include "par.h"

string sdoc = 
"VGNEW - update velocity grid file according to input gamma grid file \n"
"\n"
"vgnew ggfile= <vgfile.old >vgfile.new			\n" 
"\n"
"Required parameters:							\n"
"vgfile.old=        input interval velocity grid file name		\n"
"ggfile=            gamma grid file name				\n"
"vgfile.new=        output interval velocity grid file name		\n"
"\n"
"Optional parameters:							\n"
"updatez=0          1=to update the depth of v grid also; 0=no	\n"
"nz=from-header     number of depth points in the grid (vgfile and ggfile)\n"
"fz=from-header     depth of the first level in the grid		\n"
"dz=from-header     depth interval of the grid				\n"
"ismz=1             number of depths used in smoothing output vertically \n"
"ismx=1             number of cdps used in smoothing output horizontally \n"
"\n"
"NOTE:									\n"
" 1. The size of vgfile.old and ggfile must be the same. The trace locations \n"
"    of vgfile.old and ggfile must also be the same.			\n"
" 2. nz, dz, and fz will be obtained from the headers of both vgfile.old\n"
"    and ggfile, if not given. When both vgfile.old and ggfile do not have\n" 
"    the standard header, nz must be given, dz defaults to 1, fz defaults \n"
"    to 0.								\n" 
" 3. gamma is defined as Vnew/Vold, where Vnew is new average velocity and \n"
"    Vold is old average velocity 					\n"
"									\n"
"AUTHOR:		Zhiming Li,       ,	9/12/91   \n"		    
;

main(int argc, char **argv)
{
    string ggfile;
    FILE *infp=stdin,*outfp=stdout,*gfp;
    int iz, nz, ix, nx, i;
    float *vin, *vout, *vaout, *gamma, znew, zold, odz, *work, *smz, *smx;
    float vain, fz, dz, res;
    int updatez, indx, ismz, ismx, one=1;

    ghed vgh, ggh;
    float vscale;
    int vdtype;
    int vn1,vn2,vn3,vn4,vn5;
    float vo1,vo2,vo3,vo4,vo5,vd1,vd2,vd3,vd4,vd5; 
    float vocdp2,voline3, vdcdp2, vdline3,vgmin,vgmax;
    int vierr; 
    float gscale;
    int gdtype;
    int gn1,gn2,gn3,gn4,gn5;
    float go1,go2,go3,go4,go5,gd1,gd2,gd3,gd4,gd5; 
    float gocdp2,goline3, gdcdp2, gdline3,ggmin,ggmax;
    int gierr; 
    int n1, ierr;
    float o1,d1;
    int orient,gtype; 

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

    if (!getparstring("ggfile",&ggfile)) 
	err("name of gamma grid file (ggfile) must be given \n");

    /* open vgfile and get header values*/
    vierr = fgetghdr(infp,&vgh);
    if(vierr==0) fromghdr(&vgh,&vscale,&vdtype,&vn1,&vn2,&vn3,&vn4,&vn5,
    			&vd1,&vd2,&vd3,&vd4,&vd5,
    			&vo1,&vo2,&vo3,&vo4,&vo5,
    			&vdcdp2,&vdline3,&vocdp2,&voline3,&vgmin,&vgmax,
			&orient,&gtype);

    /* open ggfile and get header values */
    gfp = efopen(ggfile,"r");
    gierr = fgetghdr(gfp,&ggh);
    if(gierr==0) fromghdr(&ggh,&gscale,&gdtype,&gn1,&gn2,&gn3,&gn4,&gn5,
    			&gd1,&gd2,&gd3,&gd4,&gd5,
    			&go1,&go2,&go3,&go4,&go5,
    			&gdcdp2,&gdline3,&gocdp2,&goline3,&ggmin,&ggmax,
			&orient,&gtype);

    if(vierr==0 && gierr==0) {
          if(vn1!=gn1 || vo1!=go1 || vd1!=gd1 || vn2!=gn2) 
		err(" vgfile and ggfile: header values not consistant ");
    }

    ierr = 1;
    if(vierr!=0) {
	n1 = vn1;
	o1 = vo1;
	d1 = vd1;
	ierr = 0;
    } else if(gierr=0) {
	n1 = gn1;
	o1 = go1;
	d1 = gd1;
	ierr = 0;
    }
    if (!getparint("nz",&nz)) {
	if(ierr==0) {
		nz = n1;
	} else {
		err(" nz must be give ");
	} 
    } 
    if(ierr==0 && nz!=n1) err(" check nz value ");
    if (!getparfloat("fz",&fz)) {
	if(ierr==0) {
		fz = o1;
	} else {
		fz = 0.;
	}
    }
    if(ierr==0 && fz!=o1) err(" check fz value ");
    if (!getparfloat("dz",&dz)) {
	if(ierr==0) {
		dz = d1;
	} else {
		dz = 1.;
	}
    }
    if(ierr==0 && dz!=d1) err(" check dz value ");

    if (!getparint("updatez",&updatez)) updatez = 0;
    odz = 1./dz;
    if (!getparint("ismz",&ismz)) ismz=1;
    if (!getparint("ismx",&ismx)) ismx=1;
    ismz=(ismz/2)*2+1;
    ismx=(ismx/2)*2+1;
	
    fseek(gfp,0L,2);
    nx = ftell(gfp)/sizeof(float)/nz;
    fseek(gfp,0L,0);
    fseek(infp,0L,2);
    if (nx!=ftell(infp)/sizeof(float)/nz) 
	err("ggfile and vgfile sizes different !"); 
    fseek(infp,0L,0);

    if(ismz>nz) ismz=nz;
    if(ismx>nx) ismx=nx;


    
/* memory allocation */
    vin = (float*)malloc(nz*sizeof(float));
    vout = (float*)malloc(nz*nx*sizeof(float));
    vaout = (float*)malloc(nz*sizeof(float));
    gamma = (float*)malloc(nz*sizeof(float));
    if(ismz>1 || ismx>1) {
        work = (float*)malloc(nz*nx*sizeof(float));
        smz = (float*)malloc(ismz*sizeof(float));
        smx = (float*)malloc(ismx*sizeof(float));
    }

/* main loop over x direction */
    for(ix=0;ix<nx;ix++) {
       fread(vin,sizeof(float),nz,infp);
       fread(gamma,sizeof(float),nz,gfp);
       /* gamma correction of the velocity */
       for(iz=0;iz<nz;iz++) {
          vain = 0.;
          for(i=0;i<=iz;i++) {
	 	vain += vin[i];
	  }
          vain = vain/(iz+1.);
          vaout[iz] = vain*gamma[iz];
       }

       if(ismz>1) smth2d_(vaout,work,smz,smx,&nz,&one,&ismz,&one);

       vout[ix*nz] = vaout[0];
       for(iz=1;iz<nz;iz++) vout[iz+ix*nz] = iz*vaout[iz]-(iz-1)*vaout[iz-1]; 
       /* depth correction if need */
       if(updatez) {
          /* output z level */
          for(iz=0;iz<nz;iz++) {
	     znew = fz + iz * dz;
	     zold = ( znew / gamma[iz] - fz ) * odz;
	     indx = zold;
	     res = zold - indx;
	     vin[iz] = vout[iz+ix*nz];
	     if(indx>=0 && indx<nz-1) {
	        vout[iz+ix*nz] = vin[indx] + res*(vin[indx+1]-vin[indx]);
	     }
	     else if (indx<0) {
	        vout[iz+ix*nz] = vin[0];
	     }
	     else {
	        vout[iz+ix*nz] = vin[nz-1];
	     }
	  }
       }

    }

    if(ismz>1 || ismx >1) smth2d_(vout,work,smz,smx,&nz,&nx,&ismz,&ismx);
    fwrite(vout,sizeof(float),nz*nx,outfp);

    fminmax(vout,nz*nx,&vgmin,&vgmax);
    fflush(outfp);
    /* add header to output velocity grid, if input vgfile has one */
    if(vierr==0) {
	putgval(&vgh,"gmin",vgmin);
	putgval(&vgh,"gmax",vgmax);
	fputghdr(outfp,&vgh);
    }
    fclose(outfp);
    if(ismz>1 || ismx >1) {
        free(work);
        free(smz);
        free(smx);
    }
    free(vout);
    free(vin);
    free(gamma);
    free(vaout);

    return EXIT_SUCCESS;
}
