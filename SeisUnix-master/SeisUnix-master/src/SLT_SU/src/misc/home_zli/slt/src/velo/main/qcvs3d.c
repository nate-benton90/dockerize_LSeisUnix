/* quality control of velocity 3d cards */

#include "velo.h"
#include "usgrid.h"
#include "ghdr.h"
#include "gridhd.h"
#include "comva.h"
#include "par.h"



char *sdoc = 
"QCVS3D - quality control display of VS3D cards and volocity grids \n"
"\n"
"qcvs3d [parameters] xqc= yqc=					\n" 
"\n"
"Required parameters:						 	\n"
"xqc=              x coordinate (inline) of VS3D to qc 			\n"
"yqc=              y coordinate (crossline) of VS3D to qc 		\n"
"\n"
"Optional parameters:							\n"
"vs3d1=            Name of first VS3D dataset to qc 			\n"
"vs3d2=            Name of second VS3D dataset to qc			\n"
"vs3d3=            Name of third VS3D dataset to qc			\n"
"dt=100            time interval in ms to interpolate input t-v pairs   \n"
"                  before converting to interval velocity 		\n"
"                  =0 will compute interval velocity at input times	\n"
"vstype1=0         Velocity type of VS3D card (0=rms 1=avg 2=interval)	\n"
"vstype2=0         Velocity type of VS3D card (0=rms 1=avg 2=interval)	\n"
"vstype3=0         Velocity type of VS3D card (0=rms 1=avg 2=interval)	\n"
"vgrid1=           Name of 1st 3D interval velocity grid stored as v(t,x,y) \n"
"                  i.e., stored as velocity vectors not time slices 	\n"
"vgtype1=2         Velocity type of 1st vgrid (0=rms 1=average 2=interval)\n"
"vgrid2=           Name of 2nd 3D interval velocity grid stored as v(t,x,y) \n"
"                  i.e., stored as velocity vectors not time slices 	\n"
"vgtype2=2         Velocity type of 2nd vgrid (0=rms 1=average 2=interval)\n"
"vgrid3=           Name of 3rd 3D interval velocity grid stored as v(t,x,y) \n"
"                  i.e., stored as velocity vectors not time slices 	\n"
"vgtype3=2         Velocity type of 3rd vgrid (0=rms 1=average 2=interval) \n"
"paper=0           Paper plot (1=yes 0=screen)				\n"
"vauxplot=0        Plot other velocity curve (0=no 1=rms 2=avg)		\n"
"viplot=1          Plot interval velocity curves (0=no 1=yes)	\n"
"nvfmax=4096       maximum number of velocity functions each vs3d data	\n"
"                  set may have						\n"
"ntvmax=256        maximum number of t-v pairs per velocity function in \n"
"                  vs3d datasets					\n"
"tmax=             maximum time to disply (default to data)		\n"
"title=            title of the plot							\n"
"Note:									\n"
" 1. see vgrid3d for the format of VS3D					\n"
" 2. velocity curve from vs3d1 will be ploted with red color	\n"
"    velocity curve from vs3d2 will be ploted with green color	\n"
"    velocity curve from vs3d3 will be ploted with dark blue color\n"
"    velocity curve from vgrid1 will be ploted with light blue color\n"
"    velocity curve from vgrid2 will be ploted with violet color\n"
"    velocity curve from vgrid3 will be ploted with yellow color\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,			9/14/93		\n"    
;

void times(float dt, float *t, int ntv, float *time, int *nt, float tmax);

void vrms2vint(float *t, float *v, int ntv, float *time, float *vi, int nt);

void vavg2vint(float *t, float *v, int ntv, float *time, float *vi, int nt);

void vint2vrms(float *t, float *vi, float *vrms, int nt);

void vint2vavg(float *t, float *vi, float *va, int nt);

void vrmsout(FILE *datafp, float *ts, float *vs, float *work, int ns);

void vavgout(FILE *datafp, float *ts, float *vs, float *work, int ns);

void findvf(char *in, float xqc, float yqc, float *t, float *v, int *np, 
	float *xx, float *yy, int ntvmax, int nvfmax); 

main(int argc, char **argv)
{
    	FILE *vfp1, *vfp2, *vfp3;
	FILE *datafp;

	char *in1, *in2, *in3, *vgrid1, *vgrid2, *vgrid3;
	char cname[80];
	int i1, i2, i3, iv1, iv2, iv3, it;
	float xqc, yqc, dt; 
	int vstype1, vstype2, vstype3, vgtype1, vgtype2, vgtype3, nt;
	int paper, vauxplot, ia, ii, viplot;

    	float *time, *work;
	float *t1s, *t2s, *t3s, *t4s, *t5s, *t6s;
	float *v1s, *v2s, *v3s, *v4s, *v5s, *v6s;
	int n1s, n2s, n3s, n4s, n5s, n6s;

	float *t1, *v1, *t2, *v2, *t3, *v3, *t4, *v4, *t5, *v5, *t6, *v6; 
	float x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6;
	int np1=0, np2=0, np3=0, np4=0, np5=0, np6=0;
	int ntmax=4096;
	float tmp, tmax;
	int ix, iy, iseek;
	char cmd[1024];
	int ntvmax,nvfmax;
	char *title;
	

	float x4min,x4max,y4min,y4max;
	float x5min,x5max,y5min,y5max;
	float x6min,x6max,y6min,y6max;

	usghed usgh;


	
	initargs(argc,argv);
        askdoc(1);

	/* required parameters */
	if (!getparfloat("xqc",&xqc)) err(" xqc missing ");
	if (!getparfloat("yqc",&yqc)) err(" yqc missing ");

	/* optional parameters */

	if (!getparstring("vs3d1",&in1)) {
		i1 = 0;
	} else {
		i1 = 1;
	}

	if (!getparstring("vs3d2",&in2)) {
		i2 = 0; 
	} else {
		i2 = 1;
	}

	if (!getparstring("vs3d3",&in3)) {
		i3 = 0; 
	} else {
		i3 = 1;
	}

	if (!getparstring("vgrid1",&vgrid1)) {
		iv1 = 0; 
	} else {
		iv1 = 1;
		vfp1 = efopen(vgrid1,"r");
	}
	if (!getparstring("vgrid2",&vgrid2)) {
		iv2 = 0; 
	} else {
		iv2 = 1;
		vfp2 = efopen(vgrid2,"r");
	}
	if (!getparstring("vgrid3",&vgrid3)) {
		iv3 = 0; 
	} else {
		iv3 = 1;
		vfp3 = efopen(vgrid3,"r");
	}

	if (!getparfloat("dt",&dt)) dt = 100.;
	if (!getparint("vgtype1",&vgtype1)) vgtype1 = 2;
	if (!getparint("vgtype2",&vgtype2)) vgtype2 = 2;
	if (!getparint("vgtype3",&vgtype3)) vgtype3 = 2;
	if (!getparint("vstype1",&vstype1)) vstype1 = 0;
	if (!getparint("vstype2",&vstype2)) vstype2 = 0;
	if (!getparint("vstype3",&vstype3)) vstype3 = 0;
	if (!getparint("paper",&paper)) paper = 0;
	if (!getparint("vauxplot",&vauxplot)) vauxplot = 0;
	if (!getparint("viplot",&viplot)) viplot = 1;
	ia = vauxplot;
	if(ia==2) ia = 1;
	ii = viplot;
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparfloat("tmax",&tmax)) tmax = 100000.;
	if (!getparstring("title",&title)) {
		title = (char*) malloc(30*sizeof(char));
		strcpy(title,"velocity qc ");
	}		

	time = (float*) malloc(ntmax*sizeof(float));
	work = (float*) malloc(4*ntmax*sizeof(float));
	t1 = (float*) malloc(ntmax*sizeof(float));
	t2 = (float*) malloc(ntmax*sizeof(float));
	t3 = (float*) malloc(ntmax*sizeof(float));
	t4 = (float*) malloc(ntmax*sizeof(float));
	t5 = (float*) malloc(ntmax*sizeof(float));
	t6 = (float*) malloc(ntmax*sizeof(float));
	v1 = (float*) malloc(ntmax*sizeof(float));
	v2 = (float*) malloc(ntmax*sizeof(float));
	v3 = (float*) malloc(ntmax*sizeof(float));
	v4 = (float*) malloc(ntmax*sizeof(float));
	v5 = (float*) malloc(ntmax*sizeof(float));
	v6 = (float*) malloc(ntmax*sizeof(float));

	/* read in t, v at closest location of data in1 */

	n1s = 0;
	if(i1==1) {
		findvf(in1, xqc, yqc, t1, v1, &np1, &x1, &y1, ntvmax, nvfmax); 
		times(dt, t1, np1, time, &nt, tmax);
		t1s = (float*) malloc(nt*sizeof(float));
		v1s = (float*) malloc(nt*sizeof(float));
		bcopy(time,t1s,nt*sizeof(float));
		bcopy(v1,v1s,nt*sizeof(float));
		if(vstype1==0) {
			vrms2vint(t1, v1, np1, t1s, v1s, nt);
		} else if(vstype1==1) {
			vavg2vint(t1, v1, np1, t1s, v1s, nt);
		}
		n1s = nt;
	}

	n2s = 0;
	if(i2==1) {
		findvf(in2, xqc, yqc, t2, v2, &np2, &x2, &y2, ntvmax, nvfmax); 
		times(dt, t2, np2, time, &nt, tmax);
		t2s = (float*) malloc(nt*sizeof(float));
		v2s = (float*) malloc(nt*sizeof(float));
		bcopy(time,t2s,nt*sizeof(float));
		bcopy(v2,v2s,nt*sizeof(float));
		if(vstype2==0) {
			vrms2vint(t2, v2, np2, t2s, v2s, nt);
		} else if (vstype2==1) {
			vavg2vint(t2, v2, np2, t2s, v2s, nt);
		}
		n2s = nt;
	}

	n3s = 0;
	if(i3==1) { 
		findvf(in3, xqc, yqc, t3, v3, &np3, &x3, &y3, ntvmax, nvfmax); 
		times(dt, t3, np3, time, &nt, tmax);
		t3s = (float*) malloc(nt*sizeof(float));
		v3s = (float*) malloc(nt*sizeof(float));
		bcopy(time,t3s,nt*sizeof(float));
		bcopy(v3,v3s,nt*sizeof(float));
		if(vstype3==0) {
			vrms2vint(t3, v3, np3, t3s, v3s, nt);
		} else if (vstype3==1) {
			vavg2vint(t3, v3, np3, t3s, v3s, nt);
		}
		n3s = nt;
	}

	/* read in volocity function from vgrid */
	n4s = 0;
	if(iv1==1) {
		fgetusghdr(vfp1, &usgh);
		tmp = (xqc - usgh.o2)/usgh.d2 + .5;
		ix = tmp;
		if(ix<0) ix = 0;
		if(ix>=usgh.n2) ix = usgh.n2 - 1;
		x4 = usgh.o2 + ix*usgh.d2;
		tmp = (yqc - usgh.o3)/usgh.d3 + .5;
		iy = tmp;
		if(iy<0) iy = 0;
		if(iy>=usgh.n3) iy = usgh.n3 - 1;
		y4 = usgh.o3 + iy*usgh.d3;
		fprintf(stderr,"Velociy grid %s: x=%g y=%g t0=%g dt=%g nt=%d\n",
				vgrid1,x4,y4,usgh.o1,usgh.d1,usgh.n1);
		iseek = (iy*usgh.n2+ix)*usgh.n1*usgh.dtype;
		efseek(vfp1,iseek,0);
		efread(v4,sizeof(float),usgh.n1,vfp1);
		for(it=0;it<usgh.n1;it++) t4[it] = usgh.o1 + it*usgh.d1;
		np4 = usgh.n1; 
		if(vgtype1==2) {
			if(tmax < 100000.) {
				tmp = (tmax - usgh.o1)/usgh.d1;
				nt = tmp;
			} else {
				nt = np4;
			}
			t4s = (float*) malloc(nt*sizeof(float));
			v4s = (float*) malloc(nt*sizeof(float));
			bcopy(t4,t4s,nt*sizeof(float));
			bcopy(v4,v4s,nt*sizeof(float));
		} else if(vgtype1==0) {
			times(dt, t4, np4, time, &nt, tmax);
			t4s = (float*) malloc(nt*sizeof(float));
			v4s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t4s,nt*sizeof(float));
			vrms2vint(t4, v4, np4, t4s, v4s, nt);
		} else if(vgtype1=1) {
			times(dt, t4, np4, time, &nt, tmax);
			t4s = (float*) malloc(nt*sizeof(float));
			v4s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t4s,nt*sizeof(float));
			vavg2vint(t4, v4, np4, t4s, v4s, nt);
		}
		n4s = nt;
		x4min = usgh.o2;
		x4max = usgh.o2 + (usgh.n2 - 1)*usgh.d2;
		y4min = usgh.o3;
		y4max = usgh.o3 + (usgh.n3 - 1)*usgh.d3;
	}

	n5s = 0;
	if(iv2==1) {
		fgetusghdr(vfp2, &usgh);
		tmp = (xqc - usgh.o2)/usgh.d2 + .5;
		ix = tmp;
		if(ix<0) ix = 0;
		if(ix>=usgh.n2) ix = usgh.n2 - 1;
		x5 = usgh.o2 + ix*usgh.d2;
		tmp = (yqc - usgh.o3)/usgh.d3 + .5;
		iy = tmp;
		if(iy<0) iy = 0;
		if(iy>=usgh.n3) iy = usgh.n3 - 1;
		y5 = usgh.o3 + iy*usgh.d3;
		fprintf(stderr,"Velociy grid %s: x=%g y=%g t0=%g dt=%g nt=%d\n",
				vgrid2,x5,y5,usgh.o1,usgh.d1,usgh.n1);
		iseek = (iy*usgh.n2+ix)*usgh.n1*usgh.dtype;
		efseek(vfp2,iseek,0);
		efread(v5,sizeof(float),usgh.n1,vfp2);
		for(it=0;it<usgh.n1;it++) t5[it] = usgh.o1 + it*usgh.d1;
		np5 = usgh.n1; 
		if(vgtype2==2) {	
			if(tmax < 100000.) {
				tmp = (tmax - usgh.o1)/usgh.d1;
				nt = tmp;
			} else {
				nt = np5;
			}
			t5s = (float*) malloc(nt*sizeof(float));
			v5s = (float*) malloc(nt*sizeof(float));
			bcopy(t5,t5s,nt*sizeof(float));
			bcopy(v5,v5s,nt*sizeof(float));
		} else if(vgtype2==0) {
			times(dt, t5, np5, time, &nt, tmax);
			t5s = (float*) malloc(nt*sizeof(float));
			v5s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t5s,nt*sizeof(float));
			vrms2vint(t5, v5, np5, t5s, v5s, nt);
		} else if(vgtype2==1) {
			times(dt, t5, np5, time, &nt, tmax);
			t5s = (float*) malloc(nt*sizeof(float));
			v5s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t5s,nt*sizeof(float));
			vavg2vint(t5, v5, np5, t5s, v5s, nt);
		}
		n5s = nt;
		x5min = usgh.o2;
		x5max = usgh.o2 + (usgh.n2 - 1)*usgh.d2;
		y5min = usgh.o3;
		y5max = usgh.o3 + (usgh.n3 - 1)*usgh.d3;
	}

	n6s = 0;
	if(iv3==1) {
		fgetusghdr(vfp3, &usgh);
		tmp = (xqc - usgh.o2)/usgh.d2 + .5;
		ix = tmp;
		if(ix<0) ix = 0;
		if(ix>=usgh.n2) ix = usgh.n2 - 1;
		x6 = usgh.o2 + ix*usgh.d2;
		tmp = (yqc - usgh.o3)/usgh.d3 + .5;
		iy = tmp;
		if(iy<0) iy = 0;
		if(iy>=usgh.n3) iy = usgh.n3 - 1;
		y6 = usgh.o3 + iy*usgh.d3;
		fprintf(stderr,"Velociy grid %s: x=%g y=%g t0=%g dt=%g nt=%d\n",
				vgrid3,x6,y6,usgh.o1,usgh.d1,usgh.n1);
		iseek = (iy*usgh.n2+ix)*usgh.n1*usgh.dtype;
		efseek(vfp3,iseek,0);
		efread(v6,sizeof(float),usgh.n1,vfp3);
		for(it=0;it<usgh.n1;it++) t6[it] = usgh.o1 + it*usgh.d1;
		np6 = usgh.n1; 
		if(vgtype3==2) {
			if(tmax < 100000.) {
				tmp = (tmax - usgh.o1)/usgh.d1;
				nt = tmp;
			} else {
				nt = np6;
			}
			t6s = (float*) malloc(nt*sizeof(float));
			v6s = (float*) malloc(nt*sizeof(float));
			bcopy(t6,t6s,nt*sizeof(float));
			bcopy(v6,v6s,nt*sizeof(float));
		} else if(vgtype3==0) {
			times(dt, t6, np6, time, &nt, tmax);
			t6s = (float*) malloc(nt*sizeof(float));
			v6s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t6s,nt*sizeof(float));
			vrms2vint(t6, v6, np6, t6s, v6s, nt);
		} else if(vgtype3==1) {
			times(dt, t6, np6, time, &nt, tmax);
			t6s = (float*) malloc(nt*sizeof(float));
			v6s = (float*) malloc(nt*sizeof(float));
			bcopy(time,t6s,nt*sizeof(float));
			vavg2vint(t6, v6, np6, t6s, v6s, nt);
		}
		n6s = nt;
		x6min = usgh.o2;
		x6max = usgh.o2 + (usgh.n2 - 1)*usgh.d2;
		y6min = usgh.o3;
		y6max = usgh.o3 + (usgh.n3 - 1)*usgh.d3;
	}

	bzero(cmd,1024);
	bzero(cname,80);
	sprintf(cname,"coordinate.%d\0",getpid());

	if(paper==0) {
        	sprintf(cmd, "( <%s xgraph n=%d,%d,%d,%d,%d,%d,0,0,0,%d,%d,%d nplot=12 marksize=12,12,12,12,12,12,12,12,12,12,12,12 mark=0,1,2,3,4,5,0,1,2,3,4,5 label1=\"x (inline)\" label2=\"y (crossline)\" width=400 height=400 grid1=dash grid2=dash title=\"+=vs3d1 *=vs3d2 x=vs3d3 tri=vgrid1 sq=vgrid2 o=vgrid3\" ; /bin/rm -f %s ) & ", cname, i1, i2, i3, 5*iv1, 5*iv2, 5*iv3, iv1, iv2, iv3, cname);
	} else {
        	sprintf(cmd, "( <%s psgraph n=%d,%d,%d,%d,%d,%d,%d,%d,%d nplot=9 mark=0,1,2,3,4,5,3,4,5 marksize=12,12,12,12,12,12,12,12,12 label1=\"x (inline)\" label2=\"y (crossline)\" wbox=5 hbox=5 grid1=dash grid2=dash title=\"+=vs3d1 *=vs3d2 x=vs3d3 tri=vgrid1 sq=vgrid2 o=vgrid3\" labelsize=14 titlesize=16 | lpr ; /bin/rm -f %s ) & ", cname, i1, i2, i3, 5*iv1, 5*iv2, 5*iv3, iv1, iv2, iv3, cname);
	}


	datafp = efopen(cname,"w");

	if(i1==1) {
		efwrite(&x1,sizeof(float),1,datafp);
		efwrite(&y1,sizeof(float),1,datafp);
	}
	if(i2==1) {
		efwrite(&x2,sizeof(float),1,datafp);
		efwrite(&y2,sizeof(float),1,datafp);
	}
	if(i3==1) {
		efwrite(&x3,sizeof(float),1,datafp);
		efwrite(&y3,sizeof(float),1,datafp);
	}
	if(iv1==1) {
		efwrite(&x4min,sizeof(float),1,datafp);
		efwrite(&y4min,sizeof(float),1,datafp);
		efwrite(&x4max,sizeof(float),1,datafp);
		efwrite(&y4min,sizeof(float),1,datafp);
		efwrite(&x4max,sizeof(float),1,datafp);
		efwrite(&y4max,sizeof(float),1,datafp);
		efwrite(&x4min,sizeof(float),1,datafp);
		efwrite(&y4max,sizeof(float),1,datafp);
		efwrite(&x4min,sizeof(float),1,datafp);
		efwrite(&y4min,sizeof(float),1,datafp);
	}
	if(iv2==1) {
		efwrite(&x5min,sizeof(float),1,datafp);
		efwrite(&y5min,sizeof(float),1,datafp);
		efwrite(&x5max,sizeof(float),1,datafp);
		efwrite(&y5min,sizeof(float),1,datafp);
		efwrite(&x5max,sizeof(float),1,datafp);
		efwrite(&y5max,sizeof(float),1,datafp);
		efwrite(&x5min,sizeof(float),1,datafp);
		efwrite(&y5max,sizeof(float),1,datafp);
		efwrite(&x5min,sizeof(float),1,datafp);
		efwrite(&y5min,sizeof(float),1,datafp);
	}
	if(iv3==1) {
		efwrite(&x6min,sizeof(float),1,datafp);
		efwrite(&y6min,sizeof(float),1,datafp);
		efwrite(&x6max,sizeof(float),1,datafp);
		efwrite(&y6min,sizeof(float),1,datafp);
		efwrite(&x6max,sizeof(float),1,datafp);
		efwrite(&y6max,sizeof(float),1,datafp);
		efwrite(&x6min,sizeof(float),1,datafp);
		efwrite(&y6max,sizeof(float),1,datafp);
		efwrite(&x6min,sizeof(float),1,datafp);
		efwrite(&y6min,sizeof(float),1,datafp);
	}
	if(iv1==1) {
		efwrite(&x4,sizeof(float),1,datafp);
		efwrite(&y4,sizeof(float),1,datafp);
	if(iv2==1) {
		efwrite(&x5,sizeof(float),1,datafp);
		efwrite(&y5,sizeof(float),1,datafp);
	}
	if(iv3==1) {
		efwrite(&x6,sizeof(float),1,datafp);
		efwrite(&y6,sizeof(float),1,datafp);
	}
	}
	efclose(datafp);

	fprintf(stderr,"\n");
	fprintf(stderr,"Displaying Command: \n");
	fprintf(stderr,"%s \n",cmd);

	system(cmd);
	bzero(cname,80);
	sprintf(cname,"data.to.plot.%d\0",getpid());
	datafp = efopen(cname,"w");
	bzero(cmd,1024);
	
	if(paper==0) {
        	sprintf(cmd, "( <%s xgraph n=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d nplot=12 marksize=6,6,6,6,6,6,6,6,6,6,6,6 mark=0,1,2,3,4,5,0,1,2,3,4,5 linecolor=2,3,7,5,6,4,2,3,7,5,6,4 style=seismic label1=time label2=velocity width=900 height=1000 grid1=dash grid2=dash title=\"%s at x=%g y=%g\" ; /bin/rm -f %s ) & ", cname,ii*2*n1s,ii*2*n2s,ii*2*n3s,ii*2*n4s,ii*2*n5s,ii*2*n6s,ia*n1s,ia*n2s,ia*n3s,ia*n4s,ia*n5s,ia*n6s,title,xqc,yqc,cname);
	} else {
        	sprintf(cmd, "( <%s psgraph n=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d nplot=12 marksize=6,6,6,6,6,6,6,6,6,6,6,6 style=seismic label1=time label2=velocity wbox=6 hbox=8 grid1=dash grid2=dash title=\"%s at x=%g y=%g\" titlesize=16 labelsize=14 | lpr ; /bin/rm -f %s ) & ", cname,ii*2*n1s,ii*2*n2s,ii*2*n3s,ii*2*n4s,ii*2*n5s,ii*2*n6s,ia*n1s,ia*n2s,ia*n3s,ia*n4s,ia*n5s,ia*n6s,title,xqc,yqc,cname);
	}

	fprintf(stderr,"%s \n",cmd);

	free(work);
	work = (float*) malloc(n1s*4*sizeof(float));
	
	for(it=0;it<n1s;it++) {
		work[it*4] = t1s[it];
		work[it*4+1] = v1s[it];
		work[it*4+2] = t1s[it];
		if(it==n1s-1) {
			work[it*4+3] = v1s[it];
		} else {
			work[it*4+3] = v1s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n1s,datafp);

	free(work);
	work = (float*) malloc(n2s*4*sizeof(float));
	for(it=0;it<n2s;it++) {
		work[it*4] = t2s[it];
		work[it*4+1] = v2s[it];
		work[it*4+2] = t2s[it];
		if(it==n2s-1) {
			work[it*4+3] = v2s[it];
		} else {
			work[it*4+3] = v2s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n2s,datafp);


	free(work);
	work = (float*) malloc(n3s*4*sizeof(float));
	for(it=0;it<n3s;it++) {
		work[it*4] = t3s[it];
		work[it*4+1] = v3s[it];
		work[it*4+2] = t3s[it];
		if(it==n3s-1) {
			work[it*4+3] = v3s[it];
		} else {
			work[it*4+3] = v3s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n3s,datafp);

	free(work);
	work = (float*) malloc(n4s*4*sizeof(float));
	for(it=0;it<n4s;it++) {
		work[it*4] = t4s[it];
		work[it*4+1] = v4s[it];
		work[it*4+2] = t4s[it];
		if(it==n4s-1) {
			work[it*4+3] = v4s[it];
		} else {
			work[it*4+3] = v4s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n4s,datafp);

	free(work);
	work = (float*) malloc(n5s*4*sizeof(float));
	for(it=0;it<n5s;it++) {
		work[it*4] = t5s[it];
		work[it*4+1] = v5s[it];
		work[it*4+2] = t5s[it];
		if(it==n5s-1) {
			work[it*4+3] = v5s[it];
		} else {
			work[it*4+3] = v5s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n5s,datafp);

	free(work);
	work = (float*) malloc(n6s*4*sizeof(float));
	for(it=0;it<n6s;it++) {
		work[it*4] = t6s[it];
		work[it*4+1] = v6s[it];
		work[it*4+2] = t6s[it];
		if(it==n6s-1) {
			work[it*4+3] = v6s[it];
		} else {
			work[it*4+3] = v6s[it+1];
		}
	}
	if(viplot==1) efwrite(work,sizeof(float),4*n6s,datafp);

	if(vauxplot==1) {
		free(work);
		work = (float*) malloc(2*ntmax*sizeof(float));
		if(n1s>0) vrmsout(datafp,t1s,v1s,work,n1s);
		if(n2s>0) vrmsout(datafp,t2s,v2s,work,n2s);
		if(n3s>0) vrmsout(datafp,t3s,v3s,work,n3s);
		if(n4s>0) vrmsout(datafp,t4s,v4s,work,n4s);
		if(n5s>0) vrmsout(datafp,t5s,v5s,work,n5s);
		if(n6s>0) vrmsout(datafp,t6s,v6s,work,n6s);
	
	} else if(vauxplot==2) {
		free(work);
		work = (float*) malloc(2*ntmax*sizeof(float));
		if(n1s>0) vavgout(datafp,t1s,v1s,work,n1s);
		if(n2s>0) vavgout(datafp,t2s,v2s,work,n2s);
		if(n3s>0) vavgout(datafp,t3s,v3s,work,n3s);
		if(n4s>0) vavgout(datafp,t4s,v4s,work,n4s);
		if(n5s>0) vavgout(datafp,t5s,v5s,work,n5s);
		if(n6s>0) vavgout(datafp,t6s,v6s,work,n6s);
	}

	efclose(datafp);

	system(cmd);

	exit(0);

}

void vrmsout(FILE *datafp, float *ts, float *vs, float *work, int ns) {

	int it;

	vint2vrms(ts,vs,work,ns);
	bcopy(work,vs,ns*sizeof(float));
	for(it=0;it<ns;it++) {
		work[it*2] = ts[it];
		work[it*2+1] = vs[it];
	} 
	efwrite(work,sizeof(float),2*ns,datafp);
}

void vavgout(FILE *datafp, float *ts, float *vs, float *work, int ns) {

	int it;

	vint2vavg(ts,vs,work,ns);
	bcopy(work,vs,ns*sizeof(float));
	for(it=0;it<ns;it++) {
		work[it*2] = ts[it];
		work[it*2+1] = vs[it];
	} 
	efwrite(work,sizeof(float),2*ns,datafp);
}
	


void times(float dt, float *t, int ntv, float *time, int *nt, float tmax) {

	float tmp, ttmax;
	int i1, i;

	/* find out the maximum time */
	ttmax = t[ntv-1];
	if(ttmax>tmax) ttmax = tmax;

	if(dt>0.) {
		tmp = ttmax/dt + 1.5;
		*nt = tmp;
		for(i1=0;i1<*nt;i1++) time[i1] = i1*dt;
	} else {
		i = 0;
		for(i1=0;i1<ntv;i1++) {
			if(t[i1]<=tmax) {
				time[i] = t[i1];
				i = i + 1;
			}
		}
		*nt = i;
	}
}

	
void vint2vrms(float *t, float *vi, float *vrms, int nt) {

	int i1;
	
	vrms[0] = vi[0]*vi[0]*t[0];
	for(i1=1;i1<nt;i1++) {
		vrms[i1] = vrms[i1-1] + vi[i1]*vi[i1]*(t[i1]-t[i1-1]);
	}
	for(i1=1;i1<nt;i1++) {
		vrms[i1] = sqrt(vrms[i1]/t[i1]);
	}
	vrms[0] = vi[0];
}

void vint2vavg(float *t, float *vi, float *vavg, int nt) {

	int i1;
	
	vavg[0] = vi[0]*t[0];
	for(i1=1;i1<nt;i1++) {
		vavg[i1] = vavg[i1-1] + vi[i1]*(t[i1]-t[i1-1]);
	}
	for(i1=1;i1<nt;i1++) {
		vavg[i1] = vavg[i1]/t[i1];
	}
	vavg[0] = vi[0];
}


void vrms2vint(float *t, float *v, int ntv, float *time, float *vi, int nt) {

	float *work, tmax;
	int *indx;
	int i1, j;


	work = (float*) emalloc(nt*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));


	/* linearly interpolate v to nt times */
	lin1d_(t,v,&ntv,time,vi,&nt,indx);

	/* compute interval velocities via dix formula */
	work[0] = vi[0]*vi[0];
	tmax = t[ntv-1];

	for(j=1;j<nt;j++) {
		if(time[j]<=tmax) { 
			work[j] = (vi[j]*vi[j]*time[j] -  	
		   		vi[j-1]*vi[j-1]*time[j-1]) / 
		  		(time[j]-time[j-1]);
		} else {
			work[j] = work[j-1];
		}

	}
	for(j=0;j<nt;j++) {
		if(work[j]<0.) err(" Negative Interval Velocity "); 
		vi[j] = sqrt(work[j]);
	}

	free(indx);
	free(work);

}

void vavg2vint(float *t, float *v, int ntv, float *time, float *vi, int nt) {

	float *work, tmax;
	int *indx;
	int i1, j;


	work = (float*) emalloc(nt*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));


	/* linearly interpolate v to nt times */
	lin1d_(t,v,&ntv,time,vi,&nt,indx);

	/* compute interval velocities via dix formula */
	work[0] = vi[0];
	tmax = t[ntv-1];

	for(j=1;j<nt;j++) {
		if(time[j]<=tmax) { 
			work[j] = (vi[j]*time[j] -  
		   		vi[j-1]*time[j-1]) / 
		  		(time[j]-time[j-1]);
		} else {
			work[j] = work[j-1];
		}

	}
	for(j=0;j<nt;j++) {
		if(work[j]<0.) err(" Negative Interval Velocity "); 
		vi[j] = work[j];
	}

	free(indx);
	free(work);

}

/* find closest VS3D card to location (xqc,yqc) */ 
/* t and v are pre-allocated arrays of length longer than 256 */
/*  
    input:
	in= 	file name of vs3d cards
	xqc=    x coordinate of qc position
	yqc=    y coordinate of qc position
	ntvmax= maximum number of t-v pairs per velocity function
	nvfmax= maximum number of velocity functions in the input
    output:
	t=      times of closest vs3d card
	v=      velocities of closest vs3d card
	np=     number of t-v pairs at this locations
	xx=     x coorinate of the closest vs3d card
	yy=     y coorinate of the closest vs3d card

    author:		z. li		      		9/14/93 
*/
	

void findvf(char *in, float xqc, float yqc, float *t, float *v, int *np, 
	float *xx, float *yy, int ntvmax, int nvfmax) { 


	int n1, n2, nxy;
    	float *xs, *ys, *tpicks, *vpicks;
    	int *nps;
	FILE *infp;
	float dis, x, y, tmp;
	int ip, i1, idis;

	infp = efopen(in,"r");

    	/* at most 4096 input (x,y) VS3D cards with at most 256 time-vel 
		pairs each */

    	n1 = ntvmax;
    	n2 = nvfmax;

    	/* arrays used to store all VELF card's cdp, time and velocity */
    	xs = (float*)malloc(n2*sizeof(int));
    	ys = (float*)malloc(n2*sizeof(int));
    	tpicks = (float*)malloc(n1*n2*sizeof(float));
    	vpicks = (float*)malloc(n1*n2*sizeof(float));
    	nps = (int*)malloc(n2*sizeof(int));
	bzero(nps,n2*sizeof(int));

    	/* read in VS3D cards */
    	nxy = 0;
	vs3dread(infp,xs,ys,tpicks,vpicks,&nxy,nps,n1,n2);
   	if (nxy==0) err("No VS3D card input for in=%s ! Job aborted",in);
	x = xs[0];
	y = ys[0];
	dis = sqrt((xqc-x)*(xqc-x)+(yqc-y)*(yqc-y));
	idis = 0;
	*np = nps[0];
	*xx = x;
	*yy = y;
	for(i1=0;i1<*np;i1++) {
		t[i1] = tpicks[i1];
		v[i1] = vpicks[i1];
	}

	for(ip=1;ip<nxy;ip++) {
		x = xs[ip];
		y = ys[ip];
		tmp = sqrt((xqc-x)*(xqc-x)+(yqc-y)*(yqc-y));
		if(tmp<dis) {
			dis = tmp;
			idis = ip;
		}
    }
	*np = nps[idis];
	for(i1=0;i1<*np;i1++) {
		t[i1] = tpicks[idis*n1+i1];
		v[i1] = vpicks[idis*n1+i1];
	}
	*xx = xs[idis];
	*yy = ys[idis];
			
	fprintf(stderr,
	"VS3D card found for %s at x=%g y=%g ip=%d location for %d t-v pairs \n",
			in,*xx,*yy,idis+1,*np); 

	free(nps);
	free(tpicks);
	free(vpicks);	
	free(xs);
	free(ys);
	fclose(infp);

}
