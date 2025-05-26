/* B.Nemeth */
#include "suhdr.h"

/* Functions */
static void stncl(float hm, float dhx,float dhy,int **in, int *nin,float *dom);

/*********************** self documentation *****************************/
char *sdoc[] = {" SUCSPG_PREP - select a common scatter point gather from a 3D data",
"                                                                           ",
"sucspg_prep2  [optinal parameters]                            ",
"                                                                           ",
" Required Parameter:						 	    ",
"                                                                           ",
"									    ",
"       xm=       maximum offset to consider                                ",
"       dir=      Directory where to put the gather tables                   ",
"									    ",
"       Binning parameters:					",
" 	xc=1		x coordinate of the corner bin centre   ",
" 	yc=1		y coordinate of the corner bin centre   ",
" 	dbx=20		bin size in x direction		        ",
" 	dby=20		bin size in y direction		        ",
" 	dirx=1		direction of bin numbering in x  dir.   ",
" 			-1=right to left; other=left to right   ",
" 	diry=1		direction of bin numbering in Y dir.    ",
" 			-1=up to down; other= down to up        ",
" 	deg=0		rotation of the bin-grid relative to    ",
" 			coordinate system in degrees	        ",
"                                                                           ",
"  This program creates tables for all the cspg gathers than can be compouted",
"  from a 3D volume with given geometry                                     ",
"                                                                           ",
"                                                                           ",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */

int main( int argc, char *argv[] )
{

	/* General stuff */
	int verbose=0;
	int i;			/* counter */
	cwp_String dir;
	char *fname;
	FILE *fp;
	
	
	/* Cspg bin parameters */
	float xm;
	float x;
	float y;
	int x_int;
	int y_int;

	/* binning */
	float xc;
	float yc;
	float dbx;
	float dby;
	float deg;
	float degr;
	int dirx;
	int diry;
	int nx;
	int ny;
	float xe;
	float ye;
	int ncdp;
	float **cdpcxy;
	float *cdp;
	float *cdpi;
	int ix;
	int iy;
	int cc;
	int nout;
	
	int **istncl;
	int nist;
	int istx,isty;
	float dom;		/* the largest offset gap  
				   among traces in migration gather */
	
	initargs(argc, argv);
   	requestdoc(1);

	if(!getparint("verbose",&verbose)) verbose=0;
	
	/* cspg bin parameters */
	MUSTGETPARFLOAT("xm",&xm);
        MUSTGETPARSTRING("dir", &dir);

	/* binning stuff */
	if(!getparfloat("xc",&xc)) xc=1;
	if(!getparfloat("yc",&yc)) yc=1;
	if(!getparfloat("dbx",&dbx)) dbx=20;
	if(!getparfloat("dby",&dby)) dby=20;
	if(!getparfloat("deg",&deg)) deg=0;
	degr = 3.141592653/180 * deg;
	if(!getparint("cc",&cc)) cc=0;
	if(!getparint("dirx",&dirx)) dirx=1;
	if(!getparint("diry",&diry)) diry=1;
	MUSTGETPARINT("nx", &nx);
	MUSTGETPARINT("ny", &ny);
	
	/* edge of bin# 1,1 */
	xe = xc - dirx*dbx/2.0*cos(degr) - diry*dby/2.0*sin(degr);
	ye = yc - diry*dby/2.0*cos(degr) + dirx*dbx/2.0*sin(degr);
	
	/* number of cdps */
	ncdp=nx*ny;
	
	/* arrays */
	fname = ealloc1(BUFSIZ,sizeof(char));
	cdpcxy = ealloc2float(ncdp,3);
	cdpi = ealloc1float(ncdp);
	cdp=cdpcxy[0];
	
	nist=((int)(xm/dbx+0.5)*(int)(xm/dby+0.5))*4;
	istncl=ealloc2int(nist,2);
	
	/* compute bin centre coordinates */
	for(ix=1;ix<=nx; ix++) {
		for(iy=1;iy<=ny;iy++) { 
			cdpcxy[0][(ix-1)*ny+iy-1] = (float)ix*1000+iy;
			cdpcxy[1][(ix-1)*ny+iy-1] = xc + dirx*(ix-1)*dbx*cos(degr) +
                        	diry*(iy-1)*dby*sin(degr);
			cdpcxy[2][(ix-1)*ny+iy-1] = yc + diry*(iy-1)*dby*cos(degr) -
                        	dirx*(ix-1)*dbx*sin(degr);
			if(verbose==1) fprintf(stderr," %10.0f %f %f\n",
			cdpcxy[0][(ix-1)*ny+iy-1],cdpcxy[1][(ix-1)*ny+iy-1],
			cdpcxy[2][(ix-1)*ny+iy-1]);
			}
	}
	
	stncl(xm,dbx,dby,istncl,&nist,&dom);
	fprintf(stderr," Maximum offset gap in migration gather %f\n",dom);
	
	for(ix=1;ix<=nx; ix++) {
	   for(iy=1;iy<=ny;iy++) { 
	       	x = cdpcxy[1][(ix-1)*ny+iy-1];
       	    	y = cdpcxy[2][(ix-1)*ny+iy-1];
		
		nout=0;
		for(i=0;i<nist;i++) {
			istx=ix+istncl[0][i];
			isty=iy+istncl[1][i];
			if(istx >= 1 && istx <=nx) 
				if(isty >= 1 && isty <=ny) {
					cdpi[nout]=(float)(istx*1000+isty);
					nout++;
				}
		}
		
		sprintf(fname,"%s/%d",dir,ix*1000+iy);
		fp=efopen(fname,"w");
/*		fprintf(fp," %10.1f %10.1f ",x,y); */
		x_int = (int)(x*10.0+0.5);
		y_int = (int)(y*10.0+0.5);
		x = ((float)(x_int))/10;
		y = ((float)(y_int))/10;
		fwrite(&x,sizeof(float),1,fp);
		fwrite(&y,sizeof(float),1,fp);
		
		qksort(nout,cdpi);
/*		for(i=0;i<nout;i++) fprintf(fp,"%7.0f", cdpi[i]); */
		fwrite(&cdpi[0],sizeof(float),nout,fp);
		fclose(fp);
			
		fprintf(stdout,"CDP=%5d  X=%10.1f Y=%10.1f #CDPs=%5d\n",
	                ix*1000+iy,x,y,nout);

			
			
	    }			/* iy loop */
	}			/* ix loop */

   return EXIT_SUCCESS;
}
void stncl(float hm, float dhx, float dhy,int **in, int *nin,float *dom)
/* select a circular pattern from a rectangular grid */
/* hm radius of the circle, dhx, dhy, grid sizes in x and y directions */
/* in is a two dimensional array of indexes of values falling within the circle */
/*  such that sqrt(sqr(dhx*in[0])+sqr(dhy*in[1]))<=hm */
/* The center is at 0,0 */
/* in has to be declared outside large enough, typicalli 2*hm/dhx*2*hm/dhy */
/* nin is the number of values in in */
/* *dom returnes the largest offset difference among the selected offset */
{
void uqsort(int n, int *un, float *a);

	int u,k;
	float r;
	int n,cnt,ucnt;
	float *of,ldo;
	
	n=NINT(hm/dhx);
	of = ealloc1float(4*n*n);
	cnt=0;
	for(k=-n; k<=n; k++) {
		u=0;
		r=(float)sqrt((double)(SQR(k*dhx)+SQR(u*dhy)));
		while(r <= hm) {
			in[0][cnt]=k;
			in[1][cnt]=u;
			u++;
			r=sqrt(SQR(k*dhx)+SQR(u*dhy));
			of[cnt]=r;
			cnt++;
		}
	}
	for(k=-n; k<=n; k++) {
		u=-1;
		r=(float)sqrt((double)(SQR(k*dhx)+SQR(u*dhy)));
		while(r <= hm) {
			in[0][cnt]=k;
			in[1][cnt]=u;
			u--;
			r=sqrt(SQR(k*dhx)+SQR(u*dhy));
			of[cnt]=r;
			cnt++;
		}
	}
	*nin=cnt;
	
	/* do a unique sort on all offsets */
	uqsort(cnt,&ucnt,of);
	
	/* compute the largest offset difference */
	ldo=0;
	{ float dof;
		for(k=0;k<ucnt-1;k++) {
		 	dof=of[k+1]-of[k];
			if(ldo<dof) ldo=dof;
		}
	}
	
	*dom=ldo;
	
	free1float(of);
}
			
