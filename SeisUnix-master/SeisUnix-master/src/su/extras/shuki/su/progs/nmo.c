
/* nmo - normal moveout
 * Credits:
 *
*/
#include <stdio.h>
#include <math.h>
#include "../include/su.h"
#ifdef HP
#define _SETGAIN_       setgain
#define _TRPENDS_       trpends
#define _CONNMO_        connmo
#define _LINTRP_        lintrp
#define _STRTCH_        strtch
#define _SNCTRP_        snctrp
#define _VARNMO_        varnmo
#else
#define _SETGAIN_       setgain_
#define _TRPENDS_       trpends_
#define _CONNMO_        connmo_
#define _LINTRP_        lintrp_
#define _STRTCH_        strtch_
#define _SNCTRP_        snctrp_
#define _VARNMO_        varnmo_
#endif

extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
extern char *SccsId;
 
static char lSccsId[]="@(#)nmo.c	1.28\t11/15/88\n";

struct v { float *v;float *tv;} *vtv;
int ncmp;
char *vfname;
int *nvmax;
float *vxtcmp;
double atof();
static float stretch;	/* hard mute limit 			*/

/*********************** self documentation **********************/
static char *lsdoc = 
"SUNMO - NMO and Inverse NMO with constant or variable velocity\n\
FOR V(x,t) -\n\
sunmo [-v] <stdin > stdout vfname= [ stretch=1.5  nw=]\n\
where vfname a velocity file vfname=filename\n\
                         Velocity file format:\n\
                m midpoint1\n\
                        tv1.1    v1.1\n\
                        tv1.2    v1.2\n\
                m midpoint2\n\
                        tv2.1    v2.1\n\
                        tv2.2    v2.2\n\
\n\
        The midpoints in the vfname should be in increasing order\n\
\n\
 FOR V(t)\n\
 sunmo [-v] <stdin > stdout tv=t1,t2,... vel=v1,v2,...[stretch=1.5 nw=]\n\
        where tv= vel=  are time-velocity  picks (in sec)\n\
\n\
 FOR V- CONSTANT\n\
 sunmo [-v] <stdin > stdout vel=v0 [stretch=1.5  nw=]\n\
        where v0 is a constant velocity\n\
\n\
 OPTIONS:\n\
    -v  turn verbose on by default off\n\
\n\
 PARAMETERS:\n\
     stretch- stretch factor by default stretch=1.5\n\
     nw     - number of points for sinc interpolation by default linear \n\
     interpolation\n\
     tv     - time values  (in case of v(t) velocity function)\n\
";
/*****************************************************************/
 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
Subhed *abh;
Sutrace *atrin,**aatrout;
int infd,outfd;
{
	*aatrout = atrin;
}

trseq(itr,atr,atrout,abh)
int itr;
Sutrace *atr,*atrout;
Subhed *abh;
{
	static float os;        /* ... and reciprocal			*/
	static int it, nt;	/* number of samples 			*/
	static float dt;	/* sampling interval 			*/
	static float tmax;	/* nt*dt				*/
	static float *v;	/* velocity picks for vel model		*/
	static float *vel;	/* velocity picks for vel model		*/
	static float *tv;	/* time picks for vel model		*/
	static int *itv;	/* normalized time pairs */
	static unsigned nv;	/* number of v's read by getpar		*/
	int iv;
	int ntv;        	/* number of tv's read by getpar	*/
	int j;	        	/* counter over v's and tv's		*/
	static int nw;		/* number of interpolation weights	*/
	static int t0, te;	/* smallest, largest non-zero weight row*/
	static float x;	        /* offset from trace header		*/
	static float *tnmo;	/* NMO times */
	static float *gain;	/* soft and hard mute */
	static int *itnmo;	/* base point for the interpolation, i.e.,
			   we use the indices itnmo+1, ...	*/
	static float *w;	/* interpolation weights		*/
	static float *nmoed;	/* NMO stretched data			*/
	static float *vvec;	/* velocity model v[nt]			*/
	static float *ovv;	/* sloth model -- sloth=sgn(v)/(v*v) */
	static int (*trpfun)();	/* interpolation function*/
	static int (*nmofun)();	/* NMO function	 */
	int velfile(),_VARNMO_(),_CONNMO_(),_LINTRP_(),_SNCTRP_();  
	char *s1;			/* string buffer */
	static enum {const,voft,vofxt} velvar;	/* velocity variations flag */
	int  opened;
	static int oldoffset,oldcmp;
	float a,b,c;
	bool newvelfun,hit;

	/* First trace */
	if(itr==0) {
		dt = abh->dt/1000000.0; /* tr.dt in microsec */
		nt = abh->ns;
		tmax = nt*dt;
	if (verbose) fprintf(stderr, "\tdt=%g nt=%d tmax=%g\n",dt,nt,tmax);

		/* Stretch reciprocal for muting */
		os = 1.0/stretch;

		/* Select type of interpolation */
		if (lgetpar("nw", &nw)) { /* sinc interpolation */
			trpfun = _SNCTRP_;
			if (nw < 2) {
		   	err(__FILE__,__LINE__,"nw = %d--must be >= 2 for sinc interpolation", nw);
			}

			if (nw % 2) {
				nw++;
				warn(__FILE__,__LINE__,"changed nw (must be even)");
			}
			if (verbose) {
				fprintf(stderr,
			    	"\tinterpolation = sinc with %d points\n", nw);
			}
		} else { /* default linear interpolation */
			trpfun = _LINTRP_;
			nw = 2;
			if (verbose) {
				fprintf(stderr, "\tinterpolation = linear\n");
			}
		}

		/* Allocate space for nmo operations */
		tnmo  = (float *) malloc((unsigned) (nt * sizeof (float)));
		gain  = (float *) malloc((unsigned) (nt * sizeof (float)));
		itnmo = (int *)   malloc((unsigned) (nt * sizeof (int)));
		w     = (float *) malloc((unsigned) (nt * nw * sizeof (float)));
		nmoed = (float *) malloc((unsigned) (nt * sizeof (float)));
		vvec  = (float*)  malloc((unsigned) (nt * sizeof(float)));
		ovv   = (float*)  malloc((unsigned) (nt * sizeof(float)));
		/*allocate space for velocity model using maxgetpar()*/
		nv = maxgetpar();
		vel  = (float *) malloc((unsigned) (nv*sizeof (float)));
		/* Velocity model */
		if(sgetpar("vfname",&vfname)) opened=velfile(nt);    

	        if(fgetpar("vel",vel)==1) {	/* Constant? */
				opened = 2;
			}				/* Illegal */
/*			fprintf(stderr,"opened=%d\n",opened); */

			if(opened==0  && !sgetpar("tv",&s1)) {
				warn(__FILE__,__LINE__,"must specify velocity model");
				selfdoc();
			}
		if(opened==1) {	/* IF VFILE */
			/* Set lateral velocity variations flag */
			velvar = vofxt;
			/* Set velocity nmo function */
                         nmofun=_VARNMO_;
                        for (j=0 ; j < ncmp ; j++) { /* loop over midpoint */
              iv= *(nvmax+j);
              v   = (float*)  malloc((unsigned) (iv * sizeof(float)));
              itv   = (int*)  malloc((unsigned) (iv * sizeof(float)));
               for (nv=0;nv <= *(nvmax+j) ; nv++ ) { /*loop over velocities */
/*        fprintf(stderr,"\vtv=%f,itv=%f\n",vtv[j].v[nv],vtv[j].tv[nv]); */
/*        fprintf(stderr,"\nv=%d\n",nv); */
       /* normalize v and tv so that dt=1 */
          v[nv]=vtv[j].v[nv]*dt;
          itv[nv]= vtv[j].tv[nv]/dt;
/*        fprintf(stderr,"\tnt=%d,v[%d]=%f,itv=%d,dt=%f\n",nt,nv,v[nv],itv[nv],dt); */
	 }
         /* interpolate velocity model */
         vintrp(vtv[j].v,nt,v,itv,nv); 

        /* convert to sloth */
        for (it=0;it< nt;it++) {
          vtv[j].v[it] =1.0/(fabs(vtv[j].v[it])* vtv[j].v[it]);
/*        fprintf(stderr,"\vtv[%d].v[%d]=%f\n",j,it,vtv[j].v[it]);  */
                    }
                  }
		} else {			/* NOT VFILE */
/*                 fprintf(stderr,"not vfile\n"); */
			nv = fgetpar("vel",vel);
/*                 fprintf(stderr,"nv=%d\n",nv); */
                   if(!nv) {
                     warn(__FILE__,__LINE__,"velocity model unspecified");
                       selfdoc();
                           }
       /* check that user supplied v  are nonzero. */
         for (j=0; j<nv; j++ ) if(vel[j] == 0.0)  {
       err(__FILE__,__LINE__,"vel  must be non zero");
			}
			tv  = (float *) malloc((unsigned)(nv*sizeof(float)));
			itv = (int *)   malloc((unsigned)(nv * sizeof (int)));

			ntv = fgetpar("tv", tv);

			/* Test for monotonicity */
			for (j = 0; j < ntv-1; j++) {
				if (tv[j+1] <= tv[j]) {
			   	err(__FILE__,__LINE__,"tv's must increase: tv[%d]=%f tv[%d]=%f",
						j,tv[j],j+1,tv[j+1]);
				}
			}
			if(nv==1) {		/* CONSTANT VELOCITY */
				/* Convert to sloth */
				*ovv = 1.0/(fabs(*vel) * *vel * dt * dt);
				velvar = const;
				nmofun = _CONNMO_;
				if (ntv) warn(__FILE__,__LINE__,"tv ignored (constant velocity)");
				if (verbose) {
/* 					fprintf(stderr,"\tconstant velocity nmo v=%g\n",v[0]); */
				}
} else {	/* NOT CONSTANT, MUST BE V(T)*/
				velvar = voft;
	                         nmofun = _VARNMO_;
                                fprintf(stderr,"\tvariable velocity nmo\n");

				if (!ntv) {
					warn(__FILE__,__LINE__,"for variable velocity, need tv=");
					selfdoc();
				} else if (ntv != nv) {
					err(__FILE__,__LINE__,"number of tv's (%d) and v's (%d) unequal",ntv, nv);
				}
				if (verbose) {
					fprintf(stderr,"\tvertically varying velocity nmo\n");
					for (j = 0; j < nv; j++) {
/* 					fprintf(stderr,"\ttv=%.2f\tv=%g\n",tv[j],v[j]); */
					}
				}
				/* Normalize vel and tv so that dt = 1 */
				for (j=0;j<nv;j++) {
					vel[j] *= dt;
					itv[j] = tv[j]/dt;
				}

				/* Interpolate velocity model */
				vintrp(vvec,nt,vel,itv,nv);

				/* Convert to sloth */
				for(j=0;j<nt;j++) {
					ovv[j] = 1.0/(fabs(vvec[j])*vvec[j]);
				}

			} /* END ELSE (NOT CONSTANT, MUST BE V(T)) */

		} /* ENDELSE (NOT VFILE) */

		oldcmp = atr->cdp - 1;
		oldoffset = atr->offset - 1;

	}	/* ENDIF FIRST TRACE */


	if(verbose)
	fprintf(stderr,"\nsx=%d gx=%d cdp=%d offset=%d\n",atr->sx,atr->gx,atr->cdp,atr->offset);

	x = atr->offset;

	/* New velocity function ? */
 	if(velvar==vofxt&&atr->cdp!=oldcmp) {

		newvelfun = true;

		/* Find j = midpoint index in the velocity model */
			for(j=0;j<ncmp;j++) {

/* 			fprintf(stderr,"j=%d vxtcmp[%d]=%f\n",j,j,vxtcmp[j]); */

			if(vxtcmp[j]==atr->cdp) {	/* Hit */
				hit = true;
				break;
/* 			} else if(vxtcmp[j]>atr->cdp) {	 Passed */ 
			} else if(vxtcmp[j]!=atr->cdp) {	/* Passed */
				hit = false;
				break;
			}
		}

		if(j<0||j>ncmp) err(__FILE__,__LINE__,"Illigal j=%d",j);

/* 		fprintf(stderr,"j=%d hit=%d,ncmp=%d\n",j,hit,ncmp); */
/* 		fprintf(stderr,"vxt[%d].cmp=%d atr->cdp=%d vxt[%d].cmp=%d\n",j-1,vxt[j-1].cmp,atr->cdp,j,vxt[j].cmp); */

		/* Now that j is known, find the sloth for the cdp */
		if(hit) {		/* exact */
			bcopy(vtv[j].v,ovv,nt*4);
		} else if(j==0) {	/* end */
			bcopy(vtv[0].v,ovv,nt*4);
		} else if (j==ncmp) {	/* end */
			bcopy(vtv[ncmp-1].v,ovv,nt*4);
		} else {		/* linearly interpolate */
			a = vxtcmp[j] - atr->cdp;
			b = atr->cdp - vxtcmp[j-1];
			c = a + b;
			a /= c;
			b /= c;
/* 			fprintf(stderr,"a=%f b=%f\n",a,b); */
			for(it=0;it<nt;it++) {
		   	ovv[it]=a*vtv[j-1].v[it]+b*vtv[j].v[it];
			}
/* 			fprintf(stderr," ovv[0]=%g = a * vxt[%d].ovv[0]=%g + b * vxt[%d].ovv[0]=%g\n",ovv[0], j-1, vxt[j-1].ovv[0], j, vxt[j].ovv[0]); */
		}
	} else newvelfun = false;

	/* New nmo function ? */
	if(newvelfun||atr->offset!=oldoffset) {

		/* Calculate nmo time tnmo */
		(*nmofun)(tnmo, &nt, ovv, &x);
		_SETGAIN_(tnmo,gain,&nt);
		hardmute(gain,nt,os);

		for(j=0;j<nt;j+=nt/20)
/* 			fprintf(stderr,"tnmo[%d]=%g\tgain=%g\n",j,tnmo[j],gain[j]); */

		/* Calculate the interpolation coefficients */
		(*trpfun)(itnmo, w, tnmo, &nt, &nw,gain);
		_TRPENDS_(itnmo,w,&t0,&te,&nt,&nw);
	}

	/* Perform the NMO; put new data into nmoed */
	_STRTCH_(nmoed, atr->data, &nt, &t0, &te, itnmo, w, &nw);	

	/* Overwrite the segy data */
	bcopy(nmoed, atr->data, nt * sizeof (float));

	oldoffset = atr->offset;
	oldcmp = atr->cdp;

	return(1);
}

/****************************************************************/

hardmute(g,n,os)
int n;
float *g,os;
{
	int i;
	for(i=0;i<n;i++) {
		if(g[i]<os) g[i] = 0.0;
	}
}

/****************************************************************/

vintrp(vvec,nt,v,itv,nv)
int nt,nv,*itv;
float *vvec,*v;
{
	int i,j,minnttv;
	for(i=0;i<itv[0];i++) {
		vvec[i] = v[0];
	}
	minnttv = MIN(nt,itv[nv-1]);
	for(;i<minnttv;i++) {
		for(j=1;i>=itv[j];j++);
		vvec[i] = v[j-1]+(i-itv[j-1])*(v[j]-v[j-1])/(itv[j]-itv[j-1]);
	}
	for(;i<nt;i++) {
		vvec[i] = v[nv-1];
	}
}

/* INITIALIZE SELF DOCUMENTATION */
inits()
{
	int c;

	sdoc = lsdoc;
        SccsId = lSccsId;


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

	/* GET PARAMETERS */
	stretch = 1.5; fgetpar("stretch", &stretch);
}
postp(){}
