
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUCPHW - Copy trace header word to the trace within a gather where the \n"
"value is not specified \n" 
"\n"
"sucphw [parameters] <input-dat >output-data 		\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"pkey=cdp               primary key word to identify gather type 	\n" 
"                       (=cdp to indicate a cdp gather)			\n"
"skey=nhs               key word to be copied 				\n"
"                       within the gather definded by pkey              \n"
"                       (=nhs to indicate copy header value of number of\n"
"                        horizontally summed traces)			\n"
"val=0                  value of key word to be used to idicate where  	\n"
"                       the header word is not specified 		\n" 
"nsmax=240              maximum number of traces within a gather   	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	3/30/94   \n"		    
;

#ifdef __convex__
	#define file2g(x)	fseek64(x,0,1);
#else
	#define file2g(x)	fseek(x,0,1);
#endif

void copys(int *si,int ns,int val,String stype,int indxs,
	char *buf,int nsegy, FILE *outfp);

void changeval(String type, Value *val, int f);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout;
	String pkey="cdp", ptype, skey="nhs", stype;
	Value pval, sval;
	int indxp, indxs, val;

	int nfold, nsegy;
	int i, j, ip, ipre, is, nsmax, ns, nt;
	char *gather;
	int *si;


    	/* get parameters */
    	initargs(argc,argv);
    	askdoc(1);

    	getparstring("pkey",&pkey); 
    	getparstring("skey",&skey); 
    	if (!getparint("val",&val)) val = 0;
    	if (!getparint("nsmax",&nsmax)) nsmax = 240;


	/* make file size to be able to exceed 2 G on convex */
	file2g(infp);
	file2g(outfp);

	/* read in first trace for nt and dt */
        if (!fgettr(infp,&tr))  err("can't get first trace");
	nt = tr.ns; 
	nsegy = nt*sizeof(float) + 240;

	ptype  = hdtype(pkey);
       	indxp = getindex(pkey);
	stype  = hdtype(skey);
       	indxs = getindex(skey);
	gethval(&tr, indxp, &pval);
	ipre = vtoi(ptype,pval);
	gethval(&tr, indxs, &sval);
	is = vtoi(stype,sval);
	

	/* memory allocations */
	
	gather = (char*) emalloc(nsmax*nsegy*sizeof(char)); 
	si = (int*) emalloc(nsmax*sizeof(int));

	/* loop over input traces */
	ns = 0;

	do {

		gethval(&tr, indxp, &pval);
		ip = vtoi(ptype,pval);
		gethval(&tr, indxs, &sval);
		is = vtoi(stype,sval);

		if(ns>nsmax) 
			err("maximum number traces %d exceed %d \n",ns,nsmax);
		if(ip==ipre) {
			bcopy((char*)&tr,gather+ns*nsegy,nsegy);
			si[ns] = is;
			ns = ns + 1;
		} else if(ip!=ipre && ns>0) {
			copys(si,ns,val,stype,indxs,gather,nsegy,outfp);
			ns = 0;
			bcopy(&tr,gather+ns*nsegy,nsegy);
			si[ns] = is; 
			ipre = ip;
			ns = ns + 1;
		}
			
	} while(fgettr(infp,&tr)); 

	if(ns>0) {
		copys(si,ns,val,stype,indxs,gather,nsegy,outfp);
	}

	free(gather);
	free(si);
	return 0;

}


void copys(int *si,int ns,int val,String stype,int indxs,
	char *buf,int nsegy, FILE *outfp) {

	int i;
	Value sval;
	int valcp;
	segytrace tro;

	valcp = si[0];
	for(i=0;i<ns;i++) {
		if(si[i]!=val) {
			valcp = si[i];
			break;
		}
	} 

	for(i=0;i<ns;i++) {
		bcopy(buf+i*nsegy,&tro,nsegy);
		if(si[i]==val) { 
			changeval(stype, &sval, valcp);
			puthval(&tro, indxs, &sval);
		}
		fputtr(outfp,&tro);
	}
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
