#include "usgrid.h"
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUTZCONV - time-depth conversion 				\n" 
"\n"
"sutzconv vgrid= [parameters] <input-data >output-data 		\n" 
"\n"
"Required parameters:							\n"
"vgrid=                 name of interval or average velocity grid	\n"
"fout=                  time (ms) or depth of first sample to output 	\n"
"dout=                  time (ms) or depth interval to output 		\n"
"nout=                  number of samples per trace to output 		\n"
"\n"
"Optional parameters:							\n"
"tracekey=tracl         segy key word defining trace number within line \n"
"linekey=tracr          segy key word defining line number \n"
"vgtype=1               velocity type (1=interval 0=time-average)		\n"
"torz=0                 0 --- time to depth; input is time data \n"
"                       1 --- depth to time; input is depth data \n"
"vtorz=0                0 --- velocity is in time-domain \n" 
"                       1 --- velocity is in depth-domain \n" 
"ocdp2=from_vgrid       inline trace number of first velocity grid trace \n"
"dcdp2=from_vgrid       inline trace number increment of velocity grid \n"
"oline3=from_vgrid      line number of first velocity grid trace \n"
"dline3=from_vgrid      line number increment of velocity grid \n"
"orient=1               velocity grid orientation 			\n"
"                       1=(time/depth,inline,xline)			\n" 
"                       4=(inline,xline,time/depth)			\n" 
"print=1                print velocity grid location (1=yes 0=no)       \n"
"vgout=                 name of output velocity grid along seismic traces \n"
"                       (output only when specified)	\n"
"n2vgout=               number of samples in the second dimension of \n"
"                       vgout (should be total number of input traces in \n"
"                       2d; number of traces per line in 3D) \n"
"                       default to total number of input traces \n"
"n3vgout=1              number of samples in the third dimension of \n"
"                       vgout (should be 1 for 2D; number of lines for 3D)\n"
"                       number of samples in the first dimension of \n"
"                       vgout is the same as the input vgrid \n"
"\n"
"Note:									\n"
"    1. the fold of the input 3D data must be constant and specified at \n"
"       the binary header (the number of traces per cdp must be the same) \n"
"       if fold=0 in the binary header, 1 is assumed (stack data)	\n"
"    2. ocdp2,dcdp2 in velocity grid must be specified correctly to 	\n"
"       represent the first trace number within line and trace number	\n"
"       increment (inline) of velocity grid				\n"
"    3. oline3,dline3 in velocity grid must be specified correctly to 	\n"
"       represent the first line number and line number	\n"
"       increment of velocity grid				\n"
"    4. bilinear interpolation is used to extra the velocity function 	\n"
"       at input trace location from velocity grid			\n" 
"    5. when vtorz=0, velocity grid is in time with time unit of ms;\n"
"       dt in the trace haeader will be used as time sampling interval of\n"
"       input data							\n"
"    6. when vtorz=1, velocity grid is in depth with depth unit of m\n"
"       or ft; dz (or dt/1000 if dz=0) in the trace haeader will be used \n"
"       as depth sampling interval of input data. fz (or delrt if fz=0) \n"
"       in the trace header will be used as first sample depth		\n"
"    7. vgrid consists of nx by ny traces, where nx is the number of 	\n"
"       inline traces and ny is number of lines				\n" 
"\n"
"AUTHOR:		Zhiming Li,       ,	12/13/94		\n"
;

void bilint_(int *n1,int *nx,int *ny, float *x0,float *y0,
	float *dx,float *dy,float *x,float *y,float *vs,float *v);

void lin1dn_(float *xin,float *yin,int *nin,float *xo,float *yo,
	int *nout,int *indx,float *dydx0,float *dydxn);

main(int argc, char **argv)
{
	segytrace tr, tro;
	segychdr ch;
	segybhdr bh;
	usghed usgh;
	FILE *infp=stdin, *outfp=stdout;
	FILE *vgfp, *vgoutfp;
	char *vgrid, *vgout;
	int n2vgout, n3vgout, ivgout;
	int m2vgout;

	float x0,y0,dx,dy,x,y;
	int nx,ny;
	float fout,dout;
	int nout;
	int n1,torz,vtorz,orient,idisk;
	float o1,d1;
	float tmp;
	int ix, iy, change;
	int ix2, iy2;
	float resx, resy, scale;
	float xpre, ypre;
	long long lpos;
	float tzmax;

	int i1,it,iz,iti,izi,nt,nz;
	float dt,dz,ft,fz,zi,ti;
	float res,dzdt0,dzdtn,dtdz0,dtdzn;
	float *tv, *zv,*vs,*v,*v2,*to,*zo,*vread; 
	int *indx;

	int vgtype;
	int iprint;

	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk, trstart, lnstart;


  	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	if(!getparstring("vgrid",&vgrid)) err("vgrid missing"); 
	if(!getparfloat("fout",&fout)) err("fout missing");
	if(!getparfloat("dout",&dout)) err("dout missing");
	if(!getparint("nout",&nout)) err("nout missing");
	if(!getparint("orient",&orient)) orient = 1;
   	if(!getparint("torz",&torz)) torz=0;
   	if(!getparint("vtorz",&vtorz)) vtorz=0;
   	if(!getparint("vgtype",&vgtype)) vgtype=1;
   	if(!getparint("print",&iprint)) iprint=1;
	if(torz==1 && vgtype!=1) err(" depth to time requires vgtype=1");

	ivgout = 0;
   	if(getparstring("vgout",&vgout)) ivgout = 1;
   	if(!getparint("n2vgout",&n2vgout)) n2vgout = 0;
   	if(!getparint("n3vgout",&n3vgout)) n3vgout = 1;

	vgfp = efopen(vgrid,"r");
	file2g(vgfp);
	if(fgetusghdr(vgfp,&usgh)!=0) err("error fgetusghdr");

	if(ivgout==1) {
		vgoutfp = efopen(vgout,"w");
		file2g(vgoutfp);
	}

	if(!getparfloat("oline3",&y0)) y0 = usgh.oline3; 
	if(y0==0.) err("vghd.oline3 zero");
	if(!getparfloat("dline3",&dy)) dy = usgh.dline3; 
	if(dy==0.) err("vghd.dline3 zero");
	if(!getparfloat("ocdp2",&x0)) x0 = usgh.ocdp2; 
	if(x0==0.) err("vghd.ocdp2 zero");
	if(!getparfloat("dcdp2",&dx)) dx = usgh.dcdp2; 
	if(dx==0.) err("vghd.dcdp2 zero");
	if(orient==1) { 
		ny = usgh.n3; 
		if(ny==0) {
			warn(" usgh.n3 = 0; use 1 instead "); 
			ny = 1;
		}
		nx = usgh.n2; 
		n1 = usgh.n1;
		d1 = usgh.d1;
		o1 = usgh.o1;
	} else {
		ny = usgh.n2; 
		nx = usgh.n1; 
		n1 = usgh.n3;
		if(n1==0) err(" usgh.n1 = 0 ");
		d1 = usgh.d3;
		o1 = usgh.o3;
	}

	getparstring("tracekey",&tracekey);	
	getparstring("linekey",&linekey);
	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);
	
	if(orient!=usgh.orient) 
		warn(" orient in vgrid different: %d \n",usgh.orient);  

	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	fgethdr(infp,&ch,&bh);
	if(bh.fold==0) warn("fold in binary is 0; reset to 1 \n");
	if(bh.fold==0) bh.fold = 1;
	bh.hns = nout;
	bh.hdt = dout*1000.;
    fputhdr(outfp,&ch,&bh);

	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tr))  err("can't get first trace");

	idisk = 0;
	/* if(orient==1 && dx==1. && dy==1. ) idisk = 1; */
	if(orient==1) idisk = 1;
	idisk = 0;
	if(nx*ny*n1>1800000000/4) idisk=1;

	/* memory allocations */
	if(orient==4) vread = (float*) emalloc(nx*ny*sizeof(float)); 
	if(idisk==0) vs = (float*) emalloc(n1*nx*ny*sizeof(float)); 
	v = (float*) emalloc(n1*sizeof(float)); 
	v2 = (float*) emalloc(n1*sizeof(float)); 
	tv = (float *) emalloc(n1*sizeof(float));
	zv = (float *) emalloc(n1*sizeof(float));
	to = (float *) emalloc(nout*sizeof(float));
	zo = (float *) emalloc(nout*sizeof(float));
	indx = (int *) emalloc(nout*sizeof(float));

	if (torz==0) {
		nt = tr.ns; 
		dt = tr.dt*0.001;
		ft = tr.delrt;
		nz = nout;
		dz = dout;
		fz = fout;
		for(it=0;it<n1;it++) tv[it] = o1+it*d1; 
		for(iz=0;iz<nout;iz++) zo[iz] = fout+iz*dout; 
	} else {
		nz = tr.ns; 
		dz = tr.dz;
		fz = tr.fz;
		if(tr.fz==0.) {
			fz = tr.delrt;
			warn(" tr.fz equals 0; reset to tr.delrt=%f \n",fz);
		}
		if(tr.dz==0.) {
			dz = tr.dt*0.001;
			warn(" tr.dz equals 0; reset to tr.dt/1000=%f \n",dz);
		}
		nt = nout;
		dt = dout;
		ft = fout;
		for(iz=0;iz<n1;iz++) zv[iz] = o1+iz*d1;
		for(it=0;it<nout;it++) to[it] = fout+it*dout;
	}


	fprintf(stderr," \n");
	fprintf(stderr," sutzconv parameters \n");
	fprintf(stderr," =================== \n");
	fprintf(stderr," tracekey=%s linekey=%s \n",tracekey,linekey);
	fprintf(stderr," vgrid=%s \n",vgrid);
	fprintf(stderr," torz=%d vtorz=%d \n",torz,vtorz);
	fprintf(stderr," fout=%g dout=%g nout=%d \n",fout,dout,nout);
	fprintf(stderr," orient=%d \n",orient);
	fprintf(stderr," vgrid: ocdp2=%g dcdp2=%g nx=%d \n",
		x0,dx,nx);
	fprintf(stderr," vgrid: oline3=%g dline3=%g ny=%d \n",
		y0,dy,ny);
	fprintf(stderr," vgrid: o1=%g d1=%g nt/nz=%d \n",
		o1,d1,n1);

	fseek2g(vgfp,0,0);
	if(orient==1) {
		if(idisk==0) efread(vs,sizeof(float),n1*nx*ny,vgfp);
	} else {
		for(i1=0;i1<n1;i1++) {
			efread(vread,sizeof(float),nx*ny,vgfp);
			for(it=0;it<nx*ny;it++) 
				vs[i1+it*n1] = vread[it];
		} 
	}

	if(orient==4) free(vread);
	
	if(idisk==0) fprintf(stderr," velocity function read \n");

	gethval(&tr,indxtrk,&trkval);
	ix = vtoi(trktype,trkval);
	gethval(&tr,indxlnk,&lnkval);
	iy = vtoi(lnktype,lnkval);

	xpre = ix - 100;
	ypre = iy - 100;
	change = 0;
	tzmax = 9999999;
	m2vgout = 0;

	/* loop over output traces */
	do {
	/* compute velocity at output location */
	/* via bilinear interpolation */
		gethval(&tr,indxtrk,&trkval);
		ix = vtoi(trktype,trkval);
		gethval(&tr,indxlnk,&lnkval);
		iy = vtoi(lnktype,lnkval);

		x = ix;
		y = iy;
		if(x!=xpre || y!=ypre) {
			change = 1;
			xpre = x;
			ypre = y;
		} else {
			change = 0;
		}
		if(change==1) {
		
		    if(idisk==0) {
				bilint_(&n1,&nx,&ny,&x0,&y0,&dx,&dy,&x,&y,vs,v);
		    } else {
				tmp = (x - x0)/dx + 0.5;
				ix = tmp;
				resx = tmp - ix;
				ix2 = ix + 1;
				if(ix<0) {ix=0; resx=0.;ix2=0;}
				if(ix>=nx-1) {ix=nx-1;resx=0.;ix2=nx-1;}
				tmp = (y - y0)/dy + 0.5;
				iy = tmp;
				resy = tmp - iy;
				iy2 = iy+1;
				if(iy<0) { iy=0; resy=0.;iy2=0;}
				if(iy>=ny-1) {iy=ny-1; resy=0.;iy2=ny-1;}
				if((fabs(resx)<0.1)&&(fabs(resy)<0.1)) {
					lpos = iy*nx+ix;
					lpos = lpos * n1 * sizeof(float);
					fseek2g(vgfp,lpos,0);
					efread(v,sizeof(float),n1,vgfp);
				} else {
					for(i1=0;i1<n1;i1++) v[i1]=0.;

					lpos = iy*nx+ix;
					lpos = lpos * n1 * sizeof(float);
					fseek2g(vgfp,lpos,0);
					efread(v2,sizeof(float),n1,vgfp);
					scale = (1.-resx)*(1.-resy);
					for(i1=0;i1<n1;i1++)
						v[i1]+=scale*v2[i1];

					lpos = iy*nx+ix2;
					lpos = lpos * n1 * sizeof(float);
					fseek2g(vgfp,lpos,0);
					efread(v2,sizeof(float),n1,vgfp);
					scale = resx*(1.-resy);
					for(i1=0;i1<n1;i1++)
						v[i1]+=scale*v2[i1];

					lpos = iy2*nx+ix;
					lpos = lpos * n1 * sizeof(float);
					fseek2g(vgfp,lpos,0);
					efread(v2,sizeof(float),n1,vgfp);
					scale = (1.-resx)*resy;
					for(i1=0;i1<n1;i1++)
						v[i1]+=scale*v2[i1];

					lpos = iy2*nx+ix2;
					lpos = lpos * n1 * sizeof(float);
					fseek2g(vgfp,lpos,0);
					efread(v2,sizeof(float),n1,vgfp);
					scale = resx*resy;
					for(i1=0;i1<n1;i1++)
						v[i1]+=scale*v2[i1];

				}
				if(iprint==1) fprintf(stderr,
				"line=%g trace=%g i3vgrid=%d i2vgrid=%d \n",y,x,iy+1,ix+1);
		    }
		    /* output velocity grid if needed */
		    if(ivgout==1) {
			efwrite(v,sizeof(float),n1,vgoutfp);
			m2vgout = m2vgout + 1;
		    }
		    if(torz==0) {
				if(vgtype==1) {
					if(vtorz==0) {
						zv[0] = o1*v[0]*0.0005;
                       	for(i1=1;i1<n1;i1++)
                       		zv[i1] = zv[i1-1] + (v[i1-1]+v[i1])*d1*0.00025;
					} else if(vtorz==1) {
						for(i1=0;i1<n1;i1++) {
							zv[i1] = o1 + i1*d1;
						}
						tv[0] = o1/v[0]*2000.;
                		for(i1=1;i1<n1;i1++) {
               				tv[i1] = tv[i1-1] + d1/(v[i1-1]+v[i1])*4000.;
						}
					}
				} else {
                	for(i1=0;i1<n1;i1++)
                       	zv[i1] = (o1+i1*d1)*v[i1]*0.0005;
				}
				dzdt0 = (zv[1]-zv[0])/(tv[1]-tv[0]);
				dzdtn = (zv[n1-1]-zv[n1-2])/(tv[n1-1]-tv[n1-2]);
				lin1dn_(zv,tv,&n1,zo,to,&nz,indx,&dzdt0,&dzdtn);
		    } else {
				if(vgtype==1) {
					if(vtorz==1) {
						tv[0] = o1/v[0]*2000.;
                		for(i1=1;i1<n1;i1++)
               				tv[i1] = tv[i1-1] + d1/(v[i1-1]+v[i1])*4000.;
					} else if(vtorz==0) {
                		for(i1=0;i1<n1;i1++)
               				tv[i1] = o1 + d1*i1;
						zv[0] = o1*v[0]*0.0005;
                     	for(i1=1;i1<n1;i1++)
                       		zv[i1] = zv[i1-1] + (v[i1-1]+v[i1])*d1*0.00025;
					}
				} else {
               		for(i1=0;i1<n1;i1++)
               			tv[i1] = o1 + d1*i1;
                	for(i1=0;i1<n1;i1++)
                       	zv[i1] = (o1+i1*d1)*v[i1]*0.0005;
				}
				dtdz0 = (tv[1]-tv[0])/(zv[1]-zv[0]);
				dtdzn = (tv[n1-1]-tv[n1-2])/(zv[n1-1]-zv[n1-2]);
				lin1dn_(tv,zv,&n1,to,zo,&nt,indx,&dtdz0,&dtdzn);
		    }		
		}

		if(torz==0) {
			/* linear interpolate trace */
			for(iz=0;iz<nz;iz++) {
				ti = (to[iz] - ft)/dt;
				iti = ti;
				res = ti - iti;
				if(iti<0) {
					tro.data[iz] = 0.;
				} else if(iti>nt-1) {
					tro.data[iz] = 0.;
					if(tzmax>iz) tzmax = iz;
				} else if(iti==nt-1) {
					tro.data[iz] = tr.data[iti];
				} else {
					tro.data[iz] = tr.data[iti]*(1.-res)+
						res*tr.data[iti+1];
				}
			}
			bcopy(&tr,&tro,240);
			tro.ns = nz;
			tro.dz = dz;
			tro.fz = fz;
			tro.dt = dz*1000.;
			tro.delrt = fz;
		} else {
			/* linear interpolate trace */
			for(it=0;it<nt;it++) {
				zi = (zo[it] - fz)/dz;
				izi = zi;
				res = zi - izi;
				if(izi<0) {
					tro.data[it] = 0.;
				} else if(izi>nz-1) {
					tro.data[it] = 0.;
					if(tzmax>it) tzmax = it;
				} else if(izi==nz-1) {
					tro.data[it] = tr.data[izi];
				} else {
					tro.data[it] = tr.data[izi]*(1.-res)+
						res*tr.data[izi+1];
				}
			}
			bcopy(&tr,&tro,240);
			tro.ns = nt;
			tro.dt = dt*1000.;
			tro.delrt = ft;
			tro.dz = 0.;
			tro.fz = 0.;
		}

		fputtr(outfp,&tro);

	} while(fgettr(infp,&tr));

	if(tzmax == 9999999.) tzmax=(nout-1);
	tzmax = fout + tzmax*dout;
	fprintf(stderr," ==========================\n");
	fprintf(stderr,"   minimum length of output live sample ===%g\n",tzmax);
	fprintf(stderr," ==========================\n");

	free(to);
	free(zo);
	free(tv);
	free(zv);
	free(indx);
	free(vs);
	if(idisk==0) free(v);
	if(ivgout==1) {
		if(n2vgout==0) n2vgout = m2vgout;
		usgh.o2=1;
		usgh.d2=1;
		usgh.n2=n2vgout;
		usgh.o3=1;
		usgh.d3=1;
		usgh.n3=n3vgout;
		if(fputusghdr(vgoutfp,&usgh)!=0) 
			err("error fputusghdr in vgout");
	}

	return 0;

}
