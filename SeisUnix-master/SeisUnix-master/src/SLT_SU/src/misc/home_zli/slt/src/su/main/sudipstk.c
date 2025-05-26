#include "usgrid.h"
#include "usu.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUDIPSTK - local stack along defined dip directions  	\n"
"\n"
"sudipstk datain=  [parameters] > output \n"
"\n"
"Required Parameters:\n"
"datain=                    name of input 3D data (nx by ny traces)	\n"
"nx=                        number of cdp per line in the input data	\n"
"ny=                        number of lines in the input data		\n"
"pxfile=                    name of the input x-dip (ms/cdp) grid file \n"
"pyfile=                    name of the input y-dip (ms/line) grid file  \n"
"Optional Parameters:\n"
"mlimit=256                 memory limit to run the program (in MB) 	\n"
"lt=11                      length of time (samples) window 	\n"
"ipow=2                     power to be applied to the data	\n"
"                           (if <0, applied power before stack, otherwise\n"
"                           compute power after stack		\n" 
"lx=11                      number of cdps used to stack	  	\n"
"ly=5                       number of lines used to stack 		\n"
"to0=delrt                  starting time of output (in ms)		\n" 
"                           default to time of first sample in input	\n"
"dto=dt                     output sample increment (in ms)		\n"
"                           default to sampling interval in input	\n"
"nto=nt                     number of samples  per trace to output	\n"
"                           (default to number of samples/trace in input)\n"
"ixo=1                      starting output cdp position		\n"
"dxo=1                      output cdp increment			\n"
"nxo=nx                     number of cdps per line to output		\n"
"iyo=1                      starting output line position		\n"
"dyo=1                      output line increment			\n"
"nyo=ny                     number of lines to output		\n"
"\n"
"Author:	Zhiming Li		      		10-30-95		\n"
"\n";
/**************** end self doc *******************************************/

void readdata(FILE *infp, float *data, segytrace tr1, 
	int *jy, int iy, int ny, int nt, int nx, int hy, int nycore);

void readpxpy(FILE *pxfp, FILE *pyfp, 
	float o1, float o2, float o3, 
	float d1, float d2, float d3, 
	int n1, int n2, int n3,
	float *pxo, float *pyo, int iy, int ix); 

void intp3(FILE *fp, float *p1, float *p2, float *p3, float *p4, float *po,
	float o1, float d1, int n1, int n2, 
	int i21, int i22, int i31, int i32,
	float w21, float w22, float w31, float w32);

void readhead(FILE *infp, segytrace tr1, int ixx, int iyy,
	int nx, int ny, int nsegy); 

segytrace tr, tr1;

main(int argc, char **argv)
{
	int nt,it;
	float dt;
	FILE *pxfp, *pyfp, *infp, *outfp=stdout;
	char *datain,*pxfile, *pyfile;
	int iy, ix;
	int ipow;
	int nx,ny,lt,lx,ly;
	int nxo,nyo,ixo,iyo,dxo,dyo,nto;
	int ii1, ii2, iyy, ntxo;
	float tmp, tmp1, tmp2;
	int nsegy;
	float *data, *pxo, *pyo, *work, *stack;
	float to0, dto;
	int hy, i2, i22, iy0, ierr, ht, hx;
	float *tt, *tx, *ty;
	int nycore, mlimit, n1, n2, n3,jy,ixx;
	float o1,o2,o3,d1,d2,d3,t0;

	long ltmp;

	segychdr ch;
    	segybhdr bh;
	usghed usgh;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

/* open input data */
	if (!getparstring("datain",&datain)) err(" must specify datain ");
	if((infp = fopen(datain,"r"))==NULL)
             	err("datain %s open failed \n",datain);
	file2g(infp);
	fgethdr(infp,&ch,&bh);
	

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt * 0.001;
	t0 = (float)tr.delrt;

	/* get required parameters */
	if (!getparstring("pxfile",&pxfile)) err(" must specify pxfile ");
	if (!getparstring("pyfile",&pyfile)) err(" must specify pyfile ");

	if (!getparint("nx",&nx)) err(" must specify nx");
	if (!getparint("ny",&ny)) err(" must specify ny");

	/* get optional parameters */
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
	if (!getparfloat("to0",&to0)) to0 = tr.delrt;
	if (!getparfloat("dto",&dto)) dto = (float)tr.dt*0.001;
	if (!getparint("nto",&nto)) nto = nt;

	if (!getparint("mlimit",&mlimit)) mlimit = 256;



	if((pxfp = fopen(pxfile,"r"))==NULL)
             	err("pxfile %s open failed \n",pxfile);
	if((pyfp = fopen(pyfile,"r"))==NULL)
             	err("pyfile %s open failed \n",pyfile);

	pxo = (float*) emalloc(n1*sizeof(float));
	pyo = (float*) emalloc(n1*sizeof(float));
	work = (float*) emalloc(nt*sizeof(float));

	tmp = mlimit*1024*1024./(4.*nt*nx);
	nycore = (int) tmp;
	if(nycore<ly) err(" increase mlimit to %d \n",ly*nt*nx*4/1024/1024+1);
	if(dyo>=ly) nycore = ly;
	data = (float*) emalloc(nx*nycore*nt*sizeof(float));
	stack = (float*) emalloc(nto*sizeof(float));

	file2g(pxfp);
	file2g(pyfp);

	hy = (ly-1)/2;
	hx = (lx-1)/2;
	ht = (lt-1)/2;
	tt = (float*) emalloc(lt*sizeof(float));
	tx = (float*) emalloc(lx*sizeof(float));
	ty = (float*) emalloc(ly*sizeof(float));

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


	ierr = fgetusghdr(pxfp,&usgh);
		if(ierr!=0) err("error in input gridheader for pxfile ");
	fseek2g(pxfp,0,0);

	o1 = usgh.o1;
	d1 = usgh.d1;
	n1 = usgh.n1;
	o2 = usgh.o2;
	d2 = usgh.d2;
	n2 = usgh.n2;
	o3 = usgh.o3;
	d3 = usgh.d3;
	n3 = usgh.n3;

	jy  = -1;

	bh.hns = nto;
    	bh.hdt = dto*1000.;

        fputhdr(outfp,&ch,&bh);

	
	/* loop over output traces  */
	for(iy=0;iy<nyo;iy++) {

		iyy = iy*dyo + iyo;
		readdata(infp,data,tr1,&jy,iyy-1,ny,nt,nx,hy,nycore);

		for(ix=0;ix<nxo;ix++) {
			ixx = ix*dxo+ixo;
			readpxpy(pxfp,pyfp,o1,o2,o3,d1,d2,d3,n1,n2,n3,
				pxo,pyo,iyy,ixx);


			readhead(infp,tr1,ixx,iyy,nx,ny,nsegy);


			tr1.ns  = nto;
			tr1.delrt = to0;
			tr1.dt = dto*1000.;


			sstack_(data,pxo,pyo,stack,work,
				&iyy,&ixx,&jy,&nycore,&nx,
				&d1,&o1,&n1,
				&dt,&t0,&nt,
				&dto,&to0,&nto,
				&ipow,&hy,&hx,&ht,ty,tx,tt);


			for(it=0;it<nto;it++) {
				tr1.data[it] = stack[it]; 
			}

			fputtr(outfp,&tr1);
		}	

	}
	
	free(pxo);
	free(pyo);
	free(data);
	free(stack);
	free(tt);
	free(tx);
	free(ty);

	return EXIT_SUCCESS;
}

void readdata(FILE *infp, float *data, segytrace tr1, 
	int *jy, int iy, int ny, int nt, int nx, int hy, int nycore) {

	
	long ltmp;
	int i3, i2, i1, iyy, iread, i11;

	iyy = *jy; 
	iread = 0;
	if(*jy==-1) {
		iyy = iy - hy;
		if(iyy<0) iyy = 0;
		iread = 1;
	} else if( (*jy>(iy-hy) && iy>=hy)  
		|| (*jy+nycore)<(iy+hy) && (iy+hy)<ny ) {
		iyy = iy - hy; 
		if(iyy<0) iyy = 0;
		iread = 1;
	}
	*jy = iyy;
	if(iread==1) {
		ltmp = iyy*nx*(nt*4+240);
		ltmp = ltmp + 3600;
		fseek2g(infp,ltmp,0);
		for(i3=0;i3<nycore;i3++) {
			for(i2=0;i2<nx;i2++) {
				i11 = (i3*nx+i2)*nt;
				fgettr(infp,&tr1);
				for(i1=0;i1<nt;i1++) {
					data[i1+i11] = tr1.data[i1];
				}
			}
		}
		iread = 0;
	}
	
}

void readpxpy(FILE *pxfp, FILE *pyfp, 
	float o1, float o2, float o3, 
	float d1, float d2, float d3, 
	int n1, int n2, int n3,
	float *pxo, float *pyo, int iy, int ix) {

	float tmp;
	float *p1, *p2, *p3, *p4;
	int i2,i3;
	float w31, w32, w21, w22;
	int i31, i32, i21, i22;
	
	p1 = (float*) emalloc(n1*sizeof(float)); 
	p2 = (float*) emalloc(n1*sizeof(float)); 
	p3 = (float*) emalloc(n1*sizeof(float)); 
	p4 = (float*) emalloc(n1*sizeof(float)); 

	tmp = (iy-o3)/d3;
	i3 = tmp;
	if(i3<0) {
		i31 = 0; i32 = 0; w31 = 0.5; w32 = 0.5;
	} else if(i3>=n3-1) { 
		i31 = n3-1; i32 = n3-1; w31 = 0.5; w32 = 0.5;
	} else {
		i31 = i3; i32 = i3 + 1; w32 = tmp - i3; w31 = 1. - w32;
	}

	tmp = (ix-o2)/d2;
	i2 = tmp;
	if(i2<0) {
		i21 = 0; i22 = 0; w21 = 0.5; w22 = 0.5;
	} else if(i3>=n2-1) { 
		i21 = n2-1; i22 = n2-1; w21 = 0.5; w22 = 0.5;
	} else {
		i21 = i2; i22 = i2 + 1; w22 = tmp - i2; w21 = 1. - w22;
	}


	/*
	fprintf(stderr,"i21=%d i22=%d i31=%d i32=%d \n",
		i21,i22,i31,i32);
	*/
	
	intp3(pxfp, p1, p2, p3, p4, pxo,
		o1, d1, n1, n2, 
		i21, i22, i31, i32,
		w21, w22, w31, w32);


	intp3(pyfp, p1, p2, p3, p4, pyo,
		o1, d1, n1, n2, 
		i21, i22, i31, i32,
		w21, w22, w31, w32);

	free(p1);
	free(p2);
	free(p3);
	free(p4);
}


void intp3(FILE *fp, float *p1, float *p2, float *p3, float *p4, float *po,
	float o1, float d1, int n1, int n2, 
	int i21, int i22, int i31, int i32,
	float w21, float w22, float w31, float w32) {

	long lpos;
	int i1;


	lpos = i21+i31*n2;
	lpos = lpos*n1*4;
	fseek2g(fp,lpos,0);
	efread(p1,sizeof(float),n1,fp);

	lpos = i22+i31*n2;
	lpos = lpos*n1*4;
	fseek2g(fp,lpos,0);
	efread(p2,sizeof(float),n1,fp);

	lpos = i21+i32*n2;
	lpos = lpos*n1*4;
	fseek2g(fp,lpos,0);
	efread(p3,sizeof(float),n1,fp);

	lpos = i22+i32*n2;
	lpos = lpos*n1*4;
	fseek2g(fp,lpos,0);
	efread(p4,sizeof(float),n1,fp);
	
	for(i1=0;i1<n1;i1++) {
        	po[i1] = (p1[i1]*w21 + p2[i1]*w22)*w31 +
                                (p3[i1]*w21 + p4[i1]*w22)*w32;
        }

}

void readhead(FILE *infp, segytrace tr1, int ixx, int iyy,
	int nx, int ny, int nsegy) {
	long ltmp;
	ltmp  = (iyy*nx+ixx)*nsegy+3600;
	fseek2g(infp,ltmp,0);
	fgettr(infp,&tr1);
}
