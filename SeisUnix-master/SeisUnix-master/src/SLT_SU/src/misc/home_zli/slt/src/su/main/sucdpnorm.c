#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUCDPNORM - CDP gather normalization (divide trace by fold within	\n"
"            desired output offsets) 					\n"
"\n"
"sucdpnorm <stdin >stdout 						\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"                           (can also be specified as datain=stdin,     \n"
"                           instead of <stdin)				\n"
"stdout                     Name of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"Optional Parameters:\n"
"op=1                       1=normalization; 0=undo normalization	\n" 
"maxfold=120                Maximum number of fold			\n"
"oofo=0.                    minimum offset                		\n"
"dofo=999999.               offset increment                         	\n"
"nofo=1                     number of offsets                        	\n"
"ofo=0.                     offsets  (specified as 200,250.,350,600,...)\n"
"                           if specified, dofo and oofo will be ignored \n"
"signof=1                   signed offset flag 				\n"
"                           (1=absolute value; 0=signed)		\n"
"\n"
"Notes:									\n"
"1. when dataout=datain, the program can read/write to the same file	\n" 
"\n"
"Author:	Zhiming Li		      		1-11-94		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tri, tro;

#ifdef __convex__
	#define file2g(x) fseek64(x,0,1);
#else
	#define file2g(x) fseek(x,0,1);
#endif


main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	int it;		/* time sample index */
	char *datain, *dataout;
	int cdppre,cdpnow;
	FILE *infp,*outfp;
	float scale;
	int maxfold, ix, nfold; 
	float *data;
	char *header;
	int op;
	int nofo;
	float *fold, *ofo, oofo, dofo;
	int itmp, iofoin;
	int *iofo, one, io;
	float ofol, ofor, tmp;
	int signof;
	
	
	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get required parameters */

	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	} 
	file2g(infp);

	if (!getparstring("dataout",&dataout)) {
		outfp = stdout;
	} else {
		if(strcmp(dataout,datain)) { 
			outfp = efopen(dataout,"w");
		} else {
			outfp = efopen(dataout,"r+w");
		}
	} 
	file2g(outfp);

	/* get information from the first header */
	if (!fgettr(infp,&tri)) err("can't get first trace");
	nt = tri.ns;
	
	/* get other optional parameters */
	if (!getparint("maxfold",&maxfold)) maxfold = 120;
	if (!getparint("op",&op)) op = 1;
	if (!getparint("nofo",&nofo)) nofo = 1;
	if (!getparfloat("oofo",&oofo)) oofo = 0.;
	if (!getparfloat("dofo",&dofo)) dofo = 999999.;
	if (!getparint("signof",&signof)) signof = 1;
        itmp = countparname("ofo");
        if(itmp>0 && itmp!=nofo) err("number of ofo not match with nofo");
        ofo = (float*) emalloc(nofo*sizeof(float));
        if(itmp>0) {
                getparfloat("ofo",ofo);
                if(nofo>1) {
                        ofol = ofo[0] - 0.5*(ofo[1]-ofo[0]);
                        ofor = ofo[nofo-1] + 0.5*(ofo[nofo-1]-ofo[nofo-2]);
                } else {
                        ofol = ofo[0] - 0.5*dofo;
                        ofor = ofo[nofo-1] + 0.5*dofo;
	                oofo = ofo[0];
                }
        } else {
                for(io=0;io<nofo;io++) ofo[io] = oofo + io*dofo;
        }
        iofoin = itmp;


	data = (float*) malloc(nt*maxfold*sizeof(float));
	header = (char*) malloc(HDRBYTES*maxfold*sizeof(char));
	fold = (float*) malloc(nofo*sizeof(float));
	iofo = (int*) malloc(maxfold*sizeof(int));

	cdppre = tri.cdp;
	bzero(fold,nofo*sizeof(float));

	/* loop over traces */
	do {

		cdpnow = tri.cdp;
		if (cdpnow==cdppre) {
			tmp  = tri.offset;
			if(signof==1 && tmp<0. ) tmp = -tmp;
                	if(iofoin==0 || nofo==1 ) {
                        	tmp = (tmp-oofo)/dofo+.5;
                        	io = tmp;
                	} else {
                        	if(tmp<ofol) {
                                	io = -1;
                        	} else if(tmp>ofor) {
                                	io = nofo;
                        	} else {
                                	bisear_(&nofo,&one,ofo,&tmp,&io);
                                	io = io - 1;
                                	if(io<nofo-1) {
                                        	if(abs(tmp-ofo[io])	
							>abs(tmp-ofo[io+1]))
                                                	io = io + 1;
                                	}
                        	}
                	}
			if(io>=0 && io<nofo) { 
				bcopy((char*)&tri,header+nfold*HDRBYTES,
					HDRBYTES);
				bcopy((char*)tri.data,data+nfold*nt,nt*4);
				fold[io] += 1.0; 
				iofo[nfold] = io;
				nfold = nfold + 1;
			}
		} else {
			for(ix=0;ix<nfold;ix++) {
				if(op==1) {
					scale = 1./fold[iofo[ix]];
				} else {
					scale = fold[iofo[ix]];
				}
				for(it=0;it<nt;it++) 
					tro.data[it] = data[it+ix*nt]*scale; 
				bcopy(header+ix*HDRBYTES,(char*)&tro,
					HDRBYTES);
				fputtr(outfp,&tro);
			}
			nfold = 0;
			bzero(fold,nofo*sizeof(float));
			tmp  = tri.offset;
			if(signof==1 && tmp<0. ) tmp = -tmp;
                        if(iofoin==0 || nofo==1 ) {
                                tmp = (tmp-oofo)/dofo+.5;
                                io = tmp;
                        } else {
                                if(tmp<ofol) {
                                        io = -1;
                                } else if(tmp>ofor) {
                                        io = nofo;
                                } else {
                                        bisear_(&nofo,&one,ofo,&tmp,&io);
                                        io = io - 1;
                                        if(io<nofo-1) {
                                                if(abs(tmp-ofo[io])
                                                        >abs(tmp-ofo[io+1]))
                                                        io = io + 1;
                                        }
                                }
                        }
                        if(io>=0 && io<nofo) {
                                bcopy((char*)&tri,header+nfold*HDRBYTES,
                                        HDRBYTES);
                                bcopy((char*)tri.data,data+nfold*nt,nt*4);
				fold[io] += 1.0; 
				iofo[nfold] = io;
				nfold = nfold + 1;
			}
			cdppre = cdpnow;
		}
		
	} while (fgettr(infp,&tri));

	/* output last gather */
	if(nfold>0) {
		for(ix=0;ix<nfold;ix++) {
			if(op==1) {
				scale = 1./fold[iofo[ix]];
			} else {
				scale = fold[iofo[ix]];
			}
			for(it=0;it<nt;it++) 
				tro.data[it] = data[it+ix*nt]*scale; 
			bcopy(header+ix*HDRBYTES,(char*)&tro,HDRBYTES);
			fputtr(outfp,&tro);
		}
	}
	
	return EXIT_SUCCESS;
}
