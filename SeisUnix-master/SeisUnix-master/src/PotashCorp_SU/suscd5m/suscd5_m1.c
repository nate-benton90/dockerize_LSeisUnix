/* suscd5_m1.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "suhdr.h"

#define MS	3000
#define MG	10000
#define MH	5000
#define MY	100000
#define MZ	720
#define MN      1000000
#define BLANK	-9999
#define AMPSP(c) rcabs(c)

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUSCD5_M1 - perform surface consistent deconvolution module 1         ",
"             Create the spectra files for module_2                     ",
"                                                                       ",
" suscd5_m1< stdin	>stdout                                         ",
"                                                                       ",
" Required parameters:                                                  ",
"        none                                                           ",
"       ts=		array of window start times                     ",
"       te=		array of window end times                       ",
"       os=		array of offsets                                ",
"                       The number of elements in these has to be       ",
"                       monotonically increasing.                       ",
"                                                                       ",
" Optional parameters:                                                  ",
"	fmax=NYQUIST	Maximum frequency in computation                ",
"	fmin=0		Minimum frequency in computation                ",
"	hb=15		Size of offset bins                             ",
"	zb=5		Size of azimuth bins                            ",
"       fftpad=25.0	% fftpadding in trace length percentage         ",
"                                                                       ",
"	WINDOW PARAMETERS FOR AVERAGE SPECTRA ESTIMATION           	",
"                                                                       ",
"       vmin=500        Minimum velocity of window                      ",
"       vmax=1600       Maximum velocity of window                      ",
"       atmax=Full_trace Maximum time of window                         ",
"       atmin=0.0        Mimimum time of window                         ",
"                                                                       ",
"       SMOOTHING                                                       ",
"       smt=4           Degree of smoothing of input trace spectra in Hz",
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
      


#define LOOKFAC 1       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */
#define TINY exp(-200)   



/* define the cross reference table structure */
typedef struct CREF{ int s,g,h,y,z; } cref;

/* Functions */
int fputtra(FILE *fp,segy *tp,int itr);
int fgettra2(FILE *fp,segy *tp,int itr);
int st(int *T,int val, int *n);
void put_tr(FILE *fp,segy *tr,int ind,int *fpp,int NF);
void scl(FILE *fp, int NT, float lmb);
void smooth_spc(float *ftmp,int *nflim,float *filter,int ismt);
	
/* Global variables */
int NS=0;		/* # shots */
int NG=0;		/* # receivers */
int NH=0;		/* # offset */
int NY=0;		/* # cdps */
int NZ=0;		/* # azimuth */
int N=0;		/* # traces */

int main( int argc, char *argv[] )
{
	/* Segy data constans */
	static segy tr;				/* SEGY trace */
	static segy sptr;			/* SEGY trace spectra */
	int nt;                 /* number of time samples               */
	int i;			/* counter */
	int verbose;

	int SP=0;		/* shots file position pointers */
	int GP=0;		/* receivers file position pointers */
	int HP=0;		/* offset file position pointers */
	int YP=0;		/* cdps file position pointers */
	int ZP=0;		/* azimuth file position pointers */

	
	
	int *ST;	        /* shots file table*/
	int *GT;	        /* receivers file table*/
	int *HT;	        /* offset file table */
	int *YT;	        /* cdps file table */
	int *ZT;	        /* azimuth file table */
	cref *CT;		/* cross reference table */
	int ind;
	
	float *rt;     /* real trace                           */
        complex *spc;  /* complex transformed trace            */
        int nfft;
	int nfby2p1;            /* nfft/2 + 1                           */
	int nflim;
	float fmax;
	float fmin;
	int fi0;		/* first spectral value */
	float smt;		/* trace spectra smoother in Hz */
	int  ismt;		/* trace spectra smoother in smaples */
	float *sm_filter;	/* smoothing filter */
	int sm2;		/* degree of smoothing for of and az x*/
	int sm1;		/* degree of smoothing for of and az t*/
	int cdpinc;		/* increment of cdp numbers */
	int smgap;		/* maximum allowed gap before smmothin done in segments */
	float *pw;		/* power spectra */
	float *aw;		/* averaged power spectra */
	double *awd;		/* averaged power spectra */
	float d1;
	float dt;
	float snfft;		/* fft scaler */
	float fftpad;		/* fft pad */
	

	FILE *tfps;		/* temporaray files */
	FILE *tfpg;		
	FILE *tfph;		
	FILE *tfpy;		
	FILE *tfpz;
	FILE *tfpa;
	FILE *tfpt;		
     	
	/* bin sizes */
	float hb;		/* offset bin size */
	float zb;		/* azimuth bin size */

	/* Average spectra window parameters */
	float vmin;
	float vmax;
	float atmin;
	float atmax;
	int ias;
	int iae; 
	
	
	/* window parameters */
	float *ts=NULL;		/* array of start time values */
	int ntw;
	float *te=NULL;		/* array of end time values */
	float *o=NULL;		/* array of offset values */
	int now;
	int wpp;		/* window parameter pointer */
	float tstart;		/* window start time for trace */
	float tend;		/* window end time for trace */
	int its;		/* .... in samples ....*/ 
	int ite;
	float tr_o;		/* abs trace offset */
	float m;		/* slope of linear int and exp */
	float tmin;
	
	/* Resampling factor */
	
	initargs(argc, argv);
   	requestdoc(1);
	
    	if(!getparfloat("hb",&hb)) hb=15;
    	if(!getparfloat("zb",&zb)) zb=5;
    	if(!getparfloat("fftpad",&fftpad)) fftpad=25.0;
	fftpad /=100.0;
  	if(!getparint("verbose",&verbose)) verbose=0;
	
	/* Average spectra window */
    	if(!getparfloat("vmin",&vmin)) vmin=500;
    	if(!getparfloat("vmax",&vmax)) vmax=1600;
    	if(!getparfloat("atmin",&atmin)) atmin=0.0;
    	if(!getparfloat("atmax",&atmax)) atmax=-1.0;
	
	
	
	/* Window parameters */
	if (!(ntw = countparval("ts")))  MUSTGETPARFLOAT("ts", ts);
	if (!(now = countparval("te")))  MUSTGETPARFLOAT("te", te);
	if(ntw!=now) err("Number of start times must equal to the number of end times");
	
	if (!(now = countparval("o")))  MUSTGETPARFLOAT("o", o);
	if(ntw!=now) err("Number of times must equal to the number of offsets");
		
	ts = ealloc1float(ntw); 
	te = ealloc1float(ntw); 
	o = ealloc1float(ntw); 
        getparfloat("ts", ts);
        getparfloat("te", te);
	getparfloat("o", o);
   
        /* Open the tmpfiles */
	tfps = efopen("tmps.lgs","w+");
	tfpg = efopen("tmpg.lgs","w+");
	tfph = efopen("tmph.lgs","w+");
	tfpy = efopen("tmpy.lgs","w+");
	tfpz = efopen("tmpz.lgs","w+");
	tfpa = efopen("tmpa.lgs","w+");
	tfpt = efopen("tmptbl.txt","w+");
	
	/* allocate space for tables */
	ST = ealloc1int(MS);
	GT = ealloc1int(MG);
	HT = ealloc1int(MH);
	YT = ealloc1int(MY);
	ZT = ealloc1int(MZ);
	
	CT = (cref *) malloc( (size_t) MN*sizeof(cref));
	if( !CT) err(" Error in allocation of Cross Reference Table \n");
	for(i=0;i<MN;i++)
		CT[i].s=CT[i].g=CT[i].h=CT[i].y=CT[i].z=BLANK;
	
	/* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
        nt = tr.ns;
        if (!getparfloat("dt", &dt)) dt = ((double) tr.dt)/1000000.0;
        if (!dt) {
                dt = .002;
                warn("dt not set, assumed to be .002");
        }
	
	if(!getparfloat("fmax",&fmax)) fmax=0.5/dt;
	if(!getparfloat("fmin",&fmin)) fmin=0;
	if(!getparint("sm2",&sm2)) sm2=20;
	if(!getparint("sm1",&sm1)) sm1=0;
	if(!getparint("cdpinc",&cdpinc)) cdpinc=1;
	if(!getparint("smgap",&smgap))   smgap=10;
	
	if(!getparfloat("smt",&smt)) smt=4.0;

	
	/* Set up pfa fft */
        nfft = npfar(NINT(nt*(1.0+fftpad)));
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                 err("Padded nt=%d--too big", nfft);
        nfby2p1 = nfft/2 + 1;
	snfft = 1.0/nfby2p1;
        d1 = 1.0/(nfft*dt);
        fi0=NINT(fmin/d1);
	nflim=NINT(fmax/d1)-fi0;
	
	/* intergers smoother length */
	ismt = NINT((smt/0.5)/d1);
	if(!ISODD(ismt)) ismt++;
	if(verbose) fprintf(stderr," Smoothing filter integer length %d\n",ismt);
	
	/* smoothing filter */
	sm_filter = ealloc1float(ismt);
	rwa_smoothing_filter (2,ismt/2,ismt/2,sm_filter);

	/* allocate storage space for FFT*/
	rt = ealloc1float(nfft);
        pw = ealloc1float(nfby2p1);
        awd = ealloc1double(nfby2p1);
        aw = ealloc1float(nfby2p1);
        spc = ealloc1complex(nfby2p1);
        
	/* zero out the average spectra */        
	memset( (void *) awd, (int) '\0', nfby2p1*DSIZE);
	
	do {
			/* window selection */
			tr_o=fabs(tr.offset);
			tmin=tr.delrt/1000.0;
			wpp=0;
			if(tr_o < o[wpp]) {
				
				m=(ts[wpp+1]-ts[wpp])/(o[wpp+1]-o[wpp]);
				tstart = m*tr_o+ts[wpp]-(o[wpp]*m);
				
				m=(te[wpp+1]-te[wpp])/(o[wpp+1]-o[wpp]);
				tend = m*tr_o+te[wpp]-(o[wpp]*m);
			
			} else if (tr_o > o[wpp+ntw-1]) {
				
				m=(ts[wpp+ntw-1]-ts[wpp+ntw-2])/
					(o[wpp+ntw-1]-o[wpp+ntw-2]);
				tstart = m*tr_o+ts[wpp+ntw-1]-(o[wpp+ntw-1]*m);

				m=(te[wpp+ntw-1]-te[wpp+ntw-2])/
					(o[wpp+ntw-1]-o[wpp+ntw-2]);
				tend = m*tr_o+te[wpp+ntw-1]-(o[wpp+ntw-1]*m);

			} else {
				while(tr_o>o[wpp]) {
					wpp++;
				}
				m=(ts[wpp]-ts[wpp-1])/
					(o[wpp]-o[wpp-1]);
				tstart = m*tr_o+ts[wpp]-(o[wpp]*m);
				
				m=(te[wpp]-te[wpp-1])/
					(o[wpp]-o[wpp-1]);
				tend = m*tr_o+te[wpp]-(o[wpp]*m);
			}	
			
			its = MAX((tstart-tmin)/dt,0);
			ite = MIN((tend-tmin)/dt,nt-1);
			if(its>ite) its=ite;
			
			/* average spectra window */
			if(atmax>-1.0) {
				ias = MIN(MAX(NINT(MAX(tr_o/vmax,atmin)/dt),its),ite);
				iae = MAX(MIN(NINT(MIN(tr_o/vmin,atmax)/dt),ite),its);
				if(ias>iae) ias=iae;
			} else {
				ias=0;
				iae=nt;
			}


			if(verbose) 
				fprintf(stderr," suscd5 windows: off. ds de avs ave, %f %f %f %f %f\n",
			                tr_o,its*dt,ite*dt,ias*dt,iae*dt);
			
                      	/* Save the headers */
			memcpy( (void *) &sptr, (const void *) &tr, HDRBYTES);

		      	/* Get trace trace window for residuals */
		      	/* Load trace into rt (zero-padded) */
		      	memset( (void *) rt, (int) '\0', nfft*FSIZE);
                      	memcpy( (void *) &rt[its], (const void *) &tr.data[its], (ite-its)*FSIZE);

                      	sscal(nfft,snfft,rt,1);
			pfarc(1, nfft, rt, spc);

			/* Log spectra */
			{ complex ctmp1;
		      	  float *ftmp;
			  
			  	ftmp = ealloc1float(nflim);
				
		      		for (i = 0; i < nflim; ++i)  {
		      			ctmp1=cmplx(spc[i+fi0].r,spc[i+fi0].i);
		      			ftmp[i]=AMPSP(ctmp1);
		      		}
				/* smooth the spectra */
				if(ismt) {
		      			conv (ismt,-ismt/2,sm_filter,nflim,0,ftmp,nflim,0,pw);

					/* compute log spectra */ 
					for (i = 0; i < nflim; ++i)
					  	pw[i]=log(pw[i]+TINY);
				} else {
					/* compute log spectra */ 
					for (i = 0; i < nflim; ++i)
					  	pw[i]=log(ftmp[i]+TINY);
				}
					
				
				free1float(ftmp);
		      	}
                      	
	
			/* Get the average spectra from the window  ; Not log yet*/
		      	memset( (void *) rt, (int) '\0', nfft*FSIZE);
			memcpy( (void *) &rt[ias], (const void *) &tr.data[ias], (iae-ias)*FSIZE);
			
                      	sscal(nfft,snfft,rt,1);
			pfarc(1, nfft, rt, spc);
			
		      	/* Average amplitude spectra; */
			{ complex ctmp1;
		      	  float *ftmp;
			  float *ftmp1;
			  	
				ftmp = ealloc1float(nflim);
				ftmp1 = ealloc1float(nflim);
		      		
				for (i = 0; i < nflim; ++i)  {
		      			ctmp1=cmplx(spc[i+fi0].r,spc[i+fi0].i);
		      			ftmp[i]=AMPSP(ctmp1);
		      		}
				/* smooth the spectra */
				if(ismt) {
		      			conv (ismt,-ismt/2,sm_filter,nflim,0,ftmp,nflim,0,ftmp1);
					for (i = 0; i < nflim; ++i)  
						awd[i]+=(double)ftmp1[i];
				} else {
					for (i = 0; i < nflim; ++i)  
						awd[i]+=(double)ftmp[i];
				}
				
				free1float(ftmp);
				free1float(ftmp1);
		      	}

		      	/* set headers */
		      	sptr.ns=nflim;
		      	sptr.d1=d1;
		      	sptr.f1=fmin;
		      	sptr.sdel = 0;  /* Number of stacks is stored here !!! */
	
		      	/* put back to the trace */
                      	memcpy( (void *) sptr.data, (const void *) pw, sptr.ns*FSIZE);
	
		      	/* Put to the shot file */
		      	ind = st(ST,sptr.ep,&NS);
 		      	put_tr(tfps,&sptr,ind,&SP,NS);
		      	CT[N].s=SP;
/*		      	fprintf(stdout," %d %d\n",SP,N); */
	
		      	/* Put to the receiver file */
		      	ind = st(GT,sptr.sdepth,&NG);
 		      	put_tr(tfpg,&sptr,ind,&GP,NG);
		      	CT[N].g=GP;
	
		      	/* Put to the offset file */
		      	/* Do the binning by doing an integer divison
		      	 on the offset field */
		      	ind = st(HT,NINT((float)sptr.offset/hb),&NH);
 		      	put_tr(tfph,&sptr,ind,&HP,NH);
		      	CT[N].h=HP;
/*		      	fprintf(stdout," %d %d\n",HP,N); */
	
		      	/* Put to the cdp file */
		      	ind = st(YT,sptr.cdp,&NY);
 		      	put_tr(tfpy,&sptr,ind,&YP,NY);
		      	CT[N].y=YP;
	
		      	/* Put to the azimuth file */
		      	/* Do the binning by doing an integer divison
		      	 on the azimuth field */
		      	ind = st(ZT,NINT(sptr.otrav/zb),&NZ);
 		      	put_tr(tfpz,&sptr,ind,&ZP,NZ);
		      	CT[N].z=ZP;
			
		      	if(N%500==0) fprintf(stderr," Number of traces read = %10d\n",N);
	
		      	N++;
/*			fprintf(stdout," %f %f\n",awd[100],spc[100].r); 
			memcpy( (void*) tr.data, (const void*) aw ,nflim*FSIZE);
			tr.ns=nflim;
			tr.d1=d1;
		      	puttr(&tr); */
	} while(gettr(&tr));
	fprintf(stderr," Number of Traces= %d\n",N);
	fprintf(stderr," Number of Shots= %d\n",NS);
	fprintf(stderr," Number of Receivers= %d\n",NG);
	fprintf(stderr," Number of Offset= %d\n",NH);
	fprintf(stderr," Number of CDP= %d\n",NY);
	fprintf(stderr," Number of Azimuth= %d\n",NZ);
/*	for(i=0;i<N;i++) fprintf(stderr," %d %d %d %d\n",
			CT[i].s,CT[i].g,CT[i].h,CT[i].y);
*/	
	
	
	/* Compute the average log spectra */ 
	for(i=0;i<nflim;i++) { 
		aw[i] = (float) log(awd[i]/N+TINY);
	}
	
	/* Save the average spectra */
	memcpy( (void *) tr.data, (const void *) aw, nflim*FSIZE);
	tr.ns=nflim;
	tr.d1= d1;
	tr.f1=fmin;
	tr.sdel=N;
	fputtr(tfpa,&tr);
	
	/* Save the cross reference table */
	fprintf(tfpt," %15s %15s %15s %15s %15s %15s %15s %15s\n",
			 "S","G","H","Y","Z","N","dH","dZ");
	fprintf(tfpt," %15d %15d %15d %15d %15d %15d %15.3f %15.3f\n",
			 NS,NG,NH,NY,NZ,N,hb,zb);
	
	for(i=0;i<N;i++) {
		fprintf(tfpt," %15d %15d %15d %15d %15d\n",
			 	CT[i].s,CT[i].g,CT[i].h,CT[i].y,CT[i].z);
	}
	
	/* Gracefull exit */
	free1int(ST);
	free1int(GT);
	free1int(HT);
	free1int(YT);
	free1int(ZT);
	free1float(rt);
        free1float(pw);
        free1float(aw);
        free1double(awd);
        free1complex(spc);
	free1float(sm_filter);

	efclose(tfps);
	efclose(tfpg);
	efclose(tfph);
	efclose(tfpy);
	efclose(tfpz);
 	efclose(tfpa);
 	efclose(tfpt);
  
   return EXIT_SUCCESS;
}
	
void put_tr(FILE *fp,segy *tp,int ind,int *fpp,int NF)
{
	int i;
	int tmp;
	static segy tr_o;			/* old SEGY trace */
	static double *tmpa;

	if(ind==-1) {
		
		/* new entry */
		(*tp).sdel=1;	/* number of vert. stacks */
		efseek(fp,(long) 0,SEEK_END);
		fputtr(fp,tp);
		*fpp=MAX(NF-1,0);
	}  else { 
		tmpa= ealloc1double((*tp).ns);
		
		fgettra2(fp,&tr_o,ind);
		tmp = tr_o.sdel;
		for(i=0;i<(*tp).ns;i++) tmpa[i] = (double)tr_o.data[i]*(float)tmp;
		for(i=0;i<(*tp).ns;i++) tmpa[i] += (double) (*tp).data[i];
		tr_o.ns = (*tp).ns;
		tr_o.sdel = tmp+1;	/* number of vert. stacks */
		for(i=0;i<(*tp).ns;i++) tr_o.data[i] = (float)(
			tmpa[i]/(float)tr_o.sdel);
		fputtra(fp,&tr_o,ind);
		*fpp = ind;
		
		free1double(tmpa);
	} 
	efflush(fp);
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



void cptr(FILE *fpo, FILE *fpi, float *data)
{
	segy tr;
	int i;
	
	erewind(fpi);
	erewind(fpo);

	fgettr(fpi,&tr);
	do {
		/*memcpy( (void *) tr.data, (const void *) data, tr.ns*FSIZE); */
                memset( (void *) tr.data, (int) '\0', tr.ns*FSIZE);
	        /* for(i=0;i<tr.ns; i++) tr.data[i] -= data[i]; */
		fputtr(fpo,&tr);
	}while(fgettr(fpi,&tr));
}


