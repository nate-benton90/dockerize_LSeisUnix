#include "par.h"

/* read in velocity picks from input file pointer infp (VELF card dataset) */
/*VELF card format:
1---5---10---15---20---25---30---35---40---45---50---55---60---65---70---75
VELF        cdp        t1   v1   t2   v2   t3   v3   t4   v4   t5   v5

	where 	ti=time 
		vi=stacking velocity
		i=1,2,...
*/	
/* input:								*/
/*	infp			file pointer for VELF card dataset 	*/
/*	maxp			maximum number of t-v pairs per cdp 	*/
/*	maxcdp			maximum number of cdps of VELF cards 	*/
/* ouput:								*/
/*	cdps[maxcdp]		cdp numbers of VELF card input		*/	
/*	tpicks[maxcdp][maxp]	t position of picks			*/
/*	vpicks[maxcdp][maxp]	velocity of picks			*/
/*	ncdps			total number of cdps found in VELF cards*/
/*	nps[maxcdp]		number t-v pairs per cdp		*/
/*									*/	
/* author:	Zhiming Li		6/92	      			*/

void velfread(FILE *infp, int *cdps, float *tpicks, float *vpicks, int *ncdps,
      int *nps, int maxp, int maxcdp) {

     	int icmax, ic, cdpnow=0, cdppre=0, cdpchange=0, jc=0;
	int i1, i2, i3, i;
	char *cbuf, cdp[10], velo[5], time[5];
	int cardfound=0;

	icmax = maxcdp * maxp;

	cbuf = (char *) malloc(81*sizeof(char));

	/* rewind infp */
	fseek(infp,0,0);


	for (ic=0;ic<icmax;ic++) {
		if(feof(infp) !=0) break;
		bzero(cbuf,81);
                fgets(cbuf,81,infp);

                if(strncmp(cbuf, "VELF",4)==0) {
			cardfound = 1;

			strncpy(cdp,&cbuf[5],10);
                	cdpnow = atoi(cdp);

			if (cdppre == 0 ) cdppre = cdpnow ;
			/* if cdp changes */
          		if (cdpnow != cdppre && cdpnow !=0 && cdppre !=0 ) {
             			nps[cdpchange] = jc;
				cdps[cdpchange] = cdppre;
				cdpchange += 1;
				jc = 0;
             			cdppre = cdpnow;
          		}


			/* store read values in tpicks and vpicks arrays */
          		for(i=0;i<5;i++) {

				strncpy(time,&cbuf[20+i*10],5);
                        	i2 = atoi(time);
				strncpy(velo,&cbuf[25+i*10],5);
                        	i3 = atoi(velo);

				if(i3==0) break;

             			tpicks[jc+cdpchange*maxp] = i2;
             			vpicks[jc+cdpchange*maxp] = i3;
             			jc = jc + 1;
          		}
		}
	}
/* last input cdp location */
	if(cardfound==1) {
    		cdps[cdpchange] = cdppre;
    		nps[cdpchange] = jc;
		*ncdps = cdpchange + 1;
	} else {
		*ncdps = 0;
	}
	if(*ncdps>maxcdp) 
		err("number of cdps of VELF cards exceeds %d \n",maxcdp);
	free(cbuf);
}
