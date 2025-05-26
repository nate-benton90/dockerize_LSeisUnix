/* sonic transit time dt to pore pressure calculation */

#include "velo.h"
#include "subc.h"

char *sdoc = 
"DT2PP - convert sonic transit time to pore pressure gradient \n"
"\n"
"dt2pp [parameters] <input >output 						  \n" 
"\n"
"Required parameters:						 	\n"
" input         file containing 2-columns (z, dt) data points \n"
"               where z is depth (ft) and dt is transit time (us/ft) \n"
" output        file containing (z,dt,dtn,mw,pg,p) \n"
"               where dtn is normal-compaction shale transit time (us/ft) \n"
"                     pg is pressure gradient (psi/ft) \n"
"                     mw is mud weight (lb/gal) \n"
"                     p is pore pressure (psi) \n"
"              \n" 
"\n"
"Optional parameters:						 	\n"
"nrmax=100000   maximum number of rows in the input file \n"
"zmin=          minimum z value to be used to derive dtn \n" 
"               (default to the minimum data value in the input) \n"
"zmax=          maximum z value to be used to derive dtn \n" 
"               (default to the maximum data value in the input) \n"
"a=             parameter to be used to compute dtn: log(dtn)=a+b*z \n"
"b=             parameter to be used to compute dtn: log(dtn)=a+b*z \n"
"               if a and b are specified, zmin and zmax are ignored. \n"
"               otherwise, data points (z,dt) within [zmin,zmax] will \n"
"               be used to compute a and b   \n"
"deldt=         array of (dt-dtn)   (deldt1,deldt2,deldt3,...,deldtn)  \n"
"mw=            array of mud weight (mw1,mw2,mw3,...,mwn) \n" 
"               if (deldt,mw) are given, they will be used map \n"
"               (dt-dtn) to mud weight; otherwise, the relation of GOM \n"
"               will be used.   \n"
"plot=1         1=create x-window graphs of pore pressure; \n"
"               -1=create paper plots of pore pressure; \n"
"               2=create x-window graphs of linear fit: log(dt)=a+b*z; \n"
"               -2=create paper plots of linear fit: log(dt)=a+b*z; \n"
"               0=no graphics   \n"
"well=          well name \n"
"Note:          when plot=1 zmin will be used to make the mud weight \n"
"               above zmin the same as that at zmin		\n"
"\n "
"AUTHOR:		Zhiming Li,       ,	6/11/98   		\n"    
;

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
		float *dtin, *zin, *xy;
		float *pg, *mwo, *p, *dtn;
		float *del;
		int *indx;
		float *z, *dt;
		float zmin, zmax;
		float a, b;
		int imin, imax, icom, nmw;
		float *deldt, *mw;
		int nrmax, plot;
		int ir, nr, i, n;
		char cmd[2048];
		char *well;
		FILE *cmpfp;
		float tmp, z0, zn;
		int itmp, ipos;


    	/* get parameters */
    	initargs(argc,argv);
   		askdoc(1);

		if (!getparint("nrmax",&nrmax)) nrmax=100000;
		imin = 1;
		if (!getparfloat("zmin",&zmin)) imin=0;
		imax = 1;
		if (!getparfloat("zmax",&zmax)) imax=0;
		if (!getparint("plot",&plot)) plot=1;
		icom = 1;
		if (getparfloat("a",&a) && getparfloat("b",&b)) icom=0;
		if (!getparstring("well",&well)) {
			well = (char*) malloc(5*sizeof(char));
			strcpy(well,"Well ");
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

/* memory allocation */
    	cbuf = (char*)malloc(134*sizeof(char));
    	dtin = (float*)malloc(nrmax*sizeof(float));
		zin = (float*) malloc(nrmax*sizeof(float));
    	dt = (float*)malloc(nrmax*sizeof(float));
		z = (float*) malloc(nrmax*sizeof(float));


		n = 0;
    	for (ir=0;ir<nrmax;ir++) {
       		if (feof(infp) !=0 ) break;
       		for(i=0;i<134;i++) cbuf[i]=' ';
       		gets(cbuf);
			sscanf(cbuf,"%f %f \n",&zin[ir],&dtin[ir]);
			if(icom==1) {
				if(imin==0 && imax==0) {
					z[n] = zin[ir];
					dt[n] = dtin[ir];
					n += 1;
				} else {
					if((imin==0 || zin[ir]>=zmin) && 
					   (imax==0 || zin[ir]<=zmax)) {
						z[n] = zin[ir];
						dt[n] = dtin[ir];
						n += 1;
					}	
				}
			}
		}
		nr = ir-1;

/* compute a and b:  log(dt) = a + b * z */
		if(icom==1) {
			for(i=0;i<n;i++) dt[i] = log(dt[i]);
			linefit(z,dt,n,&a,&b);
		}

		dtn = (float*) malloc(nr*sizeof(float));
		del = (float*) malloc(nr*sizeof(float));
		mwo = (float*) malloc(nr*sizeof(float));
		pg = (float*) malloc(nr*sizeof(float));
		p = (float*) malloc(nr*sizeof(float));
		indx = (int*) malloc(nr*sizeof(int));

/* compute normal-compaction trend dtn */
		for(i=0;i<nr;i++) {
			dtn[i] = exp(a + b*zin[i]);
			del[i] = dtin[i] - dtn[i];
		}

/* compute mw, pg and p */
		bisear_(&nmw,&nr,deldt,del,indx);
		linin_(&nmw,&nr,deldt,del,indx,mw,mwo);
		for(i=nr-2;i>=0;i--) {
			if(zin[i]<zmin) mwo[i] = mwo[i+1];
		}
		for(i=0;i<nr;i++) {
			pg[i] = mwo[i] * 0.0522222;
			p[i] = pg[i] * zin[i];
		}
		
		fprintf(stderr," Linear Fit log(dt)=a+bz of Data Points (z,dt) \n");
		fprintf(stderr," --------------------------------------------- \n");
		fprintf(stderr,"   Number of Input Data Points = %d \n", nr);
		fprintf(stderr,"   Number of Points in linefit = %d \n", n);
		fprintf(stderr,"   Computed value of a         = %f \n", a);
		fprintf(stderr,"   Computed value of b         = %g \n", b);
		fprintf(stderr," --------------------------------------------- \n");
		fprintf(stderr,"  \n");
		fprintf(stderr,"   Delta(dt) versus Mud Weight Relation \n");
		fprintf(stderr,"   ------------------------------------ \n");
		for(i=0;i<nmw;i++)
		fprintf(stderr,"   delta(dt)=%10.4f us/ft      mw=%10.4f lb/gal \n", 
			deldt[i],mw[i]);
		fprintf(stderr,"   ------------------------------------ \n");

		for(i=0;i<nr;i++) {
			fprintf(outfp,"%10.2f %10.4f %10.4f %10.4f %10.4f %10.4f\n", 
				zin[i],dtin[i],dtn[i],mwo[i],pg[i],p[i]);
		}

	
/* plots */
		if(plot==1 || plot==-1) {
			xy = (float*) malloc(2*nr*12*sizeof(float));

			/* plot z-mw */
			for(i=0;i<nr;i++) { 
				xy[i*2] = zin[i]; 
				xy[i*2+1] = mwo[i];
			}
			bzero(cmd,2048);
			if(plot==1) {
sprintf(cmd,"xgraph n=%d title=\"%s Mud Weight\" label1=\"depth (ft)\" label2=\"mud weight (lb/gal)\" grid1=solid grid2=solid nTic1=10 nTic2=10 ",nr,well);
			} else if (plot==-1) {
sprintf(cmd,"psgraph n=%d title=\"%s Mud Weight\" label1=\"depth (ft)\" label2=\"mud weight (lb/gal)\" grid1=solid grid2=solid n1tic=10 n2tic=10 style=seismic labelsize=14 titlesize=18 | lpr ",nr,well);
			}
			cmpfp = epopen(cmd,"w");
			efwrite(xy,sizeof(float),nr*2,cmpfp);
			efclose(cmpfp);

			/* plot z-p */
			for(i=0;i<nr;i++) { 
				xy[i*2] = zin[i]; 
				xy[i*2+1] = p[i]; 
			}
			bzero(cmd,2048);
			if(plot==1) {
sprintf(cmd,"xgraph n=%d title=\"%s Pore Pressure\" label1=\"depth (ft)\" label2=\"pressure (psi)\" grid1=solid grid2=solid nTic1=10 nTic2=10 ",nr,well);
			} else if(plot==-1) {
sprintf(cmd,"psgraph n=%d title=\"%s Pore Pressure\" label1=\"depth (ft)\" label2=\"pressure (psi)\" grid1=solid grid2=solid n1tic=10 n2tic=10 style=seismic labelsize=14 titlesize=18 | lpr ",nr,well);
			}
			cmpfp = epopen(cmd,"w");
			efwrite(xy,sizeof(float),nr*2,cmpfp);
			efclose(cmpfp);

			/* plot z-dt-dtn */
			tmp = mwo[0];
			for(i=0;i<nr;i++) if(mwo[i] >tmp) tmp = mwo[i]; 
			tmp = tmp - 9. + 1.5;
			itmp = tmp;
			for(i=0;i<itmp;i++) mwo[i] = 9. + i;
			bisear_(&nmw,&itmp,mw,mwo,indx);
			linin_(&nmw,&itmp,mw,mwo,indx,deldt,del);
			for(ir=0;ir<nr;ir++) { 
				xy[ir*2] = zin[ir]; 
				xy[ir*2+1] = dtin[ir]; 
			}
			for(i=0;i<itmp;i++) {
				for(ir=0;ir<nr;ir++) { 
					xy[ir*2+(i+1)*nr*2] = zin[ir]; 
					xy[ir*2+1+(i+1)*nr*2] = dtn[ir]+del[i];
				}
			}
			bzero(cmd,2048);
			tmp = nr;
			tmp = log10(tmp); 
			n = tmp + 1;
			if(plot==1) {
				sprintf(cmd,"xgraph n=");
				ipos=9;
			} else if(plot==-1) {
				sprintf(cmd,"psgraph n=");
				ipos=10;
			}
			for(i=0;i<itmp;i++) {
				sprintf(&cmd[ipos],"%d,",nr);
				ipos = ipos + n + 1;
			}
			sprintf(&cmd[ipos],"%d",nr);
			ipos = ipos + n;
			if(plot==1) {
sprintf(&cmd[ipos]," title=\"%s Mud Weight Scale (9,10,11,...) lb/gal\" label2=\"transit time (us/ft)\" label1=\"depth (ft)\" grid1=solid grid2=solid nTic1=10 nTic2=10 ",well);
			} else if(plot==-1) {
sprintf(&cmd[ipos]," title=\"%s Mud Weight Scale (9,10,11,...) lb/gal\" label2=\"transit time (us/ft)\" label1=\"depth (ft)\" grid1=solid grid2=solid n1tic=10 n2tic=10 style=seismic labelsize=14 titlesize=18 | lpr ",well);
			}
			cmpfp = epopen(cmd,"w");
			efwrite(xy,sizeof(float),nr*2*(itmp+1),cmpfp);
			efclose(cmpfp);
		} else if( plot==2 || plot==-2) {
			xy = (float*) malloc(2*(nr+2)*sizeof(float));
			z0 = zin[0];
			zn = zin[0];
			for(ir=0;ir<nr;ir++) { 
				xy[ir*2] = zin[ir]; 
				xy[ir*2+1] = log(dtin[ir]); 
				if(z0>zin[ir]) z0=zin[ir];
				if(zn<zin[ir]) zn=zin[ir];
			}
			xy[nr*2] = z0;
			xy[nr*2+1] = a+b*z0;
			xy[nr*2+2] = zn;
			xy[nr*2+3] = a+b*zn;
			bzero(cmd,2048);
			if(plot==2) {
sprintf(cmd,"xgraph n=%d,2 title=\"%s Linear Fit: log(dt)=%f+(%g)*z \" label1=\"depth (ft)\" label2=\"log(dt)\" grid1=solid grid2=solid nTic1=10 nTic2=10 marksize=4,0 linewidth=0,1 linecolor=2,4 ",nr,well,a,b);
			} else if(plot==-2) {
sprintf(cmd,"psgraph n=%d,2 title=\"%s Linear Fit: log(dt)=%f+(%g)*z \" label1=\"depth (ft)\" label2=\"log(dt)\" grid1=solid grid2=solid n1tic=10 n2tic=10 marksize=4,0 linewidth=0,1 style=seismic labelsize=14 titlesize=18 | lpr ",nr,well,a,b);
			}
			cmpfp = epopen(cmd,"w");
			efwrite(xy,sizeof(float),(nr+2)*2,cmpfp);
			efclose(cmpfp);

		}

     free(cbuf);
     free(zin);
     free(dtin);
     free(z);
     free(dtn);
     free(mwo);
     free(pg);
     free(p);
     free(z);
     free(dt);
     free(del);
     free(deldt);
     free(mw);
     free(indx);

     return (0);
}
