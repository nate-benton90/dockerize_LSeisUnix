#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                               ",
" SUVPAR2TR - Convert data in velocity par format to segy trace ",
"                                                               ",
" suvpar2tr          >stdout tnmo= vnmo=		        ",
"                                                               ",
" Required parameters:                                          ",
"       tnmo= 		array of tnmo values                    ",
"       vnmo=           array of vnmo values                    ",
"                                                               ",
" Optional parameters:                                          ",
"       dt=0.02       sampling interval of the velocity function",
"      tmin=0.00      minimum time of the velocity function     ",
"      tmax=2.00      maximum time of the velocity function     ",
"      vtmin=1300     velocity at minimum time                  ",
"      vtmax=5300     velocity at maximum time                  ",
"      cdp=001        value of tr.cdp to set                    ",
"                                                               ",
"                                                               ",
"      uvt=0	      =1 use vtmax at tmax as a velocity        ",
"                     othervise the last vnmo value is used     ",
"                     if it is smaller than vtmax               ",
"                                                               ",
NULL};

segy trout;

int
main(int argc, char **argv)
{
      	float *tnmo=NULL;		/* array of tnmo values */
	int nt;
	float *vnmo=NULL;		/* array of vnmo values */
	int nv;

	float *tout;			/* arrays of time values */
	float dt;
	float tmin;
	float tmax;
	float vtmin;
	float vtmax;
	int cdp;
	int uvt;

        int i;



        
        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);


	
	if (!(nt = countparval("tnmo")))  MUSTGETPARFLOAT("tnmo", tnmo);
	
	if (!(nv = countparval("vnmo")))  MUSTGETPARFLOAT("vnmo", vnmo);
	if(nt!=nv) err("Number of tnmo must equal to the number of vnmo values");
       	if (!getparint("uvt", &uvt)) uvt=0;
	
	/* should we use vtmax as the last value */
	if(uvt) {
		nt++;
		nv++;
	}
		
	
		
	tnmo = ealloc1float(nt); 
	vnmo = ealloc1float(nv); 
        getparfloat("tnmo", tnmo);
	getparfloat("vnmo", vnmo);
	
	

       	if (!getparfloat("dt", &dt)) dt=0.02;
       	if (!getparfloat("tmin", &tmin)) tmin=0.00;
       	if (!getparfloat("tmax", &tmax)) tmax=2.00;
       	if (!getparfloat("vtmin", &vtmin)) vtmin=1300;
       	if (!getparfloat("vtmax", &vtmax)) vtmax=5300;
       	if (!getparint("cdp", &cdp)) cdp=1;
	
	if(uvt) {
		tnmo[nt-1]=tmax;
		vnmo[nv-1]=vtmax;
	}
	
	trout.dt=(int)(dt*1000000.0);
	trout.cdp=cdp;
	trout.trid=32;
	trout.delrt=tmin*1000;
	trout.ns=(int)((tmax-tmin)/dt+0.05)+1;
	trout.d1=dt;
	
	memset( (void *) trout.data, (int) '\0', trout.ns*FSIZE);
	
	
	tout=alloc1float(trout.ns);
	for(i=0;i<trout.ns;i++) tout[i]=tmin+i*dt;
	
	intlin(nt,tnmo,vnmo,vnmo[0],vnmo[nt-1],trout.ns,tout,trout.data);
	
	puttr(&trout);
	
	free1float(tout);
	free1float(tnmo);
	free1float(vnmo);
	
        return EXIT_SUCCESS;
}
