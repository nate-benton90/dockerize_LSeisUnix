/* plot vs3d curves */

#include "velo.h"
#include "comva.h"
#include "par.h"



char *sdoc = 
"PLOTVS3D - display of VS3D cards \n"
"\n"
"plotvs3d [parameters] <vs3d.data 					\n" 
"\n"
"Required parameters:						 	\n"
"vs3d.data=        Name of VS3D dataset to plot				\n"
"\n"
"Optional parameters:							\n"
"xmin=-9999999999  minimum inline coordinate of VS3D to plot		\n"
"xmax=9999999999   maximum inline coordinate of VS3D to plot		\n"
"ymin=-9999999999  minimum crossline coordinate of VS3D to plot		\n"
"ymax=9999999999   maximum crossline coordinate of VS3D to plot		\n"
"vstype=0          Velocity type of VS3D card (0=rms 1=avg 2=interval)	\n"
"plot=0            Plot type (0=rms 1=avg 2=int) 			\n"
"paper=0           1=hard copy plot; 0=x-window plot 			\n"
"nvfmax=4096       maximum number of velocity functions each vs3d data	\n"
"                  set may have						\n"
"ntvmax=256        maximum number of t-v pairs per velocity function in \n"
"                  vs3d datasets					\n"
"marksize=0        mark size of data points in pixels			\n"
"linewidth=1       line width in pixels				\n"
"Note:									\n"
" see vgrid3d for the format of VS3D					\n"
"\n"
"AUTHOR:	   Zhiming Li,       ,			9/14/93		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin;
	FILE *datafp;
	float xmin, xmax, ymin, ymax;
	int plot, paper, vstype, nvfmax, ntvmax;
	float *xs,*ys,*ts,*vs;
	int *nps, nxy;
	char cmd[1024];
	char cname[80];
	int *n, nplot, i, it, nt;
	float *tt, *vv, *tv;
	char parname[80];
	FILE *parfp;
	int np, marksize, linewidth;

	initargs(argc,argv);
        askdoc(1);

	/* optional parameters */
	if (!getparfloat("xmin",&xmin)) xmin = - 9999999999.;
	if (!getparfloat("xmax",&xmax)) xmax =  9999999999.;
	if (!getparfloat("ymin",&ymin)) ymin = - 9999999999.;
	if (!getparfloat("ymax",&ymax)) ymax =  9999999999.;

	if (!getparint("vstype",&vstype)) vstype = 0;
	if (!getparint("paper",&paper)) paper = 0;
	if (!getparint("plot",&plot)) plot = 0;
	if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
	if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
	if (!getparint("marksize",&marksize)) marksize = 0;
	if (!getparint("linewidth",&linewidth)) linewidth = 1;

/* read in vs3d cards */
	xs = (float*) emalloc(nvfmax*sizeof(float));
	ys = (float*) emalloc(nvfmax*sizeof(float));
	ts = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
	vs = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
	nps = (int*) emalloc(nvfmax*sizeof(int));
	n = (int*) emalloc(nvfmax*sizeof(int));
	vs3dread(infp,xs,ys,ts,vs,&nxy,nps,ntvmax,nvfmax);

	bzero(cname,80);
        sprintf(cname,"data.to.plot.%d\0",getpid());
	datafp = efopen(cname,"w");

	nplot = 0;
	np = 0;
	for(i=0;i<nxy;i++) {
		if( xmin <= xs[i] && xs[i] <= xmax &&
		    ymin <= ys[i] && ys[i] <= ymax) {
			nt = nps[i]; 
			n[nplot] = nt;
			tt = (float*) emalloc(nt*sizeof(float));
			vv = (float*) emalloc(nt*sizeof(float));
			tv = (float*) emalloc(2*nt*sizeof(float));
			for(it=0;it<nt;it++) {
				tt[it] = ts[i*ntvmax+it];
			}
			vconvert(ts+i*ntvmax,vs+i*ntvmax,nt,vstype,0,
        			 tt,vv,nt,plot,0);
			nplot = nplot + 1;
			for(it=0;it<nt;it++) {
				tv[it*2] = tt[it];
				tv[it*2+1] = vv[it];
			}
			efwrite(tv,sizeof(float),nt*2,datafp);
			free(tt);
			free(vv);
			free(tv);
			np = np + nt;	
		}
	}
	efclose(datafp);

	bzero(parname,80);
        sprintf(parname,"par.for.plot.%d\0",getpid());
	parfp = efopen(parname,"w");

       	fprintf(parfp, "nplot=%d width=900 height=1000 \n",nplot);
       	fprintf(parfp, "grid1=dash grid2=dash \n");
       	fprintf(parfp, "style=seismic label1=time label2=velocity \n");
	nt = n[0];
	fprintf(parfp, "n=%d",nt);
	for(i=1;i<nplot;i++) {
		nt = n[i];
		fprintf(parfp,",%d",nt);
	}
	if(marksize>0) {
		fprintf(parfp, " marksize=%d",marksize);
		for(i=1;i<nplot;i++) {
			fprintf(parfp,",%d",marksize);
		}
	}
	if(linewidth!=1) {
		fprintf(parfp, " linewidth=%d",linewidth);
		for(i=1;i<nplot;i++) {
			fprintf(parfp,",%d",linewidth);
		}
	}

	fprintf(parfp, "\n");
	efclose(parfp);

	bzero(cmd,1024);
	
	if(paper==0) {
        	sprintf(cmd, "( <%s xgraph par=%s ; /bin/rm -f %s; /bin/rm -f %s ) & ", cname,parname,cname,parname);
	} else {
        	sprintf(cmd, "( <%s psgraph par=%s ; /bin/rm -f %s; /bin/rm -f %s ) & ", cname,parname,cname,parname);
	}

	fprintf(stderr,"PLOT COMMAND: \n");
	fprintf(stderr,"%s \n",cmd);

	system(cmd);

	free(n);
	exit(0);

}
