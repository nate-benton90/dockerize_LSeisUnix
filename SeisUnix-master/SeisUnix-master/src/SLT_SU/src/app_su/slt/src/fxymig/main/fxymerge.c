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
"ns=                    number of traces per output line in fxymig \n"
"nl=                    number of output lines in fxymig \n"
"Optional parameters: \n"
"tracekey=              segy key word defining trace number within line \n"
"linekey=               segy key work defining line number              \n"
"trstart=               starting trace (inline) number to migrate       \n"
"traceinc=1             trace number increment to migrate  \n"
"lnstart=               starting line number to migrate \n"
"lineinc=1              line number increment to migrate                \n"
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
"skipsum=0              =1 will skip summing of all diskxyt files, \n"
"                       assuming diskxytsum was updated last time \n"  
"                       and the job aborted since the update \n"
"                       =0 will sum of all diskxyt files, \n"
"                       regardless if diskxytsum was updated or not before. \n" 
"                       After summing and storing at diskxytsum, \n"
"                       the program will start to output traces. \n"
"                       =-1 will sum of all diskxyt files, \n"
"                       regardless if diskxytsum was updated or not before. \n" 
"                       After summing and storing at diskxytsum, \n"
"                       the program will NOT output any traces. Useful  \n"
"                       when multiple machines will be used to output \n"
"                       different portion of the fxymig volume with \n"
"                       the following two parameters ilstart and ilend. \n" 
"ilstart=1              squentail starting line number index to output \n"
"ilend=nl               squentail ending line number index to output \n"
"                       ilstart and ilend can be used only when skipsum=1 \n"
"                       (image slices were already stored at diskxytsum). \n"
"                       They can be used to allow multiple machines to \n"
"                       output different portions of the migration result. \n"
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
	int skipsum=0;
	int ilstart,ilend;
	int iy0, iyn;

	long long i64;
    	long long ixytsize,ihdrsize;

        String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk, trstart, lnstart;
	int traceinc, lineinc, ikey=0, iline, itrace;
	float ftrace, fline;

	

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
   	if (!getparint("skipsum",&skipsum)) skipsum = 0;
   	if (!getparint("ilstart",&ilstart)) ilstart = 1;
   	if (!getparint("ilend",&ilend)) ilend = ny;

	/* optional parameters */
	if ( getparstring("tracekey",&tracekey) && getparstring("linekey",&linekey) ) {
		ikey = 1;
		trktype = hdtype(tracekey);
		lnktype = hdtype(linekey);
		indxtrk = getindex(tracekey);
		indxlnk = getindex(linekey);
		if (!getparint("traceinc",&traceinc)) traceinc=1;
		if (!getparint("lineinc",&lineinc)) lineinc=1;
		if (!getparint("trstart",&trstart)) err(" trstart missing");
		if (!getparint("lnstart",&lnstart)) err(" lnstart missing");
	}

	if(skipsum!=1 && ilstart!=1 && ilend!=ny) 
	err(" skipsum must be 1 for ilstart and ilend options \n"); 
 
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


	if(skipsum!=1) {
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
			fprintf(jpfp," %s size checked \n",diskxyt[i]);
		}
	}

	ixytsum = 0;
	if(getparstring("diskxytsum",&diskxytsum)) {
		if(skipsum!=1) {
			xytsumfp = fopen(diskxytsum,"w");
			fseek64(xytsumfp,0,0);
			q = (float*)emalloc(ny*nx*sizeof(float));
			qread = (float*)emalloc(ny*nx*sizeof(float));
			for(i=0;i<ni;i++) fseek64(xytfp[i],0,0);
			fprintf(jpfp," Start Sum of Image Slices ...\n");
			for(it=0;it<ntau;it++) {
				for(ix=0;ix<nx*ny;ix++) q[ix] = 0.;
				for(i=0;i<ni;i++) {
					fread(qread,sizeof(float),nx*ny,xytfp[i]);
					for(ix=0;ix<nx*ny;ix++) {
						q[ix] += qread[ix];
					}
				}			
				fwrite(q,sizeof(float),nx*ny,xytsumfp);
			}
			fprintf(jpfp," Sum of Image Slices Done");
			free(q);
			free(qread);
			fclose(xytsumfp);
			xytsumfp = fopen(diskxytsum,"r");
		} else {

			xytsumfp = fopen(diskxytsum,"r");
			i64 = 0;
			ixytsize = 0;
			fseek64(xytsumfp,i64,SEEK_END);
			ixytsize= ftell64(xytsumfp);
			fseek64(xytsumfp,0,0);
			itmp = ixytsize/(nx*ny*sizeof(float));
			if(itmp != ntau) err("check file size of %s \n",diskxytsum);
			fprintf(jpfp," diskxytsum size checked \n");
		}
		ixytsum = 1;
	}


	if(skipsum==-1) {
		fprintf(jpfp," All diskxyt summed to %s \n",diskxytsum);
		fprintf(jpfp," Will not output any trace due to skipsum=%d \n",skipsum);
		return 0;
	}
	fprintf(jpfp," Start Output \n");
	fprintf(jpfp," ============ \n");
	fprintf(jpfp," starting line index to output=%d \n",ilstart);
	fprintf(jpfp," ending line index to output=%d \n",ilend);

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

	iy0 = ilstart - 1;
	iyn = ilend;

	for (iy=iy0;iy<iyn;iy=iy+nyy) {
		nyread = nyy;
		if(iy+nyread>iyn) nyread = iyn - iy;
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
				/*
				fread(qread,sizeof(float),nx*ny,xytsumfp);
				iyy = iy*nx;
				for(ix=0;ix<nx*nyread;ix++) { 
					q[it*nx*nyy+ix] += qread[ix+iyy];
				}					
				*/
				fseek64(xytsumfp,i64,0);
				fread(qread,sizeof(float),nx*nyread,xytsumfp);
				for(ix=0;ix<nx*nyread;ix++) { 
					q[it*nx*nyy+ix] += qread[ix];
				}					
			}
		}

		i64 = iy*nx;
		i64 = i64*240;
		fseek64(hdrfp,i64,0);

		for (iyread=0;iyread<nyread;iyread++) {
			iyy = iy + iyread;
			jy0 = iyread * nx;
			for (ix=0;ix<nx;ix++) {
				for (it=0;it<ntau-1;it++) 
					tra.data[it+1] = q[it*nx*nyy+jy0+ix];
				tra.data[0] = 0.;
				fread(&tra,sizeof(char),HDRBYTES,hdrfp);
				tra.ns = ntau;
				tra.dt = (int) ( dtau * 1000000 );
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
						tra.dt = (int) (dz * 1000.);
					} else {
						tra.dt = (int) dz;
					}
				}
				tra.tracl = ix + 1;
				tra.tracr = iyy + 1;
				tra.delrt = 0;
				if(tra.ns!=ntau) fprintf(stderr," ix=%d iy=%d trace.ns=%d ntau=%d \n",
					ix+1,iyy+1,tra.ns,ntau);
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

	fprintf(jpfp," Job Done \n");

	free(q);
	free(qread);
	fclose(outfp);
	fclose(hdrfp);

	return 0;
}
