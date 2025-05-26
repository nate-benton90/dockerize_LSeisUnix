#include "comva.h"

/* read in velocity picks from input file pointer infp (VELO card dataset) */
/* VELO format:
1---5----11--15----21----27----33----39----45----51----57----63----69----75 
VELO    cdp        z1    v1    z2    v2    z3    v3    z4    v4    z5    v5 

where          cdp indicates cdp number of current panel 
               zi, i=1,2,..., are depth (or time)  
               vi, i=1,2,..., are velocity at zi 
*/
/* input:								*/
/*	infp			file pointer for VELO card dataset 	*/
/*	maxp			maximum number of z-v pairs per cdp 	*/
/*	maxcdp			maximum number of cdps of VELO cards 	*/
/* ouput:								*/
/*	cdps[maxcdp]		cdp numbers of VELO card input		*/	
/*	zpicks[maxcdp][maxp]	z position of picks			*/
/*	vpicks[maxcdp][maxp]	velocity of picks			*/
/*	ncdps			total number of cdps found in VELO cards*/
/*	nps[maxcdp]		number z-v pairs per cdp		*/
/*									*/	
/* author:	Zhiming Li		11/91	      			*/

void veloread(FILE *infp, int *cdps, float *zpicks, float *vpicks, int *ncdps,
      int *nps, int maxp, int maxcdp) {

     	int icmax, ic, cdpnow=0, cdppre=0, cdpchange=0, jc=0;
	int i1, i2, i3, i;
	char *cbuf;
	int cardfound=0;

	icmax = maxcdp * maxp;

	cbuf = (char *) malloc(81*sizeof(char));

	/* rewind infp */
	fseek(infp,0,0);


	for (ic=0;ic<icmax;ic++) {
		if(feof(infp) !=0) break;
		bzero(cbuf,81);
                fgets(cbuf,81,infp);

                if(strncmp(cbuf, "VELO",4)==0) {
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


			/* store read values in zpicks and vpicks arrays */
          		for(i=0;i<5;i++) {

				i2 = 0;
				sscanf(cbuf+15+i*12,"%d",&i2);

				i3 = 0;
				sscanf(cbuf+21+i*12,"%d",&i3);

				if(i3==0) break;

             			zpicks[jc+cdpchange*maxp] = i2;
             			vpicks[jc+cdpchange*maxp] = i3;
             			jc = jc + 1;
          		}
		}
	}
/* last input cdp location */
	if(cardfound==1) {
    		cdps[cdpchange] = cdpnow;
    		nps[cdpchange] = jc;
		*ncdps = cdpchange + 1;
	} else {
		*ncdps = 0;
	}
	if(*ncdps>maxcdp) 
		err("number of cdps of VELO cards exceeds %d \n",maxcdp);
	free(cbuf);
}
