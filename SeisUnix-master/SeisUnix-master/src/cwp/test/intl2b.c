#include "cwp.h"

/* number of tabulated interpolation coefficients */
#define ICMAX 100
#define NTABLE (ICMAX+1)

void intl2b(int nxin, float dxin, float fxin,
	int nyin, float dyin, float fyin, unsigned char *zin,
	int nxout, float dxout, float fxout,
	int nyout, float dyout, float fyout, unsigned char *zout)
/*****************************************************************************
bilinear interpolation of a 2-D array of bytes
******************************************************************************
Input:
nxin		number of x samples input (fast dimension of zin)
dxin		x sampling interval input
fxin		first x sample input
nyin		number of y samples input (slow dimension of zin)
dyin		y sampling interval input
fyin		first y sample input
zin		array[nyin][nxin] of input samples (see notes)
nxout		number of x samples output (fast dimension of zout)
dxout		x sampling interval output
fxout		first x sample output
nyout		number of y samples output (slow dimension of zout)
dyout		y sampling interval output
fyout		first y sample output

Output:
zout		array[nyout][nxout] of output samples (see notes)
******************************************************************************
Notes:
The arrays zin and zout must passed as pointers to the first element of
a two-dimensional contiguous array of unsigned char values.

Constant extrapolation of zin is used to compute zout for
output x and y outside the range of input x and y.
 
For efficiency, this function builds a table of interpolation
coefficents pre-multiplied by byte values.  To keep the table
reasonably small, the interpolation does not distinguish
between x and y values that differ by less than dxin/ICMAX
and dyin/ICMAX, respectively, where ICMAX is a parameter
defined above.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 07/02/89
Modified:  Dave Hale, Colorado School of Mines, 05/30/90
	Changed function to interpolate unsigned char 
	instead of signed char, since many color tables and 
	image processing functions (e.g., PostScript) require 
	bytes with a maximum range of 0 to 255.
*****************************************************************************/
{         
	int ixout,iyout,ic,jc,ib,iyin,iyinl;
	float xout,yout,rxin,ryin,frac;
	int *kzin,*kic;
	unsigned char *temp1,*temp2,*temp;
	static unsigned char table[NTABLE][256];
	static int tabled=0;
	static void intl2bx(int, int*, int*, int,
		unsigned char[][256], unsigned char*, unsigned char*);
	static void intl2by(int, int, int, unsigned char[][256],
		unsigned char*, unsigned char*, unsigned char*);

	/* if not already built, build byte multiplication table */
	if (!tabled) {
		for (ic=0; ic<=ICMAX; ++ic) {
			frac = (float)(ic)/(float)ICMAX;
			for (ib=0; ib<256; ++ib)
				table[ic][ib] = frac*ib;
		}
		tabled = 1;
	}                                              

	/* get workspace */
	kzin = malloc(nxout*sizeof(int));
	kic = malloc(nxout*sizeof(int));
	temp1 = malloc(nxout*sizeof(unsigned char));
	temp2 = malloc(nxout*sizeof(unsigned char));

	/* pre-compute indices for fast 1-D interpolation along x axis */
	for (ixout=0,xout=fxout; ixout<nxout; ixout++,xout+=dxout) {
		rxin = (xout-fxin)/dxin;
		if (rxin<=0) {
		 	kzin[ixout] = 0;
		 	kic[ixout] = 0;
		} else if (rxin>=nxin-1) {
		 	kzin[ixout] = nxin-2;
		 	kic[ixout] = ICMAX*256;
		} else {
		 	kzin[ixout] = (int)rxin;
		 	frac = rxin-(int)rxin;
			ic = frac*ICMAX+0.5;
		 	kic[ixout] = ic*256;
		}
	}

	/* loop over output y */
	for (iyout=0,yout=fyout; iyout<nyout; iyout++,yout+=dyout) {

		/* compute index of input y, clipped to range of input y */
		ryin = MAX(0,MIN(nyin-1,(yout-fyin)/dyin));
		iyin = MAX(0,MIN(nyin-2,ryin));

		/* if output y is not between current input y */
		if (iyin!=iyinl || iyout==0) {

			/* if 2nd temporary vector is still useful */
			if (iyin==iyinl+1 && iyout!=0) {              

				/* swap 2nd and 1st temp; compute 2nd temp */
				temp = temp1;
				temp1 = temp2;
				temp2 = temp;
				intl2bx(nxout,kzin,kic,ICMAX,
					table,zin+(iyin+1)*nxin,temp2);

			/* else if 1st temporary vector is still useful */
			} else if (iyin==iyinl-1 && iyout!=0) {

				/* swap 1st and 2nd temp; compute 1st temp */
				temp = temp1;
				temp1 = temp2;
				temp2 = temp;
				intl2bx(nxout,kzin,kic,ICMAX,
					table,zin+iyin*nxin,temp1);

			/* else if neither 1st or 2nd temp is useful */
			} else {

				/* compute 1st and 2nd temporary vectors */
				intl2bx(nxout,kzin,kic,ICMAX,
					table,zin+iyin*nxin,temp1);
				intl2bx(nxout,kzin,kic,ICMAX,
					table,zin+(iyin+1)*nxin,temp2);
			}                 

			/* remember last index of input y */
			iyinl = iyin;
		}

		/* compute index of interpolation coefficient */
		frac = ryin-iyin;
		ic = frac*ICMAX+0.5;

		/* linearly interpolate output vector by table lookup */
		intl2by(nxout,ic,ICMAX,table,
  			temp1,temp2,zout+iyout*nxout);
	}                         

	/* free workspace before returning */
	free(kzin);
	free(kic);                     
	free(temp1);
	free(temp2);
}
   
/* for internal use by intl2b:  interpolate between input x values */
static void intl2bx(int nxout, int *kzin, int *kic, int icmax, 
	unsigned char table[][256], unsigned char *zin, unsigned char *zout)
{
	int ixout,jzin,jic;
  	unsigned char *tablel,*tableh;
	tablel = &table[0][0];
	tableh = &table[icmax][0];
	for (ixout=0; ixout<nxout; ixout++) {   
		jzin = kzin[ixout];
		jic = kic[ixout];
		zout[ixout] = tableh[zin[jzin]-jic]+tablel[zin[jzin+1]+jic];
	}
}   

/* for internal use by intl2b:  interpolate between input y values */
static void intl2by(int nxout, int ic, int icmax, unsigned char table[][256],
	unsigned char *temp1, unsigned char *temp2, unsigned char *zout)
{
	int ixout;
	unsigned char *tablel, *tableh;
	tablel = &table[ic][0];
	tableh = &table[icmax-ic][0];
	for (ixout=0; ixout<nxout; ixout++)
		zout[ixout] = tableh[temp1[ixout]]+tablel[temp2[ixout]];
}

#ifdef TEST
main()
{
	int nxin,nyin,nxout,nyout,ixout,iyout;
	float dxin,fxin,dyin,fyin,fxout,dxout,fyout,dyout;
	unsigned char zin[2][2],zout[3][3];
	
	zin[0][0] = 0;		zin[0][1] = 100;
	zin[1][0] = 100;	zin[1][1] = 200;
	nxin=2;  dxin=1.0;  fxin=0.0;
	nyin=2;  dyin=1.0;  fyin=0.0;
	nxout=3;  dxout=dxin*(nxin-1)/(nxout-1);  fxout=0.0;
	nyout=3;  dyout=dyin*(nyin-1)/(nyout-1);  fyout=0.0;
	intl2b(nxin,dxin,fxin,nyin,dyin,fyin,&zin[0][0],
		nxout,dxout,fxout,nyout,dyout,fyout,&zout[0][0]);
	for (iyout=0; iyout<nyout; iyout++)
		for (ixout=0; ixout<nxout; ixout++)
			printf("zout[%d][%d] = %d\n",
				iyout,ixout,zout[iyout][ixout]);
}
#endif
