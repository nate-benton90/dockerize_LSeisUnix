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
" SUSPWHTWG - Spectral balancing on gathers                           	",
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
" fftpad=25.0		% of fft padding in trace length   		",
" ftape=5		Width of the frequency taper window in HZ	",
" wagc=0.3		Gain operator length				",
"									",
" prw=0.02		Prewithening % of rms value 			",
"			prw is computed for each filtered trace		",
"			separately.					",
" rescl=1		rescl=0 Do not remove gain  after combining     ",
"			the output trace.                               ",
" logrscl=1		Rescale the data with the log of the overall gain",
" sm1=0			Smoothing of filtered envelope in direction 1	",
" sm2=0			Smoothing of filtered envelope in direction 2	",
"									",
"									",
"									",
"									",
" The output trace is recombined from a series of bandass filtered and  ",
" gained traces. This way the relative contribution of each frequency   ",
" band to the output trace is changed, i.e. the spectrum is whitened.   ",
" To check the effect do: suspwh < data | suspecfx | suximage		",
NULL};
segy tr;

void do_env(float *data,float *env,int nt);
float rms_tr(float *data,int nt);
float mean_tr(float *data,int nt);
void do_agc_scale(float *data, int iwagc,float *scale, int nt);
void bzfilter(int npoleslo,float f3dblo,int npoleshi,float f3dbhi,int nt,float *data);

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
	float fftpad;

	int fnl,fnr;
	int verbose;
	int rescl;
	
	int nband;
	int bnd;
	float flpass;
	float flstop;
	float fhpass;
	float fhstop;
	float adb;
	float trsh=0.0;		/* scaling treshold value */
	float prw;		/* prewithening */
	float sm1;		/* smoothing in direction 1*/
	float sm2;		/* smoothing in direction 2*/
	int envf;
	float wagc;
	int iwagc;
	int logrscl;
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	
	if (!getparstring("key", &key))	key = "ep";
	if (!getparfloat("fmin", &fmin)) fmin = 5;
	if (!getparfloat("fmax", &fmax)) fmax = 120;
	if (!getparfloat("fftpad", &fftpad)) fftpad = 25.0;
	if (!getparfloat("fwind", &fwind)) fwind = 10;
	if (!getparfloat("ftape", &ftape)) ftape = 5;
	if (!getparfloat("adb", &adb)) adb =0.0;
	if (!getparfloat("prw", &prw)) prw =0.02;
	if (!getparfloat("sm1", &sm1)) sm1 =0.0;
	if (!getparfloat("sm2", &sm2)) sm1 =0.0;
	if (!getparint("verbose", &verbose)) verbose =0;
	if (!getparint("fnl", &fnl)) fnl=5;
	fnr=fnl;
	if (!getparint("rescl", &rescl)) rescl =1;
	if (!getparint("logrscl", &logrscl)) logrscl =1;
	if (!getparint("envf", &envf)) envf=0;
	if (!getparfloat("wagc", &wagc)) wagc=0.3;
	
	/* get the first record */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	if(ntr==0) err("Can't get first record\n");
	
        
	
	adb/=100.0;
	nband = NINT((fmax-fmin)/fwind);
	iwagc=NINT(wagc/dt);
        iwagc >>= 1;  /* windows are symmetric, so work with half */
	
	do{
		{ float **gdata,**env,**scale,**fdata, *rms;
		  int itr,it;		
			/* allocate array to hold the gather */
			gdata = ealloc2float(nt,ntr);
			fdata = ealloc2float(nt,ntr);
			env   = ealloc2float(nt,ntr);
			scale = ealloc2float(nt,ntr);
			rms = ealloc1float(ntr);
		
			for(itr=0;itr<ntr;itr++) {
				memset((void *) scale[itr], (int) '\0', nt*FSIZE);
				memset((void *) fdata[itr], (int) '\0', nt*FSIZE);
			}
			
			/* filter sequence */

			for ( bnd=0;bnd<nband;bnd++) {
				flstop=fmin+bnd*fwind;
				if (flstop<0.0) flstop=0.0;
				flpass=flstop+ftape;
				fhstop=flstop+fwind;
				fhpass=fhstop-ftape;
				
				if(verbose==1) 
					fprintf(stderr,
					"Frequency band in Hz: %4.2f %4.2f %4.2f %4.2f \n",
					flstop,flpass,fhpass,fhstop);
				
				/* Filter the traces */
				for(itr=0;itr<ntr;itr++) {
					memcpy( (void *) gdata[itr], (const void *) (*rec_o[itr]).data,nt*FSIZE);
					
					bp_filter_padd(flstop,flpass,fhpass,fhstop,gdata[itr],dt,nt,fftpad);
					
					if(envf==1) { 
						do_env(gdata[itr],env[itr],nt);
					} else {
						do_agc_scale(gdata[itr],iwagc,env[itr],nt);
					}
					
					/* Compute the rms value of the evelope */
					rms[itr] = rms_tr(env[itr],nt);
					
				}
				/* Threshold for scaling */
				for(itr=0;itr<ntr;itr++) {
					trsh=fabs(rms[itr]*(1.0+prw/100.0));
					/* fprintf(stderr," %f %f %d\n",rms[itr],trsh,bnd); */
					for(it=0;it<nt;it++) 
						if(env[itr][it] < trsh) env[itr][it]=trsh;
				}
				
				/* Smooth the envelope before inverse scaling */
				if(sm2 || sm1)
					dlsq_smoothing(nt,ntr,0,nt,0,ntr,sm1,sm2,0,env);
				
				/* Do scaling on the filtered traces by inverse env*/
				for(itr=0;itr<ntr;itr++) {
					{ float atmp;
						for(it=0;it<nt;it++) {
							atmp = 1.0/(env[itr][it]);
							scale[itr][it] +=atmp;
							fdata[itr][it] +=gdata[itr][it]*atmp;
						}
					}
				}
			}
			
			/* Do inverse scaling */
			for(itr=0;itr<ntr;itr++)
				for(it=0;it<nt;it++) { 
					if(rescl) {
						if(logrscl) {
							(*rec_o[itr]).data[it]=fdata[itr][it]/fabs(log(scale[itr][it]));
						} else {
							(*rec_o[itr]).data[it]=fdata[itr][it]/scale[itr][it];
						} 
					} else {
						(*rec_o[itr]).data[it]=fdata[itr][it];
					}
				}
			free2float(gdata);
			free2float(fdata);
			free2float(scale);
			free2float(env);
			free1float(rms);			

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

float mean_tr(float *data,int nt)
{
	float tmps=0.0;
	int is;
		  				
	for(is=0;is<nt;is++) {
		tmps+=data[is];
	}
	return(tmps/nt);
}					

void do_env(float *data,float *env,int nt)
/* envelope computing */
{

	int i;
	float *im;
	
	
	im = ealloc1float(nt);
	hilbert(nt,data,im);

	for(i=0;i<nt;i++)
		env[i] = sqrt(data[i]*data[i]+im[i]*im[i])+FLT_EPSILON;
	free1float(im);
}

void bzfilter(int npoleslo,float f3dblo,int npoleshi,float f3dbhi,int nt,float *data)
/* Butterworth zero phase filter calling routine */
{
	int i;
	float tmp;

	bfhighpass(npoleslo,f3dblo,nt,data,data);
	bflowpass (npoleshi,f3dbhi,nt,data,data); 
	
	/* reverse the trace for zero phase filtering */
      	for (i=0; i<nt/2; ++i) { 
	      tmp = data[i];
	      data[i] = data[nt-1 - i];
	      data[nt-1 - i] = tmp;
      	}

      	bfhighpass(npoleslo,f3dblo,nt,data,data);
      	bflowpass (npoleshi,f3dbhi,nt,data,data); 

      	/* reverse the trace back */
      	for (i=0; i<nt/2; ++i) { 
	      tmp = data[i];
	      data[i] = data[nt-1 - i];
	      data[nt-1 - i] = tmp;
      	} 
}

