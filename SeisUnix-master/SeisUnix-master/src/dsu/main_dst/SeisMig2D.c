/*
*	SeisMig2D.c
*
*	Main Program (Data distribution)
*
*	09  Sep 1994  Murillo: C
*/

#include        "SeisMig.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                 ",
" SeisMig2D - Depth Migration 			  ",
"                                                 ",
" SeisMig2D [optional parameters] > out_data_file         ",
"                                                 ",
" Creates a common offset su data file with up to four spikes   ",
" for impulse response studies                    ",
"                                                 ",
" Optional parameters:                            ",
"       nt=64           number of time samples    ",
"       ntr=32          number of traces          ",
"       dt=0.004        time sample rate in seconds             ",
"       offset=400      offset                    ",
"       nspk=4          number of spikes          ",
"       ix1= ntr/4      trace number (from left) for spike #1   ",
"       it1= nt/4       time sample to spike #1         ",
"       ix2 = ntr/4     trace for spike #2        ",
"       it2 = 3*nt/4    time for spike #2         ",
"       ix3 = 3*ntr/4;  trace for spike #3        ",
"       it3 = nt/4;     time for spike #3         ",
"       ix4 = 3*ntr/4;  trace for spike #4        ",
"       it4 = 3*nt/4;   time for spike #4         ",
"                                                 ",
NULL};

segy tr;
void put_a_step(float **q, int nx, int nz, int nt);

main(int argc, char *argv[])
{
	int  SeisMig2D(int, char **); /* Internally used */
        struct timeval	tm1, tm2;
	clock_t		clk1, clk2;
        int   		tt;
        int   		i, j, INFO, apl_tid;


/* time checking  */

/* Removed when added to SU  : 9/19/95

	CleanLog();

*/
	
        gettimeofday(&tm1, (struct timezone*)0);
	clk1 = clock();

       if ((apl_tid = pvm_mytid()) < 0) {
                pvm_perror("Error enrolling");
                return(EXIT_FAILURE);
        } 
	fprintf(stderr, "Starting DST_MIG  Application \n");


/* CODE TO PROCESS THE DATA HERE */

	INFO = SeisMig2D(argc, argv);

	clk2 = clock();
        gettimeofday(&tm2, (struct timezone*)0);

        tt = (tm2.tv_sec - tm1.tv_sec);
        fprintf(stderr, "\n\tTiempo Total del ciclo <%d> Seg. \n\n", tt);
        tt = (clk2 - clk1) / CLOCKS_PER_SEC;
        fprintf(stderr, "\n\tTotal CPU time <%d> Seg. \n\n", tt);

	/* VERY IMPORTANT */

	return EXIT_SUCCESS;

} /* End of Main program */

int SeisMig2D(int argc, char *argv[])
{
	int nt,nx,nz,nw,ntpad,ntfft;
	int it,ip,ix,iz, iw;
	int nfreqs,verbose;

	float dt,dx,dy,dz,dw;
	float freqs[4],fw,w,scale,fftscl;
	float *p, **v, *vx, **q;
	complex **cp,*cpx;

	int iw0, iw1, iw2, iw3;

	char *efile="",*vfile="";
	FILE *infp=stdin,*outfp=stdout,*efp,*vfp;
	int  VELFILE;

	void *TabInfo;
 	int  TabSize, *sizes;

        int     i, j, sz, pz;

	int 	*SeisSlaves;

        int     SeisIntPars[20];
        float   SeisFloPars[20];

        struct timeval	tm1, tm2;
        int   		tt;

	/* hook up getpar to handle the parameters */
	initargs(argc, argv);
/*
	requestdoc(1);
*/
	
	/* get required parameters */
	if (!getparstring("efile",&efile)) err("must specify efile!\n");
	if (!getparint("nt", &nt))  err("must specify nt!\n");
	if (!getparint("nx",&nx)) err("must specify nx!\n");
	if (!getparint("nz",&nz)) err("must specify nz!\n");
	
	/* get optional parameters */
	VELFILE = 1;
	if (!getparstring("vfile",&vfile)) VELFILE = 0;
	if (!getparfloat("dt",&dt)) dt = 1.0;
	if (!getparfloat("dx",&dx)) dx = 1.0;
	if (!getparfloat("dz",&dz)) dz = 1.0;
	if (!getparfloat("dy",&dy)) dy = 1.0;
	if (!getparint("ntpad",&ntpad)) ntpad=nt;
	if (!getparint("verbose",&verbose)) verbose = 0;
	if ((nfreqs=getparfloat("freqs",freqs))==0) {
		freqs[0] = 0.0;
		freqs[1] = 0.0;
		freqs[2] = 0.5/dt;
		freqs[3] = 0.5/dt;
	} else if (nfreqs!=4) {
		err("less than 4 freqs specified!\n");
	}

/* get the number of DEPTHS/PROCESS desired */
	if (!getparint("pz",&pz)) pz = 5;
	sz = nz / pz;
	sizes = alloc1int(pz);
	for (i = 0; i < pz; i++) sizes[i] = sz;
	sizes[pz - 1] += nz % pz;

/* Create "pz" SeisExtrap2D processes */
	
	fprintf(stderr, "Starting applications ... ");

	SeisSlaves = alloc1int(pz);
	if ( CreatePipe(SeisSlaves , "SeisPipe2D", pz) != 0) {
	  fprintf(stderr, "Error creating applications \n");
	  return(EXIT_FAILURE);
	}
	fprintf(stderr, " Ready (%d)\n", pz);

/* Pack the parameters to send them to each process */

	SeisIntPars[0] = nt;
	SeisIntPars[1] = nx;
	SeisIntPars[2] = nz;
	SeisIntPars[3] = ntpad;
	SeisIntPars[4] = verbose;
	SeisIntPars[5] = sz;
	SeisIntPars[6] = pz;

	SeisFloPars[0] = dt;
	SeisFloPars[1] = dx;
	SeisFloPars[2] = dz;

	SeisFloPars[3] = freqs[0];
	SeisFloPars[4] = freqs[1];
	SeisFloPars[5] = freqs[2];
	SeisFloPars[6] = freqs[3];


	
	fprintf(stderr, "Sending parameters ... ");

	TabInfo = GetAll (efile, &TabSize);
	BroadBytes(TabInfo, TabSize, SeisSlaves, pz,  MsgTable);
	BroadFI(SeisFloPars, 10, SeisIntPars, 10, SeisSlaves, pz,  MsgPars);

	fprintf(stderr, " Ready \n");

	fprintf(stderr, "Reading and Sending Velocity info ... ");

        vx = alloc1float(nx);

	if (VELFILE) {

          v = alloc2float(nz, nx);

	  /* Open the velocity file */

	  if ((vfp=fopen(vfile,"r"))==NULL) {
            fprintf(stderr, "Error opening vfile=%s\n",vfile);
	    return(EXIT_FAILURE);
          }
	
         if (fread(v[0],sizeof(float),nz*nx,vfp)!=nz*nx)
              err("Error reading velocity file\n");
	    
	  for (ip=0; ip<pz; ++ip)
	    for (iz=0; iz<sizes[ip]; ++iz) {
	      for (ix=0; ix<nx; ++ix)
                  vx[ix] = v[ix][iz];
	      SendFloat(vx, nx, SeisSlaves[ip], MsgVel);
            }

	  free2float(v);

	} else {

	  for (ix=0; ix<nx; ++ix)
            vx[ix] = 2.0; /* Constant velocity */

	  for (ip=0; ip<pz; ++ip)
	    for (iz=0; iz<sizes[ip]; ++iz)
	      SendFloat(vx, nx, SeisSlaves[ip], MsgVel);

	} /* End else */

	free1float(vx);

	fprintf(stderr, " Ready \n");

	/* Read and FFT the traces to the corresponding procesor */

	fprintf(stderr, "FFT Trace info ... ");

	ntfft = npfar(nt + ntpad);
	nw = ntfft/2+1;
        dw = 2.0*PI/(ntfft*dt);

/* AEM 9/19/95
        p = alloc1float(ntfft);
*/
        cp = alloc2complex(nw,nx);

	for (ix=0; ix<nx; ++ix) {
            if ( !gettr(&tr) )
              err("Error reading trace number (%d,%d)\n",ix+1);
	
	    p = tr. data;

	    for (it=nt; it<ntfft; ++it)
               p[it] = 0.0;
            pfarc(1,ntfft,p,cp[ix]);
	  }
/*
            if (fread(p,sizeof(float),nt,infp)!=nt)
              err("Error reading trace number (%d,%d)\n",ix+1);

	free1float(p);
*/
	fprintf(stderr, " Ready \n");

	/* Apply some pre-processing */

	/* determine sample indices for frequency filter */
	iw0 = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[0]/dw)));
        iw1 = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[1]/dw)));
        iw2 = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[2]/dw)));
        iw3 = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[3]/dw)));

        /* apply frequency filter and FFT scaling */
        for (iw=0; iw<nw; ++iw) {
                  fftscl = (iw==0 || iw==nw-1 ? 1.0/ntfft : 2.0/ntfft);
                  if (iw<iw0)
                        scale = 0.0;
                else if (iw0<=iw && iw<iw1)
                        scale = (float)(iw-iw0)/(float)(iw1-iw0)*fftscl;
                else if (iw1<=iw && iw<=iw2)
                        scale = fftscl;
                else if (iw2<iw && iw<=iw3)
                        scale = (float)(iw3-iw)/(float)(iw3-iw2)*fftscl;
                else
                        scale = 0.0;
                for (ix=0; ix<nx; ++ix) {
                        cp[ix][iw].r *= scale;
                        cp[ix][iw].i *= scale;
                }
	}

	/* fprintf(stderr, "Sending FFT Trace info ... "); */

        cpx = alloc1complex(nx);
	for (iw=0; iw<nw; ++iw) {

	    if ( !(iw % 50) )
                fprintf(stderr, "Sending FFT slice # %d \n", iw);

	    for (ix=0; ix<nx; ++ix)
                        cpx[ix] = cp[ix][iw];

	    SendCplx(cpx, nx, SeisSlaves[0], MsgSlice);

        }
	/* fprintf(stderr, " Ready \n"); */

	free1complex(cpx);
	free2complex(cp);

/* allocate workspace to receive the data */
	p = alloc1float(nx);
	q = alloc2float(nz, nx);
	if ( (q == NULL) || (p == NULL) )
		fprintf(stderr, "Error allocating space to receive output\n");

        memset( (void *) q[0], (float) '\0', nz * nx * FSIZE);
/* Receive migrated data */

	fprintf(stderr, "Waiting for the output \n");
	fflush(stderr);
        gettimeofday(&tm1, (struct timezone*)0);

	for (ip = 0; ip < pz; ++ip) {

	  for (i = 0; i < sizes[ip]; ++i) {

            RecvFI(p, nx, &iz, 1, -1, MsgDepth);

	    for (ix = 0; ix < nx; ++ix) 
	        q[ix][iz] = p[ix];
	  }

          gettimeofday(&tm2, (struct timezone*)0);
          tt = (tm2.tv_sec - tm1.tv_sec);
	  fprintf(stderr, "Depth steps from process (%3d) received After %6d Sec.\n", 
				ip, tt);
          put_a_step(q, nx, nz, nt);
	}

	/* free workspace */
	free1float(p);
	free2float(q);
	
	return(0);
} /* End of SeisMig2D() */

/* Procedure to write migrated data */

void put_a_step(float **q, int nx, int nz, int nt)
{
  int ix, iz;

  tr.ns = nz;  /* Used by the displaying program */
  tr.ntr = nx;
  tr.dt = 0.004*1000000;
  tr.trid = TREAL;

  for (ix = 0; ix < nx; ++ix) {

    memset( (void *) tr.data, (int) '\0', nt * FSIZE);

    for (iz = 0; iz < nz; ++iz) 
        tr.data[iz] = q[ix][iz];
	  
    tr.tracl = ix + 1;

    fprintf(stderr, "Sending trace number %d\n", tr.tracl);
    fprintf(stderr, "Total Trace number %d\n", tr.ntr);
    fprintf(stderr, "Trace size (ns) %d\n", tr.ns);
    puttr(&tr);

  }

}
