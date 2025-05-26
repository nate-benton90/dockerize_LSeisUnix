/* velocity grid to VS3D card conversion */

#include "velo.h"
#include "usgrid.h"
#include "comva.h"
#include "par.h"


char *sdoc = 
"VGRID2VS3D - convert velocity grid to VS3D cards  		\n"
"\n"
"vgrid2vs3d [parameters] < vgrid  >vs3d-cards 				\n" 
"\n"
"Required parameters:						 	\n"
"vgrid=                Name of velocity grid 				\n"
"vs3d-cards=           Name of dataset to output VS3D cards 		\n"
"\n"
"Optional parameters:							\n"
"fx=fxvgrid            Minimum inline coordinate of output VS3D card	\n"
"                      (default: the minimum inline coordinate in vgrid) \n"
"dx=dxvgrid            Inline coordinate increment of output VS3D cards	\n"
"                      (default: increment of inline coordinate in vgrid) \n"
"nx=1                  number of inline locations to output VS3D cards	\n"
"fy=fyvgrid            Minimum crossline coordinate of output VS3D card	\n"
"                      (default: the minimum crossline coordinate in vgrid)\n"
"dy=dyvgrid            Crossline coordinate increment of output VS3D cards\n"
"                      (default: increment of crossline coordinate in vgrid)\n"
"ny=1                  number of crossline locations to output VS3D cards \n"
"ft=ftvgrid            First time/depth (in ms/m or ft) to output VS3D cards \n"
"dt=10*dtvgrid         Time/depth interval (in ms/m or ft) to output VS3D \n" 
"nt=ntvgrid/10         Number of times/depths to output VS3D		\n"
"ivtype=0              Input velocity grid type (0=rms; 1=avg; 2=int)	\n"
"ovtype=0              Output VS3D velocity type (0=rms; 1=avg; 2=int)\n"
"ittype=0              Input velocity grid time/depth type		\n" 
"                      (0=time 1=depth)					\n"
"ottype=0              Output VS3D time/depth type (0=time 1=depth)	\n"
"template=             template name of desired output (t,x,y) locations \n"
"                      in VS3D cards format				\n" 
"                      (when this parameter is supplied, output VS3D cards \n"
"                      will be the same (t,x,y) as those defined in template,\n"
"                      (fx,dx,nx,fy,dy,ny,ft,dt,nt) will be ignored)	\n" 
"nvfmax=4096           maximum number of velocity functions in template \n"
"ntvmax=256            maximum number of t-v pairs per velocity functions   \n"
"                      in template   \n"
"\n"
"Notes:									\n"
" 1. Input velocity grid must be a standard grid file, i.e., with	\n"
"    grid header.							\n"
" 2. Input vgrid file must be (time/depth, x, y) ordered. Otherwise  	\n"
"    use gridtrsp to convert to this order				\n"
" 3. There are nx times ny velocity functions output as VS3D cards   	\n"
"\n"
"AUTHOR:         Zhiming Li,       ,	7-12-1994   	\n";

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;

	int nxvgrid,ntvgrid,nyvgrid,nx,nt,ny;
	float fxvgrid,dxvgrid,ftvgrid,dtvgrid,tmp;
	float fx,dx,ft,dt,fy,dy;
	int ivtype,ovtype;
	int ittype,ottype;

    	float *tin, *tout, *vin, *vout, *to, *vo;
	float *times, *vs; 
	float x, y, o1, o2, o3, d1, d2, d3; 
	int it, i2vgrid, i3vgrid; 
	int i2, i3, n1,n2, n3;

	usghed usgh;
	int ierr;
	float vmax, vmin; 

	char *template;
	FILE *tmpfp;
	int nvfmax, ntvmax;
	float *xs, *ys, *tpicks, *vpicks;
	int *nps, n11, n22, nxy, i, nvs, i1;

    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

	ierr = fgetusghdr(infp,&usgh);
	if(ierr!=0) err(" input vgrid header error ");
	o1 = usgh.o1;
	o2 = usgh.o2;
	o3 = usgh.o3;
	d1 = usgh.d1;
	d2 = usgh.d2;
	d3 = usgh.d3;
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3;


	if (!getparfloat("fx",&fx)) fx = o2;
	if (!getparfloat("dx",&dx)) dx = d2;
	if (!getparint("nx",&nx)) nx = 1;
	if (!getparfloat("fy",&fy)) fy = o3;
	if (!getparfloat("dy",&dy)) dy = d3;
	if (!getparint("ny",&ny)) ny = 1;
	if (!getparfloat("ft",&ft)) ft = o1;
	if (!getparfloat("dt",&dt)) dt = 10. * d1;
	if (!getparint("nt",&nt)) nt = n1/10;
	if (!getparint("ivtype",&ivtype)) ivtype=0;
	if (!getparint("ovtype",&ovtype)) ovtype=0;
	if (!getparint("ittype",&ittype)) ittype=0;
	if (!getparint("ottype",&ottype)) ottype=0;


    	tin = (float*)malloc(n1*sizeof(float));
    	vin = (float*)malloc(n1*sizeof(float));
    	to = (float*)malloc(n1*sizeof(float));
    	vo = (float*)malloc(n1*sizeof(float));
    	tout = (float*)malloc(nt*sizeof(float));
    	vout = (float*)malloc(nt*sizeof(float));
    	times = (float*)malloc(nt*sizeof(int));
    	vs = (float*)malloc(nt*sizeof(int));

	for(it=0;it<n1;it++) {
		tin[it] = o1 + it*d1;
		to[it] = tin[it];
	}
	for(it=0;it<nt;it++) tout[it] = ft + it*dt;

	if(getparstring("template",&template)) {
		tmpfp = efopen(template,"r");
		if (!getparint("nvfmax",&n22)) n22 = 4096;
        	if (!getparint("ntvmax",&n11)) n11 = 256;

        	/* arrays used to store all VS3D card's x,y,time and velocity */
        	xs = (float*)emalloc(n22*sizeof(float));
        	ys = (float*)emalloc(n22*sizeof(float));
        	tpicks = (float*)emalloc(n11*n22*sizeof(float));
        	vpicks = (float*)emalloc(n11*n22*sizeof(float));
        	nps = (int*)emalloc(n22*sizeof(int));

		nxy = 0;
        	vs3dread(tmpfp,xs,ys,tpicks,vpicks,&nxy,nps,n11,n22);
        	fprintf(stderr," %d VS3D cards read from template:%s\n",
			nxy,template);

		for(i=0;i<nxy;i++) {
			y = ys[i];
			tmp = (y - o3)/d3;
			i3vgrid = (int) tmp;
			if(i3vgrid<0) i3vgrid=0;
			if(i3vgrid>=n3) i3vgrid=n3-1;
			x = xs[i];
			tmp = (x - o2)/d2;
			i2vgrid = (int) tmp;
			if(i2vgrid<0) i2vgrid=0;
			if(i2vgrid>=n2)  i2vgrid=n2-1;
			efseek(infp,
				(i2vgrid+i3vgrid*n2)*n1*sizeof(float),0);
			efread(vin,sizeof(float),n1,infp);
			/* time/depth conversion if needed */
			vconvert(tin,vin,n1,ivtype,ittype,
				to,vo,n1,ovtype,ottype);
		
			nvs = nps[i];
			free(tout);
			free(vout);
			free(times);
			free(vs);
			tout = (float*) emalloc(nvs*sizeof(float));
			vout = (float*) emalloc(nvs*sizeof(float));
			times = (float*) emalloc(nvs*sizeof(int));
			vs = (float*) emalloc(nvs*sizeof(int));

			for(i1=0;i1<nvs;i1++)
				tout[i1] = tpicks[i*n11+i1]; 

			/* interpolate to desired output time/depth */
			lin1d_(to,vo,&n1,tout,vout,&nvs,vs);

			for(it=0;it<nvs;it++) {
				times[it] = tout[it] + 0.5;
				vs[it] = vout[it] + 0.5;
			}
			/* output the VS3D cards */
			printvs3d(x,y,nvs,times,vs,outfp);

	fprintf(stderr,"Output VS3D at x=%f y=%f ixgrid=%d iygrid=%d \n",
			x,y,i2vgrid+1,i3vgrid+1);
		}

		
		exit(0);
	}

		

	for(i3=0;i3<ny;i3++) {
		y = fy+i3*dy;
		tmp = (y - o3)/d3;
		i3vgrid = (int) tmp;
		if(i3vgrid<0) i3vgrid=0;
		if(i3vgrid>=n3) i3vgrid=n3-1;
	for(i2=0;i2<nx;i2++) {
		x = fx+i2*dx;
		tmp = (x - o2)/d2;
		i2vgrid = (int) tmp;
		if(i2vgrid<0) i2vgrid=0;
		if(i2vgrid>=n2)  i2vgrid=n2-1;

		efseek(infp,
			(i2vgrid+i3vgrid*n2)*n1*sizeof(float),0);
		efread(vin,sizeof(float),n1,infp);
		/* time/depth conversion if needed */
		vconvert(tin,vin,n1,ivtype,ittype,
				to,vo,n1,ovtype,ottype);
		/* interpolate to desired output time/depth */
		lin1d_(to,vo,&n1,tout,vout,&nt,vs);

		for(it=0;it<nt;it++) {
			times[it] = tout[it] + 0.5;
			vs[it] = vout[it] + 0.5;
		}
		/* output the VS3D cards */
		printvs3d(x,y,nt,times,vs,outfp);

	fprintf(stderr,"Output VS3D at x=%f y=%f ixgrid=%d iygrid=%d \n",
		x,y,i2vgrid+1,i3vgrid+1);
	}
	}
	
	exit(0);
}
