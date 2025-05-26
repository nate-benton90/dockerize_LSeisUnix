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
"FXYMERGE  - Merge of distributed fxymig runs and output migrated traces \n"
"\n"
"fxymerge [parameters] <3d-stack >migrated-3d-data			\n" 
"\n"
"Required parameters:							\n"
"diskhdr=               Disk name for storing input trace headers	\n"
"diskxyt=               Disk name for storing imaged time/depth slices  \n"
"                       repeat diskxyt=... as many times as number of \n" 
"                       distributed runs \n" 
"ns=                    number of traces per output line \n"
"nl=                    number of output lines \n"
"Optional parameters: \n"
"ntau=                  number of output samples per trace \n"
"dtau=                  output time sample interval (in ms) \n"
"                       (ntau and dtau default to values in input trace) \n"
"dz=0                   output depth sample interval (in m or ft) \n"
"mlimit=1024             Memory limit (in megabytes) to run the program  \n"
"jpfile=                Job print file name             \n"
"                       default: the standard error message output (screen) \n"
"                       specified: the message will be printed in the file \n"
"diskxytsum=            Disk name for storing summed imaged time/depth slices\n"
"                       if specified, the summed image slices is stored \n"
"                       Recommend to use for large dataset \n"
"\n"
"Notes:									\n"
"1. Input stack must be ASCII-IEEE SEGY data format				\n"
"2. There are ns by nl traces output, regardless of input traces.	\n"
"   Trace number within line (starting from 1 with increment of 1) will \n"
"   be updated in trace header word 'tracl', while line number (starting\n"
"   from 1 with increment of 1) will be updated in trace header word	\n" 
"   'tracr'. Input missing trace position will be marked trid=2 in the \n" 
"   output Other output header values at the missing trace may not be 	\n"
"   correct, since they are simply copied from the nearest live input trace.\n" 
"\n"
"AUTHOR:		Zhiming Li,        	1/99   			\n"
;

segytrace tra;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
   	int nt,nx,ny,ntau,it,ix,iy;
   	float dt,dtau,dz;
	float *q, *qread;
   	char *diskhdr, *jpfile;
   	FILE *infp,*outfp,*hdrfp,*jpfp;
   	string *diskxyt;
   	char *diskxytsum;
   	FILE **xytfp, *xytsumfp;
	int ni, i, itmp, ixytsum;

	int nyread, nyy, iyread, iyy, jy0; 
	int mlimit;

	long long i64;
    long long ixytsize,ihdrsize;
	

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

	/* open input, output data sets */
	infp = stdin;
	outfp = stdout;
	
   	if (!getparstring("jpfile",&jpfile)) {
		jpfp = stderr;
	} else {
		jpfp = efopen(jpfile,"w");
	}

	/* perform a fseek64 to force large file options in fread and fwrite */

	fseek64(infp,0,1);
	fseek64(outfp,0,1);

	/* required parameters */
   	if (!getparint("ns",&nx)) err(" ns missing \n");
   	if (!getparint("nl",&ny)) err(" nl missing \n");
 
	if(ny==0) ny=1;  

	/* read id headers */
    fgethdr(infp,&ch,&bh);

	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tra))  err("can't get first trace");
    nt = tra.ns;
    dt = (float)tra.dt/1000000.;

    if (!getparint("ntau",&ntau)) ntau = nt;
    if (!getparfloat("dtau",&dtau)) {
		dtau = dt;
	} else {
		dtau = dtau*0.001;
	}
    if (!getparfloat("dz",&dz)) dz = 0.;
	
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

	fputhdr(outfp,&ch,&bh);

    if (!getparint("mlimit",&mlimit)) mlimit = 1024;
    mlimit = mlimit * 1024 * 1024;

	if (!getparstring("diskhdr",&diskhdr))
		err(" diskhdr missing ");
	i64 = 0;
	hdrfp = fopen(diskhdr,"r");
	fseek64(hdrfp,i64,SEEK_END);
	ihdrsize= ftell64(hdrfp);
	fseek64(hdrfp,0,0);
	itmp = ihdrsize/(nx*ny);
	if(itmp!=240) err("check file size of %s \n",diskhdr);


	ni = countparname("diskxyt");
	if (ni<1) err(" number of inputs %d ust be greater than 0",ni);

	xytfp = (FILE **)emalloc(ni*sizeof(FILE *));
	diskxyt = (string *) emalloc(ni*sizeof(string));

	fprintf(jpfp," %d sets of disk files open ... \n", ni);

	for(i=0;i<ni;i++) {
		if(!getnparstring(i+1,"diskxyt",&diskxyt[i]))
			err("cannot get diskxyt file for %i -th input \n",i+1);
		xytfp[i] = fopen(diskxyt[i],"r");
		fprintf(jpfp," %s disk file open ... \n", diskxyt[i]);
		i64 = 0;
		ixytsize = 0;
		fseek64(xytfp[i],i64,SEEK_END);
		ixytsize= ftell64(xytfp[i]);
		fseek64(xytfp[i],0,0);
		itmp = ixytsize/(nx*ny*sizeof(float));
		if(itmp != ntau) err("check file size of %s \n",diskxyt[i]);
	}

	ixytsum = 0;
	if(getparstring("diskxytsum",&diskxytsum)) {
		xytsumfp = fopen(diskxytsum,"w");
		fseek64(xytsumfp,0,0);
		ixytsum = 1;
		q = (float*)emalloc(ny*nx*sizeof(float));
		qread = (float*)emalloc(ny*nx*sizeof(float));
		for(i=0;i<ni;i++) fseek64(xytfp[i],0,0);
		fprintf(jpfp," Start Sum of Image Slices ...\n");
		for(it=0;it<ntau;it++) {
			for(ix=0;ix<nx*ny;ix++) q[ix] = 0.;
			for(i=0;i<ni;i++) {
				fread(qread,sizeof(float),nx*ny,xytfp[i]);
				if(i==0 && it>828) {
					for(ix=0;ix<nx*ny;ix++) qread[ix] = 0.;
				}
				for(ix=0;ix<nx*ny;ix++) {
					q[ix] += qread[ix];
				}
			}			
			if(it>828) {
				for(ix=0;ix<nx*ny;ix++) q[ix] *= 1.25;
			}
			fwrite(q,sizeof(float),nx*ny,xytsumfp);
		}
		fprintf(jpfp," Sum of Image Slices Done");
		free(q);
		free(qread);
		fclose(xytsumfp);
		xytsumfp = fopen(diskxytsum,"r");
	}


	fprintf(jpfp," Start Output \n");
	fprintf(jpfp," ============ \n");

/*
	if(mlimit>1024*1024*1024) {
		nyy = 1024*1024*1024 / (nx*ntau*sizeof(float)) ;
	} else {
		nyy = mlimit / (nx*ntau*sizeof(float)) ;
	}
*/
	nyy = mlimit / (nx*ntau*sizeof(float)) ;
	
	if(nyy<1) nyy=1;
	q = (float*)emalloc(nyy*nx*ntau*sizeof(float));
	qread = (float*)emalloc(ny*nx*sizeof(float));

	for (iy=0;iy<ny;iy=iy+nyy) {
		nyread = nyy;
		if(iy+nyread>ny) nyread = ny - iy;
		bzero((char*)q,nyy*nx*ntau*sizeof(float));

		if(ixytsum==1)	fseek64(xytsumfp,0,0);

		for(it=0;it<ntau;it++) {
			i64 = it;
			i64 = i64*nx*ny+iy*nx;
			i64 = i64*sizeof(float);
			if(ixytsum==0) {
				for(i=0;i<ni;i++) {
					fseek64(xytfp[i],i64,0);
					fread(qread,sizeof(float),nx*nyread,xytfp[i]);
					for(ix=0;ix<nx*nyread;ix++) { 
						q[it*nx*nyy+ix] += qread[ix];
					}					
				}
			} else {
				fread(qread,sizeof(float),nx*ny,xytsumfp);
				iyy = iy*nx;
				for(ix=0;ix<nx*nyread;ix++) { 
					q[it*nx*nyy+ix] += qread[ix+iyy];
				}					
				/*
				fseek64(xytsumfp,i64,0);
				fread(qread,sizeof(float),nx*nyread,xytsumfp);
				for(ix=0;ix<nx*nyread;ix++) { 
					q[it*nx*nyy+ix] += qread[ix];
				}					
				*/
			}
		}

		for (iyread=0;iyread<nyread;iyread++) {
			iyy = iy + iyread;
			jy0 = iyread * nx;
			for (ix=0;ix<nx;ix++) {
				for (it=0;it<ntau-1;it++) 
					tra.data[it+1] = q[it*nx*nyy+jy0+ix];
				tra.data[0] = 0.;
				fread(&tra,sizeof(char),HDRBYTES,hdrfp);
				tra.ns = ntau;
				tra.dt = dtau * 1000000;
				tra.d1 = dtau * 1000000;
				if(tra.trid==0) {
					tra.trid=2;
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
				fputtr(outfp,&tra);
			}
		}
	}

	fprintf(jpfp," Job Done \n");

	free(q);
	free(qread);
	fclose(outfp);
	fclose(hdrfp);

	return 0;
}
