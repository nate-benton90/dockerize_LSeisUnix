#include "comva.h"

/* read in DIPS cards */
/* DIPS format:

1---5---10---15----21----27----33----39----45----51----57----63----69----75 
DIPS   cdp        zt1  dipl  dipr   zt2  dipl  dipr   zt3  dipl  dipr    

where 		zt	---	depth of dip limits
		dipl	---	dip to the left at zt
		dipr	---	dip to the right at zt

Input:
	infp	---	input file pointer where DIPS cards are
	maxp	---	max number of triplets per cdp 
	maxcdp	---	max number of cdps where DIPS are specified
Output:
	cdps	---	cdp numbers of DIPS cards (array(maxcdp))
	zps	---	depths of the tripletes (array(maxp,maxcdp))
	dipl	---	dip to the left at zps (array(maxp,maxcdp))
	dipr	---	dip to the right at zps (array(maxp,maxcdp))
	ncdps	---	number of cdps where DIPS cards are specified
	nps	---	number of tripletes at cdps (array(maxcdp))

Auhtor: Zhiming Li		9/2/92	      
*/

void dipsread(FILE *infp, int *cdps, float *zps, float *dipl, float *dipr,
int *ncdps, int *nps, int maxp, int maxcdp) {

	int icmax, ic, cdpnow=0, cdppre=0, cdpchange=0, jc=0;
        int i1, i2, i;
        char *cbuf;
	float f3, f4;
	int cardfound=0;

   	cbuf = (char *) malloc(81*sizeof(char));

	/* rewind infp */
   	fseek(infp,0,0);

	/* read in DIPS cards */
	icmax = maxcdp * maxp;

   	for (ic=0;ic<icmax;ic++) {
       		if (feof(infp) !=0 ) break;
     		bzero(cbuf,81);
                fgets(cbuf,81,infp);

        	if(strncmp(cbuf, "DIPS", 4)==0) {
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

	  		/* store read values in depth, dipl and dipr arrays */
          		for(i=0;i<3;i++) {

				i2 = 0;
				sscanf(cbuf+15+i*18,"%d",&i2);
				f3 = 0;
				sscanf(cbuf+21+i*18,"%f",&f3);
				f4 = 0.;
				sscanf(cbuf+27+i*18,"%f",&f4);

	     			if (i2==0 && f3==0. && f4==0. && jc>0 ) break;

             			zps[jc+cdpchange*maxp] = i2;
             			dipl[jc+cdpchange*maxp] = f3;
             			dipr[jc+cdpchange*maxp] = f4;
             			jc = jc + 1;
	   		}
		}
     	}


/* last input cdp location for DIPS card */
	if(cardfound==1) {
    		cdps[cdpchange] = cdpnow;
    		nps[cdpchange] = jc;
        	*ncdps = cdpchange + 1;
	} else {
        	*ncdps = 0;
	}
	if(*ncdps>maxcdp) 
		err("number of cdps of DIPS cards exceeds %d \n",maxcdp);
	free(cbuf);
}

