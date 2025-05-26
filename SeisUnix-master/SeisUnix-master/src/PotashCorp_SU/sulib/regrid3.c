#include "par.h"

void regrid3(float ***gridi,int ni1,int ni2,int ni3,
             float ***grido,int no1,int no2,int no3) 
/*
 * Credits:
 *  	CWP: Zhaobo Meng, 1996, Colorado School of Mines
 * Rewritten to a function by Balazs Nemeth 2001
 */
{
	int ix1,ix2,ix3;
	int ix1plus,ix2plus,ix3plus; /*indices+1*/
	float alpha1,alpha2,alpha3;  /*weights for interpolations*/
	int io1,io2,io3;	     /*indices for output file*/
	float xi1,xi2,xi3;


	/*linear interpolation*/
	for (io1=0;io1<no1;io1++) {
		xi1=(float)io1*(float)ni1/(float)no1;
		ix1=MIN(ni1-1,(int)xi1);
		alpha1=xi1-ix1;
		ix1plus=MIN(ni1-1,ix1+1);
		for (io2=0;io2<no2;io2++) {
		        xi2=(float)io2*(float)ni2/(float)no2;
	                ix2=MIN(ni2-1,(int)xi2);
			alpha2=xi2-ix2;
			ix2plus=MIN(ni2-1,ix2+1);
			for (io3=0;io3<no3;io3++) {
	                        xi3=(float)io3*(float)ni3/(float)no3;
	                        ix3=MIN(ni3-1,(int)xi3);
				alpha3=xi3-ix3;
				ix3plus=MIN(ni3-1,ix3+1);
				grido[io3][io2][io1]=
			alpha1*(alpha2*
					(alpha3*gridi[ix3plus][ix2plus][ix1plus]+
					(1-alpha3)*gridi[ix3plus][ix2plus][ix1])
				+
				(1-alpha2)*
					(alpha3*gridi[ix3plus][ix2][ix1plus]+
					(1-alpha3)*gridi[ix3plus][ix2][ix1])
				)+
			(1-alpha1)*
				(alpha2*
					(alpha3*gridi[ix3][ix2plus][ix1plus]+
					(1-alpha3)*gridi[ix3][ix2plus][ix1])
				+
                                (1-alpha2)*
					(alpha3*gridi[ix3][ix2][ix1plus]+
					(1-alpha3)*gridi[ix3][ix2][ix1])	
				);
			}
		}
	}
}
