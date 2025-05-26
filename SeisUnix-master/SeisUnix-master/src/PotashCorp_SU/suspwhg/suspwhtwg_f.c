/* gather oriented spectral whithening */

#include "suhdr.h"
#define LOOKFAC 2       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */


/*********************** self documentation **********************/
char *sdoc[] = {
" 									",
" SUSPWHTWG_F - Spectral balancing on gathers                           ",
" 									",
" suspwhg_f  <stdin >sdout        				        ",
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
" logrscl=0		=1 Rescale the data with the log of the overall gain",
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
" The combination is done in frequency domain				",
"									",
NULL};
segy tr;

void do_env(float *data,float *env,int nt);
float rms_tr(float *data,int nt);
float mean_tr(float *data,int nt);
void do_agc_scale(float *data, int iwagc,float *scale, int nt);

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
	
	/* FFT things */
	int nfft;
	float snfft;
	int nf;
	float df;
	complex *ct;
	float *rt;
	int ifb_s,ifb_e;
	
	
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
	if (!getparint("logrscl", &logrscl)) logrscl =0;
	if (!getparint("envf", &envf)) envf=0;
	if (!getparfloat("wagc", &wagc)) wagc=0.3;
	
	/* get the first record */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	if(ntr==0) err("Can't get first record\n");
	
        
	
	adb/=100.0;
	nband = NINT((fmax-fmin)/fwind);
	iwagc=NINT(wagc/dt);
        iwagc >>= 1;  /* windows are symmetric, so work with half */
        
	/* Set up FFT parameters */
        nfft = npfaro(nt*(1.0+fftpad/100.0), LOOKFAC * nt*(1.0+fftpad/100.0));
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;
	snfft=1.0/nfft;
	df = 1.0/(nfft*dt);
	ct   = ealloc1complex(nf);
        rt   = ealloc1float(nfft);

	
	do{
		{ float **gdata,**env,**scale,**fdata, *rms;
		  complex **cdata;
		  int itr,it;		
			/* allocate array to hold the gather */
			gdata = ealloc2float(nfft,ntr);
			fdata = ealloc2float(nt,ntr);
			cdata = ealloc2complex(nf,ntr);
			env   = ealloc2float(nt,ntr);
			scale = ealloc2float(nt,ntr);
			rms = ealloc1float(ntr);
		
			for(itr=0;itr<ntr;itr++) {
				memset((void *) scale[itr],0, nt*FSIZE);
				memset((void *) fdata[itr],0, nt*FSIZE);
			}
			
			/* filter sequence */

			for ( bnd=0;bnd<nband;bnd++) {
				flpass=MAX(fmin+bnd*fwind,0.0);
				flstop=MAX(flpass-ftape,0.0);
				if (flstop<0.0) flstop=0.0;
				fhpass=flpass+fwind;
				fhstop=fhpass+ftape;
				
				/* Compute frequency band start and end in df */
				ifb_s = NINT(flpass/df);
				ifb_e = NINT(fhpass/df);
				
				
				if(verbose==1) 
					fprintf(stderr,
					"Frequency band in Hz: %4.2f %4.2f %4.2f %4.2f \n",
					flstop,flpass,fhpass,fhstop);
				
				/* Filter the traces */
				for(itr=0;itr<ntr;itr++) {
					memcpy( (void *) gdata[itr], (const void *) (*rec_o[itr]).data,nt*FSIZE);
					memset( (void *) &gdata[itr][nt],0,(nfft-nt)*FSIZE);
					
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
				/* Do the stacking in the frequency domain for each band limited trace*/
				for(itr=0;itr<ntr;itr++) {
					
					{ float atmp;
						for(it=0;it<nt;it++) {
							atmp = 1.0/(env[itr][it]);
							scale[itr][it] +=atmp;
							gdata[itr][it]*=atmp;
						}
					}
					
					pfarc(-1,nfft,gdata[itr],ct);
					
					{ int ifr,ir;
					  int framp=9;
					  complex diff,tmpc;
						for(ifr=ifb_s;ifr< ifb_e;ifr++)
							cdata[itr][ifr] = ct[ifr];
						
						/* ramping if freq domain between bands */
						if(bnd!=0) {
							for(ifr=ifb_s,ir=framp-1;ifr>ifb_s-framp;ifr--,ir--) {
								diff=csub(ct[ifr],cdata[itr][ifr]);
								tmpc=crmul(diff,(float)ir/(float)framp);
								cdata[itr][ifr]=cadd(cdata[itr][ifr],tmpc);
							}	
						}
							
					}
					
					
				}
			}
			
			
			
			/* Do inverse fft */
			for(itr=0;itr<ntr;itr++) {
				memcpy( (void *) ct, (const void *)&cdata[itr][0] ,nf*sizeof(complex));
			 	pfacr(1, nfft, ct, rt);
				for(it=0;it<nt;it++)
					fdata[itr][it] = rt[it]*snfft;
			}
			
			/* testing */
			{int test=1;
			FILE *tfp;
			segy ttr;
			
				if(test) {
					tfp = efopen("scale.su","w");
					for(itr=0;itr<ntr;itr++) {
						for(it=0;it<nt;it++) {
							ttr.data[it]=1.0/log(scale[itr][it]);
							ttr.data[it]=1.0/scale[itr][it];
						}
						ttr.dt=dt;
						ttr.ns=nt;
						fputtr(tfp,&ttr);
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
			free2complex(cdata);			

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
