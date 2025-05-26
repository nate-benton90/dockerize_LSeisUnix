/* interval velocity to pore pressure (mud weight) calculation */

#include "velo.h"
#include "usgrid.h"
#include "subc.h"

char *sdoc = 
"VINT2MW - convert interval velocity to mud weight (lb/gal) \n"
"\n"
"vint2mw [parameters] <input >output 						  \n" 
"\n"
"Required parameters:						 	\n"
" input         interval velocity grid \n"
" output        mud weight grid \n"
"a=             parameter to be used to compute dtn: log(dtn)=a+b*z \n"
"b=             parameter to be used to compute dtn: log(dtn)=a+b*z \n"
"               dtn is the normal compaction sonic transit time \n"
"               a and b can be computed from dt2pp program \n"
"or \n"
"logdtn=        log(dtn) values at z \n"
"z=             depth positions where logdtn are specified \n"
"Optional parameters:  \n"
"deldt=         array of (dt-dtn)   (deldt1,deldt2,deldt3,...,deldtn)  \n"
"               dt is the observed sonic transit time  (1/vint) \n"
"mw=            array of mud weight (mw1,mw2,mw3,...,mwn) \n" 
"               if (deldt,mw) are given, they will be used map \n"
"               (dt-dtn) to mud weight; otherwise, the relation of GOM \n"
"               will be used.   \n"
"unit=0         unit of input velocity grid (0=ft/s 1=m/s)  \n"
"otype=0        output type (0=mud weight (lb/gal); \n"
"                            1=pressure gradient (psi/ft);  \n"
"                            2=pressure (psi);  \n"
"zmin=          minimum depth to start compute mud weight from velocity grid \n"
"               (above zmin, output mud weight is mwzmin) \n"
"               default to the minimum depth of input grid \n"
"mwzmin=        mud weight above zmin \n"
"               (if mwzmin no given, it will be set to mud weight at zmin at \n"
"                each cdp location when mwfz is not specified) \n"
"mwfz=          mud weight at the minimum depth (fz) of input velocity grid \n"
"               if specified and mwzmin is not given, the mud weight will \n"
"               be linearly interpolated from fz to zmin \n"
"jpfile=        name of the file to output job printout \n"
"               if not spicified, the print out will go to screen  \n" 
"\n "
"AUTHOR:		Zhiming Li,       ,	6/11/98   		\n"    
;

main(int argc, char **argv)
{
    	FILE *infp=stdin,*outfp=stdout;
		float *deldt, *mw, *dtn;
		float *del, *mwo, *vi;
		int *indx, nmw, unit, otype;
		float *z, *logdtn;
		float a, b;
		float zmin, mwzmin, mwfz, temp;
		int izmin;
		int nzread=0;
		char *jpfile;
		FILE *jpfp;

		float gmin, gmax;
		int n2, n3;
		usghed usgh;
		int ierr; 

		int i1, i2, i3;
		float fz, dz;
		int nz;

    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparfloat("a",&a) || !getparfloat("b",&b)) {
			nzread = countparval("logdtn");
			i1 = countparval("z");
			if(i1!=nzread) err(" check z and logdtn \n");
			if(nzread==0) err(" parameters a and b or logdtn and z missing");
			z = (float*) malloc(nzread*sizeof(float));
			logdtn = (float*) malloc(nzread*sizeof(float));
			getparfloat("z",z);
			getparfloat("logdtn",logdtn);
	    }
		nmw = 0;
		nmw = countparval("deldt");
		if(nmw != countparval("mw")) err(" check deldt and mw \n");
		if (nmw>0) {
			deldt = (float*) malloc(nmw*sizeof(float));
			mw = (float*) malloc(nmw*sizeof(float));
			getparfloat("deldt",deldt);
			getparfloat("mw",mw);
		} else {
			nmw = 14;
			deldt = (float*) malloc(nmw*sizeof(float));
			mw = (float*) malloc(nmw*sizeof(float));
			deldt[0] = 0; mw[0] = 8.7736;
			deldt[1] = 10; mw[1] = 12.8298;
			deldt[2] = 13; mw[2] = 13.7872;
			deldt[3] = 15; mw[3] = 14.3617;
			deldt[4] = 17; mw[4] = 14.9362;
			deldt[5] = 20; mw[5] = 15.5106;
			deldt[6] = 23; mw[6] = 16.1809;
			deldt[7] = 27; mw[7] = 16.7553;
			deldt[8] = 31; mw[8] = 17.1383;
			deldt[9] = 35; mw[9] = 17.5213;
			deldt[10] = 40; mw[10] = 17.8085;
			deldt[11] = 51; mw[11] = 18.4787;
			deldt[12] = 58; mw[12] = 18.7660;
			deldt[13] = 66; mw[13] = 19.1489;
		}
		if (!getparint("unit",&unit)) unit=0;
		if (!getparint("otype",&otype)) otype=0;
		if (!getparstring("jpfile",&jpfile)) {
			jpfp = stderr;
		} else {
			jpfp = fopen(jpfile,"w");
		}
		ierr = fgetusghdr(infp,&usgh);
		fz = usgh.o1;
		dz = usgh.d1;
		nz = usgh.n1;
		n2 = usgh.n2; if(n2==0) n2=1;
		n3 = usgh.n3; if(n3==0) n3=1;

		if(!getparfloat("zmin",&zmin)) zmin = fz;
		zmin = (zmin-fz)/dz+0.5;
		izmin = zmin;
		if(!getparfloat("mwzmin",&mwzmin)) mwzmin = 0.;
		if(!getparfloat("mwfz",&mwfz)) mwfz = 0.;

/* memory allocation */
    	dtn = (float*)malloc(nz*sizeof(float));
		vi = (float*) malloc(nz*sizeof(float));
		del = (float*) malloc(nz*sizeof(float));
		mwo = (float*) malloc(nz*sizeof(float));
		indx = (int*) malloc(nz*sizeof(int));

/* compute normal-compaction trend dtn */
		if(nzread==0) {
			for(i1=0;i1<nz;i1++)
				dtn[i1] = exp(a + b*(fz+i1*dz));
		} else {
			for(i1=0;i1<nz;i1++) {
				vi[i1] = fz + i1*dz;
			}
			bisear_(&nzread,&nz,z,vi,indx);
			linin_(&nzread,&nz,z,vi,indx,logdtn,dtn);
			for(i1=0;i1<nz;i1++) {
				dtn[i1] = exp(dtn[i1]);
			}
		}

/* convert vint to mw */
		efseek(infp,0,0);
		for(i3=0;i3<n3;i3++) {
			for(i2=0;i2<n2;i2++) {
				efread(vi,sizeof(float),nz,infp);
				if(unit==1) {
					for(i1=0;i1<nz;i1++) 
						vi[i1] = vi[i1] * 3.2804;
				}
				for(i1=0;i1<nz;i1++) {
					del[i1] = 1000000./vi[i1] - dtn[i1];
				}
				bisear_(&nmw,&nz,deldt,del,indx);
				linin_(&nmw,&nz,deldt,del,indx,mw,mwo);
				if(mwzmin>0.) { 
					for(i1=0;i1<izmin-1;i1++) {
						mwo[i1] = mwzmin;
					}
				} else if(mwfz>0.) {
					if(izmin>0) temp = (mwo[izmin] - mwfz)/izmin;
					for(i1=0;i1<izmin-1;i1++) {
						mwo[i1] = mwfz + i1*temp;
					}
				} else {
					for(i1=0;i1<izmin-1;i1++) {
						mwo[i1] = mwo[izmin];
					}
				}
				if(otype==1) {
					for(i1=0;i1<nz;i1++)
						mwo[i1] = mwo[i1] * 0.0522222;
				} else if(otype==2) {
					for(i1=0;i1<nz;i1++)
						mwo[i1] = mwo[i1]*0.0522222*(fz+i1*dz);
				}
				if(i2==0 && i3==0) { gmin=mwo[0]; gmax=mwo[0]; }
				for(i1=0;i1<nz;i1++) {
					if(gmin>mwo[i1]) gmin = mwo[i1];
					if(gmax<mwo[i1]) gmax = mwo[i1];
				}
				efwrite(mwo,sizeof(float),nz,outfp);
			}
		}
		usgh.gmin = gmin;
		usgh.gmax = gmax;
		ierr = fputusghdr(outfp,&usgh);
		if(ierr!=0) err(" output grid header error ");
		
		fprintf(jpfp,"   Normal-compaction dt trend: log(dtn)=a+b*z \n");
		if(nzread==0) {
			fprintf(jpfp,"    a=%f    b=%g  \n",a,b);
		} else {
			for(i1=0;i1<nzread;i1++)
			fprintf(jpfp,"    z = %f    logdtn=%f  \n",z[i1],logdtn[i1]);
		}
		fprintf(jpfp," \n");
		fprintf(jpfp,"       Depth          dtn     \n");
		fprintf(jpfp,"     --------------------------- \n");
		for(i1=0;i1<nz;i1++) {
			fprintf(jpfp,"     %10.3f    %10.3f\n",fz+i1*dz,dtn[i1]);
		}
		fprintf(jpfp," \n");
		fprintf(jpfp,"   Delta(dt) versus Mud Weight Relation \n");
		fprintf(jpfp,"   ------------------------------------ \n");
		for(i1=0;i1<nmw;i1++)
		fprintf(jpfp,"   delta(dt)=%10.4f us/ft      mw=%10.4f lb/gal \n", 
			deldt[i1],mw[i1]);
		fprintf(jpfp,"   ------------------------------------ \n");

     free(mwo);
     free(del);
     free(deldt);
     free(dtn);
     free(mw);
     free(vi);
     free(indx);

     return (0);
}
