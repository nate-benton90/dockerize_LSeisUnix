
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SURENUM - renumber the secondary key sequentially \n" 
"\n"
"surenum [parameters] <input-dat >output-data 		\n" 
"\n"
"Required parameters:							\n"
"None									\n"
"Optional parameters:							\n"
"pkey=tracl             primary key word to identify gather type 	\n" 
"skey=tracr             secondary key word to be used for trace position \n"
"                       within the gather definded by pkey              \n"
"Note:									\n"
"    1. First trace of an output gather will have skey=1, 		\n"
"       second trace of an output gather will have skey=2, 		\n"
"       and so on 		\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	9/1/99   \n"		    
;

void changeval(String type, Value *val, int f);

main(int argc, char **argv)
{

	segytrace tr;

	FILE *infp=stdin, *outfp=stdout;
	String pkey="tracl", ptype, skey="tracr", stype;
	Value pval, sval;
	int indxp, indxs;

	int nt, ns, nsegy;
	int is, ip, ipre;

   	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	getparstring("pkey",&pkey); 
   	getparstring("skey",&skey); 

	/* make file size to be able to exceed 2 G */
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
	

	nsegy = 240 + nt * sizeof(float);

	/* loop over input traces */
	ns = 0;

	do {

		gethval(&tr, indxp, &pval);
		ip = vtoi(ptype,pval);

		if(ip==ipre) {
			ns = ns + 1;
		} else if(ip!=ipre) {
			ns = 1;
			ipre = ip;
		}

		changeval(stype, &sval, ns);
		puthval(&tr, indxs, &sval);
		fputtr(outfp,&tr);
			
	} while(fgettr(infp,&tr)); 

	return 0;

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
