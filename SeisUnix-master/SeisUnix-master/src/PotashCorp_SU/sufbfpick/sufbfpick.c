/* sufbpick.c */
/* B.Nemeth */

#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {" SUFBFPICK - First break auto picker               ",
"                                                                  ",
"                                                                  ",
"          sufbfpick < infile >outfile                              ",
"                                                                  ",
"          keyg=ep                                                 ",
"          keyp=unscale	header word where to store picks           ",
"                                                                  ",
"          Template                                                ",
"          o=                                                      ",
"          t=                                                      ",
"                                                                  ",
"          peak=1	-1 trough                                   ",
"          thrs=5.0                                                ",
"          window=.02                                                ",
"                                                                  ",
NULL};
   
/* Segy data constans */
segy *trp;				/* SEGY trace */
segy trtp;				/* SEGY trace */
int main( int argc, char *argv[] )
{
	
	segy **rec_o;           /* trace header+data matrix */  
        
	cwp_String keyg;
        cwp_String typeg;              
	Value valg;
                   	
        int first=0;            /* true when we passed the first gather */
        int ng=0;
        float dt;
        int nt;
        int ntr;

	unsigned int np;	/* Number of points in pick template */
	float *t;		/* array defining pick template times */
	float *o;		/* array defining pick template offsets */
	
	float peak;
	float thrs;
	float window;
	int iwindow;
	int *itimes;
	int *itimes2;
	int phs2=1;
	
	FILE *ttp;
		
	initargs(argc, argv);
   	requestdoc(1);
        
	if (!countparval("t")) {
                np=2;
        } else {
                np=countparval("t");
        }
        
	t  = ealloc1float(np);
	o  = ealloc1float(np);
	
	if( np == countparval("o")) {
                getparfloat("t",t);
                getparfloat("o",o);
        } else {
		err(" t and o has different number of elements\n");
	}

		
        if (!getparstring("keyg", &keyg)) keyg ="ep";
        if (!getparfloat("peak",&peak)) peak =1;
	if(fabs(peak)!=1.0) peak=SGN(peak)*1.0;
	
        if (!getparfloat("window",&window)) window =0.02;
        if (!getparfloat("thrs",&thrs)) thrs = 5.0;
        
	/* get information from the first header */
        rec_o = get_gather(&keyg,&typeg,&valg,&nt,&ntr,&dt,&first);
	
	iwindow=NINT(window/dt);
	
        if(ntr==0) err("Can't get first record\n");
	do {
		ng++;
		
		itimes = ealloc1int(ntr);
		itimes2 = ealloc1int(ntr);
		
		/* Phase 1 */
		/* Loop through traces */
		{ unsigned int itr,ifbt;
		unsigned int it,itp,itpeak;
		  float fbt;
		  float rms_pre;
		  float amp;
		  float *wnd=NULL;
		  float offset;
		  float FD;
		  int nm,im;
		  float *fd,*dfd;
		
		  nm = nt-iwindow;
		  fd = ealloc1float(nm);
		  dfd = ealloc1float(nm-1);
		  
		  for(itr=0;itr<ntr;itr++) {
		  
		  	/* Linear inperpolation of estimtated fb time */
			offset =(*rec_o[itr]).offset; 
		  	intlin(np,o,t,t[0],t[np-1],1,&offset,&fbt);
		  	ifbt = NINT(fbt/dt);
		  	fprintf(stderr," .\n");
			
		  	for(im=0;im<nm;im++) {
				fd[im]=fract_D(&(*rec_o[itr]).data[im],iwindow,1,iwindow/3,1);
				/*fd[im]=snrm2(iwindow,&(*rec_o[itr]).data[im],1); */
				fprintf(stderr," %f\n",fd[im]);
			}
		  	
			for(im=0;im<nm-1;im++) {
				dfd[im]=fd[im]-fd[im+1];
			}
			(*rec_o[itr]).unscale = dt*isamaxs(nm-1,dfd,1);
		  }
		  
		  
		  free1float(fd);
		  free1float(dfd);
		}
		
	       free1int(itimes);
	       free1int(itimes2);
	       rec_o = put_gather(rec_o,&nt,&ntr);
	       rec_o = get_gather(&keyg,&typeg,&valg,&nt,&ntr,&dt,&first);
	} while(ntr);
	return EXIT_SUCCESS;

}
