#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc =
"GRIDSALT - 3D attribute grid building around a salt body \n"
"\n"
"gridsalt <infile >outfile [optional parameters]\n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D attribute grid file	(velocity)	\n"
"outfile=       name of the output 3D attribute grid file (velocity) \n"
"saltvelo=      salt velocity (attribute) value				\n"
"depthnull=     depth value to be used to identify no salt area in the 	\n"
"               depth grid files \n"
"salttop=       name of the 2D depth grid defining the top of the salt \n"
 "Optional Parameters:\n"
"salttop2=      name of the 2D depth grid defining the top of the salt \n"
"               below the overhangh 					\n"
"               (if not given, only salttop is used to define salt top)\n"
"saltbot=       name of the 2D depth grid defining the overhang bottom of \n"
"               the salt (if not given,  default to below the bottom of the  \n"
"               model --- no salt overhand bottom )		\n"
"saltbot2=      name of the 2D depth grid defining the bottom of the salt \n"
"               (if not given,  default to below the bottom of the  \n"
"               model --- no salt bottom )		\n"
"vscalbot=1.0   velocity at the salt bottom (saltbot2) will be scaled \n"
"               by vscalbot \n"
"vscalzmx=1.0   velocity at the maximum depth will be scaled by vscalzmx \n"
"               (linearly interpolated scale will be applied for velocities \n"
"               at depths between base of salt and the maximum depth) \n" 
"               when vscalbot and vscalzmx are both 1.0, no scaling applied \n"
"dscal=500.     distance from salt edge where the full scaling applied \n"
"               (linear tapering will be appled to the salt edge)    \n"
"               dscal must have the same unit as d2 and d3 in the velocity \n"
"               grid file						\n"
"check=0        check grided model				\n"
"               0=no						\n"
"               1=yes  						\n"
"               remove salt if <=nc of 8 adjacent lateral points are salt \n"  
"               fill salt if >=(8-nc) of 8 adjacent lateral points are salt \n"
"nc=0           number of adjacent points to have the same property as \n"
"               that of the checked point		\n"
"iter=1         number of iterations to check the grided volume	\n"
"vtopgrid=      grid value file  at the top of the salt top (salttop)\n" 
"vbot2grid=     grid value file at the bottom of the salt bottom (saltbot2)\n" 
"               if both vtopgrid and vbot2grid are not specified, saltvelo \n"
"               is used within the salt \n"
"               if vtopgrid is given and vbot2grid is not, vtopgrid value \n"
"               will be used below salttop \n"
"               if vbot2grid is given and vtopgrid is not, grid values \n"
"               within the salt body will be interpolated from the grid \n"
"               value at the salttop to the the vbot2grid value at the \n"
"               saltbot2 \n"
" Notes:						\n"
" 1. when there are overhungs, use four horizons: \n"
"       salttop, saltbot, salttop2 and saltbot2 as showen below \n"
"       						\n"
" ---- salttop						\n"
" **** saltbot					\n"
" ++++ salttop2 					\n"
" &&&& saltbottom2 					\n"
" SED  sediment region					\n"
" SALT salt region					\n"
"       						\n"
"       						\n"
"           SED						\n"
"         ---------- 					\n"
"  SED   .         *.      				\n"
"       .  SALT   *  .					\n"
" ------         *    .    	  			\n"
"  SALT          * SED .  SED				\n"
"                +      .				\n"
"                  +     .				\n"
"&&&&&&&&&&          + + +------------ 			\n"
"          & 						\n"
"           &						\n"
"            &&&&&&&&&&&&&&&&&&&&&&&&&& 	        \n"
"       						\n"
" 2. when there are no overhungs, use two horizons: \n"
"       salttop and saltbot2 to define the salt body \n"
"\n"
"AUTHOR:  Zhiming Li,         12/05/95			\n"
"\n";

main(int argc, char **argv)
{
	usghed usghin, usghtop, usghbot, usghtop2, usghbot2;
	usghed usghvtop, usghvbot2;
	FILE *infp,*outfp,*topfp,*botfp,*top2fp,*bot2fp;
	FILE *vtopfp,*vbot2fp;
	char *infile,*outfile,*salttop, *saltbot,*salttop2,*saltbot2;
	char *vtopgrid, *vbot2grid;
	float saltvelo, depthnull, zmax;
	int ibot, itop2, ibot2;
	int ierr;

	int n1,n2,n3;
	int i1,i2,i3;
	float d1,o1;

	float *grid, *ztop, *zbot, *ztop2, *zbot2, gmin, gmax;
	float *vtop, *vbot2;
	float top, bot, top2, bot2;
	float tmp;
	int i1top, i1bot, i1top2, i1bot2;

	float dscal, vscalbot, vscalzmx;
	float ***scale1, ***scale2;
	float r1, r2, r3, d2, d3, lam, o2, o3;
	int iscal=0,depth,slowness,iter,one,jter;

	int check,nc,ii,nsalt,niter;
	float *cube,*vs,v0,velo;
	int ivtop=0, ivbot2=0;
	float vt, vb;


	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	file2g(infp);

	ierr = fgetusghdr(infp,&usghin);
    	if(ierr!=0) err(" input grid header error ");
	if(getparstring("outfile",&outfile)) {
		outfp = efopen(outfile,"w");
	} else {
		outfp = stdout;
	}
	file2g(outfp);
	if(!getparfloat("saltvelo",&saltvelo)) err(" saltvelo missing ");
	if(!getparfloat("depthnull",&depthnull)) err(" depthnull missing ");


	if (getparstring("salttop",&salttop)) {
		topfp = efopen(salttop,"r");
		ierr = fgetusghdr(topfp,&usghtop);
      		if(ierr!=0) err(" salttop grid header error ");
	} else {
		err(" salttop missing ");
	}

	if (getparstring("saltbot",&saltbot)) {
		botfp = efopen(saltbot,"r");
		ierr = fgetusghdr(botfp,&usghbot);
      	if(ierr!=0) err(" saltbot grid header error ");
		ibot = 1;	
	} else {
		ibot = 0;
	}

	if (getparstring("salttop2",&salttop2)) {
		top2fp = efopen(salttop2,"r");
		ierr = fgetusghdr(top2fp,&usghtop2);
      	if(ierr!=0) err(" salttop2 grid header error ");
		itop2 = 1;	
	} else {
		itop2 = 0;
	}

	if (getparstring("saltbot2",&saltbot2)) {
		bot2fp = efopen(saltbot2,"r");
		ierr = fgetusghdr(bot2fp,&usghbot2);
      	if(ierr!=0) err(" saltbot2 grid header error ");
		ibot2 = 1;	
	} else {
		ibot2 = 0;
	}

	if (getparstring("vtopgrid",&vtopgrid)) {
		vtopfp = efopen(vtopgrid,"r");
		ierr = fgetusghdr(vtopfp,&usghvtop);
      	if(ierr!=0) err(" vtopgrid header error ");
		ivtop = 1;
		saltvelo = -9898;
	}
	if(getparstring("vbot2grid",&vbot2grid) ) {
		vbot2fp = efopen(vbot2grid,"r");
		ierr = fgetusghdr(vbot2fp,&usghvbot2);
      	if(ierr!=0) err(" vbot2grid header error ");
		ivbot2 = 1;
		saltvelo = -9898;
	}

	n1 = usghin.n1;
	n2 = usghin.n2;
	n3 = usghin.n3;
	o1 = usghin.o1;
	d1 = usghin.d1;
	d2 = usghin.d2;
	d3 = usghin.d3;
	o2 = usghin.o2;
	o3 = usghin.o3;
	gmin = usghin.gmin;
	gmax = usghin.gmax;

	zmax = o1 + n1*d1;

	/* memory allocations */
	ztop = (float*) emalloc(n2*n3*sizeof(float));
	zbot = (float*) emalloc(n2*n3*sizeof(float));
	ztop2 = (float*) emalloc(n2*n3*sizeof(float));
	zbot2 = (float*) emalloc(n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	vtop = (float*) emalloc(n2*n3*sizeof(float));
	vbot2 = (float*) emalloc(n2*n3*sizeof(float));

	
	if(usghin.n2!=usghtop.n1) err("check top grid header n1");
	if(usghin.n3!=usghtop.n2) err("check top grid header n2");
	if(usghin.o2!=usghtop.o1) err("check top grid header o1");
	if(usghin.o3!=usghtop.o2) err("check top grid header o2");
	if(usghin.d2!=usghtop.d1) err("check top grid header d1");
	if(usghin.d3!=usghtop.d2) err("check top grid header d2");
	efseek(topfp,0,0);
	efread(ztop,sizeof(float),n2*n3,topfp);


	if(ibot==1) {
		if(usghin.n2!=usghbot.n1) err("check bot grid header n1");
		if(usghin.n3!=usghbot.n2) err("check bot grid header n2");
		if(usghin.o2!=usghbot.o1) err("check bot grid header o1");
		if(usghin.o3!=usghbot.o2) err("check bot grid header o2");
		if(usghin.d2!=usghbot.d1) err("check bot grid header d1");
		if(usghin.d3!=usghbot.d2) err("check bot grid header d2");
		efseek(botfp,0,0);
		efread(zbot,sizeof(float),n2*n3,botfp);
	} else {
		for(i1=0;i1<n2*n3;i1++) zbot[i1] = o1+(n1-1)*d1;
	}

	if(saltvelo==-9898) {
		if(ivtop==1) {
		if(usghin.n2!=usghvtop.n1) err("check vtopgrid header n1");
		if(usghin.n3!=usghvtop.n2) err("check vtopgrid header n2");
		if(usghin.o2!=usghvtop.o1) err("check vtopgrid header o1");
		if(usghin.o3!=usghvtop.o2) err("check vtopgrid header o2");
		if(usghin.d2!=usghvtop.d1) err("check vtopgrid header d1");
		if(usghin.d3!=usghvtop.d2) err("check vtopgrid header d2");
		efseek(vtopfp,0,0);
		efread(vtop,sizeof(float),n2*n3,vtopfp);
		} 
		if(ivbot2==1) {
		if(usghin.n2!=usghvbot2.n1) err("check vbot2grid header n1");
		if(usghin.n3!=usghvbot2.n2) err("check vbot2grid header n2");
		if(usghin.o2!=usghvbot2.o1) err("check vbot2grid header o1");
		if(usghin.o3!=usghvbot2.o2) err("check vbot2grid header o2");
		if(usghin.d2!=usghvbot2.d1) err("check vbot2grid header d1");
		if(usghin.d3!=usghvbot2.d2) err("check vbot2grid header d2");
		efseek(vbot2fp,0,0);
		efread(vbot2,sizeof(float),n2*n3,vbot2fp);
		}
	}

	if(itop2==1) {
		if(usghin.n2!=usghtop2.n1) err("check top2 grid header n1");
		if(usghin.n3!=usghtop2.n2) err("check top2 grid header n2");
		if(usghin.o2!=usghtop2.o1) err("check top2 grid header o1");
		if(usghin.o3!=usghtop2.o2) err("check top2 grid header o2");
		if(usghin.d2!=usghtop2.d1) err("check top2 grid header d1");
		if(usghin.d3!=usghtop2.d2) err("check top2 grid header d2");
		efseek(top2fp,0,0);
		efread(ztop2,sizeof(float),n2*n3,top2fp);
	} else {
		for(i1=0;i1<n2*n3;i1++) ztop2[i1] = depthnull;
	}

	if(ibot2==1) {
		if(usghin.n2!=usghbot2.n1) err("check bot2 grid header n1");
		if(usghin.n3!=usghbot2.n2) err("check bot2 grid header n2");
		if(usghin.o2!=usghbot2.o1) err("check bot2 grid header o1");
		if(usghin.o3!=usghbot2.o2) err("check bot2 grid header o2");
		if(usghin.d2!=usghbot2.d1) err("check bot2 grid header d1");
		if(usghin.d3!=usghbot2.d2) err("check bot2 grid header d2");
		efseek(bot2fp,0,0);
		efread(zbot2,sizeof(float),n2*n3,bot2fp);
	} else {
		for(i1=0;i1<n2*n3;i1++) zbot2[i1] = o1+(n1-1)*d1;
	}

/* compute scales for velocity below the salt */
	if(!getparfloat("vscalbot",&vscalbot)) vscalbot = 1.0;
	if(!getparfloat("vscalzmx",&vscalzmx)) vscalzmx = 1.0;
	if(!getparfloat("dscal",&dscal)) dscal = 500.0;
	if(!getparint("check",&check)) check = 0;
	if(!getparint("iter",&iter)) iter = 1;
	if(!getparint("nc",&nc)) nc = 0;
	if(vscalbot!=1.0 || vscalzmx!=1.0) iscal = 1;
	if(ibot2==0) iscal = 0; 

	if(iscal==1) {
		one =1;
		scale1 = alloc3float(n2,n3,one);
		scale2 = alloc3float(n2,n3,one);
		for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {
			if(zbot2[i3*n2+i2]<zmax && zbot2[i3*n2+i2]!=0.) {
				scale1[0][i3][i2] = vscalbot;
				scale2[0][i3][i2] = vscalzmx;
			} else {
				scale1[0][i3][i2] = 1.0;
				scale2[0][i3][i2] = 1.0;
			}
		}
		}
		r1 = 0.;
        	r2 = (d2>0)?dscal/d2:0;
        	r3 = (d3>0)?dscal/d3:0;
        	r1 = 0.5*r1*r1 ;
        	r2 = 0.5*r2*r2 ;
        	r3 = 0.5*r3*r3 ;
		lam = 1.0;
		slowness=0;
		niter = 2;
		depth = 3;
		vsm3d(scale1,one,n3,n2,niter,depth,r1,r3,r2,lam,slowness);
		vsm3d(scale2,one,n3,n2,niter,depth,r1,r3,r2,lam,slowness);
	}


	efseek(infp,0,0);
        if(check==1) {
		cube= (float*) emalloc(n1*n2*n3*sizeof(float));
		vs = (float*) emalloc(8*sizeof(float));
	}

	for(i3=0;i3<n3;i3++) {
		for(i2=0;i2<n2;i2++) {

			efread(grid,sizeof(float),n1,infp);

			top = ztop[i3*n2+i2];
			bot = zbot[i3*n2+i2];
			top2 = ztop2[i3*n2+i2];
			bot2 = zbot2[i3*n2+i2];

			tmp = (top - o1)/d1 + 0.5; 
			i1top = tmp;
			tmp = (bot - o1)/d1 + 0.5; 
			i1bot = tmp;
			tmp = (top2 - o1)/d1 + 0.5; 
			i1top2 = tmp;
			tmp = (bot2 - o1)/d1 + 0.5; 
			i1bot2 = tmp;

			if(top==depthnull) i1top = n1;
			if(bot==depthnull) i1bot = 0;
			if(top2==depthnull) i1top2 = n1;
			if(bot2==depthnull) i1bot2 = 0;

/*
	fprintf(stderr, "top=%g bot=%g top2=%g bot2=%g line=%d trace=%d\n",
			top,bot,top2,bot2,(int)(i3*d3+o3),(int)(i2*d2+o2)); 
	fprintf(stderr, "i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n",
			i1top,i1bot,i1top2,i1bot2);
*/
			if(ivtop==1) {
				vt = vtop[i3*n2+i2];
			} else {
				if(i1top>0) {
					vt = grid[i1top];
				} else {
					vt = grid[0];
				}
			}
			if(ivbot2==1) {
				vb = vbot2[i3*n2+i2];
			} else {
				vb = vtop[i3*n2+i2];
			}


			/* salt between top and bot2 */
			if( (itop2==0 && ibot==0 && top<=bot2) || 
	    ( (i1top2==n1 || i1bot==0) && top<=bot2 && bot2!=depthnull) || 
			  (bot>=top2 && bot!=depthnull && top2!=depthnull) 
			  || (ibot2==1 && itop2==0 && ibot==0) ) {

/*
if(i2==381 && i3==169) fprintf(stderr," top to bot2 i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
if(i2==381 && i3==169) fprintf(stderr," top to bot2 top=%g bot=%g top2=%g bot2=%g \n", top,bot,top2,bot2);
*/

		if(i1top2==n1&&i1bot!=0&&i1bot2>i1bot) i1bot2=i1bot;  

	/*
	if(i1top>i1bot2)
	fprintf(stderr, "i1top=%d i1bot2=%d line=%d trace=%d\n",
			i1top,i1bot2,(int)(i3*d3+o3),(int)(i2*d2+o2)); 
	*/
				if(saltvelo!=-9898) {
					for(i1=i1top;i1<=i1bot2;i1++) {
						if(i1>=0 && i1<=n1-1) grid[i1] = saltvelo;
					}
				} else {
					for(i1=i1top;i1<=i1bot2;i1++) {
						if(i1>=0 && i1<=n1-1) {
							tmp = i1*d1+o1;
							tmp = tmp - top;
							if(bot2>top && tmp>0.) {
								grid[i1] = vt + tmp*(vb-vt)/(bot2-top);
							} else {
								grid[i1] = vt;
							}
						}
					}
				}
			/* overhangs */
			} else if(top<=bot && bot<=top2 && top2<=bot2) {
			/*
if(i2==178 && i3==271) fprintf(stderr," overhang i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
*/
				if(saltvelo!=-9898) {
					for(i1=i1top;i1<i1bot;i1++) {
						if(i1>=0 && i1<=n1-1) grid[i1] = saltvelo;
					}
					for(i1=i1top2;i1<=i1bot2;i1++) {
						if(i1<n1 && i1>=0) grid[i1] = saltvelo;
					}
					if(i1bot2<i1top2) i1bot2 = n1;
				} else {
					for(i1=i1top;i1<i1bot;i1++) {
						if(i1>=0 && i1<=n1-1) {
							tmp = i1*d1+o1;
							tmp = tmp - top;
							grid[i1] = vt + tmp * (vb-vt) / (bot2-top);
						}
					}
					for(i1=i1top2;i1<=i1bot2;i1++) {
						if(i1<n1 && i1>=0) {
							tmp = i1*d1+o1;
							tmp = tmp - top;
							grid[i1] = vt + tmp * (vb-vt) / (bot2-top);
						}
					}
					if(i1bot2<i1top2) i1bot2 = n1;
				}
			/* salt from top2 to bot2 */
			} else if(top2<=bot2 && bot<=top && bot2!=depthnull ) {
			/*
if(i2==178 && i3==271) fprintf(stderr," top2 to bot2 i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
*/
				if(saltvelo!=-9898) {
					for(i1=i1top2;i1<=i1bot2;i1++) {
						if(i1>=0 && i1<=n1-1) 
							grid[i1] = saltvelo;
					}
				} else {
					for(i1=i1top2;i1<=i1bot2;i1++) {
						if(i1>=0 && i1<=n1-1) {
							tmp = i1*d1+o1;
							tmp = tmp - top;
							grid[i1] = vt + tmp * (vb-vt) /(bot2-top);
						}
					}					
				}
			/* salt below top */
			} else if(top2<=bot) {
			/*
if(i2==178 && i3==271) fprintf(stderr," below top i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
*/
				if(i1bot2>i1bot || bot2==depthnull) i1bot2=i1bot;
				for(i1=i1top;i1<i1bot2;i1++) {
					if(i1<n1 && i1>=0) grid[i1] = saltvelo;
				}
				if(i1bot2<i1top) i1bot2 = n1;
			/* top to bot salt */
			} else if((top2<=top || top2>bot2) &&  top<= bot) {
			/*
if(i2==178 && i3==271) fprintf(stderr," top to bot i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
*/
				/*
				if(i1bot>i1bot2) i1bot=i1bot2;
				*/
				for(i1=i1top;i1<i1bot;i1++) {
					if(i1>=0 && i1<=n1-1) 
						grid[i1] = saltvelo;
				}
				i1bot2 = i1bot;	
				if(i1bot2<i1top) i1bot2 = n1;
			/* salt below top2 */
			} else if(bot<=top) {
			/*
if(i2==178 && i3==271) fprintf(stderr," below top2 i1top=%d i1bot=%d i1top2=%d i1bot2=%d \n", i1top,i1bot,i1top2,i1bot2);
*/
				if(i1top2<i1top) i1top2 = i1top;
				for(i1=i1top2;i1<=i1bot2;i1++) {
					if(i1>=0 && i1<n1) grid[i1] = saltvelo;
				}
				if(i1bot2<i1top2) i1bot2 = n1;
			}

			if(iscal==1) {
				r1 = scale1[0][i3][i2];
				r2 = scale2[0][i3][i2];
				tmp = r2 - r1;
				if((n1-1)>i1bot2) {
					tmp = tmp/(n1-1-i1bot2);
					for(i1=i1bot2;i1<n1;i1++)
						grid[i1] *=(r1+tmp*(i1-i1bot2));
				}
			}
			
			if(check==1) { 
				bcopy(grid,cube+(i3*n2+i2)*n1,
					n1*sizeof(float)); 
			} else {
				efwrite(grid,sizeof(float),n1,outfp);
				for(i1=0;i1<n1;i1++) {
					if(gmin>grid[i1]) gmin = grid[i1]; 
					if(gmax<grid[i1]) gmax = grid[i1]; 
				}
			}
		}
	}


	if(check==1) {
	  for(jter=0;jter<iter;jter++) {
	   for(i3=1;i3<n3-1;i3++) {
	      for(i2=1;i2<n2-1;i2++) {
	         for(i1=0;i1<n1;i1++) {
			v0 = cube[i1+i2*n1+i3*n1*n2];
			vs[0] = cube[i1+i2*n1+(i3-1)*n1*n2];
			vs[1] = cube[i1+i2*n1+(i3+1)*n1*n2];
			vs[2] = cube[i1+(i2-1)*n1+i3*n1*n2];
			vs[3] = cube[i1+(i2+1)*n1+i3*n1*n2];
			vs[4] = cube[i1+(i2-1)*n1+(i3-1)*n1*n2];
			vs[5] = cube[i1+(i2+1)*n1+(i3-1)*n1*n2];
			vs[6] = cube[i1+(i2-1)*n1+(i3+1)*n1*n2];
			vs[7] = cube[i1+(i2+1)*n1+(i3+1)*n1*n2];
			nsalt = 0;
			velo = 0.;
			for(ii=0;ii<8;ii++) {
				if(vs[ii]==saltvelo) {
					nsalt += 1;
				} else {
					velo = velo + vs[ii];
				}
			}
			if(nsalt<=nc && v0==saltvelo) {
				cube[i1+i2*n1+i3*n1*n2] = velo/(8.-nsalt);
			} else if(nsalt>=(8-nc) && v0!=saltvelo) {
				cube[i1+i2*n1+i3*n1*n2] = saltvelo;
			}	
		 }
	      }	
	   }	
          }
	   for(i3=0;i3<n3;i3++) {
	   	for(i2=0;i2<n2;i2++) {
			bcopy(cube+i3*n1*n2+i2*n1,grid,n1*sizeof(float));
			efwrite(grid,sizeof(float),n1,outfp);
			for(i1=0;i1<n1;i1++) {
				if(gmin>grid[i1]) gmin = grid[i1]; 
				if(gmax<grid[i1]) gmax = grid[i1]; 
			}
	   	}
	   }
	}

	usghin.gmin = gmin;
	usghin.gmax = gmax;

	ierr = fputusghdr(outfp,&usghin);
	
	free(ztop);
	free(zbot);
	free(ztop2);
	free(grid);
	if(iscal==1) free3float(scale1);	
	if(iscal==1) free3float(scale2);	
	if(check==1) free(cube);
	if(check==1) free(vs);
	exit(0);
}
