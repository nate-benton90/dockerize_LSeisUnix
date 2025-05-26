/* w-x 90-degree 3-d poststack modeling  */
/*     features:
		up-to 90 degree accuracy
		absorbing boundaries
		dispersion suppression
		stable
		implicit f.d. scheme
		spliting method used 
		f.d. error compensation 
*/

#include "par.h"

char *sdoc[] = {
" 									",
" WXMD3D - frequency-space domain 3-d modeling			 	",
" 									",
" wxmd3d [parameters] <3d-reflectivity >modeled-3d-stack		", 
" 									",
" Required parameters:							",
" nt			number of time samples per trace in stack	",
" nx			number of traces per line 			",
" ny			number of lines in 3-d volume			",
" dt 			time sampling interval 				",
" dx			trace sampling interval				",
" dy			line sampling interval				",
" dz			depth sampling interval				",
" velfile 		velocity file name (velocity is stored as v(x,y,z),",
" 			i.e., nz planes of (nx*ny))			",
" 									",
" Optional parameters:							",
" nz=all		number of depth steps				",
" istep=1 		depth extrapolation step			",
" iorder=1 		order of modeling operator			",
" 			(0=45 1=65 2=80 3=87 4=89.99 5=90 degrees)	",
" nfft=nt padded  	fft length of trace (power of 2)		",
" dfc=0.03 		dip-filtering constant (0-1)			",
" fmin=(0.5/dt)/20  	lowest frequency to migrate (hz)		",
" fmax=(0.5/dt)*2/3  	highest frequency to migrate (hz)		",
" icstep=0 		f.d. error compensation step (0=no)		",
" mlimit=16 		memory limit (in megabytes) to run the program	",
" 									",
" Note:									",
" Both input reflectivity and velocity files are stored as in (x,y,z) order. ", 
" 									",
NULL};

/*
 * AUTHOR: Zhiming Li, CSM, 5/30/90
 */

main(int argc, char **argv)
{
    int nt,nx,ny,nz,nfft,nfftq,it,ix,iy,iz,iorder,itord,lwork;
    int ipdisk=0,ivdisk=0,istep,iqdisk=0,nqt,nxy;
    int icstep,nxp,nyp,ntmp,mlimit,i1,i2,i3,mtotal;
    float dt,dx,dy,dz,*q,*v,dfc,fmin,fmax,*vx,*qt,*qx,*qx1,*vov;
    float *qq;
    complex *cp,*work,*aa,*bb,*a,*b;
    complex *bcp, *ccp; 
    char *qname,*vname,*velfile;
    char *stats,*disps;
    FILE *infp=stdin,*outfp=stdout,*velfp;

    /* get parameters */
    initargs(argc,argv);
    requestdoc(1);
    if (!getparint("nt",&nt)) err("Must specify nt!\n");
    if (!getparint("nx",&nx)) err("Must specify nx!\n");
    if (!getparint("ny",&ny)) err("Must specify ny!\n");
    if (!getparint("nz",&nz)) 
      {
        fseek(infp,0L,2);
        nz = ftell(infp)/sizeof(float)/(nx*ny);
        fseek(infp,0L,0);
      }
    if (!getparint("nfft",&nfft)) nfft=nt;
    if (nfft < nt) nfft = nt;
    nfftq = 1;
    do {nfftq = nfftq * 2 ;} while (nfftq <nfft);
    nfft = nfftq; nfftq = nfft/2+1;
    if (!getparfloat("dt",&dt)) err("Must specify dt!\n");
    if (!getparfloat("dx",&dx)) err("Must specify dx!\n");
    if (!getparfloat("dy",&dy)) err("Must specify dy!\n");
    if (!getparfloat("dz",&dz)) err("Must specify dz!\n");
    if (!getparstring("velfile",&velfile)) err("Must specify velfile!");
    if (!getparint("iorder",&iorder)) iorder = 1;
    if (!getparint("istep",&istep)) istep = 1;
    if (!getparfloat("dfc",&dfc)) dfc = 0.03;
    if (!getparfloat("fmin",&fmin)) fmin = 0.05 * .5 / dt;
    if (!getparfloat("fmax",&fmax)) fmax = 2. / 3. * .5 / dt;
    if (!getparint("icstep",&icstep)) icstep = 0;
/*    if ( ny <= 2) icstep=0;   */
    if ( icstep > 0 ) 
       {
       icstep = ((icstep+istep-1)/istep)*istep;
       }
    if ( icstep > 0 ) 
       {
       nxp = nx * 3 / 2 ; 
       ntmp = 1; do {ntmp=ntmp*2;} while (ntmp <nxp);
       nxp = ntmp; if ( nx == 1 ) nxp = 1;
       nyp = ny * 3 / 2 ; 
       ntmp = 1; do {ntmp=ntmp*2;} while (ntmp <nyp);
       nyp = ntmp; if (ny == 1 ) nyp = 1;
       bcp = (complex*)malloc(nxp*nyp*sizeof(complex));
       }
    if (!getparint("mlimit",&mlimit)) mlimit = 16;
    mlimit = mlimit * 1024 * 1024; 	

    lwork = nx*ny*5; if ( lwork < nfft*3/2+1+nx*ny ) lwork=nfft*3/2+1+nx*ny;
    /* allocate space */
    nxy = nx; if ( nxy < ny ) nxy = ny; 
    nqt = nxy; if ( nqt < nt ) nqt = nt; 
    mtotal = nxp*nyp*sizeof(complex);
    if ( icstep == 0 ) mtotal = 0;
    mtotal = mtotal + nx*ny*nfftq*sizeof(complex);
    mtotal = mtotal + lwork*sizeof(complex);
    mtotal = mtotal + 4*nxy*sizeof(complex);
    mtotal = mtotal + 2*nx*ny*nz*sizeof(float);
    mtotal = mtotal + nx*ny*nt*sizeof(float);
    mtotal = mtotal + nqt*sizeof(float);
    mtotal = mtotal + 4*nxy*sizeof(float);
    fprintf(stderr,"total memory size needed (bytes) = %d \n",mtotal);
    if ( mtotal <= mlimit ) 
       {
       i1=nfftq; ipdisk=0;
       i2 = nt; iqdisk = 0;
       i3 = nz; ivdisk = 0;
       }
    else if ( mtotal > mlimit ) 
       {
       i1 = nfftq; ipdisk=0;
       i2 = 1; iqdisk = 22;
       i3 = 1; ivdisk = 33;
       mtotal = mtotal - (nt-1)*nx*ny*sizeof(float);
       mtotal = mtotal - 2*(nz-1)*nx*ny*sizeof(float);
       fprintf(stderr,"reduced memory size (bytes) = %d \n",mtotal);
       }
    if ( mtotal > mlimit ) 
       {
       i1 = 1; ipdisk = 11;
       mtotal = mtotal - (nfftq-1)*nx*ny*sizeof(complex);
       mtotal = mtotal + nxy*nfftq*sizeof(complex);
       ccp = (complex*)malloc(nfftq*nxy*sizeof(complex));
       fprintf(stderr,"further reduced memory size (bytes) = %d \n",mtotal);
       }
    if ( mtotal > mlimit ) err("memory size too small !\n");
       

    cp = (complex*)malloc(nx*ny*i1*sizeof(complex));
    work = (complex*)malloc(lwork*sizeof(complex));
    aa = (complex*)malloc(nxy*sizeof(complex));
    bb = (complex*)malloc(nxy*sizeof(complex));
    a = (complex*)malloc(nxy*sizeof(complex));
    b = (complex*)malloc(nxy*sizeof(complex));
    q = (float*)malloc(nx*ny*i3*sizeof(float));
    v = (float*)malloc(i3*ny*nx*sizeof(float));
    qq = (float*)malloc(i2*nx*ny*sizeof(float));
    qt = (float*)malloc(nqt*sizeof(float));
    qx = (float*)malloc(nxy*sizeof(float));
    qx1 = (float*)malloc(nxy*sizeof(float));
    vx = (float*)malloc(nxy*sizeof(float));
    vov = (float*)malloc(nxy*sizeof(float));

    /* open and read velocity file */
    /* velocity is stored as v(x,y,z) */
    if ((velfp=fopen(velfile,"r"))==NULL)
        err("Error opening velfile=%s\n",velfile);
    if ( ivdisk == 0 ) 
       {
       if (fread(v,sizeof(float),nx*ny*nz,velfp)!=nx*ny*nz)
          err("Error reading velfile=%s\n",velfile);
       /* scale velocity by 1./2. for poststack */
       for (ix=0;ix<nx*ny*nz;ix++) v[ix] = v[ix] * .5;
       }
    else if ( ivdisk > 0 ) 
       {
       i3 = nx * ny * sizeof(float);
       i2 = 0;
       vname = "vfile";
       disps = "DELETE";
       stats = "UNKNOWN";
       opdisk (&ivdisk,&i3,&i2,vname,disps,stats);
       if ( i2 !=0 ) err("error open v-disk! \n");
       for (iz=1;iz<=nz;iz++)
	  {
	  fread(v,sizeof(float),nx*ny,velfp);
	  for (ix=0;ix<nx*ny;ix++) v[ix] = v[ix] * 0.5;
	  i1 = iz;
	  i2 = 0;
	  ridisk (&ivdisk,v,&i1,&i3,&i2);
	  if ( i2 != 0 ) err("error write v-disk! \n");
	  }
       }

    /* read input reflectivity q(x,y,z) */
    if (iqdisk == 0 ) 
       {
       if (fread(q,sizeof(float),nx*ny*nz,infp)!=nx*ny*nz)
          err("Error reading input\n");
       }
    else if (iqdisk > 0 ) 
       {
       i3 = nx * ny * sizeof(float);
       i2 = 0;
       qname = "qfile";
       disps = "DELETE";
       stats = "UNKNOWN";
       opdisk (&iqdisk,&i3,&i2,qname,disps,stats);
       if ( i2 !=0 ) err("error open q-disk! \n");
       for (iz=1;iz<=nz;iz++)
	  {
	  fread(q,sizeof(float),nx*ny,infp);
	  i1 = iz;
	  i2 = 0;
	  ridisk (&iqdisk,q,&i1,&i3,&i2);
	  if ( i2 != 0 ) err("error write q-disk! \n");
	  }
       }


    /* call modeling routine */
     wxmd3d (q, &nt, &nx, &dt, &dx, v, 
             cp, work, &dfc, &lwork,
	     &fmin, &fmax, &ipdisk, &nfft, &istep, &ivdisk,
	     &iqdisk, vx, qt, qx, qx1,
	     vov, aa, bb, &itord, 
	     a, b, &iorder, qname, 
	     vname, &nfftq, qq, &nz, &dz, &ny, &dy,
	     &icstep,bcp,&nxp,&nyp,
	     ccp,&nxy);

    /* output modeling result qq(t,x,y) */

    if ( iqdisk == 0 ) 
       {
       fwrite(qq,sizeof(float),nt*nx*ny,outfp);
       }
    else if (iqdisk > 0 ) 
       {
       i1 = nt * sizeof(float);
       for (ix=1;ix<=nx*ny;ix++)
          {
          i2 = ix;
          i3 = 0;
          redisk (&iqdisk,qt,&i2,&i1,&i3);
          if ( i3 != 0 ) err("error read q-disk! \n");
          fwrite(qt,sizeof(float),nt,outfp);
          }
       ftncls (&iqdisk);
       }
}
