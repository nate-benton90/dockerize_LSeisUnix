/* Copyright (c) Colorado School of Mines, 1998.*/
/* All rights reserved.                       */

/* SUTAUP: $Revision: 1.9 $ ; $Date: 1998/03/23 17:38:30 $	*/

#include "su.h"
#include "segy.h"
#include "header.h"
#include <signal.h>
#include "taup.h"
#include "math.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                       ",
" suslstk - forward and inverse local slant stack for dip filtering	",
"                                                                       ",
"    suslstk <infile >outfile  [optional parameters]                 	",
"                                                                       ",
" Optional Parameters:                                                  ",
"				                                	",
" dt=tr.dt (from header) 	time sampling interval (secs)           ",
" nx=ntr   (counted from data)	number of horizontal samples (traces)	",
" npoints=71			number of points for rho filter		",
" pminf=-0.01			start of moveout at last trace s        ",
" pmaxf=0.01			end of moveout at last trace   s        ",
" np=25			        number of slopes for Tau-P transform	",
" nwin=5		        number of traces in spatial window	",
" fh1=100			high end frequency before taper         ",
" fh2=120			high end frequency                      ",
" prw=0.01                      prewithening                            ",
" w=0			=1	apply semblance weights			",
" s=1			=1	apply smoothing weights			",
"                               1 Gaussian smoothing                    ",
"                               2 DLSQ smoothing                        ",
" sl2=nwin                      smoothing coeff in first direction      ",
" sl1=2*nwin                    smoothing coeff in spatial direction    ",
" smbwin=0.05		        semblance window in seconds		",
" pw = 1.0		        semlance weights raised to this power   ",
"                               before application                      ",
"                                                                       ",
" verbose=0	verbose = 1 echoes information			",
"								",
" tmpdir= 	 if non-empty, use the value as a directory path",
"		 prefix for storing temporary files; else if the",
"	         the CWP_TMPDIR environment variable is set use	",
"	         its value for the path; else use tmpfile()	",
NULL};


static void closefiles(void);
void semb(int sn, float **data, int nx, int nt, float *smb);
void semb_fast(int sni, float **data, int nx, int nti, float *smb);

/* Globals (so can trap signal) defining temporary disk files */
char tracefile[BUFSIZ];	/* filename for the file of traces	*/
char headerfile[BUFSIZ];/* filename for the file of headers	*/
FILE *tracefp;		/* fp for trace storage file		*/
FILE *headerfp;		/* fp for header storage file		*/


segy tr;

int
main(int argc, char **argv)
{
	int ix,it;		/* loop counters */
	int i,j,k;
	int ntr;		/* number of input traces */
	int nt;			/* number of time samples */
	int nx;			/* number of horizontal samples */
	float dt;               /* Time sample interval */
        float dx=1;               /* horizontal sample interval */
        float pminf;             /* Minimum slope for Tau-P transform */
        float pmaxf;             /* Maximum slope for Tau-P transform */
	float dpf;		/* slope sampling interval */
	int np;			/* number of slopes for slant stack */
	int nwin;		/* spatial window length */
	int npoints;		/* number of points for rho filter */
	float **twin;		/* array[nwin][nt] of window traces */	
	float **pwin;		/* array[np][nt] of sl traces */
	int ntrw;		/* number of traces in processing window */
				/* full multiple of nwin */	
	int ist;		/* start processing from this window */
	int ntfft;
	float **traces;
	int w;			/* flag to apply semblance weights */
	int s;			/* flag to apply smoothing weights */
	int sl1;		/* length of smoothing window */
	int sl2;		/* length of smoothing window */
	float *smb;		/* semblance weights */
	double pw;
	float smbwin;
	int sn;
	float *spw;		/* array of spatial weights */
	float **out_traces;	/* array[nx][nt] of output traces */	
	int verbose;		/* flag for echoing information */
	char *tmpdir;		/* directory path for tmp files */
	cwp_Bool istmpdir=cwp_false;/* true for user-given path */
	float fh1;		/* maximum frequency before taper */
	float fh2;		/* maximum frequency */
	float prw;		/* prewithening */
	
	
        /* hook up getpar to handle the parameters */
        initargs(argc,argv);
        requestdoc(1);

	if (!getparint("verbose", &verbose))	verbose = 0;

	/* Look for user-supplied tmpdir */
	if (!getparstring("tmpdir",&tmpdir) &&
	    !(tmpdir = getenv("CWP_TMPDIR"))) tmpdir="";
	if (!STREQ(tmpdir, "") && access(tmpdir, WRITE_OK))
		err("you can't write in %s (or it doesn't exist)", tmpdir);

        /* get info from first trace */
        if (!gettr(&tr))  err("can't get first trace");
        nt = tr.ns;
        dt = (float) tr.dt/1000000.0;

        /* Store traces in tmpfile while getting a count */
	if (STREQ(tmpdir,"")) {
		tracefp = etmpfile();
		headerfp = etmpfile();
		if (verbose) warn("using tmpfile() call");
	} else { /* user-supplied tmpdir */
		char directory[BUFSIZ];
		strcpy(directory, tmpdir);
		strcpy(tracefile, temporary_filename(directory));
		strcpy(headerfile, temporary_filename(directory));
		/* Trap signals so can remove temp files */
		signal(SIGINT,  (void (*) (int)) closefiles);
		signal(SIGQUIT, (void (*) (int)) closefiles);
		signal(SIGHUP,  (void (*) (int)) closefiles);
		signal(SIGTERM, (void (*) (int)) closefiles);
		tracefp = efopen(tracefile, "w+");
		headerfp = efopen(headerfile, "w+");
      		istmpdir=cwp_true;		
		if (verbose) warn("putting temporary files in %s", directory);
	}
        ntr = 0;
        do {
                ++ntr;
                efwrite(&tr, 1, HDRBYTES, headerfp);
                efwrite(tr.data, FSIZE, nt, tracefp);
        } while (gettr(&tr));

        /* get general flags and parameters and set defaults */
        if (!getparint("np",&np))             	np = 25;
        if (!getparfloat("pminf",&pminf))	pminf = -0.01;
        if (!getparfloat("pmaxf",&pmaxf))	pmaxf = 0.01;
        if (!getparfloat("fh1",&fh1))		fh1 = 100;
        if (!getparfloat("fh2",&fh2))		fh2 = 120;
        if (!getparfloat("prw",&prw))		prw = 0.01;
	if (!getparfloat("dx",&dx))		dx = 1.0;
	if (!getparint("npoints",&npoints))	npoints = 71;
	if (!getparint("nwin",&nwin))		nwin= 5;
	if (!getparfloat("dt",&dt))		dt = dt;
	if (!getparfloat("smbwin",&smbwin))	smbwin = 0.05;
	if (!getpardouble("pw",&pw))		pw = 1.0;
	if (!getparint("w",&w))			w = 0;
	if (!getparint("s",&s))			s = 0;
	if (!getparint("sl1",&sl1))		sl1 = 2*nwin;
	if (!getparint("sl2",&sl2))		sl2 = nwin;
	
        nx = ntr;
	if (dt == 0.0)
		err("header field dt not set, must be getparred");

	/* allocate space */
	ntfft=npfar(nt);
	ntrw=nwin;
	while (ntrw < ntr) {
		ntrw+=nwin;
	}
	ist = ntrw-ntr/2;
	twin = alloc2float(nt, nwin);
	pwin = ealloc2float(ntfft,np);
        traces = alloc2float(nt, ntr);
        out_traces = alloc2float(nt, ntr);
	smb = ealloc1float(nt);
	
	/* Set up some constans*/
	dpf=(pmaxf-pminf)/(np-1);
	sn = (int)(smbwin/dt+0.5);
	if(sn%2==0) sn++;
	if(nwin%2==0) nwin++;
	
	/* spatial trace weigths */
	spw = ealloc1float(nwin);
	for(k=0,i=1;k<nwin/2+1;k++,i++) spw[k] = (float)i; 
	for(k=nwin/2+1,i=nwin/2;k<nwin;k++,i--) spw[k] = (float)i; 
/*	for(k=0,i=1;k<nwin;k++,i++) spw[k] =1.0; */
	
	
	
        /* load traces into an array and close temp file */
	erewind(headerfp);
        erewind(tracefp);
	
	memset( (void *) traces[0], (int) '\0', (nt*ntr)*FSIZE);
	memset( (void *) out_traces[0], (int) '\0', (nt*ntr)*FSIZE);
	
        for (ix=0; ix<ntr; ix++)
                fread (traces[ix], FSIZE, nt, tracefp);
        efclose (tracefp);
	if (istmpdir) eremove(tracefile);

	/* do requested operation */
	 
	for(i=0; i<ntr; i+=nwin/2) {
		memcpy( (void *) twin[0], (const void *) traces[i],
			nt*nwin*FSIZE);	

		/* compute forward slant stack */
/*		fwd_tx_sstack (dt, nt, nwin, -nwin/2*dx, dx, np, pminf, dpf,
	       		twin, pwin);
*/		forward_p_transform(nwin,nt,dt,pmaxf*1000.0,pminf*1000.0,dpf*1000.0,
                                    0.0,fh1,fh2,3.0,30.0,400,5,1,0,0,1,prw,
				    0.0,nwin*dx,1,dx,0.0,0.0,0.0,twin,pwin);
/*		fwd_FK_sstack (dt, nt, nwin, -nwin/2*dx, dx, np, pminf, dpf,0,
	       		twin, pwin);
*/		
		/* compute semplance */
		if(w==1) {
			semb(sn,pwin,np,nt,smb);
			/* apply weights */
			for(j=0;j<nt;j++)
				for(k=0;k<np;k++) pwin[k][j] *=smb[j];
		}
		if(s==1) {
			gaussian2d_smoothing (np,nt,sl2,sl1,pwin);
		}
		if(s==2) {
			dlsq_smoothing (nt,np,0,nt,0,np,sl1,sl2,0,pwin);
		}
		/* compute inverse slant stack */
/*		inv_tx_sstack (dt, nt, nwin, npoints,-nwin/2*dx, dx, np,pminf,dpf,
			pwin, twin);
*/		inverse_p_transform(nwin,nt,dt,pmaxf*1000.0,pminf*1000.0,dpf*1000.0,
                                    0.0,fh1,fh2,0.0,nwin*dx,1,dx,0.0,
				    pwin,twin);
/*		inv_FK_sstack (dt, nt, nwin,-nwin/2*dx, dx, np,pminf,dpf,0,
			pwin, twin);
*/			
		{ register int itr,it,spind;;
			for(itr=0;itr<nwin;itr++) {
				spind=i+itr;
				for(it=0;it<nt;it++) {
					if(spind>0 && spind<ntr) 
							out_traces[spind][it] += spw[itr]*twin[itr][it];
						/*	out_traces[spind][it] = twin[itr][it]; */
				}
			}
		}

/*		fprintf(stderr," Trace #= %5d\n",i); */	
        }
	/* write output traces */
        erewind(headerfp);
	{       register int itr;
		for (itr=0; itr<ntr; itr++) {
			efread(&tr, 1, HDRBYTES, headerfp);
			for (it=0; it<nt; it++) 
				tr.data[it]=out_traces[itr][it];
			puttr(&tr);
		}
	}
	efclose(headerfp);
	if (istmpdir) eremove(headerfile);

	/* free allocated space */
	free2float(out_traces);
	free1float(spw);

	return EXIT_SUCCESS;

}

/* for graceful interrupt termination */
static void closefiles(void)
{
	efclose(headerfp);
	efclose(tracefp);
	eremove(headerfile);
	eremove(tracefile);
	exit(EXIT_FAILURE);
}

void semb(int sni, float **data, int nx, int nt, float *smb)
{
     	float nsum;
     	float dsum;
     	float pwr=0.1;
     
     
     /* compute semblance quotients */
/*     for (itout=0; itout<ntout; ++itout) {
     	     it = itout*dtratio;
     	     ismin = it-nsmooth/2;
     	     ismax = it+nsmooth/2;
     	     if (ismin<0) ismin = 0;
     	     if (ismax>nt-1) ismax = nt-1;
     	     nsum = dsum = 0.0;
*/
	{ register int ix,is;
		for (is=0; is<nt; ++is) {
     	    		nsum=0.0;
			dsum=0.0;
		     	for (ix=0; ix<nx; ++ix) {
     			     nsum += data[ix][is];
     		 	     dsum += data[ix][is]*
     			    	     data[ix][is];
			}
     	    	 	dsum*=nx;
			nsum=nsum*nsum;
			
    		     	smb[is] = (dsum!=0.0?nsum/dsum:0.0);
     	 		smb[is] = pow (smb[is], pwr);
     		}
	}
	gaussian1d_smoothing (nt,nt/20,smb);
}

