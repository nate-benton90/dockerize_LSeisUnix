/*
*	SeisPipe2D.c
*
*	SeisPipe2D slave process
*
*	09  Oct 1994  Murillo: c
*/

#include 	"SeisMig.h"

#define LCF 5        /* Length (same for x and y) of McClellan cosine filter */
#define LCF2 2       /* Padding DATA */
#define C1 0.0255199 /* used to improve circular symmetry of McClellan filter */

void etextrap (eTable *et, int nx, float wdxov[], complex q[]);
		
char *sdoc[] ={NULL};

main(int argc, char *argv[])
{
	int  SeisPipe2D	(DsuTask *zz);
        struct timeval	tm1, tm2;
	clock_t		clk1, clk2;
        int   		tt;
        int   		i, j, INFO;

	DsuTask 	zz;

/* time checking */
        gettimeofday(&tm1, (struct timezone*)0);
	clk1 = clock();

	InitLog(&zz, "SeisPipe2D");

	/* pvm_setopt(PvmRoute, PvmRouteDirect); */

	fprintf(zz.fp_log, "Starting SeisPipe2D Application \n" );
	fflush(zz.fp_log);

/* CODE TO PROCESS THE DATA HERE */

	INFO = SeisPipe2D(&zz);

	clk2 = clock();
        gettimeofday(&tm2, (struct timezone*)0);

        tt = (tm2.tv_sec - tm1.tv_sec);
        fprintf(zz.fp_log,"\n\tTiempo Total del ciclo <%d> Seg. \n\n", tt);
        tt = (clk2 - clk1) / CLOCKS_PER_SEC;
        fprintf(zz.fp_log,"\n\tTotal CPU time <%d> Seg. \n\n", tt);
	
	fclose(zz.fp_log);

} /* End of main */


int SeisPipe2D(DsuTask *zz)
{

  int nt,nx,nz,nw,ntpad,ntfft;
  int it,ix,iz,izz,iw,iw0,iw1,iw2,iw3,iwmin,iwmax;
  int nfreqs,verbose;

  float dt,dx,dy,dz,dw;
  float freqs[4],fw,w,scale,fftscl;
  float *p, **v, *wdxov,*sx;

  complex *cpx;
  float **qx;

  void *TabInfo;
  eTable *et;


  char 	msg[80];

  int   info, ToTid, MasterTid;
  int   sz, pz, pei;

  int	SeisIntPars[20];
  float	SeisFloPars[20];


/* Receive process control information */
  MsgLog(zz, "Receiving Control info  ... " );

  MasterTid = RecvInt(SeisIntPars, 2, -1, MsgCntl); 
  pei = SeisIntPars[0];
  ToTid = SeisIntPars[1];

  MsgLog(zz, " Ready  \n");
  
/*  Receive: 	efile and other pars ...  */

  MsgLog(zz, "Receiving parameters ..." );

  TabInfo = RecvBytes(-1, MsgTable);
  RecvFI(SeisFloPars, 10, SeisIntPars, 10, -1, -1);

  MsgLog(zz, " Ready  \n" );

/* get integer parameters */

  nt = SeisIntPars[0];
  nx = SeisIntPars[1];
  nz = SeisIntPars[2];
  ntpad = SeisIntPars[3];
  verbose = SeisIntPars[4];
  sz = SeisIntPars[5];
  pz = SeisIntPars[6];

  	
/* get Floating point parameters */
  dt = SeisFloPars[0];
  dx = SeisFloPars[1];
  dz = SeisFloPars[2];

  freqs[0] = SeisFloPars[3];
  freqs[1] = SeisFloPars[4];
  freqs[2] = SeisFloPars[5];
  freqs[3] = SeisFloPars[6];

  sz = nz / pz;
  if (pei == (pz - 1)) sz += nz % pz;

  sprintf(msg, "Receiving Velocity info (pei = %d, sz = %d) ... ", pei, sz);
  MsgLog(zz,msg);
  
  v   = alloc2float(nx, sz);

  for (iz=0; iz<sz; ++iz)
    RecvFloat(v[iz], nx, -1, MsgVel);

  MsgLog(zz, " Ready  \n" );

/* determine frequency w sampling */
  ntfft = npfar(nt+ntpad);
  nw = ntfft/2+1;
  dw = 2.0*PI/(ntfft*dt);
  iwmin = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[0]/dw)));
  iwmax = MAX(0,MIN(nw-1,NINT(2.0*PI*freqs[3]/dw)));
	
/* read extrapolator table */
  et = ezread(TabInfo);
  /* pret(zz -> fp_log, et); */
	
/* allocate workspace */

  MsgLog(zz, "Allocating space ... ");

  qx = alloc2float(nx,sz);
  sx = alloc1float(nx);
  wdxov = alloc1float(nx);
  cpx = alloc1complex(nx);
 
  MsgLog(zz, " Ready \n");

  sprintf(msg, "Process (%d) starting loop on depth steps(%d,%d)\n", 
			pei, pei*(nz/pz), pei*(nz/pz) + sz);
  MsgLog(zz, msg);

  /*  Cleanup qx */

  for (iz=0; iz<sz; ++iz)
          for (ix=0; ix<nx; ++ix)
                  qx[iz][ix] = 0.0;

  /* loop over frequencies w */
  for (iw=iwmin,w=iwmin*dw; iw<iwmax; ++iw,w+=dw) {

    if (verbose && !(iw%1)) {
      sprintf(msg, "\t%d/%d\n",iw,iwmax);
      MsgLog(zz, msg);
    }

    /* load wavefield */
    RecvCplx(cpx, nx, -1, MsgSlice);

    /* loop over depth steps nz */

    for (iz=0; iz<sz; ++iz) {

        /* compute 2.0*dx/v(x) and zero migrated data */
        for (ix=0; ix<nx; ix++)
                  sx[ix] = 2.0*dx/v[iz][ix];

        /* make w*dx/v(x) */
        for (ix=0; ix<nx; ++ix)
                wdxov[ix] = w*sx[ix];

        /* extrapolate wavefield */
        etextrap(et,nx,wdxov,cpx);

        /* accumulate migrated data */
        for (ix=0; ix<nx; ++ix)
                qx[iz][ix] += cpx[ix].r;
    }
    /* Send down the wavefield */
    if ( pei != (pz -1))
            SendCplx(cpx, nx, ToTid, MsgSlice);

  } /* End of loop for iw */

  for (iz=0; iz<sz; iz++) {

    izz = pei*(nz/pz) + iz;
    if (verbose) {
      sprintf(msg, "Sending values for iz = %d\n",izz);
      MsgLog(zz, msg);
    }

    SendFI(qx[iz], nx, &izz, 1, MasterTid, MsgDepth);
  }

  sprintf(msg, "End of processing  for (%d)\n",pei);
  MsgLog(zz, msg);

  /* free workspace */
  free1float(sx);
  free1float(wdxov);
  free2float(v);
  free2float(qx);
  free1complex(cpx);

  pvm_exit();
  return(0);
}

void etextrap (eTable *et, int nx, float wdxov[], complex q[])
/*****************************************************************************
Extrapolation for one frequency via an extrapolator table
******************************************************************************
Input:
et		pointer to extrapolator table
nx		number of samples in wavefield
wdxov		array[nx] of frequency*dx/velocity values
q		array[nx] containing current wavefield

Output:
q		array[nx] containing extrapolated wavefield
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 02/19/90
******************************************************************************/
{
	int ix,je,iwdxov,ne,nxpad,
		nwdxov=et->nwdxov,*nh=et->nh,nhmax=et->nhmax;
	float phase,*pr,*pi,spr,spi,qr,qi,er,ei,wdxovn,
		dwdxov=et->dwdxov,fwdxov=et->fwdxov,dzodx=et->dzodx;
	complex **e=et->e;
	
	/* allocate workspace */
	nxpad = nhmax-1+nx+nhmax-1;
	pr = alloc1float(nxpad);
	pi = alloc1float(nxpad);
	
	/* zero workspace and adjust pointers for zero padding */
	for (ix=0; ix<nxpad; ++ix)
		pr[ix] = pi[ix] = 0.0;
	pr += nhmax-1;
	pi += nhmax-1;
	
	/* copy input to zero-padded workspace */
	for (ix=0; ix<nx; ++ix) {
		pr[ix] = q[ix].r;
		pi[ix] = q[ix].i;
	}
		
	/* loop over output samples */
	for (ix=0; ix<nx; ix++) {
	
		/* compute pointer to extrapolator coefficients */
		wdxovn = (wdxov[ix]-fwdxov)/dwdxov;
		iwdxov = NINT(wdxovn);
		if (iwdxov<0) iwdxov = 0;
		if (iwdxov>=nwdxov) iwdxov = nwdxov-1;
		ne = nh[iwdxov];
		
		/* extrapolate with focusing term */
		spr = pr[ix];  spi = pi[ix];
		er = e[iwdxov][0].r;  ei = e[iwdxov][0].i;
		qr = er*spr-ei*spi;  qi = er*spi+ei*spr;
		for (je=1; je<ne; ++je) {
			spr = pr[ix+je]+pr[ix-je];
			spi = pi[ix+je]+pi[ix-je];
			er = e[iwdxov][je].r;
			ei = e[iwdxov][je].i;
			qr += er*spr-ei*spi;
			qi += er*spi+ei*spr;
		}
		
		/* extrapolate with shifting term */
  		phase = -dzodx*wdxov[ix];        
		er = cos(phase);  ei = sin(phase);
		q[ix].r = qr*er-qi*ei;
		q[ix].i = qr*ei+qi*er;
	}
	
	/* adjust workspace pointers and free workspace */
	pr -= nhmax-1;
	pi -= nhmax-1;
	free1float(pr);
	free1float(pi);
}
