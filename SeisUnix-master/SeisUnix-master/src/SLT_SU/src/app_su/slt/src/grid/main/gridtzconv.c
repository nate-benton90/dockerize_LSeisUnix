/* GRIDCLIP time/depth conversion program */
#include "usgrid.h"

char *sdoc = 
"GRIDTZCONV - grid time/depth conversion  program				\n"
"\n"
"gridtzconv [parameters] <grid.input >grid.output			\n" 
"\n"
"Required parameters:							\n"
"fout=        first output time or depth 			\n"
"dout=        sampling interval of output time or depth \n"
"nout=        number of output time or depth  samples per trace \n"
"vagrid=      name of time-average velocity grid \n"
"Optional parameters:							\n"
"torz=0       0=time to depth; 1=depth to time  \n"
"Notes:									\n"
" 1. The spatial locations of the time-average velocity grid are defined \n"
"    by o2, o3, d2 and d3 in the time-average grid header \n"
" 2. The spatial locations of the input grid are defined \n"
"    by o2, o3, d2 and d3 in the input grid header \n"
" 3. Input average velocity is ALWAYS time-average velocity \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/11/98   		\n"    
;

void bilint_(int *n1,int *nx,int *ny, float *x0,float *y0,
	float *dx,float *dy,float *x,float *y,float *vs,float *v);

void lin1dn_(float *xin,float *yin,int *nin,float *xo,float *yo,
	int *nout,int *indx,float *dydx0,float *dydxn);

int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;
	FILE *vafp;
	char *vagrid;
	float fout, dout;
	int nout;
	int torz, ierr;
	float *va, *v;
	float *z, *t;
	float *zo, *to, *trace, *grido;
	float dzdt0, dzdtn, dtdz0, dtdzn, tmp, x, y, res;
	int disk, itmp; 
	int i1, i2, i3, iti, izi;
	float zi, ti, gmin, gmax;
	int *indx;

	float o1v, o2v, o3v;
	float d1v, d2v, d3v;
	int n1v, n2v, n3v;

	float o1, o2, o3;
	float d1, d2, d3;
	int n1, n2, n3;

	usghed usgh;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	if(!getparfloat("fout",&fout)) err(" fout missing ");
	if(!getparfloat("dout",&dout)) err(" dout missing ");
	if(!getparint("nout",&nout)) err(" nout missing ");
	if(!getparint("torz",&torz)) torz=0;
	if(!getparstring("vagrid",&vagrid)) err(" vagrid missing ");
	vafp = efopen(vagrid,"r");

	/* get input grid parameters */
	ierr = fgetusghdr(vafp,&usgh);
    if(ierr!=0) err(" nonstandard vagrid file ");
	n1v = usgh.n1;
    n2v = usgh.n2;
    n3v = usgh.n3;
	o1v = usgh.o1;
    o2v = usgh.o2;
    o3v = usgh.o3;
	d1v = usgh.d1;
    d2v = usgh.d2;
    d3v = usgh.d3;
	if(n2v==0) n2v=1;
	if(n3v==0) n3v=1;
	disk = 0;
	if(o2v==o2 && o3v==o3 && d2v==d2 && d3v==d3 && n2v==n2 && n3v==n3) disk=1;

	ierr = fgetusghdr(infp,&usgh);
    if(ierr!=0) err(" nonstandard input file ");
	n1 = usgh.n1;
    n2 = usgh.n2;
    n3 = usgh.n3;
	o1 = usgh.o1;
    o2 = usgh.o2;
    o3 = usgh.o3;
	d1 = usgh.d1;
    d2 = usgh.d2;
    d3 = usgh.d3;
	efseek(infp,0,0);
	if(n2==0) n2=1;
	if(n3==0) n3=1;

	trace = (float*) emalloc(n1*sizeof(float));
	if(disk==0) va = (float*) emalloc(n1v*n2v*n3v*sizeof(float));
	v = (float*) emalloc(n1v*sizeof(float));
	t = (float*) emalloc(n1v*sizeof(float));
	z = (float*) emalloc(n1v*sizeof(float));

	to = (float*) emalloc(nout*sizeof(float));
	zo = (float*) emalloc(nout*sizeof(float));
	grido = (float*) emalloc(nout*sizeof(float));
	indx = (int*) emalloc(nout*sizeof(int));


	for(i1=0;i1<n1v;i1++) t[i1] = o1v + i1*d1v; 
	if(torz==0) {
		for(i1=0;i1<nout;i1++) zo[i1] = fout + i1*dout; 
	} else {
		for(i1=0;i1<nout;i1++) to[i1] = fout + i1*dout; 
	}

	efseek(vafp,0,0);
	if(disk==0) efread(va,sizeof(float),n1v*n2v*n3v,vafp);

	for(i3=0;i3<n3;i3++) {
	for(i2=0;i2<n2;i2++) {
		x = o2 + i2*d2;
		y = o3 + i3*d3;
		if(disk==0) {
			bilint_(&n1v,&n2v,&n3v,&o2v,&o3v,&d2v,&d3v,&x,&y,va,v);
		} else {
			x = (x-o2v)/d2v + 0.5;
			y = (y-o3v)/d3v + 0.5;
			itmp = x;
			i1 = y;
			itmp = (itmp+i1*n2v)*n1v*sizeof(float);
			efseek(vafp,itmp,0); 
			efread(v,sizeof(float),n1v,vafp);
		}
		for(i1=0;i1<n1v;i1++) z[i1] = (o1v+d1v*i1)*v[i1]*0.0005;

		efread(trace,sizeof(float),n1,infp);


		if(torz==0) {
			dzdt0 = (z[1] - z[0])/d1v;
			dzdtn = (z[n1v-1] - z[n1v-2])/d1v;
			lin1dn_(z,t,&n1v,zo,to,&nout,indx,&dzdt0,&dzdtn);
			for(i1=0;i1<nout;i1++) {
				ti = (to[i1] - o1)/d1;
				iti = ti;
				res = ti - iti;
				if(iti<0) {
					grido[i1] = trace[0];
				} else if(iti>n1-1) {
					grido[i1] = trace[n1-1];
				} else if(iti==n1-1) {
					grido[i1] = trace[n1-1];
				} else {
					grido[i1] = trace[iti]*(1.-res)+res*trace[iti+1];
				}
			}
		} else { 
			dtdz0 = d1v/(z[1] - z[0]);
			dtdzn = d1v/(z[n1v-1] - z[n1v-2]);
			lin1dn_(t,z,&n1v,to,zo,&nout,indx,&dtdz0,&dtdzn);
			for(i1=0;i1<nout;i1++) {
				zi = (zo[i1] - o1)/d1;
				izi = zi;
				res = zi - izi;
				if(izi<0) {
					grido[i1] = trace[0];
				} else if(izi>n1-1) {
					grido[i1] = trace[n1-1];
				} else if(izi==n1-1) {
					grido[i1] = trace[n1-1];
				} else {
					grido[i1] = trace[izi]*(1.-res)+res*trace[izi+1];
				}
			}
		}

		if(i2==0 && i3==0) { gmin=grido[0]; gmax=grido[0]; }
		for(i1=0;i1<nout;i1++) {
			if(gmin>grido[i1]) gmin=grido[i1];
			if(gmax<grido[i1]) gmax=grido[i1];
		}
		efwrite(grido,sizeof(float),nout,outfp);

	}
	}
	
	usgh.o1 = fout;
	usgh.d1 = dout;
	usgh.n1 = nout;
	usgh.gmin = gmin;
	usgh.gmax = gmax;

	ierr = fputusghdr(outfp,&usgh);
    if(ierr!=0) err("error in output gridheader");


	free(trace);
	if(disk==0) free(va);
	free(v);
	free(grido);
	free(indx);
	free(z);
	free(t);

	exit(0);
}
