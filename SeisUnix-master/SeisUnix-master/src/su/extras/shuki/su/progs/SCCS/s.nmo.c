h33307
s 00053/00053/00362
d D 1.28 88/11/15 14:02:32 shuki 40 39
c 
e
s 00001/00001/00414
d D 1.27 88/07/14 11:15:05 shemer 39 37
c a little change in selfdoc
e
s 00001/00001/00414
d R 1.27 88/07/14 10:57:49 shemer 38 37
c after finding a bug in vvec memory allocation
e
s 00002/00002/00413
d D 1.26 88/06/06 13:12:14 shuki 37 36
c Cancel ns in trace headers
e
s 00008/00003/00407
d D 1.25 88/05/29 15:34:37 shemer 36 34
c 
e
s 00006/00002/00408
d R 1.25 88/05/25 14:53:44 shemer 35 34
c with SccsId[]
e
s 00062/00049/00348
d D 1.24 88/05/25 11:45:35 shemer 34 32
c final version i hope
e
s 00010/00000/00397
d R 1.19.2.6 88/05/25 11:15:57 shemer 33 31
c 
e
s 00016/00003/00381
d D 1.23 88/05/25 06:53:01 shuki 32 29
c umainseq
e
s 00004/00004/00393
d D 1.19.2.5 88/05/12 12:58:49 shemer 31 30
c 
e
s 00030/00024/00367
d D 1.19.2.4 88/05/12 12:38:49 shemer 30 27
c 
e
s 00007/00006/00377
d D 1.22 88/05/12 11:34:21 shemer 29 28
c 
e
s 00020/00012/00363
d D 1.21 88/05/12 09:50:26 shemer 28 21
c 
e
s 00004/00004/00387
d D 1.19.2.3 88/05/08 09:41:07 shemer 27 26
c 
e
s 00041/00025/00350
d D 1.19.2.2 88/05/08 09:30:27 shemer 26 25
c final version
e
s 00002/00002/00373
d D 1.19.2.1 88/05/05 07:21:36 shemer 25 22
c 
e
s 00001/00001/00374
d D 1.19.1.3 88/05/04 16:51:17 shemer 24 23
c 
e
s 00001/00001/00374
d D 1.19.1.2 88/05/04 16:46:41 shemer 23 22
c 
e
s 00003/00001/00372
d D 1.19.1.1 88/05/04 16:35:38 shemer 22 20
c 
e
s 00002/00000/00373
d D 1.20 88/05/04 15:57:30 shemer 21 20
c 
e
s 00001/00001/00372
d D 1.19 88/05/04 10:52:35 shemer 20 19
c 
e
s 00002/00002/00371
d D 1.18 88/05/04 07:27:36 shemer 19 18
c 
e
s 00001/00001/00372
d D 1.17 88/05/04 07:12:51 shemer 18 17
c 
e
s 00001/00000/00372
d D 1.16 88/05/03 16:56:35 shemer 17 16
c 
e
s 00001/00000/00371
d D 1.15 88/05/03 16:44:25 shemer 16 15
c 
e
s 00001/00000/00370
d D 1.14 88/05/03 16:38:59 shemer 15 14
c 
e
s 00002/00002/00368
d D 1.13 88/05/03 09:08:06 shemer 14 13
c 
e
s 00001/00001/00369
d D 1.12 88/04/28 15:21:53 shemer 13 12
c 
e
s 00022/00028/00348
d D 1.11 88/04/28 15:17:31 shemer 12 11
c 
e
s 00002/00000/00374
d D 1.10 88/04/28 14:09:28 shemer 11 10
c 
e
s 00002/00001/00372
d D 1.9 88/04/28 13:52:57 shemer 10 9
c 
e
s 00001/00002/00372
d D 1.8 88/04/28 12:52:10 shemer 9 8
c 
e
s 00013/00008/00361
d D 1.7 88/04/28 12:40:51 shemer 8 6
c 
e
s 00001/00000/00368
d D 1.5.1.1 88/04/28 11:47:06 shemer 7 5
c 
e
s 00001/00000/00368
d D 1.6 88/04/28 11:41:45 shemer 6 5
c 
e
s 00001/00000/00367
d D 1.5 88/04/28 11:17:54 shemer 5 4
c 
e
s 00002/00002/00365
d D 1.4 88/04/28 10:38:41 shemer 4 3
c 
e
s 00001/00001/00366
d D 1.3 88/04/28 08:25:25 shemer 3 2
c 
e
s 00001/00001/00366
d D 1.2 88/04/27 16:19:15 shemer 2 1
c 
e
s 00367/00000/00000
d D 1.1 88/04/25 14:59:18 shemer 1 0
c date and time created 88/04/25 14:59:18 by shemer
e
u
U
f e 0
t
T
I 1

/* nmo - normal moveout
 * Credits:
 *
*/
#include <stdio.h>
#include <math.h>
D 2
#include "su.h"
E 2
I 2
#include "../include/su.h"
I 34
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
E 34
I 26
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
E 26
I 12

E 12
E 2
extern char *sdoc;
extern int xargc;
extern char **xargv;
extern bool verbose;
I 36
extern char *SccsId;
 
static char lSccsId[]="%W%\t%G%\n";

E 36
D 40
 struct v { float *v;float *tv;} *vtv;
 int ncmp;
 char vfname[50];
 int *nvmax;
 float *vxtcmp;
 double atof();
 static float stretch;	/* hard mute limit 			*/
E 40
I 40
struct v { float *v;float *tv;} *vtv;
int ncmp;
char *vfname;
int *nvmax;
float *vxtcmp;
double atof();
static float stretch;	/* hard mute limit 			*/

E 40
/*********************** self documentation **********************/
static char *lsdoc = 
D 30
D 34
"SUNMO - NMO and Inverse NMO with constant or variable velocity		\n\
E 34
I 34
D 40
"SUNMO - NMO and Inverse NMO with constant or variable velocity         \n\
E 34
D 12
 sunmo <stdin > stdout v= [ stretch=1.5 verbose=0 tv= vfname= nw=]	\n\
	Constant velocity nmo is with v=v0.				\n\
E 12
I 12
D 28
 sunmo <stdin > stdout vel= [ stretch=1.5 verbose=0 tv= vfname= nw=]	\n\
	Constant velocity nmo is with vel=v0.				\n\
E 12
	To specify a v(t) velocity give the optional			\n\
D 12
	v=v1,...vn and tv=t1,...,tn pairs of velocity-time picks.	\n\
E 12
I 12
	vel=v1,...vn and tv=t1,...,tn pairs of velocity-time picks.	\n\
E 12
	To specify a v(t,x) velocity give a velocity file,		\n\
	vfname=filename. Velocity file format:		                \n\
E 28
I 28
 FOR V(x,t) -                                                           \n\
D 34
 sunmo <stdin > stdout vfname= [ stretch=1.5 -v  nw=]	                \n\
	where vfname a velocity file vfname=filename                    \n\
	                 Velocity file format:		                \n\
E 28
		m midpoint1						\n\
			tv1.1    v1.1					\n\
			tv1.2    v1.2					\n\
	  	m midpoint2						\n\
			tv2.1    v2.1					\n\
			tv2.2    v2.2					\n\
							        	\n\
	The midpoints in the vfname should be in increasing order	\n\
							        	\n\
E 34
I 34
 sunmo [-v] <stdin > stdout vfname= [ stretch=1.5  nw=]                 \n\
        where vfname a velocity file vfname=filename                    \n\
                         Velocity file format:                          \n\
                m midpoint1                                             \n\
                        tv1.1    v1.1                                   \n\
                        tv1.2    v1.2                                   \n\
                m midpoint2                                             \n\
                        tv2.1    v2.1                                   \n\
                        tv2.2    v2.2                                   \n\
E 34
D 28
	stretch is the maximum nmo stretch for hard mute.		\n\
E 28
I 28
                                                                        \n\
I 34
        The midpoints in the vfname should be in increasing order       \n\
                                                                        \n\
E 34
 FOR V(t)                                                               \n\
D 29
 sunmo <stdin > stdout vel=v1....vn  tv=t1...tn [stretch=1.5 -v  nw=]   \n\
	where vel= tv=  are pairs of velocity time picks                \n\
                                                                        \n\ 
 FOR V- CONSTANT                                                        \n\ 
E 29
I 29
D 34
 sunmo <stdin > stdout tv=t1,t2,... vel=v1,v2,... [stretch=1.5 -v nw=]  \n\
	where tv= vel=  are time-velocity  picks                        \n\
E 34
I 34
 sunmo [-v]  <stdin > stdout tv=t1,t2,... vel=v1,v2,...[stretch=1.5 nw=]\n\
D 39
        where tv= vel=  are time-velocity  picks                        \n\
E 39
I 39
        where tv= vel=  are time-velocity  picks  (in sec)              \n\
E 39
E 34
                                                                        \n\
 FOR V- CONSTANT                                                        \n\
E 29
D 34
 sunmo <stdin > stdout vel=v0 [stretch=1.5 -v  nw=]                     \n\
I 29
	where v0 is a constant velocity                                 \n\
E 34
I 34
 sunmo [-v] <stdin > stdout vel=v0 [stretch=1.5  nw=]                   \n\
        where v0 is a constant velocity                                 \n\
E 34
E 29
                                                                        \n\
 OPTIONS:                                                               \n\
D 34
    -v  turn verbose on                                                 \n\
E 34
I 34
    -v  turn verbose on by default off                                  \n\
E 34
                                                                        \n\
D 29
 PARAMETERS:                                                            \n\ 
E 29
I 29
 PARAMETERS:                                                            \n\
E 29
     stretch- stretch factor by default stretch=1.5                     \n\
E 40
I 40
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
E 40
     nw     - number of points for sinc interpolation by default linear \n\
D 40
     interpolation                                                      \n\
D 29
     tv     - time for vel time pairs (in case of v(t) velocity function\n\
E 29
I 29
     tv     - time values  (in case of v(t) velocity function)          \n\
E 40
I 40
     interpolation\n\
     tv     - time values  (in case of v(t) velocity function)\n\
E 40
E 29
E 28
D 34
									\n\
E 34
D 28
	The default interpolation is linear.  To specify the		\n\
	alternative sinc interpolation, give the optional		\n\
	parameter, nw, that specifies the number of points		\n\
	to use.								\n\
	verbose (1=YES, 0=NO) for echoing of info to stderr.		\n\
E 30
I 30
"SUNMO - NMO and Inverse NMO with constant or variable velocity         \n\
 FOR V(x,t) -                                                           \n\
D 31
 sunmo <stdin > stdout vfname= [ stretch=1.5 -v  nw=]                   \n\
E 31
I 31
 sunmo [-v] <stdin > stdout vfname= [ stretch=1.5  nw=]                 \n\
E 31
        where vfname a velocity file vfname=filename                    \n\
                         Velocity file format:                          \n\
                m midpoint1                                             \n\
                        tv1.1    v1.1                                   \n\
                        tv1.2    v1.2                                   \n\
                m midpoint2                                             \n\
                        tv2.1    v2.1                                   \n\
                        tv2.2    v2.2                                   \n\
                                                                        \n\
        The midpoints in the vfname should be in increasing order       \n\
                                                                        \n\
 FOR V(t)                                                               \n\
D 31
 sunmo <stdin > stdout tv=t1,t2,... vel=v1,v2,... [stretch=1.5 -v nw=]  \n\
E 31
I 31
 sunmo [-v]  <stdin > stdout tv=t1,t2,... vel=v1,v2,...[stretch=1.5 nw=]\n\
E 31
        where tv= vel=  are time-velocity  picks                        \n\
                                                                        \n\
 FOR V- CONSTANT                                                        \n\
D 31
 sunmo <stdin > stdout vel=v0 [stretch=1.5 -v  nw=]                     \n\
E 31
I 31
 sunmo [-v] <stdin > stdout vel=v0 [stretch=1.5  nw=]                   \n\
E 31
        where v0 is a constant velocity                                 \n\
                                                                        \n\
 OPTIONS:                                                               \n\
D 31
    -v  turn verbose on                                                 \n\
E 31
I 31
    -v  turn verbose on by default off                                  \n\
E 31
                                                                        \n\
 PARAMETERS:                                                            \n\
     stretch- stretch factor by default stretch=1.5                     \n\
     nw     - number of points for sinc interpolation by default linear \n\
     interpolation                                                      \n\
     tv     - time values  (in case of v(t) velocity function)          \n\
E 30
E 28
";
D 30
/*****************************************************************/
E 30
D 32
trseq(itr,atr,abh)
int itr;
Sutrace *atr;
E 32
I 32
 
/* PRE PROCESSING */
prep(abh,atrin,aatrout,infd,outfd)
E 32
Subhed *abh;
I 32
Sutrace *atrin,**aatrout;
int infd,outfd;
E 32
{
I 32
D 34
    *aatrout = atrin;
E 34
I 34
D 40
   *aatrout = atrin;
E 40
I 40
	*aatrout = atrin;
E 40
E 34
}
D 34
 
/* TRACE SEQUENTIAL TPOW GAIN PROCESSING */
E 34
I 34

E 34
trseq(itr,atr,atrout,abh)
D 34
int itr;    
E 34
I 34
int itr;
E 34
Sutrace *atr,*atrout;
Subhed *abh;
{
E 32
D 12
	static float os;	/* ... and reciprocal			*/
E 12
I 12
	static float os;        /* ... and reciprocal			*/
E 12
	static int it, nt;	/* number of samples 			*/
	static float dt;	/* sampling interval 			*/
	static float tmax;	/* nt*dt				*/
	static float *v;	/* velocity picks for vel model		*/
I 8
	static float *vel;	/* velocity picks for vel model		*/
E 8
	static float *tv;	/* time picks for vel model		*/
	static int *itv;	/* normalized time pairs */
	static unsigned nv;	/* number of v's read by getpar		*/
D 12
	 int iv;
	int ntv;	/* number of tv's read by getpar	*/
	int j;		/* counter over v's and tv's		*/
E 12
I 12
D 40
        int iv;
E 40
I 40
	int iv;
E 40
	int ntv;        	/* number of tv's read by getpar	*/
	int j;	        	/* counter over v's and tv's		*/
E 12
	static int nw;		/* number of interpolation weights	*/
	static int t0, te;	/* smallest, largest non-zero weight row*/
D 12
	static float x;	/* offset from trace header		*/
E 12
I 12
	static float x;	        /* offset from trace header		*/
E 12
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
D 12
        int velfile(),varnmo_(),connmo_(),lintrp_();
E 12
I 12
D 26
D 34
        int velfile(),varnmo_(),connmo_(),lintrp_(),snctrp_();  
E 34
I 34
D 40
        int velfile(),_VARNMO_(),_CONNMO_(),_LINTRP_(),_SNCTRP_();  
E 34
E 26
I 26
        int velfile(),_VARNMO_(),_CONNMO_(),_LINTRP_(),_SNCTRP_();  
E 26
E 12
	char s1[50];			/* string buffer */
E 40
I 40
	int velfile(),_VARNMO_(),_CONNMO_(),_LINTRP_(),_SNCTRP_();  
	char *s1;			/* string buffer */
E 40
	static enum {const,voft,vofxt} velvar;	/* velocity variations flag */
D 40
        int  opened;
E 40
I 40
	int  opened;
E 40
	static int oldoffset,oldcmp;
	float a,b,c;
	bool newvelfun,hit;

	/* First trace */
D 40
          if(itr==0) {
E 40
I 40
	if(itr==0) {
E 40
D 37
		dt = atr->dt/1000000.0; /* tr.dt in microsec */
		nt = atr->ns;
E 37
I 37
		dt = abh->dt/1000000.0; /* tr.dt in microsec */
		nt = abh->ns;
E 37
		tmax = nt*dt;
	if (verbose) fprintf(stderr, "\tdt=%g nt=%d tmax=%g\n",dt,nt,tmax);

		/* Stretch reciprocal for muting */
D 40
              os = 1.0/stretch;
E 40
I 40
		os = 1.0/stretch;
E 40

		/* Select type of interpolation */
		if (lgetpar("nw", &nw)) { /* sinc interpolation */
I 12
D 13
                        trpfun = snctrp;
E 13
I 13
D 26
D 34
                        trpfun = snctrp_;
E 34
I 34
D 40
                        trpfun = _SNCTRP_;
E 40
I 40
			trpfun = _SNCTRP_;
E 40
E 34
E 26
I 26
                        trpfun = _SNCTRP_;
E 26
E 13
E 12
			if (nw < 2) {
D 26
D 34
		   	err("nw = %d--must be >= 2 for sinc interpolation", nw);
E 34
I 34
		   	err(__FILE__,__LINE__,"nw = %d--must be >= 2 for sinc interpolation", nw);
E 34
E 26
I 26
		   	err(__FILE__,__LINE__,"nw = %d--must be >= 2 for sinc interpolation", nw);
E 26
			}

			if (nw % 2) {
				nw++;
D 26
D 34
				warn("changed nw (must be even)");
E 34
I 34
				warn(__FILE__,__LINE__,"changed nw (must be even)");
E 34
E 26
I 26
D 27
				warn(__FILE__,LINE__,"changed nw (must be even)");
E 27
I 27
				warn(__FILE__,__LINE__,"changed nw (must be even)");
E 27
E 26
			}
			if (verbose) {
				fprintf(stderr,
			    	"\tinterpolation = sinc with %d points\n", nw);
			}
		} else { /* default linear interpolation */
D 26
D 34
                     trpfun = lintrp_;
E 34
I 34
D 40
                     trpfun = _LINTRP_;
E 40
I 40
			trpfun = _LINTRP_;
E 40
E 34
E 26
I 26
                     trpfun = _LINTRP_;
E 26
			nw = 2;
D 26
			if (verbose) {
E 26
I 26
D 27
			NE__,if (verbose) {
E 27
I 27
			if (verbose) {
E 27
E 26
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
I 8
D 40
             /*allocate space for velocity model using maxgetpar()*/
D 9
                          nv = maxgetpar()
E 9
I 9
                          nv = maxgetpar();
E 9
                vel  = (float *) malloc((unsigned) (nv*sizeof (float)));
D 9

E 9
E 8
				/* Velocity model */
I 11
D 12
              fprintf(stderr,"before vfname\n");
E 11
		if(sgetpar("vfname",vfname))     
                 opened = velfile(nt);
I 11
              fprintf(stderr,"after vfname\n");
E 11
D 4
		if(!opened) err("can't fopen(%s,%s)",vfname,"r");
E 4
I 4
/* 		if(!opened) err("can't fopen(%s,%s)",vfname,"r"); */
E 4
D 8
			 if(fgetpar("v",v)==1) {	/* Constant? */
E 8
I 8
D 10
			 if(fgetpar("vel",vel)==1) {	/* Constant? */
E 10
I 10
	 if(fgetpar("vel",vel)==1) {	/* Constant? */
E 12
I 12
		if(sgetpar("vfname",vfname)) opened=velfile(nt);    
E 40
I 40
		/*allocate space for velocity model using maxgetpar()*/
		nv = maxgetpar();
		vel  = (float *) malloc((unsigned) (nv*sizeof (float)));
		/* Velocity model */
		if(sgetpar("vfname",&vfname)) opened=velfile(nt);    
E 40

	        if(fgetpar("vel",vel)==1) {	/* Constant? */
E 12
E 10
E 8
D 19
				opened = 2;
E 19
I 19
D 20
				opened = 1;
E 20
I 20
				opened = 2;
E 20
E 19
			}				/* Illegal */
I 15
D 26
D 34
                 fprintf(stderr,"opened=%d\n",opened);
E 34
I 34
D 40
/*                  fprintf(stderr,"opened=%d\n",opened); */
E 40
I 40
/*			fprintf(stderr,"opened=%d\n",opened); */
E 40
E 34
E 26
I 26
/*                  fprintf(stderr,"opened=%d\n",opened); */
E 26
I 17
D 18
                 fprintf(stderr,"sgetpar\n",sgetpar("tv",s1));
E 18
I 18
D 36
                 fprintf(stderr,"sgetpar=%d\n",sgetpar("tv",s1));
E 36
E 18
E 17
E 15
I 10
D 12
              fprintf(stderr,"opened=%d\n",opened);
E 10
                         if(opened!=2 && opened !=1 && !sgetpar("tv",s1)) {
E 12
I 12

D 19
                if(opened!=2 && opened !=1 && !sgetpar("tv",s1)) {
E 19
I 19
D 40
                if(opened==0  && !sgetpar("tv",s1)) {
E 40
I 40
			if(opened==0  && !sgetpar("tv",&s1)) {
E 40
E 19
E 12
D 26
D 34
				warn("must specify velocity model");
E 34
I 34
				warn(__FILE__,__LINE__,"must specify velocity model");
E 34
E 26
I 26
				warn(__FILE__,__LINE__,"must specify velocity model");
E 26
				selfdoc();
			}
D 4
		if(opened) {	/* IF VFILE */
E 4
I 4
		if(opened==1) {	/* IF VFILE */
E 4
			/* Set lateral velocity variations flag */
			velvar = vofxt;
			/* Set velocity nmo function */
D 26
D 34
                         nmofun=varnmo_;
E 34
I 34
                         nmofun=_VARNMO_;
E 34
E 26
I 26
                         nmofun=_VARNMO_;
E 26
D 12
       for (j=0 ; j < ncmp ; j++) { /* loop over midpoint */
/*         fprintf(stderr,"\nvmax=%d\n",*(nvmax+1)); */
E 12
I 12
                        for (j=0 ; j < ncmp ; j++) { /* loop over midpoint */
E 12
D 3
              iv=*(nvmax+j);
E 3
I 3
              iv= *(nvmax+j);
E 3
D 12
             v   = (float*)  malloc((unsigned) (iv * sizeof(float)));
                itv   = (int*)  malloc((unsigned) (iv * sizeof(float)));
          for (nv=0;nv <= *(nvmax+j) ; nv++ ) { /*loop over velocities */
E 12
I 12
              v   = (float*)  malloc((unsigned) (iv * sizeof(float)));
              itv   = (int*)  malloc((unsigned) (iv * sizeof(float)));
               for (nv=0;nv <= *(nvmax+j) ; nv++ ) { /*loop over velocities */
E 12
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
I 16
D 26
D 34
                fprintf(stderr,"not vfile\n");
E 34
I 34
/*                 fprintf(stderr,"not vfile\n"); */
E 34
E 26
I 26
/*                 fprintf(stderr,"not vfile\n"); */
E 26
E 16
I 7
           fprintf(stderr,"i am in else\n");
E 7
I 6
D 8
             fprintf(stderr,"i am in else\n")
E 6
			nv = fgetpar("v",v);
E 8
I 8
D 12
           fprintf(stderr,"i am in else\n");
E 12
			nv = fgetpar("vel",vel);
E 8
I 5
D 12
           fprintf(stderr,"nv=%d\n",nv);
E 12
I 12
D 26
D 34
                fprintf(stderr,"nv=%d\n",nv);
E 34
I 34
/*                 fprintf(stderr,"nv=%d\n",nv); */
E 34
E 26
I 26
/*                 fprintf(stderr,"nv=%d\n",nv); */
E 26
E 12
E 5
                   if(!nv) {
D 26
D 34
                     warn("velocity model unspecified");
E 34
I 34
                     warn(__FILE__,__LINE__,"velocity model unspecified");
E 34
E 26
I 26
D 27
                     warn(__FILE__,LINE__,"velocity model unspecified");
E 27
I 27
                     warn(__FILE__,__LINE__,"velocity model unspecified");
E 27
E 26
                       selfdoc();
                           }
       /* check that user supplied v  are nonzero. */
D 8
         for (j=0; j<nv; j++ ) if(v[j] == 0.0)  {
       err("v must be non zero");
				err("v's must be non zero");
E 8
I 8
         for (j=0; j<nv; j++ ) if(vel[j] == 0.0)  {
D 26
D 34
       err("vel  must be non zero");
				err("vel's must be non zero");
E 34
I 34
       err(__FILE__,__LINE__,"vel  must be non zero");
E 34
E 26
I 26
       err(__FILE__,__LINE__,"vel  must be non zero");
E 26
E 8
			}
			tv  = (float *) malloc((unsigned)(nv*sizeof(float)));
			itv = (int *)   malloc((unsigned)(nv * sizeof (int)));

			ntv = fgetpar("tv", tv);

			/* Test for monotonicity */
			for (j = 0; j < ntv-1; j++) {
				if (tv[j+1] <= tv[j]) {
D 26
D 34
			   	err("tv's must increase: tv[%d]=%f tv[%d]=%f",
E 34
I 34
			   	err(__FILE__,__LINE__,"tv's must increase: tv[%d]=%f tv[%d]=%f",
E 34
E 26
I 26
			   	err(__FILE__,__LINE__,"tv's must increase: tv[%d]=%f tv[%d]=%f",
E 26
						j,tv[j],j+1,tv[j+1]);
				}
			}
			if(nv==1) {		/* CONSTANT VELOCITY */
				/* Convert to sloth */
D 8
				*ovv = 1.0/(fabs(*v) * *v * dt * dt);
E 8
I 8
				*ovv = 1.0/(fabs(*vel) * *vel * dt * dt);
E 8
				velvar = const;
D 26
D 34
				nmofun = connmo_/*_*/;	/* <<< HP PATCH >>> */
				if (ntv) warn("tv ignored (constant velocity)");
E 34
I 34
				nmofun = _CONNMO_;
				if (ntv) warn(__FILE__,__LINE__,"tv ignored (constant velocity)");
E 34
E 26
I 26
				nmofun = _CONNMO_;
				if (ntv) warn(__FILE__,__LINE__,"tv ignored (constant velocity)");
E 26
				if (verbose) {
/* 					fprintf(stderr,"\tconstant velocity nmo v=%g\n",v[0]); */
				}
D 25
D 34
} else {	/* NOT CONSTANT, MUST BE V(T)
E 34
I 34
} else {	/* NOT CONSTANT, MUST BE V(T)*/
E 34
E 25
I 25
} else {	/* NOT CONSTANT, MUST BE V(T)*/
E 25
				velvar = voft;
I 21
D 36
                                fprintf(stderr,"\tvariable velocity nmo\n");
E 36
E 21
D 22
D 34
				nmofun = varnmo_/*_*/;	/* <<< HP PATCH >>> */
E 34
I 34
	                         nmofun = _VARNMO_;
E 34
E 22
I 22
                                fprintf(stderr,"\tvariable velocity nmo\n");
D 23
D 25
/* 	                         nmofun = varnm_; */
E 25
I 25
D 26
	                         nmofun = varnmo_;
E 26
I 26
	                         nmofun = _VARNMO_;
E 26
E 25
E 23
I 23
D 24
	                         nmofun = varnm_;
E 24
I 24
	                         nmofun = varnmo_;
E 24
E 23
                                fprintf(stderr,"\tvariable velocity nmo\n");
E 22
I 21
                                fprintf(stderr,"\tvariable velocity nmo\n");
E 21

				if (!ntv) {
D 26
D 34
					warn("for variable velocity, need tv=");
E 34
I 34
					warn(__FILE__,__LINE__,"for variable velocity, need tv=");
E 34
E 26
I 26
					warn(__FILE__,__LINE__,"for variable velocity, need tv=");
E 26
					selfdoc();
				} else if (ntv != nv) {
D 26
D 34
					err("number of tv's (%d) and v's (%d) unequal",ntv, nv);
E 34
I 34
					err(__FILE__,__LINE__,"number of tv's (%d) and v's (%d) unequal",ntv, nv);
E 34
E 26
I 26
					err(__FILE__,__LINE__,"number of tv's (%d) and v's (%d) unequal",ntv, nv);
E 26
				}
				if (verbose) {
					fprintf(stderr,"\tvertically varying velocity nmo\n");
					for (j = 0; j < nv; j++) {
D 12
/* 						fprintf(stderr,"\ttv=%.2f\tv=%g\n",tv[j],v[j]); */
E 12
I 12
/* 					fprintf(stderr,"\ttv=%.2f\tv=%g\n",tv[j],v[j]); */
E 12
					}
				}
D 8
				/* Normalize v and tv so that dt = 1 */
E 8
I 8
				/* Normalize vel and tv so that dt = 1 */
E 8
				for (j=0;j<nv;j++) {
D 14
					v[j] *= dt;
E 14
I 14
					vel[j] *= dt;
E 14
					itv[j] = tv[j]/dt;
				}

				/* Interpolate velocity model */
D 14
				vintrp(vvec,nt,v,itv,nv);
E 14
I 14
				vintrp(vvec,nt,vel,itv,nv);
E 14

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
D 36
			} else if(vxtcmp[j]>atr->cdp) {	/* Passed */
E 36
I 36
/* 			} else if(vxtcmp[j]>atr->cdp) {	 Passed */ 
			} else if(vxtcmp[j]!=atr->cdp) {	/* Passed */
E 36
				hit = false;
				break;
			}
		}

D 26
D 34
		if(j<0||j>ncmp) err("Illigal j=%d",j);
E 34
I 34
		if(j<0||j>ncmp) err(__FILE__,__LINE__,"Illigal j=%d",j);
E 34
E 26
I 26
		if(j<0||j>ncmp) err(__FILE__,__LINE__,"Illigal j=%d",j);
E 26

/* 		fprintf(stderr,"j=%d hit=%d,ncmp=%d\n",j,hit,ncmp); */
/* 		fprintf(stderr,"vxt[%d].cmp=%d atr->cdp=%d vxt[%d].cmp=%d\n",j-1,vxt[j-1].cmp,atr->cdp,j,vxt[j].cmp); */

		/* Now that j is known, find the sloth for the cdp */
		if(hit) {		/* exact */
D 12
/*                  fprintf(stderr,"i am before copy\n"); */
E 12
			bcopy(vtv[j].v,ovv,nt*4);
D 12
/*                  fprintf(stderr,"i am after copy\n"); */
E 12
		} else if(j==0) {	/* end */
			bcopy(vtv[0].v,ovv,nt*4);
		} else if (j==ncmp) {	/* end */
			bcopy(vtv[ncmp-1].v,ovv,nt*4);
		} else {		/* linearly interpolate */
D 12
/*                        fprintf(stderr,"i am after intrp\n"); */
E 12
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
D 26
D 34
		setgain_/*_*/(tnmo,gain,&nt);	/* <<< HP PATCH >>> */
E 34
I 34
		_SETGAIN_(tnmo,gain,&nt);
E 34
E 26
I 26
		_SETGAIN_(tnmo,gain,&nt);
E 26
		hardmute(gain,nt,os);

		for(j=0;j<nt;j+=nt/20)
/* 			fprintf(stderr,"tnmo[%d]=%g\tgain=%g\n",j,tnmo[j],gain[j]); */

		/* Calculate the interpolation coefficients */
		(*trpfun)(itnmo, w, tnmo, &nt, &nw,gain);
D 26
D 34
		trpends_/*_*/(itnmo,w,&t0,&te,&nt,&nw);	/* <<< HP PATCH >>> */
E 34
I 34
		_TRPENDS_(itnmo,w,&t0,&te,&nt,&nw);
E 34
E 26
I 26
		_TRPENDS_(itnmo,w,&t0,&te,&nt,&nw);
E 26
	}

	/* Perform the NMO; put new data into nmoed */
D 26
D 34
	strtch_/*_*/(nmoed, atr->data, &nt, &t0, &te, itnmo, w, &nw);	/* <<< HP PATCH >>> */
E 34
I 34
	_STRTCH_(nmoed, atr->data, &nt, &t0, &te, itnmo, w, &nw);	
E 34
E 26
I 26
	_STRTCH_(nmoed, atr->data, &nt, &t0, &te, itnmo, w, &nw);	
E 26

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
I 36
        SccsId = lSccsId;

E 36

	/* GET OPTIONS */
	while( (c=getopt(xargc,xargv,"v"))!=EOF) {
		switch(c) {
		case 'v':
			verbose = true;
			break;
		case '?':
D 26
D 34
			warn("getopt returned '?'");
E 34
I 34
			warn(__FILE__,__LINE__,"getopt returned '?'");
E 34
E 26
I 26
D 27
			warn(__FILE__,LINE__,"getopt returned '?'");
E 27
I 27
			warn(__FILE__,__LINE__,"getopt returned '?'");
E 27
E 26
		}
	}

	/* GET PARAMETERS */
	stretch = 1.5; fgetpar("stretch", &stretch);
}
I 32
D 34

E 34
postp(){}
I 34
D 40

E 40
E 34
E 32
E 1
