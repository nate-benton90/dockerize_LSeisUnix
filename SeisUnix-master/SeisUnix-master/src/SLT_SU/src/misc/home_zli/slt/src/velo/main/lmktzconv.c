/* LMKTZCONV program */
#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"LMKTZCONV - Landmark time-depth convertion program			\n"
"\n"
"lmktzconv <landmark.in.file vgrid= [parameters] >landmark.out.file	\n" 
"\n"
"Required parameters:							\n"
"landmark.in.file=  name of Landmark input file (time or depth)		\n"
"landmark.out.file= name of Landmark output file (depth or time)	\n"
"vgrid=             name of velocity grid 				\n"
"Optional parameters:							\n"
"xlpos=2            column position of landmark crossline number	\n"
"slpos=1            column position of landmark line number 		\n"
"tzpos=5            column position of landmark time or depth  		\n"
"maxp=250000        maximum number of rows in the landmark pick file	\n"
"torz=0             type of input landmark picks 			\n"
"                   (0=time   output will be depth			\n"
"                    1=depth  output will be time)			\n"
"scalei=0.001       scale to be applied to time or depth values of      \n"
"                   input landmark picks so that they will agree with	\n"
"                   time (s) or depth (m or ft) units of velocity grid	\n"
"                   (when input is time, scalei should be 0.001 to 	\n"
"                   convert ms to s, since velocity grid is usually	\n"
"                   ft/s or m/s; when input is depth, it can also be 	\n"
"                   0.3048 to convert picks from ft to m, if velocity 	\n"
"                   grid is in m/s.					\n"
"scaleo=1.000       scale to be applied to depth or time values of 	\n"
"                   output landmark picks. 				\n"
"                   (when output is dpeth, scaleo should be 3.28084 to	\n"
"                   output depth in ft, if velocity is m/s; when output	\n"
"                   is time, scaleo should be 1000 to output time in	\n"
"                   ms, if velocity is m/s or ft/s)  			\n"
"o1=                minimum time in ms of velocity grid 		\n"
"d1=                time sampling interval in ms of velocity grid	\n" 
"ocdp2=             minimum crossline number of velocity grid 		\n"
"dcdp2=             crossline number increment of velocity grid		\n"
"oline3=            minimum line number of velocity grid		\n"
"dline3=            line number increment of velocity grid 		\n"
"gtype=2            velocity grid type (1=time-rms 2=time-average 	\n"
"                   3=time-interval)					\n"
"\n"
"                   The above seven parameters default to grid header	\n"
"                   of input vgrid; use gridheader to print or update	\n"
"                   the input velocity grid if needed; when supplied	\n"
"                   they will overwrite the values in the velocity grid	\n"
"NOTES:						 			\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	11/24/94   		\n"
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout, *vfp;
	string vgrid; 

	int maxp=250000,xlpos=2,slpos=1,tzpos=5,torz=0;
	float ocdp2, dcdp2, oline3, dline3, o1, d1;
	int gtype;
	float xl,sl,tz,zt;
	usghed usgh;
	int n1, n2, n3, i1;

	int ierr;
	float *vs,*t,*v,*va,*z,vaa;
	float *fbuf;
	char *cbuf;
	int itmp,one=1,indx,nc,nf;
	float tmp;
	int ittype,ottype,ivtype,ovtype;
	float scaleo, scalei, vn;
	int ip;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* get input parameters */
	if( !getparstring("vgrid",&vgrid) ) err("vgrid missing");
	vfp = efopen(vgrid,"r");
	ierr = fgetusghdr(vfp, &usgh);
	if(ierr!=0) err("error get grid header ");

	if( !getparfloat("scaleo",&scaleo) ) scaleo = 1.0;
	if( !getparfloat("scalei",&scalei) ) scalei = 0.001;
	
	if( !getparfloat("o1",&o1) ) o1 = usgh.o1;
	if( !getparfloat("d1",&d1) ) d1 = usgh.d1;
	if( !getparfloat("ocdp2",&ocdp2) ) ocdp2 = usgh.ocdp2;
	if( !getparfloat("dcdp2",&dcdp2) ) dcdp2 = usgh.dcdp2;
	if( !getparfloat("oline3",&oline3) ) oline3 = usgh.oline3;
	if( !getparfloat("dline3",&dline3) ) dline3 = usgh.dline3;
	if( !getparint("gtype",&gtype) ) gtype = usgh.gtype;
	if(gtype!=1 && gtype!=2 && gtype!=3) 
		err("invalid grid type");
	if(dcdp2==0) err("dcdp2 equals 0");
	if(dline3==0) err("dline3 equals 0");

	if( !getparint("xlpos",&xlpos) ) xlpos=2; xlpos -= 1;
	if( !getparint("slpos",&slpos) ) slpos=1; slpos -= 1;
	if( !getparint("tzpos",&tzpos) ) tzpos=5; tzpos -= 1;
	if( !getparint("maxp",&maxp) ) maxp=250000;
	if( !getparint("torz",&torz) ) torz=0;
	o1 = o1 * 0.001;
	d1 = d1 * 0.001;

	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3;
	nc = 200;
	nf = 10;

	/* memory allocations */
        vs = (float*)emalloc(n1*n2*n3*sizeof(float));
        v = (float*)emalloc(n1*sizeof(float));
        t = (float*)emalloc(n1*sizeof(float));
        va = (float*)emalloc(n1*sizeof(float));
        z = (float*)emalloc(n1*sizeof(float));
        fbuf = (float *) malloc(nf*sizeof(float));
        cbuf = (char *) emalloc(nc*sizeof(char));
	for(i1=0;i1<n1;i1++) t[i1] = o1+i1*d1;
	ivtype = gtype -1; 
	ovtype = 1;
	ittype = 0;
	ottype = ittype;

	efseek(vfp,0,0);
	efread(vs,sizeof(float),n1*n2*n3,vfp);

        fgets(cbuf,nc,infp);
        do {
		sscanf(cbuf,"%f %f %f %f %f",
			&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],&fbuf[4]);
		xl = fbuf[xlpos];
		sl = fbuf[slpos];
		tz = fbuf[tzpos]*scalei;
		
		bilint_(&n1,&n2,&n3,&ocdp2,&oline3,&dcdp2,&dline3,
			  &xl,&sl,vs,v);
		if(ivtype==ovtype) {
			for(i1=0;i1<n1;i1++) va[i1] = v[i1];
		} else {
			vconvert(t,v,n1,ivtype,ittype,t,va,n1,ovtype,ottype);
		}

		if(torz==0) {
			tmp  = (tz - o1)/d1;
			itmp = tmp;
		} else {
			for(i1=0;i1<n1;i1++) z[i1] = t[i1]*va[i1]*0.5;
			bisear_(&n1,&one,z,&tz,&itmp);
			itmp = itmp - 1;
			tmp = tz - z[itmp]; 
		}	

		
		if(itmp<0 || n1==1 || tz==0.) {
			vaa = va[0];
		} else if(itmp>=n1-1) {
			vn = t[n1-1]*va[n1-1]-va[n1-2]*t[n1-2]; 
			vn = vn/d1;
			vaa = va[n1-1]*t[n1-1]+vn*(tz-t[n1-1]);
			vaa = vaa/tz;
		} else {
			if(torz==1) tmp = tmp/(z[itmp+1]-z[itmp]) + itmp;
			vaa=va[itmp]+(va[itmp+1]-va[itmp])*(tmp-itmp);
		} 
		if(torz==0) {
			zt = tz*vaa*0.5*scaleo;
		} else {
			zt = tz/vaa*2.0*scaleo;
		}

		fbuf[tzpos] = zt;
		fprintf(outfp, 
		"                    %10.2f%10.2f%12.2f%12.2f%12.4f   \n",
                        fbuf[0],fbuf[1],fbuf[2],fbuf[3],fbuf[4]);
                bzero(cbuf,nc);
	} while(fgets(cbuf,nc,infp));
		
	free(cbuf);
	free(fbuf);
	free(vs);
	free(v);
	free(va);
	free(t);
	free(z);

	exit(0);
}
