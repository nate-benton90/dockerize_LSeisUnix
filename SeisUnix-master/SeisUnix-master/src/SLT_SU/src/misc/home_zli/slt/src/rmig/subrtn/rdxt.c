
/* read in cshot listing file */
/* author:	Zhiming Li 	*/
/* 11/24/92 */


#include "rmig.h"


/* test main program */

/*
main(int argc, char **argv) {

	int ne, ns, *nr, np, maxnl, nl;
	float *xs, *zs, *xr, *zr;
	int *etype, *nep, *ner;
	float *xe, *ze, *t, *vel;
	FILE *fp, *ofp;
	int maxnr, nsrd, srd0, srdinc;
	int is, ir, ip, ie, il;

	string fname;

        initargs(argc,argv);

	getparstring("fname", &fname);
	fp = efopen(fname,"r");

	maxnl = 128;
	maxnr = 240*3;

	vel = (float*) malloc(maxnl*sizeof(float));

	rdlshd(&ns, &nl, &ne, maxnl, fp, vel);


	fprintf(stderr,"ns=%d nl=%d ne=%d \n",ns,nl,ne);
	for(il=0;il<nl;il++)
		fprintf(stderr,"  layer %d Vel=%g \n",il+1,vel[il]);


	np = nl*2 - 1;
	nsrd = 1;
	srd0 = 1;
	srdinc = 1;

	etype = (int*) malloc(ne*nsrd*sizeof(int));
	nr = (int*) malloc(nsrd*sizeof(int));
	nep = (int*) malloc(nsrd*ne*maxnr*sizeof(int));
	ner = (int*) malloc(nsrd*ne*sizeof(int));
	xs = (float*) malloc(nsrd*sizeof(float));
	zs = (float*) malloc(nsrd*sizeof(float));
	xr = (float*) malloc(maxnr*nsrd*sizeof(float));
	zr = (float*) malloc(maxnr*nsrd*sizeof(float));
	xe = (float*) malloc(maxnr*nsrd*ne*np*sizeof(float));
	ze = (float*) malloc(maxnr*nsrd*ne*np*sizeof(float));
	t = (float*) malloc(maxnr*nsrd*ne*sizeof(float));


	rdxt(ne, ns, np, nr, xs, zs, 
		xr, zr, etype, xe, ze, t,
		nep, ner, srd0, srdinc, nsrd, fp, maxnr);

	for(is=0;is<nsrd;is++) {
		fprintf(stderr,
			"Source xs=%f zs=%f is=%d \n",xs[is],zs[is],is+1);
		for(ir=0;ir<nr[is];ir++) {
			fprintf(stderr," Receiver xr=%f zr=%f ir=%d \n",
				xr[is*maxnr+ir],zr[is*maxnr+ir],ir+1);
		}
		for(ie=0;ie<ne;ie++) {
			fprintf(stderr," Event ie=%d etype=%d \n",
				ie+1,etype[ne*is+ie]);
			for(ir=0;ir<ner[is*ne+ie];ir++) {
				fprintf(stderr," Ray ir=%d \n",ir+1);
				for(ip=0;ip<nep[is*ne*maxnr+ie*maxnr+ir];ip++) {
					fprintf(stderr,
					" Coordinate xe=%f ze=%f ip=%d\n",
					xe[is*ne*maxnr*np+ie*maxnr*np+ir*np+ip],
					ze[is*ne*maxnr*np+ie*maxnr*np+ir*np+ip],
					ip+1);
				}
				fprintf(stderr," Time t=%f \n",
					t[is*ne*maxnr+ie*maxnr+ir]);
			}
		}
	}

	ofp = fopen("junk","w");
	wtxt(ne, ns, np, nr, xs, zs, 
		xr, zr, etype, xe, ze, t,
		nep, ner, srd0, srdinc, nsrd, ofp, maxnr, nl, vel);
	
	

}
*/


/* read csmlisting header:

   void rdlshd(int *ns, int *nl, int *ne, int maxnl, FILE *fp, float* vel);

   input :
	maxnl	---	maximum number of layers
	fp	---	input file pointer
   output:
	ns	---	number of shots
	nl	---	number of layers
	ne	---	number of events per shot
	vel	---	velocities of layers

   author:	Zhiming Li	
*/

void rdlshd(int *ns, int *nl, int *ne, int maxnl, FILE *fp, float *vel) {

	char *buf;
	int is, ie, il;

	buf = (char *)malloc(81*sizeof(char));

        is = 0;
        ie = 0;
	il = 0;


	efseek(fp,0,0);
        if(feof(fp) !=0) err("end of file reached");
        bzero(buf,81);
        if(fgets(buf,81,fp)==NULL) err("end of file reached");
        bzero(buf,81);
        if(fgets(buf,81,fp)==NULL) err("end of file reached");
        if(strncmp(buf, "                    CSHOT1 Listing File",39)!=0)
	err("not a CSHOT1 Listing File");

        do {
                if(feof(fp) !=0) break;
                bzero(buf,81);
                if(fgets(buf,81,fp)==NULL) break;

                if(strncmp(buf, "  Velocities:",13)==0) {
			do {
                		bzero(buf,81);
                		if(fgets(buf,81,fp)==NULL) break;
                		if(strncmp(buf, "\n",1)==0) break;
                        	sscanf(buf+11,"%f",&vel[il]);
				il = il + 1;
			} while ( il < maxnl ); 
                } else if(strncmp(buf, "  Number of shots =",19)==0) {
                        sscanf(buf+20,"%d",&is);
                } else if(strncmp(buf, "  Number of events per shot",27)==0) {
                        sscanf(buf+29,"%d",&ie);
		}
	} while (is==0 || il==0 || ie==0); 

	*ns = is;
	*ne = ie;
	*nl = il;
	free(buf);
	fseek(fp,0,0);
}

/* read x t info from csmlisting file from cshot: 

   void rdxt(int ne, int ns, int np, int *nr, float *xs, float *zs, 
	float *xr, float *zr, int *etype, float *xe, float *ze, float *t,
	int *nep, int srd0, int srdinc, int nsrd, FILE *fp, int maxnr,
	int nl, float *vel);

   input:
	ne	---	number of events per shot
	ns	---	number of shots
	np	---	maximum number of points an event may cross interfaces 
	srd0	---	first shot to read x-t info
	srdinc	---	shot increment to read x-t info
	nsrd	---	number of shots to read x-t info
	maxnr	---	maximum number of rays per event 
			(maxnr > nr; same reflector may have multiple 
			arrivals at one receiver location)
	fp	---	file pointer of csmlisting file 
	nl	---	number of layers
   output:
	nr[nsrd]		---	number of receivers at each shot
	xs[nsrd]		---	x position of shot
	zs[nsrd]		---	z position of shot
	xr[nsrd][maxnr]		---	receiver x position
	zr[nsrd][maxnr]		---	receiver z position
	etype[nsrd][ne]		---	event type (1=reflection 0=head wave
							-1=direct wave)
	xe[nsrd][maxnr][ne][np]	---	x position of event cross at interfaces 
	ze[nsrd][maxnr][ne][np]	---	z position of event cross at interfaces 
	t[nsrd][maxnr][ne]	---	travel times of events
	nep[nsrd][maxnr][ne]	---	number of cross positions of an event
	ner[nsrd][ne]		---	number of rays of an event

   author:	Zhiming Li	      	11/25/92

*/	


void rdxt(int ne, int ns, int np, int *nr, float *xs, float *zs, 
	float *xr, float *zr, int *etype, float *xe, float *ze, float *t,
	int *nep, int *ner, int srd0, int srdinc, int nsrd, 
	FILE *fp, int maxnr) {

	char *buf;
	int is, js;
	int ir, ie, ip;
	int nsread, ifound, idone, ievent;



	buf = (char *)malloc(81*sizeof(char));

	is = srd0;
	nsread = 0;
	ifound = 0;
	idone = 0;

	bzero((char*)nep,nsrd*maxnr*ne*sizeof(int));
	bzero((char*)ner,nsrd*ne*sizeof(int));
	bzero((char*)nr,nsrd*sizeof(int));


	do {
                if(feof(fp) !=0) break;
                bzero(buf,81);
                if(fgets(buf,81,fp)==NULL) break;

		if(strncmp(buf, "  x and z coordinates of shot",29)==0) {
                        sscanf(buf+30,"%d",&js);
			if(js!=is) {
				ifound = 0; 
			} else {
				ifound = 1;
			}
		}

		if(ifound==0) continue;

		/* read in source position */
		bzero(buf,81);
               	if(fgets(buf,81,fp)==NULL) break;
		sscanf(buf,"%f %f",&xs[nsread],&zs[nsread]);
		
		/* read in receiver positions */
               	if(fgets(buf,81,fp)==NULL) break;
		bzero(buf,81);
               	if(fgets(buf,81,fp)==NULL) break;
		sscanf(buf+24,"%d",&nr[nsread]);
		bzero(buf,81);
               	if(fgets(buf,81,fp)==NULL) break;
		if(strncmp(buf, "  x and z coordinates of receivers :",
			 36)==0) {
			 for(ir=0;ir<nr[nsread];ir++) {
				bzero(buf,81);
                		if(fgets(buf,81,fp)==NULL)break;
				sscanf(buf,"%f %f",
					&xr[nsread*maxnr+ir],
					&zr[nsread*maxnr+ir]);
			}
		}

               	if(fgets(buf,81,fp)==NULL) break;
               	if(fgets(buf,81,fp)==NULL) break;

		ie = 0;
		do {
			ir = 0;
			ip = 0;
			ievent = 0;
			do {
				bzero(buf,81);
                		if(fgets(buf,81,fp)==NULL) goto readend;

				if(strncmp(buf, "  Shot",6)==0) {
                        		sscanf(buf+22,"%d",&ie);
					ievent = 1;
					continue;
				}else if(strncmp(buf, "  End of Shot",13)==0){
					goto readend;
				}

				if (ievent==0) continue;


				if(strncmp(buf,
				"  This is a reflection event.",29)==0) {
					etype[nsread*ne+ie-1] = 1;
				}else if(strncmp(buf,
				"  This is a head wave event.",28)==0) {
					etype[nsread*ne+ie-1] = 0;
				}else if(strncmp(buf,
				"  This is a direct wave.",24)==0) {
					etype[nsread*ne+ie-1] = -1;
				}else if(strncmp(buf, "   t =",6)==0) {
                        		sscanf(buf+6,"%f",
						&t[nsread*ne*maxnr+
						  (ie-1)*maxnr+ir]);
					nep[nsread*ne*maxnr+(ie-1)*maxnr+ir]=ip;
					ir = ir + 1;
					ip = 0; 
				}else if(strncmp(buf, "  End of event",14)==0){
					ip = 0;
					break;
				}else if(strncmp(buf, "  End of Shot",13)==0){
					ie = ne + 1;
					break;
				}else if(strncmp(buf, "\n",1)==0){
					continue;
				}else {
                        		sscanf(buf,"%f %f",
						&xe[nsread*ne*maxnr*np+
						   (ie-1)*maxnr*np+
						   ir*np+ip],
						&ze[nsread*ne*maxnr*np+
						   (ie-1)*maxnr*np+
						   ir*np+ip]);
					ip = ip + 1;
				}

			} while (ir<maxnr);
			ner[nsread*ne+ie-1] = ir;
		} while(ie<=ne);

		readend:

		ifound = 0;
		is = is + srdinc;
		nsread = nsread + 1;
		if(nsread==nsrd) idone=1;


	} while( idone == 0 );

	free(buf);
	
}

/* write csm listing file */

void wtxt(int ne, int ns, int np, int *nr, float *xs, float *zs, 
	float *xr, float *zr, int *etype, float *xe, float *ze, float *t,
	int *nep, int *ner, int srd0, int srdinc, int nsrd, 
	FILE *fp, int maxnr,
	int nl, float *vel) {

	int is;
	int ir, ie, ip, il;


	fprintf(fp,"\n");
	fprintf(fp,"                    CSHOT1 Listing File\n");
	fprintf(fp,"\n");
	fprintf(fp,"\n");
	fprintf(fp,"  Velocities:\n");
	for (il=1;il<=nl;il++) {
		fprintf(fp,"  layer %2d  %7.1f\n",il,vel[il-1]);
	}
	fprintf(fp,"\n");
	fprintf(fp,"  Number of shots = %4d\n",ns);
	fprintf(fp,"  Number of events per shot = %4d\n",ne);
	fprintf(fp,"\n");

	for (is=0;is<ns;is++) {
		fprintf(fp,"  x and z coordinates of shot%4d\n",is+1);
		fprintf(fp," %9.2f %9.2f\n",xs[is],zs[is]);
		fprintf(fp,"\n");
		fprintf(fp,"  Number of receivers = %4d\n",nr[is]); 
		fprintf(fp,"  x and z coordinates of receivers :\n");
		for(ir=0;ir<nr[is];ir++) {
			fprintf(fp," %9.2f %9.2f\n",xr[is*maxnr+ir],
				zr[is*maxnr+ir]);
		}
		fprintf(fp,"\n");
		for(ie=0;ie<ne;ie++) {
			fprintf(fp,"\n");
			fprintf(fp,"  Shot %4d      event %4d\n",is+1,ie+1);
			if(etype[is*ne+ie]==1) {
				fprintf(fp,"  This is a reflection event.\n");
			} else if (etype[is*ne+ie]==0) {
				fprintf(fp,"  This is a head wave event.\n");
			} else if (etype[is*ne+ie]==-1) {
				fprintf(fp, "  This is a direct wave.\n");
			}
			fprintf(fp,"\n");
			for(ir=0;ir<ner[is*ne+ie];ir++) {
				for(ip=0;ip<nep[is*ne*maxnr+ie*maxnr+ir];ip++){
					fprintf(fp," %9.2f %9.2f\n",
						xe[is*ne*maxnr*np+
                                                   ie*maxnr*np+ir*np+ip],
                                                ze[is*ne*maxnr*np+
                                                   ie*maxnr*np+ir*np+ip]);
				}
				fprintf(fp,"   t =  %9.6f\n",
					t[is*ne*maxnr+ie*maxnr+ir]);
				fprintf(fp,"\n");
			}
			fprintf(fp,"  End of event\n");
			fprintf(fp,"\n");
		}
		fprintf(fp,"  End of Shot\n");
		fprintf(fp,"\n");
		fprintf(fp,"\n");
	}
	fprintf(fp,"  End of listing\n");
}

/* get x-t at receiver locations 

void getxrt(FILE *fp, int maxnr, int *nevent, int *nxt, 
	float *xrs, float *trs, float *xhs, float *zhs, int sread) {

input:
	fp		---	file pointer of input csm listing file
	maxnr		---	maximum number of rays per event
	sread		---	the shot index to read x-t file (1,2,3....)
output:
	nevent			---	number of event
	nxt[nevent]		---	number of rays per event read
	xrs[nevent][maxnr]	---	x positions of rays at receivers
	trs[nevent][maxnr]	---	time of rays at receivers
	xhs[nevent][maxnr]	---	x positions of rays at interface
	zhs[nevent][maxnr]	---	z positions of rays at interface

author:
	Zhiming Li		      		12/2/92
*/
	
void getxrt(FILE *fp, int maxnr, int *nevent, int *nxt, 
	float *xrs, float *trs, float *xhs, float *zhs, int sread) {

	int ne, ns, *nr, np, maxnl, nl;
	float *xs, *zs, *xr, *zr;
	int *etype, *nep, *ner;
	float *xe, *ze, *t;
	int nsrd, srd0, srdinc;
	int is, ir, ip, ie;
	float *vel;

	/* maximum number of layers */
	maxnl = 128;
	vel = (float*)malloc(maxnl*sizeof(float)); 

	rdlshd(&ns, &nl, &ne, maxnl, fp, vel);


	np = nl*2 - 1;
	srd0 = sread;
	nsrd = 1;
	srdinc = 1;

	etype = (int*) malloc(ne*nsrd*sizeof(int));
	nr = (int*) malloc(nsrd*sizeof(int));
	nep = (int*) malloc(nsrd*ne*maxnr*sizeof(int));
	ner = (int*) malloc(nsrd*ne*sizeof(int));
	xs = (float*) malloc(nsrd*sizeof(float));
	zs = (float*) malloc(nsrd*sizeof(float));
	xr = (float*) malloc(maxnr*nsrd*sizeof(float));
	zr = (float*) malloc(maxnr*nsrd*sizeof(float));
	xe = (float*) malloc(maxnr*nsrd*ne*np*sizeof(float));
	ze = (float*) malloc(maxnr*nsrd*ne*np*sizeof(float));
	t = (float*) malloc(maxnr*nsrd*ne*sizeof(float));

	rdxt(ne, ns, np, nr, xs, zs, 
		xr, zr, etype, xe, ze, t,
		nep, ner, srd0, srdinc, nsrd, fp, maxnr);


	for(ie=0;ie<ne;ie++) {
		for(ir=0;ir<ner[ie];ir++) {
			xrs[ie*maxnr+ir] = 
				xe[ie*maxnr*np+ir*np+nep[ie*maxnr+ir]-1]; 
			trs[ie*maxnr+ir] = 
				t[ie*maxnr+ir]; 
			xhs[ie*maxnr+ir] = 
				xe[ie*maxnr*np+ir*np+nep[ie*maxnr+ir]/2]; 
			zhs[ie*maxnr+ir] = 
				ze[ie*maxnr*np+ir*np+nep[ie*maxnr+ir]/2]; 
		}
		nxt[ie] = ner[ie];
	}

	*nevent = ne;

	free(etype);
	free(nr);
	free(nep);
	free(ner);
	free(xs);
	free(zs);
	free(xr);
	free(zr);
	free(xe);
	free(ze);
	free(t);
	free(vel);
}

/* given receiver positions and source positions, find r1,r2,r3,r4,dr 
for cshot */
void findrs(float *r,int nr, float s, 
	float *r1, float *r2, float *r3, float *r4, float *dr) {

	int ir, jr;
	int ineg, ipos;
	float f1n, f2n, f1p, f2p, tmp; 
	float *drs;
	int *count, indx, itmp; 

	
	drs = (float*) malloc(nr*sizeof(float));
	count = (int*) malloc(nr*sizeof(int));


	ineg = 0;
	ipos = 0;
	f1n = r[0] - s;
	f2n = r[0] - s;
	f1p = r[0] - s;
	f2p = r[0] - s;
	if(f1n>=0.) {
		ipos = 1;	
	} else {
		ineg = 1;
	}

	
	for(ir=1;ir<nr;ir++) {
		tmp = r[ir] - s;
		if( tmp<0.) {
			ineg = 1;
			if(tmp<f1n) {
				f1n = tmp;
			} else if(tmp>f2n) {
				f2n = tmp;
			}
		} else {
			ipos = 1;
			if(tmp<f1p) {
				f1p = tmp;
			} else if(tmp>f2p) {
				f2p = tmp;
			}
		}
	}

	bzero(count,nr*sizeof(int));
	for(ir=1;ir<nr;ir++) {
		drs[ir] = fabs(r[ir]-r[ir-1]); 
		count[ir] = 1;
	}
	for(ir=2;ir<nr;ir++) {
		for(jr=1;jr<ir-1;jr++) {
			if(drs[ir]==drs[jr]) {
				count[jr] += 1;
				drs[ir] = -1.;
				count[ir] = 0;
				break;
			} 
		}
	}

	indx = 1;
	itmp = count[1];

	for(ir=2;ir<nr;ir++) {
		if(count[ir] >itmp) {
			itmp = count[ir];
			indx = ir;
		}
	}

	*dr = drs[indx];
	
	if(ipos==1 && ineg==1) {
		*r1 = f1n;
		*r2 = f2n;
		*r3 = f1p;
		*r4 = f2p;
	} else if(ipos==1 && ineg==0) {
		*r1 = f1p;
		*r2 = f1p + *dr;
		*r3 = f1p + *dr * 2.;
		*r4 = f2p;
	} else if(ipos==0 && ineg==1) {
		*r1 = f1n;
		*r2 = f1n + *dr;
		*r3 = f1n + *dr * 2.;
		*r4 = f2n;
	}

	free(drs);
	free(count);
}

/* update x, z, and t arrays using ray flad iray */ 

void xtupdate(int maxnr,int ne, int np, int ppos,
        float *xe, float *ze, float *t, int *ner, int *nep,
        int *iray) {

        int ie,ir;
        int jr, ip;

        for(ie=0;ie<ne;ie++) {
                jr = 0;
                for(ir=0;ir<ner[(ppos-1)*ne+ie];ir++) {
                        if(iray[ie*maxnr+ir]==1) {
                                for(ip=0;ip<nep[ie*maxnr+ir];ip++) {
                                        xe[(ppos-1)*maxnr*ne*np+ie*maxnr*np+
                                           jr*np+ip] =
                                        xe[(ppos-1)*maxnr*ne*np+ie*maxnr*np+
                                           ir*np+ip];
                                        ze[(ppos-1)*maxnr*ne*np+ie*maxnr*np+
                                           jr*np+ip] =
                                        ze[(ppos-1)*maxnr*ne*np+ie*maxnr*np+
                                           ir*np+ip];
		                }
                                nep[(ppos-1)*maxnr*ne+ie*maxnr+jr] =
                                        nep[(ppos-1)*maxnr*ne+ie*maxnr+ir];
                                t[(ppos-1)*maxnr*ne+ie*maxnr+jr] =
                                        t[(ppos-1)*maxnr*ne+ie*maxnr+ir];
                                jr = jr + 1;
                        }
                }
                ner[(ppos-1)*ne+ie] = jr;
        }
}

/* from x,z,t arrays to arrays specialy used in rmig, xtedit and xzedit */
 
void xt2xrt(int maxnr,int ne, int np, int ppos,
        float *xe, float *ze, float *t, int *ner, int *nep,
        float *xrs, float *trs, float *xhs, float *zhs, int *nxt, int *iray) {

        int ie,ir;

        for(ie=0;ie<ne;ie++) {
                for(ir=0;ir<ner[(ppos-1)*ne+ie];ir++) {
                        xrs[ie*maxnr+ir] =
                                xe[(ppos-1)*maxnr*ne*np+ie*maxnr*np+ir*np
                                        +nep[(ppos-1)*maxnr*ne+ie*maxnr+ir]-1];
                        trs[ie*maxnr+ir] =
                                t[(ppos-1)*maxnr*ne+ie*maxnr+ir];
                        xhs[ie*maxnr+ir] =
                                xe[(ppos-1)*maxnr*ne*np+ie*maxnr*np+ir*np
                                        +nep[(ppos-1)*maxnr*ne+ie*maxnr+ir]/2];
                        zhs[ie*maxnr+ir] =
                                ze[(ppos-1)*maxnr*ne*np+ie*maxnr*np+ir*np
                                        +nep[(ppos-1)*maxnr*ne+ie*maxnr+ir]/2];
                        iray[ie*maxnr+ir] = 1;
                }
                nxt[ie] = ner[(ppos-1)*ne+ie];
        }
}


