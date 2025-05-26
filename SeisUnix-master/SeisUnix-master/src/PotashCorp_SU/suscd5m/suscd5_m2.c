/* suscd5_m2.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "suhdr.h"


/*********************** self documentation *****************************/
char *sdoc[] = {
" SUSCD5_M2 - perform surface consistent deconvolution solution         ",
"             files created by module one are used                      ",
"                                                                       ",
" suscd_m2       	                                                ",
"                                                                       ",
" Required parameters:                                                  ",
"                                                                       ",
" Optional parameters:                                                  ",
"	nit=7	        number of iterations in Gauss-Seidel            ",
"	fns=s.lgs	Name of file with shot residual log spectra	",
"	fng=g.lgs     	Name of file with receiver residual log spectra ",
"	fnh=h.lgs    	Name of file with offset residual log spectra   ",
"	fny=y.lgs     	Name of file with CDP residual log spectra      ",
"	fnz=z.lgs     	Name of file with azimuth residual log spectra  ",
"	fna=a.lgs     	Name of file with average amplitude spectra     ",
"                                                                       ",
"       SMOOTHING                                                       ",
"       smt=4           Degree of smoothing of input trace spectra in Hz",
"       sm1=0           Degree of smoothing of cdp spectra              ",
"       sm2=20          spectras; 0 for no smoothing.                   ",
"                       sm1 timewise sm2 linewise                       ",
"       cdpinc=1        increment of cdp numbers                        ",
"       smgap=10        largest gap before smoothing in segments        ",
"                                                                       ",
"                                                                       ",
"                                                                       ",
"                                                                       ",
NULL};

/* For detailes see:
P.W. Cary and G.A. Lorentz, Four-component surface consistent
deconvolution, 1993. Geophysics, 58: 383-392 */ 

/* Internals:
   The program reads the traces. 
   Computes the log spectra for each trace.
   The log spectra than added a temporary files
   One file per component.
   Within the files for each componenet value there is a
   slot where the traces with that compnenet valeu are accumulated.
   I.e. reciver point 101 is always added to same trace of the reciver file.
   
   Which compnent number belongs to what trace position in the file is
   stored at ST RT etc tables.
   
   There is a xross reference tabel that CT that keeps track of where
   the traces went into each file.
   I.e. 109th trace is the 109th element of the table,
   in the table ct.s filed shows that to which trace in the shot file
   was trace 109 added.

   Once all the trces are read the average spectra is computed and substracted from
   the temporary files. These contain know the deviations form the avreage spectra.
   
   Gauss-Seidel itaration is used to solve for residual spectras. 

*/ 
      


/* define the cross reference table structure */
typedef struct CREF{ int s,g,h,y,z; } cref;

/* Global variables */
int NS=0;		/* # shots */
int NG=0;		/* # receivers */
int NH=0;		/* # offset */
int NY=0;		/* # cdps */
int NZ=0;		/* # azimuth */
int N=0;		/* # traces */

/* Functions */
int fputtra(FILE *fp,segy *tp,int itr);
int fgettra2(FILE *fp,segy *tp,int itr);
void substr(FILE *fp,float *a,int n, int NT);
void smooth2d(FILE *fp, int NT, float sm1,float sm2,cwp_String key,int inc,int gap);
void cptr(FILE *fpo, FILE *pi,int n);
void zptr(FILE *fpo, FILE *fpi,int n);
void GS(FILE *fps, FILE *fpg, FILE *fph, FILE *fpy, FILE *fpz, FILE *fpa,
	FILE *tfps,FILE *tfpg,FILE *tfph, FILE *tfpy, FILE *tfpz,
	   cref *T, int n, float lambda,float sm1,float sm2,int inc,int gap,
	   int sit, int git, int hit, int yit,int zit);
void debias(FILE *fspc,FILE *fpa);
void scan_table(cref *T,int n, cwp_String bs,cwp_String ss,int ti,float *uiv,int *un);
	
int main( int argc, char *argv[] )
{
	/* Segy data constans */
	int verbose;
	
	cref *CT;		/* cross reference table */

	int dump;
	int nit;		/* Number of iteration in the Gauss-Seidel */
	float lambda;		/* Stabilization term for Gauss-Seidel */
	
	int sm2;		/* degree of smoothing for of and az x*/
	int sm1;		/* degree of smoothing for of and az t*/
	int cdpinc;		/* increment of cdp numbers */
	int smgap;		/* maximum allowed gap before smmothin done in segments */
	
	int i;			/* counter */
	
	int sit;		/* iteration flags for each components */
	int git;
	int yit;
	int hit;
	int zit;
	

	FILE *fps;		/* shot spectra file pointer */		
	FILE *fpg;		/* receiver spectra file pointer */
	FILE *fph;		/* offset spectra file pointer */
	FILE *fpy;		/* CDP spectra file pointer */
	FILE *fpz;		/* azimuth spectra file pointer */
	FILE *fpa;		/* average spectra file pointer */
	FILE *tfps;		/* temporaray files */
	FILE *tfpg;		
	FILE *tfph;		
	FILE *tfpy;		
	FILE *tfpz;		
	FILE *tfpa;		
	FILE *tfpt;		
	char *fns;		/* file names */
	char *fng;
	char *fnh;
	char *fny;
	char *fnz;
	char *fna;
      	
	/* bin sizes */
	float hb;		/* offset bin size */
	float zb;		/* azimuth bin size */
	
	initargs(argc, argv);
   	requestdoc(1);
	
	if(!getparstring("fns",&fns)) fns="s.lgs";
 	if(!getparstring("fng",&fng)) fng="g.lgs";
 	if(!getparstring("fnh",&fnh)) fnh="h.lgs";
 	if(!getparstring("fny",&fny)) fny="y.lgs";
 	if(!getparstring("fnz",&fnz)) fnz="z.lgs";
 	if(!getparstring("fna",&fna)) fna="a.lgs";
  	if(!getparint("dump",&dump)) dump=0;
   	if(!getparint("nit",&nit)) nit=7;
    	if(!getparfloat("lambda",&lambda)) lambda=1.0;
  	if(!getparint("verbose",&verbose)) verbose=0;
	if(!getparint("sm2",&sm2)) sm2=20;
	if(!getparint("sm1",&sm1)) sm1=0;
	if(!getparint("cdpinc",&cdpinc)) cdpinc=1;
	if(!getparint("smgap",&smgap))   smgap=10;

	if(!getparint("sit",&sit)) sit=1;
	if(!getparint("git",&git)) git=1;
	if(!getparint("hit",&hit)) hit=1;
	if(!getparint("yit",&yit)) yit=1;
	if(!getparint("zit",&zit)) zit=1;
	
		
   
        /* Open the tmpfiles */
	tfps = efopen("tmps.lgs","r");
	tfpg = efopen("tmpg.lgs","r");
	tfph = efopen("tmph.lgs","r");
	tfpy = efopen("tmpy.lgs","r");
	tfpz = efopen("tmpz.lgs","r");
	tfpa = efopen("tmpa.lgs","r");
	tfpt = efopen("tmptbl.txt","r");
	
	/* Scan in the Cross reference table */
	/* First 8 entries 
	   S G H Y Z N dh dz */
	/* skip text fields */
	for(i=0;i<8;i++) fscanf(tfpt," %*s");
	
	fscanf(tfpt," %d %d %d %d %d %d %f %f",&NS,&NG,&NH,&NY,&NZ,&N,&hb,&zb);
	
	fprintf(stderr," Number of shots %10d\n",NS);
	fprintf(stderr," Number of receivers %10d\n",NG);
	fprintf(stderr," Number of offsets %10d\n",NH);
	fprintf(stderr," Number of CDPs %10d\n",NY);
	fprintf(stderr," Number of azimuths %10d\n",NZ);
	fprintf(stderr," Number of traces %10d\n",N);
	fprintf(stderr," Offset bin size %10.3f\n",hb);
	fprintf(stderr," Azimuth bin size %10.3f\n",zb);

	
	CT = (cref *) malloc( (size_t) N*sizeof(cref));
	if( !CT) err(" Error in allocation of Cross Reference Table \n");
	
	for(i=0;i<N;i++)
		fscanf(tfpt," %d %d %d %d %d",&CT[i].s,&CT[i].g,&CT[i].h,&CT[i].y,&CT[i].z);
	
	
	/* compute the individual residuals with Gauss-Seidel decomposition */
	
	/* open the output files */
	fps = efopen(fns,"w+");
        fpg = efopen(fng,"w+");
        fph = efopen(fnh,"w+");
        fpy = efopen(fny,"w+");
        fpz = efopen(fnz,"w+");
        fpa = efopen(fna,"w+");
	
	
	/* initial estimates  */
	
	/* Average spectra, just copy */
	cptr(fpa,tfpa,1); 
	
	/* initial estimate of the residuals is zero */
	zptr(fps,tfps,NS); 
	zptr(fpg,tfpg,NG); 
	zptr(fph,tfph,NH); 
	zptr(fpy,tfpy,NY); 
	zptr(fpz,tfpz,NZ); 
	
	fprintf(stderr," GS\n");
	/* Gauss-Seidel iterations */
	for(i=0;i<nit;i++) {	
		GS(fps,fpg,fph,fpy,fpz,fpa,
		   tfps,tfpg,tfph,tfpy,tfpz,
		   CT,N,lambda,sm1,sm2,NINT(hb),NINT(3*hb),
		   sit,git,hit,yit,zit);
		   
		
	}

	
	/* Gracefull exit */
	efclose(tfps);
	efclose(tfpg);
	efclose(tfph);
	efclose(tfpy);
	efclose(tfpz);
	efclose(tfpa);
	efclose(tfpt);
	efclose(fps);
	efclose(fpg);
	efclose(fph);
	efclose(fpy);
	efclose(fpz);
 	efclose(fpa);
  
   return EXIT_SUCCESS;
}

void GS(FILE *fps, FILE *fpg, FILE *fph, FILE *fpy, FILE *fpz, FILE *fpa,
	FILE *tfps,FILE *tfpg,FILE *tfph, FILE *tfpy, FILE *tfpz,
	   cref *T, int n, float lambda,float sm1,float sm2,int inc,int gap,
	   int sit, int git, int hit, int yit,int zit)
/* Gauss-Seidel decomposition */
{
	
	int ti;
	int i,j,un,tt;
	int nw;
	float scale;
	double errs,errg,errh,erry,errz;
	float *uiv;
	float *ar;
	static segy trsum,trall,trtmp,trspec,trav;
	

	/* load the average spectra */
	erewind(fpa);
	fgettr(fpa,&trav);
	
	
	/* array of unique index values */
	uiv=ealloc1float(n);
	
	/* array for average updates */
	erewind(tfph);
	fgettr(tfph,&trsum);
	
	nw=trsum.ns;
	ar = ealloc1float(nw);
	
	/* rewind all the files */
	erewind(fps);  erewind(fpg);  erewind(fph);  erewind(fpy);  erewind(fpz); 
	erewind(tfps); erewind(tfpg); erewind(tfph); erewind(tfpy); erewind(tfpz); 

	fprintf(stderr," .\n");
	/* AZIMUTH */
	/* for each sum azimuth spectra stored in tfpz */
	/* remove the estimated cdp, shot, offset, receiver compnent */
	/* this results in the azimuth spectra estimate */
	ti=0;
	tt=0;
	errz=0.0;
	fgettr(tfpz,&trsum);
        memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
	if(zit) {
		do {
	              /* empty the storage */
	              memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
	              
	              /* cdp */
	              scan_table(T,n,"z","y",ti,uiv,&un);
	              for(i=0;i<un;i++) {
	        		      fgettra2(fpy,&trtmp,(int)(uiv[i]));
	        		      for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
	              }       
	              tt=un;
	              
	              /* shot */
	              scan_table(T,n,"z","s",ti,uiv,&un);
	              for(i=0;i<un;i++) {
	        		      fgettra2(fps,&trtmp,(int)(uiv[i]));
	        		      for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
	              }       
	              if(tt!=un) err(" Error in GS\n");       
	        	      
	              /* offset */
	              scan_table(T,n,"z","h",ti,uiv,&un);
	              for(i=0;i<un;i++) {
	        		      fgettra2(fph,&trtmp,(int)(uiv[i]));
	        		      for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
	              }       
	              if(tt!=un) err(" Error in GS\n");       
	        	      
	              /* reciver */
	              scan_table(T,n,"z","g",ti,uiv,&un);
	              for(i=0;i<un;i++) {
	        		      fgettra2(fpg,&trtmp,(int)(uiv[i]));
	        		      for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
	              }       
	              if(tt!=un) err(" Error in GS\n");       
	              
	              scale = (float)trsum.sdel;
	              
	              /* Do the correction */
	              fgettra2(fpz,&trspec,ti);
	              for(j=0;j<trsum.ns;j++) 
	        	      trspec.data[j]= (1.0-lambda)*trspec.data[j]+
	        			      lambda*((scale*trsum.data[j]-trall.data[j]
	        			      -scale*trav.data[j])/scale);    
	              
	              trspec.sdel=1;  
	              fputtra(fpz,&trspec,ti);
	              
	              /* Compute the error */
	              for(j=0;j<trsum.ns;j++) {
	        	      ar[j] = trsum.data[j]*scale-trspec.data[j]*scale
	        		      -trall.data[j]-scale*trav.data[j];
	        	      errz += SQR(ar[j]);	      
	              } 
	              
	              /* Update the average component*/
	              for(j=0;j<trsum.ns;j++) trav.data[j]+=ar[j]/scale;
	              
	              if(ti%500==0) fprintf(stderr,"# of Azimuth spectra done = %10d\n",ti);
	              ti++;
	
	 	} while(fgettr(tfpz,&trsum));
	 } else {
	 	cptr(fpz,tfpz,NZ);
	 }

	errz /=ti*trsum.ns;
	errz=sqrt(errz);
	
	debias(fpz,fpa);
	
	/* rewind all the files */
	erewind(fps);  erewind(fpg);  erewind(fph);  erewind(fpy);  erewind(fpz); 
	erewind(tfps); erewind(tfpg); erewind(tfph); erewind(tfpy); erewind(tfpz); 
		
	/* OFFSETS */
	/* for each sum Offset spectra stored in tfph */
	/* remove the estimated shot, receiver, cdp, azimuth and average compnent */
	/* this results in the offset spectra estimate */
	ti=0;
	tt=0;
	errh=0.0;
	fgettr(tfph,&trsum);
        memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
        memset( (void *) ar, (int) '\0', trsum.ns*FSIZE);
	if(hit) {
		 do {
		 	 /* empty the storage */
		 	 memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
		 	 
		 	 /* cdp */
		 	 scan_table(T,n,"h","y",ti,uiv,&un);
		 	 for(i=0;i<un;i++) {
		 			 fgettra2(fpy,&trtmp,(int)(uiv[i]));
		 			 for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
		 	 }	 
		 	 tt=un;
		 	 
		 	 /* receiver */
		 	 scan_table(T,n,"h","g",ti,uiv,&un);
		 	 for(i=0;i<un;i++) {
		 			 fgettra2(fpg,&trtmp,(int)(uiv[i]));
		 			 for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
		 	 }	 
		 	 if(tt!=un) err(" Error in GS\n");	 
		 		 
		 	 /* shot */
		 	 scan_table(T,n,"h","s",ti,uiv,&un);
		 	 for(i=0;i<un;i++) {
		 			 fgettra2(fps,&trtmp,(int)(uiv[i]));
		 			 for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
		 	 }	 
		 	 if(tt!=un) err(" Error in GS\n");	 
		 		 
		 	 /* azimuth */
		 	 scan_table(T,n,"h","z",ti,uiv,&un);
		 	 for(i=0;i<un;i++) {
		 			 fgettra2(fpz,&trtmp,(int)(uiv[i]));
		 			 for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
		 	 }	 
		 	 if(tt!=un) err(" Error in GS\n");	 
		 	 
		 	 scale = (float)trsum.sdel;
		 	 
		 	 /* Do the correction */
		 	 fgettra2(fph,&trspec,ti);
		 	 for(j=0;j<trsum.ns;j++) 
		 		 trspec.data[j]= (1.0-lambda)*trspec.data[j]+
		 				 lambda*((scale*trsum.data[j]-trall.data[j]
		 				 -scale*trav.data[j])/scale);	 

		 	 trspec.sdel=1;  
		 	 fputtra(fph,&trspec,ti);
		 	 
		 	 /* Compute the error */
		 	 for(j=0;j<trsum.ns;j++) {
		 		 ar[j] = trsum.data[j]*scale-trspec.data[j]*scale
		 			 -trall.data[j]-scale*trav.data[j];
		 		 errh += SQR(ar[j]);		 
		 	 } 
		 	 
		 	 /* Update the average component*/
		 	 for(j=0;j<trsum.ns;j++) trav.data[j]+=ar[j]/scale;
		 	 
		 	 if(ti%500==0) fprintf(stderr,"# of Offset spectra done = %10d\n",ti);
		 	 ti++;
	
		 } while(fgettr(tfph,&trsum));
	
		 /* if smoothing is required smooth the offset spectra */
		 if(sm1>0 || sm2>0 ) smooth2d(fph,ti,sm1,sm2,"offset",inc,gap);
	

		 errh /=ti*trsum.ns;
		 errh=sqrt(errh);
	} else {
	 	cptr(fph,tfph,NH);
	}
	
	/* debias(fph,fpa); */
	
	/* rewind all the files */
	erewind(fps);  erewind(fpg);  erewind(fph);  erewind(fpy);  erewind(fpz); 
	erewind(tfps); erewind(tfpg); erewind(tfph); erewind(tfpy); erewind(tfpz);
		
		
	/* SHOTS */
	/* for each sum shot spectra stored in tfps */
	/* remove the estimated cdp, receiver, offset, azimuth compnent */
	/* this results in the shot spectra estimate */
	ti=0;
	tt=0;
	errs=0.0;
	fgettr(tfps,&trsum);
        memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
	if(sit) {
		do {
			/* empty the storage */
			memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
			
			/* cdp */
			scan_table(T,n,"s","y",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpy,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			tt=un;
			
			/* reveiver */
			scan_table(T,n,"s","g",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpg,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
				
			/* offset */
			scan_table(T,n,"s","h",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fph,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
				
			/* azimuth */
			scan_table(T,n,"s","z",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpz,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
			
			scale = (float)trsum.sdel;
			
			/* Do the correction */
			fgettra2(fps,&trspec,ti);
			for(j=0;j<trsum.ns;j++) 
				trspec.data[j]= (1.0-lambda)*trspec.data[j]+
						lambda*((scale*trsum.data[j]-trall.data[j]
						-scale*trav.data[j])/scale);	
			
			trspec.sdel=1;	
			fputtra(fps,&trspec,ti);
			
			/* Compute the error */
			for(j=0;j<trsum.ns;j++) {
				ar[j] = trsum.data[j]*scale-trspec.data[j]*scale
					-trall.data[j]-scale*trav.data[j];
				errs += SQR(ar[j]);		
			} 
			
			/* Update the average component*/
			for(j=0;j<trsum.ns;j++) trav.data[j]+=ar[j]/scale;
			
			
			if(ti%500==0) fprintf(stderr,"# of Shot spectra done = %10d\n",ti);
			ti++;
	
		} while(fgettr(tfps,&trsum));

		errs /=ti*trsum.ns;
		errs=sqrt(errs);
	} else {
	 	cptr(fps,tfps,NS);
	}
	
	/* rewind all the files */
	erewind(fps);  erewind(fpg);  erewind(fph);  erewind(fpy);  erewind(fpz); 
	erewind(tfps); erewind(tfpg); erewind(tfph); erewind(tfpy); erewind(tfpz); 
		
	/* RECEIVERS */
	/* for each sum receiver spectra stored in tfpg */
	/* remove the estimated cdp, shot, offset, azimuth compnent */
	/* this results in the receiver spectra estimate */
	ti=0;
	tt=0;
	errg=0.0;
	fgettr(tfpg,&trsum);
        memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
	if(git) {
		do {
			/* empty the storage */
			memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
			
			/* cdp */
			scan_table(T,n,"g","y",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpy,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			tt=un;
			
			/* shot */
			scan_table(T,n,"g","s",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fps,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");
			
			/* offset */
			scan_table(T,n,"g","h",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fph,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
				
			/* azimuth */
			scan_table(T,n,"g","z",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpz,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
			
			scale = (float)trsum.sdel;
			
			/* Do the correction */
			fgettra2(fpg,&trspec,ti);
			for(j=0;j<trsum.ns;j++) 
				trspec.data[j]= (1.0-lambda)*trspec.data[j]+
						lambda*((scale*trsum.data[j]-trall.data[j]
						-scale*trav.data[j])/scale);	
			
			trspec.sdel=1;	
			fputtra(fpg,&trspec,ti);
			
			/* Compute the error */
			for(j=0;j<trsum.ns;j++) {
				ar[j] = trsum.data[j]*scale-trspec.data[j]*scale
					-trall.data[j]-scale*trav.data[j];
				errg += SQR(ar[j]);		
			} 
			
			/* Update the average component*/
			for(j=0;j<trsum.ns;j++) trav.data[j]+=ar[j]/scale;
			
			
			if(ti%500==0) fprintf(stderr,"# of Reciver spectra done = %10d\n",ti);
			ti++;
	
		} while(fgettr(tfpg,&trsum));

		errg /=ti*trsum.ns;
		errg=sqrt(errg);
	} else {
	 	cptr(fpg,tfpg,NG);
	}
		
	/* rewind all the files */
	erewind(fps);  erewind(fpg);  erewind(fph);  erewind(fpy);  erewind(fpz); 
	erewind(tfps); erewind(tfpg); erewind(tfph); erewind(tfpy); erewind(tfpz);
		

	/* CDPS */
	/* for each sum CDP spectra stored in tfpy */
	/* remove the estimated shot, receiver, offset azimuth compnent */
	/* this results in the CDP spectra estimate */
	ti=0;
	tt=0;
	erry=0.0;
	fgettr(tfpy,&trsum);
        memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
	if(yit) {
		do {

			/* empty the storage */
			memset( (void *) trall.data, (int) '\0', trsum.ns*FSIZE);
			
			/* shots */
			scan_table(T,n,"y","s",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fps,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			tt=un;
			
			/* reveiver */
			scan_table(T,n,"y","g",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpg,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
				
			/* offset */
			scan_table(T,n,"y","h",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fph,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
				
			/* azimuth */
			scan_table(T,n,"y","z",ti,uiv,&un);
			for(i=0;i<un;i++) {
					fgettra2(fpz,&trtmp,(int)(uiv[i]));
					for(j=0;j<trsum.ns;j++) trall.data[j] +=trtmp.data[j];
			}	
			if(tt!=un) err(" Error in GS\n");	
			
			scale = (float)trsum.sdel;
			
			/* Do the correction */
			fgettra2(fpy,&trspec,ti);
			for(j=0;j<trsum.ns;j++) 
				trspec.data[j]= (1.0-lambda)*trspec.data[j]+
						lambda*((scale*trsum.data[j]-trall.data[j]
						-scale*trav.data[j])/scale);	
			
			trspec.sdel=1;	
			fputtra(fpy,&trspec,ti);
			
			/* Compute the error */
			for(j=0;j<trsum.ns;j++) {
				ar[j] = trsum.data[j]*scale-trspec.data[j]*scale
					-trall.data[j]-scale*trav.data[j];
				erry += SQR(ar[j]);		
			} 
			
			/* Update the average component*/
			for(j=0;j<trsum.ns;j++) trav.data[j]+=ar[j]/scale;
			
			if(ti%500==0) fprintf(stderr,"# of CDP spectra done = %10d\n",ti);
			ti++;
	
		} while(fgettr(tfpy,&trsum));

		erry /=ti*trsum.ns;
		erry=sqrt(erry);
	} else {
	 	cptr(fpy,tfpy,NY);
	}

	/* Error estimate */
	fprintf(stderr," Realtive Error= %15.10e\n",(errs+errg+errh+erry+errz)/5.0);
	
	/* Change in the average spectra component */
	for(j=0;j<trsum.ns;j++)
			ar[j] /=n; 
			
	/* Update the sum of componenets and the average spectra with the change */
	erewind(tfps); erewind(tfpg); 
	erewind(tfph); erewind(tfpy); 
	erewind(tfpz); erewind(fpa);

	/* Copy tha accumulated total change to stdout */
	puttr(&trav);
	fputtr(fpa,&trav);
	
	/* Free the temporary array */
	free1float(uiv);
	free1float(ar);
}

int fputtra(FILE *fp,segy *tp,int itr)
{
	int erro;
	
	erro=efseek(fp,(long) itr*((*tp).ns*FSIZE+240),SEEK_SET);
	fputtr(fp,tp);
	/* go to the end by default */
	erro = efseek(fp,0,SEEK_END);
	return(erro);
}

int fgettra2(FILE *fp,segy *tp,int itr)
{
	int erro;
	
	erro=efseek(fp,(long) itr*((*tp).ns*FSIZE+HDRBYTES),SEEK_SET);
	fgettr(fp,tp);
	/* go to the end by default */
	erro = efseek(fp,0,SEEK_END);
	return(erro);
}

int st(int *T,int val, int *n)
/* search a table for a specific entry */
{
	int i;
	
	
	for(i=0;i<*n;i++) {
		if(val==T[i]) return(i);
	}
	/* not in the table, therefore new entry */
	T[*n]=val;
	*n+=1;
	return(-1);
}

void substr(FILE *fp,float *a,int n, int NT)
{
	int i,j;
	segy tr;
	erewind(fp);
	
	i=0;	
	for(i=0;i<NT;i++){
		fgettra2(fp,&tr,i);
		for(j=0;j<n;j++) tr.data[j] -= a[j];  /* Make difference */
		fputtra(fp,&tr,i);
	}
	efflush(fp);
}

void smooth2d(FILE *fp, int NT, float sm1,float sm2,cwp_String key,int inc,int gap)
{
	int j,i;
	segy tr;
	float **data;
	float **data_sm;
	int *trind;
	float *trival;
	float *trival_sort;
	int *trivalseg_ind;
	int nseg;

	cwp_String type;
	int indx;
        Value val;
	
	if (NT==1) return;
	
	
	/* get the header type and indx value */
	type = hdtype(key);
        indx = getindex(key);
	
	trival=ealloc1float(NT);
	trival_sort=ealloc1float(NT);
	trivalseg_ind=ealloc1int(NT);
	trind=ealloc1int(NT);
	
	erewind(fp);
	fgettra2(fp,&tr,0);
	gethval(&tr, indx, &val);
	trival[0] = vtof(type,val);
	trind[0]=0;
	
	data=ealloc2float(tr.ns,NT);
	data_sm=ealloc2float(tr.ns,NT);
	
	memcpy((void *) data[0],(const void *) tr.data,tr.ns*FSIZE);	
	
	for(j=1;j<NT;j++) {
		fgettra2(fp,&tr,j);
		gethval(&tr, indx, &val);
		trival[j] = vtof(type,val);
		trind[j]=j;
		memcpy((void *) data[j],(const void *) tr.data,tr.ns*FSIZE);	
	} 	
	
	/* Sorting according to key */
	qkisort (NT,trival,trind);
	
	/* Load data into data_sm where it is going to be smoothed */
	for(j=0;j<NT;j++) 
		for(i=0;i<tr.ns;i++) 
			data_sm[j][i] = data[trind[j]][i];

	/* Create the sorted trival array */
	for(j=0;j<NT;j++) 
		trival_sort[j] = trival[trind[j]];
	
	/* Get the segment boundaries from trival_sort*/	
	segm_bnd(trival_sort,NT,(float)inc,(float)gap,trivalseg_ind,&nseg);
	
	/*smooth segments separately */
	{ int iseg,ins,ine;
				
		for(iseg=0;iseg<nseg;iseg++){
			ins=trivalseg_ind[iseg];
			if(iseg==nseg-1) { 
				ine=NT;
			} else {
				ine=trivalseg_ind[iseg+1]-1;
			}
			fprintf(stderr,"Smoothing segment# %d from trace %d to trace %d\n",
				iseg,ins,ine);
			dlsq_smoothing(tr.ns,NT, 0,tr.ns, ins,ine, sm1,sm2,0,data_sm);
		}
	}
	
	erewind(fp);
	
	for(j=0;j<NT;j++) {
		fgettra2(fp,&tr,trind[j]);
		memcpy((void *) tr.data,(const void *) data_sm[j],tr.ns*FSIZE);	
		fputtra(fp,&tr,trind[j]);
	} 	
	efflush(fp);
	free2float(data);
	free2float(data_sm);
	free1float(trival);
	free1float(trival_sort);
	free1int(trivalseg_ind);
	free1int(trind);
}

void cptr(FILE *fpo, FILE *fpi,int n)
{
	segy tri;
	segy tro;
	int i;
	
	erewind(fpi);
	erewind(fpo);

	for(i=0;i<n;i++) {
		fgettr(fpi,&tri);
		memcpy( (void *) &tro, (const void *) &tri, tri.ns*FSIZE+HDRBYTES);
	        /* for(i=0;i<tr.ns; i++) tr.data[i] -= data[i]; */
		fputtr(fpo,&tro);
	}
}

void zptr(FILE *fpo, FILE *fpi,int n)
{
	segy tr;
	int i;
	
	erewind(fpi);
	erewind(fpo);

	for(i=0;i<n;i++) {
		fgettr(fpi,&tr);
                memset( (void *) tr.data, (int) '\0', tr.ns*FSIZE);
		fputtr(fpo,&tr);
	}
}


void debias(FILE *fspc,FILE *fpa)
{
	segy tr,atr;
	int i,iw,ntr,nw;
	double *mean;

	erewind(fspc);
	erewind(fpa);
	fgettr(fspc,&tr);
	fgettr(fpa,&atr);
	
	nw =tr.ns;
	mean = ealloc1double(nw);
	memset( (void *) mean, (int) '\0', nw*DSIZE);
	
	ntr=0;
	do{
		for(iw=0;iw<nw;iw++) 
			mean[iw] +=tr.data[iw];
		ntr++;
	}while(fgettr(fspc,&tr)); 

	for(iw=0;iw<nw;iw++) 
		mean[iw] /=(double)ntr;
		
	erewind(fspc);
	erewind(fpa);
	
	for(i=0;i<ntr;i++) {
		fgettra2(fspc,&tr,i);
		for(iw=0;iw<nw;iw++) { 
			tr.data[iw] -= (float)mean[iw];
			atr.data[iw] += (float)mean[iw];
		}
		fputtra(fspc,&tr,i);		
	}
	fputtr(fpa,&atr);		
	free1double(mean);
}

void scan_table(cref *T,int n, cwp_String bs,cwp_String ss,int ti,float *uiv,int *un)
{
/* Return the location and the total number of entries found in the table
   for the specified search */


#define Y 1
#define H 2
#define S 3
#define G 4
#define Z 5
		
		int sws=7,swf=7;
		int scmp=0;
		int hit,i;
        	
		if      (STREQ(bs, "y"))        sws = Y;
        	else if (STREQ(bs, "h"))     	sws = H;
        	else if (STREQ(bs, "s"))     	sws = S;
        	else if (STREQ(bs, "g"))     	sws = G;
        	else if (STREQ(bs, "z"))     	sws = Z;
		
		if      (STREQ(ss, "y"))        swf = Y;
        	else if (STREQ(ss, "h"))     	swf = H;
        	else if (STREQ(ss, "s"))     	swf = S;
        	else if (STREQ(ss, "g"))     	swf = G;
        	else if (STREQ(ss, "z"))     	swf = Z;
		
		hit=0;
		for(i=0;i<n;i++) {
			switch(sws) {
			
			case Y : scmp=T[i].y;
			break;
			
			case H : scmp=T[i].h;
			break;
			
			case S : scmp=T[i].s;
			break;
			
			case G : scmp=T[i].g;
			break;
			
			case Z : scmp=T[i].z;
			break;
			default:  
                                err("mysterious operation=\"%d\"", sws);
                        }
		
			if(scmp==ti) { 
				switch(swf) {
				
				case Y : uiv[hit]=(float)T[i].y;
				break;
				
				case H : uiv[hit]=(float)T[i].h;
				break;
				
				case S : uiv[hit]=(float)T[i].s;
				break;
				
				case G : uiv[hit]=(float)T[i].g;
				break;
				
				case Z : uiv[hit]=(float)T[i].z;
				break;
				default:  
                                	err("mysterious operation=\"%d\"", swf);
                        	}
				hit++;
			}

		}
		*un=hit;
		
#undef Y
#undef H
#undef S
#undef G
#undef Z
}
