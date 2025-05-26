/* velocity card format conversion */

#include "velo.h"
#include "usu.h"


char *sdoc = 
"VELF2HVEL - convert wgc VELF cards to HVEL cards 	 		\n"
"\n"
"velf2vs3d [parameters] <velf-cards >hvel-cards				\n" 
"\n"
"Required parameters:						 	\n"
"x1=                    x coordinate of 1st corner of the 3D master grid\n"
"y1=                    y coordinate of 1st corner of the 3D master grid\n"
"s1=                    xline number of 1st corner of the 3D master grid\n"
"l1=                    line number of 1st corner of the 3D master grid\n"
"cdp1=                  cdp number of 1st corner of the 3D master grid \n"
"x2=                    x coordinate of 2nd corner of the 3D master grid\n"
"y2=                    y coordinate of 2nd corner of the 3D master grid\n"
"s2=                    xline number of 2nd corner of the 3D master grid\n"
"l2=                    line number of 2nd corner of the 3D master grid\n"
"cdp2=                  cdp number of 2nd corner of the 3D master grid \n"
"x3=                    x coordinate of 3rd corner of the 3D master grid\n"
"y3=                    y coordinate of 3rd corner of the 3D master grid\n"
"s3=                    xline number of 3rd corner of the 3D master grid\n"
"l3=                    line numbe of 3rd corner of the 3D master grid\n"
"cdp3=                  cdp number of 3rd corner of the 3D master grid \n"
"                       (See Notes for details)                        \n"
"hveltype=1        HANDVEL type (1=cdp number   2=(line, xline) number \n"
"                                3=cdplbl ) \n"
"nvfmax=4096       maximum number of velocity functions in input VELF   \n"
"                  dataset                                              \n"
"ntvmax=256        maximum number of t-v pairs per velocity functions   \n"
"                  in input VELF dataset                                \n"
"smin=-99999       minimum xline number to output HANDVEL			\n"
"smax=99999        maximum xline number to output HANDVEL			\n"
"lmin=-99999       minimum line number to output HANDVEL			\n"
"lmax=99999        maximum line number to output HANDVEL			\n"
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
"   (x2,y2) has the largest s value and the smallest l value            \n"
"   (x3,y3) has the smallest s value and the largest l value            \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	2/11/96   		\n"    
;

main(int argc, char **argv)
{
    char *cbuf; 
    int n1, n2, i, nxin, jv, icdpnow, icdp;
    int icmax,ic,nvt,itime,ivelo;
    char velo[5], time[5], cdp[10], ctmp[10];
	char card[4];
    FILE *infp=stdin,*outfp=stdout;
    float x,y,s,l;
    float  *times, *vrms; 
    int icdpspnt=0, ispnt=0;
    float x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3;
	float cdp1, cdp2, cdp3;
    float xnow, ynow;
    float sn, ln;
	float snnow, lnnow;
	float icdpl,icdps;
	float tmp;
	int cdpnum,hveltype,cdplbl;
	float smin,smax,lmin,lmax;

    	/* get parameters */
    	initargs(argc,argv);
   	askdoc(1);

	if (!getparfloat("x1",&x1)) err("must specify x1");
        if (!getparfloat("y1",&y1)) err("must specify y1");
        if (!getparfloat("s1",&s1)) err("must specify s1");
        if (!getparfloat("l1",&l1)) err("must specify l1");
        if (!getparfloat("cdp1",&cdp1)) err("must specify cdp1");
        if (!getparfloat("x2",&x2)) err("must specify x2");
        if (!getparfloat("y2",&y2)) err("must specify y2");
        if (!getparfloat("s2",&s2)) err("must specify s2");
        if (!getparfloat("l2",&l2)) err("must specify l2");
        if (!getparfloat("cdp2",&cdp2)) err("must specify cdp2");
        if (!getparfloat("x3",&x3)) err("must specify x3");
        if (!getparfloat("y3",&y3)) err("must specify y3");
        if (!getparfloat("s3",&s3)) err("must specify s3");
        if (!getparfloat("l3",&l3)) err("must specify l3");
        if (!getparfloat("cdp3",&cdp3)) err("must specify cdp3");
        if (!getparint("hveltype",&hveltype)) hveltype=1;
        if (!getparfloat("smin",&smin)) smin=-99999;
        if (!getparfloat("smax",&smax)) smax=99999;
        if (!getparfloat("lmin",&lmin)) lmin=-99999;
        if (!getparfloat("lmax",&lmax)) lmax=99999;


	    icdps = (cdp2-cdp1)/(s2-s1); 
	    icdpl = (cdp3-cdp1)/(l3-l1); 

/* memory allocation */
        if (!getparint("ntvmax",&n1)) n1 = 256;
        if (!getparint("nvfmax",&n2)) n2 = 4096;
    icmax = n1 * n2;
    icdpnow = 0;
    nxin = 0;

    cbuf = (char*)malloc(81*sizeof(char));
    times = (float*)malloc(n1*sizeof(int));
    vrms = (float*)malloc(n1*sizeof(int));

    jv = 0;

/* read input velf file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
		/*
		fprintf(stderr,"%s\n",cbuf);
		*/
       	if ( cbuf[0]=='S' && cbuf[1]=='P' && cbuf[2]=='N' && cbuf[3]=='T' )  {

/*
		sscanf(cbuf,"%4c %d %g %g %g %g \n",card,&icdpspnt,&sn,&x,&y,&ln);
*/

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

/*
		fprintf(stderr,"icdpspnt=%d \n",icdpspnt);
*/

		xy2sl(x1,y1,s1,l1,x2,y2,s2,l2,x3,y3,s3,l3,x,y,&s,&l);
		
		tmp = cdp1 + ((int)(s-s1+0.5))*icdps + ((int)(l-l1+0.5))*icdpl;
		cdpnum = (int)( tmp + 0.5);
	}

       	if(cbuf[0]=='V' &&cbuf[1]=='E' &&cbuf[2]=='L' &&cbuf[3]=='F' ) {
	  	strncpy(cdp,&cbuf[5],10);
	  	icdp = atoi(cdp);
		if(icdp!=icdpspnt && icdp!=0) 
		err("CDF %d of SPNT card different from CDF of VELF CDF=%d",
			icdpspnt,icdp);
		if(icdp!=cdpnum && icdp!=0) 
		warn("recomputed cdp %d different from cdp %d in VELF",cdpnum,icdp);
	  	if (icdpnow == 0 ) {
			icdpnow = cdpnum;
			xnow = x;
			ynow = y;
			snnow = s;
			lnnow = l;
		}
	  	if (cdpnum != icdpnow && icdp!=0 && icdpnow != 0 ) {
	      	nvt = jv;
	      	jv = 0;
			if(lnnow>=lmin && lnnow<=lmax &&
			   snnow>=smin && snnow<=smax) {
	      		nxin = nxin + 1;
				fprintf(stderr,
			"VELF2HVEL at cdp=%d sn=%d ln=%d x=%-12.2f y=%-12.2f nvt=%d\n",
			icdpnow,(int)(snnow+0.5),(int)(lnnow+0.5),xnow,ynow,nvt); 
				if(hveltype==1) {
					printhvel(icdpnow,nvt,times,vrms,outfp);
		    	} else if(hveltype==2) {
					printhvel2(lnnow,snnow,nvt,times,vrms,outfp);
				} else if(hveltype==3) {
					cdplbl = (int)(lnnow+0.5)*10000+(int)(snnow+0.5);	
					printhvel(cdplbl,nvt,times,vrms,outfp);
				} else {
					err("invalid hveltype: %d \n",hveltype);
				}
			}
	      	icdpnow = cdpnum;
			xnow = x;
			ynow = y;
			snnow = s;
			lnnow = l;
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
	if(lnnow>=lmin && lnnow<=lmax &&
	   snnow>=smin && snnow<=smax) {
		nxin = nxin + 1;	
		if(hveltype==1) {
			printhvel(icdpnow,nvt,times,vrms,outfp);
		} else if(hveltype==2) {
			printhvel2(lnnow,snnow,nvt,times,vrms,outfp);
		} else if(hveltype==3) {
			cdplbl = (int)(lnnow+0.5)*10000+(int)(snnow+0.5);	
			printhvel(cdplbl,nvt,times,vrms,outfp);
		} else {
			err("invalid hveltype: %d \n",hveltype);
		}
		fprintf(stderr,
		"VELF2HVEL at cdp=%d sn=%d ln=%d x=%-12.2f y=%-12.2f nvt=%d\n",
			icdpnow,(int)(snnow+0.5),(int)(lnnow+0.5),xnow,ynow,nvt); 
	}
    }
     fprintf(stderr,"\n");
     fprintf(stderr,"VELF2HVEL conversion done for %d cdps\n",nxin);
     free(times);
     free(vrms);
     free(cbuf);
}

