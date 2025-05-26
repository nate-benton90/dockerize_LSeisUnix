/* gather oriented spectral whithening */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "suhdr.h"
float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" SUSPWHG - Spectral balancing on gathers                           	",
" 									",
" suspwhg  <stdin >sdout        						",
" 									",
" Required parameters:							",
" none									",
" 									",
" Optional parameters:							",
" key=ep		gather flag              			",
" fmin=5		Mininum frequency              			",
" fmax=120		Maximum frequency of the seismic band		",
" fwind=10		Width of the bandpass filters in HZ		",
" ftape=5		Width of the frequency taper window in HZ	",
"									",
" prw=1.0		Prewithening % of rms value 			",
"			prw is computed for each filtered trace		",
"			separately					",
" sm1=0			Smoothing of filtered envelope in direction 1	",
" sm2=5			Smoothing of filtered envelope in direction 2	",
"									",
"									",
" rescl=1		rescl=0 Do not remove gain  after combining     ",
"			the output trace                                ",
"									",
"			Credit to all who wrote the gaining routines!   ",
"									",
" The output trace is recombined from a series of bandass filtered and  ",
" gained traces. This way the relative contribution of each frequency   ",
" band to the output trace is changed, i.e. the spectrum is whitened.   ",
" To check the effect do: suspwh < data | suspecfx | suximage		",
NULL};
segy tr;

void do_env(float *data,float *env,int nt, float trsh);
float rms_tr(float *data,int nt);

int main(int argc, char **argv)
{
	segy **rec_o;		/* trace header+data matrix */
	int first=0;		/* true when we passed the first gather */
	cwp_String key;		/* header key word from segy.h		*/
	cwp_String type;	/* ... its type				*/
	Value val;
	
	int nt;			/* number of points on input trace	*/
	int ntr;		/* number of traces in gather */
	float dt;
	float fmin;		/* number of frequencies		*/
	float fmax;
	float fwind;		/* frequency window */
	float ftape;		/* frequency taper window */

	int fnl,fnr;
	int verbose;
	int rescl;
	
	int nband;
	float bnd;
	float fpass;
	float apass;
	float astop;
	float fstop;
	float adb;
	float rms;		/* rms value of the trace */
	float trsh;		/* scaling treshold value */
	float prw;		/* prewithening */
	float sm1;		/* smoothing in direction 1*/
	float sm2;		/* smoothing in direction 2*/
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	
	if (!getparstring("key", &key))	key = "ep";
	if (!getparfloat("fmin", &fmin)) fmin = 5;
	if (!getparfloat("fmax", &fmax)) fmax = 120;
	if (!getparfloat("fwind", &fwind)) fwind = 10;
	if (!getparfloat("ftape", &ftape)) ftape = 5;
	if (!getparfloat("adb", &adb)) adb =0.0;
	if (!getparfloat("prw", &prw)) prw =0.1;
	if (!getparfloat("sm1", &sm1)) sm1 =0.0;
	if (!getparfloat("sm2", &sm2)) sm1 =5.0;
	if (!getparint("verbose", &verbose)) verbose =0;
	if (!getparint("fnl", &fnl)) fnl=5;
	fnr=fnl;
	if (!getparint("rescl", &rescl)) rescl =1;
	
	/* get the first record */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	if(ntr==0) err("Can't get first record\n");
	
        
	
	adb/=100.0;
	nband = NINT((fmax-fmin)/(fwind+ftape));
	
	do{
		{ float **gdata,**env,**scale,**fdata;
		  int itr,it;		
			/* allocate array to hold the gather */
			gdata = ealloc2float(nt,ntr);
			fdata = ealloc2float(nt,ntr);
			env   = ealloc2float(nt,ntr);
			scale = ealloc2float(nt,ntr);
		
			for(itr=0;itr<ntr;itr++) {
				memset((void *) scale[itr], (int) '\0', nt*FSIZE);
				memset((void *) fdata[itr], (int) '\0', nt*FSIZE);
			}
			
			/* filter sequence */
			for ( bnd=0;bnd<nband;bnd++) {
				apass=fmin+bnd*(fwind+ftape);
				fpass=apass-ftape;
				if (fpass<0.0) fpass=0.0;
				astop=apass+fwind;
				fstop=astop+ftape;
				if(verbose==1) 
					fprintf(stderr,
					"Frequency band in Hz: %4.2f %4.2f %4.2f %4.2f\n",
					fpass,apass,astop,fstop);
					
				/* Filter the traces */
				for(itr=0;itr<ntr;itr++) {
					memcpy( (void *) gdata[itr], (const void *) (*rec_o[itr]).data,nt*FSIZE);
					bp_filter(fpass,apass,astop,fstop,gdata[itr],dt,nt);
					
					/* Compute the rms value of the trace */
					rms = rms_tr(gdata[itr],nt);
					
					/* Threshold for scaling */
					trsh=rms*prw;
					do_env(gdata[itr],env[itr],nt,trsh);
				}
				
				/* Smooth the envelope before inverse scaling */
				dlsq_smoothing(nt,ntr,0,nt,0,ntr,sm1,sm2,0,env);
				
				/* Do scaling on the filtered traces by inverse env*/
				for(itr=0;itr<ntr;itr++) {
					{ float atmp;
						for(it=0;it<nt;it++) {
							atmp = 1.0/env[itr][it];
							fdata[itr][it] +=gdata[itr][it]*atmp;
							scale[itr][it] +=atmp;
						}
					}
				}
					
			
			}
			
			/* Do inverse scaling */
			for(itr=0;itr<ntr;itr++)
				for(it=0;it<nt;it++) { 
					if(rescl) {
						(*rec_o[itr]).data[it]=fdata[itr][it]/scale[itr][it];
					} else {
						(*rec_o[itr]).data[it]=fdata[itr][it];
					}
				}
			free2float(gdata);
			free2float(fdata);
			free2float(scale);
			free2float(env);
		}
		rec_o = put_gather(rec_o,&nt,&ntr);
		rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
		
	} while(ntr);

	return EXIT_SUCCESS;
}
float rms_tr(float *data,int nt)
{
	float tmps=0.0,tmp;
	int is;
		  				
	for(is=0;is<nt;is++) {
		tmp=data[is];
		tmps+=tmp*tmp;
	}
	return(sqrt(tmps/nt));
}					

void do_env(float *data,float *env,int nt, float trsh)
/* envelope computing */
{

	int i;
	float *im;
	
	
	im = ealloc1float(nt);
	hilbert(nt,data,im);

	for(i=0;i<nt;i++)
		env[i] = sqrt(data[i]*data[i]+im[i]*im[i])+FLT_EPSILON;
	free1float(im);
	
	for(i=0;i<nt;i++) if(env[i] < trsh) env[i]=trsh;
	
}
