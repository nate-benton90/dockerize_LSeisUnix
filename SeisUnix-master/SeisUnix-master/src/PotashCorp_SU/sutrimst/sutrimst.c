#include "suhdr.h"
#include "segy.h"
#include "header.h"
#include <signal.h>
#include "taup.h"
#include "math.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                       ",
" sutrimst - cross-correlation statics                          	",
"                                                                       ",
"    sutrimst <infile >outfile  [optional parameters]                 	",
"                                                                       ",
" Optional Parameters:                                                  ",
"				                                	",
" dt=tr.dt (from header) 	time sampling interval (secs)           ",
" nx=ntr   (counted from data)	number of horizontal samples (traces)	",
" tmin=0.1			minimum time in correlation window (s)  ",
" tmax=1.1			maximum time in correlation window (s)  ",
" maxshft=0.010			maximum allowable time shift  (s)       ",
" a=0				if the computed shift is larger than    ",
"                               maxshft and a=0 no time shift is performed ",
"				if a=1 maxshft time shift is applied    ",
"								",
" nit=3				Number of iteration to do on the gather ",
"                                                                       ",
"                                                                       ",
"								",
" tmpdir= 	 if non-empty, use the value as a directory path",
"		 prefix for storing temporary files; else if the",
"	         the CWP_TMPDIR environment variable is set use	",
"	         its value for the path; else use tmpfile()	",
"								",
" The cross-correlation value between the supertrace		",
" and the individual traces * 1000 is stored in header word corr",
"								",
"								",
"								",
NULL};

/* Globals (so can trap signal) defining temporary disk files */
char tracefile[BUFSIZ];	/* filename for the file of traces	*/
char headerfile[BUFSIZ];/* filename for the file of headers	*/
FILE *tracefp;		/* fp for trace storage file		*/
FILE *headerfp;		/* fp for header storage file		*/
static void closefiles(void);
int max (float *trace, int mode, float perc, int nt);

segy tr;

int
main(int argc, char **argv)
{
	int ix,it;		/* loop counters */
	int ntr;		/* number of input traces */
	int nt;			/* number of time samples */
	float dt;               /* Time sample interval */
        float tmin;             /* Minimum time in correlation window */
        float tmax;             /* Maximum  time in correlation window */
        float maxshft;          /* Maximum shift */
	float **traces;		/* array[nx][nt] of output traces */	
	float *sptr;		/* super trace */
	float *xtr;		/* correlaton window */
	int ntx;		/* correlation window size in samples */
	int lag=0;
	int maxlag;
	int a;
	int verbose;		/* flag for echoing information */
	char *tmpdir;		/* directory path for tmp files */
	float *xcorv;		/* x correlation value */
	float norm_str;
	float norm_tr;
	int nit,iter;
	cwp_Bool istmpdir=FALSE ;/* true for user-given path */
	
        /* hook up getpar to handle the parameters */
        initargs(argc,argv);
        requestdoc(1);

	if (!getparint("verbose", &verbose))	verbose = 0;
	if (!getparint("a", &a))		a = 0;

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
        if (!getparfloat("tmin",&tmin))		tmin = 0.1;
        if (!getparfloat("tmax",&tmax))		tmax = 1.1;
        if (!getparfloat("maxshft",&maxshft))	maxshft = 0.010;
	if (!getparfloat("dt",&dt))		dt = dt;
	if (!getparint("nit",&nit))		nit = 3;
	
	if (dt == 0.0)
		err("header field dt not set, must be getparred");
	
	/* Constans */
	ntx = NINT((tmax-tmin)/dt);
	maxlag = NINT(maxshft/dt);
	
	/* allocate space */
       	traces = alloc2float(nt, ntr);
        sptr = alloc1float(nt);
	xtr = ealloc1float(ntx);
	xcorv = ealloc1float(ntr);
	
	/* load traces into an array and close temp file */
	erewind(headerfp);
        erewind(tracefp);
	
	memset( (void *) traces[0], (int) '\0', nt*ntr*FSIZE);
	
        for (ix=0; ix<ntr; ix++)
                fread (traces[ix], FSIZE, nt, tracefp);
        efclose (tracefp);
	if (istmpdir) eremove(tracefile);

	/* This is for testing */
/*        for (ix=0; ix<ntr; ix++) {
		lag=0;
       		if(ix==3) lag=-10;
		fread (&traces[ix][lag], FSIZE, nt, tracefp);
		fprintf(stderr," %d\n",lag);
	}
	efclose (tracefp);
	if (istmpdir) eremove(tracefile);
*/
	
	for(iter=0;iter<nit;iter++) {
	
		/* Stack a super trace */ 
		memset( (void *) sptr, (int) '\0', nt*FSIZE);
		{ int *nnz;
	
			nnz=ealloc1int(nt);
			memset( (void *) nnz, (int) '\0', nt*FSIZE);
	
			for(ix=0; ix<ntr; ix++) {
				for(it=0; it<nt; it++) 
					if(!CLOSETO(traces[ix][it],0.0)) {
						sptr[it] += traces[ix][it];
						nnz[it]+=1;
					}
        		}
		
			/* Normalize */
			for(it=0; it<nt; it++) 
				if(nnz[it])
					sptr[it]/=(float)nnz[it];
			free1int(nnz);
		}
	
		norm_str=0.0;
		for(it=0; it<nt; it++) 
			norm_str+=SQR(sptr[it]);
		norm_str = sqrt(norm_str);
	
	
	
		/* Compute the shifts and apply them */
		erewind(headerfp);
		for(ix=0; ix<ntr; ix++) {
		
			memset( (void *) tr.data, (int) '\0', nt*FSIZE);
		
			/* do the cross correlation */
			/* xcor(nt,0,sptr,nt,0,traces[ix],ntx,-ntx/2,xtr); */
			do_facor(sptr,traces[ix],xtr,nt,-ntx/2,ntx);
		
			norm_tr=0.0;
			for(it=0; it<nt; it++) 
				norm_tr+=SQR(traces[ix][it]);
			norm_tr = sqrt(norm_tr);

			/* pick the maximum */
                	lag = -isamaxs(ntx,xtr,1)+ntx/2-1;
			xcorv[ix] = xtr[isamaxs(ntx,xtr,1)]/(norm_tr*norm_str);
	
                	/* do not exceed maxshift */
                	if(abs(lag) > maxlag) {
				lag= lag/abs(lag)*maxlag;
				if(a==0) lag=0;
			}	
		

			/* apply the shift */
			if ( lag <= 0) {
				memmove((void*) &(traces[ix])[0], (const void *)
					&(traces[ix])[abs(lag)],
				(size_t) (nt-abs(lag))*FSIZE);
			} else {
				memmove((void*) &(traces[ix])[lag], (const void *)
				&(traces)[ix][0],
				(size_t) (nt-lag)*FSIZE);
			}
			  
		}
	}
			
	for(ix=0; ix<ntr; ix++) {
		memcpy((void*) &(tr.data)[0], (const void *)
		       	&(traces[ix])[0],nt*FSIZE);
		/* add header */
		efread(&tr, 1, HDRBYTES, headerfp);
		tr.corr=NINT(xcorv[ix]*1000.0);
		puttr(&tr);
	}
	
	
	
	efclose(headerfp);
	if (istmpdir) eremove(headerfile);

	/* free allocated space */
	free2float(traces);

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
