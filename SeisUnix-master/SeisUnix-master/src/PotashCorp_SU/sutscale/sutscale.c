/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {"SUTSCALE -  Time to depth scaling< infile >outfile     ",
"                                                                             ",
"     sutscale < infile >outfile [Optional paramers]                          ",
"                                                                             ",
"                                                                             ",
"     Optinal parameters:                                                     ",
"                                                                             ",
"     sco=0.0		Array of old time values                              ",
"     scn=0.0		Array of new time scale values at times specified     ",
"                       by the sco array                                      ",
"     dtn=dt		New time axis sampling intervall                      ",
"                                                                             ",
"                       The new time scale is computed by sinc interpolation  ",
"                       and extrapolation                                     ",
NULL};
   
/* Segy data constans */
segy tr,trn;				/* SEGY trace */

int main( int argc, char *argv[] )
{
        int nt;		/* Number of samples */
        int nnt;	/* New Number of samples */
	int nts;	/* number of scale values */
	float *sco;	/* old reference array */
	float *scn;	/* new reference array */
	float *scoa;	/* old axis array */
	float *scna;	/* new axis array */
	float *scnta;	/* new axis array */
	float *scouta;	/* new axis array */
	float ndt;	/* new sample rate */
	float dt;	/* old sample rate */
	float *data;	/* new data storage array */
	                

	initargs(argc, argv);
   	requestdoc(1);
        
	if (!getparfloat("ndt", &ndt))   ndt= 1.0; 
	
	if ((nts = countparval("sco"))!=0) {
		sco = ealloc1float(nts);
		getparfloat("sco",sco);
	}
	
	if ((countparval("sco"))!=nts) {
		err("Number of values in array sco and scn has to be equal!\n");
	} else {
		scn = ealloc1float(nts);
		getparfloat("scn",scn);
	}
	
        if (!gettr(&tr)) err("can't get first trace");
	nt=tr.ns;
	dt=(float)(tr.dt)/1000000.0;
	
	scoa=ealloc1float(nt);
	scna=ealloc1float(nt);
	{ register int it;
		for(it=0;it<nt;it++) scoa[it]=tr.delrt+it*dt;
	}
	intlin(nts,sco,scn,0.0,0.0,nt,scoa,scna);
	
	/* linear extrapolation */
	if(sco[0] > scoa[0]) {
		{ register int it,its=0;
			float m,b;
				while(scna[its]==0.0){
					its++;
				}
				m=(scn[1]-scn[0])/(sco[1]-sco[0]);
				b=scn[0]-m*sco[0];
				for(it=0;it<its;it++) scna[it]=m*scoa[it]+b;
		}
	}
	if(sco[nts-1] < scoa[nt-1]) {
		{ register int it,its=nt-1;
			float m,b;
				while(scna[its]==0.0){
					its--;
				}
				m=(scn[nts-1]-scn[nts-2])/(sco[nts-1]-sco[nts-2]);
				b=scn[nts-1]-m*sco[nts-1];
				for(it=its+1;it<nt;it++) scna[it]=m*scoa[it]+b;
		}
	}
	{ register int it;
		for(it=0;it<nt;it++) fprintf(stderr," %f %f\n",scoa[it],scna[it]);
	}
	
	
	/* regular sampling of the out put seismic trace */
	nnt = (int)((scna[nt-1]-scna[0])/ndt)+1;
	scnta=ealloc1float(nnt);
	{ register int nit;
		for(nit=0;nit<nnt;nit++) scnta[nit]=scna[0]+nit*ndt;
	}	
	scouta = ealloc1float(nnt);
	intlin(nt,scna,scoa,0,0,nnt,scnta,scouta);
/*	{ register int it;
		for(it=0;it<nnt;it++) fprintf(stderr," %f %f\n",scnta[it],scouta[it]);
	}
*/	
	/* resample the seismic trace */
	data = ealloc1float(nnt);

	do {
		ints8r(nt,dt,tr.delrt,tr.data,0.0,0.0,nnt,scouta,data);
		memcpy((void *) tr.data, (const void *) data, nnt*FSIZE);
		tr.ns=nnt;
		tr.dt=NINT(ndt*1000000.0);
		tr.d1=ndt;
		tr.f1=scna[0];
		tr.delrt=NINT(scna[0]*1000);
		puttr(&tr);
	} while(gettr(&tr));
  
   	return EXIT_SUCCESS;
}
