#include "su.h"
#include "segy.h"
#include "usgrid.h"

char *sdoc =
"SUTOSMUTE - apply top of salt mute to nmo-correted depth gathers      \n"
"\n"
"sutosmute [parameters] < input.data >muted.data                        \n"
"\n"
"Required parameters:                                                   \n"
" vigrid=       interval velocity grid v(z,trace,line)                   \n"
" tracekey=     segy key word for trace (crossline) number		\n"
" linekey=      segy key word for line number 				\n"
" saltvelo=     salt velocity 						\n"
" tosgrid=      top of salt depth grid z(trace,line)                   \n"
"\n"
"Optional parameters:                                                   \n"
"mutetaper=0    number of samples to taper the mute zones   		\n"
"option=1       1=apply mute and store mute depth in trace header 	\n"
"               0=store mute depth in trace header only, do not apply;	\n"
"fz=            depth of first sample			\n"
"               default to fz in the trace header if present \n" 
"               otherwise, default to delrt/1000 in the trace header \n" 
"dz=            sampling interval 			\n"
"               default to dz in the trace header if present  	\n" 
"               otherwise, default to dt/1000 in the trace header \n" 
"x0=0           minimum offset \n"
"z0=0           mute depth at minimum offset \n"
"xb=8000        maximum offset \n"
"zb=8000        mute depth at maximum offset \n"
"depthnull=0    value in tosgrid to indicate no top of salt 		\n"
"Notes:				\n"
"      1. Program calculates the offset xc where critical angle occurs    \n"
"         at the top of salt zc. Mute depth is calculated from (x0,z0),    \n"
"         (xc,zc) and (xb,zb) for given input offset, then applied and \n"
"         stores mute depth  in the trace header. \n"
"      2. Trace headers used: ns, abs(offset), fz, dz, delrt, dt, mute.\n" 
"      3. vigrid and tosgrid must have the same dimensions in \n"
"         number of traces and number of lines. 		\n"
"      4. o2 and d2 of gridheader of vigrid are first trace and \n"
"         trace number increment of vigrid. o3 and d3 of gridheader \n"
"         of vigrid are first line and line number increment of vigrid. \n"
"      5. o1 and d1 of gridheader of tosgrid are first trace and \n"
"         trace number increment of tosgrid. o2 and d2 of gridheader \n"
"         of tosgrid are first line and line number increment of tosgrid. \n"
"      6. When xc is less than x0, xc will be set to x0. \n"
"      7. Critical offset xc, for given top of salt depth zc, \n"
"         is calculated as,				\n"
"              xc = 2 * zc /(sqrt( vr*vr - 1.)		\n"
"         where vr = saltvelo/va(zc), and va(zc) is average velocity at zc. \n"
"      8. Mute depth calculation is linearly interpolated as follows  \n"
" \n"
"                        x0   xc          xb      offset              \n"
"              0 --------#----#-----------#-------------              \n" 
"                |       *                                            \n"
"                |       *                                            \n"
"                |  P    *        Z                                   \n"
"            z0  # - - - *                                            \n"
"                |        *                                           \n"
"                |         *        E                                 \n"
"                |          *                                         \n"
"                |     A     *                                        \n"
"            zc  # - - - - - -*       R                               \n"
"                |              *                                     \n"
"                |                *                                   \n"
"                |                  *    O                            \n"
"                |        S           *                               \n"
"                |                      *                             \n"
"            zb  # - - - - - - - - - - - -***********                 \n"
"                |                                  |                 \n"
"                |                                  |                 \n"
"                |           S                      |                 \n"
"          depth |                                  |                 \n"
"\n"
"Example: \n"
"      sutosmute < prsdm.with.nmo.gathers.su  \n"
"      vigrid=prsdm.vi.z.grid tracekey=ep linekey=fldr \n"
"      tosgrid=top.salt.z.grid saltvelo=14750 depthnull=0 \n"
"      x0=4000 z0=2850 xb=24000 zb=24000 \n" 
"      > tosmute.prsdm.gathers.su    \n"
"\n"
"AUTHOR:                Zhiming Li,       ,     8/21/02   \n"
"\n"
;
 
segy tr;

main(int argc, char **argv)
{

	char *tosgrid, *vigrid;
	float saltvelo;
	int mutetaper,mtend; 
	float o1,d1,o2,d2,o3,d3;
	int n1,n2,n3;
	float o1_tos,o2_tos,d1_tos,d2_tos;
	int n1_tos,n2_tos;
	float fz, dz;
	int option;

	String tracekey, linekey, trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;

	FILE *vifp, *tosfp;
	usghed usghvi,usghtos;
	FILE *infp=stdin, *outfp=stdout;

	float *taper, temp;
	float *vi, *tos;
	int i;
	int one=1;
	float xc, zc, r, vr;
	float x0, z0, xb, zb, depthnull, zmute, zmax;
	int imute;
	float xpre, ypre, x, y;
	int change, it, nt, ix, iy;
	float offset, z, *v, ts;
	int iz;
	int i1,i2,i3;


    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);


    	if (!getparstring("vigrid",&vigrid)) { 
		err("vigrid missing");
	} else {
		vifp = efopen(vigrid,"r");
		if(fgetusghdr(vifp,&usghvi)!=0) err("error fgetusghdr");
		o1 = usghvi.o1; o2 = usghvi.o2; o3 = usghvi.o3;
		d1 = usghvi.d1; d2 = usghvi.d2; d3 = usghvi.d3;
		n1 = usghvi.n1; n2 = usghvi.n2; n3 = usghvi.n3;
	}
    	if (!getparstring("tosgrid",&tosgrid)) {
		err("tosgrid missing");
	} else {
		tosfp = efopen(tosgrid,"r");
		if(fgetusghdr(tosfp,&usghtos)!=0) err("error fgetusghdr");
		o1_tos = usghtos.o1; o2_tos = usghtos.o2;
		d1_tos = usghtos.d1; d2_tos = usghtos.d2;
		n1_tos = usghtos.n1; n2_tos = usghtos.n2;
	}
	if(o1_tos!=o2) err(" o1 tosgrid and o2 vigrid different ");
	if(d1_tos!=d2) err(" d1 tosgrid and d2 vigrid different ");
	if(n1_tos!=n2) err(" n1 tosgrid and n2 vigrid different ");
	if(o2_tos!=o3) err(" o2 tosgrid and o3 vigrid different ");
	if(d2_tos!=d3) err(" d2 tosgrid and d3 vigrid different ");
	if(n2_tos!=n3) err(" n2 tosgrid and n3 vigrid different ");

	vi = (float*) malloc(n1*n2*n3*sizeof(float));
	v = (float*) malloc(n1*sizeof(float));
	tos = (float*) malloc(n2*n3*sizeof(float));
	fseek(vifp,0,0);
	efread(vi,sizeof(float),n1*n2*n3,vifp);
	fseek(tosfp,0,0);
	efread(tos,sizeof(float),n2*n3,tosfp);


    	if (!getparfloat("saltvelo",&saltvelo)) err(" saltvelo missing");

	if (!getparstring("tracekey",&tracekey)) err(" tracekey missing");
	if (!getparstring("linekey",&linekey)) err(" linekey missing");
	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);

    	if (!getparint("option",&option)) option=1;
    	if (!getparint("mutetaper",&mutetaper)) mutetaper=0;
    	if (!getparfloat("x0",&x0)) x0=0;
    	if (!getparfloat("z0",&z0)) z0=0;
    	if (!getparfloat("xb",&xb)) xb=8000;
    	if (!getparfloat("zb",&zb)) zb=8000;
    	if (!getparfloat("depthnull",&depthnull)) depthnull=0.;

    	/* Set up taper weights if tapering requested */
    	if (mutetaper>0) {
    		taper = (float*)malloc(mutetaper*sizeof(float));
        	for (i = 0; i < mutetaper; ++i) {
            		temp = sin((i+1)*3.141592654/(2.*mutetaper));
            		taper[i] = temp*temp;
        	}
    	}

	/* make file size to be able to exceed 2 G */
    	file2g(infp);
    	file2g(outfp);
	
	/* read in first trace */
    	if (!fgettr(infp,&tr))  err("can't get first trace");
    	if (!getparfloat("fz",&fz)) {
		fz = tr.fz;
		if(fz==0.) fz = tr.delrt/1000.;
	} else {
		if(fz!=tr.fz) warn(" fz and trace header fz different \n");
		if(fz!=tr.fz) warn(" fz=%f assumed \n",fz);
	}
    	if (!getparfloat("dz",&dz)) {
		dz = tr.dz;
		if(dz==0.) dz = tr.dt/1000.;
	} else {
		if(dz!=tr.dz) warn(" dz and trace header dz different \n");
		if(dz!=tr.dz) warn(" dz=%f assumed \n",dz);
	}


	fprintf(stderr," sutosmute parameters \n");
	fprintf(stderr," ==================== \n");
	fprintf(stderr," linekey=%s tracekey=%s \n",linekey,tracekey);
	fprintf(stderr," mutetaper=%d saltvelo=%f \n",mutetaper,saltvelo);
	fprintf(stderr," option=%d ",option);
	fprintf(stderr," fz=%f dz=%f \n",fz,dz);
	fprintf(stderr," x0=%f z0=%f \n",x0,z0);
	fprintf(stderr," xb=%f zb=%f \n",xb,zb);
	fprintf(stderr," depthnull=%f \n",depthnull);

    	nt = tr.ns;
	gethval(&tr,indxtrk,&trkval);
	ix = vtoi(trktype,trkval);
	gethval(&tr,indxlnk,&lnkval);
	iy = vtoi(lnktype,lnkval);
	xpre = ix - 1;
	ypre = iy - 1;

	zmax = (nt-1)*dz+fz;
	for(iz=0;iz<n2*n3;iz++) {
		if(fabs(tos[iz]-depthnull)<0.1) tos[iz] =  zmax;
	}

	/* compute average velocity */
	for(i3=0;i3<n3;i3++) {
	for(i2=0;i2<n2;i2++) {
		v[0] = vi[i2*n1+i3*n1*n2];
		for(i1=1;i1<n1;i1++) {
			temp = (o1+(i1-1)*d1)*v[i1-1]+d1*vi[i1+i2*n1+i3*n1*n2];
			v[i1] = temp/(o1+i1*d1);
		}
		for(i1=0;i1<n1;i1++) vi[i1+i2*n1+i3*n1*n2]=v[i1];
	}
	}

    	/* main loop over input traces */
    	do { 

        	offset = tr.offset;
		if (offset<0.) offset = - offset;

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
			bilint_(&n1,&n2,&n3,&o2,&o3,&d2,&d3,&x,&y,vi,v);
			bilint_(&one,&n2,&n3,&o2,&o3,&d2,&d3,&x,&y,tos,&ts);

			/* calculate critical offset at tos depth */
			z = (ts - o1)/d1;
			iz = z;
			if(iz>=0 && iz<=n1-1) { 
				vr = saltvelo/v[iz];
		/* fprintf(stderr," iz=%d vr=%f v=%f trace=%g line=%g\n",iz,vr,v[iz],x,y); */
				if(vr>1.) {
					r = sqrt(vr*vr-1.);
				} else {
					r = 0.;
				}
			} else {
				r = 0.;
			}
		/* fprintf(stderr," iz=%d r=%f trace=%g line=%g\n",iz,r,x,y); */

			if(r>0.) {
				xc = ts * 2. / r; 
				zc = ts;
			} else {
				xc = xb;
				zc = zb;
			}

		/* fprintf(stderr," xc=%f zc=%f trace=%g line=%g\n",xc,zc,x,y); */

			if(xc > xb) { 
				xc = xb - 1.;
				zc = zb - 1.;
			} else if (xc < x0) {
				xc = x0 + 1.;
			}

		}


		if(offset < x0) {
			zmute = z0;
		} else if ( (offset >=x0) && (offset <= xc) ) {
			zmute = z0 + (offset-x0)*(zc-z0)/(xc-x0);
		} else if ( (offset >=xc) && (offset <= xb) ) {
			zmute = zc + (offset-xc)*(zb-zc)/(xb-xc);
		} else {
			zmute = zb;
		}


		tr.mute = (int)zmute;

		zmute = (zmute - fz)/dz;
		imute = zmute;

		if(option==1) {
			/* apply zero mute */
			for(it=0;it<imute;it++) tr.data[it] = 0.;

			/* tapering */
			if(mutetaper>0) {
	   			mtend=((mutetaper+imute)<(int)tr.ns)?mutetaper:(int)tr.ns-imute;
	   			for(it=0;it<mtend;it++) 
					tr.data[it+imute] *= taper[it];
			}
		}

		fputtr(outfp,&tr);

	} while(fgettr(infp,&tr));

    if (mutetaper>0) free(taper);
    free(vi);
    free(tos);
    free(v);

    return 0;
}
