char *sdoc = 
"RMIG - Reflection-mapping depth migration of shot gathers \n"
"\n"
"rmig ep= datain= xtfile= [optional parameters] \n"
"\n"
"Required Parameters:\n"
"datain=                file name of shot gathers			\n"
"hfile=                 name of hfile 					\n"
"x0=                    minimum x position of migrated section		\n" 
"z0=                    minimum z position of migrated section		\n" 
"dx=                    x spacing of migrated section			\n" 
"dz=                    z spacing of migrated section			\n" 
"nx=                    number of x positions of migrated section	\n" 
"nz=                    number of z positions of migrated section	\n" 
"Optional Parameters:							\n"
"xtfile=                name of x-t file; ignored when confirm=0	\n"
"                       the xt file computed from csmodel (usually name \n"
"                       as csm_os=OS.listing, where OS is the starting	\n"
"                       source x position)				\n" 
"                       (if not given, csmodel is called to compute the \n"
"                       xtfile)						\n"
"dataout=               file name of imaged section (stacked after mig) \n"
"                       (if not given, no output)			\n"
"oep=1                  energy point number of shot in datain to model	\n"
"dep=1                  energy point number increment to migrate 	\n"
"nep=1                  number of shots to migrate			\n" 
"strace=0               starting trace position of the shot in datain 	\n" 
"                       (if 0, a header search will be performed to find\n"
"                        the shot position, input ep must be in 	\n"
"                        increasing order when strace=0) 		\n"
"spos0=1                shot point number of oep in xtfile 		\n"
"v0=1500                surface velocity at this shot			\n"
"maxng=240              maximum number of traces per shot		\n"
"maxne=32               maximum number of events in xtfile 		\n"
"maxar=3                maximum number of arrivals per event per receiver\n"
"mwd=1                  mapping width (in traces) per input trace 	\n"
"tpow=0.                power of time gain to be applied to input trace	\n"
"fcdp=1                 cdp number of first output trace		\n"
"dcdp=1                 cdp number increment of output traces		\n"
"confirm=0              confirm each shot migration before proceeding to\n"
"                       next one (0=no 1=yes)				\n"
"shotout=               name of dataset to save migrated traces of all 	\n"
"                       shots before stack (if not given, no output)	\n"
"mmap=0                 multiple mapping function allowed (0=no 1=yes)  \n"
"aper=9999999999.       migration aperature (maxmimum lateral distance 	\n"
"                       from midpoint to output positions)		\n"
"mmfile=migmute.file    migration mute file for interactive mute on	\n"
"                       migrated shot gater (used only when confirm=1) 	\n"
"                       mmfile is used to store stacked migrated shot 	\n"
"                       gathers and folds of stacking; It must be the	\n"
"                       same name if user does rmig in interactive and	\n"
"                       did restart rmig after migrating some shots in	\n"
"                       the previous runs				\n"
"history=rmig.history   history file to indicate where migration stoped \n"
"                       last time (used only when confirm=1)            \n"
"maxhn=                 maximum horizon number in hfile to be used in	\n"
"                       rmig						\n" 
"mapmax=0               mapping beyond the last ray-traced horizon	\n"
"                       to the maximum depth of migration (0=no 1=yes)  \n" 
"lpass=10.0             length (in second) of passing zone at each side \n"
"                       of computed arrival times to be applied to trace\n"
"                       before migration                      		\n"
"ltaper=-1.             length (in second) of tapering zone after       \n"
"                       passing zone:					\n"
"          		 						\n"
"          			     predicted arrival time		\n"
"          			       ^				\n"
"                                      |				\n"
"                               -------|-------				\n"
"                              .       |       .			\n"
"                             .        |        .			\n"
"          		 --------------|------------------		\n"
"          	             |  |  --->| lpass|<---			\n"
"                           ltaper					\n"
"									\n"
"                        when ltaper<0., it will be determined 		\n"
"                        automatically such that tapering will be done  \n"
"                        to the middle of the two arrival times		\n" 
"  vcid=0               velocity contrast interface display (when confirm=1)\n"
"                       (0=display interface regardless velocity contrast\n"
"                        1=display only when there is velocity contrast)\n"
"          								\n"
"\n"
" author: Zhiming Li, Jean-Claude Dulac and Kevin Hammel;          12/2/92 \n"
"\n";

#include "rmig.h"
#include "usgrid.h"
#include "su.h"

void ughupdate(usghed *ugh,int nz,int nx,float dz,float dx,float z0,float x0,
	int dcdp,int fcdp,int *ixlive,int *i2live,int *n2live);

int svUIi(char * dFile);

void rmfile(char *fname);

main (argc,argv)
int argc; char **argv;
{
	int ep, strace, ir, ie, ig, maxng, ix, ng, spos, maxar, mwd;
	int ep0, spos0, ns, dep, ishotout=1, mmap; 
	char *datain, *xtfile, *hfile, *dataout, *shotout;
	char xtnew[80];
	char *mmfile="migmute.file";
	char *mmshot="migmute.file.shot";
	char *shotdata="shot.data";
	char *history="rmig.history";
	char cmd[1024];
	FILE *infp, *xtfp, *outfp, *shotfp, *hisfp, *xtnewfp;
	segytrace tr, tro;
	segybhdr bh;
	segychdr ch;

	usghed ugh, ughshot;
	
	float *xm,*tm,*sx,*gx,*scale;
	float dt, *tmute, *t0, v0, *trace;
	int nx,nz,maxnr,n2,*nxt,maxne,ne,nt,iz, it;
	float x0, z0, dx, dz,*zmig;
	float *xe, *ze, *te;
	float *xs, *ts, *xhs, *zhs;
	int *index, *idg, *nes;
	float *sort, tmp, aper;
	int jg, maxnes, is; 
	float *tgain, tpow;
	int iout=1, fcdp, dcdp;
	int ixt=1;
	float r1,r2,r3,r4,dr,sxx;
	int *iee, *ier, *iray;


	int confirm, itrps, icount=0;
	FILE *tty, *datafp, *sdfp;
	float *mig, *fold, *migshot, *foldshot;
	int *ixlive, itmp, *ixshot;
	float clip;
/*	char ayes, nshs[4]; */
	char nshs[4];
	int nsnext=1, nscount=0;

  /* User Interface strings */
  char *uiAskAgain="rmigAskAgain.d";
  char *uiDone="rmigDone.d";
  char *uiNsNext="rmigNsNext.d";
  char *uiReMigrate="rmigReMigrate.d";
  char *uiReRayTrace="rmigReRetrace.d";

	int nsmig=0, vcid;
	char *buf;

	int isht0, nsht;
	int imigedit=0, sortz=0;
	int irayedit=0;
	int maxhn, ih, ipos, jh, mapmax;

	float ltaper, lpass;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* obtain parameters */
        if (!getparstring("datain", &datain)) err(" datain must be specified ");
        if (!getparstring("dataout", &dataout)) iout=0;
	if (!getparstring("xtfile", &xtfile)) {
		ixt = 0;
		xtfile = (char*) malloc(80*sizeof(char));
	}
	if (!getparstring("hfile", &hfile)) err("hfile missing");
        if (!getparfloat("x0", &x0)) err("x0 missing");
        if (!getparfloat("z0", &z0)) err("z0 missing");
        if (!getparfloat("dx", &dx)) err("dx missing");
        if (!getparfloat("dz", &dz)) err("dz missing");
        if (!getparint("nx", &nx)) err("nx missing");
        if (!getparint("nz", &nz)) err("nz missing");
        if (!getparint("maxng", &maxng)) maxng = 240 ;
        if (!getparint("spos0", &spos0)) spos0 = 1;
        if (!getparint("oep", &ep0)) ep0=1;
        if (!getparint("dep", &dep)) dep=1;
        if (!getparint("nep", &ns)) ns=1;
        if (!getparint("strace", &strace)) strace=0;
        if (!getparfloat("v0", &v0)) v0=1500;
        if (!getparint("mwd", &mwd)) mwd=1;
        if (!getparfloat("tpow", &tpow)) tpow=0.;
        if (!getparint("fcdp", &fcdp)) fcdp=1;
        if (!getparint("dcdp", &dcdp)) dcdp=1;
        if (!getparint("confirm", &confirm)) confirm=0;
        if (!getparstring("shotout", &shotout)) ishotout=0;
        if (!getparint("mmap", &mmap)) mmap=0;
        if (!getparfloat("aper", &aper)) aper=9999999999.;
        if (!getparfloat("ltaper", &ltaper)) ltaper = -1.0;
        if (!getparfloat("lpass", &lpass)) lpass = 10.;
	getparstring("mmfile", &mmfile);
	getparstring("mmshote", &mmshot);
	getparstring("shotdata", &shotdata);
	getparstring("history", &history);
        if (!getparint("maxhn", &maxhn)) maxhn=0;
        if (!getparint("mapmax", &mapmax)) mapmax=0;
        if (!getparint("vcid", &vcid)) vcid=0;

	mig = (float*) malloc(nx*nz*sizeof(float));
	fold = (float*) malloc(nx*nz*sizeof(float));
	bzero(mig,nx*nz*sizeof(float));
	bzero(fold,nx*nz*sizeof(float));

	if(confirm==1) {
		if((hisfp = fopen(history,"r"))!=NULL) {
			fclose(hisfp);
			hisfp = efopen(history,"r+");
			if((datafp = fopen(mmfile,"r"))==NULL) { 
			    warn("mmfile=%s not found for restarted migration",
					mmfile);
			    err("Or delete history=%s for new migration", 
					history);
			} else {
				efclose(datafp);
			}
			buf = (char *) malloc(81*sizeof(char));
			do {
				bzero(buf,81);
				if(fgets(buf,81,hisfp)==NULL) break;
				if(strncmp(buf, 
					"Number of Shots Migrated:",25)==0) {
					sscanf(buf+25,"%d",&nsmig);
				} else if (strncmp(buf,
					"Trace Position of Next Shot:",28)==0) {
					sscanf(buf+28,"%d",&strace);
				} else if (strncmp(buf,
					"Source Point of Next Shot:",26)==0) {
					sscanf(buf+26,"%d",&ep0);
				}
			} while(feof(hisfp)==0);
			free(buf);
			fprintf(stderr," From history file: \n");
			fprintf(stderr,"Number of Shots Migrated: %d\n",nsmig);
			fprintf(stderr,"Trace Position of Next Shot: %d\n",
				strace);
			fprintf(stderr,"Source Point of Next Shot: %d\n",
				ep0);
		} else {
			hisfp = efopen(history,"w");
			datafp = efopen(mmfile,"w");
			fwrite(mig,sizeof(float),nx*nz,datafp);
			fwrite(fold,sizeof(float),nx*nz,datafp);
			bzero(&ugh,100);
			ugh.scale = 1.e-6;
			ugh.dtype = 4;
			ugh.n1 = nz;
			ugh.n2 = nx;
			ugh.n3 = 2;
			ugh.n4 = 1;
			ugh.n5 = 1;
			ugh.d1 = dz;
			ugh.d2 = dx;
			ugh.d3 = 1.;
			ugh.o1 = z0;
			ugh.o2 = x0;
			ugh.o3 = 1.;
			ugh.dcdp2 = dcdp;
			ugh.ocdp2 = fcdp;
			fputusghdr(datafp,&ugh);
			fclose(datafp);
		}

	}

	/* open xtfile */
	if(ixt==1 && confirm==0 ) {
        	if( (xtfp = fopen(xtfile,"r"))==NULL) err("xtfile not found"); 
	}
        if(!getparint("maxar",&maxar)) maxar =3;
	maxnr = maxng * maxar;
        if(!getparint("maxne",&maxne)) maxne = 32;
        /* array allocations */
        nxt = (int*) malloc(maxne*sizeof(int));
        xs = (float*) malloc(maxne*maxnr*sizeof(float));
        ts = (float*) malloc(maxne*maxnr*sizeof(float));
        xhs = (float*) malloc(maxne*maxnr*sizeof(float));
        zhs = (float*) malloc(maxne*maxnr*sizeof(float));
	iray = (int*) malloc(maxnr*maxne*sizeof(int));
	scale = (float*) malloc(mwd*sizeof(float));

	/* open and search for the first shot gather */
	infp = efopen(datain,"r");
	fgethdr(infp, &ch, &bh);
	if (!fgettr(infp,&tr)) err("can't get first trace");
	dt = (float)tr.dt * 0.000001;
	nt = tr.ns;
	if(strace==0) {
		efseek(infp,0,0);
                strace = hdsearch(infp,"ep",(double)ep0);
		strace = (strace-3600)/(tr.ns*sizeof(float)+240);
        }else {
		efseek(infp,3600+(strace-1)*(tr.ns*sizeof(float)+240),0);
	}
	trace = (float*) malloc(nt*maxng*sizeof(float));
	sx = (float*) malloc(maxng*sizeof(float));
	gx = (float*) malloc(maxng*sizeof(float));
	tm = (float*) malloc(maxng*sizeof(float));
	t0 = (float*) malloc(maxng*sizeof(float));
	tmute = (float*) malloc(maxng*sizeof(float));
	tgain = (float*) malloc(nt*sizeof(float));

	if(tpow!=0.) {
		tmp = tr.delrt * 0.001;
		for(it=0;it<nt;it++) {
			if(tmp+it*dt!=0.) {
				tgain[it] =  pow(tmp+it*dt,tpow); 
			} else {
				tgain[it] =  0.;
			}
		}
	} else {
		for(it=0;it<nt;it++) tgain[it] = 1.;
	}

	zmig = (float*) malloc(nz*sizeof(float));
	index = (int*) malloc(nz*sizeof(int));
	ixlive = (int*) malloc(nx*sizeof(int));
	bzero(ixlive,nx*sizeof(int));

	if(ishotout==1) {
		migshot = (float*) malloc(nx*nz*sizeof(float));
		foldshot = (float*) malloc(nx*nz*sizeof(float));
		ixshot = (int*) malloc(nx*sizeof(int));
	}

	for(iz=0;iz<nz;iz++) zmig[iz] = z0 + iz*dz;

	/* prepare for output shot gathers */
	if(ishotout==1) {

		shotfp = fopen(shotout,"w");
		bh.hns = nz;
		bh.ntrpr = nx;
		auxputhdr(shotfp, &ch, &bh);
		bzero((char*)&tro,240);
		tro.ns = nz;
		tro.dz = dz;
		tro.fz = z0;
		tro.dt = tr.dt;
		tro.mute = 0;
		tro.muts = 0;
		tro.trid = 1;
		tro.scalco = 1;
		tro.scalel = 1;
		tro.duse = 1;
		tro.delrt = z0;
		tro.cdpt = 1;
	}

	for(is=nsmig;is<ns;is++) {
		
		spos = spos0 + is; 
		bzero(trace,nt*maxng*sizeof(float));

		/* read in the shot gather */
		ig = 0;
		ep = ep0 + (is-nsmig) * dep;

		do {
			if(!fgettr(infp,&tr)) goto quit;
			if(tr.ep==ep) break;
		} while (TRUE);

		do {
			if(tr.ep==ep) {
				if(ig==0 && confirm==1 && ixt==0) {
					strace = (eftell(infp)-3600)/
						 	(nt*sizeof(float)+240);
					strace = strace;
				}
                        	sx[ig] = tr.sx;
				gx[ig] = tr.gx;
				t0[ig] = (float) tr.delrt * 0.001;
				tmute[ig] = (float) tr.mute * 0.001;
				tm[ig] = (float) fabs((double)tr.offset) /v0;
				for(it=0;it<nt;it++) 
					trace[it+ig*nt] = tr.data[it]*tgain[it];
				ig = ig + 1;
			} else {
				efseek(infp,-tr.ns*sizeof(float)-240,1);
				break;
			}
		} while (ig<maxng && fgettr(infp,&tr));
		ng = ig;

		if(ng<4) continue; 

		sort = (float*) malloc(ng*sizeof(float));
		nes = (int*) malloc(ng*sizeof(int));
		idg = (int*) malloc(ng*sizeof(int));

		for(ig=0;ig<ng;ig++) idg[ig] = ig;
		qkisort(ng,gx,idg);
		for(ig=0;ig<ng;ig++) sort[ig] = gx[idg[ig]];

		/* compute the xt file if needed */

		reraytrace:

		if(ixt==0) {
			bzero(cmd,1024);
			sxx = sx[0];
			findrs(sort,ng,sxx,&r1,&r2,&r3,&r4,&dr);

                         sprintf(cmd,
"csmodel hfile=%s os=%g r1=%g r2=%g r3=%g r4=%g dr=%g rfsave=1 comp=-1 ",
                         	hfile,sxx,r1,r2,r3,r4,dr);
			if(maxhn!=0) {
				ipos = strlen(cmd);
				sprintf(&cmd[ipos],"hn="); 
				ipos = ipos + 3;
				for(ih=2;ih<maxhn;ih++) {
					sprintf(&cmd[ipos],"%d,",ih); 
					jh = log10(ih) + 1;
					ipos = ipos + jh + 1;
				}
				sprintf(&cmd[ipos],"%d",maxhn); 
			}

                        system(cmd);
	
			/* clean up ray tracing files */
			bzero(cmd,1024);
			sprintf(cmd,
			"/bin/rm -f csm_os=%g.data plotcolors_os=%g",sxx,sxx);
			system(cmd);
			bzero(cmd,1024);
			sprintf(cmd,
			"/bin/rm -f geometry-file_os=%g model-file_os=%g",
				sxx,sxx);
			system(cmd);
			bzero(cmd,1024);
			sprintf(cmd,
			"/bin/rm -f param1_1_os=%g param1_2_os=%g",sxx,sxx);
			system(cmd);
			bzero(cmd,1024);
			sprintf(cmd,
			"/bin/rm -f param1_os=%g param2_os=%g",sxx,sxx);
			system(cmd);

			/* get the xt file names */
			bzero(xtfile,80);
			sprintf(xtfile,"csm_os=%g.listing\0",sxx);
			bzero(xtnew,80);
			sprintf(xtnew,"csmlisting.new\0");
			bzero(cmd,1024);
                       	sprintf(cmd,"cp %s %s",xtfile,xtnew);
			system(cmd);
			rmfile(xtfile);

			/* overlay and reflection mapping path editing */
			/* x-window ray editing */ 
/*
                        if(confirm==1 && nsnext<=1) {
				bzero(cmd,1024);
                                sprintf(cmd,
"xtedit ep=%d strace=%d maxng=%d datain=%s perc=99 xtfile=%s xtnew=%s &\n",
				ep,strace,ng,datain,xtfile,xtnew);
                        	system(cmd);
				bzero(cmd,1024);
                                sprintf(cmd,
"xzedit xtfile=%s hfile=%s mmap=%d aper=%f sortz=%d xtnew=%s vcid=%d &\n",
				xtfile,hfile,mmap,aper,sortz,xtnew,vcid);
                        	system(cmd);
			}

*/
			/* edit rays */
			/* SeisView rayedit */
/* */

			if(confirm==1 && nsnext<=1 ) {
				if(irayedit==0) {
					irayedit = 1;
					sdfp = efopen(shotdata,"w");
					bh.hns = nt;
					bh.ntrpr = ng;
					auxputhdr(sdfp, &ch, &bh);
					bcopy((char*)&tr,(char*)&tro,240);
					for(ig=0;ig<ng;ig++) {
						tro.sx = sx[ig];
						tro.gx = gx[ig];
						tro.offset = tro.gx - tro.sx;
						for(it=0;it<nt;it++) {
							tro.data[it]=
								trace[ig*nt+it];
						}
						auxputtr(sdfp,&tro);
					}
					fclose(sdfp);
					bzero(cmd,1024);
					sprintf(cmd, 
	"rayedit rayedit.d hfile=%s csmodel=%s data=%s 2> /dev/null",
						hfile,xtnew,shotdata);
					sdfp = epopen(cmd,"w");
				} else {
					auxputhdr(sdfp, &ch, &bh);
					bcopy((char*)&tr,(char*)&tro,240);
					for(ig=0;ig<ng;ig++) {
						tro.sx = sx[ig];
						tro.gx = gx[ig];
						tro.offset = tro.gx - tro.sx;
						for(it=0;it<nt;it++) {
							tro.data[it]=
								trace[ig*nt+it];
						}
						auxputtr(sdfp,&tro);
					}
					fflush(sdfp);
				}
			}

/* */

			if(confirm==1 && nsnext<=1 ) {
				raywait:
           if(!svUIi(uiDone)) goto raywait;
        /*
                		printf(" Have you finished rayedit? (y/n)->");
                                scanf("%1s",&ayes) ;
        */
				/*
				tty = efopen("/dev/tty","r");
                	fprintf(stderr, " Have you finished rayedit? (y/n)->");
                		ayes = fgetc(tty);
                		efclose(tty);
				*/
        /*
				   if(ayes=='n') goto raywait; 
        */
			}

		}
		
		remigrate:

		bzero(nxt,maxne*sizeof(int));
		if(ishotout==1) {
			bzero(migshot,nz*nx*sizeof(float)); 
			bzero(foldshot,nz*nx*sizeof(float)); 
			bzero(ixshot,nx*sizeof(int)); 
		}
		if(ixt==0) {
			xtnewfp = efopen(xtnew,"r");
        		getxrt(xtnewfp, maxnr, &ne, nxt, xs, ts, xhs, zhs, 1);
			efclose(xtnewfp);
		} else {
        		/* read xt file at this shot */
        		getxrt(xtfp, maxnr, &ne, nxt, xs, ts, xhs, zhs, spos);
		}

		/*
                fprintf(stderr, " after getxrt \n");
		*/

		/* event selections at receiver locations */
		maxnes = ne * maxar; 
		xe = (float*) malloc(maxnes*ng*sizeof(float));
		ze = (float*) malloc(maxnes*ng*sizeof(float));
		te = (float*) malloc(maxnes*ng*sizeof(float));
		iee = (int*) malloc(maxnes*ng*sizeof(int));
		ier = (int*) malloc(maxnes*ng*sizeof(int));

		bzero(iray,maxnr*maxne*sizeof(int));
		for(ie=0;ie<ne;ie++) {
			for(ir=0;ir<nxt[ie];ir++) {
				iray[ie*maxnr+ir] = 1;
			}
		}

		/*
                fprintf(stderr, " before evsel \n");
		*/

		evsel(ng, ne, maxnr, maxnes, mmap,
        		sx, sort,
			xs, ts, xhs, zhs, nxt, iray, 
        		xe, ze, te, nes, ier, iee,
			sortz, aper);

		/*
                fprintf(stderr, " after evsel \n");
		*/

                /*fprintf(stderr, "\n");*/

		/* event-mapping depth migration */
		for(ig=0;ig<ng;ig++) {
			jg = idg[ig];

			taper(nt,t0[jg],dt,trace+jg*nt,nes[ig],te+ig*maxnes,
				lpass,ltaper);

			if(ishotout==0 ) {
			  	emig_(&nx,&nz,&x0,&dx,mig,&nt,&t0[jg],
				&dt,&tmute[jg],trace+jg*nt,scale,&nes[ig],
				xe+ig*maxnes,ze+ig*maxnes,te+ig*maxnes,zmig,
				&tm[jg],fold,index,&mwd,&sx[jg],&gx[jg],
				ixlive,&mapmax,&aper);
			} else if(ishotout==1) {
			  	emig_(&nx,&nz,&x0,&dx,migshot,&nt,&t0[jg],
				&dt,&tmute[jg],trace+jg*nt,scale,&nes[ig],
				xe+ig*maxnes,ze+ig*maxnes,te+ig*maxnes,zmig,
				&tm[jg],foldshot,index,&mwd,&sx[jg],&gx[jg],
				ixshot,&mapmax,&aper);
			}
		}

		free(xe);
		free(ze);
		free(te);
		free(iee);
		free(ier);

		if(ishotout==1) {
			itrps = 0;
			for(ix=0;ix<nx;ix++) {
				if(ixshot[ix]==1) {
					itmp = ix*nz;
					for(iz=0;iz<nz;iz++) {
						mig[iz+itmp] +=
							migshot[iz+itmp];
						fold[iz+itmp] +=
							foldshot[iz+itmp];
					}
					ixlive[ix] = 1;

					for(iz=0;iz<nz;iz++) {
						if(foldshot[iz+itmp]>1.) {
							tro.data[iz] = 
							migshot[iz+itmp]/
							foldshot[iz+itmp];
						} else {
							tro.data[iz] = 
							migshot[iz+itmp];
						}
					}
					icount += 1;
					itrps += 1;
					tro.tracl = icount;
					tro.tracr = itrps;
					tro.ep = ep;
					tro.fldr = ep;
					tro.cdp = fcdp + ix*dcdp;
					tro.sx = sxx;
					tro.gx = 2*(x0+ix*dx)-sxx;
					tro.offset = tr.gx - tr.sx;
					auxputtr(shotfp,&tro);
				}
			}
		}

		/* view the shot migrated, apply mute, etc. */
		if(confirm==1 && ( nscount==0 || is==ns-1) ) {

			if(imigedit==0) {
				imigedit = 1;
				/* update grid headers */
				ughupdate(&ughshot,nz,nx,dz,dx,z0,x0,dcdp,fcdp,
					ixlive,&isht0,&nsht);

				/* write to shot grid */
				datafp = efopen(mmshot,"w");
				efwrite(mig+isht0*nz,sizeof(float),
					nz*nsht,datafp);
				efwrite(fold+isht0*nz,sizeof(float),
					nz*nsht,datafp);
				fputusghdr(datafp,&ughshot);
				efclose(datafp);

				/* pick, apply migration mute and stack */

                        	bzero(cmd,1024);
                        	sprintf(cmd,
		     "migedit data=%s hfilein=%s shot=%s 2>/dev/null",
                                	mmfile,hfile,mmshot);
				datafp = epopen(cmd,"w"); 
			} else {
				/* update grid headers */
				ughupdate(&ughshot,nz,nx,dz,dx,z0,x0,dcdp,fcdp,
					ixlive,&isht0,&nsht);
				fputusghdr(datafp,&ughshot);
				efwrite(mig+isht0*nz,sizeof(float),
					nz*nsht,datafp);
				efwrite(fold+isht0*nz,sizeof(float),
					nz*nsht,datafp);
				fflush(datafp);
			}

			bzero(mig,nx*nz*sizeof(float));
			bzero(fold,nx*nz*sizeof(float));
			bzero(ixlive,nx*sizeof(int));
		
			/* if re-migration is needed */
      			if(svUIi(uiReMigrate)) goto remigrate;

      /*
                	printf(
    		" Remigrate of the current shot, after Rayedit? (y/n) ->");
                        scanf("%1s",&ayes) ;
			if(ayes=='y') goto remigrate;
      */

			/* if re-ray tracing is needed */
      			if(svUIi(uiReRayTrace)) goto reraytrace;
      /*
                	printf(
    		" Ray tracing again for the current shot? (y/n) ->");
                        scanf("%1s",&ayes) ;
			if(ayes=='y') goto reraytrace;
      */




			/* update the history file */
			fseek(hisfp,0,0);
			fprintf(hisfp,"Number of Shots Migrated: %d\n",is+1);
			fprintf(hisfp,"Trace Position of Next Shot: %d\n",
				strace+ng);
			fprintf(hisfp,"Source Point of Next Shot: %d\n",
				ep+dep);
			fflush(hisfp);

			/* check with user about the migedit */
         		nsnext=svUIi(uiNsNext);
      /*
                	printf(
    	" Number of Shots to Migrate Next (AFTER SAVING MIGEDIT RESULT!!) ->");
			scanf("%d",&nsnext);
      */
			/*
                	fprintf(stderr,
    	" Number of Shots to Migrate Next (AFTER SAVING MIGEDIT RESULT!!) ->");
			tty = efopen("/dev/tty","r");
                	gets(nshs);
                	efclose(tty);
			*/

			/* clean up xt files */
			rmfile(xtfile);
			rmfile(xtnew);

			if(nsnext==0) goto quit; 
			nscount = nsnext;
		} else if(confirm==0 && ixt==0) {
			/* clean up xt files */
			rmfile(xtfile);
			rmfile(xtnew);
		}

		if (nsnext==1) {
			nscount = 0;
		} else {
			nscount = nscount - 1;
		}


		free(nes);
		free(idg);
		free(sort);

		fprintf(stderr,"migration done for %d traces at shot %d \n",
				ng,is+1); 

	}

	quit:


	if(confirm==1) {
		askagain:
      if(!svUIi(uiAskAgain)) goto askagain;
     /*
                printf(" Have you exited from migedit? (y/n)->");
		scanf("%1s",&ayes);
    */
		/*
		tty = efopen("/dev/tty","r");
                fprintf(stderr, " Have you exited from migedit? (y/n)->");
                ayes = fgetc(tty);
                efclose(tty);
		*/
    /*
		if(ayes=='n') goto askagain;
    */
		/*
		epclose(sdfp);
		epclose(datafp);
		*/
		datafp = efopen(mmfile,"r");
		efread(mig,sizeof(float),nz*nx,datafp);
		efread(fold,sizeof(float),nz*nx,datafp);
		efclose(datafp);

		unlink(mmshot);
		unlink(shotdata);
	}

	
	for(iz=0;iz<nx*nz;iz++) {
		if(fold[iz]>1.) mig[iz] = mig[iz]/fold[iz];
	}

	if(iout==1) {

		outfp = fopen(dataout,"w");
		bh.fold = 1;
		bh.tsort= 4;
		bh.hns = nz;
		bh.ntrpr = nx;
		fputhdr(outfp, &ch, &bh);
		tr.ns = nz;
		tr.dz = dz;
		tr.fz = z0;
		tr.mute = 0;
		tr.muts = 0;
		tr.trid = 1;
		tr.scalco = 1;
		tr.scalel = 1;
		tr.duse = 1;
		tr.delrt = z0;
		tr.cdpt = 1;
		tr.offset = 0;

		for(ix=0;ix<nx;ix++) {
			tr.tracl = ix + 1;
			tr.tracr = ix + 1;
			tr.ep = ix + 1;
			tr.fldr = ix + 1;
			tr.cdp = fcdp + ix*dcdp;
			tr.sx = x0 + ix*dx;
			tr.gx = tr.sx;
		
			for(iz=0;iz<nz;iz++) tr.data[iz] = mig[iz+ix*nz];
			fputtr(outfp,&tr);
		}
		fclose(outfp);
	}
        return 0 ;
}


/* update the unscaled header of a grid, and return pointer position to the
grid array */

void ughupdate(usghed *ugh,int nz,int nx,float dz,float dx,float z0,float x0,
	int dcdp,int fcdp,int *ixlive,int *i2live,int *n2live) {

		int i2, n2, i20;
		float o2;
			bzero((char*)ugh,100);
                        ugh->scale = 1.e-6;
                        ugh->dtype = 4;
                        ugh->n1 = nz;
                        ugh->n2 = nx;
                        ugh->n3 = 2;
                        ugh->n4 = 1;
                        ugh->n5 = 1;
                        ugh->d1 = dz;
                        ugh->d2 = dx;
                        ugh->d3 = 1.;
                        ugh->o1 = z0;
                        ugh->o2 = x0;
                        ugh->o3 = 1.;
                        ugh->dcdp2 = dcdp;
                        ugh->ocdp2 = fcdp;

			i20 = 0;
			
			for(i2=0;i2<nx;i2++) {
				if(ixlive[i2]==1) {
					i20 = i2;
					break;
				}
			}

			n2 = nx - 1;
			for(i2=nx-1;i2>=0;i2--) {

				if(ixlive[i2]==1) {
					n2 = i2;
					break;
				}
			}

			n2 = n2 - i20 + 1;

			o2 = x0 + i20*dx;
                        ugh->o2 = o2;
                        ugh->n2 = n2;
                        ugh->ocdp2 = fcdp + i20*dcdp;
	
			*i2live = i20;
			*n2live = n2;
}

int svUIi(char * dFile){
  char cmd[1024];
  int iRet;
  FILE * fStdOut;

  sprintf (cmd, "testui %s", dFile);
  fStdOut=popen(cmd,"r");
  fscanf(fStdOut,"%d", &iRet);
  pclose(fStdOut);
  return iRet;
} 


void rmfile(char *fname) {
	char cmd[1024];
	bzero(cmd,1024);
  	sprintf (cmd, "/bin/rm -f %s", fname);
	system(cmd);
}
