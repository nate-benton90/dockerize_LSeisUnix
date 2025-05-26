
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUPICK  - auto pick of gun delayed traces  				\n" 
"\n"
"supick [parameters] <input-data list=  		\n" 
"\n"
"Required parameters:							\n"
"input-data             input data of common pkey sorted 		\n"
"list=                  output file to print pkey and skey of picked traces \n"
"pkey=                  primary key to perform auto pick (gather type)	\n"
"skey=                  secondary key besides pkey to output 		\n"
"                       only for those traces picked			\n"
"tmin=                  minimum time (ms) to search     		\n"
"tmax=                  maximum time (ms) to search                     \n"
"ttolmin=               min tolerance (ms) of picks from averge time of	\n"
"                       the maximum peak over the gather		\n" 
"ttolmax=               max tolerance (ms) of picks from averge time of	\n"
"                       the maximum peak over the gather		\n" 
"Optional parameters:							\n"
"nsmax=1024             maximum number of trace per pkey gather   	\n"
"lwin=nsmax             number of traces to compute averge time of 	\n"
"                       max peak					\n"
"dkey=                  key (header word) to print to disco cards for edit \n"
"dcards=                data set name to print dkey values for disco	\n"
"Note:									\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/16/95   \n"		    
;

void autopicks(float *data, int n1, int n2, float dt, 
	float ttolmin, float ttolmax,
	int *indx, int *np, float *a, float *peak, int lwin);
void output(FILE *outfp, int *indx, char *buf, int np, 
	int indxp, int indxs, String ptype, String stype,
	float *peak, float tmin, float *a, float dt);

void findpeak(float x1,float x2,float x3,float y1,float y2,float y3,
		float *xp, float *yp);

void dcardout(FILE *outfp, int *indx, char *buf, int np, 
	int indxd, String dtype);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp, *dfp;
	char *list, *dcards;
	String pkey, ptype, skey, stype, dkey, dtype;
	Value pval, sval;
	int indxp, indxs, indxd;

	float tmin,tmax,dt,*a;
	int nsmax;
	int ip, ipre, np, n1, i10, ns, i1, is;
	int lwin;

	float *data, *peak, ttolmin, ttolmax;
	int *indx;
	char *buf;
	int idisco;


    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

    	if(!getparstring("pkey",&pkey)) err("pkey missing");  
    	if(!getparstring("skey",&skey)) err("skey missing"); 
    	if(!getparstring("list",&list)) err("list missing"); 
	outfp = efopen(list,"w");
    	if (!getparfloat("tmin",&tmin)) err("tmin missing");
    	if (!getparfloat("tmax",&tmax)) err("tmax missing");
    	if (!getparfloat("ttolmin",&ttolmin)) err("ttolmin missing");
    	if (!getparfloat("ttolmax",&ttolmax)) err("ttolmax missing");
    	if (!getparint("nsmax",&nsmax)) nsmax = 1024;
    	if (!getparint("lwin",&lwin)) lwin = nsmax;

	idisco = 0;
    	if(getparstring("dcards",&dcards) &&
    	   getparstring("dkey",&dkey) ) idisco = 1;
	if(idisco==1) dfp = efopen(dcards,"w");

	/* read in first trace for nt and dt */
        if (!fgettr(infp,&tr))  err("can't get first trace");
	dt = (float)tr.dt/1000.;
	n1 = (tmax-tmin)/dt+1.;
	i10 = (tmin-tr.delrt/1000)/dt;

	/* memory allocations */
	data = (float*) emalloc(nsmax*n1*sizeof(float));
	indx = (int*) emalloc(nsmax*sizeof(int));
	buf = (char*) emalloc(nsmax*240*sizeof(char));
	peak = (float*) emalloc(nsmax*sizeof(float));
	a = (float*) emalloc(nsmax*sizeof(float));
	

	ptype  = hdtype(pkey);
       	indxp = getindex(pkey);
	stype  = hdtype(skey);
       	indxs = getindex(skey);
	gethval(&tr, indxp, &pval);
	ipre = vtoi(ptype,pval);
	gethval(&tr, indxs, &sval);
	dtype  = hdtype(dkey);
       	indxd = getindex(dkey);
	
	/* loop over input traces */

	ns = 0;
	for(is=0;is<nsmax;is++) indx[is] = -10000;

	fprintf(outfp," =========== supick =========== \n");
	fprintf(outfp," output values of pkey and skey of autopicks \n");

	do {
		gethval(&tr, indxp, &pval);
		ip = vtoi(ptype,pval);
		if(ns>nsmax) 
			err("maximum number traces %d exceed %d \n",ns,nsmax);
		if(ip==ipre && tr.trid==1) {
			for(i1=0;i1<n1;i1++)
				data[i1+ns*n1] = tr.data[i1+i10];
			bcopy(&tr,buf+ns*240,240);
			ns = ns + 1;
		} else if(ip!=ipre && ns>1) {
			autopicks(data,n1,ns,dt,ttolmin,ttolmax,
				indx,&np,a,peak,lwin);
			fprintf(outfp," === for %s=%d ns=%d === \n",
				pkey,ipre,ns);
			fprintf(outfp," %s %s trace average-time pick-time \n",
				pkey,skey);
			output(outfp,indx,buf,np,indxp,indxs,ptype,stype,
				peak,tmin,a,dt);
/*
			if(idisco==1) dcardout(dfp,indx,buf,np,indxd,dtype);
*/
			ns = 0;
			for(i1=0;i1<n1;i1++)
				data[i1+ns*n1] = tr.data[i1+i10];
			bcopy(&tr,buf+ns*240,240);
			for(is=0;is<nsmax;is++) indx[is] = -10000;
			ipre = ip;
			ns = ns + 1;
		}
			
	} while(fgettr(infp,&tr)); 

	if(ns>0) {
		autopicks(data,n1,ns,dt,ttolmin,ttolmax,indx,&np,a,peak,lwin);
		fprintf(outfp," === for %s=%d ns=%d === \n",
			pkey,ipre,ns);
		fprintf(outfp," %s %s trace average-time pick-time \n",
			pkey,skey);
		output(outfp,indx,buf,np,indxp,indxs,ptype,stype,
			peak,tmin,a,dt);
/*
		if(idisco==1) dcardout(dfp,indx,buf,np,indxd,dtype);
*/
	}

	return 0;

}

void autopicks(float *data, int n1, int n2, float dt, 
	float ttolmin, float ttolmax, 
	int *indx, int *np, float *a, 
	float *peak, int lwin) {

	int i1, i2;
	int ip;
	float p;
	float at, tmp, ttol1, ttol2;
	float xp, yp;
	float x1, x2, x3, y1, y2, y3;
	int la, ia, nn;
	
		
	at = 0;

	la = lwin;
	if (lwin>n2 || lwin==0) la = n2;

	for(i2=0;i2<n2;i2++) {
		p = 0.;
		peak[i2] = -9999.;
		for(i1=1;i1<n1-1;i1++) {
			if(data[i1+i2*n1]>data[i1-1+i2*n1] &&
			   data[i1+i2*n1]>data[i1+1+i2*n1] ) {
				x1 = i1-1;
				x2 = i1;
				x3 = i1+1;
				y1 = data[i1-1+i2*n1];
				y2 = data[i1+i2*n1];
				y3 = data[i1+1+i2*n1];
				findpeak(x1,x2,x3,y1,y2,y3,&xp,&yp);
				if(yp>p) {
					p = yp;
					peak[i2] = xp;
				}
			}
		}
	}

/*
*/
	for(i2=0;i2<n2;i2++) {
		if(peak[i2]<0.) {
			for(i1=i2-1;i1>=0;i1--) {
				if(peak[i1]>0.) {
					peak[i2] = peak[i1];
					break;
				}
			}
			if(peak[i2]<0.) {
				for(i1=i2+1;i1<n2;i1++) {
					if(peak[i1]>0.) {
						peak[i2] = peak[i1];
						break;
					}
				}
			}
		}
	}

	for(i2=0;i2<n2;i2++) {
		at = 0.;
		nn = 0;
		for(i1=-la/2;i1<=la/2;i1++) {
			ia = i2+i1;
			if(ia<0) continue;
			if(ia>=n2) continue;
			at = at + peak[ia];
			nn = nn + 1;
		}
		a[i2] = at / nn;
	}


	ip = 0;
	ttol1 = ttolmin/dt;
	ttol2 = ttolmax/dt;

	for(i2=0;i2<n2;i2++) {
		tmp = peak[i2] - a[i2];
		if(tmp<ttol1 || tmp>ttol2) {
			indx[ip] = i2;
			ip = ip + 1;
		}
	}
	*np = ip;
}
	
void output(FILE *outfp, int *indx, char *buf, int np, 
	int indxp, int indxs, String ptype, String stype,
	float *peak, float tmin, float *a, float dt) {

	int i2, is, ip, ii;
	Value pval, sval; 

	segytrace to;

	for (i2=0;i2<np;i2++) {
		ii = indx[i2];
		bcopy(buf+ii*240,&to,240);
		gethval(&to, indxp, &pval);
		ip = vtoi(ptype,pval);
		gethval(&to, indxs, &sval);
		is = vtoi(stype,sval);
		fprintf(outfp," %d %d %d aver=%g pick=%g\n",
			ip,is,ii+1,a[ii]*dt+tmin,peak[ii]*dt+tmin);
	}
}

void dcardout(FILE *outfp, int *indx, char *buf, int np, 
	int indxd, String dtype) {

	int i2,ii;
	Value dval;

	segytrace td;
	int *id, nd, i1;


	nd = 0;
	id  = (int*) emalloc(8*sizeof(int));
	for (i2=0;i2<np;i2++) {
		ii = indx[i2];
		bcopy(buf+ii*240,&td,240);
		gethval(&td, indxd, &dval);
		if(nd>8) {
			for(i1=0;i1<nd-1;i1++) fprintf(outfp,"%d\t",id[i1]);
			fprintf(outfp,"%d\n",id[nd-1]);
			nd = 0;
			id[nd] = vtoi(dtype,dval);
		} else {
			id[nd] = vtoi(dtype,dval);
			nd = nd + 1;
		}
	}
	if(nd>0) {
		for(i1=0;i1<nd-1;i1++) fprintf(outfp,"%d\t",id[i1]);
		fprintf(outfp,"%d\n",id[nd-1]);
	}
	free(id);
}

void findpeak(float x1,float x2,float x3,float y1,float y2,float y3,
		float *xp, float *yp) {
	float a,b,c,aa,bb;
	aa = (y3-y2)/(x3-x2);
	bb = (y1-y2)/(x1-x2);
	b = (aa-bb)/(x3-x1);
	a = aa - b * (x3+x2);
	c = y2 - a*x2 - b*x2*x2;
	*xp = - a/(2.*b);
	*yp = a*(*xp)+b*(*xp)*(*xp)+c;
}
