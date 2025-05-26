/* suknmo.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "suhdr.h"

#define I               cmplx(0.0, 1.0)
#define PIBY2           0.5 * PI
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */
float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

/*********************** self documentation *****************************/
char *sdoc[] = {
"						 		       	",
"SUKNMO3D - Perform a Stolt migration on Common Scatterpoint Gathers 	",
"                                                                       ",
" vc=			Velocity to do a constat velocity stack         ",
"          		If this is given then f,nvnmo fac are           ",
"               	ignored                                         ",
" nstval=		number of cdps					",
"                                                                       ",
" f=                    filename of the velocity file                   ",
"			function in the file f		 		",
" fac=1.0		factor to multiply the velocity values with	",
" w=0.6			Stolt stretch factor                            ",
" fmax=NYQUIST		Maximum frequency to migrate                    ",
"                                                                       ",
" key=cdp		header keyword that marks the stackable gathers	",
NULL};
   
/* Segy data constans */
segy intrace,outtrace,vtr;				/* SEGY trace */

static void xindex_p (int nx, float **ax, float x, int *index);
void get_vel(float **v,float *stvel,int *stval_ind,float **stval_sort, 
	     int nval,int nt,cwp_String type,Value  val, float *vrms);
static void makeut (float vstolt, float fmax, float *vt,
	int nt, float dt, float **ut, int *nu, float *du, float **tu);
static void makeu (float vstolt, float *v, int nt, float dt, float *u);
static void stolt1k (float k, float v, float s, float fmax, int nt, float dt,
	complex *p, complex *q);

int main( int argc, char *argv[] )
{
	cwp_String key;	/* header key word from segy.h		*/
	cwp_String type;/* ... its type				*/
	int indx;	/* ... its index			*/
	int nt;		/* number of data points on trace	*/
	float dt;	/* Sample interval */
	int ntr;	/* Number of traces in gather */
	int fold;	/* number of traces stacked		*/
	int newtracl;	/* tracl for stacked traces		*/
	int verbose;	/* verbose flag				*/
	float *migdata; /* migrated trace         */ 
	float *vrms;	/* array of rms velocities */
	float t0;	/* time of first sample */
	float tn;	/* time of sample */
	float vmin=9999;/* minimum knmo velocity */
	float vmax=-999;/* maximum knmo velocity */
        
	segy **rec_o;           /* trace header+data matrix */
        int first=0;            /* true when we passed the first gather */
        int ng=0;
	Value val;	/* value of key in current gather	*/
	 	


	/* velocity file and related things */
        cwp_String f="";        /* file containig cdps & velocity functions */
        FILE *fp;               /* file pointer */
	float fac=1.0;
	int nstval;		/* number of values of stacking flags */
	float *stval;		/* float array of the stacking flags */
	int *stval_ind;		/* sorted index of stvals */
	float **stval_sort;	/* sorted pointer array pointing to the 
				  values of stval */
	float *vnmo=NULL;	/* temporary arryay of v values */
	float *tnmo=NULL;	/* temporary arryay of t values */
	float **vv;		/* 2d array of interpolated v values */
	float v;		/* velocity value */	
	int nvnmo;		/* number of velocities picks per trace*/
	float vc;		/* constant velocity */

	/* Stolt migration things */
	float w;		/* Stolt stretch factor */
	float fmax;		/* maximum freq. to migrate */
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Set parameters */
	if (!getparfloat("vc",&vc)) vc = -1.0;
	/* Velocity file and related things */
       	MUSTGETPARINT("nstval",&nstval);
       	MUSTGETPARSTRING("f",&f);
	if (!getparfloat("fac",&fac)) fac = 1.0;
	if (!getparfloat("w",&w)) w = 0.6;
	
	/* stacking related things */
	if (!getparint   ("verbose", &verbose))	 verbose = 0;
	if (!getparstring("key", &key))		 key = "cdp";
	type = hdtype(key);
	indx = getindex(key);

        /* get the first record */
        rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	t0 = (*rec_o[0]).delrt;
	fold = 0;
	
	if (!getparfloat("fmax",&fmax)) fmax = (0.5/dt);
        /* Read velocity file */
        
	if((fp=fopen(f,"r"))==NULL)
                        err("cannot open file=%s\n",f);
	fgettr(fp,&vtr); 

	nvnmo=vtr.ns;

	migdata = ealloc1float(nt);
	vrms = ealloc1float(nt);
        stval = ealloc1float(nstval);
	stval_ind = ealloc1int(nstval);
	stval_sort = ealloc1(nstval, sizeof(float *));
        if(vc<0.0) {
		vnmo = ealloc1float(nvnmo);
        	tnmo = ealloc1float(nvnmo);
	}
        vv = ealloc2float(nt,nstval);

        
	/* Sort the stacking field values with index sort */
	{ register int ist;
		for(ist=0;ist<nstval;ist++) stval_ind[ist]=ist;
	}

	qkisort(nstval,stval,stval_ind);
	
	/* stval_sort is a pointer array pointing to the 
	   values of stval in a way that it was sorted
	   by quicksort */
	   
	{ register int ist;
		for(ist=0;ist<nstval;ist++)
			stval_sort[ist] = &stval[stval_ind[ist]];
	}
	
	
        /* Read tnmo vnmo pairs */         
        { int it;
	
	if(vc<0.0) {
		for(it=0;it<nvnmo;it++)
			tnmo[it]=vtr.delrt/1000.0+it*(double)vtr.dt/1000000.0;
	
        	do {
			cdp[icdp]=trv.cdp;
			for(it=0; it<nvnmo;it++) 
				vnmo[it] =fac*vtr.data[it];
			for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
				intlin(nvnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
				vv[icdp++][it] = v;
				if(v>vmax) vmax=v;
				if(v<vmin) vmin=v;
			}
			icdp++;
		} while(fgettr(fp,&vtr));
        	fclose(fp);
	} else {
		{ register int ist,it;
			for(ist=0;ist<nstval; ++ist )
                		for (it=0; it<nt; ++it) {
                        		vv[ist][it] = vc;
				}
		}
		vmax=vc;
		vmin=vc;
	}
			
	
	/* Echo velocities back */
	fprintf(stderr," Maximum velocity = %f\n",vmax);
	fprintf(stderr," Minimum velocity = %f\n",vmin);


	/* Loop over traces */
	newtracl = 1;
	gethval(rec_o[0],indx, &val);
	
	
        do {
		/* get the appropriate velocity function from vv into vrms */
		get_vel(vv,stval,stval_ind,stval_sort,nstval,nt,type,val,vrms);
		
		/* Process the record */
		
		{ float **px,*ut,*tu;
		  complex **pk;
		  float du;
		  int nu; 
		  float vstolt=vmin;
		  int itr,ntr2;
		  float nxpad;
		  int nxfft;
		  int nk,ik,iu;
		  float dx,dk,scale,k;
		  int ntm;
		  float offmax,offmin;
		  int ntr0;	/* number of traces in the gather padded up to zero offset */
		  int ftr;	/* location of the first trace of the csp gather in px array */
		  
		  
			offmax = (*rec_o[ntr-1]).offset;
			offmin = (*rec_o[0]).offset;
			dx = (offmax-offmin)/(ntr-1);
			ntr0 = NINT(offmax/dx);
			ftr = NINT(offmin/dx);
		  	ntr2=2*ntr0;
			
			/* make u(t) and t(u) for Stolt stretch */
			makeut(vstolt,fmax,vrms,nt,dt,&ut,&nu,&du,&tu);
			
			/* wavenumber (k) sampling */
			nxpad = 0.5*vmax*nt*dt/dx;
			nxfft = npfar(ntr2+nxpad);
			nk = nxfft/2+1;
			dk = 2.0*PI/(nxfft*dx);
			scale=1.0/nxfft;
			
			/* Allocate gathers for the migration */
			ntm=MAX(nu,nt);
			px = ealloc2float(ntm,nxfft);
			pk = ealloc2complex(ntm,nk);
			
			/* setup the gather for migraton */
			for(itr=0;itr<ntr;itr++) {
				
				/* if necessary, stretch */
				if (nu!=nt && tu!=NULL) {
					float *temp=ealloc1float(nu);
					for (itr=ftr; itr<ntr; ++itr) {
						ints8r(nt,dt,0.0,(*rec_o[itr]).data,0.0,0.0,nu,tu,temp);
						memcpy((void *) px[ntr-1-itr],(const void *) temp, nu*FSIZE);
						memcpy((void *) px[ntr+itr],(const void *) temp, nu*FSIZE);
					}
					free1float(temp);
				} else {
					memcpy((void *) px[ntr-1-itr],(const void *) (*rec_o[itr]).data, nt*FSIZE);
					memcpy((void *) px[ntr+itr],(const void *) (*rec_o[itr]).data, nt*FSIZE);
				}
			}
			
			/* Zero from ntr2 tp nxfft */
			for(itr=ntr2;itr<nxfft;itr++) 
				memset( (void *) px[itr],  (int) '\0', nt*FSIZE);	
		
                        /* free unstreched gathers */
			for(itr=0;itr<ntr;itr++) {
                               free((void*) rec_o[itr]);
                        }

			/* Fourier transform p(u,x) to p(u,k) */
			pfa2rc(-1,2,ntm,nxfft,px[0],pk[0]);

			/* migrate each wavenumber */
			for (ik=1,k=dk; ik<nk; ++ik,k+=dk)
				stolt1k(k,vstolt,w,fmax,
					nu,du,pk[ik],pk[ik]);

			/* Fourier transform p(u,k) to p(u,x) and scale */
			pfa2cr(1,2,ntm,nxfft,pk[0],px[0]);
			for (itr=0; itr<ntr2; ++itr)
				for (iu=0; iu<nu; ++iu)
					px[itr][iu] *= scale;

			/* if necessary, unstretch */
			if (nu!=nt && ut!=NULL) {
				float *temp=ealloc1float(nt);
				for (itr=0; itr<ntr2; ++itr) {
					ints8r(nu,du,0.0,px[itr],0.0,0.0,
						nt,ut,temp);
					memcpy((void *) px[itr],(const void *) temp, nu*FSIZE);
				}
				free1float(temp);
			}
					
                    /*    for(itr=0;itr<ntr2;itr++) {
				memcpy((void *) outtrace.data,(const void *) px[itr], nt*FSIZE);
				outtrace.ns=nt;
				outtrace.dt=(short)((double)dt*1000000.0);
          			puttr(&outtrace);
                        }
		    */
			memcpy((void *) outtrace.data,(const void *) px[ntr-1], nt*FSIZE);
			outtrace.ns=nt;
			outtrace.dt=(short)((double)dt*1000000.0);
          		puttr(&outtrace);
		    
		    	
			free1float(ut);
			free1float(tu);
			free2float(px);
			free2complex(pk);
			
                }
		
		
			
                ng++;
                rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
		
        } while(ntr);
			
			
	if(verbose) warn("Number of gathers= %d \n",ng);
	free1float(migdata);

   	return EXIT_SUCCESS;
}
	
void get_vel(float **v,float *stvel,int *stval_ind,float **stval_sort, 
	     int nval,int nt,cwp_String type,Value  val, float *vrms)
{
	float fvalue;
	int index;
	
	fvalue = vtof(type,val);
/*	fprintfval(stderr, type, val);
	fprintf(stderr,"\n");
*/	
	/* find the value fvalue among the elements of nstval */
	/* I it is not there give an error */
	if(fvalue < *stval_sort[0]) 
			err(" 1 Value # %f not in velocity file\n",fvalue);
	if(fvalue > *stval_sort[nval-1]) 
			err(" 2 Value # %f not in velocity file\n",fvalue);

	/* Value should be within the bounds */
	xindex_p(nval,stval_sort,fvalue,&index);
	if(fvalue != *stval_sort[index])  
			err(" 3 Value # %f not in velocity file\n",fvalue);
	
	/* Reference back index to the true ordering of v arrays */
	/* and copy v values from v matrix to vrms vector */
	memcpy( (void *) vrms, (const void *) v[stval_ind[index]],nt*FSIZE);
}

static void
xindex_p (int nx, float **ax, float x, int *index)
/*****************************************************************************
determine index of x with respect to an array of x values
******************************************************************************
Input:
nx		number of x values in array ax
ax		array[nx] of monotonically increasing or decreasing x values
x		the value for which index is to be determined
index		index determined previously (used to begin search)

Output:
index		for monotonically increasing ax values, the largest index
		for which ax[index]<=x, except index=0 if ax[0]>x;
		for monotonically decreasing ax values, the largest index
		for which ax[index]>=x, except index=0 if ax[0]<x
******************************************************************************
Notes:
This function is designed to be particularly efficient when called
repeatedly for slightly changing x values; in such cases, the index 
returned from one call should be used in the next.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 12/25/89
         Balazs Nemeth, Modified to work with pointer arrays pointing to 
	 floats 11/26/99 
*****************************************************************************/
{
	int lower,upper,middle,step;

	/* initialize lower and upper indices and step */
	lower = *index;
	if (lower<0) lower = 0;
	if (lower>=nx) lower = nx-1;
	upper = lower+1;
	step = 1;

	/* if x values increasing */
	if (*ax[nx-1]>*ax[0]) {

		/* find indices such that ax[lower] <= x < ax[upper] */
		while (lower>0 && *ax[lower]>x) {
			upper = lower;
			lower -= step;
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && *ax[upper]<=x) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (x>=*ax[middle])
				lower = middle;
			else
				upper = middle;
		}

	/* else, if not increasing */
	} else {

		/* find indices such that ax[lower] >= x > ax[upper] */
		while (lower>0 && *ax[lower]<x) {
			upper = lower;
			lower -= step;
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && *ax[upper]>=x) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (x<=*ax[middle])
				lower = middle;
			else
				upper = middle;
		}
	}

	/* return lower index */
	*index = lower;
}

/* SU sustolt functions */

static void makeut (float vstolt, float fmax, float *vrms,
	int nt, float dt, float **ut, int *nu, float *du, float **tu)
/*****************************************************************************
Compute u(t) and t(u) for Stolt stretch
******************************************************************************
Input:
vstolt		Stolt migration velocity
fmax		maximum frequency
vrms		array[nt] of RMS velocities
nt		number of t samples
dt		t sampling interval (first t assumed to be zero)

Output
ut		array[nt] of u(t); NULL if constant velocity
nu		number of u samples
du		u sampling interval (first u assumed to be zero)
tu		array[nu] of t(u); NULL if constant velocity
*****************************************************************************/
{
	int it;
	
	/* check for constant velocity */
	for (it=1; it<nt; ++it)
		if (vrms[it]!=vrms[0]) break;
		
	/* if constant velocity */
	if (it==nt) {
		*ut = NULL;
		*tu = NULL;
		*nu = nt;
		*du = dt;

	/* else if velocity not constant */
	} else {
		int it,nuu;
		float duu,delu,umax,*u,*t;
		
		/* u(t) */
		u = alloc1float(nt);
		makeu(vstolt,vrms,nt,dt,u);

		/* smallest du and maximum u */
		duu = FLT_MAX;
		for (it=1; it<nt; ++it) {
			delu =	u[it]-u[it-1];
			if (delu<duu) duu = delu;
		}
		umax = u[nt-1];

		/* u sampling */
		duu = duu/(2.0*fmax*dt);
		nuu = 1+NINT(umax/duu);

		/* t(u) */
		t = alloc1float(nuu);
		yxtoxy(nt,dt,0.0,u,nuu,duu,0.0,0.0,(nt-1)*dt,t);

		/* set output parameters before returning */
		*ut = u;
		*tu = t;
		*nu = nuu;
		*du = duu;
	}
}



static void makeu (float vstolt, float *v, int nt, float dt, float *u)
/*****************************************************************************
Compute				     t
	u(t) = sqrt( 2/vstolt^2 * Integral ds*s*(v(s)^2) )
				     0
via the trapezoidal rule.
******************************************************************************
Input:
vstolt		Stolt migration velocity
v		array[nt] of RMS velocities
nt		number of t samples
dt		t sampling interval

Output
u		array[nt] of u(t)
*****************************************************************************/
{
	int it;
	float t,scale,sum;

	scale = 2.0/(vstolt*vstolt);
	u[0] = sum = 0.0;
	for (it=1,t=dt; it<nt; ++it,t+=dt) {
		sum += 0.5*dt*(t*v[it]*v[it]+(t-dt)*v[it-1]*v[it-1]);
		u[it] = sqrt(scale*sum);
	}
}

static void stolt1k (float k, float v, float s, float fmax, int nt, float dt,
	complex *p, complex *q)
/*****************************************************************************
Stolt's migration for one wavenumber k
******************************************************************************
Input:
k		wavenumber
v		velocity
s		Stolt stretch factor (0<s<2; use s=1 for constant velocity)
fmax		maximum frequency (in cycles per unit time)
nt		number of time samples
dt		time sampling interval (first time assumed to be zero)
p		array[nt] containing data to be migrated

Output
q		array[nt] containing migrated data (may be equivalenced to p)
*****************************************************************************/
{
	int nw,it,nwtau,iwtau,ntau,itau,iwtaul,iwtauh;
	float vko2s,wmax,dw,fw,dwtau,fwtau,wtau,dtau,
		wtauh,wtaul,scale,fftscl,a,b,*wwtau;
	complex czero=cmplx(0.0,0.0),*pp,*qq;

	/* modify stolt stretch factor to simplify calculations below */
	if (s!=1.0) s = 2.0-s;

	/* (v*k/2)^2 */
	vko2s = 0.25*v*v*k*k;

	/* maximum frequency to migrate in radians per unit time */
	wmax = 2.0*PI*MIN(fmax,0.5/dt);

	/* frequency sampling - must pad to avoid interpolation error;
	 * pad by factor of 2 because time axis is not centered;
	 * pad by factor of 1/0.6 because 8-point sinc is valid
	 * only to about 0.6 Nyquist
	 */
	nw = nt*2/0.6;
	nw = npfao(nw,nw*2);
	dw = 2.0*PI/(nw*dt);
	fw = -PI/dt;

	/* migrated time */
	ntau = nt;
	dtau = dt;

	/* migrated frequency - no need to pad since no interpolation */
	nwtau = npfao(ntau,ntau*2);
	dwtau = 2.0*PI/(nwtau*dtau);
	fwtau = -PI/dtau;

	/* tweak first migrated frequency to avoid wtau==0.0 below */
	fwtau += 0.001*dwtau;

	/* high and low migrated frequencies - don't migrate evanescent */
	wtauh = sqrt(MAX(0.0,wmax*wmax-s*vko2s));
	iwtauh = MAX(0,MIN(nwtau-1,NINT((wtauh-fwtau)/dwtau)));
	iwtaul = MAX(0,MIN(nwtau-1,NINT((-wtauh-fwtau)/dwtau)));
	wtauh = fwtau+iwtauh*dwtau;
	wtaul = fwtau+iwtaul*dwtau;

	/* workspace */
	pp = alloc1complex(nw);
	qq = alloc1complex(nwtau);
	wwtau = alloc1float(nwtau);

	/* pad with zeros and Fourier transform t to w, with w centered */
	for (it=0; it<nt; it+=2)
		pp[it] = p[it];
	for (it=1; it<nt; it+=2) {
		pp[it].r = -p[it].r;
		pp[it].i = -p[it].i;
	}
	for (it=nt; it<nw; ++it)
		pp[it].r = pp[it].i = 0.0;
	pfacc(1,nw,pp);

	/* zero -Nyquist frequency for symmetry */
	pp[0] = czero;

	/* frequencies at which to interpolate */
	if (s==1.0) {
		for (iwtau=iwtaul,wtau=wtaul; wtau<0.0; ++iwtau,wtau+=dwtau)
			wwtau[iwtau] = -sqrt(wtau*wtau+vko2s);
		for (; iwtau<=iwtauh; ++iwtau,wtau+=dwtau)
			wwtau[iwtau] = sqrt(wtau*wtau+vko2s);
	} else {
		a = 1.0/s;
		b = 1.0-a;
		for (iwtau=iwtaul,wtau=wtaul; wtau<0.0; ++iwtau,wtau+=dwtau)
			wwtau[iwtau] = b*wtau-a*sqrt(wtau*wtau+s*vko2s);
		for (; iwtau<=iwtauh; ++iwtau,wtau+=dwtau)
			wwtau[iwtau] = b*wtau+a*sqrt(wtau*wtau+s*vko2s);
	}
	
	/* interpolate */
	ints8c(nw,dw,fw,pp,czero,czero,
		iwtauh-iwtaul+1,wwtau+iwtaul,qq+iwtaul);

	/* fft scaling and obliquity factor */
	fftscl = 1.0/nwtau;
	if (s==1.0) {
		for (iwtau=iwtaul,wtau=wtaul; iwtau<=iwtauh; 
			++iwtau,wtau+=dwtau) {
			scale = fftscl*wtau/wwtau[iwtau];
			qq[iwtau].r *= scale;
			qq[iwtau].i *= scale;
		}
	} else {
		a = 1.0/(s*s);
		b = 1.0-1.0/s;
		for (iwtau=iwtaul,wtau=wtaul; iwtau<=iwtauh; 
			++iwtau,wtau+=dwtau) {
			scale = fftscl*(b+a*wtau/(wwtau[iwtau]-b*wtau));
			qq[iwtau].r *= scale;
			qq[iwtau].i *= scale;
		}
	}

	/* zero evanescent frequencies */
	for (iwtau=0; iwtau<iwtaul; ++iwtau)
		qq[iwtau] = czero;
	for (iwtau=iwtauh+1; iwtau<nwtau; ++iwtau)
		qq[iwtau] = czero;

	/* Fourier transform wtau to tau, accounting for centered wtau */
	pfacc(-1,nwtau,qq);
	for (itau=0; itau<ntau; itau+=2)
		q[itau] = qq[itau];
	for (itau=1; itau<ntau; itau+=2) {
		q[itau].r = -qq[itau].r;
		q[itau].i = -qq[itau].i;
	}
	
	/* free workspace */
	free1complex(pp);
	free1complex(qq);
	free1float(wwtau);
}
		
