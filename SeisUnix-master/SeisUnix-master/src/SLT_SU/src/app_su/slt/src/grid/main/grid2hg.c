char *sdoc =
"GRID2HG - 3D grid to horizons grid conversion \n"
"\n"
"grid2hg <infile [required parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D grid file	(velocity)	\n"
"gridval=       grid value to define where the 3D body is				\n"
"top=           name of the 2D depth grid defining the top of the 3D body \n"
"ovtop=         name of the 2D depth grid defining the overhang top \n"
"ovbot=         name of the 2D depth grid defining the overhang bottom \n"
"bottom=        name of the 2D depth grid defining the bottom of the 3D body\n"
"nullval=       null value to define absence of the 3D body	\n"
"Optional Parameters:\n"
"tol=0.01       tolerance value where |gridval-grid|<tol is within the 3D \n"
"               body \n"

"\n"
"AUTHOR:  Zhiming Li,         9/5/96			\n"
"\n";
#include "usgrid.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed ugh;
	FILE *infp,*topfp,*botfp,*ovtfp,*ovbfp;
	char *infile,*top,*bottom,*ovtop,*ovbot;
	float *ztop, *zbot, *zovt, *zovb, *grid;
	float gridval, nullval, tol;
	int ierr;
	int itop, iovt, iovb, ibot;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,d2,d3;
	float o1,o2,o3;
	float gmin, gmax;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	ierr = fgetusghdr(infp,&ugh);
    if(ierr!=0) err(" input grid header error ");

	if(getparstring("top",&top)) {
		topfp = efopen(top,"w");
	} else {
		err(" top file missing");
	}
	if(getparstring("ovtop",&ovtop)) {
		ovtfp = efopen(ovtop,"w");
	} else {
		err(" overhang top file missing");
	}
	if(getparstring("ovbot",&ovbot)) {
		ovbfp = efopen(ovbot,"w");
	} else {
		err(" overhang bottom file missing");
	}
	if(getparstring("bottom",&bottom)) {
		botfp = efopen(bottom,"w");
	} else {
		err(" bottom file missing");
	}
	if(!getparfloat("gridval",&gridval)) err(" gridval missing");
	if(!getparfloat("nullval",&nullval)) err(" nullval missing");
	if(!getparfloat("tol",&tol)) tol = 0.01;


	n1 = ugh.n1;
	n2 = ugh.n2;
	n3 = ugh.n3;
	o1 = ugh.o1;
	o2 = ugh.o2;
	o3 = ugh.o3;
	d1 = ugh.d1;
	d2 = ugh.d2;
	d3 = ugh.d3;

	/* memory allocations */
	ztop = (float*) emalloc(n2*n3*sizeof(float));
	zbot = (float*) emalloc(n2*n3*sizeof(float));
	zovb = (float*) emalloc(n2*n3*sizeof(float));
	zovt = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(grid,sizeof(float),n1,infp);

			itop = -1;
			iovt = -1;
			iovb = -1;
			ibot = -1;
			ztop[i2+i3*n2] = nullval;
			zovt[i2+i3*n2] = nullval;
			zovb[i2+i3*n2] = nullval;
			zbot[i2+i3*n2] = nullval;

			for(i1=0;i1<n1;i1++) {
				if(fabs(grid[i1]-gridval)<tol) {
					itop = i1;
					break;
				}
			}
			for(i1=itop+1;i1<n1;i1++) {
				if(fabs(grid[i1-1]-gridval)<tol &&
				   fabs(grid[i1]-gridval)>tol) {
				   iovt = i1-1;
				   break;
				}

			}
			for(i1=iovt+1;i1<n1;i1++) {
				if(fabs(grid[i1-1]-gridval)>tol &&
				   fabs(grid[i1]-gridval)<tol) {
				   iovb = i1;
				   break;
				}
			}
			for(i1=iovb+1;i1<n1;i1++) {
				if(fabs(grid[i1-1]-gridval)<tol &&
				   fabs(grid[i1]-gridval)>tol) {
				   ibot = i1-1;
				   break;
				}
			}
			if(fabs(grid[n1-1]-gridval)<tol) ibot=n1-1;

			if(itop>=0 && iovt>=0 && iovb<0 && ibot<0)  {
				ibot = iovt;
				iovt = -1;
			}

		    if(itop>=0) ztop[i2+i3*n2] = o1+itop*d1;
		    if(iovt>=0) zovt[i2+i3*n2] = o1+iovt*d1;
		    if(iovb>=0) zovb[i2+i3*n2] = o1+iovb*d1;
		    if(ibot>=0) zbot[i2+i3*n2] = o1+ibot*d1;
		}
	}

	ugh.o1 = o2;
	ugh.o2 = o3;
	ugh.d1 = d2;
	ugh.d2 = d3;
	ugh.n1 = n2;
	ugh.n2 = n3;
	ugh.o3 = 0.;
	ugh.d3 = 0.;
	ugh.n3 = 1;

	n1 = n2  * n3;

	fminmax(ztop,n1,&gmin,&gmax);
	ugh.gmax = gmax;
	ugh.gmin = gmin;
	efwrite(ztop,sizeof(float),n1,topfp);
	ierr = fputusghdr(topfp,&ugh);
	if(ierr!=0) err(" error output top file");
	efclose(topfp);
	fminmax(zovt,n1,&gmin,&gmax);
	ugh.gmax = gmax;
	ugh.gmin = gmin;
	efwrite(zovt,sizeof(float),n1,ovtfp);
	ierr = fputusghdr(ovtfp,&ugh);
	efclose(ovtfp);
	if(ierr!=0) err(" error output overhang top file");
	fminmax(zovb,n1,&gmin,&gmax);
	ugh.gmax = gmax;
	ugh.gmin = gmin;
	efwrite(zovb,sizeof(float),n1,ovbfp);
	ierr = fputusghdr(ovbfp,&ugh);
	if(ierr!=0) err(" error output overhang bottom file");
	efclose(ovbfp);
	fminmax(zbot,n1,&gmin,&gmax);
	ugh.gmax = gmax;
	ugh.gmin = gmin;
	efwrite(zbot,sizeof(float),n1,botfp);
	ierr = fputusghdr(botfp,&ugh);
	if(ierr!=0) err(" error output bottom file");
	efclose(botfp);
	

	
	exit(0);
}
