
#include "usu.h"
#include "subc.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "usgrid.h"
#include "comva.h"
#include "su.h"
#include "segy.h"
#include "header.h"
#include "cwp.h"

char *sdoc[] = {
"KZMIG - Kirchhoff depth Migration of 3D poststack/prestack data 	",
" ",
"kzmig [parameters] <input-data >migrated-data				", 
" ",
"Required parameters:							",
"3D master grid parameters:						", 
"x1=                    x coordinate of 1st corner of the 3D master grid",
"y1=                    y coordinate of 1st corner of the 3D master grid",
"s1=                    s coordinate of 1st corner of the 3D master grid",
"l1=                    l coordinate of 1st corner of the 3D master grid",
"cdp1=                  cdp number of 1st corner of the 3D master grid  ",
"x2=                    x coordinate of 2nd corner of the 3D master grid",
"y2=                    y coordinate of 2nd corner of the 3D master grid",
"s2=                    s coordinate of 2nd corner of the 3D master grid",
"l2=                    l coordinate of 2nd corner of the 3D master grid",
"cdp2=                  cdp number of 2nd corner of the 3D master grid  ",
"x3=                    x coordinate of 3rd corner of the 3D master grid",
"y3=                    y coordinate of 3rd corner of the 3D master grid",
"s3=                    s coordinate of 3rd corner of the 3D master grid",
"l3=                    l coordinate of 3rd corner of the 3D master grid",
"cdp3=                  cdp number of 3rd corner of the 3D master grid  ",
"                       (See Note 1 for details)			",
" ",
"dds=                   cdp spacing in inline direction 		",
"ddl=                   Line spacing in crossline 			",
 " ",
"Optional parameters:							",
"ds=dds                 cdp group interval to output			",
"dl=ddl                 Line group  interval to output 			",
"datain=                input data set name (instead of standard input) ",
"dataout=               output data set name (instead of standard output)",
"ttfile=ttfile          file name of input traveltime tables		",
"fzt=from-ttfile        depth of first sample in traveltime tables 	" ,
"nzt=from-ttfile       	Number of depth samples in traveltime tables 	",
"dzt=from-ttfile       	depth interval in traveltime tables		",
"fst=from-ttfile       	first s coordinate in traveltime tables 	", 
"nst=from-ttfile       	Number of s coordinates in traveltime tables	",
"dst=from-ttfile        s interval in traveltime tables			",
"flt=from-ttfile       	first l coordinate in traveltime tables 	", 
"nlt=from-ttfile       	Number of l coordinates in traveltime tables	",
"dlt=from-ttfile        l interval in traveltime tables			",
"fxst=from-ttfile       first s coordinate of source locations in 	",	
"				traveltime tables 	", 
"nxst=from-ttfile       Number of s coordinates of source locations in 	",	
"				traveltime tables 	", 
"dxst=from-ttfile       s interval of source locations in traveltime 	",
"				tables			",
"fyst=from-ttfile       first l coordinate of source locations in 	",	
"				traveltime tables 	", 
"nyst=from-ttfile       Number of l coordinates of source locations in 	",	
"				traveltime tables 	", 
"dyst=from-ttfile       l interval of source locations in traveltime 	",
"				tables			",
"THE ABOVE 15 PARAMETERS ARE READ IN FROM TTFILE ONLY			",
"fzo=fzt                first output depth for migration 		",
" Or ",
"hzgrid=                horizon grid name to specify the starting depth ",
"                         of output depth migraiton. ", 
"                         2D grid file obtained from lmk2grid, gridop2 and ",
"                         gridheader programs. ",
"                         The coordinates (o1,o2,d1,d2) in the grid ",
"                         header of hzgrid must be in the processing ",
"                         coordinates (s,l). ", 
"nzo=10*nzt             Number of output migrated samples per trace	", 
"dzo=0.1*dzt            Output migrated depth sample interval  		",
"                       						",
"flexbin=0              =1 Input data have been flexbinned to normalize	",
"                        amplitudes before migration, the migration will",
"                        not scale the summation by the migration fold	",
"                       =0 Otherwise, use migration fold to normalize 	",
"                        output 					",
"oofo=0.                output minimum offset (>0.)			", 
"dofo=999999.           output offset increment				", 
"obtol=.5*dofo          offset bin tolerance (distance between the ",
"                       accepted input offset and the output offset ",
"                       (default to half of the output offset increment). ",
"                       useful for migrating every other offset:         ",  
"                       e.g.: When dofo=400 and input offset increment ",
"                       is 200, specify obtol=100 (instead of 200) ",
"                       kzmig will migrated only those offsets  ", 
"                       whose distance from the output offset bin center ",
"                       is equal or less than 100.  ",
"nofo=1                 number of output offsets			",
"ofo=0.                 output offsets  (specified as 200,250.,350,600,...) ",
"                       if specified, dofo and oofo will be ignored	", 
"offnear=0              1=any offset (absolute) outside first offset bin ",
"                         is used in migration and summed to min offset", 
"                       0=otherwise will be rejected 			", 
"offfar=0               1=any offset (absolute) outside last offset bin ",
"                         is used in migration and summed to max offset", 
"                       0=otherwise will be rejected 			", 
"sstart=s1              Minimum s (trace position within line) of 	",
"                       migration output				",
"                       (NOTE: this is coordinates, i.e., first trace	",
"                       of a line at s1, 2nd at s1+ds, 3rd at s1+2*ds, ...)",
"ns=(s2-s1)/ds+1        Number of cdp (group) per line of migration output ",
"lstart=l1              Minimum l (line position) of migration output 	",
"                       (NOTE: this is coordinates, i.e., first line	",
"                       l1, 2nd at l1+dl, 3rd at l1+2*dl, ...)		",
"nl=(l3-l1)/dl+1        Number of lines (group) of migration output 	",
"send=                  Ending s position of migration output		",
"lend=                  Ending l position of migration output           ", 
"ntrend=                Number of cdp bins to output from (sstart,lstart) ",
"                       to (send,lend)					", 
"                       (specify send, lend and ntrend will output only ",
"                       one slant line from (sstart,lstart) to 		",
"                       (send,lend) of total ntrend cdps; if not 	",
"                       specified, will output to ns by nl cdps)	", 
"nds=1                  Number of cdp locations per output cdp group	",
"                       (for migration velocity analysis (MVA))   ",
"ndl=1                  Number of line locations per output line group  ",
"mlimit=256             Memory limit (in megabytes) to run the program  ",
"gath=0	                type of gather: 1--common shot; 2--commom 	",
"                       receiver; 0 --other types 		",
"mtrace=1000            maximum number of traces per migration step 	",
"offmax=6000            Maximum input offset (default in m)		", 
"apers=smax/4.          Migration aperture in inline (s) direction (the",
"                       maximum lateral distance from s position of 	",
"                       the input midpoint to s position of output midpoint),",
"                       where smax is the maximum of lateral distance 	",
"                       in inline direction				",
"aperl=lmax/4.          Migration aperture in crossline (l) direction (the",
"                       maximum lateral distance from l position of 	",
"                       the input midpoint to l position of output midpoint),",
"                       where lmax is the maximum of lateral distance 	",
"                       in crossline direction				",
"angmax=60.             maximum angle (from vertical) in degrees to output ",
"                       migration from an input trace, i.e.,            ",
"                          tmin = 2*dis/(v0*tan(angmax*3.141592654/180.)) ",
"                       where tmin is the minimum time of an output trace, ",
"                       dis is the distance between the output midpoint ",
"                       and input midpoint, v0 is the surface velocity at ",
"                       input midpoint position 			", 
"v0=1500                reference velocity at the surface. (default m/s) ",	
"dvz=0.2                reference vertical velocity gradient (default m/s/m ",
"tpow=-1.0              power of time gain to be appled to the input    ",
"                       before migration   ",
"zpow=1.0               power of depth gain to be appled to the output  ",
"                       after migration   ",
"isave=0                Mode of saving diskimg, diskhdr and diskfld	",
"                       0=remove the datasets after completion        	",
"                       1=save the datasets after completion          	", 
"                       (will be sent to 1 when backupo is specified)	",
"diskimg=DISKIMAGE      Disk name for storing imaged time sections 	",
"diskhdr=DISKHEADER     Disk name for storing output trace headers  	",
"diskfld=DISKFOLD       Disk name for storing output trace folds  	",
"backupi=               Name of backup tape name of the previous run	",
"                       (if given, e.g., stssxyz.kzmig.bck1, the backup ",
"                        will be read in to overwrite the disk file of	",
"                        diskimg, diskhdr and diskfld; 			",
"                        otherwise, the program will use diskimg, 	",
"                        diskhdr and diskfld, if present)		",
"backupo=               Name of backup tape name of the current run	",
"                       (if given, e.g., stssxyz.kzmig.bck2, the backup ",
"                        tape will be made after the job is done;	",
"                        otherwise, only disk files will be created 	",
"                        if isave=1) 					",
"tapecntl=0             Input control (for tape or pipe or disk input)	",
"                       0=tape or pipe input ",
"                         The program will read and not process until 	",
"                         the input trace counter exceeds the number of ",
"                         traces process in the previous run of same input",
"                       1=more than one disk input 	",
"                         The program will skip the number of traces ",
"                         processed in the previous run of same input ",
"                         before processing ",
"                         This option should be used when multiple runs ",
"                         (each with a single input) are needed ",
"                      -1=only one disk file input ",
"                         The program will skip the total number of traces",
"                         processed in the previous run before processing ",
"                       2=input from pipe, but processing will start from ",
"                         first trace (like sudecomp input)",
"ntras=2000000000       Maximum number of traces to process in the 	",
"                       current run, before end of the input		",
"hisfile=KZMIG.HISFILE  histroy file name to indicate how many traces   ",
"                       has been migrated in total (all runs) and in	",
"                       current run ",
"                       (warning: remove this file before starting 	",
"                        migration if this is not a retart job, i.e.,   ",
"                        the migration starts at the first input trace) ",
"                       If user does not specify hisfile, KZMIG.HISFILE ",
"                       will be removed after normal completion of job. ",
"f0=10.0                first high cut frequecy (Hz) in anti-aliasing 	",
"                       filters						",
"df=5.0                 high cut frequecy (Hz) increment in anti-aliasing",
"                       filters						",
"nf=12                  number of anti-aliasing filters			",
"ftaper=5.0             frequency taper length (Hz) in the filters	",
"ksmax=0.5/dds          maximum wavenumber in line			",
"klmax=0.5/ddl          maximum wavenumber cross line			",  
"                       Note the default maximum xline (l) wavenumber ",
"                       is twice of the Nyquist ",
"rmsmar=50.             maximum allowable ratio between sample amplitude",
"                       and the rms value of amplitudes of the whole 	",
"                       trace. When input amplitude/rms exceeds this 	",
"                       value, the trace will be treated as noise trace,",
"                       and not used in migration. A warning message 	", 
"                       will then be printed to a file (default to 	",
"                       BAD_TRACE_FILE)					",
"badtracefile=          name of file to record bad traces in the input	",
"                       (default to BAD_TRACE_FILE)			",
"jpfile=                Job print file name                             ",
"                          default: the standard error unit             ",
"                            (a) create prefix.exxxx file if using qsub ",
"                                on convex, where prefix is the first   ",
"                                7 characters of the script file name   ",
"                                and xxxx is the job number;            ",
"                            (b) send e-mail when using at command;     ",
"                            (c) send printed message to screen when    ",
"                                running the job without qsub or at     ",
"                                commands.                              ",
"                          specified: the message will be printed       ",
"                            in the named file as the job goes          ",
"ntrdsk=                number of traces migrated per disk file update  ",
"                       (diskimg and diskfld will be updated once for   ",
"                        every migrated ntrdsk traces;                  ",
"                        default to the end of the job)                 ",
"ncpu=1                 number of cpu to use in the run 		",
"                       (used in kzmigp program, ignored in kzmig)	",
"traceout=1             output migrated seismic trace (1=yes 0=no) ",  
"                       (traceout=0 may be used to skip output, if ",
"                        more input dataset is going to be migrated ", 
"                        in the next run) ",
"on2trace=0             total number of traces in the input is ",
"                       specified at the first 4 bytes of the trace ",
"                       header of the first trace (0=no 1=yes) ",
"tr1=                   trace number at 1st corner of the 3D master grid",
"tr2=                   trace number at 2nd corner of the 3D master grid",
"tr3=                   trace number at 3rd corner of the 3D master grid",
"ln1=                   line number at 1st corner of the 3D master grid",
"ln2=                   line number at 2nd corner of the 3D master grid",
"ln3=                   line number at 3rd corner of the 3D master grid",
"tracekey=              segy trace header key word for trace number ",
"linekey=               segy trace header key word for line number ",
"strace=0               when (trace#-strace)%jthtrace==0, pass to migration",
"                       otherwise skip ",
"jthtrace=1             pass every jth-trace to migration ", 
"mintrace=-2000000000   minimum input trace number to migrate ",
"maxtrace=2000000000    maximum input trace number to migrate ",
"sline=0                when (line#-sline)%jthline==0, pass to migration",
"                       otherwise skip ",
"jthline=1              pass every jth-line to migration ", 
"minline=-2000000000    minimum input line number to migrate ",
"maxline=2000000000     maximum input line number to migrate ",
"outputonly=0           when outputonly=1, it will not process any traces,",
"                       and will start to output from previous saved ",
"                       disk files. Use when the output failed last time. ",
" ",
"Notes:		",
"0. 1st corner of the 3D master grid is at minimum line and minimum trace",
"   2nd corner of the 3D master grid is at minimum line and maximum trace",
"   3rd corner of the 3D master grid is at maximum line and minimum trace",
"   when tracekey, linekey, tr1, tr2, tr3, ln1, ln2 and ln3 are given,",
"   the output trace headers will be updated at missing input trace location",
"   (trid=2) lwith line and trace numbers",
"1. Input data must be SEGY (IEEE) data format				",
"2. Input ttfile consists of nzt*nst*nlt*nxst*nyst floating point samples ",
"   with 100-byte of grid header					", 
"3. s is the lateral position along the inline direction, l is the 	",
"   lateral position along the crossline direction. They may be different",
"   from the (x,y) position of the 3D survey. 				",
"4. source (sx,sy) and receiver (gx,gy) coordinates are used to determine",
"   the midpoint x and y position on the 3D grid:			",
"		x = (gx+sx)/2 * scalco					",
"		y = (gy+sy)/2 * scalco		when scalco>1		", 
"    OR:								",
"		x = (gx+sx)/2 / (-scalco)	when scalco<0		",
"		y = (gy+sy)/2 / (-scalco)				",
"   where gx is the receiver x coordinate from the trace header 	",
"    	  sx is the source x coordinate from the trace header 		",
"         gy is the receiver y coordinate from the trace header 	",
"    	  sy is the source y coordinate from the trace header 		",
"         scalco is the scaler to be applied to gx,gy,sx,sy		",
"5. To migrate the 3-D dataset in stages (so that you get backups of    ",
"   each stage), use ntras, diskimg, diskhdr, diskfld papameters	",
"   if the input dataset is stored as disk file,			",
"   for example,							",
"	stage 1: migrate 50000 traces 					",
"	   kzmig ... ntras=50000 isave=1 diskimg=/huge/id/tmpimg.data 	",
"                    diskhdr=/huge/id/tmphdr.data			", 
"                    diskfld=/huge/id/tmpfld.data			", 
"                    backupo=stssxyz.kzmig.backup1 hisfile=hf ...	",
"	stage 2: migrate next 50000 traces 				",
"	   kzmig ... ntras=50000 isave=1 diskimg=/huge/id/tmpimg.data 	",
"                    diskhdr=/huge/id/tmphdr.data			",
"                    diskfld=/huge/id/tmpfld.data			", 
"                    backupi=stssxyz.kzmig.backup1 hisfile=hf		",
"                    backupo=stssxyz.kzmig.backup2 ...			",
"	stage 3: migrate  the final 10000 traces 			",
"	   kzmig ... ntras=10000 isave=0 diskimg=/huge/id/tmpimg.data 	",
"                    diskhdr=/huge/id/tmphdr.data			",
"                    diskfld=/huge/id/tmpfld.data			", 
"                    backupi=stssxyz.kzmig.backup2 hisfile=hf		",
" ",
"   If a job was completed normally at last stage, backupi should not	",
"   be specified unless diskimg, diskhdr and diskfld are bad, to save	",
"   the time of reading these three datasets into disk.			",
"               							",
"6. To migrate the 3-D dataset in stages (so that you get backups of    ",
"   each stage), use diskimg, diskhdr, diskfld papameters and taperead	",
"   if the input dataset is stored as tapes,				",
"   for example,							",
"	stage 1: migrate first 5 tapes 					",
"          taperead sti=1 ntp=5 ... |					", 
"	   kzmig ... isave=1 diskimg=/huge/id/tmpimg.data 		",
"                    diskhdr=/huge/id/tmphdr.data			", 
"                    diskfld=/huge/id/tmpfld.data			", 
"                    backupo=stssxyz.kzmig.backup1 ...			",
"	stage 2: migrate next 5 tapes 					",
"          taperead sti=6 ntp=5 ... |					", 
"	   kzmig ... isave=1 diskimg=/huge/id/tmpimg.data 		",
"                    diskhdr=/huge/id/tmphdr.data			", 
"                    diskfld=/huge/id/tmpfld.data			", 
"                    backupi=stssxyz.kzmig.backup1 			",
"                    backupo=stssxyz.kzmig.backup2 ...			",
"	stage 3: migrate  the final 10 tapes 				",
"          taperead sti=11 ntp=10 ... |					",
"	   kzmig ... isave=0 diskimg=/huge/id/tmpimg.data 		",
"                    diskhdr=/huge/id/tmphdr.data			", 
"                    diskfld=/huge/id/tmpfld.data			",
"                    backupi=stssxyz.kzmig.backup2 			",
" ",
"   If a job was completed normally at last stage, backupi should not	",
"   be specified unless diskimg, diskhdr and diskfld are bad, to save	",
"   the time of reading these three datasets into disk.			",
"   Do not specify ntras and hisfile parameters in this case.		",
" ",
"7. s and l positions of an input trace are computed using the three 	",
"   master-grid corner positions					",
"									",
"        | y								",
"        |   .l        * (x4,y4)					",
"        |    .     .    .						",
"        |     .  .       .         . s					",
"       (x3,y3) *          .      .					",
"        |        .          .  .					",
"        |         .          * (x2,y2)					",
"        |          .       .						",
"        |            .  .						",
"        |             * (x1,y1)					",
"        |								",
"        |--------------------------------- x				",
"									",
"   (x1,y1) has the smallest s value and the samllest l value		",
"              (s1 is usually =0.0, l1 is usually =0.0)			",
"   (x2,y2) has the largest s value and the smallest l value		",
"              (s2 is usually =(ncdppl-1)*dds, l2 is usually =0.0)	",
"   (x3,y3) has the smallest s value and the largest l value		",
"              (s3 is usually =0.0, l3 is usually =(nlines-1)*ddl 	",
"   where 								",
"         ncdppl is the number of traces per line in the master grid	",
"         dds is the trace spacing (within a line) in the master grid	",
"         nlines is the number of lines in the master grid		",
"         ddl is the line spacing in the master grid 			",
"	  s is the coordinate (in m or ft --- NOT just an integer number)",
"              of trace position within a 3d line 			",
"         l is the coordinate (in m or ft --- NOT just an integer number)",
"              of line position within a 3d survey 			",
" 8. When doing velocity analysis, the output locations are		",
"       the output inline locations (s):				",
" sstart      (ss1),      ss1+dds,      ...,       ss1+(nds-1)*dds,	",
" sstart+ds   (ss2),      ss2+dds,      ...,       ss2+(nds-1)*dds 	",
" sstart+2*ds (ss3),      ss3+dds,      ...,       ss3+(nds-1)*dds 	",
"    ...       								",
" sstart+(ns-1)*ds (ssn), ssn+dds,      ...,       ssn+(nds-1)*dds      ",
"       the output crossline locations (l):				",
" lstart      (ll1),      ll1+ddl,      ...,       ll1+(ndl-1)*ddl,	",
" lstart+dl   (ll2),      ll2+ddl,      ...,       ll2+(ndl-1)*ddl 	",
" lstart+2*dl (ll3),      ll3+ddl,      ...,       ll3+(ndl-1)*ddl 	",
"    ...       								",
" lstart+(nl-1)*ds (lln), lln+ddl,      ...,       lln+(ndl-1)*ddl      ",
" 9. The primary memory requirement is 					",
"        ns*nds*nl*ndl*(nofo*nzo+nzt*ntab)*4 (byte)				", 
"    ntab = .5*(offmax/dxst)*(offmax/dyst)		", 
"         + 3.*sqrt( (offmax/dxst)**2 + (offmax/dyst)**2 )  ",
" 10. when nlines is less than 10, a 2.5-D rho filter is applied to 	",
"     compensate for 2D data input					",
" ",
NULL};
/* "AUTHOR:		Z. Liu and Z. Li        9/94   			", */

void migs(int nl, int ns, float **mig, int nz,
	int ktrace, int ntl, float dldt, float *trace, 
	float *tras, float dtl, float tminl, int nt, 
	float *ss, float *sl, float *gs, float *gl, 
	int *imutel, float *s, float *l,
	int *iofs, float apers, float aperl, float **fold,
	float *work1, float *work2, float *wsave, float *tracef, float *sqrtf,
	int tahd, int *ifcut, int *ltaper, float ksmax, float klmax,
	float f0, float df, int nf, float ftaper, int nsave, int nfft,
	float dz, float fz, float angmax,
	int nxs, int nys, float fxs, float fys, float dxs, float dys,
	int nzo, float *fzo, float dzo,
	int nr, float dr, float *tb, float *pb, float **ttab, int igh); 

void pszm3d_(float *trace,int *nt,float *t0,float *dt,float *sx,float *sy,
       float *gx,float *gy,int *imute,float *mig,float *x,float *y,
       int *nxy,int *nzo,float *dzo,float *fzo,float *aperx,float *apery,
       float *fold,float *f0,float *df,int *nf,float *ftaper,
       int *ifcut,int *ltaper,float *tracef,float *wsave,int *nsave,
       float *work1,float *work2,float *sqrtf,int *tahd,int *nfft,
       float *kxmax,float *kymax,float *angmax,float *ts,float *tg,
       float *tit,float *wt,float *ampt,int *nz,float *dz,float *fz,
       float *tb,float *pb,int *nr,float *dr);    

void radix_(int *nt, int *nfft);
void timeb_(int *nr, int *nz, float *dr, float *dz, float *fz, float *a,
	float *v0, float *t, float *p);
void resit_(float *t, int *nx, int *ny, int *nz, int *nz0, int *nr,
	float *dx, float *dy, float *dr, float *fx, float *fy, 
	float *x0, float *y0, float *tb);
void resit0_(float *t, int *nout, int *nz, int *nz0, int *nr, float *dr,
	float *x, float *y, float *x0, float *y0, float *tb);
void latint_(int *nx, int *ny, int *nz, float *x, float *y, float *t,
	int *n1, int *n2, int *nzt, int *n0, float *f1, float *f2, 
	float *d1, float *d2, float *tt);
void bisear_(int *nxin, int *nxout, float *xin, float *xout, int *indx);
void blinint_(int *nx, int *ny, int *nz, float *sx, float *sy, float *t0,
    float *t1, float *t2, float *t3, float *t);

float disp2l(float a, float b, float c,float x, float y);
void xy2abc(float x1, float y1, float x2, float y2,
        float *a, float *b, float *c);
float disaper(float aperx,float apery,float x1,float y1,float x2,float y2);

void bilint_(int *n1, int *nx, int *ny, float *x0, float *y0, 
	float *dx, float *dy, float *x, float *y, float *f, float *g); 

segytrace tra, tr;
segybhdr bh;
segychdr ch;


main(int argc, char **argv)
{
	int nlines, ncdppl;
   	int nt,nzo,nz,nz0,it,ix,iy;
    	FILE *infp,*outfp,*ttfp,*imgfp,*hdrfp,*hffp,*fldfp;
	float x1,x2,x3,y1,y2,y3,s1,s2,s3,l1,l2,l3;
	char *backupi="", *backupo="";
	char *diskimg="DISKIMAGE", *diskhdr="DISKHEADER";
	char *ttfile,*datain,*dataout;
	char *diskfld="DISKFOLD", *hisfile="KZMIG.HISFILE";
	int ibacko,isave, ihis=0, ipre=0;
	float ds, dl, tmp;
	int mtrace, ntrpre=0, ktrace, itrace, jtrace, ntrace, nofo;
	int ltrace;
	int *imutel, m1000, ifact, it0;
	float **mig, **fold, *tgain, *zgain,*s, *l, *trace, *tras;
 	float *ofo;
	int *off, itgain, izgain, nl, ns, ntras, is, il, io;
	int nsread, nlread;
	int mlimit, nlcore, *iofs; 
	long long llimit;
	float tpow, zpow, sstart, lstart, t0;
	float xs, ys, xg, yg, dt, dzo, fzo, *fzout, v0, dvz; 
	float *hz, sss, lll, hzz, hzmin, hzmax;
	char *hzgrid;
	FILE *hzfp;
	usghed hzgh;
	float oshz,olhz,dshz,dlhz;
	int nshz,nlhz;
	int ihzgrid=0, itr;
	float *ss, *sl, *gs, *gl, ms, ml, ofomin, dofo, obtol; 
	float *st, *lt;
	char *buf;
	int itmp, itmp2, ntl;
	float tminl, dtl, dldt, apers, aperl;
 	float f0, df, ftaper;
	int nf, nfft, *ifcut, *ltaper, nsave, tahd;
	float *work1, *work2, *tracef, *wsave, *sqrtf;
	float ddl, dds, ksmax, klmax;
	int ndl, nds, ntmp, tapecntl;
	float rmsmar, ampmax, amprms;
	char *badtracefile="BAD_TRACE_FILE";
	FILE *btfp;
	float dd, mx, my, ofs;
	int ids, idl; 
	int cdp1, cdp2, cdp3;
	int cdppds, cdppdds, cdppdl, cdppddl, cdpnum;
	int nss, nll, iix, iiy, is0, il0;
	float sinmin, sinmax, linmin, linmax; 
	float angmax; 
	char *jpfile;
        FILE *jpfp;
	int ntrend, nsout, nlout, isend;
	float send, lend, dsout, dlout, dend, *disend;
	float ps, pl;
	int iofoin=0;
	float ofol, ofor;
	int offnear, offfar, flexbin;
	int one=1; 
	int range,gath,oldheadx,oldheady,newheadx,newheady,gottrace,ready,done,
	    nr,ntab,itab,nxs0,nys0,nxs,nys,iz,ixs,iys,izi,izw,
	    nxsb,nysb,nxs0b,nys0b;
	float **ttab,*tb0,*pb0,*tb,*pb,*tt;
	float offmax,ssmin,ssmax,slmin,slmax,rmax,dr,zi,fz,fxs,fys,scale,fold0;	
	
	usghed ugh;
	int nzt,nst,nlt,nxst,nyst,ierr;
	float fzt,fst,flt,fxst,fyst,exst,eyst,dzt,dst,dlt,dxst,dyst,ezt,est,elt;
	float aa,bb,cc,dismax,tabx,taby;
	float ltmin, ltmax, stmin, stmax;
 	
	long long lwork, ltmp;
	long long lpos, nseek;

	float tr1,tr2,tr3,ln1,ln2,ln3,fline,ftrace;
	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;
	int intline, inttrace, ikey;
	int strace, jthtrace;
	int mintrace, maxtrace;
	int minline, maxline;
	int sline, jthline;


	int ntrdsk, ndskwt;

	int ncpu, traceout;
	char *envs;
	char *datetime;
	int on2trace, ntotal;
	int outputonly;

   	/* get parameters */
   	initargs(argc,argv);
	requestdoc(1);

	/* open input and output data sets */
    	if (!getparstring("datain", &datain)) {
		infp = stdin;
	} else {
    		infp = efopen(datain,"r");
	}
    	if (!getparstring("dataout", &dataout)) {
		outfp = stdout;
	} else {
    		outfp = efopen(dataout,"w");
	}
	fseek2g(infp,0,1);
    fseek2g(outfp,0,1);

	/* required parameters */
    	if (!getparfloat("x1",&x1)) err("must specify x1");
    	if (!getparfloat("y1",&y1)) err("must specify y1");
    	if (!getparfloat("s1",&s1)) err("must specify s1");
    	if (!getparfloat("l1",&l1)) err("must specify l1");
    	if (!getparint("cdp1",&cdp1)) err("must specify cdp1");
    	if (!getparfloat("x2",&x2)) err("must specify x2");
    	if (!getparfloat("y2",&y2)) err("must specify y2");
    	if (!getparfloat("s2",&s2)) err("must specify s2");
    	if (!getparfloat("l2",&l2)) err("must specify l2");
    	if (!getparint("cdp2",&cdp2)) err("must specify cdp2");
    	if (!getparfloat("x3",&x3)) err("must specify x3");
    	if (!getparfloat("y3",&y3)) err("must specify y3");
    	if (!getparfloat("s3",&s3)) err("must specify s3");
    	if (!getparfloat("l3",&l3)) err("must specify l3");
    	if (!getparint("cdp3",&cdp3)) err("must specify cdp3");
    	if (!getparfloat("dds",&dds)) err("Must specify dds!\n");
    	if (!getparfloat("ddl",&ddl)) ddl = 0.;
 
	/* optional parameters */
    	if (!getparfloat("sstart",&sstart)) sstart = s1;
    	if (!getparfloat("lstart",&lstart)) lstart = l1;
    	if (!getparfloat("ds",&ds)) ds = dds;
    	if (!getparfloat("dl",&dl)) dl = ddl;
    	if (!getparint("ndl",&ndl)) ndl = 1; if(ndl<1) ndl=1;
    	if (!getparint("nds",&nds)) nds = 1; if(nds<1) nds=1;
    	if (!getparfloat("angmax",&angmax)) angmax = 60.;

	if(ds<dds) err(" dds larger than ds ");
	if(dl<ddl) err(" ddl larger than dl ");

	if(dds>0.) {
		tmp = (s2-s1)/dds + 1.5;
		ncdppl = tmp;
	} else {
		ncdppl = 1;
 	}

	if(ddl!=0.) {
		tmp = (l3-l1)/ddl + 1.5;
		nlines = tmp;
	} else {
		nlines = 1;
	}
 	if(nlines<10) {
		tahd = 1;
		fold0 = 10.;
	}
	else {
		tahd = 0;
		fold0 = 100.;
	}
    	if (!getparint("nl",&nlread)) nlread = nlines;
    	if (!getparint("ns",&nsread)) nsread = ncdppl;
	nl = nlread;
	ns = nsread;
    	if (dl==0. && nl>1) err("Must specify nonzero dl for migration");
    	if (ds==0. && ns>1) err("Must specify nonzeor ds for migration");

	if(dds==0.) { dds = 1.0; ds = dds; }
	if(ddl==0.) { ddl = 1.0; dl = ddl; }

	if(ncdppl>1) {
		tmp = (cdp2 - cdp1)/(ncdppl-1.) + .5;
		cdppdds = tmp;
		tmp = (cdp2 - cdp1)/(ncdppl-1.) + .5;
		cdppds = tmp;
	} else {
		cdppdds = nlines;
		cdppds = nlines;
	}
	if(cdppdds<1) cdppdds = 1;
	if(cdppds<1) cdppds = 1;
	if(nlines>1) {
		tmp = (cdp3 - cdp1)/(nlines-1.) + .5;
		cdppddl = tmp;
		tmp = (cdp3 - cdp1)/(nlines-1.) + .5;
		cdppdl = tmp;
	} else {
		cdppddl = ncdppl;
		cdppdl = ncdppl;
	}
	if(cdppddl<1) cdppddl = 1;
	if(cdppdl<1) cdppdl = 1;

	if(cdppdds!=1 && cdppddl!=1 ) {
		warn("cdppdds=%d cdppddl=%d ",cdppdds,cdppddl);
		err("check cdp1, cdp2, cdp3 ");
	}
	if(cdppdds==1) {
		cdpnum = 0;
	} else {
		cdpnum = 1;
	}


	tmp = ds/dds+0.5;
	nss  = tmp;
	tmp = dl/ddl+0.5;
	nll  = tmp;
	tmp = (sstart-s1)/dds+0.5;
	is0 = tmp;
	tmp = (lstart-l1)/ddl+0.5;
	il0 = tmp;


    	if (!getparint("mtrace",&mtrace)) mtrace = 1000;
    	if (!getparint("ntras",&ntras)) ntras = 2000000000;
	

    	if (!getparfloat("tpow",&tpow)) tpow = -1.;
    	if (!getparfloat("zpow",&zpow)) zpow = 1.;
	itgain = 0;
	izgain = 0;
	if(tpow!=0.) itgain = 1;
	if(zpow!=0.) izgain = 1;

    	if (!getparint("ifact",&ifact)) ifact = 1;
    	if (!getparfloat("apers",&apers)) apers = fabs(ns*ds)/4.;
    	if (!getparfloat("aperl",&aperl)) aperl = fabs(nl*dl)/4.;

    	if (!getparfloat("rmsmar",&rmsmar)) rmsmar = 50.;
	getparstring("badtracefile",&badtracefile);
	if( (btfp = fopen(badtracefile,"r"))!=NULL ) {
		efclose(btfp);	
		btfp = efopen(badtracefile,"a");
	} else {
		btfp = efopen(badtracefile,"w");
	}

    	if ( getparfloat("send",&send) 
    	  && getparfloat("lend",&lend) 
    	  && getparint("ntrend",&ntrend) ) {
		if(ntrend<1) err("ntrend must be greater than 0 ");
		nsout = ntrend;
		nlout = 1;
	} else {
		ntrend = 0;
		nsout = ns;
		nlout = nl;
	} 

	if(!getparint("ntrdsk",&ntrdsk)) ntrdsk = 0;
        ndskwt = 0;

	if (!getparint("ncpu",&ncpu)) ncpu = 1;
	envs = (char*) emalloc(80*sizeof(char));
        if(!getenv("PARALLEL")) {
                sprintf(envs,"%s=%d","PARALLEL",ncpu);
                putenv(envs);
        }
	if (!getparint("traceout",&traceout)) traceout = 1;
	if (!getparint("on2trace",&on2trace)) on2trace = 0;

   	if (!getparint("outputonly",&outputonly)) outputonly = 0;

	ikey = 0;
	if ( getparstring("tracekey",&tracekey)
		&& getparstring("linekey",&linekey) ) {

        ikey = 1;
		trktype = hdtype(tracekey);
		lnktype = hdtype(linekey);
		indxtrk = getindex(tracekey);
		indxlnk = getindex(linekey);
		if(!getparfloat("tr1",&tr1)) err(" tr1 missing");
		if(!getparfloat("tr2",&tr2)) err(" tr2 missing");
		if(!getparfloat("tr3",&tr3)) err(" tr3 missing");
		if(!getparfloat("ln1",&ln1)) err(" ln1 missing");
		if(!getparfloat("ln2",&ln2)) err(" ln2 missing");
		if(!getparfloat("ln3",&ln3)) err(" ln3 missing");
		if(!getparint("strace",&strace)) strace = 0;
		if(!getparint("jthtrace",&jthtrace)) jthtrace = 1;
		if(!getparint("sline",&sline)) sline = 0;
		if(!getparint("jthline",&jthline)) jthline = 1;
		if(!getparint("mintrace",&mintrace)) mintrace = -2000000000;
		if(!getparint("maxtrace",&maxtrace)) maxtrace = 2000000000;
		if(!getparint("minline",&minline)) minline = -2000000000;
		if(!getparint("maxline",&maxline)) maxline = 2000000000;
	}

	/* read id headers */
    if(outputonly==0) {
		fgethdr(infp,&ch,&bh);
	}

	/* read in first trace for nt and dt */
	if(outputonly==0) {
   		if (!fgettr(infp,&tra)) err("can't get first trace");
	} else {
			getparstring("diskhdr",&diskhdr);
			hdrfp = efopen(diskhdr,"r");
			fseek2g(hdrfp,0,1);
			if(fread(&tra,sizeof(char),240,hdrfp)!=240)
				err(" read error on %s \n",diskhdr);
			efclose(hdrfp);
	}
   	nt = tra.ns;
   	dt = (float)tra.dt/1000000.;
	t0 = (float)tr.delrt/1000;

	if(outputonly==1) idhdrs(&ch,&bh,nt);

	ntotal = tra.tracl;

	ntl = nt * ifact;
	dtl = dt/ifact;
	tminl = t0;
	dldt = dtl/dt;

	/* get traveltime grid header info */
	if(!getparstring("ttfile",&ttfile))  ttfile="ttfile";
	ttfp = efopen(ttfile,"r");
	fseek2g(ttfp,0,1);
	ierr = fgetusghdr(ttfp,&ugh);
	if (ierr!=0) err("Grid parameters of %s required!\n",ttfile);

	nzt = ugh.n1;	fzt = ugh.o1;	dzt = ugh.d1;
	nst = ugh.n2;	fst = ugh.o2;	dst = ugh.d2;
	nlt = ugh.n3;	flt = ugh.o3;	dlt = ugh.d3;
	nxst = ugh.n4;	fxst = ugh.o4;	dxst = ugh.d4;
	nyst = ugh.n5;	fyst = ugh.o5;	dyst = ugh.d5;
	if (dyst==0.) dyst = 1.;
	if (dxst==0.) dxst = 1.;
	if (dst==0.) dst = 1.;
	if (dlt==0.) dlt = 1.;

	ezt = fzt+(nzt-1)*dzt;
	if(dlt<999990){
		est = fst+(nst-1)*dst;
		elt = flt+(nlt-1)*dlt;
	} else{
		est = send;
		elt = lend;
	}
	exst = fxst+(nxst-1)*dxst;
	eyst = fyst+(nyst-1)*dyst;
	if(!getparint("izw",&izw)) izw = (fzt<4*dzt)? 4-fzt/dzt: 0;

   	if (!getparfloat("v0",&v0)) v0 = 1500.;
   	if (!getparfloat("dvz",&dvz)) dvz = 0.2;

   	if (!getparfloat("offmax",&offmax)) offmax = 6000;

	tabx = offmax/dxst;
	if(tabx>nxst) tabx = nxst;
	taby = offmax/dyst;
	if(taby>nyst) taby = nyst;
	ntab = 0.5*tabx*taby+9.+3.*sqrt(tabx*tabx+taby*taby);
	if(ntab>(tabx+3)*nyst) ntab = (tabx+3)*nyst;
	if(ntab>(taby+3)*nxst) ntab = (taby+3)*nxst;
	if(ntab>nxst*nyst) ntab = nxst*nyst;
   
   	if (!getparint("nzo",&nzo)) nzo = 10*(nzt-1)+1;
   	if (!getparfloat("dzo",&dzo)) dzo = 0.1*dzt;
   	if (!getparfloat("fzo",&fzo)) fzo = fzt;
	/*
	fprintf(stderr,"fzo=%g fzt=%g fzo+(nzo-1)*dzo=%g ezt=%g \n",
		fzo,fzt,fzo+(nzo-1)*dzo,ezt);
	*/

	if(fabs(fzo-fzt)<0.1*dzo) fzo = fzt;
	if(fabs(fzo+(nzo-1)*dzo-ezt)<0.1*dzo) ezt  = fzo+(nzo-1)*dzo;

	if(fzo<fzt || fzo+(nzo-1)*dzo>ezt) {
	  warn("migration depth fzo=%g fze=%g \n", fzo, fzo+(nzo-1)*dzo);
	  warn("traveltime depth fzt=%g ezt=%g \n", fzt, ezt);
	  err("migration depth is beyond the depth in traveltime table!\n");
	}
	if(getparstring("hzgrid",&hzgrid)) {
		ihzgrid = 1;
	} else {
		ihzgrid = 0;
	}

	if(ihzgrid==0) {
		nz0 = (int)((fzo-fzt)/dzt);
		fz = fzt+nz0*dzt;
		nz = 1+(int)((fzo+(nzo-1)*dzo-fzt)/dzt+0.99)-nz0;
	} else {
		fzo = fzt;
		nz0 = 0;
		fz = fzt;
		nz = 1+nzt;
	}
 
   	if (!getparint("offnear",&offnear)) offnear = 0;
   	if (!getparint("offfar",&offfar)) offfar = 0;
   	if (!getparint("flexbin",&flexbin)) flexbin = 0;

   	if (!getparfloat("oofo",&ofomin)) ofomin = 0.;
   	if (!getparfloat("dofo",&dofo)) dofo = 999999.;
   	if (!getparfloat("obtol",&obtol)) obtol = 0.5*dofo;
   	if (!getparint("nofo",&nofo)) nofo = 1;
	itmp = countparname("ofo");
	if(itmp>0 && itmp!=nofo) err("number of ofo not match with nofo");
	ofo = (float*) emalloc(nofo*sizeof(float));
	if(itmp>0) {
		getparfloat("ofo",ofo);
		if(nofo>1) {
			ofol = ofo[0] - 0.5*(ofo[1]-ofo[0]);
			ofor = ofo[nofo-1] + 0.5*(ofo[nofo-1]-ofo[nofo-2]);
		} else {
			ofol = ofo[0] - 0.5*dofo;
			ofor = ofo[nofo-1] + 0.5*dofo;
			ofomin = ofo[0];
		}
	} else {
		for(io=0;io<nofo;io++) ofo[io] = ofomin + io*dofo;
	}
	iofoin = itmp;
	
   	if (!getparfloat("f0",&f0)) f0 = 10.;
   	if (!getparfloat("df",&df)) df = 5.;
   	if (!getparint("nf",&nf)) nf = 12;
   	if (!getparfloat("ftaper",&ftaper)) ftaper = 5.;

	/* compute maximum wavenumbers to be used in migration */
   	if (!getparfloat("ksmax",&ksmax)) {
		if(ncdppl>1) {
			ksmax = 0.5/dds;
		} else { 
			ksmax = 0.5/0.001;
		}
	}
   	if (!getparfloat("klmax",&klmax)) {
		if(nlines>1) {
			klmax = 0.5/ddl;
		} else {
			klmax = 0.5/0.001;
		}
	}

   	if (!getparint("tapecntl",&tapecntl)) tapecntl = 0;
	
	nsave = ((ntl * 3 / 2)/2)*2;
	radix_(&nsave,&nfft);
	nsave = nfft * 2 + 30;

	/* update id headers and write to output */
	bh.hns = nzo;
	bh.fold = nofo;
	if(dzo>32.) {
		bh.hdt = dzo;
	} else {
		bh.hdt = dzo*1000;
	}
	if(nofo>1) {
		bh.tsort = 2;
	} else {
		bh.tsort = 4;
	}
 	if(traceout==1) fputhdr(outfp,&ch,&bh);


   	if (!getparint("mlimit",&mlimit)) mlimit = 256;
   	llimit = mlimit;
	llimit = llimit * 1024 * 1024;

	if (!getparint("gath",&gath)) gath = 0;

	getparstring("diskhdr",&diskhdr);
	getparstring("diskimg",&diskimg);
	getparstring("diskfld",&diskfld);
	if(getparstring("hisfile",&hisfile)) ihis=1;

	if(!getparstring("jpfile",&jpfile)) {
                jpfp = stderr;
        } else {
                jpfp = efopen(jpfile,"w");
        }


	if (!getparint("isave",&isave)) isave = 0;

	if (getparstring("backupi",&backupi)) {
		fprintf(jpfp," Backup from the previous run ");
                fprintf(jpfp," Backup %s , %s and %s from %s \n",
                        diskimg,diskhdr,diskfld,backupi);
		tar3fr(backupi,diskimg,diskhdr,diskfld);
	}

	ibacko = 0;
	if (getparstring("backupo",&backupo)) ibacko = 1;
	if(ibacko==1) isave=1;
	if(ntrdsk>0) isave=1;

	fprintf(jpfp,"\n");
	fprintf(jpfp," -------- KZMIG PRINTOUT -------- \n");
	fprintf(jpfp,"\n");

	/* detect starting trace of migration */
	jtrace = 0;
	if( (hffp = fopen(hisfile,"r"))!=NULL ) {
		fclose(hffp);
                hffp = efopen(hisfile,"r+");
		buf = (char *) malloc(81*sizeof(char));
		do {
                                bzero(buf,81);
                                if(fgets(buf,81,hffp)==NULL) break;
                                if(strncmp(buf,
                                "Number of Traces Processed: ",28)==0) {
                                        sscanf(buf+28,"%d %d",&ntrpre,&jtrace);
                                }
                } while(feof(hffp)==0);
		free(buf);
		fprintf(jpfp," From history file: \n");
		fprintf(jpfp,"Number of Traces Processed: %d\n",ntrpre);
		if(jtrace>0) {
                fprintf(jpfp,"Number of Traces Processed for this input: %d\n",
                        jtrace);
		}
	} else {
		hffp = efopen(hisfile,"w");
		ntrpre = 0;
	}

	/*  compute the reference traveltimes and lateral slowness	*/
	dr = (dds<ddl)?dds:ddl;
	rmax = sqrt((exst-fst)*(exst-fst)+(eyst-flt)*(eyst-flt));
	if(rmax<sqrt((est-fxst)*(est-fxst)+(elt-fyst)*(elt-fyst)))
		rmax = sqrt((est-fxst)*(est-fxst)+(elt-fyst)*(elt-fyst));
	if(rmax>0.5*offmax+sqrt(apers*apers+aperl*aperl)+dxst+dyst)
		rmax = 0.5*offmax+sqrt(apers*apers+aperl*aperl)+dxst+dyst;

	nr = 1+(int)(rmax/dr+0.99);

/*
	fprintf(stderr," before timeb dds=%g ddl=%g \n",dds,ddl);
	fprintf(stderr," before timeb rmax=%d dr=%g nr=%d nzt=%d \n",
	rmax,dr,nr,nzt);
*/

 	tb0 = alloc1float(nzt*nr);
 	pb0 = alloc1float(nzt*nr);


 	timeb_(&nr,&nzt,&dr,&dzt,&fzt,&dvz,&v0,tb0,pb0);
	free1float(pb0);
 	pb = alloc1float(nz*nr);
 	tb = alloc1float(nz*nr);
 	timeb_(&nr,&nz,&dr,&dzt,&fz,&dvz,&v0,tb,pb);


	/* compute memory requirement for migration*/
 
	ntmp = nl*ndl;
	nl = ntmp;
	ntmp = ns*nds;
	ns = ntmp;

	nlcore = nl;

	if(ntrend==0) {
		nsout = ns;
		nlout = nl;
		dsout = ds;
		dlout = dl;
	} else {
		nsout = ntrend;
		nlout = 1;
		nlcore = 1;
		dsout = (send-sstart)/(nsout-1);
		dlout = (lend-lstart)/(nsout-1);
		dend = sqrt ( (send-sstart)*(send-sstart)
		      + (lend-lstart)*(lend-lstart) ) / (nsout-1);
	}

	/* get hzgrid if specified */
	fzout = (float*) malloc(nsout*nlout*sizeof(float));
	if(ihzgrid==1) {
		hzfp = efopen(hzgrid,"r");
		ierr = fgetusghdr(hzfp,&hzgh);
		if (ierr!=0) err("Grid parameters of %s required!\n",hzgrid);
		nshz = hzgh.n1;
		nlhz = hzgh.n2;
		oshz = hzgh.o1;
		olhz = hzgh.o2;
		dshz = hzgh.d1;
		dlhz = hzgh.d2;
		hz = (float*) malloc(nshz*nlhz*sizeof(float));
		fseek(hzfp,0,0);
		efread(hz,sizeof(float),nshz*nlhz,hzfp);
		fclose(hzfp);
	} else {
		for(ix=0;ix<nlout*nsout;ix++) fzout[ix] = fzo;
	}

	/* check grid paramters if migration output line is slant	*/
	if(dlt>999990) {
		if(fabs(fst-sstart)<0.1) fst = sstart;
		if(fabs(flt-lstart)<0.1) flt = lstart;
		tmp = sqrt((send-sstart)*(send-sstart)
			+(lend-lstart)*(lend-lstart));
		tmp = tmp/dst+1.5;
		itmp = tmp;
		if(nlt!=1 || fst!=sstart || flt!=lstart || itmp!=nst)
err("grid parameters in traveltime table are inconsistent with output!\n");
 	}
		
	lwork = nsout * nlcore;
	lwork = lwork*nzo*nofo*sizeof(float);

	ltmp = nz*nsout*nlout*nofo+nt+ntl+nt*mtrace;
	ltmp = ltmp+2*nsout*nlout+mtrace*4;
	ltmp = ltmp  + nfft*(nf+2) + ntl*nf + nsave;
	ltmp = ltmp * sizeof(float);
	ltmp = ltmp + (nsout*nlout*nofo+mtrace+nf*2)*sizeof(int);
	ltmp += (nz*nsout*nlout*ntab+nzt*nst*nlt)*sizeof(float);

	ltmp += (nzt*nr+2*nz*nr)*sizeof(float);
	ltmp += 2*nz*nsout*nlout*sizeof(float);
	lwork = lwork + ltmp;
	tmp = (lwork+1024*1024-1)/(1024*1024); 

	if(lwork>llimit) err("mlimit too small; need mlimit=%d \n ",(int)tmp);

	tmp = llimit - lwork;
	tmp = tmp / (nz*nsout*nlout*sizeof(float));
	lwork = lwork - nz*nsout*nlout*ntab*sizeof(float); 
	ntab =  ntab + tmp;
	if(ntab>nxst*nyst)  ntab = nxst*nyst; 
	lwork = lwork + nz*nsout*nlout*ntab*sizeof(float); 
	
	fprintf(jpfp," Total Memory used (in Byte) = %g \n", (float)lwork);
	fprintf(jpfp,"\n");

	/* allocation of memory */
	mig = ealloc2float(nsout*nlcore*nzo,nofo);	
	fold = ealloc2float(nsout*nlcore*nz,nofo);	
	off = (int*) emalloc(nsout*nlout*nofo*sizeof(int));	
	tgain = (float*) emalloc(nt*sizeof(float));
	zgain = (float*) emalloc(nzo*sizeof(float));
 	trace = (float*) emalloc(ntl*sizeof(float));	
	tras = (float*) emalloc(nt*mtrace*sizeof(float));	
	imutel = (int*) emalloc(mtrace*sizeof(int));
	s = (float*) emalloc(nsout*nlout*sizeof(float));
	l = (float*) emalloc(nsout*nlout*sizeof(float));
	st = (float*) emalloc(nst*sizeof(float));
	lt = (float*) emalloc(nst*sizeof(float));
 	ss = (float*) emalloc(mtrace*sizeof(float));
	sl = (float*) emalloc(mtrace*sizeof(float));
	gs = (float*) emalloc(mtrace*sizeof(float));
	gl = (float*) emalloc(mtrace*sizeof(float));
	iofs = (int*) emalloc(mtrace*sizeof(int));
 	work1 = (float*) emalloc(nfft*nf*sizeof(float));
 	sqrtf = (float*) emalloc(nfft*sizeof(float));
	tracef = (float*) emalloc(ntl*nf*sizeof(float));
	work2 = (float*) emalloc(nfft*sizeof(float));
	wsave = (float*) emalloc(nsave*sizeof(float));
 	ifcut = (int*) emalloc(nf*sizeof(int));
	ltaper = (int*) emalloc(nf*sizeof(int));
 	disend = (float*) emalloc(nsout*nofo*sizeof(float));
 	ttab = ealloc2float(nz*nsout*nlout,ntab);
	tt = (float*) emalloc(nzt*nst*nlt*sizeof(float));	

	datetime=(char*) emalloc(28*sizeof(char));

	itrace = 0;

	bzero(fold[0],nofo*nsout*nlout*nz*sizeof(float));
	bzero(mig[0],nofo*nsout*nlcore*nzo*sizeof(float));
	if(ntrend>0) {
		for(io=0;io<nsout*nofo;io++) disend[io] =  99999999999.;
		dismax = disaper(apers,aperl,sstart,lstart,send,lend);
		xy2abc(sstart,lstart,send,lend,&aa,&bb,&cc);
	}

	if(ntrend==0) { 
		for(il=0;il<nlout;il++) {
			for(is=0;is<nsout;is++) {
				s[is+il*nsout] = sstart+is/nds*ds+is%nds*dds;
			}
		}
		for(il=0;il<nlout;il++) {
			for(is=0;is<nsout;is++) {
				l[is+il*nsout] = lstart+il/ndl*dl+il%ndl*ddl;
			}
		}
	} else {
		for(is=0;is<ntrend;is++) {
			s[is] = sstart+is*(send-sstart)/(ntrend-1.);
			l[is] = lstart+is*(lend-lstart)/(ntrend-1.);
		}
		for(is=0;is<nst;is++) {
			st[is] = sstart+is*(send-sstart)/(nst-1.);
			lt[is] = lstart+is*(lend-lstart)/(nst-1.);
		}
	}

	/* bilinear interpolation of output depth from hzgrid */
	if(ihzgrid==1) {
		for(itr=0;itr<nsout*nlout;itr++) {
			sss = s[itr];
			lll = l[itr];
			bilint_(&one,&nshz,&nlhz,&oshz,&olhz,&dshz,&dlhz,&sss,&lll,hz,&hzz);
			hzz = hzz/dzo;
			itmp = hzz + 0.5;
			hzz = itmp * dzo;
			fzout[itr] = hzz;
			/*
			fprintf(stderr,"fzout=%g itr=%d \n",fzout[itr],itr);
			*/
			if(itr==0) {hzmin = hzz; hzmax = hzz;}
			if(hzmin>hzz) hzmin = hzz;
			if(hzmax<hzz) hzmax = hzz;
		}
		if(hzmin<fzt || hzmax >ezt) {
	fprintf(stderr," hzgrid min=%g max=%g outside of tracvel time depths\n",
		hzmin,hzmax);
		}
	}

	sinmin = s[0];
	sinmax = s[0];
	linmin = l[0];
	linmax = l[0];

	for(is=0;is<nsout*nlout;is++) {
		if(sinmin>s[is]) sinmin = s[is];
		if(sinmax<s[is]) sinmax = s[is];
		if(linmin>l[is]) linmin = l[is];
		if(linmax<l[is]) linmax = l[is];
	}

/*	check migration output range	*/
	stmin = fst; if (est<fst) stmin = est;
    ltmin = flt; if (elt<flt) ltmin = elt;
    stmax = est; if (est<fst) stmax = fst;
    ltmax = elt; if (elt<flt) ltmax = flt;

    if(stmin>sinmin+1.0 || stmax<sinmax-1.0
        || ltmin>linmin+1.0 || ltmax<linmax-1.0) {
                warn(" migration output is out of time table ranges !\n");
    }

    if(stmin>sinmin+1.0) err(" migration output smmin=%g < travel time stmin=%g \n",sinmin,stmin);
    if(stmax<sinmax-1.0) err(" migration output smmax=%g > travel time stmax=%g \n",sinmax,stmax);
    if(ltmin>linmin+1.0) err(" migration output lmmin=%g < travel time ltmin=%g \n",linmin,ltmin);
    if(stmax<sinmax-1.0) err(" migration output lmmax=%g > travel time ltmax=%g \n",linmax,ltmax);
    
    /*
    if(stmin>sinmin+1.0 || stmax<sinmax-1.0
        || ltmin>linmin+1.0 || ltmax<linmax-1.0) {
                warn("stmin=%g > sinmin=%g ? \n",stmin,sinmin);
                warn("stmax=%g < sinmax-0.01=%g ? \n",stmax,sinmax-0.01);
                warn("ltmin=%g > linmin=%g ? \n",ltmin,linmin);
                warn("ltmax=%g < linmax-0.01=%g ? \n",ltmax,linmax-0.01);
                err(" migration output is out of time tables!\n");
    }
    */

	sinmin = sinmin - apers;
	sinmax = sinmax + apers;
	linmin = linmin - aperl;
	linmax = linmax + aperl;

	/* allocate disk space for image */
	if(ntrpre>0) {

		fprintf(jpfp," open diskimg file... \n");
		imgfp = efopen(diskimg,"r+");
		fseek2g(imgfp,0,1);
		ltmp = nzo*nofo*nlout*nsout;
		if(fread(mig[0],sizeof(float),ltmp,imgfp)!=ltmp)
			err(" read error on %s \n",diskimg);
		efclose(imgfp);
		fprintf(jpfp," diskimg file opened  \n");

		fprintf(jpfp," open diskfld file... \n");
		fldfp = efopen(diskfld,"r+");
		fseek2g(fldfp,0,1);
		ltmp = nofo*nsout*nlout*nz;
		if(fread(fold[0],sizeof(float),ltmp,fldfp)!=ltmp)
			err(" read error on %s \n",diskfld);
		efclose(fldfp);
		fprintf(jpfp," diskfld file opened  \n");

		fprintf(jpfp," open diskhdr file... \n");
		hdrfp = efopen(diskhdr,"r+");
		fseek2g(hdrfp,0,1);
		fprintf(jpfp," diskhdr file opened  \n");
		
		for(ix=0;ix<nsout*nlout*nofo;ix++) {
			if(fread(&tr,sizeof(char),240,hdrfp)!=240)
				err(" read error on %s \n",diskhdr);
			off[ix] = tr.offset;
			if(ntrend>0 ) disend[ix] = tr.ungpow;
		}
		ipre = 1;
	} else if(isave==1) {
		imgfp = efopen(diskimg,"w+r");
		fseek2g(imgfp,0,1);
		bzero(mig[0],nofo*nsout*nlout*nzo*sizeof(float));
		fwrite(mig[0],sizeof(float),nofo*nsout*nlout*nzo,imgfp);
 		fprintf(jpfp," initialize diskimg file... \n");
 		fprintf(jpfp," initialization of diskimg file done \n");
		efclose(imgfp);

		fprintf(jpfp," initialize diskfld file... \n");
		fldfp = efopen(diskfld,"w+r");
		fseek2g(fldfp,0,1);
		bzero(fold[0],nofo*nsout*nlout*nz*sizeof(float));
		fwrite(fold[0],sizeof(float),nofo*nsout*nlout*nz,fldfp);
		efclose(fldfp);
		fprintf(jpfp," initialization of diskfld file done  \n");
	}

	if(ntrpre==0) {
		fprintf(jpfp," initializing diskhdr file... \n");
		hdrfp = efopen(diskhdr,"w");
		bzero(&tr,240);
		tr.offset = 99999999;
		tr.ns = tra.ns;
		tr.dt = tra.dt;
		tr.trid = 2;
		if(ntrend>0) tr.ungpow = 99999999999.;
		for(iy=0;iy<nlout;iy++) {
			tr.tracr = iy + 1;
			for(ix=0;ix<nsout;ix++) {
				tmp = (l[iy*nsout+ix] - l1)/ddl + 0.5;
				iiy = tmp;
				tmp = (s[iy*nsout+ix] - s1)/dds + 0.5;
				iix = tmp;
				if(cdpnum==0) {
					tr.cdp = iiy*ncdppl + iix + cdp1;
				} else {
					tr.cdp = iix*nlines + iiy + cdp1;
				}
				tr.tracl = ix + 1;
				ms = s[iy*nsout+ix];
				ml = l[iy*nsout+ix];
				sl2xy(s1,l1,x1,y1,s2,l2,x2,y2,s3,l3,x3,y3,ms,ml,&mx,&my);
				if(ikey==1) {
					fline = (ml - l1)*(ln3-ln1)/(l3-l1) + ln1 + 0.5;
					ftrace = (ms - s1)*(tr2-tr1)/(s2-s1) + tr1 + 0.5;
					intline = fline;
					inttrace = ftrace;
					itov(trktype,&trkval,inttrace);
					itov(lnktype,&lnkval,intline);
					puthval(&tr,indxtrk,&trkval);
					puthval(&tr,indxlnk,&lnkval);
				}
				if(dzo<=32.) { 
					tr.dt = dzo*1000+0.5;
				} else {
					tr.dt = dzo;
				}
				tr.dt = dzo*1000.+0.5;
				tmp = fzout[ix+iy*nsout]+0.5;
				itmp = tmp;
				tr.delrt = (short)itmp;
				for(io=0;io<nofo;io++) {
					tr.cdpt  = io+1;
					ofs = ofo[io];
					tr.sx = mx-ofs/2;
					tr.gx = mx+ofs/2;
					tr.sy = my;
					tr.gy = my;
					fwrite(&tr,sizeof(char),240,hdrfp);
				}
			}
		}
		fprintf(jpfp," initializing diskhdr file done \n");
		for(ix=0;ix<nofo*nsout*nlout;ix++) off[ix] = 99999999;
		fflush(hdrfp);
		
	}


	ktrace = 0;
	m1000 = 0;
	oldheadx = newheadx = 0;
	oldheady = newheady = 0;
	ssmin = slmin = 99999999.0;
	ssmax = slmax = -99999999.0;
 
	if(itgain==1) {
		for(it=0;it<nt;it++) {
			tmp = t0 + it*dt;
			if(tmp!=0.0) {
				tgain[it] = pow(tmp,tpow);
			} else {
				tgain[it] = 0.;
			}
		}
 	} else {
		for(it=0;it<nt;it++) tgain[it] = 1.0;
	}
		
	fprintf(jpfp," \n");
	fprintf(jpfp," Migration Parameters \n");
	fprintf(jpfp," -------------------- \n");
	fprintf(jpfp," x1=%g y1=%g s1=%g l1=%g cdp1=%d\n",x1,y1,s1,l1,cdp1);
	fprintf(jpfp," x2=%g y2=%g s2=%g l2=%g cdp2=%d\n",x2,y2,s2,l2,cdp2);
	fprintf(jpfp," x3=%g y3=%g s3=%g l3=%g cdp3=%d\n",x3,y3,s3,l3,cdp3);
	fprintf(jpfp," sstart=%g lstart=%g ns=%d nl=%d ds=%g dl=%g \n",
		sstart,lstart,nsread,nlread,ds,dl);
	if(ntrend>0) fprintf(jpfp," send=%g lend=%g ntrend=%d dend=%g \n",
			send,lend,ntrend,dend);
	if(ntrend>0) fprintf(jpfp," dismax=%g aa=%g bb=%g cc=%g \n",
			dismax,aa,bb,cc);
	fprintf(jpfp," fzo=%g nzo=%d dzo=%g\n",fzo,nzo,dzo);
	if(ihzgrid==1) fprintf(jpfp," hzgrid=%s\n",hzgrid);
	fprintf(jpfp," nds=%d ndl=%d dds=%g ddl=%g \n",
		nds,ndl,dds,ddl);
	fprintf(jpfp," nss=%d nll=%d is0=%d il0=%d \n",
		nss,nll,is0,il0);
	fprintf(jpfp," cdppdds=%d cdppds=%d cdppddl=%d cdppdl=%d cdpnum=%d\n",
		cdppdds, cdppds, cdppddl, cdppdl, cdpnum);
	fprintf(jpfp," ntrpre=%d nt=%d ntl=%d dt=%g t0=%g nfft=%d\n",
		ntrpre,nt,ntl,dt,t0,nfft);
fprintf(jpfp," tpow=%g zpow=%g ofomin=%g dofo=%g obtol=%f nofo=%d isave=%d \n",
		tpow,zpow,ofomin,dofo,obtol,nofo,isave);
	fprintf(jpfp," apers=%g aperl=%g angmax=%g tapecntl=%d\n",
		apers,aperl,angmax,tapecntl);
	fprintf(jpfp," nlcore=%d mtrace=%d ntras=%d mlimit=%d ntrdsk=%d \n",
		nlcore,mtrace,ntras,(int)(llimit/(1024*1024)),ntrdsk);
	fprintf(jpfp," f0=%g df=%g nf=%d ftaper=%g ksmax=%g klmax=%g\n",
		f0,df,nf,ftaper,ksmax,klmax);
	fprintf(jpfp," offmax=%g ntab=%d nr=%d \n",offmax,ntab,nr);
 	fprintf(jpfp," fzt=%g dzt=%g nzt=%d \n",fzt,dzt,nzt);
	fprintf(jpfp," fst=%g dst=%g nst=%d \n",fst,dst,nst);
	fprintf(jpfp," flt=%g dlt=%g nlt=%d \n",flt,dlt,nlt);
	fprintf(jpfp," fxst=%g dxst=%g nxst=%d \n",fxst,dxst,nxst);
	fprintf(jpfp," fyst=%g dyst=%g nyst=%d \n",fyst,dyst,nyst);
	fprintf(jpfp," v0=%g dvz=%g gath=%d \n",v0,dvz,gath);
	fprintf(jpfp," diskimg=%s\n",diskimg);
	fprintf(jpfp," diskfld=%s\n",diskfld);
	fprintf(jpfp," diskhdr=%s\n",diskhdr);
	fprintf(jpfp," ttfile=%s\n",ttfile);
	fprintf(jpfp," backupi=%s\n",backupi);
	fprintf(jpfp," backupo=%s\n",backupo);
	fprintf(jpfp," hisfile=%s\n",hisfile);
	fprintf(jpfp," ncpu=%d traceout=%d\n",ncpu,traceout);
	if(on2trace==1) fprintf(jpfp," total_number_of_input_trace=%d \n",ntotal);
	if(ikey==1) {
		fprintf(jpfp," tr1=%g tr2=%g tr3=%g \n",tr1,tr2,tr3);
		fprintf(jpfp," ln1=%g ln2=%g ln3=%g \n",ln1,ln2,ln3);
		fprintf(jpfp," tracekey=%s linekey=%s \n",tracekey,linekey);
		fprintf(jpfp," strace=%d jthtrace=%d sline=%d jthline=%d \n",
			strace,jthtrace,sline,jthline);
		fprintf(jpfp," mintrace=%d maxtrace=%d \n",mintrace,maxtrace);
		fprintf(jpfp," minline=%d maxline=%d  \n",minline,maxline);
	}
	fprintf(jpfp," \n");


/*
	gettime(datetime);
 	fprintf(jpfp," start migration at %s ... \n",datetime);
*/
 	fprintf(jpfp," start migration at ... \n");
 	fflush(jpfp); 

 	m1000 = ntrpre/1000;
	ntrace = 0;

	if(tapecntl==1) {
		if(jtrace>0) {
        	lpos = nt*sizeof(float)+240;
            lpos = lpos * jtrace + 3600;
            fseek2g(infp,lpos,0);
            if(outputonly==0) fgettr(infp,&tra);
        }
		ltrace = jtrace;
		jtrace = ntrpre;
	} else if(tapecntl==-1) {
		ltrace = 0;
		jtrace = ntrpre;
		lpos = nt*sizeof(float)+240;
		lpos = lpos * ntrpre + 3600;
		fseek2g(infp,lpos,0);
		if(outputonly==0) fgettr(infp,&tra);
	} else if(tapecntl==0) {
		ltrace = jtrace;
		jtrace = ntrpre - jtrace;
	} else if(tapecntl==2) {
		ltrace = jtrace;
		jtrace = ntrpre;
	}

	/* set old header for the first trace */
	if(gath==1){
	 	oldheadx = tra.sx;
 	 	oldheady = tra.sy;
 	} else if(gath==2){
	 	oldheadx = tra.gx;
 	 	oldheady = tra.gy;
 	} 
	range = gottrace = 1;
	ready = done = 0;
	nxsb = nysb = nxs0b = nys0b = 0;

	if (outputonly==1) goto no_mig;
    
	do {
 		
		/* if gather has changed or no more input traces */
		if ( ready || (!gottrace && ktrace>0) ) {
  			fxs = fxst+nxs0*dxst;
			fys = fyst+nys0*dyst;

/*
	fprintf(stderr,"fxs=%g nxs=%d fys=%g nys=%d \n",fxs,nxs,fys,nys);
				fprintf(jpfp,"read ttable time=%f \n",walltime());
*/

	    	for(iys=0,ys=fys; iys<nys; ++iys,ys+=dyst) 
  		    for(ixs=0,xs=fxs; ixs<nxs; ++ixs,xs+=dxst){
 				itab = iys*nxs+ixs;
			
 		    	nseek = nxst*(nys0+iys)+nxs0+ixs;
				nseek = nseek*nlt*nst*nzt;
   		    	fseek2g(ttfp,nseek*sizeof(float),SEEK_SET);

 				fread(tt,sizeof(float),nst*nlt*nzt,ttfp);

				/* compute residual t */
				if(dlt<999990) 
      	   			resit_(tt,&nst,&nlt,&nzt,&izw,&nr,&dst,&dlt,
				    	&dr,&fst,&flt,&xs,&ys,tb0);
				else   
      	    		resit0_(tt,&nst,&nzt,&izw,&nr,&dr,
			    		st,lt,&xs,&ys,tb0);
  
				/* intrepolation t to output lateral positions */ 
   				latint_(&nsout,&nlout,&nz,s,l,ttab[itab],
			 		&nst,&nlt,&nzt,&nz0,&fst,&flt,&dst,&dlt,tt);
	      	}
 
			/* migration of ktrace */
			migs(nlout,nsout,mig,nz,
				ktrace,ntl,dldt,trace, 
				tras,dtl,tminl,nt, 
				ss,sl,gs,gl, 
				imutel,s,l,
				iofs,apers,aperl,fold,
				work1,work2,wsave,tracef,sqrtf,
				tahd,ifcut,ltaper,ksmax,klmax,
				f0,df,nf,ftaper,nsave,nfft, 
				dzt,fz,angmax, 
				nxs,nys,fxs,fys,dxst,dyst,
				nzo,fzout,dzo,
				nr,dr,tb,pb,ttab,gath);

			itrace = itrace + ktrace;
			ndskwt = ndskwt + ktrace;
 			ktrace = 0;
/*	fprintf(jpfp," processing done for total %d input traces \n",jtrace);*/
 	fflush(jpfp); 

                        /* if no more traces, break */
                        if (!gottrace) break;

			ready = 0;
		}

		if(!gottrace) break;

	    do {
		jtrace = jtrace + 1;
		if(jtrace<=ntrpre) continue;
		ntrace = ntrace + 1;

		/* dead trace */ 
		if(tra.trid!=1) {
		fprintf(btfp," trid not 1, at trace=%d cdp=%d cdpt=%d\n",
			jtrace,tra.cdp,tra.cdpt); 
			fflush(btfp);
			continue;
		}

		/* trace select */
        if(ikey==1) {
			gethval(&tra,indxtrk,&trkval);
			inttrace = vtoi(trktype,trkval);
			if( (inttrace-strace)%jthtrace!=0 
				|| inttrace<mintrace || inttrace>maxtrace) continue;
			gethval(&tra,indxlnk,&lnkval);
			intline = vtoi(lnktype,lnkval);
			if( (intline-sline)%jthline!=0 
				|| intline<minline || intline>maxline ) continue;
		}

		/* compute rms and max of trace amplitudes */
		itmp = (tra.mute-tra.delrt);
		itmp = itmp*1000/(int)tra.dt; 

		ampmax = 0.;
		amprms = 0.;
		if(itmp<0) itmp = 0;
		it0 = 0;

		for(it=itmp;it<nt;it++) {
			tmp = fabs(tra.data[it]);
			if(it0==0 && tmp>0.) it0 = it;
			amprms = amprms + tmp*tmp;
			if(tmp>ampmax) ampmax = tmp;
 		}
		itmp = it0;
		
		it0 = tra.delrt + itmp * (int)tra.dt/1000; 
		if(it0>tra.mute) tra.mute = it0; 
		/* set the mute time to be 10 samples shallower */
		/* just to be save */
		tra.mute = tra.mute - 10*tra.dt/1000;
		if(tra.mute<0) tra.mute = 0;
		
		if(nt>itmp) amprms = sqrt(amprms/(nt-itmp));
		if(amprms==0.) {
		fprintf(btfp," zero trace, at trace=%d cdp=%d cdpt=%d\n",
			jtrace,tra.cdp,tra.cdpt); 
			fflush(btfp);
			continue;
		} else {
			if (ampmax>amprms*rmsmar) {
fprintf(btfp," bad trace with ampmax=%g  rms=%g, at trace=%d cdp=%d cdpt=%d\n",
		ampmax,amprms,jtrace,tra.cdp,tra.cdpt); 
				fflush(btfp);
				continue;
			}
		}


		/* source and group x, y locations */
		xs = tra.sx;
		ys = tra.sy;
		xg = tra.gx;
		yg = tra.gy;

		if(tra.scalco>1) {
			xs = xs *tra.scalco;
			ys = ys *tra.scalco;
			xg = xg *tra.scalco;
			yg = yg *tra.scalco;
		} else if(tra.scalco<0) {
			xs = xs / (-tra.scalco);
			ys = ys / (-tra.scalco);
			xg = xg / (-tra.scalco);
			yg = yg / (-tra.scalco);
		}

 		/* source and group s, l locations */
		xy2sl(x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3,xs,ys,
			&ss[ktrace],&sl[ktrace]);

		xy2sl(x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3,xg,yg,
			&gs[ktrace],&gl[ktrace]);
 
 		if( ss[ktrace]<fxst || gs[ktrace]<fxst ||
		    ss[ktrace]>exst || gs[ktrace]>exst ||
		    sl[ktrace]<fyst || gl[ktrace]<fyst ||
		    sl[ktrace]>eyst || gl[ktrace]>eyst ) {

/*
	    	fprintf(jpfp," ss=%g gs=%g fxst=%g exst=%g \n",
				ss[ktrace],gs[ktrace],fxst,exst);
	    	fprintf(jpfp," sl=%g gl=%g fyst=%g eyst=%g \n",
				sl[ktrace],gl[ktrace],fyst,eyst);
	    	fprintf(jpfp," trace %d is out of time tables\n",jtrace);
*/

  			continue;
		}
 		if( (ss[ktrace]-gs[ktrace])*(ss[ktrace]-gs[ktrace])+ 
 		    (sl[ktrace]-gl[ktrace])*(sl[ktrace]-gl[ktrace])>
 			offmax*offmax) {
			/*
 		    fprintf(btfp," trace %d has offset too large \n",jtrace);
			*/
			continue;
		}
		ms = (ss[ktrace] + gs[ktrace])/2.;
		ml = (sl[ktrace] + gl[ktrace])/2.;

		if(ntrend==0) { 

			/*
			itmp = tra.cdp - cdp1;
			if(cdpnum==0) {
				itmp = itmp/ncdppl;
				iiy = itmp;
				iix = tra.cdp - cdp1 - iiy*ncdppl;
			} else {
				itmp = itmp/nlines;
				iix = itmp;
				iiy = tra.cdp - cdp1 - iix*nlines;
			}
			*/

			/* replaced by the following */
			tmp = (ml - l1)/ddl + 0.5;
			iiy = tmp;
			tmp = (ms - s1)/dds + 0.5;
			iix = tmp;

			is = (iix-is0)/nss;
			ids = iix - is*nss - is0;

			if(ids>=0 && ids<nds) {
				is = is * nds + ids; 
			} else {
				is = -1;
			}
		
			il = (iiy-il0)/nll;
			idl = iiy - il*nll - il0;

			if(idl>=0 && idl<ndl) {
				il = il * ndl + idl;
			} else {
				il = -1;
			} 

		} else {
			is = 0;
			il = 0;
		}
		
 			
		tmp = tra.offset;
		if(iofoin==0 || nofo==1 ) {
			if(tmp<0.) tmp = - tmp;
			tmp = (tmp-ofomin)/dofo+.5;
			io = tmp;
			if(tmp<0.) io = -1;
		} else {
			if(tmp<ofol) {
				io = -1;
			} else if(tmp>ofor) {
				io = nofo;
			} else {
				bisear_(&nofo,&one,ofo,&tmp,&io);
				io = io - 1;
				if(io<nofo-1) {
					if(abs(tmp-ofo[io])>abs(tmp-ofo[io+1]))
						io = io + 1;
				}
			}
		}

		if(offnear==1 && io<0 ) io = 0;
		if(offfar==1 && io>=nofo) io = nofo-1;
		if(io>=0 && io<=nofo-1) {
			tmp = tra.offset;
			if(iofoin==0 || nofo==1 ) {
				if(tmp<0) tmp = -tmp;
			}
			tmp = tmp - ofo[io];
			if(tmp<0.) tmp =  - tmp;
			if(tmp>obtol*1.001) io=-1;
		}
 
		if(io<0 || io>=nofo) {
		/*
 		    fprintf(btfp," trace %d has offset io=%d \n",jtrace,io);
		*/
			continue;
		}

		if( ms < sinmin || ms >sinmax) continue;
		if( ml < linmin || ml >linmax) continue;
/*
	fprintf(stderr,"ms=%g ml=%g jtrace=%d dis=%g\n",
		ms,ml,jtrace,disp2l(aa,bb,cc,ms,ml));
*/
		if( ntrend>0 && disp2l(aa,bb,cc,ms,ml)>dismax ) {
			/*
 		    fprintf(btfp," trace %d outside aperture \n",jtrace);
			*/
			continue;
		}

 		/* update minimum and maximum (s,l) coordinates	*/
		if(ss[ktrace]<ssmin) ssmin = ss[ktrace];
		if(gs[ktrace]<ssmin) ssmin = gs[ktrace];
		if(sl[ktrace]<slmin) slmin = sl[ktrace];
		if(gl[ktrace]<slmin) slmin = gl[ktrace];
		if(ss[ktrace]>ssmax) ssmax = ss[ktrace];
		if(gs[ktrace]>ssmax) ssmax = gs[ktrace];
		if(sl[ktrace]>slmax) slmax = sl[ktrace];
		if(gl[ktrace]>slmax) slmax = gl[ktrace];

		/* find traveltime tables for this gather */
		nxs0 = (ssmin-fxst)/dxst;
		nys0 = (slmin-fyst)/dyst;
		nxs = 1+(int)((ssmax-fxst)/dxst+0.99)-nxs0;
		nys = 1+(int)((slmax-fyst)/dyst+0.99)-nys0;
 
 		if(nys*nxs>ntab){
			range = 0;
			nxs = nxsb;
			nys = nysb;
			nxs0 = nxs0b;
			nys0 = nys0b;
			continue;
		}else{
  			nxsb = nxs;
			nysb = nys;
			nxs0b = nxs0;
			nys0b = nys0;
		}


		iofs[ktrace] = io + 1;

 		/* apply tpow */
                if(itgain==1) for(it=0;it<nt;it++) tra.data[it] *= tgain[it];
		for(it=0;it<nt;it++) tras[it+ktrace*nt] = tra.data[it];

			
		itmp = (tra.mute-tra.delrt);
 		itmp = itmp*1000/(int)tra.dt*ifact + 1; 
		if(itmp<1) itmp = 1;
		if(itmp>ntl) itmp = ntl; 
		imutel[ktrace] = itmp;
 
 
/*
fprintf(stderr,"il=%d is=%d ml=%g ms=%g cdp=%d offset=%d io=%d tracl=%d\n",
		il,is,ml,ms,tra.cdp,tra.offset,io,tra.tracl); 
*/

		if(il>=0 && il<nlout && is>=0 && is<nsout) {
            if(ikey==1) {
				ms = s[il*nsout+is];
				ml = l[il*nsout+is];
				fline = (ml - l1)*(ln3-ln1)/(l3-l1) + ln1 + 0.5;
				ftrace = (ms - s1)*(tr2-tr1)/(s2-s1) + tr1 + 0.5;
				intline = fline;
				inttrace = ftrace;
				itov(trktype,&trkval,inttrace);
				itov(lnktype,&lnkval,intline);
				puthval(&tra,indxtrk,&trkval);
				puthval(&tra,indxlnk,&lnkval);
			}
			tmp = fzout[is+il*nsout]+0.5;
			itmp = tmp;
			tra.delrt = (short)itmp;
			if(ntrend==0) {
			   if(abs(abs(tra.offset)-ofo[io])
                                <abs(off[io+is*nofo+il*nofo*ns]-ofo[io])) {
				off[io+is*nofo+il*nofo*ns] = abs(tra.offset);
				lpos=io+is*nofo+il*nofo*ns;
				lpos=lpos*240;
				fseek2g(hdrfp,lpos,0);
 				fwrite(&tra,sizeof(char),240,hdrfp);
 			   } 
			} else {
				if(lend==lstart) {
					ps = ms;
					pl = lstart;
				} else {
					tmp = (send-sstart)/(lend-lstart);
					pl = ml + tmp*(lstart*tmp+ms-sstart); 
					pl = pl/(1.+tmp*tmp);
					ps = tmp*(pl-lstart) + sstart;
				}
				if(dsout!=0.) {
					tmp = (ps - sstart)/dsout + 0.5;
					isend = tmp; 
				} else { 
					tmp = (pl - lstart)/dlout + 0.5;
					isend = tmp;
				}
				if (isend>=0 && isend<ntrend) {
				     tmp = sqrt( (ms-s[isend])*(ms-s[isend])
				         + (ml-l[isend])*(ml-l[isend]) );
				     if (tmp<disend[io+isend*nofo]) { 
					 	disend[io+isend*nofo] = tmp;
					 	tra.ungpow  = tmp;
				     	lpos=io+isend*nofo;
				     	lpos=lpos*240;
					 	fseek2g(hdrfp,lpos,0);
 				     	if(fwrite(&tra,sizeof(char),240,hdrfp)!=240)
							err(" error write %s \n",diskhdr);
				      }
			        }  
			}
		}

 		ktrace += 1;

		if(jtrace/1000>m1000) { 
		/*
		gettime(datetime);
		fprintf(jpfp," processing done for %d input traces at %s\n",
			jtrace,datetime);
		*/
		fprintf(jpfp," processing done for %d input traces \n",
			jtrace);

		m1000 = jtrace/1000;
		fflush(jpfp);
		}
	    }while(done);

/*
fprintf(stderr,"jtrace=%i,range=%i ntrace=%d \n",jtrace,range,ntrace);
*/
	
 		if(range){
			if(ntrace==ntras) gottrace = 0;
			else if(!fgettr(infp,&tra))   
				gottrace = 0;
		} else {
			jtrace -= 1;
			ntrace -= 1;
		}

		if(gottrace) {
			if(gath==1){
	 			newheadx = tra.sx;
	 			newheady = tra.sy;
  			} else if(gath==2){
	 			newheadx = tra.gx;
	 			newheady = tra.gy;
 			}
			if(ktrace==mtrace || !range) ready = 1;
			if(newheadx!=oldheadx || newheady!=oldheady){
				oldheadx = newheadx;
				oldheady = newheady;
				ready = 1;
 			}
			if(ready){  
				range = 1;
   				ssmin = slmin = 99999999.0;
				ssmax = slmax = -99999999.0;
			}
			if(!ktrace) ready = 0;
		}
	
		if(ndskwt>=ntrdsk && ntrdsk>0) {
                /* update disk file */
                fldfp = efopen(diskfld,"r+");
                fseek2g(fldfp,0,1);
                fwrite(fold[0],sizeof(float),
				nz*nlout*nsout*nofo,fldfp);
                efclose(fldfp);
                imgfp = efopen(diskimg,"r+");
                fseek2g(imgfp,0,1);
                fwrite(mig[0],sizeof(float),nofo*nsout*nlout*nzo,imgfp);
                efclose(imgfp);
                fprintf(hffp,"Number of Traces Processed: %d %d \n",
                                jtrace,jtrace-ntrpre+ltrace);
                        fflush(hffp);
                        ndskwt=0;
                }

	} while(!done); 

	no_mig:
 
 /*
	gettime(datetime);
	fprintf(jpfp," %d input traces read in this run at %s\n",ntrace,datetime);
	fprintf(jpfp," %d input traces migrated in this run at %s\n",
		itrace,datetime);
*/
	fprintf(jpfp," %d input traces read in this run \n",ntrace);
	fprintf(jpfp," %d input traces migrated in this run \n",
		itrace);

	fflush(jpfp);

	efclose(hdrfp);

	if(isave==1 && ntrace>0 && outputonly==0 ) {
		fldfp = efopen(diskfld,"r+");
		fseek2g(fldfp,0,1);
		fwrite(fold[0],sizeof(float),nlout*nsout*nz*nofo,fldfp);
		efclose(fldfp);
	} 

	if(isave==0) unlink(diskfld);

	if(ftell(btfp)==0) { 
		efclose(btfp);
		unlink("BAD_TRACE_FILE");
	} else {
		efclose(btfp);
	}

	if(isave==1 && ntrace>0 && outputonly==0 )  {
		imgfp = efopen(diskimg,"r+");
		fseek2g(imgfp,0,1);
		fwrite(mig[0],sizeof(float),nofo*nsout*nlout*nzo,imgfp);
		efclose(imgfp);
	}

	if(isave==1 && ntrace>0 && outputonly==0 ) 
		fprintf(jpfp," backup to disk done \n");

	ntrpre = ntrpre + ntrace;
	if(outputonly==0) 
		fprintf(hffp,"Number of Traces Processed: %d 0 \n",ntrpre);
	efclose(hffp);
	if(ihis==0) unlink(hisfile);


	/* output */
	hdrfp = efopen(diskhdr,"r");
	fseek2g(hdrfp,0,1);

	if((ntrace == ntras || feof(infp)!=0 || outputonly==1 ) && traceout==1)  {
	     fprintf(jpfp," start output ... \n");

	    if(izgain==1) {
	     	for(iz=0; iz<nzo; iz++){
				tmp = fzo+iz*dzo;
				zgain[iz] = pow(tmp,zpow);
	     	} 
	    } else {
	     	for(iz=0; iz<nzo; iz++){
				tmp = fzo+iz*dzo;
				zgain[iz] = 1.0;
	     	} 
        }			

  	    for(iy=0;iy<nlout;iy++) {
		for(ix=0;ix<nsout;ix++) {
			for(io=0;io<nofo;io++) {
				itmp = iy*nsout+ix;
				itmp2 = itmp*nzo;
 				bcopy(mig[io]+itmp2,tra.data,nzo*sizeof(float));
				itmp2 = itmp*nz;
	    		if(ihzgrid==1) {
					if(izgain==1) {
	     				for(iz=0; iz<nzo; iz++){
							tmp = fzout[ix+iy*nsout]+iz*dzo;
							zgain[iz] = pow(tmp,zpow);
	     				} 
	    			} else {
	     				for(iz=0; iz<nzo; iz++){
							zgain[iz] = 1.0;
	     				} 
        			}			
				}
				if(flexbin==0) {
  					for(iz=0;iz<nz;iz++) 	
						fold[io][itmp2+iz] = 
							sqrt(fold0+fold[io][itmp2+iz]);
  					for(iz=0;iz<nzo;iz++){
				   		zi = (fzo+iz*dzo-fz)/dzt;
				   		izi = zi;
				   		if(izi>nz-2) izi = nz-2;
 				   		scale = (1.0-zi+izi)*
						fold[io][itmp2+izi]+(zi-izi)*
						fold[io][itmp2+izi+1]; 
				   		tra.data[iz] = tra.data[iz]/
						scale*zgain[iz];
					}
				} else {
  					for(iz=0;iz<nzo;iz++){
				    		tra.data[iz] = tra.data[iz]*
								zgain[iz];
					}
				}

				/* update headers */
				bzero(&tra,240);
				if(fread(&tra,sizeof(char),240,hdrfp)!=240)
					err(" error reading %s \n",diskhdr);

				tra.offset = ofo[io];
				tra.tracl = ix + 1;
				tra.tracr = iy + 1;
				tra.cdpt = io + 1;
				if(dzo<=32.) { 
					tra.dt = dzo*1000+0.5;
				} else {
					tra.dt = dzo;
				}
				tmp = fzout[ix+iy*nsout]+0.5;
				itmp = tmp;
				tra.delrt = (short)itmp;
				tra.dz = dzo;
				tra.fz = fzout[ix+iy*nsout];


				ofs = ofo[io];
				ms = s[iy*nsout+ix];
				ml = l[iy*nsout+ix];
				sl2xy(s1,l1,x1,y1,s2,l2,x2,y2,s3,l3,x3,y3,ms,ml,&mx,&my);
				if(tra.trid==1) {
					if(tra.scalco>1) {
						tra.sx = tra.sx * tra.scalco;
						tra.sy = tra.sy * tra.scalco;
						tra.gx = tra.gx * tra.scalco;
						tra.gy = tra.gy * tra.scalco;
					} else if(tra.scalco<0)  {
						tra.sx = tra.sx /(-tra.scalco);
						tra.sy = tra.sy /(-tra.scalco);
						tra.gx = tra.gx /(-tra.scalco);
						tra.gy = tra.gy /(-tra.scalco);
					}
					tmp = tra.gx-tra.sx;
					dd = tra.gy-tra.sy;
					tmp = tmp*tmp + dd*dd;
              				dd = sqrt(tmp);
                   			if(dd>0.) {
                   				tra.sx = mx+ofs*(tra.sx-mx)/dd;
                       				tra.gx = mx+ofs*(tra.gx-mx)/dd;
                       				tra.sy = my+ofs*(tra.sy-my)/dd;
                       				tra.gy = my+ofs*(tra.gy-my)/dd;
                    			} else {
                    				tra.sx = mx-ofs/2;
                       				tra.gx = mx+ofs/2;
                       				tra.sy = my;
                       				tra.gy = my;
                    			}
				} else {
                    			tra.sx = mx-ofs/2;
                    			tra.gx = mx+ofs/2;
                    			tra.sy = my;
                    			tra.gy = my;
				}

				tra.scalco = 0;

 
				tra.ns = nzo;
				tra.mute = 0;
 
 /*
 				if(ntrend>0) {
*/
					tmp = (l[iy*nsout+ix] - l1)/ddl + 0.5;
					iiy = tmp;
					tmp = (s[iy*nsout+ix] - s1)/dds + 0.5;
					iix = tmp;
					if(cdpnum==0) {
						tra.cdp = iiy*ncdppl+iix+cdp1;
					} else {
						tra.cdp = iix*nlines+iiy+cdp1;
					}
/*
				}
*/

                if(ikey==1) {
					ms = s[iy*nsout+ix];
					ml = l[iy*nsout+ix];
					fline = (ml - l1)*(ln3-ln1)/(l3-l1) + ln1 + 0.5;
					ftrace = (ms - s1)*(tr2-tr1)/(s2-s1) + tr1 + 0.5;
					intline = fline;
					inttrace = ftrace;
					itov(trktype,&trkval,inttrace);
					itov(lnktype,&lnkval,intline);
					puthval(&tra,indxtrk,&trkval);
					puthval(&tra,indxlnk,&lnkval);
				}

				/* output */
				fputtr(outfp,&tra);
			}
		}
	    }
	    fprintf(jpfp," output done \n");
	    fflush(jpfp);
	} 
	
	efclose(outfp);
	

	efclose(hdrfp);
	if(isave==0) unlink(diskhdr);
	if(isave==0) unlink(diskimg);


	free(mig);
	free2float(ttab);

	/* backup to tape */
	if(ibacko==1) {
                fprintf(jpfp," Backup %s , %s and %s to %s \n",
                        diskimg,diskhdr,diskfld,backupo);
		tar3to(backupo,diskimg,diskhdr,diskfld);
        }

	return 0;
}



void migs(int nl, int ns, float **mig, int nz,
	int ktrace, int ntl, float dldt, float *trace, 
	float *tras, float dtl, float tminl, int nt, 
	float *ss, float *sl, float *gs, float *gl, 
	int *imutel, float *s, float *l,
	int *iofs, float apers, float aperl, float **fold,
	float *work1, float *work2, float *wsave, float *tracef, float *sqrtf,
	int tahd, int *ifcut, int *ltaper, float ksmax, float klmax,
	float f0, float df, int nf, float ftaper, int nsave, int nfft,
	float dz, float fz, float angmax,
	int nxs, int nys, float fxs, float fys, float dxs, float dys,
	int nzo, float *fzo, float dzo,
	int nr, float dr, float *tb, float *pb, float **ttab, int gath) 
{


	int itr, i, ir0, it, io;
	float tmp;

	int ixs,iys,itab;
	float ax,sx,ay,sy,*ts,*tg,*tit,*wt,*ampt;
	int nsl;
	float ssold,slold,gsold,glold;

	nsl = ns * nl;
	/* allocate work arrays	*/
	ts = alloc1float(nsl*nz);
	tg = alloc1float(nsl*nz);
	tit = alloc1float((nz+1)*nsl);
	wt = alloc1float((nz+1)*nsl);
	ampt = alloc1float((nz+1)*nsl);
 
 	/* linearly interpolate input trace */
	for(itr=0;itr<ktrace;itr++) {
		ir0 = itr*nt;
		if(ntl!=nt) {
               		for(it=0;it<ntl;it++) {
                   		tmp = it*dldt;
                       		i = (int)tmp;
                       		tmp = tmp - i;
                   		if(i>=0 && i<nt-1) {
                       			trace[it] = 
						(1.-tmp)*tras[i+ir0]+
						tmp*tras[i+1+ir0];
                       		}
			}	
		} else {
               		for(it=0;it<ntl;it++)
                     		trace[it] = tras[it+ir0];
		}

	   if(itr==0) {
		ssold = ss[0] - 10000;
		slold = sl[0] - 10000;
		gsold = gs[0] - 10000;
		glold = gl[0] - 10000;
	   }

/*
	fprintf(stderr," ss=%g ssold=%g sl=%g slold=%g itr=%d \n",
		ss[itr],ssold,sl[itr],slold,itr); 
		fprintf(stderr,"ttable interpolation time=%f\n",walltime());
*/


	   /* linearly interpolate traveltimes to source and receiver */
	    if(fabs(ss[itr]-ssold)>0.1 || fabs(sl[itr]-slold)>0.1) {
		
		ssold = ss[itr];
		slold = sl[itr];

  		ax = (ss[itr]-fxs)/dxs;
		ixs = (int)ax;
   		if(ixs==nxs-1 && nxs>1) ixs = nxs-2;
 		sx = ax-ixs;
		if(sx<=0.01) sx = 0.0;
		if(sx>=0.99) sx = 1.0;
		ay = (sl[itr]-fys)/dys;
		iys = (int)ay;
 		if(iys==nys-1 && nys>1) iys = nys-2;
 		sy = ay-iys;
		if(sy<=0.01) sy = 0.0;
		if(sy>=0.99) sy = 1.0;
		itab = iys*nxs+ixs;
 		if(nxs==1 && nys==1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab],ttab[itab],ttab[itab],ts);
		else if(nxs==1 && nys>1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab],ttab[itab+nxs],ttab[itab+nxs],ts);
 		else if(nxs>1 && nys==1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab+1],ttab[itab],ttab[itab+1],ts);
		else
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab+1],ttab[itab+nxs],ttab[itab+nxs+1],ts);
 
	    } 
/*
	fprintf(stderr," gs=%g gsold=%g gl=%g glold=%g itr=%d \n",
		gs[itr],gsold,gl[itr],glold,itr); 
*/
	    if(fabs(gs[itr]-gsold)>0.1 || fabs(gl[itr]-glold)>0.1) {
		
		gsold = gs[itr];
		glold = gl[itr];
	
  		ax = (gs[itr]-fxs)/dxs;
		ixs = (int)ax;
   		if(ixs==nxs-1 && nxs>1) ixs = nxs-2;
 		sx = ax-ixs;
		if(sx<=0.01) sx = 0.0;
		if(sx>=0.99) sx = 1.0;
		ay = (gl[itr]-fys)/dys;
		iys = (int)ay;
 		if(iys==nys-1 && nys>1) iys = nys-2;
 		sy = ay-iys;
		if(sy<=0.01) sy = 0.0;
		if(sy>=0.99) sy = 1.0;
		itab = iys*nxs+ixs;

 		if(nxs==1 && nys==1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab],ttab[itab],ttab[itab],tg);
		else if(nxs==1 && nys>1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab],ttab[itab+nxs],ttab[itab+nxs],tg);
 		else if(nxs>1 && nys==1)
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab+1],ttab[itab],ttab[itab+1],tg);
		else
	    	    blinint_(&ns,&nl,&nz,&sx,&sy,ttab[itab],
  		      ttab[itab+1],ttab[itab+nxs],ttab[itab+nxs+1],tg);
 	    }
 
 			/* 3d prestack depth migration */
			io = iofs[itr]-1;

/*
   	fprintf(stderr,"io=%i itr=%d imute=%d \n",io,itr,imutel[itr]);  
*/

/*
		fprintf(stderr,"start migration time=%f\n",walltime());
*/
		pszm3d_(trace,&ntl,&tminl,&dtl,
			&ss[itr],&sl[itr],&gs[itr],&gl[itr],
 			&imutel[itr],mig[io],s,l,&nsl,
			&nzo,&dzo,fzo,
 			&apers,&aperl,fold[io],
			&f0,&df,&nf,&ftaper,
			ifcut,ltaper,tracef,
			wsave,&nsave,work1,work2,sqrtf,&tahd,&nfft,
			&ksmax,&klmax,&angmax,
			ts,tg,tit,wt,ampt,&nz,&dz,&fz,
			tb,pb,&nr,&dr);
/*
		fprintf(stderr,"finish migration time=%f\n",walltime());
*/

	}

 	free1float(ts);
	free1float(tg);
	free1float(tit);
	free1float(wt);
	free1float(ampt);
 
}

/* compute distance between a point to a line */
/* (x,y) to ax+by+c=0 */

float disp2l(float a, float b, float c,float x, float y) {
        float tmp;
                tmp = fabs(a*x+b*y+c)/sqrt(a*a+b*b);
                return tmp;
}

/* compute a, b, c for (x1,y1) (x2,y2) */
void xy2abc(float x1, float y1, float x2, float y2,
        float *a, float *b, float *c) {
        *a = - (y2 - y1);
        *b = (x2 - x1);
        *c = x1*y2-y1*x2;
}

float disaper(float aperx,float apery,float x1,float y1,float x2,float y2) {

        float a, b, c;
        float x, y;

        xy2abc(x1,y1,x2,y2,&a,&b,&c);
        x = x1 + aperx;
        if((x2>x1 && y2>y1) || (x2<x1 && y2<y1)) {
                y = y1 - apery;
        } else {
                y = y1 + apery;
        }
        return disp2l(a,b,c,x,y);
}
