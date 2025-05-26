/*     features:
		up-to 90 degree accuracy
		absorbing boundaries
		dispersion suppression
		stable
		implicit f.d. scheme
		splitting method used 
		f.d. errors compensation
		parallel computation

*/

#include "usu.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "comva.h"
#include "su.h"
#include "segy.h"
#include "header.h"
#include <sys/stat.h>

char *sdoc = 
"FXYMIG  - frequency-space domain 3-d migration			 	\n"
"\n"
"fxymig [parameters] <3d-stack >migrated-3d-data			\n" 
"\n"
"Required parameters:							\n"
"tracekey=              segy key word defining trace number within line \n"
"linekey=               segy key work defining line number 		\n"
"Or the following Four parameters defining 3D master grid:		\n"
"cdp1=                  first cdp number of 3D master grid              \n"
"cdpinc=                cdp number increment of 3D master grid          \n"
"ncdppl=                number of cdp (traces) per line of 3D master grid \n"
"nlines=                number of lines of 3D master grid		\n" 
"                       (See Note 6 for details)			\n"
"Or the following 12 3D master grid parameters:				\n" 
"x1=                    x coordinate of 1st corner of the 3D master grid\n"
"y1=                    y coordinate of 1st corner of the 3D master grid\n"
"s1=                    s coordinate of 1st corner of the 3D master grid\n"
"l1=                    l coordinate of 1st corner of the 3D master grid\n"
"x2=                    x coordinate of 2nd corner of the 3D master grid\n"
"y2=                    y coordinate of 2nd corner of the 3D master grid\n"
"s2=                    s coordinate of 2nd corner of the 3D master grid\n"
"l2=                    l coordinate of 2nd corner of the 3D master grid\n"
"x3=                    x coordinate of 3rd corner of the 3D master grid\n"
"y3=                    y coordinate of 3rd corner of the 3D master grid\n"
"s3=                    s coordinate of 3rd corner of the 3D master grid\n"
"l3=                    l coordinate of 3rd corner of the 3D master grid\n"
"                       (See Note 7 for details)			\n"
"                       If cdp1, cdpinc, ncdppl and nliness are given,  \n"
"                       s1=l1=0. 					\n"
"\n"
"ds=                    trace to trace (inline) interval to migrate	\n"
"dl=                    line to line interval to migrate (required when nl>1)\n"
"velfile=               INTERVAL velocity file name (velocity is stored as \n"
"                       v(ns,nl,nvs), i.e., nvs planes of (ns*nl))	\n"
"\n"
"Optional parameters:							\n"
"datain=                input dataset name (default to standard input) 	\n" 
"dataout=               output dataset name (default to standard output)\n" 
"cdpnum=0               cdp numbering type (0=inline then crossline)	\n" 
"                                           1=crossline then inline)	\n" 
"                       used only when cdp1, cdpinc, ncdppl and nlines  \n"
"                       are specified for 3d master grid.   		\n"
"The following two parameters define the migration grid origin:	\n"
"trstart=            	starting trace (inline) number to migrate	\n" 
"traceinc=1             trace number increment to migrate  \n"
"lnstart=            	starting line number to migrate	\n" 
"lineinc=1              line number increment to migrate 		\n"
"Or the following two parameters define the migration grid origin,	\n"
"                       when tracekey and linekey are not specified:	\n"
"sstart=s1              Minimum s (trace position within line) to migrate \n"
"                       (NOTE: this is coordinates, i.e., first trace	\n"
"                       of a line at s1, 2nd at s1+ds, 3rd at s1+2*ds, ...)\n"
"lstart=l1              Minimum l (line position) to migrate data	\n"
"                       (NOTE: this is coordinates, i.e., first line	\n"
"                       l1, 2nd at l1+dl, 3rd at l1+2*dl, ...)		\n"
"ns=ncdppl              Number of cdp (traces) per line to migrate	\n"
"                       (when ncdppl is specified, otherwise required) 	\n"
"nl=nlines              Number of lines in 3-d volume to migrate	\n"
"                       (when nlines is not specified, 1 is assumed)	\n"
"nvs=from-velfile       Number of depth (time) slices in velocity file	\n"
"dvs=from-velfile       Depth (time) slice interval in velocity file	\n"
"                       in METERS or FT if dz is NOT zero (DEPTH MIG)	\n"
"                       in MS if dz is zero (TIME MIG)			\n"
"ntau=nt                Number of output migrated samples per trace	\n" 
"dtau=dt                Output migrated time sample interval (in ms)\n"
"                       (dt is the input time sample interval from 	\n"
"                       trace header)					\n"
"dz=0.                  Output depth interval (in METERS or FT) 	\n"
"                       if not zero, each output trace will contain	\n"
"                       ntau samplles of DEPTH-migrated samples with    \n"
"                       sample interval of dz.	(DEPTH MIGRATION)	\n"
"                       if zero, each output trace will contain         \n"
"                       ntau samples of TIME-migrated samples with	\n"
"                       sample interval of dtau. (TIME MIGRATION)	\n"
"iestep=5               Extrapolation step in output samples			\n"
"iorder=1               Order of migration operator			\n"
"                       (0=45 1=65 2=80 3=87 4=89.99 5=90 degrees)	\n"
"nfft=trace_length*1.5  fft length of trace 				\n"
"dfc=0.03               Dip-filtering constant (0.<=dfc<=1)		\n"
"fmin=(0.5/dt)/20       Minimum frequency to migrate (hz)		\n"
"fmax=(0.5/dt)*1/2      Maximum frequency to migrate (hz) at z=0 or t=0	\n"
"fmaxend=fmax           Maximum frequency to migrate (hz) at tzend \n"
"tzend=99999            Maximum time (in ms) or maximum depth (in m or ft) \n"
"                       where fmaxend is specified \n"
"icstep=25              F.D. error compensation step in output samples (0=no)\n"
"nffs=ns*1.5            Fft length of inline direction when icstep>0	\n"
"nffl=nl*1.5            Fft length of crossline direction when icstep>0	\n"
"lvec=                  Vectorization length (computed internally)	\n"
"                       for 3D migration, must be the maximum of ns and nl \n"
"                       for 2D migration, must be less than or equal to	\n"
"                       nfft/2						\n"
"mlimit=512             Memory limit (in megabytes) to run the program  \n"
"isave=0                Mode of saving disk datasets of wavefield, 	\n"
"                       trace headers, and the imaged 3d volume         \n"
"                       0=remove these datasets after completion        \n" 
"                       1=save these datasets after completion          \n" 
"diskxyw=DISKWAVE       Disk name for storing wavefield			\n" 
"                       (frequency slices) during downward extrapolation \n"
"diskhdr=DISKHEADER     Disk name for storing input trace headers	\n"
"diskxyt=DISKIMAGE      Disk name for storing imaged time/depth slices  \n"
"ncpu=2                 number of CPUs to use (0<= ncpu <=32)		\n"
"durfile=               Disk update record file				\n"
"                       If specified, diskxyw and diskxyt will be updated \n"
"			             once nwmig frequencies have been downward \n"
"                        migrated nsteps. This allows restart of the job \n"
"                        from where the job was stopped by unexpected \n"
"                        system interference at the cost of more elapse time.\n"
"                        If not given (default), the disks will be backup \n"
"                        and reusable for the next step only after the \n"
"                        current job finishes normally. \n"
"nwmig=ncpu             number of frequency slices to migrate nsteps before \n"
"                        disk backup (should be integer multiple of ncpu ) \n"
"nsteps=                number of depth/time slices to hold in memory \n"
"                        recommended value 100, 200, ....    \n"
"                        (default to a number computed internally 	\n"
"                        according the memory limit)			\n" 
"jpfile=                Job print file name				\n"
"                          default: the standard error unit 		\n"
"                            (a) create prefix.exxxx file if using qsub \n"
"                                on convex, where prefix is the first 	\n"
"                                7 characters of the script file name 	\n"
"                                and xxxx is the job number;		\n"
"                            (b) send e-mail when using at command;   	\n" 
"                            (c) send printed message to screen when 	\n"
"                                running the job without qsub or at 	\n"
"                                commands.				\n" 
"                          specified: the message will be printed 	\n"
"                            in the named file as the job goes		\n" 
"backupi=               Name of backup tape name of the previous run	\n"
"                       (if given, e.g., stssxyz.fxymig.bck1, the backup \n"
"                        will be read in to overwrite the disk files	\n"
"                        diskxyw, diskhdr, and diskxyt; 		\n"
"                        otherwise, the program will use diskxyw, 	\n"
"                        diskhdr, diskxyt if they are present)		\n" 
"backupo=               Name of backup tape name of the current run	\n"
"                       (if given, e.g., stssxyz.fxymig.bck2, the backup \n"
"                        tape will be made after the job is done;	\n"
"                        otherwise, only diskxyw, diskhdr, and diskxyt 	\n"
"                        will be created if isave=1) 			\n"
"diskxywb=              Name of disk to backup diskxyw during migration \n" 
"diskhdrb=              Name of disk to backup diskhdr during migration \n"
"diskxytb=              Name of disk to backup diskxyt during migration \n"
"                       (Only when the above three disks are specified, \n"
"                       the program will backup three files every nsteps) \n"
"traceout=1             output migrated seismic trace (1=yes 0=no) \n"
"                       (traceout=0 may be used when running distributed \n"
"                       fxymig on multiple machines). Use fxymerge to   \n"
"                       output migrated traces from diskfiles of the \n"
"                       multiple machines  \n"
"ifmin=                 minimum frequency index to migrate (calulated from \n"
"                       the fxymcal program) for this job \n"
"                       (frequency 0Hz is at index 0, nyquist at nfft/2) \n"
"ifmax=                 maximum frequency index to migrate (calulated from \n"
"                       the fxymcal program) for this job \n"
"                       if both ifmin and ifmax is specified, only the \n"
"                       frequency slices specified between ifmin and ifmax \n"
"                       will be migrated in the job \n"
"maxamp=1.0e+20         maximum absolute amplitude allowed in input \n"
"                       when an amplitude of input trace is larger than \n"
"                       maxamp, the job will be canceled \n"
"minvel=500             minimum velocity allowed in input velocity \n"
"maxvel=50000           maximum velocity allowed in input velocity \n"
"                       when the input velocity is outside (minvel,maxvel) \n"
"                       the job will be canceled \n"
"                       if minvel or maxvel is specified as negative number \n"
"                       no check will be performed \n"
"\n"
"Notes:									\n"
"1. Input stack must be ASCII-IEEE SEGY data format				\n"
"2. Input interval velocity file must be transposed:			\n"
"   i.e., stored as velocity time/depth slices for time/depth migration \n"
"3. s is the lateral position along the inline direction, l is the 	\n"
"   lateral position along the crossline direction. They may be different\n"
"   from the (x,y) position of the 3D survey. 				\n"
"4. When cdp1, cdpinc, ncdppl, and nlines are specified, the cell position\n"
"   of the inpute trace is determined by its cdp number. Otherwise, the	\n"
"   source (x,y) and receiver (x,y) coordinates are used to determine	\n"
"   the midpoint x and y position on the 3D grid:			\n"
"		x = (gx+sx)/2 * scalco					\n" 
"		y = (gy+sy)/2 * scalco		when scalco>1		\n" 
"    OR:								\n"	
"		x = (gx+sx)/2 / (-scalco)	when scalco<0		\n" 
"		y = (gy+sy)/2 / (-scalco)				\n" 
"   where gx is the receiver x coordinate from the trace header 	\n"
"    	  sx is the source x coordinate from the trace header 		\n"
"         gy is the receiver y coordinate from the trace header 	\n"
"    	  sy is the source y coordinate from the trace header 		\n"
"         scalco is the scaler to be applied to gx,gy,sx,sy		\n" 
"5. To migrate the 3-D dataset in stages (so that you get backups of    \n"
"   each stage), use ntau, isave, diskxyw, diskhdr, diskxyt papameters	\n"
"   for example,							\n"
"	stage 1: extrapolate to 500-th time/depth sample		\n"
"	   fxymig ... ntau=500 isave=1 diskxyw=/huge/id/tmpxyw.data 	\n"
"	              diskhdr=/huge/id/tmphdr.data 			\n"
"                     diskxyt=/huge/id/tmpimg.data 			\n"
"                     backupo=stssxyz.fxymig.backup1 ...		\n"
"	stage 2: extrapolate to the 1000-th time/depth sample		\n"
"	   fxymig ... ntau=1000 isave=1 diskxyw=/huge/id/tmpxyw.data 	\n"
"	              diskhdr=/huge/id/tmphdr.data 			\n"
"                     diskxyt=/huge/id/tmpimg.data 			\n"
"                     backupi=stssxyz.fxymig.backup1 ...		\n"
"                     backupo=stssxyz.fxymig.backup2 ...		\n"
"	stage 3: extrapolate to the final (1500-th) time/depth sample	\n"
"	   fxymig ... ntau=1500 isave=0 diskxyw=/huge/id/tmpxyw.data 	\n"
"	              diskhdr=/huge/id/tmphdr.data 			\n"
"                     backupi=stssxyz.fxymig.backup2 ...		\n"
"6. cdp1, cdpinc, ncdppl, nlines are used to define master 3D grid:	\n"
"               							\n"
"  cdp1+(nlines-1)*ncdppl*cdpinc  cdp1+(nlines*ncdppl-1)*cdpinc		\n"
"                    *------------------*    <----------line nlines	\n"
"                    |                  |				\n"
"                    |                  |				\n"
"                    |                  |				\n"
"                    |                  |				\n"
"cdp1+2*ncdppl*cdpinc|                  |				\n"
" cdp1+ncdppl*cdpinc |                  |    <----------line 2		\n"
"                    *------------------*    <----------line 1 		\n"
"                  cdp1            cdp1+(ncdppl-1)*cdpinc 		\n"
"                                                                       \n"
" within the master 3D grid, cdp number is incremented with cdpinc, i.e.,\n"
" first line starts with cdp1, second line starts with cdp1+ncdppl*cdpinc,\n"
" etc., the last line starts with cdp1+(nlines-1)*ncdppl*cdpinc.	\n"
" Input data may have some missing traces at some cdps, but the cdp 	\n"
" numbers of those input traces must match the cdp number definition 	\n"
" shown above. The s1 and l1 values at the starting cell (cdp1) position\n"
" will then be zero.							\n"
"7. s and l positions of an input trace are computed using the three 	\n"
"   master-grid corner positions					\n" 
"									\n"
"        | y								\n"
"        |   .l        * (x4,y4)					\n"
"        |    .     .    .						\n"
"        |     .  .       .         . s					\n"
"       (x3,y3) *          .      .					\n"
"        |        .          .  .					\n"
"        |         .          * (x2,y2)					\n"
"        |          .       .						\n"
"        |            .  .						\n"
"        |             * (x1,y1)					\n"
"        |								\n"
"        |--------------------------------- x				\n"
"									\n"
"   (x1,y1) has the smallest s value and the samllest l value		\n"
"              (s1 is usually =0.0, l1 is usually =0.0)			\n"
"   (x2,y2) has the largest s value and the smallest l value		\n"
"              (s2 is usually =(ncdppl-1)*ds, l2 is usually =0.0)	\n"
"   (x3,y3) has the smallest s value and the largest l value		\n"
"              (s3 is usually =0.0, l3 is usually =(nlines-1)*dl 	\n"
"   where 								\n"
"         ncdppl is the number of traces per line in the master grid	\n"
"         ds is the trace spacing (within a line) in the master grid	\n"
"         nlines is the number of lines in the master grid		\n"
"         dl is the line spacing in the master grid 			\n"
"	  s is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of trace position within a 3d line 			\n"
"         l is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of line position within a 3d survey 			\n"
"8. There are ns by nl traces output, regardless of input traces.	\n"
"   Trace number within line (starting from 1 with increment of 1) will \n"
"   be updated in trace header word 'tracl', while line number (starting\n"
"   from 1 with increment of 1) will be updated in trace header word	\n" 
"   'tracr'. Input missing trace position will be filled with zero traces \n"
"   before migration, and the resulting migrated traces at these locations\n" 
"   will be outputed with header word 'trid' equal 2 (dead trace). Other\n"
"   output header values at these missing trace locations may not be 	\n"
"   correct, since they are simply copied from the nearest live input 	\n"
"   traces.								\n" 
"9. Memory requirement can be estimated by, 				\n"
"  ns*nl*nwmig*8+ns*nl*7*ncpu*8+ns*nl*(nsteps+4+nsteps*dtau/dvs)*4 (BYTES)\n"
"\n"
 
"AUTHOR:		Zhiming Li,        	6/92   			\n"
;

void wl2d(int nx,int ny,int nw,int iy,complex *cp,char *trhdr,float *fold,
		FILE *xywfp, FILE *hdrfp, FILE *jpfp,
		String trktype, String lnktype,
		Value trkval, Value lnkval,
		int indxtrk, int indxlnk, int trstart, int lnstart,
		int traceinc, int lineinc, int ikey, 
		complex *cpbig,int nybig, int iw2disk);

void wxymig_(int *nx,int *ny,int *ntau,int *nw,int *itau0,
		int *iestep,int *icstep,int *nvs,int *nkx,int *nky,
		int *nq,int *nv,int *ncp,int *lvec,int *lplane,int *iorder,
		int *naux1,int *naux2,int *naux,float *dt,float *dx,float *dy,
		float *dtau,float *dfc,float *vref,float *dvs,
		float *q,float *v,float *vxy,
		float *om,float *oms,float *vyx,float *w,
		float *aux1,float *aux2,
		complex *a,complex *aa,complex *r,float *cpt,
		complex *caux,complex *shift,complex *al,complex *ar,
		complex *cp,complex *bcp,complex *cpp,
		char *namexyw,char *namexyt,char *namevel,
		int *lenxyw,int *lenxyt,int *lenvel,
		complex *asave,complex *aasave,int *iisave,float *va,
		char *namejp,int *lenjpf,int *ncpu,
		char *namexywb,char *namexytb,
		int *lenxywb,int *lenxytb,
		float *qbuf,complex *cpbuf,complex *cph,float *dzov,int *ncph,
		char *namedur,int *lendur,int *itaudur,int *iwdur,
		int *ntauend,int *iwend,int *nwmig,int *nf,int *ifmin);

segytrace tra;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
	int cdp1, nlines, ncdppl, cdpinc, icdps;
   	int nt,nx,ny,ntau,nw,nfft,nfftq,it,iw,ix,iy,iorder,itau0,nsteps;
    	int iestep,icstep,nffx,nffy,naux1,naux2,naux;
    	int nvs,nv,nq,ncp,lvec,lplane,isave,iq;
    	int nkx,nky,ntmp,estep,cstep,ntrace;
	int idataxyt,idataxyw,idatahdr,jx,jy,ir,ii;
	int iwmin, iwmax;
   	float dt,dx,dy,dtau,dvs,dz,dfc,fmin,fmax,x0m,y0m,vref; 
	int ifmin, ifmax, traceout,ifrange,nf, ifminr;
	float fmaxend, tzend;
	int iwend, ntauend;
	float pi,dw,wmin,wmax,xm,ym,tmp;
	float *q,*v,*vxy,*vyx,*om,*oms,*aux1,*aux2,*va;
	float *fold,*wsave,*trfft;
   	complex *cp,*aa,*a,*r,*bcp,*caux,*shift,*al,*ar,*cpp;
	complex *cpbig;
	float *cpt;
   	char *diskxyt,*diskxyw,*diskhdr,*velfile;
   	char *diskxytb,*diskxywb,*diskhdrb;
   	char *datain,*dataout,*trhdr;
   	FILE *infp,*outfp,*velfp,*xywfp,*xytfp,*hdrfp;
   	FILE *xywbfp,*xytbfp,*hdrbfp;
	char *namevel, *namexyt, *namexyw;
	char *namexytb, *namexywb;
	int lenxywb=0, lenxytb=0;
	int lenvel, lenxyw, lenxyt, dummyunit=77;
	float x1,x2,x3,y1,y2,y3,s1,s2,s3,l1,l2,l3,sm,lm;
	complex *asave, *aasave; 
	int iisave=0;
	char *backupi, *backupo;
	int ibacki, ibacko, ibackd;
	int cdpnum;
	char *jpfile;
	char *durfile;
	char *namejp;
	char *namedur;
	int lenjpf;
	int lendur;
	int itau0dur, itaudur, iwdur;
	char tau0dur[10], taudur[9], wdur[7];
	char dump1[3], dump2[3], dump3[2], dump4[8], dump5[4];
	FILE *jpfp;
	FILE *durfp;
	int ncpu, nwmig;
	float *w;
	complex *cpbuf;
	float *qbuf, *dzov;
	complex *cph;
	int ncph=32768;
	float temp;
	int ntaulast;
	float maxamp, minvel, maxvel;
	int itau;

	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk, trstart, lnstart;
	int traceinc, lineinc, ikey=0, iline, itrace;
	float ftrace, fline;
	
	ghed gh;
	int n1,n2,n3,n4,n5,dtype,ierr;
	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5,ocdp2,oline3,dcdp2,dline3;
	float gmin,gmax,scale;
	int orient, gtype;

	int ix0, iy0, jy0;
	int nyread, nyy, iyread, ixx, iyy; 

	long long lwork, i64, l64, lmem;
	int mlimit;
	long long llimit;
    long long ixywsize,ixytsize,ihdrsize;
	
	char *envs;

	int nybig;

	struct stat sbuf;
	int itime1, itime2, itime3;


   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* open input, output and velocity data sets */
    	if (!getparstring("datain", &datain)) {
		infp = stdin;
	} else {
    		infp = fopen(datain,"r");
	}
    	if (!getparstring("dataout", &dataout)) {
		outfp = stdout;
	} else {
    		outfp = fopen(dataout,"w");
	}
    	if (!getparstring("velfile",&velfile)) err("must specify velfile");
	
    	if (!getparstring("jpfile",&jpfile)) {
		jpfp = stderr;
		lenjpf = 0;
	} else {
		lenjpf = strlen(jpfile);
		jpfp = efopen(jpfile,"w");
		namejp = (char*) emalloc(lenjpf+1);
		bcopy(jpfile,namejp,lenjpf);
		namejp[lenjpf]='\0';
	}
  	if (!getparstring("durfile",&durfile)) {
		lendur = 0;
		iwdur = 0;
		itau0dur = 0;
		itaudur = 0;
	} else {
		lendur = strlen(durfile);
		if( (durfp = fopen(durfile,"r"))!=NULL ) {
			iwdur = 0;
			itau0dur = -999;
			itaudur = 0;
			do {
				fscanf(durfp,"%s %d %s %d %s %d %s %s %s %s %s\n",
				tau0dur, &itau0dur, taudur, &itaudur, wdur, &iwdur,
				dump1,dump2,dump3,dump4,dump5);
			} while (feof(durfp)==0);
		} else {
			durfp = fopen(durfile,"w");
			iwdur = 0;
			itau0dur = -999;
			itaudur = 0;
		}
		fclose(durfp);
		namedur = (char*) emalloc(lendur+1);
		bcopy(durfile,namedur,lendur);
		namedur[lendur]='\0';
	}

	/* perform a fseek64 to force large file options in fread and fwrite */

	fseek64(infp,0,1);
	fseek64(outfp,0,1);

	/* required parameters */
  	if ( getparstring("tracekey",&tracekey) 
		&& getparstring("linekey",&linekey) ) {
		icdps = -1;
		ikey = 1;
		trktype = hdtype(tracekey);
		lnktype = hdtype(linekey);
		indxtrk = getindex(tracekey);
		indxlnk = getindex(linekey);
		s1 = 0.; l1 = 0.; 
   		if (!getparint("ns",&nx)) err(" ns missing \n");
   		if (!getparint("nl",&ny)) err(" nl missing \n");
   		if (!getparint("traceinc",&traceinc)) traceinc=1;
   		if (!getparint("lineinc",&lineinc)) lineinc=1;
   	} else if ( getparint("cdp1",&cdp1) && getparint("ncdppl",&ncdppl) &&
   	     getparint("nlines",&nlines) && getparint("cdpinc",&cdpinc) ) {
		s1 = 0.; l1 = 0.; x1 = 0.; y1 = 0.;
		s2 = 0.; l2 = 0.; x2 = 0.; y2 = 0.;
		s3 = 0.; l3 = 0.; x3 = 0.; y3 = 0.;
   		if (!getparint("ns",&nx)) nx = ncdppl;
   		if (!getparint("nl",&ny)) ny = nlines; 
		icdps = 1;
		if (cdpinc==0) cdpinc = 1;
   		if (!getparint("cdpnum",&cdpnum)) cdpnum = 0;
	} else {
		icdps = 0;
   		if (!getparfloat("x1",&x1)) err("must specify x1");
   		if (!getparfloat("y1",&y1)) err("must specify y1");
   		if (!getparfloat("s1",&s1)) err("must specify s1");
   		if (!getparfloat("l1",&l1)) err("must specify l1");
   		if (!getparfloat("x2",&x2)) err("must specify x2");
   		if (!getparfloat("y2",&y2)) err("must specify y2");
   		if (!getparfloat("s2",&s2)) err("must specify s2");
   		if (!getparfloat("l2",&l2)) err("must specify l2");
   		if (!getparfloat("x3",&x3)) err("must specify x3");
   		if (!getparfloat("y3",&y3)) err("must specify y3");
   		if (!getparfloat("s3",&s3)) err("must specify s3");
   		if (!getparfloat("l3",&l3)) err("must specify l3");
   		if (!getparint("ns",&nx)) {
			if(getparint("ncdppl",&ncdppl)) {
				nx = ncdppl;
			} else {
				err("must specify ns");
			}
		}
   		if (!getparint("nl",&ny)) {
			if(getparint("nlines",&nlines)) {
				ny = nlines;
			} else {
				ny = 1;  
			}
		}
	}
	if (!getparfloat("maxamp",&maxamp)) maxamp = 1.e+20;
	if (!getparfloat("minvel",&minvel)) minvel = 500.;
	if (!getparfloat("maxvel",&maxvel)) maxvel = 50000;

 
	if(ny==0) ny=1;  
    if (!getparfloat("ds",&dx)) err("Must specify ds!\n");
	dy = 1.;
    if (!getparfloat("dl",&dy) && ny>1) 
	err("Must specify dl for 3D migration");
	if(dy==0.) dy = 1.0;

	/* read id headers */
    fgethdr(infp,&ch,&bh);

	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tra))  err("can't get first trace");
    nt = tra.ns;
    dt = (float)tra.dt/1000000.;

	if(icdps==0 || icdps==-1 ) {
		cdp1 = tra.cdp;
		cdpinc = 1;
		ncdppl = nx;
		nlines = ny;
	}	
	/* get velociry grid header info */
	velfp = fopen(velfile,"r");
	fseek64(velfp,0,1);
	
	ierr = fgetghdr(velfp,&gh);
	if (ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,
        		&n4,&n5,&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
        		&dcdp2,&dline3,&ocdp2,&oline3,
        		&gmin,&gmax,&orient,&gtype);

	/* optional parameters */
    if (!getparint("nvs",&nvs)) {
		if(ierr==0) {
			nvs = n3;
		} else {
			err("Must specify nvs ");
		}
	}
    if (!getparfloat("dvs",&dvs)) {
		if(ierr==0) {
			dvs = d3;
		} else {
			err("Must specify dvs ");
		}
	}

    if (!getparint("ntau",&ntau)) ntau = nt;
    if (!getparfloat("dtau",&dtau)) {
		dtau = dt;
	} else {
		dtau = dtau*0.001;
	}
    if (!getparfloat("dz",&dz)) dz = 0.;
	vref = 2.*dz/dtau;
	
	if(icdps>=0) {
    		if (!getparfloat("sstart",&x0m)) x0m = s1;
    		if (!getparfloat("lstart",&y0m)) y0m = l1;
	} else {
    		if (!getparint("trstart",&trstart)) err(" trstart missing");
    		if (!getparint("lnstart",&lnstart)) err(" lnstart missing");
		    s1 = o1;
		    l1 = o2;
    		x0m = o1;
    		y0m = o2;
	}

	/* update id headers and write to output */
	bh.hns = ntau;
	bh.hdt = dtau * 1000000; 
	if(dz>0.) {
		if(dz<=32) {
			bh.hdt = dz * 1000.;
		} else {
			warn(" Binary file hdt and trace header dt in meters or feet \n");
			bh.hdt = dz;
		}
	}

    if (!getparint("traceout",&traceout)) traceout = 1;
	if(traceout==1) fputhdr(outfp,&ch,&bh);

	if(ierr==0) {
		/*
		if(o1!=x0m) err("o1=%g x0m=%g \n",o1,x0m);
		if(o2!=y0m) err("o2=%g y0m=%g \n",o2,y0m);
		if(o3!=0.) err("o3=%g \n",o3);
		if(d1!=dx) err("d1=%g dx=%g \n",d1,dx);
		if(d2!=dy) err("d2=%g dy=%g \n",d2,dy);
		if(d3!=dvs) err("d3=%g dvs=%g \n",d3,dvs);
		if(n1!=nx) err("n1=%d nx=%d \n",n1,nx);
		if(n2!=ny) err("n2=%d ny=%d \n",n2,ny);
		if(n3!=nvs) err("n3=%d nvs=%d \n",n3,nvs);
		*/

		if(fabs(o1-x0m)>0.1 || fabs(o2-y0m)>0.1 || 
			fabs(o3)>0.1 ||
		   	fabs(d1-dx)>0.1 || fabs(d2-dy)>0.1 || 
			fabs(d3-dvs)>0.1 ||
		   	n1!=nx || n2!=ny || n3!=nvs) {

			fprintf(jpfp,
				"o1=%g sstart=%g o2=%g lstart=%g o3=%g \n",
				o1,x0m,o2,y0m,o3);
			fprintf(jpfp,
				"d1=%g ds=%g d2=%g dl=%g d3=%g dvs=%g\n",
				d1,dx,d2,dy,d3,dvs);
			fprintf(jpfp,
				"n1=%d ns=%d n2=%d nl=%d n3=%d nvs=%d\n",
				n1,nx,n2,ny,n3,nvs);
			fprintf(jpfp," check velfile header \n");
			fflush(jpfp);

			err("check velfile header ");
		}
	}
	fseek64(velfp,0,0);
	if ( minvel>0. && maxvel>0. ) {
		vxy = (float*) emalloc(nx*ny*sizeof(float));
		for(itau=0;itau<nvs;itau++) {
			efread(vxy,sizeof(float),nx*ny,velfp);
			for(iyy=0;iyy<ny;iyy++) { 
			for(ixx=0;ixx<nx;ixx++) {
				if(vxy[ixx+iyy*nx]<minvel || vxy[ixx+iyy*nx]>maxvel) {
			fprintf(jpfp," velocity=%g error at ix=%d iy=%d itau=%d \n",
					ixx+1,iyy+1,itau+1);
					err("check velfile values ");
				}
			}
			}
		}
		free(vxy);
	}

    if (!getparint("nfft",&nfft)) nfft = nt * 3 / 2;
    ntmp = (nfft+1)/2*2;
    radix_(&ntmp,&nfft);
	nfftq = nfft/2+1;
    if (!getparint("iestep",&iestep)) {
    	if (!getparint("estep",&estep)) {
			iestep = 5;
		} else {
			iestep = (estep / (dtau*1000.) + 0.5);
		}
	}
    if (!getparint("iorder",&iorder)) iorder = 1;
    if (!getparfloat("dfc",&dfc)) dfc = 0.03;
    if (!getparfloat("fmin",&fmin)) fmin = 0.05 * .5 / dt;
    if (!getparfloat("fmax",&fmax)) fmax = 1. / 2.0 * .5 / dt;
    if (!getparfloat("fmaxend",&fmaxend)) fmaxend = fmax;
	if(fmaxend>fmax) fmaxend=fmax;
    if (!getparfloat("tzend",&tzend)) tzend = 99999.;
    if (!getparint("icstep",&icstep)) {
    	if (!getparint("cstep",&cstep)) { 
			icstep=25;
		} else {
			icstep = cstep / (dtau*1000.) + 0.5; 
		}
	}
    if (!getparint("nffs",&nffx)) nffx = nx * 3 / 2;
    nffx = (nffx+1)/2*2;
    radix_(&nffx,&nkx);
    if (!getparint("nffl",&nffy)) nffy = ny * 3 / 2;
    nffy = (nffy+1)/2*2;
    radix_(&nffy,&nky);
	if(nx==1) nkx=1;
	if(ny==1) nky=1; 
    if (!getparint("lvec",&lvec)) lvec = 0;
    if (!getparint("mlimit",&mlimit)) mlimit = 512;
	llimit = mlimit;
	llimit = llimit * 1024 * 1024;

	idataxyw = 1;
	if (!getparstring("diskxyw",&diskxyw)) {
		diskxyw = (char*) emalloc(80*sizeof(char));
		sprintf(diskxyw,"DISKWAVE\0");
		idataxyw = 0;
	}
	idatahdr = 1;
	if (!getparstring("diskhdr",&diskhdr)) {
		diskhdr = (char*) emalloc(80*sizeof(char));
		idatahdr = 0;
		sprintf(diskhdr,"DISKHEADER\0");
	}
	idataxyt = 1;
	if (!getparstring("diskxyt",&diskxyt)) {
		diskxyt = (char*) emalloc(80*sizeof(char));
		sprintf(diskxyt,"DISKIMAGE\0");
		idataxyt = 0;
	}

	if ( getparstring("diskxytb",&diskxytb) &&
	     getparstring("diskxywb",&diskxywb) &&
	     getparstring("diskhdrb",&diskhdrb) ) {
		ibackd = 1;
	} else {
		ibackd = 0;
	}

	if (!getparint("isave",&isave)) isave = 0;

	ibacki = 0;
	if (getparstring("backupi",&backupi)) {
		ibacki = 1;
		fprintf(jpfp," Backup from the previous run ");
		fflush(jpfp);
		tar3fr(backupi,diskhdr,diskxyt,diskxyw);
	}


	ibacko = 0;
	if (getparstring("backupo",&backupo)) ibacko = 1;
	if(ibacko==1) isave=1;

	if (!getparint("ncpu",&ncpu)) ncpu = 2;
	if (!getparint("nwmig",&nwmig)) nwmig = ncpu;
	if(nwmig<ncpu) {
	   warn(" nwmig:%d < ncpu:%d;  nwmig reset to %d \n",nwmig, ncpu, ncpu);
	   nwmig = ncpu;
	} else {
	/* make sure nwmig is an integer multiple of ncpu */
	   nwmig = ((nwmig+ncpu-1)/ncpu)*ncpu;
	}
	if (ncpu<1 || ncpu>32) err(" check ncpu value in setup");
	
	envs = (char*) emalloc(80*sizeof(char));
    if(!getenv("PARALLEL")) {
    	sprintf(envs,"%s=%d","PARALLEL",ncpu);
        putenv(envs);
    }

    if (!getparint("ifmin",&ifmin)) ifmin = -1;
    if (!getparint("ifmax",&ifmax)) ifmax = -1;
	ifrange=1;
	if(ifmin==-1 || ifmax==-1) ifrange = 0;

	/* detect starting time of migration */
	ixytsize = 0;
	ntaulast = ntau;
	if(idataxyt==1) {
		if((xytfp = fopen(diskxyt,"r"))!=NULL) {
			i64 = 0;
           	fseek64(xytfp,i64,SEEK_END);
           	ixytsize= ftell64(xytfp);
           	fclose(xytfp);
			ntaulast = ixytsize/(nx*ny*sizeof(float));
    	}
	}
	itau0 = ixytsize/(nx*ny*sizeof(float)) + 1;

	if(lendur!=0) {
		if(itau0dur > itau0) err("check durfile parameter itau0dur"); 
		if(itau0dur > 0) itau0 = itau0dur;
		if(itau0dur==-999) itau0=1;
	}

	if (!getparint("nsteps",&nsteps)) nsteps = ntau-itau0+1;
	if(ny==1) nsteps = ntau-itau0+1;
	if(nsteps>ntau-itau0+1) nsteps = ntau - itau0 + 1;

    if ( icstep > 0 ) icstep = ((icstep+iestep-1)/iestep)*iestep;
    if ( icstep ==0 ) {
		nkx = 1;
		nky = 1;
	}
	/* compute number of frequencies to migrated */
	pi = 3.141592654;
	dw = 2.*pi/(nfft*dt);
	wmin = fmin*2*pi/dw;
	iwmin = wmin;
	if (iwmin<1) iwmin = 1;
	if (iwmin>=nfftq) iwmin = nfftq-1;
	wmax = fmax*2*pi/dw;
	iwmax = wmax;
	if (iwmax<1) iwmax = 1;
	if (iwmax>=nfftq) iwmax = nfftq-1;
	nw = iwmax - iwmin + 1;
	wmin = iwmin * dw;
	tmp = fmaxend*2.*pi/dw;
	iwend= tmp;
	if(iwend>iwmax) iwend=iwmax;
	iwend = iwend - iwmin + 1;
	if(dz>0.) {
		tmp = tzend/dz + 1.5;
		ntauend = tmp;
	} else {
		tmp = tzend/(dtau*1000.) + 1.5;
		ntauend = tmp;
	}

	if(ifrange==0) {
		ifmin = iwmin;
		ifmax = iwmax;
		nf = ifmax - ifmin + 1;
	} else {
		if(ifmin<iwmin) ifmin = iwmin;
		if(ifmax>iwmax) ifmax = iwmax;
		wmin = ifmin * dw;
		nf = ifmax - ifmin + 1;
	}


	if(ny>1) {
		lvec = nx; 
		if (lvec<ny) lvec=ny;
	} else {
		if (lvec==0) lvec = nf;
	}

	fprintf(jpfp,"\n");
	fprintf(jpfp," -------- FXYMIG PRINTOUT -------- \n");
	fprintf(jpfp,"\n");
	fflush(jpfp);

	/* compute memory requirement for migration*/
	ncp = nf;
	nq = nsteps;

	if(dz>0.) {
		tmp = nq * dz/dvs + 1.5;
	} else {
		tmp = nq*dtau/(dvs*0.001) + 1.5;
	}
	nv = (int) tmp;
	if(nv<1) nv=1;
	if(ny==1) nv = nvs;
	naux1 = (nkx+15) * 4;
	naux2 = (nky+15) * 4;
	naux = nkx;
	if(nky>nkx) naux = nky;
	lplane = nx*ny;
	if(ny==1) lplane=nx*lvec;

	if(ny==1) { ncpu = 1; nwmig = 1;}
 
	if(ny>1 && iorder<=1 && dx==dy ) iisave = 1;

	lmem = nx * ny;
	lmem = lmem * ncp;
	lwork = (lmem + (4*lplane+naux+nkx*nky+2*lvec)*ncpu+nx*ny*nwmig+ncph+nf)
			*sizeof(complex);
	lmem = nx*ny;
	lmem = lmem * (2+nv+nq);
	lwork = lwork + ( (naux1+naux2+lvec)*ncpu+
		+ 2*lplane+nf+lmem+ncpu)
		*sizeof(float) 
		+ iisave*2*lplane*ncpu*sizeof(complex); 

/*
	fprintf(jpfp,"lwork=%f nx=%d ny=%d ncp=%d \n",(float)lwork,nx,ny,ncp);
	fprintf(jpfp,"mlimit=%d lwork=%lld\n",mlimit,lwork);
	fprintf(jpfp,"lplane=%d naux=%d nw=%d nkx=%d nky=%d\n",
			lplane,naux,nw,nkx,nky);
	fprintf(jpfp,"lvec=%d naux1=%d naux2=%d nv=%d nq=%d iisave=%d\n",
			lvec,naux1,naux2,nv,nq,iisave);

	lwork = (nx*ny*ncp + 6*lplane + naux + nw + nkx*nky + 2*lvec)
		*sizeof(complex);
	fprintf(jpfp,"mlimit=%d lwork=%lld\n",mlimit,lwork);
*/


/*
	if(lwork>llimit && ny>1) {
*/
	if(ny>1) {
		fprintf(jpfp,
		"Wavefield Disk I/O Used To Reduce Memory Requirement \n");
		ncp = 1;
		lwork = (nx*ny*ncp + (4*lplane+naux+nkx*nky+2*lvec)*ncpu+nx*ny*nwmig
			+ ncph + nf)*sizeof(complex);
		lmem = nx*ny;
		lmem = lmem * (2+nv+nq);
		lwork = lwork 
			+((naux1+naux2+lvec)*ncpu+2*lplane
			+nf+lmem+ncpu)
			*sizeof(float) 
			+ iisave*2*lplane*ncpu*sizeof(complex); 
	}
	if(lwork>llimit && ny==1) {
		fprintf(jpfp,
		"Velocity Disk I/O Used To Reduce Memory Requirement \n");
		nv = 1;
		lwork = (nx*ny*ncp + (4*lplane+naux+nkx*nky+nx*ny+2*lvec)
			+ ncph + nf)*sizeof(complex);
		lmem = nx*ny;
		lmem = lmem * (2+nv+nq);
		lwork = lwork 
			+((naux1+naux2+lvec)*ncpu+lplane*2
			+nf+lmem+ncpu)
			*sizeof(float) 
			+ iisave*2*lplane*ncpu*sizeof(complex); 
	}
	if(lwork>llimit && ny>1) {
		fprintf(jpfp,
	"Image/Velocity Disk I/O Used To Reduce Memory Requirement \n");
		/*
		for(iq=nq;iq>=1;iq=iq/2) {
			if(dz>0.) {
				tmp = iq * dz/dvs + 1.5;
			} else {
				tmp = iq*dtau/(dvs*0.001) + 1.5;
			}
			nv = (int) tmp;
			if(nv<1) nv=1;
			lwork = (nx*ny*ncp + 
				(4*lplane+naux+nkx*nky+2*lvec)*ncpu+nx*ny*nwmig+ncph+nf)
				*sizeof(complex);
			lwork = lwork 
				+((naux1+naux2+lvec)*ncpu+lplane*2
				+nf+nx*ny*(2+nv+iq)+ncpu)
				*sizeof(float) 
				+ iisave*2*lplane*ncpu*sizeof(complex); 
			if(lwork<llimit) break;
		}
		nq = iq;
		if(nq == 0) nq = 1;
		if(nq>nsteps) nq = nsteps;
		*/
                if(dz>0.) {
                        temp = dz/dvs;
                } else {
                        temp = dtau/(dvs*0.001);
                }
                lwork = (nx*ny*ncp+(4*lplane+naux+nkx*nky+2*lvec)*ncpu
                        +nx*ny*nwmig + ncph + nf)*sizeof(complex);
                lwork = lwork + ( (naux1+naux2+lvec)*ncpu+
                        + lplane*2+nf+nx*ny*2+ncpu)
                        *sizeof(float)
                        + iisave*2*lplane*ncpu*sizeof(complex);

/*
                fprintf(stderr,"llimit=%f lwork=%f \n",(float)llimit,(float)lwork);
*/

                tmp = (llimit-lwork)/((1.+temp)*nx*ny*sizeof(float));
                nq = tmp;
				if(nq>nsteps || nq<0) nq = nsteps;
                nv = nq * temp + 1.5;
/*
                fprintf(stderr,"nq=%d nv=%d \n",nq,nv);
*/
                lwork = lwork + (nq+nv)*nx*ny*sizeof(float);
	} else if(nq<ntau-itau0+1) {
		fprintf(jpfp,
	"Image/Velocity Disk I/O Used To Reduce Memory Requirement \n");
	}

	if(lwork>llimit && iisave>0 && nq==1) {
	      	fprintf(jpfp,
		"FD Coefficients Recomputed To Reduce Memory Requirement\n");
		iisave = 0;
		lwork = (nx*ny*ncp + (4*lplane+naux+nkx*nky+2*lvec)*ncpu 
			+ nx*ny*nwmig + ncph + nf)*sizeof(complex);
		lwork = lwork 
			+ (naux1+naux2+2*lplane+lvec*ncpu+nf
			+ nx*ny*(2+nv+nq)+ncpu)*sizeof(float) 
			+ iisave*2*lplane*ncpu*sizeof(complex); 
	}
	if(lwork>llimit && nq==1 ) {
		fprintf(jpfp,
			"Memory Limit Too Small. Need At Least %g Bytes \n",
			(float)(lwork)); 
		fprintf(jpfp, "Job Cancelled \n");

		err("Memory Limit Too Small. Need At Least %g Bytes \n",
			(float)(lwork)); 
	}

	fprintf(jpfp," Total Memory needed (in MB) = %g \n", 
		(float)lwork/1024./1024.);
	fprintf(jpfp,"\n");


	if(lendur!=0 && itau0dur>0 && itaudur>1) { 
		if(nq<(itaudur-itau0dur+1)) warn(" nq=%d too small \n",nq); 
		if(nq<(itaudur-itau0dur+1)) warn(" itaudur=%d itau0dur=%d \n",
			itaudur,itau0dur);
		nq = itaudur - itau0dur + 1;
		if(nq>nsteps || ntau>ntaulast) nq = nsteps;
		if(dz>0.) {
			tmp = nq * dz/dvs + 1.5;
		} else {
			tmp = nq*dtau/(dvs*0.001) + 1.5;
		}
		nv = (int) tmp;
		if(nv<1) nv=1;
		if(ny==1) nv = nvs;
	}


/*print setup parameters */

	fprintf(jpfp,"\n");
	fprintf(jpfp," Migration Parameters: \n");
	fprintf(jpfp," ===================== \n");
	if(icdps>=0) {
	fprintf(jpfp," cdp1=%d ncdppl=%d nlines=%d \n", cdp1,ncdppl,nlines);
	fprintf(jpfp," x1=%f y1=%f s1=%f l1=%f \n", x1,y1,s1,l1);
	fprintf(jpfp," x2=%f y2=%f s2=%f l2=%f \n", x2,y2,s2,l2);
	fprintf(jpfp," x3=%f y3=%f s3=%f l3=%f \n", x3,y3,s3,l3);
	fprintf(jpfp," sstart=%f lstart=%f \n", x0m,y0m);
	} else {
	fprintf(jpfp," tracekey=%s linekey=%s \n", tracekey,linekey);
	fprintf(jpfp," traceinc=%d lineinc=%d \n", traceinc,lineinc);
	fprintf(jpfp," trstart=%d lnstart=%d \n", trstart,lnstart);
	}
	fprintf(jpfp," ntau=%d nt=%d nw=%d ns=%d nl=%d \n", ntau,nt,nw,nx,ny);
	fprintf(jpfp," dtau=%f dt=%f dz=%f ds=%f dl=%f \n",
		dtau*1000.,dt*1000.,dz,dx,dy);
	fprintf(jpfp," itau0=%d iestep=%d icstep=%d nvs=%d naux=%d \n",
		itau0,iestep,icstep,nvs,naux);
	fprintf(jpfp," nfft=%d nffs=%d nffl=%d nsteps=%d nv=%d ncp=%d \n",
		nfft,nkx,nky,nq,nv,ncp);
	fprintf(jpfp," lvec=%d lplane=%d iorder=%d naux1=%d naux2=%d \n",
		lvec,lplane,iorder,naux1,naux2);
	fprintf(jpfp," dfc=%f vref=%f dvs=%f isave=%d iisave=%d \n", 
		dfc,vref,dvs,isave,iisave);
	fprintf(jpfp, " velfile=%s \n", velfile);
	fprintf(jpfp, " diskxyw=%s \n", diskxyw);
	fprintf(jpfp, " diskhdr=%s \n", diskhdr);
	fprintf(jpfp, " diskxyt=%s \n", diskxyt);
	if(ibackd==1) {
		fprintf(jpfp, " diskxywb=%s \n", diskxywb);
		fprintf(jpfp, " diskhdrb=%s \n", diskhdrb);
		fprintf(jpfp, " diskxytb=%s \n", diskxytb);
	}
	if(lendur>0) {
		fprintf(jpfp, " durfile=%s \n", durfile);
		fprintf(jpfp, " itau0dur=%d itaudur=%d iwdur=%d\n", 
			itau0dur,itaudur,iwdur);
	}

	fprintf(jpfp," fmin=%g fmax=%g mlimit=%d ncpu=%d nwmig=%d \n", 
		fmin, fmax, (int)(llimit/1024./1024.),ncpu,nwmig);
	fprintf(jpfp," fmaxend=%g tzend=%g \n", fmaxend, tzend);
	fprintf(jpfp," traceout=%d \n", traceout);
	if(ifrange==1) {
		fprintf(jpfp," ifmin=%d ifmax=%d nf=%d iwmin=%d iwmax=%d iwend=%d \n", 
			ifmin,ifmax,nf,iwmin,iwmax,iwend+iwmin-1);
	}

	if(ibacki==1) fprintf(jpfp, " backupi=%s \n", backupi);
	if(ibacko==1) fprintf(jpfp, " backupo=%s \n", backupo);
	fprintf(jpfp," ===================== \n");
	fprintf(jpfp,"\n");
	fflush(jpfp);

/* compute relative lowest frequency index */
	ifminr = ifmin - iwmin + 1;


	/* allocate memory for trace fft */
	lwork = nx*nf*sizeof(complex)+(nfft*2+15+nx)*sizeof(float)
			+nx*HDRBYTES*sizeof(char);
	if(lwork>llimit) 
		err("Memory Limit Too Small. Need At Least %g Mega Bytes \n",
			(lwork+1024*1024-1)/(1024*1024)); 

	nybig = llimit/2/(nx*nf*sizeof(complex));
	if(nybig<1) nybig = 1;

   	cp = (complex*)emalloc(nx*nf*sizeof(complex));
   	cpbig = (complex*)emalloc(nybig*nx*nf*sizeof(complex));
	trfft  = (float*)emalloc(nfft*sizeof(float)); 
	wsave  = (float*)emalloc((2*nfft+15)*sizeof(float));
	fold = (float*)emalloc(nx*sizeof(float));
	trhdr = (char*)emalloc(nx*HDRBYTES*sizeof(char));


	/* alloc disk spaces for wavefield data p(x,y,w) and trace headers */
	ixywsize = 0;
	if(idataxyw==1 ) {
		if((xywfp = fopen(diskxyw,"r"))!=NULL) {
			i64 = 0;
       		fseek64(xywfp,i64,SEEK_END);
       		ixywsize= ftell64(xywfp);
       		fclose(xywfp);
		}
   	}

	i64 = nx*ny;
	i64 = i64*nf;
	i64 = i64*sizeof(complex);
   	if( ixywsize!= i64 ) {
		xywfp = fopen(diskxyw,"w+");
	} else {
		xywfp = fopen(diskxyw,"r+");
	} 
    fseek64(xywfp,0,0);

	ihdrsize = 0;
	if(idatahdr==1 ) {
		if((hdrfp = fopen(diskhdr,"r"))!=NULL) {
			i64 = 0;
        	fseek64(hdrfp,i64,SEEK_END);
           	ihdrsize= ftell64(hdrfp);
       		fclose(hdrfp);
        }
	}
    i64 = nx*ny;
	i64 = i64*HDRBYTES;

    if( ihdrsize!=i64 ) {
		hdrfp = fopen(diskhdr,"w+");
	} else {
		hdrfp = fopen(diskhdr,"r+");
	} 
    fseek64(hdrfp,0,0);


	l64 = nx*ny;
	l64 = l64*nf*sizeof(complex);
	if(ihdrsize==i64 && ixywsize==l64 && itau0dur!=-999) goto no_fft_needed;

	bzero(cp,nx*nf*sizeof(complex));
	bzero(trhdr,nx*HDRBYTES*sizeof(char));
	bzero(cpbig,nybig*nx*nf*sizeof(complex));


	/* see if we can allocate the disk space */
	fclose(xywfp);
	xywfp = fopen(diskxyw,"w+"); 
	fclose(hdrfp);
	hdrfp = fopen(diskhdr,"w+"); 

	for(iy=0;iy<ny;iy++) { 
		i64 = iy*nf*nx;
		i64 = i64*sizeof(complex);
		fseek64(xywfp,i64,0);
		fwrite(cp,sizeof(complex),nx*nf,xywfp);

		i64 = iy*nx*HDRBYTES;
		i64 = i64*sizeof(char);
		fseek64(hdrfp,i64,0);
		fwrite(trhdr,sizeof(char),nx*HDRBYTES,hdrfp);
	}
	i64 = 0;
	fseek64(xywfp,i64,0);
	fseek64(hdrfp,i64,0);

	xytfp = fopen(diskxyt,"w+");
	qbuf = (float*) emalloc(nx*ny*sizeof(float));
	bzero(qbuf,nx*ny*sizeof(float));
	fseek64(xytfp,i64,0);
	for(it=0;it<ntau;it++) {
	  	efwrite(qbuf,sizeof(float),nx*ny,xytfp);
	}
	free(qbuf);
	fclose(xytfp);

	/* trace fft and transpose data */
	/* initialize fft */
	rffti_(&nfft,wsave);
	jy = -1;
	jx = 0;
	ntrace = 0;

	tmp = (x0m - s1)/dx;
	ix0 = tmp; 
	tmp = (y0m - l1)/dy;
	iy0 = tmp; 

	do {
		if(icdps==-1) {
			gethval(&tra,indxtrk,&trkval);
			ix = vtoi(trktype,trkval);
			ix =  (ix - trstart);
			if(ix%traceinc!=0 || ix<0) continue;
			ix  = ix/traceinc;
			gethval(&tra,indxlnk,&lnkval);
			iy = vtoi(lnktype,lnkval);
			iy =  (iy - lnstart);
			if(iy%lineinc!=0 || iy<0) continue;
			iy = iy/lineinc;
		} else if(icdps==1) { 
			if(cdpnum==0) {
				iy = (tra.cdp - cdp1)/ncdppl/cdpinc;
				ix = (tra.cdp-cdp1-iy*ncdppl*cdpinc)/cdpinc 
					-ix0;
				iy = iy - iy0;
			} else {
				ix = (tra.cdp - cdp1)/nlines/cdpinc;
				iy = (tra.cdp-cdp1-ix*nlines*cdpinc)/cdpinc 
					-iy0;
				ix = ix - ix0;
			}
		} else { 
			/* x and y positions of midpoint */
			if(tra.scalco>1) {
				xm = (tra.sx + tra.gx)*0.5*tra.scalco;
				ym = (tra.sy + tra.gy)*0.5*tra.scalco;
			} else if(tra.scalco<0) {
				xm = (tra.sx + tra.gx)*0.5/(-tra.scalco);
				ym = (tra.sy + tra.gy)*0.5/(-tra.scalco);
			} else {
				xm = (tra.sx + tra.gx)*0.5;
				ym = (tra.sy + tra.gy)*0.5;
			}
			/* migration-grid line and trace positions */
			xy2sl(x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3,
				xm,ym,&sm,&lm);
			tmp = (lm - y0m)/dy + 0.5;
			iy = tmp;
			tmp = (sm - x0m)/dx + 0.5;
			ix  = tmp; 
		}


		/*
		fprintf(jpfp,"is=%d il=%d xm=%g ym=%g\n",ix+1,iy+1,xm,ym);
		*/

		/* trace outside migration grid */
		if (iy<0 || iy >= ny || ix<0 || ix>=nx || iy<jy ) continue;
		if (jy == -1) jy = iy;

		/* fft and save to a buffer */
		if (iy==jy) {
			for(it=0;it<nfft;it++) trfft[it] = 0.;
			if(tra.trid==1) bcopy((char*)tra.data,
				(char*)trfft,nt*sizeof(float));
			bcopy((char*)&tra,trhdr+ix*HDRBYTES,HDRBYTES);
			fold[ix] = fold[ix] + 1;
			for(it=0;it<nt;it++) {
				if(fabs(trfft[it])>maxamp) {
			fprintf(jpfp," bad amplitude %g (>%g) found at it=%d ix=%d iy=%d\n",
					trfft[it],maxamp,it+1,ix+1,iy+1);
			err(" bad amplitude %g (>%g) found at it=%d ix=%d iy=%d\n",
					trfft[it],maxamp,it+1,ix+1,iy+1);
				}	
			}
			rfftf_(&nfft,trfft,wsave);
			for(iw=ifmin;iw<=ifmax;iw++) {
				ir = 2 * iw - 1;
				ii = 2 * iw;
				cp[ix+(iw-ifmin)*nx].r = cp[ix+(iw-ifmin)*nx].r
							 + trfft[ir];
				cp[ix+(iw-ifmin)*nx].i = cp[ix+(iw-ifmin)*nx].i
							 + trfft[ii];
			}
			jx = jx + 1;
		/* change line */
		} else {
			fprintf(jpfp,
				" Input %d live traces for line %d\n",jx,jy+1);
			fflush(jpfp);
			ntrace += jx;
			/* write this line to disk */
			wl2d(nx,ny,nf,jy,cp,trhdr,fold,xywfp,hdrfp,jpfp,
			 	trktype,lnktype,
				trkval,lnkval,indxtrk,indxlnk,trstart,lnstart,
				traceinc, lineinc, ikey, 
				cpbig,nybig,0); 
			/* reintialize for the next line */
			bzero(cp,nx*nf*sizeof(complex));
			bzero(trhdr,nx*HDRBYTES*sizeof(char));
			bzero(fold,nx*sizeof(float));
			jx = 0;
			jy = iy;
			/* save first trace of next line */ 
			for(it=0;it<nfft;it++) trfft[it] = 0.;
			if(tra.trid==1) bcopy((char*)tra.data, 
				(char*)trfft, nt*sizeof(float));		
			bcopy((char*)&tra,trhdr+ix*HDRBYTES,HDRBYTES);
			fold[ix] = fold[ix] + 1;
			rfftf_(&nfft,trfft,wsave);
			for(iw=ifmin;iw<=ifmax;iw++) {
				ir = 2 * iw - 1;
				ii = 2 * iw;
				cp[ix+(iw-ifmin)*nx].r = cp[ix+(iw-ifmin)*nx].r
							 + trfft[ir];
				cp[ix+(iw-ifmin)*nx].i = cp[ix+(iw-ifmin)*nx].i
							 + trfft[ii];
			}
			jx = jx + 1;
		} 
	} while(fgettr(infp,&tra));


	/* for the last line */
	if (jy<ny && jx > 0 ) {
		fprintf(jpfp,
			" Input %d Live Traces For Line %d\n",jx,jy+1);
		fflush(jpfp);
		ntrace += jx;
		/* write this line to disk */
		wl2d(nx,ny,nf,jy,cp,trhdr,fold,xywfp,hdrfp,jpfp,
			trktype,lnktype,
			trkval,lnkval,indxtrk,indxlnk,trstart,lnstart,
			traceinc,lineinc,ikey, 
			cpbig,nybig,1); 
	}
	fprintf(jpfp, 
		" There Are %d Total Live Traces for Migration \n",ntrace);
	fprintf(jpfp, " Input fft Of Time Completed And Saved \n");
	fflush(jpfp);

	if(ntrace==0) err(" Check 3D setup parameters: no trace input ntrace=0 ");

	no_fft_needed:
	/* We want to close standard input now, because input is done.
	   However, the SUN f77 compiler does not allow first fortran open
	   after the standard input is closed. So we issue a dummy
	   fortran open call before we close the standard input */ 
/*
	dummyopen_(&dummyunit);
*/
	fclose(infp);

	/* free space used in trace fft */

    free(cp);
    free(cpbig);
	free(trfft); 
	free(wsave);
	free(fold);
	free(trhdr); 

	/* allocate more disk space for diskxyt if needed */
	if(ntau>ntaulast) {
		fprintf(jpfp, " Add more disk space for %s \n",diskxyt);
		fprintf(jpfp, "  ... last ntau = %d \n",ntaulast);
		fprintf(jpfp, "  ... currrent ntau = %d \n",ntau);
		xytfp = fopen(diskxyt,"r+");
		i64 = ntaulast;
		i64 = i64 * nx * ny;
		i64 = i64  * sizeof(float);
		fseek64(xytfp,i64,0);
		qbuf = (float*) emalloc(nx*ny*sizeof(float));
		bzero(qbuf,nx*ny*sizeof(float));

		for(it=ntaulast;it<ntau;it++) {
	  		fwrite(qbuf,sizeof(float),nx*ny,xytfp);
		}
		free(qbuf);
		fclose(xytfp);
	}

	/* allocate space for migration */

	lmem = 0;
    	cp = (complex*)emalloc(nx*ny*ncp*sizeof(complex));
	lmem += nx*ny*ncp*sizeof(complex);
       	bcp = (complex*)emalloc(nkx*nky*ncpu*sizeof(complex));
	lmem += nkx*nky*ncpu*sizeof(complex);
       	cpp = (complex*)emalloc(nx*ny*nwmig*sizeof(complex));
	lmem += nx*ny*nwmig*sizeof(complex);
       	caux = (complex*)emalloc(naux*ncpu*sizeof(complex));
	lmem += naux*ncpu*sizeof(complex);
       	shift = (complex*)emalloc(nf*sizeof(complex));
	lmem += nf*sizeof(complex);
    	aa = (complex*)emalloc(lplane*ncpu*sizeof(complex));
	lmem += lplane*ncpu*sizeof(complex);
    	a = (complex*)emalloc(lplane*ncpu*sizeof(complex));
	lmem += lplane*ncpu*sizeof(complex);
    	r = (complex*)emalloc(lplane*ncpu*sizeof(complex));
	lmem += lplane*ncpu*sizeof(complex);
    	cpt = (float*)emalloc(lplane*2*ncpu*sizeof(float));
	lmem += lplane*ncpu*2*sizeof(float);
    	al = (complex*)emalloc(lvec*ncpu*sizeof(complex));
	lmem += lvec*ncpu*sizeof(complex);
    	ar = (complex*)emalloc(lvec*ncpu*sizeof(complex));
	lmem += lvec*ncpu*sizeof(complex);
    	cpbuf = (complex*)emalloc(lplane*sizeof(complex));
	lmem += lplane*sizeof(complex);
    	cph = (complex*)emalloc(ncph*sizeof(complex));
	lmem += ncph*sizeof(complex);
	if(iisave>0) {
    		aasave = (complex*)emalloc(iisave*lplane*ncpu*sizeof(complex));
		lmem += iisave*lplane*ncpu*sizeof(complex);
    		asave = (complex*)emalloc(iisave*lplane*ncpu*sizeof(complex));
		lmem += iisave*lplane*ncpu*sizeof(complex);
	} else {
    		aasave = (complex*)emalloc(sizeof(complex));
		lmem += sizeof(complex);
    		asave = (complex*)emalloc(sizeof(complex));
		lmem += sizeof(complex);
	}
    	aux1 = (float*)emalloc(naux1*ncpu*sizeof(float));
	lmem += naux1*ncpu*sizeof(float);
    	aux2 = (float*)emalloc(naux2*ncpu*sizeof(float));
	lmem += naux2*ncpu*sizeof(float);
/*
    	aux1 = (float*)emalloc((naux1+naux2)*ncpu*sizeof(float));
	lmem += (naux1+naux2)*ncpu*sizeof(float);
    	aux2 = aux1+naux1*ncpu;
*/
    	om = (float*)emalloc(nf*sizeof(float));
	lmem += nf*sizeof(float);
    	oms = (float*)emalloc(lvec*ncpu*sizeof(float));
	lmem += lvec*ncpu*sizeof(float);
    	vyx = (float*)emalloc(lplane*sizeof(float));
	lmem += lplane*sizeof(float);
    	vxy = (float*)emalloc(nx*ny*sizeof(float));
	lmem += nx*ny*sizeof(float);
    	v = (float*)emalloc(nv*ny*nx*sizeof(float));
	lmem += nv*nx*ny*sizeof(float);
		q = (float*)emalloc(nq*nx*ny*sizeof(float));
	lmem += nq*nx*ny*sizeof(float);
    	va = (float*)emalloc(nv*sizeof(float));
	lmem += nv*sizeof(float);
    	w = (float*)emalloc(ncpu*sizeof(float));
	lmem += ncpu*sizeof(float);
    	qbuf = (float*)emalloc(lplane*sizeof(float));
	lmem += lplane*sizeof(float);
    	dzov = (float*)emalloc(nx*ny*sizeof(float));
	lmem += nx*ny*sizeof(float);

	/*
	err("allocation done lmem=%f \n",(float)lmem);
	*/
	fprintf(jpfp," Total Memory used (in MB) = %g \n", 
		(float)lmem/1024./1024.);
	fprintf(jpfp,"\n");

/* initialize backup disks*/
	if(lendur!=0 && itau0dur>0 && ntau==ntaulast) {
		stat(diskxyw,&sbuf);
		itime1 = (int) sbuf.st_mtime;
		stat(diskxyt,&sbuf);
		itime2 = (int) sbuf.st_mtime;
		stat(durfile,&sbuf);
		itime3 = (int) sbuf.st_mtime;
		if(itime3<itime1 || itime3<itime2) {
			if(ibackd!=1) {
			fprintf(jpfp, 
			" Updates of disks diskxyw or diskxyt failed during last run \n");
			err(" Updates of disks diskxyw or diskxyt failed during last run");
			} 
		}
	} else {
		itime1 = 1;
		itime2 = 2;
		itime3 = 3;
	}
	if(ibackd==1) {

		lenxytb = strlen(diskxytb);
		lenxywb = strlen(diskxywb);
		namexytb = (char*) emalloc(lenxytb+1);
		namexywb = (char*) emalloc(lenxywb+1);
		bcopy(diskxytb,namexytb,lenxytb);
		bcopy(diskxywb,namexywb,lenxywb);
		namexytb[lenxytb]='\0';
		namexywb[lenxywb]='\0';

		hdrbfp = fopen(diskhdrb,"w");
		fseek64(hdrfp,0,0);
		fprintf(jpfp," Copy %s to %s \n",diskhdr,diskhdrb); 
		for(ix=0;ix<nx*ny;ix++) {
	  		fread(&tra,sizeof(char),HDRBYTES,hdrfp);
	  		fwrite(&tra,sizeof(char),HDRBYTES,hdrbfp);
		}
		efclose(hdrbfp);
		
		if(itime3>=itime1 && itime3>=itime2) {

			xywbfp = fopen(diskxywb,"w");
			fseek64(xywbfp,0,0);
			bzero(cp,nx*ny*sizeof(complex));
			i64 = 0;
			fseek64(xywfp,i64,0);
			fprintf(jpfp," Copy %s to %s \n",diskxyw,diskxywb); 
			for(iw=0;iw<nf;iw++) {
	  			efread(cp,sizeof(complex),nx*ny,xywfp);
	  			efwrite(cp,sizeof(complex),nx*ny,xywbfp);
			}
			efclose(xywbfp);

			xytbfp = fopen(diskxytb,"w");
			fseek64(xytbfp,0,0);
			if(idataxyt==1) {
				fprintf(jpfp," Copy %s to %s \n",diskxyt,diskxytb); 
				if(itau0>1) {
                    xytfp = efopen(diskxyt,"r");
                    fseek64(xytfp,0,0);
                    for(it=0;it<itau0-1;it++) {
                          efread(q,sizeof(float),nx*ny,xytfp);
                          efwrite(q,sizeof(float),nx*ny,xytbfp);
                    }
                    efclose(xytfp);
                }
				bzero(q,nx*ny*sizeof(float));
				for(it=itau0-1;it<ntau;it++) {
	  				efwrite(q,sizeof(float),nx*ny,xytbfp);
				}
		  	} else {
				bzero(q,nx*ny*sizeof(float));
				for(it=0;it<ntau;it++) {
	  				efwrite(q,sizeof(float),nx*ny,xytbfp);
				}
			}
			efclose(xytbfp);
		} else {
			xywbfp = fopen(diskxywb,"r");
			fseek64(xywbfp,0,0);
			bzero(cp,nx*ny*sizeof(complex));
			i64 = 0;
			fseek64(xywfp,i64,0);

			fprintf(jpfp,
			" Updates of disks diskxyw or diskxyt failed during last run \n");
			fprintf(jpfp," Copy %s to %s \n",diskxywb,diskxyw); 
			for(iw=0;iw<nf;iw++) {
	  			efread(cp,sizeof(complex),nx*ny,xywbfp);
	  			efwrite(cp,sizeof(complex),nx*ny,xywfp);
			}
			efclose(xywbfp);

			xytbfp = fopen(diskxytb,"r");
			fseek64(xytbfp,0,0);
            xytfp = efopen(diskxyt,"r+");
            fseek64(xytfp,0,0);
			fprintf(jpfp," Copy %s to %s \n",diskxytb,diskxyt); 
            for(it=0;it<ntau;it++) {
            	efread(q,sizeof(float),nx*ny,xytbfp);
                efwrite(q,sizeof(float),nx*ny,xytfp);
            }
            efclose(xytbfp);
			iwdur = 0;
			fprintf(jpfp, " Remigrate all frequency slices: set iwdur=0 \n");
		}
	}

	/* read the ffted wavefield into cp if needed */ 
	if(ncp==nf) {
		i64 = 0;
		fseek64(xywfp,i64,0);
		bzero(cp,nx*ny*nf*sizeof(complex));
	  	fread(cp,sizeof(complex),nx*ny*ncp,xywfp);
		if(isave==0) {
			fclose(xywfp);
			eremove(diskxyw);
		}
	} else {
		fclose(xywfp);
	}


	/* read in velocity if needed */
	if(nv==nvs) {
		if((velfp=fopen(velfile,"r"))==NULL)
			err("Input Velocity File velfile Error");
		fseek64(velfp,0,0);
	  	fread(v,sizeof(float),nx*ny*nvs,velfp);
		fclose(velfp);	
	} else {
		fclose(velfp);
	}

	/* compute frequencies to migrate */ 
	for(iw=0;iw<nf;iw++) om[iw] = wmin +iw*dw;
 
/*
	dump2xplot(v,nx,nvs,0,"v plot");
	dump2xplot(cp,nx*2,nf,1,"cp plot");
*/

	fprintf(jpfp,"\n");
	fprintf(jpfp," Start Migration \n");
	fprintf(jpfp," =============== \n");
	fflush(jpfp);

	fflush(jpfp);

    	/* call migration routine */

	lenvel = strlen(velfile);
	lenxyt = strlen(diskxyt);
	lenxyw = strlen(diskxyw);
	
	namevel = (char*) emalloc(lenvel+1);
	namexyt = (char*) emalloc(lenxyt+1);
	namexyw = (char*) emalloc(lenxyw+1);

	bcopy(velfile,namevel,lenvel);
	bcopy(diskxyt,namexyt,lenxyt);
	bcopy(diskxyw,namexyw,lenxyw);

	namevel[lenvel]='\0';
	namexyt[lenxyt]='\0';
	namexyw[lenxyw]='\0';


	if(dz==0.) dvs = dvs * 0.001;

        fclose(hdrfp);

	if(lenjpf>1) fclose(jpfp);

    wxymig_(&nx,&ny,&ntau,&nw,&itau0,&iestep,&icstep,
		&nvs,&nkx,&nky,&nq,&nv,&ncp,&lvec,&lplane,&iorder,
		&naux1,&naux2,&naux,
		&dt,&dx,&dy,&dtau,&dfc,&vref,&dvs,
		q,v,vxy,
		om,oms,vyx,w,
		aux1,aux2,
		a,aa,r,cpt,
		caux,shift,al,ar,
		cp,bcp,cpp,
		namexyw,namexyt,namevel,
		&lenxyw,&lenxyt,&lenvel,
		asave,aasave,&iisave,va,
		namejp,&lenjpf,&ncpu,
		namexywb,namexytb,
		&lenxywb,&lenxytb,
		qbuf,cpbuf,cph,dzov,&ncph,
		namedur,&lendur,&itaudur,&iwdur,
		&ntauend,&iwend,&nwmig,&nf,&ifminr);

	if(lenjpf>1) jpfp = efopen(jpfile,"a");

	fprintf(jpfp," =============== \n");
	fprintf(jpfp," Migration Done\n");
	fprintf(jpfp,"\n");

	/* free space */
	if(isave==0) free(cp);
	free(bcp);
	free(aa);
	free(a);
	free(r);
	free(cpt);
	free(caux);
	free(shift);
	free(al);
	free(ar);
	free(aux1);
	free(aux2);
	free(om);
	free(oms);
	free(vyx);
	free(vxy);
	free(v);
	free(va);
	free(asave);
	free(aasave);
	if(ncp==1 && idataxyw==0) eremove(diskxyw);

   	/* output migration result q(x,y,t) */
    hdrfp = fopen(diskhdr,"r");
	i64 = 0;
	fseek64(hdrfp,i64,0);

	fprintf(jpfp," Start Output \n");
	fprintf(jpfp," ============ \n");
	if ( nq==(ntau-itau0+1) && ntau>0 ) {
		if(itau0==1) {
			if(traceout==1) {
       			for (iy=0;iy<ny;iy++) {
       				for (ix=0;ix<nx;ix++) {
       					for (it=0;it<ntau-1;it++) 
						tra.data[it+1] = 
						q[(it*ny+iy)*nx+ix];
						tra.data[0] = 0;
	  					fread(&tra,sizeof(char),HDRBYTES,hdrfp);
						tra.ns = ntau;
						tra.dt = dtau * 1000000;
						tra.d1 = dtau * 1000000;
						if(tra.trid==0) {
							tra.trid = 2;
							if(cdpnum==0) {
                               tra.cdp = cdp1+(iy+iy0)*ncdppl*cdpinc
                                         +(ix+ix0)*cdpinc;
                        	} else {
                               tra.cdp = cdp1+(iy+iy0)*cdpinc
                                         +(ix+ix0)*nlines*cdpinc;
                        	}
						}
						if(dz>0.) {
							if(dz<=32.) {
								tra.dt = dz * 1000.;
							} else {
								tra.dt = dz;
							}
							tra.dz = dz;
							tra.fz = 0.;
							tra.mute = 0;
							tra.mutb = 0;
						}
						tra.tracl = ix + 1;
						tra.tracr = iy + 1; 
						tra.delrt = 0;
						if(ikey==1) {
							ftrace = ix*traceinc + trstart + 0.5;
							fline = iy*lineinc + lnstart + 0.5;
							itrace = ftrace;
							iline = fline;
							itov(trktype,&trkval,itrace);
							itov(lnktype,&lnkval,iline);
							puthval(&tra,indxtrk,&trkval);
							puthval(&tra,indxlnk,&lnkval);
						}
						fputtr(outfp,&tra);
					}
				}
			}			
			if(isave==1) {
				xytfp = fopen(diskxyt,"r+");
				fseek64(xytfp,0,0);
				fwrite(q,sizeof(float),nx*ny*ntau,xytfp);
				fclose(xytfp);
			}
		} else {
			xytfp = fopen(diskxyt,"r+");
			i64 = (itau0-1);
			i64 = i64*nx*ny;
			i64 = i64*sizeof(float);
			fseek64(xytfp,i64,0);
			fwrite(q,sizeof(float),nx*ny*nq,xytfp);
			fclose(xytfp);
		}
	} 
	if ( (itau0 > 1 || nq!=ntau) && ntau>0) {
		free(q);
		if(traceout==1) {
			if(llimit>128*1024*1024) {
				nyy = 128*1024*1024 / (nx*ntau*sizeof(float)) ;
			} else {
				nyy = llimit / (nx*ntau*sizeof(float)) ;
			}
			if(nyy<1) nyy=1;
			q = (float*)emalloc(nyy*nx*ntau*sizeof(float));
			if( q==0 ) err("llimit too small");
			xytfp = fopen(diskxyt,"r");
       		for (iy=0;iy<ny;iy=iy+nyy) {
				nyread = nyy;
				if(iy+nyread>ny) nyread = ny - iy;
				bzero((char*)q,nyy*nx*ntau*sizeof(float));

				for(it=0;it<ntau;it++) {
					i64 = it;
					i64 = i64*nx*ny+iy*nx;
					i64 = i64*sizeof(float);
					fseek64(xytfp,i64,0);
	   				fread(q+it*nx*nyy,sizeof(float),
						nx*nyread,xytfp);
				}
				for (iyread=0;iyread<nyread;iyread++) {
					iyy = iy + iyread;
					jy0 = iyread * nx;
       				for (ix=0;ix<nx;ix++) {
       					for (it=0;it<ntau-1;it++) 
						tra.data[it+1] = 
							q[it*nx*nyy+jy0+ix];
						tra.data[0] = 0.;
	  					fread(&tra,sizeof(char),HDRBYTES,hdrfp);
						tra.ns = ntau;
						tra.dt = dtau * 1000000;
						tra.d1 = dtau * 1000000;
						if(tra.trid==0) {
							tra.trid=2;
							if(cdpnum==0) {
                             tra.cdp = cdp1+(iy+iy0)*ncdppl*cdpinc
                                           +(ix+ix0)*cdpinc;
                        	} else {
                             tra.cdp = cdp1+(iy+iy0)*cdpinc
                                           +(ix+ix0)*nlines*cdpinc;
                        	}
						}
						if(dz>0.) {
							tra.dz = dz;
							tra.fz = 0;
							tra.mute = 0;
							tra.mutb = 0;
							if(dz<=32.) {
								tra.dt = dz * 1000.;
							} else {
								tra.dt = dz;
							}
						}
						tra.tracl = ix + 1;
						tra.tracr = iyy + 1; 
						tra.delrt = 0;
						if(ikey==1) {
							ftrace = ix*traceinc + trstart + 0.5;
							fline = iyy*lineinc + lnstart + 0.5;
							itrace = ftrace;
							iline = fline;
							itov(trktype,&trkval,itrace);
							itov(lnktype,&lnkval,iline);
							puthval(&tra,indxtrk,&trkval);
							puthval(&tra,indxlnk,&lnkval);
						}
						fputtr(outfp,&tra);
					}
				}
			}
			fclose(xytfp);
		}
		if(isave==0 ) {
			eremove(diskxyt);
		}
    }

	fprintf(jpfp," Job Done \n");

	free(q);
	fclose(outfp);
	fclose(hdrfp);
	if(isave==0) eremove(diskhdr);
	if(isave==1) {
		if(ncp==nf) {
			xywfp = fopen(diskxyw,"w");
			fseek64(xywfp,0,0);
			fwrite(cp,sizeof(complex),nx*ny*nf,xywfp);
			fclose(xywfp);
		}
	} else {
		if(ncp!=nf) eremove(diskxyw);
	} 

	if(ibacko==1) {
		fprintf(jpfp," Backup %s , %s and %s to %s \n", 
			diskhdr,diskxyt,diskxyw,backupo); 
		tar3to(backupo,diskhdr,diskxyt,diskxyw);
	}

	return 0;
}

/* write a input fft-transformed line to xyw-disk and write trace headers of 
a line to hdr-disk */

void wl2d(int nx,int ny,int nw,int iy,complex *cp,char *trhdr,float *fold,
		FILE *xywfp, FILE *hdrfp, FILE *jpfp,
		String trktype, String lnktype,
		Value trkval, Value lnkval,
		int indxtrk, int indxlnk, int trstart, int lnstart,
		int traceinc, int lineinc, int ikey, 
		complex *cpbig, int nybig, int iw2disk) {

	int ix,kx,ntmp,iw; 
	int itmp, mybig, jy;
	long long i64;
	int iline, itrace;
	float ftrace, fline;

	/* zero fill in missing traces */
	for(ix=0;ix<nx;ix++) {
		if(fold[ix]==0.) {
			fprintf(jpfp,
			"   Missing Trace At Shotpoint is=%d Line il=%d \n",
				ix+1,iy+1);
			fflush(jpfp);
			ntmp = ix - 1;
			if(ntmp<0) ntmp = 0;
			for(kx=ntmp;kx<nx;kx++) {
				if(fold[kx]!=0.) {
					for(iw=0;iw<nw;iw++) {
				         	cp[ix+iw*nx].r = 0.;
				         	cp[ix+iw*nx].i = 0.;
					}
		           		fold[ix]=1.0;
		      			bcopy(trhdr+kx*HDRBYTES,
                        			trhdr+ix*HDRBYTES,
						HDRBYTES);
					bzero(trhdr+28+ix*HDRBYTES,2);
		      			break;
	   			}
			}
		}
	}
	/* normalize */
	for(ix=0;ix<nx;ix++) {
		if(fold[ix]>1.0) {
			for(iw=0;iw<nw;iw++) {
		         	cp[ix+iw*nx].r = cp[ix+iw*nx].r/fold[ix];
		         	cp[ix+iw*nx].i = cp[ix+iw*nx].i/fold[ix];
			}
		}
	}

	jy = iy%nybig;
	mybig = jy + 1;

/*
	fprintf(stderr," iy=%d nybig=%d jy=%d \n",iy,nybig,jy);
*/

	/* save to cpbig */
	for(ix=0;ix<nx;ix++) {
		for(iw=0;iw<nw;iw++) {
			cpbig[iw*nybig*nx+jy*nx+ix].r = cp[ix+iw*nx].r;
			cpbig[iw*nybig*nx+jy*nx+ix].i = cp[ix+iw*nx].i;
		}
	}

	/* write to disk */
	if( iw2disk==1 || mybig==nybig ) {

		itmp = iy / nybig;
		itmp = itmp * nybig;

/*
		fprintf(stderr," iy=%d nybig=%d itmp=%d mybig=%d \n",
			iy,nybig,itmp,mybig);
*/

		for(iw=0;iw<nw;iw++) {
			i64 = iw;
			i64 = i64*nx*ny+itmp*nx;
			i64 = i64*sizeof(complex);
			fseek64(xywfp,i64,0);
	   		fwrite(cpbig+iw*nx*nybig,sizeof(complex),nx*mybig,xywfp);
		}
		bzero(cpbig,nybig*nw*nx*sizeof(complex));
	}

	/* save headers to disk */
	i64 = iy*nx*HDRBYTES*sizeof(char);
	fseek64(hdrfp,i64,0);
	if(ikey==1) {
		for(ix=0;ix<nx;ix++) {
			ftrace = ix*traceinc + trstart + 0.5;
			fline = iy*lineinc + lnstart + 0.5;
			itrace = ftrace;
			iline = fline;
			itov(trktype,&trkval,itrace);
			itov(lnktype,&lnkval,iline);
			bcopy(trhdr+ix*HDRBYTES,(char*)&tra,HDRBYTES);
			puthval(&tra,indxtrk,&trkval);
			puthval(&tra,indxlnk,&lnkval);
			bcopy((char*)&tra,trhdr+ix*HDRBYTES,HDRBYTES);
		}
	}
	fwrite(trhdr,sizeof(char),nx*HDRBYTES,hdrfp);
}
