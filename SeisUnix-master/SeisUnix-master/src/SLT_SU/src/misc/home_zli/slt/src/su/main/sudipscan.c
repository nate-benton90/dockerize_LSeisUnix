#include "usgrid.h"
#include "usu.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUDIPSCAN - dip scan of 3D data 	\n"
"\n"
"sudipscan datain=  [parameters]\n"
"\n"
"Required Parameters:\n"
"datain=                    name of input 3D data (nx by ny traces)	\n"
"nx=                        number of cdp per line in the input data	\n"
"ny=                        number of lines in the input data		\n"
"pxfile=                    name of the output x-dip (ms/cdp) grid file \n"
"pyfile=                    name of the output y-dip (ms/line) grid file  \n"
"amfile=                    name of the amplitude grid file    \n"
"Optional Parameters:\n"
"px0=-10.                   starting x-dip to scan (in ms/cdp)		\n"
"dpx=1.                     x-dip increment to scan (in ms/cdp)		\n"
"npx=21                     number of x-dips to scan \n"
"py0=-5.                    starting y-dip to scan (in ms/line)		\n"
"dpy=1.                     y-dip increment to scan (in ms/line)		\n"
"npy=11                     number of y-dips to scan \n"
"lt=11                      length of time (samples) to compute semblance	\n"
"ipow=2                     power to be applied to the data			\n"
"lx=11                      number of cdps used to scan dip	  		\n"
"ly=5                       number of lines used to scan dip 		\n"
"ito=1                      starting output sample index			\n"
"dto=1                      output sample increment (in samples)	\n"
"nto=nt                     number of samples  per trace to output	\n"
"                           (default to number of samples/trace in input)\n"
"ixo=1                      starting output cdp position			\n"
"dxo=1                      output cdp increment					\n"
"nxo=nx                     number of cdps per line to output		\n"
"iyo=1                      starting output line position			\n"
"dyx=1                      output line increment					\n"
"nyo=ny                     number of lines to output		\n"
"perc=50                    amplitude percentile used to detect signal \n"
"\n"
"Author:	Zhiming Li		      		10-30-95		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

main(int argc, char **argv)
{
	int nt,it;
	float dt;
	FILE *amfp, *pxfp, *pyfp;
	char *datain,*amfile, *pxfile, *pyfile;
	FILE *infp;
	int iy, ix;
	int ipx, ipy;
	float px0,py0,dpx,dpy;
	int ipow;
	int npx,npy,nx,ny,lt,lx,ly;
	int nxo,nyo,ixo,iyo,dxo,dyo,ito,dto,nto;
	int ii1, ii2, iyy, ntxo;
	float pxmin, pxmax, pymin, pymax, ammin, ammax;
	float tmp, tmp1, tmp2;
	int nsegy;
	float *data, *amp, *pxo, *pyo, *amo, *work, *wk1, *wk2, perc ;
	int *ipxo, *ipyo;
	float *px, *py, *to;
	int hy, i2, i22, iy0, ierr, ht, hx;
	float *tt, *tx, *ty;

	long ltmp;

	usghed usgh;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

/* open input data */
	if (!getparstring("datain",&datain)) err(" must specify datain ");
	if((infp = fopen(datain,"r"))==NULL)
             	err("datain %s open failed \n",datain);
	file2g(infp);
	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt * 0.001;

	/* get required parameters */
	if (!getparstring("amfile",&amfile)) err(" must specify amfile ");
	if (!getparstring("pxfile",&pxfile)) err(" must specify pxfile ");
	if (!getparstring("pyfile",&pyfile)) err(" must specify pyfile ");

	if (!getparint("nx",&nx)) err(" must specify nx");
	if (!getparint("ny",&ny)) err(" must specify ny");

	/* get optional parameters */
	if (!getparfloat("px0",&px0)) px0 = -10.;
	if (!getparfloat("dpx",&dpx)) dpx = 1.;
	if (!getparint("npx",&npx)) npx = 21;
	if (!getparfloat("py0",&py0)) py0 = -5.;
	if (!getparfloat("dpy",&dpy)) dpy = 1.;
	if (!getparint("npy",&npy)) npy = 11;
	if (!getparint("lt",&lt)) lt = 11;
	if (!getparint("ipow",&ipow)) ipow = 2;
	if (!getparint("lx",&lx)) lx = 11;
	if (!getparint("ly",&ly)) ly = 5;
	if(ny<ly) ly = (ny-1)/2*2+1;

	if (!getparint("ixo",&ixo)) ixo = 1;
	if (!getparint("dxo",&dxo)) dxo = 1;
	if (!getparint("nxo",&nxo)) nxo = nx;
	if (!getparint("iyo",&iyo)) iyo = 1;
	if (!getparint("dyo",&dyo)) dyo = 1;
	if (!getparint("nyo",&nyo)) nyo = ny;
	if (!getparint("ito",&ito)) ito = 1;
	if (!getparint("dto",&dto)) dto = 1;
	if (!getparint("nto",&nto)) nto = nt;
	if (!getparfloat("perc",&perc)) perc = 50;
	perc = perc * 0.01;


	if((pxfp = fopen(pxfile,"w+"))==NULL)
             	err("pxfile %s open failed \n",pxfile);
	if((pyfp = fopen(pyfile,"w+"))==NULL)
             	err("pyfile %s open failed \n",pyfile);
	if((amfp = fopen(amfile,"w+"))==NULL)
             	err("amfile %s open failed \n",amfile);

	ntxo = nto * nxo;
	pxo = (float*) emalloc(ntxo*sizeof(float));
	pyo = (float*) emalloc(ntxo*sizeof(float));
	amo = (float*) emalloc(ntxo*sizeof(float));
	to = (float*) emalloc(nto*sizeof(float));
	px = (float*) emalloc(npx*sizeof(float));
	py = (float*) emalloc(npy*sizeof(float));

	for(it=0;it<nto;it++) to[it] = ito + it*dto;
	for(ipx=0;ipx<npx;ipx++) px[ipx] = (px0+ipx*dpx)/dt;
	for(ipy=0;ipy<npy;ipy++) py[ipy] = (py0+ipy*dpy)/dt;

	for(it=0;it<ntxo;it++) {
		pxo[it] = 0.;
		pyo[it] = 0.;
		amo[it] = 0.;
	}

	file2g(pxfp);
	file2g(pyfp);
	file2g(amfp);

	for(iy=0;iy<nyo;iy++) {
		efwrite(pxo,sizeof(float),ntxo,pxfp);
		efwrite(pyo,sizeof(float),ntxo,pyfp);
		efwrite(amo,sizeof(float),ntxo,amfp);
	}

	fseek2g(pxfp,0,0);
	fseek2g(pyfp,0,0);
	fseek2g(amfp,0,0);

	data = (float*) emalloc(nx*ly*nt*sizeof(float));
	amp = (float*) emalloc(nto*npx*npy*sizeof(float));
	work = (float*) emalloc(nt*sizeof(float));


	hy = (ly-1)/2;
	hx = (lx-1)/2;
	ht = (lt-1)/2;
	tt = (float*) emalloc(lt*sizeof(float));
	tx = (float*) emalloc(lx*sizeof(float));
	ty = (float*) emalloc(ly*sizeof(float));

	ipxo = (int*) emalloc(nto*sizeof(int));
	ipyo = (int*) emalloc(nto*sizeof(int));
	wk1 = (float*) emalloc(nto*sizeof(float));
	wk2 = (float*) emalloc(nto*sizeof(float));

	for(it=0;it<lt;it++) {
		tmp = it - ht;
		if(tmp<0) tmp = - tmp;
		tt[it]  = 2./(lt+1) * (1.-2.*tmp/(lt+1));
	}
	for(ix=0;ix<lx;ix++) {
		tmp = ix - hx;
		if(tmp<0) tmp = - tmp;
		tx[ix]  = 2./(lx+1) * (1.-2.*tmp/(lx+1));
	}
	for(iy=0;iy<ly;iy++) {
		tmp = iy - hy;
		if(tmp<0) tmp = - tmp;
		ty[iy]  = 2./(ly+1) * (1.-2.*tmp/(ly+1));
	}

	nsegy = 240+nt*sizeof(float);

	/* loop over output lines  */

	for(iy=1;iy<=nyo;iy++) {
		iyy = iyo+(iy-1)*dyo;
		ii2 = 0;
		/* read in a few lines for dip scan */
		for(i2=iyy-hy;i2<=iyy+hy;i2++) {
			i22 = i2;
			if(i22<1) i22=1;
			if(i22>ny) i22=ny;
			ltmp = (i22-1)*nx*nsegy+3600;
			fseek2g(infp,ltmp,0);
			for(ix=0;ix<nx;ix++) {
				fgettr(infp,&tr);
				ii1 = ii2*nx*nt+ix*nt;
				for(it=0;it<nt;it++)
						data[it+ii1] = tr.data[it];
			}
			ii2 = ii2 + 1;
		}

		/* scan through all dips to find px, py and amplitude along
		   the dip of max semblance */
		dipscn_(data,amp,work,&nt,&nx,&hy,
			pxo,pyo,amo,&ipow,
			&ixo,&dxo,&nxo,
			to,px,py,&nto,&npx,&npy,
			&ht,&hx,tt,tx,ty,
			&perc,ipxo,ipyo,wk1,wk2);


		/* scale px and py back */
		for(it=0;it<ntxo;it++) {
			pxo[it] *= dt;
			pyo[it] *= dt;
		}

		/* output 3 grid files */
		efwrite(pxo,sizeof(float),ntxo,pxfp);
		efwrite(pyo,sizeof(float),ntxo,pyfp);
		efwrite(amo,sizeof(float),ntxo,amfp);

		/* find grid min and max values */
		if(iy==1) {
			fminmax(pxo, ntxo, &pxmin, &pxmax);
			fminmax(pyo, ntxo, &pymin, &pymax);
			fminmax(amo, ntxo, &ammin, &ammax);
		} else {
			fminmax(pxo, ntxo, &tmp1, &tmp2);
			if(tmp1<pxmin) pxmin = tmp1;
			if(tmp2>pxmax) pxmax = tmp2;
			fminmax(pyo, ntxo, &tmp1, &tmp2);
			if(tmp1<pymin) pymin = tmp1;
			if(tmp2>pymax) pymax = tmp2;
			fminmax(amo, ntxo, &tmp1, &tmp2);
			if(tmp1<ammin) ammin = tmp1;
			if(tmp2>ammax) ammax = tmp2;
		}

	}

	free(pxo);
	free(pyo);
	free(amo);
	free(data);
	free(amp);
	free(work);
	free(to);
	free(px);
	free(py);
	free(tt);
	free(tx);
	free(ty);

	/* add grid header */
	usgh.scale = 1.e-6;
	usgh.dtype = 4;
	usgh.n1 = nto;
	usgh.n2 = nxo;
	usgh.n3 = nyo;
	usgh.n4 = 1;
	usgh.n5 = 1;
	usgh.o1 = tr.delrt + (ito-1.)*(float)tr.dt;
	usgh.o2 = ixo;
	usgh.o3 = iyo;
	usgh.o4 = 0.;
	usgh.o5 = 0.;
	usgh.d1 = dto*(float)tr.dt/1000.;
	usgh.d2 = dxo;
	usgh.d3 = dyo;
	usgh.d4 = 0.;
	usgh.d5 = 0.;
	usgh.dcdp2 = dxo;
	usgh.dline3 = dyo;
	usgh.ocdp2 = ixo;
	usgh.oline3 = iyo;
	usgh.orient = 1;
	usgh.gtype = 0;

	/* px grid file */
	usgh.gmin = pxmin;
	usgh.gmax = pxmax;
	ierr = fputusghdr(pxfp,&usgh);
			if(ierr!=0) err("error in output gridheader for pxfile ");
	/* py grid file */
	usgh.gmin = pymin;
	usgh.gmax = pymax;
	ierr = fputusghdr(pyfp,&usgh);
			if(ierr!=0) err("error in output gridheader for pyfile ");
	/* am grid file */
	usgh.gmin = ammin;
	usgh.gmax = ammax;
	ierr = fputusghdr(amfp,&usgh);
			if(ierr!=0) err("error in output gridheader for amfile ");

	efclose(pxfp);
	efclose(pyfp);
	efclose(amfp);

	return EXIT_SUCCESS;
}
