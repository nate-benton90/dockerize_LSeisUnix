/* TK domain filter */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#define LOOKFAC 1       /* Look ahead factor for npfaro   */
#define PFA_MAX 720720  /* Largest allowed nfft           */
float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#define PIP2 1.570796327

/*********************** self documentation **********************/
char *sdoc[] = { " SUTKFILTTV -  TK domain filter			        ", 
"                       						",
"      sutkfilttv < stdin > stdout [optional parameters]			",
"                       						",
"      key=ep		header keyword describing the gathers		",
"      percs=50		Cutoff percentage of the higher k values at 0 time",
"      perce=80		Cutoff percentage of the higher k values at max time",
"      ramp=7           number of traces to ramp the cutoff		",
"      scale=0          scale=1 scale traces before filtering.          ",
"      dx=25            reciver spacing                        		",
"      tpd=4            trace padding on both side of the gather        ",
"      trl=15		trace number limit; if the gather has less      ",
"                       than this number no processing is done		",
"                       						",
"      The gather is transformed into the T-K domain, than the largest	",
"      perc percentage of the K values is zerod out. The ramp provides	",
"      a smooth transition to zero values.				",
NULL};
void polygonalFilter(float *f, float *amps,
			int npoly, int nfft, float dt, float *filter);
segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first);
segy **put_gather(segy **rec,int *nt, int *ntr);
segy tr;
segy tr2;
int verbose;
int main( int argc, char *argv[] )
{
	cwp_String key;		/* header key word from segy.h		*/
	cwp_String type;	/* ... its type				*/
	Value val;
	segy **rec_o;		/* trace header+data matrix */
	int first=0;		/* true when we passed the first gather */
	int ng=0;
	float dt;
	int nt;
	int ntr;
	
	int nfft=0;		/* lenghth of padded array */
	float snfft;		/* scale factor for inverse fft */
	int nf=0;		/* number of frequencies */
	float d1;			/* frequency sampling int. */
	float *rt;		/* real trace */
	float *rtp;		/* padded real trace */
	complex *st;	       /* complex trace */
	float **fddr;		/* frequency domain data */
	float **fddi;		/* frequency domain data */
	float percs=50;		/* cutoff percentage at start*/
	float perce=80;		/* cutoff percentage at end*/
	float dperc;		/* cutoff percentage at change */
	float perc;		/* cutoff percentage at current time level*/
	int ramp=7;		/* length of ramp  */
	int scale;		/* scale flag */
	int trl;		/* trace number limit */
	float *scv;		/* array of scale values */
	float **filter;		/* filter array for each time level (nt )*/
	int npoly;
	int tpd;		/* number of traces to padd */ 
	float *f;
	float *amps;
	float dx;		/* spatial sampling intervall */
		
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	
	if (!getparstring("key", &key))	key = "ep";
	if (!getparfloat("percs", &percs)) percs = 50.0;
	if (!getparfloat("perce", &perce)) perce = 80.0;
	if (!getparfloat("dx", &dx)) dx = 25.0;
	if (!getparint("ramp", &ramp)) ramp = 7;
	if (!getparint("scale", &scale)) scale = 0;
	if (!getparint("trl", &trl)) trl = 15;
	if (!getparint("tpd", &tpd)) tpd = 3;
	
        /* filter stuff */
	npoly = 2;
        f = ealloc1float(npoly);
        amps = ealloc1float(npoly);


	/* get the first record */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	if(ntr==0) err("Can't get first record\n");
	
	dperc=(perce-percs)/nt;
	
	do {
		ng++;
	     if(ntr > trl ) {
		/* set up the fft */
		nfft = npfaro(ntr+2*tpd, LOOKFAC * (ntr+2*tpd));
        	if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                	err("Padded nt=%d--too big", nfft);
        	nf = nfft/2 + 1;
        	snfft=1.0/nfft;
		d1=snfft/dx;

		fddr = ealloc2float(nt,nf); 
		fddi = ealloc2float(nt,nf); 
		rtp = ealloc1float(nfft);
		rt = &rtp[tpd];
		st = ealloc1complex(nf);
        	filter = ealloc2float(nf,nt);
		scv = ealloc1float(ntr);

		/* precompute filters for each nt level */
		{ int it;
			for(it=0;it<nt;it++) {
				perc=percs+it*dperc;
				f[0]=d1*nf*(1.0-perc/100.0);
				f[1]=f[0]+d1*nf*(ramp/100.0);
				/* fprintf(stderr," %f %f\n",f[0],f[1]); */
				amps[0]=1.0;
				amps[1]=0.0;
				/* Build the polygonal filter filter[]*/
				polygonalFilter(f,amps,npoly,nfft,dx,filter[it]);
			}
		}
		
		memset( (void *) scv, (int) '\0', ntr*FSIZE);
		if(scale) { 
			{ register int itr,it;
				for(itr=0;itr<ntr;itr++) {
					for(it=0;it<nt;it++)
						scv[itr] +=SQR((*rec_o[itr]).data[it]);
					scv[itr] = sqrt(scv[itr]/nt);
					if(scv[itr]>0.0)
						sscal(nt,1.0/scv[itr],(*rec_o[itr]).data,1);				
				}
			}
		}
		
		
		{ register int itr,it,ifr;
			for(it=0;it<nt;it++) {
				/* padding from left */
				for(itr=0;itr<tpd;itr++) rtp[itr]=NINT(itr/tpd)*(*rec_o[0]).data[it];		
 				for(itr=0;itr<ntr;itr++) 		
               				rt[itr]=(*rec_o[itr]).data[it];
				/* padding from right */
				for(itr=0;itr<tpd;itr++) rt[ntr+itr]=NINT((tpd-1-itr)/tpd)*(*rec_o[ntr-1]).data[it];		
                		memset( (void *) &rtp[ntr+2*tpd], (int) '\0', (nfft - ntr-2*tpd)*FSIZE);
				pfarc(1, nfft, rtp, st);
				for(ifr=0;ifr<nf;ifr++) { 
					fddr[ifr][it] = st[ifr].r;
					fddi[ifr][it] = st[ifr].i;
				}
			}
		}
		{ register int itr,it,ifr,tsh;
			tsh=(int)(nf*perc/100.0);
			for(it=0;it<nt;it++) {
				for (ifr=0; ifr<nf;++ifr) {
					 st[ifr]=
					 crmul(cmplx(fddr[ifr][it],fddi[ifr][it]),filter[it][ifr]);
					 
				}
/*				for(ifr=0;ifr<nf-tsh;ifr++) { 
					st[ifr].r = fddr[ifr][it];
					st[ifr].i = fddi[ifr][it]; 
				}
				for(ifr=nf-tsh,ir=0;ifr<=MIN(nf-tsh+ramp,nf);ifr++,ir++) {
					st[ifr].r = fddr[ifr][it]*cos(ir*PIP2/ramp);
					st[ifr].i = fddi[ifr][it]*cos(ir*PIP2/ramp); 
				}
				for(ifr=MIN(nf-tsh+ramp+1,nf);ifr<nf;ifr++) {
					st[ifr].r = 0.0;
					st[ifr].i = 0.0; 
				}
*/				pfacr(-1, nfft, st, rtp);
				if(scale) {
					for(itr=0;itr<ntr;itr++) 		
                				(*rec_o[itr]).data[it]=rt[itr]*snfft*scv[itr];
				} else {
					for(itr=0;itr<ntr;itr++) 		
                				(*rec_o[itr]).data[it]=rt[itr]*snfft;
				}
			}
		}
			
/*		{ register int itr,it,ifr;
			for(ifr=0;ifr<nf;ifr++) {
				for(it=0;it<nt;it++) tr2.data[it] = fddi[ifr][it];		
				tr2.ns=nt;
				tr2.dt=(int)(dt*1000000.0);
				puttr(&tr2);
			}
		}
		{ register int itr;
			for(itr=0;itr<ntr;itr++)
				free1((void *)rec_o[itr]);
		}
		rec_o=NULL;
*/
		free2float(fddr);
		free2float(fddi);
		free1float(rtp);
		free1float(scv);
		free1complex(st);
		free2float(filter);
	    }
	    rec_o = put_gather(rec_o,&nt,&ntr);
	    rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	} while(ntr);

	warn("Number of gathers %10d\n",ng);
	 
	return EXIT_SUCCESS;
}

segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first)
{
	int nsegy;
	FILE *tracefp;
	FILE *headerfp;
	segy **rec=NULL;
	int ntrr=0;
	int indx=0;
	static Value val;
	
	
	*type = hdtype(*key);
	indx = getindex(*key);
        *ntr = 0;

	if(*first==0) {

		/* Take a trace count */
        	/* get info from first trace */
        	nsegy = gettr(&tr);
		if (nsegy==0)  err("can't get first trace");
        	ntrr++;
		*nt = tr.ns;
        	*dt = (float) tr.dt/1000000.0;
		gethval(&tr, indx, n_val);
		*first=1;
	}

        /* Store traces in tmpfile while getting a count */
	tracefp = etmpfile();
	headerfp = etmpfile();
	if (verbose) warn("using tmpfile() call");
        do {
                efwrite(&tr, 1, HDRBYTES, headerfp);
                efwrite(tr.data, FSIZE, *nt, tracefp);
		/* read the next trace */
		*ntr+=1;
		val=*n_val;
		nsegy = gettr(&tr);
		if(nsegy) ntrr++;
		gethval(&tr, indx, n_val);
	} while (nsegy && !valcmp(*type,val,*n_val));

	/* No more data */
	if(nsegy==0 && ntrr==0 ) {
		 *ntr=0;
		 return(rec=NULL);
	}
        
	if(rec!=NULL) free(rec);
	
	/* allocate memory for the record */
	{ register int i;
		rec = ealloc1(*ntr,sizeof(segy *));
		for(i=0;i<*ntr;i++)
			rec[i] = (segy *)ealloc1((*nt*FSIZE+HDRBYTES),sizeof(char));
	}
	
	
	/* load traces into an array and close temp file */
	erewind(headerfp);
        erewind(tracefp);
	{ register int ix;
        	for (ix=0; ix<*ntr; ix++)
                	fread (rec[ix],HDRBYTES, 1, headerfp);
        	efclose (headerfp);
		for(ix=0; ix<*ntr; ix++)
                	fread ((*rec[ix]).data,FSIZE, *nt, tracefp);
        	efclose (tracefp);
	}
	
	return(rec);	
}

segy **put_gather(segy **rec,int *nt, int *ntr)
{


	segy tr;
	
	{ register int i;
		for(i=0;i<*ntr;i++) {
			memcpy( (void *) &tr, (const void *) rec[i],
					*nt*FSIZE+HDRBYTES);
			puttr(&tr);
			free1((void *)rec[i]);
		}
	}
	return(rec=NULL);
}
void polygonalFilter(float *f, float *amps, int npoly,
				int nfft, float dt, float *filter)
/*************************************************************************
polygonalFilter -- polygonal filter with sin^2 tapering
**************************************************************************
Input:
f		array[npoly] of frequencies defining the filter
amps		array[npoly] of amplitude values
npoly		size of input f and amps arrays
dt		time sampling interval
nfft		number of points in the fft

Output:
filter		array[nfft] filter values
**************************************************************************
Notes: Filter is to be applied in the frequency domain
**************************************************************************
Author:  CWP: John Stockwell   1992
*************************************************************************/
#define PIBY2   1.57079632679490
{
        int *intfr;             /* .... integerizations of f		*/
        int icount,ifs;		/* loop counting variables              */
	int taper=0;		/* flag counter				*/
        int nf;                 /* number of frequencies (incl Nyq)     */
        int nfm1;               /* nf-1                                 */
        float onfft;            /* reciprocal of nfft                   */
        float df;               /* frequency spacing (from dt)          */

        
	intfr=alloc1int(npoly);

        nf = nfft/2 + 1;
        nfm1 = nf - 1;
        onfft = 1.0 / nfft;

        /* Compute array of integerized frequencies that define the filter*/
        df = onfft / dt;
        for(ifs=0; ifs < npoly ; ++ifs) {
                intfr[ifs] = NINT(f[ifs]/df);
                if (intfr[ifs] > nfm1) intfr[ifs] = nfm1;
        }

	/* Build filter, with scale, and taper specified by amps[] values*/
	/* Do low frequency end first*/
	for(icount=0; icount < intfr[0] ; ++icount) 
		filter[icount] = amps[0] * onfft;

	/* now do the middle frequencies */
	for(ifs=0 ; ifs<npoly-1 ; ++ifs){
	   if(amps[ifs] < amps[ifs+1]) {	
		++taper;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; ++icount) {
		    float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
		    float s = sin(c*(icount - intfr[ifs] + 1));
		    float adiff = amps[ifs+1] - amps[ifs];
		    filter[icount] = (amps[ifs] + adiff*s*s) * onfft;
		}
	   } else if (amps[ifs] > amps[ifs+1]) {	
		++taper;
		for(icount=intfr[ifs]; icount<=intfr[ifs+1]; ++icount) {
			   float c = PIBY2 / (intfr[ifs+1] - intfr[ifs] + 2);
                	   float s = sin(c*(intfr[ifs+1] - icount + 1));
			   float adiff = amps[ifs] - amps[ifs+1];
                	   filter[icount] = (amps[ifs+1] + adiff*s*s) * onfft;
		  }
	   } else 
		if(!(taper)){
		for(icount=intfr[ifs]; icount <= intfr[ifs+1]; ++icount)
		   	   filter[icount] = amps[ifs] * onfft;
		} else {
		for(icount=intfr[ifs]+1; icount <= intfr[ifs+1]; ++icount)
		   	   filter[icount] = amps[ifs] * onfft;
		}
	}

	/* finally do the high frequency end */
	for(icount=intfr[npoly-1]+1; icount<nf; ++icount){
		filter[icount] = amps[npoly-1] * onfft;
	}

}
