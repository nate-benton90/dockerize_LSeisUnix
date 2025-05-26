/* velocity card conversion */

#include "par.h"


char *sdoc = 
"VELO2VELF - convert        VELO cards to wgc VELF cards	 	\n"
"\n"
"velo2velf [parameters] < velo-cards >velf-cards			\n" 
"\n"
"Required parameters:						 	\n"
"none									\n"
"\n"
"Optional parameters:							\n"
"none									\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	4/92   		\n"    
;

void vo2vf(int icdpnow,int nvt,float *depths,float *vas,FILE *outfp);

main(int argc, char **argv)
{
    char *cbuf; 
    int n1, n2, i, nxin, jv, icdpnow, icdp;
    int icmax,ic,nvt,idepth,ivelo;
    float *depths,*vas;
    FILE *infp=stdin,*outfp=stdout;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

/* memory allocation */
    n1 = 100;
    n2 = 100;
    icmax = 10000;
    icdpnow = 0;
    nxin = 0;

    cbuf = (char*)malloc(81*sizeof(char));
    vas = (float*)malloc(n1*sizeof(float));
    depths = (float*)malloc(n1*sizeof(float));

    jv = 0;

/* read input velo file */
    for (ic=0;ic<icmax;ic++) {
	if(feof(infp) !=0) break;
        bzero(cbuf,81);
        fgets(cbuf,81,infp);

	if(strncmp(cbuf, "VELO",4)==0) { 
		icdp = 0;
                sscanf(cbuf+5,"%d",&icdp);
	  	if (icdpnow == 0 ) icdpnow = icdp;
	  	if (icdp != icdpnow && icdp!=0 && icdpnow != 0 ) {
	      		nvt = jv;
	      		nxin = nxin + 1;
	      		jv = 0;
			vo2vf(icdpnow,nvt,depths,vas,outfp);
	      		icdpnow = icdp;
		}
	  	for(i=0;i<5;i++) {
			idepth = 0;
                        sscanf(cbuf+15+i*12,"%d",&idepth);
                        ivelo = 0;
                        sscanf(cbuf+21+i*12,"%d",&ivelo);
	     		if (ivelo == 0) break; 

	     		depths[jv] = idepth;
	     		vas[jv] = ivelo;
	     		jv = jv + 1;
	     	}
	}
     }

     if (jv>0) {
	nxin = nxin + 1;	
	nvt = jv;
	vo2vf(icdpnow,nvt,depths,vas,outfp);
     }
     fprintf(stderr,"\n");
     fprintf(stderr,"VELO to VELF conversion done for %d cdps\n",nxin);
     free(depths);
     free(vas);
     free(cbuf);
}

/* convert depth-vav pairs to time-vrms pairs */
void vo2vf(int icdpnow,int nvt,float *depths,float *vas,FILE *outfp) {

	int jc,it,ip,p1,p2,npout;
	float *times, *vis, *vrms, tmp;

	times = (float *) malloc(nvt*sizeof(float));
	vis = (float *) malloc(nvt*sizeof(float));
	vrms = (float *) malloc(nvt*sizeof(float));

	vis[0] = vas[0];
	for(it=1;it<nvt;it++) {
		vis[it] = (vas[it]*depths[it] - 
			   vas[it-1]*depths[it-1])/
			  (depths[it]-depths[it-1]);
	}
	times[0] = depths[0] / vis[0] * 2.0; 
	for(it=1;it<nvt;it++) {
		times[it] = times[it-1] + 
			     (depths[it]-depths[it-1])/vis[it]*2.0;
	}
	vrms[0] = vis[0]; 
	for(it=1;it<nvt;it++) {
		vrms[it] =  sqrt ( ( vrms[it-1]*vrms[it-1]*times[it-1] + 
				vis[it]*vis[it]*(times[it]-times[it-1]) )
				/times[it] );

	}

	fprintf(stderr,"\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"VELO-to-VELF conversion at cdp = %d \n",icdpnow); 
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
		if(jc==0) {
           		fprintf(outfp, "VELF      %5d     ",icdpnow);
		} else {
           		fprintf(outfp, "VELF                ");
		}
           	for(ip=0;ip<npout;ip++) {
               		tmp = times[jc*5+ip]*1000 + 0.5;
               		p1 = tmp;
               		tmp = vrms[jc*5+ip] + .5;
               		p2 = tmp;
               		fprintf(outfp,"%5d%5d",p1,p2);
           	}
           	fprintf(outfp,"\n");
       	}
	
	free(vis);
	free(times);
	free(vrms);

}

