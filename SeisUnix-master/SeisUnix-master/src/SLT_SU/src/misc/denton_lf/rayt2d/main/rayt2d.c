
/* rayt2d: $Revision: 1.2 $ ; $Date: 94/10/11 13:51 $	*/

#include "usgrid.h"
#include "cwp.h"
#include "par.h"

char *sdoc[] = {
"rayt2d - traveltime tables calulated by paraxial ray tracing ", 
" ",
"rayt2d vfile= tfile= [optional parameters] ",
" ",
"Required Parameters: ",
"vfile=stdin            file containing velocitiy v(nz,nx)    	 ",
"tfile=stdout           file containing traveltime tables  ", 
"			t(nzo,nxo,nxs)	    ", 
" ",
"Optional Parameters: ",
"dt=0.008  		time sampling interval in ray tracing	 ",
"nt=401  		number of time sampling in ray tracing	 ",
"                       (one-way travel time calculation)	 ",
" ",
"the following six parameters are from velocity grid header ONLY",
"fz=from-vfile	        first depth sample in velocity 	", 
"nz=from-vfile       	Number of depth samples in velocity 	 ",
"dz=from-vfile       	depth interval in velocity		 ",
"fx=from-vfile       	first lateral coordinate in velocity 	", 
"nx=from-vfile		Number of lateral coordinates in velocity	 ",
"dx=from-vfile		lateral interval in velocity",
" ",			  
"fxo=fx                 first x coordinate in traveltime table ",
"fzo=fz                 first z coordinate in traveltime table ",
"dxo=dx                 x interval in traveltime table ",
"dzo=dz                 z interval in traveltime table ",
"nxo=nx                 number of x samples in traveltime table ",
"nzo=nz                 number of z samples in traveltime table ",
" ",
"nxs=1                  number of x samples for source locations	 ",
"fxs=0.                 x coordinate of first source ", 
"dxs=2*dxo              x coordinate increment of sources ", 
"aperx=0.5*nx*dx        ray tracing aperature in x-direction ", 
" ",
"fa=-60            	first take-off angle ", 
"da=2            	increment of take-off angle ",
"na=61            	number of take-off angles ",
"amin=0            	minimum emergence angle  ", 
"                       measured from vertical --- moving upwards	",
"amax=90            	maximum emergence angle	 ",
" ",
"fac=0.01            	factor to determine radius for extrapolation	 ",
"ek=1              	=1 to implement eikonal equation in shadow zones ",
"ms=1			print verbal information at every ms finished sources	 ",
"jpfile=			name of output job print file",
"restart=n		job is restarted (=y yes; =n no)		 ",
"         ", 
" ",
"Note:	 ",
"1. Each traveltime table is calculated by paraxial ray tracing; then 	 ",
"   traveltimes in shadow zones are computed by solving eikonal equation.  ",
"2. A smoothed velocity is prefered. All sampling information of the	 ",
"   velocity is included in the file grid header.			 ",
"3. Traveltime table and source locations must be within velocity model. ", 
"4. Ray tracing aperatures can be choosen as sum of migration aperatures ", 
"   plus half of maximum offset. ", 
"5. memory requirement for this program is about,		",
"             (4*nz*(nx+mx)+3*nxo*nzo+10*128*1001)*4 bytes		",
"   where mx = 2+2*aperx/dx			",
" ",
NULL};

/* Author:		Zhenyue Liu, 8/15/94	 */

void raytime_(int *na,int *nt,int *nxo,int *nzo,float *xs,
  float *fa,float *da,float *amin,float *amax,
  float *dt,float *fxo,float *fzo,float *dxo,float *dzo,
  float *fac,float *t,int *nx,int *nz,float *fx,float *fz,
  float *dx,float *dz,float *v,float *vxx,float *vxz,float *vzz,float *s);
  
void ov2int_(float *v,int *nx,int *nz,float *fx,float *fz,float *dx,float *dz,
  float *ov2,int *nxo,int *nzo,float *fxo,float *fzo,float *dxo,float *dzo);
 
void dv2_(int *nx,int *nz,float *dx,float *dz,float *v,
  float *vxx,float *vxz,float *vzz);
 
void eiknl_(float *t,float *ov2,float *tt1,float *tt2,int *nx,
  int *nz,float *dx,float *dz,float *tmax);

void trans_(int *nx,int *nz,int *nxt,int *nx0,float *v,float *vt);

 
main(int argc, char **argv)
{
	int 	na,nat,nt,nxs,nxo,nzo,nx,nz,nxt,nx0,mx;
 	float   dt,xs,fxs,dxs,exs,fxo,fzo,dxo,dzo,exo,
		fa,a,ea,amin,amax,da,fat,fac,tmax,aperx,temp,
		fx,fz,dx,dz,odx,odz,ex,ez,fxt,ext,
 		*v,*vxx,*vxz,*vzz,*vt,*vxxt,*vxzt,*vzzt,		
 		*t,*s,*ov2,*tt1,*tt2;
	int ixs,ixs0,nsize,ek,ms;
  	char *vfile, *tfile, *jpfile;
  	char *restart; 	
 	FILE *vfp, *tfp, *jpfp;
	long long isize;
  
    usghed ugh;
	int ierr; 
 	float scale;
 	float gmin,gmax;
  
	
	/* hook up getpar to handle the parameters */
	initargs(argc,argv);
	requestdoc(1);
		
	/* get velocity information from header file */
	if( !getparstring("vfile",&vfile) ) {
		vfp = stdin;
	} else {
		vfp = fopen(vfile,"r"); 
	} 
	ierr = fgetusghdr(vfp,&ugh);
        if(ierr!=0) err("fgetusghdr error");
  	nx = ugh.n2;
	nz = ugh.n1;
	fx = ugh.o2;
 	fz = ugh.o1;
	dx = ugh.d2;
 	dz = ugh.d1;
	if(nx<3 || nz<3 )
err("number of velocity samples in each direction must be not less than 3!\n");

	/* get optional parameters */
	if (!getparint("nt",&nt)) nt = 401;
	if(nt>1001) err("nt cannot exceed 1001!  \n");
	if (!getparfloat("dt",&dt)) dt = 0.008;
	tmax = (nt-1)*dt; 
	if (dt<=0.) err("dt must be positive!  \n");
 	if (!getparint("nxo",&nxo)) nxo = nx;
 	if (!getparint("nzo",&nzo)) nzo = nz;
	if(ek && (nxo<3 || nzo<3) )
	    err("Traveltime table needs at least three traces for eikonal equation making up!  \n");
	if (!getparfloat("fxo",&fxo)) fxo = fx;
 	if (!getparfloat("fzo",&fzo)) fzo = fz;
	if (!getparfloat("dxo",&dxo)) dxo = dx;
  	if (!getparfloat("dzo",&dzo)) dzo = dz;
	exo = fxo+(nxo-1)*dxo;
 
 	if (!getparint("nxs",&nxs)) nxs = 1;
 	if (!getparfloat("fxs",&fxs)) fxs = 0.;
 	if (!getparfloat("dxs",&dxs)) dxs = dxo*2;
 	exs = fxs+(nxs-1)*dxs;
 	if (!getparfloat("aperx",&aperx)) aperx = 0.5*nx*dx;
	if(nxs>1) aperx += dxs;
	mx = 2.0*aperx/dx+2.99;
	if(mx>nx) mx = nx;

 	if (!getparfloat("fa",&fa)) fa = -60;
 	if (!getparfloat("da",&da)) da = 2.;
	if (da<=0.) err("da must be positive!  \n");
	if (!getparint("na",&na)) na = 61;
	if (!getparfloat("amin",&amin)) amin = 0;
	if (!getparfloat("amax",&amax)) amax = 90;
	if (amax>180 || amin<0 ) 
		err("amin and amax must be within 0 to 180 degrees!\n");	
   	fa *= PI/180.;
	da *= PI/180.;
	ea = fa+(na-1)*da;
	amin *= PI/180.;
	amax *= PI/180.;
   	if (!getparfloat("fac",&fac)) fac = 0.01;
 	if (!getparint("ek",&ek)) ek = 1;
 	if (!getparstring("restart",&restart)) restart = "n"; 
	if (!getparint("ms",&ms)) ms = 1;
 

  	ixs0 = 0;
 	isize = 0;

	nsize = nzo*nxo;
	if( !getparstring("tfile",&tfile) ) {
		tfp = stdout;
	} else {
		if((tfp = fopen(tfile,"r"))!=NULL) {
			fclose(tfp);
			tfp = fopen(tfile,"r+");	
		} else {
	   		tfp = fopen(tfile,"w");
		}
		if(restart[0]=='y') { 
			fseek2g(tfp,0,SEEK_END);
			isize = ftell2g(tfp);
			isize = isize/(sizeof(float)*nsize);
			ixs0 = isize;
 			/* if(isize==ns) return 0; */
		} else {
			fclose(tfp);
			tfp = fopen(tfile,"w");	
		}
		fseek2g(tfp,isize*nsize*sizeof(float),SEEK_SET);
 
	}

	/* ensure sources is in grid */
	ex = fx+(nx-1)*dx;
 	ez = fz+(nz-1)*dz;
	if(fx-fxs>0.0 || ex-exs<0.0 || fz>0.0) 
		err("source lies outside of specified x grid  \n");
  
	if(fx>fxo || ex<exo || fz>fzo || ez<fzo+(nzo-1)*dzo) 
		err("output lies outside of specified x grid  \n");

        if (!getparstring("jpfile",&jpfile)) {
                jpfp = stderr;
        }else {
                jpfp = fopen(jpfile,"w");
        }

        fprintf(jpfp," \n");
        fprintf(jpfp," RAYT2D parameters \n");
        fprintf(jpfp,"===================\n");
        fprintf(jpfp," vfile=%s \n",vfile);
        fprintf(jpfp," tfile=%s \n",tfile);
        fprintf(jpfp," one-way time: dt=%g nt=%d \n",dt,nt);
        fprintf(jpfp," nz=%d fz=%g dz=%g \n",nz,fz,dz);
        fprintf(jpfp," nx=%d fx=%g dx=%g \n",nx,fx,dx);
        fprintf(jpfp," nzo=%d fzo=%g dzo=%g \n",nzo,fzo,dzo);
        fprintf(jpfp," nxo=%d fxo=%g dxo=%g \n",nxo,fxo,dxo);
        fprintf(jpfp," nxs=%d fxs=%g dxs=%g\n",nxs,fxs,dxs); 
        fprintf(jpfp," aperx=%g mx=%d\n",aperx,mx);
        fprintf(jpfp," fa=%g da=%g na=%d \n",fa*180./PI,da*180./PI,na);
        fprintf(jpfp," amin=%g amax=%g \n",amin*180./PI,amax*180./PI);
        fprintf(jpfp," ek=%d ms=%d jpfile=%s restart=%s\n",
                ek,ms,jpfile,restart);
 
/* velocity input */
 	/* allocate space */
 	v = alloc1float(nx*nz);
 	vxx = alloc1float(nx*nz);
 	vxz = alloc1float(nx*nz);
 	vzz = alloc1float(nx*nz);
  	vt = alloc1float(mx*nz);
 	vxxt = alloc1float(mx*nz);
 	vxzt = alloc1float(mx*nz);
 	vzzt = alloc1float(mx*nz);
 	ov2 = alloc1float(nzo*nxo);
 
	/* read velocities */
	fseek(vfp,0,0);
	fread(v,sizeof(float),nx*nz,vfp);
   	fprintf(jpfp,"\n\tfinish velocity input  \n" );
  
	/* compute second derivatives of velocity  */
	dv2_(&nx,&nz,&dx,&dz,v,vxx,vxz,vzz);
   
 	/* compute slowness squares  */
	ov2int_(v,&nx,&nz,&fx,&fz,&dx,&dz,ov2,&nxo,&nzo,
		&fxo,&fzo,&dxo,&dzo);

    
	/* allocate time arrays   */
 	tt1 = alloc1float(nxo);
 	tt2 = alloc1float(nxo);
	t = alloc1float(nzo*nxo);
	s = alloc1float(nzo*nxo);
 

   	fprintf(jpfp,"\tbegin traveltime calculation  \n" );

	/* loop over sources */
        for (ixs=ixs0,xs=fxs+ixs0*dxs;ixs<nxs;ixs++,xs+=dxs){  
 
	  /* reduce the velocity model according to source and output	*/
	    temp = fxo;
	    if(xs<temp) temp = xs;
	    if(xs-aperx>temp) temp = xs-aperx;
	    nx0 = (temp-fx)/dx;
	    fxt = fx+nx0*dx;
	    temp = fxo+(nxo-1)*dxo;
	    if(xs>temp) temp = xs;
	    if(xs+aperx<temp) temp = xs+aperx;
	    nxt = 1+(int)((temp-fx)/dx+0.99)-nx0;
	    ext = fxt+(nxt-1)*dx;
 
	  /* determine range of taking-off angle	*/
	    fat = fa;
	    nat = na;
	    if(xs==fxt && fat<0){
		fat = 0;
		nat = ea/da+1.5;
 	    } else if(xs==ext && ea>0) 
		nat = 1.5-fa/da;
  

	  /* compute travel time by paraxial ray tracing */

	    if(nxt<nx ) {
		trans_(&nx,&nz,&nxt,&nx0,v,vt);
		trans_(&nx,&nz,&nxt,&nx0,vxx,vxxt);
 		trans_(&nx,&nz,&nxt,&nx0,vxz,vxzt);
 		trans_(&nx,&nz,&nxt,&nx0,vzz,vzzt);

 	        raytime_(&nat,&nt,&nxo,&nzo,&xs,&fat,&da,&amin,&amax,
		  &dt,&fxo,&fzo,&dxo,&dzo,&fac,t,&nxt,&nz,&fxt,&fz,&dx,&dz,
		  vt,vxxt,vxzt,vzzt,s);
	    }else
	      	raytime_(&nat,&nt,&nxo,&nzo,&xs,&fat,&da,&amin,&amax,
		  &dt,&fxo,&fzo,&dxo,&dzo,&fac,t,&nx,&nz,&fx,&fz,&dx,&dz,
		  v,vxx,vxz,vzz,s);
 
  
	  /* make up in shadow zone by eikonal equation	*/
   	    if (ek) eiknl_(t,ov2,tt1,tt2,&nxo,&nzo,&dxo,&dzo,&tmax);  
 
 	    /* write traveltime  	*/
  	    fwrite(t,sizeof(float),nxo*nzo,tfp);
 
	    if(ixs%ms==0)
       fprintf(jpfp,"travel time computed at source ixs=%d xs=%g  \n",ixs+1,xs);
	}

	ugh.scale = 1.;
	ugh.dtype = 4;
 	ugh.n1 = nzo;
  	ugh.n2 = nxo;
  	ugh.n3 = 1;
 	ugh.n4 = nxs;
  	ugh.n5 = 1;
  	ugh.d1 = dzo;
	ugh.d2 = dxo;
	ugh.d3 = dxo;
   	ugh.d4 = dxs;
   	ugh.d5 = dxs;
 	ugh.o1 = fzo; 
	ugh.o2 = fxo; 
	ugh.o3 = 0.; 
 	ugh.o4 = fxs;
	ugh.o5 = 0.; 
 	ugh.gmin = 0.;
	ugh.gmax = tmax;

 
 	fputusghdr(tfp,&ugh); 

  	fclose(tfp);
 
 	/* free space */
 	free1float(v);
  	free1float(vxx);
  	free1float(vxz);
  	free1float(vzz);
 	free1float(vt);
   	free1float(vxxt);
  	free1float(vxzt);
  	free1float(vzzt);
  	free1float(t);
	free1float(s);	
  	
	return 0;
}
