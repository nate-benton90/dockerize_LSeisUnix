#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDATLAYER - grid value extraction along a layer \n"
"\n"
"gridatlayer <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D attribute grid file (velocity)	\n"
"outfile=       name of the output 2D attribute grid file \n"
"layer=         name of the 2D depth grid defining the depth of the layer \n"
"Optional Parameters:\n"
"gmin=-999999   minimum value to output \n"
"gmax=999999    maximum value to output \n"
"intp=0         re-compute the output value when gmin>=g_out or gout>=gmax \n"
"               0=no will output the clipped value \n"
"               1=will interpolate from values gmin<g<gmax \n"
"               otherwise=will output the value of intp \n"
"dismax=999999  maximum distance (in grid points) of input grids \n" 
"               (gmin<g<gmax) to be used in interpolating an output \n"
"               point (when intp=1)		\n"
" Notes:						\n"
"   1.  The 1st and 2nd dimensions of the 2D layer grid file must match \n"
"       the 2nd and 3rd dimensions of the input 3D grid  \n"
"   2.  At least 3 points of input grids (gmin<g<gmax) will be used \n"
"       to interpolate an output grid (where g<=gmin or g>=gmax). \n"
"       If within a radius of dismax (center at the output grid), \n"
"       less than 3 points of input grids are found, the radius will \n"
"       be incresed by 50% and search will start again until 3 or more \n"
"       input grids are found. \n"
"\n"
"AUTHOR:  Zhiming Li,         2/12/2000			\n"
"\n";

void interp(float *xs,float *ys,float *vi,int nv,float x,float y,
	float *vo,float dismax);

int main(int argc, char **argv)
{
	usghed usghin, usghlayer;
	FILE *infp,*outfp,*layerfp;
	char *infile,*outfile,*layer;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1;

	float *grid, *zlayer, gmin, gmax;
	float *vlayer;
	float tmp;
	int ierr;
	int intp;
	float *xs,*ys,*vi,x,y,d2,d3;
	int nv, *ilive;
	float dismax;

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	ierr = fgetusghdr(infp,&usghin);
    	if(ierr!=0) err(" input grid header error ");
	if(getparstring("outfile",&outfile)) {
		outfp = efopen(outfile,"w");
	} else {
		outfp = stdout;
	}
	file2g(infp);
	file2g(outfp);

	if (getparstring("layer",&layer)) {
		layerfp = efopen(layer,"r");
		ierr = fgetusghdr(layerfp,&usghlayer);
      		if(ierr!=0) err(" layer grid header error ");
	} else {
		err(" layer missing ");
	}

	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	d2 = 1.;
	d3 = 1.;

	if(!getparfloat("gmin",&gmin)) gmin=-999999.;
	if(!getparfloat("gmax",&gmax)) gmax=999999.;
	if(!getparfloat("dismax",&dismax)) dismax=999999.;
	if(!getparint("intp",&intp)) intp=0;

	/* memory allocations */
	zlayer = (float*) emalloc(n2*n3*sizeof(float));
	vlayer = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	xs = (float*) emalloc(n2*n3*sizeof(float));
	ys = (float*) emalloc(n2*n3*sizeof(float));
	vi = (float*) emalloc(n2*n3*sizeof(float));
	ilive = (int*) emalloc(n2*n3*sizeof(int));
	
	if(usghin.n2!=usghlayer.n1) err("check layer header n1");
	if(usghin.n3!=usghlayer.n2) err("check layer header n2");
	if(usghin.o2!=usghlayer.o1) err("check layer header o1");
	if(usghin.o3!=usghlayer.o2) err("check layer header o2");
	if(usghin.d2!=usghlayer.d1) err("check layer header d1");
	if(usghin.d3!=usghlayer.d2) err("check layer header d2");
	efseek(layerfp,0,0);
	efread(zlayer,sizeof(float),n2*n3,layerfp);

/* compute grid values at the layer */
	efseek(infp,0,0);
	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			efread(grid,sizeof(float),n1,infp);
			tmp = (zlayer[i2+i3*n2] - o1)/d1 + 0.5; 
			i1 = tmp;
			if(i1<0) {
				vlayer[i2+i3*n2] = grid[0];
			} else if(i1>=n1-1) {
				vlayer[i2+i3*n2] = grid[n1-1];
			} else {
				vlayer[i2+i3*n2] = grid[i1]+
					(tmp-i1)*(grid[i1+1]-grid[i1]);
			}
		}
	}

/* clip or interpolate grid values for g<=gmin or g>=gmax */
	if(gmin!=-999999. || gmax!=999999.) {
		nv = 0;
		for (i3=0;i3<n3;i3++) {
		for (i2=0;i2<n2;i2++) {
			ilive[i2+i3*n2] = 0;
			if(vlayer[i2+i3*n2]>gmin && vlayer[i2+i3*n2]<gmax ) {
				ilive[i2+i3*n2] = 1;
				vi[nv] = vlayer[i2+i3*n2];
				xs[nv] = i2*d2;
				ys[nv] = i3*d3;
				nv = nv + 1;
			}
		}
		}
		for (i3=0;i3<n3;i3++) {
		for (i2=0;i2<n2;i2++) {
			if(ilive[i2+i3*n2]==0) {
				if(intp!=1 || nv==0) {
					if(intp==0 || intp==1) {
					if(vlayer[i2+i3*n2]<gmin) {
						vlayer[i2+i3*n2]=gmin;
					} else if(vlayer[i2+i3*n2]>gmax) {
						vlayer[i2+i3*n2]=gmax;
					}
					} else { 
						vlayer[i2+i3*n2]=intp;
					}
				} else {
					x = i2*d2;
					y = i3*d3;
					interp(xs,ys,vi,nv,x,y,&tmp,dismax);
					vlayer[i2+i3*n2] = tmp;
				}
			}
		}
		}
	}

	gmin = vlayer[0];
	gmax = vlayer[0];
	for(i2=0;i2<n2*n3;i2++) {
		if(gmin>vlayer[i2]) gmin = vlayer[i2];
		if(gmax<vlayer[i2]) gmax = vlayer[i2];
	}

	usghlayer.gmin = gmin;
	usghlayer.gmax = gmax;

	efwrite(vlayer,sizeof(float),n2*n3,outfp);
	ierr = fputusghdr(outfp,&usghlayer);
	
	free(zlayer);
	free(vlayer);
	free(grid);
	exit(0);
}

void interp(float *xs,float *ys,float *vi,int nv,float x,float y,
float *vo,float dismax) {
	int i;
	float norm, *scale; 
	int nnv;
	float tmp;
	float *vii;
	int *indx;
	float *dis;

	dis = (float*) emalloc(nv*sizeof(float));
	vii = (float*) emalloc(nv*sizeof(float));
	indx = (int*) emalloc(nv*sizeof(int));
	scale = (float*) emalloc(nv*sizeof(float));

	for(i=0;i<nv;i++) {
		dis[i] = sqrt( (x-xs[i])*(x-xs[i])+(y-ys[i])*(y-ys[i]) );
		indx[i] = i;
	} 
	qkisort (nv, dis, indx);

	norm = 0.;
	nnv = 0;
	for(i=0;i<nv;i++) {
		tmp = dis[indx[i]];
		if(tmp<dismax) {
			scale[nnv] = 1./tmp;
			norm = norm + scale[nnv];
			vii[nnv] = vi[indx[i]];
			nnv = nnv + 1;
		} else {
			if(nnv<3 && nv>=3) {
				scale[nnv] = 1./tmp;
				norm = norm + scale[nnv];
				vii[nnv] = vi[indx[i]];
				nnv = nnv + 1;
			} else {
				break;
			}
		}
	}	

	*vo = 0.;
	for(i=0;i<nnv;i++) {
		*vo = *vo + vii[i]*scale[i]/norm;
	}

	free(indx);
	free(dis);
	free(scale);
	free(vii);
}
