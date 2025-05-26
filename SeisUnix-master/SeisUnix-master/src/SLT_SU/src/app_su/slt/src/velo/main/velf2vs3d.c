/* velocity card format conversion */

#include "velo.h"
#include "usu.h"


char *sdoc = 
"VELF2VS3D - convert wgc VELF cards to VS3D cards 	 		\n"
"\n"
"velf2vs3d [parameters] <velf-cards >vs3d-cards				\n" 
"\n"
"Required parameters:						 	\n"
"x1=                    x coordinate of 1st corner of the 3D master grid\n"
"y1=                    y coordinate of 1st corner of the 3D master grid\n"
"s1=                    s coordinate of 1st corner of the 3D master grid\n"
"l1=                    l coordinate of 1st corner of the 3D master grid\n"
"x2=                    x coordinate of 2nd corner of the 3D master grid\n"
"y2=                    y coordinate of 2nd corner of the 3D master grid\n"
"s2=                    s coordinate of 2nd corner of the 3D master grid\n"
"l2=                    l coordinate of 2nd corner of the 3D master grid\n"
"x3=                    x coordinate of 3rd corner of the 3D master grid\n"
"y3=                    y coordinate of 3rd corner of the 3D master grid\n"
"s3=                    s coordinate of 3rd corner of the 3D master grid\n"
"l3=                    l coordinate of 3rd corner of the 3D master grid\n"
"                       (See Notes for details)                        \n"
"nvfmax=4096       maximum number of velocity functions in input VELF   \n"
"                  dataset                                              \n"
"ntvmax=256        maximum number of t-v pairs per velocity functions   \n"
"                  in input VELF dataset                                \n"
"smin=-99999999    minimum s coordinate to output vs3d cards		\n" 
"smax=99999999     maximum s coordinate to output vs3d cards		\n" 
"lmin=-99999999    minimum l coordinate to output vs3d cards		\n" 
"lmax=99999999     maximum l coordinate to output vs3d cards		\n" 
"\n"
"Optional parameters:							\n"
"none									\n"
"\n"
"Notes:									\n"
"1. This program converts WGC's VELF cards (with SPNT cards) to VS3d	\n"
"   cards. The x and y coordinates of WGC's SPNT cards are transformed 	\n"
"   to the s (shotpoint position within 3d line) and l (line position) 	\n"
"    coodinates of the VS3d cards.				 	\n"
"2. The three master-grid corner positions are defined as               \n"
"                                                                       \n"
"        | y                                                            \n"
"        |   .l        * (x4,y4)                                        \n"
"        |    .     .    .                                              \n"
"        |     .  .       .         . s                                 \n"
"       (x3,y3) *          .      .                                     \n"
"        |        .          .  .                                       \n"
"        |         .          * (x2,y2)                                 \n"
"        |          .       .                                           \n"
"        |            .  .                                              \n"
"        |             * (x1,y1)                                        \n"
"        |                                                              \n"
"        |                                                              \n"
"        |                                                              \n"
"        |--------------------------------- x                           \n"
"                                                                       \n"
"   (x1,y1) has the smallest s value and the samllest l value           \n"
"              (s1 is usually =0.0, l1 is usually =0.0)                 \n"
"   (x2,y2) has the largest s value and the smallest l value            \n"
"              (s2 is usually =(ns-1)*ds, l2 is usually =0.0)           \n"
"   (x3,y3) has the smallest s value and the largest l value            \n"
"              (s3 is usually =0.0, l3 is usually =(nl-1)*dl            \n"
"   where                                                               \n"
"         ns is the number of traces per line in the master grid        \n"
"         ds is the trace spacing (within a line) in the master grid    \n"
"         nl is the number of lines in the master grid                  \n"
"         dl is the line spacing in the master grid                     \n"
"         s is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of trace position within a 3d line                       \n"
"         l is the coordinate (in m or ft --- NOT just an integer number)\n"
"              of line position within a 3d survey                      \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	7/7/92   		\n"    
;

main(int argc, char **argv)
{
    char *cbuf; 
    int n1, n2, i, nxin, jv, icdpnow, icdp;
    int icmax,ic,nvt,itime,ivelo;
    char velo[5], time[5], cdp[10], ctmp[10];
    FILE *infp=stdin,*outfp=stdout;
    float x,y,s,l;
    int  *times, *vrms; 
    int icdpspnt=0, ispnt=0;
    float x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3;
    float snow, lnow;
    float sn, ln, snnow, lnnow;
	float smin, smax, lmin, lmax;

    	/* get parameters */
    	initargs(argc,argv);
   	askdoc(1);

	if (!getparfloat("x1",&x1)) err("must specify x1");
        if (!getparfloat("y1",&y1)) err("must specify y1");
        if (!getparfloat("s1",&s1)) err("must specify s1");
        if (!getparfloat("l1",&l1)) err("must specify l1");
        if (!getparfloat("x2",&x2)) err("must specify x2");
        if (!getparfloat("y2",&y2)) err("must specify y2");
        if (!getparfloat("s2",&s2)) err("must specify s2");
        if (!getparfloat("l2",&l2)) err("must specify l2");
        if (!getparfloat("x3",&x3)) err("must specify x3");
        if (!getparfloat("y3",&y3)) err("must specify y3");
        if (!getparfloat("s3",&s3)) err("must specify s3");
        if (!getparfloat("l3",&l3)) err("must specify l3");
        if (!getparfloat("smin",&smin)) smin = -99999999.;
        if (!getparfloat("smax",&smax)) smax = 99999999.;
        if (!getparfloat("lmin",&lmin)) lmin = -99999999.;
        if (!getparfloat("lmax",&lmax)) lmax = 99999999.;


/* memory allocation */
        if (!getparint("ntvmax",&n1)) n1 = 256;
        if (!getparint("nvfmax",&n2)) n2 = 4096;
    icmax = n1 * n2;
    icdpnow = 0;
    nxin = 0;

    cbuf = (char*)malloc(81*sizeof(char));
    times = (int*)malloc(n1*sizeof(int));
    vrms = (int*)malloc(n1*sizeof(int));

    jv = 0;

    fprintf(outfp,
"1--4----------16------24------32------40------48------56------64------72 \n");
    fprintf(outfp,
"CARD           S       L      t1      v1      t2      v2      t3      v3 \n");
    fprintf(outfp,"\n");

/* read input reflectivity file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
       	if ( cbuf[0]=='S' && cbuf[1]=='P' && cbuf[2]=='N' && cbuf[3]=='T' )  {
	  	strncpy(ctmp,&cbuf[5],10);
	  	icdpspnt = atoi(ctmp);
	  	strncpy(ctmp,&cbuf[15],10);
	  	sn = atof(ctmp);
	  	strncpy(ctmp,&cbuf[25],10);
	  	x = atof(ctmp);
	  	strncpy(ctmp,&cbuf[35],10);
	  	y = atof(ctmp);
	  	strncpy(ctmp,&cbuf[45],10);
	  	ln = atof(ctmp);

		xy2sl(x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3,x,y,&s,&l);

	}
       	if(cbuf[0]=='V' &&cbuf[1]=='E' &&cbuf[2]=='L' &&cbuf[3]=='F' ) {
	  	strncpy(cdp,&cbuf[5],10);
	  	icdp = atoi(cdp);
		if(icdp!=icdpspnt && icdp!=0) 
		err("CDF of SPNT card different from CDF of VELF at CDF=",icdp);
	  	if (icdpnow == 0 ) {
			icdpnow = icdp;
			snow = s;
			lnow = l;
			snnow = sn;
			lnnow = ln;
		}
	  	if (icdp != icdpnow && icdp!=0 && icdpnow != 0 ) {
	      		nvt = jv;
	      		jv = 0;
				if(snow>=smin && snow<=smax &&
				   lnow>=lmin && lnow<=lmax) {
	      			nxin = nxin + 1;
					fprintf(stderr,
	"VELF-to-VS3D conversion at cdp=%d sn=%g ln=%g s=%g l=%g nvt=%d \n",
					icdpnow,snnow,lnnow,snow,lnow,nvt); 
					printvs3d(snow,lnow,nvt,times,vrms,outfp);
				}
	      		icdpnow = icdp;
			snow = s;
			lnow = l;
			snnow = sn;
			lnnow = ln;
		}
	  	for(i=0;i<5;i++) {
	     		strncpy(time,&cbuf[20+i*10],5);
	     		itime = atoi(time);
	     		strncpy(velo,&cbuf[25+i*10],5);
	     		ivelo = atoi(velo);
	     		if (ivelo == 0) break; 
	     		times[jv] = itime;
	     		vrms[jv] = ivelo;
	     		jv = jv + 1;
	     	}
	}
     }


    if (jv>0) {
	nvt = jv;
	if(snow>=smin && snow<=smax &&
	   lnow>=lmin && lnow<=lmax) {
		nxin = nxin + 1;	
		printvs3d(snow,lnow,nvt,times,vrms,outfp);
		fprintf(stderr,
	"VELF-to-VS3D conversion at cdp=%d sn=%g ln=%g s=%g l=%g nvt=%d\n",
		icdpnow,snnow,lnnow,snow,lnow,nvt); 
	}
    }
     fprintf(stderr,"\n");
     fprintf(stderr,"VELF to VS3D conversion done for %d cdps\n",nxin);
     free(times);
     free(vrms);
     free(cbuf);
}

