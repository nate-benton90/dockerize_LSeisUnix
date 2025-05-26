#include "velo.h"
#include "xplot.h"
#include "usgrid.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUVAUTOP - velocity automatic picking \n"
"\n"
"suvautop <stdin >stdout vint_ori= [optional parameters] \n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp data 			\n"
"stdout                     Name of final 3D interval velocity HANDVEL card	\n"
"vint_ori                   Name of original 3D time interval velocity grid  \n"
"Optional Parameters:\n"
"plow=90                    Lowest percentage of rms velocity to scan \n"
"phigh=110                   Highest percentage of rms velocity to scan \n"
"np=41                      Number of percentage scans in the program \n"
"tracekey=tracl             Keyword for inline trace number of input trace \n" 
"linekey=tracr              Keyword for line number of input trace \n"
"lwin=                      Window length in ms to compute semblance \n"
"                           default to the time sampling interval of input \n"
"                           original interval velocity grid	\n"
"vw=                        water (or weathering layer) velocity \n"
"                           default to the minimum velocity of the input \n"
"                           velocity grid				\n"
"votype=0                   Output HANDVEL velocity type	\n"
"                           0=rms 1=average 2=interval		\n"
"smute=2.                   Maximum NMO stretch factor allowed \n"
"alpha=.2                   weighting coefficient of penalty function \n"
"                           (deviation from the original interval velocity) \n"
"power=2                    power applied to data to compute semblance	\n"
"                           (only integer allowed) 	\n"
"pvimin=80.                 minimum percentage change of interval velocity\n" 
"                           allowed (from input interval velocity) \n"
"pvimax=120.                maximum percentage change of interval velocity\n" 
"                           allowed (from input interval velocity) \n"
"invr=1                     inversion of rms velocity allowed (0=no 1=yes) \n"
"itout=0                    output time step control				\n"
"                           0=output time step same as vint_ori 	\n" 
"                           1=output times only at large semblance locations\n" 
"qcsemb=0                   qc and screen dump of velan semblance	\n"
"                           0=no 1=yes		\n"							
"Notes:		\n"
"1. Output HANDVEL card will print line and trace locations	\n"
"2. Line and trace locations of the original interval velocity grid \n"
"   are obtained from oline3, dline3, ocdp2 and dcdp2 of the grid header \n"
"   if they are specified; otherwise, they will be obtained from o3, d3, \n"
"   o2 and d2 of the grid header.				\n"
"3. It is recommended to run agc on input data before velocity picking for \n"
"   gathers without geometric-spreading corrections. \n" 
"4. Velocity picking will start from the water (or weathering) bottom. \n"
"   The water (or weathering) bottom time is detected from vw and the \n"
"   input interval velocity grid.										\n"
"\n"
"Author:	Zhiming Li		      		12-16-97		\n"
"\n";
/**************** end self doc *******************************************/


void vi2vr2(float *vi, float *vr2, int n1, float o1, float d1,
	int nt, float ot, float dt, float *twb); 
void nmosum(float *num, float *den, float *nnz, float *data, 
	float *vr2, float offset, float o1, float d1, int n1, 
	float ot, float dt, int nt, float op, float dp, int np, 
	float smute, int power);
void semcal(float *num, float *den, float *nnz, float *sem,
	float ot, float dt, int nt, int np,
	float o1, float d1, int n1, int nw, int power,
	float op, float dp, int qcsemb);
void vapick(float *sem, int np, int n1, float op, float dp,
	float o1, float d1, float twb, float *vi, 
	float *vo, int votype, float alpha, 
	float pvimin, float pvimax, int itout, float *to, 
	int *ntout, int qcsemb, int invr);
float fpower(float a, int power);
void findp(float *sem, int np, int n1, int ip1, int i1,
	float op, float dp, float *s, float *p);


segytrace tr;

main(int argc, char **argv)
{
	FILE *gfp, *infp=stdin, *outfp=stdout;
	char *vint_ori;
	int n1, n2, n3,ierr;
	float o1,o2,o3,d1,d2,d3;
	float *vi, *vr2, *vo, *to;
	float *nnz, *den, *num, *sem;
	float tmp;
	int power;
	int lwin;
	int na, nw;
	int nt, votype;
	float ot, dt;
	float plow, phigh, op, dp, smute, vw;
	float alpha, pvimin, pvimax;
	int np, itout, ntout, qcsemb, invr;

	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	int indxtrk, indxlnk;
	Value trkval, lnkval;			

	float x, y, twb;
	int ixpre, iypre, ix, iy, i1, it, ixx, iyy;
	int cdp;  
	float offset;

	int loc;


	usghed usgh;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("vint_ori",&vint_ori)) err(" vint_ori missing ");
	gfp = efopen(vint_ori,"r");
	ierr = fgetusghdr(gfp, &usgh);
	if(ierr!=0) err(" error getting input grid header ");
	n1 = usgh.n1; o1 = usgh.o1; d1 = usgh.d1;
	n2 = usgh.n2; o2 = usgh.o2; d2 = usgh.d2;
	n3 = usgh.n3; o3 = usgh.o3; d3 = usgh.d3;
	if(usgh.dcdp2!=0) d2 = usgh.dcdp2; if(usgh.ocdp2!=0) o2 = usgh.ocdp2;
	if(usgh.dline3!=0) d3 = usgh.dline3; if(usgh.oline3!=0) o3 = usgh.oline3;

	/* option to open >2GB input file */
	file2g(infp);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	ot = tr.delrt;
	dt = tr.dt/1000.;
	nt = tr.ns;

	
	/* get other optional parameters */
	if (!getparint("lwin",&lwin)) lwin=d1;
	if (!getparfloat("plow",&plow)) plow=90.;
	if (!getparfloat("phigh",&phigh)) phigh=110.;
	if (!getparint("np",&np)) np=41;
	if (!getparfloat("smute",&smute)) smute=2.;
	if (!getparfloat("vw",&vw)) vw=0.;
	if (!getparfloat("alpha",&alpha)) alpha=0.2;
	if (!getparint("votype",&votype)) votype=0;
	if (!getparint("power",&power)) power=2;
	if (!getparfloat("pvimin",&pvimin)) pvimin=80.;
	if (!getparfloat("pvimax",&pvimax)) pvimax=120.;
	if (!getparint("itout",&itout)) itout=0;
	if (!getparint("qcsemb",&qcsemb)) qcsemb=0;
	if (!getparint("invr",&invr)) invr=1;
	pvimin = pvimin*0.01;
	pvimax = pvimax*0.01;

	plow = plow * 0.01;
	phigh = phigh * 0.01;
	dp = (phigh-plow)/(np-1);
	op = plow;
	tmp = lwin/dt+0.5;
	nw = tmp; 

	if(vw==0.) vw = usgh.gmin;
	if(vw==0.) {
		vw = 99999999999.;
		efseek(gfp,0,0);
		for(i1=0;i1<n2*n3;i1++) {
			efread(vi,sizeof(float),n1,gfp);
			for(it=0;it<n1;it++) {
				if(vw>vi[it]) vw=vi[it];
			}
		}
	}

	getparstring("tracekey",&tracekey);
	getparstring("linekey",&linekey);
	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);

	/* job printout */
	fprintf(stderr," ==== SUVAUTOP === \n");
	fprintf(stderr," vint_ori=%s \n",vint_ori);
	fprintf(stderr," plow=%g phigh=%g np=%d \n",plow*100.,phigh*100.,np);
	fprintf(stderr," tracekey=%s linekey=%s \n",tracekey,linekey);
	fprintf(stderr," lwin=%d vw=%g votype=%d \n",lwin,vw,votype);
	fprintf(stderr," smute=%g alpha=%g power=%d \n",smute,alpha,power);
	fprintf(stderr," pvimin=%g pvimax=%g invr=%d itout=%d qcsemb=%d \n",
					pvimin*100.,pvimax*100.,invr,itout,qcsemb);
	fprintf(stderr," \n");


	/* memery allocation */

	vi = (float*) emalloc(n1*sizeof(float));
	vo = (float*) emalloc(n1*sizeof(float));
	to = (float*) emalloc(n1*sizeof(float));
	sem = (float*) emalloc(np*n1*sizeof(float));
	num = (float*) emalloc(nt*np*sizeof(float));
	den = (float*) emalloc(nt*np*sizeof(float));
	nnz = (float*) emalloc(nt*np*sizeof(float));
	vr2 = (float*) emalloc(nt*sizeof(float));

	bzero(sem,n1*np*sizeof(float));
	bzero(num,nt*np*sizeof(float));
	bzero(den,nt*np*sizeof(float));
	bzero(nnz,nt*np*sizeof(float));

	gethval(&tr,indxtrk,&trkval);
	ixpre = vtoi(trktype,trkval);
	gethval(&tr,indxlnk,&lnkval);
	iypre = vtoi(lnktype,lnkval);

	x = (ixpre-o2)/d2 + 0.5;
	y = (iypre-o3)/d3 + 0.5;
	ixx = x;
	iyy = y;
	if(ixx<0) ixx=0; if(ixx>=n2) ixx=n2-1;
	if(iyy<0) iyy=0; if(iyy>=n3) iyy=n3-1;
	loc = (iyy*n2+ixx)*n1*sizeof(float); 
	efseek(gfp,loc,0);
	efread(vi,sizeof(float),n1,gfp);
	vi2vr2(vi,vr2,n1,o1,d1,nt,ot,dt,&twb); 
	cdp = tr.cdp;

	for(i1=0;i1<n1;i1++) to[i1] = o1 + i1*d1;

	/* loop over traces */
	do {
		gethval(&tr,indxtrk,&trkval);
		ix = vtoi(trktype,trkval);
		gethval(&tr,indxlnk,&lnkval);
		iy = vtoi(lnktype,lnkval);

		if(ix==ixpre && iy==iypre) {
			offset = tr.offset;
			/* nmo and stack using different percentages of velocity */
			nmosum(num,den,nnz,tr.data,vr2,offset,o1,d1,n1, 
				ot,dt,nt,op,dp,np,smute,power);
		} else {
			/* calculate semblance */
			semcal(num,den,nnz,sem,ot,dt,nt,np,o1,d1,n1,nw,power,op,dp,qcsemb); 
			/* velocity picking */
			vapick(sem,np,n1,op,dp,o1,d1,twb,vi,vo,votype,alpha,
				pvimin,pvimax,itout,to,&ntout,qcsemb,invr); 
		fprintf(stderr,
		" Velocity picked at cdp=%d %s=%d %s=%d i2vg=%d i3vg=%d twb=%g \n",
			cdp, tracekey, ixpre, linekey, iypre, ixx+1, iyy+1, twb);
			/* output handvel card */
			x = ixpre;
			y = iypre;
			printhvel2(y,x,ntout,to,vo,outfp);

			/* first trace of next cdp */
			ixpre = ix;
			iypre = iy;
			cdp = tr.cdp;
			x = (ixpre-o2)/d2 + 0.5;
			y = (iypre-o3)/d3 + 0.5;
			ixx = x;
			iyy = y;
			if(ixx<0) ixx=0; if(ixx>=n2) ixx=n2-1;
			if(iyy<0) iyy=0; if(iyy>=n3) iyy=n3-1;
			loc = (iyy*n2+ixx)*n1*sizeof(float); 
			efseek(gfp,loc,0);
			efread(vi,sizeof(float),n1,gfp);
			vi2vr2(vi,vr2,n1,o1,d1,nt,ot,dt,&twb); 
			bzero(sem,n1*np*sizeof(float));
			bzero(num,nt*np*sizeof(float));
			bzero(den,nt*np*sizeof(float));
			bzero(nnz,nt*np*sizeof(float));
			offset = tr.offset;
			nmosum(num,den,nnz,tr.data,vr2,offset,o1,d1,n1, 
				ot,dt,nt,op,dp,np,smute,power);
		}

	} while (fgettr(infp,&tr));

/* last cdp */
	semcal(num,den,nnz,sem,ot,dt,nt,np,o1,d1,n1,nw,power,op,dp,qcsemb);
	vapick(sem,np,n1,op,dp,o1,d1,twb,vi,vo,votype,alpha,
			pvimin,pvimax,itout,to,&ntout,qcsemb,invr); 
fprintf(stderr,
		" Velocity picked at cdp=%d %s=%d %s=%d i2vg=%d i3vg=%d twb=%g \n",
			cdp, tracekey, ixpre, linekey, iypre, ixx+1, iyy+1, twb);
	x = ixpre;
	y = iypre;
	printhvel2(y,x,ntout,to,vo,outfp);


	free(vi);
	free(to);
	free(vo);
	free(vr2);
	free(sem);
	free(den);
	free(num);
	free(nnz);

	return EXIT_SUCCESS;
}

/* compute rms velocity squared from interval velocity */
/* and linearly interpolate to input trace time */
/* detect water bottom/weather layer time */
void vi2vr2(float *vi, float *vr2, int n1, float o1, float d1,
	int nt, float ot, float dt, float *twb) {

	int i1, it;
	float tmp, t;
	float *wk;

	wk = (float*) emalloc(n1*sizeof(float));

	tmp = vi[0];
	it = 0;
	for(i1=1;i1<n1;i1++) {
		if(fabs(1.-vi[i1]/tmp)<0.001) {
			it = i1;
		} else {
			break;
		}
	}
	*twb = o1+it*d1;

	wk[0] = vi[0];
	tmp = vi[0]*vi[0]*o1;
	for(i1=1;i1<n1;i1++) {
		tmp = tmp + vi[i1]*vi[i1]*d1;
		wk[i1] = sqrt(tmp/(o1+i1*d1));
	}

	for (it=0;it<nt;it++) {
		t = ((ot + it*dt) - o1)/d1;
		i1 = t;
		if(i1<0) {
			tmp = wk[0]+t*(wk[1]-wk[0]);
		} else if(i1>=n1-1) {
			tmp = wk[n1-2]+(t-n1+2)*(wk[n1-1]-wk[n1-2]);
		} else {
			tmp = wk[i1]+(t-i1)*(wk[i1+1]-wk[i1]);
		}
		vr2[it] = tmp*tmp;
	}

	free(wk);
}

/* nmo and stack using different percentages of velocity */
void nmosum(float *num, float *den, float *nnz, float *data, 
	float *vr2, float offset, float o1, float d1, int n1, 
	float ot, float dt, int nt, float op, float dp, int np, 
	float smute, int power) {

	int it, ip, iti, ipt;
	float frac, temp, tn, ti, odt, op2, p; 
	float *wk;

	wk = (float*) emalloc(nt*sizeof(float));

	for(it=0;it<nt;it++)
		wk[it] = offset*offset/vr2[it];

/*
	fprintf(stderr,"offset=%g vr=%g it=1 op=%g dp=%g \n",
		offset,sqrt(vr2[0]),op,dp);
	fprintf(stderr,"offset=%g vr=%g it=nt op=%g dp=%g \n",
		offset,sqrt(vr2[nt-1]),op,dp);
*/

	odt = 1./dt;

	for(ip=0;ip<np;ip++) {
		p = op + ip*dp;
		op2 = 1./(p*p)*1000000.;
		ipt = ip*nt;
		for(it=0, tn=ot+it*dt; it<nt; ++it, tn+=dt) {
	  		ti = sqrt(tn*tn+wk[it]*op2);
			if(tn>0.) {
				if(ti/tn<=smute) {
	  				ti = (ti-ot)*odt;
					iti = ti;
					if (iti<nt-1 && iti>=0) {
						frac = ti-iti;
						temp = (1.0-frac)*data[iti]+frac*data[iti+1];
						if(temp!=0.) {
							num[ipt+it] += temp;
							den[ipt+it] += fpower(fabs(temp),power);
							/*
							den[ipt+it] += pow(fabs(temp),(float)power);
							*/
							nnz[ipt+it] += 1.0;
						}
					}
				}
			}
		}
	}
	free(wk);
}

/* calculate semblance */
void semcal(float *num, float *den, float *nnz, float *sem,
	float ot, float dt, int nt, int np,
	float o1, float d1, int n1, int nw, int power,
	float op, float dp, int qcsemb) {

	int i1, it, is, ismin, ismax; 
	float t, odt, nsum, dsum;
	int nh, ip, ipt;

	float *wk;


	wk = (float *)emalloc(np*sizeof(float));
	odt = 1./dt;
	nh = nw/2;

/*
	dump2xplot(num,nt,np,0,"num");
	dump2xplot(den,nt,np,0,"den");
	dump2xplot(nnz,nt,np,0,"nnz");
*/

/*
	#pragma MP taskloop private(t,it,ismin,ismax,ip,ipt,nsum,dsum,wk)
	#pragma MP taskloop shared(o1,odt,ot,nh,nt,power,sem)
*/
	for(i1=0;i1<n1;i1++) {
		t = o1 + i1*d1;
		t = (t - ot)*odt;
		it = t;
		ismin = it - nh;
		ismax = it + nh;
		if(ismin<0) ismin=0; 
		if(ismax>nt-1) ismax = nt-1;
		/*
		fprintf(stderr,"i1=%d it=%d ismin=%d ismax=%d \n",i1,it,ismin,ismax);
		*/
		for(ip=0;ip<np;ip++) {
			ipt = ip*nt;
			nsum = dsum = 0.0;
			for(is=ismin;is<ismax;is++) {
				nsum += fpower(fabs(num[ipt+is]),power);
			/*
				nsum += pow(fabs(num[ipt+is]),(float)power);
			*/
				dsum += nnz[ipt+is]*den[ipt+is];
			}
			sem[ip+i1*np] = (dsum!=0.0?nsum/dsum:0.0);
		}

		for(ip=1;ip<np-1;ip++) {
			wk[ip] = .5*sem[ip+i1*np]+.5*(sem[ip-1+i1*np]+sem[ip+1+i1*np]);  
		}
		wk[0] = 0.75*sem[i1*np]+0.25*sem[1+i1*np];
		wk[np-1] = 0.75*sem[np-1+i1*np]+0.25*sem[np-2+i1*np];
		for(ip=0;ip<np;ip++) sem[ip+i1*np] = wk[ip];
	}
	free(wk);

	if(qcsemb==1) dump2xplotn(sem,np,n1,0,"semblance",
		op*100.,o1,dp*100.,d1,"percentage of Vrms","time","solid","solid");

	wk = (float *) emalloc(n1*sizeof(float));

	for(ip=0;ip<np;ip++) {
		for(i1=0;i1<n1;i1++) wk[i1] = sem[ip+i1*np];
		for(i1=1;i1<n1-1;i1++) {
			sem[ip+i1*np] = .5*wk[i1]+.25*(wk[i1-1]+wk[i1+1]);
		}
		sem[ip] = 0.75*wk[0] + 0.25*wk[1];
		sem[ip+(n1-1)*np] = 0.75*wk[n1-1] + 0.25*wk[n1-2];
	}
	free(wk);

}

void vapick(float *sem, int np, int n1, float op, float dp,
	float o1, float d1, float twb, float *vi, 
	float *vo, int votype, float alpha, 
	float pvimin, float pvimax, int itout, float *to, 
	int *ntout, int qcsemb, int invr) {

	int *ippp;

	int i1, iw, ip0, ip;
	int ipp;
	float tmp, vr2t, p0, phigh;
	float t, smax, dpmmax, dppmax, dvmmax, dvpmax, v2l, v2h, obj;
	float v0, v2, p, rdp, rdv;  
	float *wk;
	float *pm1, *pm2, *pp, *vr2p;
	float *sm1, *sm2;
	float *vr2m1, *vr2m2;
	int ip1, ip2, nn1;
	float m1, m2, s1, s2, p1, p2;
	float s, tabove, vr2above, vr2;
	int i1above, i, *i1m;
	int *indx, *ii, *jj, j;
	float *a;
	float zero;
	int n2, i2;


	tmp = (twb - o1)/d1+0.5;
	iw = tmp;

	wk = (float*) emalloc(n1*sizeof(float));
	ippp = (int*) emalloc(n1*sizeof(int));
	pm1 = (float*) emalloc(n1*sizeof(float));
	pm2 = (float*) emalloc(n1*sizeof(float));
	pp = (float*) emalloc(n1*sizeof(float));
	vr2p = (float*) emalloc(n1*sizeof(float));
	sm1 = (float*) emalloc(n1*sizeof(float));
	sm2 = (float*) emalloc(n1*sizeof(float));
	vr2m1 = (float*) emalloc(n1*sizeof(float));
	vr2m2 = (float*) emalloc(n1*sizeof(float));
	i1m = (int*) emalloc(n1*sizeof(int));
	indx = (int*) emalloc(n1*sizeof(int));
	a = (float*) emalloc(n1*sizeof(float));
	ii = (int*) emalloc(n1*sizeof(int));
	jj = (int*) emalloc(n1*sizeof(int));

	/* compute Vrms**2 */
	wk[0] = vi[0]*vi[0];
	tmp = wk[0]*o1;
	for(i1=1;i1<n1;i1++) {
		tmp = tmp + vi[i1]*vi[i1]*d1;
		wk[i1] = tmp/(o1+i1*d1);
	}

	tmp = (1.-op)/dp+0.5;
	ip0 = tmp;
	if(ip0<0) ip0 = 0;
	if(ip0>np-1) ip0 = np-1;
	p0 = op + ip0*dp;
	phigh = op+(np-1)*dp;

	/* give more weights to the semblance at the initial velocity */
	for (i1=0;i1<n1;i1++) {
		sem[ip0+i1*np] *= (1.+alpha);
		if(ip0-1>=0) sem[ip0-1+i1*np] *= (1.+alpha*.5);
		if(ip0+1<np) sem[ip0+1+i1*np] *= (1.+alpha*.5);
	}

	for(i1=0;i1<=iw;i1++) vo[i1] = vi[i1];
	for(i1=0;i1<n1;i1++) ippp[i1] = ip0;

	/* find local maximum */
	nn1 = 0;
	for(i1=iw+1;i1<n1-1;i1++) {
		m1 = 0.;
		m2 = 0.;
		ip1 = -1;
		ip2 = -1;
		for(ip=1;ip<np;ip++) {
			tmp = sem[ip+i1*np];
			if(tmp>sem[ip-1+i1*np] && tmp>sem[ip+1+i1*np] && 	
			   tmp>sem[ip+(i1-1)*np] && tmp>sem[ip+(i1+1)*np] ) {
				if(tmp>m1) {
					m1 = tmp; ip1 = ip;
					m2 = m1; ip2 = ip1;
				} else if(tmp>m2) {
					m2 = tmp; ip2 = ip;
				}
			}
		}
		if(ip1>1) {

			findp(sem,np,n1,ip1,i1,op,dp,&s,&p);

			pm1[nn1] = p;
			sm1[nn1] = s;
			i1m[nn1] = i1;
			vr2m1[nn1] = wk[i1] * p*p;

			if(ip2>1) {
				findp(sem,np,n1,ip2,i1,op,dp,&s,&p);
				pm2[nn1] = p; 
				sm2[nn1] = s; 
				vr2m2[nn1] = wk[i1]*p*p;
			} else {
				pm2[nn1] = -999.; 
			}
			nn1 = nn1+1;
		}
	}

/*
	for(i=0;i<nn1;i++) fprintf(stderr,"i=%d i1m=%d \n",i,i1m[i]);
*/


	/* find largest semblance locations */
	for(i=0;i<nn1;i++) {
		indx[i] = i;
		a[i] = - sm1[i];
	}
	qkisort(nn1,a,indx);
	
	ii[0] = i1m[indx[0]];
	jj[0] = indx[0];
	i1 = 1;
	for(i=1;i<nn1;i++) {
		t = i1m[indx[i]];
		for(j=0;j<i1;j++) {
			tmp = t - ii[j];
			tmp = fabs(tmp);
			if(tmp<=5) break;
			if(j==i1-1) {
				ii[i1] = i1m[indx[i]];
				jj[i1] = indx[i];
				i1 = i1 + 1;
			}
		}
	}
	nn1 = i1;

/*
	for(i=0;i<nn1;i++) {
		fprintf(stderr,"i1=%d jj=%d \n",ii[i],jj[i]);
	}
*/

	for(i=0;i<nn1;i++) {
		a[i] = ii[i];
		indx[i] = i;
	}
	qkisort(nn1,a,indx);

	for(i=0;i<nn1;i++) i1m[i] = ii[indx[i]];	
	/*
	for(i=0;i<nn1;i++) fprintf(stderr,"i1=%d jj=%d \n",i1m[i],jj[indx[i]]);
	*/

	for(i=0;i<nn1;i++) a[i] = sm1[jj[indx[i]]];
	for(i=0;i<nn1;i++) sm1[i] = a[i];
	for(i=0;i<nn1;i++) a[i] = pm1[jj[indx[i]]];
	for(i=0;i<nn1;i++) pm1[i] = a[i];
	for(i=0;i<nn1;i++) a[i] = vr2m1[jj[indx[i]]];
	for(i=0;i<nn1;i++) vr2m1[i] = a[i];

	for(i=0;i<nn1;i++) a[i] = sm2[jj[indx[i]]];
	for(i=0;i<nn1;i++) sm2[i] = a[i];
	for(i=0;i<nn1;i++) a[i] = pm2[jj[indx[i]]];
	for(i=0;i<nn1;i++) pm2[i] = a[i];
	for(i=0;i<nn1;i++) a[i] = vr2m2[jj[indx[i]]];
	for(i=0;i<nn1;i++) vr2m2[i] = a[i];

	tmp = (1.-op)/dp+0.5;
	ip0 = tmp;

	vr2p[0] = wk[iw];
	vr2above = wk[iw];
	tabove = o1 + iw*d1;
	i1above = iw;


	/* auto pick the velocity */
	for(i=0;i<nn1;i++) {
		i1 = i1m[i];
		t = o1 + i1*d1;
		tmp = (wk[i1]*t-wk[i1above]*(o1+i1above*d1))/(t-o1-i1above*d1);
		v2l = tmp * pvimin;
		v2h = tmp * pvimax;

		smax =  sm1[i];
		vr2 = vr2m1[i];
		v2 = (vr2*t - vr2above*tabove)/(t-tabove);
		p = pm1[i];

/*
		fprintf(stderr,"i1=%d v2l=%g v2h=%g vi=%g \n",i1,v2l,v2h,sqrt(tmp));
		fprintf(stderr,"i1=%d p=%g t=%g tab=%g vr2=%g vr2ab=%g v2=%g \n",
			i1,p,t,tabove,vr2,vr2above,v2);
*/

		if( (v2<v2l || v2>v2h) && pm2[i] != -999. ) {
			smax =  sm2[i];
			vr2 = vr2m2[i];
			v2 = (vr2*t - vr2above*tabove)/(t-tabove);
			p = pm2[i];
		}

		if(invr==1 || vr2>vr2above ) {

			if(v2>=v2l && v2<=v2h) {
				vr2p[i] = vr2; 
				pp[i] = p;
			} else {
				vr2p[i] = wk[i1];
				pp[i] = 1.0;
			}

			vr2above = vr2p[i];
			tabove = t;
			i1above = i1;
		} else {
			i1m[i] = -1;
		}

	}

	i = 0;
	for(i1=0;i1<nn1;i1++) {
		if(i1m[i1]!=-1) {
			i1m[i] = i1m[i1];
			vr2p[i] = vr2p[i1];
			pp[i] = pp[i1];
			i = i + 1;
		}
	}
	nn1 = i;

/* linear interpolation of rms velocity */

	/* time and rms velocity at large semblance picks */
	for(i1=0;i1<nn1;i1++) {
		pm1[i1] = o1 + i1m[i1]*d1;
		sm1[i1] = sqrt(vr2p[i1]); 
	}

	/* linearly interpolate the picks to input velocity grid times */
	for(i1=0;i1<n1;i1++) pm2[i1] = o1 + i1*d1;
	tmp = (sm1[nn1-1] - sm1[nn1-2])/(pm1[nn1-1]-pm1[nn1-2]);
	zero = 0.;
	lin1dn_(pm1,sm1,&nn1,pm2,sm2,&n1,indx,&zero,&tmp);

	/* compute vrms*vrms*t and vrms */
	vr2t = vo[iw]*vo[iw]*(o1+iw*d1);
	for(i1=iw+1;i1<n1;i1++) { 
		tmp = (o1+i1*d1)*sm2[i1]*sm2[i1] - vr2t;
		if(tmp>0) {
			vo[i1] = sqrt(tmp/d1); 
		} else {
			vo[i1] = vo[i1-1];
		}
		vr2t = (o1+i1*d1)*sm2[i1]*sm2[i1];
	}

	/* compute the output velocity */
	/* vrms output */
	if(votype==0) {
		wk[0] = vo[0];
		tmp = vo[0]*vo[0]*o1;
		for(i1=1;i1<n1;i1++) {
			tmp = tmp + vo[i1]*vo[i1]*d1;		
			wk[i1] = sqrt(tmp/(o1+i1*d1));
		}
		for(i1=0;i1<n1;i1++) vo[i1] = wk[i1];
	/* vavg output */
	} else if(votype==1) {
		wk[0] = vo[0];
		tmp = vo[0]*o1;
		for(i1=1;i1<n1;i1++) {
			tmp = tmp + vo[i1]*d1;		
			wk[i1] = tmp/(o1+i1*d1);
		}
		for(i1=0;i1<n1;i1++) vo[i1] = wk[i1];
	}

	/* output time sampling */
	if(itout==0) {
		*ntout = n1;
		for(i1=0;i1<n1;i1++) to[i1] = o1+i1*d1;
	} else {
		if(iw==0) {
			to[0] = 0.;
			*ntout = 1;
		} else if(iw>0) {
			*ntout = 2;
			to[0] = 0.;
			to[1] = twb;
		}
		for(i1=0;i1<nn1;i1++) to[*ntout+i1] = o1 + i1m[i1]*d1;
		*ntout += nn1;

		if(to[*ntout-1]< (o1+(n1-1)*d1)) {
			*ntout += 1;
			to[*ntout-1] = o1 + (n1-1)*d1;
		}

		for(i1=0;i1<n1;i1++) {
			pm1[i1] = o1 + i1*d1;
			wk[i1] = vo[i1]; 
		}
		i1 = *ntout;
		lin1d_(pm1,wk,&n1,to,vo,&i1,indx);
	}

/* p-semblance to v-semblance conversion and screen dump */
	if(qcsemb==1) {
		vconvert(to,vo,i1,votype,0,to,sm2,i1,0,0);
		if(itout==0) {
			for(i1=0;i1<n1;i1++) pm1[i1] = o1+i1*d1;
		}
		vconvert(pm1,vi,n1,2,0,pm1,sm1,n1,0,0);

		/*
		for(i1=0;i1<n1;i1++) { 
			fprintf(stderr,"to=%g vo=%g pm1=%g vi=%g sm1=%g \n",
				to[i1],vo[i1],pm1[i1],vi[i1],sm1[i1]);
		}
		*/
		free(wk);
		zero = 999999.;
		tmp = 0.;
		for(i1=0;i1<n1;i1++) {
			if(zero>sm1[i1]) zero = sm1[i1];
			if(tmp<sm1[i1]) tmp = sm1[i1];
		}
		/* if(zero<2000) dv = 25; if(zero>2000) dv = 100.; */
		if(zero<2000) v2h = 30; if(zero>2000) v2h = 100.;
		zero = zero * op;
		tmp = tmp * (op+(np-1)*dp);
		n2 = (tmp - zero)/v2h + 1;
		fprintf(stderr,"vmin=%g vmax=%g dv=%g nv=%d \n",zero,tmp,v2h,n2); 
		wk = (float *) emalloc(n1*n2*sizeof(float));
		bzero(wk,n1*n2*sizeof(float));
		for(i2=0;i2<n2;i2++) {
			tmp = zero + i2*v2h;
			for(i1=0;i1<n1;i1++) {
				v2l = (tmp/sm1[i1]-op)/dp;
				ip = v2l;
				v2l = v2l - ip;
				if(ip>=0 && ip<np-2)
				wk[i1+i2*n1] = sem[ip+i1*np]*(1.-v2l) + sem[ip+1+i1*np]*v2l; 
			}
		}
		dump2xplotn(wk,n1,n2,0,"weighted semblance without picks",
			o1,zero,d1,v2h,"time","rms velocity","solid","solid");
		fprintf(stderr," \n");
		for(i1=0;i1<*ntout;i1++) {
			t = to[i1];
			t = (t - o1)/d1 + 0.5;
			i = t; 
			tmp = (sm2[i1] - zero)/v2h + 0.5;
			i2 = tmp;
			if(i2>=0 && i2<n2) wk[i2*n1+i] = 0.;
			if(i1>0) { 
				tmp = sm2[i1]*sm2[i1]*to[i1] - sm2[i1-1]*sm2[i1-1]*to[i1-1];
				tmp = tmp/(to[i1] - to[i1-1]);
				tmp = sqrt(tmp);
			} else {
				tmp = sm2[0];
			}
	fprintf(stderr," t=%8.1f    Vrms_ori=%8.1f    Vrms_pic=%8.1f    Vint_pic=%8.1f \n", to[i1],sm1[i],sm2[i1],tmp);
		}
		dump2xplotn(wk,n1,n2,0,"weighted semblance with picks",
			o1,zero,d1,v2h,"time","rms velocity","solid","solid");
	}


	free(wk);
	free(ippp);
	free(pm1);
	free(pm2);
	free(pp);
	free(vr2p);
	free(sm1);
	free(sm2);
	free(vr2m1);
	free(vr2m2);
	free(i1m);
	free(indx);
	free(a);
	free(ii);
	free(jj);

}

float fpower(float a, int power)
{
	int i;
	float p;

	p = a;
	for(i=1;i<power;i++) p *= a;

	return p;
}


void findp(float *sem, int np, int n1, int ip1, int i1,
	float op, float dp, float *s, float *p){

	float p1,p2,p3,s1,s2,s3;
	float ss, pp;

			p1 = -1.;
			p2 = 0.;
			p3 = 1.;
			s1 = sem[ip1-1+i1*np];
			s2 = sem[ip1+i1*np];
			s3 = sem[ip1+1+i1*np];
			pol3max_(&p1,&p2,&p3,&s1,&s2,&s3,&pp,&ss);

			*s = ss;
			*p = op + (ip1+pp)*dp; 
}


