/* velocity interpolation */

#include "par.h"


char *sdoc = 
"VELF2VELO - convert wgc VELF cards to        VELO cards 	 	\n"
"\n"
"velf2velo [parameters] < velf-cards >velo-cards			\n" 
"\n"
"Required parameters:						 	\n"
"none									\n"
"\n"
"Optional parameters:							\n"
"none									\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	3/10/92   		\n"    
;

void vf2vo(int icdpnow,int nvt,float *times,float *vrms,FILE *outfp);

main(int argc, char **argv)
{
    char *cbuf; 
    int n1, n2, i, nxin, jv, icdpnow, icdp;
    int icmax,ic,nvt,itime,ivelo;
    float *times,*vrms;
    char velo[5], time[5], cdp[10];
    FILE *infp=stdin,*outfp=stdout;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

/* memory allocation */
    n1 = 256;
    n2 = 256;
    icmax = n1 * n2;
    icdpnow = 0;
    nxin = 0;

    cbuf = (char*)malloc(81*sizeof(char));
    vrms = (float*)malloc(n1*sizeof(float));
    times = (float*)malloc(n1*sizeof(float));

    jv = 0;

    fprintf(outfp,
"1---5---10---15----21----27----33----39----45----51----57----63----69----75\n")
;
    fprintf(outfp,
"NAME   cdp         z1    v1    z2    v2    z3    v3    z4    v4    z5    v5\n")
;
    fprintf(outfp,"\n");

/* read input reflectivity file */
    for (ic=0;ic<icmax;ic++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
       	if ( cbuf[0]=='V' && cbuf[1]=='E' && cbuf[2]=='L' && cbuf[3]=='F' )  {
	  	strncpy(cdp,&cbuf[5],10);
	  	icdp = atoi(cdp);
	  	if (icdpnow == 0 ) icdpnow = icdp;
	  	if (icdp != icdpnow && icdp!=0 && icdpnow != 0 ) {
	      		nvt = jv;
	      		nxin = nxin + 1;
	      		jv = 0;
			vf2vo(icdpnow,nvt,times,vrms,outfp);
	      		icdpnow = icdp;
		}
	  	for(i=0;i<5;i++) {
	     		strncpy(time,&cbuf[20+i*10],5);
	     		itime = atoi(time);
	     		strncpy(velo,&cbuf[25+i*10],5);
	     		ivelo = atoi(velo);
	     		if (ivelo == 0) break; 
	     		times[jv] = itime * 0.001;
	     		vrms[jv] = ivelo;
	     		jv = jv + 1;
	     	}
	}
     }

     if (jv>0) {
	nxin = nxin + 1;	
	nvt = jv;
	vf2vo(icdpnow,nvt,times,vrms,outfp);
     }
     fprintf(stderr,"\n");
     fprintf(stderr,"VELF to VELO conversion done for %d cdps\n",nxin);
     free(times);
     free(vrms);
     free(cbuf);
}

/* convert time-vrms pairs to depth-vav pairs */
void vf2vo(int icdpnow,int nvt,float *times,float *vrms,FILE *outfp) {

	int jc,it,ip,p1,p2,npout;
	float *depths, *vis, *vas,tmp;

	depths = (float *) malloc(nvt*sizeof(float));
	vis = (float *) malloc(nvt*sizeof(float));
	vas = (float *) malloc(nvt*sizeof(float));

	vis[0] = vrms[0];
	for(it=1;it<nvt;it++) {
		vis[it] = sqrt( (vrms[it]*vrms[it]*times[it] - 
			   vrms[it-1]*vrms[it-1]*times[it-1])/
			  (times[it]-times[it-1]) );
	}
	depths[0] = times[0] * vis[0] * 0.5; 
	for(it=1;it<nvt;it++) {
		depths[it] = depths[it-1] + 
			     (times[it]-times[it-1])*vis[it]*0.5;
	}
	vas[0] = vis[0]; 
	for(it=1;it<nvt;it++) {
		vas[it] = (depths[it-1]*vas[it-1]+vis[it]*
				(depths[it]-depths[it-1]))
			  /depths[it];	
	}

	fprintf(stderr,"\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"VELF-to-VELO conversion at cdp = %d \n",icdpnow); 
	fprintf(stderr,"\n");
	fprintf(stderr," --TIME--*--VRMS--*---VI---*--DEPTH--*--VA--\n",
			icdpnow); 
	for(it=0;it<nvt;it++) {
	fprintf(stderr,"  %5.0f    %5.0f    %5.0f     %5.0f  %5.0f \n",
		times[it]*1000.,vrms[it],vis[it],depths[it],vas[it]); 
	}

	for (jc=0;jc<(nvt+4)/5;jc++) {
       		if( (jc+1)*5 < nvt ) {
           		npout = 5;
           	} else {
              		npout = nvt - jc*5;
           	}
           	fprintf(outfp, "VELO %5d     ",icdpnow);
           	for(ip=0;ip<npout;ip++) {
               		tmp = depths[jc*5+ip] + 0.5;
               		p1 = tmp;
               		tmp = vas[jc*5+ip] + .5;
			p2 = tmp; 
               		fprintf(outfp,"%6d%6d",p1,p2);
           	}
           	fprintf(outfp,"\n");
       	}
	
	free(vis);
	free(depths);
	free(vas);

}

