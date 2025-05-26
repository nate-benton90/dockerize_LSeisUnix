/* ascii (z,x,y,g) to grid (z,x,y,g) conversion */

#include "usgrid.h"
#include "velo.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"ASC2GRID - convert ASCII (z,x,y,g) to grid (z,x,y,g) \n"
"\n"
"asc3grid [parameters] <ascii-file  >grid 				\n" 
"\n"
"Required parameters:						 	\n"
"ascii-file        Name of file containing columns of z x y g 	\n"
"grid              Name of grid file 			\n"
"zmin=             minimum z of input data points to be used \n" 
"zmax=             maximum z of input data points to be used \n" 
"xmin=             minimum x of input data points to be used \n" 
"xmax=             maximum x of input data points to be used \n" 
"ymin=             minimum y of input data points to be used \n" 
"ymax=             maximum y of input data points to be used \n" 
"fz=               First z coordinate to output grid 	\n"
"dz=               z increment to output grid 		\n"
"nz=               number of z positions to output grid 	\n"
"fx=               First x coordinate to output grid 	\n"
"dx=               x increment to output grid 		\n"
"nx=               number of x positions to output grid 	\n"
"fy=               First y coordinate to output grid 	\n"
"dy=               y increment to output grid 		\n"
"ny=               number of y positions to output grid 	\n"
"\n"
"Optional parameters:							\n"
"nrmax=100000      maximum number of rows (z,x,y,g) in input ascii file \n"
"nf=3              number of closest data points at a constant z used to 	\n"
"                  interpolate grid at an output location		\n" 
"                  =0   use all the data points whose (x,y) distance from 	\n"
"                  output is less than dismax 				\n"
"dismax=10000      maximum (x,y) distance of which a data point can	\n"
"                  be used to interpolate output			\n" 
"\n"
"AUTHOR:	   Zhiming Li,       ,	5/29/98   		\n"    
;

main(int argc, char **argv)
{
   	FILE *infp=stdin, *outfp=stdout;
	float fx,fy,fz,dx,dy,dz;
	float xmin,xmax,ymin,ymax,zmin,zmax;
	int nx,ny,nz;
	int nf, nrmax;
	float dismax;
   	float *xs, *ys, *zs, *gs;
	float *grid;
	char *cbuf;
	int ir, i, ii, jj, kk, ix, iy, iz, nn;
	float *work, *xx, *yy, *gg;
	int *indx, *zz, *nxy, *counts;
	int one=1;
	float x, y, z, g, tmp;

	usghed usgh;
	float gmin, gmax;
	int ierr;
	int orient=1, gtype=0;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* required parameters */
	if (!getparfloat("fx",&fx)) err(" fx missing "); 
	if (!getparfloat("fy",&fy)) err(" fy missing ");
	if (!getparfloat("fz",&fz)) err(" fz missing ");
	if (!getparfloat("dx",&dx)) err(" dx missing ");
	if (!getparfloat("dy",&dy)) err(" dy missing ");
	if (!getparfloat("dz",&dz)) err(" dz missing ");
	if (!getparint("nx",&nx)) err(" nx missing ");
	if (!getparint("ny",&ny)) err(" ny missing ");
	if (!getparint("nz",&nz)) err(" nz missing ");
	if (!getparfloat("zmin",&zmin)) err(" zmin missing "); 
	if (!getparfloat("zmax",&zmax)) err(" zmax missing "); 
	if (!getparfloat("xmin",&xmin)) err(" xmin missing "); 
	if (!getparfloat("xmax",&xmax)) err(" xmax missing "); 
	if (!getparfloat("ymin",&ymin)) err(" ymin missing "); 
	if (!getparfloat("ymax",&ymax)) err(" ymax missing "); 

	/* optional parameters */
	if (!getparint("nf",&nf)) nf=3;
	if (!getparfloat("dismax",&dismax)) dismax=10000.;
	if (!getparint("nrmax",&nrmax)) nrmax = 100000;

   	xs = (float*)emalloc(nz*nx*ny*sizeof(float));
   	ys = (float*)emalloc(nz*nx*ny*sizeof(float));
   	zs = (float*)emalloc(nz*nx*ny*sizeof(float));
   	gs = (float*)emalloc(nz*nx*ny*sizeof(float));
   	counts = (int*)emalloc(nz*nx*ny*sizeof(int));
	cbuf = (char*)emalloc(134*sizeof(char));	

	bzero(counts,nx*ny*nz*sizeof(int));
	bzero(xs,nx*ny*nz*sizeof(float));
	bzero(zs,nx*ny*nz*sizeof(float));
	bzero(ys,nx*ny*nz*sizeof(float));
	bzero(gs,nx*ny*nz*sizeof(float));
   	/* read in ascii file */
	/*
	fprintf(stderr,"zmin=%f zmax=%f xmin=%f xmax=%f ymin=%f ymax=%f\n",
		zmin,zmax,xmin,xmax,ymin,ymax);
	*/
	for(ir=0;ir<nrmax;ir++) {
		if (feof(infp) !=0 ) break;
		for(i=0;i<134;i++) cbuf[i]=' ';
		gets(cbuf);
		sscanf(cbuf,"%f %f %f %f \n",&z,&x,&y,&g);

		if(zmin<=z && z<=zmax && xmin<=x && x<=xmax && ymin<=y && y<=ymax) {
		/* fprintf(stderr,"%f %f %f %f \n",z,x,y,g); */
			tmp = (z - fz)/dz + 0.5;
			iz = tmp;
			tmp = (y - fy)/dy + 0.5;
			iy = tmp;
			tmp = (x - fx)/dx + 0.5;
			ix = tmp;

/* 			fprintf(stderr," iz=%d ix=%d iy=%d \n",iz,ix,iy); */ 

			if(iz>=0 && iz<nz && ix>=0 && ix<nx && iy>=0 && iy<ny) {
				ii = iz*nx*ny+iy*nx+ix;
				counts[ii] += 1;
				xs[ii] += x;
				ys[ii] += y;
				zs[ii] += z;
				gs[ii] += g;
/*				fprintf(stderr,"%f %f %f %f \n",z,x,y,g);  */
			}
		}
	}

	fprintf(stderr," total of %d rows read from input \n",ir-1);

	free(cbuf);

	nxy = (int*) emalloc(nz*sizeof(int));
	grid = (float*) emalloc(nx*ny*nz*sizeof(float));
	bzero(nxy,nz*sizeof(int));

	for(iz=0;iz<nz;iz++) {
		ii = iz*nx*ny;
		for(iy=0;iy<ny;iy++) {
			for(ix=0;ix<nx;ix++) {
				jj = ii + iy*nx + ix;
				if(counts[jj]>0) {
					nxy[iz] += 1;
					xs[jj] = xs[jj]/counts[jj];
					ys[jj] = ys[jj]/counts[jj];
					zs[jj] = zs[jj]/counts[jj];
					gs[jj] = gs[jj]/counts[jj];
				}
			}
		}
		nn = nxy[iz];
		fprintf(stderr," at depth iz=%d nxy=%d \n",iz,nxy[iz]);
		if(nn>0) {
			xx = (float*) emalloc(nn*sizeof(float));
			yy = (float*) emalloc(nn*sizeof(float));
			gg = (float*) emalloc(nn*sizeof(float));
			work = (float*) emalloc(nn*sizeof(float));
			indx = (int*) emalloc(nn*sizeof(int));
			kk = 0;
			for(iy=0;iy<ny;iy++) {
				for(ix=0;ix<nx;ix++) {
					jj = ii + iy*nx + ix;
					if(counts[jj]>0) {
						xx[kk] = xs[jj];
						yy[kk] = ys[jj];
						gg[kk] = gs[jj];
						kk = kk + 1;
					}
				}
			}

/* fprintf(stderr," iz=%d nxy=%d kk=%d \n",iz,nn,kk); */

			for(iy=0;iy<ny;iy++) {
				for(ix=0;ix<nx;ix++) {
					x = fx + ix*dx;
					y = fy + iy*dy;
					if(nf==0) {
						plint_(xx,yy,gg,&one,&kk,&x,&y,&g,work,&dismax,indx);
					} else {
						intp2d_(xx,yy,gg,&one,&kk,&x,&y,&g,&nf,indx,work);
					}
					grid[iz*nx*ny+iy*nx+ix] = g;
				}
			}
			free(xx);
			free(yy);
			free(gg);	
			free(work);	
			free(indx);	
		}

	}

	free(xs);
	free(ys);
	free(zs);
	free(gs);
	free(counts);

	zz = (int*) emalloc(nz*sizeof(int));

	nn = 0;
	for(iz=0;iz<nz;iz++) { 
		if(nxy[iz]>0) {
			zz[nn] =  iz;
			nn += 1;  
		}
	}

	fprintf(stderr," interpolate to missing %d z levels %d \n", nz-nn);
		
	for(iz=0;iz<nz;iz++) {
		jj = nxy[iz];
		if(jj<1) {
			if(iz<zz[0]) {
				kk = zz[0];
				for(iy=0;iy<ny;iy++)
				for(ix=0;ix<nx;ix++)
					grid[ix+iy*nx+iz*nx*ny] = grid[ix+iy*nx+kk*nx*ny];
			} else if(iz>zz[nn-1]) {
				kk = zz[nn-1];
				for(iy=0;iy<ny;iy++)
				for(ix=0;ix<nx;ix++)
					grid[ix+iy*nx+iz*nx*ny] = grid[ix+iy*nx+kk*nx*ny];
			} else {
				for(ii=0;ii<nn-1;ii++) {
					if(zz[ii]<iz && iz<zz[ii+1]) {
						kk = zz[ii];
					    ir = zz[ii+1];
						for(iy=0;iy<ny;iy++)
						for(ix=0;ix<nx;ix++)
							grid[ix+iy*nx+iz*nx*ny] = grid[ix+iy*nx+kk*nx*ny]
							 + (grid[ix+iy*nx+ir*nx*ny]-
								grid[ix+iy*nx+kk*nx*ny])/(zz[ii+1]-zz[ii])
							   *(iz-zz[ii]);
						break;

					}
				}
			}
		}
	}
	free(zz);
	free(nxy);

	gs = (float*) emalloc(nz*sizeof(float));
	bzero(gs,nz*sizeof(float));

	gmin = grid[0];
	gmax = grid[0];

	fprintf(stderr," start output \n"); 

	for(iy=0;iy<ny;iy++) {
		for(ix=0;ix<nx;ix++) {
			for(iz=0;iz<nz;iz++) {
				gs[iz] = grid[iz*nx*ny+iy*nx+ix];
				if(gmin>gs[iz]) gmin=gs[iz];
				if(gmax<gs[iz]) gmax=gs[iz];
			}
			fwrite(gs,sizeof(float),nz,outfp);
		}
	}
	
	free(grid);
	free(gs);

	fprintf(stderr," output header \n"); 
	
	bzero((char*)&usgh,GHDRBYTES);
	usgh.scale = 1.e-6;
	usgh.n1 = nz;
	usgh.n2 = nx;
	usgh.n3 = ny;
	usgh.d1 = dz;
	usgh.d2 = dx;
	usgh.d3 = dy;
	usgh.o1 = fz;
	usgh.o2 = fx;
	usgh.o3 = fy;
	usgh.dtype = 4;
	usgh.gmin = gmin;
	usgh.gmax = gmax;
	usgh.orient = orient;
	usgh.gtype = gtype;
	usgh.n4 = 1;
	usgh.n5 = 1;

	ierr = fputusghdr(outfp,&usgh);
	if(ierr!=0) err("error output grid header ");

	return 0;

}
