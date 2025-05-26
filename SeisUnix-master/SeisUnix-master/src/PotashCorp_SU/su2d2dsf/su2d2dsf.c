/* su2d2xs */
/* B.Nemeth */



#include "suhdr.h"
#define GNU_SOURCE

/*********************** self documentation *****************************/
char *sdoc[] = {" SU2D2DSF - su 2d line to Gocad surface with drapped seismic",
"                                                                       ",
"     su2d2dsf < stdin fname=                                            ",
"                                                                       ",
"     Required parameters:                                              ",
"                                                                       ",
"     fname=		Name of Gocad xsection object                   ",
"                                                                       ",
"     Optional parameters:                                              ",
"                                                                       ",
"     xh=gx             Trace header word of cdp x coordinate           ",
"     yh=gy             Trace header word of cdp y coordinate           ",
"     zh=gelev          Trace header word of cdp z coordinate           ",
"     datum=		If specified     it overrides tr.gelev          ",
"                                                                       ",
"     dtscale=1		Scale the dt field with this number             ",
"     bendian=0		1 if the code is running on bin endian machine  ",
"                                                                       ",
"                                                                       ",
"     Coordinate scaler word scalco is applied to  the coordinates      ",
"     according to the SEGY standard.                                   ",
"                                                                       ",
"     Sample interval has to be specified in millimeters,(              ",
"     1/1000 of a meter) and should be a negative number if the seismic ",  
"     is looking upwards.                                             ",
"                                                                       ",
"                                                                       ",
"                                                                       ",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */

int main( int argc, char *argv[] )
{
	/* Segy data constans */
        int ntr=0;                /* number of traces                     */
	
	double x;
	double y;
	double z;
	double x0=0.0,y0=0.0,z0=0.0;
	double xn=0.0,yn=0.0,zn=0.0;
	double ze=0.0;
	double datum;
	double xprev=0.0,yprev=0.0;
	double zt;
	double zt_mean=0.0;
	double zb;
	double cscale;
	double escale;
	double dtscale;
	int nvrtx=1;
	int itr;
	float dtr;		/* CDP spacing */
	double ddtr;		/* temporary storage for CDP spacing */
	int bendian;
        cwp_String fname;       /* name of output files                 */
        char fdn[BUFSIZ];       /* name of output files                 */
        char fhn[BUFSIZ];       /* name of output files                 */
        char fvn[BUFSIZ];       /* name of output files                 */
        
        FILE *hfp;
        FILE *dfp;
	FILE *tmpf;
        
	
	cwp_String xh;           /* header key word from segy.h          */
        cwp_String yh;           /* header key word from segy.h          */
        cwp_String zh;           /* header key word from segy.h          */
        cwp_String xtype;       /* .its type          */
        cwp_String ytype;       /* ..its type         */
        cwp_String ztype;       /* ..its type         */
        Value xval;              /* .its value        */
        Value yval;             /* ..its value        */
        Value zval;             /* ..its value        */
        int xindx;		/* ..its index */
        int yindx;		/* ..its index */
        int zindx;		/* ..its index */
	float tmp;
	
	

	initargs(argc, argv);
   	requestdoc(1);
        
        if (!getparint("bendian", &bendian)) bendian = 0;
	MUSTGETPARSTRING("fname",&fname);
        if (!getparstring("xh", &xh)) xh = "gx";
        if (!getparstring("yh", &yh)) yh = "gy";
        if (!getparstring("zh", &zh)) zh = "gelev";
        if (!getparfloat("dtr", &dtr)) dtr = 0.0;
        if (!getparfloat("datum", &tmp)) tmp = 1.0/0.0;
        datum = (double)tmp;
        if (!getparfloat("dtscale", &tmp)) tmp = 1.0;
        dtscale = (double)tmp;
	
	
	/* open files */
        strncpy(fdn,fname,MIN(strlen(fname),BUFSIZ));
        strncpy(fhn,fname,MIN(strlen(fname),BUFSIZ));
        strncpy(fvn,fname,MIN(strlen(fname),BUFSIZ));
        strcat(fdn,"_seismic@@");
        strcat(fhn,".ts");
        strcat(fvn,"_vo");
        dfp = efopen(fdn,"w");
        hfp = efopen(fhn,"w");
	tmpf = tmpfile();
        
	xtype = hdtype(xh);
        xindx = getindex(xh);
        ytype = hdtype(yh);
        yindx = getindex(yh);
        ztype = hdtype(zh);
        zindx = getindex(zh);
 	
        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");

	fprintf(hfp,"GOCAD Tsurf 1\n");
	fprintf(hfp,"HEADER {\n");
	fprintf(hfp,"name:%s\n",fname);
	fprintf(hfp,"solid:true\n");
	fprintf(hfp,"*draping:on\n");
	fprintf(hfp,"*map:true\n");
	fprintf(hfp,"*map*precise:true\n");
	fprintf(hfp,"*map*smoothed:true\n");
	fprintf(hfp,"*map*parameters:%s seismic 2 0\n",fvn);
	fprintf(hfp,"*map*uprop:U\n");
	fprintf(hfp,"*map*vprop:V\n");
	fprintf(hfp,"*map*use_uv:true\n");
	fprintf(hfp,"}\n");
	fprintf(hfp,"PROPERTIES U V\n");
	fprintf(hfp,"PROPERTY_CLASSES u v\n");
	fprintf(hfp,"ESIZES 1  1\n");
	fprintf(hfp,"UNITS none none\n");
	fprintf(hfp,"TFACE\n");
	
        cscale=pow((double)10.0,(double)tr.scalco);
        gethval(&tr, xindx, &xval);
        gethval(&tr, yindx, &yval);
        gethval(&tr, zindx, &zval);
	x0 = vtod(xtype,xval)*cscale;
	y0 = vtod(ytype,yval)*cscale;
	if(!finite(datum)) {
		z0 = -tr.delrt/1000.0;
	} else {
		z0 = datum;
	}
	ze = -tr.ns*tr.dt/1000.0*dtscale;
	xprev = x0;
	yprev = y0;
	
	ddtr=0.0;
	
	do {
                cscale=pow((double)10.0,(double)tr.scalco);
                escale=pow((double)10.0,(double)tr.scalel);
        	gethval(&tr, xindx, &xval);
        	gethval(&tr, yindx, &yval);
        	gethval(&tr, zindx, &zval);
		x = vtod(xtype,xval)*cscale;
		y = vtod(ytype,yval)*cscale;
		z = vtod(ztype,zval)*escale-(double)tr.f1;
		
		/* Compute trace spacing */
		if(dtr==0.0) {
			ddtr += (double) distance(x,y,xprev,yprev);
		}
		
		if(x==xprev && y==yprev) warn("Same traces coordinates have been detected %s\n",fname);
		xprev=x; yprev=y;
		
		
		ntr++;
		/* Write seismic data into to a temporary file */
		fputtr(tmpf,&tr);

		
	} while(gettr(&tr));
	
	dtr = (double)ddtr/(double)(ntr-1);
	
	fprintf(stderr," Trace spacing %f \n", dtr);
	
	/* X0,Y0,Z0 coordinates of the first sample of the first trace */
	/* XN,YN,ZN coordinates of the first sample of the last trace */

	xn=x;
	yn=y;
	zn=zt;
	
	rewind(tmpf);
	
	
	
	
	/* Do the binnig
	   We want to project to the surface with a direction prepedicular to the 
	   xo,yo xn,yn line */
	/* To do it properly the voxet needs to be re-binned */
	
	
	{
	int it;
	float dv;
	float v=0.0;
		
		
		dv = 1.0/(ntr-1);
		
        	
		if (!fgettr(tmpf,&tr)) err("can't get first trace");
                
		do {
                	cscale=pow((double)10.0,(double)tr.scalco);
                	escale=pow((double)10.0,(double)tr.scalel);
        		gethval(&tr, xindx, &xval);
        		gethval(&tr, yindx, &yval);
        		gethval(&tr, zindx, &zval);
			x = vtod(xtype,xval)*cscale;
			y = vtod(ytype,yval)*cscale;
			z = vtod(ztype,zval)*escale-(double)tr.f1;
			
			/*fprintf(stderr," %f \n",(double)tr.delrt); */
			if(!finite(datum)) {
				zt = z;
			} else {
				zt = datum;
			}
			zb = zt-(double)(tr.ns-1)*(double)tr.dt/1000.0*dtscale;
			
			zt_mean+=zt;  
		
			fprintf(hfp,"PVRTX %d %12.3f %12.3f %12.3f %12.8f %12.8f\n",
				 nvrtx, x, y, zt, 0.0,v);  
			fprintf(hfp,"PVRTX %d %12.3f %12.3f %12.3f %12.8f %12.8f\n",
				 nvrtx+1, x, y, zb, 1.0,v); 

			if(!bendian) 
                                for(it=0;it<tr.ns;it++) swap_float_4(&tr.data[it]);
			
			efwrite(tr.data,sizeof(float),tr.ns,dfp);

			/* fprintf(stdout," %f %f\n",v,v); */
			nvrtx+=2;
			v += dv;
			
		} while(fgettr(tmpf,&tr));
		
	}
                
	zt_mean /=ntr; 
	

	{ int top,bot;
		top = 1;
		bot = 2;
		for(itr=0;itr<ntr-1;itr++) {
			
			fprintf(hfp,"TRGL %d %d %d\n", top,top+1,top+2);
			fprintf(hfp,"TRGL %d %d %d\n", bot,bot+1,bot+2);
			top +=2;
			bot +=2;
		}
		
	}
	
	fprintf(hfp,"\n\n");
	fprintf(hfp,"END\n\n");
	
 	fprintf(hfp,"GOCAD Voxet 1\n");
 	fprintf(hfp,"HEADER {\n");
 	fprintf(hfp,"name:%s_vo\n",fname);
 	fprintf(hfp,"*cage:false\n");
 	fprintf(hfp,"*seismic*smoothed:false\n");
 	fprintf(hfp,"*painted:off\n");
 	fprintf(hfp,"*painted*variable:seismic\n");
 	fprintf(hfp,"}\n");
	fprintf(hfp,"AXIS_O %10.3f %10.3f %10.3f\n", x0,y0,zt_mean);
 	fprintf(hfp,"AXIS_U %10.3f %10.3f %10.3f\n",0.0,0.0,ze/tr.ns);
 	fprintf(hfp,"AXIS_V %10.3f %10.3f %10.3f\n",(xn-x0)/ntr,(yn-y0)/ntr,0.0);
 	fprintf(hfp,"AXIS_W %10.3f %10.3f %10.3f\n",-(yn-y0)/ntr,-(xn-x0)/ntr,0.0);
 	fprintf(hfp,"AXIS_N %d %d 1\n",tr.ns,ntr);
 	fprintf(hfp,"AXIS_MIN 0. 0. 0.\n");
 	fprintf(hfp,"AXIS_MAX %d %d 1.\n",tr.ns-1,ntr-1);
 	fprintf(hfp,"AXIS_NAME \" axis-1\" \" axis-2\" \" axis-3\"\n");
 	fprintf(hfp,"AXIS_UNIT none none none\n");
 	fprintf(hfp,"AXIS_TYPE even even even\n");
 
 	/* Propert y block */
	fprintf(hfp,"PROPERTY 1 \"seismic\"\n");
	fprintf(hfp,"PROPERTY_CLASS 1 \"seismic2D\"\n");
	fprintf(hfp,"PROPERTY_CLASS_HEADER 1 \"seismic2D\" {\n");
 	fprintf(hfp,"*colormap:gray\n");
 	fprintf(hfp,"*colormap*reverse:true\n");
 	fprintf(hfp,"}\n");
	fprintf(hfp,"PROP_UNIT 1 none\n");
	fprintf(hfp,"PROP_ESIZE 1 4\n");
 	fprintf(hfp,"PROP_SIGNED 1 0\n");
 	fprintf(hfp,"PROP_ETYPE 1  IEEE\n");
 	fprintf(hfp,"PROP_FORMAT 1  RAW\n");
 	fprintf(hfp,"PROP_OFFSET 1 0\n");
	fprintf(hfp,"PROP_FILE 1 %s\n",fdn);
	fprintf(hfp,"END\n");

	/* Close the files */
        efclose(dfp);
        efclose(hfp);
	fclose(tmpf);   
	
	fprintf(stderr," Trace# %d \n",ntr);

	return EXIT_SUCCESS;
}
