#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUPSTACK - Partial Stack and NMO 			\n"
"\n"
"supstack <stdin vnmogrid= >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"                           (can also be specified as datain=stdin,      \n"
"                           instead of <stdin)				\n"
"stdout                     Name of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"vnmogrid=                  Name of NMO velocity grid 			\n"
"                           (velocity stored as (t,x,y) order with grid \n"
"                           header)					\n" 
"dofo=                      Output offset increment 			\n" 
"                           =0.0, will simply perform NMO;		\n" 
"                           >0.0, will do partial stack;		\n"
"                           <0.0, will simply perform inverse NMO;	\n" 
"Optional Parameters:\n"
"ofomin=0                   Minimum output offset (positive number)	\n"
"                           (ignored when dofo<=0.0)			\n" 
"nofo=60                    Maximum number of output offsets            \n"
"                           (ignored when dofo<=0.0)			\n" 
"binkey=offset              trace header key name to be used in offset	\n"
"                           binning					\n"
"minkey=ofomin              key value of minimum (center) offset bin	\n" 
"dkey=dofo                  key value increment to bin output offset	\n"
"                           if offset is used to bin output, do not 	\n"
"                           specify binkey, minkey and dkey, unless	\n"
"                           signed offset binning is desired		\n" 
"tracekey=tracl             segy key word defining trace number inline \n"
"linekey=tracr              segy key word defining line number \n"
"ocdp2=from_vnmogrid        inline trace number of first velocity grid trace \n"
"dcdp2=from_vnmogrid        inline trace number increment of velocity grid \n"
"oline3=from_vnmogrid       line number of first velocity grid trace \n"
"dline3=from_vnmogrid       line number increment of velocity grid \n"
"or \n" 
"cdp1=                      global cdp number at first trace of velocity grid\n"
"dcdpx=1                    cdp number increment of velocity traces in   \n"
"                           the inline (2nd axis) of velocity grid  \n"
"ncdppl=                    number of cdp per line in the 3D seismic grid \n"
"dline=1                    line number increment of velocity traces in \n"
"                           the crossline (3rd axis) of velocity grid		\n"
"                           when cdp1 is specified, ocdp2, dcdp2, oline3 \n"
"                           dline3, tracekey and linekey will be ignored. \n"
"                           The global cdp number in the input trace header \n"
"                           will be used, together with cdp1, dcdpx, ncdppl \n"
"                           and dline, to compute the velocity location \n"
"ntvgrid=from-vnmogrid      Number of time samples per trace in vnmogrid \n"
"dtvgrid=from-vnmogrid      Time interval (in ms) of vnmogrid \n"
"ftvgrid=from-vnmogrid      Minimum Time (in ms) of vnmogrid \n"
"smute=1.5                  Samples with NMO stretch exceeding smute \n"
"                           are zeroed\n"
"                           if smute > 9999.0, no stretch mute limit \n"
"lmute=25                   Length (in samples) of linear ramp for \n"
"                           stretch mute\n"
"sscale=1                   =1 to divide output samples by NMO stretch factor\n"
"                           =0 no					\n"
"intype=0                   nmo/inmo interpolation type (0=linear 1=sinc) \n"
"iratio=10                  nmo/inmo mapping time interpolation/computation \n"
"                           ratio (must be >= 1)			\n"
"print=1                    print velocity grid location when input location\n"
"                           changes (1=yes 0=no) \n"
"retain=0                   when dofo>0 (partial stack), any trace with \n"
"                           offset outside of the output offset bin range \n"
"                           will be ignored if retain=0; it will stacked \n"
"                           onto the nearest offset bin when retain=1 \n"
"\n"
"Author:	Zhiming Li		      		6-19-92		\n"
"Notes:									\n"
"  1. Constant velocity function extrapolation applied outside cdp range of \n"
"     vnmogrid								\n" 
"  2. Only positive offset output allowed. Reciprocity will be applied to \n"
"     negative offsets and stack them to positive offsets		\n"
"  3. When vnmogrid is a standard grid file (includes header), 	\n"	
"     ntvgrid, dtvgrid and ftvgrid will default to the values in the	\n"	
"     grid header. When vnmogrid is not a standard grid file, these three  \n"
"     parameters will default to					  \n"
"         ntvgrid   = number of samples per input trace			  \n"
"         dtvgrid   = sampling rate of input trace			  \n" 
"         ftvgrid   = first sampling time of input trace		  \n" 
"  4. When input vgrid size < 1GB, 3D velocity will be read in the program \n"
"     at once and a bilinear interpolation will be used to compute the \n"
"     velocity function \n"
"  5. When input vgrid size >= 1GB, velocity will be read in at the nearest \n"
"     trace and line (or global cdp) location of the grid. No interpolation  \n"
"     is used. \n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

void pstkout(float *gather, char *headers, int nofo, int *fold, int nt,
		float ofomin, float dofo, FILE *outfp);   

void readovv(FILE *vgfp, int ntvgrid, int nx, int ny, int ix, int iy,
	float dtvgrid, float ftvgrid, int nt, float dt, float ft, float *ovvt,
	int idisk, float *vs, float x0, float y0, float dx, float dy, float x, float y);

void finds(float *t, float *s, int nt, float *tnew, float *snew, int ntnew);


main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	float dt;	/* time sampling interval */
	float ft;	/* time of first sample */
	int it;		/* time sample index */
	float *ovvt;	/* array[nt] of sloth for a particular trace */
	float smute;	/* zero samples with NMO stretch exceeding smute */
	float osmute;	/* 1/smute */
	int lmute;	/* length in samples of linear ramp for mute */
	int itmute;	/* zero samples with indices less than itmute */
	int sscale;	/* if non-zero, apply NMO stretch scaling */
	int oldoffset;	/* offset of previous trace */
	int oldcdp;	/* cdp of previous trace */
	int newcdp;	/* cdp of current trace */
	int newsloth;	/* if non-zero, new sloth function was computed */
	float tn;	/* NMO time (time after NMO correction) */
	float *qtn;	/* NMO-corrected trace q(tn) */
	float *ttn;	/* time t(tn) for NMO */
	float *atn;	/* amplitude a(tn) for NMO */
	float *tnt;	/* time tn(t) for inverse NMO */
	float *aovv;	/* temporary used to interpolate ovv array */
	float temp, tmp, temp2;/* temporary float */
	float mutime;
	int imut;
	FILE *vgfp;
	int *fold, iof, ntvgrid,  nofo;
	float *gather, ofomin, dofo, dtvgrid, ftvgrid, odtt2, ofo, ofo2;
	char *headers, *vgfile, *datain, *dataout;
	int inmo=0;
	FILE *infp,*outfp;
	int cdp1, mcdp, ocdp, ncdppl;
	float dcdpx, dline;
	int intype, iratio, ntc;
	float *ttnc, *tntc, dtc, ftc, fratio, tmin, tmax;
	float *vs;
	int idisk=1;
	float minkey, dkey, keyvalue;
	String binkey, typekey;
	Value valkey;
	int key;

    String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;
	float fx,dx,fy,dy,x,y;
	int i2,i3,ix,iy;
	int iprint, retain;

	int n1,n2,n3,n4,n5;
    float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
    float scale, ocdp2, oline3, dcdp2, dline3;
	float vmin, vmax;
    int dtype,ierr,orient,gtype;
	ghed gh;

	long long ilonglong;
	off_t lofset;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("vnmogrid",&vgfile)) 
		err(" File vnmogrid must be specified ");
	if((vgfp = fopen(vgfile,"r"))==NULL)
                err("Input vnmogrid file %s not found \n",vgfile);
	/* obtain grid header info */
	file2g(vgfp);
	ierr = fgetghdr(vgfp, &gh);
	if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                         &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                         &dcdp2,&dline3,&ocdp2,&oline3,
						 &vmin,&vmax,&orient,&gtype);

	if(n2==0) n2 = 1; 
	if(n3==0) n3 = 1;
	if(n1*n2*n3*sizeof(float)<1000000000) {
		idisk = 0;
	}

	if (!getparfloat("dofo",&dofo)) 
		err(" Parameter dofo must be specified ");
	/*
		1        nmo
	inmo=   0	 partial nmo
	       -1	 inverse nmo
	*/
	
	if(dofo==0.0) {
		inmo = 1;
	} else if(dofo<0.) {
		inmo = -1;
	} else {
		inmo = 0;
	}

	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	} 
	file2g(infp);

	if (!getparstring("dataout",&dataout)) {
		outfp = stdout;
	} else {
		outfp = efopen(dataout,"w");
	} 
	file2g(outfp);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000000.0;
	ft = tr.delrt/1000.0;

	if(tr.delrt!=0)  err("trace start time not zero ");
	
	/* get other optional parameters */
	if (!getparfloat("ofomin",&ofomin)) ofomin = 0.0;
	if (!getparint("nofo",&nofo)) nofo = 60; 
	if(inmo!=0) nofo = 1;
	if (getparint("cdp1",&cdp1)) {
		if(cdp1==0) err("cdp1 can not be zero");
		if (!getparfloat("dcdpx",&dcdpx)) dcdpx = 1;
		if (!getparfloat("dline",&dline)) dline = 1;
		if (!getparint("ncdppl",&ncdppl)) err(" must specify ncdppl ");
		if(n3==0) n3 = 1;
	} else {
		cdp1 = 0;
	    getparstring("tracekey",&tracekey);
	    getparstring("linekey",&linekey);
		trktype = hdtype(tracekey);
		lnktype = hdtype(linekey);
		indxtrk = getindex(tracekey);
		indxlnk = getindex(linekey);
	    if(!getparfloat("ocdp2",&fx) && ierr==0) fx = ocdp2;
	    if(!getparfloat("dcdp2",&dx) && ierr==0) dx = dcdp2;
	    if(!getparfloat("oline3",&fy) && ierr==0) fy = oline3;
	    if(!getparfloat("dline3",&dy) && ierr==0) dy = dline3;
	}
  	if(!getparint("print",&iprint)) iprint = 1;
  	if(!getparint("retain",&retain)) retain = 0;

	if (!getparint("ntvgrid",&ntvgrid)) {
		if(ierr==0) {
			ntvgrid = n1; 
		} else {
			ntvgrid = nt; 
		}
	}
	if (!getparfloat("dtvgrid",&dtvgrid)) {
		if(ierr==0) {
			dtvgrid = d1 * 0.001;
		} else {
			dtvgrid = dt;
		}
	} else {
		dtvgrid = dtvgrid/1000.0;
	}
	if (!getparfloat("ftvgrid",&ftvgrid)) {
		if(ierr==0) {
			ftvgrid = o1 * 0.001;
		} else {
			ftvgrid = ft;
		}
	} else {
		ftvgrid = ftvgrid/1000.0;
	}

	if (!getparfloat("smute",&smute)) smute = 1.5;
	if (smute<=0.0) err("smute must be greater than 0.0");
	if (!getparint("lmute",&lmute)) lmute = 25;
	if (!getparint("sscale",&sscale)) sscale = 1;
	if (!getparint("intype",&intype)) intype = 0;
	if (!getparint("iratio",&iratio)) iratio = 10;
	if(iratio<1) err("iratio must not be less then 1"); 

	/* key for offset bin */
	if (!getparstring("binkey", &binkey)) {
		binkey = "offset"; 
		minkey = ofomin; 
		dkey = dofo; 
		key = 0;
	} else {
		key = 1;
		if(!getparfloat("minkey", &minkey)) minkey = ofomin; 
		if(!getparfloat("dkey", &dkey)) dkey = dofo; 
	} 
	typekey = hdtype(binkey);

	dtc = dt * iratio; 
	ftc = ft/dt;
	if(iratio>1) {
		ntc = nt/iratio + 1; 
	} else {
		ntc = nt;
	}
	fratio = iratio;
	tmin = -1.0;
	tmax = nt + 1.;

	/* allocate workspace */
	ovvt = (float*) emalloc(nt*sizeof(float));
	ttn = (float*) emalloc(nt*sizeof(float));
	atn = (float*) emalloc(nt*sizeof(float));
	qtn = (float*) emalloc(nt*sizeof(float));
	tnt = (float*) emalloc(nt*sizeof(float));
	aovv = (float*) emalloc(nt*sizeof(float));
	gather = (float*) emalloc(nofo*nt*sizeof(float));
	headers = (char*) emalloc(nofo*HDRBYTES);
	fold = (int*) emalloc(nofo*sizeof(int));
	if( idisk==0 ) {
		vs = (float*) emalloc(ntvgrid*n2*n3*sizeof(float)); 
		ilonglong = 0;
		bcopy(&ilonglong,&lofset,8);
		fseek2g(vgfp,lofset,0);	
		fread(vs,sizeof(float),ntvgrid*n2*n3,vgfp);
		fprintf(stderr," velocity file read into the program \n"); 
	} else {
		vs = (float*) emalloc(ntvgrid*sizeof(float)); 
	}

	ttnc = (float*) emalloc(nt*sizeof(float));
	tntc = (float*) emalloc(nt*sizeof(float));

	odtt2 = 1./dt/dt;
	bzero(fold,nofo*sizeof(int));
	bzero(gather,nofo*nt*sizeof(float));
	for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio) tntc[it] = tn;
	for (it=0,tn=ftc; it<nt; ++it,tn+=1.0) tnt[it] = tn;
	osmute = 1.0/smute;

	/* compute new sloth function ovv(t) at first trace */
	if(cdp1==0) {
		gethval(&tr,indxtrk,&trkval);
		ix = vtoi(trktype,trkval);
		gethval(&tr,indxlnk,&lnkval);
		iy = vtoi(lnktype,lnkval);
		x = (ix - fx)/dx + 0.5;
		i2 = x;
		y = (iy - fy)/dy + 0.5;
		i3 = y;
		x = ix;
		y = iy;
	} else {
		y = (tr.cdp - cdp1)/ncdppl;
		i3 = y;
		x = ( (tr.cdp - cdp1) - i3*ncdppl )/dcdpx + 0.5;
		i2 = x;
		y = y / dline;
		i3 = y;
		x = (tr.cdp - cdp1) - i3*ncdppl;
		fx = 0.;
		dx = dcdpx;
		y = (tr.cdp - cdp1)/ncdppl;
		fy = 0.;
		dy = dline;
	}
	/* set old cdp and old offset for first trace */
	if(cdp1!=0) {
		oldcdp = tr.cdp;
		ocdp = tr.cdp;
	} else {
		oldcdp = iy*10000+ix;
		ocdp = oldcdp;
	}
	oldoffset = tr.offset - 1;
	mcdp = 1;

	if(i2<0) i2=0; 
	if(i2>n2-1) i2=n2-1;
	if(i3<0) i3=0; 
	if(i3>n3-1) i3=n3-1;
	if(iprint==1) {
		if(cdp1>0) {
			fprintf(stderr," vgrid location i2=%d i3=%d for cdp=%d \n", 
				i2+1,i3+1,tr.cdp);
		} else {
		  fprintf(stderr," vgrid location i2=%d i3=%d for trace=%d line=%d\n", 
				i2+1,i3+1,ix,iy);
		}
	}
	readovv(vgfp,ntvgrid,n2,n3,i2,i3,dtvgrid,ftvgrid,ntc,dtc,ft,ovvt,
			idisk,vs,fx,fy,dx,dy,x,y);
	newsloth = 1;



	/* loop over traces */
	do {

		if(cdp1==0) {
			gethval(&tr,indxtrk,&trkval);
			ix = vtoi(trktype,trkval);
			gethval(&tr,indxlnk,&lnkval);
			iy = vtoi(lnktype,lnkval);
			x = (ix - fx)/dx + 0.5;
			i2 = x;
			y = (iy - fy)/dy + 0.5;
			i3 = y;
			newcdp = iy*10000+ix;
			x = ix;
			y = iy;
		} else {
			y = (tr.cdp - cdp1)/ncdppl;
			i3 = y;
			x = ( (tr.cdp - cdp1) - i3*ncdppl )/dcdpx + 0.5;
			i2 = x;
			newcdp = tr.cdp;
			y = y / dline;
			i3 = y;
			x = (tr.cdp - cdp1) - i3*ncdppl;
			fx = 0.;
			dx = dcdpx;
			y = (tr.cdp - cdp1)/ncdppl;
			fy = 0.;
			dy = dline;
		}

		if ( newcdp!=oldcdp ) {

			/* output partial stacked gather */
			if(inmo==0)
			pstkout(gather,headers,nofo,fold,nt,ofomin,dofo,outfp);
			bzero(fold,nofo*sizeof(int));
			bzero(gather,nofo*nt*sizeof(float));

			/* compute new sloth function ovv(t) */
			if(i2<0) i2=0; 
			if(i2>n2-1) i2=n2-1;
			if(i3<0) i3=0; 
			if(i3>n3-1) i3=n3-1;
			if(iprint==1) {
				if(cdp1>0) {
					fprintf(stderr," vgrid location i2=%d i3=%d for cdp=%d \n", 
						i2+1,i3+1,tr.cdp);
				} else {
		  fprintf(stderr," vgrid location i2=%d i3=%d for trace=%d line=%d\n", 
						i2+1,i3+1,ix,iy);
				}
			}
			readovv(vgfp,ntvgrid,n2,n3,i2,i3,dtvgrid,ftvgrid,ntc,dtc,ft,ovvt,
					idisk,vs,fx,fy,dx,dy,x,y);
			newsloth = 1;
			mcdp = mcdp + 1;
		} else {
			newsloth = 0;
		}

		if(inmo==0) {
			if(key==1) {
				gethdval(&tr, binkey, &valkey);
				keyvalue = vtof(typekey,valkey);
				ofo = (keyvalue - minkey)/dkey + 0.5;
				iof = ofo;
			} else {
				ofo = tr.offset;
				ofo = (fabs(ofo) - ofomin)/dofo + 0.5;
				iof = ofo;
			}
		} else { 
			iof = 0;
		}

/*
fprintf(stderr,"keyvalue=%g iof=%d \n",keyvalue,iof);
*/
		
		if(retain==1) {
			if(iof<0) iof = 0;
			if(iof>nofo-1) iof = nofo - 1;
		}
		if(iof<0 || iof > nofo-1 ) continue;

		/* if sloth function or offset has changed */
		if (newsloth || tr.offset!=oldoffset) {
		
			if(inmo==0) {
				ofo = ofomin + iof * dofo;
				ofo2 = ofo*ofo; 
				temp = (tr.offset*tr.offset-ofo2)*odtt2;
				temp2 = ofo2 * odtt2;
			} else {
				temp = (tr.offset*tr.offset)*odtt2;
			}

			/* compute time t(tn) (normalized) */
			/* nmo */
			if(inmo==1) {
				for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio)
					ttnc[it] = sqrt(tn*tn+temp*ovvt[it]);
			/* inverse nmo */
			} else if(inmo==-1) {
				/* compute inverse nmo time */
				for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio)
					ttnc[it] = sqrt(tn*tn+temp*ovvt[it]);

				/* interpolate sloth */
				finds(ttnc,ovvt,ntc,tntc,aovv,ntc);

				for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio) {
					tmp=tn*tn - temp*aovv[it];
					if(tmp>0.) {
						ttnc[it] = sqrt(tmp);
					} else {
						ttnc[it] = nt*3;
					}
				}
			/* partial nmo */
			} else {
				/* compute inverse nmo time */
				for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio)
					ttnc[it] = sqrt(tn*tn+temp2*ovvt[it]);
				/* interpolate sloth */
				finds(ttnc,ovvt,ntc,tntc,aovv,ntc);

				for (it=0,tn=ftc; it<ntc; ++it,tn+=fratio) {
					tmp=temp*aovv[it]+tn*tn;
					if(tmp>0.) {
						ttnc[it] = sqrt(tmp);
					} else {
						ttnc[it] = nt*3;
					}
				}
			}
			
			/* linear interpolate ttnc to ttn */
			if(iratio==1) {
				bcopy((char*)ttnc,(char*)ttn,nt*sizeof(float));
			} else {
				intsln(ntc,fratio,ftc,ttnc,
					tmin,tmax,nt,tnt,ttn);
			}
			
			/* compute inverse of stretch factor a(tn) */
			if(ttn[0]<nt && ttn[1]<nt) {
				atn[0] = ttn[1]-ttn[0];
			} else {
				atn[0] = 0.;
			}
			for (it=1; it<nt-1; ++it) {
				if(ttn[it]<nt && ttn[it]<nt) {
					atn[it] = ttn[it]-ttn[it-1];
				} else {
					atn[it] = 0.; 
				}
			}
			if(ttn[nt-1]<nt && ttn[nt-2]<nt) {
				atn[nt-1] = ttn[nt-1]-ttn[nt-2];
			} else {
				atn[nt-1] = 0.;
			}

			for(it=1;it<nt;it++) {
				if(atn[it]<0.) {
					atn[it-1] = 0.;
					ttn[it-1] = nt + 1;
				}
			}

			if(atn[1]==0) {
				atn[0] = 0.;
				ttn[0] = 0.;	
			}
			for(it=0;it<nt;it++) {
				if(atn[it]>smute*2) {
					atn[it] = 0.;
					ttn[it] = nt+1;
				}
			}
			/* determine index of first sample to survive mute */
			for (it=0,itmute=0; it<nt && atn[it]<osmute; ++it) 
				itmute++;

		}

		
		/* intype=0 do partial nmo via linear interpolation */
		/* intype=1 do partial nmo via 8-point sinc interpolation */

		if(intype==0) {
			intsln(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&ttn[itmute],&qtn[itmute]);
		} else {
			ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
				nt-itmute,&ttn[itmute],&qtn[itmute]);
		}
			
		/* apply mute */
		for (it=0; it<itmute; ++it)
			qtn[it] = 0.0;

		/* update mute time */
		mutime = (tr.mute*0.001-ft)/dt;
		imut = mutime;

		if(imut>0) {
			for(it=itmute;it<nt;it++) {
				if(imut<ttn[it]) {
					mutime = it;
					break;
				}
			}
		}
		imut = mutime;


		if(itmute > imut) imut = itmute;
		mutime = (imut*dt+ft)*1000.;
		tr.mute = (int) mutime;

		/* apply linear ramp */
		for (it=itmute; it<itmute+lmute && it<nt; ++it)
			qtn[it] *= (float)(it-itmute+1)/(float)lmute;

			
		/* if specified, scale by the NMO stretch factor */
		if (sscale)
			for (it=itmute; it<nt; ++it)
				qtn[it] *= atn[it];
			
		/* stack the partial NMO corrected trace to output gather */
		if(inmo==0) {
			for(it=0;it<nt;it++) 
				gather[it+iof*nt] += qtn[it]; 
			fold[iof] += 1;
			bcopy((char*)&tr,headers+iof*HDRBYTES,HDRBYTES);

		} else {
			for(it=0;it<nt;it++) tr.data[it] = qtn[it]; 
			fputtr(outfp,&tr);
		}

		/* remember offset and cdp */
		oldoffset = tr.offset;
		oldcdp = newcdp;

	} while (fgettr(infp,&tr));

	/* output last gather */

	if(inmo==0) pstkout(gather,headers,nofo,fold,nt,ofomin,dofo,outfp);
	
	if(cdp1!=0) {
		fprintf(stderr,
		" Total %d cdp gathers starting at cdp=%d processed \n",mcdp,ocdp); 
	} else {
		fprintf(stderr,
		" Total %d gathers starting at cdplbl=%d processed \n",mcdp,ocdp); 
	}

	free(gather);
	free(headers);
	free(fold);

	return EXIT_SUCCESS;
}

/* interpolate sloth */ 
void finds(float *t, float *s, int nt, float *tnew, float *snew, int ntnew){

	float *tsort, *ssort;	
	int *indx, it, itnew;
	float tmp, dt;

	tsort = emalloc(nt*sizeof(float));
	ssort = emalloc(nt*sizeof(float));
	indx = emalloc(nt*sizeof(int));
	for(it=0;it<nt;it++) indx[it] = it;
	qkisort(nt,t,indx);
	for(it=0;it<nt;it++) {
		tsort[it] = t[indx[it]];
		ssort[it] = s[indx[it]];
	}	

	for(itnew=0;itnew<ntnew;itnew++) {
		tmp = tnew[itnew];
		if(tmp<=tsort[0]) {
			snew[itnew] = ssort[0];
		} else if(tmp>=tsort[nt-1]) {
			snew[itnew] = ssort[nt-1];
		} else {
			for(it=0;it<nt-1;it++) {
				if(tmp>=tsort[it] && tmp<=tsort[it+1]) {
					dt = tsort[it+1]-tsort[it]; 
					if(dt!=0.) {
						snew[itnew] = ssort[it] + 
							(tmp-tsort[it])*
							(ssort[it+1]-ssort[it])
							/dt;
					} else {
						snew[itnew] = ssort[it]; 
					}
					break;
				}
			}
		}
	}

	free(tsort);
	free(ssort);
	free(indx);
}



/* output partial stacked gather  */
void pstkout(float *gather, char *headers, int nofo, int *fold, int nt,
		float ofomin, float dofo, FILE *outfp) {  

	segytrace tro;
	int it, iof, mx, my;
	float scale, dd, ofo;

	for(iof=0;iof<nofo;iof++) {
		if(fold[iof]>=1) {
			scale=1./fold[iof];
			for(it=0;it<nt;it++) 
				tro.data[it] = gather[it+iof*nt]*scale;
			bcopy(headers+iof*HDRBYTES,(char*)&tro, HDRBYTES);
			mx = (tro.gx+tro.sx)/2;
			my = (tro.gy+tro.sy)/2;
			dd = tro.offset;
			dd = fabs(dd);
			ofo = ofomin + iof*dofo;
			if(dd>0.) {
				tro.sx = mx+ofo*(tro.sx-mx)/dd; 
				tro.gx = mx+ofo*(tro.gx-mx)/dd; 
				tro.sy = my+ofo*(tro.sy-my)/dd; 
				tro.gy = my+ofo*(tro.gy-my)/dd; 
			} else {
				tro.sx = mx-ofo/2; 
				tro.gx = mx-ofo/2; 
				tro.sy = my; 
				tro.gy = my; 
			}
			tro.offset = ofo;
			fputtr(outfp,&tro);
		}
	}
}


/* read and interpolate sloth function from an Vnmo grid file */

void readovv(FILE *vgfp, int ntvgrid, int nx, int ny, int ix, int iy,
	float dtvgrid, float ftvgrid, int nt, float dt, float ft, float *ovvt,
	int idisk, float *vs, float x0, float y0, float dx, float dy, float x, float y) {
	
	float *vread, ratio, t, ftr;
	int icdp, it, itread;
	float tmp;
	long long lpos;
	off_t lofset;

	icdp = ix + iy*nx;

	lpos = icdp;
	lpos = lpos*ntvgrid*sizeof(float);

	vread = (float*) emalloc(ntvgrid*sizeof(float));

	if(idisk==1) {
		bcopy(&lpos,&lofset,8);
		fseek2g(vgfp,lofset,0);	
		fread(vread,sizeof(float),ntvgrid,vgfp);
	} else {
		bilint_(&ntvgrid,&nx,&ny,&x0,&y0,&dx,&dy,&x,&y,vs,vread);
	}


	ratio = dt/dtvgrid;
	ftr = (ft - ftvgrid)/dtvgrid;

	for(it=0;it<nt;it++) {
		t = it*ratio + ftr;
		itread = t;
		if(itread < 0) {
			tmp = vread[0];
		} else if(itread >= ntvgrid-1) {
			tmp = vread[ntvgrid-1];
		} else {
			tmp = vread[itread] + (t-itread)*
				(vread[itread+1]-vread[itread]);
		}
		ovvt[it] = 1./(tmp*tmp);
	}

	free(vread);
}

