/* su3dkmig */
/* B.Nemeth */
/*float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg) */


#include "suhdr.h"
#define I               cmplx(0.0, 1.0)
#define PIBY2           0.5 * PI
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */
#define TPR 100

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
"  facs=1.0		velocity scale factor at zero time		    ",
"  face=1.0		velocity scale factor at end of trace		    ",
"  test=0               1 do not migrate just scan the data and echo info   ",
NULL};
   
segy tr;			  /* SEGY data trace */
segy trv;			  /* SEGY velocity trace */

void stncl(float hm, float dhx, float dhy,int **in, int *nin,float *dom);
void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph);
void xinterp(float **in_traces, float **out_traces, int ninterp, 
		int nt, int nx, float freq1, float freq2, int lagc, 
		int lent, int lenx, int xopt, float dt, int iopt);
void runav(int n,int len,float *a,float *b);
		
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
	int itp;		/* number of traces to insert between two migration gather traces */
	int *apt=NULL;		/* array of apperture time limits in mig. gath*/
	float   r;		/* maximum radius with a given apperture */
	float ir2;		/* r/d2 */
	float ir3;		/* r/d3 */
	float d2;		/* spatial sampling int. in dir 2. */
	float d3;		/* spatial sampling int. in dir 3. */
	float **mgd=NULL;	/* migration gather data */
	float **mgdi=NULL;	/* interpolated migration gather data */
	float *migt;		/* migrated data trace */
	float dm;		/* migration gather spatial sample int. */
	int im;			/* number of traces in migration gather */
	int imi;		/* number of traces in densified migration gather */
	int *hmax;		/* migrated trace data non zero smaples */
	int *trzi;		/* index of zero of a trace */
	char **dummyi;		/* index array that the trace contains zeros only */
	float facs;		/* velocity scale factor at beginning of trace*/
	float face;		/* velocity scale factor at end of trace */
	float dfac;		/* velocity scale factor change */
	int sphr;		/* spherical divergence flag */
	int imt;		/* mute time sample of trace */
	float tmp;
	int imoff;
	int **igtr=NULL;
	int nigtr;
	int n2;
	int n3;
	float vmin=9999;	/* Minimum migration velocity */
	float vmax=-9999;	/* Maximum migration velocity */
	float dom;		/* the largest offset gap  
				   among traces in migration gather */
	int test;		/* test flag */

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
	
	if (!getparfloat("facs", &facs))	facs=1.0;
	if (!getparfloat("face", &face))	face=facs;
	dfac=(face-facs)/(ns-1);
	if (!getparint("verbose", &verbose))	verbose=0;
	if (!getparint("sphr", &sphr))	sphr=0;
	if (!getparint("itp", &itp))	itp=0;
	
	if (!getparfloat("apr", &apr))	apr=75;
	apr*=3.141592653/180;
	
	if (!getparint("test", &test))	test=0;

	/* allocate arrays */
	data = bmalloc(sizeof(float),ns,ntr);
	vel = bmalloc(sizeof(float),nsv,ntr);
	velf = ealloc1float(nsv); 
	velfi = ealloc1float(ns);
	migt = ealloc1float(ns);
	vdt = ealloc1float(nsv);
	ddt = ealloc1float(ns);
	ap = ealloc1float(ns);
	hmax = ealloc1int(ns);
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
	power = 1.0;sign = 1.0;phasefac = 0.0;
	phase = phasefac * PI;
         
	/* Set up for fft */
        nfft = npfar((int)1.5*ns);
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
			filt[i] = crmul(I,omega);			
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
				for(it=0;it<ns;it++) {
					velfi[it] *=(facs+dfac*it);
					if(vmin>velfi[it]) vmin=velfi[it];
					if(vmax<velfi[it]) vmax=velfi[it];
				}
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
	imi = (im-1)*(itp+1)+1;
		
	/*  allocate migration gather */
	mgd = ealloc2float(ns,im);
	mgdi = ealloc2float(ns,imi);
	apt = ealloc1int(im);
	trzi=ealloc1int(ns);
	/* set up the stencil for selecting traces */
	igtr = ealloc2int(ir2*ir3,2);
	stncl(r, d2, d3,igtr,&nigtr,&dom);
	
	if(verbose) {
		fprintf(stderr," Maximum Velocity %f\n",vmax);
		fprintf(stderr," Minimum Velocity %f\n",vmin);
		fprintf(stderr," Maximum radius %f\n",r);
		fprintf(stderr," Maximum offset %f\n",
			sqrt(SQR((n2-1)*d2)+SQR((n3-1)*d3)));
		fprintf(stderr," Maximum offset gap in migration gather %f\n",dom);
		if(dom>dm) fprintf(stderr," Warning Maximum offset gap is larger than dm !\n");
	}

	if(test) exit(1);
	/* main processing loop */
	for(i3=0;i3<n3;i3++) 
	    for(i2=0;i2<n2;i2++) {
		memset( (void *) tr.data, (int) '\0',ns*FSIZE);
		memset( (void *) trzi, (int) '\0',ns*FSIZE);
		if(dummyi[i3][i2] < 1) {
			
			/* Zero the migration gathers */
			{ int izt;
				for(izt=0;izt<im;izt++) {
					memset( (void *) mgd[izt], (int) '\0',ns*FSIZE);
				}
			}
			
			/* set the mask arry for the trace */
			bmread(data,1,0,i3*n2+i2,ns,tr.data);
			{ int itz=0;
				while(tr.data[itz]==0.0 && itz < ns) {
					trzi[itz]=1;
					itz++;
				}
			}
			
			/* get the velocity function */
			bmread(vel,1,0,i3*n2+i2,nsv,velf);
			
			/* linear interpolation from nsv to ns values */  
			intlin(nsv,vdt,velf,velf[0],velf[nsv-1],ns,ddt,velfi);
		
			/* Apply scale factor to velocity */
			{ register int it;
				for(it=0;it<ns;it++) velfi[it] *=(facs+dfac*it);
			}

			/* create the migration gather */
			{ register int itr,ist2,ist3;
				for(itr=0;itr<nigtr;itr++) {
					
					/* trace indexes */
					ist2=i2+igtr[0][itr];
					ist3=i3+igtr[1][itr];
					if(ist2 >= 0 && ist2 <n2) 
						if(ist3 >= 0 && ist3 <n3) {
							if(dummyi[ist3][ist2] <1) {
								
								/* migration offset of the trace */
								imoff =NINT(sqrt(SQR(igtr[0][itr]*d2)
							     	                 +SQR(igtr[1][itr]*d3))/dm);
								
								/* get the trace */
								bmread(data,1,0,ist3*n2+ist2,ns,tr.data);
								imoff=MIN(imoff,im-1);
								{ int it;
									/* get the mute time for this 
									  offset, apperture and velocity */
									xindex(ns,ap,imoff*dm,&imt);
									for(it=imt;it<ns;it++) 
										mgd[imoff][it]+=tr.data[it];
								}
							}
						}
				}
			}

			/* Densify traces of the migration gather
			   with interpolation */
			{ float f1=5.0,f2=20.0;
			  int lagc=400,lent=5,lenx=1; 
				xinterp(mgd,mgdi,itp,ns,im,f1,f2,lagc,lent,lenx,0,dt,0);
			}
			
			/* Diagnostics */
			/*{ int ids,idt;
				for(idt=0;idt<im;idt++) {
					memcpy( (void *) &tr.data, (const void *) mgd[idt],ns*FSIZE);
					tr.ns=ns;
					tr.dt=dt*1000000.0;
					puttr(&tr);
				}  
				for(idt=0;idt<imi;idt++) {
					memcpy( (void *) &tr.data, (const void *) mgdi[idt],ns*FSIZE);
					tr.ns=ns;
					tr.dt=dt*1000000.0;
					puttr(&tr);
				} 
			} */
						
			memset( (void *) tr.data, (int) '\0',ns*FSIZE);
			memset( (void *) migt, (int) '\0',ns*FSIZE);
			memset( (void *) hmax, (int) '\0',ns*ISIZE);
			/* do a knmo */
			{ register int ix,it;
				   float dmi=dm/(itp+1);
				for(ix=0;ix<imi;ix++) {
					
					/* get the mute time for this 
					offset, apperture and velocity */
					xindex(ns,ap,ix*dmi,&imt);
					knmo(mgdi[ix],migt,ns,velfi,0,ix*dmi,dt,imt,sphr);
					
					/* Diagnostics */
					/*{ segy trt;
						memcpy( (void *) &trt.data, (const void *) migt,ns*FSIZE);
						trt.ns=ns;
						trt.dt=dt*1000000.0;
						trt.cdp=i3*1000+i2;
						puttr(&trt);
					} */
					
					/* summ the gather */
						for(it=0;it<ns;it++) { 
						if(migt[it]!=0.0) { 
								tr.data[it] += migt[it];
								/* record the maximum offset with nonzero value*/
								hmax[it]=ix;
						}
					}
				}

			}
			/* Weight the stacked trace by the apperture. */
			/* Because of the limited offset and time only a portion of the */
			/* diffraction contributes to the stacked trace. */
			/* This is going to be compensated here */
			{ int it;
			  float aprl,off; 
				for(it=0;it<ns;it++) {
					off = 2.0*hmax[it]*dm;
					if(hmax[it]) {
						aprl=atan2(off,(velfi[it]*dt*it));
						tr.data[it] *= PIBY2/aprl;
					}
				}
			} 
			
			/* Do the Kirchoff amplitude, time scaling*/
			if(sphr) {
				{ register int i;
        				tr.data[0]=0.0;
					for (i = 1; i < ns; ++i)  
						tr.data[i] *= d2*d3/(PI*SQR(velfi[i])*i*dt);
        			}
			}
		
			/*Do the phase filtering; */
			/* d/dt term in Kirchoff equation */
               		memcpy( (void *) rt, (const void *) tr.data, ns*FSIZE);
               		memset((void *) (rt + ns), (int) '\0', nzeros);

         		pfarc(1, nfft, rt, ct);
        		{ register int i;
        			for (i = 0; i < nf; ++i)  ct[i] = cmul(ct[i], filt[i]);
        		}
         		pfacr(-1, nfft, ct,tr.data);
        		
		} /* end of dummy if */
		
		efread(&tr, 1, HDRBYTES, headerfp);
		puttr(&tr);

		if(verbose==2) fprintf(stderr," %d %d\n",i2,i3);

	    }   /* end of i2 loop */
	}	/* end of i3 loop */
	/* This should be the last thing */
	efclose(headerfp);
	/* Free memory */
	free2int(igtr);
	free2float(mgd);
	free2float(mgdi);
	free1int(apt);
	bmfree(data);
	bmfree(vel);
	free1float(velfi);
	free1float(velf);
	free1float(ddt);
	free1float(vdt);
	free1float(ap);
	free1int(hmax);
	free1float(migt);
	free1float(rt);
	free1int(trzi);
	free1complex(ct);
	free1complex(filt);
	free2((void **) dummyi);
	
	return EXIT_SUCCESS;
}
void stncl(float hm, float dhx, float dhy,int **in, int *nin,float *dom)
/* select a circular pattern from a rectangular grid */
/* hm radius of the circle, dhx, dhy, grid sizes in x and y directions */
/* in is a two dimensional array of indexes of values falling within the circle */
/*  such that sqrt(sqr(dhx*in[0])+sqr(dhy*in[1]))<=hm */
/* The center is at 0,0 */
/* in has to be declared outside large enough, typicalli 2*hm/dhx*2*hm/dhy */
/* nin is the number of values in in */
/* *dom returnes the largest offset difference among the selected offset */
{
void uqsort(int n, int *un, float *a);

	int u,k;
	float r;
	int n,cnt,ucnt;
	float *of,ldo;
	
	n=NINT(hm/dhx);
	of = ealloc1float(4*n*n);
	cnt=0;
	for(k=-n; k<=n; k++) {
		u=0;
		r=(float)sqrt((double)(SQR(k*dhx)+SQR(u*dhy)));
		while(r <= hm) {
			in[0][cnt]=k;
			in[1][cnt]=u;
			u++;
			r=sqrt(SQR(k*dhx)+SQR(u*dhy));
			of[cnt]=r;
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
			of[cnt]=r;
			cnt++;
		}
	}
	*nin=cnt;
	
	/* do a unique sort on all offsets */
	uqsort(cnt,&ucnt,of);
	
	/* compute the largest offset difference */
	ldo=0;
	{ float dof;
		for(k=0;k<ucnt-1;k++) {
		 	dof=of[k+1]-of[k];
			if(ldo<dof) ldo=dof;
		}
	}
	
	*dom=ldo;
	
	free1float(of);
}

void uqsort(int n, int *un, float *a)
/* Unique sort of float array
   only outputs the first element of a equal sub-sequence
   the unique element are returned as the first un elmenst of array a
*/
{
        int i,in;
        
        if(n==1) {
                *un=1;
                return;
        }
        /* do a quick sort */
        qksort(n,a);
        
        /* check for equal sub-sequence */
        i=1;
        in=1;
        while(in<n) {   
                while(a[in]==a[i-1] && in!=n) {
                        in++;
                }
                a[i]=a[in];
                i++;
        }
        *un=i-1;
}

void knmo(float *ind, float *outd,int nt, float *vrms, 
          float t0,float offset,float dt,int imt,int sph)
{
/* This routine applies the moveout corection, obliquity factor
	The phase shift filter, spherical divergence factor
	have to be applied to the stacked trace */
/* imt index of first live sample in input trace */
	register int i;
	float *txa;	/* amplitude of interpolated tx */
	float of2;
	float t;
	float tm;
	float *ttn;
	float *atn;
	int ssc=1;
		
	
	tm=imt*dt;
	if (offset==0.0) {
		for(i=0;i<nt;i++) {
			t=t0+i*dt;
			outd[i] = ind[i];
		}
		return;
	} else {	
		memset( (void *) outd, (int) '\0',nt*FSIZE);
		ttn=ealloc1float(nt);
		atn=ealloc1float(nt);
		memset( (void *) ttn, (int) '\0',nt*FSIZE);
		
		/* Diffraction shape times */
		of2=4.0*SQR(offset)/(dt*dt);
		for(i=0;i<nt;i++) {
			ttn[i]=sqrt((float)i*(float)i+of2/SQR(vrms[i]));
		}
		
		/* Do not let diffraction xross each other */
		{
			for(i=nt-1;i>1;i--) 
				if(ttn[i-1] > ttn[i]) imt=MAX(i,imt);
		}
		
		/* inverse stretch factor */
                atn[0] = ttn[1]-ttn[0];
		for(i=1;i<nt;i++) 
			atn[i]=ttn[i]-ttn[i-1];
			
		ints8r(nt,1.0,0,ind,0.0,0.0,nt,&ttn[0],&outd[0]);
                
		/* if specified, scale by the stretch factor */
                if (ssc)
                	for (i=imt; i<nt; ++i)
                        	outd[i] *= atn[i];
				
		free1float(atn);
			
	}
		/* Scale by the obliquity factor To/T */ 
		
		for(i=imt;i<nt;i++) {
			txa = &outd[i];
			*txa *=SQR((float)i/ttn[i]);				
		}
		
	/* tapering and zeroing above and delow the data zone */
	{ int si,ei,it,itpr=TPR,itpra;
	
		si=MAX(imt,0);
		ei=MIN(imt+itpr,nt);
		itpra=ei-si+1;
		for(it=0;it<si;it++) outd[it]=0.0;
		for(it=si;it<ei;it++) outd[it]
			*=(float)sin((double)((it-si)*1.57/itpra));
/*		ilast = NINT(ttn[nt-1]);
		si=MIN(MAX(ilast-itpr,0),nt-1);
		ei=MAX(MIN(ilast,nt),0);
		itpra=ei-si+1;
		for(it=ei;it<nt;it++) outd[it]=0.0;
		for(it=si;it<ei;it++) outd[it]
			*=(float)sin((double)((ei-it-1)*1.57/itpra)); */
	}
		free1float(ttn);
	return;
}

void xinterp(float **in_traces, float **out_traces, int ninterp, 
		int nt, int nx, float freq1, float freq2, int lagc, 
		int lent, int lenx, int xopt, float dt, int iopt)
/******************************************************************************
interpolate input data in space placing "ninterp" synthetic traces 
between each pair of original input traces
*******************************************************************************
Function parameters:
Input:
int ninterp		number of traces to interpolate between each input trace
int nt			number of time samples
int nx			number of input traces
float freq1		low-end frequency for picking (good default: 3 Hz)
float freq2		high-end frequency for picking (good default: 20 Hz)
int lagc		length of AGC operator for picking (good default:400 ms)
int lent		length of time smoother in samples for picker
                        (good default: 5 samples)
int lenx		length of space smoother in samples for picker
                        (good default: 1 sample)
int xopt		1 = use differences for spatial derivative
                            (works with irregular spacing)
                        0 = use FFT derivative for spatial derivatives
                            (more accurate but requires regular spacing and
                            at least 16 input tracs--will switch to differences
                            automatically if have less than 16 input traces)
float dt		sample rate in sec
int iopt		0 = interpolate: output 1+(nx-1)*(1+ninterp) traces
                           with ninterp traces between each pair of input traces
			1 = compute low-pass model: output nx traces
                            on original trace locations -- This is typically
                            used for Quality Control if the interpolator
                            is failing for any reason
			2 = compute dip picks in units of samples/trace: 
                            output nx traces on original trace locations
in_traces		2-D array of input data

Output:
out_traces		2-D array of interpolated traces


Notes:
This routine outputs 'ninterp' interpolated traces between each pair of 
input traces.  The values for lagc, freq1, and freq2 are only used for
event tracking. The output data will be full bandwidth with no agc.  The 
suggested default parameters typically will do a satisfactory job of 
interpolation for dips up to about 12 ms/trace.  Using a larger value for 
freq2 causes the algorithm to do a better job on the shallow dips, but to 
fail on the steep dips.  Only one dip is assumed at each time sample between 
each pair of input traces.  The original input traces are passed through
this routine without modification.

The key assumption used here is that the low frequency data are unaliased
and can be used for event tracking.  Those dip picks are used to interpolate
the original full-bandwidth data, giving some measure of interpolation
at higher frequencies which otherwise would be aliased.  Using iopt equal
to 1 allows you to visually check whether the low-pass picking model is aliased.
If you can't visually pick dips correctly on the low-pass picking 
model, this computer routine will fail.

The place this code is most likely to fail is on the first breaks.
*******************************************************************************
Credits:
Adapted by Gabriel Alvarez (1995) from suradon.c written by John 
	Anderson (1993)
******************************************************************************/
{
	int	ntfft,ntfftny,nxfft,nxfftny,j,k,ix,it,ixm;
	float	df,dff,wa,wb,dxx,eps=FLT_EPSILON,f,fcl,fch;
	float 	*rt,*rrt,*a,*b,*p,*time,*aa,*bb,*save;
	complex	*crt,*ccrt;
	float    **pa,**pb;

	/* defensive programming */
	if(nx<2 || (iopt==0 && ninterp==0) ) {
		for(ix=0;ix<nx;ix++) 
			memcpy((void *) out_traces[ix], (const void *) in_traces[ix],nt*FSIZE);
	}

	/* define useful variables */
	lent=1+2*(lent/2);
	lenx=1+2*(lenx/2);
	lagc=1 + lagc*0.001/dt;

	ntfft=npfar(nt);
	ntfftny=1+ntfft/2;
	nxfft=npfar(nx);
	nxfftny=1+nxfft/2;

	df=1./(ntfft*dt);

	/* allocate working space */
	crt = ealloc1complex( MAX(ntfftny,nxfftny));
	rt = (float *)crt;
	
	ccrt = ealloc1complex( ntfftny);
	rrt = (float *)ccrt;
	a =  ealloc1float( MAX(nx,nt));
	b =  ealloc1float( MAX(nx,nt));
	p =  ealloc1float(nt);
	time = ealloc1float( nt);
	aa =  ealloc1float( MAX(nx,nt));
	bb =  ealloc1float( MAX(nx,nt));

	pa  = ealloc2float(nt,nx);
	pb  = ealloc2float(nt,nx);

	/* loop computing filtered data for picking purposes in pa */
	/* compute time derivative of filtered data in pb */
	dff=2.*PI/ntfft;
	for(ix=0;ix<nx;ix++) {
		for(it=0;it<nt;it++) {
			rt[it]=in_traces[ix][it];
			a[it]=fabs(rt[it]);
		}
		runav(nt,lagc,a,b);
		runav(nt,lagc,b,a);
		for(it=0;it<nt;it++) rt[it]=rt[it]/(a[it]+eps);	
		for(it=nt;it<ntfft;it++) rt[it]=0.;
		pfarc(1,ntfft,rt,crt);
		for(it=0;it<ntfftny;it++){
			f=it*df;
			fcl=(f/freq1);
			fcl=fcl*fcl*fcl*fcl;
			fch=(f/freq2);
			fch=fch*fch*fch*fch;
			f=fcl/( (1.+fcl)*(1.+fch) );
			crt[it]=crmul(crt[it],f);
			ccrt[it]=cmul(crt[it],cmplx(0.,-it*dff));
		}
		pfacr(-1,ntfft,crt,rt); 
		memcpy((void *) pa[ix],(const void *) rt,nt*FSIZE);
		pfacr(-1,ntfft,ccrt,rrt); 
		memcpy((void *) pa[ix],(const void *) rrt,nt*FSIZE);
	} 

	if(iopt==1){
		for(ix=0;ix<nx;ix++){
		memcpy((void *) rt,(const void *) pa[ix],nt*FSIZE);
			for (it=0;it<nt;it++)
				out_traces[ix][it]=rt[it];
		}
		free2float(pa);
		free2float(pb);
		free1complex(crt);
		free1complex(ccrt);
		free1float(a);
		free1float(b);
		free1float(p);
		free1float(time);
		free1float(aa);
		free1float(bb);
		return;
	}

	/* loop computing spatial derivative of data for picking purposes*/
	nxfft=npfar(nx);
	nxfftny=1+nxfft/2;
	dxx=2.*PI/(nxfft*nxfft);
	if(nx<16) xopt=1;
	for(it=0;it<nt;it++) {
		for(j=0;j<nx;j++) rt[j]=pa[j][it];
		if(xopt) {
			for(j=0;j<nx-1;j++) rt[j]=rt[j+1]-rt[j];
			rt[nx-1]=rt[nx-2];
		}else{
			for(j=nx;j<nxfft;j++) rt[j]=0.;
			pfarc(1,nxfft,rt,crt);
			for(j=0;j<nxfftny;j++){
				crt[j]=cmul(crt[j],cmplx(0.,-j*dxx));
			}
			pfacr(-1,nxfft,crt,rt); 
		}
		for(j=0;j<nx;j++) pa[j][it]=rt[j];
	} 

	/* compute dot products and smooth over time */
	for(ix=0;ix<nx;ix++) {
		memcpy((void *) a,(const void *) pa[ix],nt*FSIZE);
		memcpy((void *) b,(const void *) pb[ix],nt*FSIZE);
		for(it=0;it<nt;it++) {
			aa[it]=a[it]*b[it];
			bb[it]=b[it]*b[it];
		}
		runav(nt,lent,aa,a);
		runav(nt,lent,a,aa);
		runav(nt,lent,bb,b);
		runav(nt,lent,b,bb);
		memcpy((void *) pa[ix],(const void *) a,nt*FSIZE);
		memcpy((void *) pb[ix],(const void *) b,nt*FSIZE);
	}

	/* smooth dot products in x */
	if(lenx>1){
	    for(it=0;it<nt;it++) {
		for(j=0;j<nx;j++) a[j]=pa[j][it];
		for(j=0;j<nx;j++) b[j]=pb[j][it];
		runav(nx,lenx,a,aa);
		runav(nx,lenx,aa,a);
		runav(nx,lenx,b,bb);
		runav(nx,lenx,bb,b);
		for(j=0;j<nx;j++) pa[j][it]=a[j];
		for(j=0;j<nx;j++) pb[j][it]=b[j];
	    }
	}

	/* loop computing p, interpolating, and outputting results */
	/* get first trace from input data */
	for (it=0; it<nt; it++)
		a[it]=in_traces[0][it];
	for(ix=1;ix<nx;ix++) {
		ixm=ix-1;
		memcpy((void *) aa,(const void *) pa[ix],nt*FSIZE);
		memcpy((void *) bb,(const void *) pb[ix],nt*FSIZE);
		for(it=0;it<nt;it++) {
			p[it] = - aa[it]/( bb[it] + eps );
		}
		
		/* get input traces one by one */
		for (it=0; it<nt; it++)
			b[it]=in_traces[ix][it];	
		if(iopt==2) {
	
			/* write to output array */
			for (it=0; it<nt; it++)
				out_traces[ixm][it]=p[it];
			/* don't output dip picks except on original traces */
		}else{
			/* write to output array */
			for (it=0; it<nt; it++)
				out_traces[ixm*(ninterp+1)][it]=a[it];
			for(k=0;k<ninterp;k++){
				wa=(1.+k)/(1+ninterp);
				wb=1.-wa;
				for(it=0;it<nt;it++) time[it] = it - p[it]*wa;		
				ints8r(nt,1.0,0.,a,0.0,0.0,nt,time,aa);
				for(it=0;it<nt;it++) time[it] = it + p[it]*wb;		
				ints8r(nt,1.0,0.,b,0.0,0.0,nt,time,bb);
				for(it=0;it<nt;it++) aa[it]=wb*aa[it]+wa*bb[it];

				/* write to output array */
				for (it=0; it<nt; it++)
					out_traces[k+1+ixm*(ninterp+1)][it]=
						aa[it];
			}
		}
		save=a;
		a=b;
		b=save;  
	} 
	if(iopt==2) {
		
		/* write to output array */
		for (it=0; it<nt; it++)
			out_traces[nx-1][it]=p[it];
	}else{
		/* write to output array */
		for (it=0; it<nt; it++)
			out_traces[(nx-1)*(ninterp+1)][it]=a[it];
	}


	/* close files, free temporary memory */
	free2float(pa);
	free2float(pb);
	free1complex(crt);
	free1complex(ccrt);
	free1float(a);
	free1float(b);
	free1float(p);
	free1float(time);
	free1float(aa);
	free1float(bb);

	return;
}

void runav(int n,int len,float *a,float *b)
/******************************************************************************
compute a boxcar running average filter
*******************************************************************************
int n           number of samples in a[] and b[]
int len         length of running average in samples
float a[n]      input array
float b[n]      output array
*******************************************************************************
Author: John Anderson (visitor to CSM from Mobil) Spring 1993
******************************************************************************/
{
        float sum=0.;
        int j,lenh=len/2;
        if(len<=1) {
                for(j=0;j<n;j++) b[j]=a[j];
                return;
        }
        for(j=0;j<MIN(len,n);j++) sum+=a[j];
        for(j=0;j<MIN(lenh,n);j++) b[j]=sum;
        for(j=lenh;j<n-lenh;j++) {
                sum=sum+a[j+lenh]-a[j-lenh];
                b[j]=sum;
        }
        for(j=MAX(0,n-lenh);j<n;j++) b[j]=sum;
        for(j=0;j<n;j++) b[j]/=len;
        return;
}
