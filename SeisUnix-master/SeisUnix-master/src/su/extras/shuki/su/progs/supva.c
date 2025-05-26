#include <stdio.h>
#include <math.h>
#include "../include/su.h"

#ifdef HP
#define _SETGAIN_       setgain
#define _TRPENDS_       trpends
#define _CONNMO_        connmo
#define _LINTRP_        lintrp
#define _STRTCH_        strtch
#define _SEMBLAN_       semblan
#else
#define _SETGAIN_       setgain_
#define _TRPENDS_       trpends_
#define _CONNMO_        connmo_
#define _LINTRP_        lintrp_
#define _STRTCH_        strtch_
#define _SEMBLAN_       semblan_
#endif
#define BLOCK 3000
#define BLOCK1 750000


char	*sdoc;
int	xargc;
char	**xargv;
bool verbose;
char *SccsId[]="@(#)supva.c	1.4 11/15/88\n";

int	nalloc, dalloc, nalloc1, dalloc1;
float	 *x;
static float stretch;  /* hard mute limit                      */
/*********************** self documentation **********************/
static char *lsdoc =
"SUPVA - VELOCITY ANALYS\n\
 supva [-v] <stdin > stdout [velfs=1400 vells=5300 cdflist=all \n\            cdfpa=1 stretch=1.5 gate=32 nvf=40 cdfr= incdf= swfs= swls= ]\n\
 PARAMETERS:\n\
           cdflist - list of input cdfs\n\
                   by default cdflist=all\n\
\n\
           cdfpa - number of cdf per analysis\n\
                   by default cdfpa=1\n\
\n\
           velfs - minimum trial velocity\n\
                   by default velfs= 1400\n\
\n\
           vells - maximum trial velocity\n\
                   by default vells= 5300\n\
\n\
           nvf   - number of trial velocities\n\
                   by default   nvf=   40\n\
\n\
           stretch - stretch limit factor\n\
                   by default stretch=1.5\n\
\n\
          gate     - lentgth of the time gate (in msec)\n\
                   by default gate=    32\n\
\n\
  optionaly instead of velosities the input can be slownesses  \n\
          swfs    - minimum trial slowness\n\
          swls    - maximum trial slowness\n\
\n\
  optionaly instead of cdflist the input can be\n\
          cdfr    - first cdf\n\
          incdf   - cdf increment\n\
\n\
 OPTIONS:\n\
          -v  turn verbose on by default off\n\
";
/*****************************************************************/


main(ac, av)
int	ac; 
char	**av;
{
	Sutrace tr;
	static float	os;                              /*... and reciprocal*/
	static int	nw;                              /* number interpolation weights*/
	static int	t0, te;                          /* smallest largest non zero weights */
	static float	*gain;                           /* soft and hard mute */
	static float	*tnmo;                           /* times after nmo */
	static int	*itnmo;
	static float	*w;                              /* interpolation weights*/
	static float	*nmoed;                          /* NMO stretched data*/
        static float    *data,*data1,*data2;             /* input data */
	static int	nt;                              /*number of samples */
	static float	*semb;                           /* semblance*/
	static float	dt, *stack1, *stack2;
	static float	cdfr;                            /* first cdf for analys */
	static float	incva;                           /* velosity  increment */
	static int	 nva;                            /* number of analyses to process */
	static float	 velfs;                          /* minimum velocity */
	static float	 vells;                          /* maximum velocity*/
	static float	 swfs;                          /* minimum velocity */
	static float	 swls;                          /* maximum velocity*/
	static float	cdfpa;                           /* number of cdf's per analysis*/
	static float	vel;                             /* vel for nmo*/
	static float    gate;                            /* gate for semblance */
	static float    nvf;                             /* number of velocity functions*/
	static float    incdf;                           /* cdf increment */
	static float    cdf; 
	static float	vel2;                            /* sloth model -- sloth=sgn(v)/(v*v) */
	static float	*cdflist, *cdfran;
	static float	offset;
        static  float *xx;
         int _VARNMO_(),_CONNMO_(),_LINTRP_(),_SNCTRP_();


	struct sem { 
		float	*n;
		float	*dn	} *stack;
	Subhed bh;
	int	infd, outfd;
	int	c, iv, ic, nx, ix, it, il, i, im,in,id;
        int    nxs,icount,icountc,a,b,oldnxs,all,flag;
	Section coff;

	xargc = ac; 
	xargv = av;
        inits();
	infd = input();
	outfd = output();

	/* GET PARAMETERS */
	cdfpa=1;
	fgetpar("cdfpa", &cdfpa); 
        velfs=1400;
        vells=5300;
        nvf=40;
        incva=(vells-velfs)/(nvf-1);
	if (fgetpar("velfs", &velfs)) {/* first velocity*/
	if (!fgetpar("vells", &vells)){
        warn(__FILE__,__LINE__," Must be specify  vells");
        selfdoc();
           }
        
	fgetpar("nvf", &nvf);
	incva = (vells - velfs) / (nvf - 1);

        } else { /* for increment in slowness */
        if (fgetpar("swfs",swfs)){
        velfs=swfs;
        if (!fgetpar("swls",swls)){
        warn(__FILE__,__LINE__," Must be specify swls ");
        selfdoc();
           }
         vells=swls;
	fgetpar("nvf", &nvf);
	incva = (swfs - swls) / (nvf - 1);
         } /* end if */
	} /* end else*/
	stretch = 1.5; 
	fgetpar("stretch", &stretch);
	gate = 32;   
	fgetpar("gate", &gate);
        all=1;

        nva=maxgetpar();
        if(verbose)fprintf(stderr,"nva=%d\n",nva);
	cdflist = ( float *) malloc((unsigned) ( nva * sizeof(float)));

	if (fgetpar("cdflist", cdflist)) {/* list of cdf */
        all=0;

		cdfran = ( float *) malloc((unsigned)  (nva * cdfpa * sizeof(float)));
		cdfpr(cdfpa, cdflist, nva, cdfran);

	} else {/* for increment in cdf */
		if (fgetpar("cdfr", &cdfr)) {
                nva=1000;
		if (!fgetpar("incdf", &incdf)) {
			warn(__FILE__, __LINE__, "Must specify incdf");
			selfdoc();
		}
		cdfran = (float *)malloc((unsigned) ( nva * cdfpa * sizeof(float)));
                all=0;
		for (ic = 0, cdf = cdfr; ic < nva * cdfpa; cdf + = incdf, ic++){
			cdfran[ic] = cdf; 
		}
           } /* end if */
	} /* end else*/
        if( all==1) { nva=1000;
                
		cdfran = ( float *) malloc((unsigned)  (nva * cdfpa * sizeof(float)));
                           }/* in case of all cdfs */
      
	/* PASS ASCII HEADER */
	apass(infd, outfd);

	/* READ BINARY HEADER */
	getbh(infd, &bh);
	nt = bh.ns;
	tr.data = (float * )malloc(bh.ns * bh.esize);
	dt = bh.dt / 1000000.;
	fprintf (stderr, "dt=%f\n", dt);

	/* ADD HISTORY LOG */
	hislog(outfd);

	/* WRITE BINARY HEADER */

        gate= gate/(dt*1000);
	bh.ns = (nt / gate)*2-1;
	putbh(outfd, &bh);
   /*printout of input parameters */
        fprintf(stderr,"cdfpa  =%f\n",cdfpa);
        fprintf(stderr,"velfs  =%f\n",velfs);
        fprintf(stderr,"vells  =%f\n",vells);
        fprintf(stderr,"nvf    =%f\n",nvf);
        fprintf(stderr,"stretch=%f\n",stretch);
        fprintf (stderr, "gate=%f in samples\n", gate);

	/* Stretch reciprocal for muting */
	os = 1.0 / stretch;
	nw = 2;


	coff.n1 = nt;
	/* allocation for tnmo operations*/
	tnmo = (float *) malloc((unsigned) (nt * sizeof(float)));
	data1 = (float *) malloc((unsigned)(nt * sizeof(float)));
	gain  = (float *) malloc((unsigned) (nt * sizeof (float)));
	itnmo = (int *)   malloc((unsigned) (nt * sizeof (int)));
	w     = (float *) malloc((unsigned) (nt * nw * sizeof (float)));
	nmoed = (float *) malloc((unsigned) (nt * sizeof (float)));
        stack1 = (float *) malloc((nt * sizeof(float)));
        stack2 = (float *) malloc((nt * sizeof(float)));
				i =( nt / gate)*2-1;
					semb = (float *) malloc((i * sizeof(float)));
         nalloc1 = BLOCK1;
                dalloc1 = nalloc1;
                data2= (float*) malloc(nalloc1);
         nalloc = BLOCK;
                dalloc = nalloc;
                xx= (float*) malloc(nalloc);
 



	if (cdfpa > 1) { /* case for more then one cdf for every analys */

		for (ic = 0, id=0; ic < nva;ic++, id += cdfpa) {
                      nxs=0;
                 icount=0;
                 icountc=0;
                 c=0;
                a=0;
                b=0;

			for (il = 0; il < cdfpa; il++) {
				cdf = cdfran[il +id];
				if(verbose)fprintf(stderr, "cdf=%f\n", cdf);
			flag=getcdf(infd, &coff, cdf,all);
                        if(!flag) exit(0);
				data = coff.data;
				nx = coff.n2;
                                nxs + = nx;

				for (ix = 0; ix < nx; ix++) {
					i = nt * ix;
                                          xx[b+ix] = x[ix];
                                           /* MEMORY REALLOCATION */
                if( (nxs) >= nalloc) {
                        nalloc += dalloc;
                        xx = (float*) realloc(xx,nalloc);
                        if(x==NULL)
                                err(__FILE__,__LINE__,"Can't realloc %d bytes",nalloc);
                }
                if( (nxs*nt*sizeof(float)) >= nalloc) {
                        nalloc1 += dalloc1;
                        data2 = (float*) realloc(data2,nalloc1);
                        if(data2==NULL)
                                err(__FILE__,__LINE__,"Can't realloc %d bytes",nalloc);
                }
          icount++;


					for (it = 0; it < nt; it++) {
                    a=c+i;
                    in=a+it;

						data2[a+it] = data[i+it];
                      icountc++;
					}/* end it */
				}/*end ix*/
                         b=icount;
                               c=icountc;

			} /* end il */

			/* allocation for semblance operations for every cdf*/
	if(!ic)		stack = (struct sem *)  malloc ((unsigned)(nxs * sizeof(struct sem )));
        if(nxs>oldnxs ) stack = (struct sem *) realloc (stack,(unsigned)(nxs*sizeof(struct sem )));
                  oldnxs=nxs;

			for (iv = 0, vel = velfs ; iv < nvf ; vel += incva, iv++) {
	if(verbose)			fprintf(stderr, "nxs=%d,vel=%f\n",  nxs, vel);
				vel2 = 1.0 / (fabs(vel) * vel * dt * dt);
                                if (swfs) vel2= vel*vel/ dt*dt ;
				for (ix = 0 ; ix < nxs; ix++) {
					offset = xx[ix];
/*                                           fprintf(stderr, "iv=%d,nxs=%d,vel=%f offset=%f\n", iv, nxs, vel,offset); */

					il = nt * ix;
					for (it = 0; it < nt; it++) { 
						data1[it] = data2[il+it];
					}
/* 	 if(ic==1)         for (it=0;it<nt;it++) { fprintf(stderr,"data1=%f ix=%d it=%d\n",data1[it],ix,it);} */
					if (!iv) {
		if(!ic)				stack[ix].n = (float *) malloc ((unsigned)(nt * sizeof(float)));
	        if(!ic)					stack[ix].dn = (float *) malloc ((unsigned)(nt * sizeof(float)));
					}

					_CONNMO_(tnmo, &nt, &vel2, &offset);
					_SETGAIN_(tnmo, gain, &nt);
					hardmute(gain, nt, os);

					/* Calculate the interpolation coefficients */
					_LINTRP_(itnmo, w, tnmo, &nt, &nw, gain);
					_TRPENDS_(itnmo, w, &t0, &te, &nt, &nw);

					/* Perform the NMO; put new data into nmoed */
					_STRTCH_(nmoed, data1, &nt, &t0, &te, itnmo, w, &nw);

					/* allocation for every offset */
					for (it = 0; it < nt; it++) {
						stack[ix].n[it] = 0;
						stack[ix].dn[it] = 0.;
					}
					for (it = 0; it < nt; it++) {
						stack[ix].n[it]  = nmoed [it];

						stack[ix].dn[it]  = nmoed [it] * nmoed[it];
					}

				}  /* end loop over offsets */
				for (it = 0; it < nt; it++) {
					stack1[it] = 0;
					stack2[it] = 0.;
				}
				for (it = 0; it < nt ; it++) {
					for (im = 0; im < nxs ; im++) {
						stack1[it]  = stack1[it] + stack[im].n[it];
						stack2[it]  = stack2[it] + stack[im].dn[it];

					} /* end over im */
				} /* end loop over time */
				i =( nt / gate)*2-1;
				_SEMBLAN_(stack1, stack2, &gate, &nt, semb, &i, &nxs);

				/*                 }                 */
				bcopy(semb, data, i * (sizeof(float)));
				/*         for(i=0;i< nt/gate ; i++){ */
                                if(all==1)cdf=coff.o2;
				putcdf(outfd, data, cdf, vel, i);
			}/*end loop over velocities */

		} /* end loop over cdf */


	} else {/* if cdfpa=1 */
		for (ic = 0 ; ic < nva * cdfpa ; ic++) {
			cdf = cdfran[ic];
			flag=getcdf(infd, &coff, cdf,all);
                        if(!flag) exit(0);
			data = coff.data;

			if (verbose) 
				fprintf(stderr, "Read  nt=%d \n",
				    nt);


			nx = coff.n2;
			/* allocation for semblance operations for every cdf*/
   if(!ic)		stack = (struct sem *)  malloc ((unsigned)(nx * sizeof(struct sem )));
   if(nx>oldnxs)  	stack = (struct sem *)  realloc (stack,(unsigned)(nx * sizeof(struct sem )));
                        oldnxs=nx;

			for (iv = 0, vel = velfs ; iv < nvf ; vel += incva, iv++) {
	if(verbose)			fprintf(stderr, "nx=%d,vel=%f\n",  nx, vel);
				vel2 = 1 / (vel * vel);
				vel2 = 1.0 / (fabs(vel) * vel * dt * dt);
				for (ix = 0 ; ix < nx; ix++) {
					offset = x[ix];
					il = nt * ix;
					for (it = 0; it < nt; it++) { 
						data1[it] = data[il+it];
					}
					_CONNMO_(tnmo, &nt, &vel2, &offset);
					_SETGAIN_(tnmo, gain, &nt);
					hardmute(gain, nt, os);

					/* Calculate the interpolation coefficients */
					_LINTRP_(itnmo, w, tnmo, &nt, &nw, gain);
					_TRPENDS_(itnmo, w, &t0, &te, &nt, &nw);

					/* Perform the NMO; put new data into nmoed */
					_STRTCH_(nmoed, data1, &nt, &t0, &te, itnmo, w, &nw);

					/* allocation for every offset */
					if (!iv) {
			if(!ic)			stack[ix].n = (float *) malloc ((unsigned)(nt * sizeof(float)));
		        if(!ic)			stack[ix].dn = (float *) malloc ((unsigned)(nt * sizeof(float)));
					}
					for (it = 0; it < nt; it++) {
						stack[ix].n[it] = 0;
						stack[ix].dn[it] = 0.;
					}
					for (it = 0; it < nt; it++) {
						stack[ix].n[it]  = nmoed [it];

						stack[ix].dn[it]  = nmoed [it] * nmoed[it];
					}

				}  /* end loop over offsets */
				for (it = 0; it < nt; it++) {
					stack1[it] = 0;
					stack2[it] = 0.;
				}
				for (it = 0; it < nt ; it++) {
					for (im = 0; im < nx ; im++) {
						stack1[it]  = stack1[it] + stack[im].n[it];
						stack2[it]  = stack2[it] + stack[im].dn[it];
					} /* end over im */
				} /* end loop over time */
				i = (nt / gate)*2-1;
				_SEMBLAN_(stack1, stack2, &gate, &nt, semb, &i, &nx);

				bcopy(semb, data, i * (sizeof(float)));
                                if(all==1) cdf=coff.o2;
				putcdf(outfd, data, cdf, vel, i);
			}/*end loop over velocities */

		} /* end loop over cdf */
	} /* end else */

} /* end main */


cdfpr(cdfpa, cdflist, nva, cdfran)
float	cdfpa;
int     nva;
float	*cdflist, *cdfran;
{
	int	i, j, ij;
	ij = 0;
	fprintf (stderr, "cdflist=%f\n", cdflist[0]);
	for (i = 0; i < nva  ; i++) {
		for (j = 0; j < cdfpa  ; j++) {
			cdfran[ij] = cdflist[i] + j;
			fprintf(stderr, "cdfran=%f,ij=%d\n", cdfran[ij], ij);
			ij++;
		}
	}
}


/****************************************************************/

hardmute(g, n, os)
int	n;
float	*g, os;
{
	int	i;
	for (i = 0; i < n; i++) {
		if (g[i] < os) 
			g[i] = 0.0;
	}
}



/****************************************************************/
/* INITIALIZE SELF DOCUMENTATION */
inits()
{
        int c;
 
        sdoc = lsdoc;
 
 
        /* GET OPTIONS */
        while( (c=getopt(xargc,xargv,"v"))!=EOF) {
                switch(c) {
                case 'v':
                        verbose = true;
               break;
                case '?':
                        warn(__FILE__,__LINE__,"getopt returned '?'");
                }
        }
 
}

