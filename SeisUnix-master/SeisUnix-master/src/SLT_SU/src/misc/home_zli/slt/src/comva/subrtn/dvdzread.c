#include "comva.h"

/* read in velocity gradient picks from input file pointer infp (DVDZ card */
/* dataset) 								*/
/* DVDZ format:
1---5---10---15----21----27----33----39----45----51----57----63----69----75 
DVDZ   cdp        zt1   zb1 dvdz1   zt2   zb2 dvdz2   zt3   zb3 dvdz3    

where          cdp indicates cdp number of current panel 
               zti, i=1,2,..., are depth of top of gradient analysis zone 
               zbi, i=1,2,..., are depth of bottom of gradient analysis zone
               dvdzi, i=1,2,..., are interval velocity gradient at [zti,zbi]
*/
/*									*/
/* input:                                                               */
/*      infp                    file pointer for VELO card dataset      */
/*      maxp                    maximum number of z-v pairs per cdp     */
/*	maxcdp  		maximum number of cdps of VELO cards 	*/
/* ouput:                                                               */
/*      cdps[maxcdp]            cdp numbers of VELO card input          */
/*      ztps[maxcdp][maxp]      z positions of tops of layers           */
/*      zbts[maxcdp][maxp]      z positions of bottoms of layers        */
/*      dvdz[maxcdp][maxp]      interval velocity gradient at layers    */
/*      ncdps                   total number of cdps found in DVDZ cards*/
/*      nps[maxcdp]             number zt-zb-dvdz pairs per cdp         */
/*                                                                      */
/* author:      Zhiming Li              11/91                           */

void dvdzread(FILE *infp, int *cdps, float *ztps, float *zbts, float *dvdz, 
int *ncdps, int *nps, int maxp, int maxcdp) {

	int icmax, ic, cdpnow=0, cdppre=0, cdpchange=0, jc=0;
        int i1, i2, i3, i;
        char *cbuf;
	float f4;
	int cardfound=0;

   	cbuf = (char *) malloc(81*sizeof(char));

	/* rewind infp */
   	fseek(infp,0,0);

	/* read in DVDZ cards */
	icmax = maxcdp * maxp;

   	for (ic=0;ic<icmax;ic++) {
       		if (feof(infp) !=0 ) break;
     		bzero(cbuf,81);
                fgets(cbuf,81,infp);

        	if(strncmp(cbuf, "DVDZ", 4)==0) {
			cardfound = 1;

			cdpnow = 0;
                        sscanf(cbuf+5,"%d",&cdpnow);

	  		if (cdppre == 0 ) cdppre = cdpnow ;
			/* if cdp changes */
          		if (cdpnow != cdppre && cdpnow !=0 && cdppre !=0 ) {
				nps[cdpchange] = jc;
                                cdps[cdpchange] = cdppre;
                                cdpchange += 1;
                                jc = 0;
                                cdppre = cdpnow;
			}

	  		/* store read values in depth, vel and dvdz arrays */
          		for(i=0;i<3;i++) {

				i2 = 0;
				sscanf(cbuf+15+i*18,"%d",&i2);
				i3 = 0;
				sscanf(cbuf+21+i*18,"%d",&i3);
				f4 = 0.;
				sscanf(cbuf+27+i*18,"%f",&f4);

	     			if (i2==0 && i3== 0) break;

             			ztps[jc+cdpchange*maxp] = i2;
             			zbts[jc+cdpchange*maxp] = i3;
             			dvdz[jc+cdpchange*maxp] = f4;
             			jc = jc + 1;
	   		}
		}
     	}
/* last input cdp location for DVDZ card*/
	if(cardfound==1) {
    		cdps[cdpchange] = cdpnow;
    		nps[cdpchange] = jc;
        	*ncdps = cdpchange + 1;
	} else {
        	*ncdps = 0;
	}
	if(*ncdps>maxcdp) 
		err("number of cdps of DVDZ cards exceeds %d \n",maxcdp);
	free(cbuf);
}

