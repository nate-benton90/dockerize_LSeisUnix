/* BSWAP subroutines */
#include "su.h"
#include "bswap.h"
#include "sucomp.h"


/* convert SU_compress header */
void cmhconvert(char *chin, char *chout) {
	int i;
	int sizecmh;

	sizecmh = 56;

	bcopy(chin,chout,sizecmh);

	bswap8(&chin[20],&chout[20]);

	for(i=0;i<6;i++) {
		bswap4(&chin[28+i*4],&chout[28+i*4]);
	}
}

/* convert binary header file */
void bhconvert(char *cin, char *cout) {
	int i, ibyte;

	ibyte = 0;
	for (i=0;i<3;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 3 * 4;
	for (i=0;i<24;i++) {
		bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 24 * 2;
	/* the rest using 4-byte swap */
	for (i=0;i<85;i++) {
		bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}
/* convert trace header */
void trhdconvert(char *cin,char *cout) {
	int i, ibyte;

	ibyte = 0;
	/* tracl - cdpt   bytes 1-28 */
	for(i=0;i<7;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 7 * 4;
	/* trid - duse   bytes 29-36 */
	for(i=0;i<4;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 4 * 2;
	/* offset - gwdep   bytes 37-68 */
	for(i=0;i<8;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 8 * 4;
	/* scalel - scalco   bytes 69-72 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* sx - gy           bytes 73-88 */
	for(i=0;i<4;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 4 * 4;
	/* counit - otrav    bytes 89-180 */
	for(i=0;i<46;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 46 * 2;
	/* d1 - unscale    bytes 181-204 */
	for(i=0;i<6;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 6 * 4;
	/* mark - mutb    bytes 205-208 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* dz - fz        bytes 209-216 */
	for(i=0;i<2;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 2 * 4;
	/* n2 -           bytes 217-220 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* the rest using 4-byte swap */ 
	for (i=0;i<5;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}

void traceconvert(char *cin,char *cout,int nt) {
	int i;
	for (i=0;i<nt;i++) {
	 	bswap4(&cin[i*4],&cout[i*4]);
	}
}

/* byte swapping of 8-bytes interger or float */
void bswap8(char *c8i, char *c8o)  {
	c8o[0] = c8i[7];	
	c8o[1] = c8i[6];	
	c8o[2] = c8i[5];	
	c8o[3] = c8i[4];	
	c8o[4] = c8i[3];	
	c8o[5] = c8i[2];	
	c8o[6] = c8i[1];	
	c8o[7] = c8i[0];	
}

/* byte swapping of 4-bytes interger or float */
void bswap4(char *c4i, char *c4o)  {
	c4o[0] = c4i[3];	
	c4o[1] = c4i[2];	
	c4o[2] = c4i[1];	
	c4o[3] = c4i[0];	
}
/* byte swapping of 2-bytes interger */
void bswap2(char *c2i, char *c2o)  {
	c2o[0] = c2i[1];	
	c2o[1] = c2i[0];	
}
