/* velfcity interpolation */

#include "velo.h"


char *sdoc = 
"VEFF2VS3d - convert HGS VEFF cards to        VS3D cards 	 	\n"
"\n"
"veff2vs3d [parameters] < veff-cards >vs3d-cards			\n" 
"\n"
"Required parameters:						 	\n"
"none									\n"
"\n"
"Optional parameters:							\n"
"lstart=1         offset of crossline coordinates			\n"
"dl=1             crossline coordinate increment			\n" 
"sstart=1         offset of inline coordinates				\n"
"ds=1             inline coordinate increment				\n" 
"nvfmax=4096      maximum number of velocity functions in input veff   \n"
"                 dataset                                              \n"
"ntvmax=256       maximum number of t-v pairs per velocity functions   \n"
"                 in input veff dataset 	                        \n"
"lmin=1           minimum line number to output VS3D cards		\n" 
"lmax=100000      maximum line number to output VS3D cards		\n" 
"smin=1           minimum inline cdp (trace) number to output VS3D cards\n" 
"smax=100000      maximum inline cdp (trace) number to output VS3D cards\n" 
"Notes:									\n"
" 1. s = sstart + (icdp-1)*ds						\n"
" 2. l = lstart + (iline-1)*dl						\n"
"\n"
"Author:	Z. Li		      		6/14/94			\n"
"\n"
;

main(int argc, char **argv)
{
    char *cbuf, *nav ; 
    int n1, i, n2, nxin, jv, icdpnow, icdp, iline ;
    int icmax,ic,nvt,itime,ivelf, ix, iy;
    float x, y ;
    int *times,*vrms;
    char velf[5], time[5], cdp[6], line[10], aline[10], dum1[12], dum2[12];
    FILE *infp=stdin,*outfp=stdout ;
    float lstart, dl, sstart, ds;
    float s, l;	
    int lmin=1, lmax=100000, smin=1, smax=100000;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

    if(!getparfloat("lstart",&lstart)) lstart = 1;	
    if(!getparfloat("dl",&dl)) dl = 1;	
    if(!getparfloat("sstart",&sstart)) sstart = 1;	
    if(!getparfloat("ds",&ds)) ds = 1;	
    getparint("smin",&smin);
    getparint("smax",&smax);
    getparint("lmin",&lmin);
    getparint("lmax",&lmax);

     
/* memory allocation */
    if(!getparint("ntvmax",&n1)) n1 = 256;
    if(!getparint("nvfmax",&n2)) n2 = 4096;
    icmax = n1*n2;
    icdpnow = 0;
    nxin = 0;

 
    cbuf = (char*)malloc(81*sizeof(char));
    vrms = (int*)malloc(n1*sizeof(int));
    times = (int*)malloc(n1*sizeof(int));

    jv = 0;

    fprintf(outfp,
"1--4----------16------24------32------40------48------56------64------72 \n");
    fprintf(outfp,
"CARD           S       L      t1      v1      t2      v2      t3      v3 \n");
    fprintf(outfp,"\n");


/* read input velocity file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
       	  if ( cbuf[0]=='L' && cbuf[1]=='I' && cbuf[2]=='N' && cbuf[3]=='E' ) {
	  	strncpy(line,&cbuf[7],8);
	  	strncpy(aline,&cbuf[11],3);
	  	iline = atoi(line);
		l = lstart + (iline - 1)*dl;
	  }
     	  if ( cbuf[0]=='V' && cbuf[1]=='E' && cbuf[2]=='L' && cbuf[3]=='S'
     	    && cbuf[4]=='S' && cbuf[5]=='P' ) {
	  	strncpy(cdp,&cbuf[7],6);
	  	strncpy(dum1,&cbuf[52],12);
	  	strncpy(dum2,&cbuf[64],12);

                sscanf(dum1,"%f", &x) ;
                sscanf(dum2,"%f", &y) ;
		icdp = atoi(cdp) ;
		ix = x ;
		iy = y ;
	  	if (icdpnow == 0 ) icdpnow = icdp;
	  	if (icdp != icdpnow && icdp!=0 && icdpnow != 0 ) {
			if(icdpnow>=smin && icdpnow<=smax &&
			   iline>=lmin && iline<=lmax) {
				s = sstart + (icdpnow-1) * ds;
	      			nvt = jv;
        			printvs3d(s,l,nvt,times,vrms,outfp);
				fprintf(stderr,
		"VEFF-to-VS3D conversion at cdp=%d line=%d for %d t-v pairs \n",
                                	icdpnow,iline,nvt);
	      			nxin = nxin + 1;
			}
	      		jv = 0;
	      		icdpnow = icdp;
		}
	  }
       	  if ( cbuf[0]=='V' && cbuf[1]=='E' && cbuf[2]=='F' && cbuf[3]=='F' ) {
	  	for(i=0;i<6;i++) {
	     		strncpy(velf,&cbuf[7+i*12],6);
	     		strncpy(time,&cbuf[13+i*12],6);
			ivelf =atoi(velf) ;
			itime = atoi(time) ;
	     		if (ivelf == 0) break; 
	     		times[jv] = itime ;
	     		vrms[jv] = ivelf;
	     		jv = jv + 1;
	     	}
	}
    }

     if (jv>0) {
	if(icdpnow>=smin && icdpnow<=smax &&
	   iline>=lmin && iline<=lmax) {
		nvt = jv;
		s = sstart + (icdpnow-1) * ds;
        	printvs3d(s,l,nvt,times,vrms,outfp);
		fprintf(stderr,
        	"VEFF-to-VS3D conversion at cdp=%d line=%d for %d t-v pairs \n",
        		icdpnow,iline,nvt);
		nxin = nxin + 1;	
	}
     }
     fprintf(stderr,"\n");
     fprintf(stderr,"VEFF to VS3D conversion done for %d cdps\n",nxin);
     free((char*) times);
     free((char*) vrms);
     free(cbuf);
}
