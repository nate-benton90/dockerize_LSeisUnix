
#include "usu.h"
#include "subc.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "su.h"
#include "segy.h"
#include "header.h"

char *sdoc = 
"KTMIG - Kirchhoff Time Migration of 2D/3D poststack/prestack data 	\n"
"\n"
"ktmig [parameters] <input-data >migrated-data				\n" 
"\n"
"Required parameters:							\n"
"3D master grid parameters:						\n" 
"x1=                    x coordinate of 1st corner of the 3D master grid\n"
"y1=                    y coordinate of 1st corner of the 3D master grid\n"
"s1=                    s coordinate of 1st corner of the 3D master grid\n"
"l1=                    l coordinate of 1st corner of the 3D master grid\n"
"cdp1=                  cdp number of 1st corner of the 3D master grid\n"
"x2=                    x coordinate of 2nd corner of the 3D master grid\n"
"y2=                    y coordinate of 2nd corner of the 3D master grid\n"
"s2=                    s coordinate of 2nd corner of the 3D master grid\n"
"l2=                    l coordinate of 2nd corner of the 3D master grid\n"
"cdp2=                  cdp number of 2nd corner of the 3D master grid\n"
"x3=                    x coordinate of 3rd corner of the 3D master grid\n"
"y3=                    y coordinate of 3rd corner of the 3D master grid\n"
"s3=                    s coordinate of 3rd corner of the 3D master grid\n"
"l3=                    l coordinate of 3rd corner of the 3D master grid\n"
"cdp3=                  cdp number of 3rd corner of the 3D master grid\n"
"                       (See Note 1 for details)			\n"
"\n"
"dds=                   cdp spacing in inline direction 		\n"
"ddl=                   Line spacing in crossline 			\n"
"velfile=               RMS velocity file name (velocity is stored as 	\n"
"                       v(nvs,nsv,nlv)					\n"
"\n"
"Optional parameters:							\n"
"ds=dds                 cdp group interval to output			\n"
"dl=ddl                 Line group  interval to output 			\n"
"datain=                input data set name (instead of standard input) \n"
"dataout=               output data set name (instead of standard output) \n"
"t0v=from-velfile       time (ms) of first sample in velfile  		\n" 
"ntv=from-velfile       Number of time samples per velocity trace	\n"
"dtv=from-velfile       time interval (ms) in velocity file		\n"
"s0v=from-velfile       smallest s coordinate in velfile  		\n" 
"nsv=from-velfile       Number of s coordinates in velocity file	\n"
"dsv=from-velfile       s interval of velocity file			\n"
"l0v=from-velfile       smallest l coordinate in velfile  		\n" 
"nlv=from-velfile       Number of l coordinates in velocity file	\n"
"dlv=from-velfile       l interval of velocity file			\n" 
"tau0=0.                minimum output time in ms			\n"
"ntau=nt                Number of output migrated samples per trace	\n" 
"dtau=dt                Output migrated time sample interval (in ms)\n"
"                       (dt is the input time sample interval from 	\n"
"                       trace header)					\n"
"oofo=0.                output minimum offset (>0.)			\n" 
"dofo=999999.           output offset increment				\n" 
"obtol=.5*dofo          offset bin tolerance (distance between the \n"
"                       accepted input offset and the output offset \n"
"                       (default to half of the output offset increment). \n"
"                       useful for migrating every other offset:         \n"
"                       e.g.: When dofo=400 and input offset increment \n"
"                       is 200, specify obtol=100 (instead of 200) \n"
"                       kzmig will migrated only those offsets  \n"
"                       whose distance from the output offset bin center \n"
"                       is equal or less than 100. \n"
"nofo=1                 number of output offsets			\n"
"ofo=0.                 output offsets  (specified as 200,250.,350,600,...) \n"
"                       if specified, dofo and oofo will be ignored	\n" 
"sstart=s1              Minimum s (trace position within line) of 	\n"
"                       migration output				\n"
"                       (NOTE: this is coordinates, i.e., first trace	\n"
"                       of a line at s1, 2nd at s1+ds, 3rd at s1+2*ds, ...)\n"
"ns=(s2-s1)/ds+1        Number of cdp (group) per line of migration output \n"
"lstart=l1              Minimum l (line position) of migration output 	\n"
"                       (NOTE: this is coordinates, i.e., first line	\n"
"                       l1, 2nd at l1+dl, 3rd at l1+2*dl, ...)		\n"
"nl=(l3-l1)/dl+1        Number of lines (group) of migration output 	\n"
"send=                  Ending s position of migration output		\n"
"lend=                  Ending l position of migration output           \n" 
"ntrend=                Number of cdp bins to output from (sstart,lstart) \n"
"                       to (send,lend)					\n" 
"                       (specify send, lend and ntrend will output only \n"
"                       one slant line from (sstart,lstart) to 		\n"
"                       (send,lend) of total ntrend cdps; if not 	\n"
"                       specified, will output to ns by nl cdps)	\n" 
"nds=1                  Number of cdp locations per output cdp group	\n"
"ndl=1                  Number of line locations per output line group  \n"
"mlimit=256             Memory limit (in megabytes) to run the program  \n"
"mtrace=1000            Number of traces read per migration step	\n"
"apers=smax/4.          Migration aperature in inline (s) direction (the\n"
"                       maximum lateral distance from s position of 	\n"
"                       the input midpoint to s position of output midpoint),\n"
"                       where smax is the maximum of lateral distance 	\n"
"                       in inline direction				\n"
"aperl=lmax/4.          Migration aperature in crossline (l) direction (the\n"
"                       maximum lateral distance from l position of 	\n"
"                       the input midpoint to l position of output midpoint),\n"
"                       where lmax is the maximum of lateral distance 	\n"
"                       in crossline direction				\n"
"angmax=60.             maximum angle (from vertical) in degrees to output \n"
"                       migration from an input trace, i.e.,            \n"
"                          tmin = 2*dis/(v0*tan(angmax*3.141592654/180.)) \n"
"                       where tmin is the minimum time of an output trace, \n"
"                       dis is the distance between the output midpoint \n"
"                       and input midpoint, v0 is the surface velocity at \n"
"                       input midpoint position 			\n" 
"vmin=                  minimum velocity (default: read from velocity   \n"
"                       header file); must be specified if value 0 is \n"
"                       obtained from the velocity header. 		\n"
"tpow=-1.0              power of time gain to be applied to the input    \n"
"                       before migration 				\n"
"tpowaf=0.0             power of time gain to be applied to the output \n"
"                       after migration 				\n"
"isave=0                Mode of saving diskimg, diskhdr and diskfld	\n"
"                       0=remove the datasets after completion        	\n" 
"                       1=save the datasets after completion          	\n" 
"                       (will be sent to 1 when backupo is specified)	\n" 
"diskimg=DISKIMAGE      Disk name for storing imaged time sections 	\n"
"diskhdr=DISKHEADER     Disk name for storing output trace headers  	\n"
"diskfld=DISKFOLD       Disk name for storing output trace folds  	\n"
"backupi=               Name of backup tape name of the previous run	\n"
"                       (if given, e.g., stssxyz.ktmig.bck1, the backup \n"
"                        will be read in to overwrite the disk file of	\n"
"                        diskimg, diskhdr and diskfld; 			\n"
"                        otherwise, the program will use diskimg, 	\n"
"                        diskhdr and diskfld, if present)		\n" 
"backupo=               Name of backup tape name of the current run	\n"
"                       (if given, e.g., stssxyz.ktmig.bck2, the backup \n"
"                        tape will be made after the job is done;	\n"
"                        otherwise, only disk files will be created 	\n"
"                        if isave=1) 					\n"
"tapecntl=0             Input control (for tape or pipe  or disk input)	\n"
"                       0=tape or pipe input \n"
"                         The program will read and not process until 	\n"
"                         the input trace counter exceeds the number of \n"
"                         traces processed in the previous run of same input\n"
"                       1=more than one disk input \n"
"                         The program will skip the number of traces \n"
"                         processed in the previous run of the same input \n"
"                         before processing \n"
"                         This option should be used when multiple runs \n"
"                         (each with a single input) are needed	\n"
"                      -1=only one disk file input 	\n"
"                         The program will skip the total number of traces \n"
"                         processed in the previous run before processing \n"
"                       2=input from pipe, but processing will start from \n"
"                         first trace (like sudecomp input) \n"
"ntras=2000000000       Maximum number of traces to process in the 	\n"
"                       current run, before end of the input		\n"
"hisfile=KTMIG.HISFILE  histroy file name to indicate how many traces   \n"
"                       have been migrated in total (all runs) and in \n"
"                       current run	\n"
"                       (warning: remove this file before starting 	\n"
"                        migration if this is not a retart job, i.e.,   \n"
"                        the migration starts at the first input trace) \n" 
"                       If user does not specify hisfile, KTMIG.HISFILE \n"
"                       will be removed after normal completion of job. \n"
"f0=10.0                first high cut frequecy (Hz) in anti-aliasing 	\n"
"                       filters						\n" 
"df=10.0                high cut frequecy (Hz) increment in anti-aliasing\n"
"                       filters						\n" 
"nf=7                   number of anti-aliasing filters			\n"
"ftaper=5.0             frequency taper length (Hz) in the filters	\n"
"ksmax=0.5/dds          maximum wavenumber in line			\n"
"klmax=0.5/ddl          maximum wavenumber cross line			\n"  
"rmsmar=50.             maximum allowable ratio between sample amplitude\n"
"                       and the rms value of amplitudes of the whole 	\n"
"                       trace. When input amplitude/rms exceeds this 	\n"
"                       value, the trace will be treated as noise trace,\n"
"                       and not used in migration. A warning message 	\n" 
"                       will then be printed to a file (default to 	\n"
"                       BAD_TRACE_FILE)					\n"
"badtracefile=          name of file to record bad traces in the input	\n"
"                       (default to BAD_TRACE_FILE)			\n" 
"jpfile=                Job print file name                             \n"
"                          default: the standard error unit             \n"
"                            (a) create prefix.exxxx file if using qsub \n"
"                                on convex, where prefix is the first   \n"
"                                7 characters of the script file name   \n"
"                                and xxxx is the job number;            \n"
"                            (b) send e-mail when using at command;     \n"
"                            (c) send printed message to screen when    \n"
"                                running the job without qsub or at     \n"
"                                commands.                              \n"
"                          specified: the message will be printed       \n"
"                            in the named file as the job goes          \n"
"ntrdsk=                number of traces migrated per disk file update	\n"
"                       (diskimg and diskfld will be updated once for 	\n"
"                        every migrated ntrdsk traces;			\n" 
"                        default to the end of the job 			\n"
"ncpu=1                 number of CPUs to be used		\n"
"traceout=1             output migrated seismic trace (1=yes 0=no) \n"
"                       (traceout=0 may be used to skip output, if \n"
"                        more input dataset is going to be migrated \n"
"                        in the next run) \n"
"on2trace=0             total number of traces in the input is \n"
"                        specified at the first 4 bytes of the trace \n"
"                        header of the first trace (0=no 1=yes) \n"
"tr1=                   trace number at 1st corner of the 3D master grid\n"
"tr2=                   trace number at 2nd corner of the 3D master grid\n"
"tr3=                   trace number at 3rd corner of the 3D master grid\n"
"ln1=                   line number at 1st corner of the 3D master grid\n"
"ln2=                   line number at 2nd corner of the 3D master grid\n"
"ln3=                   line number at 3rd corner of the 3D master grid\n"
"tracekey=              segy trace header key word for trace number \n"
"linekey=               segy trace header key word for line number \n"
"strace=0               when (trace#-strace)%jthtrace==0, pass to migration \n"
"                       otherwise skip \n"
"jthtrace=1             pass every jth-trace to migration \n"
"mintrace=-2000000000   minimum input trace number to migrate \n"
"maxtrace=2000000000    maximum input trace number to migrate \n"
"sline=0                when (line#-sline)%jthline==0, pass to migration \n"
"                       otherwise skip \n"
"jthline=1              pass every jth-line to migration \n"
"minline=-2000000000    minimum input line number to migrate \n"
"maxline=2000000000     maximum input line number to migrate \n"
"\n"
"Notes:									\n"
"0. 1st corner of the 3D master grid is at minimum line and minimum trace \n"
"   2nd corner of the 3D master grid is at minimum line and maximum trace \n"
"   3rd corner of the 3D master grid is at maximum line and minimum trace \n"
"   when tracekey, linekey, tr1, tr2, tr3, ln1, ln2 and ln3 are given, \n"
"   the output trace headers will be updated at missing input trace location \n"
"   (trid=2) with line and trace numbers \n"
"1. Input data must be SEGY (IEEE) data format				\n"
"2. Input RMS velocity file consists of ntv*nsv*nlv floating point samples \n"
"   with/without grid header						\n" 
"3. s is the lateral position along the inline direction, l is the 	\n"
"   lateral position along the crossline direction. They may be different\n"
"   from the (x,y) position of the 3D survey. 				\n"
"4. source (sx,sy) and receiver (gx,gy) coordinates are used to determine\n"
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
"   each stage), use ntras, diskimg, diskhdr, diskfld papameters	\n"
"   if the input dataset is stored as disk file,			\n"
"   for example,							\n"
"	stage 1: migrate 50000 traces 					\n"
"	   ktmig ... ntras=50000 isave=1 diskimg=/huge/id/tmpimg.data 	\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupo=stssxyz.ktmig.backup1 hisfile=hf ...	\n"
"	stage 2: migrate next 50000 traces 				\n"
"	   ktmig ... ntras=50000 isave=1 diskimg=/huge/id/tmpimg.data 	\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupi=stssxyz.ktmig.backup1 hisfile=hf		\n"
"                    backupo=stssxyz.ktmig.backup2 ...			\n"
"	stage 3: migrate  the final 10000 traces 			\n"
"	   ktmig ... ntras=10000 isave=0 diskimg=/huge/id/tmpimg.data 	\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupi=stssxyz.ktmig.backup2 hisfile=hf		\n"
"\n"
"   If a job was completed normally at last stage, backupi should not	\n"
"   be specified unless diskimg, diskhdr and diskfld are bad, to save	\n"
"   the time of reading these three datasets into disk.			\n"
"               							\n"
"6. To migrate the 3-D dataset in stages (so that you get backups of    \n"
"   each stage), use diskimg, diskhdr, diskfld papameters and taperead	\n"
"   if the input dataset is stored as tapes,				\n"
"   for example,							\n"
"	stage 1: migrate first 5 tapes 					\n"
"          taperead sti=1 ntp=5 ... |					\n" 
"	   ktmig ... isave=1 diskimg=/huge/id/tmpimg.data 		\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupo=stssxyz.ktmig.backup1 ...			\n"
"	stage 2: migrate next 5 tapes 					\n"
"          taperead sti=6 ntp=5 ... |					\n" 
"	   ktmig ... isave=1 diskimg=/huge/id/tmpimg.data 		\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupi=stssxyz.ktmig.backup1 			\n"
"                    backupo=stssxyz.ktmig.backup2 ...			\n"
"	stage 3: migrate  the final 10 tapes 				\n"
"          taperead sti=11 ntp=10 ... |					\n" 
"	   ktmig ... isave=0 diskimg=/huge/id/tmpimg.data 		\n"
"                    diskhdr=/huge/id/tmphdr.data			\n" 
"                    diskfld=/huge/id/tmpfld.data			\n" 
"                    backupi=stssxyz.ktmig.backup2 			\n"
"\n"
"   If a job was completed normally at last stage, backupi should not	\n"
"   be specified unless diskimg, diskhdr and diskfld are bad, to save	\n"
"   the time of reading these three datasets into disk.			\n"
"   Do not specify ntras and hisfile parameters in this case.		\n"
"\n"
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
"              (s2 is usually =(ncdppl-1)*dds, l2 is usually =0.0)	\n"
"   (x3,y3) has the smallest s value and the largest l value		\n"
"              (s3 is usually =0.0, l3 is usually =(nlines-1)*ddl 	\n"
"   where 								\n"
"         ncdppl is the number of traces per line in the master grid	\n"
"         dds is the trace spacing (within a line) in the master grid	\n"
"         nlines is the number of lines in the master grid		\n"
"         ddl is the line spacing in the master grid 			\n"
"	  s is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of trace position within a 3d line 			\n"
"         l is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of line position within a 3d survey 			\n"
" 8. When doing velocity analysis, the output locations are		\n"
"       the output inline locations (s):				\n"
" sstart      (ss1),      ss1+dds,      ...,       ss1+(nds-1)*dds,	\n"
" sstart+ds   (ss2),      ss2+dds,      ...,       ss2+(nds-1)*dds 	\n"
" sstart+2*ds (ss3),      ss3+dds,      ...,       ss3+(nds-1)*dds 	\n"
"    ...       								\n"
" sstart+(ns-1)*ds (ssn), ssn+dds,      ...,       ssn+(nds-1)*dds      \n"
"       the output crossline locations (l):				\n"
" lstart      (ll1),      ll1+ddl,      ...,       ll1+(ndl-1)*ddl,	\n"
" lstart+dl   (ll2),      ll2+ddl,      ...,       ll2+(ndl-1)*ddl 	\n"
" lstart+2*dl (ll3),      ll3+ddl,      ...,       ll3+(ndl-1)*ddl 	\n"
"    ...       								\n"
" lstart+(nl-1)*ds (lln), lln+ddl,      ...,       lln+(ndl-1)*ddl      \n"
" 9. The primary memory requirement is 					\n"
"            ns*nds*nlcore*nofo*ntau*4 (byte) + velocity grid size (byte)	\n" 
"            + ns*nds*nlcore*nfft*4 (byte) \n"
"    nlcore is set to nl*ndl.						\n"
" 10. when nlines is less than 10, a 2.5-D rho filter is applied to 	\n"
"     compensate for 2D data input					\n"
"\n"
"AUTHOR:		Zhiming Li,        	3/93   			\n"
;

void migs(int nl, int ns, int nofo, float *mig, int ntau,
	FILE *imgfp, int ktrace, int ntl, float dldt, float *trace, 
	float *tras, float dtl, float tminl, int nt, 
	float *ss, float *sl, float *gs, float *gl, 
	int *imute, float *fovt2, float *s, float *l,
	float *tm, int *iofs, float apers, float aperl, float *fold, 
	float *w1, float *work1, float *work2, float *wsave, float *tracef,
	int *ifcut, int *ltaper, float ksmax, float klmax, 
	float f0, float df, int nf, float ftaper, int nsave, int nfft,
	float dtau, float tau0, float angmax,
	int *indxw,int nindxw, int incdxw,
	float *resamp, int ires, int ntres,
	float *ss2, float *scs, float *scg, int ncpu, 
	float  *vr2, float *vi2, float *vq4, float *tau);

void fhigint_(float *f0,float *df,int *nf,float *ftaper,float *dt,
	int *ifcut,int *ltaper,float * wsave,int *nsave,int *nfft);

void radix_(int *nt,int *nfft);

void bisear_(int *n,int *nnew,float *x,float *xnew,int *indx);

void f2p5n_(float *data,int *nt,int *i3d,int *m, int *l);

void vr22vi2_(float *vr2,float *vi2,int *ntau);
void vi22vq4_(float *vi2,float *vq4,int *ntau);
void vt2s2_(float *vr2,float *vq4,float *s2,float *tau,int *ntau);

void pstm3d_(float *trace,int *nt,float *t0,float *dt,float *sx,float *sy,
	float *gx,float *gy,int *imute,
	float *fovt2,float *mig,int *nxy,int *ntm,float *x,float *y,
	float *tm,float *aperx,float *apery,float *fold,float *w1,
	float *f0,float *df,int *nf,float *ftaper,
	int *ifcut,int *ltaper,float *tracef,
	float *wsave,int *nsave,float *work1,float *work2,int *nfft,
	float *kxmax,float *kymax,float *dtau,float *tau0,float *angmax,
	int *indxw,int *nindxw,int *incdx,int *np,
	float *resamp,int *ires,int *ntres,float *s2,float *scs,float *scg);


segytrace tra, tr;
segybhdr bh;
segychdr ch;


main(int argc, char **argv)
{
	int nlines, ncdppl;
   	int nt,ntau,it,ix,iy;
    	FILE *infp,*outfp,*velfp,*imgfp,*hdrfp,*hffp,*fldfp;
	float x1,x2,x3,y1,y2,y3,s1,s2,s3,l1,l2,l3,sm,lm;
	char *backupi="", *backupo="";
	char *diskimg="DISKIMAGE", *diskhdr="DISKHEADER";
	char *velfile,*datain,*dataout;
	char *diskfld="DISKFOLD", *hisfile="KTMIG.HISFILE";
	int ibacki, ibacko,isave, ihis=0, ipre=0;
	float ds, dl, tmp;
	int mtrace, ntrpre=0, ktrace, itrace, jtrace, ntrace, nofo;
	int ltrace; 
	int *imutel, cdppre, cdpnow, m1000, ifact, it0;
	int vread=0, ivdisk=1;
	float *velos;
	float *mig, *fold, *tm, *tmig, *tgain, *s, *l, *tugain, *trace, *tras;
	float *fovt2;
	float *ofo;
	int *off, itgain, nl, ns, ntras, ntv, is, il, io, nsv, nlv;
	int nsread, nlread;
	int mlimit, nlcore, nlpos, nlmig, *iofs, itv; 
	float tpow, sstart, lstart, t0, dtv, t0v, dsv, dlv, s0v, l0v;
	float tpowaf;
	float xs, ys, xg, yg, dt, dtau, tau0, v, *vel; 
	float *ss, *sl, *gs, *gl, ms, ml, ofomin, dofo, obtol; 
	char *buf;
	int ilv, isv, itmp, itmp2, ntl, itr, ir0, i;
	float tminl, dtl, dldt, apers, aperl;
	float *w1;
	float f0, df, ftaper;
	int nf, nfft, *ifcut, *ltaper, nsave;
	int mm,ll;
	float *work1, *work2, *tracef, *wsave;
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
	int ntrend, nsout, nlout, isend, ilend;
	float send, lend, dsout, dlout, dend, *disend;
	float ps, pl;
	int iofoin=0;
	float ofol, ofor;
	int one=1; 
	float *resamp;
	int ires, ntres; 
	int *indxw, nindxw, incdxw;
	int ntrdsk, ndskwt;

	float *vr2, *vi2, *vq4, *tau, *ss2, *scs, *scg;
	int i3d, ncpu, traceout;
	char *envs;
	int on2trace, ntotal;
	
	
	ghed gh;
	int n1,n2,n3,n4,n5,dtype,ierr,orient,gtype;
	float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5,ocdp2,oline3,dcdp2,dline3;
	float gmin=0.,gmax,scale;
	
	long long lwork, ltmp;
	long long lpos;

	float tr1,tr2,tr3,ln1,ln2,ln3,fline,ftrace;
	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;
	int intline, inttrace, ikey;
	int strace, jthtrace;
	int mintrace, maxtrace;
	int minline, maxline;
	int sline, jthline;    

	int ispre, isnow, ilpre, ilnow;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

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
    	if (!getparstring("velfile",&velfile)) err("must specify velfile");

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
   	if (!getparfloat("tpowaf",&tpowaf)) tpowaf = 0.;
	itgain = 0;
	if(tpow!=0. || tpowaf!=0.) itgain = 1;
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
    fgethdr(infp,&ch,&bh);

	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tra))  err("can't get first trace");
    nt = tra.ns;
    dt = (float)tra.dt/1000000.;
	t0 = (float)tra.delrt/1000;

	ntotal = tra.tracl;

	ntl = nt * ifact;
	dtl = dt/ifact;
	tminl = t0;
	dldt = dtl/dt;

	/* get velociry grid header info */
	velfp = efopen(velfile,"r");
	fseek2g(velfp,0,1);
	ierr = fgetghdr(velfp,&gh);
	if (ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,
        		&n4,&n5,&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
        		&dcdp2,&dline3,&ocdp2,&oline3,
        		&gmin,&gmax,&orient,&gtype);

	if(gmin==0. && ierr==0) 
		warn(" gmin=0 from velocity header file");

	if(gmin==0.) {
    		if (!getparfloat("vmin",&gmin)) err(" vmin must be specified");
	}

	/* optional parameters */
    	if (!getparint("ntv",&ntv)) {
		if(ierr==0) { ntv = n1; } else { err("Must specify ntv "); }
	}
    	if (!getparfloat("dtv",&dtv)) {
		if(ierr==0) { dtv = d1; } else { err("Must specify dtv "); }
	}
    	if (!getparfloat("t0v",&t0v)) { 
		if(ierr==0) { t0v = o1; } else { err("Must specify t0v "); }
	}
    	if (!getparint("nsv",&nsv)) {
		if(ierr==0) { nsv = n2; } else { err("Must specify nsv "); }
	}
    	if (!getparfloat("dsv",&dsv)) {
		if(ierr==0) { dsv = d2; } else { err("Must specify dsv "); }
	}
    	if (!getparfloat("s0v",&s0v)) { 
		if(ierr==0) { s0v = o2; } else { err("Must specify s0v "); }
	}
    	if (!getparint("nlv",&nlv)) {
		if(ierr==0) { nlv = n3; } else { err("Must specify nlv "); }
	}
    	if (!getparfloat("dlv",&dlv)) {
		if(ierr==0) { dlv = d3; } else { err("Must specify dlv "); }
	}
    	if (!getparfloat("l0v",&l0v)) { 
		if(ierr==0) { l0v = o3; } else { err("Must specify l0v "); }
	}

	if(dlv==0.)  dlv = dl;
	if(dsv==0.)  dsv = ds;
	 

   	if (!getparint("ntau",&ntau)) ntau = nt;
   	if (!getparfloat("dtau",&dtau)) {
		dtau = dt;
	} else {
		dtau = dtau * 0.001;
	}
   	if (!getparfloat("tau0",&tau0)) {
		tau0 = 0.;
	} else {
		tau0 = tau0 * 0.001;
	}
		
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
    	if (!getparfloat("df",&df)) df = 10.;
    	if (!getparint("nf",&nf)) nf = 7;
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
	bh.hns = ntau;
	bh.hdt = dtau * 1000000; 
	if(traceout==1) fputhdr(outfp,&ch,&bh);


   	if (!getparint("mlimit",&mlimit)) mlimit = 256;
   	mlimit = mlimit * 1024 * 1024;

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

	ibacki = 0;
	if (getparstring("backupi",&backupi)) {
		ibacki = 1;
		fprintf(jpfp," Backup from the previous run ");
                fprintf(jpfp," Backup %s , %s and %s from %s \n",
                        diskimg,diskhdr,diskfld,backupi);
		tar3fr(backupi,diskimg,diskhdr,diskfld);
	}

	ibacko = 0;
	if (getparstring("backupo",&backupo)) ibacko = 1;
	if(ibacko==1) isave=1;

	fprintf(jpfp,"\n");
	fprintf(jpfp," -------- KTMIG PRINTOUT -------- \n");
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


	/* compute memory requirement for migration*/
 
	ntmp = nl*ndl;
	nl = ntmp;
	ntmp = ns*nds;
	ns = ntmp;

	nlcore = nl;

	incdxw = 10;
	nindxw = ntau / incdxw + 1;
	ires = 4;
	ntres = nt*ires + 1;
	

	if(ntrend==0) {
		nsout = ns;
		nlout = nl;
	} else {
		nsout = ntrend;
		nlout = 1;
		nlcore = 1;
		dsout = (send-sstart)/(nsout-1);
		dlout = (lend-lstart)/(nsout-1);
		dend = sqrt ( (send-sstart)*(send-sstart)
		      + (lend-lstart)*(lend-lstart) ) / (nsout-1);
	}

	lwork = nsout * nlcore;
	lwork = lwork*ntau*nofo*sizeof(float);
	ltmp = nsout*nlout*nofo+nt + ntau + ntl + nt*mtrace;
	ltmp = ltmp + 2*ntau + 2*ntau*mtrace + 2*nsout*nlout + ntv + mtrace*4;
	ltmp = ltmp + ntau + nfft*(nf+nsout*nlout) + ntl*nf + nsave;
	ltmp = ltmp + nsout*nofo + ntres*nf + ntau;
	ltmp = ltmp + ntau*nsout*nlout*2 + ntau*mtrace + ntau*3;
	ltmp = ltmp * sizeof(float);
	ltmp = ltmp+(nsout*nlout*nofo+mtrace*2+nf*2+nindxw*nsout*nlout)*sizeof(int);
	lwork = lwork + ltmp;
	ltmp = ntv*nsv*nlv*sizeof(float);
	lwork = lwork + ltmp;

	ivdisk = 0;
	if(lwork>mlimit) {
		warn("Velocity Disk I/O Used to Reduce Memory \n");
		lwork = lwork - ltmp;
		ivdisk = 1;
	}

	if(lwork>mlimit) err("mlimit too small ");
	
	fprintf(jpfp," Total Memory used (in Byte) = %g \n", (float)lwork);
	fprintf(jpfp,"\n");

	/* allocation of memory */
	mig = (float*) emalloc(nsout*nlcore*nofo*ntau*sizeof(float));	
	fold = (float*) emalloc(nsout*nlout*nofo*sizeof(float));	
	off = (int*) emalloc(nsout*nlout*nofo*sizeof(int));	
	tgain = (float*) emalloc(nt*sizeof(float));
	tugain = (float*) emalloc(ntau*sizeof(float));
	trace = (float*) emalloc(ntl*sizeof(float));	
	tras = (float*) emalloc(nt*mtrace*sizeof(float));	
	imutel = (int*) emalloc(mtrace*sizeof(int));
	tm = (float*) emalloc(ntau*sizeof(float));
	tmig = (float*) emalloc(ntau*sizeof(float));
	fovt2 = (float*) emalloc(ntau*mtrace*sizeof(float));
	s = (float*) emalloc(nsout*nlout*sizeof(float));
	l = (float*) emalloc(nsout*nlout*sizeof(float));
	vel = (float*) emalloc(ntv*sizeof(float));
	ss = (float*) emalloc(mtrace*sizeof(float));
	sl = (float*) emalloc(mtrace*sizeof(float));
	gs = (float*) emalloc(mtrace*sizeof(float));
	gl = (float*) emalloc(mtrace*sizeof(float));
	iofs = (int*) emalloc(mtrace*sizeof(int));
	w1 = (float*) emalloc(ntau*sizeof(float));
	work1 = (float*) emalloc(nfft*nf*sizeof(float));
	tracef = (float*) emalloc(ntl*nf*sizeof(float));
	work2 = (float*) emalloc(nfft*nsout*nlout*sizeof(float));
	wsave = (float*) emalloc(nsave*sizeof(float));
	ifcut = (int*) emalloc(nf*sizeof(int));
	ltaper = (int*) emalloc(nf*sizeof(int));
	disend = (float*) emalloc(nsout*nofo*sizeof(float));
	resamp = (float*) emalloc(ntres*nf*sizeof(float));
	indxw = (int*) emalloc(nindxw*nsout*nlout*sizeof(int));

	ss2 = (float*) emalloc(ntau*sizeof(float));
	scg = (float*) emalloc(ntau*nsout*nlout*sizeof(float));
	scs = (float*) emalloc(ntau*nsout*nlout*sizeof(float));
	vr2 = (float*) emalloc(ntau*mtrace*sizeof(float));
	vi2 = (float*) emalloc(ntau*sizeof(float));
	tau = (float*) emalloc(ntau*sizeof(float));
	vq4 = (float*) emalloc(ntau*sizeof(float));

	if(ivdisk==0) velos = (float*) emalloc(ntv*nsv*nlv*sizeof(float));

	itrace = 0;

	bzero(fold,nofo*nsout*nlout*sizeof(float));
	bzero(mig,nofo*nsout*nlcore*ntau*sizeof(float));
	if(ntrend>0) {
		for(io=0;io<nsout*nofo;io++) disend[io] =  99999999999.;
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
		fprintf(jpfp,
		" Output s and l Locations of Target Line\n");
		for(is=0;is<ntrend;is++) {
			s[is] = sstart+is*(send-sstart)/(ntrend-1.);
			l[is] = lstart+is*(lend-lstart)/(ntrend-1.);
			fprintf(jpfp,
				"   s=%g l=%g trace=%d \n",s[is],l[is],is+1); 
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
		
	sinmin = sinmin - apers;
	sinmax = sinmax + apers;
	linmin = linmin - aperl;
	linmax = linmax + aperl;

	/* allocate disk space for image */
	if(ntrpre>0) {

		fprintf(jpfp," open diskimg file... \n");
		imgfp = efopen(diskimg,"r+");
		fseek2g(imgfp,0,1);
		ltmp = ntau*nofo*nlout*nsout;
		efread(mig,sizeof(float),ltmp,imgfp);
		fprintf(jpfp," diskimg file opened  \n");

		fprintf(jpfp," open diskfld file... \n");
		fldfp = efopen(diskfld,"r+");
		fseek2g(fldfp,0,1);
		efread(fold,sizeof(float),nofo*nsout*nlout,fldfp);
		efclose(fldfp);
		fprintf(jpfp," diskfld file opened  \n");

		fprintf(jpfp," open diskhdr file... \n");
		hdrfp = efopen(diskhdr,"r+");
		fseek2g(hdrfp,0,1);
		fprintf(jpfp," diskhdr file opened  \n");
		
		for(ix=0;ix<nsout*nlout*nofo;ix++) {
			efread(&tr,sizeof(char),240,hdrfp);
			off[ix] = tr.offset;
			if(ntrend>0 ) disend[ix] = tr.dz;
		}
		ipre = 1;
	} else if(isave==1) {
		imgfp = efopen(diskimg,"w+r");
		fseek2g(imgfp,0,1);
		bzero(tm,ntau*sizeof(float));
		fprintf(jpfp," initialize diskimg file... \n");
		for(ix=0;ix<nsout*nlout*nofo;ix++) 
			efwrite(tm,sizeof(float),ntau,imgfp);
		fprintf(jpfp," initialization of diskimg file done \n");
		if(nlcore==nlout) efclose(imgfp);

		fprintf(jpfp," initialize diskfld file... \n");
		fldfp = efopen(diskfld,"w+r");
		fseek2g(fldfp,0,1);
		bzero(fold,nofo*nsout*nlout*sizeof(float));
		efwrite(fold,sizeof(float),nofo*nsout*nlout,fldfp);
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
		if(ntrend>0) tr.dz = 99999999999.;
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
				fline = (ml-l1)*(ln3-ln1)/(l3-l1) + ln1 + 0.5;
				ftrace = (ms-s1)*(tr2-tr1)/(s2-s1) + tr1 + 0.5;
					intline = fline;
					inttrace = ftrace;
					itov(trktype,&trkval,inttrace);
					itov(lnktype,&lnkval,intline);
					puthval(&tr,indxtrk,&trkval);
					puthval(&tr,indxlnk,&lnkval);
				}
				for(io=0;io<nofo;io++) {
					tr.cdpt  = io+1;
					ofs = ofo[io];
					tr.sx = mx-ofs/2;
					tr.gx = mx+ofs/2;
					tr.sy = my;
					tr.gy = my;
					efwrite(&tr,sizeof(char),240,hdrfp);
				}
			}
		}
		fprintf(jpfp," initializing diskhdr file done \n");
		for(ix=0;ix<nofo*nsout*nlout;ix++) off[ix] = 99999999;
		fflush(hdrfp);
		
	}


	ktrace = 0;
	cdppre = tra.cdp - 1;
	ispre = -9999;
	ilpre = -9999;
	m1000 = 0;
	for(it=0;it<ntau;it++) {
		tmig[it] = tau0 + it*dtau;
		tm[it] = tmig[it]*0.5/dtl;
	}

	for(it=0;it<ntau;it++) tau[it] = tau0 + (it+1)*dtau;

	if(itgain==1) {
		for(it=0;it<nt;it++) {
			tmp = t0 + it*dt;
			if(tmp!=0.0) {
				tgain[it] = pow(tmp,tpow);
			} else {
				tgain[it] = 0.;
			}
		}
		for(it=0;it<ntau;it++) {
			tmp = tau0 + it*dtau;
			if(tmp!=0.0) {
				tugain[it] = pow(tmp,tpowaf);
			} else {
				tugain[it] = 0.;
			}
		}
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
	fprintf(jpfp," nds=%d ndl=%d dds=%g ddl=%g \n",
		nds,ndl,dds,ddl);
	fprintf(jpfp," nss=%d nll=%d is0=%d il0=%d \n",
		nss,nll,is0,il0);
	fprintf(jpfp," cdppdds=%d cdppds=%d cdppddl=%d cdppdl=%d cdpnum=%d\n",
		cdppdds, cdppds, cdppddl, cdppdl, cdpnum);
	fprintf(jpfp," ntrpre=%d nt=%d ntau=%d ntl=%d nfft=%d\n",
		ntrpre,nt,ntau,ntl,nfft);
	fprintf(jpfp," dt=%f dtau=%f t0=%f tau0=%f \n",
		dt*1000.,dtau*1000.,t0*1000.,tau0*1000.);
	fprintf(jpfp," tpow=%g ofomin=%g dofo=%g nofo=%d isave=%d \n",
		tpow,ofomin,dofo,nofo,isave);
	fprintf(jpfp," apers=%g aperl=%g angmax=%g tapecntl=%d\n",
		apers,aperl,angmax,tapecntl);
	fprintf(jpfp," nlcore=%d mtrace=%d ntras=%d mlimit=%d \n",
		nlcore,mtrace,ntras,mlimit/(1024*1024));
	fprintf(jpfp," f0=%g df=%g nf=%d ftaper=%g ksmax=%g klmax=%g\n",
		f0,df,nf,ftaper,ksmax,klmax);
	fprintf(jpfp," t0v=%g dtv=%g ntv=%d \n",t0v,dtv,ntv);
	fprintf(jpfp," s0v=%g dsv=%g nsv=%d \n",s0v,dsv,nsv);
	fprintf(jpfp," l0v=%g dlv=%g nlv=%d \n",l0v,dlv,nlv);
	fprintf(jpfp," diskimg=%s\n",diskimg);
	fprintf(jpfp," diskfld=%s\n",diskfld);
	fprintf(jpfp," diskhdr=%s\n",diskhdr);
	fprintf(jpfp," backupi=%s\n",backupi);
	fprintf(jpfp," backupo=%s\n",backupo);
	fprintf(jpfp," hisfile=%s\n",hisfile);
	fprintf(jpfp," ntrdsk=%d tpowaf=%g \n",ntrdsk,tpowaf);
	fprintf(jpfp," ncpu=%d traceout=%d \n",ncpu,traceout);
	if(on2trace==1) fprintf(jpfp," total_number_of_input_trace=%d \n",ntotal);
	if(ikey==1) { 
		fprintf(jpfp," tr1=%g tr2=%g tr3=%g \n",tr1,tr2,tr3);
		fprintf(jpfp," ln1=%g ln2=%g ln3=%g \n",ln1,ln2,ln3);
		fprintf(jpfp," tracekey=%s linekey=%s \n",tracekey,linekey); 
		fprintf(jpfp," strace=%d jthtrace=%d sline=%d jthline=%d \n",
			strace,jthtrace,sline,jthline);
		fprintf(jpfp," mintrace=%d maxtrace=%d \n",mintrace,maxtrace);
		fprintf(jpfp," minline=%d maxline=%d \n",minline,maxline);
	}
						    


	fprintf(jpfp," \n");
	

	fprintf(jpfp," start migration ... \n");
	fflush(jpfp);

	t0v = t0v * 0.001;
	dtv = dtv * 0.001;

	m1000 = ntrpre/1000;
	ntrace = 0;

	if(tapecntl==1) {
		if(jtrace>0) {
			lpos = nt*sizeof(float)+240;
			lpos = lpos * jtrace + 3600;
			fseek2g(infp,lpos,0);
			fgettr(infp,&tra);
		}
		ltrace = jtrace;
		jtrace = ntrpre;
	} else if(tapecntl==-1) {
		ltrace = 0;
		jtrace = ntrpre;
		lpos = nt*sizeof(float)+240;
		lpos = lpos * ntrpre + 3600;
		fseek2g(infp,lpos,0);
		fgettr(infp,&tra);
	} else if(tapecntl==0) {
		ltrace = jtrace;
		jtrace = ntrpre - jtrace;
	} else if(tapecntl==2) {
		ltrace = jtrace;
		jtrace = ntrpre;
	}

	i3d = 3;
	if(nlines<10) i3d=2;

	/* initialization of fft and taper for antialiasing filter */
	fhigint_(&f0,&df,&nf,&ftaper,&dtl,ifcut,ltaper,wsave,&nsave,&nfft);

	/* read in velocity file if ivdisk=0 */
	if(ivdisk==0) {
		fseek2g(velfp,0,0);
		efread(velos,sizeof(float),ntv*nsv*nlv,velfp);
	}

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


		ms = (ss[ktrace] + gs[ktrace])/2.;
		ml = (sl[ktrace] + gl[ktrace])/2.;

	fprintf(stderr,"ml=%g ms=%g cdp=%d offset=%d \n",
		ml,ms,tra.cdp,tra.offset); 

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

		if(io>=0 && io<=nofo-1) {
			tmp = tra.offset;
		 	if(iofoin==0 || nofo==1) {
				if(tmp<0.) tmp = - tmp;
			}
			tmp = tmp - ofo[io];
		      	if(tmp<0.) tmp =  - tmp;
			if(tmp>obtol*1.001) io=-1;
	 	}

	fprintf(stderr,"il=%d is=%d ml=%g ms=%g cdp=%d offset=%d io=%d\n",
		il,is,ml,ms,tra.cdp,tra.offset,io); 

		if(io<0 || io>=nofo) continue;

		if( ms < sinmin || ms >sinmax) continue;
		if( ml < linmin || ml >linmax) continue;

		iofs[ktrace] = io + 1;

		/* apply rho filter */
		f2p5n_(tra.data,&nt,&i3d,&mm,&ll);
		
		/* apply tpow */
                if(itgain==1) for(it=0;it<nt;it++) tra.data[it] *= tgain[it];
		for(it=0;it<nt;it++) tras[it+ktrace*nt] = tra.data[it];

			
		itmp = (tra.mute-tra.delrt);
		itmp = itmp*1000/(int)tra.dt*ifact + 1; 
		if(itmp<1) itmp = 1;
		if(itmp>ntl) itmp = ntl; 
		imutel[ktrace] = itmp;



		if(il>=0 && il<nlout && is>=0 && is<nsout) {
			if(ntrend==0) {
			   if(abs(tra.offset)<off[io+is*nofo+il*nofo*nsout]) {
				off[io+is*nofo+il*nofo*nsout] = abs(tra.offset);
				lpos=io+is*nofo+il*nofo*nsout;
				lpos=lpos*240;
				fseek2g(hdrfp,lpos,0);
				tmp = tra.mute*0.001;
				tmp = tmp*tmp-tra.offset*tra.offset/gmin/gmin; 
				if(tmp>=0.) {
					tmp = sqrt(tmp);
					tmp = tmp * 1000.;
					itmp = tra.mute;
					tra.mute = tmp;	
				}
				efwrite(&tra,sizeof(char),240,hdrfp);
				if(tmp>=0.) tra.mute = itmp;
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
			   	   if (abs(tra.offset)<off[io+isend*nofo]) {
				      tmp = sqrt( (ms-s[isend])*(ms-s[isend])
				          + (ml-l[isend])*(ml-l[isend]) );
				      if (tmp<disend[io+isend*nofo]) { 
					 disend[io+isend*nofo] = tmp;
					 tra.dz = tmp;
				         lpos=io+isend*nofo;
				         lpos=lpos*240;
					 fseek2g(hdrfp,lpos,0);
				         tmp = tra.mute*0.001;
				         tmp = tmp*tmp-tra.offset*tra.offset/
						gmin/gmin; 
				         if (tmp>=0.) {
					    tmp = sqrt(tmp);
					    tmp = tmp * 1000.;
					    itmp = tra.mute;
					    tra.mute = tmp;	
				         }
				         efwrite(&tra,sizeof(char),240,hdrfp);
				      }
				   }
			        }  
			}
		}

		/* find velocity */
		cdpnow = tra.cdp;
		isnow = is;
		ilnow = il;
		/*
		if(cdpnow!=cdppre || vread==0 ) { 
		*/
		if(isnow!=ispre || ilnow!=ilpre || vread==0 ) { 
			cdppre = cdpnow;
			ispre = isnow;
			ilpre = ilnow;
			tmp = (ms-s0v)/dsv + .5;
			isv = tmp;
			if(isv<0) isv = 0;
			if(isv>nsv-1) isv = nsv - 1;
			tmp = (ml-l0v)/dlv + .5;
			ilv = tmp;
			if(ilv<0) ilv = 0;
			if(ilv>nlv-1) ilv = nlv - 1;

			lpos = (isv + ilv*nsv)*ntv;
			lpos = lpos * sizeof(float);

			if(ivdisk==1) {
				fseek2g(velfp,lpos,0);
				efread(vel,sizeof(float),ntv,velfp);
			} else {
				for(it=0;it<ntv;it++) {
					vel[it] = velos[it+(isv+ilv*nsv)*ntv];
				}
			}
			vread = 1;
		}


		/* compute fovt2 = 4./(v*t)**2    */ 
		for(it=0;it<ntau;it++) {
			tmp = (tmig[it]-t0v)/dtv;
			itv = tmp;
			if(itv<0) {
				v = vel[0];
			} else if(itv>=ntv-1) {
				v = vel[ntv-1];
			} else {
				tmp = tmp - itv;
				v = (1.-tmp)*vel[itv]+tmp*vel[itv+1];
			}
			vr2[ktrace*ntau+it] = v*v;
			tmp = v*tmig[it];
			if(tmp==0.) tmp = v * dtau; 
			fovt2[ktrace*ntau+it] = 4./(tmp*tmp);
		}
		ktrace += 1;
			
		if(ktrace==mtrace || ktrace==ntras) {
			migs(nlout,nsout,nofo,mig,ntau,
				imgfp,ktrace,ntl,dldt,trace, 
				tras,dtl,tminl,nt,
				ss,sl,gs,gl, 
				imutel,fovt2,s,l,
				tm,iofs,apers,aperl,fold,
				w1,work1,work2,wsave,tracef,
				ifcut,ltaper,ksmax,klmax,
				f0,df,nf,ftaper,nsave,nfft,
				dtau,tau0,angmax,
				indxw,nindxw,incdxw,
				resamp,ires,ntres,
				ss2,scs,scg,ncpu,
				vr2,vi2,vq4,tau);
			itrace = itrace + ktrace;
			ndskwt = ndskwt + ktrace;
			ktrace = 0;
		}


		if(jtrace/1000>m1000) { 
		fprintf(jpfp," process done for %d input traces\n",jtrace);
			m1000 = jtrace/1000;
		fflush(jpfp);
		}	

		if(ndskwt>=ntrdsk && ntrdsk>0) {
		/* update disk file */
			fldfp = efopen(diskfld,"r+");
			fseek2g(fldfp,0,1);
			efwrite(fold,sizeof(float),nlout*nsout*nofo,fldfp);
			efclose(fldfp);
			imgfp = efopen(diskimg,"r+");
			fseek2g(imgfp,0,1);
			fwrite(mig,sizeof(float),nofo*nsout*nlout*ntau,imgfp);
			efclose(imgfp);
		fprintf(hffp,"Number of Traces Processed: %d %d \n",
				jtrace,jtrace-ntrpre+ltrace);
			fflush(hffp);
			ndskwt=0;
		}
	} while (ntrace<ntras && fgettr(infp,&tra));

	if(ktrace>0) {
		migs(nlout,nsout,nofo,mig,ntau,
			imgfp,ktrace,ntl,dldt,trace, 
			tras,dtl,tminl,nt, 
			ss,sl,gs,gl, 
			imutel,fovt2,s,l,
			tm,iofs,apers,aperl,fold,
			w1,work1,work2,wsave,tracef,
			ifcut,ltaper,ksmax,klmax,
			f0,df,nf,ftaper,nsave,nfft, 
			dtau,tau0,angmax, 
			indxw,nindxw,incdxw,
			resamp,ires,ntres,
			ss2,scs,scg,ncpu,
			vr2,vi2,vq4,tau);
		itrace = itrace + ktrace;
		ktrace = 0;
	fprintf(jpfp," processing done for total %d input traces \n",jtrace);
	}

	fprintf(jpfp," %d input traces processed in this run \n",ntrace);
	fflush(jpfp);
	ntrpre = ntrpre + ntrace;


	efclose(hdrfp);
	hdrfp = efopen(diskhdr,"r");
	fseek2g(hdrfp,0,1);

	if(isave==1 && ntrace>0) {
		fldfp = efopen(diskfld,"r+");
		fseek2g(fldfp,0,1);
		efwrite(fold,sizeof(float),nlout*nsout*nofo,fldfp);
		efclose(fldfp);
	} 
	if(isave==0) unlink(diskfld);

	if(ftell(btfp)==0) { 
		efclose(btfp);
		unlink("BAD_TRACE_FILE");
	} else {
		efclose(btfp);
	}
	if(nlcore<nlout || (ipre==1&&ntrdsk==0)) efclose(imgfp);

	if(isave==1 && nlcore==nlout)  {
		imgfp = efopen(diskimg,"r+");
		fseek2g(imgfp,0,1);
		fwrite(mig,sizeof(float),nofo*nsout*nlout*ntau,imgfp);
		efclose(imgfp);
	}

	if(isave==1) fprintf(jpfp," backup to disk done \n");

	fprintf(hffp,"Number of Traces Processed: %d 0 \n",ntrpre);
	efclose(hffp);
	if(ihis==0) unlink(hisfile);


	/* output */
	if(nlcore<nlout) fseek2g(imgfp,0,0);
	fseek2g(hdrfp,0,0);

	if( (ntrace == ntras || feof(infp)!=0) && traceout==1) {
	     fprintf(jpfp," start output ... \n");

	     for(iy=0;iy<nlout;iy++) {
		for(ix=0;ix<nsout;ix++) {
			for(io=0;io<nofo;io++) {
				itmp = (io*nsout*nlout+iy*nsout+ix);
				itmp2 = itmp*ntau;
				bcopy(mig+itmp2,tra.data,ntau*sizeof(float));
				if(fold[itmp]>1.) {
					scale = 1./fold[itmp];
					for(it=0;it<ntau;it++) 
						tra.data[it]=tra.data[it]*scale;
				}

				/* update headers */
				bzero(&tra,240);
				efread(&tra,sizeof(char),240,hdrfp);
				tra.offset = ofo[io];
				tra.tracl = ix + 1;
				tra.tracr = iy + 1;
				tra.cdpt = io + 1;
				tra.dz = 0.;
				ofs = ofo[io];
				if(tra.scalco>1) {
					tra.sx = tra.sx *tra.scalco;
					tra.sy = tra.sy *tra.scalco;
					tra.gx = tra.gx *tra.scalco;
					tra.gy = tra.gy *tra.scalco;
				} else if(tra.scalco<0) {
					tra.sx = tra.sx / (-tra.scalco);
					tra.sy = tra.sy / (-tra.scalco);
					tra.gx = tra.gx / (-tra.scalco);
					tra.gy = tra.gy / (-tra.scalco);
				}
				if(tra.trid==1) {
					mx = (tra.gx+tra.sx)/2;
                        		my = (tra.gy+tra.sy)/2;
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
					ms = s[iy*nsout+ix];
					ml = l[iy*nsout+ix];
					sl2xy(s1,l1,x1,y1,s2,l2,x2,y2,s3,l3,x3,y3,ms,ml,&mx,&my);
					tra.sx = mx-ofs/2;
					tra.gx = mx+ofs/2;
					tra.sy = my;
					tra.gy = my;
				}
 
				tra.scalco = 0;
				tra.delrt = tau0 * 1000.;
				tra.ns = ntau;
				tra.dt = dtau * 1000000.;
				if(tra.trid!=1) {
					tmp = (tau0 + (ntau-1)*dtau)*1000;
					tra.mute = tmp;
				}

				if(itgain==1) {
					for(it=0;it<ntau;it++) 
						tra.data[it]=tra.data[it]
							*tugain[it];

				}

				if(ntrend>0) {
					tmp = (l[iy*nsout+ix] - l1)/ddl + 0.5;
					iiy = tmp;
					tmp = (s[iy*nsout+ix] - s1)/dds + 0.5;
					iix = tmp;
					if(cdpnum==0) {
						tra.cdp = iiy*ncdppl+iix+cdp1;
					} else {
						tra.cdp = iix*nlines+iiy+cdp1;
					}
				}
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
	if(ivdisk==0) free(velos);

	/* backup to tape */
	if(ibacko==1) {
                fprintf(jpfp," Backup %s , %s and %s to %s \n",
                        diskimg,diskhdr,diskfld,backupo);
                tar3to(backupo,diskimg,diskhdr,diskfld);
        }

	return 0;
}



void migs(int nl, int ns, int nofo, float *mig, int ntau,
	FILE *imgfp, int ktrace, int ntl, float dldt, float *trace, 
	float *tras, float dtl, float tminl, int nt, 
	float *ss, float *sl, float *gs, float *gl, 
	int *imutel, float *fovt2, float *s, float *l,
	float *tm, int *iofs, float apers, float aperl, float *fold,
	float *w1, float *work1, float *work2, float *wsave, float *tracef,
	int *ifcut, int *ltaper, float ksmax, float klmax,
	float f0, float df, int nf, float ftaper, int nsave, int nfft,
	float dtau, float tau0, float angmax, 
	int *indxw,int nindxw, int incdxw,
	float *resamp, int ires, int ntres,
	float *s2, float *scs, float *scg, int ncpu, 
	float  *vr2, float *vi2, float *vq4, float *tau) {


	int il, itr, i, ir0, it, ii;
	float tmp;

	long lwork, ltmp;
	int nsl;

	nsl = ns*nl;
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
	
		/* 3d prestack time migration */
		ii = iofs[itr]-1;
		vr22vi2_(vr2+itr*ntau,vi2,&ntau);
		vi22vq4_(vi2,vq4,&ntau);
		vt2s2_(vr2+itr*ntau,vq4,s2,tau,&ntau);
		pstm3d_(trace,&ntl,&tminl,&dtl,&ss[itr],&sl[itr],
			&gs[itr],&gl[itr],&imutel[itr],
			fovt2+itr*ntau,mig+ii*ntau*nsl,&nsl,&ntau,s,l,
			tm,&apers,&aperl,fold+nsl*ii,w1,
			&f0,&df,&nf,&ftaper,
			ifcut,ltaper,tracef,
			wsave,&nsave,work1,work2,&nfft,
			&ksmax,&klmax,&dtau,&tau0,&angmax,
			indxw,&nindxw,&incdxw,&ncpu,
			resamp,&ires,&ntres,s2,scs,scg);
	}

}
