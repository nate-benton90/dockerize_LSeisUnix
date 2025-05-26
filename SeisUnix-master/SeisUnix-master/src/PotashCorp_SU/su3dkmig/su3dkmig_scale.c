/* su3dkmig */
/* B.Nemeth */
float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#define I               cmplx(0.0, 1.0)
#define PIBY2           0.5 * PI
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */

/*********************** self documentation *****************************/
char *sdoc[] = {"  SU program to perform 3D Kirchhoff migration         ",
"  sudkmig fs=stack fv=velocity volume                                 ",
"                                                                           ",
"									    ",
"  Required parameters:							    ",
"  The data volume has to be square and sorted.				    ",
"  The number of traces in second direction is nx in the third direction is ny",
"  To get the data sorted in the right way susort <inf >outf f3 f2	    ",
"  where f2 is the header word containing the numbering in the second direction  ",
"  and  f3 is the header word containing the numbering in the third  direction   ",
"  The velocity voulme has to be sorted in the same order and it also has to",
"  be the same dimensions as the data volume. 		                    ",
"  Timewise the velocity volume could be sampled with a different rate as the data ",
"  volume as these are going to be interpolated				    ",
"  TRACES WITH TR.TRID SET TO > 1 ARE NOT GOING TO BE MIGRATED              ",
"                                                                           ",
"  n2=			number of traces in second dimension		    ",
"  n3=			number of traces in third  dimension		    ",
"  d2=			data spatial sampling in dir. 2			    ",
"  d3=			data spatial sampling in dir. 3			    ",
"  apr=			migration apperture in degrees			    ",
"  dm=(d2+d3)/2		migration gather spatial sampling int.		    ",
"  fac=2.0		velocity scale factor				    ",
"  sphr=0		1 restore spherical divergence			    ",
NULL};
   
segy tr;			  /* SEGY data trace */
segy trv;			  /* SEGY velocity trace */

void stncl(float hm, float dhx, float dhy,int **in, int *nin);
void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph);

int main( int argc, char *argv[] )
{
        int ntr=0;                /* number of traces                     */
        int ntrv=0;               /* number of traces                     */
	int ns=0;
	int nsv=0;
	float dt;
	float dtv;
	
	cwp_String fs;
	cwp_String fv;
	FILE *fps;
	FILE *fpv;
	FILE *headerfp;
		
	float *data;		/* data matrix of the migration volume */
	float *vel;		/* velocity matrix */
	float *velfi;		/* velocity function interpolated to ns values*/
	float *velf;		/* velocity function */
	float *vdt;
	float *ddt;
	float *ap;		/* array of apperture values in m */
	float apr;		/* array of apperture values in m */
	int *apt=NULL;		/* array of apperture time limits in mig. gath*/
	float   r;		/* maximum radius with a given apperture */
	float ir2;		/* r/d2 */
	float ir3;		/* r/d3 */
	float d2;		/* spatial sampling int. in dir 2. */
	float d3;		/* spatial sampling int. in dir 3. */
	float **mgd=NULL;	/* migration gather data */
	float *migt;		/* migrated data trace */
	int **mgdnz=NULL;		/* migration gather data non zero samples*/
	float dm;		/* migration gather spatial sample int. */
	int im;			/* number of traces in migration gather */
	int *mtnz;		/* migrated trace data non zero smaples */
	char **dummyi;		/* index array that the trace contains zeros only */
	float fac;		/* velocity scale factor */
	int sphr;		/* spherical divergence flag */
	int imt;		/* mute time sample of trace */
	float tmp;
	int imoff;
	int **igtr=NULL;
	int nigtr;
	int n2;
	int n3;

	int verbose;
	
	/* phase shift filter stuff */
        float power;            /* power of i omega applied to data     */
        float amp;              /* amplitude associated with the power  */
        float arg;              /* argument of power                    */
        float phasefac;         /* phase factor                         */
        float phase;            /* phase shift = phasefac*PI            */
        complex exparg;         /* cexp(I arg)                          */
        register float *rt;     /* real trace                           */
        register complex *ct;   /* complex transformed trace            */
        complex *filt;          /* complex power                        */
        float omega;            /* circular frequency                   */
        float domega;           /* circular frequency spacing (from dt) */
        float sign;             /* sign in front of i*omega default -1  */
        int nfft;               /* number of points in nfft             */
        int nf;                 /* number of frequencies (incl Nyq)     */
        float onfft;            /* 1 / nfft                             */
        size_t nzeros;          /* number of padded zeroes in bytes     */
	
	initargs(argc, argv);
   	requestdoc(1);
	
        MUSTGETPARSTRING("fs",&fs);
        MUSTGETPARSTRING("fv",&fv);
        MUSTGETPARINT("n2",&n2);
        MUSTGETPARINT("n3",&n3);
        MUSTGETPARFLOAT("d2",&d2);
        MUSTGETPARFLOAT("d3",&d3);
	
	if (!getparfloat("dm", &dm))	dm=(d2+d3)/2.0;
	
	/* open datafile */
        fps = efopen(fs,"r");
	fpv = efopen(fv,"r");
	
	/* Open tmpfile for headers */
	headerfp = etmpfile();

	/* get information from the first data trace */
	ntr = fgettra(fps,&tr,0);
	if(n2*n3!=ntr) err(" Number of traces in file %d not equal to n2*n3 %d \n",
			     ntr,n2*n3);
	ns=tr.ns;
	if (!getparfloat("dt", &dt))	dt = ((float) tr.dt)/1000000.0;
	if (!dt) {
		dt = .002;
		warn("dt not set, assumed to be .002");
	}

	/* get information from the first velocity trace */
	ntrv = fgettra(fpv,&trv,0);
	if(ntrv!=ntr) err(" Number of traces in velocity file %d differ from %d \n",
			     ntrv,ntr);
	nsv=trv.ns;
	if (!getparfloat("dtv", &dtv))	dtv = ((float) trv.dt)/1000000.0;
	if (!dtv) {
		dtv = .002;
		warn("dtv not set, assumed to be .002 for velocity");
	}
	
	if (!getparfloat("fac", &fac))	fac=2.0;
	if (!getparint("verbose", &verbose))	verbose=0;
	if (!getparint("sphr", &sphr))	sphr=0;
	
	if (!getparfloat("apr", &apr))	apr=75;
	apr*=3.141592653/180;

	/* allocate arrays */
	data = bmalloc(sizeof(float),ns,ntr);
	vel = bmalloc(sizeof(float),nsv,ntr);
	velf = ealloc1float(nsv); 
	velfi = ealloc1float(ns);
	migt = ealloc1float(ns);
	vdt = ealloc1float(nsv);
	ddt = ealloc1float(ns);
	ap = ealloc1float(ns);
	mtnz = ealloc1int(ns);
	dummyi = (char **) ealloc2(n2,n3,sizeof(char));
	
	/* Times to do interpolation of velocity from sparse sampling */
	/* to fine sampling of the data */
	{ register int it;
		for(it=0;it<nsv;it++) vdt[it]=it*dtv;
		for(it=0;it<ns;it++)  ddt[it]=it*dt;
	}
	
	/* Read traces into data */
        /* Store headers in tmpfile */
        ntr=0;
	erewind(fps);
	erewind(fpv);
		
	{ register int i2,i3;
	for(i3=0;i3<n3;i3++) 
		for(i2=0;i2<n2;i2++) {
			fgettr(fps,&tr);
			fgettr(fpv,&trv);
			if(tr.trid > 2) dummyi[i3][i2]=1;
			else dummyi[i3][i2]=0;	
			efwrite(&tr, 1, HDRBYTES, headerfp);
		 	bmwrite(data,1,0,i3*n2+i2,ns,tr.data);
		 	bmwrite(vel,1,0,i3*n2+i2,nsv,trv.data);
		}
	erewind(headerfp);

	/* set up the phase filter */
	power = 1.0;sign = 1.0;phasefac = 0.5;
	phase = phasefac * PI;
         
	/* Set up for fft */
        nfft = npfaro(ns, LOOKFAC * ns);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;
        onfft = 1.0 / nfft;
        nzeros = (nfft - ns) * FSIZE;
        domega = TWOPI * onfft / dt;
        
	/* Allocate fft arrays */
        rt   = ealloc1float(nfft);
        ct   = ealloc1complex(nf);
        filt = ealloc1complex(nf);
        
	/* Set up args for complex power evaluation */
        arg = sign * PIBY2 * power + phase;
        exparg = cexp(crmul(I, arg));
        {       
		register int i;
                for (i = 0 ; i < nf; ++i) {

                        omega = i * domega;
		
		        /* kludge to handle omega=0 case for power < 0 */
                        if (power < 0 && i == 0) omega = FLT_MAX;

                        /* calculate filter */
                        amp = pow(omega, power) * onfft;
			filt[i] = crmul(exparg, amp);
                }
        }
	
	/* set up constants for migration */ 
	if(verbose) fprintf(stderr," Setting up constants....\n");
	r=0;
	for(i3=0;i3<n3;i3++) 
	    for(i2=0;i2<n2;i2++) {
		if(dummyi[i3][i2] < 1) {
			
			/* get the velocity function */
			bmread(vel,1,0,i3*n2+i2,nsv,velf);
			
			/* linear interpolation from nsv to ns values */  
			intlin(nsv,vdt,velf,velf[0],velf[nsv-1],ns,ddt,velfi);
			
			/* Apply scale factor to velocity */
			{ register int it;
				for(it=0;it<ns;it++) velfi[it] *=fac;
			}
			
			/* compute maximum radius from apperture and velocity */
			{ register int it;
				for(it=0;it<ns;it++) 
				ap[it] = ddt[it]*velfi[it]*tan(apr)/2.0;
			}
			tmp = ap[isamax(ns,ap,1)];
			if(tmp>r) r=tmp;
		}
	}
	r=MIN(r,sqrt(SQR((n2-1)*d2)+SQR((n3-1)*d3)));
	ir2 =  (int)(2*r/d2)+1;
	ir3 =  (int)(2*r/d3)+1;
	im = (int)(r/dm)+1;
		
	/*  allocate migration gather */
	mgd = ealloc2float(ns,im);
	mgdnz = ealloc2int(ns,im);
	apt = ealloc1int(im);
	/* set up the stencil for selecting traces */
	igtr = ealloc2int(ir2*ir3,2);
	stncl(r, d2, d3,igtr,&nigtr);
	
	if(verbose) {
		fprintf(stderr," Maximum radius %f\n",r);
		fprintf(stderr," Maximum offset %f\n",
			sqrt(SQR((n2-1)*d2)+SQR((n3-1)*d3)));
	}

	/* main processing loop */
	for(i3=0;i3<n3;i3++) 
	    for(i2=0;i2<n2;i2++) {
		memset( (void *) tr.data, (int) '\0',ns*FSIZE);
		if(dummyi[i3][i2] < 1) {
			memset( (void *) mgd[0], (int) '\0',ns*im*FSIZE);
			memset( (void *) mgdnz[0], (int) '\0',ns*im*ISIZE);
			/* get the velocity function */
			bmread(vel,1,0,i3*n2+i2,nsv,velf);
		
			/* linear interpolation from nsv to ns values */  
			intlin(nsv,vdt,velf,velf[0],velf[nsv-1],ns,ddt,velfi);
		
			/* Apply scale factor to velocity */
			{ register int it;
				for(it=0;it<ns;it++) velfi[it] *=fac;
			}

			/* create the migration gather */
			{ register int itr,ist2,ist3;
				for(itr=0;itr<nigtr;itr++) {
					ist2=i2+igtr[0][itr];
					ist3=i3+igtr[1][itr];
					if(ist2 >= 0 && ist2 <n2) 
						if(ist3 >= 0 && ist3 <n3) {
							if(dummyi[ist3][ist2] <1) {
								imoff = (int) ( 
								sqrt(SQR(igtr[0][itr]*d2)
							     	    +SQR(igtr[1][itr]*d3))/dm+0.5);
								bmread(data,1,0,ist3*n2+ist2,ns,tr.data);
								imoff=MIN(imoff,im-1);
								{ register int it;									
									/* get the mute time for this 
									  offset, apperture and velocity */
									xindex(ns,ap,imoff*dm,&imt);
									for(it=imt;it<ns;it++)
										if(tr.data[it]!=0) {
											mgd[imoff][it]+=tr.data[it];
											mgdnz[imoff][it]+=1;
									}	
								}
							}
						}
				}
			}

			/* normalize the gather */
				{ register int ix,it;
				for(ix=0;ix<im;ix++)
					for(it=0;it<ns;it++) 
						if(mgdnz[ix][it] > 1) mgd[ix][it] /=(float) mgdnz[ix][it];
			}
			memset( (void *) tr.data, (int) '\0',ns*FSIZE);
			memset( (void *) mtnz, (int) '\0',ns*ISIZE);
		
			/* do a knmo */
			{ register int ix,it;
				for(ix=0;ix<im;ix++) {
					/* get the mute time for this 
					offset, apperture and velocity */
					xindex(ns,ap,ix*dm,&imt);
					knmo(mgd[ix],migt,ns,velfi,0,ix*dm,dt,imt,sphr);
					/* stack the gather */
						for(it=0;it<ns;it++) { 
						if(migt[it]!=0.0) { 
								tr.data[it] += migt[it];
								mtnz[it]++;
						}
/*						tr.data[it] += mgd[ix][it]; */
					}
				}

			}
			{ register int it;
				for(it=0;it<ns;it++) 
					if(mtnz[it]>1) tr.data[it] /=(float)mtnz[it];
			}
		
			/*Do the phase filtering before the trace is released*/
                	/* Load trace into rt (zero-padded) */
               		memcpy( (void *) rt, (const void *) tr.data, ns*FSIZE);
               		memset((void *) (rt + ns), (int) '\0', nzeros);

         		pfarc(1, nfft, rt, ct);
        		{ register int i;
        			for (i = 0; i < nf; ++i)  ct[i] = cmul(ct[i], filt[i]);
        		}
         		pfacr(-1, nfft, ct, rt);
     			memcpy( (void *) tr.data, (const void *) rt, ns*FSIZE);
			
		} /* end of dummy if */
		/* spit out the gather */
		efread(&tr, 1, HDRBYTES, headerfp);
		puttr(&tr);
		if(verbose) fprintf(stderr," %d %d\n",i2,i3);
	    }   /* end of i2 loop */
	}	/* end of i3 loop */
	/* This should be the last thing */
	efclose(headerfp);
	/* Free memory */
	free2int(igtr);
	free2float(mgd);
	free2int(mgdnz);
	free1int(apt);
	bmfree(data);
	bmfree(vel);
	free1float(velfi);
	free1float(velf);
	free1float(ddt);
	free1float(vdt);
	free1float(ap);
	free1int(mtnz);
	free1float(migt);
	free1float(rt);
	free1complex(ct);
	free1complex(filt);
	free2((void **) dummyi);
	
	return EXIT_SUCCESS;
}
void stncl(float hm, float dhx, float dhy,int **in, int *nin)
/* select a circular pattern from a rectangular grid */
/* hm radius of the circle, dhx, dhy, grid sizes in x and y directions */
/* in is a two dimensional array of indexes of values falling within the circle */
/*  such that sqrt(sqr(dhx*in[0])+sqr(dhy*in[1]))<=hm */
/* The center is at 0,0 */
/* in has to be declared outside large enough, typicalli 2*hm/dhx*2*hm/dhy */
/* nin is the number of values in in */
{
	int u,k;
	float r;
	int n,cnt;
	
	n=(int)(hm/dhx+0.5);
	cnt=0;
	for(k=-n; k<=n; k++) {
		u=0;
		r=(float)sqrt((double)(SQR(k*dhx)+SQR(u*dhy)));
		while(r <= hm) {
			in[0][cnt]=k;
			in[1][cnt]=u;
			u++;
			r=sqrt(SQR(k*dhx)+SQR(u*dhy));
			cnt++;
		}
	}
	for(k=-n; k<=n; k++) {
		u=-1;
		r=(float)sqrt((double)(SQR(k*dhx)+SQR(u*dhy)));
		while(r <= hm) {
			in[0][cnt]=k;
			in[1][cnt]=u;
			u--;
			r=sqrt(SQR(k*dhx)+SQR(u*dhy));
			cnt++;
		}
	}
	*nin=cnt;
}
void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph)
{
/* This routine applies the moveout corection, obliquity factor and 
	spherical divergence factor to the gather. The phase shift filter has to be
	applied to the stacked trace */
/* imt index of first live sample in input trace */
	register int i;
	float tx;
	float txa;	/* amplitude of interpolated tx */
	float of2;
	float r;
	float z;
	float t;
	float tmax;
	float tm;
		
	
	tm=imt*dt;
	if (offset==0.0) {
		for(i=0;i<nt;i++) {
			t=t0+i*dt;
			if(sph) { z=vrms[i]/2.0*t;
				  outd[i] = ind[i]*(vrms[i]*z);
			} else 
			outd[i] = ind[i];
		}
	} else {	
		memset( (void *) outd, (int) '\0',nt*FSIZE);
		of2=4.0*SQR(offset);
		tmax=t0+(nt-1)*dt;
		for(i=imt;i<nt;i++) {
			t=t0+i*dt;
			
			/* knmo moveout */
			tx = sqrt(SQR(t) + of2/SQR(vrms[i]));
			if(tx<tmax && tx>tm) { 
				ints8r(nt,dt,t0,ind,0,0,1,&tx,&txa);
				
				/* obliquity factor */
				txa *=tx/t;
				
				if(sph) { /* 3D spherical divergence */
					z=vrms[i]/2.0*(t);
					r=sqrt(SQR(offset)+SQR(z));
					txa *=vrms[i]*r;
				}
					
				outd[i]=txa;
			}
		}
	}
	/* tapering and zeroing above the apperture */
	{ register int si,ei,it;
		si=MAX(imt,0);
		ei=MIN(imt+30,nt);
 		memset( (void *) &outd[0], (int) '\0',si*FSIZE);
		for(it=si;it<ei;it++) outd[it]
			*=(float)sin((double)((it-si)*1.57/30.0));
	}
	return;
}
