#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUSLICE - Slice through a 3d Cube of data along a given surface	\n"
"\n"
"suslice <stdin >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"                           (can also be specified as datain=stdin,      \n"
"                           instead of <stdin)				\n"
"stdout                     Name of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"sfgrid=                    Name of the surface grid 			\n"
"                           (stored as (x,y) order with grid header)	\n" 
"nx=                        number of cdp per line in the input data	\n"
"ny=                        number of lines in the input data		\n"
"ox=                        starting inline x coordinate		\n"
"oy=                        starting crossline y coordinate		\n"
"dx=                        increment of inline x coordinate		\n"
"dy=                        increment of crossline y coordinate		\n"
"Optional Parameters:\n"
"op=0                       operation to extract data			\n"
"                           0=amplitude (with sign)			\n"
"                           1=absolute amplitude			\n"
"                           2=rms amplitude                   		\n"
"                           3=medium of amplitude (with sign)		\n"
"                           4=medium of absolute amplitude 		\n"
"lwin=1                     window length in samples to compute 	\n"
"                           desired output				\n"
"a=0                        constant shift in ms or m (or ft) applied to \n"
"                           the t value read from sfgrid		\n"
"b=1                        scale to be applied to the t value read from \n"
"                           the surface grid				\n"
"track=0                    track nearest peak/trough			\n"
"                           0=do not track 				\n"
"                           1=track the nearest peak			\n"
"                          -1=track the nearest trough			\n"
"                           2=track the nearest peak/trough		\n"
"                             depending sign of the data value		\n"
"ltrack=1                   track window length in samples 		\n"
"near=0                     take the nearest sample			\n"
"                           1=yes 					\n"
"                           0=no; will compute				\n"
"NOtes:									\n"
" 1. the output value will be computed from the following:		\n"
"                							\n"
"                	 j=+twin/2					\n"
"                o(x,y) = Window  ( op( i(ti+j,x,y) ) )			\n"  
"                        j=-twin/2 					\n"
"    where,								\n"
"               ti = a + t(x,y) * b					\n"
"               t(x,y) is the time to compute the desired output defined \n"
"               in the sfgrid				\n"
"2. the input must be padded so that each line will have nx traces	\n"	
"3. the output is a grid file with nx by ny floating points plus 100 	\n"
"   bytes of grid header						\n"
"4. ox,oy,dx,dy are in the same units as those defined in the grid header \n"
"   of sfgrid file: o1,o2,d1,d2						\n"
"5. if a trace location is outside the grid, constant time from the edge \n"
"   in the sfgrid will be used						\n" 
"  	\n" 
"\n"
"Author:	Zhiming Li		      		8-9-94		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

#ifdef __convex__
	#define file2g(x) fseek64(x,0,1);
#else
	#define file2g(x) fseek(x,0,1);
#endif


main(int argc, char **argv)
{
	int nt;		
	float dt;	
	float ft;	
	int it;		
	FILE *sffp;
	char *sfgrid, *datain, *dataout;
	FILE *infp,*outfp;
	int iy, ix, near;
	int i1, i2;
	float ox, oy, dx, dy, x, y;
	int nx, ny;
	float a, b;
	int op, lwin, lh, it1, it2;
	float *oxy, *data, tmp, t, ti, *txy;
	int iti, itj, itrace, nti, nh;
	int track, ltrack, lth;
	int it0, itp, itn;
	float tt;
	

	int n1,n2,n3,n4,n5;
        float o1,o2,o3,o4,o5,d1,d2,d3,d4,d5;
        float scale, ocdp2, oline3, dcdp2, dline3;
	float vmin, vmax;
        int dtype,ierr,orient=0,gtype=0;
	ghed gh;



	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("sfgrid",&sfgrid)) err(" must specify sfgrid ");
	if (!getparint("nx",&nx)) err(" must specify nx");
	if (!getparint("ny",&ny)) err(" must specify ny");
	if (!getparfloat("dx",&dx)) err(" must specify dx");
	if (!getparfloat("dy",&dy)) err(" must specify dy");
	if (!getparfloat("ox",&ox)) err(" must specify ox");
	if (!getparfloat("oy",&oy)) err(" must specify oy");
	if (!getparint("op",&op)) op = 0;
	if (!getparint("near",&near)) near = 0;

	/* get optional parameters */
	if (!getparfloat("a",&a)) a = 0.;
	if (!getparfloat("b",&b)) b = 1.;
	if (!getparint("lwin",&lwin)) lwin = 1;
	if (!getparint("track",&track)) track = 0;
	if (!getparint("ltrack",&ltrack)) ltrack = 1;
	lth = ltrack/2;
	if(ltrack==1 && track!=0)
		warn(" track set to 0 because of ltrack=1 \n");

	/* get optional parameters */
	if (!getparfloat("a",&a)) a = 0.;
	if (!getparfloat("b",&b)) b = 1.;
	if (!getparint("lwin",&lwin)) lwin = 1;
	if (!getparint("track",&track)) track = 0;
	if (!getparint("ltrack",&ltrack)) ltrack = 1;
	lth = ltrack/2;
	if(ltrack==1 && track!=0) {
		warn(" track set to 0 because of ltrack=1 \n");
		track = 0;
	}
	lwin = lwin/2*2+1;
	if(lwin<0) lwin = 1;

	lh = lwin / 2;

	if((sffp = fopen(sfgrid,"r"))==NULL)
             	err("Input sfgrid file %s not found \n",sfgrid);
	/* obtain grid header info */
	ierr = fgetghdr(sffp, &gh);
	if(ierr==0) {
		fromghdr(&gh,&scale,&dtype,
			&n1,&n2,&n3,&n4,&n5,
                      	&d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                       	&dcdp2,&dline3,&ocdp2,&oline3,
			&vmin,&vmax,&orient,&gtype);
	} else {
		err(" non standard sfgrid file ");
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
	dt = (float)tr.dt/1000.0;
	ft = tr.delrt;
	
	/* allocate workspace */
	txy = (float*) emalloc(n1*n2*sizeof(float));
	data = (float*) emalloc(lwin*sizeof(float));
	oxy = (float*) emalloc(nx*ny*sizeof(float));

	efread(txy, sizeof(float), n1*n2, sffp);

	itrace = 0;

	do {

		iy = itrace/nx;
		ix = itrace - iy * nx;
		x = ox + ix*dx;
		y = oy + iy*dy;
		x = (x - o1)/d1 + 0.5;
		y = (y - o2)/d2 + 0.5 ;
		i1 = x;
		i2 = y;
		if(i2<0) i2=0;
		if(i2>n2-1) i2=n2-1;
		if(i1<0) i1=0;
		if(i1>n1-1) i1=n1-1;

		t = txy[i2*n1+i1]*b + a;
		t = (t - ft)/dt;
		it = t;

		if(near==1) {
		 	it = t;
			t = it;
		}

		if(track==1 || (track==2 && tr.data[it]>0.)) {
			for(it=0;it<lth;it++) {
				/* search down */
				tt = t + it;
				itn = tt - 1;
				it0 = tt;
				itp = tt + 1;
				if(it0>0 && it0<it-1) {
					if(tr.data[it0]>=tr.data[itn] &&
					   tr.data[it0]>=tr.data[itp]) {
						t = it0;
						break;
					}
				}
				/* search up */
				tt = t - it;
				itn = tt - 1;
				it0 = tt;
				itp = tt + 1;
				if(it0>0 && it0<nt-1) {
					if(tr.data[it0]>=tr.data[itn] &&
					   tr.data[it0]>=tr.data[itp]) {
						t = it0;
						break;
					}
				}
			}
		} else if(track==-1 || (track==2 && tr.data[it]<0.)) {
			for(it=0;it<lth;it++) {
				/* search down */
				tt = t + it;
				itn = tt - 1;
				it0 = tt;
				itp = tt + 1;
				if(it0>0 && it0<nt-1) {
					if(tr.data[it0]<=tr.data[itn] &&
					   tr.data[it0]<=tr.data[itp]) {
						t = it0;
						break;
					}
				}
				/* search up */
				tt = t - it;
				itn = tt - 1;
				it0 = tt;
				itp = tt + 1;
				if(it0>0 && it0<it-1) {
					if(tr.data[it0]<=tr.data[itn] &&
					   tr.data[it0]<=tr.data[itp]) {
						t = it0;
						break;
					}
				}
			}
		}

		bzero(data,lwin*sizeof(float));
		nti = 0;

		for(it=-lh;it<=lh;it++) {
			ti = t + it; 
			iti = ti;
			itj = it+lh;
			if(iti>=0 && iti<nt-1) {
				tmp = ti - iti;
				data[nti] = tr.data[iti]*(1.-tmp) + 
						tr.data[iti+1]*tmp;
				nti = nti + 1;
			}
		}

		tmp = 0.;
		if(op==1 || op==4) {
			for(it=0;it<nti;it++) data[it] = fabs(data[it]);
		} else if(op==2) {
			for(it=0;it<nti;it++) data[it] =data[it]*data[it];
		}
		if(op==0 || op==1 || op==2 ) {
			for(it=0;it<nti;it++) {
				tmp += data[it]; 
			}
			if(nti>1.) tmp  = tmp / nti;
			if(op==2 && tmp>0.) tmp = sqrt(tmp);
		} else {
			nh = nti/2;
			qkfind(nh,nti,data);
			tmp = data[nh];
		}	
		oxy[ix+iy*nx] = tmp; 
		itrace = itrace + 1;

	} while (fgettr(infp,&tr));


	vmin = oxy[0];
	vmax = oxy[0];
	for(ix=0;ix<nx*ny;ix++) {
		if(vmin>oxy[ix]) vmin = oxy[ix];
		if(vmax<oxy[ix]) vmax = oxy[ix];
	}

			
	efwrite(oxy,sizeof(float),nx*ny,outfp);
	scale = 1.0e-6;
	dtype = 4;
	n1 = nx;
	o1 = ox;
	d1 = dx;
	n2 = ny;
	o2 = oy;
	d2 = dy;
	n3 = 1;
	o3 = 0;
	d3 = 1;
	n4 = 1;
	o4 = 0;
	d4 = 0;
	n5 = 1;
	o5 = 0;
	d5 = 0;
	ocdp2 = 0;
	dcdp2 = 0;
	oline3 = 0;
	dline3 = 0;
	orient = 0;
	gtype = 0;
		
	toghdr(&gh,&scale,&dtype,
		&n1,&n2,&n3,&n4,&n5,
                &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                &dcdp2,&dline3,&ocdp2,&oline3,
		&vmin,&vmax,&orient,&gtype);

	ierr = fputghdr(outfp, &gh);
	if(ierr!=0) warn(" output header error");
	

	return EXIT_SUCCESS;
}
