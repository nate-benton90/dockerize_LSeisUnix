
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUFILL - Fill in live traces at missing or dead trace positions 	\n" 
"\n"
"sufill [parameters] <input-dat >output-data 		\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"pkey=tracl             primary key word to identify gather type 	\n" 
"                       (=tracl to indicate a crossline)		\n"
"skey=tracr             secondary key word to be used for trace position \n"
"                       within the gather definded by pkey              \n"
"ofill=                 starting skey number to fill 			\n"
"                       (default to the minimum skey of each input gather) \n"
"nfill=                 number of skey number to ouput			\n"
"                       (default to the number of skey of each input gather) \n"
"dfill=1                skey number increment                           \n"
"nsmax=512              maximum number traces per input gather          \n"
"fillzero=0             fill in with zero trace or interpolated trace   \n"
"                       0=interpolated    1=zero			\n" 
"caprange=1000000       capture range to interpolate			\n"
"                       if there are two traces whose skey vaules are	\n"
"                           skey(fill) - skey(trace 1) <= caprange    \n"
"                       and						\n"
"                           skey(trace 2) - skey(fill) <= caprange    \n"
"                       then, the filled trace will be interpolated 	\n"
"                       from trace 1 and trace 2.			\n"
"zerocheck=0            check zero traces in the input and remove them 	\n"
"                       from input traces to be used in interpolation 	\n"  
"                       (0=no 1=yes)					\n"
"fillend=1              fill in zero traces or copy the end trace \n"
"                       or linearly extrapolate at \n"
"                       outside of input trace range \n"
"                       0=fill with zero; 1=copy; -1=linear extrapolated \n"
"                       9999=not to output	\n"
"Note:									\n"
"    1. when fillzero=1, fill-in traces will be zero traces. 		\n"
"    2. fill-in traces will be obtained by linearly interpolating the two \n"
"       nearest traces within the input gather, when fillzero=0.	\n"
"    3. fill-in traces will be done at either missing skey location or  \n"
"       at trace location where trid is not 1 (dead trace)		\n" 
"    4. missing or dead traces outside the input trace range of the gather \n"
"       will be filled by linearly extrapolating the two nearest traces \n"
"       within the input gather, when fillend=-1; copy from nearest trace	\n"
"       when fillend=1; set to zero when fillend=0; \n"
"    5. in the output, original traces will be marked with key word     \n"
"       duse=1 (segy trace header bytes 35-36), while filled traces 	\n"
"       will be marked with key word duse=2	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	9/16/93   \n"		    
;

void fills(float *si, int ns, int *ofill, int dfill, int *nfill, 
	String stype, int indxs, String pkey, int ip,  
	int iof, int inf, char *buf, int nsegy, int nt,  FILE *outfp, 
	int fillzero, float caprange, int fillend);

void changeval(String type, Value *val, int f);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout;
	String pkey="tracl", ptype, skey="tracr", stype;
	Value pval, sval;
	int indxp, indxs, fillzero, fillend;

	int ofill, dfill, nfill;
	int iof=1, inf=1;
	int nsmax, nt, ns, nsegy;
	int is, ip, ipre;
	float caprange;
	int zerocheck, it, idata;

	float *si;

	char *buf;


   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	getparstring("pkey",&pkey); 
   	getparstring("skey",&skey); 
   	if (!getparint("ofill",&ofill)) iof = 0;
   	if (!getparint("dfill",&dfill)) { dfill = 1;}
   	if (!getparint("nfill",&nfill)) inf = 0;
   	if (!getparint("nsmax",&nsmax)) nsmax = 512;
   	if (!getparint("fillzero",&fillzero)) fillzero = 0;
   	if (!getparint("zerocheck",&zerocheck)) zerocheck = 0;
   	if (!getparfloat("caprange",&caprange)) caprange = 1000000.;
   	if (!getparint("fillend",&fillend)) fillend = 1;

	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	/* read in first trace for nt and dt */
        if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns; 

	ptype  = hdtype(pkey);
   	indxp = getindex(pkey);
	stype  = hdtype(skey);
   	indxs = getindex(skey);
	gethval(&tr, indxp, &pval);
	ipre = vtoi(ptype,pval);
	gethval(&tr, indxs, &sval);
	is = vtoi(stype,sval);
	

	/* memory allocations */
	
	nsegy = 240 + nt * sizeof(float);
	buf = (char*) malloc(nsmax*nsegy*sizeof(char)); 
	si = (float*) malloc(nsmax*sizeof(float)); 


	/* loop over input traces */
	ns = 0;

	do {

		gethval(&tr, indxp, &pval);
		ip = vtoi(ptype,pval);
		gethval(&tr, indxs, &sval);
		is = vtoi(stype,sval);

		if(zerocheck==1) {
			idata = 0;
			for(it=0;it<nt;it++) {
				if(tr.data[it]!=0.) {
					idata = 1;
					break;
				}
			}
			if(idata==0) tr.trid = 2;
		}

		if(ns>nsmax) 
			err("maximum number traces %d exceed %d \n",ns,nsmax);
		if(ip==ipre && tr.trid==1) {
			bcopy((char*)&tr,buf+ns*nsegy,nsegy);
			si[ns] = is;
			ns = ns + 1;
		} else if(ip!=ipre && ns>0) {
			fills(si,ns,&ofill,dfill,&nfill,
				stype,indxs,pkey,ipre, 
				iof,inf,buf,nsegy,nt,outfp,
				fillzero,caprange,fillend); 
			ns = 0;
			if(tr.trid==1) {
				bcopy(&tr,buf+ns*nsegy,nsegy);
				si[ns] = is; 
			}
			ipre = ip;
			ns = ns + 1;
		}
			
	} while(fgettr(infp,&tr)); 

/*
	for(is=0;is<ns;is++) fprintf(stderr,"is=%d si=%g \n",is,si[is]);
*/
			
	if(ns>0) {
		fills(si,ns,&ofill,dfill,&nfill,
			stype,indxs,pkey,ipre,
			iof,inf,buf,nsegy,nt,outfp,
			fillzero,caprange,fillend); 
	}

	return 0;

}


void fills(float *si, int ns, int *ofill, int dfill, int *nfill, 
	String stype, int indxs, String pkey, int ip, 
	int iof, int inf, char *buf, int nsegy, int nt, FILE *outfp,
	int fillzero, float caprange, int fillend) {

	int i, ii, j1, j2, it;
	int *sortindex, *indx;
	float *ss, *so; 
	Value sval;

	float rat, tmp, s;

	segytrace tro, tro1, tro2;
	int iout;

	/* sort si into ascending order */
	sortindex = (int*) malloc(ns*sizeof(int));
	for(i=0;i<ns;i++) sortindex[i] = i;
	qkisort(ns,si,sortindex);

	tmp = si[sortindex[0]];
	s = si[sortindex[0]];
/*
	for(i=0;i<ns;i++) {
		ii = sortindex[i];
		if(s==si[ii]) {
			if(tmp>0.) { 
				si[ii] = tmp*1.0001; 
			} else if(tmp<0.) {
				si[ii] = tmp*0.9999;
			} else {
				si[ii] = 0.0001;
			}
		} else {
			s = si[ii];
		}
		tmp = si[ii];
	}
*/
	
	if(inf==0) *nfill = ns;
	if(iof==0) *ofill = si[sortindex[0]];

	indx = (int*) malloc(*nfill*sizeof(int));
	ss = (float*) malloc(ns*sizeof(float));
	so = (float*) malloc(*nfill*sizeof(float));

	for(i=0;i<ns;i++) {
		ss[i] = si[sortindex[i]];
	}

	/* output nfill traces */
	for(i=0;i<*nfill;i++) {
		so[i] = *ofill + i*dfill;
	}
	bisear_(&ns,&(*nfill),ss,so,indx);

	for(i=0;i<*nfill;i++) {
		ii = indx[i] - 1;
		iout = 1;

		if(ii<0 || so[i]<ss[0]) {
			j1 = sortindex[0];
			if(ns>1) {
				j2 = sortindex[1];
			} else {
				j2 = sortindex[0];
			}
		} else if(ii>=ns-1) {
			j1 = sortindex[ns-1];
			if(ns>1) {
				j2 = sortindex[ns-2];
			} else {
				j2 = sortindex[ns-1];
			}
		} else {
			j1 = sortindex[ii];
			j2 = sortindex[ii+1];
		}
			
		bcopy(buf+j1*nsegy,&tro1,nsegy);
		bcopy(buf+j2*nsegy,&tro2,nsegy);

		if(si[j1]!=si[j2]) {
			rat = (so[i]-si[j1])/(si[j2]-si[j1]);	
		} else {
			rat = 0.;
		}

/*
		fprintf(stderr,
		"so=%g indx=%d i=%d j1=%d j2=%d rat=%g sij1=%g sij2=%g \n",
		so[i],indx[i],i,j1,j2,rat,si[j1],si[j2]);
*/

		if(abs(so[i]-si[j1])<0.1) {
			bcopy(&tro1,&tro,nsegy);
			tro.duse = 1;
		} else if(abs(so[i]-si[j2])<0.1) {
			bcopy(&tro2,&tro,nsegy);
			tro.duse = 1;
		} else {
			if(fillzero==0) {
				if(abs(so[i]-si[j1])<=caprange && 
					abs(so[i]-si[j2])<=caprange) {
					for(it=0;it<nt;it++) {
						tro.data[it] = tro1.data[it] +
							rat*(tro2.data[it]-
				     	     		tro1.data[it]);
					} 
				} else {
					for(it=0;it<nt;it++)
						tro.data[it] = 0.;
				}
			} else {
				for(it=0;it<nt;it++)
					tro.data[it] = 0.;
			}
		
			if(so[i]<ss[0]) {
				if(fillend==0) {
					for(it=0;it<nt;it++) 
						tro.data[it] = 0.;
				} else if(fillend==1) {
					for(it=0;it<nt;it++) 
						tro.data[it] = tro1.data[it];
				}
				if(fillend==9999) iout=0;
			} else if(so[i]>ss[ns-1]) {
				if(fillend==0) {
					for(it=0;it<nt;it++) 
						tro.data[it] = 0.;
				} else if(fillend==1) {
					for(it=0;it<nt;it++) 
						tro.data[it] = tro2.data[it];
				}
				if(fillend==9999) iout=0;
			}

			if(abs(so[i]-si[j1])<abs(so[i]-si[j2])) {
				bcopy(&tro1,&tro,240);
			} else {
				bcopy(&tro2,&tro,240);
			}
			tmp = tro1.mute + rat*(tro2.mute-tro1.mute) + .5;
			tro.mute = tmp;
			tmp = tro1.sx + rat*(tro2.sx-tro1.sx) + .5;
			tro.sx = tmp;
			tmp = tro1.sy + rat*(tro2.sy-tro1.sy) + .5;
			tro.sy = tmp;
			tmp = tro1.gx + rat*(tro2.gx-tro1.gx) + .5;
			tro.gx = tmp;
			tmp = tro1.gy + rat*(tro2.gy-tro1.gy) + .5;
			tro.gy = tmp;
			tmp = tro1.cdp + rat*(tro2.cdp-tro1.cdp) + .5; 
			tro.cdp = tmp;
			tmp = tro1.offset + rat*(tro2.offset-tro1.offset) + .5; 
			tro.offset = tmp;
			tro.duse = 2;
		}
		changeval(stype, &sval, *ofill+i*dfill);
		puthval(&tro, indxs, &sval);


		if(iout==1) fputtr(outfp,&tro);
	}
	fprintf(stderr, 
" Fill-in done from input %d live traces to output %d traces at %s=%d \n",
	ns, *nfill, pkey, ip);


	free(so);
	free(ss);
	free(indx);
	free(sortindex);
}




void changeval(String type, Value *val, int f) {

	switch (*type) {
        case 's':
                err("can't change char header word");
        break;
        case 'h':
                val->h = f;
        break;
        case 'u':
                val->u = f;
        break;
        case 'l':
                val->l = f;
        break;
        case 'v':
                val->v = f;
        break;
        case 'i':
                val->i = f;
        break;
        case 'p':
                val->p = f;
        break;
        case 'f':
                val->f = f;
        break;
        case 'd':
                val->d = f;
        break;
        default:
                err("unknown type %s", type);
        break;
        }
}
