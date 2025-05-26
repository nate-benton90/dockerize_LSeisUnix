
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
"KMMERGE - Merge results from different-input/same-output KZMIG/KTMIG runs",
" ",
"kmmerge <input [parameters] >output 				", 
" ",
"Required parameters:							",
"input=                 any output of one KZMIG/KTMIG run		",
"output=                merged seismic output data set name 		",
"x1=                    x coordinate of 1st corner of the 3D master grid",
"y1=                    y coordinate of 1st corner of the 3D master grid",
"s1=                    s coordinate of 1st corner of the 3D master grid",
"l1=                    l coordinate of 1st corner of the 3D master grid",
"cdp1=                  cdp number of 1st corner of the 3D master grid	",
"x2=                    x coordinate of 2nd corner of the 3D master grid",
"y2=                    y coordinate of 2nd corner of the 3D master grid",
"s2=                    s coordinate of 2nd corner of the 3D master grid",
"l2=                    l coordinate of 2nd corner of the 3D master grid",
"cdp2=                  cdp number of 2nd corner of the 3D master grid\n",
"x3=                    x coordinate of 3rd corner of the 3D master grid",
"y3=                    y coordinate of 3rd corner of the 3D master grid",
"s3=                    s coordinate of 3rd corner of the 3D master grid",
"l3=                    l coordinate of 3rd corner of the 3D master grid",
"cdp3=                  cdp number of 3rd corner of the 3D master grid	",
"dds=                   cdp spacing in inline direction                 ",
"ddl=                   Line spacing in crossline                       ",
" ",
"diskimg=               Disk name for storing imaged time sections 	",
"diskhdr=               Disk name for storing output trace headers  	",
"diskfld=               Disk name for storing output trace folds  	",
"                       (diskimg,diskhdr,diskfld must be repeated 	",
"                        n times for n inputs)				",
"kzorkt=                kzmig or ktmig merge (0=kzmig 1=ktmig)		",
"Optional parameters:			",
"when kzorkt=0 the following 5 parameters are used			",
"nzo=from_input         number of output samples per trace		",
"fzo=from_input         depth of first sample of output trace		",
"dzo=from_input         depth sampling interval of ouptut trace		",
"ttfile=ttfile          file name of input traveltime tables            ",
"zpow=0.0               power of depth gain to be appled to the output  ",
"                       after migration   ",
"when ktorkt=1 the following 4 parameters are used			",
"ntau=from_input        number of output samples per trace		",
"tau0=from_input        time of first sample of output trace (in s)	",
"dtau=from_input        time sampling interval of ouptut trace (in s)	",
"tpow=-1.0              power of time gain to be appled to the input    ",
"                       before migration (an ungain will be applied after ",
"                       migration)                                      ",
" ",
"flexbin=0              1=Input data to migration have been flexbined	",  
"                         the fold dataset will not be applied to output",
"                       0=sum the folds and apply to the summed image	",
"oofo=0.                output minimum offset (>0.)                     ",
"dofo=999999.           output offset increment                         ",
"nofo=1                 number of offsets			",
"ofo=0.                 output offsets  (specified as 200,250.,350,600,...) ",
"                       if specified, dofo and oofo will be ignored     ",
"sstart=s1              Minimum s (trace position within line) of       ",
"                       migration output                                ",
"                       (NOTE: this is coordinates, i.e., first trace   ",
"                       of a line at s1, 2nd at s1+ds, 3rd at s1+2*ds, ...)",
"ns=(s2-s1)/ds+1        Number of cdp (group) per line of migration output ",
"lstart=l1              Minimum l (line position) of migration output   ",
"                       (NOTE: this is coordinates, i.e., first line    ",
"                       l1, 2nd at l1+dl, 3rd at l1+2*dl, ...)          ",
"nl=(l3-l1)/dl+1        Number of lines (group) of migration output     ",
"ds=dds                 cdp group interval to output                    ",
"dl=ddl                 Line group  interval to output                  ",
"send=                  Ending s position of migration output           ",
"lend=                  Ending l position of migration output           ",
"ntrend=                Number of cdp bins to output from (sstart,lstart) ",
"                       to (send,lend)                                  ",
"                       (specify send, lend and ntrend will output only ",
"                       one slant line from (sstart,lstart) to          ",
"                       (send,lend) of total ntrend cdps; if not        ",
"                       specified, will output to ns by nl cdps)        ",
"nds=1                  Number of cdp locations per output cdp group    ",
"ndl=1                  Number of line locations per output line group  ",
"	",
" ",
"AUTHOR:		Z Li,        12/94   			",
NULL};

segytrace tra, tr;
segybhdr bh;
segychdr ch;


main(int argc, char **argv)
{
	float x1,x2,x3,y1,y2,y3,s1,s2,s3,l1,l2,l3;
	float dds,ddl,ds,dl;
	float send, lend;
	int cdp1, cdp2, cdp3;
	int ntrend,nsout,nlout,ntmp,il,is,flexbin;
	float *s, *l, *migs, *mig, *fold, *folds; 
	float zi, scale;
	string *diskimg, *diskfld, *diskhdr;
	char *ttfile;
	FILE **difp, **dffp, **dhfp, *ttfp, *infp=stdin, *outfp=stdout;
	int ni, i, lpos,izi,iz,it;
	float mx,my,dd,ofs;
	int nofo, ns, nl, nzo;
	int nz, ntau, kzorkt,nz0,ierr,io;
	float fz,fzo,dzo,tau0,dtau,fzt,dzt;
	float ofomin, dofo, sstart,lstart,tmp,fold0; 
	int ndl,nds,ncdppl,nlines,nlread,nsread;
	int cdppds, cdppdds, cdppdl, cdppddl, cdpnum;
        int iix, iiy;
	float *zgain, zpow, tpow;

	float *ofo;
	usghed ugh;
	
	int itmp,itmp1;

    	/* get parameters */
    	initargs(argc,argv);
	requestdoc(1);

	fseek2g(infp,0,1);
        fseek2g(outfp,0,1);

	/* read id headers */
        fgethdr(infp,&ch,&bh);
	fgettr(infp,&tr);

	fputhdr(outfp,&ch,&bh);


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
        if (!getparfloat("ddl",&ddl)) err("Must specify ddl!\n"); 


	ni = countparname("diskimg");
	if(ni<=1) err(" number of inputs %d ust be greater than 1",ni);
	if(countparname("diskfld")!=ni) err("check diskfld");
	if(countparname("diskhdr")!=ni) err("check diskhdr");

	diskimg = (string *) emalloc(ni*sizeof(string)); 
	diskfld = (string *) emalloc(ni*sizeof(string)); 
	diskhdr = (string *) emalloc(ni*sizeof(string)); 
	difp = (FILE **)emalloc(ni*sizeof(FILE *));
	dffp = (FILE **)emalloc(ni*sizeof(FILE *));
	dhfp = (FILE **)emalloc(ni*sizeof(FILE *));

	for(i=0;i<ni;i++) {
		if(!getnparstring(i+1,"diskimg",&diskimg[i]))
                        err("cannot get diskimg file for %i -th input \n",i+1);
		if(!getnparstring(i+1,"diskfld",&diskfld[i]))
                        err("cannot get diskfld file for %i -th input \n",i+1);
		if(!getnparstring(i+1,"diskhdr",&diskhdr[i]))
                        err("cannot get diskhdr file for %i -th input \n",i+1);

		difp[i]=efopen(diskimg[i],"r");
		dffp[i]=efopen(diskfld[i],"r");
		dhfp[i]=efopen(diskhdr[i],"r");
	}

	fprintf(stderr," %d sets of disk files open ... \n", ni);

	if(!getparint("kzorkt",&kzorkt)) err(" kzorkt missing");
	if(kzorkt==0) {
		if(!getparint("nzo",&nzo)) nzo = tr.ns;
		if(!getparfloat("dzo",&dzo)) dzo = tr.dz;
		if(!getparfloat("fzo",&fzo)) fzo = tr.fz;
		if(!getparfloat("zpow",&zpow)) zpow = 0.0;
		if(!getparstring("ttfile",&ttfile))  err(" ttfile missing");
		ttfp = efopen(ttfile,"r");
		fseek2g(ttfp,0,1);
		ierr = fgetusghdr(ttfp,&ugh);
        	if (ierr!=0) err(" Grid parameters of %s required!\n",ttfile);
		fzt = ugh.o1;   dzt = ugh.d1;
		nz0 = (int)((fzo-fzt)/dzt);
		fz = fzt+nz0*dzt;
        	nz = 1+(int)((fzo+(nzo-1)*dzo-fzt)/dzt+0.99)-nz0;

	} else {
		if(!getparint("ntau",&ntau)) ntau = tr.ns;
                if(!getparfloat("dtau",&dtau)) dtau = tr.dt * 0.000001;
                if(!getparfloat("tau0",&tau0)) tau0 = tr.delrt * 0.001;
		if(!getparfloat("tpow",&tpow)) tpow = -1.0;
	}

	if (!getparfloat("oofo",&ofomin)) ofomin = 0.;
        if (!getparfloat("dofo",&dofo)) dofo = 999999.;
        if (!getparint("nofo",&nofo)) nofo = 1;
        itmp = countparname("ofo");
        if(itmp>0 && itmp!=nofo) err("number of ofo not match with nofo");
        ofo = (float*) emalloc(nofo*sizeof(float));
	if(itmp>0) {
                getparfloat("ofo",ofo);
        } else {
                for(io=0;io<nofo;io++) ofo[io] = ofomin + io*dofo;
        }
	
	if (!getparfloat("sstart",&sstart)) sstart = s1;
        if (!getparfloat("lstart",&lstart)) lstart = l1;
        if (!getparfloat("ds",&ds)) ds = dds;
        if (!getparfloat("dl",&dl)) dl = ddl;
        if (!getparint("ndl",&ndl)) ndl = 1; if(ndl<1) ndl=1;
        if (!getparint("nds",&nds)) nds = 1; if(nds<1) nds=1;
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
                fold0 = 10.;
        }
        else {
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
	ntmp = nl*ndl;
        nl = ntmp;
        ntmp = ns*nds;
        ns = ntmp;

        if(ntrend==0) {
                nsout = ns;
                nlout = nl;
        } else {
                nsout = ntrend;
                nlout = 1;
	}

	s = (float*) emalloc(nsout*nlout*sizeof(float));
        l = (float*) emalloc(nsout*nlout*sizeof(float));

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
        }




	if(!getparint("flexbin",&flexbin)) flexbin = 0;

	
	if(kzorkt==1) {
		nzo = ntau;	
		nz = ntau;
		fzo = tau0;
		dzo = dtau;
		zpow = -tpow;
	}

	mig = (float*) emalloc(nzo*sizeof(float));
	migs = (float*) emalloc(nzo*nsout*nofo*sizeof(float));
	fold = (float*) emalloc(nz*sizeof(float));
	folds = (float*) emalloc(nz*nsout*nofo*sizeof(float));
	zgain = (float*) emalloc(nzo*sizeof(float));

	fprintf(stderr," nofo=%d nsout=%d nlout=%d \n",nofo,nsout,nlout);
	fprintf(stderr," start output ... \n");

	for(iz=0;iz<nzo;iz++) {
		tmp = fzo+iz*dzo;
		if(zpow!=0.0) {
			if(tmp!=0.0) {
				zgain[iz] = pow(tmp,zpow);
			} else {
				zgain[iz] = 0.0;
			}
		} else {
			zgain[iz] = 1.0;
		}
	}

	for(il=0;il<nlout;il++) {
		bzero(migs,nzo*nsout*nofo*sizeof(float));
		bzero(folds,nz*nsout*nofo*sizeof(float));
		for(io=0;io<nofo;io++) {
			itmp = io*nsout*nlout + il*nsout;
			for(i=0;i<ni;i++) {
				lpos =  itmp*nzo*sizeof(float);
				efseek(difp[i],lpos,0);
				for(is=0;is<nsout;is++) {
					efread(mig,sizeof(float),nzo,difp[i]);
					itmp1 = (is*nofo+io)*nzo;
					for(iz=0;iz<nzo;iz++) {
						migs[itmp1+iz] += mig[iz];
					}
				}
				lpos =  itmp*nz*sizeof(float);
				efseek(dffp[i],lpos,0);
				for(is=0;is<nsout;is++) {
					efread(fold,sizeof(float),nz,dffp[i]);
					itmp1 = (is*nofo+io)*nz;
					for(iz=0;iz<nz;iz++) {
						folds[itmp1+iz] += fold[iz];
					}
				}
			}
		}
		for(is=0;is<nsout;is++) {
			for(io=0;io<nofo;io++) {
				for(i=0;i<ni;i++) {
					efread(&tr,sizeof(char),240,dhfp[i]);
					if(i==0) {
						bcopy(&tr,&tra,240);
					} else {
						if(tr.ungpow<tra.ungpow) 
							bcopy(&tr,&tra,240);
						if(tr.trid==1 && tra.trid!=1) 
							bcopy(&tr,&tra,240);
					}
				}
				itmp = (is*nofo+io)*nzo;
				itmp1 = (is*nofo+io)*nz;
				if(flexbin==0) {
					if(kzorkt==0) {
                                        	for(iz=0;iz<nz;iz++)
                                                	fold[iz] =
                                                	sqrt(fold0+
							folds[itmp1+iz]);
                                        	for(iz=0;iz<nzo;iz++){
                                                	zi=(fzo+iz*dzo-fz)/dzt;
                                                	izi = zi;
                                                	if(izi>nz-2) izi = nz-2;
                                                	scale = (1.0-zi+izi)*
                                                        	fold[izi]+
                                                        	(zi-izi)*
                                                        	fold[izi+1];
                                                	tra.data[iz] = 
								migs[itmp+iz]/
                                                        	scale;
                                        	}
					} else {
						for(it=0;it<ntau;it++) {
							scale = folds[itmp+it];
							if(scale>1.) {
                                                	tra.data[it] = 
								migs[itmp+it]/
                                                        	scale;
							} else {
                                                	tra.data[it] = 
								migs[itmp+it];
							}
						}
					}
                                } else {
                                        for(iz=0;iz<nzo;iz++)
                                                tra.data[iz] = migs[itmp+iz];
                                }

				for(iz=0;iz<nzo;iz++)
					tra.data[iz] *= zgain[iz];

				/* update header */
				tra.offset = ofo[io];
                                tra.tracl = is + 1;
                                tra.tracr = il + 1;
                                tra.cdpt = io + 1;
				if(kzorkt==0) {
                                	tra.dz = dzo;
                                	tra.fz = fzo;
					tra.ns = nzo;
					tra.mute = 0;
					tra.f1 = fzo;
					tra.d1 = dzo;
				} else {
					tra.delrt = tau0;
                                	tra.ns = ntau;
                                	tra.dt = dtau * 1000000.;
					if(tra.trid!=1) {
                                        tra.d1 = tra.dt;
                                        tmp = (tau0 + (ntau-1)*dtau)*1000;
                                        tra.mute = tmp;
					}
				}
                                ofs = ofo[io];
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
                                }


				if(ntrend>0) {
                                        tmp = (l[il*nsout+is] - l1)/ddl + 0.5;
                                        iiy = tmp;
                                        tmp = (s[il*nsout+is] - s1)/dds + 0.5;
                                        iix = tmp;
                                        if(cdpnum==0) {
                                                tra.cdp = iiy*ncdppl+iix+cdp1;
                                        } else {
                                                tra.cdp = iix*nlines+iiy+cdp1;
                                        }
                                }

                                /* output */
                                fputtr(outfp,&tra);
			}
		}
	}
	
	fprintf(stderr," merge output done \n");

	free(migs);
	free(mig);
	free(folds);
	free(fold);
	
	return 0;
}
