/* sudummy.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "suhdr.h"


/*********************** self documentation *****************************/
char *sdoc[] = {"                                                       ",
"       SUPATSR - pattern search                                        ",
"                                                                       ",
"       supatsr < infile > oufile pf= [optional parameters]             ",
"                                                                       ",
"       Required parameters:                                            ",
"                                                                       ",
"       pf=                ascii file of the pattern (wavelet)          ",
"                                                                       ",
"                                                                       ",
"       Optional parameters:                                            ",
"                                                                       ",
"       ts=0.01              search time window  in seconds             ",
"                                                                       ",
"       t=0.8                                                           ",
"                                                                       ",
"       pna=amp.su	   Name of su file where the maximum amplitude   ",
"                          value of the correlation functions are written",
"       pnt=time.su	   Name of su file where the time of the maximum ",
"                          value of the correlation functions are written",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */
segy otr;
float *data;				/* trace data */


int main( int argc, char *argv[] )
{

	int nt;                 	/* number of time samples */ 
	float dt;       
	int ntp;                 	/* number of time samples in pattern*/        
	char *pf="";           		/* name of pattern file*/
	char *pna="";           	/* name of correlation pick file */
	char *pnt="";           	/* name of correlation pick file */
	FILE *fpf;			/* file pointer to pattern file */
	FILE *fpfa;			/* file pointer to pick file */
	FILE *fpft;			/* file pointer to pick file */
	
	float *data;
	float *pat;
	int im;				/* index of the maximum valu of the corr. function */
	float ts;			/* search time window */
	float t;			/* centre of search */
	int its;			/* start of search */
	int ite;			/* end of search */
	float sts;			/* start of search */
	float ste;			/* end of search */
	
	int bc=0;
	int pcaf=0;

	initargs(argc, argv);
   	requestdoc(1);

	MUSTGETPARSTRING("pf",&pf);
	if (!getparfloat("t", &t)) t =0.8;
	if (!getparfloat("ts", &ts)) ts =0.01;
	if (!getparstring("pna", &pna)) pna ="amp.su";
	if (!getparstring("pnt", &pnt)) pnt ="time.su";
	
	fpf = efopen(pf,"r");
	
	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;
	dt=(float)((double)tr.dt/1000000.0);
	
	/* Maximum search window */
	sts=t-ts;
	its=NINT((sts-tr.delrt/1000.0)/dt);
	ste=sts+2.0*ts;
	ite=its+NINT(2.0*ts/dt);
	
	fprintf(stderr," %f %f\n",sts,ste);	
	
	/* allocate arrays */
	data = ealloc1float(nt);
	pat = ealloc1float(nt);

	{ int i=0;
                fscanf(fpf," %f",&pat[i]);
		while(!feof(fpf)){
                	i++;
			if(i>nt) err(" Pattern is longer than data trace!\n");
                	fscanf(fpf," %f",&pat[i]);
        	}
		fprintf(stderr," %d\n",i);
		ntp=i;
	}
	fclose(fpf);
	
	fpfa = efopen(pna,"w");
	fpft = efopen(pnt,"w");
	
	do {
		
		memcpy( (void *) &otr, (const void *) &tr, HDRBYTES);
		memset((void *)  otr.data, (int) '\0', nt);
		
		if(bc) {
			memcpy((void *) &otr.data[0], (const void *) pat, ntp*FSIZE);
			do_bccorr(otr.data,tr.data,otr.data,nt,0,nt,
				0.5/dt*0.1,0.5/dt*0.9,dt);
		} else if(pcaf) {
			{ float **dat;
			  int itw,it,ind;
			  dat = matrix(ntp,2);
			  pca_alloc(ntp,2);
			  
			  for(itw=0;itw<ntp;itw++) dat[itw+1][1]=pat[itw];
			  
			  for(it=0;it<nt;it++) {
			  	for(itw=0;itw<ntp;itw++) {
					ind=it-ntp/2+itw;
					if(ind>=0 && ind<nt ) {
						dat[itw+1][2]=tr.data[ind];
					} else {
						dat[itw+1][2]=0.0;
					}
				}
				otr.data[it]=pca(dat,ntp,2,2);
			 }
			  pca_free(ntp,2);
			  free_matrix(dat,ntp,2);  
			}
		} else {
			xcor(nt,0,tr.data,ntp,0,pat,nt,-nt +1 ,otr.data);
		}
		
		if (!pcaf){
			/* Flip it back from reverse */
			{ float *a;
		   	int i;
				a=ealloc1float(nt);
				for(i=0;i<nt;i++) a[i]=otr.data[nt-1-i];
				memcpy((void *) &otr.data[0], (const void *) a, nt*FSIZE);
				free1float(a);
			}
		
			/* Normalize */
			sscal(nt,1.0/pat[isamaxs(ntp,pat,1)],otr.data,1);
		}
		
		/* Find the magnitude and the time of the maximum value */
		memcpy((void *) data, (const void *) &otr.data[its], (ite-its)*FSIZE);
		{ int i;
			for(i=0;i<ite-its;i++) if(data[i]<0.0) data[i]=0.0;
		}
		im=isamaxs(ite-its,data,1)+its;
		
		
		/* write out correlation functin */
		puttr(&otr);
		
		/* write out amplitude */
		otr.ns=1;
		otr.f1=im;
		otr.delrt=0;
		otr.data[0]=otr.data[im];
		fputtr(fpfa,&otr);
		
		/* write out time value */
		otr.ns=1;
		otr.f1=im;
		otr.delrt=0;
		otr.data[0]=im*dt+tr.delrt/1000.0;
		fputtr(fpft,&otr);
	} while(gettr(&tr));

	fclose(fpfa);
	fclose(fpft);
   	
	free1float(data);
	free1float(pat);
	
	return EXIT_SUCCESS;
}
