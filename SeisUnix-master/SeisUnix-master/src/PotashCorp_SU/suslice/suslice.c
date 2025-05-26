/* suslice */
/* B.Nemeth */



#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {" SUSLICE - select a time slice from 3D data                "
"                                                                           ",
"suslice < infile > outfile [optinal parameters]                            ",
"                                                                           ",
" Required Parameter:						 	    ",
"                                                                           ",
" ts			time of the slice; 				    ",
" 			selected sample will be ts/(tr.dt/10^6)		    ",
"                                                                           ",
" sts			sample number of the slice; 			    ",
"                                                                           ",
"                                                                           ",
"                                                                           ",
"                                                                           ",
"                                                                           ",
" Output is a flat binary file which can be viewed with ximage		    ",	
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */
float *data;				/* trace data */

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	segy tr;				/* SEGY trace */
	float *data;				/* trace data */
	int nt;                 /* number of time samples               */
        int ntr=0;              /* number of traces                     */
	int i,flag,lt=0;	/* counter */
	float dt;		/* sample interval */
	float ts=1.0/0.0;		/* time of slice */
	int sts;		/* sample number of slice */
	int sts_s;		/* sample number of slice with shift */
	 
	initargs(argc, argv);
   	requestdoc(1);
	
	if(!getparfloat("ts",&ts) && !getparint("sts",&sts)) {
		err("Must specify ts = time of slice (s) or sts = sample number of slice ");
	} 
	
	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;
	dt = tr.dt;
	
	if(dt==0.0) {
		if(!getparfloat("dt",&dt)) { dt=0.002;
			warn("tr.dt is not set dt=0.002 is assumed");
		}
	} else {
		dt/=1000000.0;
	}
	
	if(!getparint("sts",&sts)) {
		if (!isnan(ts)) {
			sts = (int) ( (ts-(float)tr.delrt/1000.0)/( dt));
			fprintf(stderr," Computed slice sample number %d\n",sts);
		}
	}
	sts_s = sts+(tr.delrt/1000.0)/dt;
	
	if( sts_s<0 || sts_s>nt) {
		warn("Valid range %5.f %5.f %d\n",tr.delrt/1000.0,tr.delrt/1000.0+(nt-1)*dt,sts_s);
		err(" Time of slice is out of bound");
	}
	
	data = ealloc1float(nt);
	
	do {
		sts_s = sts+(tr.delrt/1000.0)/dt;
		memcpy( (void *) data, (const void *) tr.data, nt*FSIZE);
		fwrite(&data[sts_s],FSIZE,1,stdout);
	} while(gettr(&tr));
	
	free1float(data);
   return EXIT_SUCCESS;
}
