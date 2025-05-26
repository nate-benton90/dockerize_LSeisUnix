/* velocity card format conversion */

#include "velo.h"


char *sdoc = 
"HVEL2VS3D - convert DISCO HANDVEL cards to VS3D cards 	 		\n"
"\n"
"hvel2vs3d [parameters] <handvel-cards >vs3d-cards			\n" 
"\n"
"Required parameters:						 	\n"
"os=           starting lateral position along the inline direction     \n"
"              (e.g., os=0.)						\n"
"ol=           starting lateral position along the crossline direction  \n"
"ds=           cdp spacing in the inline direction                      \n"
"dl=           line spacing in the crossline direction                  \n"
"\n"
"Optional parameters:						 	\n"
"ocdp=         starting cdp number                                      \n"
"ns=           number of cdp's per line                                 \n"
"nl=           number of lines                                          \n"
"incdp=        cdp number increment					\n" 
"cdpnum=0      cdp numbering type (0=inline then crossline)		\n"
"                                 (1=crossline then inline)		\n"
"cdptype=0     type of cdp number in HANVEL card			\n"
"              =0 global sequential cdp number				\n"
"              =1 cdplbl number (line-xline)				\n"
"              Only when cdptype=1 the following four parameters may 	\n"
"              be needed (ocdp, ns, nl, incdp, cdpnum are ignored)	\n"
"ocdplbls=1    starting line number					\n"
"ocdplblx=1    starting cross line number				\n"
"dcdplbls=1    line number increment					\n"
"dcdplblx=1    cross line number increment				\n"
"nvfmax=4096   maximum number of velocity functions in input HANDVEL    \n"
"              dataset                                              \n"
"ntvmax=256    maximum number of t-v pairs per velocity functions   \n"
"              in input HANDVEL dataset                                 \n"
"line=0        0=cdp is global cdp; 1=cdp is the crossline number	\n"
"sposhvel=     HANDVEL carries inline position; =3 at third field of  \n"
"              HANDVEL cards \n" 
"lposhvel=     HANDVEL carries crossline position; =4 at fourth field of \n"  
"              HANDVEL cards \n"
"              when sposhvel and lposhvel are specified, output s and l \n"
"              of VS3D will be copied directly form HANDVEL cards \n"
"\n"
"Notes:									\n"
"1. This program converts DISCO's HANDVEL cards to VS3d	\n"
"   cards. The cdp location of DISCO's HANDVEL cards are transformed 	\n"
"   to the s (shotpoint position within 3d line) and l (line position) 	\n"
"    coodinates of the VS3d cards.				 	\n"
"2. The 3d geometry is defined below:					\n"
"\n"
"                L (crossline)	     	 				\n"
"          			              				\n"
"   ol+(nl-1)*dl *---------------------------------* 			\n"
"                |                                 |			\n"
"                |                                 |			\n"
"                |                                 |		 	\n"
"                |                                 |			\n"
"                |                                 |			\n"
"                |                                 |			\n"
"                |                                 |			\n"
"                |                                 |			\n"
"                |                                 |			\n"
"            ol  *---------------------------------*      S (inline)	\n"
"	        os                          os+(ns-1)*ds		\n"
"\n"
"3. When cdpnum=0, the cdp number of the 1st cdp of the 1st line is ocdp,\n"
"   the cdp number of the 2nd cdp of the 1st line is (ocdp+incdp), 	\n"
"   the cdp number of the 3rd cdp of the 1st line is (ocdp+2*incdp), ...,\n"
"   the cdp number of the i-th cdp of the j-th line is computed using:  \n"
"              cdp(i,j) = [(j-1)*ns+(i-1)]*incdp + ocdp			\n"
"4. When cdpnum=1, the cdp number of the 1st cdp of the 1st line is ocdp,\n"
"   the cdp number of the 1st cdp of the 2nd line is (ocdp+incdp), 	\n"
"   the cdp number of the 1st cdp of the 3rd line is (ocdp+2*incdp),...	\n"
"   the cdp number of the i-th cdp of the j-th line is computed using:  \n"
"              cdp(i,j) = [(j-1)+(i-1)*nl]*incdp + ocdp			\n"
"5. When cdptype=1, ocdplbls, ocdplblx, dcdplbls, dcdplblx, ds, dl are 	\n"
"   are used to determine s and l positions of a given cdplbl 		\n"  
"\n"
"AUTHOR:		Zhiming Li,       ,	7/15/92   		\n"    
;

main(int argc, char **argv)
{
    	char *cbuf; 
    	int n1, n2, i, nxin, jv, icdpnow, icdp;
    	int icmax,ic,nvt;
	float ivelo,itime;
    	FILE *infp=stdin,*outfp=stdout;
    	float time4[4], velo4[4];
    	float s,l;
    	float  *times, *vrms; 
    	int ns, nl, incdp, itmp, is, il, ocdp;
    	float os,ol,ds,dl;
    	int cdpnum=0;
    	int cdptype=0, cdplbl, cdplbls, cdplblx, ocdplbls=1, ocdplblx=1;
    	int dcdplbls=1, dcdplblx=1;
	int line=0, iline=0, ilinenow=0;
	int sposhvel=0, lposhvel=0;
	float snow, lnow;
	float *fbuf;

    	/* get parameters */
    	initargs(argc,argv);
   	askdoc(1);

	if (!getparint("sposhvel",&sposhvel)) sposhvel=0;
	if(sposhvel!=0) {
		if(sposhvel<2 || sposhvel>10) 
			err(" check sposhvel=%d must be >=2 and <=10 \n",sposhvel);
	}
	if (!getparint("lposhvel",&lposhvel)) lposhvel=0;
	if(lposhvel!=0) {
		if(lposhvel<2 || lposhvel>10) 
			err(" check lposhvel=%d must be >=2 and <=10 \n",lposhvel);
	}

	if(lposhvel==0 || sposhvel==0) {
		if (!getparfloat("os",&os)) err("must specify os");
		if (!getparfloat("ol",&ol)) err("must specify ol");
		if (!getparfloat("ds",&ds)) err("must specify ds");
		if (!getparfloat("dl",&dl)) err("must specify dl");
		if (!getparint("cdptype",&cdptype)) cdptype=0;
		if(cdptype==0) {
			if (!getparint("ocdp",&ocdp)) err("must specify ocdp");
			if (!getparint("ns",&ns)) err("must specify ns");
			if (!getparint("nl",&nl)) err("must specify nl");
			if (!getparint("incdp",&incdp)) err("must specify incdp");
			if (!getparint("cdpnum",&cdpnum)) cdpnum = 0;
		} else {
			if (!getparint("ocdplbls",&ocdplbls)) ocdplbls=1;
			if (!getparint("ocdplblx",&ocdplblx)) ocdplblx=1;
			if (!getparint("dcdplbls",&dcdplbls)) dcdplbls=1;
			if (!getparint("dcdplblx",&dcdplblx)) dcdplblx=1;
		}
		if (!getparint("line",&line)) line=0;
	}



/* memory allocation */
	if (!getparint("ntvmax",&n1)) n1=256;
	if (!getparint("nvfmax",&n2)) n2=4096;
    	icmax = n1 * n2 ;
    	icdpnow = 0;
	iline = -99999;
    	nxin = 0;

    	cbuf = (char*)malloc(81*sizeof(char));
    	times = (float*)malloc(n1*sizeof(float));
   	vrms = (float*)malloc(n1*sizeof(float));
    	fbuf = (float*)malloc(9*sizeof(float));

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
	if(line==1) {
       		if ( cbuf[0]=='*' && cbuf[1]=='*' && cbuf[2]=='l' && cbuf[3]=='i'
       	   	&&cbuf[4]=='n' && cbuf[5]=='e' ) sscanf(cbuf+6,"%d",&iline);
	}
       	if ( cbuf[0]=='H' && cbuf[1]=='A' && cbuf[2]=='N' && cbuf[3]=='D' 
		&& cbuf[4]=='V' && cbuf[5]=='E' && cbuf[6]=='L' )  {

		if(sposhvel==0 || lposhvel==0) {
			icdp = 0;
			sscanf(cbuf+8,"%d",&icdp);
			if(icdp==0) err("cdp number can not be zero");

  			if (icdpnow == 0 ) {
				icdpnow = icdp;
				if(line==1) ilinenow = iline;
			}
		} else {
			sscanf(cbuf+8,"%f %f %f %f %f %f %f %f %f ",
				&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],
				&fbuf[4],&fbuf[5],&fbuf[6],&fbuf[7],&fbuf[8]);
			snow = fbuf[sposhvel-2];
			lnow = fbuf[lposhvel-2];
			icdp = snow;
			iline = lnow;
			if(icdpnow == 0) { 
				s = snow;
				l = lnow;
				icdpnow = icdp;
				ilinenow = iline;
			}
		}
	} else if(icdpnow!=0 && cbuf[0]!='*' ) { 
	  	if (icdp != icdpnow || iline !=ilinenow ) {
	      		nvt = jv;
	      		nxin = nxin + 1;
	      		jv = 0;
			if(sposhvel==0 || lposhvel==0) {
	        	    if(line==0) {
				if(cdptype==0) {
					fprintf(stderr,
		"HANDVEL-to-VS3D conversion at cdp=%d for %d t-v pairs \n",
						icdpnow,nvt); 
					itmp = (icdpnow - ocdp)/incdp; 
					if(cdpnum==0) {
						il = itmp/ns;
						is = itmp - il * ns;
					} else {
						is = itmp/nl;
						il = itmp - is * nl;
					}
				} else {
					cdplbls = icdpnow/10000;
					cdplblx = icdpnow - cdplbls*10000;
					il = (cdplbls-ocdplbls)/dcdplbls;
					is = (cdplblx-ocdplblx)/dcdplblx;
					fprintf(stderr,
	"HANDVEL-to-VS3D conversion at cdplbl=%d il=%d is=%d for %d t-v pairs \n",
					icdpnow,il+1,is+1,nvt); 
				}		
			    } else {
				is = icdpnow - 1;
				il = ilinenow - 1;
				fprintf(stderr,
	"HANDVEL-to-VS3D conversion at line=%d xline=%d for %d t-v pairs \n",
					ilinenow,icdpnow,nvt);
			    }
			    s = os + is*ds;
			    l = ol + il*dl;
	      		    icdpnow = icdp;
			    if(line==1) ilinenow  = iline;
			    printvs3d(s,l,nvt,times,vrms,outfp);
			} else {
				fprintf(stderr,
	"HANDVEL-to-VS3D conversion at lpos=%f spos=%f for %d t-v pairs \n",
					l,s,nvt);
	      		    	icdpnow = icdp;
			    	ilinenow  = iline;
			    	printvs3d(s,l,nvt,times,vrms,outfp);
				s = snow;
				l = lnow;
			}
		}
		for(i=0;i<4;i++) {
			time4[i] = 0.;
			velo4[i] = -999999.;
		}
		sscanf(cbuf,"%f %f %f %f %f %f %f %f",
			&time4[0],&velo4[0],&time4[1],&velo4[1],
			&time4[2],&velo4[2],&time4[3],&velo4[3]);

	  	for(i=0;i<4;i++) {
	     		itime = time4[i];
	     		ivelo = velo4[i];
	     		if (ivelo == -999999.) break; 
	     		times[jv] = itime;
	     		vrms[jv] = ivelo;
	     		jv = jv + 1;
	     	}
	   }
     }


     if (jv>0) {
	nxin = nxin + 1;	
	nvt = jv;

	if(sposhvel==0 || lposhvel==0) {
	    if(line==0) {
		if(cdptype==0) {
			fprintf(stderr,
			"HANDVEL-to-VS3D conversion at cdp=%d for %d t-v pairs \n",
			icdpnow,nvt);
			itmp = (icdpnow - ocdp)/incdp; 
			if(cdpnum==0) {
				il = itmp/ns;
				is = itmp - il * ns;
			} else {
				is = itmp/nl;
				il = itmp - is * nl;
			}
		} else {
			cdplbls = icdpnow/10000;
			cdplblx = icdpnow - cdplbls*10000;
			il = (cdplbls-ocdplbls)/dcdplbls;
			is = (cdplblx-ocdplblx)/dcdplblx;
			fprintf(stderr,
"HANDVEL-to-VS3D conversion at cdplbl=%d il=%d is=%d for %d t-v pairs \n",
			icdpnow,il+1,is+1,nvt); 
		}
	    } else {
			is = icdpnow - 1;
			il = ilinenow - 1;
			fprintf(stderr,
"HANDVEL-to-VS3D conversion at line=%d xline=%d for %d t-v pairs \n",
			ilinenow,icdpnow,nvt);
	    }
	    s = os + is*ds;
	    l = ol + il*dl;
	} else {
		fprintf(stderr,
	"HANDVEL-to-VS3D conversion at lpos=%f spos=%f for %d t-v pairs \n",
					l,s,nvt);
	}

	printvs3d(s,l,nvt,times,vrms,outfp);
     }
     fprintf(stderr,"\n");
     fprintf(stderr,"HANDVEL to VS3D conversion done for %d cdps\n",nxin);
     free(times);
     free(vrms);
     free(cbuf);
}


