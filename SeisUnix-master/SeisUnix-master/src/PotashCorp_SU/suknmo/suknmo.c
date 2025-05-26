/* suknmo.c */
/* B.Nemeth */


#include "suhdr.h"

#define I               cmplx(0.0, 1.0)
#define PIBY2           0.5 * PI
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */

/*********************** self documentation *****************************/
char *sdoc[] = {"SUKNMO - Perform a Kirchoff nmo and stacking             ",
"                                                                        ",
"   This needs some cleanup -                                            ",
NULL};
   
/* Segy data constans */
segy intrace,outtrace;				/* SEGY trace */

void knmo(float *ind, float *outd,int nt, float *vrms,
	 float t0,float offset,float dt);

int main( int argc, char *argv[] )
{
	cwp_String key;	/* header key word from segy.h		*/
	cwp_String type;/* ... its type				*/
	int indx;	/* ... its index			*/
	int nt;		/* number of data points on trace	*/
	int nsegy;	/* number of bytes in the segy		*/
	Value val;	/* value of key in current gather	*/
	Value valnew;	/* value of key in trace being treated	*/
	int fold;	/* number of traces stacked		*/
	int *nnz;	/* number of non-zero values stacked	*/
	float normpow;	/* divide by nnz[i]^normpow		*/
	int newtracl;	/* tracl for stacked traces		*/
	int verbose;	/* verbose flag				*/
	float *knmodata;/* Kirchoff nmo corrected trace         */ 
	float *vrms;	/* array of rms velocities */
	float t0;	/* time of first sample */
	
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
        float dt;               /* sample spacing (secs) on input trace */
        float omega;            /* circular frequency                   */
        float domega;           /* circular frequency spacing (from dt) */
        float sign;             /* sign in front of i*omega default -1  */
        int nfft;               /* number of points in nfft             */
        int nf;                 /* number of frequencies (incl Nyq)     */
        float onfft;            /* 1 / nfft                             */
        size_t nzeros;          /* number of padded zeroes in bytes     */
        size_t ntsize;          /* nt in bytes                          */


	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);


	/* Set parameters */
	if (!getparint   ("verbose", &verbose))	 verbose = 0;
	if (!getparfloat ("normpow", &normpow))	 normpow = 1.0;
	if (!getparstring("key", &key))		 key = "cdp";

	type = hdtype(key);
	indx = getindex(key);

	/* Set up for first trace (must compare new key field each time) */
	nsegy = gettr(&intrace);
	nt = intrace.ns;
        ntsize = nt * FSIZE;
	dt = intrace.dt/1000000.0;
	t0 = intrace.delrt;
	
	memcpy( (void *) &outtrace, (const void *) &intrace, nsegy);
	nnz = ealloc1int(nt);
	{ register int i;
	  for (i = 0; i < nt; i++){
		if (intrace.data[i] != 0.0)  nnz[i]=1;
		else nnz[i] = 0;
	  }
        }
	fold = 1;

	knmodata = ealloc1float(nt);
	vrms = ealloc1float(nt);
	{ register int i;
		for(i=0;i<nt;i++) vrms[i]=2000;
	}


	/* set up the phase filter */
	power = 1.0;sign = 1.0;phasefac = 0.5;
	phase = phasefac * PI;
         
	/* Set up for fft */
        nfft = npfaro(nt, LOOKFAC * nt);
        if (nfft >= SU_NFLTS || nfft >= PFA_MAX)
                err("Padded nt=%d -- too big", nfft);

        nf = nfft/2 + 1;
        onfft = 1.0 / nfft;
        nzeros = (nfft - nt) * FSIZE;
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


	/* Loop over traces */
	newtracl = 1;
	gethval(&intrace, indx, &val);
	while (nsegy) {		     /* While previous trace non-empty */
		nsegy = gettr(&intrace);
		gethval(&intrace, indx, &valnew);
		if (valcmp(type, val, valnew) || !nsegy) {	
			/* Either val and valnew differ, indicating a  */
			/* new gather or nsegy is zero, indicating the */
		        /* end of the traces.                          */
			if (verbose) {
				fprintf(stderr, "val=");
				fprintfval(stderr, type, val);
				fprintf(stderr, "\tfold=%d\n", fold);
			}

			/* Add header info and output stack */
			outtrace.nhs = fold;
			outtrace.tracl = newtracl++;
			outtrace.offset = 0;
			if (normpow && fold != 1) {
			        register int i;
				for (i = 0; i < nt; ++i) {
				    float nnzi = nnz[i];
				    if (nnzi)
					outtrace.data[i] /= pow(nnzi, normpow);
				}
			}
			
			/*Do the phase filtering before the trace is released*/
                	/* Load trace into rt (zero-padded) */
               		 memcpy( (void *) rt, (const void *) outtrace.data, ntsize);
               		 memset((void *) (rt + nt), (int) '\0', nzeros);

                	/* FFT */
                	pfarc(1, nfft, rt, ct);


                	/* Apply filter */
                	{ register int i;
                	for (i = 0; i < nf; ++i)  ct[i] = cmul(ct[i], filt[i]);
                	}

                	/* Invert */
                	pfacr(-1, nfft, ct, rt);
               		memcpy( (void *) outtrace.data, (const void *) rt, ntsize);
			
			/* dump the trace to stdout */
			puttr(&outtrace);

			/* Set up for next gather */
			memcpy( (void *) &outtrace,
					(const void *) &intrace, nsegy);
			{ register int i;
	  		  for (i = 0; i < nt; i++){
				if (intrace.data[i] != 0.0)  nnz[i]=1;
				else nnz[i] = 0;
			  }
			}
			fold = 1;
			val = valnew;

		} else { /* still in same gather */
			/* before stacking do the KNMO on the trace */
			knmo(intrace.data,knmodata,nt,vrms,t0,intrace.offset,dt);
			/* stacking happens here */
			{	register int i;
				for (i = 0; i < nt; ++i) {
					float dat = knmodata[i];
					if (!(dat == 0.0))  ++nnz[i];
					outtrace.data[i] += dat;
				}
			}
			++fold;
		}
	}
	
	free1float(knmodata);

   return EXIT_SUCCESS;
}

void knmo(float *ind, float *outd,int nt, float *vrms, float t0,float offset,float dt)
{
/* This routine applies the moveout corection, obliquity factor and 
	spherical divergence factor to the gather. The phase shift filter
	is applied to the stacked trace */
	int i;
	float tx2;
	float tx2a;
	float of2;
	float r;
	float z;
	float t;
	float tmax;
	
	of2=4*SQR(offset);
	tmax=t0+(nt-1*dt);
	memset( (void *) outd, (int) '\0',nt*FSIZE);
	for(i=0;i<nt;i++) {
		t=t0+i*dt;
		/* knmo moveout */
		tx2 = SQR(t) + of2/SQR(vrms[i]);
		if(tx2>tmax) return;
		ints8r(nt,dt,t0,outd,0,0,1,&tx2,&tx2a);
		
		z=vrms[i]*(t);
		r=sqrt(SQR(offset)+SQR(z));
		/* obliquit factor */
		tx2a *=z/r;
		/* 3D spherical divergence */
		tx2a /=vrms[i]*r;
		
		outd[i]=tx2a;
	}
	return;
}
