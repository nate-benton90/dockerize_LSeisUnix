h07359
s 00034/00034/00461
d D 1.4 88/11/15 14:03:26 shuki 9 8
c 
e
s 00008/00011/00487
d D 1.3 88/06/23 07:49:11 shemer 8 6
c 
e
s 00006/00010/00488
d R 1.3 88/06/22 12:09:40 shemer 7 6
c 
e
s 00074/00067/00424
d D 1.2 88/06/20 14:22:16 shemer 6 1
c 
e
s 00073/00066/00425
d R 1.2 88/06/19 11:55:44 shemer 5 1
c 
e
s 00073/00066/00425
d R 1.2 88/06/19 11:43:12 shemer 4 1
c 
e
s 00073/00066/00425
d R 1.2 88/06/19 11:25:48 shemer 3 1
c 
e
s 00069/00064/00427
d R 1.2 88/06/19 10:47:47 shemer 2 1
c 
e
s 00491/00000/00000
d D 1.1 88/06/15 14:24:02 shemer 1 0
c date and time created 88/06/15 14:24:02 by shemer
e
u
U
t
T
I 1
#include <stdio.h>
#include <math.h>
D 9
#include "/src/su/include/su.h"
E 9
I 9
#include "../include/su.h"
E 9

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
D 6
char	*SccsId[] = "@(#)supva.c     1.3\t5/25/88\n";
E 6
I 6
D 9
char *SccsId[]="@(#)supva.c	1.2\t6/19/88\n";
E 9
I 9
char *SccsId[]="%W% %G%\n";
E 9

E 6
int	nalloc, dalloc, nalloc1, dalloc1;
float	 *x;
static float stretch;  /* hard mute limit                      */
/*********************** self documentation **********************/
static char *lsdoc =
D 6
"SUPVA - VELOCITY analys                                       \n\
 supva [-v] <stdin > stdout nva= velfs= vells= cdflist= cdfpa= [stretch=1.5 gate=32 nvf=50 cdfr= incdf= swfs= swls=   ]                      \n\
E 6
I 6
D 9
"SUPVA - VELOCITY ANALYS                                       \n\
E 9
I 9
"SUPVA - VELOCITY ANALYS\n\
E 9
 supva [-v] <stdin > stdout [velfs=1400 vells=5300 cdflist=all \n\            cdfpa=1 stretch=1.5 gate=32 nvf=40 cdfr= incdf= swfs= swls= ]\n\
E 6
D 9
 PARAMETERS:                                                   \n\
D 6
        nva-number of analyses to process                      \n\
E 6
I 6
           cdflist - list of input cdfs                        \n\
                   by default cdflist=all                      \n\
E 6
                                                               \n\
           cdfpa - number of cdf per analysis                  \n\
I 6
                   by default cdfpa=1                          \n\
E 6
                                                               \n\
           velfs - minimum trial velocity                      \n\
I 6
                   by default velfs= 1400                      \n\
E 6
                                                               \n\
           vells - maximum trial velocity                      \n\
I 6
                   by default vells= 5300                      \n\
E 6
                                                               \n\
D 6
           nvf   - number of trial velocities by default=50    \n\
E 6
I 6
           nvf   - number of trial velocities                  \n\
                   by default   nvf=   40                      \n\
E 6
                                                               \n\
D 6
           stretch - stretch limit factor by default=1.5       \n\
E 6
I 6
           stretch - stretch limit factor                      \n\
                   by default stretch=1.5                      \n\
E 6
                                                               \n\
D 6
          gate     - lentgth of the time gate (in msec)     \n\
                     by default gate=32                         \n\
E 6
I 6
          gate     - lentgth of the time gate (in msec)        \n\
                   by default gate=    32                      \n\
                                                               \n\
E 9
I 9
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
E 9
E 6
  optionaly instead of velosities the input can be slownesses  \n\
D 9
          swfs    - minimum trial slowness                     \n\
          swls    - maximum trial slowness                     \n\
I 6
                                                               \n\
E 6
  optionaly instead of cdflist the input can be                \n\
          cdfr    - first cdf                                  \n\
          incdf   - cdf increment                              \n\
I 6
                                                               \n\
E 6
 OPTIONS:                                                      \n\
D 6
    -v  turn verbose on by default off                         \n\
E 6
I 6
          -v  turn verbose on by default off                   \n\
E 9
I 9
          swfs    - minimum trial slowness\n\
          swls    - maximum trial slowness\n\
\n\
  optionaly instead of cdflist the input can be\n\
          cdfr    - first cdf\n\
          incdf   - cdf increment\n\
\n\
 OPTIONS:\n\
          -v  turn verbose on by default off\n\
E 9
E 6
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
D 6
	static float	 nva;                            /* number of analyses to process */
E 6
I 6
	static int	 nva;                            /* number of analyses to process */
E 6
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
D 6
	int	c, iv, ic, nx, ix, it, il, i, im,in,id,icdf,imm;
        int    nxs,icount,icountc,a,b,icmp,oldnxs;
E 6
I 6
	int	c, iv, ic, nx, ix, it, il, i, im,in,id;
        int    nxs,icount,icountc,a,b,oldnxs,all,flag;
E 6
	Section coff;

	xargc = ac; 
	xargv = av;
I 6
        inits();
E 6
	infd = input();
	outfd = output();
D 6
        inits();
E 6

	/* GET PARAMETERS */
D 6
	if (!fgetpar("nva", &nva)) {
		warn(__FILE__, __LINE__, "Must specify nva");
		selfdoc();
	}
	if (!fgetpar("cdfpa", &cdfpa)) {
		warn(__FILE__, __LINE__, "Must specify cdfpa");
		selfdoc();
	}
E 6
I 6
D 8
	fgetpar("cdfpa", &cdfpa); 
E 8
	cdfpa=1;
I 8
	fgetpar("cdfpa", &cdfpa); 
E 8
        velfs=1400;
        vells=5300;
        nvf=40;
        incva=(vells-velfs)/(nvf-1);
E 6
	if (fgetpar("velfs", &velfs)) {/* first velocity*/
D 6
	fgetpar("vells", &vells);
	nvf = 50 ;   
E 6
I 6
	if (!fgetpar("vells", &vells)){
        warn(__FILE__,__LINE__," Must be specify  vells");
        selfdoc();
           }
        
E 6
	fgetpar("nvf", &nvf);
	incva = (vells - velfs) / (nvf - 1);
I 6

E 6
        } else { /* for increment in slowness */
D 6
        if (!fgetpar("swfs",swfs)){
        warn(__FILE__,__LINE__," Must be specify swfs or velfs");
        selfdoc();
           }
E 6
I 6
        if (fgetpar("swfs",swfs)){
E 6
        velfs=swfs;
        if (!fgetpar("swls",swls)){
D 6
        warn(__FILE__,__LINE__," Must be specify swls or vells");
E 6
I 6
        warn(__FILE__,__LINE__," Must be specify swls ");
E 6
        selfdoc();
           }
         vells=swls;
D 6
	nvf = 50 ;   
E 6
	fgetpar("nvf", &nvf);
	incva = (swfs - swls) / (nvf - 1);
I 6
         } /* end if */
E 6
	} /* end else*/
	stretch = 1.5; 
	fgetpar("stretch", &stretch);
	gate = 32;   
	fgetpar("gate", &gate);
I 6
        all=1;
E 6

I 6
        nva=maxgetpar();
        if(verbose)fprintf(stderr,"nva=%d\n",nva);
E 6
	cdflist = ( float *) malloc((unsigned) ( nva * sizeof(float)));
I 6

E 6
	if (fgetpar("cdflist", cdflist)) {/* list of cdf */
D 6
		fprintf (stderr, "cdflist=%f\n", cdflist[0]);
E 6
I 6
        all=0;

E 6
		cdfran = ( float *) malloc((unsigned)  (nva * cdfpa * sizeof(float)));
		cdfpr(cdfpa, cdflist, nva, cdfran);

	} else {/* for increment in cdf */
D 6
		if (!fgetpar("cdfr", &cdfr)) {
			warn(__FILE__, __LINE__, "Must specify cdfr or cdflist");
			selfdoc();
		}
E 6
I 6
		if (fgetpar("cdfr", &cdfr)) {
                nva=1000;
E 6
		if (!fgetpar("incdf", &incdf)) {
			warn(__FILE__, __LINE__, "Must specify incdf");
			selfdoc();
		}
		cdfran = (float *)malloc((unsigned) ( nva * cdfpa * sizeof(float)));
D 6
		for (ic = 0, cdf = cdfr; ic < nva * cdfpa; cdf + = incdf, ic++) {
E 6
I 6
D 8
                all=1;
E 8
I 8
                all=0;
E 8
		for (ic = 0, cdf = cdfr; ic < nva * cdfpa; cdf + = incdf, ic++){
E 6
			cdfran[ic] = cdf; 
		}
I 6
           } /* end if */
E 6
	} /* end else*/
D 6

E 6
I 6
        if( all==1) { nva=1000;
                
		cdfran = ( float *) malloc((unsigned)  (nva * cdfpa * sizeof(float)));
                           }/* in case of all cdfs */
      
E 6
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
D 6
       gate= gate/(dt*1000);
        fprintf (stderr, "gate=%f\n", gate);
E 6
I 6

        gate= gate/(dt*1000);
E 6
	bh.ns = (nt / gate)*2-1;
	putbh(outfd, &bh);
I 6
   /*printout of input parameters */
        fprintf(stderr,"cdfpa  =%f\n",cdfpa);
        fprintf(stderr,"velfs  =%f\n",velfs);
        fprintf(stderr,"vells  =%f\n",vells);
        fprintf(stderr,"nvf    =%f\n",nvf);
        fprintf(stderr,"stretch=%f\n",stretch);
        fprintf (stderr, "gate=%f in samples\n", gate);
E 6

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
I 8
				i =( nt / gate)*2-1;
					semb = (float *) malloc((i * sizeof(float)));
E 8
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
D 6
				fprintf(stderr, "cdf=%f\n", cdf);
				getcdf(infd, &coff, cdf);
E 6
I 6
				if(verbose)fprintf(stderr, "cdf=%f\n", cdf);
D 8
				getcdf(infd, &coff, cdf,all);
E 8
I 8
			flag=getcdf(infd, &coff, cdf,all);
                        if(!flag) exit(0);
E 8
E 6
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
D 6
            imm=b+ix;
/*       fprintf(stderr,"im=%d,xx=%f\n",imm,xx[imm]); */
E 6
          icount++;


					for (it = 0; it < nt; it++) {
                    a=c+i;
                    in=a+it;

						data2[a+it] = data[i+it];
                      icountc++;
D 6
						/*        fprintf(stderr,"data2=%f,data=%f,i=%d,it=%d\n",data2[i+it],data[i+it],i,it); */
E 6
					}/* end it */
				}/*end ix*/
                         b=icount;
                               c=icountc;
D 6
/*       fprintf(stderr,"ddddddddd   b=%d,c=%d\n",b,c); */
E 6

			} /* end il */
D 6
/*              fprintf(stderr,"nxs=%d\n",nxs); */
E 6

			/* allocation for semblance operations for every cdf*/
	if(!ic)		stack = (struct sem *)  malloc ((unsigned)(nxs * sizeof(struct sem )));
        if(nxs>oldnxs ) stack = (struct sem *) realloc (stack,(unsigned)(nxs*sizeof(struct sem )));
                  oldnxs=nxs;

			for (iv = 0, vel = velfs ; iv < nvf ; vel += incva, iv++) {
D 6
/* 				fprintf(stderr, "iv=%d,nxs=%d,vel=%f\n", iv, nxs, vel); */
E 6
I 6
	if(verbose)			fprintf(stderr, "nxs=%d,vel=%f\n",  nxs, vel);
E 6
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
D 6
						/*               fprintf(stderr,"nmoed=%f it=%d vel=%f\n",nmoed[it],it,vel); */
E 6

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
D 6
						/*           fprintf(stderr,"stac1=%f,sta2=%f,it=%d\n",stack1[it],stack2[it],it); */
E 6

					} /* end over im */
				} /* end loop over time */
				i =( nt / gate)*2-1;
D 6
				fprintf(stderr, "i=%d\n", i);
E 6
D 8
				if (!iv) { 
					semb = (float *) malloc((i * sizeof(float)));
				}
E 8
				_SEMBLAN_(stack1, stack2, &gate, &nt, semb, &i, &nxs);
D 6
/* 				for (i = 0; i < nt / gate ; i++) { */
/* 					fprintf(stderr, "semb=%f,i=%d,vel=%f\n", semb[i], i, vel); */
/* 				} */
E 6

				/*                 }                 */
				bcopy(semb, data, i * (sizeof(float)));
				/*         for(i=0;i< nt/gate ; i++){ */
D 6
				/*         fprintf(stderr,"semb=%f,i=%d,vel=%f\n",data[i],i,vel);} */
E 6
I 6
D 8
                                cdf=coff.o2;
E 8
I 8
                                if(all==1)cdf=coff.o2;
E 8
E 6
				putcdf(outfd, data, cdf, vel, i);
			}/*end loop over velocities */

		} /* end loop over cdf */


	} else {/* if cdfpa=1 */
		for (ic = 0 ; ic < nva * cdfpa ; ic++) {
			cdf = cdfran[ic];
D 6
/* 			fprintf(stderr, "ic=%d,cdff=%f\n", ic, cdf); */
			getcdf(infd, &coff, cdf);

E 6
I 6
			flag=getcdf(infd, &coff, cdf,all);
                        if(!flag) exit(0);
E 6
			data = coff.data;

			if (verbose) 
				fprintf(stderr, "Read  nt=%d \n",
				    nt);


			nx = coff.n2;
			/* allocation for semblance operations for every cdf*/
   if(!ic)		stack = (struct sem *)  malloc ((unsigned)(nx * sizeof(struct sem )));
D 6
   if(nx>oldnxs)  	stack = (struct sem *)  malloc (stack,(unsigned)(nx * sizeof(struct sem )));
E 6
I 6
   if(nx>oldnxs)  	stack = (struct sem *)  realloc (stack,(unsigned)(nx * sizeof(struct sem )));
E 6
                        oldnxs=nx;

			for (iv = 0, vel = velfs ; iv < nvf ; vel += incva, iv++) {
D 6
/* 				fprintf(stderr, "iv=%d,nx=%d,vel=%f\n", iv, nx, vel); */
E 6
I 6
	if(verbose)			fprintf(stderr, "nx=%d,vel=%f\n",  nx, vel);
E 6
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
D 6
						/*           if(nmoed[it]>0.00001)          fprintf(stderr,"nmoed=%f it=%d vel=%f\n",nmoed[it],it,vel); */
E 6

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
D 6
				fprintf(stderr, "i=%d\n", i);
E 6
D 8
				if (!iv) { 
					semb = (float *) malloc((i * sizeof(float)));
				}
E 8
				_SEMBLAN_(stack1, stack2, &gate, &nt, semb, &i, &nx);
D 6
/* 				for (i = 0; i < (nt / gate)*2-1 ; i++) { */
/* 					fprintf(stderr, "semb=%f,i=%d,vel=%f\n", semb[i], i, vel); */
/* 				} */
E 6

D 6
				/*                 }                 */
E 6
				bcopy(semb, data, i * (sizeof(float)));
D 6
				/*         for(i=0;i< nt/gate ; i++){ */
				/*         fprintf(stderr,"semb=%f,i=%d,vel=%f\n",data[i],i,vel);} */
E 6
I 6
D 8
                                cdf=coff.o2;
E 8
I 8
                                if(all==1) cdf=coff.o2;
E 8
E 6
				putcdf(outfd, data, cdf, vel, i);
			}/*end loop over velocities */

		} /* end loop over cdf */
	} /* end else */

} /* end main */


cdfpr(cdfpa, cdflist, nva, cdfran)
D 6
float	cdfpa, nva;
E 6
I 6
float	cdfpa;
int     nva;
E 6
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

E 1
